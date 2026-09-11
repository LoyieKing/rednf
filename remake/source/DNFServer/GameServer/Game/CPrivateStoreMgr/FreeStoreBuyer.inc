// ORIG 0x85ca914（报告 FreeStoreBuyer.md 逐指令）：买家表 find 命中则 erase，返回命中与否。
int CPrivateStoreMgr::FreeStoreBuyer(CUser* user)
{
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeBuyers.find(user);
    if (it != m_storeBuyers.end())
    {
        m_storeBuyers.erase(it);
        return 1;
    }
    return 0;
}