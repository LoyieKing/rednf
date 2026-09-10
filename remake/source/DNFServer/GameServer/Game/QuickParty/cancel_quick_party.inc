bool CQuickPartySystemManager::cancel_quick_party(CParty* party, bool flag, CUser* user)
{
    // [b28d-L1] 修法（P167）：ORIG 37 calls 三处 get_quick_party_index（无
    // qpIndex 具名局部）：①短路条件 `party==0 || party->get_quick_party_index()==-1`
    // ②日志三元 ③`GetQuickParty(party->get_quick_party_index())` 内联实参。
    // 原形用 qpIndex 缓存使 4.4 少一次调用（P167 报 MISSING×1）+ 多一槽。
    if (party == 0 || party->get_quick_party_index() == -1) {
        if (party == 0 && user != 0) {
            LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x1e3,
                                  "QUICK_PARTY_LOG : PARTY IS NULL!! (%d)",
                                  reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacNo());  // ORIG: CUserCharacInfo::getCurCharacNo((CUserCharacInfo*)user)
        } else {
            LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x1e5,
                                  "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY INDEX (%d)!!",
                                  party->get_quick_party_index());
        }
        return 0;
    }

    CQuickParty* qp =
        G_CGameManager()->GetQuickParty(party->get_quick_party_index());
    if (qp == 0) {
        LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x1ec,
                              "QUICK_PARTY_LOG : NO EXIST QUICK PARTY!!");
        return 0;
    }

    if (party->GetEPLPState() > 1 && flag) {
        party->SetSelectedEPLPCmd((char)-1);            // ORIG 写 -1（0x826a4db）
        // CParty+0xb24 内嵌 CBattle_Field（局部视图 CPartyBattleFieldView）；
        // P48/P183：ORIG 两次独立 `mov party,%edx; add $0xb24,%edx`（无 bf 缓存），
        // getRandomBuffDungeonType 结果直传 setEplpWaitingTimer（无 buffType 槽）。
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.setEplpWaitingTimer(
            (RandomBuffDungeon)
                reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.getRandomBuffDungeonType(
                    qp->get_dungeon_index()));
    }

    STQuickPartyPoolMap_Key key;
    key.m_dungeonIndex = qp->get_dungeon_index();
    key.m_dungeonDiff = qp->get_dungeon_diff();

    if (!remove_quick_party_in_pool(qp, key))
        return 0;

    if (!flag)
        qp->set_change_quick_party_count(true);

    int count = 0;                                    // ORIG 0x826a586 预清
    if (party->get_member_count() == 1 || flag) {
        qp->delete_party_index(party->GetPartyIndex());
        count = qp->get_quick_party_count();
        party->init_quick_party_data();
        if (count <= 0) {
            qp->destroy();
            return 1;
        }
    } else {
        count = qp->get_quick_party_count() - 1;
        if (count <= 0) {
            LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x216,
                                  "QUICK_PARTY_LOG : QUICK PARTY COUNT ERROR!! (%d)", count);
            qp->delete_party_index(party->GetPartyIndex());
            party->init_quick_party_data();
            qp->destroy();
            return 0;
        }
    }

    std::map<STQuickPartyPoolMap_Key, STQuickPartyPoolMap_Data>::iterator it =
        m_poolMap.find(key);
    if (it != m_poolMap.end()) {
        if (!push_quick_party_in_pool(key, it->second, 0, count, qp, true)) {
            LogManager::logFormat(1, "QuickParty.cpp", __PRETTY_FUNCTION__, 0x229,
                                  "QUICK_PARTY_LOG : PUSH ERROR!! (%d)", count);
            return 0;
        }
    }
    return 1;
