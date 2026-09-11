int CInventory::checkToFix() const
{
    // ORIG 0x8509616：i 单槽 -0xc（两循环复用）；两条件拆双层 if——
    // checkStackableLimit 实参形为 get_add_info 先 call(0x850966a，结果留 %eax
    // 免槽)，随后独立二次寻址装载 m_addInfo(0x8509688)。
    int i;
    for (i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0 && m_pEquipSlot[i].m_field1 != 1)
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info()))
            {
                return 1;
            }
        }
    }
    for (i = 0x39; i <= 0xc8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info()))
            {
                return 1;
            }
        }
    }
    return 0;
}