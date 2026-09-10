Inter_OnTimeEventRewardEnd::~Inter_OnTimeEventRewardEnd() {}
// ORIG 0x084e1062 T dispatch_sig —— 准时事件奖励结束：置奖励态并全员广播结束包。
// 报告 docs/class_func_reports/Inter_OnTimeEventRewardEnd/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=96（ORIG 0x084e1062）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=53（ORIG 0x84e1062）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→3（102 vs 99，TU 121/1331/5/133 不回退）：真修两处——
//   ① 补 ORIG 0x084e106a 死写入 `char* p; p=data;`（P171/P198 两句式，ORIG `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` 后从未读）；
//   ② 两个 cMyTrace 改全表达式临时链（P221/P227，消 else 分支具名对象槽）。残余=N7 虚调寄存器+块序（N104）。
