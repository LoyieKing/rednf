// insert_timer_waiting_party_matching @0x0826acea：
// InsertTimer(objType=1, qpIndex, msg=0x9e, sec=0x14, key, 0x9e)。
// [b28d-L1] N82附 封顶：ORIG 0x0826acea 首值 gen_timer_key→%ebx、get_quick_party_index→%esi；4.4 恒 esi/ebx 对调（裸内联实参/P226 语句表达式两形实测均不可翻），P167 CLEAR（4/4）语义等价。
// [b28d-solo-QuickParty] verdict=NEAR 封顶（28=28，仅 gen_timer_key→%esi /
// get_quick_party_index→%ebx 与 ORIG 的 %ebx/%esi 角色互换；P167 CLEAR）。本轮实测：
// ①裸内联/双 register/具名 uint key 恒互换（具名 uint 落槽 -0xc 反劣 29 条）；
// ②`register unsigned int key` + 内联 index 令 gen→%ebx 但 4.4 多发 `mov %ebx,%esi`
// 中转（29 条，N18a 反向）；③4.4.6 后端（-B cc1plus446bin）复测同样互换 ⇒ 非 N7
// 后端敏感族，属 N82附「多实参 call 首值 callee-saved 对调」封顶。
void CQuickPartySystemManager::insert_timer_waiting_party_matching(
    CQuickParty* qp)
{
    // ORIG：gen_timer_key→%ebx、get_quick_party_index→%esi callee-saved 缓存（P24 内联实参）。
    // b28c-lane3 判 N82附：裸内联实参 4.4 恒 esi/ebx 对调，裸语句表达式形（P226）
    // 实测无效（仍 esi/ebx）⇒ 多实参 call 首值 callee-saved 对调不可复现。
    // [L3FLinkClosureQuickParty 2026-09-11] 复核：补测 `(int)qp->gen_timer_key(...)`
    // 显式转换形亦无效（仍 gen→%esi / index→%ebx，28/28）⇒ N82附 封顶确认。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                qp->get_quick_party_index(),
                                (TIMER_MESSAGE)0x9e, 0x14,
                                qp->gen_timer_key((TIMER_MESSAGE)0x9e), 0x9e);
}