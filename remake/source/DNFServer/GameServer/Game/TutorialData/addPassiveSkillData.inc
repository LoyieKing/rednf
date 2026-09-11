// ORIG _ZN12TutorialData19addPassiveSkillDataEiii @0x08ab8944：
// p1<4 门控；SkillData{+4:p3,+0:p2,+8:-1}（ORIG 写序）push 入 m_passiveSkills[p1]。
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
}