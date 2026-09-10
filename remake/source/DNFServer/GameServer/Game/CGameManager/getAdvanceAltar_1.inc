advancealtar::StageControl* CGameManager::getAdvanceAltar(int index)
{
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(index);
    if (it == m_stageMap.end())
        return 0;
    return it->second;
