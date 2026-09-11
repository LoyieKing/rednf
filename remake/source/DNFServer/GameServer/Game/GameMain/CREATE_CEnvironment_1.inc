// ---------------------------------------------------------------------------
// GlobalInstance 单例 CREATE_*/DESTROY_*（ORIG T 0x82a7029..0x82a7136）：
// 每个 CREATE_X() == GlobalInstance<X>::inst()，DESTROY_X() == destroy()，
// 与 ORIG 反汇编逐一核对（call _ZN14GlobalInstanceIXE4instEv / 7destroyEv）。
// ---------------------------------------------------------------------------
void CREATE_CEnvironment()      { GlobalInstance<CEnvironment>::inst(); }
