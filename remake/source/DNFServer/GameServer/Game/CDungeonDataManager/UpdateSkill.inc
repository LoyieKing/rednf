bool CDungeonDataManager::UpdateSkill(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<DungeonKey, CDungeonNode>::iterator it =
        m_dungeonMap.find(((SigDungeonStatisticKeyView*)stat)->m_key);
    if (it == m_dungeonMap.end())
        return false;
    CDungeonNode* node = &it->second;
    node->UpdateSkill(stat);
    return true;
}