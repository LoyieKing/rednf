// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDeathTowerPlayDataStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CDeathTowerPlayDataStatistic::SendDeathTowerPlayInfo。
// ORIG 0x0860f45c T
// （_ZN28CDeathTowerPlayDataStatistic22SendDeathTowerPlayInfoEicPK30stDeathTowerRecordMemberInfo_tjj）。
// 报告：docs/class_func_reports/CDeathTowerPlayDataStatistic/SendDeathTowerPlayInfo.md
// 语义：按 idx 取成员记录（stride 0x17）的 +0x14 level 低字节，构造并发送
//   Job 统计包（0xc3d, 0x16）；level==0 时仅 log 错误不发送。随后无条件
//   构造并发送 Party 统计包（0xc3f, 0x10）。恒返回 1（__PRETTY_FUNCTION__
//   证据显示 ORIG 原返回类型为 bool，ABI 等价 int）。
// 两个 Packet 类的弱内联 ctor（ORIG 0x0860ffe6 / 0x0861000a）在各自头文件
// 内联定义（C1/C2 弱符号与 ORIG 一致），本 TU 仅实现 SendDeathTowerPlayInfo。
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
}
