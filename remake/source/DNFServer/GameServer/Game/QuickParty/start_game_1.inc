// ---- start_game @0x0826a972：带 key 版（先校验人数再出池）。----
bool CQuickPartySystemManager::start_game(CQuickParty* qp,
                                          const STQuickPartyPoolMap_Key& key)
{
    if (qp == 0)
        return false;
    if (!check_quick_party_min_count(key, qp->get_quick_party_count()))
        return false;
    if (!G_CGameManager()->GetQuickPartySystemManager()
             ->remove_quick_party_in_pool(qp, key))
        return false;
    start_game(qp);
    return true;
}