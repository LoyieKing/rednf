void Inter_NpcLimitBuyItemInfo::sendPacket(CUser* user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 133/133：去 inventory 局部
    // （P0：getCurCharacInvenR 返回值寄存器直传 GetInvenRef）⇒ 帧 0x30→0x20 归位。
    PacketGuard guard;
    guard.put_header(1, 0x1d1);
    guard.put_byte(1);
    guard.put_byte(static_cast<int>(static_cast<char>(m_data1c)));
    if (m_data1c == 0)
    {
        guard.put_short(m_param1);
        guard.put_int(m_invenItem1.m_addInfo);
        guard.put_int(m_invenItem1.m_addInfo2);
        guard.put_short(static_cast<int>(m_invenItem1.m_fieldb));
        guard.put_byte(static_cast<int>(m_invenItem1.m_amp.getAbilityType()) & 0xff);
        guard.put_short(m_invenItem1.m_amp.getAbilityValue() & 0xffff);
        guard.put_packet(m_invenItem1);
    }
    guard.put_int(m_invenItem2.m_addInfo);
    // [b28d-solo-Inter_LoadEtc_impl] P0：ORIG 0x084e6d70 无 inventory 局部
    // （getCurCharacInvenR 返回值寄存器直传 GetInvenRef 实参）。
    const Inven_Item* ref = user->getCurCharacInvenR()->GetInvenRef(1, m_param2);
    if (ref == 0)
    {
        guard.put_int(0);
    }
    else
    {
        guard.put_int(ref->m_addInfo2);
    }
    guard.finalize(true);
    user->Send(guard);
