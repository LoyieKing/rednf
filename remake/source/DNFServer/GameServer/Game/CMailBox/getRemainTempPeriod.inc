// ORIG 0x0833d88c (_ZNK...18getRemainTempPeriodEii)
// docs/class_func_reports/user_creature__CCreatureMgr/getRemainTempPeriod.md：
// m_field30 有 key=uid：v<1 直接返回 v；否则 r=v-nowSec，r==0 时取 1。
// 无记录时委托装备宠的 GetRemainDate（BLOCKED: CCreatureItem 未建模 → -1，推断）。
int CCreatureMgr::getRemainTempPeriod(int uid, int nowSec) const
{
    std::map<int, int>::const_iterator it = m_field30.find(uid);
    if (it != m_field30.end())
    {
        if (it->second > 0)
        {
            int r = it->second - nowSec;
            if (r == 0)
                return 1;
            return r;
        }
        return it->second;
    }
    CCreatureItem* item = FindCreatureItem(uid);
    if (item == 0)
        return -1;
    // ORIG 委托 CCreatureItem::GetRemainDate(nowSec)。
    return item->GetRemainDate(nowSec);
}