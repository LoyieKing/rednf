// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RandGen/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// RandGen：静态 float 随机辅助。ORIG 符号：
//   getFloat01()             T 0x08ae0206
//   getFloat01(XorRand4*)    T 0x08ae0244
//   getFloat(float,float)    T 0x08ae0288
//   getFloat(XorRand4*,ff)   T 0x08ae02a4
// 核心：(x % 65535) * (1/65536)，x 来自 XorRand4；常量 0x8e37cc0 = 0x37800000 = 2^-16。
#include "RandGen.h"
#include "XorRand4.h"

#include "RandGen/getFloat01.inc"


#include "RandGen/getFloat01_1.inc"


#include "RandGen/getFloat.inc"


#include "RandGen/getFloat_1.inc"



