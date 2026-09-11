// ============================================================================
// CInventory::MakeSecuRewarditemPacket（ORIG 0x084fd6d2）。仅 EQUIP(1) 生效：
// 空装备槽返回 0；否则写 put_short(slot)，item_idx 依 m_addInfo2 为 0 时置 -1，
// 再 put_int(m_addInfo2) 并返回 1。
// ============================================================================
int CInventory::MakeSecuRewarditemPacket(INVEN_TYPE invenType, int slot,
                                         PacketGuard& packet) const
{
    // b28d-solo-CInventory [P0+P223 双岛]：①ORIG 无 `buf` 局部槽——每调用点重新
    // `mov 0x14(%ebp),%eax; mov %eax,(%esp)`（形参直用）⇒ 删除具名引用局部；
    // ②ORIG 两 fail 块独立落 +0xd5/+0xdc（`jne <tail>` / `je <mid>`）⇒ 双 goto
    // 标签（fail2 前、fail1 后）复现（探针 /tmp/probe_guard2.cpp fD 逐条同构）。
    if (invenType != INVEN_TYPE_EQUIP)
        goto l_msr_fail1;
    if (m_pEquipSlot[slot].m_addInfo == 0)
        goto l_msr_fail2;
    ((InterfacePacketBuf*)&packet)->put_short(slot);
    if (m_pEquipSlot[slot].m_addInfo == 0 || m_pEquipSlot[slot].m_addInfo2 == 0)
        ((InterfacePacketBuf*)&packet)->put_item_idx(0xffffffff);
    else
        ((InterfacePacketBuf*)&packet)->put_item_idx(m_pEquipSlot[slot].m_addInfo);
    ((InterfacePacketBuf*)&packet)->put_int(m_pEquipSlot[slot].m_addInfo2);
    return 1;
l_msr_fail2:
    return 0;
l_msr_fail1:
    return 0;
}