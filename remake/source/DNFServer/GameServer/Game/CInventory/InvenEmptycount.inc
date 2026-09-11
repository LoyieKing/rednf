int CInventory::InvenEmptycount(Inven_Item::ITEM_TYPE type) const
{
    int count = 0;
    int start;
    int end;
    StartEndPos(type, start, end);
    if (IsCreatureItemType(type))
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                count++;
            }
        }
        return count;
    }
    else
    {
        if (type == 8)
        {
            for (int i = 0; i < 0x69; i++)
            {
                if (m_pAvatarSlot[i].m_addInfo == 0)
                {
                    count++;
                }
            }
            // ORIG 0x8505132 +0xb2: avatar 循环耗尽后本块自带 mov -0x18,%eax
            // return（4.4 合并到公共尾会少一条）。
            return count;
        }
        else
        {
            for (int i = start; i <= end; i++)
            {
                if (m_pEquipSlot[i].m_addInfo == 0)
                {
                    count++;
                }
            }
        }
        return count;
    }
}