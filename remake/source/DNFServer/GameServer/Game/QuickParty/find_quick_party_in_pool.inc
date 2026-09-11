// ---- find_quick_party_in_pool @0x0826a9fe：
// 从档位 list 逐个弹出 index 取 CQuickParty；取不到的记 trace 继续弹。----
// [b28d-L1] IDENTICAL/AE（routed 口径）：ORIG 0x0826a9fe；P167 CLEAR（7/7）。
// [b28d-solo-QuickParty] verdict=AE（TU 口径 41=41）；P167 CLEAR。
// [L3FLinkClosureQuickParty 2026-09-11] 负结论（ORIG 0x0826aa7b `nop` @+0x7d，循环条件
// +0x7e 之前，1 条、无任何跳转引用）：三形实测均不可复现 —— ①`continue;`（无条件，
// 42/42，无 nop）；②空语句 `;`（42/42，无 nop）；③`for (; !pool.empty(); )` 空增量
// （42/42，无 nop）。⇒ 4.1 在循环体末尾/增量位置 emit nop 而 4.4 不 emit（版本差异，
// 非源码形）。AE 口径（剥离 nop）已达标；保留 while 原形。
// [L3FLinkClosureQuickParty 2026-09-11] §6.5.1 串直证：ORIG PF 串为
// `QuickParty::CQuickParty* QuickParty::CQuickPartySystemManager::find_quick_party_in_pool(QuickParty::LIST_QUICK_PARTY&)`
// ⇒ 形参书写类型用 ORIG 的 typedef 名（statically identical to std::list<int>，
// mangled 不变）。typedef 落在 namespace QuickParty 内（本 include 位置）。
typedef std::list<int> LIST_QUICK_PARTY;
CQuickParty* CQuickPartySystemManager::find_quick_party_in_pool(
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
    QuickParty::LIST_QUICK_PARTY& pool)
{
    CQuickParty* qp = 0;             // ORIG -0x10 头部 movl $0x0 初始化
    int qpIndex;                     // ORIG -0xc 深槽
    while (!pool.empty())
    {
        qpIndex = pool.front();
        pool.pop_front();
        qp = G_CGameManager()->GetQuickParty(qpIndex);
        // b28c-lane3：ORIG 0x0826aa43 `jne 0x96`（直跳函数尾 return 块）。
        // 循环内 `return qp` 会被 4.4 物化成 `je <trace>; mov -0x10,%eax; jmp`，
        // 改 break + 循环外 return 后 4.4 与 ORIG 同为 jne 直达尾块。
        if (qp != 0)
            break;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x29c, 5);
        trace("QUICK_PARTY_LOG : NO INTENDED ROUTINE!!");
    }
    return qp;
}