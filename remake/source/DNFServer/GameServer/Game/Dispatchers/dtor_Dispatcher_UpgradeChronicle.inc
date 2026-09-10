Dispatcher_UpgradeChronicle::~Dispatcher_UpgradeChronicle() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08223330 orig/ours=538/524 ndiff-blocks=91
//   帧差 sub $17c vs $16c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x17c,%esp | movw   $0x0,-0x4e(%ebp) | movl   $0x0,-0x54(%ebp)
// [SweepDispatchers3 2026-09-09] 修真缺口：vector<int> 遍历增量改后置 vIt++（P28：
//   ORIG 增量段 ppEi×3，p167 MISSING ppEi×3 / EXTRA ppEv×3 归零）。gap 13→9
//   （528 vs 537），残差 = 4.1 码风（je/movl/lea/xor 各 2-3）。
