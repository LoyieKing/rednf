// ============================================================================
// PvPSkillTreeParameterScript::getPvPSkillTree —— ORIG 0x08a5db5a（0x119 字节）
// 报告：docs/class_func_reports/PvPSkillTreeParameterScript/getPvPSkillTree.md
// key=(uchar job, uchar firstGrow, uchar secondGrow) 查 m_map0；命中后内层按
// grade 查找；命中时 *out = flag ? &value.m_vec0c(+0x0c) : &value.m_vec00(+0x00)
// 并返回 true，否则返回 false。
// ============================================================================
bool PvPSkillTreeParameterScript::getPvPSkillTree(
    int job, int firstGrow, int secondGrow, int grade, bool flag,
    std::vector<std::pair<int, int> >** out) const
{
    PvPSkillTreeKey key(static_cast<unsigned char>(job),
                        static_cast<unsigned char>(firstGrow),
                        static_cast<unsigned char>(secondGrow));

    std::map<PvPSkillTreeKey,
             std::map<int, PvPSkillTreeValue> >::const_iterator treeIt =
        m_map0.find(key);
    if (treeIt != m_map0.end())
    {
        std::map<int, PvPSkillTreeValue>::const_iterator gradeIt =
            treeIt->second.find(grade);
        if (gradeIt != treeIt->second.end())
        {
            if (flag)
                *out = const_cast<std::vector<std::pair<int, int> >*>(
                    &gradeIt->second.m_vec0c);
            else
                *out = const_cast<std::vector<std::pair<int, int> >*>(
                    &gradeIt->second.m_vec00);
            return true;
        }
    }
    return false;
}