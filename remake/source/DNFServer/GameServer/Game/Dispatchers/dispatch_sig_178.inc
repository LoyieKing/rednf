int Dispatcher_AradAvatarConvert::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x74a;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() <= 2 || user->get_state() > 4)
    {
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 7);
        return 0;
    }
    const int avatarSocketErr = user->isEnableAvatarSocketAction();
    if (avatarSocketErr != 0)
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x24f,
            (unsigned char)(avatarSocketErr & 0xff));
        return 0;
    }
    // ORIG 栈 -0x1a（2 对齐槽位）而 4.4 落 -0x1c：该类型自然对齐 4，变量级
    // packed 属性被 GCC 忽略（实测警告），只能靠类型本身 pack —— 需改
    // MoreTypes.h 的 SigAradAvatarConvert sizeof 0x10→0x12 才有 2 对齐槽位，
    // 证据不足，记 N 类（跨版本栈对齐洞）。
    arad::SigAradAvatarConvert sig;      // ORIG 栈 -0x1a，C1 清 0x10
    if (packet.get_short(sig.m_selectNo) != 1)
    {
        return 0x76d;
    }
    if (packet.get_short(*reinterpret_cast<unsigned short*>(&sig.m_slot)) != 1)
    {
        return 0x771;
    }
    if (packet.get_int(*reinterpret_cast<unsigned long*>(&sig.m_itemId)) != 1)
    {
        return 0x775;
    }
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x780), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x33, (int)user->GetUID());
    guard->put_binary((void*)&sig, 0xe);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
