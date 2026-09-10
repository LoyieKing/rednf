// [L3FLinkClosureQuickParty 2026-09-11] §6.5.1 串直证：ORIG PF 串为
// `QuickParty::CQuickParty* QuickParty::CQuickPartySystemManager::find_quick_party_in_pool(QuickParty::LIST_QUICK_PARTY&)`
// ⇒ 形参书写类型用 ORIG 的 typedef 名（statically identical to std::list<int>，
// mangled 不变）。typedef 落在 namespace QuickParty 内（本 include 位置）。
typedef std::list<int> LIST_QUICK_PARTY;
CQuickParty* CQuickPartySystemManager::find_quick_party_in_pool(
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
