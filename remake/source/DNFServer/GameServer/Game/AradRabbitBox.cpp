// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AradRabbitBox/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CRabbitBox ORIG 符号实例化 TU（头内 inline 定义，本 TU 强制发出弱符号）。
// [AnonLane2 2026-09-11 试形-回滚] 曾删本垫片：全局符号集与逐符号 verdict
// 全等（STStackableScript.cpp 等提供同名 W），但本 TU 四数 6/4/0/0/1 → 0/0/0/0/0
// （identical/ae 归属转移），按「TU 四数不劣化」口径回滚，判词保留。
#include "AradRabbitBox.h"

namespace
{
#include "AradRabbitBox/forceEmit.inc"
{
    CRabbitBox a;
    CRabbitBox b(a);              // C1 拷贝构造 0x0852488c
    CRabbitBox c;
    c = b;                        // operator= 0x08197ec2
    volatile int v = b.getItemIndex() + b.getItemCount();  // 0x08197e9e / 0x08197eaa
    b.setItemIndex(1);            // 0x089c3338
    b.SetItemCount(2);            // 0x089c3346
    b.SetDropRate(3);             // 0x089c3354
    delete new CRabbitBox();      // D0 0x08197e80（内部 call D1 0x08197e5a）
    (void)a; (void)c; (void)v;
}
}
