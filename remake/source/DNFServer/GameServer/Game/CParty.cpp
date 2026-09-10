// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CParty/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r CParty（G2 队伍聚合根）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/CParty.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 依赖子对象（CBattle_Field/BattleData/GameResultSet/...）为后续批次，
// 本 TU 以 asm-label extern 发调用、以 char 数组占位，主 agent 集成后替换。
#include <string.h>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>
// 显式构造/析构调用带模板参数(含逗号)时需 typedef，否则 ~std::map<int,int>() 的逗号被解析为分隔符。
typedef std::map<int, int> MapII;
#include <vector>
#include <new>

#include "QuickParty.h"   // 必须先于 CDungeon.h/CParty.h（QUICKPARTY_H 守卫需先放开，避免 CDungeon 最小 CQuickPartySystemManager 与权威冲突）
#include "CParty.h"
#include "CDungeon.h"
#include "Party.h"       // 仅ORIG 类 Party（DungeonPartyManager 数据类族，worklist Party.txt）
#include "CInventory.h"
#include "InterfacePacketBuf.h"
#include "CPartyTelePort.h"
#include "LogManager.h"

#include "BestClearTime.h"          // setNewRecord（dungeon_start 经 GetCharacExpandData(0x12)）
#include "CDungeonClear.h"          // getClearedDungeonDiff（dungeon_start 难度门槛）
#include "SkillSlot.h"              // checkskillSlotEmptyCount（dungeon_start 反外挂）
#include "HistoryLog.h"             // WriteDunStart
#include "CStatisticServerProxy.h"  // statistc_proxy::add（get_item 随机选项拾取统计）
#include "DB_AccountCargoSync.h"    // DB_InsertRandomOptionItemInform::makeRequest（get_item）
#include "CGenUniqueNo.h"           // genIPGNo（get_item 时装入库）
#include "CCirculationStatistic.h"  // AddCirculationStatistic（金币流水）
#include "DataManagerInitDepends.h" // IsLightServer
#include "Arad_DataManager.h"       // ARAD::Singleton<ServiceRestrictManager>
#include "PacketTypes.h"            // MSG_PARTY_SET_INFO 完整类型
#include "CSecretShop.h"            // secretshop::CSecretShop 完整类型
#include "CServerProxyMgr.h"        // CServerProxyMgr<CGuildServerProxy>
#include "CAssaultMgr.h"            // pvp_assault 单例（walkout_member）
#include "CSpecialItemRoutingManager.h" // StartRouting（get_item 史诗路由）
#include "CTradeSpace.h"            // check_trade_possibility/cancel_trade（game_start）
#include "CSystemTime.h"            // getCurSec/getCurTickCount/getCurDate
#include "ServiceRestrictManager.h" // isRestricted（drop_item 金币丢弃限制）

#include "CWorldMap.h"
#include "ServerParameterScript.h"

#include "BattleData.h"
#include "SECRET_SHOP_DATA.h"
#include "Secu_HackLogCheckByParty.h"   // Secu_HackLogCheckByParty::Init/ctor（§7）

#include "PvPTypes.h"
#include "CGameManager.h"
#include "CGuildServerProxy.h"
#include "GameResultSet.h"
#include "CVillageMonsterMgr.h"
#include "CDataManager.h"
#include "CDimensionActivationEvent.h"
#include "CBattle_Field.h"
#include "CEquipItem.h"   // CEquipItem::m_itemType@+0x234（drop_item 丢弃限制）
#include "W9_EventSmall.h"        // CCharacBloodDungeon::GetEnterCount
                                  //（get_blood_dungeon_admission_fee /
                                  //  beginBloodClearProcess ORIG call 085bfe88）
#include "GameWorld.h"
#include "TimerQueue.h"   // insert_time_* 定时器注册（ORIG 0x8630e16 InsertTimer）
#include "CUserProc.h"   // APSystem::CUserProc::ClearActionAndSendtoUser（SetCardNumber）
#include "CPCRoomBurningEvent.h"   // generateBloodRewardExp 事件加成
#include "CExpDoubleEvent.h"
#include "CBurningFatigueEvent.h"
// MapInfo 经 CBattle_Field.h→WarField.h 引入；显式析构/构造带模板参数需 typedef。
typedef std::vector<MapInfo> MapInfoVec;
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDungeonEntranceLog.h"
#include "GameTypes.h"

#include "EventNewCharacterReward.h"
#include "GlobalData.h"
#include "CServerEvent.h"   // CServerEvent::GetExpRate（sumGainedEachExp）

// ---- ClearRewardFinal 批次（clear_reward / set_clear_reward_exp_parameter）----
#include "CItemGeneratorMgr.h"       // WongWork::CItemGeneratorMgr::getGenerator(2)
#include "CPartyExpBonusEvent.h"     // repeat 事件 0x18 队伍经验加成
#include "HeroMissionEvent.h"        // ARAD::Singleton<HeroMissionEvent>::processMission
#include "CConditionEventManager.h"  // IsFreeGoldCard / ProcessCheckStepUp
#include "CGlobalEffectManager.h"    // s_globalEffectManager_ 效果 2
#include "CEnvironment.h"            // G_CEnvironment / get_channel_no
#include "STExpReward.h"             // set_clear_reward_exp_parameter 局部结构
#include "Quest.h"                   // check_clear_item/get_clear_itemlist（clear_reward）

// ============================================================================
// 所有 extern "C" asm 桥已删除（game_verify_guide.md §9/§10）：
// 改用 include 权威头 + 真实 C++ 调用；子对象以 reinterpret_cast +
// placement-new 等价替换原 asm 标签，ABI 与 ORIG 保持一致。
//
// ============================================================================
// CParty::cMember
// ============================================================================
#include "CParty/cMember.inc"
}

#include "CParty/Init.inc"
}

// ============================================================================
// CParty 构造 / 析构
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x20/0x4c：成员构造序与 pad 类型化（CBattle_Field/BattleData 内嵌）待 Main 裁决。
// [SweepCParty2 2026-09-09] 真 bug 修复：删 m_battleField 二次构造 ⇒ 308→287（ORIG 220，
// 剩余 gap 67 = N83 placement-new 前端内联 vs 4.4 extern _ZnwjPv（7 处 pad 区）+ 构造序差）。
#include "CParty/CParty.inc"
}

// [SweepCParty] 真 bug：删 m_battleField 显式析构（真实成员，编译器已自动析构 ⇒ 原为
// 双重析构）。105→101。残余 = N104（ORIG 每个析构带异常中继块，4.1/4.4 EH 布局差）。
#include "CParty/dtor_CParty.inc"
}

#include "CParty/send_to_party.inc"
}

// ============================================================================
// 基础访问器
// ============================================================================
#include "CParty/SetIDX.inc"
}

#include "CParty/GetPartyIndex.inc"
}

#include "CParty/set_state.inc"
}

#include "CParty/get_state.inc"
}

#include "CParty/IsSinglePlay.inc"
}

#include "CParty/SetSinglePlay.inc"
}

#include "CParty/GetEPLPState.inc"
}

#include "CParty/GetSelectedEPLPCmd.inc"
}

#include "CParty/SetSelectedEPLPCmd.inc"
}


#include "CParty/GetMember.inc"
}

#include "CParty/get_user.inc"
}

#include "CParty/getTitleIndex.inc"
}

#include "CParty/setTitleIndex.inc"
}

#include "CParty/getTitle.inc"
}

#include "CParty/setTitle.inc"
}

#include "CParty/IsAutoCreated.inc"
}

#include "CParty/SetAutoCreated.inc"
}

#include "CParty/getUserMax.inc"
}

#include "CParty/setUserMax.inc"
}

#include "CParty/getDungIndex.inc"
}

#include "CParty/setDungIndex.inc"
}

#include "CParty/getDungDiffi.inc"
}

#include "CParty/setDungDiffi.inc"
}

#include "CParty/get_dungeon_clear_state.inc"
}

#include "CParty/set_dungeon_clear_state.inc"
}

#include "CParty/reset_recv_flag.inc"
}

#include "CParty/SetRecvResultFlag.inc"
}

#include "CParty/getStraightVictories.inc"
}

#include "CParty/setStraightVictories.inc"
}

#include "CParty/incStraightVictories.inc"
}

#include "CParty/resetStraightVictories.inc"
}

#include "CParty/getStartGamePartyCount.inc"
}

#include "CParty/setStartGamePartyCount.inc"
}

#include "CParty/IsFirstMapClear.inc"
}

#include "CParty/SetFirstMapClear.inc"
}

#include "CParty/IsPremiumGoldCardParty.inc"
}

#include "CParty/SetPremiumGoldCardParty.inc"
}

#include "CParty/ResetPremiumGoldCardParty.inc"
}

#include "CParty/GetPremiumGoldCardDefaultItem.inc"
}

#include "CParty/SetPremiumGoldCardDefaultItem.inc"
}

#include "CParty/ResetPremiumGoldCardDefaultItem.inc"
}

#include "CParty/GetSecretShopData.inc"
}

#include "CParty/checkBossRoom.inc"
}

#include "CParty/checkStartRoom.inc"
}

#include "CParty/DeleteDungeonDropItem.inc"
}

#include "CParty/get_quick_party_index.inc"
}

#include "CParty/set_quick_party_index.inc"
}

#include "CParty/is_quick_party.inc"
}

#include "CParty/set_quick_party.inc"
}

#include "CParty/set_gm_random_buff_type.inc"
}

#include "CParty/get_random_buff_type.inc"
}

#include "CParty/IsWeekendEvent.inc"
}

#include "CParty/SetWeekendEvent.inc"
}

#include "CParty/getDungeonMapSaving.inc"
}

#include "CParty/setDungeonMapSaving.inc"
}

// ORIG 0x085be39a（_ZN6CParty25checkFreeRevivalConditionEPK8CDungeon）：
// 1) m_battleField.m_bloodState==1 → true；2) dungeon 为空时按战场 dungeon_index 找回；
// 3) CDungeon::checkFreeRevivalCondition(diff) 命中 → true；
// 4) 否则需维度副本（get_dimension_possible>0）且重复事件槽 0x5e 在进行中，
//    再按 diff 查 CDimensionActivationEvent::checkFreeRevivalCondition。
#include "CParty/checkFreeRevivalCondition.inc"
}

#include "CParty/GetPartyTelePort.inc"
}

#include "CParty/getPassedMapInfoList.inc"
}


#include "CParty/Get_party_overlapped_drop_ratio_rare.inc"
}

#include "CParty/Get_party_overlapped_drop_ratio_unique.inc"
}

#include "CParty/ClearMapHitCount.inc"
}

#include "CParty/IsRoutingState.inc"
}

#include "CParty/SetStartRouting.inc"
}

#include "CParty/SetEndRouting.inc"
}

#include "CParty/SetRoutingItem.inc"
}

#include "CParty/GetRoutingItemID.inc"
}

#include "CParty/GetRoutingInvenItem.inc"
}

#include "CParty/GetRoutingItemUniqueID.inc"
}

#include "CParty/GetMemberRoutingState.inc"
}

#include "CParty/SetMemberRoutingState.inc"
}

#include "CParty/SetRoutingTimerOver.inc"
}

// ============================================================================
// 生命周期
// ============================================================================
#include "CParty/init_quick_party_data.inc"
}

// ============================================================================
// CParty::CItemRoutingData
// ============================================================================
#include "CParty/Reset.inc"
}

#include "CParty/CItemRoutingData.inc"
}

#include "CParty/dtor_CItemRoutingData.inc"
}

#include "CParty/GetRoutingState.inc"
}

#include "CParty/SetRoutingState.inc"
}

#include "CParty/GetRoutingInvenItem_1.inc"
}

#include "CParty/GetRoutingItemIndex.inc"
}

#include "CParty/SetRoutingItemIndex.inc"
}

#include "CParty/GetRoutingItemUniqueID_1.inc"
}

#include "CParty/GetMemberRoutingState_1.inc"
}

#include "CParty/SetMemberRoutingState_1.inc"
}

#include "CParty/getManager.inc"
}

// b28d-solo-CParty: DIFF 残 1 处 —— N1：ORIG lea 0x1(%eax),%edx vs 4.4 add+mov（已试 (char) 截断形无效）；202 vs 201
#include "CParty/init.inc"
}

#include "CParty/destroy.inc"
}

#include "CParty/SetEPLPState.inc"
}

// ============================================================================
// 批次 A 补全（ORIG 0x85a932a / 0x85a21ca / 0x85b8f92 等；语义见 CParty.md）
// ============================================================================

#include "CParty/check_passive_skill.inc"
}

#include "CParty/isHelpAbuseParty.inc"
}

// ORIG T 0x085a21d4（0x71 字节）：任一有效成员 isHelpAbuseParty 则真。
#include "CParty/isHelpAbuseParty_1.inc"
}

#include "CParty/MemberPenaltyRate.inc"
}

#include "CParty/GetPartyMemberCoinLimit.inc"
}

#include "CParty/SetPartyMemberCoinLimit.inc"
}

#include "CParty/getStandardDimensionLevel.inc"
}

#include "CParty/isTournamentVictory.inc"
}

#include "CParty/GetTournamentDungeonClearState.inc"
}

#include "CParty/SetTournamentDungeonClearState.inc"
}

#include "CParty/set_host.inc"
}

// ============================================================================
// STGainedEachExpData / getMonsterTotalExp / sumGainedEachExp
// ============================================================================
// ORIG _ZN19STGainedEachExpDataC1Ev @ 0x08151a56（W，C2 同址）：
//   movl $0x0,(%eax)           → m_exp = 0
//   movl $0x3f800000,0x4(%eax) → m_expPartyDivisor = 1.0f（0x3f800000）
// ORIG 无 D1/D2（平凡析构）。
#include "CParty/STGainedEachExpData.inc"
}

// ----------------------------------------------------------------------------
// CParty::getMonsterTotalExp @ 0x085a23dc（_ZN6CParty18getMonsterTotalExpEjciffb）
// 逐指令转写（docs/class_func_reports/CParty/getMonsterTotalExp.md + objdump）：
//   base = G_CDataManager() 返回值 + 0x68；
//   flag(char,有符号) <= 3 时 rate = *(float*)(base + 4*(flag+0xfc))，
//   否则 rate = 1.0f（0x3f800000 立即数）；
//   bool d 非 0：rate *= *(float*)0x08cbbbd0（字节 00 00 40 40 = 3.0f）；
//   x87 链（fildll/fdivrp/fmuls 就近舍入 fistpll）：
//     (double)(unsigned long long)exp / 2.0f（*(float*)0x08cbbbd4，
//     字节 00 00 00 40 = 2.0f）
//       * 参数 c(0x1c(%ebp)) * *(float*)(base + 4*(a+0xe4) + 8) * b * rate
#include "CParty/getMonsterTotalExp.inc"
}

// ----------------------------------------------------------------------------
// CParty::sumGainedEachExp @ 0x085a2488（_ZN6CParty16sumGainedEachExpEP5CUserRK19STGainedEachExpData）
// 逐段转写（docs/class_func_reports/CParty/sumGainedEachExp.md + objdump）。
// 注：ORIG CUser::get_growth_power_exp_reward 为 sret 结构返回；本工程内该函数
// 以 int 返回同一 dword（CUser.cpp），故按 (v>>8)&0xff 取原结构 +1 字节，语义等价。
// CUser::getAddExpRateFromPremium(type)（ORIG W 0x085bfd76）=
// GetAddExpBonus(type)（ORIG W 0x085bf940）：type∈[0,0x69] 时返回
// g_SPremiumInfo[type].m_field44（表基 0x09483a84-0x09483a40=+0x44），否则 0；
// 该转发链尚未在本工程建模为成员函数，此处就地取权威表字段，不引入新 undefined。
// ----------------------------------------------------------------------------
// [SweepCParty] 补真调用 CUser::getAddExpRateFromPremium(0x4f)（旧码就地取
// g_SPremiumInfo 表字段绕过调用 ⇒ p167 MISS 1，已消）。gap 1（220 vs 219）；p167 CLEAR
//（余 C1/C2 别名）。残余 = N82/N126 帧槽（0x78 vs 0x88）。
#include "CParty/sumGainedEachExp.inc"
}

// ============================================================================
// change_manager / IsExistInvisible / battle_data_reset / 发送族
// ============================================================================
// CParty::change_manager @ 0x859c832 —— 清空旧 manager 槽位，选举新 manager。
// 返回 1=成功 / 0=队伍已空（写日志）。
#include "CParty/change_manager.inc"
}

// ==== CParty::change_manager @ 0x0859c978（两参版，change_manager() 无参版同族报告）====
// ORIG _ZN6CParty14change_managerEP5CUseri T：仅现任 manager 可换任；
// slot 越界/空槽→21、非本人→8、目标已是 manager→18；成功换 m_manager 并广播。
#include "CParty/change_manager_1.inc"
}

// CParty::IsExistInvisible @ 0x85b6410 —— 任一有效成员当前角色不可见则 true。
#include "CParty/IsExistInvisible.inc"
}

// CParty::battle_data_reset @ 0x859aec2 —— 副本战斗数据与快捷队伍状态复位。
#include "CParty/battle_data_reset.inc"
}

// CParty::isHelpAbuseParty 依赖的 send_host_info @ 0x859cdd2。
#include "CParty/send_host_info.inc"
}

// CParty::get_party_realtime_info @ 0x859cbac —— 实时队伍信息（始终返回 1）。
#include "CParty/get_party_realtime_info.inc"
}

// CParty::send_party_realtime_info @ 0x859cd24。
#include "CParty/send_party_realtime_info.inc"
}

