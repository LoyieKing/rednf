// ---- importAvatarDisjointRandomRewardScript T 0x089824b4（1429B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAvatarDisjointRandomRewardScript.md
// （双段完整阅读）。双相派发同族：5 键注册链 +0x29d→+0x304→+0x365→+0x3c6→
// +0x4ca，公共尾 +0x526。id：[/event]0x32e [title]0x330 [explain]0x333
// [random reward item]0x336 [send mail]0x346。[random reward item]（+0x417）：
// char/uint 双槽每轮先清 0，key=(char)ScanInt 字节截断存槽（+0x436 mov %al），
// 成功对经 make_pair&lt;char&amp;,unsigned&amp;&gt; 引用对临时+value_type 转换+
// map::insert 三段（P134 嵌套实参形），任一 ScanInt 失败回主循环。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
bool importAvatarDisjointRandomRewardScript(AvatarDisjointRandomRewardScript* script,
