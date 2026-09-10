int CDungeonDataManager::InsertSkillKey(int skillIndex, int job)
{
    for (std::map<DungeonKey, CDungeonNode>::iterator it = m_dungeonMap.begin();
         it != m_dungeonMap.end(); it++)
    {
        CDungeonNode* node = &it->second;
        const DungeonKey& key = it->first;
        node->InsertSkillKey(skillIndex, job,
                             key.m_dungeonIndex, key.m_dungeonDiff);
    }
    return 1;
