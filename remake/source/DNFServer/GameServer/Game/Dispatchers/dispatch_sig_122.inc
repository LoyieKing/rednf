int Dispatcher_UseSharedEffectItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int itemIdx = 0;                    // -0x18
    short slot = 0;                     // -0x1a
    if (!packet.get_int(itemIdx))
    {
        return LineFunc(0xd919,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(slot))
    {
        return LineFunc(0xd91a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() != 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 1);
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 1);
        return 0;
    }
    // P48 去缓存：ORIG 对 getCurCharacInvenW 两次独立重调（GetInvenSlot/delete_item）。
    Inven_Item item = user->getCurCharacInvenW()->GetInvenSlot(1, (int)slot);
    if (item.isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 0x16);
        return 0;
    }
    if (!(0 < item.m_addInfo2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 0x16);
        return 0;
    }
    if (item.m_addInfo != itemIdx)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 1);
        return 0;
    }
    CStackableItem* proto =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(itemIdx));
    CStackableItem* found = proto;
    if (found == 0 || !found->IsSharedEffectItem())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 0x11);
        return 0;
    }
    if (user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, (int)slot, 1, (eItemDelReason)3, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 1);
        return 0;
    }
    PacketGuard guard;
    guard.put_header(1, 0xff);
    guard.put_byte(1);
    guard.put_short((int)slot);
    guard.finalize(true);
    user->Send(guard);
    guard.clear();
    guard.put_header(0, 0xdb);
    guard.put_short(user->get_unique_id() & 0xffff);
    guard.put_int(itemIdx);
    guard.finalize(true);
    G_GameWorld()->send_all_with_state(guard, 3);
    return 0;
