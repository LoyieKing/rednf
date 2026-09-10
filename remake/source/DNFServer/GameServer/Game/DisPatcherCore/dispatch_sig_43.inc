int DisPatcher_SetIP::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() >= 4)
        return 0;

    // ORIG 0x081f468c：wire 字段全体是单个栈 blob（base -0xf6，`lea -0xf6,%eax;
    // add $0xd/e/12/16/18/1c/20` 传址、`movzbl -0xe9` 等 disp 读）——单一 pack(1)
    // 视图结构体本地量复现连续落栈（技47/50 同址视图法；独立局部分散落槽为 4.4 常态、
    // 与 ORIG 差 20+ 行）。
    #pragma pack(push, 1)
    struct SetIPMsgView {
        char m_head[13];                 // +0x00..0x0c
        unsigned char m_natType;         // +0x0d（ORIG `movzbl -0xe9` 单步零延装载 ⇒ uchar）
        unsigned int m_innerIp;          // +0x0e
        unsigned int m_natIp;            // +0x12
        unsigned short m_natPort;        // +0x16
        unsigned int m_mtu;              // +0x18
        unsigned int m_macLen;           // +0x1c（ORIG get_int(unsigned int&) 实证）
        char m_macBuf[0x41];             // +0x20..0x60
    };
    #pragma pack(pop)
    SetIPMsgView msg;                    // 技36：无具名引用中转（引用会多占 spill 槽）
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 调 get_byte(char&)（ERc）。
    if (!packet.get_byte(*reinterpret_cast<char*>(&msg.m_natType)))
        return LineFunc(0x80e4, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(msg.m_innerIp))
        return LineFunc(0x80e5, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(msg.m_natIp))
        return LineFunc(0x80e6, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(msg.m_natPort))
        return LineFunc(0x80e7, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(msg.m_mtu))
        return LineFunc(0x80e8, __PRETTY_FUNCTION__, 0, 0);
    user->m_network.set_nat(msg.m_natType, msg.m_innerIp, msg.m_natIp,
                            msg.m_natPort, msg.m_mtu);

    if (!packet.get_int(msg.m_macLen))
        return LineFunc(0x8100, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(msg.m_macBuf, 0x41, msg.m_macLen))
        return LineFunc(0x8102, __PRETTY_FUNCTION__, 0, 0);

    char digestHex[0x41] = {0};          // P312：ORIG 0x081f490c 内联对齐剥离+rep stos 慢路
    getSha256UsingSalt(reinterpret_cast<const unsigned char*>(msg.m_macBuf),
                       0x41, digestHex);

    cMyTrace trace(__PRETTY_FUNCTION__, 0x810b, 0);
    trace("(acc_id: %s) (nat type: %d) (inner_ip: %d) (nat_ip: %d) "
          "(nat_port: %d) (mac_addr: %s) (mtu: %d)",
          NumberToString(user->get_acc_id(), 0), (int)(signed char)msg.m_natType,
          msg.m_innerIp, msg.m_natIp, (int)msg.m_natPort, digestHex, msg.m_mtu);

    // 第二次 set_nat：以 mac 摘要串写入（ORIG set_nat(...,const char* macAddr)）
    user->m_network.set_nat(msg.m_natType, msg.m_innerIp, msg.m_natIp,
                            msg.m_natPort, digestHex);

    char peerIp[0x14] = {0};             // ORIG 0x081f4a94..0x081f4ab0 5×movl $0 清零
    // [G5-SigA] P52/P183：ORIG 0x081f4ae4 `call GetPeerIP2` 后返回值**不经槽**，
    // 由 callee-saved %ebx 跨 0x081f4ae4..0x081f4af0 的 get_str_ip 调用保活
    // （0x081f4ae9 `mov %ebx,0x4(%esp)`），即原码是单语句
    // `checkDenyIP(get_str_ip(), GetPeerIP2(peerIp, 0x14))`（实参右→左求值）。
    // 原具名 char* ip2 多出槽写入 + 该 4B 槽使帧 0x12c→0x13c（fn_diff_tu 实测）。
    if (!(bool)(unsigned char)checkDenyIP(
            user->m_network.get_str_ip(),
            user->m_network.GetPeerIP2(peerIp, 0x14)))
    {
        PacketGuard packetGuard;
        packetGuard.put_header(1, 2);
        packetGuard.put_byte(1);
        packetGuard.finalize(true);
        user->Send(packetGuard);
        return 0;
    }

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "PacketDispatcher_Impl_1.cpp", 0x811d),
                       true);
    guard.operator*()->operator<<((int)0x9e);
    guard.operator*()->operator<<((int)-1);
    // [b28d-solo-DisPatcherCore] P103：ORIG 先调 CStreamGuard::operator->() 再
    // GetInBuffer（p167 MISSING _ZN12CStreamGuardptEv）⇒ 保留 operator-> 链。
    SIG_SAVE_PROXY_LOG* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_PROXY_LOG>();
    sig->m_accId = user->get_acc_id();
    strncpy(sig->m_strIp, user->m_network.get_str_ip(), 0x10);
    strncpy(sig->m_peerIp, peerIp, 0x10);
    sig->m_natType = (unsigned char)msg.m_natType;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);

    DB_SelectAllowProxyUser::makeRequest((int)user->GetUID(),
                                         user->get_acc_id());
    return 0;
}
