bool PvPSkillTreeParameterScript::getGiveSkill(
    int job, int firstGrow, int secondGrow,
    std::vector<std::pair<int, int> >& out) const
{
    // ORIG 0x08a5e178：m_map30[(job,firstGrow)] 命中时 out = value.m_giveSkills(+0x34)，返回 true
    // ORIG 0x8a5e178 首条 `movl $0x0,0x14(%ebp)` + 回读 `movzbl` ⇒ 形参槽清零后
    // 参与 key 构造（非常量内联）。
    secondGrow = 0;
    PvPSkillTreeKey key(static_cast<unsigned char>(job),
                        static_cast<unsigned char>(firstGrow),
                        static_cast<unsigned char>(secondGrow));
    std::map<PvPSkillTreeKey,
             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::const_iterator it =
        m_map30.find(key);
    if (it != m_map30.end())
    {
        out = it->second.m_giveSkills;
        return true;
    }
    return false;
}