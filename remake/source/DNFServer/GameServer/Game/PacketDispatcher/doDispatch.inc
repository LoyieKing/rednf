bool PacketDispatcher::doDispatch(CUser* user, ENUM_PACKETCLASS packetClass,
                                  unsigned short cmd, char* data, int len,
                                  ENUM_PACKETCLASS packetClass2, unsigned short flag)
{
    // b28d-solo-PacketDispatcher 判词同上（N18a / P197④ / N82，callsite CLEAR）。
    (void)packetClass2;
    // b28d-solo-S161 原判词（已被 S177 形替代，保留证据）：P232 前置 `char framePad[4]`
    // 撑槽使 head 等槽对齐（ndiff 342→308；[8]/double 反劣 380）。**S177 复核后删除
    // framePad，改由末尾 `const int limit = 0x7d0;` 一槽两用**（复现 ORIG 0x08594dd0 的
    // `movl $0x7d0,-0xc(%ebp)` 死写入 + 浅槽整体深 4 对齐 ORIG dispatcher@-0x20）；帧仍
    // 0x17390（framePad 与 limit 并存会使帧 16 对齐进位到 0x173a0，已实测回退）。
    log_recv_packet(user, packetClass, cmd, data, len);

    typedef int (*CheckFn)(void*, unsigned int, unsigned short);  // vtbl+0x30
    // b28d-solo-S161：与 dispatch 同法（register + 单层解引用）复现 ORIG 0x0859497d
    // `mov (%eax),%esi` 单段直落 + cmd 落 %ebx 的角色分配。
    register CheckFn fn = *reinterpret_cast<CheckFn*>(
        reinterpret_cast<char*>(
            *reinterpret_cast<void**>(GlobalData::s_antibotChecker_)) + 0x30);
    if (fn(GlobalData::s_antibotChecker_, (unsigned int)user->getAntibotKey(), cmd) != 0)
    {
        if (flag != 0x25c)
        {
            LogManager::logFormat(
                1, "PacketDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0xa06, "m_id:%s, cmd:%d", NumberToString(user->get_acc_id(), 0),
                (int)cmd);
        }
    }

    IDispatcher* dispatcher = get_dispatcher((int)cmd);
    if (dispatcher == 0)
        return 0;

    server_load_monitoring();
    user->backupSeed();

    if (0xd < len && flag == 0x25c)
    {
        unsigned int serverCrc = Encryption::crc32N(data + 0xb, len - 0xb);
        // [b28d-solo-S316] ORIG 0x08594a9c 首次取址 `lea -0x31;add $7`（数组形），
        // 0x08594ab1/0x08594ada 后两次读值 `mov -0x2a(%ebp),%edx/%ecx`（disp 直读）——
        // 4.4 对 `head+7` 指针形恒重发 lea+add ⇒ 用具名 pack(1) 视图（13B/align1，
        // 槽位 -0x31 与帧零变化）成员访问折叠 disp；首次仍用指针算术保 lea+add。
        struct __attribute__((packed)) DispatchPacketHead
        {
            char m_prefix[7];
            unsigned int m_checksum;
            char m_tail[2];
        };
        DispatchPacketHead head;
        memcpy(&head, data, 0xd);
        // 线格式视图：协议头 +7 处为校验和 dword（非对齐）。ORIG 0x8594a?? 实证
        // 两次 `lea head; add $7` 直接内联（P183 去缓存）：无 clientCrc 局部槽。
        _NS_PI_2ND_MakeChecksumTo1Byte2(serverCrc,
                                        *(unsigned int*)((char*)&head + 7));
        if (head.m_checksum != serverCrc)
        {
            LogManager::logFormat(
                1, "PacketDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0xa37, "Check Sum Error: server(%d), client(%d), mid(%s)",
                serverCrc, head.m_checksum,
                NumberToString(user->get_acc_id(), 0));
            user->DisConnSig((DISCONN_SIG)0xd, true, 0);
            return 0;
        }
    }

    PacketBuf packet;
    packet.bind_packet(data, len);
    user->setLastPacketID((int)cmd);

    // [b28d-solo-S400] DenyPacket 段（S339 判词推翻）：ORIG 0x08594b92..0x08594bb6 先把
    //   `(A||B)` 物化为寄存器 bool（`mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`，
    //   无槽往返）再 `call DenyPacket`。S339 穷举五形（P226 只包左项 / 整条件语句表达式 /
    //   三目 / `!!` / while）均不可；**P225 推广的「拆嵌套 if」形是钥匙**：把 `(A||B) && X`
    //   改写成 `if (A||B) { if (X) return 1; }` ⇒ 4.4 复现 ORIG 的物化岛
    //   （探针 /tmp/pd_p4.cpp n1 与 ORIG 0x8594b9a..0x8594bd5 逐条同构；n2..n9 仍直落）。
    if (G_GameWorld()->GetChannelType() == 7 ||
        G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        if (DenyPacket((int)cmd))
            return 1;
    }

    // [b28d-solo-PacketDispatcher S339] P171 照抄：ORIG 0x08594bd5 段在 `cmpw $0x25b` 前**无**
    //   `movl $0x0,-0x1c(%ebp)`（tick0 只在 cmd==0x25b 分支内赋值，非 0x25b 路径不读）
    //   ⇒ 原码是未初始化声明；我方 `= 0` 多一条死 store。
    unsigned int tick0;
    if (cmd == 0x25b)
        tick0 = GlobalData::s_systemTime_.getCurTickCount();

    // [b28d-solo-PacketDispatcher S339] P198①预清必写：ORIG 0x08594bef `movl $0x0,-0x18(%ebp)`
    //   （tick0 分支之后、调用之前）⇒ 原码是「声明预清 + 独立赋值」两句式。
    int errResult = 0;
    errResult = m_chinaError.doErrorDispatcher(user, cmd);
    // [b28d-solo-PacketDispatcher S339] P225 钥匙：ORIG 0x08594c8d 段为
    //   `cmpl $0x0,-0x18(%ebp); setne %al; test %al,%al; je`（4.1 槽比零 bool 物化），
    //   4.4 对 `if (v != 0)` 恒直跳 `je`（探针 /tmp/pd_probe_while.cpp f_if）。
    //   `while (v != 0) { …; return 0; }` 形精确复现 setne/test 两段（同探针 f_while），
    //   循环体全路径 return ⇒ 语义与 if 完全等价。
    while (errResult != 0)
    {
        // ORIG 0x08594c9a 段：`mov -0x18(%ebp),%eax; cmp $0x1,%eax; je; cmp $0x2,%eax;
        // jne` 寄存器决策链（P251/P286：无内存直比）⇒ 原码是 switch（值在寄存器）。
        // [b28d-solo-PacketDispatcher S339] P46 叶块序：ORIG 物理叶块 case2(ERROR_TING, 0x8594c40) 在
        //   case1(ERROR_NOT_TING, 0x8594cbb) 之前，且 case2 为 `jne` fall-through
        //   （0x8594c3a jne 出口）⇒ 原码 case 书写序为降序（case 2 先）；升序书写时
        //   4.4 恒产 `je case2; jmp 出口`（多 1 条 jmp）。
        switch (errResult)
        {
        case 2:
            LogManager::logFormat(
                1, "PacketDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0xa5b, "[doErrorDispatcher] ERROR_TING - code : %d, m_id :%s",
                (int)cmd, NumberToString(user->get_acc_id(), 0));
            user->DisConnSig((DISCONN_SIG)0xa, true, 0);
            break;
        case 1:
            LogManager::logFormat(
                1, "PacketDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0xa60, "[doErrorDispatcher] ERROR_NOT_TING - code : %d, m_id :%s",
                (int)cmd, NumberToString(user->get_acc_id(), 0));
            break;
        }
        return 0;
    }

    int handlerResult = dispatcher->dispatch_template(user, packet);
    // [b28d-solo-PacketDispatcher S339] 同上：ORIG 0x08594ce8 段 `cmpl $0x0,-0x1c(%ebp); setne %al;
    //   test %al,%al; je` 槽比零物化 ⇒ 用 while 形复现（P225）。
    while (handlerResult != 0)
    {
        user->DisConnSig((DISCONN_SIG)0xa, true, handlerResult);
        return 0;
    }

    if (cmd == 0x25b)
    {
        GlobalData::s_systemTime_.update();
        unsigned int elapsed = GlobalData::s_systemTime_.getCurTickCount() - tick0;
        // ORIG 0x08594dd0 `movl $0x7d0,-0xc(%ebp)` + `cmpl $0x7d0,-0x10(%ebp)`：
        // 原码是具名 const 局部（4.1 发射初始化 store、读取处折叠为立即数比较，P171 死写入）。
        // 该局部同时补齐我方浅槽整体浅 4（ORIG dispatcher@-0x20 vs 我方 -0x1c）的帧差。
        const int limit = 0x7d0;
        if (limit < elapsed)
        {
            LogManager::logFormat(
                1, "PacketDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0xa7a, "Delay notice, OnRecvBufferFromClient dispatch time : %u",
                elapsed);
        }
    }
    if (cmd != 0x25b)
    {
        _NS_PI_Check_CMD(packetClass, cmd, packet, user);
    }
    return 1;
