// ---- importPurchaseCashItemBonusEventScript T 0x089834a6（2404B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importPurchaseCashItemBonusEventScript.md
// （双段完整阅读）。双相派发同族：8 键注册链 +0x2e0→+0x347→+0x3b4→+0x424→
// +0x494→+0x54a→+0x5be→+0x6aa，公共尾 +0x933。id：[/event]0x4a1 [title]0x4a3
// [explain]0x4a6 [db table]0x4a9 [item idx]0x4ac [item idx start]0x4b6
// [item idx end]0x4b9 [send]0x4c6。函数级 uint start(-0x13c) 初值 0（+0x45）。
// [item idx]（+0x4f1）uint 槽清 0→ScanInt(&ok)→set<unsigned>+0xc insert，
// 失败回主循环；[item idx start] ScanInt(NULL) 存 start；[item idx end]
// （+0x61e）uint end=ScanInt(&ok)，ok&&start!=0&&end!=0 时
// `for(;start<=end;++start) set.insert(start)`（+0x695 setbe 无符号界）。
// [send] 节（+0x6fe）：局部 rule ctor 后节内 while(ScanType)，四标签经
// std::operator== 直比（不入查表）：三个 [purchase * count] ScanInt(&ok)
// 成功→节内 continue、失败→退出节回主循环（+0x776/+0x778，此前简化形方向
// 写反=语义缺口，现按 ORIG 修正）；[send mail] 局部 mail ctor+reset，
// importActionMailScript 失败立即退出节（rule 不复位保留原值，+0x861..868），
// 成功则三字段均 !=-1 时 push_back(pair(rule,mail) 值临时+~pair)、随后
// rule.reset() 后回节内（+0x924 test ebx/jne +0x71b）。未知 token 忽略
// 继续；节内 ScanType 失败退出节回主循环。ok(-0x135) 共享即返回值（+0x960）。
// 残差归因（N 类，等计数 563=563）：4 节 ok 检查点 je/jne 朝向（4.1 先
// xor 物化再反判 vs 4.4 直接 test 反转目标，setcc-fold 族）、节头 4 枚
// 前跳 nop;jmp 岛（+0x70e..+0x717，jump-threading 族）、pair 构造实参
// %eax↔%edx（N7）、make_pair sret 后 `sub $0x4` 伴随（P75 族）均不可由
// 合法源码翻正；[item idx end] 的 `xor;test;jne` 已用 != true 形复现。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
bool importPurchaseCashItemBonusEventScript(PurchaseCashItemBonusScript* script,
