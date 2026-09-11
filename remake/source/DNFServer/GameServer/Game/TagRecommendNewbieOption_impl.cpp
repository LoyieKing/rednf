// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TagRecommendNewbieOption_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// TagRecommendNewbieOption_impl.cpp —— tagRecommendNewbieOption W 符号单 TU 载体
// （C1/C2/D1/D2/clear 均类内内联于 TagRecommendNewbieOption.h，本 TU 以
// force_emit 强制实例化产出符号，供 check_tu_game_orig.sh 与 ORIG 对比）。
// ORIG：C1 0x0891ad16 / clear 0x0891ad2a / D1 0x0893908c（均 W）。
// 报告：/tmp/df_batches/OrigNewbie.md
#include "TagRecommendNewbieOption.h"

// [AnonLane2 2026-09-11 试形-回滚] 曾删本垫片（目标符号全局有其它定义者），
// 但本 TU 四数 0/2/0/0/1 → 0/0/0/0/0（ae −2）⇒ 按 TU 口径回滚，判词保留。
namespace
{
#include "TagRecommendNewbieOption_impl/force_emit.inc"

}
