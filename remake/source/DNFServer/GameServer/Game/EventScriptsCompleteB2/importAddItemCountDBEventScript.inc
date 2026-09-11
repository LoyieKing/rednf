// ---- importAddItemCountDBEventScript T 0x0897fc99（1480B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAddItemCountDBEventScript.md
// （双段完整阅读）。双相派发同族：6 键注册链 +0x2a8→+0x30f→+0x36d→+0x3ce→
// +0x42f→+0x4c4，公共尾 +0x559。id：[/event]0xe9 [title]0xeb [explain]0xee
// [db table]0xf1 [item index]0xf4 [quest index]0xff。[item index]/[quest
// index] 均为「槽清 0→ScanInt(&ok)→失败回主循环→push_back→重循环」的收集
// 循环（+0x47d/+0x512，此前简化形漏循环=语义缺口），vec 分别 +0x8/+0x14；
// [db table] 串存 +0x20。ok(-0xa9) 共享即返回值。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
bool importAddItemCountDBEventScript(AddItemCountDBScript* script, char const* path)
