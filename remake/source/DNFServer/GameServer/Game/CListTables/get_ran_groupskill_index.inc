int CSkillList::get_ran_groupskill_index(int jobType, int group, int skillIdx)
{
    // [b28d-solo-CListTables N(N82): 138=138 + p167 调用点 CLEAR（17/17，== / mmEi /
    // ppEi 全对齐），残差仅槽号整体深移 0x10（ORIG it/range -0x4c/-0x48 vs 我们
    // -0x3c/-0x38）；char[0x10] 死局部撑帧实测 sub 0x58→0x78 且槽号不平移 ⇒ 封顶。]
    int count = m_groupSkillMap[jobType].count(group);
    if (count == 0)
    {
        return -1;
    }
    int rand = get_rand_int(count);
    // [b28d-solo-CListTables] ORIG 0x08351c1e：range 声明为 const_iterator 对
    // ⇒ equal_range 结果走 pair 转换构造（ORIG call ...C1ISt17_Rb_tree_iterator…）；
    // for 内后置 it++（ppEi 带临时槽）。
    std::pair<std::multimap<int, CSkill*>::const_iterator,
              std::multimap<int, CSkill*>::const_iterator> range =
        m_groupSkillMap[jobType].equal_range(group);
    std::multimap<int, CSkill*>::const_iterator it = range.first;
    for (int i = 0; i < rand; ++i)
    {
        it++;
    }
    CSkill* skill = it->second;
    if (skill->get_index() == skillIdx)
    {
        // [b28d-solo-CListTables] ORIG 0x08351cea：== 正极性（ORIG 调
        // operator==，非 !=）+ then 为 it-- 分支 + 两处后置 it--（mmEi）。
        it++;
        if (it == range.second)
        {
            it--;
            if (it == range.first)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0xfde, 5);
                trace("[%s][%d]_( i == res.first )",
                      __PRETTY_FUNCTION__,
                      0xfde);
            }
            else
            {
                it--;
                skill = it->second;
            }
        }
        else
        {
            skill = it->second;
        }
    }
    return skill->get_index();
