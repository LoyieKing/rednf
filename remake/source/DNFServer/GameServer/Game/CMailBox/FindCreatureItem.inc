// ORIG 0x08339292 (_ZNK...16FindCreatureItemEi)：m_creatureItems map 查找。
CCreatureItem* CCreatureMgr::FindCreatureItem(int uid) const
{
    std::map<int, CCreatureItem*>::const_iterator it = m_creatureItems.find(uid);
    if (it != m_creatureItems.end())
        return it->second;
    return 0;
}