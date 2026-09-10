bool CLevelUpReward::check_recv_level(int level, int job, int growType, bool flag)   // [b28d-Main 代落·S366 P292] int→bool
{
    std::map<int, Condition_LevelUp>::iterator it =
        static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.find(job);
    if (it != static_cast<LevelUpRewardScript*>(m_script1c)->m_conditions.end())
    {
        // value 基址 +5 字节过滤：先零扩展比 0xff（通配），再有符号扩展比 growType。
        if (it->second.m_filter1 == static_cast<unsigned char>(0xff)
            || static_cast<int>(static_cast<signed char>(it->second.m_filter1))
               == growType)
        {
            for (int i = 0; static_cast<int>(it->second.m_items.size()) > i; ++i)
            {
                if (flag)
                {
                    if (it->second.m_items[i].m_level <= level)   // setle 分支
                        return 1;
                }
                else if (it->second.m_items[i].m_level == level)  // sete 分支
                {
                    return 1;
                }
            }
        }
    }
    return 0;
