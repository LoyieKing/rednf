// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBattle_Field/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CBattle_Field（G2-3 第一批）。
// 逐函数对照 docs/class_func_reports/CBattle_Field.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径，分支/调用目标→<T>、
// 大绝对地址→<A>、去@plt、剥尾部 nop、C2/D2 同址别名良性）。
// 已并入原 CBattle_Field_map.cpp（地图/移动小簇）；草稿文件已删除。
#include <cstring>

#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "InterfacePacketBuf.h"
#include "TimerQueue.h"
#include "PacketGuard.h" // 权威 PacketGuard(0xc)
#include "CQuickPartyRewardManager.h"
#include "GameRand.h"  // get_rand_int

// ============================================================================

// ============================================================================
// 外部方法（权威头已提供声明：MazeScript::getGridR 见 CDungeon.h，
// CSystemTime::getCurTickCount 见 CSystemTime.h，CRidable::CheckAppearRidableObject
// 见 CRidable.h；GlobalData::s_systemTime_ 见 CUserCharacInfo.h）
// ============================================================================
CDataManager* G_CDataManager();

// CDungeon 局部布局代理（真实 CDungeon.h 已含 +0x8a0 字段布局的完整类型）：
// 本代理仅用于 +0x8a0 字段直接位移访问，保持 ORIG 直接位移加载形态。
struct CDungeon_8a0_proxy
{
    char m_pad[0x8a0];
    int m_8a0;
};

// G_CDataManager() +0x6170 处的 int 表视图（incUltimateRoundClearPoint 08307096
// 实证 add $0x185c,%edx + (%eax,%edx,4) 寻址 = 元素索引 v+0x185c 的成员数组
// 折算形态；视图仅本 TU 使用）
struct CDataManager_UltimateArr_proxy
{
    char m_pad[0x6170];
    int m_arr[1];
};

// ============================================================================
// 小访问器（ORIG 弱符号，直接读反汇编实现）
// ============================================================================

#include "CBattle_Field/SetTournamentCurRound.inc"


#include "CBattle_Field/SetParty.inc"


#include "CBattle_Field/SetCurSeatNo.inc"


#include "CBattle_Field/GetMapInfoKeyValue.inc"


#include "CBattle_Field/GetCurrentMapInfo.inc"


#include "CBattle_Field/GetMapInfoFromPos.inc"


#include "CBattle_Field/getMapWidth.inc"


#include "CBattle_Field/getMapHeight.inc"


#include "CBattle_Field/check_start_point.inc"


#include "CBattle_Field/check_end_point.inc"


#include "CBattle_Field/get_cur_map.inc"


#include "CBattle_Field/GetMapInfo.inc"


#include "CBattle_Field/getMaze.inc"


#include "CBattle_Field/GetCurrentMapIndex.inc"


#include "CBattle_Field/IsClearDungeon.inc"


#include "CBattle_Field/get_dungeon_index.inc"


#include "CBattle_Field/get_dungeon_diff.inc"


#include "CBattle_Field/GetDungeon.inc"


#include "CBattle_Field/GetMonsterMap.inc"


#include "CBattle_Field/get_map_index.inc"


#include "CBattle_Field/GetCurrentMap.inc"


#include "CBattle_Field/ClearCondition.inc"


#include "CBattle_Field/IsVisitedCurrentMap.inc"


#include "CBattle_Field/setWorldMapTowerArea.inc"


#include "CBattle_Field/isWorldMapTowerArea.inc"


#include "CBattle_Field/GetCurPosX.inc"


#include "CBattle_Field/GetCurPosY.inc"


#include "CBattle_Field/GetLastMapInfo.inc"


#include "CBattle_Field/getCurPosXY.inc"


#include "CBattle_Field/getLastPosXY.inc"


#include "CBattle_Field/swap_curpos_xy.inc"


// ============================================================================
// 小型访问器（读 ORIG 反汇编逐个实现）
// ============================================================================

#include "CBattle_Field/getBloodState.inc"


#include "CBattle_Field/SetEnableHellDungeon.inc"


#include "CBattle_Field/setUltimateDifficulty.inc"


#include "CBattle_Field/getUltimateDifficylty.inc"


#include "CBattle_Field/IsAssignNPCByPotion.inc"


#include "CBattle_Field/IsEnableHellDungeon.inc"


#include "CBattle_Field/set_dungeon_diff.inc"


#include "CBattle_Field/setBloodState.inc"


#include "CBattle_Field/GetTournamentCurRound.inc"


#include "CBattle_Field/IsFirstEnterDungeon.inc"


#include "CBattle_Field/ResetFirstEnterDungeon.inc"


#include "CBattle_Field/getLayeredMapIndex.inc"


#include "CBattle_Field/setLayeredMapIndex.inc"


#include "CBattle_Field/getLayeredMapKeyValue.inc"


#include "CBattle_Field/getLayeredMapInfoIndex.inc"


