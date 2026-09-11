// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_Script/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Arad_Script.h"

// ============================================================================
// [b28d-64 判词汇总 · 泳道64] 本 TU 4 件（ORIG 地址见各报告 /tmp/b28d_lane_64_result.tsv）：
//   AradJumping_Script::importScript 0x089... 92/102 N封顶：P167 CLEAR 39/39；
//     残余 = cMyTrace 字符串重定位 + nop 岛落点 + %esi/%ebx 实参保活序（N80/N7）。
//   Arad_Script::Load(PKc)           418/421 N封顶：P167 CLEAR 39/39；槽序/寄存器
//     （本 TU 路由 -O2 + gnu++0x，ORIG 为 -O0）。
//   Arad_Script::Load(PKc, S1)       397/400 N封顶：P167 CLEAR 31/31；同上。
//   ARAD::SCRIPT::importAradEventScript 340/360 N封顶：P167 EXTRA
//     SEventPeriodData/SRewardPeriodData::~D1 ×2（4.4 对 vector 元素/临时多发析构 + EH）；
//     MISSING 空 ⇒ 语义完成、N91 封顶。
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>

// [moved] include 区统一收敛到文件头（中部 include 在 lsd44 链下会污染后续标准头）

#include "Arad_Script/Arad_Script.inc"


#include "Arad_Script/Arad_Script_1.inc"


#include "Arad_Script/GetRowNum.inc"


#include "Arad_Script/dtor_Arad_Script.inc"


#include "Arad_Script/Create.inc"


#include "Arad_Script/ExpandCol.inc"


#include "Arad_Script/ExpandRow.inc"


#include "Arad_Script/make_file_path.inc"


#include "Arad_Script/GetIdx4Col.inc"


#include "Arad_Script/GetIdx4Row.inc"


#include "Arad_Script/GetInt.inc"


#include "Arad_Script/GetInt_1.inc"


#include "Arad_Script/GetInt_2.inc"


#include "Arad_Script/GetInt_3.inc"


#include "Arad_Script/GetFloat.inc"


#include "Arad_Script/GetFloat_1.inc"


#include "Arad_Script/GetFloat_2.inc"


#include "Arad_Script/GetFloat_3.inc"


#include "Arad_Script/GetDouble.inc"


#include "Arad_Script/GetDouble_1.inc"


#include "Arad_Script/GetDouble_2.inc"


#include "Arad_Script/GetDouble_3.inc"


#include "Arad_Script/GetString.inc"


#include "Arad_Script/GetString_1.inc"


#include "Arad_Script/GetString_2.inc"


#include "Arad_Script/GetString_3.inc"


#include "Arad_Script/SetInt.inc"


#include "Arad_Script/SetInt_1.inc"


#include "Arad_Script/SetInt_2.inc"


#include "Arad_Script/SetInt_3.inc"


#include "Arad_Script/SetString.inc"


#include "Arad_Script/SetString_1.inc"


#include "Arad_Script/SetString_2.inc"


#include "Arad_Script/SetString_3.inc"


#include "Arad_Script/Load.inc"


#include "Arad_Script/Load_1.inc"


