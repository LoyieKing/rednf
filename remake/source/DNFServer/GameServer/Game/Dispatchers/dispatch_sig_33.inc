int Dispatcher_FairPvPScore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0822793c（PacketDispatcher_Impl_1.cpp 0xe439 区）
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    unsigned short uniqueId = 0;
    if (!packet.get_short(uniqueId))
    {
        return LineFunc(0xe439,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char mode = 0;
    if (!packet.get_byte(mode))
    {
        return LineFunc(0xe43c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if ((unsigned short)user->get_unique_id() == uniqueId)
    {
        PacketGuard guard;
        guard.put_header(1, 0x177);
        guard.put_byte(1);
        guard.put_short((int)uniqueId);
        fair_pvp::CFairPvPScore* score =
            (fair_pvp::CFairPvPScore*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
        score->MakePacketSocre(guard, (int)mode);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    CUser* target = G_GameWorld()->find_from_world(uniqueId);
    if (target == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x177, 3);
        return 0;
    }
    PacketGuard guard;
    if (mode == 1)
    {
        // ORIG：每次操作重取 getCurCharacInvenR（P48 无 CSE）；数量字段
        // 直读 +0x07（m_addInfo2，非 get_add_info 调用）。
        int slot = user->getCurCharacInvenR()->check_item_exist(0xcf0);
        if (slot == -1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x177, 0x11);
            return 0;
        }
        Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
        if (item.m_addInfo2 < 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x177, 0x11);
            return 0;
        }
        if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slot, 1,
                                                    (eItemDelReason)3, true) != 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x177, 0x11);
            return 0;
        }
        guard.clear();
        guard.put_header(0, 0xe);
        guard.put_byte(0);
        guard.put_short(1);
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, guard);
        guard.finalize(true);
        user->Send(guard);
    }
    guard.clear();
    guard.put_header(1, 0x177);
    guard.put_byte(1);
    guard.put_short((int)uniqueId);
    fair_pvp::CFairPvPScore* score =
        (fair_pvp::CFairPvPScore*)target->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
    score->MakePacketSocre(guard, (int)mode);
    guard.finalize(true);
    user->Send(guard);
    return 0;
