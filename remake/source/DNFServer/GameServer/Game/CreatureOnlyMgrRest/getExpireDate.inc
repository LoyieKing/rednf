int CCreatureMgr::getExpireDate(int uid) const
{
    CCreatureItem* item = FindCreatureItem(uid);
    if (item == 0)
        return -1;
    return item->getExpireTime();
