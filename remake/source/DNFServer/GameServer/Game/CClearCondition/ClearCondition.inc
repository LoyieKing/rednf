bool CClearCondition::ClearCondition(ENUM_DUNGEON_CLEAR_CONDITION_TYPE type, int idx)
{
    if ((int)type < 0 || (int)type > 4)
    {
        return false;
    }
    // ORIG 0x82fefce：+0x14 指向 vector<DungeonClearCondition>（外部条件表），
    // 与 m_vec 同步遍历；条目 m_0/m_4 匹配 type/idx 时，若 m_8 > 当前计数
    // 则计数自增并 m_count++，否则返回 false；结尾 (m_flag <= m_count)。
    std::vector<DungeonClearCondition>::iterator itd = m_pCondVec->begin();
    std::vector<DungeonClearCondition>::iterator endd = m_pCondVec->end();
    std::vector<int>::iterator itv = m_vec.begin();
    std::vector<int>::iterator endv = m_vec.end();
    while (itd != endd && itv != endv)
    {
        DungeonClearCondition* d = &*itd;
        int* v = &*itv;
        // [实测回退] 操作数序交换（(int)type == d->m_0）零效（d=10 不变，反引入
        // `mov -0x10(%ebp),%eax` 新装载）⇒ 维持原序；残余为 N82 块排布族。
        if (d->m_0 == (int)type && d->m_4 == idx)
        {
            if (d->m_8 > *v)
            {
                ++(*v);
                ++m_count;
            }
            else
            {
                return false;
            }
        }
        ++itd;
        ++itv;
    }
    if (m_flag > m_count)
    {
        return false;
    }
    return true;
