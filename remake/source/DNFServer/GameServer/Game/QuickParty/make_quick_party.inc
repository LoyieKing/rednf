CParty* CQuickPartySystemManager::make_quick_party(CQuickParty* qp)
{
    if (qp == 0)
        return 0;

    // [b28d-solo-QuickParty] 修法（P0/声明序）：ORIG 0x0826b22a 先 `cmpl $0x0,0xc(%ebp);
    // jne` 再构造 detached（0x0826b23a 后）⇒ 原码 detached 声明在 qp 判空之后；提前
    // 声明使 4.4 把 detached 排深 4 并整体平移全帧槽号。
    std::list<CUser*> detached;

    // 选 host：优先 EPLP 等待态>1 的队伍，否则任一 quick party 属性队伍。
    CParty* member;
    CParty* host = 0;
    for (int i = 0; i < 4; ++i)
    {
        int partyIdx = qp->get_party_index(i);
        member = G_CGameManager()->GetParty(partyIdx);
        if (member == 0)
            continue;
        if (member->GetEPLPState() > 1)
        {
            host = member;
            break;
        }
        if (member->is_quick_party())
            host = member;
    }
    if (host == 0)
    {
        for (int i = 0; i < 4; ++i)
        {
            int partyIdx = qp->get_party_index(i);
            host = G_CGameManager()->GetParty(partyIdx);
            if (host != 0)
                break;
        }
        if (host == 0)
        {
            qp->destroy();
            LogManager::logFormat(
                1, "QuickParty.cpp",
                __PRETTY_FUNCTION__,
                0x3d0, "QUICK_PARTY_LOG : PARTY IS NULL!!");
            return 0;
        }
    }

    host->set_state(1);
    host->set_quick_party(true);
    host->setDungIndex(1);
    host->setDungDiffi(0);
    if (host->IsAutoCreated())
    {
        host->SetSinglePlay(false);
        host->SetAutoCreated(false);
        G_GameWorld()->send_party_info_to_all(host, 0);
    }

    // 其余成员队伍解散，把用户依次收拢（记录进 detached）。
    for (int i = 0; i < 4; ++i)
    {
        int partyIdx = qp->get_party_index(i);
        if (partyIdx == -1)
            continue;
        CParty* member = 0;
        member = G_CGameManager()->GetParty(partyIdx);
        if (member == 0 || member == host)
            continue;
        for (int slot = 0; slot < 4; ++slot)
        {
            if (!member->checkValidUser(slot))
                continue;
            member->delete_party_for_quick_party(member->get_user(slot),
                                                 detached);
        }
    }

    std::list<CUser*>::iterator it = detached.begin();
    std::list<CUser*>::iterator itEnd = detached.end();
    for (; it != itEnd; ++it)
    {
        host->join_user(*it);
    }

    qp->destroy();
    host->init_quick_party_data();
    return host;
