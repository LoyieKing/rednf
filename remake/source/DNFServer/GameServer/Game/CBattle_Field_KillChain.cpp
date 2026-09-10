// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CBattle_Field_KillChain/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CBattle_Field_KillChain.cpp —— CBattle_Field 域击杀/奖励链第三阶依赖
// （LinkKillChain 批次；全部经 docs/class_func_reports/CBattle_Field/ 对应报告
//   ## 汇编 + ## 反编译 C 完整读取后忠实转写，无桩）：
//
//   KillHellPartyGroupMonsterCnt   T 0x0830d704（KillHellPartyGroupMonsterCnt.md）
//   checkKilledSpawnBloodMonsters  T 0x08304f8e（checkKilledSpawnBloodMonsters.md）
//   send_elevator_time_packet      T 0x0830d848（send_elevator_time_packet.md）
//   getItemDropDungeonType         T 0x0830aa46（getItemDropDungeonType.md；
//                                    _MakeDropItems 链上依赖，同批补齐）
//   setPremiumDropRate             T 0x0830b848（setPremiumDropRate.md；依赖）
//   _MakeDropItems                 T 0x0830adf6（_MakeDropItems.md，0xa51 字节）
//   _MakeNotiPacketDieMonster      T 0x0830b8cc（_MakeNotiPacketDieMonster.md）
//
// 本 TU 不编辑权威头（CBattle_Field.h/WarField.h 已具全部声明）。
// ============================================================================

#include <string.h>

#include <list>
#include <map>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h         // CBattle_Field/MapInfo/map_item/map_monster
#include "CDungeon.h"              // get_dimension_possible/get_standard_level/…
#include "CDataManager.h"          // G_CDataManager/find_item/find_monster
#include "CGuildServerProxy.h"    // CEventBase / CEventManager::GetRepeatEvent
                                   //   CItemGenerator/CItemGeneratorMgr/CLuckPoint
#include "CItemGeneratorMgr.h"
#include "CSystemTime.h"           // CSystemTime::getCurSec
#include "GlobalData.h"            // s_systemTime_/s_event_manager
#include "LogManager.h"            // LogManager::logFormat
#include "InterfacePacketBuf.h"    // PacketGuard 打包接口
#include "Inven_Item.h"
#include "CEquipItem.h"            // getAvatarPeriod/getAvatarSocket
#include "CItemAmplifier.h"        // stAmplifyOption_t::getAbilityType/Value
#include "GamePODTypes.h"          // stAvatarEmblemInfo_t
#include "BlueMarble.h"            // BlueMarble::getBuff
#include "PvPTypes.h"              // BlueMarbleBuffType
#include "CUserCharacInfo.h"       // get_charac_job/get_charac_level
#include "TimerQueue.h"            // G_TimerQueue/InsertTimer/TIMER_MESSAGE

// CEventBase 虚槽 +0x34 = IsEventing(CUser*)（ORIG _MakeDropItems vcall 实证）。

namespace
{

char const s_bfkc_pretty_kill_hell[] =
    "void CBattle_Field::KillHellPartyGroupMonsterCnt(MapInfo&, map_monster&, bool&)";

char const s_bfkc_pretty_check_blood[] =
    "void CBattle_Field::checkKilledSpawnBloodMonsters(int)";

// G_CDataManager()->getBlueMarbleScript() 返回 void*；ORIG 0x0830b6fa/0x0830b70d
// 为单条 `mov 0x68(%eax)`/`mov 0x6c(%eax)` disp 读（float 系数）。取址算术
// （char*+0x68）在 4.4 恒多一条 add ⇒ 类型化视图（P58，trivial、零符号发射）。
struct BfkcBlueMarbleScriptView
{
    char m_pad00[0x68];
    float m_f1;      // +0x68
    float m_f2;      // +0x6c
};

}  // namespace

