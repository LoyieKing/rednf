int Dispatcher_AradUseAvatarRecharge::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x69c;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() <= 2 || user->get_state() > 4)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 7);
        return 0;
    }
    arad::SigAradUseAvatarRecharge sig;  // ORIG 栈 -0x18，C1 清 0x10
    // ORIG 读序：先 sig+0x02（stoneSlot），后 sig+0x00（avatarSlot）。
    if (packet.get_short(sig.stoneSlot) != 1)
    {
        return 0x6b6;
    }
    if (packet.get_short(sig.avatarSlot) != 1)
    {
        return 0x6ba;
    }
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x6c2), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x30, (int)user->GetUID());
    guard->put_binary((void*)&sig, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
