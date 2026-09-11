int Dispatcher_AradEmblemCompound::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x6f1;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() <= 2 || user->get_state() > 4)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 7);
        return 0;
    }
    const int avatarSocketErr = user->isEnableAvatarSocketAction();
    if (avatarSocketErr != 0)
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x24e,
            (unsigned char)(avatarSocketErr & 0xff));
        return 0;
    }
    arad::SigAradEmblemCompound sig;     // ORIG 栈 -0x22，C1 清 0x18
    if (packet.get_short(sig.m_slot0) != 1)
    {
        return 0x713;
    }
    if (packet.get_short(sig.m_slot1) != 1)
    {
        return 0x717;
    }
    if (packet.get_short(sig.m_selectIdx) != 1)
    {
        return 0x71b;
    }
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x723), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x32, (int)user->GetUID());
    guard->put_binary((void*)&sig, 0x16);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}