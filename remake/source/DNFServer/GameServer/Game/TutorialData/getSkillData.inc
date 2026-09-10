TutorialData::SkillData* TutorialData::getSkillData(int slot, int hotKey)
{
    std::map<int, SkillData*>::iterator it = m_skillMaps[slot].find(hotKey);
    if (it != m_skillMaps[slot].end()) {
        return it->second;
    }
    return 0;
