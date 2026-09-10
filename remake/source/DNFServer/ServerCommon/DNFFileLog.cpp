// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DNFFileLog/<Func>.inc（不参与编译、内部不含 #include）。
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

// [b28d-11b] P186 判词（_Z41__static_initialization_and_destruction_0ii）：该符号是
// TU 级 static init 入口，ORIG df_game_r 中存在 493 个同名 local 实例
// （`nm -S --defined-only` 实证，地址 0x0807ee76/0x0807ff9a/0x08083c60… 各不同
// 大小 0xba/0x40/0x69…）；compare_tu 按符号名索引只能取到其中一个，未必是本
// TU 对应实例 ⇒ 属 P108/P186 族结构性噪音，非实现缺口，封顶不再对齐。


#include <iostream>
#include "DNFFunctionLib.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

CFileLogWriter* CFileLogWriterInstance() {
    static CFileLogWriter instance;
    return &instance;
}


// ---- CFLog ----

#include "DNFFileLog/CFLog.inc"

#include "DNFFileLog/dtor_CFLog.inc"

#include "DNFFileLog/writeLog.inc"

#include "DNFFileLog/writeLog_1.inc"

// ---- CFileLogWriter（构造/析构已移至 CFileLogWriterBase.cpp，-fno-exceptions） ----

#include "DNFFileLog/writeLog_2.inc"

#include "DNFFileLog/writeRawLog.inc"

// ---- CFileLogWriterInstance ----


// ---- CMyFileLog ----

#include "DNFFileLog/CMyFileLog.inc"

#include "DNFFileLog/operator.inc"

// ---- CMyRawFileLog ----

#include "DNFFileLog/operator_1.inc"

// ---- CToolFileLog ----

#include "DNFFileLog/CToolFileLog.inc"

// DWARF 参数名：CToolFileLog::operator()(this, filename, no, format) —— no 为 int
#include "DNFFileLog/operator_2.inc"
