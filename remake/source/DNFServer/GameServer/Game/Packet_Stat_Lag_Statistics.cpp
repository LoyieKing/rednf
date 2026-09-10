// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Packet_Stat_Lag_Statistics/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Packet_Stat_Lag_Statistics.h"

#include "Packet_Stat_Lag_Statistics/Packet_Stat_Lag_Statistics.inc"
    : PacketHeader(0x17b9, 0x14c)   // ORIG PacketHeader(0x17b9, 0x14c)（arg1=0x17b9）
    , m_flagA(0)                    // movb $0x0,0xa(%eax)
    , m_flagB(0)                    // movb $0x0,0x5b(%eax)
{
    // m_modules[7]/m_dungeons[9] 子对象由成员声明自动构造
    //（ORIG 循环调 stModuleLagInfo/stDunGeonLagInfo ctor）。
}

// ORIG D1：逆序析构 9×stDunGeonLagInfo、7×stModuleLagInfo（隐式成员析构等效）。
#include "Packet_Stat_Lag_Statistics/dtor_Packet_Stat_Lag_Statistics.inc"
{
}
