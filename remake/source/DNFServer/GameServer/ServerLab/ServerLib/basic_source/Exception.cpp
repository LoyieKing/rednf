// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Exception/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include "Exception.h"

namespace nsl {

#include "Exception/Exception.inc"
{
    iErrorCode_ = iErrorCode;
    nLine_ = 0;
    szFunctionNameA_ = NULL;
    szErrorMsg_[0] = '\0';
}

#include "Exception/Exception_1.inc"
{
    iErrorCode_ = 0;
    nLine_ = nLine;
    szFunctionNameA_ = szFunctionNameA;
    szErrorMsg_[0] = '\0';
}

#include "Exception/Exception_2.inc"
{
    iErrorCode_ = iErrorCode;
    nLine_ = nLine;
    szFunctionNameA_ = szFunctionNameA;
}

#include "Exception/Exception_3.inc"
{
    iErrorCode_ = 0;
    nLine_ = 0;
    szFunctionNameA_ = NULL;
}

#include "Exception/Exception_4.inc"
{
    iErrorCode_ = iErrorCode;
    nLine_ = 0;
    szFunctionNameA_ = NULL;

    va_list args;
    va_start(args, szMessage);
    vsprintf(szErrorMsg_, szMessage, args);
    va_end(args);
}

#include "Exception/Exception_5.inc"
{
    nLine_ = nLine;
    szFunctionNameA_ = szFunctionNameA;

    va_list args;
    va_start(args, szMessage);
    vsprintf(szErrorMsg_, szMessage, args);
    va_end(args);
}

} // namespace nsl