// CParty::send_quick_party_matching_result @ 0x859b73e。
#include "CParty/send_quick_party_matching_result.inc"
}

// ORIG 反汇编为 for(i=0; i<=3; ++i) 结构（初始化后直接 jmp 到尾部条件测试）。

// CParty::CheckEnterEventDungeon @ 0x859ee66 —— 事件地下城条件（必备/奖励材料）。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x60/0x70 + 守卫极性；259 vs 276
#include "CParty/CheckEnterEventDungeon.inc"
}

// CParty::checkInoutConditionDungeon @ 0x85ac59c —— 进入副本前的进出条件判定，
// 返回 RetryFailReason 值（10 无副本 / 10 禁止进入 / 1 疲劳 / 2 材料 / 4 维度 /
// 7 其它 / 8 封印门 / 9 可进入）。
// 依赖若干 CParty / CWorldMap / CWorldMapList 方法（ORIG 符号已声明，定义桩由
// 并行批次提供）：CheckEnterDimensionDungeon / check_dungeon_start /
// CheckEnterVillageAttackRevenge / CheckDestroyConditionSealDoor /
// CheckMemberFatigue / CWorldMapList::find_world_map /
// CWorldMap::IsInHellDungeon / CWorldMap::hasDeathTower。
// [SweepCParty] 补真缺口：循环内 get_user 三次独立调用（P0/P183，旧码 u 缓存 ⇒ MISS 2）
// + param[4] 改 memset（ORIG 两次 call memset）。p167 28=28 CLEAR（余 C1/C2 别名）。
#include "CParty/checkInoutConditionDungeon.inc"
}

// ============================================================================
// 链接阻断方法（ORIG T/W）：6 个 CParty 方法 + depended helper（主 agent 授权一并实现）
// ============================================================================

// 全局：getHellPartyEntranceItemNeedCount（ORIG 0x8891be6，非成员函数）
//   汇编：eax = 40*n - 0x708；除以 100（magic 0x51eb851f, sar 5）；+10。
#include "CParty/getHellPartyEntranceItemNeedCount.inc"
}

#include "CParty/CheckMemberFatigue.inc"
}

#include "CParty/sendInoutConditionDungeon.inc"
}

// ORIG 0x859f6e2：遍历 4 成员，任一有效用户是 GM → 返回 1；否则 ServerParameterScript::isDungeonOpen(idx)。
#include "CParty/checkDugeonInoutTime.inc"
}

// ORIG 0x859f75e：进入血腥地下城入场费 = lottoCost + price + (inc * price) / 100。
//   price = ServerParameterScript::GetPriceAverage(charac_level)
//   inc   = ServerParameterScript::GetAdmissionIncrease(CCharacBloodDungeon::GetEnterCount)
//   lotto = ServerParameterScript::GetLottoCost()
#include "CParty/get_blood_dungeon_admission_fee.inc"
}

#include "CParty/check_dungeon_start.inc"
}

#include "CParty/CheckEnterDimensionDungeon.inc"
}

// ORIG 0x85ba804：村庄攻击/复仇模式进入检查。
#include "CParty/CheckEnterVillageAttackRevenge.inc"
}

// [SweepCParty] 142=142 blocks=0（AE/IDENTICAL）。真 bug：delete_item 返回值被丢弃 +
// trace 无条件前置 ⇒ `if(!delete_item()) trace else SendUpdateItem`；del 正极性；
// P227 全表达式 cMyTrace 链；it->first/second 去具名缓存；slotResult=0 前置。
// ORIG 0x85bc86e：地狱肉鸽免票检查/扣除。
//   worldMap = this->m_hellWorldMap（缓存）；worldMap->m_hellPassItemList 为 pair<int,int> 列表。
#include "CParty/CheckHellDungeonFreepassItemHaveAndDel.inc"
}

// [SweepCParty] gap 10→3（188 vs 190）：P331 声明序（passCount/validCount/haveCount/slot）、
// haveCount=0 提前、正极性 if、去 u 缓存、非 const iterator（Ghidra 实证）、result bool 化。
// 残余 = N-88（槽比 setne 物化）+ N18b（haveCount 内存直比，4.4 恒折 cmp mem）。
#include "CParty/CheckDestroyConditionSealDoor.inc"
}

#include "CParty/check_timer_key.inc"
}

#include "CParty/gen_timer_key.inc"
}

#include "CParty/resetFinishLoadingVar.inc"
}

#include "CParty/CanDoPartyActionAssalutState.inc"
}

#include "CParty/resetMemberUseCoin.inc"
}

#include "CParty/checkBloodRenewMyRecord.inc"
}

#include "CParty/IsSelectCard.inc"
}

#include "CParty/_IsCannotConnectP2P.inc"
}

#include "CParty/_GetConnectP2PPoint.inc"
}

#include "CParty/check_allmember_die.inc"
}

#include "CParty/get_live_count.inc"
}

#include "CParty/get_live_count_enter_map.inc"
}

#include "CParty/GetValidPartyMember.inc"
}

#include "CParty/IsExistUser.inc"
}

#include "CParty/_getMemberLevelGap.inc"
}

#include "CParty/getMemberLevelGap.inc"
}

#include "CParty/increase_dungeon_coin_count.inc"
}

#include "CParty/IsReturnUserParty.inc"
}
#include "CParty/send_invite_bluemarble.inc"
}
#include "CParty/get_member_count.inc"
}
// ORIG W 0x08145882：getMemberNames = 转发 _getMemberNames（call 0x85b4bde）。
#include "CParty/getMemberNames.inc"
}

// ORIG T 0x085b9e20：循环 i=0..3，_checkValidUser 过滤后读 (this+0x78+i*0x18)
// 的 CUser* 与 user 比较，命中返回 i，否则 -1。
#include "CParty/GetUserPosition.inc"
}

 char* CParty::_getMemberNames(char* buf)
{
    char nameBuf[34];
    // ORIG 反汇编：m_manager==0 与 getCurCharacName() 结果直接作 sprintf 实参
    // （无中间变量，je/jmp 合并后 eax 直接进 0x8(%esp)）→ 用三元内联。
    sprintf(buf, "\"%s\",",
            (m_manager == 0) ? "NoMember" : m_manager->getCurCharacName());
    for (int i = 0; i <= 3; ++i)
    {
        // b28d-49 @0x85b4c63：ORIG 单次 sprintf —— `_checkValidUser(i) ? 名字 : "N/A"`
        // 三元内联（je 跳过 getCurCharacName 后 %eax 直接进 0x8(%esp)）；
        // if/else 两臂各写一次 sprintf 会多一次调用（ndiff 8）。
        if (i == 3)
            sprintf(nameBuf, "\"%s\"",
                    _checkValidUser(i)
                        ? m_member[i].m_pUser->getCurCharacName() : "N/A");
        else
            sprintf(nameBuf, "\"%s\",",
                    _checkValidUser(i)
                        ? m_member[i].m_pUser->getCurCharacName() : "N/A");
        strncat(buf, nameBuf, 0xff);
    }
    buf[0xff] = '\0';
    return buf;
}

// b28d-solo-CParty: N 封顶 —— N82 帧 0x8c/0x9c + callee-saved %edi/%esi 角色差；277 vs 270
#include "CParty/_checkValidUser.inc"
}
#include "CParty/checkValidUser.inc"
#include "CParty/GetMemberSlotNo.inc"
}
// CParty::IncreaseSetPlayResultCount —— ORIG T 0x085b22e2。
// P210：定义置于 callee GetMemberSlotNo 之后，4.4 精确继承 outgoing（帧 0x28→0x18=ORIG）。
#include "CParty/IncreaseSetPlayResultCount.inc"
}
// b28d-solo-CParty: N 封顶 —— N82 帧 0xf8/0xe8 + 槽复用；215 vs 209
#include "CParty/Reset_party_overlapped_drop_ratio.inc"
}
// [SweepCParty] 补真缺口：put_byte(0)+put_short(get_member_count())（第一分支）、
// send_all(packet, m_manager) 二参版、memset→char[0x40]={0}（rep stos）、cMyTrace 具名化
// （去 placement new）。p167 58=58 CLEAR（余 C1/C2、D1/D2 别名）。
#include "CParty/ReturnToVillage.inc"
}
// b28d-solo-CParty: N 封顶 —— N82 帧 0x16c/0x19c；518 vs 473
#include "CParty/leave_user.inc"
}
#include "CParty/IsEventCharacParty.inc"
}
// b28d-solo-CParty: DIFF 残 2 处 —— ORIG 用 %ebx 缓存 &items[i] 重读 + sete 物化（P197④/N82）；225 vs 223
#include "CParty/CheckEnterAncientDungeon.inc"
}
// b28d-solo-CParty: N 封顶 —— N82 帧 0x5c/0x60 + 块序极性；270 vs 263
#include "CParty/UseAncientDungeonItems.inc"
}

// ============================================================================
// CParty::set_charac_live —— ORIG 0x085b29bc（0x195 字节）
//   报告：docs/class_func_reports/CParty/set_charac_live.md
//   找到 user 槽位后按 state 写 +0x380(live)/+0x384(enterMap)/+0x388 标志；
//   state==1 时 m_member[i].m_field14(+0x8c) 计数 +1；随后发包 0x20 广播。
// ============================================================================
#include "CParty/set_charac_live.inc"
}

// ============================================================================
// stClearRewardExpParameter_t / stClearRewardBonusExp_t 构造
//   ORIG W 0x0814a8b8 / 0x0814a800（逐 dword 清零；参数结构 +0x54/+0x55 字节
//   清零、+0x58/+0x5c 置 -1）。
// ============================================================================
#include "CParty/stClearRewardExpParameter_t.inc"
}

#include "CParty/stClearRewardBonusExp_t.inc"
}

// ============================================================================
// CParty::CalcAvatarBonus / CalcCreatureBonus / getAvatarBonus / getCreatureBonus
//   ORIG 0x085acfde / 0x085ad084 / 0x0814a9c2(W) / 0x0814a9e2(W)
//   报告：docs/class_func_reports/CParty/CalcAvatarBonus.md、CalcCreatureBonus.md
// ============================================================================
// CalcAvatarBonus：任一有效成员装备了 avatar → 单人队伍 0.02f，否则 0.05f；无则 0.0f
#include "CParty/CalcAvatarBonus.inc"
}

// CalcCreatureBonus：user 当前角色装备了 creature → 0.05f，否则 0.0f
#include "CParty/CalcCreatureBonus.inc"
}

#include "CParty/getAvatarBonus.inc"
}

#include "CParty/getCreatureBonus.inc"
}

// ============================================================================
// CParty::getClearRewardBonusExp —— ORIG 0x085ad278（0x73b 字节）
//   报告：docs/class_func_reports/CParty/getClearRewardBonusExp.md
// ============================================================================
// ORIG 乘加成公式：fildll(经验) × flds(比率) 后 fistpll——x87 默认就近舍入。
// 源码直接写 (long long)((double)(long long)exp * rate) 即产生相同指令序列
//（-march=i386 下为 fucompp/fnstsw/fistpll 形态，与 ORIG 一致）。
// [SweepCParty] p167 CLEAR（7=7）；gap 7（615 vs 608）。构成：N85（x87 比较 ×7，
// 每处 ORIG fucompp/fnstsw/sahf 4 条 vs 4.4 fucomip/fstp 2 条 ⇒ 我方 -14，属
// -march=i486 路由族，禁改路由）+ 我方代码生成 +7（type 槽/N76 直存/累加寄存器化）。
// [SweepCParty2] 尾部 premiumAdvExp/serverEventExp 求值序已按 ORIG 0x085ad6d6/0x085ad708
// 与 total 累加序 0x085ad7c2/0x085ad7ca 修正（纯求值序，指令数不变）。语义已对齐，封顶。
#include "CParty/getClearRewardBonusExp.inc"
}

// ============================================================================
// Prod_CParty 批次（2026-08-24）：CParty producer 函数（9 个 ORIG 符号）
// 逐函数对照 docs/class_func_reports/CParty/{GetPartyType,SendWalkout,
// SetPartyInfoUI,create_party,dungeon_start,game_start,set_single_play,
// walkout_member,get_item,drop_item}.md 的 ## 汇编 + ## 反编译 C 全文转写；
// 常量 _DAT_08cbbbcc=100.0f / _DAT_08cbbbe0=2.5 取自 df_game_r .rodata。
// ============================================================================

// ---- CParty::GetPartyType（ORIG T 0x85b62c0）----
#include "CParty/GetPartyType.inc"
}

// ---- CParty::SendWalkout（ORIG T 0x859cb00）----
#include "CParty/SendWalkout.inc"
}

// ---- CParty::SetPartyInfoUI（ORIG T 0x859b234）----
#include "CParty/SetPartyInfoUI.inc"
}

// ---- CParty::create_party（ORIG T 0x859b1be）----
#include "CParty/create_party.inc"
}

// ---- CParty::set_single_play（ORIG T 0x859b142）----
#include "CParty/set_single_play.inc"
}


// ---- CParty::walkout_member（ORIG T 0x859ca4a）----
#include "CParty/walkout_member.inc"
}

// ---- CParty::game_start（ORIG T 0x859d718）----
// 状态==2 直接返回；否则逐成员 make_basic_info/goto_dungeon/交易清理，
// 广播 0x2(基本)/0x3(进图名单)/0x1b(血宫信息) 三包；村庄/区域不一致走
// HACK_VILL_AREA 日志并 DisConnSig(0x18) 全员断线。
// [SweepCParty] 补 GetPartyTelePort()（旧码直取 &m_pad1ad0）+ hellQuests 迭代器遍历
// （旧码下标形产 EXTRA operator[]）；p167 104=104 CLEAR（余 C1/C2、D1/D2 别名）。
#include "CParty/game_start.inc"
#define bd (m_battleData)
    if (get_member_count() == 1)
    {
        bd.SetHellPartyValueTotal(
            static_cast<stHellParty_t*>(
                G_CDataManager()->get_hellparty_script_values())
                ->m_baseValue);   // ORIG game_start.md 0859dd76 `mov 0x8(%eax)`
                                  // = stHellParty_t::m_baseValue
    }

    // ORIG this+0xd0c = CBattle_Field+0x1e8 = m_hellPartyRatio0（int，2026-09-03
    // 头类型修正；见 reset_hell_party_value/send_hellParty_value 报告）。
    int hellDiv = reinterpret_cast<CBattle_Field*>(&m_battleField)->m_hellPartyRatio0;
    int hellVal = (int)(bd.m_hellPartyValueTotal * 100.0f / (float)hellDiv);
    basicPacket.put_int(hellVal);

    std::vector<int> hellQuests;                            // -0x70
    bool hellEnable =
        (m_hellWorldMap != 0 && m_hellWorldMap->IsInHellDungeon() &&
         reinterpret_cast<CBattle_Field*>(&m_battleField)
             ->IsEnableHellDungeon());
    if (hellEnable)
    {
        CheckClearQuestHellParty(hellQuests);
        size_t qn = hellQuests.size();                      // -0x10
        if ((int)qn < 1)
        {
            basicPacket.put_byte(0);
            m_field1864 = 1;
        }
        else
        {
            basicPacket.put_byte((int)qn);
            // ORIG 0x0859de..：迭代器遍历（p167 MISS begin/end/de/pp/ne；我方旧码
            // 下标形产 EXTRA operator[]）。
            for (std::vector<int>::iterator it = hellQuests.begin();
                 it != hellQuests.end(); ++it)
                basicPacket.put_short(*it);
        }
    }
    else
    {
        basicPacket.put_byte(0);
    }

    if (!make_blood_dungeon_info(user, basicPacket))
        make_blood_dungeon_info_blank(basicPacket);

    bool deathTowerArea = false;                            // -0x29
    worldIdx = G_GameWorld()->GetWorldMapIndex(user);       // -0x28
    if (worldIdx > 0)
    {
        CWorldMap* wm2 =
            G_CDataManager()->m_worldMapList.find_world_map(worldIdx);  // -0xc
        (void)wm2;
        if (m_hellWorldMap != 0 && m_hellWorldMap->hasDeathTower())
            deathTowerArea = true;
    }

    if (isDungeonDifficultyInfromNotice(deathTowerArea))
    {
        basicPacket.put_byte(1);
        m_manager->setMemberDungeonFlag(true);
    }
    else
    {
        basicPacket.put_byte(0);
    }
    if (deathTowerArea)
        reinterpret_cast<CBattle_Field*>(&m_battleField)
            ->setWorldMapTowerArea(deathTowerArea);

    basicPacket.finalize(true);
    send_to_party(basicPacket);

    m_recvResultFlag = 0;                                   // +0x118
    // ORIG 0x0859e05x 经 CParty::GetPartyTelePort()（p167 MISS，旧码直取 &m_pad1ad0）。
    reinterpret_cast<CPartyTelePort*>(GetPartyTelePort())->reset_teleport_data();
}
#undef bd

