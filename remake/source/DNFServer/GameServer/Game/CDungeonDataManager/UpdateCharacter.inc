bool CDungeonDataManager::UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<DungeonKey, CDungeonNode>::iterator it =
        m_dungeonMap.find(*reinterpret_cast<const DungeonKey*>(&stat->m_key));
    if (it == m_dungeonMap.end())
        return false;
    CDungeonNode* node = &it->second;
    node->UpdateCharacter(stat);
    return true;
}