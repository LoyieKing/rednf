// ---- respond_special_item_skill_confirm —— ORIG 0x0827cdd0 ----
// 日志后调用 CPremiumLetheManager::ConfirmSkillReq(user)。
void SpecialItemHandler::respond_special_item_skill_confirm(int,
                                                            const STSpecailItem_Param& param,
                                                            STSpecailItem_Result&)
{
    // b28d-D42 P28/P0：ORIG 0x0827cdd0 先取 mgr（-0x1c 槽）；user 无具名槽
    // （每处 `mov 0x10(%ebp),%eax; mov (%eax),%eax` 逐点重读 param.m_user）。
    CPremiumLetheManager* mgr = G_CGameManager()->GetPremiumLetheManager();
    // b28d-D42 P227/P87：ORIG 0x0827cdd0 在 cMyTrace ctor 前右起求 varargs
    // 并保活 edi/esi/ebx（style→edi、characNo→esi、charName→ebx）⇒ 全表达式
    // 临时链形（具名三局部会退化为栈槽）。
    cMyTrace(__PRETTY_FUNCTION__, 0x1e1, 0)(
        "ONE_DAY_LETHE : BUY_SKILL_CONFIRM_TICKET, char(%s), char_no(%d), style(%d)",
        static_cast<CUser*>(param.m_user)->getCurCharacName(),
        static_cast<CUser*>(param.m_user)->getCurCharacNo(),
        static_cast<CUser*>(param.m_user)->GetCurCharacSkillTreeIndex());
    // b28d-D42 P223 变体：ORIG 0x0827cdd0 尾部为 `jmp <epilogue>; nop` 岛
    // ⇒ 裸早返形复现（if(mgr==0) return;）。
    if (mgr == 0)
        return;
    mgr->ConfirmSkillReq(static_cast<CUser*>(param.m_user));
}