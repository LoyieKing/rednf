// ORIG T 0x08a9ebee：getStrikerSkillVector(first,0)；空指针返回 0；
// 迭代器遍历命中 m_field00==skillIndex 返回 1，否则 0（无 setne 规范化 → int）。
int StrikerScript::isCommonSkill(short firstGrowType, int skillIndex)
{
    std::vector<StrikerSkill>* vec =
        getStrikerSkillVector(firstGrowType, 0);
    if (vec == 0)
        return 0;
    for (std::vector<StrikerSkill>::iterator it = vec->begin();
         it != vec->end(); it++)
    {
        if (it->m_field00 == skillIndex)
            return 1;
    }
    return 0;
}