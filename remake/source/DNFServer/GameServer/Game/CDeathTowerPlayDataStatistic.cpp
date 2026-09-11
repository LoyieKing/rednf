// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDeathTowerPlayDataStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CDeathTowerPlayDataStatistic.h"

#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "Packet_DeathTower_Statistic_Playdata_Job.h"
#include "Packet_DeathTower_Statistic_Playdata_Party.h"

// P86/P30：ORIG 三处 `imul $0x17,%eax,%eax; add 0x14(%ebp),%eax` 前向类型
// 真实下标访问形（probe pr4.cpp k1 实证：形参指针 + 完整类型下标出 add mem
// 形；TU 局部 MemberView 视图则恒 lea 双步）。本 TU 补全定义（仅类型，
// 不实例化不发射符号；stride 0x17 与 ORIG 访存偏移一致）。
class stDeathTowerRecordMemberInfo_t
{
public:
    char m_pad[0x14];
    unsigned char m_level;   // +0x14
    unsigned char m_job;     // +0x15
    unsigned char m_grow;    // +0x16
};

#include "CDeathTowerPlayDataStatistic/SendDeathTowerPlayInfo.inc"

