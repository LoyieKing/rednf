// ---- importPcBangPlayTimeScript T 0x0898512c（1661B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importPcBangPlayTimeScript.md
// （双段完整阅读）。同 importAttendanceScript 双相派发形：8 个 (键,id) 注册链
// +0x2d3→+0x33a→+0x3a7→+0x417→+0x47b→+0x4e2→+0x549→+0x5ae，公共尾 +0x611。
// id：[/event]0x5d2 [title]0x5d7 [explain]0x5da [db table]0x5dd [play time]
// 0x5e0 [number of time]0x5e3 [start tag]0x5e6 [end tag]0x5e9。[play time]
// +0xc(int)/[number of time]+0x10(byte mov %dl)/[start tag]+0x12(word mov
// %dx)/[end tag]+0x14(word) 均 ScanInt(NULL) 直存；-0x1d bool 槽初值 0 仅作
// ScanType 失败返回（+0x63e movzbl -0x1d），此外从不写。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
bool importPcBangPlayTimeScript(PcBangPlayTimeScript* script, char const* path)
