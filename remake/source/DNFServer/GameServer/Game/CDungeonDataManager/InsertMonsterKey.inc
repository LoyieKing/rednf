int CDungeonDataManager::InsertMonsterKey(int monsterIndex)
{
    for (std::map<DungeonKey, CDungeonNode>::iterator it = m_dungeonMap.begin();
         it != m_dungeonMap.end(); it++)
    {
        CDungeonNode* node = &it->second;
        node->InsertMonsterKey(monsterIndex);
    }
    return 1;
