// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CRelayBattleMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r CRelayBattleMgr（G2-4 PvP 链，接力战管理器，PvP_Room +0x620）还原。
// 逐函数对照 docs/class_func_reports/CRelayBattleMgr.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// PvP_Room / CRelayBattle / CRandomValue / CDescending 跨类方法按声明引用
// ORIG 真实符号（不新造桩；链接桩由 PvP_deps.cpp / 主 agent 提供）。
#include <algorithm>
#include <cstring>
#include <vector>

#include "CRelayBattleMgr.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "InterfacePacketBuf.h"
#include "PvP_Room.h"
#include "TimerQueue.h"
#include "PacketGuard.h"

// ============================================================================
// GlobalData（CUserCharacInfo.h 已声明 namespace GlobalData；补缺失成员）
// ============================================================================
namespace GlobalData
{
extern CSystemTime s_systemTime_;
}

// ============================================================================
// 依赖子对象 / 外部函数（PvP_Room.h / TimerQueue.h 权威头提供真实声明）
// ============================================================================

// ---- 随机排序辅助（ORIG 弱符号，定义归 PvP_deps.cpp / 主 agent） ----
class CRandomValue
{
public:
    CRandomValue(std::vector<int>& seq, int a, int b, int c);
    int operator()();

    std::vector<int>* m_pSeq;  // +0x00
    int m_end;                 // +0x04
    int m_start;               // +0x08
    int m_count;               // +0x0c
};

class CDescending
{
public:
    bool operator()(int a, int b);
};

// ---- PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供单例定义） ----


#include "CRelayBattleMgr/Reset.inc"


#include "CRelayBattleMgr/Reset_1.inc"


#include "CRelayBattleMgr/SetParent.inc"


#include "CRelayBattleMgr/OnCreateRoom.inc"


#include "CRelayBattleMgr/OnJoinRoom.inc"


#include "CRelayBattleMgr/OnLeaveRoom.inc"


#include "CRelayBattleMgr/OnDiePlayer.inc"


#include "CRelayBattleMgr/OnChangeTeam.inc"


#include "CRelayBattleMgr/OnReady.inc"


#include "CRelayBattleMgr/OnRequestFight.inc"


#include "CRelayBattleMgr/OnStart.inc"


#include "CRelayBattleMgr/OnFight.inc"


#include "CRelayBattleMgr/GetKillCount.inc"


#include "CRelayBattleMgr/GetTimerKey.inc"


#include "CRelayBattleMgr/GetChangeSequenceCount.inc"


#include "CRelayBattleMgr/GetAllKillUserIndex.inc"


#include "CRelayBattleMgr/IncreaseChangeSequenceCount.inc"


#include "CRelayBattleMgr/DecreaseChangeSequenceCount.inc"


#include "CRelayBattleMgr/ResetChangeSequenceCount.inc"


#include "CRelayBattleMgr/CreateRandomSequenceVector.inc"


#include "CRelayBattleMgr/ArrangeChangeSequence.inc"


#include "CRelayBattleMgr/ArrangeNoChangeSequence.inc"


#include "CRelayBattleMgr/ArrangeMatchSequence.inc"


#include "CRelayBattleMgr/GetNextPlayer.inc"


#include "CRelayBattleMgr/SendRequestFight.inc"


#include "CRelayBattleMgr/TurnPlayer.inc"

