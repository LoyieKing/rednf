#ifndef DNF_GAME_ADVANCE_ALTAR_PROC_STAGE_H_
#define DNF_GAME_ADVANCE_ALTAR_PROC_STAGE_H_

// ============================================================================
// advancealtar_ProcStage.h —— advancealtar::{StageEndType, Timer_StageControl,
// HistoryLog} 权威声明。
//
// [BigP3D 成员化 2026-09-12] CharacData / ProcStage / _SlotItem / _SummonObjectMs /
//   _MapMonsterSummary 的**完整定义已原样上移 Game/advancealtar.h**（StageControl 的
//   子对象必须为真实成员，编译器才会按 [except.ctor] 生成成员构造/销毁序列与 EH
//   landing pad = ORIG C1/D1 形态；4.4.7 真实成员 + 初始化列表探针与 ORIG D1 逐条
//   同形实证）。本头反向 include advancealtar.h（单向，无环）取得同名类型，既有
//   消费点（Dispatchers.cpp / PacketDispatcher_Secu.cpp / advancealtar_Manager.cpp /
//   advancealtar_ProcStage.cpp / advancealtar_StageControl.cpp）零改动即兼容。
//
// 布局证据（df_game_r, elf32-i386）：
//   - ProcStage ctor 0x0812dc18（advancealtar__ProcStage/ProcStage.md 全文）
//   - CharacData ctor 0x0812d024（advancealtar__CharacData/CharacData.md）
#include <cstring>
#include <vector>
//   - StageControl 子对象槽位：getProcStage 0x081348ae（this+0x04）、
//     getCharacData 0x081348a0（this+0x98）——ORIG W thunk objdump 实证。
//   - 访问器 thunk（ORIG W，objdump 实证）：CharacData::setUser 0x08134806
//     写 +0x04、getUser 0x08134814 读 +0x04；ProcStage::getState 0x08134838 /
//     setState 0x08134844（+0x04）、getTimeLine 0x08134852（+0x90）、getMap
//     0x08134860（+0x74）、getStageDifficulty 0x0813486c（+0x68）、
//     getCurrentSurvivalRoundNo 0x08134878（+0x7c+1）、getStartStageTime
//     0x08134886（+0x8c）、getMapBasisLevel 0x08134894（+0x6c）。
//
// 全部子对象已为真实成员（2026-08-25 StageControl 批次）：+0x18
// ProcSummonObjectMs（0x38）、+0x50 map<short,_MapMonsterSummary>、
// +0x80 vector<int>；CharacData +0x10 map<unsigned char,_SlotItem> 同批落地。
// ============================================================================

#include <cstring>

#include "advancealtar.h"               // CharacData/ProcStage/_SlotItem/_SummonObjectMs/_MapMonsterSummary
                                        // （上移后的权威定义；须先于本头其它类型使用点展开）
#include "NOTIPacketStruct.h"           // _NotiAdvanceAltarSpawnUnit/_NotiAdvanceAltarClearStage（ProcStage 发送用）
#include "advancealtar_StageDifficulty.h"  // StageDifficulty::T
#include "AdvanceAltarField.h"     // _RewardItem 完整定义（ShopType 幂等块已收敛）
#include "advancealtar_SummonObject.h"  // b28d-52：_SummonObject 叶子头（_SummonObjectMs::m_summon 真实子对象）
#include "advancealtar_StageDifficulty.h"  // StageDifficulty::T

class CUser;
class CDungeon;
class CMap;
struct MSG_MONSTER_DIE;  // 完整定义 Secu_HackLogCheckByParty.h（killMonster 引用）

namespace advancealtar
{

class StageControl;

// ---- ORIG 枚举 ----
// StageType：权威定义 advancealtar_StageDifficulty.h（STAGE_NORMAL/STAGE_SURVIVAL，
// 自 CDungeon.h 收敛，本头经该叶子取得）。ProcStage::State / StageEndType 仅
// dieCharac/giveUpGame/setState 引用，此前无建模，落在本头。
// StageEndType 权威定义已迁 advancealtar.h（本头 include 该头；此处仅守卫幂等
// 副本，防直接包含本头时缺类型——与权威版同一 DNF_GAME_ADVANCE_ALTAR_STAGE_END_TYPE_DEFINED）。
#ifndef DNF_GAME_ADVANCE_ALTAR_STAGE_END_TYPE_DEFINED
#define DNF_GAME_ADVANCE_ALTAR_STAGE_END_TYPE_DEFINED
struct StageEndType
{
    enum T
    {
        END_N0 = 0,     // isEndStage 全清死亡标记 [名推断]
        END_N1 = 1,     // dieCharac / RegistTimerClearReward
        END_N2 = 2,     // giveUpGame
        END_N3 = 3      // isEndStage 全阶段完成 [名推断]
    };
};
#endif

// ---- Timer_StageControl（静态语义函数，asm 不使用 this）----
struct Timer_StageControl
{
    static void RegistTimerCheckFinsh(int index);                             // T 0x0812f850
    static void RegistTimerEPLP(int index, StageEndType::T endType);          // T 0x0812f8ec
    static void RegistTimerEPLPReturnVillage(int index);                      // T 0x0812f956
    static void RegistTimerClearReward(int index, StageEndType::T endType);   // T 0x0812f89c
};

// ---- 全局自由函数 ----
void getMemberNames(CUser* user, char* buf);   // T 0x0812cf54

}  // namespace advancealtar

#endif  // DNF_GAME_ADVANCE_ALTAR_PROC_STAGE_H_
