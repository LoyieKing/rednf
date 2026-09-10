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
