// ORIG Timer_QuickPartyWaitingPartyMatching::dispatch_sig @ 0x863ac18 (0x17e)
// [b28d-solo-TimerDispatcher2] 已 AE：P52 链式 receiver 内联（去 game/mgr 槽）+
// 短路条件内联去 bool cancelNow 槽 ⇒ 帧 0x38→0x28、全槽回正、113/113。
bool Timer_QuickPartyWaitingPartyMatching::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    QuickParty::CQuickParty* quickParty =
        G_CGameManager()->GetQuickParty(p1);
    if (quickParty == 0)
        return 0;

    if (!quickParty->check_timer_key((TIMER_MESSAGE)p3, p2))
        return 0;

    const int quickPartyCount = quickParty->get_quick_party_count();
    // ORIG：count < 2 && first_matching_cnt == count（movsbl 符号扩展后比较）
    if ((quickPartyCount < 2) &&
        (static_cast<int>(quickParty->get_first_matching_cnt()) == quickPartyCount))
    {
        return 0;
    }

    if (quickParty->get_timer_key((TIMER_MESSAGE)p3) == 1)
    {
        if (!cancel_quick_party(quickParty))
            return 0;
    }

    // ORIG：!(cnt < count) && !(is_change_quick_party_count()) 时取消
    // [b28d-solo-TimerDispatcher2] ORIG 无 firstCnt/cancelNow 槽（`movsbl %al,%eax;
    // cmp -0xc(%ebp),%eax; jl skip` + `xor $1; test; je skip` 两直跳）⇒ 短路条件
    // 内联（旧具名 bool 槽多 4 条物化 + 帧 +0x10）。
    if (static_cast<int>(quickParty->get_first_matching_cnt()) >= quickPartyCount &&
        !quickParty->is_change_quick_party_count())
    {
        if (!cancel_quick_party(quickParty))
            return 0;
    }

    QuickParty::STQuickPartyPoolMap_Key poolKey;   // ORIG ctor W @0x082344ca
    poolKey.m_dungeonIndex = quickParty->get_dungeon_index();
    poolKey.m_dungeonDiff  = static_cast<unsigned char>(quickParty->get_dungeon_diff());

    // [b28d-solo-TimerDispatcher2] P52/P183：ORIG 无 game/mgr 具名槽（`call
    // G_CGameManager; mov %eax,(%esp); call GetQuickPartySystemManager` 链式直传）
    // ⇒ 内联 receiver 链，帧 0x38→0x28、全槽回正。
    G_CGameManager()->GetQuickPartySystemManager()->start_game(quickParty, poolKey);
    return 1;
}