void CInventory::SetInventoryMemory(InventoryMemory* mem)
{
    // InventoryMemory 布局（CUser.h 权威，ctor 0x822d3b8 四段构造实证）：
    // m_items1[312]@+0x0000、m_items2[105]@+0x4a58、m_pCreatureSlot 段 m_items3[242]@+0x635d。
    m_pEquipSlot = mem->m_items1;
    m_pAvatarSlot = mem->m_items2;
    m_pCreatureSlot = mem->m_items3;
}