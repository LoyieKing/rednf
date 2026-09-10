// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUnixTimer/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r ORIG 还原 —— CUnixTimer（Game 计时族）
// 参照 objdump：GetNowTime 0x085ed776 / SetLastTime 0x085ed7ae /
// GetTimeInterval 0x085ed7c8 / CUnixTimer::CUnixTimer 0x085ed7f2
// （基类 CDnFTimer::CDnFTimer 0x085ed7e4 仅设 vptr，隐式生成即可对齐）。
#include "CUnixTimer.h"

#include <sys/time.h>

#include "CUnixTimer/CUnixTimer.inc"
{
    // ORIG：call CDnFTimer::CDnFTimer()（设基类 vptr）+ 覆盖本类 vptr；体空。
}

#include "CUnixTimer/GetNowTime.inc"
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

#include "CUnixTimer/SetLastTime.inc"
{
    m_lastTime = GetNowTime();
}

#include "CUnixTimer/GetTimeInterval.inc"
{
    return GetNowTime() - m_lastTime;
}
