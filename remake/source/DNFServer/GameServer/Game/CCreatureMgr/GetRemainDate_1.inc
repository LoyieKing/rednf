// ORIG CCreatureMgr::GetRemainDate(int, int) const @0x833d78e：
//   m_tempExpireDelta（+0x2c）!= -1 → 直接返回它；
//   否则 FindCreatureItem(uid)：空 → -1；否则 → item->GetRemainDate(nowSec)。
int CCreatureMgr::GetRemainDate(int uid, int nowSec) const
{
    if (m_tempExpireDelta != -1)        // +0x2c
        return m_tempExpireDelta;
    CCreatureItem* item = FindCreatureItem(uid);
    if (item == 0)
        return -1;
    return item->GetRemainDate(nowSec);
}
