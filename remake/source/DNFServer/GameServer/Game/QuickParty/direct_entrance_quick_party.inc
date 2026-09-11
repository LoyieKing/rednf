// ---- direct_entrance_quick_party @0x0826a762：队长请求直接入场 ----
int CQuickPartySystemManager::direct_entrance_quick_party(CUser* user)
{
    if (user == 0)
        return 1;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
        return 1;

    CQuickParty* qp =
        G_CGameManager()->GetQuickParty(party->get_quick_party_index());
    if (qp == 0)
        return 1;

    int count = qp->get_quick_party_count();
    // [b23S3C] ORIG 0x0826a762 `call get_first_matching_cnt` 出现两次（+0xb4/+0xd8，
    // 均紧跟 `mov -0x14(%ebp),%eax` 重取 qp），无 firstMatchingCnt 具名槽（P0/P48）
    // ⇒ 删缓存局部与 raw 中转块，改每用点重取 + (int) 直比 count。
    // 队伍未满且正好等于首次匹配人数：直接拒绝入场。
    if (count < 2 && static_cast<int>(qp->get_first_matching_cnt()) == count)
        return 0xf5;

    // 已到匹配目标人数且无人数变动：解除队列并通知散队。
    if (static_cast<int>(qp->get_first_matching_cnt()) >= count &&
        !qp->is_change_quick_party_count())
    {
        CParty* target = qp->get_party_object();
        if (target == 0)
            return 1;
        if (G_CGameManager()->GetQuickPartySystemManager()->cancel_quick_party(
                target, true, 0))
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(0, 0x17f);
            packet.finalize(true);
            target->send_to_party(packet);
            return 1;
        }
    }

    STQuickPartyPoolMap_Key key;
    key.m_dungeonIndex = qp->get_dungeon_index();
    key.m_dungeonDiff = static_cast<unsigned char>(qp->get_dungeon_diff());
    G_CGameManager()->GetQuickPartySystemManager()->start_game(qp, key);
    return 0;
}