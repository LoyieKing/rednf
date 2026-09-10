Dispatcher_ReportMannerlessUser::~Dispatcher_ReportMannerlessUser() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112/N197②] ORIG 0x08206124 orig/ours=232/231
//   prologue 差：ORIG `push %ebx`（值跨 call 保活占 callee-saved）vs 我方直接
//   sub $0x1f8,%esp；4.4 未升格（P197② 边界 / N82 寄存器分配跨版本）。语义已对齐。
//   首差: push   %ebx | sub    $0x1f8,%esp
