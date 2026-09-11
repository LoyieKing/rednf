bool CAddItemCountDB::is_event_item(int itemId)
{
    // ORIG 0x810e08e：size() 后按下标遍历 m_itemIdxs（每次内联解引用脚本）。
    int count = static_cast<AddItemCountDBScript*>(m_script1c)->m_itemIdxs.size();
    for (int i = 0; i < count; ++i)
    {
        if (static_cast<AddItemCountDBScript*>(m_script1c)->m_itemIdxs[i] == itemId)
            return 1;
    }
    return 0;
}