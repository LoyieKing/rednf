Dispatcher_ExitPrivateStore::~Dispatcher_ExitPrivateStore() {}
// ORIG 0x0820269c（报告 Dispatcher_ExitPrivateStore/dispatch_sig.md）：
// state<3 || curCharacR==0 → 0；GetStoreBuyer 后经 IBuyer 调 ExitPrivateStore()。
// [b28c-32] verdict=NEAR：去 mgr 缓存槽 + 删 ORIG 无判空；余 N7（44→39）。
