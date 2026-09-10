Inter_MonitorDeathTowerRanking::~Inter_MonitorDeathTowerRanking() {}
// ORIG 0x084d3290 T dispatch_sig —— 死亡之塔排行登记。
// 报告 docs/class_func_reports/Inter_MonitorDeathTowerRanking/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=59（ORIG 0x084d3290）：槽序/帧差(N82/P212/P221)+混合残差；gap+12。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=78（ORIG 0x84d3290）：N82/P112 帧槽平移+P30/N4 add-vs-lex。
// [lane2 2026-09-10] 裸偏移指针算术 → 视图数组成员（`pPkt->m_ranks[i*4+j+4]`）复现
//   ORIG 084d32c3 `add $0x4,%edx; mov 0x2(%eax,%edx,4)`；补包基址具名局部（ORIG -0x24）。
//   残余 = 索引计算寄存器分配/label 顺序（gap -6，N18a/LRA 族）。
