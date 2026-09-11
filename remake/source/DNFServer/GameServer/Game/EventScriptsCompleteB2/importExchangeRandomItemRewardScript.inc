// ---- importExchangeRandomItemRewardScript T 0x08981e0f（1701B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importExchangeRandomItemRewardScript.md
// （双段完整阅读）。双相派发同族：6 个 (键,id) 注册链 +0x2a8→+0x30f→+0x376→
// +0x3d7→+0x540→+0x5d7，公共尾 +0x636。id：[/event]0x2e8 [title]0x2ea
// [explain]0x2ed [material]0x2f0 [random reward item]0x308 [send mail]0x312。
// [material] 节（+0x428）：先 ScanType 一次（失败 mov $0,%ebx 直返假，+0x445）；
// 该 token 经 std::operator== 与 "[exchange rule]" 直比（+0x460，非查表）；
// 相等则 ScanInt(&ok) 字节存 +0x8，失败跳公共尾回主循环；两路均汇入 uint
// 整数对循环（+0x491：槽先清 0，make_pair<unsigned&,unsigned&> 引用对临时
// （P61/P81）→ pair<int,int>::pair<unsigned,unsigned> 转换构造 →
// vector<pair<int,int>>::push_back）。[random reward item]（+0x58e）int 槽每轮
// 清 0 后 ScanInt(&ok)，成功 push_back 至 +0x18；ok 共享变量即返回值（+0x663）。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
bool importExchangeRandomItemRewardScript(ExchangeRandomItemRewardScript* script,
