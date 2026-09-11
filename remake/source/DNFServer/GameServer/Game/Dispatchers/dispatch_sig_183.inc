int Dispatcher_AvartarHiddenOptionChange::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x632;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() <= 2 || user->get_state() > 4)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 7);
        return 0;
    }
    arad::SigAvatarHiddenOptionChange sig;  // ORIG 栈 -0xe，C1 清 6 字节
    if (packet.get_short(sig.m_materialSlot) != 1)
    {
        return 0x64d;
    }
    if (packet.get_short(sig.m_avatarSlot) != 1)
    {
        return 0x651;
    }
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x659), true);   // P134 内联 Acquire，无 stream 局部
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2e, (int)user->GetUID());
    guard->put_binary((void*)&sig, 6);                            // P103 operator-> 链
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}