// ORIG _ZN12TutorialData13addSkillIndexEiiiii @0x08ab8888：
// p1<4 门控；SkillData{+4:p4,+0:p3,+8:p5}（ORIG 写序）；键=getConvertHotKey(p2)；
// 键为 -1 时丢弃（按 ORIG 不释放）；插入 m_skillMaps[p1]。
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
}