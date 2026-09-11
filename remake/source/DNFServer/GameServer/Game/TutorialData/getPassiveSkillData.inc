// ORIG _ZN12TutorialData19getPassiveSkillDataEi @0x08ab89a6（sret）：
// p1 越界返回空副本；命中返回成员向量副本。
std::vector<TutorialData::SkillData*> TutorialData::getPassiveSkillData(int slot)
{
    if (slot < 0 || slot > 3) {
        return std::vector<SkillData*>();
    }
    return m_passiveSkills[slot];
}