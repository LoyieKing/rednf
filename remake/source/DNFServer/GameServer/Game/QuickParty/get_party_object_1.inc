// ---- get_party_object() @0x0826972c：返回首个登记且仍存在的成员 party ----
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x082696d0；P167 CLEAR（2/2）。
// [b28d-solo-QuickParty] verdict=AE（TU 口径 33=33）；P167 CLEAR。
// [L3FLinkClosureQuickParty 2026-09-11] ORIG 0x0826972c +0x1c `je +0x46`（i++ 语句，
// 该处无 nop；函数共 33 条，尾块 `mov $0,%eax` 在 +0x55）⇒ ORIG 用正面 if 包裹
// （`if (m_partyIndex[i] != -1) {...}`），无 continue ⇒ 4.4 不生成「循环体末尾 nop」。
// 原 continue 形多出 1 条 nop（34 vs 33）。
CParty* CQuickParty::get_party_object()
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_partyIndex[i] != -1)
        {
            CParty* party = G_CGameManager()->GetParty(m_partyIndex[i]);
            if (party != 0)
                return party;
        }
    }
    return 0;
}