// ---- CParty::dungeon_start（ORIG T 0x85a0954）----
// 返回：0=成功；1=SelectDungeon/封印门/血宫置态失败；0xe=等级不足；
// 0x13=未通关该难度或难度倒退；0x15=dungeon 查表失败；8=EPLP 状态异常；
// 其余为 GetDungeonIndex_isTOD/checkInoutConditionDungeon 透传。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x20c/0x27c + P0 指针缓存；1715 vs 1662
#include "CParty/dungeon_start.inc"
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
#define bd (m_battleData)

    resetMemberUseCoin();
    m_firstMapClear = 0;                                   // +0x324
    ClearMapHitCount();
    m_secu.startDungeon();

    int todApcIdx = -1;                                    // -0x78
    int err = GetDungeonIndex_isTOD(dungeonIdx, todApcIdx);  // -0x74
    if (err != 0)
        return err;

    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);   // -0x70
    if (dungeon == 0)
        return 0x15;

    PacketGuard packet;                                    // -0x84
    int partyType = m_battleField.m_bloodState;                           // -0x6c ← +0xcd8
    SetPartyMemberCoinLimit(get_member_count() *
                            dungeon->GetPartyMemberCoinLimit());
    m_traceMobDieHack.setDungeonDiff(type);

    if (GetEPLPState() != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (!_checkValidUser(i))
                continue;
            WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
            hack->addServerHackCnt(m_member[i].m_pUser,
                                   (WongWork::ENUM_HACKTYPE)0x195, 1, 0, 0);
        }
        return 8;
    }

    // b28d-49 @0x85a0b17：ORIG 三调 get_min_level（85a0b26 条件 / 85a0b46 then /
    // 85a0b56 else），条件序 = IsLightServer 先、`get_min_level() > 1` 后（P157 逐点重调，
    // 不缓存）。原「先赋初值再条件重调」只有 2 次调用（P167 MISSING -1）。
    int minLevel;
    if (IsLightServer() != 0 && dungeon->get_min_level() > 1)
        minLevel = dungeon->get_min_level();
    else
        minLevel = dungeon->get_min_level();

    if (m_manager->IsGameMasterMode() != 1 &&
        minLevel > m_manager->get_charac_level())
        return 0xe;

    if (!checkDungeonDifficulty(type, dungeon))
    {
        char clearedDiff =
            m_manager->getDungeonClearRefR()->getClearedDungeonDiff(
                (short)dungeonIdx);                        // -0x51
        if (clearedDiff == 0)
            return 0x13;
        if ((clearedDiff < (char)type) && ((char)type != 4))
            return 0x13;
    }

    int questStage = 0;                                    // -0x64
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if (partyType == 1)
            continue;
        questStage = m_member[i].m_pUser->CheckQuestMap(dungeon->get_index());
        if (questStage > 0)
            break;
    }

    SetUsedCoinCount(0);

    if (!CheckHaveHellPartyPassItem(dungeon, dtype))
        return 0;

    bool randomHell = false;                               // -0x85
    int inoutErr =
        checkInoutConditionDungeon(dungeon, (ENUM_DUNGEON_MODE)partyType,
                                   (ENUM_CMDPACKET)0x10);  // -0x60
    if (inoutErr != 0)
        return 0;

    bf.check_random_appear_hell_dungeon(dungeon, (ENUM_DUNGEON_MODE)partyType,
                                        dtype, randomHell);

    int memberCount = get_member_count();
    if (!bf.SelectDungeon(dungeonIdx, dungeon, (int)(char)type, dtype,
                          memberCount, m_hellWorldMap, randomHell, questStage))
        return 1;

    choose_random_buff((QuickParty::RandomBuffDungeon)bf.getRandomBuffDungeonType(0));

    // [2026-09-10 试形 P312/P78] ORIG 该点无 memset call（p167 EXTRA memset）⇒
    // `={0}` 让 4.4 内联 rep stos（OnFightVillageMonster/leave_user 同族实证）。
    char names[256] = {0};                                 // -0x1cc
    _getMemberNames(names);

    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        reinterpret_cast<BestClearTime*>(m_member[i].m_pUser->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0x12))
            ->setNewRecord(false);

        int mode = partyType;                              // -0x70 复用槽
        char* names2 = _getMemberNames(names);
        const char* dunName = dungeon->GetDungeonName();
        m_member[i].m_pUser->m_historyLog.EnterDungeon(dunName, (int)(char)type,
                                                       names2, mode);
        m_member[i].m_pUser->resetDungeonClearHackCheck();

        if (m_member[i].m_pUser->getMoveSpace() == 0)
        {
            int firstEmpty = 0;                            // -0xb4
            SkillSlot* skillW = reinterpret_cast<SkillSlot*>(
                m_member[i].m_pUser->getCurCharacSkillW());
            int emptyCnt = skillW->checkskillSlotEmptyCount(firstEmpty);   // -0x44
            for (int k = 0; k < emptyCnt; ++k)
            {
                int set = m_member[i].m_pUser->SetETC(0x1a6, firstEmpty);
                WongWork::CHackAnalyzer* hack =
                    m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a6, 1,
                                       (unsigned int)set, 0);
            }

            int cntA = 0, cntB = 0, cntC = 0, cntD = 0, cntE = 0, cntF = 0;
            char growFirst = m_member[i].m_pUser->getCurCharFirstGrowType();
            int job = m_member[i].m_pUser->get_charac_job();
            CInventory* invenW = m_member[i].m_pUser->getCurCharacInvenW();
            invenW->checkEquipmentState(job, (int)growFirst, cntA, cntB, cntC,
                                        cntD, cntE, cntF);

            int lvl = m_member[i].m_pUser->get_charac_level();
            if (lvl > 0xf)
            {
                for (int k = 0; k < cntA; ++k)
                {
                    WongWork::CHackAnalyzer* hack =
                        m_member[i].m_pUser->getHackAnalyzer();
                    hack->addServerHackCnt(m_member[i].m_pUser,
                                           (WongWork::ENUM_HACKTYPE)0x1a4, 1, 0, 0);
                }
            }
            lvl = m_member[i].m_pUser->get_charac_level();
            if (lvl > 0x17)
            {
                for (int k = 0; k < cntB; ++k)
                {
                    int set = m_member[i].m_pUser->SetETC(0x1a5, cntC);
                    WongWork::CHackAnalyzer* hack =
                        m_member[i].m_pUser->getHackAnalyzer();
                    hack->addServerHackCnt(m_member[i].m_pUser,
                                           (WongWork::ENUM_HACKTYPE)0x1a5, 1,
                                           (unsigned int)set, 0);
                }
            }
            for (int k = 0; k < cntD; ++k)
            {
                WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a7, 1, 0, 0);
            }
            for (int k = 0; k < cntE; ++k)
            {
                WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a8, 1, 0, 0);
            }
            for (int k = 0; k < cntF; ++k)
            {
                WongWork::CHackAnalyzer* hack = m_member[i].m_pUser->getHackAnalyzer();
                hack->addServerHackCnt(m_member[i].m_pUser,
                                       (WongWork::ENUM_HACKTYPE)0x1a9, 1, 0, 0);
            }
        }
    }

    // 封印门判定：地狱图+类型1+旗标已立时必须消耗成功，否则整队失败。
    if (m_hellWorldMap != 0 && m_hellWorldMap->IsInHellDungeon() &&
        dtype == 1 &&
        m_sealDoorFlag != 0 &&
        UseSealDoorItems(m_hellWorldMap->m_field34) == 0)
        return 1;

    if ((char)dungeon->m_bloodDungeonType < 1)
    {
        if (!dungeon->isTournamentDungeon())
        {
            packet.clear();
            packet.put_header(0, 0x1c);
            packet.put_short(dungeonIdx);
            packet.put_byte((int)(char)type);
            bf.MakePacket(0x1c, &packet);
            send_to_party(packet);
            packet.clear();
            bf.MakePacket(0x1d, &packet);
            send_to_party(packet);
            _Send_TOD_APCInfo(todApcIdx);
        }
        else
        {
            packet.clear();
            packet.put_header(0, 0x182);
            packet.put_short(dungeonIdx);
            packet.put_byte((int)(char)type);
            bf.MakePacket(0x182, &packet);
            send_to_party(packet);
            packet.clear();
            bf.MakePacket(0x183, &packet);
            send_to_party(packet);
        }
    }
    else
    {
        packet.clear();
        packet.put_header(0, 0xd4);
        packet.put_short(dungeonIdx);
        packet.put_byte((int)(char)type);
        bf.MakePacket(0xd4, &packet);
        send_to_party(packet);
        packet.clear();
        bf.MakePacket(0xd3, &packet);
        send_to_party(packet);
        if (!bf.setBloodState((ENUM_BLOOD_STATE)2))
            return 1;
    }

    // 计时/奖励旗标区复位（BattleData +0x10..+0x38）
    bd.m_field20 = (int)GlobalData::s_systemTime_.getCurTickCount();   // +0x34c
    bd.m_field24 = 0;                                                  // +0x350
    bd.m_field10 = 0;                                                  // +0x33c
    bd.m_field14 = 0;                                                  // +0x340
    bd.m_field18 = 0;                                                  // +0x344
    bd.m_field1c = 0;                                                  // +0x348
    bd.m_field2c = 0;                                                  // +0x358
    bd.m_field30 = 0;                                                  // +0x35c
    bd.m_field34 = bd.m_field20;                                       // +0x360
    bd.m_field38 = bd.m_field24;                                       // +0x364

    set_state(2);
    SetEPLPState(1);
    gen_timer_key((TIMER_MESSAGE)0x2d);
    SetSelectedEPLPCmd(-1);
    int startCount = get_member_count();                   // -0x5c
    setStartGamePartyCount(startCount);

    secretshop::SECRET_SHOP_DATA& shop = m_secretShop;
    if (((char)dungeon->m_bloodDungeonType < 1) && m_manager->get_state() != 10 &&
        dungeon->isTowerOfDespairDungeon() == 0 &&
        dungeon->isTournamentDungeon() == 0)
    {
        // b28d-49 @0x85a18c6：ORIG 第三实参是重调 get_member_count()（`mov %esi,0xc(%esp)`），
        // 非 startCount 缓存（P157/P0；P167 MISSING get_member_count -1）。
        shop.m_openCode = GlobalData::s_secret_shop->LotteryNpc(
            dungeon->get_index(), dungeon->get_standard_level(), get_member_count());
    }
    else
    {
        shop.m_openCode = 1000;
    }

    char premiumCard = 0;                                  // -0x55
    unsigned int cardItem = 0;                             // -0x8c
    CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
    if (ev41->IsEventing(0) && IsReturnUserParty())
    {
        premiumCard = isPremiumGoldCardReward(m_battleField.m_pDungeon, cardItem) ? 1 : 0;
        if (premiumCard != 0)
        {
            SetPremiumGoldCardParty();
            SetPremiumGoldCardDefaultItem(cardItem);
        }
    }

    m_recvFlag.Clear();   // +0x1dc

    for (int i = 0; i <= 3; ++i)
    {
        for (int j = 0; j <= 1; ++j)
            m_field204[j][i] = (char)0xff;                 // +0x204[j][i]
        m_dungeonClearState = 0;                           // +0x1e0
        if (!_checkValidUser(i))
            continue;

        GlobalData::s_secret_shop->SendSecretShopNpc(m_member[i].m_pUser, shop.m_openCode);
        m_member[i].m_pUser->getCurCharacQuestW()->set_authen_data((QUEST_CONDITION)0,
                                                  dungeonIdx, (int)(char)type);
        if (dungeon != 0)
            SetProperLevelDungeon(m_member[i].m_pUser);

        if (partyType != 1)
        {
            bool fatigueSkip = (dungeon->m_villageAttackEventFlag == 1) || (dungeon->m_bloodDungeonType != 0) ||
                               (dungeonIdx == 10000) ||
                               (dungeon->isNoFatigueDungeon() == 1);
            if (!fatigueSkip)
            {
                if (!dungeon->isTournamentDungeon())
                {
                    m_member[i].m_pUser->FatigueUp(0);
                }
                else
                {
                    m_member[i].m_pUser->FatigueUp(dungeon->getTournamentRoundFatigue());
                    m_member[i].m_pUser->SendFatigue();
                }
            }
        }

        m_member[i].m_pUser->setPlayingDungeonIndex(dungeonIdx);
        m_member[i].m_pUser->m_character.PlayInfo::Reset();    // ORIG PlayInfo::Reset @ user+0x8e034（不清 m_f3c）
        m_member[i].m_pUser->m_character.m_f20 = startCount;   // user+0x8e054
        m_member[i].m_pUser->m_field8e074 = 0;                 // user+0x8e074（CUser 直属字段；Layout_D 布局修正）

        // b28d-49 @0x85a1c34：ORIG 无 stdLvl/muLvl 缓存 —— 条件里逐点重调
        // get_standard_level（85a1c34/85a1c6b）+ get_charac_level（85a1c56/85a1c8d），
        // 每项 `lea ±5/±3` 就地折叠（P157/P0；P167 各 MISSING -1）。
        bool properLevel =
            ((m_member[i].m_pUser->get_charac_level() >=
              m_battleField.m_pDungeon->get_standard_level() - 5) &&
             (m_member[i].m_pUser->get_charac_level() <=
              m_battleField.m_pDungeon->get_standard_level() + 3));
        if (properLevel)
        {
            bd.m_flag28[0] = 1;                // +0x354
            bd.m_flag28[1] = 1;                // +0x355
        }

        int maxBefore = getMemberMaxLevelBeforeDungeon();
        int lvBefore = m_member[i].m_pUser->get_charac_level_before_dungeon();
        if ((maxBefore - lvBefore >= 7) && (startCount != 1))
        {
            bd.m_flag28[2] = 1;                // +0x356
            if (isGuildParty() ||
                FindUserByCharId(m_member[i].m_pUser->get_connect_upper_member()))
                bd.m_flag28[3] = 0;            // +0x357
            else
                bd.m_flag28[3] = 1;
        }
        else
        {
            bd.m_flag28[2] = 0;
            bd.m_flag28[3] = 0;
        }

        if (m_battleField.m_pDungeon->get_dimension_possible() > 0)
        {
            bd.m_flag28[0] = 1;
            bd.m_flag28[1] = 1;
        }

        HistoryLog::WriteDunStart((_IO_FILE*)m_member[i].m_pUser->m_field796f8,
                                  m_member[i].m_pUser->get_charac_job(),
                                  m_member[i].m_pUser->get_charac_level(),
                                  (int)m_member[i].m_pUser->getCurCharacGrowType());

        if (getManager()->getMoveSpace() != 1)
        {
            // b28d-49 @0x85a1e6e：ORIG 按 `cmpl $0x1,-0x6c(%ebp); je` 分两臂各发一次
            // IncrementDungeonEntrance（85a1e8b false / 85a1ea9 true），非 bool 实参直传
            //（P167 MISSING GetInstanceDungeonEntranceLog/IncrementDungeonEntrance 各 -1）。
            if (partyType != 1)
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    dungeonIdx, false);
            else
                GetInstanceDungeonEntranceLog()->IncrementDungeonEntrance(
                    dungeonIdx, true);
        }
        if (partyType != 1)
            m_member[i].m_pUser->IncCurCharacDungeonPlayCount();
        if (partyType == 1)
        {
            user_creature::CCreatureMgr* cm =
                const_cast<CInventory*>(m_member[i].m_pUser->getCurCharacInvenR())->GetCreatureMgrW();
            cm->TurnStomach(false);
        }
        m_member[i].m_pUser->set_level_before_dungeon((short)m_member[i].m_pUser->get_charac_level());

        if (dungeon != 0)
            SetProperDungeonEvent(m_member[i].m_pUser, dungeonIdx);
        SetEndRouting();
        static_cast<XNuclear::CHades*>(m_member[i].m_pUser->getHades())->StdCateUpdate(2, 0);
    }

    for (int a = 0; a <= 1; ++a)
        for (int b = 0; b <= 3; ++b)
            m_field1e4[a][b] = 0;                          // +0x1e4[a][b]

    Reset_party_overlapped_drop_ratio();
    check_passive_skill();
    send_loadingcheck_msg();

    CEventBase* ev57 = GlobalData::s_event_manager->GetRepeatEvent(0x57);
    // b28d-49 @0x85a2076：ORIG 两臂各发一次 SetWeekendEvent（85a2086 true / 85a209b false），
    // 三目形只发一次（P167 MISSING SetWeekendEvent -1）。
    if (ev57->IsEventing(0))
        SetWeekendEvent(true);
    else
        SetWeekendEvent(false);

    QuickParty::RewardData_Param rewardParam(this);
    QuickParty::RewardData_Result rewardResult;
    set_random_reward((QuickParty::RandomBuffType)9, rewardParam, rewardResult);
    SendPartyMemberTagChracInfo();
    return 0;
}
#undef bf
#undef bd

// ---- CParty::get_item（ORIG T 0x85a3b98）----
// §6.2 巨件核销：P167 264/264 [0缺0多]；gap 2499→2；残差归因 N82（ORIG push %edi+槽基
// 错位）+ N18b/X1a（`!=1` 比较 4.4 恒折 cmp$1，ORIG xor$1+test 共 12 处不可翻）。
// 真修：①金币循环非 winner 臂原多发一次 Inven_Item::get_add_info()（EXTRA +1）——
// ORIG 85a410f 循环外把 share 拷入 -0xb4 副本槽、三目 else 臂读副本，补 noBonusGain 局部；
// ②删 `CBattle_Field& bf` 缓存槽（P183），9 处调用展开为 m_battleField 直调，
// 复现 ORIG 每次 lea 0xb24(%eax) 重物化。
// 返回 1=拾取成功；0=失败（错误码经 1/0x2e 包下发）。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x32c/0x350；2497 vs 2494
#include "CParty/get_item.inc"
}

