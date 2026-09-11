// ============================================================================
// importAIInfoScript —— ORIG T 0x08a390db
// 报告 _lt_global_gt_/importAIInfoScript.md 完整转写。
// 调用点差集：MISS=0；EXTRA 仅 _Unwind_Resume×2（EH landing pad 版本差）+
// C1↔C2 / D1↔D2 同址别名对（P93 伪差）。
// [round5] tok/isType 提升到函数头（复现 ORIG 入口 C1 + `movb $0,-0x21` 槽图基线）
// + byLevel 分支块序交换（ORIG 直落块 = !byLevel / makeAIFullPath）⇒ our 461→462。
// 残余 13 条 = 4.1 `movzbl 槽/xor $1/test %al,%al/je` bool 物化族（N122：4.4 对
// bool 局部恒 cmpb $0 直比，源形不可造）与 `while (ScanStr(&s))` 落槽形
// ⇒ 判语义完成、byte 层 N 封顶（§6.2 同口径）。
// ============================================================================
bool importAIInfoScript(STAIInfoScript* info, std::string* tag, bool byLevel);
