bool CAddItemCountDB::is_event_quest(int questId)
{
    // ORIG 0x810e0f4：size() 后按下标遍历 m_questIds（每次内联解引用脚本）。
    int count = static_cast<AddItemCountDBScript*>(m_script1c)->m_questIds.size();
    for (int i = 0; i < count; ++i)
    {
        if (static_cast<AddItemCountDBScript*>(m_script1c)->m_questIds[i] == questId)
            return 1;
    }
    return 0;
}