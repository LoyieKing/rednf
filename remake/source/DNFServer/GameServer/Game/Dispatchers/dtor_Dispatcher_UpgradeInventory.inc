Dispatcher_UpgradeInventory::~Dispatcher_UpgradeInventory() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0822a4bc orig/ours=275/264 ndiff-blocks=39
//   帧差 sub $8c vs $4 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x8c,%esp | movl   $&_ZZN27Dispatcher_UpgradeInventory12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
// [SweepDispatchers3 2026-09-09] 修真缺口：删 invenR/capOld/capNew 缓存，每个使用点
//   重新 getCurCharacInvenR()/get_inventory_capacity()（P160）⇒ ORIG 的 9 次/6 次调用
//   归位。gap 11→0（275=275），DIFF→AE（报告 dispatch_sig.md Ghidra C 已读）。