#include "CBattle_Field/getCurrentLayeredMapInfo.inc"


#include "CBattle_Field/IsKilledAllHellGruoups.inc"


#include "CBattle_Field/SetRidableObjectState.inc"


#include "CBattle_Field/CheckAppearRidableObject.inc"


#include "CBattle_Field/GetFieldMob.inc"


#include "CBattle_Field/getBloodMaxRound.inc"


#include "CBattle_Field/BloodRoundMaxTimePacket.inc"


#include "CBattle_Field/getNextRoundRandomDifficulty.inc"


// ============================================================================
// 嵌套类型方法（ORIG 弱符号）
// ============================================================================

#include "CBattle_Field/TournamentInfo.inc"


#include "CBattle_Field/Clear.inc"


bool CBattle_Field::TournamentInfo::operator<(const TournamentInfo& other) const
{
    if (m_group < other.m_group)
        return true;
    return false;
}

#include "CBattle_Field/setBloodRoundOrder.inc"


#include "CBattle_Field/setPreMapBloodRoundOrder.inc"


#include "CBattle_Field/incBloodRoundOrder.inc"


#include "CBattle_Field/getBloodRoundOrder.inc"


#include "CBattle_Field/setBloodSmallRoundOrder.inc"


#include "CBattle_Field/incBloodSmallRoundOrder.inc"


#include "CBattle_Field/getBloodSmallRoundOrder.inc"


#include "CBattle_Field/setBloodLastSmallRoundOrder.inc"


#include "CBattle_Field/getBloodLastSmallRoundOrder.inc"


#include "CBattle_Field/reset_blood_values.inc"


#include "CBattle_Field/reset_blood_values_1.inc"


#include "CBattle_Field/stRewardItem_t.inc"


#include "CBattle_Field/reset.inc"


#include "CBattle_Field/CBloodClearRewardData.inc"


#include "CBattle_Field/reset_1.inc"


#include "CBattle_Field/onStartBloodRound.inc"


#include "CBattle_Field/addPlayTime.inc"


#include "CBattle_Field/getPlayTime.inc"


#include "CBattle_Field/getBloodRoundClearTime.inc"


#include "CBattle_Field/getCurrentBloodRound.inc"


#include "CBattle_Field/setCurrentBloodRound.inc"


#include "CBattle_Field/getRewardItemCount.inc"


#include "CBattle_Field/getRewardItem.inc"


#include "CBattle_Field/getRewardExp.inc"


#include "CBattle_Field/setRewardExp.inc"


#include "CBattle_Field/setRewardItem.inc"


#include "CBattle_Field/setBloodClear.inc"


#include "CBattle_Field/isBloodClear.inc"


#include "CBattle_Field/getUltimateRoundClearPoint.inc"


#include "CBattle_Field/incUltimateRoundClearPoint.inc"


#include "CBattle_Field/CTowerOfDespairRewardData.inc"


#include "CBattle_Field/dtor_CTowerOfDespairRewardData.inc"


#include "CBattle_Field/reset_tod.inc"


#include "CBattle_Field/stConditionEnterBossRoomStatus_t.inc"


#include "CBattle_Field/stConditionEnterBossRoomStatus_t_1.inc"


#include "CBattle_Field/dtor_stConditionEnterBossRoomStatus_t.inc"


CBattle_Field::stConditionEnterBossRoomStatus_t&
CBattle_Field::stConditionEnterBossRoomStatus_t::operator=(
    const stConditionEnterBossRoomStatus_t& other)
{
    m_monsterIdx = other.m_monsterIdx;
    m_pos = other.m_pos;
    m_curKillCount = other.m_curKillCount;
    m_maxKillCount = other.m_maxKillCount;
    return *this;
}

// [b28d-Main 代落·S324 P299/P317] 用户 ctor/dtor 定义删除（声明端同步）：
// 隐式 ctor 由使用点发射 W（含 m_pos 的 pair 默认构造），隐式 trivial dtor 的
// D1/D2 由 new_allocator::destroy 发射 W——与 ORIG 0830ee4c / 08316adc 同形。

// ============================================================================
// 外部依赖（CGameManager.h 与本头组合存在重定义冲突，按 ORIG 符号做最小声明，
// 链接解析到真实定义 —— 与 QuickParty.cpp 同一模式）
// ============================================================================
class CGameManager
{
public:
    QuickParty::CQuickPartyRewardManager* GetQuickPartyRewardManager();
};
CGameManager* G_CGameManager();

// WongWork::CDeathTower 最小完成类型（CUser.h 仅前置声明；非虚方法，仅用 this）。
namespace WongWork
{
class CDeathTower
{
public:
    int genTimerKey(TIMER_MESSAGE msg);   // ORIG 0x8142c46 域内（见 CGameManager.h 声明）
    int getIdx();
};
}

#include "CBattle_Field/getRandomBuffDungeonType.inc"


#include "CBattle_Field/setEplpWaitingTimer.inc"

