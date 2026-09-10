CQuickParty* CQuickPartySystemManager::pop_quick_party_in_pool(
    int& outCount, STQuickPartyPoolMap_Data& data, int slotIdx,
    int partyIdx, int firstMatchingCnt)
{
    if (slotIdx > 3 || slotIdx < 0)
        return 0;

    CQuickParty* qp = 0;
    if (slotIdx == 0)
    {
        qp = G_CGameManager()->GetQuickParty();
        if (qp == 0)
            return 0;
        qp->set_party_index(partyIdx);
        outCount = firstMatchingCnt;
        qp->set_first_matching_cnt(static_cast<char>(firstMatchingCnt));
        return qp;
    }
    else
    {
        qp = find_quick_party_in_pool(data.m_lists[slotIdx]);
        if (qp == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x352, 5);
            trace("QUICK_PARTY_LOG : NO INTENDED ROUTINE!!");
            qp = G_CGameManager()->GetQuickParty();
            if (qp == 0)
                return 0;
            qp->set_party_index(partyIdx);
            outCount = firstMatchingCnt;
            qp->set_first_matching_cnt(static_cast<char>(firstMatchingCnt));
        }
        else
        {
            qp->set_party_index(partyIdx);
            outCount = qp->get_quick_party_count();
        }
    }
    return qp;