// ---- CParty::drop_item（ORIG T 0x85a63f4）----
// §6.2 巨件核销：P167 108/108 [0缺0多]；gap 863→7；残差归因 N82（槽基错位）+
// N18b/X1a（比较物化）+ map_item 整块 rep movsl 传参形（N78 族跨版本）。
// 真修 5 处：①删 src.reset()（EXTRA：ORIG 仅 Inven_ItemC1 无 reset 调用）；
// ②t==2 delete 分支 const_cast<CCargo*>(getCurCharacCargoR()) → getCurCharacCargoW()
//（MISSING getCurCharacCargoW / EXTRA R 各 1，ORIG 85a6dba）；
// ③堆叠超限条件 m_addInfo2→m_addInfo（ORIG 85a7010 读 -0x11a=mi+0x12=+0x02 字段，
// 极性/字段错位真 bug）；④trace 第 3 参 get_add_info()→m_addInfo2 直读（ORIG 85a7042
// 读 -0x115 槽不 call，EXTRA get_add_info+1）；⑤尾包补 put_int(mi.m_item.m_fieldd)
//（MISSING put_int，ORIG 85a71e2 读 m_item+0xd，位于 put_short(m_fieldb) 之后）。
// 返回 0=成功；0xd=稀有度限制；0x11=删除/扣款失败；0x13=交易中；
// 0x16=落物失败；0x17=禁止丢弃；0xd1=服务限制；-1=item 查表失败。
// b28d-solo-CParty: N 封顶 —— N82 帧 + 槽；872 vs 863
// [轴 B 自校 N] tsV 行 0x85a6994 系 **switch 跳表边界**（ORIG `cmp $0x5,%eax;
// ja <跳表>`），我方该处 `cmp $0x5; jne`（4.4 决策树 lowering）⇒ 上下文不同构，
// 非类型符号性点（与 A15 `_makeDropItem` 同类假阳性）。未改源码。
#include "CParty/drop_item.inc"
}

// ============================================================================
// insert_time_*（ORIG 0x85b2114..0x85b22e1）：gen_timer_key 后向 TimerQueue
// 注册 OBJ_TYPE_1 定时器，param1=队伍索引，sec=调用方参数，param3=0。
// ============================================================================
#include "CParty/insert_time_dungeon_clear_msg.inc"
}

#include "CParty/insert_time_card_select_msg.inc"
}

#include "CParty/insert_time_clear_dungeon_reward_msg.inc"
}

#include "CParty/insert_time_dungeon_score_scroll_msg.inc"
}

#include "CParty/insert_time_card_select_right_msg.inc"
}

// ============================================================================
// CParty::set_play_result —— ORIG 0x085b2068：仅队长发起时把全员
// setPlayingDungeonIndex(-1) 并置 state=3 / m_recvResultFlag=1。
// ============================================================================
#include "CParty/set_play_result.inc"
}

// ============================================================================
// CParty::forced_set_play_result —— ORIG 0x085b26e8：非 state==3 时向全队
// b28d-solo-CParty: AE —— P248档② m_arr50 char[4]→bool[4]（BattleData.h，已 R2 广播）+ P285 空 else{return;} 复现尾 nop 岛；97=97
// 广播 0x22 结算强制包头（byte0/int0/byte0/byte0）。
// ============================================================================
#include "CParty/forced_set_play_result.inc"
}

// ============================================================================
// CParty::map_load_forced_finish —— ORIG 0x085b1e66：装载超时强制收尾；
// 空队走 destroy+日志，否则未完成装载的有效成员逐个 giveup_game。
// ============================================================================
#include "CParty/map_load_forced_finish.inc"
}

// ============================================================================
// 卡片选择（ORIG 0x85b401c..0x85b494e）。m_field204[type][slot]：
// dungeon_reward_card 表，0xff=未选。
// ============================================================================
#include "CParty/HaveAlreadySelectedCard.inc"
}

#include "CParty/_checkGoldCardSelectable.inc"
}

#include "CParty/SendEachCardInfo.inc"
}

// [SweepCParty] 真 bug：tryInsertItemIntoInventory 的 receiver 由 W 改 R（ORIG 0x085b42xx
// `call getCurCharacInvenR`；W 为历史误用）。p167 CLEAR（16=16）。残余 = N63（4.4 对
// if-return 物化 mov $0/jmp）+ N82 帧。
#include "CParty/SetCardNumber.inc"
#define reward (m_battleData.m_itemGroups2[slot][1])   // +0x7ad + slot*0x7a
    // ORIG 085b425f `call *%edx; xor $1,%eax; test; je 440e`：原码为 `if (!IsEventing(0))`
    // 正极性包裹 reward 段；tryInsertItemIntoInventory 为 const ⇒ R 版，返回值直比（无槽）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x41)->IsEventing(0))
    {
    if (reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()
            ->tryInsertItemIntoInventory(reward) == -1)
    {
        reward.m_addInfo = 0;                           // +0x7af / +0x7b4 清零
        reward.m_addInfo2 = 0;
    }
    else
    {
        if ((!GlobalData::s_event_manager->GetRepeatEvent(0x40)->IsEventing(0) &&
             !IsReturnUserParty() && !IsEventCharacParty()) || reward.m_field1 != 1)
        {
            // 不清奖励槽
        }
        // b28d-solo-CParty2（p167）：ORIG 0x85b43aa `call getCurCharacInvenW`（**非 const** 版，
        // 体内先 `call enableSaveInven` 再返回 inven 指针）⇒ 本处必须用 W 版（R 版无
        // enableSaveInven 副作用 = 真语义缺口）。上方 tryInsertItemIntoInventory 处
        // ORIG 0x85b4273 确为 R 版，保持不动。
        else if (reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                     ->check_empty_count(Inven_Item::ITEM_TYPE_EQUIP, 2) == 1)
        {
            // 空位充足，不清
        }
        else
        {
            reward.m_addInfo = 0;
            reward.m_addInfo2 = 0;
        }
    }
    }

    // C05③ 单尾 fail 块（Main 判据②多层形）：ORIG 5 条失败 jcc
    // （0x2b8/0x2fc/0x319/0x345，另 0x0d6）= 全部汇到同一 0x44c4（mov $1,%eax）
    // ⇒ 原码为嵌套 if + 单一公共尾 return，而非 5 个独立 `goto ret_true`
    // （后者令 4.4 每个 site 各复制一份尾块）。
    if (user != 0)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)8, 0, (ENUM_PACKETCLASS)0);
        int itemId = reward.m_addInfo;                  // -0x14
        if (itemId != 0)
        {
            CItem* item = G_CDataManager()->find_item(itemId);   // -0x10
            if (item != 0)
            {
                int rarity = item->get_rarity();        // -0xc
                if (rarity == 0 || rarity == 1 || rarity == 2 ||
                    rarity == 3 || rarity == 4)
                {
                    APSystem::CUserProc::ClearActionAndSendtoUser(
                        user, (APSystem::_ActionGroupIndex)0x15, 0,
                        (ENUM_PACKETCLASS)0);
                }
            }
        }
    }
ret_true:
    return true;
fail:
    return false;
}
#undef reward

#include "CParty/CompleteCardSelect.inc"
}

#include "CParty/SendCardSelectComplete.inc"
}

#include "CParty/IsCompleteCardSelectOfMembers.inc"
}

// ============================================================================
// CParty::TournamentDungeonClear —— ORIG 0x85bd236：锦标赛结算；逐有效成员
// 发卡牌奖励，胜利路径追加卡型 1 与通关日志，随后注册 0x15 秘商定时器。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧/槽 + P0 指针缓存；316 vs 311
#include "CParty/TournamentDungeonClear.inc"
}

// ============================================================================
// CParty::MakeTournamentReward —— ORIG 0x85bcb40：按当前轮次生成奖励与金币，
// 逐成员发经验/奖励包（0x184），置锦标赛清除状态并注册 0x2e 定时器。
// ============================================================================
// [SweepCParty 2026-09-09] gap 16→1（448 vs 449）。真 bug：generateRandomItem→
// generateSpecificItem（ORIG 虚槽 +0x0c；旧码 +0x08 调错虚函数）。P183/P0 去 bf/bd 引用
// 局部 + user/dungIdx 缓存；memcpy→逐字段赋值；dungeon const 化消 const_iterator 转换构造；
// victory 正极性；refLevel 副本 + ref 字段序（+0x35 先于 +0x34）。残余 = N85（x87 比较）
// + N7（寄存器角色）。p167 CLEAR。
#include "CParty/MakeTournamentReward.inc"
}

#include "CParty/IsCollectDungeonStatistic.inc"
extern Inven_Item g_emptySlot;      // ORIG 全局 0x943ddc0（AccountCargo.cpp 定义）

// ============================================================================
// CParty::OnKilledMonster —— ORIG 0x85b5a4c：怪物击杀反外挂核查链
// （伤害占比/跨图伤害/秒杀检测）后按 check_grid_clear 收图。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x44/0x74 + 槽；325 vs 331
#include "CParty/OnKilledMonster.inc"
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
    if (!bf.IsClearDungeon() && dieInfo.m_fieldA22 == 0 && user != 0)
    {
        // 实参序按 ORIG 0x85b5a91..0x85b5ac4 取证：
        //   arg2 = (unsigned short)dieInfo->m_fieldF  [movzwl 0xf(%eax),%eax]
        //   arg3 = (unsigned short)monsterIdx         [mov 0x10(%ebp),%eax; movzwl %ax,%edx]
        //   arg4 = dieInfo->m_killValue2              [mov 0xa1a(%eax),%eax]
        // 被调方 addMonsterDamage(nMonsterIndex, monsterId, damage) 以 **monsterId 为
        // map key**（CTraceMobDieHack.cpp:135 m_map1.insert(make_pair(monsterId, log))）
        // ⇒ ORIG 语义 = 按"怪物序号"键控、把 killer uid 记入 m_monsterIndex；
        // 旧码把两实参互换（按 killer uid 键控）为转写错误，已正。
        // ORIG 085b5aa7 段 `call addMonsterDamage; cmp $0x2,%eax; setg %al`（无结果槽）
        // ⇒ 结果直比，勿设具名 addCnt 局部（P0）。
        if (2 < m_traceMobDieHack
                    .addMonsterDamage((unsigned short)dieInfo.m_fieldF,
                                       (unsigned short)monsterIdx,
                                       (int)dieInfo.m_killValue2))
        {
            WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
            hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0xc9, 1, 0, 0);
        }
    }

    bool doCrossCheck = true;
    int dungIdx = bf.get_dungeon_index();
    if (dungIdx == 0x3e || dungIdx == 0x3f || dungIdx == 0x40)
        doCrossCheck = false;

    if (doCrossCheck && dieInfo.m_fieldA22 == 0 &&
        monster.m_roleType < 4 &&
        (unsigned int)dieInfo.m_killValue2 <
            (unsigned int)monster.m_active.getHpMax() &&      // +0x38 子对象
        monsterIdx != 10 && monsterIdx != 0xc351 && monsterIdx != 0x40a)
    {
        for (int i = 0; i < 4; ++i)                     // -0x20
        {
            if (checkValidUser(i))
            {
                CUser* member = get_user(i);
                void* memberParty = member->GetParty();
                void* userParty = user->GetParty();
                if (memberParty != 0 && userParty != 0 && member != 0)
                {
                    // ORIG OnKilledMonster.md 085b5c30/085b5c3d `mov 0xcac(%eax)` =
                    // CParty::m_battleField.m_pDungeon
                    CDungeon* mpDungeon =
                        reinterpret_cast<CParty*>((memberParty))->m_battleField.m_pDungeon;
                    CDungeon* upDungeon =
                        reinterpret_cast<CParty*>((userParty))->m_battleField.m_pDungeon;
                    if (mpDungeon == 0 || upDungeon == 0)
                    {
                        if (member == user)
                            user->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0x70, 1, 0, 0);
                        else
                            member->getHackAnalyzer()->addServerHackCnt(
                                member, (WongWork::ENUM_HACKTYPE)0x71, 1, 0, 0);
                    }
                    else
                    {
                        bool special = mpDungeon->get_index() == 0x2afe ||
                                       upDungeon->get_index() == 0x2afe;
                        if (special)
                        {
                            if (member == user)
                                user->getHackAnalyzer()->addServerHackCnt(
                                    user, (WongWork::ENUM_HACKTYPE)0x70, 1, 2, 0);
                            else
                                member->getHackAnalyzer()->addServerHackCnt(
                                    member, (WongWork::ENUM_HACKTYPE)0x71, 1, 2, 0);
                        }
                        else if (member == user)
                        {
                            user->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0x70, 1, 0, 0);
                        }
                        else
                        {
                            member->getHackAnalyzer()->addServerHackCnt(
                                member, (WongWork::ENUM_HACKTYPE)0x71, 1, 0, 0);
                        }
                    }
                }
            }
        }
    }

    bool instantCheck = false;
    if (dieInfo.m_fieldA22 == 0 && monster.m_roleType < 4)
    {
        MapInfo* mapInfo = bf.GetCurrentMapInfo();
        if (mapInfo->m_killChecker1.getUseSkillMaterialTime() == 0)
            instantCheck = true;
    }
    if (instantCheck)
    {
        int level = 0;                                  // -0x10
        // P167（修正版）实证：ORIG 0x85b5e94/e4a/e58 三级判定各调一次 getHpMax
        //（共 3 次），无 hpMax 缓存 —— 原缓存少 2 次 call。
        if ((unsigned int)monster.m_active.getHpMax() * 4 <
            (unsigned int)dieInfo.m_killValue2)
            level = 3;
        else if ((unsigned int)monster.m_active.getHpMax() * 3 <
                 (unsigned int)dieInfo.m_killValue2)
            level = 2;
        else if ((unsigned int)monster.m_active.getHpMax() * 2 <
                 (unsigned int)dieInfo.m_killValue2)
            level = 1;
        if (level != 0)
        {
            WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
            hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x86, level, 0, 0);
        }
    }
    if (bf.check_grid_clear())
        bf.onClearMap(true);
}
#undef bf

// ============================================================================
// CParty::OnFightVillageMonster —— ORIG 0x85b9596：村庄受攻战入场；管理器
// 受理后走 game_start 全流程并广播 0x1c/0x1d 地图包。
// ============================================================================
// [SweepCParty] 补真缺口：_getMemberNames 两次调用（第二次在循环内取返回值作
// EnterDungeon 实参，旧码仅 1 次）+ names 改 `={0}` 消 EXTRA memset。
// p167 CLEAR（余 C1/C2、D1/D2 别名）。
#include "CParty/OnFightVillageMonster.inc"
}

// ============================================================================
// CParty::OnFinishVillageMonsterFighting —— ORIG 0x85b99ac：村庄战收尾；
// 结算胜负、giveup_game 并对全员发 0x1ec 复仇地下城资格包。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧/槽；307 vs 302
#include "CParty/OnFinishVillageMonsterFighting.inc"
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
    if (bf.check_grid_clear())
        isClear = true;
    else
        notClear = true;
    // ORIG 0x085b9ab3 `movzbl -0x3d(%ebp),%edx`（=isClear）⇒ 旧码误传 notClear，真 bug。
    ret = GlobalData::s_villageMonsterMgr->OnKillVillageMonster(user, isClear);
    if (ret == 0)
    {

    giveup_game(user, false, isClear, flag);
    // ORIG OnFinishVillageMonsterFighting.md 085b9b12 `movw $0x0,0xc4c(%eax)` =
    // CBattle_Field(this+0xb24)+0x128 → m_villageMonsterId（short）
    reinterpret_cast<CBattle_Field*>(&m_battleField)->m_villageMonsterId = 0;
    {
        PacketGuard ack;                                // -0x60
        ack.clear();
        ack.put_header(1, 0x10c);
        ack.put_byte(1);
        ack.finalize(true);
        user->Send(ack);
    }
    if (!GlobalData::s_villageMonsterMgr->OnEvent() &&
        GlobalData::s_villageMonsterMgr->GetRewardType() == 1)
    {
        // [轴 B，地址自校 0x085b9be0] ORIG `mov -0x2c,%eax; cmp -0x30,%eax;
        // jb +0x24a` = **无符号**比较（我方旧文 int 产 `jl`，符号性相反）。
        // 以 ORIG 为准改 unsigned：`remain` 仍为 int（仅写入 body+2 的 4 字节块）。
        unsigned int nowSec =
            (unsigned int)GlobalData::s_systemTime_.getCurSec();     // -0x34
        unsigned int closeTime =
            (unsigned int)GlobalData::s_revengeDungeonMgr->GetCloseTime();  // -0x30
        int remain =
            (int)(closeTime < nowSec ? 0 : closeTime - nowSec);      // -0x2c
        for (int i = 0; i < 4; ++i)                     // -0x28
        {
            if (members[i] != 0)
            {
                char body[10];                          // -0x6a
                memset(body, 0, sizeof(body));
                *(int*)(body + 2) = remain;             // ORIG body+2 写 remain
                PacketGuard packet;                     // -0x78
                packet.put_header(0, 0x1ec);
                // ORIG 0x085b9c68：size() 先于 attackCount（Ghidra local_24/local_20 序）。
                // [判据③/N18b] ORIG 槽 -0x20(uint size)/-0x1c(uint attackCount，uchar 截断后
                //   存 32 位槽：`movzbl %al,%eax; mov %eax,-0x1c`)/-0x18(循环变量 uint)/
                //   -0x14/-0x10/-0xc 三字段。旧码 attackCount 用 uchar 槽(-0x19)、
                //   循环变量 int（ORIG 尾测 `setb` 无符号）⇒ 全部 32 位无符号化。
                unsigned int condCnt =
                    (unsigned int)
                        G_CDataManager()->m_serverParameter.m_revenge498.size();
                unsigned int attackCount =
                    (unsigned char)reinterpret_cast<CUserCharacInfo*>(members[i])
                        ->GetCurVillageAttackCount();
                if (2 < attackCount)                    // ORIG body+6 条件写 0x1e41
                    *(int*)(body + 6) = 0x1e41;
                packet.put_binary(body, 10);
                packet.finalize(true);
                members[i]->Send(packet);
                members[i]->SetVillageAttackedReward(false);
                // P175：ORIG 走 size()+operator[] 索引形（p167 MISS operator[]/size，
                // EXTRA 迭代器族）；每次下标重取 G_CDataManager()（3 次独立调用）。
                // P183 反向：ORIG 无 CUserCharacInfo* 具名缓存，两处调用点各自从
                //   `-0x50(%ebp,%eax,4)` 重取 members[i]（旧码 ci 缓存槽 ⇒ 实参装载序差）。
                for (unsigned int k = 0; k < condCnt; ++k)
                {
                    unsigned int f0 = G_CDataManager()
                        ->m_serverParameter.m_revenge498[k].m_field0;
                    unsigned int f4 = G_CDataManager()
                        ->m_serverParameter.m_revenge498[k].m_field4;
                    int f8 = G_CDataManager()
                        ->m_serverParameter.m_revenge498[k].m_field8;
                    // P196 折痕方向：ORIG 0x085b9d66 `mov -0x1c(attackCount),%eax;
                    //   cmp -0x14(f0),%eax; jb` ⇒ 写成 attackCount 居左才出该装载序。
                    if (attackCount >= f0 && attackCount <= f4)
                    {
                        reinterpret_cast<CUserCharacInfo*>(members[i])
                            ->SetCurRevengeDungeonCount((unsigned char)f8);
                        reinterpret_cast<CUserCharacInfo*>(members[i])
                            ->VillageAttack_DBUpdate();
                        break;
                    }
                }
            }
        }
    }
        ret = 0;                                        // ORIG 085b9de7 `mov $0x0,%ebx`
    }
    }
    return ret;
}
#undef bf

