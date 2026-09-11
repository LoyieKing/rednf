// ORIG 0x085ca358（报告 FindStoreSeller.md 逐指令）：卖家表(+0x98) find，命中返 mapped。
CPrivateStore* CPrivateStoreMgr::FindStoreSeller(CUser* user)
{
    if (!user)
        return 0;
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeSellers.find(user);
    if (it != m_storeSellers.end())
        return it->second;
    return 0;
}