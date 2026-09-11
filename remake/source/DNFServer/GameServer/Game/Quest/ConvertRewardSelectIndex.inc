// 单一短路表达式 + P226 语句表达式承载条件。**真语义修正**：ORIG 循环
// 正常结束返回 i（== size()）而非 0xffffffff（早退才返回 -1），已按 ORIG 照抄。
unsigned int Quest::ConvertRewardSelectIndex(const CUser* user, int index)
{
    if (!user)
    {
        return 0xffffffff;
    }
    if ((index == -1) || (index == 0xffff))
    {
        return 0xffffffff;
    }
    int i = 0;
    int count = 0;
    for (i = 0; i < (int)m_selectReward.size(); ++i)
    {
        if (({ (m_selectReward[i].m_job == 0xb) ||
               ((((const RewardItemInfoJobView*)&m_selectReward[i])->m_job ==
                 (ENUM_CHARACTERJOB)user->get_charac_job()) &&
                ((m_selectReward[i].m_growType == -1) ||
                 (m_selectReward[i].m_growType ==
                  (int)user->getCurCharFirstGrowType()))); }))
        {
            // b28d-solo-Quest [P47/P220]：ORIG 0x083538b8 `cmp -0xc(%ebp),%eax;
            // je +0x123`（je 直达函数尾 return 岛）⇒ 试 goto 尾标签形。
            if (index == count)
            {
                goto return_index;
            }
            ++count;
        }
    }
return_index:
    return i;
}