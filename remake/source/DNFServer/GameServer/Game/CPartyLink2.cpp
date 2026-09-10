// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPartyLink2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CPartyLink2.cpp —— CParty/CDataManager 域链接期符号恢复（2026-08-25，LinkPartyData2）
//
// 覆盖 5 个 ORIG 符号（全部经 docs/class_func_reports/ 对应报告
// ## 汇编 + ## 反编译 C 完整读取后忠实转写）：
//   CParty::kill_monster            T 0x085a27e8（kill_monster.md，0x1381 字节）
//   CParty::SendSecretShopItemList  T 0x085b9ecc（SendSecretShopItemList.md）
//   CParty::setBestClearTime        T 0x085be178（setBestClearTime.md）
//   CBattle_Field::convertAPCRoleType2MonsterRoleType
//                                   T 0x0830a812（objdump 转写；CBattle_Field.h:385
//                                    已声明成员，完整类型为 CParty 子对象视图，
//                                    以 ORIG mangled 提供定义）
//   CAICharacter::getPvPWinPoint    T 0x0834a240（getPvPWinPoint.md；CAICharacter
//                                    完整类为 CharacterAIA2.cpp TU 局部，无权威头，
//                                    以 ORIG mangled 提供定义，
//                                    +0x1c/+0x20 布局经 set.md 实证）
//
// 本 TU 不编辑 CParty.cpp/CParty.h/CBattle_Field.*，全部经既有权威头/视图访问。
// clear_reward(int) 与 set_clear_reward_exp_parameter 由 CClearRewardBatch2 批次负责，
// 不在本域。
// ============================================================================

#include <algorithm>
#include <string.h>

#include "QuickParty.h"       // 必须先于 CDungeon.h/CParty.h（守卫惯例，与 CParty.cpp 同）
#include "CParty.h"
#include "CDataManager.h"     // m_rankSystem / m_aiCharacterList / CAICharacterList
#include "MonsterIndependentDrop.h"  // CAICharacter 完整镜像（getPvPWinPoint 真实调用）
#include "CUser.h"            // CUser/CUserCharacInfo/CHackAnalyzer/cUserHistoryLog
#include "CDungeon.h"         // CDungeon::IsEnterEachMap / get_exp_weight / get_index
#include "CMap.h"             // CMap 完整类型（GetCurrentMap->m_f0）
#include "WarField.h"         // map_monster 完整定义
#include "CBattle_Field.h"    // CBattle_Field 完整类型（bf 子对象视图）
#include "CBattle_Field_shared.h"  // STKillMonsterData
#include "BattleData.h"       // BattleData 完整类型（bd 子对象视图）
#include "CExpandEquipslot.h"  // GetCharacExpandData 返回类型完整化（上行 CData）
#include "ActiveStaticInfo.h" // ActiveStaticInfo::getHpMax
#include "GameRand.h"         // get_rand_int
#include "LogManager.h"       // cMyTrace
#include "GlobalData.h"       // s_event_manager
#include "CGuildServerProxy.h"// CEventManager::GetRepeatEvent / CEventBase
#include "ServiceRestrictManager.h"  // ServiceRestrictManager / RestrictCategory
#include "Arad_DataManager.h" // ARAD::Singleton<T> 权威模板
#include "BlueMarble.h"       // BlueMarble::getBuff
#include "PvPTypes.h"         // BlueMarbleBuffType::T
#include "BestClearTime.h"    // BestClearTime::setBestClearTime
#include "CSecretShop.h"      // secretshop::CSecretShop::SendSecretShopItemList
#include "SECRET_SHOP_DATA.h" // secretshop::SALE_INFO
#include "HistoryLog.h"       // HistoryLog::WriteKillMob
#include "Secu_HackLogCheckByParty.h"  // Secu_HackLogCheckByParty::DieMob / MSG_MONSTER_DIE
#include "InterfacePacketBuf.h"
#include "GameEnums.h"        // ENUM_DUNGEON_CLEAR_CONDITION_TYPE
#include "CPartyExpBonusEvent.h"   // CPartyExpBonusEvent::getBonusRate（ORIG W 0x85bfe1a）

namespace
{

// 子对象视图（与 CParty.cpp / CPartyDepsD2.cpp 同惯例）。
// P49：-O0 下 inline 助手仍发射真实 call 符号（p167 EXTRA
// _ZN12_GLOBAL__N_1{bf,bd,traceHack}EP6CParty），而 ORIG 均为内联子对象访问
// ⇒ 改宏：零符号、逐点展开，语义不变。
#define LINK2_BF(p)        ((p)->m_battleField)
#define LINK2_BD(p)        ((p)->m_battleData)
// [b28d-solo-CParty2] CParty.h 字段化：+0x210 区为具名成员 m_traceMobDieHack。
#define LINK2_TRACEHACK(p) ((p)->m_traceMobDieHack)

}  // namespace

// ============================================================================
// CBattle_Field::convertAPCRoleType2MonsterRoleType(char) const —— ORIG T 0x0830a812。
// 5→0（普通） 6→1（Boss） 7→2（精英） 8→3（特殊），其余原样返回。
// 声明在 CBattle_Field.h:400；本 TU 为该符号唯一定义点。§4.5：删 asm 别名桥，
// 改真实成员定义，调用点自然发射同名 T 符号。
// ============================================================================
#include "CPartyLink2/convertAPCRoleType2MonsterRoleType.inc"
}

// ============================================================================
// CParty::setBestClearTime —— ORIG T 0x085be178（setBestClearTime.md）。
// 遍历 4 个成员槽：checkValidUser(i)==1 时 GetCharacExpandData(user, 0x12)
// 得 BestClearTime*，调 BestClearTime::setBestClearTime(dungeonIdx, diff, time)。
// ============================================================================
#include "CPartyLink2/setBestClearTime.inc"
}

// ============================================================================
// CParty::SendSecretShopItemList —— ORIG T 0x085b9ecc（SendSecretShopItemList.md）。
// pos = GetUserPosition(user)；0<=pos<4 时经 GlobalData::s_secret_shop 把
// this+0x2a8+pos*0x1c（= m_secretShopInfo[pos]）处的 SALE_INFO vector 发给 user。
// ============================================================================
#include "CPartyLink2/SendSecretShopItemList.inc"
}

// ============================================================================
// CParty::kill_monster —— ORIG T 0x085a27e8（kill_monster.md，0x1381 字节）。
// 关键字段映射（汇编逐指令核对）：
//   STKillMonsterData：uid=monsterIdx, mapIdx, dieReason=memberCount,
//     +8=avgMoneyPercent, +0xc=minStdDropRate, +0x10/+0x11=die[0xa22/0xa23],
//     +0x14=set_random_reward(DROPPROB)->result.m_rate14
//   奖励分表：G_CDataManager()->m_serverParameter.m_expGroup368[memberCount-1]
//   击杀计数：BattleData(+0x32c).m_killedMonster0/1/2、m_field48、m_field4c
//   金卡掉落上限：G_CDataManager()->m_serverParameter.m_4968（dm+0x49d0），
//     当前地图 m_f0（CMap+0xf0）非 0 则覆盖；命中上限时 m_field48 不增。
//   维度判定：dungeon+0x89e / +0x89f（Cheat 维度标志）与 party short +0xc4c。
// ============================================================================
#include "CPartyLink2/kill_monster.inc"
}
