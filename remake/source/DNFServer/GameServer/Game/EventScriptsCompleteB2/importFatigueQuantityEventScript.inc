// ---- importFatigueQuantityEventScript T 0x08982fc6（1248B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importFatigueQuantityEventScript.md
// （双段完整阅读）。双相派发同族：5 键注册链 +0x29d→LAB(title)→explain→
// db table→user type，公共尾 +0x474。id：[/event]0x478 [title]0x47a
// [explain]0x47d [db table]0x480 [user type]0x483。[user type] ScanInt(NULL)
// 字节截断值先物化存 +0xc（+0x46c mov %eax,%edx / mov %dl）；-0x1d bool 槽
// 初值 0 仅作 ScanType 失败返回（+0x4a1 movzbl -0x1d），此外从不写。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
bool importFatigueQuantityEventScript(FatigueQuantityScript* script, char const* path)