// ============================================================================
// W6_ARAD.ScriptPods —— ORID df_game_r 的 ARAD::SCRIPT 域残余真实现清零批。
//
// 覆盖 ORIG 符号（nm -S 实证，全部弱/T）：
//   _ZN4ARAD6SCRIPT16SEventPeriodDataC1Eijj    W 0x088b4368（【objdump】三字段直存）
//   _ZN4ARAD6SCRIPT16SEventPeriodDataD1Ev      W 0x088b558a（5B 空体【objdump】）
//   _ZN4ARAD6SCRIPT16SEventPeriodDataeqERKS1_  W 0x088b4388（仅比 +0x00【objdump】）
//   _ZN4ARAD6SCRIPT17SRewardPeriodDataD1Ev     W 0x088b5764（5B 空体【objdump】）
//   _ZN4ARAD6SCRIPT17SRewardPeriodDataeqERKS1_ W 0x088b43da（仅比 +0x00【objdump】）
//   _ZN4ARAD6SCRIPT22_AradJumpingRewardItemD1Ev W 0x088b597e（5B 空体【objdump】）
//   _ZN4ARAD6SCRIPT21AradJumpingScriptDataC1Ev W 0x0837170a / D1Ev W 0x0837171e
//   _ZN4ARAD6SCRIPT18AradJumping_ScriptC1Ev    W 0x08371732 / D1Ev W 0x08371746
//   _ZN4ARAD6SCRIPT18AradJumping_Script17getRDARScriptDataEv W 0x088b43ee
//   _ZN4ARAD6SCRIPT18AradJumping_Script12importScriptEPcRS1_ T 0x088b4128
//   _ZN4ARAD6SCRIPT8readTimeEPs                T 0x088b3970（报告 ARAD__SCRIPT/readTime.md）
//   _ZN4ARAD6SCRIPT10uint32TimeEPs             T 0x088b3b4b（报告 ARAD__SCRIPT/uint32Time.md）
//   _ZN4ARAD6SCRIPT21importAradEventScriptE... T 0x088b3bd8（报告 importAradEventScript.md）
// getCurRewardItem(T 0x088b424a) 已由 AradPeriodB.cpp 批次提供，此处不重复定义。
//
// Ctor/Dtor 族说明：ORIG AradJumpingScriptData C1 仅调用 vector<_AradJumpingRewardItem>
// 默认构造；AradJumping_Script C1/D1 直接转发基类 —— 空体定义 + vector 成员即忠实对应。
// ============================================================================
#include <algorithm>
#include "CDataManager.h"               // ARAD::SCRIPT::AradJumping*（m_aradJumping 权威段）
#include <ctime>

#include "DNFLexWrapper.h"               // loadRDARScriptFile / ScanType / ScanInt(bool*)
#include "LogManager.h"                  // LogManager::logFormat(int, file, func, line, fmt, ...)
#include "Arad_EventPeriodDataManager.h" // SEventPeriodData / SRewardPeriodData / readTime 声明

namespace ARAD {
namespace SCRIPT {

// [AnonLane2 2026-09-11 匿名轴] 旧 TU 级 `kRDAREtcParamFile` 已删（ORIG 全库对
// 该名零命中，ORIG 侧仅匿名 .rodata 串），使用点内联同值字面量。

#include "Arad_Script/SEventPeriodData.inc"


bool SEventPeriodData::operator==(SEventPeriodData const& other)
{
    return m_eventCode == other.m_eventCode;   // 【objdump】仅比 +0x00
}

#include "Arad_Script/readTime.inc"


bool SRewardPeriodData::operator==(SRewardPeriodData const& other)
{
    return m_eventCode == other.m_eventCode;   // 【objdump】仅比 +0x00
}

// ---- uint32Time -------------------------------------------------------------
// 报告 ARAD__SCRIPT/uint32Time.md 全文转写：
//   time(NULL) → localtime → 改 y(-1900)/m(-1)/d/h（min/sec=0）→ mktime，
//   返回值沿用 mktime 返回（ORIG eax 直通）。年/月加载后 cwtl/movswl 符号扩展。
unsigned int uint32Time(short* t)
{
    time_t now = ::time(0);
    tm* lt = ::localtime(&now);
    lt->tm_year = (int)*t + -1900;       // lea -0x76c(%eax)
    lt->tm_mon  = (int)t[1] + -1;        // lea -0x1(%eax)
    lt->tm_mday = (int)t[2];             // movswl
    lt->tm_hour = (int)t[3];             // movswl
    lt->tm_min  = 0;
    lt->tm_sec  = 0;
    return (unsigned int)::mktime(lt);
}

}  // namespace SCRIPT

// ============================================================================
// AradJumping 族生命周期（见 CDataManager.h 中声明段注释的 ORIG 地址清单）。
// ============================================================================
namespace SCRIPT {

// [b28d-solo] _AradJumpingRewardItem 无用户 dtor（P317/P12）：ORIG D1/D2 W 0x088b597e
// 由 new_allocator<T>::destroy/construct 使用点发射为 W；显式空体定义会使类型
// non-trivial 并让 importScript 多发 D1 call + EH landing（ORIG 无）。

#include "Arad_Script/AradJumpingScriptData.inc"


#include "Arad_Script/dtor_AradJumpingScriptData.inc"


#include "Arad_Script/AradJumping_Script.inc"


#include "Arad_Script/dtor_AradJumping_Script.inc"


#include "Arad_Script/getRDARScriptData.inc"


#include "Arad_Script/importScript.inc"

#include "Arad_Script/importAradEventScript.inc"


}  // namespace SCRIPT
}  // namespace ARAD
