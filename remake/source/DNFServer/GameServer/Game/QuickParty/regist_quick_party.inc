bool CQuickPartySystemManager::regist_quick_party(
    const STQuickPartyPoolMap_Key& key, STQuickPartyPoolMap_Data& data,
    int slotIdx, CParty* party, int firstMatchingCnt)
{
    if (party == 0)
        return false;

    int outCount = 0;
    CQuickParty* qp = pop_quick_party_in_pool(outCount, data, slotIdx,
                                              party->GetPartyIndex(),
                                              firstMatchingCnt);
    if (qp == 0)
        return false;
    // P197-N-cap：push_quick_party_in_pool 返 bool，`==1` 被 fold（无物化）；改 exec 返回类型属越界，记 N-cap。
    if (!push_quick_party_in_pool(key, data, party, outCount, qp, true))
        return false;
    return true;
