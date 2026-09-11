// [b28d-solo-CCargo] AE（fresh 190=190；compare_tu clean 口径）——if/else 交换：
// ORIG `test %eax,%eax; je <+0x1e0 空槽块>`（非空块直落、空槽块在函数尾 `jmp +0x29e`）
// ⇒ 源码 `if (m_addInfo != 0) {非空} else {空槽}`。p167 调用点差集 CLEAR（28=28）。
// 原 b21 verdict=DIFF / N82 判词作废（块序翻正后槽号自然对齐）。
void CCargo::MakeItemPacket(PacketGuard* packet, int slot) const
{
    ((InterfacePacketBuf*)packet)->put_short(slot);
    // [b28d-solo-CCargo] ORIG 0x0850c160 `test %eax,%eax; je <+0x1e0 空槽块>`：
    // 非空块直落、空槽块在函数尾（`jmp +0x29e` 收尾）⇒ 源码写
    // `if (addInfo != 0) {非空} else {空槽}`（4.4 分支块序与源码同向）。
    if (m_pItems[slot].m_addInfo != 0) {
        ((InterfacePacketBuf*)packet)->put_int(m_pItems[slot].m_addInfo);
        ((InterfacePacketBuf*)packet)->put_int(m_pItems[slot].m_addInfo2);
        ((InterfacePacketBuf*)packet)->put_byte(
            (int)GetIntegratedPvPItemAttr(m_pItems[slot]) & 0xff);
        ((InterfacePacketBuf*)packet)->put_short(m_pItems[slot].m_fieldb);
        ((InterfacePacketBuf*)packet)->put_byte(m_pItems[slot].m_field0);
        if (G_GameWorld()->IsEnchantRevisionChannel())
            ((InterfacePacketBuf*)packet)->put_int(0);
        else
            ((InterfacePacketBuf*)packet)->put_int(m_pItems[slot].m_fieldd);
        ((InterfacePacketBuf*)packet)->put_byte(
            m_pItems[slot].m_amp.getAbilityType() & 0xff);
        ((InterfacePacketBuf*)packet)->put_short(
            m_pItems[slot].m_amp.getAbilityValue() & 0xffff);
        G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(
            m_pParent, m_pItems[slot], packet);
        ((InterfacePacketBuf*)packet)->put_packet(m_pItems[slot]);
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