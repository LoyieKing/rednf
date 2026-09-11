// [b28d-solo-CListTables N82 NEAR] ORIG 0x08351c1e：138=138 + p167 17/17 CLEAR
// （== / mmEi / ppEi 全对齐），残差仅槽号整体深移 0x10（ORIG it/range -0x4c/-0x48
// vs 我们 -0x3c/-0x38）；char[0x10] 死局部撑帧实测 sub 0x58→0x78 且槽号不平移 ⇒ 封顶。
int CSkillList::get_ran_groupskill_index(int jobType, int group, int skillIdx)
{
    // [b28d-solo-CListTables N(N82): 138=138 + p167 调用点 CLEAR（17/17，== / mmEi /
    // ppEi 全对齐），残差仅槽号整体深移 0x10（ORIG it/range -0x4c/-0x48 vs 我们
    // -0x3c/-0x38）；char[0x10] 死局部撑帧实测 sub 0x58→0x78 且槽号不平移 ⇒ 封顶。]
    // [K2 2026-09-11 复核·实验负结论] P190 双取址 double 撑帧实测：帧 0x58→0x78（+0x20
    // 过头，ORIG 0x68）、且 pad 落**深侧**（低地址），浅侧 it/range 槽仍 -0x30（ORIG -0x40）
    // **不平移** ⇒ 撑帧无法移动浅侧槽位；ndiff 42→16 但 verdict 仍 NEAR。
    // ⇒ N126（4.1/4.4 首槽基址常数）/N82 封顶，勿再试 pad。
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
    // [PoolB b28f · PoolE 旋钮实验·负结论] 曾把指针局部 `CSkill* skill` 改
    // `unsigned int skillAddr=(unsigned int)it->second` 承载地址（Main 广播：
    // 指针/整型分区旋钮）⇒ 实测**残余槽图不变**（ORIG -0x40/-0x48/-0x4c vs 我方
    // -0x30/-0x38/-0x3c 的整组 -0x10 平移仍在，verdict 仍 NEAR 138/139）
    // ⇒ 本件残余为 N126 版本首槽基址常数（4.1 首槽 -0x4 / 4.4 -0xc 族），
    // 非指针分区所致，已回退。
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
}