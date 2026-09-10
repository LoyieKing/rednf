int GetItemIdByAddInfo(Inven_Item* items, int count, int addInfo)
{
    for (int i = 0; i < count; ++i)
    {
        // P183：ORIG 每次访问重发 imul $0x3d 全链（无 it 指针槽，P0）。
        if (items[i].m_addInfo2 == addInfo && items[i].m_addInfo2 != 0)
            return items[i].m_addInfo;
    }
    return 0;