// ============================================================================
// CParty::prepare_dungeon_clear —— ORIG 0x85aa39a：清图前处理（追踪器、任务
// 认证、宠物经验）并视条件触发 ClearDungeon。
// ============================================================================
#include "CParty/prepare_dungeon_clear.inc"
}

// ============================================================================
// CParty::party_dungeon_fail —— ORIG 0x85b3670：全灭败北处理（计时差记录、
// 逐成员退场与惩罚、状态复位或解散；锦标赛路径改发奖励）。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x6c/0x8c + P167 EXTRA new/delete landing（P90/P83）；463 vs 444
// [轴 B 自校 N] tsV 两行（0x85b39c5/0x85b3be8）系**序号错配假阳性**：我方循环尾
// 实为 `addl $1,-0x2c; cmpl $0x3,-0x2c; setle` = ORIG 的 setle（槽号 -0x2c vs
// ORIG -0x28 致错配）。未改源码。
#include "CParty/party_dungeon_fail.inc"
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
#define bd (m_battleData)
    char skipPanalty = 0;                               // -0x31
    CDungeon* dungeon = m_battleField.m_pDungeon;                      // -0x30
    if (dungeon != 0)
    {
        // ORIG party_dungeon_fail.md 085b36cb `movzbl 0x89f(%eax)` =
        // CDungeon::m_bloodDungeonType（unsigned char，宽度修正：原 char→movsbl 异形）
        skipPanalty = (dungeon->m_bloodDungeonType > 0) ? 1 : 0;
        if (dungeon->isTowerOfDespairDungeon())
            skipPanalty = 1;
    }
    // ORIG 085b36f0 `movzwl 0xc4c(%eax)` = m_villageMonsterId（非 0 判）
    if (reinterpret_cast<CBattle_Field*>(&m_battleField)->m_villageMonsterId != 0)
        skipPanalty = 1;
    if (dungeon != 0 &&
        (dungeon->get_dimension_possible() >= 1 ||
         // ORIG 085b3718 `movzbl 0x89c(%eax)` = CDungeon::m_89c（unsigned char，
         // 宽度修正：原 (char*)→movsbl 异形）
         dungeon->m_89c != 0))
        skipPanalty = 1;

    if (dungeon->isTournamentDungeon())
    {
        bf.m_tournamentReward.ResetReward();
        MakeTournamentReward(bf.m_tournamentReward);
        return;
    }

    unsigned int now =
        (unsigned int)GlobalData::s_systemTime_.getCurTickCount();
    unsigned int elapsedLow = now - (unsigned int)bd.m_field20;
    bd.m_field0 = (int)(elapsedLow - (unsigned int)bd.m_field2c);
    bd.m_field4 =
        (int)((0u - (unsigned int)(now < (unsigned int)bd.m_field20)
               - (unsigned int)bd.m_field24 - (unsigned int)bd.m_field30)
              - (unsigned int)(elapsedLow < (unsigned int)bd.m_field2c));
    SaveCoin((eCoinSubReason)2);
    CheckHackAverageHitCount();
    m_secu.dungeonFail();

    PacketGuard packet;
    for (int slot = 0; slot < 4; ++slot)                // -0x2c
    {
        if (_checkValidUser(slot))
        {
            CUser* user = m_member[slot].m_pUser;
            OnFailedDungeonClear(user);
            G_GameWorld()->out_from_dungeon(user);
            if (user->getMoveSpace() != (UserSpace::T)1)
            {
                GetInstanceDungeonEntranceLog()->DecrementDungeonEntrance(
                    dungeon->get_index() != 0, true);
            }
            if (user->getMoveSpace() == (UserSpace::T)1)
            {
                BlueMarble* marble =
                    reinterpret_cast<BlueMarble*>(user->getBlueMarble());
                if (marble != 0)
                    marble->giveupDungeon(user);
            }
            if (m_battleField.m_bloodState != 1)
            {
                // ORIG 085b3952 `movzbl 0x85c` = m_entryRestrictFlag；
                //      085b396e `movzbl 0x87a` = m_villageAttackEventFlag
                //（两处 unsigned 读，与 CParty.cpp:2303/2319 既有
                //  `(unsigned char)` 消费形态同口径）
                char has85c =
                    (dungeon != 0 &&
                     (unsigned char)dungeon->m_entryRestrictFlag != 0) ? 1 : 0;
                char has87a =
                    (dungeon != 0 &&
                     (unsigned char)dungeon->m_villageAttackEventFlag != 0) ? 1 : 0;
                if (skipPanalty != 1 && has85c != 1 && has87a != 1)
                    user->giveup_panalty();
            }
        }
    }

    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short((short)get_member_count());
    if (m_autoCreated == 0)                             // +0x13c
    {
        for (int i = 0; i < 4; ++i)
        {
            if (_checkValidUser(i))
                m_member[i].m_pUser->make_basic_info((char*)&packet, 0);
        }
        set_state(1);
        battle_data_reset();
        packet.finalize(true);
        if (!IsExistInvisible())
            G_GameWorld()->send_all(packet);
        else
            send_to_party(packet);
    }
    else
    {
        short mgrIndex = m_manager->GetPartyIndex();
        int mgrState = m_manager->get_state();
        unsigned int accId = m_manager->get_acc_id();
        char* idStr = NumberToString(accId, 0);
        // [P227/N83] 删 placement-new（ORIG 无 _ZnwjPv/_ZdlPvS_）⇒ 具名对象直构。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3541, 0);
        trace(
            "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)",
            idStr, mgrState, (int)mgrIndex);
        m_manager->SetPartyIndex(-1);
        if (getManager()->getMoveSpace() != (UserSpace::T)1)
            m_manager->set_state((ch_state)3);
        m_manager->make_basic_info((char*)&packet, 0);
        destroy();
        packet.finalize(true);
        if (getManager()->getMoveSpace() == (UserSpace::T)1)
            send_to_party(packet);
        else
            G_GameWorld()->send_all(packet, m_host);
    }
}
#undef bf
#undef bd

// ============================================================================
// CParty::dungeon_clear —— ORIG 0x85ab736：胜利结算；发放三组奖励物品与金卡，
// 清空奖励槽并注册 0x15 秘商定时器。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x50/0x70 + P167 缺口；325 vs 389
#include "CParty/dungeon_clear.inc"
#define bd (m_battleData)
    // [P331/P212 重排声明序] ORIG 序言 `movl $0x0,-0x20(%ebp)` 出现在
    // `lea -0x2c(%ebp); call PacketGuard::C1Ev` **之前** ⇒ 该 0 初始化变量
    // （goldCardCount）声明先于 PacketGuard；memberCount 赋值在 ctor 之后（-0x1c）。
    int goldCardCount = 0;                              // ORIG -0x20（ctor 前）
    PacketGuard packet;                                 // ORIG -0x2c
    int memberCount = get_member_count();               // ORIG -0x1c
    for (int slot = 0; slot <= 3; ++slot)               // -0x18
    {
        if (slot > 3)
        {
            SetEPLPState(2);
            int sec = 0x3c;                             // -0x1c
            if (m_secretShop.IsOpen())
                // ORIG dungeon_clear.md 085abb54 `mov 0xa784(%eax)` =
                // SecretShopScript::m_int84（同 TournamentDungeonClear 站点）
                sec = G_CDataManager()->m_secretShop.m_int84 * 12;
            int key = gen_timer_key((TIMER_MESSAGE)0x15);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                        GetPartyIndex(), (TIMER_MESSAGE)0x15,
                                        sec, key, 0);
            set_state(1);
            sendInoutConditionDungeon(
                (RetryFailReason::T)checkInoutConditionDungeon());
            if (IsPremiumGoldCardParty())
                sendPremiumGoldCardStatistic(m_battleField.m_pDungeon->get_standard_level(),
                                             memberCount, goldCardCount);
            CheckHackAverageHitCount();
            ClearMapHitCount();
            m_secu.dungeonClear();
            return;
        }
        if (_checkValidUser(slot))
        {
// §P331 第二形（实测 634→390）：ORIG 对 m_member[slot].m_pUser 与
// m_itemGroups{0,1,2}[slot] 每次访问都从 this 重物化（imul$0x7a 13 处、add$0x79d 4 处、
// add$0x760 3 处；本方原 3/0/0 —— 被 user + g0/g1/g2 四个具名指针吸收）。slot 为循环
// 变量、体内不改 slot/this ⇒ 逐次寻址同物，无可观察行为变化。
#define user (m_member[slot].m_pUser)
#define g0 (m_battleData.m_itemGroups0[slot])
#define g1 (m_battleData.m_itemGroups1[slot])
#define g2 (m_battleData.m_itemGroups2[slot])
            int put = _putItemIntoUser(user, &g0[0], packet);
            put += _putItemIntoUser(user, &g1[0], packet);
            if (HaveAlreadySelectedCard(
                    (char)slot, (eClearRewardCardType_t)1))
            {
                CItem* card = G_CDataManager()->find_item(g2[1].m_addInfo);
                if (card != 0)
                {
                    int price = 0;
                    G_Store()->GetSellItemPrice(g2[1], card,
                                                (short)g2[1].m_addInfo2,
                                                false, price);
                    GetInstanceValueStatistic()->AddValueStatistic(
                        (VALUE_STATISTIC_FIELD)5, user, (unsigned int)price);
                }
                put += _putItemIntoUser(user, &g2[0], packet);
                CEventBase* ev40 =
                    GlobalData::s_event_manager->GetRepeatEvent(0x40);
                bool giveGold = ev40->IsEventing(0) || IsReturnUserParty() ||
                                IsEventCharacParty();
                if (giveGold && !IsPremiumGoldCardParty())
                    put += _putItemIntoUser(user, &g2[0], packet);
                ++goldCardCount;
            }
            (void)put;
            g0[0].reset();
            g0[1].reset();
            g1[0].reset();
            g1[1].reset();
            g2[0].reset();
            g2[1].reset();
            bd.ResetEventDungeonClearPoint();
            HistoryLog::WriteDunClear((FILE*)user->m_field796f8);
            OnSuccessedDungeonClear(user);
        }
    }
}
#undef bd
#undef user
#undef g0
#undef g1
#undef g2

// ============================================================================
// CParty::_putItemIntoUser —— ORIG 0x85aab84：双槽奖励发放（items[0] 金币/
// 主物、items[1] 装备），含背包失败邮件退还与幸运点扣减。
// ============================================================================
// §6.2 巨件核销：P167 126/126 [0缺0多]；gap 739→+21（our 760）；残差归因 N82
//（槽基错位）+ N80/N83（cMyTrace 日志段跨版本求值/构造形）。
// 真修 7 处：①put_binary 重载选错 ×3——(char*) 强转选中非 const 版
// put_binary(char*,int)，ORIG 全为 put_binary(const char*,int)（PKc），改 (const char*)
// /删强转走完整链；②金币日志段合并回单 if（误拆双 if 多发一次 get_add_info 条件）；
// ③删 placement new×2（_ZnwjPv/_ZdlPvS_/_Unwind_Resume EXTRA），改 cMyTrace 临时
// 链式（P37）；④删 oldMoney 死局部（EXTRA get_money+1）；⑤金币尾包 put_int(get_money)
// 改 getCurCharacInvenR() 完整链（MISSING R，ORIG 85aae9b）；⑥avatar 段 avMgrR 缓存
// 展开（P183）：getJewelSocketData/GetExpansionInfo 各走 R→GetAvatarItemMgrR 完整链
//（MISSING R/GetAvatarItemMgrR 各 1）；⑦金币尾包补第 4 个 put_byte(0)
//（MISSING put_byte，ORIG 85aaf37，put_short(0) 之后 put_packet 之前）。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x1fc/0x20c；761 vs 739
#include "CParty/_putItemIntoUser.inc"
}

// ============================================================================
// CParty::onBloodClear —— ORIG 0x85b7c9e：血之副本单轮结算日志与成败回调。
// ============================================================================
#include "CParty/onBloodClear.inc"
}

// ============================================================================
// CParty::BloodClearRewardPacket —— ORIG 0x85b8bf8：0xd1 血轮奖励包
// （四组 int 对 + 组数，物品表按 0x3d 步进）。
// ============================================================================
#include "CParty/BloodClearRewardPacket.inc"
}

// ============================================================================
// CParty::generateBloodRewardExp —— ORIG 0x85b7ea2：血轮经验生成
// （基础×轮次权重×进出次数权重 → getClearRewardBonusExp 加成聚合）。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x140/0x170；349 vs 316
#include "CParty/generateBloodRewardExp.inc"
}

// ============================================================================
// CParty::beginBloodClearProcess —— ORIG 0x85b75d4：血轮结算主流程
// （状态机推进 → 逐成员奖励/经验/记录包 0xd0 → 0x23 定时器）。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0xb0/0xc0；404 vs 475
#include "CParty/beginBloodClearProcess.inc"
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
    if (bf.getBloodState() >= 3)
        return;

    int playTime = 0;                                   // -0x34
    int round = 0;                                      // -0x40[0]
    int bestTime = 0;                                   // -0x30
    int bestRound = 0;                                  // -0x2c
    int ultimatePoint = 0;                              // -0x28
    playTime = bf.m_bloodReward.getPlayTime();
    if (flag)
    {
        round = bf.getBloodMaxRound();
        bf.m_bloodReward.setCurrentBloodRound(round);
    }
    else
    {
        round = bf.m_bloodReward.getCurrentBloodRound();
    }

    if (!bf.setBloodState((ENUM_BLOOD_STATE)3))
        return;

    // ORIG beginBloodClearProcess.md 085b76b1 `movzbl 0x89f(%eax)` =
    // CDungeon::m_bloodDungeonType（unsigned char 宽度修正）
    if (flag && m_battleField.m_pDungeon->m_bloodDungeonType == 2)
    {
        short maxRound = (short)bf.getBloodMaxRound();
        ultimatePoint =
            bf.getNextRoundDifficulty((short)(maxRound - 1));
        bf.m_bloodReward.incUltimateRoundClearPoint(ultimatePoint);
    }

    for (int slot = 0; slot < 4; ++slot)                // -0x20
    {
        if (!(get_user(slot) == 0 || !checkValidUser(slot)))
        {
            // P183 反向（ORIG 085b778b/77b2/77d9/7800 实证）：ORIG 无具名 user/ci
            // 缓存，每个使用点各自 9 条重算 m_member[slot].m_pUser（add $0x78 序列）。
            get_user(slot)->setChattingMessageCount(0);
            onBloodClear(flag, slot);
            reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->set_charac_party_bonus_exp(0);
            reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->set_charac_member_bonus_exp(0);
            reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->set_charac_fatigue_buf_bonus_exp(0);
            reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                ->set_charac_seria_buf_bonus_exp(0);

            checkBloodRenewMyRecord(slot, playTime, round,
                                    bf.get_dungeon_index());

            int itemMax = round * 10 / bf.getBloodMaxRound();   // -0x18
            if (itemMax == 0)
                itemMax = 1;
            int itemLimit = 10;                         // -0x40[1]
            int itemCount = std::min(itemLimit, round);
            // 报告 generateBloodRewardItem.md + ORIG 0x85b78ef/78e7 实证：第 2 参为
            // itemMax（setRewardItem/循环次数），第 4 参为 itemCount（field324 乘数）；
            // 原序把两者写反（round=0 时行为可见差异）。
            generateBloodRewardItem(slot, itemMax,
                                    (unsigned int)reinterpret_cast<CUserCharacInfo*>(
                                        m_member[slot].m_pUser)->get_charac_level(),
                                    (unsigned int)itemCount, flag);

            unsigned int mobReward = 0;                 // -0x44
            G_CDataManager()->get_mob_reward(
                reinterpret_cast<CUserCharacInfo*>(m_member[slot].m_pUser)
                    ->get_charac_level(),
                mobReward);
            QuickParty::RewardData_Param rp(this);
            QuickParty::RewardData_Result rr;
            rr.m_value00 = (int)mobReward;
            set_random_reward((QuickParty::RandomBuffType)0, rp, rr);
            mobReward = (unsigned int)rr.m_value00;

            // ORIG 085b799e/3d2 `add $0x6164,%eax; call 082f10ee`
            //（= _ZNKSt6vectorIfSaIfE4sizeEv，nm 实证）：0x6164 =
            // CDataManager::m_bloodDungeon(+0x5e30)+0x334 → m_vec334.size()。
            // 原 vptr 解引用+8 裸读形态错误，改真实 size() 调用（-O0 产 call）。
            int inoutSize =
                static_cast<int>(G_CDataManager()->m_bloodDungeon.m_vec334.size());
            // ORIG 085b79d0/404 `call GetCharacExpandData(1); call 085bfe88`
            //（= _ZNK18CCharacBloodDungeon13GetEnterCountEv，读 +0x18）——
            // 原本地视图读 +0x0c（m_weeklyEnterCount）为偏移错读，修正。
            int enterCount =
                reinterpret_cast<CCharacBloodDungeon*>(
                    m_member[slot].m_pUser->GetCharacExpandData(
                        (ENUM_CHARAC_EXPAND_TYPE)1))
                    ->GetEnterCount();   // -0x6c
            int expLimit = inoutSize - 1;               // -0x40[2]
            int inoutCnt = std::min(expLimit, enterCount);
            generateBloodRewardExp(slot, round, mobReward,
                                   (short)inoutCnt);

            unsigned int rewardExp =
                bf.m_bloodReward.getRewardExp(slot);    // -0x10
            if (1000000 < rewardExp)
            {
                LogManager::logFormat(
                    1, "party.cpp",
                    __PRETTY_FUNCTION__, 0x3c79,
                    "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP(%u)\n",
                    rewardExp);
                rewardExp = 1000000;
                bf.m_bloodReward.setRewardExp(slot, 1000000);
            }
            if (round == 0)
            {
                playTime = 0;
                round = 0;
            }
            bestTime = (int)reinterpret_cast<CUserCharacInfo*>(
                m_member[slot].m_pUser)->getBloodBestTimeRecord();
            bestRound = reinterpret_cast<CUserCharacInfo*>(
                m_member[slot].m_pUser)->getBloodBestRoundRecord();
            if (bestRound == -1)
            {
                bestTime = playTime;
                bestRound = round;
            }
            PacketGuard packet;                         // -0x78
            packet.put_header(0, 0xd0);
            packet.put_int(playTime);
            packet.put_int(round);
            packet.put_int(bestTime);
            packet.put_int(bestRound);
            packet.put_int(bf.getBloodMaxRound());
            packet.put_int((int)bf.m_bloodReward.getRewardExp(slot));
            packet.finalize(true);
            m_member[slot].m_pUser->Send(packet);

            int key = gen_timer_key((TIMER_MESSAGE)0x23);
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                        GetPartyIndex(), (TIMER_MESSAGE)0x23,
                                        8, key, 0x23);
        }
    }
}
#undef bf

