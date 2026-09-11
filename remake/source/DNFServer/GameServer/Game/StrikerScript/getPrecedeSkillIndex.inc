// ORIG T 0x08a9ea96：getStrikerSkillVector(first,second)；空指针返回 0；
// 循环 operator[] 命中 m_field00==skillIndex 返回 m_field08（+8），未命中 0。
// 循环变量 unsigned（Ghidra uint local_10；ORIG seta 无符号判续）；
// vec 先置 0 再赋值（ORIG call 前 movl $0,-0x10 实证）。
int StrikerScript::getPrecedeSkillIndex(short firstGrowType, short secondGrowType,
                                        int skillIndex)
{
    std::vector<StrikerSkill>* vec = 0;
    vec = getStrikerSkillVector(firstGrowType, secondGrowType);
    if (vec == 0)
        return 0;
    for (unsigned int i = 0; i < vec->size(); ++i)
    {
        if ((*vec)[i].m_field00 == skillIndex)
            return (*vec)[i].m_field08;
    }
    return 0;
}