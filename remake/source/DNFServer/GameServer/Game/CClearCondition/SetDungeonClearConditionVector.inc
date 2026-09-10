void CClearCondition::SetDungeonClearConditionVector(
    std::vector<DungeonClearCondition>& vec)
{
    // ORIG 0x82fef12：Clear 后若 vec 非空，+0x14 存 vec 地址，并对每个
    // 条目：m_flag(+0x00) += 条目 m_8 字段，同时向 m_vec push 一个 0。
    Clear();
    if (!vec.empty())
    {
        m_pCondVec = &vec;
        for (std::vector<DungeonClearCondition>::iterator it = vec.begin();
             it != vec.end(); ++it)
        {
            m_flag += it->m_8;
            m_vec.push_back(0);
        }
    }
