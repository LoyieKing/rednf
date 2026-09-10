// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AuctionHelpers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "AuctionHelpers.h"

#include "AuctionHelpers/IsPriceError.inc"
{
    // ORIG 单布尔形态（Ghidra 等）：`(a<-1||b<-1) || (b!=-1 && b<=a)`。
    if ((price < -1 || limit < -1) || (limit != -1 && price >= limit))
        return 1;
    return 0;
}

#include "AuctionHelpers/IsValidMultiplyResult.inc"
{
    // [本波·DIFF→AE] ORIG 0x08213e40（34 insns，无栈帧）逐条即 4.1 对
    // `(long long)a * b > 0x7fffffff` 的展开：signed 64 位乘法（`mov %eax,%ebx;
    // sar $0x1f,%ebx` 符号修正 + `imul/mul` 高位）+ 64 位常量比较
    // （hi<0→假、hi>0→真、hi==0→`cmp $0x7fffffff` 无符号比较 lo），
    // 结果经 %ecx 汇聚（`mov $0x1,%ecx` … `mov $0x0,%ecx` … `mov %ecx,%eax`）
    // —— 正是 **三目** `cond ? 0 : 1` 的 4.1 形态（前置 mov $1 即 else 臂）。
    // 旧 b28d-41 判词 N（手写 Ghidra 式 `(a>>31)*b + (b>>31)*a + hi` +
    // 具名 A/B/P/acc 四局部）会多出整段二次展开与 5 个栈槽；按 P0 全部内联
    // 为单一表达式后 4.4 产物与 ORIG 逐条全等（无具名局部、无栈帧）。
    return ((long long)a * (long long)b > 0x7fffffff) ? 0 : 1;
}
