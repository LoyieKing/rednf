int Dispatcher_AvartarRoulette::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x312;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() < 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 7);
        return 0;
    }
    arad::SigAvatarRoulette sig;      // ORIG 栈 -0xb，C1 清 3 字节
    if (packet.get_byte(sig.m_type) != 1)
    {
        return 0x32f;
    }
    if (packet.get_byte(sig.m_slotIdx) != 1)
    {
        return 0x331;
    }
    if (sig.m_type > 0xa)
    {
        return 0x334;
    }
    if (sig.m_slotIdx > 9)
    {
        return 0x337;
    }
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x347), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2c, (int)user->GetUID());
    guard->put_binary((void*)&sig, 3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
