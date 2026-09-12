// ORIG T 0x0860ab2e（报告 CSpecialItemRoutingManager/ArriveMemberSelection.md
// 全文转写）：GetMemberRoutingState(user) 已非 0 → 返回 0（已选过）；
// 否则 sel==0 → SetMemberRoutingState(user,1)、sel==1 → SetMemberRoutingState(user,2)
// （其它值不写状态）；随后 HasAllMemberSelection() 则 cancelTimer+processRouting；
// 返回 1。
// b28c-L20: AE — 修「恒返回 0」真语义 bug（原 goto FAIL 让成功路径也落到 return 0）；
// [b28d-58] AE：FAIL 块（return 0，ORIG +0x6e）夹在 sel 链与 HasAll 之间，
// CHECK 在 +0x75 —— 该块序即 `if (state == 0) { sel 链 } else { return 0; }`
// 的 4.1 布局；原 goto FAIL/CHECK 形令 4.4 在 FAIL 前后各插一条跳转对齐 nop
// （+0x6e/+0x76）且 CHECK 漂到 +0x77 ⇒ 改自然 if/else 后逐条全等。
// [R16a13] 返回类型 int→bool（P292：唯一调用点 0x08256ba 为 `xor $1` 形）；
// 定义体不变（常量返回 int/bool 同形，探针 probe_bool.s 实证）。
bool CSpecialItemRoutingManager::ArriveMemberSelection(CUser* user, CParty* party,
                                                       char sel)
{
    int state = party->GetMemberRoutingState(user);           // ORIG 0x085ba150
    if (state == 0)
    {
        if (sel == 0)
            party->SetMemberRoutingState(user, 1);            // ORIG 0x085ba0d8
        else if (sel == 1)
            party->SetMemberRoutingState(user, 2);
    }
    else
    {
        return 0;
    }
    if (party->HasAllMemberSelection())                       // ORIG 0x085ba1c0
    {
        cancelTimer(party);
        processRouting(party);
    }
    return 1;
}