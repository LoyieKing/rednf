// ---- importUsedFatigueGiveItemEventScript T 0x0898491b（2065B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importUsedFatigueGiveItemEventScript.md
// （双段完整阅读）。同双相派发形：10 个 (键,id) 注册链 +0x2ec→+0x355→+0x3c2→
// +0x432→+0x4a2→+0x531→+0x5af→+0x609→+0x6b5→+0x72f，公共尾 +0x7a2。
// id：[/event]0x58f [title]0x592 [explain]0x595 [db table]0x598 [user type]
// 0x59c [used fatigue]0x5a1 [charc event]0x5a7 [apply job type]0x5aa
// [used fatigue limit]0x5af [send mail]0x5b5。result(-0xf9) 初值 true；
// [/event] 返回 result 本体（+0x349 movzbl -0xf9，非恒真）；[user type]
// ScanInt(&result) 字节存 +0xc、[used fatigue] int 存 +0x10、[used fatigue
// limit] int 存 +0x1c，三者失败即 mov $0,%ebx 返回假（+0x527/+0x5a5/+0x725）；
// [charc event] 置字节 +0x14=1；[apply job type] 局部 string → ScanStr →
// getCharacterJobToEnumValue 存 +0x18；[send mail] 失败仅把 result 置假并继续。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
bool importUsedFatigueGiveItemEventScript(UsedFatigueGiveItemScript* script,
