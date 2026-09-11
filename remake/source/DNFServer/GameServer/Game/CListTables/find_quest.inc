Quest* QuestList::find_quest(int idx)
{
    __gnu_cxx::hash_map<int, Quest*>::iterator it;
    it = m_questMap.find(idx);
    if (it != m_questMap.end())
    {
        return it->second;
    }
    return 0;
}