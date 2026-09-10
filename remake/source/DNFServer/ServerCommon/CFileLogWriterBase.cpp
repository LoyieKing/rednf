// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFileLogWriterBase/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "DNFFileLog.h"

// 原始：CFileLogWriter 构造/析构在独立 TU（-fno-exceptions，无 <iostream> 初始化桩）。
// 反汇编验证：原始 C1/C2/D1/D2 均无 EH 清理（无 map D1/_Unwind_Resume），
// 而 writeLog/writeRawLog 含 CGuard 隐式清理（-fexceptions）。
//
// 构建：必须 -fno-exceptions（coserver build-coserver.sh 已特殊处理；
// auction 侧 build-auction.sh 需同样对 CFileLogWriterBase.cpp 加 -fno-exceptions，
// 否则 gcc 4.4 对 map 成员构造固定生成 EH landing pad / _Unwind_Resume）。

#include "CFileLogWriterBase/CFileLogWriter.inc"

#include "CFileLogWriterBase/dtor_CFileLogWriter.inc"
