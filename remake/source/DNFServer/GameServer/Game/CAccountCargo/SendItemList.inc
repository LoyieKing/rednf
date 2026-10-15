bool CAccountCargo::SendItemList()   // b28d-5：随 CUser.h:848 bool 化（ORIG 0x0865dcaf 直传形）
{
    // [b28d-solo-CAccountCargo] ORIG 0x0828a88a：无 buf/world 具名局部——每调用点
    // 重新 `lea -0x18(%ebp)`，G_GameWorld() 结果 %eax 直通虚调用（P0 右值内联）。
    // 原 buf 局部占 -0x14 槽使帧 0x20→0x30、循环变量 -0xc→-0x10 全帧平移。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xd);
    ((InterfacePacketBuf*)&packet)->put_byte(0xc);
    ((InterfacePacketBuf*)&packet)->put_short(m_capacity);
    ((InterfacePacketBuf*)&packet)->put_int(m_money);
    ((InterfacePacketBuf*)&packet)->put_short(GetItemCount());
    for (int i = 0; (int)m_capacity > i; ++i) {
        if (m_slots[i].m_addInfo != 0) {
            ((InterfacePacketBuf*)&packet)->put_short(i);
            ((InterfacePacketBuf*)&packet)->put_int(m_slots[i].m_addInfo);
            ((InterfacePacketBuf*)&packet)->put_int(m_slots[i].m_addInfo2);
            ((InterfacePacketBuf*)&packet)->put_byte(
                (unsigned char)GetIntegratedPvPItemAttr(m_slots[i]));
            ((InterfacePacketBuf*)&packet)->put_short(m_slots[i].m_endurance);
            ((InterfacePacketBuf*)&packet)->put_byte(m_slots[i].m_seal);
            if (G_GameWorld()->IsEnchantRevisionChannel())
                ((InterfacePacketBuf*)&packet)->put_int(0);
            else
                ((InterfacePacketBuf*)&packet)->put_int(m_slots[i].m_extendInfo);
            ((InterfacePacketBuf*)&packet)->put_byte(m_slots[i].m_amp.getAbilityType() & 0xff);
            ((InterfacePacketBuf*)&packet)->put_short(m_slots[i].m_amp.getAbilityValue() & 0xffff);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_packet(m_slots[i]);
        }
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return m_pUser->Send(packet);
}