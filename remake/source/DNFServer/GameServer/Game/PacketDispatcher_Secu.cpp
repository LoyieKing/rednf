// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 Dispatchers.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN30Dispatcher_SecurityCardAuthRpy4readER9PacketBufR8MSG_BASE
//   ORIG TU     ：PacketDispatcher_Secu.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include "NOTIPacketStruct.h"  // _NotiAdvanceAltarUpdateRidable（1126 行发送用）
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::make_internal_stream_jpn / ENUM_INTERNALPACKET_JPN
#include "MoreTypes.h"            // arad::SigAradJumpingCharacter（AradJumpingCharacter dispatch_sig 完整类型）
#include "TaiwanInternal.h"        // Taiwan::internal_stream（P2PHolePunching process）
#include "PvPTypes.h"              // CMTRand（Dispatcher_UseVendingMachine ctor）
#include "CPartyTelePort.h"        // CPartyTelePort::send_teleport_status/process_teleport/reset_teleport_data
#include "Teleport.h"            // Teleport/STCheckRuleData（Dispatcher_SoloTeleport check_error/send）
#include "CUserProc.h"             // APSystem::CUserProc（ClearAction/GetTodayReward dispatch_sig）
#include "CCharacInformNotice.h"   // CCharacInformNotice::handleInformNoticeFlag
#include "Packet_Monitor_Other_Channel_Chat_Hyper_Link.h"  // ItemHyperLinkMessage process +0x26 跨服转发
#include "Packet_Monitor_Other_Channel_Chat.h"           // CreatureSendMessage/CreatureScriptMessage 私聊监控包（shared 权威 packed 0x13b）
#include "Packet_Approve_Join_Guild.h"  // 8924 行使用（局部栈对象）
#include "MSG_COMPOUND_AVATAR.h"        // Dispatcher_Compound_Avatar::dispatch_sig 局部栈对象
#include "InterSelectPcroomDailyReward.h"   // RequestPrcoomDayilyReward 发放后回包/落库
#include "DBUpdatePcroomDailyReward.h"      // RequestPrcoomDayilyReward 发放后落库
#include "advancealtar_ProcStage.h"  // ProcStage/CharacData 完整类型（procStagePause/characData 系列具名化）
#include "CMyFileLog.h"
#include "CompoundEmblemDeps.h"
#include "CEventScriptMng.h"   // EventClassify::CEventScriptMng / CEventEntity /
#include "CEventScriptMng.h"   // 新增：EventClassify::CEventScriptMng/CObjectBringUp
#include "CTournamentDungeonReward.h"   // CTournamentDungeonReward /
#include "CEventCreateDnf.h"   // CEventCreateDnf（Dispatcher_EventCreateDnfRequest 奖励核算 / IsEventing）
#include "DBInsertCreateDnfUserInfo.h"   // DBInsertCreateDnfUserInfo::makeRequest（创建角色奖励落库）
#include <utility>             // std::pair<int,int>
#include <vector>
#include "CDataManager.h"            // G_CDataManager / find_item
#include "../../../shared/packet/include/Packet_P2P_Statistics.h"  // shared 权威（ORIG ctor @0x0822c696）
#include "CDataManager.h"       // G_CDataManager / find_skill
#include "CGameManager.h"            // MSG_CHECK_MAIL_RECEIVER（set<type,T> 模板）
#include "CGuildServerProxy.h"       // CGuildServerProxy::SendGuildMail
#include "BreakAwayPreventSystem.h"  // s_BreakAwaySys 完整类型（giveBreakAwayDungeonClearReward）
#include "CInventory.h"              // CSecu_ProtectionField / GetInvenTypeFromItemSpace /
#include "CInventory.h"         // CInventory::GetInvenData/setCopy/delete_item/SetInventoryMemory
#include "CMailBoxHelper.h"          // WongWork::CMailBoxHelper::ReqDBCheckReceiver
#include "CServerProxyMgr.h"         // CServerProxyMgr<CGuildServerProxy>::GetServerProxy
#include "CSkill.h"             // CSkill::IsSpecialSkill
#include "CStreamGuard.h"            // CStreamGuard ctor/operator*/->/<</GetInBuffer
#include "CSyncSlangFilter.h"        // CSyncSlangFilter::GetInstance / HasSlang
#include "CUserCharacInfo.h"    // getCurCharac*/get_charac_*
#include "CEquipItem.h"         // CEquipItem::m_itemType（DisJointAvatar +0x234）
#include "RandomOptionScript.h" // RandomOptionScript::m_providerModified154（ChangeRandomOption +0x154）
#include "CompoundEmblemDeps.h"      // LineFunc(int, char const*, int, unsigned)
#include "CompoundEmblemDeps.h" // LineFunc(int, char const*, int, unsigned)
#include "GameEnums.h"          // ENUM_SKILL_TREE_KIND / eItemDelReason / ENUM_CMDPACKET
#include "GameTypes.h"          // cMyTrace
#include "cMyTraceNoop.h"        // cMyTraceNoop（ReportBadP2PUser 日志）
#include "cHistoryTrace.h"      // UseCreatureEvolutionItem process ItemEvolute 日志
#include "GameWorld.h"          // G_GameWorld / IsPvPSkilTreeChannel
#include "GlobalData.h"              // s_guild_proxy_mgr / s_stream_pool / s_msgq_mgr
#include "InterfacePacketBuf.h" // clear/put_header/put_byte/put_short/put_int/finalize
#include "Arad_DataManager.h"             // ARAD::Singleton<ServiceRestrictManager>::Get（UpgradeInventory）
#include "DB_UpdateInventoryCapacity.h"   // UpgradeInventory DB 请求
#include "Inven_Item.h"         // Inven_Item ctor/reset
#include "CBattle_Field.h"               // DieMobAll: IsClearDungeon（战场视图）
#include "CExpandEquipslot.h"            // Equipslot_Switch（CUser.h 仅前向）
#include "CTimeGate.h"                   // CollectItems check_error isOpen（GlobalData.h 仅前向）
#include "MsgQueueMgr.h"             // MsgQueueMgr::put(QUEUE_IDX, CStreamGuard&)
#include "PacketGuard.h"        // PacketGuard
#include "ServiceRestrictManager.h"  // ServiceRestrictManager::isRestricted
#include "SigTypes.h"                // SIG_FIND_FACTORY_HUB
#include "SkillSlot.h"          // refund_skill/get_skill_level/get_remain_*_at_index
#include "StreamPool.h"              // StreamPool::Acquire
#include "Dispatchers.h"         // Dispatcher_UseRightOfChangeGrowType / read 上移前置
#include "W8LT_DBTask.h"  // StubS3A 依赖
#include "CSpecialItemRoutingManager.h"  // StubS3A 依赖
#include "CExtraction.h"             // expert_extraction::CExtraction（process 虚调用）
#include "CExtractionMgr.h"          // expert_extraction::CExtractionMgr::get_extracter
#include "PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA.h"  // AuctionMyRegistedItemInfo PCK 包（StubS3A）
#include "PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP.h"  // AuctionMyRegistedItemInfo PCK 包（StubS3A）
#include <cstddef>
#include <ctime>
#include <string.h>
#include <string> // LineFunc(int, char const*, int, unsigned)
#include "CStatisticServerProxy.h" // statistc_proxy::add（UnsealRandomOption 统计）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString
#include "Cipher.h"              // Cipher::Decrypt（MouseRegister 密码管线）
#include "DB_AccountCargoSync.h" // DB_PassPadUpdateFailCnt / DB_GoblinPadUpdateReward
#include "CAssaultMgr.h"                    // DspD4 B1/B6/B7: pvp_assault::GetInstanceAssaultMgr/OnChangeHp
#include "PCK_AUCTION_MY_AUCTION_HISTORY_GA.h"  // B6: 拍卖历史包
#include "AuctionProxy.h"                   // B6: GlobalData::s_auction_proxy IsRunning/SendPacket
#include "CPrivateStoreMgr.h"               // B3/B6: private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "W8LT_EventTail.h"                 // B5: GiveGrowCreatureEvent（CheckEventInfo/processEvent）
#include "Packet_Overflow_Statistic_Add.h"  // B5: Overflow 统计包（shared packed 0xd）
#include "DB_JoinGuildInfo.h"               // B4: DB_JoinGuildInfo/DB_CancelJoinGuild makeRequest
#include <new>                              // B7: Client_Spec_Statistic send placement new
#include "CQuestShop.h"                     // B5: Clear_Used_QP → CQuestShop::clearUsedQp
#include "CCommunityServerProxy.h"          // B6: SendPvPBuddyList 完整类型
#include "CAchievement.h"                   // B5: AchievementTrigger setTrigger
#include "CExpertJob.h"                     // B2: expert_job::CExpertJob/CEnchanter/ENUM_CLOSE_MISSION_TYPE
#include "Packet_Client_Spec_Statistic.h"   // B7: Client_Spec_Statistic send 包
#include "CFairPvPScore.h"       // fair_pvp::CFairPvPScore（SelectItem 领取链）
#include "CDataManager.h"
#include "BlueMarble.h"
#include "CGuildServerProxy.h"
#include "Quest.h"         // Quest/stSelectQuestParam 完整定义
#include "CodeHackChecksum.h"   // CodeHackChecksum（Dispatcher_CodeCheckSum）
#include <cstring>
#include <iterator>
#include <list>
#include "CDailyScheduleManager.h"
#include "SigTypes.h"      // SIG_FORBID_USER_TO_PLAY 权威定义
#include "CGM_Manager.h"
#include "CGameManager.h"
#include "CInventory.h"
#include "CMonitorServerProxy.h"
#include "CParty.h"
#include "PvP_Room.h"
#include "CPrivateStoreMgr.h"
#include "CServerProxyMgr.h"
#include "CStreamGuard.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "TimerQueue.h"
#include "STComputeDamageVariable.h"   // D1 依赖：SDC 伤害变量权威布局（computeHpDamageSDC）
#include "CraneMinigameManager.h"        // CraneMinigameManager 完整类型（CranePickUp）
#include "StrikerScript.h"               // StrikerScript 完整类型（SkillTreeSelect）
#include "CCeraShop.h"                   // WongWork::CCeraShop 完整类型（BuyAutomatItem）
#include "ItemVendingMachine.h"          // ItemVendingMachine 完整类型（BuyAutomatItem）
#include "CConditionEventManager.h"      // CConditionEventManager 完整类型（RequestConditionEventReward）
#include "CCoinEventPerDay.h"           // CCoinEventPerDay 完整类型（GetCoinNoPerDay）
#include "COnTimeEvent.h"                // COnTimeEvent 完整类型（RequestOnTimeEventReward）
#include "BingoEvent.h"                 // BingoEvent 完整类型（reqBingoQuizMark/sendMaxMatchLineReward）
#include "Secu_DataControl.h"           // Secu_DataControl::SetControlData（GlobalData.h 前向）
#include "SECRET_SHOP_DATA.h"            // secretshop::SECRET_SHOP_DATA::IsOpen/IsCompleteBuy
#include "SECRET_SHOP_DATA.h"            // secretshop::SECRET_SHOP_DATA::IsOpen/IsCompleteBuy
#include "RestrictNameDeps.h"            // checkValidCharacName 0x0820582b / isUtf8StrInUnicodeRange 0x08ac2d6c
#include "CPrePayInfoMgr.h"              // G_CPrePayInfoMgr() ORIG W 0x08231f20
#include "Arad_ItemBonus.h"              // ARAD::CheckAccountItem ORIG T 0x08196ac8
#include "DB_UpdateAvatarHistory.h"      // DB_UpdateAvatarHistory::makeRequest ORIG T 0x0844536a
#include "ChecksumAlgo.h"                // CSHA1（Dispatcher_SecurityCardIssue）
#include "ROI_Category.h"                // ROI_Category::_sort ORIG W 0x0822ac30
#include "DataManagerInitDepends.h"      // SDCActiveStatusSyncServer（SDC_ActiveStatus_Damage_Check）
#include "BossTowerFunc.h"               // Global::CGlobalFunc::Get_State_Check ORIG T 0x081466f0
#include "DisPatcherCore.h"              // DisPatcher_DebugCommand（New_Gmdebug_Command）
#include "GameMasterCmd.h"               // game_master::CCommand/CParser/CGameMasterMgr
#include "Packet_Stat_Lag_Statistics.h"          // ORIG W ctor 0x0822c144
#include "Packet_Loading_Time_Report_Statistics.h" // ORIG W ctor 0x0822c040
#include "Packet_Guild_Guild_Cargo_Upgrade.h"     // ORIG W ctor 0x0822bfea
#include "../../../shared/packet/include/Packet_Guild_Guild_Cargo_Pop_Item.h"   // shared 权威（ORIG W ctor 0x0822bf38）
#include "../../../shared/packet/include/Packet_Guild_Guild_Cargo_Move_Item.h"  // shared 权威（ORIG W ctor 0x0822bf62）
#include "PCK_AUCTION_ASK_OWNER_IS_VIP_GP.h"     // ORIG W ctor 0x0822ebd8
#include "PCK_AUCTION_BIDDING_GA.h"              // 拍卖竞价请求包
#include "PCK_AUCTION_REGIST_CANCEL_GP.h"        // ORIG 拍卖登记取消 GP 包
#include "PCK_AUCTION_REGIST_CANCEL_GA.h"        // ORIG 拍卖登记取消 GA 包
#include "PCK_AUCTION_ASK_AVERAGE_PRICE_GA.h"    // ORIG 拍卖均价请求 GA 包
#include "PCK_AUCTION_ASK_AVERAGE_PRICE_GP.h"    // ORIG 拍卖均价请求 GP 包
#include "PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP.h"  // ORIG W ctor 0x0822eafa
#include "PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA.h"  // 拍卖无道具键搜索 GA 包
#include "PCK_AUCTION_BUY_ITEM_APIECE_GA.h"      // ORIG 拍卖按件购买 GA 包
#include "PCK_AUCTION_MY_BIDDING_INFO_GA.h"      // ORIG 拍卖我的竞价信息 GA 包
#include "Packet_Guild_Cargo_Requst.h"           // ORIG W ctor 0x0822be52（D3）
#include "Packet_Monitor_Member_Secede.h"        // ORIG W ctor 0x0822ba34（D3）
#include "Packet_Monitor_Call_Member_List.h"     // D3 公会成员列表包
#include "Packet_PvPChannelInfo.h"               // D3 PvP 频道信息包
#include "Packet_Guild_Check_Guild_Cargo_Push_Item.h"  // GuildCargoPushItem process 发送（Fix7_I）
#include "Packet_Monitor_Member_Enter_Reply.h"   // ORIG 0x13 字节（D3）
#include "Packet_Request_BlackList.h"            // D3 黑名单请求包
#include "Packet_Delete_To_BlackList.h"          // ORIG 栈对象 0x2c（D3）
#include "PCK_AUCTION_SEARCH_BY_ITEMKEY_GA.h"
#include "PCK_AUCTION_SEARCH_BY_ITEMKEY_GP.h"
#include "Packet_Guild_Cargo_History_Request.h"  // ORIG W ctor 0x0822be8a（D3）
#include "Packet_Stat_Compatibility_Index.h"     // ORIG W ctor 0x0822c61e（D3）
#include "Packet_Monitor_Request_Member_Enter.h" // ORIG W ctor 0x0822b9ce（D3）
#include "Packet_Guild_Attendance_Info.h"        // ORIG W ctor 0x0822c384（D3）
#include "Packet_Monitor_Request_Charac_Info.h"  // D3 角色信息请求包
#include "Packet_Monitor_Set_Sub_Guild_Master.h" // D3 副会长设置包
#include "Packet_Assert_Manager_Info.h"          // D3 管理器断言包
#include "MSG_DIE_ASSAULT_PLAYER.h"              // DspD4 域：死亡突袭玩家消息完整类型
#include "CSecretShop.h"                         // secretshop::CSecretShop::BuyItem ORIG 0x085fb16e
#include "CInGameAdvertisementManager.h"          // CInGameAdvertisementManager/InGameADManagerInstance 0x080fc9e9
#include "HeroMissionEvent.h"                    // HeroMissionEvent::sendMissionData 0x0816770e / rewardMission
#include "HeroMissionCondition.h"                // HeroMissionCondition::MissionNo::T
#include "GrowthEquipEvent.h"                    // GrowthEquipEvent::SendInfinityGrowthWeapon ORIG 0x080f74b2
#include "DB_EventUpdateGrowthWeaponTimepiece.h" // ORIG T 0x084497ea makeRequest(u,s,i)
#include <algorithm>                          // case9 std::min（ORIG 080df90e 库调用）
#include "../../../shared/packet/include/Packet_Monitor_Notice_Message.h"   // ORIG 真类（W ctor 0x0822bbd6，case3 GMS）
#include "../../../shared/packet/include/Packet_Forbid_Chat_By_Monitor.h"   // ORIG 真类（W ctor 0x0822bc70，case5 禁言）

#pragma pack(push, 1)
struct ScrMsgView
{
    char m_head[0xd];              // MSG_BASE（0xd 字节）
    unsigned char m_secuType;      // +0x0d（0=安全卡、1=密保卡 pad）
    unsigned char m_cryptoType;    // +0x0e（check_error：非 0、<=0xd、!=5）
    char m_data[0x1000];           // +0x0f 密文区（get_binary 读 m_dataLen 字节）
    int m_dataLen;                 // +0x100f（ORIG 紧凑布局：0xf+0x1000=0x100f；
                                   //   缺 pack(1) 时 int 对齐把 m_dataLen 推至 0x1010）
    unsigned char m_answerType;    // +0x1013
};

#pragma pack(pop)
#include "PacketDispatcher_Secu/read_321.inc"
