// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CParty/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CParty/cMember.inc"


#include "CParty/Init.inc"


#include "CParty/CParty.inc"


#include "CParty/dtor_CParty.inc"


#include "CParty/send_to_party.inc"


#include "CParty/SetIDX.inc"


#include "CParty/GetPartyIndex.inc"


#include "CParty/set_state.inc"


#include "CParty/get_state.inc"


#include "CParty/IsSinglePlay.inc"
{
    return m_singlePlay;
}

#include "CParty/SetSinglePlay.inc"
{
    m_singlePlay = flag ? 1 : 0;
}

#include "CParty/GetEPLPState.inc"
{
    return m_eplpState;
}

#include "CParty/GetSelectedEPLPCmd.inc"
{
    return m_selectedEPLPCmd;
}

#include "CParty/SetSelectedEPLPCmd.inc"
{
    m_selectedEPLPCmd = cmd;
}


#include "CParty/GetMember.inc"
{
    return m_member;
}

#include "CParty/get_user.inc"
{
    return m_member[idx].m_pUser;
}

#include "CParty/getTitleIndex.inc"
{
    return m_titleIndex;
}

#include "CParty/setTitleIndex.inc"
{
    struct V { char m_pad[0x11b]; unsigned char m_f; };
    ((V*)this)->m_f = idx;
}

#include "CParty/getTitle.inc"


#include "CParty/setTitle.inc"


#include "CParty/IsAutoCreated.inc"


#include "CParty/SetAutoCreated.inc"


#include "CParty/getUserMax.inc"


#include "CParty/setUserMax.inc"


#include "CParty/getDungIndex.inc"


#include "CParty/setDungIndex.inc"


#include "CParty/getDungDiffi.inc"


#include "CParty/setDungDiffi.inc"


#include "CParty/get_dungeon_clear_state.inc"


#include "CParty/set_dungeon_clear_state.inc"


#include "CParty/reset_recv_flag.inc"


#include "CParty/SetRecvResultFlag.inc"


#include "CParty/getStraightVictories.inc"


#include "CParty/setStraightVictories.inc"


#include "CParty/incStraightVictories.inc"


#include "CParty/resetStraightVictories.inc"


#include "CParty/getStartGamePartyCount.inc"


#include "CParty/setStartGamePartyCount.inc"


#include "CParty/IsFirstMapClear.inc"


#include "CParty/SetFirstMapClear.inc"


#include "CParty/IsPremiumGoldCardParty.inc"


#include "CParty/SetPremiumGoldCardParty.inc"


#include "CParty/ResetPremiumGoldCardParty.inc"


#include "CParty/GetPremiumGoldCardDefaultItem.inc"


#include "CParty/SetPremiumGoldCardDefaultItem.inc"


#include "CParty/ResetPremiumGoldCardDefaultItem.inc"


#include "CParty/GetSecretShopData.inc"


#include "CParty/checkBossRoom.inc"


#include "CParty/checkStartRoom.inc"


#include "CParty/DeleteDungeonDropItem.inc"


#include "CParty/get_quick_party_index.inc"


#include "CParty/set_quick_party_index.inc"


#include "CParty/is_quick_party.inc"


#include "CParty/set_quick_party.inc"


#include "CParty/set_gm_random_buff_type.inc"


#include "CParty/get_random_buff_type.inc"


#include "CParty/IsWeekendEvent.inc"


#include "CParty/SetWeekendEvent.inc"


#include "CParty/getDungeonMapSaving.inc"


#include "CParty/setDungeonMapSaving.inc"


#include "CParty/checkFreeRevivalCondition.inc"


#include "CParty/GetPartyTelePort.inc"


#include "CParty/getPassedMapInfoList.inc"



#include "CParty/Get_party_overlapped_drop_ratio_rare.inc"


#include "CParty/Get_party_overlapped_drop_ratio_unique.inc"


#include "CParty/ClearMapHitCount.inc"


#include "CParty/IsRoutingState.inc"


#include "CParty/SetStartRouting.inc"


#include "CParty/SetEndRouting.inc"


#include "CParty/SetRoutingItem.inc"


#include "CParty/GetRoutingItemID.inc"


#include "CParty/GetRoutingInvenItem.inc"


#include "CParty/GetRoutingItemUniqueID.inc"


#include "CParty/GetMemberRoutingState.inc"


#include "CParty/SetMemberRoutingState.inc"


#include "CParty/SetRoutingTimerOver.inc"


#include "CParty/init_quick_party_data.inc"


