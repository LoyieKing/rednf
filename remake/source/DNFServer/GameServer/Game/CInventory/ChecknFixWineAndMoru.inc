void CInventory::ChecknFixWineAndMoru()
{
    // ORIG 0x8509886：i 单槽 -0xc；两循环均含 IsOverStackableItem 的 uchar
    // 实参双零扩展形（movzbl 0x1,%eax; movzbl %al,%edx → 0x4(%esp)）。
    char pad[8];
    int i;
    int d1;
    int d2;
    for (i = 3; i <= 8; i++)
    {
        // ORIG 0x85098a7..+0xa4：四级平铺 je/jne 同跳 INC(+0xbf)，无 bool 物化。
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            if (m_pEquipSlot[i].m_field1 != 1)
            {
                if (!IsOverStackableItem(m_pEquipSlot[i].m_addInfo,
                                         (unsigned char)m_pEquipSlot[i].m_field1))
                {
                    if (m_pEquipSlot[i].m_addInfo2 > 799)
                    {
                        m_pEquipSlot[i].m_addInfo2 = 1;
                    }
                }
            }
        }
    }
    for (i = 0x39; i <= 0xc8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            if (!IsOverStackableItem(m_pEquipSlot[i].m_addInfo,
                                     (unsigned char)m_pEquipSlot[i].m_field1))
            {
                if (m_pEquipSlot[i].m_addInfo2 > 799)
                {
                    m_pEquipSlot[i].m_addInfo2 = 1;
                }
            }
        }
    }
    (void)pad;
    (void)d1;
    (void)d2;
