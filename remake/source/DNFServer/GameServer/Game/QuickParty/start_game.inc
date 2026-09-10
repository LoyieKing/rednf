void CQuickPartySystemManager::start_game(CQuickParty* qp)
{
    if (qp == 0)
    {
        LogManager::logFormat(
            1, "QuickParty.cpp",
            __PRETTY_FUNCTION__,
            0x40c, "QUICK_PARTY_LOG : QUICK PARTY IS NULL!!");
        return;
    }
    if (qp->get_quick_party_count() <= 1)
    {
        LogManager::logFormat(
            1, "QuickParty.cpp",
            __PRETTY_FUNCTION__,
            0x412,
            "QUICK_PARTY_LOG : NO START!! BECAUSE OF QUICK PARTY COUNT 1 !!");
        return;
    }

    CParty* merged = 0;
    merged = make_quick_party(qp);
    if (merged == 0)
    {
        LogManager::logFormat(
            1, "QuickParty.cpp",
            __PRETTY_FUNCTION__,
            0x41e, "QUICK_PARTY_LOG : NO EXIST PARTY!!");
        return;
    }

    insert_timer_waiting_start_game(merged);
    merged->send_party_realtime_info();
    merged->send_party_ipinfo();
    G_GameWorld()->send_party_info_to_all(merged, 2);
    merged->send_quick_party_matching_result(true, 0);
    // b28c-lane3 判 N：ORIG 0x0826b6d1 `setle;test;je 826b6ef`（nop 岛）+
    // 0x0826b6ed `jmp 826b6f0`；4.4 对 `if(c){body}` 恒折掉尾 jmp+nop（81 vs 80）。
    // 已试 P223 goto 岛 / P225 while+break 两形均不产尾 jmp。
    if (merged->GetEPLPState() <= 1)
    {
        merged->game_start(merged->getManager());
    }
    else
    {
        return;
    }
