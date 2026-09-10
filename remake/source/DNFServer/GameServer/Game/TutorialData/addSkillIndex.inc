void TutorialData::addSkillIndex(int slot, int hotKey,
                                 int skillIndex, int field4, int field8)
{
    if (slot > 3) {
        return;
    }

    SkillData* skill = new SkillData;
    skill->m_field4 = field4;
    skill->m_field0 = skillIndex;
    skill->m_field8 = field8;

    int convertedKey = getConvertHotKey(hotKey);
    if (convertedKey == -1) {
        return;
    }

    m_skillMaps[slot].insert(
        std::make_pair(convertedKey, skill));
