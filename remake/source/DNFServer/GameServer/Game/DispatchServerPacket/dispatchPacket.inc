// §6.2 巨件核销 [b28d-solo-CAuction]：verdict DIFF（N82 全帧槽号错位，不可复现）。
// 三件套：①gap ORIG 6348 / OUR 6346（+2）；②P167 调用点差集 1245/1245 [0缺0多]
// （含 0x9de put idx5、0xfa5/0x1b63/0x1b64/0x1b65 DB_Q(idx2)、0xb54 双 DB
// makeRequest、0x3ff/0x2712/0x2713 cMyTrace、0x27f9/0x27fa make_internal_stream_jpn）；
// ③cmp5 按 switch 决策树配对 188 case，实质差异 0（余 6 处伪差：printf@plt 后缀、
// __PRETTY_FUNCTION__ 本地 r 符号 vs .rodata 重定位，链接后同址）。
// 本波修正：
// ① 0xb65 缺 fall-through 真语义 bug：ORIG 0847a316 put 后无 jmp，直落 0x2712 的
//    cMyTrace 块（Ghidra C LAB_0847a31b 双入口实证）→ 去 break 并并入 0x2712 块；
// ② 0x1b6a 具名局部槽 → switch(视图字段) + RevengeView（P251/P0）；
// ③ 67 处字段读取形态：裸偏移 *(int*)(d+N)（4.4 恒 add+load，P30/N102）与冗余
//    (int) cast（4.4 两段 eax→ebx 中转，P265/P211 档③）→ IntView0a..29/UIntView
//    具名视图；14 处 ORIG 两段形用 unsigned 视图 + (int) cast 复现；
// ④ 0xb54 跳转极性反演（jne/je 落点，P59/P222）+ 字节视图 UCharView0a。
// [BigAuction 复核修正] 旧判"N82 全帧槽号错位不可复现"系误诊：case 数据槽 174 个
// 两侧本就 1:1 对齐，真因是**本函数 case 块声明序与 ORIG 4.1 body 布局序不一致**
// （ORIG body 序非数值序：0x3e8,0x3ea,0x3f9,0x3ee,0x27e2,0x3f8,0x5e0,...）。
// 本波按 ORIG 决策树 je→body 入口地址序重排 186 个 case 块后 slot↔case 175/175
// 全中（验证脚本 /tmp/disp_verify.py），call 目标多重集 1245/1245 0缺0多、
// cmp $imm 多重集 253/253 0缺0多（Main 要求的方法证明）。
// 本波再修正（N82 判词作废）：
// ① guard/trace 帧底槽全部对上 ORIG（-0x310/-0x308/-0x2f8/-0x2e8）：翻法 = 把
//    0xdbf/0xdcc 两处 named cMyTrace 局部改单表达式临时链形（P227 同族）——4.4
//    临时槽与 named 槽分组排布，temporize 后 guard 自动落最低槽；
// ② 池选择中段 4.1 双 cmp 形可翻：else-if 条件写全 `payloadLen >= 0x1000 &&
//    payloadLen <= 0x3fff`（ORIG jbe 死分支 = `>=` 假路，语义不变），4.4 产出
//    同款 cmpw $0xfff; jbe 冗余对；
// ③ 残余仅链接伪差：printf@plt 后缀、.o 尾部对齐 nop（relink 即消）。
// [BigDispatchPacket 2026-09-09 N132] AE 封顶定案：残余 = 纯数据符号地址差，非形态差。
// 取证（fresh /tmp/tu_CAuction.o，check_tu_routed 2026-09-09）：
//   ① 指令数/帧全等：归一化 ours=6343 / orig=6343、0 差异行（fn_diff_tu）；两侧
//      prologue 同为 push edi/esi/ebx + sub $0x32c，槽号/助记符逐条一致；
//   ② p167 调用点多重集 1245/1245 [0缺0多]：MISSING 仅 cMyTraceC2/CStreamGuardD2，
//      EXTRA 为同址 C1/D1 别名（P93 同址别名伪差，豁免）；
//   ③ 链接后 strict（不归一 >=0x40000000 大地址）残差 196 行，100% 为数据符号地址：
//      · GlobalData::s_msgq_mgr 0x940bd68→0x910b54c ×175
//      · s_stream_pool 0x940bd6c→0x910b550 / s_big_stream_pool 0x940bda4→0x910b584 /
//        s_small_stream_pool 0x940bda8→0x910b588 各 ×1
//      · s_villageMonsterMgr 0x941f77c→0x9160ff8、s_revengeDungeonMgr 0x941f780→0x9160ffc ×3、
//        s_hades_proxy 0x940be34→0x910b5b8（.bss 内偏移 ORIG 0x166a8 vs 我方 0x24c8c）
//      · .rodata 本 TU 串 8 处："DispatchServerPacket.cpp" 0x8c6ff60→0x8b37ba0 ×4、
//        __PRETTY_FUNCTION__ 0x8c70f00→0x8b37fa0 ×4、"[GUILD MARK CHANGE]…" 0x8c6ff7c→0x8b37bbc、
//        "[Packet_Request_Revenge]…" 0x8c6ffd0→0x8b37c10、"TOD: bug random select" 0x8c70023→0x8b37c63、
//        "TOD: bug reload" 0x8c7003a→0x8b37c7a、"GEO IP TABLE RELOAD FAIL" 0x8c7004a→0x8b37c8a 各 ×1
// 根因（本 TU 无解）：ORIG 该函数所属 TU = DispatchServerPacket.cpp，其 .rodata 块在链接序中的
// 位置与我方 CAuction.cpp 不同（.rodata 基址 0x8ae5840 vs 0x8a3e000；块内
// "DispatchServerPacket.cpp"→__PRETTY_FUNCTION__ 间距 ORIG 0xba0 vs 我方 0x400）；7 个 GlobalData
// 静态成员定义在 GlobalData.cpp（非本 TU）。⇒ 属全库段布局族（N132/N-LAYOUT），AE→IDENTICAL
// 需 Main 级链接布局对齐；禁搬数据定义位置（会改构造序）。已试形：无（地址差不可由源形翻转，
// P321-T5 类型/形状旋钮不适用）。
bool CDispatchServerPacket::dispatchPacket(char* data)
{
    // ORIG 0x8473660：上游服务器包按首 short(sig) 重写头并转投 DISPATCH_Q。
    unsigned short payloadLen = ((PacketHeader*)data)->packetSize;
    payloadLen += 8;
    CStreamGuard guard(true);
    if (payloadLen < 0x1000)
    {
        guard.Set(GlobalData::s_small_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x36));
    }
    else if (payloadLen >= 0x1000 && payloadLen <= 0x3fff)  // 4.1 双 cmp 形（ORIG 重验边界）
    {
        guard.Set(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x3a));
    }
    else
    {
        guard.Set(GlobalData::s_big_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x3e));
    }
    switch (((PacketHeader*)data)->packetId)
    {
        case 0x3e8:
        {
        char* d = data;
        guard.operator*()->operator<<(65);
        guard.operator*()->operator<<((((Packet_Monitor_UDP_Login*)d)->m_channelId));
        guard->put_binary(d, 51);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

    case 0x3ea:
    {
        char* d = data;
        guard.operator*()->operator<<(66);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 11);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3f9:
        {
        char* d = data;
        guard.operator*()->operator<<(301);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3ee:
        {
        char* d = data;
        guard.operator*()->operator<<(67);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 14);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27e2:
        {
        char* d = data;
        guard.operator*()->operator<<(759);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3f8:
        {
        char* d = data;
        guard.operator*()->operator<<(289);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 11);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x5e0:
        {
        char* d = data;
        guard.operator*()->operator<<(171);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 415);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x5dc:
        {
        char* d = data;
        guard.operator*()->operator<<(172);
        guard.operator*()->operator<<((int)(((Packet_Register_To_BlackList*)d)->m_dbid));
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x5dd:
        {
        char* d = data;
        guard.operator*()->operator<<(173);
        guard.operator*()->operator<<((int)(((Packet_Delete_To_BlackList*)d)->m_dbid));
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3ef:
        {
        // 0x3ef：权威头 Packet_Monitor_Char_Info::m_channel 为 uchar，ORIG 此处 movl 整字读（4B），字段宽度不符，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(68);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 52);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x5e2:
        {
        char* d = data;
        guard.operator*()->operator<<(174);
        guard.operator*()->operator<<((int)(((BlackListNoticeView*)d)->m_idByChannel));
        guard->put_binary(d, 59);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3f2:
        {
        char* d = data;
        guard.operator*()->operator<<(70);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x44c:
        {
        char* d = data;
        guard.operator*()->operator<<(71);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 14);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x44d:
        {
        char* d = data;
        guard.operator*()->operator<<(74);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 34);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x40c:
        {
        char* d = data;
        guard.operator*()->operator<<(98);
        guard.operator*()->operator<<(((IntView29*)d)->m_v);
        guard->put_binary(d, 45);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x40b:
        {
        char* d = data;
        guard.operator*()->operator<<(97);
        guard.operator*()->operator<<((int)(((SetGuildKeyView*)d)->m_channel));
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x40a:
        {
        char* d = data;
        guard.operator*()->operator<<(96);
        guard.operator*()->operator<<((int)(((NoticeGuildInfoView*)d)->m_channel));
        guard->put_binary(d, 313);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3fd:
        {
        // 0x3fd：包类 Packet_Monitor_Notice_Guild_Enter 首成员为 ST_Notice_Guild_Enter 结构（Guild 域头，
        // 本 TU 不可达且无标量首字段证据），布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(86);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 90);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3fe:
        {
        char* d = data;
        guard.operator*()->operator<<(87);
        guard.operator*()->operator<<((((IntView0a*)d)->m_v));
        guard->put_binary(d, 85);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3ff:
        {
        char* d = data;
        guard.operator*()->operator<<(88);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 22);
        // ORIG put_binary 后、MsgQueueMgr::put 前 cMyTrace(0x1a4,0) 打公会徽章变更日志
        // （channel=*(int*)(+10), character=*(int*)(+0xe), guildkey=*(int*)(+0x12)）。
        // ORIG 单表达式链：日志实参先于 cMyTrace 构造求值（edi/esi/ebx 预装载）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1a4, 0)(
            "[GUILD MARK CHANGE] Recv from guild server. (channel:%d, character:%u, "
            "guildkey:%u)",
            (int)(((GuildMarkChangeView*)d)->m_channel), ((IntView0e*)d)->m_v, ((IntView12*)d)->m_v);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3fc:
        {
        char* d = data;
        guard.operator*()->operator<<(89);
        guard.operator*()->operator<<((((GuildKeyIntView*)d)->m_guildKey));
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x400:
        {
        char* d = data;
        guard.operator*()->operator<<(90);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x401:
        {
        char* d = data;
        guard.operator*()->operator<<(91);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x423:
        {
        char* d = data;
        guard.operator*()->operator<<(198);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x424:
        {
        char* d = data;
        guard.operator*()->operator<<(199);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x428:
        {
        char* d = data;
        guard.operator*()->operator<<(200);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x406:
        {
        // 0x406：重建源码无该类，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(92);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 9023);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x407:
        {
        char* d = data;
        guard.operator*()->operator<<(93);
        guard.operator*()->operator<<((int)(((GuildMemberLoginOutView*)d)->m_channel));
        guard->put_binary(d, 50);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3f3:
        {
        char* d = data;
        guard.operator*()->operator<<(94);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Other_Channel_Chat_ToUser*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x40d:
        {
        char* d = data;
        guard.operator*()->operator<<(95);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Call_Guild_Info*)d)->m_uid));
        guard->put_binary(d, 399);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x42d:
        {
        char* d = data;
        guard.operator*()->operator<<(216);
        guard.operator*()->operator<<((int)(((Packet_Web_Notify_Message_To_Guild*)d)->m_guildKey));
        guard->put_binary(d, 119);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x42f:
        {
        // 0x42f：重建源码无该类，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(100);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x430:
        {
        char* d = data;
        guard.operator*()->operator<<(101);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Set_Sub_Guild_Master*)d)->m_guildKey));
        guard->put_binary(d, 58);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x431:
        {
        char* d = data;
        guard.operator*()->operator<<(102);
        guard.operator*()->operator<<((int)((UIntView0a*)d)->m_v);
        guard->put_binary(d, 44);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x42c:
        {
        char* d = data;
        guard.operator*()->operator<<(215);
        guard.operator*()->operator<<((int)(((GuildMemberGradeView*)d)->m_channel));
        guard->put_binary(d, 19);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4b6:
        {
        char* d = data;
        guard.operator*()->operator<<(108);
        guard.operator*()->operator<<(((IntView0b*)d)->m_v);
        guard->put_binary(d, 56);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4b7:
        {
        char* d = data;
        guard.operator*()->operator<<(109);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4b8:
        {
        char* d = data;
        guard.operator*()->operator<<(110);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 50);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4ba:
        {
        char* d = data;
        guard.operator*()->operator<<(111);
        guard.operator*()->operator<<(((IntView0d*)d)->m_v);
        guard->put_binary(d, 57);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4b9:
        {
        char* d = data;
        guard.operator*()->operator<<(112);
        guard.operator*()->operator<<(((IntView0d*)d)->m_v);
        guard->put_binary(d, 57);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4bb:
        {
        char* d = data;
        guard.operator*()->operator<<(113);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4bc:
        {
        char* d = data;
        guard.operator*()->operator<<(114);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4bd:
        {
        char* d = data;
        guard.operator*()->operator<<(115);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4bf:
        {
        char* d = data;
        guard.operator*()->operator<<(116);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4be:
        {
        char* d = data;
        guard.operator*()->operator<<(117);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 481);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4c0:
        {
        char* d = data;
        guard.operator*()->operator<<(118);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 56);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4c1:
        {
        char* d = data;
        guard.operator*()->operator<<(122);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x44f:
        {
        char* d = data;
        guard.operator*()->operator<<(133);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x450:
        {
        char* d = data;
        guard.operator*()->operator<<(134);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 14);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x44e:
        {
        char* d = data;
        guard.operator*()->operator<<(85);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x514:
        {
        char* d = data;
        guard.operator*()->operator<<(130);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x414:
        {
        char* d = data;
        guard.operator*()->operator<<(153);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 348);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x546:
        {
        char* d = data;
        guard.operator*()->operator<<(170);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x642:
        {
        char* d = data;
        guard.operator*()->operator<<(186);
        guard.operator*()->operator<<((int)(((MemberExpLevelUpView*)d)->m_idByChannel));
        guard->put_binary(d, 19);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x672:
        {
        char* d = data;
        guard.operator*()->operator<<(188);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Add_Buddy*)d)->m_idByChannel));
        guard->put_binary(d, 59);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x674:
        {
        char* d = data;
        guard.operator*()->operator<<(189);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Del_Buddy*)d)->m_idByChannel));
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x9dc:
        {
        char* d = data;
        guard.operator*()->operator<<(146);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 267);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x9dd:
        {
        char* d = data;
        guard.operator*()->operator<<(148);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 48);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x9e0:
        {
        char* d = data;
        guard.operator*()->operator<<(183);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 266);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x9de:
        {
        char* d = data;
        guard.operator*()->operator<<(184);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 20);
        // ORIG put(s_msgq_mgr,5,guard)：QUEUE_IDX 枚举仅命名 0..4，第 6 槽(idx=5)
        // 无队列名（MsgQueueMgr ctor 5→-1 共 6 槽），仍按 ORIG 投 idx5。
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(5), guard);
    }
        break;

        case 0x3f7:
        {
        char* d = data;
        guard.operator*()->operator<<(206);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x76c:
        {
        char* d = data;
        guard.operator*()->operator<<(211);
        guard.operator*()->operator<<((int)(((Packet_GM_Request_Mid*)d)->m_gmId));
        guard->put_binary(d, 52);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x432:
        {
        char* d = data;
        guard.operator*()->operator<<(218);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Send_Guild_Mail*)d)->m_guildId));
        guard->put_binary(d, 19);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x434:
        {
        char* d = data;
        guard.operator*()->operator<<(219);
        guard.operator*()->operator<<((int)(((Packet_Guild_Call_Guild_Invite*)d)->m_guildKey));
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x435:
        {
        char* d = data;
        guard.operator*()->operator<<(220);
        guard.operator*()->operator<<((int)(((GuildInviteToInvitedView*)d)->m_e));
        guard->put_binary(d, 71);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x436:
        {
        // 0x436：权威头 m_accept 为 uchar，ORIG movl 整字读（4B），字段宽度不符，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(221);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x437:
        {
        char* d = data;
        guard.operator*()->operator<<(222);
        guard.operator*()->operator<<((int)(((GuildInviteToCallerView*)d)->m_e));
        guard->put_binary(d, 52);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4cb:
        {
        char* d = data;
        guard.operator*()->operator<<(229);
        guard.operator*()->operator<<((int)(((TowerRankingView*)d)->m_idByChannel));
        guard->put_binary(d, 98);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4cf:
    {
        guard.operator*()->operator<<(230);
        guard.operator*()->operator<<(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x439:
        {
        // 0x439：DBMW 统计族多类共用 pid 0x439，+0x14 处字段归属存疑（Party=char / Party_Charac=uint），布局未建模。
        char* d = data;
        guard.operator*()->operator<<(234);
        guard.operator*()->operator<<((int)((UIntView14*)d)->m_v);
        guard->put_binary(d, 82);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x43d:
        {
        char* d = data;
        guard.operator*()->operator<<(231);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 48);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x43b:
        {
        char* d = data;
        guard.operator*()->operator<<(232);
        guard.operator*()->operator<<((int)(((Packet_Guild_Request_Guild_Master_Delegate*)d)->m_requesterCharNo));
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x43e:
        {
        char* d = data;
        guard.operator*()->operator<<(233);
        guard.operator*()->operator<<((int)(((HasBeenGuildMasterView*)d)->m_channel));
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x43f:
        {
        // 0x43f：权威头 +0xa 为 char m_guildName[]，ORIG movl 整字读（4B），字段宽度不符，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(237);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 45);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x445:
        {
        char* d = data;
        guard.operator*()->operator<<(298);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 46);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb54:
        {
        char* d = data;
        // ORIG je 8476eeb/8476f0c：d[10] 非 0 → HackType::makeRequest()，否则 BackIP::makeRequest()。
        // 0x3ea/0x3eb（DB_ReloadAutoPunishRule* 请求包）无重建包类（makePacketHeader 手拼），布局未建模，保留字节读。
        if (((UCharView0a*)d)->m_v == 0)
            DB_ReloadAutoPunishRuleBackIP::makeRequest();
        else
            DB_ReloadAutoPunishRuleHackType::makeRequest();
    }
        break;

        case 0xb5f:
        {
        char* d = data;
        guard.operator*()->operator<<(401);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 12);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x444:
        {
        // 0x444：权威头 +0x12 为 char m_name[0x1e]，ORIG 此处 movl 整字读（4B），字段宽度不符，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(291);
        guard.operator*()->operator<<((int)((UIntView12*)d)->m_v);
        guard->put_binary(d, 46);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x446:
        {
        char* d = data;
        guard.operator*()->operator<<(292);
        guard.operator*()->operator<<((int)(((GuildNameChangeView*)d)->m_channel));
        guard->put_binary(d, 45);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x447:
        {
        char* d = data;
        guard.operator*()->operator<<(388);
        guard.operator*()->operator<<((int)((UIntView12*)d)->m_v);
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xa8c:
        {
        char* d = data;
        guard.operator*()->operator<<(293);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 61);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb55:
    {
        guard.operator*()->operator<<(294);
        guard.operator*()->operator<<(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb57:
    {
        guard.operator*()->operator<<(341);
        guard.operator*()->operator<<(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb58:
        {
        char* d = data;
        guard.operator*()->operator<<(343);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb5e:
        {
        char* d = data;
        guard.operator*()->operator<<(381);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 1213);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xc1c:
        {
        // 0xc1c：Packet_Monitor_Notify_Auction_Mail 定义在 ServerLab Auction 域 cpp，无字段头，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(239);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 38);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfa2:
        {
        char* d = data;
        guard.operator*()->operator<<(307);
        guard.operator*()->operator<<((int)(((Packet_PvPChannelInfo*)d)->m_uid));
        guard->put_binary(d, 6136);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfa3:
        {
        char* d = data;
        guard.operator*()->operator<<(308);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 42);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfa5:
        {
        char* d = data;
        guard.operator*()->operator<<(309);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 128);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(...,2,guard)
    }
        break;

        case 0xfab:
        {
        // 0xfab：重建源码无该类（ORIG 无导出符号），布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(352);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfac:
        {
        // 0xfac：重建源码无该类，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(353);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfad:
        {
        // 0xfad：重建源码无该类，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(354);
        guard.operator*()->operator<<(((IntView0b*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6d8:
        {
        char* d = data;
        guard.operator*()->operator<<(372);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfae:
        {
        char* d = data;
        guard.operator*()->operator<<(368);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6dc:
        {
        char* d = data;
        guard.operator*()->operator<<(369);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6de:
        {
        char* d = data;
        guard.operator*()->operator<<(370);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6e2:
        {
        char* d = data;
        guard.operator*()->operator<<(371);
        guard.operator*()->operator<<((int)(((ChannelGuildAgitView*)d)->m_channel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4d2:
        {
        char* d = data;
        guard.operator*()->operator<<(374);
        guard.operator*()->operator<<((int)((UIntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6e0:
        {
        char* d = data;
        guard.operator*()->operator<<(378);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6b0:
        {
        char* d = data;
        guard.operator*()->operator<<(379);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfaf:
        {
        char* d = data;
        guard.operator*()->operator<<(382);
        guard.operator*()->operator<<((int)(((Packet_Pcroom_Response*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b6a:
        {
        RevengeView* v = (RevengeView*)data;
        // ORIG 0x8477dab：SetRewardType(d+10) → switch(d+10){1:Open, 2/0:Close} →
        // 0x1b6a：revenge 包为 makePacketHeader(0x1b6a,..) 手拼（Monitor/DNFPacketTranslater.cpp:3343），无类建模。
        // SetCloseTime(d+14) → printf("[Packet_Request_Revenge] ...")。
        GlobalData::s_villageMonsterMgr->SetRewardType(
            (ENUM_VILLAGE_ATTACKED_REWARD)v->m_reward);
        switch (v->m_reward)
        {
        case 1:
            GlobalData::s_revengeDungeonMgr->OpenRevengeDungeon();
            break;
        case 2:
        case 0:
            GlobalData::s_revengeDungeonMgr->CloseRevengeDungeon();
            break;
        }
        GlobalData::s_revengeDungeonMgr->SetCloseTime(v->m_closeTime);
        printf("[Packet_Request_Revenge] isRevengeDungeonOpen : %d, "
               "DungeonClose_Remain_Time : %u\n",
               v->m_reward, v->m_closeTime);
    }
        break;

        case 0x1004:
        {
        char* d = data;
        guard.operator*()->operator<<(389);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1005:
        {
        char* d = data;
        guard.operator*()->operator<<(390);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x100a:
        {
        // 0x100a：Monitor 域 makePacketHeader(0x100a,0x1e1) 手拼包，无类建模，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(392);
        guard.operator*()->operator<<((int)((UIntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1006:
        {
        char* d = data;
        guard.operator*()->operator<<(391);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6e5:
        {
        char* d = data;
        guard.operator*()->operator<<(398);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6b2:
        {
        char* d = data;
        guard.operator*()->operator<<(408);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 23);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6ea:
        {
        char* d = data;
        guard.operator*()->operator<<(409);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x100f:
        {
        char* d = data;
        guard.operator*()->operator<<(416);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb60:
        {
        char* d = data;
        guard.operator*()->operator<<(418);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70a:
        {
        char* d = data;
        guard.operator*()->operator<<(426);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x712:
        {
        char* d = data;
        guard.operator*()->operator<<(427);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70b:
        {
        char* d = data;
        guard.operator*()->operator<<(428);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70c:
        {
        char* d = data;
        guard.operator*()->operator<<(429);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70d:
        {
        char* d = data;
        guard.operator*()->operator<<(430);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70e:
        {
        char* d = data;
        guard.operator*()->operator<<(431);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70f:
        {
        char* d = data;
        guard.operator*()->operator<<(432);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x713:
        {
        char* d = data;
        guard.operator*()->operator<<(434);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x2328:
        {
        char* d = data;
        guard.operator*()->operator<<(435);
        guard.operator*()->operator<<((int)(((GuildBoardReplyView*)d)->m_f));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x2332:
        {
        // 0x2332：权威头 Packet_Guild_Reply_Guild_Board_Delete 的 m_charNo 在 +0x10，+0xf 无字段；ORIG movl 整字读，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(436);
        guard.operator*()->operator<<((int)((UIntView0f*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1039:
        {
        char* d = data;
        guard.operator*()->operator<<(402);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x103a:
        {
        char* d = data;
        guard.operator*()->operator<<(404);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x9d3:
        {
        char* d = data;
        guard.operator*()->operator<<(437);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1773:
        {
        char* d = data;
        guard.operator*()->operator<<(441);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1774:
        {
        char* d = data;
        guard.operator*()->operator<<(442);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1776:
        {
        char* d = data;
        guard.operator*()->operator<<(443);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1777:
        {
        char* d = data;
        guard.operator*()->operator<<(444);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1778:
        {
        char* d = data;
        guard.operator*()->operator<<(445);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1775:
        {
        char* d = data;
        guard.operator*()->operator<<(446);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x177a:
        {
        char* d = data;
        guard.operator*()->operator<<(447);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x177e:
        {
        char* d = data;
        guard.operator*()->operator<<(448);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb61:
        {
        char* d = data;
        guard.operator*()->operator<<(461);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x17c0:
        {
        char* d = data;
        guard.operator*()->operator<<(463);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x17c2:
        {
        char* d = data;
        guard.operator*()->operator<<(465);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b66:
        {
        char* d = data;
        guard.operator*()->operator<<(202);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 351);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b5b:
        {
        char* d = data;
        guard.operator*()->operator<<(470);
        guard.operator*()->operator<<((int)(((Packet_Response_PvP_Buddy_Conn_List*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b5d:
        {
        char* d = data;
        guard.operator*()->operator<<(471);
        guard.operator*()->operator<<((int)(((Packet_Notice_Request_Add_PvP_Buddy*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b5f:
        {
        char* d = data;
        guard.operator*()->operator<<(472);
        guard.operator*()->operator<<((int)(((Packet_Notice_Add_PvP_Buddy_Result*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b61:
        {
        char* d = data;
        guard.operator*()->operator<<(473);
        guard.operator*()->operator<<((int)(((Packet_Response_Remove_PvP_Buddy*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b62:
        {
        char* d = data;
        guard.operator*()->operator<<(474);
        guard.operator*()->operator<<((int)(((Packet_Notice_PvP_Buddy_In_Out*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b63:
        {
        char* d = data;
        guard.operator*()->operator<<(475);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(...,2,guard)
    }
        break;

        case 0x1b64:
        {
        char* d = data;
        guard.operator*()->operator<<(476);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(...,2,guard)
    }
        break;

        case 0x1b65:
        {
        char* d = data;
        guard.operator*()->operator<<(477);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(...,2,guard)
    }
        break;

        case 0x27f9:
        {
        char* d = data;
        // ORIG je 84797de：make_internal_stream_jpn(guard, 0x2a, -1) + put_binary(dyn)。
        ARAD::DISPATCHER::make_internal_stream_jpn(
            guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2a, -1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27fa:
        {
        char* d = data;
        // ORIG je 847985a：make_internal_stream_jpn(guard, 0x2b, -1) + put_binary(dyn)。
        ARAD::DISPATCHER::make_internal_stream_jpn(
            guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2b, -1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1bbd:
        {
        char* d = data;
        guard.operator*()->operator<<(532);
        guard.operator*()->operator<<(((IntView0f*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1bc1:
        {
        char* d = data;
        guard.operator*()->operator<<(533);
        guard.operator*()->operator<<(((IntView12*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f40:
        {
        char* d = data;
        guard.operator*()->operator<<(503);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f42:
        {
        char* d = data;
        guard.operator*()->operator<<(504);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b6c:
        {
        char* d = data;
        guard.operator*()->operator<<(521);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b6d:
        {
        char* d = data;
        guard.operator*()->operator<<(522);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f45:
        {
        char* d = data;
        guard.operator*()->operator<<(523);
        guard.operator*()->operator<<((int)(((Packet_Find_Charac_Name_UseUID*)d)->m_dbid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f4a:
        {
        char* d = data;
        guard.operator*()->operator<<(528);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f4b:
        {
        char* d = data;
        guard.operator*()->operator<<(529);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f4c:
        {
        char* d = data;
        guard.operator*()->operator<<(530);
        guard.operator*()->operator<<((int)(((TimeSyncLoginView*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1bc3:
        {
        char* d = data;
        guard.operator*()->operator<<(539);
        guard.operator*()->operator<<(((IntView12*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1bc7:
        {
        char* d = data;
        guard.operator*()->operator<<(541);
        guard.operator*()->operator<<(((IntView0e*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1bc8:
        {
        char* d = data;
        guard.operator*()->operator<<(542);
        guard.operator*()->operator<<(((IntView0e*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfe6:
        {
        // 0xfe6：重建源码无该类（4070），布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(694);
        if (((IntView0e*)d)->m_v != 0)
            guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        else
            guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfe7:
    {
        // ORIG 0x847a140：GlobalData::s_hades_proxy->ConnTcpHadesSvr() 后直返。
        GlobalData::s_hades_proxy->ConnTcpHadesSvr();
    }
        break;

        case 0xb64:
        {
        char* d = data;
        guard.operator*()->operator<<(603);
        guard.operator*()->operator<<((int)(((PunishCancelView*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x2711:
        {
        char* d = data;
        guard.operator*()->operator<<(608);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 270);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x2717:
        {
        char* d = data;
        // ORIG 0x847a272：CSwitchLog::SetLogWriteFlag(*(int*)(d+10)) 后直返
        // （ORIG mov 0xa(%eax),%eax 整字读取，非单字节）。
        CSwitchLog::SetLogWriteFlag(((IntView0a*)d)->m_v);
    }
        break;

        case 0xb65:
        {
        char* d = data;
        guard.operator*()->operator<<(628);
        guard.operator*()->operator<<((int)(((SecuConnectWebView*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        // ORIG 0847a316 put 后无 jmp：直落 0x2712 的 cMyTrace 块（fall through 照抄，P171）
        case 0x2712:
    {
        // ORIG je 847a31b：cMyTrace(0xdbf,5) "TOD: bug random select" 后直返。
        cMyTrace(__PRETTY_FUNCTION__, 0xdbf, 5)("TOD: bug random select");
    }
        break;

        case 0x2713:
    {
        // ORIG je 847a35c：cMyTrace(0xdcc,5) "TOD: bug reload" 后直返。
        cMyTrace(__PRETTY_FUNCTION__, 0xdcc, 5)("TOD: bug reload");
    }
        break;

        case 0x2718:
        {
        char* d = data;
        guard.operator*()->operator<<(677);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x2719:
        {
        char* d = data;
        guard.operator*()->operator<<(678);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x271a:
        {
        char* d = data;
        guard.operator*()->operator<<(679);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x271b:
        {
        char* d = data;
        guard.operator*()->operator<<(680);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27d9:
        {
        char* d = data;
        guard.operator*()->operator<<(709);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27da:
        {
        // 0x27da：LimitNpcBuyItemInfoAll +0xa 仅有占位名 m_padA（非权威字段名），保留裸读。
        char* d = data;
        guard.operator*()->operator<<(710);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27db:
        {
        char* d = data;
        guard.operator*()->operator<<(711);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27dc:
        {
        char* d = data;
        guard.operator*()->operator<<(712);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27df:
        {
        char* d = data;
        guard.operator*()->operator<<(718);
        guard.operator*()->operator<<(((IntView0e*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27e8:
        {
        char* d = data;
        guard.operator*()->operator<<(763);
        guard.operator*()->operator<<((int)(((CollectRewardView*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27e9:
        {
        char* d = data;
        guard.operator*()->operator<<(764);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27e7:
        {
        char* d = data;
        guard.operator*()->operator<<(762);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27f2:
        {
        char* d = data;
        guard.operator*()->operator<<(770);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27f8:
        {
        char* d = data;
        guard.operator*()->operator<<(815);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27fe:
    {
        // ORIG je 847ab96：initGeolocation 失败 → logFormat(1,...,0x1143,
        // "GEO IP TABLE RELOAD FAIL")，恒返 1。
        if (!ARAD::Singleton<RestrictGeolocation>::Get()->initGeolocation())
        {
            LogManager::logFormat(1, "DispatchServerPacket.cpp", __PRETTY_FUNCTION__,
                                  0x1143, "GEO IP TABLE RELOAD FAIL");
        }
    }
        break;

        case 0x27ff:
    {
        // ORIG je 847abd8：ServiceRestrictManager::loadPolicy()，恒返 1。
        ARAD::Singleton<ServiceRestrictManager>::Get()->loadPolicy();
    }
        break;

        case 0x6a5:
    {
        // ORIG je 847abf1：空 case（无操作），返 1。
    }
        break;

        case 0x6af:
    {
        // ORIG je 847abf4：空 case（无操作），返 1。
    }
        break;

        case 0x6ae:
    {
        // ORIG je 847abf7：空 case（无操作），返 1。
    }
        break;

        case 0x6a8:

        case 0x6a9:
    {
        // ORIG 08473c03：sub $0x6a8; cmp $1; jbe → 0x6a8/0x6a9 空 case，返 1。
    }
        break;

        case 0x6ab:

        case 0x6ac:
    {
        // ORIG cmp $0x6ab jge 0847abee：0x6ab/0x6ac 空 case，返 1。
    }
        break;

    default:
    {
        // 未知 sig:ORIG LAB_0847abe7 mov $0x0,%ebx → 返回 0(调用侧
        // GlobalData.cpp `if (!dispatchPacket(data))` 据此走 fallback 处理)。
        return 0;
    }

    }
    // 已知 sig 统一走 ORIG 0847abfb（mov $1,%ebx）→ 返回 1。
    return 1;
}