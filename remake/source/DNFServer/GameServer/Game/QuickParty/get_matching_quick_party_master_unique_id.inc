// get_matching_quick_party_master_unique_id @0x08269936：
// 首选 EPLP 状态 >1 的成员 party；否则任一 quick party；否则首个存在的 party；
// 返回其 manager 的 unique_id（无则 0xffff）。
// [b28d-L1] N82 封顶：ORIG 0x08269936；P167 CLEAR（13/13）⇒ 语义完成，残余为帧/槽位形态差（ORIG sub 0x48 vs 4.4 push ebx+sub 0x34）。
// [b28d-solo-QuickParty] verdict=AE（104=104）：修法=P0 第二循环/第三段具名 partyIdx 落槽
// （内联实参使 4.4 升格 %ebx 跨 G_CGameManager()）+ party 循环外声明 `= 0`（ORIG -0x28
// 槽 + movl $0 初始化）+ P171 删 mgr 判空（ORIG 0x08269a75 无 test）。旧 N82 判词作废。
unsigned short CQuickParty::get_matching_quick_party_master_unique_id()
{
    int slot = -1;
    unsigned short uid = 0xffff;
    // [b28d-solo-QuickParty] 修法（N93/P212）：ORIG 槽序 party=-0x28 深于 i=-0x24/
    // partyIdx=-0x20 ⇒ 原码 party 为循环外声明（循环内仅赋值）；循环内声明使 4.4
    // 把它排到 i/partyIdx 之上（-0x20）。
    CParty* party = 0;
    for (int i = 0; i <= 3; ++i)
    {
        int partyIdx = get_party_index(i);
        party = G_CGameManager()->GetParty(partyIdx);
        if (party == 0)
            continue;
        if (party->GetEPLPState() > 1)
        {
            slot = i;
            break;
        }
        if (party->is_quick_party())
            slot = i;
    }
    if (slot == -1)
    {
        for (int i = 0; i <= 3; ++i)
        {
            // [b28d-solo-QuickParty] 修法（P0）：ORIG 0x082699ef 具名 partyIdx 落 -0x18 槽
            // 后传 GetParty；内联实参使 4.4 升格 %ebx 跨 G_CGameManager() 保活（push %ebx
            // + 帧 0x34、PartyGuard 槽整体漂移）。
            int partyIdx = get_party_index(i);
            if (G_CGameManager()->GetParty(partyIdx) != 0)
            {
                slot = i;
                break;
            }
        }
    }
    if (slot != -1 && slot >= 0 && slot <= 3)
    {
        // [b28d-solo-QuickParty] 修法（P0）：ORIG 0x08269a4a 具名 partyIdx 落 -0x14 槽。
        int partyIdx = get_party_index(slot);
        CParty* party = G_CGameManager()->GetParty(partyIdx);
        if (party != 0)
        {
            // [b28d-solo-QuickParty] 修法（P171 反向照抄）：ORIG 0x08269a75 无 mgr 判空
            // （getManager 结果直接 `mov -0xc,%eax; call get_unique_id`）。
            CUser* mgr = party->getManager();
            uid = static_cast<unsigned short>(mgr->get_unique_id());
        }
    }
    return uid;
}