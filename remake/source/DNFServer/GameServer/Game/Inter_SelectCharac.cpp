// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Inter_SelectCharac/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— Inter_SelectCharac（ISigDispatcher 派生，仅 4 个 ORIG 符号）。
//   dispatch_sig @ 0x084c528a（size 0xa）：push ebp; mov esp,ebp; mov $0x0,%eax;
//     pop ebp; ret —— 即 return 0;（报告 docs/class_func_reports/Inter_SelectCharac/dispatch_sig.md）。
//   D1/D2 @ 0x084fa236（size 0x30）：设 vptr(0x8c8ff48) → call ISigDispatcherD1Ev →
//     mov $0,%eax; test %al,%al; je（恒假 deleting-check）→ leave; ret —— 用户定义空析构。
//   D0 @ 0x084fa266（size 0x1e）：call D1 → operator delete —— 编译器自动生成。
// 无 C1/C2（隐式 ctor 不产出符号），本 TU 不定义构造函数。

#include "Inter_SelectCharac.h"

#include "Inter_SelectCharac/dtor_Inter_SelectCharac.inc"


#include "Inter_SelectCharac/dispatch_sig.inc"

