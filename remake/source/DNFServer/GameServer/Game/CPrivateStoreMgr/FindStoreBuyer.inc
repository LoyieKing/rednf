// ORIG 0x085ca2da（报告 FindStoreBuyer.md 逐指令）：买家表(+0xb0) find，命中返 mapped。
CPrivateStore* CPrivateStoreMgr::FindStoreBuyer(CUser* user)
{
    if (!user)
        return 0;
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeBuyers.find(user);
    if (it != m_storeBuyers.end())
        return it->second;
    return 0;
}