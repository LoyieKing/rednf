void CQuickPartySystemManager::insert_timer_waiting_party_matching(
    CQuickParty* qp)
{
    // ORIG：gen_timer_key→%ebx、get_quick_party_index→%esi callee-saved 缓存（P24 内联实参）。
    // b28c-lane3 判 N82附：裸内联实参 4.4 恒 esi/ebx 对调，裸语句表达式形（P226）
    // 实测无效（仍 esi/ebx）⇒ 多实参 call 首值 callee-saved 对调不可复现。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                qp->get_quick_party_index(),
                                (TIMER_MESSAGE)0x9e, 0x14,
                                qp->gen_timer_key((TIMER_MESSAGE)0x9e), 0x9e);
