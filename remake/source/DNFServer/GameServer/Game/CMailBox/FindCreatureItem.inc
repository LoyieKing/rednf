CCreatureItem* CCreatureMgr::FindCreatureItem(int uid) const
{
    std::map<int, CCreatureItem*>::const_iterator it = m_creatureItems.find(uid);
    if (it != m_creatureItems.end())
        return it->second;
    return 0;
