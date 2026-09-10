std::vector<TutorialData::SkillData*> TutorialData::getPassiveSkillData(int slot)
{
    if (slot < 0 || slot > 3) {
        return std::vector<SkillData*>();
    }
    return m_passiveSkills[slot];
