// ============================================================================
// importAICharacterCommonScript —— ORIG T 0x088c9ccc
// 报告 _lt_global_gt_/importAICharacterCommonScript.md 完整转写。
// §6.2 超大函数（2105 insns）验收：调用点差集 320/320 全命中（MISS/EXTRA=0）；
// gap 186 → −5 → **−3**（round5：两处 `switch(ScanFloatStr)` + 删 fv 初值 +
// [hell monster] 补具名 int 中转槽，our 2111→2108）；13 处非分支比较已与 ORIG
// 逐条同形（含 -0x10 槽号）。残余为 N82 帧槽错位（ORIG 0x130 vs NEW 0x160，
// P182 不强凑）与 ORIG 共享 int 槽（-0xa4 族）⇒ 判语义完成、byte 层 N 封顶。
// 返回 bool：文件加载失败 / [rarity] 解析失败或=6 / [pvp win point] 逆序 /
//   [drop avatar item] 任一扫描失败 / [animation job] 失败 /
//   兜底 importAIInfoScript 失败 → false；ScanType 终结 → true。
// ============================================================================
bool importAIInfoScript(STAIInfoScript* info, std::string* tag, bool byLevel);
