// ORIG 0x85ca874（报告 FreeStoreSeller.md 逐指令）：卖家表 find 命中则先经
// StaticPool(+0x68)::Free 归还 store 对象再 erase 表项，返回命中与否。
int CPrivateStoreMgr::FreeStoreSeller(CUser* user)
{
    std::map<CUser*, CPrivateStore*>::iterator it = m_storeSellers.find(user);
    if (it != m_storeSellers.end())
    {
        m_storePool.Free(it->second);
        m_storeSellers.erase(it);
        return 1;
    }
    return 0;
}