int Dispatcher_CheckGuildAddressDouble::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xa3af,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int len = 0;                                           // -0x14
    if (!packet.get_int(len))
    {
        return LineFunc(0xa3be,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char* addr = user->getCreateGuildAddr();               // -0x10
    if (addr == 0)
    {
        return LineFunc(0xa3c2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(addr, 0xd, len))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa0, 0x6e);
        return 0;
    }
    if (!check_url(addr))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa0, 0x70);
        return 0;
    }
    // [G1Csig · P0 槽律] ORIG 0x082087a4（size 0x281）直证：`call Acquire; mov %eax,(%esp);
    // movl $0x1,0x4(%esp); call CStreamGuard` —— stream **无具名槽**（我方原写具名
    // Stream* 使 len/addr 槽整体下移 4：len -0x14→-0x18、addr -0x10→-0x14）
    // ⇒ 去具名改直传（同 dispatch_sig_146 手法）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "PacketDispatcher_Impl_1.cpp", 0xa3d1), true);     // 0x0828fa86
    **guard << (int)0xec;
    **guard << (int)user->GetUID();
    SIG_DOUBLE_CHECK_GUILD_ADDRESS* sig =
        guard->GetInBuffer<SIG_DOUBLE_CHECK_GUILD_ADDRESS>();
    memset(sig, 0, 0xd);
    memcpy(sig, addr, len);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;                                              // guard 析构 0x0861c8d2
