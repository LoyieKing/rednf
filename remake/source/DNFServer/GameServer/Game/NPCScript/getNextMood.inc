int CNPCScript::getNextMood(ENUM_NPC_MOOD& mood, unsigned int& count) const
{
    if (m_npcScript.m_favorableRelationShip.m_fav.m_moodChanges.empty())
    {
        mood = ENUM_NPC_MOOD_NORMAL;
        count = 0;
        return 0;
    }
    ++count;
    if (m_npcScript.m_favorableRelationShip.m_fav.m_moodChanges.size() <= count)
    {
        count = 0;
    }
    mood = m_npcScript.m_favorableRelationShip.m_fav.m_moodChanges.at(count).first;
    return m_npcScript.m_favorableRelationShip.m_fav.m_moodChanges.at(count).second;
