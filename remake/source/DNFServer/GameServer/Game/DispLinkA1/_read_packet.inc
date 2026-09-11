// ============================================================================
// 9. Dispatcher_Compound_ExtreamItem::_read_packet  ORIG 0x08225806
// [b28d-solo-DispLinkA1] verdict=AE（172=172，ndiff=0；p167 12/12 CLEAR）。
//   修法：P0 删具名 slot 局部（ORIG 值留 %edi 跨 GetInvenSlot）；P211 档③
//   字段侧旋钮——m_itemRefs 改 4 字节整型存储（Dispatchers.h），复现 ORIG
//   RHS-first + `mov %eax,%edx` 中转（const CItem*/const void* 两形实测均同型）。
// ============================================================================
int Dispatcher_Compound_ExtreamItem::_read_packet(PacketBuf& packet)
{
    if (m_user == 0)
        return 0;

    _reset();

    // +0x04：目标槽
    packet.get_short(*reinterpret_cast<unsigned short*>(&m_field4));

    for (int i = 0; i < 2; ++i) {
        // +0x08 + i*2：材料槽位
        packet.get_short(
            *reinterpret_cast<unsigned short*>(&m_slots[i]));
        // [b28d-solo-DispLinkA1 P0] ORIG 无具名 slot 槽（值留 %edi 跨
        // GetInvenSlot 调用），实参内联直传 m_slots[i]。
        m_materials[i] =
            m_user->getCurCharacInvenR()->GetInvenSlot(1, m_slots[i]);
        if (!m_materials[i].isEquipableItemType())
            return 0;
        // find_item(m_materials[i].m_addInfo)（ORIG 读 this+i*0x3d+0xe int）；
        // [P211 档③] 字段为整型存储（异型 RHS）⇒ 复现 ORIG RHS-first+edx 中转。
        m_itemRefs[i] = reinterpret_cast<unsigned int>(
            G_CDataManager()->find_item(m_materials[i].m_addInfo));
    }

    // +0xcd：选择字节（≤2 合法）
    packet.get_byte(*reinterpret_cast<char*>(&m_select));
    if (m_select > 2)
        return 0;

    // 目标物品：槽 m_field4；按 item idx 判定 +0xd4 目标类别
    const Inven_Item target =
        m_user->getCurCharacInvenR()->GetInvenSlot(1, m_field4);
    if (target.isEmpty())
        return 0;
    if (target.m_addInfo == 0x4e3)
        m_targetKind = 0;
    else if (target.m_addInfo == 0x4e4)
        m_targetKind = 1;
    else if (target.m_addInfo == 0x28becf)
        m_targetKind = 3;
    else
        m_targetKind = 2;
    return 1;
}