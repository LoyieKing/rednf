// ORIG T 0x08a9e97a：外层 i=0..1（setle 有符号判续）；i==0 用
// getStrikerSkillVector(first,second)，否则 (first,0)；空返回 0；
// 迭代器遍历（begin/end/operator->/后置 it++）命中 m_field00==skillIndex
// 返回 m_field04（+4）。
int StrikerScript::getComboIndex(short firstGrowType, short secondGrowType,
                                 int skillIndex)
{
    std::vector<StrikerSkill>* vec = 0;
    for (int i = 0; i <= 1; ++i)
    {
        if (i == 0)
            vec = getStrikerSkillVector(firstGrowType, secondGrowType);
        else
            vec = getStrikerSkillVector(firstGrowType, 0);
        if (vec == 0)
            return 0;
        for (std::vector<StrikerSkill>::iterator it = vec->begin();
             it != vec->end(); it++)
        {
            if (it->m_field00 == skillIndex)
                return it->m_field04;
        }
    }
    return 0;
}