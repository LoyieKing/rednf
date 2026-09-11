// [b28c-4] ORIG 0x080e09bc 实形 = ScriptInterface::C1（写基类 vptr）→
// Singleton<Manager>::C1（空）→ 派生 vptr → 5 条 `mov 0x8(%ebp),%eax;
// movl $0x0,0x8|0xc|0x10|0x14|0x18(%eax)` → ScriptLoader::C1(this+4)。
// 该 5 条落在 map(+4) 的 _Rb_tree_impl 内部（color/parent/left/right/
// node_count），是 4.1 对成员 value-init 的**前置 zero-init**（P90/N77 族），
// 非 Manager 成员——原 5 个 int 成员系臆造，已由 Main 代落删除
// （硬证据：单例 obj nm -S size=0x1c = vptr4 + m_loader 0x18）。
// b28d-solo-CCeraShopAddRestrict: AE（27=27；P299/CWG434 由 CUser.h 删 ScriptLoader
// 用户空 ctor 代落）。
Manager::Manager()
    // [b28c-4] 试 value-init 形复现 ORIG 4.1 对 m_loader 的前置 zero-init
    //（P90/N77：4.4 按 CWG434 可能仍不 emit）。
    : m_loader()
{
}