// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CommonTime/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CommonTime 定义在 ../CommonTime.h。本 TU 只放方法。
// ORIG SetCurTime 0x082a4224 写到 +0x0c，对象大小 0x10。
#include "../CommonTime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static_assert(sizeof(CommonTime) == 0x10, "CommonTime is 0x10");
static_assert(__builtin_offsetof(CommonTime, m_wday) == 0x05, "m_wday");
static_assert(__builtin_offsetof(CommonTime, m_curTime) == 0x08, "m_curTime");
static_assert(__builtin_offsetof(CommonTime, m_createTime) == 0x0c, "m_createTime");


#include "CommonTime/SetCreateTime.inc"


#include "CommonTime/SetCreateTime_1.inc"


#include "CommonTime/SetCurTime.inc"


#include "CommonTime/clear.inc"


#include "CommonTime/GetCreateTime.inc"


#include "CommonTime/MakeTime.inc"


CommonTime::operator long() const
{
    tm tmv;
    MakeTime(tmv);
    return mktime(&tmv);
}
