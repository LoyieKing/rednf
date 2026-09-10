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
                              "quick party index(%d) already alloc",
                              qp->get_quick_party_index());
    }
    return qp;
