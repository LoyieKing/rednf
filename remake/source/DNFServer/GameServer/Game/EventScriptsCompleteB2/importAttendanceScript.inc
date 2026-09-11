// ---- importAttendanceScript T 0x08981539（2262B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAttendanceScript.md（汇编+
// Ghidra C 双段完整阅读）。ORIG 为「函数级静态 __lookup(map<const char*,int,
// less_TCHAR>)/__init_lock(SpinLock)/__inited(bool) 三件套 + case 贯穿建表链」
// 双相派发（同 importTitleBookEtcScript 形）：首令牌以 key=0 进入注册链，自
// case 0x26f（与 case 0 合并，+0x306）起逐 case insert 全部 12 个 (键,id)
// （+0x306→+0x36d→+0x3da→+0x44a→+0x4ba→+0x52e→+0x59e→+0x60e→+0x6d1→+0x736→
// +0x7ad→+0x80b），落公共尾（+0x867）leave+置位后回 guard 重派发同一令牌。
// id：[/event]0x26f [title]0x271 [explain]0x274 [db table]0x277 [fatigue]0x27a
// [reward explain]0x299 [final reward explain]0x29c [reward]0x29f
// [final day condition]0x2ad [final reward]0x2b0 [send mail]0x2c7
// [msg overbtn]0x2cb。[reward] 节为 pair 默认构造+ScanInt(&ok) 成对循环
// （+0x65c..+0x6ca，ok 即 -0x121 返回值共享槽）；其余数值节 ScanInt(NULL)
// 直存；ScanType 失败返回 ok（+0x893 movzbl -0x121）。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
bool importAttendanceScript(AttendanceScript* script, char const* path)
