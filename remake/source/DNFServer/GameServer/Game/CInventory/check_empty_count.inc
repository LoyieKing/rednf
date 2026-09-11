// 返回类型 bool（batch-13 Main 终裁，P22：调用点 test %al/xor$1 形均为 bool 产物）；
// 体内 return 1/0 即 return true/false（0/1 语义与全部调用点兼容）。
bool CInventory::check_empty_count(Inven_Item::ITEM_TYPE type, int count) const
{
    int empty = 0;
    int start;
    int end;
    StartEndPos(type, start, end);
    if (IsCreatureItemType(type))
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pCreatureSlot[i].m_addInfo == 0)
            {
                empty++;
            }
            if (empty >= count)
            {
                return 1;
            }
        }
        return 0;
    }
    else
    {
        for (int i = start; i <= end; i++)
        {
            if (m_pEquipSlot[i].m_addInfo == 0)
            {
                empty++;
            }
            if (empty >= count)
            {
                return 1;
            }
        }
        return 0;
    }
}