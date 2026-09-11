// ---- SDCActiveStatusSyncServer C1/D1（ORIG W 0x0823487c/0x08234890，仅委托基类）----
// 定义置于本 TU（基类 C1/D1 定义在 CtorProducers_2.cpp）：caller TU 不含 callee
// 定义体 ⇒ 4.4 unit-at-a-time 不塌缩 outgoing 帧，保持 ORIG 的 sub $0x18（NL4 实证）。
SDCActiveStatusSyncServer::SDCActiveStatusSyncServer()
{
    // 仅委托基类 ctor（隐式基类构造，asm call _ZN25SDCActiveStatusSyncCommonC1Ev）。
}