// ============================ DB 依赖 ============================
// [b28d-lane6] DIFF·N93/N82：槽号与块序（60 vs 63 条）。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF → n1_v 新口径 AE（gap=+0，blocks=0）⇒ 旧判词作废。
bool DB_LoadAutoMarketConditionsControl::makeRequest(bool flag)
{
    // ORIG T 0x08433ada _ZN34DB_LoadAutoMarketConditionsControl11makeRequestEb
    // [M1S3EmblemLink 2026-09-11 P0] ORIG 0x08433b00 `call Acquire` 结果留在 %eax，
    //   紧接 `movl $0x1,0x8(%esp); mov %eax,0x4(%esp); lea -0x10(%ebp),%eax; …
    //   call CStreamGuard::CStreamGuard(Stream*, bool)` —— 无 `Stream*` 具名槽
    //   （我方原多 -0xc 槽 + 2 条槽往返）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7c43), true);
    guard.operator*()->operator<<(0x15a);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    // [M1S3EmblemLink 2026-09-11 P0] ORIG 0x08433b89 正常路径 `jmp 0x08433ba6`
    //   直接进 epilogue（`lea -0x10; call CStreamGuard::~CStreamGuard; add $0x30;
    //   pop ebx/esi/ebp; ret`），**无** `mov $0x1,%ebx` / `mov %ebx,%eax` 返回值
    //   物化 ⇒ ORIG 源码无 return 语句（返回寄存器内容未定义）。删发明 `return true;`。
#if !remake_verify
    return 1;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}