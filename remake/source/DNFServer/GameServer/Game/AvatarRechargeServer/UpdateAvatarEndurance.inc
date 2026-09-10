void AvatarRechargeServer::UpdateAvatarEndurance(CUser::eSendTarget target,
                                                 CUser* user, short slot)
{
    if (slot < 0 || slot > 9)
        return;
    // [b28d-solo-AvatarRechargeServer] P48 去缓存：ORIG 0x0819142a `mov %eax,(%esp);
    // call GetInvenRef` 无 invenW 槽（getCurCharacInvenW 结果右值直传实参槽）。
    Inven_Item* item = user->getCurCharacInvenW()->GetInvenRef(0, (int)slot);
    if (item == 0 || item->isEmpty())
        return;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x220);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short((int)slot);
    packet.put_int(item->m_addInfo);
    packet.put_short(
        (int)(unsigned short)item->m_fieldSplit.m_avatarEndur);
    packet.finalize(true);
    user->SendPacket(target, packet);