// ============================================================================
// CParty::generateBloodRewardItem —— ORIG 0x85b840e：血轮物品奖励
// （rand<1000 分稀有/普通两生成路径，终局轮追加终极奖励）。
// ============================================================================
// [SweepCParty] 真 bug/缺口：find_item 目标由 CDataManager 改 CItemList（ORIG
// `CItemList::find_item(G_CDataManager()+0xc)`）；ref 初始化 memcpy→逐字段赋值；
// 去 dm 缓存（ORIG 两次独立 call G_CDataManager）。
// b28d-solo-CParty2：迭代器 begin/de 按 ORIG 6/6 对账修正（rare 段首处内联 begin
// 表达式 + 非 const `it` 复用两次 de；normal 段恢复第二对 begin/de）⇒ p167 只剩
// C1/C2、D1/D2 名族与 CLuckPoint EXTRA（判 N/P204：删独立局部会少发射 12 个 ORIG
// 同名 W 副本，账面回退）。ndiff 360→328。
#include "CParty/generateBloodRewardItem.inc"
#define bf (*reinterpret_cast<CBattle_Field*>(&m_battleField))
    bf.m_bloodReward.setRewardItem(slot, round);
    for (int i = 0; i < round; ++i)                     // -0x3c
    {
        WongWork::stGenerateRefData_t ref;              // -0x198
        WongWork::stGenerateResult_t result;            // -0xa4
        CLuckPoint luckPoint;                           // -0x98
        // [SweepCSecurityCard 2026-09-10] 已试形（回滚）：ORIG 0x085b8472
        // `lea -0xa0(%ebp),%eax; add $0xc,%eax` 指向 result.m_luckPoint
        //（stGenerateResult_t 内嵌 ::CLuckPoint@+0x0c）；改 `GetLuckPoint(&result.m_luckPoint,…)`
        // 删独立局部后 p167 EXTRA 消失，但本 TU 少发射 12 个 ORIG 同名 W 副本
        // ⇒ compare 四数 147/323→144/314 账面回退，按验收「不回退」回滚，判 N/P204。
        GetLuckPoint(&luckPoint, false);
        unsigned int roll = (unsigned int)get_rand_int(1000);   // -0x38
        // ORIG generateBloodRewardItem.md 085b849e `mov 0x6154(%eax)` /
        // 085b84b0 `mov 0x6158(%eax)`（int）：0x6154/58 =
        // CDataManager::m_bloodDungeon(+0x5e30)+0x324/0x328 →
        // stBloodDungeon_t::m_field324[0]/[1]（int，稀有/普通掉落基数）
        // P0/P183：ORIG 0x085b8499/085b84ab 两次独立 call G_CDataManager（无 dm 缓存）。
        unsigned int rareLine =
            G_CDataManager()->m_bloodDungeon.m_field324[0] * param2;    // -0x34
        unsigned int normalLine =
            G_CDataManager()->m_bloodDungeon.m_field324[1] * param2;    // -0x30

        if (roll < rareLine)
        {
            // ORIG 0x085b84c9..0x085b8505 逐条：+0x35=0、+0x34=param1、+0x36=1、
            // +0x38=movl 1、+0x3c/+0x3e/+0x40/+0x42=movw 1（无 memcpy）。
            ref.m_field35 = 0;                          // +0x35
            ref.m_field34 = (char)param1;               // +0x34
            ref.m_rateSlot = 1;                         // +0x36
            ref.m_rankBonusIndex = 1;                   // +0x38
            ref.m_goldBase = 1;                         // +0x3c
            ref.m_goldFactor = 1;                       // +0x3e
            ref.m_field40 = 1;                          // +0x40
            ref.m_field42 = 1;                          // +0x42

            WongWork::stGenerateResult_t specific;      // -0xd4
            WongWork::CItemGeneratorMgr* genMgr =
                G_CDataManager()->getItemGenerator();
            genMgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                ->generateSpecificItem(ref, specific);
            // b28d-solo-CParty2（p167 对账）：ORIG rare 段 begin/de = (0x85b8569,8577)
            // + (0x85b8643,8674) + 0x85b86ad ⇒ 首处内联 begin 表达式（无 item 引用槽），
            // 后续用**非 const 迭代器** `it`（ORIG 的 operator* 为
            // `_ZNK...IP10Inven_Item...deEv` 非 const 版，两次 de 复用同一迭代器）。
            bf.m_bloodReward.setRewarItemCopy(
                *specific.m_dropItems.begin(), slot, i);

            int dungIdx =
                m_battleField.m_pDungeon != 0 ? m_battleField.m_pDungeon->get_index() : 0;   // -0x2c
            std::vector<Inven_Item>::iterator it = specific.m_dropItems.begin();
            QuickParty::RewardData_Param rp(this);
            rp.m_param4 = dungIdx;
            QuickParty::RewardData_Result rr;
            // [SweepCSecurityCard 2026-09-10] ORIG 0x085b8679 `mov 0x7(%eax),%eax`
            // 直读 +0x07（Inven_Item::m_addInfo2），无 get_add_info out-of-line
            // call（p167 EXTRA get_add_info ×1）。
            rr.m_value00 = (*it).m_addInfo2;
            set_basic_reward((QuickParty::BasicRewardType)0xb, rp, rr);
            Inven_Item& mutable_item = const_cast<Inven_Item&>(*it);
            mutable_item.m_addInfo2 = rr.m_value00;
            // ORIG 085b86bd `mov 0x6160(%eax)`：m_bloodDungeon+0x330 →
            // stBloodDungeon_t::m_field330（float，flds 消费同宽）
            float addRate = G_CDataManager()->m_bloodDungeon.m_field330;
            bf.m_bloodReward.setRewardItemAddInfo(addRate, slot, i);
        }
        else if (roll < normalLine)
        {
            // ORIG 0x085b8720..0x085b874a 逐条：+0x35=0、+0x34=param1、
            // +0x38=movl 0x3f800000（1.0f 位型）、+0x40=指针整字、+0x3c=movl 0。
            ref.m_field35 = 0;
            ref.m_field34 = (char)param1;
            ref.m_rankBonusIndex = 0x3f800000;          // +0x38
            ref.m_dungeon = m_battleField.m_pDungeon;   // +0x40
            ref.m_field3c = 0;                          // +0x3c

            WongWork::CItemGeneratorMgr* genMgr =
                G_CDataManager()->getItemGenerator();
            genMgr->getGenerator((WongWork::CItemGeneratorMgr::eGenerateType_t)2)
                ->generateRandomItem(ref, result);
            if (!result.m_dropItems.empty())
            {
                const Inven_Item& item = *result.m_dropItems.begin();
                // b28d-solo-CParty2（p167 对账）：ORIG normal 段 begin/de =
                // (0x85b87ba,87c8) + (0x85b886c,887a) 两对 ⇒ setRewarItemCopy 实参
                // 写内联 begin 表达式（勿复用 item 引用；O0 无 CSE）。
                bf.m_bloodReward.setRewarItemCopy(
                    *result.m_dropItems.begin(), slot, i);
                // ORIG 0x085b8926 `CItemList::find_item(G_CDataManager()+0xc)`。
                CItem* card =
                    G_CDataManager()->m_itemList->find_item(item.m_addInfo);
                if (card != 0)
                    CLuckPoint::UseLuckPoint(m_member[slot].m_pUser,
                                             (int)(unsigned char)item.m_field1,
                                             card->get_rarity(),
                                             card->get_grade());
            }
        }
    }

    // ORIG 085b8a12 `movzbl 0x89f(%eax)` = CDungeon::m_bloodDungeonType
    //（unsigned char 宽度修正）
    if (flag && m_battleField.m_pDungeon->m_bloodDungeonType == 2)
    {
        int ultIdx = bf.m_bloodReward.getUltimateRewardItem();   // -0x24
        // ORIG 0x085b8a46 `CItemList::find_item(G_CDataManager()+0xc)`。
        CItem* card = G_CDataManager()->m_itemList->find_item(ultIdx);   // -0x20
        if (card != 0)
        {
            WongWork::stGenerateResult_t specific;
            Inven_Item made;                        // -0x14e（默认构造）
            G_CDataManager()->m_itemList->create_item(
                ENUM_ITEM_CREATE_TYPE_1, made, 1);
            specific.m_dropItems.push_back(made);
            const Inven_Item& item = *specific.m_dropItems.begin();
            bf.m_bloodReward.setRewarItemCopy(item, slot, round - 1);
        }
    }
}
#undef bf

// ============================================================================
// CParty::onBloodRewardTimer —— ORIG 0x85b6a48：0x22/0x23/0x24/0x25 血轮
// 定时器状态机（奖励包→物品/经验发放→收尾）。
// ============================================================================
// §6.2 巨件核销：P167 88/88 [0缺0多]；gap 766→46；残差归因 N82（帧 0x1cc vs 0x1ec
// 全帧槽基错位）+ P168a③b（checkValidUser bool callee 调用侧 xor$1 物化，跨版本
// 不可翻）+ 分支布局镜像（jne/je 块序）。
// 真修：0x24 分支体内 8 处 get_user(slot) → m_member[slot].m_pUser 内联读
//（ORIG 仅两处循环条件 call get_user，体内 11 处 add $0x78 数组寻址，EXTRA +8）；
// 删 `CBattle_Field& bf` 缓存槽（P183），bf 21 处调用展开 m_battleField 直调。
// b28d-solo-CParty: N 封顶 —— N82 帧 0x1cc/0x1ec；720 vs 763
#include "CParty/onBloodRewardTimer.inc"
#define item (rewardBase + i)
                    int putSlot = -1;                   // -0x28
                    // ORIG onBloodRewardTimer.md 085b6d92/98 `imul $0x3d +
                    // mov 0x2(%eax)`：Inven_Item(sizeof 0x3d)+0x02 → m_addInfo
                    if (item->m_addInfo != -1)
                    {
                        if (item->m_addInfo == 0)   // 金币槽
                        {
                            int gained =
                                reinterpret_cast<CUserCharacInfo*>(
                                    m_member[slot].m_pUser)
                                    ->getCurCharacInvenW()
                                    ->gain_money(item->get_add_info(),
                                                 (eMoneyAddReason)5, true,
                                                 false);
                            if (gained < item->get_add_info())
                                m_member[slot].m_pUser->SendMoneyFullReason(
                                    (ENUM_MONEY_FULL_REASON)0,
                                    (unsigned long)item->get_add_info(),
                                    (unsigned long)gained);
                            putSlot = 0;
                            // ORIG 085b6e88 `movzbl 0x89f(%eax)` =
                            // CDungeon::m_bloodDungeonType（unsigned char 宽度修正）
                            if (m_battleField.m_pDungeon->m_bloodDungeonType == 1)
                                GetInstanceCirculationStatistic()
                                    ->AddCirculationStatistic(
                                        (CIRCULATION_STATISTIC_FIELD)0xb,
                                        m_member[slot].m_pUser,
                                        (unsigned int)gained);
                            // ORIG 085b6ed8 `movzbl 0x89f(%eax)`（同上）
                            else if (m_battleField.m_pDungeon->m_bloodDungeonType == 2)
                                GetInstanceCirculationStatistic()
                                    ->AddCirculationStatistic(
                                        (CIRCULATION_STATISTIC_FIELD)0xc,
                                        m_member[slot].m_pUser,
                                        (unsigned int)gained);
                        }
                        else
                        {
                            putSlot =
                                reinterpret_cast<CUserCharacInfo*>(
                                    m_member[slot].m_pUser)
                                    ->getCurCharacInvenW()
                                    ->insertItemIntoInventory(
                                        *item, (eItemAddReason)0xb, true,
                                        true);
                        }
                    }
                    if (putSlot < 0)
                    {
                        // Inven_Item::m_addInfo（同上 085b6dae/366 站点）
                        int itemKey = item->m_addInfo;
                        if (itemKey == 0x4e3 || itemKey == 0x4e4)
                        {
                            if (G_CDataManager()->find_item(itemKey) != 0)
                            {
                                char title[21];         // -0x65
                                strncpy(title,
                                        g_scriptStringManager_.findString(
                                            4, "game_server_msg_153", 0),
                                        0x14);
                                char bodyText[256];     // -0x165
                                strncpy(bodyText,
                                        g_scriptStringManager_.findString(
                                            4, "game_server_msg_154", 0),
                                        0xff);
                                unsigned int serverGroup =
                                    (unsigned int)m_member[slot].m_pUser->GetServerGroup();
                                unsigned int characNo =
                                    (unsigned int)reinterpret_cast<CUserCharacInfo*>(
                                        m_member[slot].m_pUser)
                                        ->getCurCharacNo();
                                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                                    title, *item, 0, characNo, bodyText,
                                    (int)strlen(bodyText), 8,
                                    (ENUM_SERVER_GROUP)serverGroup, false,
                                    false);
                            }
                        }
                    }
                    else
                    {
                        PacketGuard packet;             // -0x50
                        packet.put_header(0, 0xe);
                        packet.put_byte(item->GetItemSpace());
                        packet.put_short(1);
                        INVEN_TYPE invenType =
                            (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)
                                item->GetItemSpace());
                        reinterpret_cast<CUserCharacInfo*>(
                            m_member[slot].m_pUser)
                            ->getCurCharacInvenR()
                            ->MakeItemPacket(invenType, putSlot, packet);
                        packet.finalize(true);
                        m_member[slot].m_pUser->Send(packet);
                    }
                }

                unsigned int rewardExp =
                    (unsigned int)m_battleField.m_bloodReward.getRewardExp(slot);   // -0x34
                if (1000000 < rewardExp)
                {
                    LogManager::logFormat(
                        1, "party.cpp",
                        __PRETTY_FUNCTION__, 0x3bd4,
                        "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP!! (%u)\n",
                        rewardExp);
                    rewardExp = 1000000;
                }
                int sp = 0;                             // -0x40
                int sfp = 0;                            // -0x44
                if (m_member[slot].m_pUser->gain_exp_sp(
                        (int)rewardExp, sp, sfp, (CUser::eExpAddReason)2, 0,
                        false))
                    m_member[slot].m_pUser->SendNotiPacket((CUser::eSendTarget)2,
                                                   (ENUM_NOTIPACKET)2, 1);
            }
        }
        m_battleField.setBloodState((ENUM_BLOOD_STATE)5);
        SetEPLPState(2);
        int key = gen_timer_key((TIMER_MESSAGE)0x25);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, GetPartyIndex(),
                                    (TIMER_MESSAGE)0x25, 0x3c, key, 0x25);
        sendInoutConditionDungeon(
            (RetryFailReason::T)checkInoutConditionDungeon());
    }
    else if ((int)msg == 0x25)
    {
        if (GetEPLPState() != 2)
            return 0;
        if (GetSelectedEPLPCmd() == 3)
            return 0;
        finishBloodDungeon();
    }
    else if ((int)msg == 0x22)
    {
        if (m_battleField.getBloodState() < 2)
            return 0;
        if (get_member_count() < 1)
            return 0;
        // P48：ORIG 085b7512/085b7583 对 check_grid_clear/check_end_point/isBloodClear
        // 在 not-all-dead 与 all-dead 两支各独立计算一次（不缓存 clearOk 局部）。
        if (!check_allmember_die())
        {
            if (m_battleField.check_grid_clear() && m_battleField.check_end_point() &&
                m_battleField.m_bloodReward.isBloodClear())
                beginBloodClearProcess(true);
        }
        else
        {
            if (m_battleField.check_grid_clear() && m_battleField.check_end_point() &&
                m_battleField.m_bloodReward.isBloodClear())
                beginBloodClearProcess(true);
            else
                beginBloodClearProcess(false);
        }
    }
    return 1;
}
#undef item

