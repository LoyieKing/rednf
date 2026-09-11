// ORIG 0x085ca698（报告 GetStoreSeller.md 逐指令）：已开店则直取；否则从
// StaticPool(+0x68) 申请——池满退回 +0x04 嵌入 store（SetSeller 后返回 0），
// 成功则 Clear/SetParent/SetSeller 并登记卖家表，*out=store 返回 1。
int CPrivateStoreMgr::GetStoreSeller(ISeller** out, CUser* user)
{
    CPrivateStore* store;
    if ((store = FindStoreSeller(user)) == 0)
    {
        if ((store = m_storePool.Acquire()) == 0)
        {
            *out = &m_embeddedStore;
            m_embeddedStore.SetSeller(user);
            return 0;
        }
        store->Clear();
        store->SetParent(this);
        store->SetSeller(user);
        m_storeSellers[user] = store;
    }
    *out = store;
    return 1;
}