// ============================================================================
// KillHellPartyGroupMonsterCnt —— ORIG T 0x0830d704。
// 地狱派对组击杀计数：m_killedAllHellGroups(+0x1ca) 置位直接返回；
// 以 mob.m_hellPartyGroupOrder(+0x336，ORIG movzwl 实证)查 MapInfo::m_hellPartyGroup：
// 未登记 → HELLPARTY_LOG 0x1dd9；组剩余==0 → 移除组并 killedAll=true，
// 组表清空时置 m_killedAllHellGroups；否则剩余-1 且 killedAll=false。
// ============================================================================
#include "CBattle_Field_KillChain/KillHellPartyGroupMonsterCnt.inc"
}

// ============================================================================
// checkKilledSpawnBloodMonsters —— ORIG T 0x08304f8e。
// 血轮小轮收口判定：lastSmall<smallRound 时（未到最后一小轮）仅当
// monsterCount==0 且当前地图非 boss 房图才提前 onFinishBloodRound(false)；
// 否则遍历当前图怪物中属于本 smallRound 者（m_bloodRoundOrder>=smallRound），
// 该 smallRound 相对上一 smallRound 发生轮次或难度变化才推进结算：
// ultimate（dm+0x89f==2）路径走 incUltimateRoundClearPoint/
// forceValuesToChangeRound/setUltimateDifficulty + 0x26 计时器（末轮 1s，
// 否则 10s + sendUltimateRandomDifficulty）；普通路径 BloodSpawnTimerManager(true,false)。
// ============================================================================
#include "CBattle_Field_KillChain/checkKilledSpawnBloodMonsters.inc"
}

// ============================================================================
// send_elevator_time_packet —— ORIG T 0x0830d848。
// 组 0xfd 包：byte(m_elevatorTimer +0x139)、byte(state)；finalize 后 send_to_party。
// ============================================================================
#include "CBattle_Field_KillChain/send_elevator_time_packet.inc"
}

// ============================================================================
// getItemDropDungeonType —— ORIG T 0x0830aa46。
// 无副本=3；维度可入=2；脚本 boss 图(dungeon+0x89c)=1；普通=0。
// ============================================================================
#include "CBattle_Field_KillChain/getItemDropDungeonType.inc"
}

// 血轮或掉落域类型!=0 或绝望之塔或 ref+0x40 低字节!=0 → rate=0；
// premiumRate==0 → -1（禁用）；否则写 premiumRate。
// ORIG 0830b88a movzbl 0x40(%eax)（1 字节读），无 m_pDungeon 空检查；
// 2026-08-30 Fix21_C 修正（原用 4 字节 m_dungeon 指针比较且多加空检查）。
// ============================================================================
#include "CBattle_Field_KillChain/setPremiumDropRate.inc"
}

// ============================================================================
// _MakeNotiPacketDieMonster —— ORIG T 0x0830b8cc。
// 怪物死亡通知包（0x26）：uid、掉落列表逐项（idx/数量/属性/avatar 周期·徽章/
// 强化选项/裸 Inven_Item/头像标志）、mapIndex、APC 击杀数、终点坐标。
// ============================================================================
#include "CBattle_Field_KillChain/_MakeNotiPacketDieMonster.inc"
}

// ============================================================================
// _MakeDropItems —— ORIG T 0x0830adf6（0xa51 字节）。
// 生成器驱动掉落：填 WongWork::stGenerateRefData_t（两遍 blob 写入，中间穿插
// 条件生成调用），经 CItemGeneratorMgr 各 eGenerateType 槽生成 Inven_Item 集，
// 再逐项转 map_item（分配 dropIdx/createTick、蓝矿金币缩放、幸运归属）推入
// itemList 与 MapInfo::Add_Item。
// ref blob 槽位按 ORIG 写入宽度逐条对齐（+0x34..+0x42 为调用方上下文区）。
// ============================================================================
#include "CBattle_Field_KillChain/_MakeDropItems.inc"
}
