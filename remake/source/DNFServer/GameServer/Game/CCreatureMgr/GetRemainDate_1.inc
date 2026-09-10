int CCreatureMgr::GetRemainDate(int uid, int nowSec) const
{
    if (m_tempExpireDelta != -1)        // +0x2c
        return m_tempExpireDelta;
    CCreatureItem* item = FindCreatureItem(uid);
    if (item == 0)
        return -1;
    return item->GetRemainDate(nowSec);
}
