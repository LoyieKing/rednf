// ============================================================================
// 快速队伍
// ============================================================================
QuickParty::CQuickParty* CGameManager::GetQuickParty()
{
    QuickParty::CQuickParty* qp = m_quickPartyPool.Acquire();
    if (qp == 0)
        return 0;
    qp->set_quick_party_index(m_quickPartyPool.GetIndex(qp));
    int qpIndex = qp->get_quick_party_index();
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(qpIndex);
    if (it == m_quickPartyMap.end())
    {
        m_quickPartyMap[qp->get_quick_party_index()] = qp;
        return qp;
    }
    else
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xb64,
                              // [K9GameRestA-S322 · §6.5.1] ORIG 0x08295450：实串 @0x8c16bf8
                              // = "QUICK_PARTY_LOG : " + EUC-KR(이미 존재하는 QUICK 파티 할당: %d)。
                              // [CeraShop12Run 逐字节复核补正] 末尾韩文段是 `할당 요청`（含空格 +
                              // \xbf\xe4\xc3\xbb），原稿漏 5 字节。
                              "QUICK_PARTY_LOG : \xc0\xcc\xb9\xcc \xc1\xb8\xc0\xe7\xc7\xcf\xb4\xc2 QUICK \xc6\xc4\xc6\xbc \xc7\xd2\xb4\xe7 \xbf\xe4\xc3\xbb: %d",
                              qp->get_quick_party_index());
    }
    return qp;
}