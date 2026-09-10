int CInventory::getPackedCodeArray1(unsigned long* out, int maxCount)
{
    // b28d-37: ORIG 四段循环各带具名 Inven_Item* 局部（P0：槽往返 -0x28/-0x20/-0x18/-0x10），
    // 且第 1..3 段循环变量双初始化（P191：`i = 0;` + `for (i = 0; ...)`），
    // 第 4 段为底部条件 for(i<=0xf1) + 循环后 return count。
    int count = 0;
    int i = 0;
    for (i = 0; i < 0x1a; i++)
    {
        Inven_Item* p = &m_invenItem[i];
        if (p->m_addInfo != 0)
        {
            unsigned long code = p->getPackedCode1();
            out[count] = code;
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
    i = 0;
    for (i = 0; i < 0x138; i++)
    {
        Inven_Item* p = &m_pEquipSlot[i];
        if (p->m_addInfo != 0)
        {
            unsigned long code = p->getPackedCode1();
            out[count] = code;
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
    i = 0;
    for (i = 0; i < 0x69; i++)
    {
        Inven_Item* p = &m_pAvatarSlot[i];
        if (p->m_addInfo != 0)
        {
            unsigned long code = p->getPackedCode1();
            out[count] = code;
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
    for (i = 0; i <= 0xf1; i++)
    {
        Inven_Item* p = &m_pCreatureSlot[i];
        if (p->m_addInfo != 0)
        {
            unsigned long code = p->getPackedCode1();
            out[count] = code;
            count++;
            if (maxCount <= count)
            {
                return count;
            }
        }
    }
    return count;
