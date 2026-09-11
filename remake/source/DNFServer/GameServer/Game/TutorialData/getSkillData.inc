// ORIG _ZN12TutorialData12getSkillDataEii @0x08ab89fa：
// 用 p2 原值查 m_skillMaps[p1]（与插入的转换键不同口径，忠实转写）。
TutorialData::SkillData* TutorialData::getSkillData(int slot, int hotKey)
{
    std::map<int, SkillData*>::iterator it = m_skillMaps[slot].find(hotKey);
    if (it != m_skillMaps[slot].end()) {
        return it->second;
    }
    return 0;
}