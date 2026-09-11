// ---------------------------------------------------------------------------
// get_useJobFatigueMatchGift(char, int, int) —— ORIG T 0x0836580e
//   gift = m_useJobFatigue.m_giftMap.find((int)job)，未找到返回 0；
//   u1 = gift.m_rewardMap.upper_bound(arg2 - arg3)
//   u2 = gift.m_rewardMap.upper_bound(arg2)（asm -0x20+arg3 即还原 arg2）
//   若 u1==end 且 u2==end → 0；u1==u2 → 0；否则返回 u1->second。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：声明序 jobType→diff→it 使
//   diff(-0x20)/jobType(-0x1c)/it(-0x24) 全对齐；第二次 upper_bound 的 key 是
//   匿名临时 `diff + b`（非具名局部，ORIG -0x14 为引用实参物化）；
//   单出口 `if (it != end) { ... } return 0;` 复现共享尾块。ORIG 0x0836580e。
int CDataManager::get_useJobFatigueMatchGift(char job, int a, int b)
{
    int jobType;
    const int diff = a - b;
    jobType = (int)job;
    std::map<int, useCharacterJobFatigue::fatigueMatchGift>::iterator it =
        m_useJobFatigue.m_giftMap.find(jobType);
    if (it != m_useJobFatigue.m_giftMap.end())
    {
        std::map<int, int>::iterator u1 =
            it->second.m_rewardMap.upper_bound(diff);
        std::map<int, int>::iterator u2 =
            it->second.m_rewardMap.upper_bound(diff + b);
        if (u1 == it->second.m_rewardMap.end() &&
            u2 == it->second.m_rewardMap.end())
        {
            return 0;
        }
        if (u1 != u2)
        {
            return u1->second;
        }
    }
    return 0;
}