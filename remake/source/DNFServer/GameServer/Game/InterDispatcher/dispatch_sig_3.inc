int Inter_CreateCharac::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2A-PC 本轮残留 N（已修 2 处）] 已修：logPath 255B 清零改 `= {0}`（ORIG rep stos 形）、
    //   4 个跨 CMyFileLog 调用量加 register（characNo/serverGroup/hangame/accStr）。
    //   残留：ORIG 序言 `push %edi`（多一个 callee-saved）与帧 0x2ec vs 本方 0x300；后者属
    //   父泳道统一裁决的「我方多 4B 槽」共享机制，不 hack。其余为 `mov %eax,%edi/%esi`
    //   免槽 vs `mov %eax,<S>` 型位点，尚未逐个定位，留 N。
    // ORIG 0x084bee06 报告 docs/class_func_reports/Inter_CreateCharac/dispatch_sig.md
    // 两段全读。SIG_CREATE_CHARAC 布局见 SigTypes.h（m_slot/m_field23/m_eventCharacFlag/
    // m_resultCode/m_characNo 全具名）。
    if (user->get_state() == 0)
        return 0x6c0;
    if (!user->unlockCreateCharac())
        return 0x6c4;

    SIG_CREATE_CHARAC* sig = reinterpret_cast<SIG_CREATE_CHARAC*>(data);
    PacketGuard packet;                    // ORIG 局部包对象全程复用
    packet.put_header(1, 5);

    if (sig->m_resultCode != 0)
    {   // DB 失败应答：put_byte(0) + 错误映射字节（ORIG 每分支各自 put_byte，p167 6 个调用点）
        packet.put_byte(0);
        if (sig->m_resultCode == 1)
            packet.put_byte(0x12);
        else if (sig->m_resultCode == 4)
            packet.put_byte(0x14);
        else if (sig->m_resultCode == 5)
            packet.put_byte(0x5f);
        else if (sig->m_resultCode == 3)
            packet.put_byte(0x18);
        else
            packet.put_byte(2);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    if ((char)sig->m_field23 > '\0')
    {   // 跳跳角色活动日志（ORIG localtime/sprintf/inet_ntoa/CMyFileLog 链）
        const long curSec = GlobalData::s_systemTime_.getCurSec();
        struct tm localTm;
        struct tm* pTm = localtime_r(&curSec, &localTm);
        char timeBuf[256];
        sprintf(timeBuf, "%02d:%02d:%02d",
                pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
        char ipStr[16] = {
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
        in_addr addr;
        addr.s_addr = static_cast<unsigned int>(user->get_public_ip_address());
        strcpy(ipStr, inet_ntoa(addr));
        // [G2A-PC] ORIG 段 `lea <S>,%edx; mov $0x0,%eax; mov $0x3f,%ecx; mov %edx,%edi;
        //   rep stos %eax,%es:(%edi); mov %ax,(%edx); add $0x2,%edx; mov %al,(%edx)` = gcc 对
        //   `char logPath[255] = {0};` 的定型展开（63 dword + 2B + 1B = 255B 全零）。
        //   手写 for 清零会产 `movl $0x0,<S>; jmp; movb $0x0,-0x1d0(%ebp,%eax,1); addl;
        //   cmpl $0xfe; setle; test; jne` 循环形（本 diff N 侧 9 条）⇒ 原码是初始化式。
        char logPath[255] = {0};
        sprintf(logPath, "./log/%s/JumpingCharacter",
                G_CEnvironment()->get_file_name());

        // [G2A-PC P231/P52 callee-saved 免槽] ORIG 段把这四个跨 CMyFileLog 构造/调用的
        //   临时量直落 callee-saved：084d…[157] `mov %eax,%edi`（characNo）、[161-166]
        //   `test %al,%al; je; mov $0x1,%ebx; jmp; mov $0x0,%ebx`（hangame→%ebx）、
        //   [172] `mov %eax,%esi`（accStr）；本方同位置为 `mov %eax,<S>` 落槽形
        //   （本 diff N 侧）⇒ 加 register 存储类（纯源级，ORIG 亦同形）。
        register const unsigned int characNo = sig->m_characNo;
        register const int serverGroup = G_CEnvironment()->get_server_group();
        register const bool hangame = user->isHangameUser() ? true : false;
        register char* accStr =
            NumberToString(static_cast<unsigned int>(user->get_acc_id()), 0);
        CMyFileLog log(
            "virtual int Inter_CreateCharac::dispatch_sig(CUser*, char*, int)",
            0x703);
        log(logPath, ",%s,%d,%d,%d,%s,%s", accStr, hangame, serverGroup,
            static_cast<int>(characNo), ipStr, timeBuf);

        user->setUserEventCharacterFlag(sig->m_eventCharacFlag);
    }

    user->CreateCharac(sig);
    HistoryLog::WriteCreateCh(
        *reinterpret_cast<FILE**>(&user->m_field796f8),   // +0x796f8 文件句柄槽
        sig->m_name, static_cast<int>((char)sig->m_slot));
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(2);
    user->make_basic_info(reinterpret_cast<char*>(&packet), 2);
    packet.finalize(true);
    user->Send(packet);
    user->UpdateCharacView();
    process_event(user, sig);

    // 跳跳角色奖励判定：服务器组==10 且 2011-01-01 活动期内且 sig+0x534d 非 0
    // （ORIG 0x84bf255 cmp $0x478e7050;ja → x<=0x478e7050 含边界）
    // [b28d-solo-InterDispatcher] P226 语句表达式：ORIG 0x84bf255..0x84bf268 短路物化
    // 岛（mov$1/mov$0/test al）零槽直测；具名 bool 初值+置位形 4.4 恒落槽（+3 条）。
    // ORIG 0x84bf239 内联字段读 `mov 0x378(%eax),%eax`（0x198+0x1e0）⇒ 字段访问，非 getter 调用。
    if (({ G_CEnvironment()->m_serverEnvir.m_serverGroup == 10 &&
          (unsigned int)GlobalData::s_systemTime_.getCurSec() <= 0x478e7050u; }) &&
        *reinterpret_cast<unsigned char*>(
            &sig->m_pad534d[0]) != 0)   // +0x534d
    {   // 活动创建礼（按职业表 0x1c70..0x1c74 取道具）
        Inven_Item item;
        int giftIdx = 0;
        switch ((char)sig->m_slot)
        {
        case 0: giftIdx = 0x1c70; break;
        case 1: giftIdx = 0x1c71; break;
        case 2: giftIdx = 0x1c72; break;
        case 3: giftIdx = 0x1c73; break;
        case 4: giftIdx = 0x1c74; break;
        }
        item.m_field1 = 2;
        item.m_addInfo2 = 1;
        item.m_fieldb = 0;
        item.ResetItemAttr();
        if (giftIdx != 0 && G_CDataManager()->find_item(giftIdx) != 0)
        {
            const char* titleRaw = g_scriptStringManager_.findString(
                4, "game_server_msg_10", reinterpret_cast<bool*>(0));
            const int titleLen = static_cast<int>(strlen(titleRaw));
            const char* body = g_scriptStringManager_.findString(
                4, "game_server_msg_10", reinterpret_cast<bool*>(0));
            const char* sender = g_scriptStringManager_.findString(
                4, "game_server_msg_09", reinterpret_cast<bool*>(0));
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                sender, item, 0u, sig->m_characNo, body, titleLen,
                0u, user->GetServerGroup(), false, false);
        }
    }

    CEventBase* ryosika = GlobalData::s_event_manager->GetRepeatEvent(0x74);
    if (ryosika != 0)
        reinterpret_cast<CAradRyosikaEvent*>(ryosika)->DoEvent(
            user, static_cast<int>(sig->m_characNo), 1,
            static_cast<int>((char)sig->m_slot));
    return 0;