#include "CParty/Reset.inc"


#include "CParty/CItemRoutingData.inc"


#include "CParty/dtor_CItemRoutingData.inc"


#include "CParty/GetRoutingState.inc"


#include "CParty/SetRoutingState.inc"


#include "CParty/GetRoutingInvenItem_1.inc"


#include "CParty/GetRoutingItemIndex.inc"


#include "CParty/SetRoutingItemIndex.inc"


#include "CParty/GetRoutingItemUniqueID_1.inc"


#include "CParty/GetMemberRoutingState_1.inc"


#include "CParty/SetMemberRoutingState_1.inc"


#include "CParty/getManager.inc"


#include "CParty/init.inc"


#include "CParty/destroy.inc"


#include "CParty/SetEPLPState.inc"


// ============================================================================
// 批次 A 补全（ORIG 0x85a932a / 0x85a21ca / 0x85b8f92 等；语义见 CParty.md）
// ============================================================================

#include "CParty/check_passive_skill.inc"


#include "CParty/isHelpAbuseParty.inc"


#include "CParty/isHelpAbuseParty_1.inc"


#include "CParty/MemberPenaltyRate.inc"


#include "CParty/GetPartyMemberCoinLimit.inc"


#include "CParty/SetPartyMemberCoinLimit.inc"


#include "CParty/getStandardDimensionLevel.inc"


#include "CParty/isTournamentVictory.inc"


#include "CParty/GetTournamentDungeonClearState.inc"


#include "CParty/SetTournamentDungeonClearState.inc"


#include "CParty/set_host.inc"


#include "CParty/STGainedEachExpData.inc"


#include "CParty/getMonsterTotalExp.inc"


#include "CParty/sumGainedEachExp.inc"


#include "CParty/change_manager.inc"


#include "CParty/change_manager_1.inc"


#include "CParty/IsExistInvisible.inc"


#include "CParty/battle_data_reset.inc"


#include "CParty/send_host_info.inc"


#include "CParty/get_party_realtime_info.inc"


#include "CParty/send_party_realtime_info.inc"


#include "CParty/send_quick_party_matching_result.inc"


// ORIG 反汇编为 for(i=0; i<=3; ++i) 结构（初始化后直接 jmp 到尾部条件测试）。

#include "CParty/CheckEnterEventDungeon.inc"


#include "CParty/checkInoutConditionDungeon.inc"


// ============================================================================
// 链接阻断方法（ORIG T/W）：6 个 CParty 方法 + depended helper（主 agent 授权一并实现）
// ============================================================================

#include "CParty/getHellPartyEntranceItemNeedCount.inc"


#include "CParty/CheckMemberFatigue.inc"


#include "CParty/sendInoutConditionDungeon.inc"


#include "CParty/checkDugeonInoutTime.inc"


#include "CParty/get_blood_dungeon_admission_fee.inc"


#include "CParty/check_dungeon_start.inc"


#include "CParty/CheckEnterDimensionDungeon.inc"


#include "CParty/CheckEnterVillageAttackRevenge.inc"


#include "CParty/CheckHellDungeonFreepassItemHaveAndDel.inc"


#include "CParty/CheckDestroyConditionSealDoor.inc"


#include "CParty/check_timer_key.inc"


#include "CParty/gen_timer_key.inc"


#include "CParty/resetFinishLoadingVar.inc"


#include "CParty/CanDoPartyActionAssalutState.inc"


#include "CParty/resetMemberUseCoin.inc"


#include "CParty/checkBloodRenewMyRecord.inc"


#include "CParty/IsSelectCard.inc"


#include "CParty/_IsCannotConnectP2P.inc"


#include "CParty/_GetConnectP2PPoint.inc"


#include "CParty/check_allmember_die.inc"


#include "CParty/get_live_count.inc"


#include "CParty/get_live_count_enter_map.inc"


#include "CParty/GetValidPartyMember.inc"


#include "CParty/IsExistUser.inc"


#include "CParty/_getMemberLevelGap.inc"


#include "CParty/getMemberLevelGap.inc"


#include "CParty/increase_dungeon_coin_count.inc"


#include "CParty/IsReturnUserParty.inc"

#include "CParty/send_invite_bluemarble.inc"

#include "CParty/get_member_count.inc"

#include "CParty/getMemberNames.inc"


#include "CParty/GetUserPosition.inc"


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

#include "CParty/_checkValidUser.inc"

#include "CParty/checkValidUser.inc"
#include "CParty/GetMemberSlotNo.inc"

