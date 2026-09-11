// ORIG 0x8580f28：m_moodChanges（NpcScript+0x180 处 pair<MOOD,uint> vector）
//   轮转：空表时 mood=1/count=0 返回 0；否则 ++count 后越界回绕，
//   输出 at(count) 的 first/second。
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
}