LevelupSupport2ndScript::~LevelupSupport2ndScript() {}

// ============================================================================
// C. importActionMailScript —— T 0x0897f147（1656 字节）
//    [objdump·报告缺失] 转写。标签：[inven]/[job](8 职业)/[detail]/[/send mail]。
// ============================================================================
// ---- importActionMailScript T 0x0897f147（1656B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importActionMailScript.md（双段
// 完整阅读）。此函数主循环用 token 串比较链（非 __lookup 表——ORIG 无
// 惰性建表段，实证汇编 +0x4a 起即 operator== 链）。[job] 节内循环：
// ScanType 失败跳回主循环头（+0x0bb）；每轮先默认构造 pair<int,int> kv
// （-0xbc/-0xb8 跨职业共享，+0x0c9 在比较链之前），命中职业则
// job(-0xb4)=立即数 + kv.first/kv.second = ScanInt(&ok)×2 +
// value_type(job,kv) 引用实参构造 + map::insert（P134 嵌套形）。
// [detail] coin 校验为 signed char <0 || 15< 双段（js/jle，+0x5e6/+0x5f1）。
// 残差归因（N 类）：[job] 8 职业分支已按 ORIG 物理复制+具名 entry/res 独立
// 槽（计数 378/380），但 4.4 栈槽分配器按 SSA 序落位（token 在浅端 -0xc，
// ORIG 4.1 在深端 -0xac），全函数槽号文本错位不可由声明序翻转复现（P112）；
// 比较链 je/jne 朝向为 setcc-fold 族。