#include "CParty/IncreaseSetPlayResultCount.inc"

#include "CParty/Reset_party_overlapped_drop_ratio.inc"

#include "CParty/ReturnToVillage.inc"

#include "CParty/leave_user.inc"

#include "CParty/IsEventCharacParty.inc"

#include "CParty/CheckEnterAncientDungeon.inc"

#include "CParty/UseAncientDungeonItems.inc"


#include "CParty/set_charac_live.inc"


#include "CParty/stClearRewardExpParameter_t.inc"


#include "CParty/stClearRewardBonusExp_t.inc"


#include "CParty/CalcAvatarBonus.inc"


#include "CParty/CalcCreatureBonus.inc"


#include "CParty/getAvatarBonus.inc"


#include "CParty/getCreatureBonus.inc"


#include "CParty/getClearRewardBonusExp.inc"


// ============================================================================
// Prod_CParty 批次（2026-08-24）：CParty producer 函数（9 个 ORIG 符号）
// 逐函数对照 docs/class_func_reports/CParty/{GetPartyType,SendWalkout,
// SetPartyInfoUI,create_party,dungeon_start,game_start,set_single_play,
// walkout_member,get_item,drop_item}.md 的 ## 汇编 + ## 反编译 C 全文转写；
// 常量 _DAT_08cbbbcc=100.0f / _DAT_08cbbbe0=2.5 取自 df_game_r .rodata。
// ============================================================================

#include "CParty/GetPartyType.inc"


#include "CParty/SendWalkout.inc"


#include "CParty/SetPartyInfoUI.inc"


#include "CParty/create_party.inc"


#include "CParty/set_single_play.inc"



#include "CParty/walkout_member.inc"


#include "CParty/game_start.inc"

#undef bd

#include "CParty/dungeon_start.inc"

#undef bf
#undef bd

#include "CParty/get_item.inc"


#include "CParty/drop_item.inc"


#include "CParty/insert_time_dungeon_clear_msg.inc"


#include "CParty/insert_time_card_select_msg.inc"


#include "CParty/insert_time_clear_dungeon_reward_msg.inc"


#include "CParty/insert_time_dungeon_score_scroll_msg.inc"


#include "CParty/insert_time_card_select_right_msg.inc"


#include "CParty/set_play_result.inc"


#include "CParty/forced_set_play_result.inc"


#include "CParty/map_load_forced_finish.inc"


#include "CParty/HaveAlreadySelectedCard.inc"


#include "CParty/_checkGoldCardSelectable.inc"


#include "CParty/SendEachCardInfo.inc"


#include "CParty/SetCardNumber.inc"

#undef reward

#include "CParty/CompleteCardSelect.inc"


#include "CParty/SendCardSelectComplete.inc"


#include "CParty/IsCompleteCardSelectOfMembers.inc"


#include "CParty/TournamentDungeonClear.inc"


#include "CParty/MakeTournamentReward.inc"


#include "CParty/IsCollectDungeonStatistic.inc"
extern Inven_Item g_emptySlot;      // ORIG 全局 0x943ddc0（AccountCargo.cpp 定义）

#include "CParty/OnKilledMonster.inc"

#undef bf

#include "CParty/OnFightVillageMonster.inc"


#include "CParty/OnFinishVillageMonsterFighting.inc"

#undef bf

#include "CParty/prepare_dungeon_clear.inc"


#include "CParty/party_dungeon_fail.inc"

#undef bf
#undef bd

#include "CParty/dungeon_clear.inc"

#undef bd
#undef user
#undef g0
#undef g1
#undef g2

#include "CParty/_putItemIntoUser.inc"


#include "CParty/onBloodClear.inc"


#include "CParty/BloodClearRewardPacket.inc"


#include "CParty/generateBloodRewardExp.inc"


#include "CParty/beginBloodClearProcess.inc"

#undef bf

#include "CParty/generateBloodRewardItem.inc"

#undef bf

#include "CParty/onBloodRewardTimer.inc"

#undef item

#include "CParty/finishBloodDungeon.inc"

#include "CParty/set_clear_reward_exp_parameter.inc"

#include "CParty/clear_reward.inc"


#include "CParty/GetUsedCoinCount.inc"


#include "CParty/SetUsedCoinCount.inc"


#include "CParty/IncreaseUsedCoinCount.inc"

#include "CParty/GetAssaultState.inc"


#include "CParty/SetAssaultState.inc"



#include "CParty/Party.inc"


#include "CParty/Reset_1.inc"

