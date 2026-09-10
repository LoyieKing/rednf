void CInventory::CheckNFix()
{
    // ORIG 0x8509738 起：i 单槽 -0xc；与 checkToFix 同拆双层 if（get_add_info
    // 先 call 留 %eax、二次独立寻址装 m_addInfo）。
    int i;
    for (i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0 && m_pEquipSlot[i].m_field1 != 1)
        {
            if (!checkStackableLimit((unsigned long)m_pEquipSlot[i].m_addInfo,
                                     m_pEquipSlot[i].get_add_info()))
            {
                m_pEquipSlot[i].m_addInfo2 = 1;
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
                m_pEquipSlot[i].m_addInfo2 = 1;
            }
        }
    }
