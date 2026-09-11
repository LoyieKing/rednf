// ---- importAccountFirstLoginEventScript T 0x0897f7bf（1242B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAccountFirstLoginEventScript.md
// （双段完整阅读）。双相派发同族：5 键注册链 +0x29d→LAB(0x fab4)→(fb12)→
// (fb73)→(fbd1)，公共尾 +0x46e。id：[/event]0xc4 [title]0xc6 [explain]0xc9
// [db table]0xcc [send mail]0xcf；mail 存 +0xc；-0x1d bool 槽仅作 ScanType
// 失败返回（+0x49b）。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
bool importAccountFirstLoginEventScript(AccountFirstLoginScript* script, char const* path)