// ============================================================================
// CParty::finishBloodDungeon —— ORIG 0x85b8f92：单行转发 ReturnToVillage。
// ============================================================================
#include "CParty/finishBloodDungeon.inc"
}
// ============================================================================
// CParty::set_clear_reward_exp_parameter —— ORIG T 0x085adb00（0x5d4 字节）
//   报告：docs/class_func_reports/CParty/set_clear_reward_exp_parameter.md
//   逐段对照 ## 汇编 + ## 反编译 C 转写。要点：
//   - PC 房间燃烧经验：GetExpFactor()/100 为【整数除法】（imul 0x51eb851f/sar 5）；
//     双倍经验 GetExpFactor()/100.0f 为浮点除（flds _DAT_08cbbbcc=100.0f）。
//   - m_pcRoomSameUserBonus(+0x34) 按原始 dword 位型写入（读取方按 float 解释）。
//   - 回归用户队伍奖励 0x3e4ccccd = 0.2f。
//   - 成长之力：get_growth_power_exp_reward 返回 dword，取 (v>>16)&0xff / 100。
//   - getAddExpRateFromPremium(0x4f)：ORIG 直呼 CUser 方法（W 0x085bfd76 =
//     WongWork::CUserPremium::GetAddExpBonus(type)：type∈[0,0x69] 时返回
//     g_SPremiumInfo[type].m_field44）；该转发链未建模为成员函数，
//     就地取权威表字段（与 sumGainedEachExp 同惯例）。
// ============================================================================
// [SweepCParty] 补真缺口：GetChannelScript 前第三次独立 call G_CDataManager
//（旧码复用 dm ⇒ p167 MISS 1，已消）。p167 58=58 CLEAR（余 C1/C2 别名）。
#include "CParty/set_clear_reward_exp_parameter.inc"
}
// ============================================================================
// CParty::clear_reward —— ORIG T 0x085ae0d4（0x3119 字节）
//   报告：docs/class_func_reports/CParty/clear_reward.md（## 汇编 + ## 反编译 C
//   逐段对照转写）。浮点常量（df_game_r .rodata）：_DAT_08cbbbcc=100.0f、
//   _DAT_08cbbbec=10000.0f、0x08cbbbf0=double 100.0；fistp 就近舍入序列按工程
//   惯例以 (int)/(long long)(x+0.5) 转写。
//   stGenerateRefData_t 调用上下文区按 ORIG 原始偏移写入：
//     +0x34..+0x37 字节：stdLevel/diff/memberCount 打包（全程按字节写）；
//     +0x38 dword（m_rankBonusRate 视图）：rankBonusIdx → mapRatio → getGoldCardCreateRate
//     三次位型覆写（ORIG -0x210 槽 0x036a/0x0529/0x11db/0x1539 实证）；
//     +0x3c dword（m_field3c 视图）：金币半字打包与 premium 组号 0/3、1/4、5 复用；
//     +0x40 CDungeon*（第二遍整字写覆盖 +0x40..43 半字死写）。
//   奖励槽视图：BattleData::m_itemGroups{0,1,2}[slot][2]（this+0x3a0/+0x588/+0x770）。
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x3bc/0x460 + 大量缺口；2595 vs 3090
#include "CParty/clear_reward.inc"
#define bd m_battleData
#define bfView (*reinterpret_cast<CBattle_Field*>(&m_battleField))

    // ORIG:local_ec(G_CDataManager 首调结果)复用于 find_dungeon/getRankBonusIndex/+0x68。
    int rankBonusIdx = dm->getRankBonusIndex(param);
    ServerParameterScript* spScript =
        &dm->m_serverParameter;                              // dm+0x68 子对象

    int stdLevel = 0;
    if (dungeon->get_dimension_possible() > 0)
        stdLevel = getStandardDimensionLevel();
    else
        stdLevel = dungeon->get_standard_level();

    bd.m_field68c += 1;

    int gridCount = 0;
    // ORIG：getMaze() 在两个循环条件与体内各调用一次（不提取缓存）。
    for (int gy = 0; gy < bfView.getMaze()->m_width; ++gy)
    {
        for (int gx = 0; gx < bfView.getMaze()->m_height; ++gx)
        {
            const GridScript& grid = bfView.getMaze()->getGridR(gy, gx);
            if (grid.m_gridValue != 0)
                gridCount += 1;
        }
    }
    float mapRatio =
        static_cast<float>(bd.m_field68c) / static_cast<float>(gridCount);

    int ratePerMapMax =
        static_cast<WongWork::CClearReward*>(
            G_CDataManager()->getItemGenerator()->getGenerator(
                WongWork::CItemGeneratorMgr::eGenerateType_2))
            ->getRewardItemRatePerMapMaxCount(gridCount);
    if (ratePerMapMax != 0)
    {
        mapRatio = static_cast<float>(
                       static_cast<unsigned long long>(ratePerMapMax)) *
                   mapRatio / 10000.0f;
    }

    WongWork::stGenerateRefData_t ref;
    ref.m_field35 = static_cast<char>(bfView.get_dungeon_diff());
    ref.m_field34 = static_cast<char>(stdLevel);
    ref.m_rateSlot = static_cast<char>(memberCount);
    ref.m_rankBonusIndex = rankBonusIdx;
    // ORIG 0x085ae447..0x085ae476：movw this+0x374→+0x3c、this+0x36c→+0x3e、
    // 0→+0x40、this+0x370→+0x42（半字视图，随后第二遍 m_battleField.m_pDungeon 整字写覆盖 +0x40..43）
    ref.m_goldBase = static_cast<unsigned short>(bd.m_field48);      // +0x3c
    ref.m_goldFactor = static_cast<unsigned short>(bd.m_killedMonster1);  // +0x3e
    ref.m_field40 = 0;                                               // +0x40
    ref.m_field42 = static_cast<unsigned short>(bd.m_killedMonster2);    // +0x42

    WongWork::stGenerateResult_t result;
    // ORIG:条件为 != 0(power_war 分支先发射,generateSpecificItem 为 else)。
    if (dungeon->m_89e != 0)
    {
        // ORIG 0x085ae520 区：getGenerator 链在循环【前】求值一次（local_a8 缓存），
        // 循环体仅 checkValidUser→get_charac_level→power_war(...)。
        WongWork::CClearReward* powerGen =
            static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_2));
        for (int i = 0; i < 4; ++i)
        {
            if (_checkValidUser(i))
            {
                powerGen->power_war_dungeon_gold_clear_reward(
                    ref, result, m_member[i].m_pUser->get_charac_level());
            }
        }
    }
    else
    {
        static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_2))->generateSpecificItem(ref, result);
    }
    increase_result_card_gold(result);

    // ORIG 0x085ae55f/0x085ae57a：dropItems 迭代器 begin() 在第二遍 ref 赋值【前】
    // 构造一次（iterator→const_iterator 转换调用），跨 slot 递进消费；end() 每轮重取。
    std::vector<Inven_Item>::const_iterator dropIt = result.m_dropItems.begin();

    ref.m_field35 = static_cast<char>(bfView.get_dungeon_diff());
    ref.m_field34 = static_cast<char>(stdLevel);
    ref.m_rankBonusRate = mapRatio;                    // +0x38 float 视图（ORIG 0x085ae5fd）
    ref.m_dungeon = dungeon;

    int minStdDropRate = 100;
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            ServiceRestrictManager* restrictMgr =
                ARAD::Singleton<ServiceRestrictManager>::Get();
            if (restrictMgr->isRestricted(RestrictCategory::Enum_3, 1))
            {
                minStdDropRate =
                    std::min(m_member[i].m_pUser->getStdDropRate(), minStdDropRate);
            }
        }
    }

    char guildTodayFlag[4] = {0, 0, 0, 0};
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i) && m_member[i].m_pUser->IsTodayGuildMember())
        {
            guildTodayFlag[i] = 1;
            for (int j = 0; j <= 3; ++j)
            {
                if (_checkValidUser(j) &&
                    m_member[i].m_pUser->get_charac_guildkey() ==
                        m_member[j].m_pUser->get_charac_guildkey())
                {
                    guildTodayFlag[j] = 1;
                }
            }
        }
    }

// §P183 形态核销：ORIG 对 m_member[slot].m_pUser 每次访问都从 this 重新物化
// （add $0x78 共 63 处；-0x58(%ebp) 槽 41 次），我方原以 `CUser* user` 缓存 ⇒
// 净缺 ~330 条地址计算指令。改宏为「每次直取」，指令数复现 ORIG，语义等价
// （slot 在两循环内均为循环变量，循环体无对 user 的赋值）。
#define user (m_member[slot].m_pUser)
    for (int slot = 0; slot <= 3; ++slot)
    {
        if (!_checkValidUser(slot))
        {
            continue;
        }

        if (dungeon->m_entryRestrictFlag != 0 &&
            user->IsDeleteEventDungeonInvitation())
        {
            user->EventDungeonClearReward(dungeon->m_vec86c);
        }
        if (G_CDataManager()->getNewRankIndex(param) < 3)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, (APSystem::_ActionGroupIndex)0x10, 0,
                static_cast<ENUM_PACKETCLASS>(0));
        }

