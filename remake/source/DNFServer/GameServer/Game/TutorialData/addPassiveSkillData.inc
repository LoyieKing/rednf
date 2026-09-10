void TutorialData::addPassiveSkillData(int slot, int skillIndex, int field4)
{
    if (slot >= 4) {
        return;
    }
    SkillData* skill = new SkillData;
    skill->m_field4 = field4;
    skill->m_field0 = skillIndex;
    skill->m_field8 = -1;
    m_passiveSkills[slot].push_back(skill);
