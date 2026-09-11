void CGameManager::PutQuickParty(QuickParty::CQuickParty* qp)
{
    // P212 头部声明使 find sret 缓冲对齐 ORIG -0x20；余差 key/eraseKey/erase-arg
    // 三槽号（4.4 标量分区 vs 4.1 生命周期穿插，N82附封顶，v1/v4(double pad)/v5/v7 探针穷举）。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    // ORIG 0x08295608。
    if (qp == 0)
        return;
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(qp->get_quick_party_index());
    if (it != m_quickPartyMap.end())
    {
        m_quickPartyMap.erase(qp->get_quick_party_index());
        m_quickPartyPool.Free(qp);
        qp->init();
    }
}