// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CVillageMonsterMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— village_attacked::CVillageMonsterMgr
// 独立 TU。OnKillVillageMonster(int,int,unsigned short,bool)/OnChangeState/
// SendVillageMonsterFightResult/MakeVillageMonsterListPacket/SendVillageMonsterList
// /GetRewardType 由 VillageMonster_helpers.cpp 提供（本 TU 不重复定义）。
// 逐函数对照 ORIG df_game_r 反汇编（证据见各函数注释）。

#include "CVillageMonsterMgr.h"

#include <cstdlib>

#include "CGameManager.h"   // CUser / CParty / pvp_assault::Point2d 完整类型
#include "PacketGuard.h"
#include "GameWorld.h"      // G_GameWorld / send_all / send_to_area
#include "TimerQueue.h"     // G_TimerQueue / InsertTimer
#include "GameRand.h"       // get_rand_int

#include "CVillageMonsterMgr/CVillageMonster.inc"


// ============================================================================
// CParty 村战状态区局部视图（ORIG：CBattle_Field(+0xb24) 嵌入子区 BF+0x128）。
// CParty.h 尚未具名该子区；本 TU 以局部视图按 ORIG 偏移具名访问（不跨 TU）。
// 字段对应：OnFightVillageMonster 0x86b4672 写 / OnKillVillageMonster 0x86b4866 读。
// ============================================================================

#include "CVillageMonsterMgr/include.inc"


#include "CVillageMonsterMgr/GetDistance.inc"

// ============================================================================
// pvp_assault::IsIncludeVirtualMoveAreas —— ORIG T 0x82e5a0b（村战 OnRun 依赖）。
//   遍历虚拟移动区：点落在某区 margin 内 → 返回 1（outX/outY 维持原坐标）；
//   否则取各区域近边缘中距原点的最小值（margin!=0 写绝对坐标，margin==0 写
//   增量），全程未命中返回 0。
// ============================================================================
namespace pvp_assault
{

#include "CVillageMonsterMgr/IsIncludeVirtualMoveAreas.inc"


}  // namespace pvp_assault

#include "CVillageMonsterMgr/GetDistance_1.inc"
{
    // ORIG 0x82e59f1 尾调 Point2d::operator-（= |dx|+|dy| 曼哈顿距离）。
    // 语义等价（ORIG 传参形态差异属编译器形式；operator- 实现一致）。
    return a - b;   // ORIG 尾调 Point2d::operator-（曼哈顿距离 10 insns，我们内联 29）
}

// ---- ORIG 域常量（nm：_ZN16village_attackedL…E，r 段；值自 ORIG rodata 读取）----
namespace village_attacked
{
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
static const int MONSTER_REGEN = 1;                // 0x08bc43c0
static const int MONSTER_MOVE_PIXEL = 300;         // 0x08bc43b0 (0x12c)
static const int MONSTER_UPDATE_RATE = 1;          // 0x08bc43b8
static const int MAX_START_TIME_COUNT = 3;         // 0x08bc4394
static const int start_time_count_next[3] = {5, 4, 1};     // 0x08bc43a4 (0xc 字节)
static const int MIN_MONSTER_MOVE_PIXEL = 50;      // 0x08bc43b4 (0x32)
static const int start_time_count_remain[3] = {10, 5, 1};  // 0x08bc4398 (0xc 字节)
static const int MONSTER_MOVE_RANDOM_CONST = 3;    // 0x08bc43bc

// ============================================================================
// CVillageMonster area 构造 —— ORIG 0x86b3752
// ============================================================================

#include "CVillageMonsterMgr/CVillageMonsterArea.inc"


#include "CVillageMonsterMgr/OnMoveArea.inc"


#include "CVillageMonsterMgr/OnMoveArea_1.inc"


#include "CVillageMonsterMgr/OnEvent.inc"


#include "CVillageMonsterMgr/InsertVillageMonster.inc"


#include "CVillageMonsterMgr/CVillageMonster_1.inc"


#include "CVillageMonsterMgr/dtor_CVillageMonster.inc"


#include "CVillageMonsterMgr/GetUID.inc"


#include "CVillageMonsterMgr/OnRunVillageMonster.inc"


#include "CVillageMonsterMgr/OnFightVillageMonster.inc"


#include "CVillageMonsterMgr/MakeVillageMonsterPositionPacket.inc"


#include "CVillageMonsterMgr/OnRunVillageMonster_1.inc"


#include "CVillageMonsterMgr/SendVillageMonsterPosition.inc"


#include "CVillageMonsterMgr/GetAttackedMonster.inc"


#include "CVillageMonsterMgr/OnCreateVillageMonster.inc"


#include "CVillageMonsterMgr/OnDestroyVillageMonster.inc"


#include "CVillageMonsterMgr/OnFightVillageMonster_1.inc"


#include "CVillageMonsterMgr/CVillageMonsterMgr.inc"


#include "CVillageMonsterMgr/dtor_CVillageMonsterMgr.inc"


#include "CVillageMonsterMgr/OnStartVillageAttacked.inc"


#include "CVillageMonsterMgr/OnCreateVillageMonster_1.inc"


#include "CVillageMonsterMgr/OnDestroyVillageMonster_1.inc"
{
    for (std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.begin();
         it != m_areas.end(); ++it)
    {
        it->second.OnDestroyVillageMonster();
    }
    m_field04 = 0;
    m_field00 = 0;
    // P95：ORIG 0x86b43d4 末段 store 先载 this 后载 reward；具名视图复现。
    struct CVMgrRewardView
    {
        char m_head[0x20];
        int  m_field20;
    };
    ((CVMgrRewardView*)this)->m_field20 = (int)reward;
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf2);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
}

#include "CVillageMonsterMgr/OnEndRewardVillageMonster.inc"


#include "CVillageMonsterMgr/OnRunVillageMonster_2.inc"


#include "CVillageMonsterMgr/OnRegenVillageMonster.inc"


#include "CVillageMonsterMgr/OnKillVillageMonster.inc"


#include "CVillageMonsterMgr/OnFightVillageMonster_2.inc"


}  // namespace village_attacked
