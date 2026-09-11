// get_quick_party_count @0x082698be：累加各登记 party 的当前成员数。
// [b28d-L1] IDENTICAL/AE（routed 口径 check_tu_routed）：ORIG 0x082698be 逐条对齐（40/40，P167 CLEAR）。
// [b28d-solo-QuickParty] verdict=AE（TU 口径 39=39）；P167 CLEAR（0 missing/0 extra）。
// [L3FLinkClosureQuickParty 2026-09-11] ORIG 两个 `je` 落点不同：+0x23 `je +0x5f`（i++ 语句）
// 与 +0x46 `je +0x5e`（= 循环体末尾 nop，被 continue 引用）。⇒ ORIG 外层为正面 if 包裹
// （`if (m_partyIndex[i] != -1) {...}`），内层才是 `if (party == 0) continue;`。
// 原写法把外层写成 continue，令 je#1 落到 nop、je#2 落到 i++，两处落点互换（40 vs 40 但形不同）。
int CQuickParty::get_quick_party_count()
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    int total = 0;
    for (int i = 0; i < 4; ++i) {
        if (m_partyIndex[i] != -1)
        {
            CParty* party = G_CGameManager()->GetParty(m_partyIndex[i]);
            if (party == 0)
                continue;
            // ORIG：get_member_count 结果落具名槽 -0xc 后再累加
            int memberCount = party->get_member_count();
            total += memberCount;
        }
    }
    return total;
}