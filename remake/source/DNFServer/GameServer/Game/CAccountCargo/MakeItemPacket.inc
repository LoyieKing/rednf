// [b28d-solo-CAccountCargo 判词·AE] ORIG 0x0828ab1c：158/158；空槽/非空块序按 ORIG
//   `test m_addInfo; je <空槽块尾部+0x196>` 反转为 `if (addInfo != 0) {非空} else {空槽}`
//   （P191/P222）。p167 CLEAR（27/27）。
void CAccountCargo::MakeItemPacket(PacketGuard* packet, int slot) const
{
    // [b28d-solo-CAccountCargo] ORIG 0x0828ab1c：`test m_addInfo; je <空槽块
    // +0x196 尾部>` ⇒ 非空块 fall-through、空槽块后置 ⇒ 源形 `if (addInfo != 0)
    // {非空} else {空槽}`（P191/P222 块序）。原 `== 0` 正极性令空槽块前置。
    ((InterfacePacketBuf*)packet)->put_short(slot);
    if (m_slots[slot].m_addInfo != 0) {
        ((InterfacePacketBuf*)packet)->put_int(m_slots[slot].m_addInfo);
        ((InterfacePacketBuf*)packet)->put_int(m_slots[slot].m_addInfo2);
        ((InterfacePacketBuf*)packet)->put_byte(
            (int)GetIntegratedPvPItemAttr(m_slots[slot]) & 0xff);
        ((InterfacePacketBuf*)packet)->put_short(m_slots[slot].m_endurance);
        ((InterfacePacketBuf*)packet)->put_byte(m_slots[slot].m_seal);
        if (G_GameWorld()->IsEnchantRevisionChannel())
            ((InterfacePacketBuf*)packet)->put_int(0);
        else
            ((InterfacePacketBuf*)packet)->put_int(m_slots[slot].m_extendInfo);
        ((InterfacePacketBuf*)packet)->put_byte(
            m_slots[slot].m_amp.getAbilityType() & 0xff);
        ((InterfacePacketBuf*)packet)->put_short(
            m_slots[slot].m_amp.getAbilityValue() & 0xffff);
        ((InterfacePacketBuf*)packet)->put_byte(0);
        ((InterfacePacketBuf*)packet)->put_packet(m_slots[slot]);
    } else {
        ((InterfacePacketBuf*)packet)->put_int(-1);
        ((InterfacePacketBuf*)packet)->put_int(0);
        ((InterfacePacketBuf*)packet)->put_byte(0);
        ((InterfacePacketBuf*)packet)->put_short(0);
        ((InterfacePacketBuf*)packet)->put_byte(0);
        ((InterfacePacketBuf*)packet)->put_int(0);
        ((InterfacePacketBuf*)packet)->put_byte(0);
        ((InterfacePacketBuf*)packet)->put_short(0);
        ((InterfacePacketBuf*)packet)->put_byte(0);
        ((InterfacePacketBuf*)packet)->put_packet(g_emptySlot);
    }
}