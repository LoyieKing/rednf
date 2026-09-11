// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSystemTime/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#include "CSystemTime.h"
#include "GlobalData.h"

#include "CSystemTime/CSystemTime.inc"


#include "CSystemTime/dtor_CSystemTime.inc"
{
}

// ============================================================================
// OS_API::GetDateTimeTick（ORIG 0x858c802 T）
// 返回全局 CSystemTime（ORIG 0x941f714 = GlobalData::s_systemTime_）的
// getCurSec()，即当前 Unix 秒（已由 update 周期刷新）。
// 声明见 CInventory.h：namespace OS_API { int GetDateTimeTick(); }
// 注意：必须位于 getCurSec 定义之前——callee 未见定义时 4.4 帧 0x28/槽 -0xc
// 与 ORIG 一致；定义前置会使帧缩为 0x14（probe12/13 实证）。
// ============================================================================
namespace OS_API
{
#include "CSystemTime/GetDateTimeTick.inc"
{
    int tick = GlobalData::s_systemTime_.getCurSec();
    return tick;
}
}

#include "CSystemTime/getCurSec.inc"
{
    return m_sec;
}

#include "CSystemTime/getCurTickCount.inc"
{
    return m_msec;
}

// getCurDate：定义已移入 CSystemTime.h 内联（ORIG W 形，buf 随发）。

#include "CSystemTime/update.inc"
{
    gettimeofday(&m_tv, 0);
    m_sec = m_tv.tv_sec;
    m_msec = (m_sec - m_lastSec) * 1000 + m_tv.tv_usec / 1000;
}

namespace OS_API
{
#include "CSystemTime/snprintf.inc"

}