#define rewardItems (bd.m_itemGroups0[slot])
#define goldCardItems (bd.m_itemGroups2[slot])
#define pcRoomItems (bd.m_itemGroups1[slot])
        if (dungeon->m_89e == 0)
        {
            int lvl = user->get_charac_level();
            double basePenalty = G_CDataManager()->BaseExpPenalty(lvl, stdLevel);
            if (basePenalty >= 1.0 || dungeon->isRiskDungeon())
            {
                unsigned int guildLvl = static_cast<unsigned int>(
                    static_cast<unsigned short>(
                        static_cast<const STGuildDBInfoOnly*>(
                            user->GetGuildDBInfo())->m_memberCount));  // +0x22
                unsigned int tempExp =
                    static_cast<unsigned int>(user->get_guild_temp_exp());
                float weight = G_CDataManager()->m_guildParameter.getExpWeight(guildLvl);
                int addExp = static_cast<int>(
                    static_cast<double>(weight) *
                        static_cast<double>(tempExp));
                // ORIG:条件为 != 0(today 分支先发射,add_guild_exp 为 else)。
                if (guildTodayFlag[slot] != 0)
                {
                    addExp += static_cast<int>(
                        G_CDataManager()->m_guildParameter.m_fieldFC / 100.0 *
                            static_cast<double>(addExp));
                    user->add_guild_today_exp(addExp);
                }
                else
                {
                    user->add_guild_exp(addExp);
                }
            }
        }
        else
        {
            int lvl = user->get_charac_level();
            ref.m_field34 = static_cast<char>(lvl);
            unsigned int guildLvl = static_cast<unsigned int>(
                static_cast<unsigned short>(
                    static_cast<const STGuildDBInfoOnly*>(
                        user->GetGuildDBInfo())->m_memberCount));  // +0x22
            unsigned int tempExp =
                static_cast<unsigned int>(user->get_guild_temp_exp());
            float weight = G_CDataManager()->m_guildParameter.getExpWeight(guildLvl);
            int addExp = static_cast<int>(
                static_cast<double>(weight) * static_cast<double>(tempExp));
            // ORIG:条件为 != 0(today 分支先发射,add_guild_exp 为 else)。
            if (guildTodayFlag[slot] != 0)
            {
                addExp += static_cast<int>(
                    G_CDataManager()->m_guildParameter.m_fieldFC / 100.0 *
                        static_cast<double>(addExp));
                user->add_guild_today_exp(addExp);
            }
            else
            {
                user->add_guild_exp(addExp);
            }
        }
        user->reset_guild_temp_exp();

        {
            ServiceRestrictManager* restrictMgr =
                ARAD::Singleton<ServiceRestrictManager>::Get();
            if (restrictMgr->isRestricted(RestrictCategory::Enum_3, 1))
            {
                ref.m_dropRate = user->getStdDropRate();
            }
        }

        {
            if (dropIt != result.m_dropItems.end())
            {
                rewardItems[0].setCopy(*dropIt);
                ++dropIt;
            }
        }

        if (m_battleData.m_arr54[slot] != 1 && m_battleData.m_arr58[slot] != 1 &&
            m_battleData.m_arr5c[slot] != 1)
        {
            bd.m_arr64[slot] += 1;
        }

        float deadPenalty = calDeadUserDungeonPenalty(slot);
        if (deadPenalty > 0.0f)
        {
            int addInfo = rewardItems[0].get_add_info();
            rewardItems[0].set_add_info(static_cast<int>(
                (1.0f - deadPenalty) * static_cast<float>(addInfo)));
            m_battleData.m_arr60[slot] = 1;
        }

        {
            int addInfo = rewardItems[0].get_add_info();
            rewardItems[0].set_add_info(static_cast<int>(
                static_cast<double>(static_cast<float>(minStdDropRate) / 100.0f) *
                    static_cast<double>(addInfo)));
        }

        ref.m_field3c =                                          // +0x3c 整字视图
            user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)) ? 3 : 0;
        WongWork::stGenerateResult_t genResult;
        GetLuckPoint(reinterpret_cast<CLuckPoint*>(&genResult.m_luckPoint), false);
        static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_2))->generateRandomItem(ref, genResult);
        if (!genResult.m_dropItems.empty())
        {
            rewardItems[1].setCopy(*genResult.m_dropItems.begin());
        }

        ref.m_field3c =                                          // +0x3c 整字视图
            user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)) ? 4 : 1;
        goldCardItems[0].setCopy(rewardItems[0]);
        goldCardItems[0].set_add_info(0);
        // ORIG：金卡组共用一条 getGenerator 链缓存（local_78）。
        WongWork::CClearReward* clearGen =
            static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(
                WongWork::CItemGeneratorMgr::eGenerateType_2));

        bool goldCardPath = true;
        if (clearGen == 0)
        {
            goldCardPath = false;
        }
        else
        {
            CEventBase* ev10 = GlobalData::s_event_manager->GetRepeatEvent(0x10);
            if (ev10->IsEventing(static_cast<CUser*>(0)))
                goldCardPath = false;
        }
        if (goldCardPath)
        {
            float commissionRate = 1.0f;
            CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
            if (ev41->IsEventing(static_cast<CUser*>(0)) && IsPremiumGoldCardParty())
            {
                commissionRate = *reinterpret_cast<float*>(
                    &G_CDataManager()->m_serverParameter
                         .m_premiumGoldCard4894.m_field0);   // +0x48fc float 位型
            }
            CGameManager* gameMgr = G_CGameManager();
            CConditionEventManager* condMgr = gameMgr->GetConditionEventManager();
            if (condMgr != 0)
            {
                int step = user->GetCurConditionEventStep();
                if (condMgr->IsFreeGoldCard(user, static_cast<char>(step + 1)))
                    commissionRate = 0.0f;
            }
            int commission = clearGen->getGoldCardCommission(stdLevel);
            m_field1e4[1][slot] = static_cast<int>(
                static_cast<float>(commission) * commissionRate);
            float createRate = clearGen->getGoldCardCreateRate();
            // ORIG 0x085af2af：fstps -0x210(=ref+0x38) —— float 槽覆盖，非 +0x34
            ref.m_rankBonusRate = createRate;
        }

        genResult.m_dropItems.clear();
        {
            CEventBase* ev41 = GlobalData::s_event_manager->GetRepeatEvent(0x41);
            if (ev41->IsEventing(static_cast<CUser*>(0)) && IsPremiumGoldCardParty())
            {
                clearGen->generatePremiumGoldCardItem(
                    ref, genResult, static_cast<ENUM_RARITY>(-1),
                    GetPremiumGoldCardDefaultItem());
            }
            else
            {
                clearGen->generateRandomItem(ref, genResult);
                if (!genResult.m_dropItems.empty() &&
                    (IsReturnUserParty() || IsEventCharacParty()))
                {
                    clearGen->generatePremiumGoldCardItem(
                        ref, genResult, static_cast<ENUM_RARITY>(1), 10000);
                }
            }
        }

        // ORIG 0x085af461 区：非空 setCopy 代码物理在前、empty 的 reset+ev13 块
        // 在后（P59 布局方向）——源写非空先判形。
        if (!genResult.m_dropItems.empty())
        {
            goldCardItems[1].setCopy(*genResult.m_dropItems.begin());
            if (goldCardItems[1].m_addInfo == -1 || dungeon->m_89e != 0)
            {
                goldCardItems[1].reset();
            }
        }
        else
        {
            goldCardItems[1].reset();
            CEventBase* ev13 = GlobalData::s_event_manager->GetRepeatEvent(0x13);
            if (ev13->IsEventing(static_cast<CUser*>(0)))
            {
                clearGen->generateGoldCardBlankItem(goldCardItems[1]);
            }
        }

        if (dungeon->m_villageAttackEventFlag != 0)
        {
            Inven_Item eventItem;
            eventItem.m_addInfo =
                G_CDataManager()->m_serverParameter.m_field4a4;  // +0x50c
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), eventItem, 1);
            goldCardItems[0].reset();
            goldCardItems[1].reset();
            rewardItems[0].reset();
            rewardItems[1].reset();
            rewardItems[1].setCopy(eventItem);
            bd.ResetEventDungeonClearPoint();
        }

        // ORIG 0x085af60d：mov -0xc8→-0x210(=ref+0x38) —— mapRatio 位型重写 float 槽
        ref.m_rankBonusRate = mapRatio;
        if (user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0xc)))
        {
            pcRoomItems[0].setCopy(rewardItems[0]);
            pcRoomItems[0].set_add_info(0);
            ref.m_field3c = 5;                                 // +0x3c 整字视图
            genResult.m_dropItems.clear();
            static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(WongWork::CItemGeneratorMgr::eGenerateType_2))->generateRandomItem(ref, genResult);
            // ORIG 0x085af5xx 区：非空 setCopy 物理在前，empty 分支后置（P59）；
            // empty 内 reset 在 getGenerator 链之后。
            if (!genResult.m_dropItems.empty())
            {
                pcRoomItems[1].setCopy(*genResult.m_dropItems.begin());
            }
            else
            {
                WongWork::CClearReward* pcRoomGen =
                    static_cast<WongWork::CClearReward*>(G_CDataManager()->getItemGenerator()->getGenerator(
                        WongWork::CItemGeneratorMgr::eGenerateType_2));
                pcRoomItems[1].reset();
                if (pcRoomGen != 0)
                {
                    CEventBase* ev1d =
                        GlobalData::s_event_manager->GetRepeatEvent(0x1d);
                    if (ev1d->IsEventing(static_cast<CUser*>(0)))
                    {
                        pcRoomGen->generatePcRoomCardBlankItem(pcRoomItems[1]);
                    }
                }
            }
        }
    }

    unsigned int mobReward = 0;
    if (dungeon->m_89e != 1)
    {
        G_CDataManager()->get_mob_reward(stdLevel, mobReward);
    }
    float partyBonusRate = spScript->m_expGroup368[memberCount - 1];
    partyBonusRate +=
        static_cast<CPartyExpBonusEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x18))
            ->getBonusRate(memberCount, isHelpAbuseParty());

    unsigned int totalMobExp = 0;
    if (dungeon->m_89e != 1)
    {
        int killCount = bd.GetTotalKilledMonsterCount();
        float baseExp = static_cast<float>(killCount) *
                        static_cast<float>(mobReward) / 2.0f * partyBonusRate;
        float diffWeight =
            spScript->m_monsterExpTable398[bfView.get_dungeon_diff()];
        double weighted =
            static_cast<double>(baseExp) * static_cast<double>(diffWeight);
        totalMobExp = static_cast<unsigned int>(static_cast<long long>(
            weighted * dungeon->get_exp_weight()));
    }

    PacketGuard pkBasic;
    PacketGuard pkReward;
    std::list<map_item> questItemList;
    Inven_Item questItem;

    for (int slot = 0; slot <= 3; ++slot)
    {
        if (!_checkValidUser(slot))
        {
            continue;
        }
        user->setChattingMessageCount(0);

        if (dungeon->m_89e != 0)
        {
            G_CDataManager()->get_mob_reward(user->get_charac_level(), mobReward);
            int killCount = bd.GetTotalKilledMonsterCount();
            float baseExp = static_cast<float>(killCount) *
                            static_cast<float>(mobReward) / 2.0f * partyBonusRate;
            float diffWeight =
                spScript->m_monsterExpTable398[bfView.get_dungeon_diff()];
            double weighted =
                static_cast<double>(baseExp) * static_cast<double>(diffWeight);
            totalMobExp = static_cast<unsigned int>(static_cast<long long>(
                weighted * dungeon->get_exp_weight()));
        }

        int userLevel = user->get_charac_level();
        unsigned int personalExp = 0;
        if (dungeon->m_89e == 1)
        {
            // ORIG：get_index()==100 与否两分支体相同（退化分支，忠实保留）。
            if (dungeon->get_index() == 100)
            {
                personalExp = totalMobExp / memberCount;
            }
            else
            {
                personalExp = totalMobExp / memberCount;
            }
        }
        // ORIG:条件为 isRisk(MPR-only 分支先发射)。
        else if (dungeon->isRiskDungeon())
        {
            double scaled = static_cast<double>(
                static_cast<float>(static_cast<double>(
                    static_cast<unsigned long long>(totalMobExp)) *
                    static_cast<float>(MemberPenaltyRate())));
            personalExp = static_cast<unsigned int>(static_cast<long long>(
                scaled / memberCount));
        }
        else
        {
            double penalty = G_CDataManager()->BaseExpPenalty(userLevel, stdLevel);
            double scaled = static_cast<double>(
                static_cast<float>(static_cast<double>(
                    static_cast<unsigned long long>(totalMobExp)) * penalty) *
                static_cast<float>(MemberPenaltyRate()));
            personalExp = static_cast<unsigned int>(static_cast<long long>(
                scaled / memberCount));
        }

        QuickParty::RewardData_Param rewardParam(this);
        QuickParty::RewardData_Result rewardResult;
        rewardResult.m_value00 = static_cast<int>(personalExp);
        set_random_reward(static_cast<QuickParty::RandomBuffType>(0), rewardParam,
                          rewardResult);
        personalExp = static_cast<unsigned int>(rewardResult.m_value00);

        stClearRewardExpParameter_t expParam;
        expParam.m_deadMemberCount = slot;
        expParam.m_rankBonusIdx = rankBonusIdx;
        set_clear_reward_exp_parameter(user, personalExp, expParam);
        stClearRewardBonusExp_t bonus;
        unsigned int bonusTotal =
            getClearRewardBonusExp(user, personalExp, expParam, bonus);

        bool helpAbuse = isHelpAbuseParty(user);
        user->calcHelpAbuseRatio(bonusTotal, helpAbuse);
        user->reset_level_before_dungeon();

        int spPoints = 0;
        int sfpPoints = 0;
        bool leveledUp = user->gain_exp_sp(
            static_cast<int>(bonusTotal), spPoints, sfpPoints,
            static_cast<CUser::eExpAddReason>(2), 0, false);
        if (leveledUp)
        {
            pkBasic.clear();
            pkBasic.put_header(0, 2);
            pkBasic.put_byte(1);
            pkBasic.put_short(1);
            user->make_basic_info(reinterpret_cast<char*>(&pkBasic), 1);
            pkBasic.finalize(true);
            user->Send(pkBasic);
        }

        if (user->IsHavePremiumAdvantage())
        {
            float premiumDeduction;   // ORIG quirk: flds -0x2cc has no writer
            int deduction = static_cast<int>(premiumDeduction);
            user->m_character.m_f28 -= deduction;
            user->m_character.m_f34 -= deduction;
        }

        questItemList.clear();

        pkReward.clear();
        pkReward.put_header(0, 0x23);
        pkReward.put_int(bonus.m_channelBonusExp + static_cast<int>(personalExp));
        pkReward.put_int(bonus.m_rankWeightExp);
        user->set_charac_party_bonus_exp(static_cast<int>(bonus.m_rankWeightExp));

        unsigned int extraExp = 0;
        {
            // ORIG 0x085b0249 区：分母 = 表值重读(fVar1) + 新算 bonusRate(lVar20)。
            // partyBonusRate 局部已含首轮 bonusRate，不能再用它作基数再加。
            float tableRate = spScript->m_expGroup368[memberCount - 1];
            float bonusRate =
                static_cast<CPartyExpBonusEvent*>(
                    GlobalData::s_event_manager->GetRepeatEvent(0x18))
                    ->getBonusRate(memberCount, isHelpAbuseParty());
            if (personalExp != 0)
            {
                int sub = static_cast<int>(
                    static_cast<float>(personalExp) / (bonusRate + tableRate));
                unsigned int sum = bonus.m_avatarBonusExp +
                                   bonus.m_rankWeightExp +
                                   bonus.m_selectedBonusExp +
                                   bonus.m_channelBonusExp +
                                   bonus.m_premiumAdvExp +
                                   bonus.m_serverEventExp +
                                   bonus.m_serverChannelExp +
                                   bonus.m_creatureBonusExp;
                extraExp = (personalExp - static_cast<unsigned int>(sub)) *
                           (sum / personalExp + 1);
            }
        }
        pkReward.put_int(static_cast<int>(extraExp));
        pkReward.put_int(bonus.m_avatarBonusExp);
        pkReward.put_byte(bonus.m_bestType);
        pkReward.put_int(bonus.m_selectedBonusExp + bonus.m_serverEventExp +
                         bonus.m_burningFatigueExp);
        pkReward.put_int(bonus.m_periodItemExp);
        pkReward.put_int(bonus.m_premiumAdvExp);
        pkReward.put_int(bonus.m_pcRoomSameUserExp);
        pkReward.put_int(bonus.m_serverChannelExp);
        pkReward.put_int(bonus.m_upperMemberExp);
        pkReward.put_int(bonus.m_creatureBonusExp);
        pkReward.put_int(bonus.m_returnUserExp);
        pkReward.put_int(bonus.m_eventCharacExp);
        pkReward.put_int(bonus.m_manageExp);
        pkReward.put_int(bonus.m_growthPowerExp);
        pkReward.put_int(bonus.m_premiumAddExp);
        int questCountPos = pkReward.get_index();
        pkReward.put_byte(0);

        int questPutCount = 0;
        for (int qi = 0; qi < 0x14; ++qi)
        {
            UserQuest* questRef = user->getCurCharacQuestRefR();
            if (questRef->m_quest[qi] == 0 || questRef->m_trigger[qi] == 0)
            {
                continue;
            }
            Quest* quest = G_CDataManager()->find_quest(questRef->m_quest[qi]);
            if (quest == 0)
            {
                continue;
            }
            if (!quest->check_clear_item(bfView.get_dungeon_index(),
                                         static_cast<int>(
                                             questRef->m_authenData.m_characClass)))
            {
                continue;
            }
            int requiredCount = questRef->m_quest[qi];
            if (requiredCount <= 0)
            {
                continue;
            }
            // ORIG：默认构造 vector + pair，再由 get_clear_itemlist 返回值赋值。
            std::vector<std::pair<int, int> > clearItems;
            std::pair<int, int> entry;
            clearItems = quest->get_clear_itemlist(bfView.get_dungeon_index());
            // [轴 B，地址自校 0x085b0907] ORIG `addl $1,-0x30; call <size>;
            // cmp -0x30,%eax; setg %al` = **有符号**比较（我方旧文 unsigned
            // 产 `seta`，符号性相反）⇒ 以 ORIG 为准改 int + (int)size()。
            for (int ci = 0; ci < (int)clearItems.size(); ++ci)
            {
                questItem.reset();
                entry = clearItems.at(ci);
                G_CDataManager()->m_itemList->create_item(
                    static_cast<ENUM_ITEM_CREATE_TYPE>(1), questItem,
                    entry.second);
                CInventory* inventory = user->getCurCharacInvenW();
                int putSlotNo = inventory->insertItemIntoInventory(
                    questItem, static_cast<eItemAddReason>(0xb), true, true);
                if (putSlotNo >= 0)
                {
                    pkReward.put_short(putSlotNo);
                    pkReward.put_int(entry.first);
                    pkReward.put_int(questItem.get_add_info());
                    pkReward.put_short(static_cast<int>(questItem.m_fieldb));
                    pkReward.put_byte(static_cast<int>(
                        static_cast<unsigned char>(
                            questItem.m_amp.getAbilityType())));
                    pkReward.put_short(
                        static_cast<int>(questItem.m_amp.getAbilityValue()));
                    questPutCount += 1;
                    if (questItem.m_field1 == 1)
                    {
                        CItem* itemDef = G_CDataManager()->find_item(entry.first);
                        if (itemDef != 0)
                        {
                            CLuckPoint::UseLuckPoint(
                                user,
                                static_cast<int>(questItem.m_field1),
                                itemDef->get_rarity(),
                                itemDef->get_grade());
                        }
                    }
                }
            }
        }
        if (questPutCount != 0)
        {
            pkReward.put_byte(questCountPos, questPutCount);
        }

        for (int j = 0; j <= 3; ++j)
        {
            if (_checkValidUser(j))
            {
                _makeRewardItemInfo(m_member[j].m_pUser, bd.m_itemGroups0[j],
                                    pkReward);
            }
            else
            {
                pkReward.put_byte(0);
            }
        }
        pkReward.put_int(m_field1e4[1][slot]);

        for (int j = 0; j <= 3; ++j)
        {
            // ORIG：两分支体相同（各 put_byte(0)，退化分支，忠实保留）。
            if (_checkValidUser(j))
            {
                pkReward.put_byte(0);
            }
            else
            {
                pkReward.put_byte(0);
            }
        }

        for (int j = 0; j <= 3; ++j)
        {
            if (_checkValidUser(j))
            {
                if (!m_member[j].m_pUser->isAffectedPremium(
                        static_cast<ENUM_PREMIUM_TYPE>(0xc)))
                {
                    pkReward.put_byte(0);
                }
                else
                {
                    _makeRewardItemInfo(m_member[j].m_pUser,
                                        bd.m_itemGroups1[j], pkReward);
                }
            }
            else
            {
                pkReward.put_byte(0);
            }
        }

        // ORIG clear_reward.md +0x2aa2..+0x2b00：flds 100.0 × hellValue ÷
        // fildl(int m_hellPartyRatio0) 后 fldcw RC=11 纯截断（无 +0.5f fadd）。
        int hellCoinValue = static_cast<int>(
            100.0f * bd.m_hellPartyValueTotal /
                static_cast<float>(bfView.m_hellPartyRatio0));
        pkReward.put_int(hellCoinValue);

        bool returnFlag = false;
        if (user->GetParty() != 0)
        {
            returnFlag =
                reinterpret_cast<CParty*>(user->GetParty())->IsReturnUserParty();
        }
        // ORIG:if/else 两分支各一次 put_byte 调用(静态双站点)。
        if (returnFlag)
        {
            pkReward.put_byte(1);
        }
        else
        {
            pkReward.put_byte(0);
        }

        bool eventFlag = false;
        if (user->GetParty() != 0)
        {
            eventFlag =
                reinterpret_cast<CParty*>(user->GetParty())->IsEventCharacParty();
        }
        if (eventFlag)
        {
            pkReward.put_byte(1);
        }
        else
        {
            pkReward.put_byte(0);
        }

        if (expParam.m_deadPenaltyRate != 0.0f)
        {
            pkReward.put_byte(1);
        }
        else
        {
            pkReward.put_byte(0);
        }

        pkReward.finalize(true);
        user->Send(pkReward);

        if (m_battleField.m_pDungeon != 0 && user != 0 && m_battleField.m_bloodState != 1 &&
            user->getCurCharacR() != 0)
        {
            bool properDungeon = dungeon->m_bloodDungeonType == 0 &&
                                 dungeon->get_dimension_possible() == 0 &&
                                 dungeon->m_89e != 1 &&
                                 !dungeon->isTowerOfDespairDungeon();
            if (properDungeon && user->IsProperDungeonUser())
            {
                HeroMissionEvent* heroMission =
                    ARAD::Singleton<HeroMissionEvent>::Get();
                heroMission->processMission(
                    user, static_cast<HeroMissionCondition::MissionNo::T>(1), 1);
                user->IncProperDungeonClearCount();
                CGameManager* gameMgr2 = G_CGameManager();
                CConditionEventManager* condMgr2 =
                    gameMgr2->GetConditionEventManager();
                if (condMgr2 != 0)
                {
                    condMgr2->ProcessCheckStepUp(user, 2);
                }
            }
        }
        if (m_battleField.m_pDungeon != 0 && user != 0 && m_battleField.m_bloodState != 1 &&
            user->getCurCharacR() != 0)
        {
            bool properLevel = dungeon->m_bloodDungeonType == 0 &&
                               dungeon->m_89e != 1 &&
                               !dungeon->isTowerOfDespairDungeon();
            if (properLevel && user->IsProperLevelDungeonUser())
            {
                user->ProcProperLevelDungeonClear();
            }
        }
    }
#undef user
#undef rewardItems
#undef goldCardItems
#undef pcRoomItems
#undef bd
#undef bfView

    if (IsPremiumGoldCardParty())
    {
        PacketGuard goldCardPacket;
        goldCardPacket.put_header(0, 0x12a);
        goldCardPacket.finalize(true);
        send_to_party(goldCardPacket);
    }
    if (dungeon->m_entryRestrictFlag == 0)
    {
        DungeonPermission(param);
    }
}

#include "CParty/GetUsedCoinCount.inc"
}

#include "CParty/SetUsedCoinCount.inc"
}

#include "CParty/IncreaseUsedCoinCount.inc"
}
#include "CParty/GetAssaultState.inc"
}

#include "CParty/SetAssaultState.inc"
}


#include "CParty/Party.inc"
}

// b28d-Main：`Party::~Party()` 已改为 Party.h 内 inline 定义（ORIG 该符号在头内可见，
// 否则 map<int,Party>::construct 会多发 16 条 EH landing pad）；此处不得再定义（重定义）。
#include "CParty/Reset_1.inc"
}
