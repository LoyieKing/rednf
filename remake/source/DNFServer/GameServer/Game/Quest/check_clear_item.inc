bool Quest::check_clear_item(int itemIdx, int dungeonIdx) const
{
    if (m_clearItems.size() != 0)
    {
        DungeonClearItem item;
        for (int i = 0; i < (int)m_clearItems.size(); ++i)
        {
            item = m_clearItems.at(i);
            if (item.m_dungeonIdx == -1)
            {
                if (item.m_itemIdx == itemIdx)
                {
                    return 1;
                }
            }
            else if ((item.m_itemIdx == itemIdx) &&
                     (item.m_dungeonIdx == dungeonIdx))
            {
                return 1;
            }
        }
    }
    return 0;
