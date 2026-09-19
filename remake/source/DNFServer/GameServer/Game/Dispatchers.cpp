// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatchers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
// ORIG 域常量（nm：_ZN15InformNotice2ndL6kIndexE / L4kBitE，r 段内部链接；
// 2nd 通知标志板容量 = kIndex*kBit = 0x80）
namespace InformNotice2nd
{
static const int kIndex = 16;
static const int kBit = 8;
}

#include "advancealtar_ProcStage.h"  // ProcStage/CharacData 完整类型（procStagePause/characData 系列具名化）

#include "CMyFileLog.h"
#include "CompoundEmblemDeps.h"
#include "Dispatchers/isSetBit_Uint.inc"
#include "Dispatchers/setBit_Uint.inc"
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
// [BigGM2-GI] 显式提前本 TU 权威头（原本经 315 行 DisPatcherCore.h 间接引入；
// include guard 保证内容只展开一次，符号集零变化——闸2 comm -3 实测核验）。
#include "Dispatchers.h"         // Dispatcher_UseRightOfChangeGrowType / read 上移前置

#include "Dispatchers/check_url.inc"
// ORIG _ZL6gmList 文件级静态 @data 0x093701d8（5×int，objdump -s 实测初值）。
// PacketDispatcher_Impl_1.cpp 静态；AuctionMyRegistedItemInfo 等 GM 迟延 trace 用。
static int gmList[5] = { 0x004c2dc8, 0x005cf723, 0x00a9c742, 0x00a2ad85, 0x009ae1ae };

#include "Dispatchers/read.inc"


#include "Dispatchers/check_url_1.inc"

#include "W8LT_DBTask.h"  // StubS3A 依赖
#include "CSpecialItemRoutingManager.h"  // StubS3A 依赖
#include "CExtraction.h"             // expert_extraction::CExtraction（process 虚调用）
#include "CExtractionMgr.h"          // expert_extraction::CExtractionMgr::get_extracter
// StubS3A：DB 请求类本地声明（与 W8LT_DBTask.cpp 内建模一致；定义在 W8LT_DBTask.cpp/StubS3A_deps.cpp）
class DB_ReqQueryCharacInfo : public DBDispatcher
{
public:
    DB_ReqQueryCharacInfo() {}
    static void makeRequest(int uid, const char* name);
};
// ORIG T 0x0842bfaa _ZN28DB_SecurityCardUpdateFailCnt11makeRequestEjj
// ORIG T 0x08436388 _ZN25DB_SecurityCardAddCertCnt11makeRequestEj
// 真实定义：W8LT_DBTask.cpp（静态 makeRequest 全文转写）；本 TU 局部类声明
// 与定义一致，调用点直调成员（符号同名，链接到真实实现；§8 全禁）。
class DB_SecurityCardUpdateFailCnt
{
public:
    static void makeRequest(unsigned int accId, unsigned int failCnt);
};
class DB_SecurityCardAddCertCnt
{
public:
    static void makeRequest(unsigned int accId);
};
// ORIG T 0x0842b790 _ZN22DB_SecurityCardDisuse11makeRequestEj
// 真实定义：W8LT_DBTask.cpp（静态 makeRequest 全文转写）+ Dispatchers.cpp 尾（dispatch）；
// 本 TU 局部类声明与定义一致，调用点直调成员（符号同名，链接到真实实现）。
class DB_SecurityCardDisuse
{
public:
    static void makeRequest(unsigned int accId);
};
class DB_ReqQueryCharacInfoMailbox : public DBDispatcher
{
public:
    DB_ReqQueryCharacInfoMailbox() {}
    static void makeRequest(int uid, const char* name);
};
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

#include "Dispatchers/decreaseRankOfFairPVPEquipmentPackageKey.inc"
{
    // ORIG：整槽 -0x4 = key，槽高 16 位 -0x2 减 1 写回后返回整槽（低 16 位保留）。
    union
    {
        int k;
        unsigned short w[2];
    } u;
    u.k = key;
    u.w[1] = static_cast<unsigned short>(u.w[1] - 1);   // ORIG 无下溢保护（wrap）
    return u.k;
}
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

#include "Dispatchers/makeRequest.inc"


#include "Dispatchers/makeRequest_1.inc"

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

// SIG_FORBID_USER_TO_PLAY 权威定义见 SigTypes.h（FixStubS3 已删除其重复 POD，冲突消解）。

#include "Dispatchers/_NS_IPScan.inc"
#include "Dispatchers/_NS_MI_RecvMemoryIntegrityModulation.inc"
#include "Dispatchers/_NS_NonClient_GetResponse.inc"
#include "Dispatchers/_NS_PI_2ND_RecvPacketIntegrityModulation_C2C.inc"
#include "Dispatchers/_NS_PI_2ND_RecvPacketIntegrityModulation_S2C.inc"
#include "Dispatchers/_NS_ProcessScan.inc"
#include "Dispatchers/_NS_PacketIntegrity.inc"

#include "STComputeDamageVariable.h"   // D1 依赖：SDC 伤害变量权威布局（computeHpDamageSDC）
#include "Dispatchers/computeHpDamageSDC.inc"
class DB_SecurityCardRetransfer
{
public:
    static void makeRequest(int type, char* characName, char* targetName,
                            unsigned int secCardKey, char const* src1,
                            char const* src2, char const* src3);
};
// DspD1Fix：TU 顶部统一前向/extern 声明区（ORIG mangled/地址逐条注释）
// ============================================================================

// ---- 补充权威头 include ----
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
// [b23S3C] Dispatcher_AuctionSearchByItemKey 迁真类（ORIG ctor 0x0822e6aa/0x0822ea84）：
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

// ---- ORIG 真实全局/自由符号声明（无权威头；nm -S 核实）----

// ORIG T 0x088920f1 _Z19decryptBVHackPacketPcR14stBVHackInfo_t：
// 反汇编（0x88920f1..0x88921c2）：XOR 0x6e3f4b9c 解密 0x30 字节（12×int），
// 随后按 6×double（+0x00/+0x08/+0x10/+0x18/+0x20/+0x28）拷贝到出参。
struct stBVHackInfo_t
{
    double m_values[6];   // ORIG 出参偏移 +0x00..+0x28（fldl/fstpl 序列）
};
#include "Dispatchers/decryptBVHackPacket.inc"

#include "Dispatchers/IsAuctionServerDisconnected.inc"

// ORIG T 0x081f793c _Z11MakePVPRoomP5CUserR17MSG_MAKE_PVP_ROOM14ENUM_CMDPACKET15PVP_BATTLE_MODE
struct MSG_MAKE_PVP_ROOM;   // 完整布局仅经偏移视图访问（QuickJoinRoom msgBuf 0x3d）
#include "Dispatchers/MakePVPRoom.inc"

#include "Dispatchers/EnterPVPRoom.inc"

// ORIG R 0x08af45e4 BLEEDING_DAMAGE_IN_ACTION_RATE（.rodata 4 字节 float，与
// BLEEDING_DAMAGE_TERM 0x08af45e0 相邻）——SDC_ActiveStatus_Damage_Check 阈值。
extern "C" float BLEEDING_DAMAGE_IN_ACTION_RATE;

// zlib uncompress（ORIG 经 uncompress@plt 调用；工具链无 zlib.h，手写声明）
extern "C" int uncompress(unsigned char* dest, unsigned long* destLen,
                          const unsigned char* src, unsigned long srcLen);

// ORIG T 0x08979726 _ZN20GuildParameterScript23getGuildCargoUpradeInfoEi：
// GuildParameterScript::getGuildCargoUpradeInfo / LevelupSupportEventManger
// GetStepIndex/InsertItem：真实定义由 UserMiscLink.cpp 提供（CDataManager.h:1946、
// LevelupSupportEventManger.h:154-155 声明），调用点已直调，asm 桥删除（§8 全禁）。

// ORIG T 0x086ad054 _ZNK9UserQuest27CheckDungeonEventStoryPauseEiii
// ORIG T 0x086ad48a _ZNK9UserQuest21isDoingJobChangeQuestEv
//（真实定义 UserQuest.cpp:954/989，CUser.h 已补声明）
//
// ORIG T 0x085f0eb0 _ZN13random_option15GetRandomOptionEv
//（真实定义 GlobalData.cpp:4059；返回 G_CDataManager()+0x148 RandomOptionScript）
namespace random_option
{
void* GetRandomOption();
}

// ORIG T 0x0842b57a _ZN20DB_SecurityCardIssue11makeRequestEijPKcS1_S1_S1_S1_
// ORIG T 0x0842bb5c _ZN23DB_SecurityCardActivate11makeRequestEijPKcb
// ORIG T 0x0843df0e _ZN16DB_DenyJoinGuild11makeRequestEijj
// ORIG T 0x0843cc06 _ZN17DB_CheckJoinGuild11makeRequestEiP20SIG_CHECK_JOIN_GUILD
// ORIG T 0x0843dd7c _ZN16DB_GuildJoinList11makeRequestEij17ENUM_SERVER_GROUP
// ORIG T 0x0843d19a _ZN19DB_RequestJoinGuild11makeRequestEiP22SIG_REQUEST_JOIN_GUILD
// 真实定义：DB_SecurityCardIssue/Activate、DB_DenyJoinGuild、DB_CheckJoinGuild、
// DB_GuildJoinList 在 DBSecuLink.cpp（局部类 + makeRequest 全文转写）；
// DB_RequestJoinGuild 在 Dispatchers.cpp 尾部（26492 区）。本 TU 局部类声明
// 与 DBSecuLink.cpp 一致，调用点直调成员（符号同名，链接到真实实现；§8 全禁）。
class DB_SecurityCardIssue
{
public:
    static void makeRequest(int uid, unsigned int accId,
                            char const* certKey, char const* serverKey,
                            char const* phone, char const* sha1Hex,
                            char const* cardData);   // _ZN...EijPKcS1_S1_S1_S1_
};
class DB_SecurityCardActivate
{
public:
    static void makeRequest(int uid, unsigned int accId,
                            char const* certKey, bool activated);  // _ZN...EijPKcb
};
class DB_DenyJoinGuild
{
public:
    static void makeRequest(int uid, unsigned int guildKey,
                            unsigned int applicant);   // _ZN...Eijj
};
class DB_CheckJoinGuild
{
public:
    static void makeRequest(int uid, SIG_CHECK_JOIN_GUILD* sig);  // _ZN...EiP20SIG...
};
class DB_GuildJoinList
{
public:
    static void makeRequest(int uid, unsigned int guildKey,
                            ENUM_SERVER_GROUP group);   // _ZN...Eij17ENUM...
};
class DB_RequestJoinGuild
{
public:
    static void makeRequest(int userIdx, SIG_REQUEST_JOIN_GUILD* sig);  // _ZN...EiP22SIG...
};

// ORIG T 0x084460c0 _ZN23DB_Check3rdPartyConcent11makeRequestEij
//（真实定义 W8LT_DBTask.cpp:2831，CStreamGuard → DB_Q；本 TU 局部类声明）
class DB_Check3rdPartyConcent
{
public:
    static void makeRequest(int uid, unsigned int accId);
};

// ORIG T 0x081b5b70 _ZNK23NeedMaterialDataManager15getNeedMaterialEPK5CItem
//（真实定义 NeedMaterialDataManager.cpp:22；返回 m_map 值指针，头声明 CItem*）

// ORIG T 0x080ebce0 _ZN20CraneMinigameManager13isPickSuccessEsi（CranePickUp；
// 真实定义 EventBattleLink.cpp，CraneMinigameManager.h 已声明）

// ORIG T 0x08334850
// _ZN22CConditionEventManager27RequestConditionEventRewardEP5CUserscRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
//（真实定义 EventBattleLink.cpp，CConditionEventManager.h 已声明）

// ORIG T 0x080ca262 _ZN10BingoEvent16reqBingoQuizMarkER5CUseri
// ORIG T 0x080ca622 _ZN10BingoEvent22sendMaxMatchLineRewardER5CUser
//（真实定义 EventBattleLink.cpp，BingoEvent.h 已声明）

// ORIG T 0x0860c86c _ZN17StatisticsNatType15IncreaseNatTypeEiih
//（真实定义 StatisticsNatType.cpp:121，GameWorld.h 已补声明）
//
// ORIG T 0x08286a30 _ZN16Secu_DataControl14SetControlDataEP5CUserhtt
//（真实定义 DBSecuLink.cpp:226，Secu_DataControl.h 已声明）

// LimitNpcBuyItemInfo：ORIG 强符号 C1 0x08110b70（objdump 实证）——
// PacketHeader(0x27d9,0x26) 后 7×movl $0 清 +0xa..+0x25；+0x26..+0x33 不重置。
// （旧注释误记为 W 0x0822c4d0 + memset(+0x16,0x1e)，已按反汇编更正。）
struct LimitNpcBuyItemInfo
{
    LimitNpcBuyItemInfo()
        : m_header(0x27d9, 0x26), m_itemIdReq(0), m_dbid(0), m_charNo(0),
          m_itemId(0), m_count(0), m_1e(0), m_22(0)
    {}   // ORIG 0x08110b70 族：+0x1e/+0x22 真实字段初始化 → movl $0,0x1e(%eax) 单级
    PacketHeader m_header;                // +0x00..+0x09 包头
    unsigned int m_itemIdReq;             // +0x0a（GetUID）
    unsigned int m_dbid;                  // +0x0e（acc_id）
    unsigned int m_charNo;                // +0x12（charac_no）
    unsigned int m_itemId;                // +0x16（msg+0xd）
    unsigned int m_count;                 // +0x1a
    unsigned int m_1e;                    // +0x1e（ORIG movl $0,0x1e(%eax) 单级）
    unsigned int m_22;                    // +0x22（ORIG movl $0,0x22(%eax) 单级）
    unsigned char m_tail26[0xe];          // +0x26..+0x33
} __attribute__((packed));   // ORIG C1 0x08110b70：+0xa 起 7×movl $0 清零（紧凑布局）

// class BingoEvent 前向（reqBingoQuizMark/sendMaxMatchLineReward 经 asm 桥）
class BingoEvent;

// Pakcet_FindUserName_use_Charac_no：ORIG 仅导出 W ctor 0x0822c2e2（符号名
// “Pakcet”为 ORIG 原始拼写，勿更正）；sizeof 0x34，字段经裸偏移访问
//（+0x0a GetUID / +0x0e characNo / +0x16 角色名区 0x1e，见使用点注释）。
struct Pakcet_FindUserName_use_Charac_no : public PacketHeader
{
    // ORIG C1 W 0x0822c2e2：PacketHeader(0x1f45,0x34)（pad 区不重置）、
    // uint(+0xa)=uint(+0xe)=uint(+0x12)=0、memset(+0x16,0,0x1e)（+0x16..+0x33）。
    Pakcet_FindUserName_use_Charac_no()
        : PacketHeader(0x1f45, 0x34)
    {
        m_uid = 0;
        m_characNo = 0;
        m_field12 = 0;
        memset(m_name, 0, 0x1e);
    }
    unsigned int m_uid;                    // +0x0a（GetUID）
    unsigned int m_characNo;               // +0x0e
    unsigned int m_field12;                // +0x12
    char m_name[0x1e];                     // +0x16..+0x33（角色名区，ctor memset 0）
} __attribute__((packed));   // packed：无 pack 时 uint 成员 4 对齐致字段 +2（m_uid@+0xc），与 ORIG +0xa 不符

// MSG_EPLP_COMMAND：ORIG 无独立导出符号，仅经 Dispatcher_EPLPCommand::
// _BroadCastPacket（0x081fee14）以引用访问；报告反编译只读 +0x0d/+0x0e
// 两个命令字节（【推断】前 0xd 字节为包头区，不设 static_assert）。
struct MSG_EPLP_COMMAND
{
    char m_pad00[0xd];        // 包头区（+0x00..+0x0c，ORIG 偏移视图）
    unsigned char m_cmd;      // +0x0d：命令字节（_BroadCastPacket 首个 put_byte）
    unsigned char m_subCmd;   // +0x0e：子命令字节（_BroadCastPacket 第二个 put_byte）
};


// ============================================================================
// advancealtar/ARAD dispatcher 还原辅助（Dispatchers.cpp 内局部，不改头文件）
// ============================================================================
// 注意：shared/packet/include/CMDPacketStruct.h 定义的全局 MSG_BASE/ParamBase
// 与本工程 PacketDispatcher.h 的定义冲突，不能 include；因此本 TU 按 ORIG
// 线格式自行声明 advancealtar 请求/响应 POD（13 字节请求基体 + 派生负载；
// 响应 = 4 字节 ParamBase 视图 + 负载，整体 put_binary）。

namespace altar_pkt
{
// ---- ORIG 线格式（CMDPacketStruct.h 同源，详见该头文件注释）----
// 请求基体 13 字节（ORIG MSG_BASE），负载从 +0x0d 开始。
struct ReqBase
{
    unsigned char m_data[13];
};

// Dispatcher_Pause：req=14B，res=12B（0x242）
struct ReqPause
{
    unsigned char m_data[0xe];
};
struct ResPause
{
    int m_nErrorCode;        // +0x04
    unsigned char m_bPause;  // +0x08（回显 req+0x0d）
    unsigned char m_pad[3];
};

// Dispatcher_BuyItem：req=32B，res=28B（0x23d）
struct ReqBuyItem
{
    unsigned char m_data[0x20];
};
struct ResBuyItem
{
    int m_nErrorCode;        // +0x04
    int m_nBuyIndex;         // +0x08 = req+0x10
    int m_nShopType;         // +0x0c = req+0x14
    int m_nLevel;            // +0x10 = req+0x18
    unsigned short m_wFrom;  // +0x14 = req+0x1c
    unsigned short m_wTo;    // +0x16 = req+0x1e
    int m_nTotalCost;        // +0x18
};

// Dispatcher_SetSlot：req=36B，res=28B（0x23e）
struct ReqSetSlot
{
    unsigned char m_data[0x24];
};
struct ResSetSlot
{
    int m_nErrorCode;        // +0x04
    int m_field08;           // +0x08 = req+0x10
    unsigned char m_bSet;    // +0x0c = req+0x14
    unsigned char m_pad[3];
    unsigned char m_slot[12];  // +0x10 = req+0x18（_SetSlotData）
};

// Dispatcher_ResetStar：req=20B，res=12B（0x244）
struct ReqResetStar
{
    unsigned char m_data[0x14];
};
struct ResResetStar
{
    int m_nErrorCode;  // +0x04
    unsigned char m_pad[8];
};

// Dispatcher_SummonUnit：req=24B，res=20B（0x240）
struct ReqSummonUnit
{
    unsigned char m_data[0x18];
};
struct ResSummonUnit
{
    int m_nErrorCode;        // +0x04
    unsigned char m_bySlotType;  // +0x08 = req+0x0d
    unsigned char m_pad[3];
    int m_nUnitId;           // +0x0c = req+0x10
    int m_nGage;             // +0x10
};

// Dispatcher_UpgradeGage：req=24B，res=20B（0x23f）
struct ReqUpgradeGage
{
    unsigned char m_data[0x18];
};
struct ResUpgradeGage
{
    int m_nErrorCode;        // +0x04
    int m_nGageLevelBefore;  // +0x08
    int m_nGageLevelAfter;   // +0x0c
    int m_nGage;             // +0x10
};

// Dispatcher_ExchangeSlot：req=48B，res=36B（0x241）
struct ReqExchangeSlot
{
    unsigned char m_data[0x30];
};
struct ResExchangeSlot
{
    int m_nErrorCode;        // +0x04
    int m_field08;           // +0x08 = req+0x10
    unsigned char m_slot1[12];  // +0x0c = req+0x18
    unsigned char m_slot2[12];  // +0x18 = req+0x24
};

// Dispatcher_GetAchievementReward：req=20B，res=12B（0x243）
struct ReqGetAchievementReward
{
    unsigned char m_data[0x14];
};
struct ResGetAchievementReward
{
    void* m_vtbl;          // +0x00（STBaseResponse 虚表槽，reinterpret_cast 对齐）
    int m_nErrorCode;      // +0x04
    int m_nAchievementIdx; // +0x08（sendAchievementReward 索引，恒 0）
};
}  // namespace altar_pkt

// [P4 负结论·2026-09-11 实测回滚] 此处三件（getUserStageControl / procStagePause /
//   characDataSummonSlotUnit）在 ORIG 中**无同名符号**（nm -S --defined-only df_game_r
//   0 命中），我方 .o 内也**零调用者**（objdump -dr 引用计数 0）—— 表面是死代码。
//   但**净删除有害**：实测删除后 TU 四数 identical 1009→999、ae 2548→2541（合计
//   −17），missing 仅 812→807（−5）⇒ 合计分 −17 净损。原因：三件中
//   characDataSummonSlotUnit 是 19 个 `std::map<unsigned char, advancealtar::_SlotItem>`
//   模板实例（find/end/_Rb_tree*/_Select1st/_Rb_tree_iterator 等）在本 .o 的**唯一实例
//   化者**；df_game_r 是链接后整库，这 19 个实例在 ORIG 中由别 TU 提供并与我方逐条相同
//   ⇒ 它们原本贡献 identical/AE。删除源码 = 我方 .o 不再发射这些 ORIG 有的符号，
//   identical/AE 直接蒸发。**保留死代码反而更接近 ORIG 的符号形态**。
//   结论：三件**保留不改**（编译产物符号集是判据的一部分，不是"多余符号"）。
namespace
{
// ORIG CUser::getAdvanceAltar()：返回用户当前 advancealtar StageControl。
// [推断] 当前工程未建模 CUser 内 StageControl 指针成员；按等价语义用
// CUser::getAdvanceAltarIndex() + CGameManager::getAdvanceAltar(int) 映射。
advancealtar::StageControl* getUserStageControl(CUser* user)
{
    if (user == 0)
    {
        return 0;
    }
    short idx = user->getAdvanceAltarIndex();
    if (idx < 0)
    {
        return 0;
    }
    return G_CGameManager()->getAdvanceAltar(idx);
}

#include "Dispatchers/procStagePause.inc"


#include "Dispatchers/characDataSummonSlotUnit.inc"
{
    if (cd == 0)
    {
        return 3;
    }
    std::map<unsigned char, advancealtar::_SlotItem>::iterator it =
        cd->m_slotItems.find(slotType);
    if (it == cd->m_slotItems.end())
    {
        return 4;
    }
    const advancealtar::_SlotItem& slotItem = it->second;
    int base = *reinterpret_cast<int*>(G_CDataManager()->m_altarConfig.m_strPtr);
    int cost = base * slotItem.m_field8;
    int gage = cd->m_gage;
    if (slotItem.m_needGage == unitId && cost <= gage)
    {
        cd->m_gage = gage - cost;
        return 0;
    }
    return 2;
}

}  // namespace

namespace advancealtar {

// Wave5 三分解方法共用的请求/响应字节视图
// （CMDPacketStruct::STBaseRequest / STBaseResponse 均为不透明载体，
//  ORIG process_packet 按偏移读写，见各 advancealtar__Dispatcher_*/报告）。
inline char* reqBytes(CMDPacketStruct::STBaseRequest& r) { return reinterpret_cast<char*>(&r); }
inline const char* reqBytes(const CMDPacketStruct::STBaseRequest& r) { return reinterpret_cast<const char*>(&r); }
inline char* resBytes(CMDPacketStruct::STBaseResponse& r) { return reinterpret_cast<char*>(&r); }

#include "Dispatchers/dtor_Dispatcher_Pause.inc"
#include "Dispatchers/dispatch_sig.inc"

#include "Dispatchers/process.inc"

#include "Dispatchers/read_1.inc"

#include "Dispatchers/send_packet.inc"


#include "Dispatchers/check_error.inc"

#include "Dispatchers/read_packet.inc"

#include "Dispatchers/process_packet.inc"


#include "Dispatchers/dtor_Dispatcher_BuyItem.inc"
#include "Dispatchers/dispatch_sig_1.inc"

#include "Dispatchers/process_1.inc"

#include "Dispatchers/read_2.inc"

#include "Dispatchers/send_packet_1.inc"


#include "Dispatchers/check_error_1.inc"

#include "Dispatchers/read_packet_1.inc"

#include "Dispatchers/process_packet_1.inc"


#include "Dispatchers/dtor_Dispatcher_SetSlot.inc"
#include "Dispatchers/dispatch_sig_2.inc"

#include "Dispatchers/process_2.inc"

#include "Dispatchers/read_3.inc"

#include "Dispatchers/send_packet_2.inc"


#include "Dispatchers/check_error_2.inc"

#include "Dispatchers/read_packet_2.inc"

// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0814006e orig/ours=67/69 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,%edx | mov    -0xc(%ebp),%eax | mov    %edx,0x4(%eax)
// [g6 2026-09-11] 本 TU 具名视图（同 ExchangeSlot 手法，探针 /tmp/k1sbx/g6/probe/p1.cpp）：
//   ORIG 0x081400eb `mov 0x18(%eax),%eax`（_SetSlotData.m_type 单 disp 读）与
//   0x08140115 `lea 0x18(%eax),%ecx`（取址单 lea）；协议头 uchar[12] 数组退化
//   +cast 会产 `add $0x18,%eax; mov (%eax),%eax`（+1/处）。
struct AltarSetSlotReqView
{
    char m_pad0d[0xd];                          // +0x00（STBaseRequest 线格式头）
    int m_field10;                              // +0x10
    bool m_bSet;                                // +0x14（bool 成员 load→arg 双 movzbl）
    char m_pad15[3];                            // +0x15
    union { unsigned char m_raw[0xc];
            CMDPacketStruct::_SetSlotData m_data; } m_slot;   // +0x18
};
#include "Dispatchers/process_packet_2.inc"


#include "Dispatchers/dtor_Dispatcher_ResetStar.inc"
#include "Dispatchers/dispatch_sig_3.inc"

#include "Dispatchers/process_3.inc"

#include "Dispatchers/read_4.inc"

#include "Dispatchers/send_packet_3.inc"


#include "Dispatchers/check_error_3.inc"

#include "Dispatchers/read_packet_3.inc"

#include "Dispatchers/process_packet_3.inc"


#include "Dispatchers/dtor_Dispatcher_StartGame.inc"
#include "Dispatchers/check_error_4.inc"

#include "Dispatchers/process_4.inc"
{
    // ORIG 0x813f8ce（报告 advancealtar__Dispatcher_StartGame/process.md 全文已读）：
    // msg@-0x10、param@-0xc 两局部槽；check_error 结果直存 p+4（call 后回读槽，
    // 不 hoist %ebx）；(p+4)!=0 → 真体 fall-through return 0（je → call 段）。
    // [b28d-solo-S162] P211 档③：字段与 RHS(int) 异型 ⇒ 4.4 翻 RHS-first+%edx 中转
    // （ORIG 0x0813f8d8 `mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,0x4(%eax)`）。
    struct _P4 { int m0; unsigned int m4; };
    MSG_BASE& m = msg;                // ORIG -0x10 死存储（调用直传形参 msg）
    _P4* const p = reinterpret_cast<_P4*>(&param);   // ORIG -0xc（指针形：
                                                    // store 时 base 从 -0xc 载 %eax、
                                                    // 值留 %edx → ORIG `mov %edx,0x4(%eax)`）
    (void)m;
    // ORIG 0x813f8ce：call → mov %eax,%edx（value 先落 %edx）→ `mov -0xc(%ebp),%eax`
    // （地址从 p 的局部槽 reload，非形参槽 0x14）→ `mov %edx,0x4(%eax)`。
    // [b28d-solo-S162] 原具名 ec + dst 两局部各占一槽（帧 +8、槽整体漂移）。
    // ORIG 值是 call 结果右值直传（P0 无槽往返）⇒ 单语句内联实参形。
    p->m4 = check_error(user, msg);
    if (p->m4 != 0)
    {
        return 0;
    }
    {
        user->m_advanceAltar.checkAndCreateDefaultRidable();  // ORIG this=user+0x8df60
    }
    return 0;
}
#include "Dispatchers/read_5.inc"
{
    // ORIG 0x813f7b0：读 0xd 字节（msg 引用先存 -0xc）
    MSG_BASE& m = msg;
    packet.get_binary((char*)&m, 0xd);
    return 0;
}
#include "Dispatchers/send.inc"
{
    // ORIG 0x813f92a
    struct _P4 { int m0; int m1; };   // +0x04 槽（ORIG mov 0x4(%eax) 寻址模拟）
    // 技巧36：ORIG 只 `mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)` 单槽物化；
    // 我方多一个 ParamBase& p 中转变量 ⇒ 多占 -0x18 并把 PacketGuard 挤到 -0x24
    // （ORIG 是 -0x20），整帧位移。直接对形参取地址即可一并归位。
    _P4& q = *reinterpret_cast<_P4*>(&param);
    if (q.m1 == 0)
    {
        StageControl::gotoStage(user);   // ORIG 0x0812fdd4 静态语义
    }
    PacketGuard guard;
    guard.put_header(1, 0x23c);
    guard.put_byte(1);
    guard.put_binary((char*)&q, 8);              // ORIG `mov -0x14(%ebp),%eax`（q 槽）
    if (q.m1 != 0)
    {
        guard.finalize(true);
        user->Send(guard);
    }
    else
    {
        user->m_advanceAltar.setCurrentAdvanceAltarData(RidableId::RIDEABLE);  // ORIG 常量 1
        user->set_state((ch_state)0xd);
        guard.put_int(1);
        // b28d-solo-Dispatchers verdict=DIFF [N-P63] ORIG 0x0813f92a orig/ours=117/114
        //   ORIG 循环 = 入口 `jmp <底部条件>`（P283）+ 条件 `setle/test/jne` 物化（P51）
        //   两旋钮组合。实测：goto 尾标签形得入口 jmp 但条件被 4.4 折成 `jle`；
        //   do-while 形得 setcc 物化但无入口 jmp ⇒ 两形不可兼得，按 N-P63/N112 封顶。
        // [b28d-solo-S206] 再试 while 形（P25：while 生前置 jmp 条件 + 尾部
        //   cmpl/setle/test/jne 物化；do-while 形缺入口 jmp，goto 形丢 setcc）。
        int i = 0;
        while (i <= 0)
        {
            AdvanceAltarData* data = user->m_advanceAltar.getAdvanceAltarDataByIndex(i);
            if (data != 0)
            {
                putRidableInfoToPacket(guard, *data);
                // [b28d-solo-Dispatchers] P223 形：ORIG 0x0813fa57 `je 0813fa6d`
                //   （跳过清零块）＋ 0x0813fa6a 体尾 `jmp 0813fa6d` ＋ 0x0813fa6c
                //   `nop` 岛 ⇒ 负条件 goto 尾空标签（普通 if 形被 4.4 jump-thread）。
                if (data->m_field20 == 0) goto update_done;
                data->m_field20 = 0;
                data->m_update = 1;      // ORIG +0xcc
                update_done:;
            }
            ++i;
        }
        guard.finalize(true);
        user->Send(guard);
    }
}

#include "Dispatchers/dtor_Dispatcher_SummonUnit.inc"
#include "Dispatchers/dispatch_sig_4.inc"

#include "Dispatchers/process_5.inc"

#include "Dispatchers/read_6.inc"

#include "Dispatchers/send_packet_4.inc"


#include "Dispatchers/check_error_5.inc"

#include "Dispatchers/read_packet_4.inc"

#include "Dispatchers/process_packet_4.inc"


#include "Dispatchers/dtor_Dispatcher_UpgradeGage.inc"
#include "Dispatchers/dispatch_sig_5.inc"

#include "Dispatchers/process_6.inc"

#include "Dispatchers/read_7.inc"

#include "Dispatchers/send_packet_5.inc"


#include "Dispatchers/check_error_6.inc"

#include "Dispatchers/read_packet_5.inc"

#include "Dispatchers/process_packet_5.inc"


#include "Dispatchers/dtor_Dispatcher_ExchangeSlot.inc"
#include "Dispatchers/dispatch_sig_6.inc"

#include "Dispatchers/process_7.inc"

#include "Dispatchers/read_8.inc"

#include "Dispatchers/send_packet_6.inc"


#include "Dispatchers/check_error_7.inc"

#include "Dispatchers/read_packet_6.inc"

// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081408a4 orig/ours=59/59 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    0x24(%eax),%ecx | lea    0x18(%eax),%edx | mov    0xc(%ebp),%eax
// [g6 2026-09-11] 本 TU 具名视图：ORIG 0x081408a4 两 slot 取址为单条
//   `lea 0x24(%eax),%ecx` / `lea 0x18(%eax),%edx`；协议头 uchar[12] 数组退化
//   +cast 会产 `add $imm,%eax; mov %eax,%edx`（2 条/处）。探针
//   /tmp/k1sbx/g6/probe/p1.cpp：具名 struct 成员取址折 lea、成员读折单条 disp。
struct AltarExchangeSlotReqView
{
    char m_pad0d[0xd];                          // +0x00（STBaseRequest 线格式头）
    int m_field10;                              // +0x10
    char m_pad14[4];                            // +0x14
    union { unsigned char m_raw[0xc];
            CMDPacketStruct::_SetSlotData m_data; } m_slot1;   // +0x18
    union { unsigned char m_raw[0xc];
            CMDPacketStruct::_SetSlotData m_data; } m_slot2;   // +0x24
};
#include "Dispatchers/process_packet_6.inc"


#include "Dispatchers/dtor_Dispatcher_GetAchievementReward.inc"
#include "Dispatchers/dispatch_sig_7.inc"

#include "Dispatchers/process_8.inc"

#include "Dispatchers/read_9.inc"

#include "Dispatchers/send_packet_7.inc"


#include "Dispatchers/check_error_8.inc"

#include "Dispatchers/read_packet_7.inc"

#include "Dispatchers/process_packet_7.inc"


}
#include "Dispatchers/dtor_Dispatcher_Redeem.inc"
#include "Dispatchers/dispatch_sig_8.inc"

#include "Dispatchers/process_9.inc"
#include "Dispatchers/read_10.inc"

#include "Dispatchers/dtor_Dispatcher_UseDye.inc"
#include "Dispatchers/check_error_9.inc"

#include "Dispatchers/read_11.inc"


#include "Dispatchers/dtor_Dispatcher_Antibot.inc"
#include "Dispatchers/dispatch_sig_9.inc"

#include "Dispatchers/process_10.inc"
#include "Dispatchers/read_12.inc"

#include "Dispatchers/dtor_Dispatcher_ChangeHp.inc"
#include "Dispatchers/dispatch_sig_10.inc"

#include "Dispatchers/process_11.inc"
#include "Dispatchers/read_13.inc"

#include "Dispatchers/dtor_Dispatcher_Teleport.inc"

#include "Dispatchers/read_14.inc"



#include "Dispatchers/dtor_Dispatcher_UseJewel.inc"
#include "Dispatchers/dispatch_sig_11.inc"
{
    // ORIG 0x08217bd6 帧 sub $0x288：除 4 个实参槽外还有 0x270 字节未使用局部（P31 dead local）。
    char __orig_dead[624];
    return LineFunc(0xc9e2,
                    __PRETTY_FUNCTION__,
                    0, 0);
}
#include "Dispatchers/process_12.inc"
#include "Dispatchers/read_15.inc"

#include "Dispatchers/dtor_Dispatcher_BingoQuiz.inc"
// ORIG getBingoData() 返回 void*；check_error 按 +0x04/+0x05 char 字段单级读取
// （df_game_r 0x0825eb5d movzbl 0x5(%eax),%eax / 0x0825eb72 movzbl 0x4(%eax),%eax）。
// ORIG send 族（0x081e3056 等）：param 指针落 -0xc 槽，字段两次 mov 0x4(%eax) 单级读。
struct Err4ParamView { int m_pad0; int m_4; };

struct BingoCheckView { char m_pad[4]; char m_b4; bool m_b5; unsigned char m_b6; };  // bool 免 setne 物化

#include "Dispatchers/check_error_10.inc"

#include "Dispatchers/process_13.inc"

#include "Dispatchers/read_16.inc"

#include "Dispatchers/send_1.inc"



#include "Dispatchers/dtor_Dispatcher_DieMobAll.inc"
#include "Dispatchers/dispatch_sig_12.inc"

#include "Dispatchers/process_14.inc"
#include "Dispatchers/read_17.inc"

#include "Dispatchers/dtor_Dispatcher_SkillInit.inc"
#include "Dispatchers/check_error_11.inc"

#include "Dispatchers/process_15.inc"

#include "Dispatchers/read_18.inc"

#include "Dispatchers/send_2.inc"

#include "Dispatchers/process_skill_init.inc"


#include "Dispatchers/dtor_Dispatcher_Antibot_DP.inc"
#include "Dispatchers/dispatch_sig_13.inc"

#include "Dispatchers/process_16.inc"
#include "Dispatchers/read_19.inc"

#include "Dispatchers/dtor_Dispatcher_AskRematch.inc"
#include "Dispatchers/dispatch_sig_14.inc"

#include "Dispatchers/process_17.inc"
#include "Dispatchers/read_20.inc"

#include "Dispatchers/dtor_Dispatcher_BVHackInfo.inc"
#include "Dispatchers/dispatch_sig_15.inc"

#include "Dispatchers/process_18.inc"
#include "Dispatchers/read_21.inc"

#include "Dispatchers/dtor_Dispatcher_BlueMarble.inc"
#include "Dispatchers/check_error_12.inc"

#include "Dispatchers/process_19.inc"

#include "Dispatchers/read_22.inc"

#include "Dispatchers/send_3.inc"


#include "Dispatchers/dtor_Dispatcher_ChangeHost.inc"
#include "Dispatchers/dispatch_sig_16.inc"

#include "Dispatchers/process_20.inc"
#include "Dispatchers/read_23.inc"

#include "Dispatchers/checkComboSkillIndex.inc"
{
    // G_CDataManager() 返回对象 +0x14 处为组合技列表基址 char*
    // （成员访问形态 mov 0x14(%eax),%eax 与 ORIG 一致；镜像视图同 ComboSkill.cpp）
    struct DataManagerComboListView
    {
        char m_pad[0x14];
        char* m_pComboList;  // +0x14
    };
    std::vector<std::pair<int, int> >* comboList =
        (std::vector<std::pair<int, int> >*)(
            ((DataManagerComboListView*)G_CDataManager())->m_pComboList + 0x48fc);
    for (int i = 0; i <= 5; ++i)
    {
        // ORIG 0x0825f2b9：循环体首 `movswl -0x1c(%ebp),%ebx`（idx 符号扩展
        // 跨 operator[] call 缓存 %ebx），比较 `cmp %eax,%ebx; sete`（技巧22/32）。
        // [b28c-30] P0 清绑：原 register int probe asm("ebx") 绑定已删（N96），
        // 改 short 形参表达式直用（P200：短整表达式作普通实参/比较直传）。
        if (idx == (*comboList)[i].first)
        {
            return 1;
        }
    }
    return 0;
}

#include "Dispatchers/dtor_Dispatcher_ComboSkill.inc"
#include "Dispatchers/check_error_13.inc"

#include "Dispatchers/process_21.inc"

#include "Dispatchers/read_24.inc"

#include "Dispatchers/send_4.inc"
{
    stComboSkill& out = static_cast<stComboSkill&>(param);
    if (out.m_result == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1fd);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fd,
                                 (unsigned char)(out.m_result & 0xff));
    }
}

#include "Dispatchers/dtor_Dispatcher_GM_Command.inc"
// ---- GM 命令信号局部视图（仅本 TU 使用）----
namespace gm_pkt
{
#pragma pack(push, 1)
// SIG_FORBID_USER_TO_PLAY 前 9 字节视图（ORIG memset 9B 后按 +0/+4/+5 写）
struct SigForbidPlayBody
{
    int m_characNo;
    unsigned char m_flag;
    int m_days;
};
#pragma pack(pop)
}  // namespace gm_pkt

#include <algorithm>                          // case9 std::min（ORIG 080df90e 库调用）
// 显式路径:Game 同名空壳优先级会遮蔽 -I（CAuction.cpp 同案）
#include "../../../shared/packet/include/Packet_Monitor_Notice_Message.h"   // ORIG 真类（W ctor 0x0822bbd6，case3 GMS）
#include "../../../shared/packet/include/Packet_Forbid_Chat_By_Monitor.h"   // ORIG 真类（W ctor 0x0822bc70，case5 禁言）

// §6.2 巨件核销：P167 344/344 [0缺0多]；gap 2058→1960；残差归因 N82
// （帧/槽号全帧错位+两守卫源语句顺序）+ N78（sub $0x51c vs $0x43c 帧），语义完整
// （详见 docs/asm_translation_patterns.md 负例表）。真修：
// ① case3/5 监控包 gm_pkt::MonitorNoticeMessage/ForbidChatByMonitor 发明符号迁 ORIG
//   真类 Packet_Monitor_Notice_Message(ctor 0x0822bbd6)/Packet_Forbid_Chat_By_Monitor
//   (ctor 0x0822bc70)（显式路径 include 避 Game 同名空壳，CAuction.cpp 同案）；
// ② case4/5/6/9/0xb/0xc/0xd 补 ORIG 30B 零填充查表副本 memset+strncpy（P0 槽律）；
// ③ case9 升降级循环 if(nextLv>200) 内联化改回真实 std::min 库调用（ORIG 080df90e）；
// ④ 各 case get_int/get_str 返回值按 ORIG 不测试（bool 返回仅 get_byte 检查）；
// ⑤ 始段 accIdStr 命名缓存按 P0 内联为实参右值。
// ORIG 0x81fa19e：GM 命令分发（cmd=0..0xd）。校验链 user!=0/getCurCharacR!=0/
// get_byte 成功；失败走 LineFunc 断言路径（返回 0）。响应包 header 0x6e：
// local_1d0 = put_byte(1)+put_byte(cmd)+payload；local_1c4 = 固定拒绝包
// (put_byte(99),put_byte(1))，目标用户不存在时发给 GM 本人。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081fa19e orig/ours=2058/1960 ndiff-blocks=355
//   帧差 sub $51c vs $42c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
// [BigGM2-wave9 re-audit] 语义复核后 3 处真修（canon ndiff 1265→1221，gap 84→49，
//   calls 344/344 全平、cmp imm 0x63/0x64 差异消解）：
//   ①头部求值序：ORIG 0x81fa220-272 = get_acc_id→NumberToString→(mov %eax,%ebx)→
//     trace ctor(0x8908)——P329 ⑤ 的内联把 ctor 提前了 2 个 call，改回具名局部；
//   ②case9 result：ORIG -0x4d 为 **unsigned char** 槽（0x81fb607 `mov %al,-0x4d` 低
//     8 位截断 + 0x81fb681 `cmpb $0x0`），int→unsigned char 忠实复现截断语义；循环改
//     回 ORIG 叶块序（diff==0→升→降）+ 独立计数器形（0x81fb5b7 -0x4c/-0x48 setl/setg）；
//   ③case0xd clamp：ORIG 0x81fc20d 三分支 test/jg→cmp $0x63,jle（>99→100），三目链
//     改 if/else-if 形；④cmd 零化删（ORIG 0x81fa277 无 `movb $0` 直发 get_byte）。
//   残差 49 指令全为 4.1 码风族（源码级不可诱发，case1 已实验 `!(<=)` 反转无效）：
//   xor$1/testal（14 处，@0x81fa406）· setle+jne（14 处，@0x81fa49e）· lea 基址+add
//   拆分寻址（53 处，@0x81fa277）· ebx/esi 跨 call 寄存器缓存（@0x81fa235/0x81faaca）
//   · false-island 聚尾排布（@0x81fc41a..）。struct=549 与上述族一一对应。判 N82+N78 维持。
//   首差: sub    $0x51c,%esp | movl   $&_ZZN21Dispatcher_GM_Command12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN21Dispatcher
// [SweepDispatchers 2026-09-09] 独立复核：p167 344/344 仅剩 C1/C2/D1/D2 同址别名（P93）；
//   多重集差 133/63 全为上述 4.1 码风族（add 链 53 / setle 14 / xor 14 / mov$imm 30）；
//   无缺失 callee/分支 ⇒ 维持 N82+N78（wave9 三处真修仍在；当前 1975 vs ORIG 2058）。
// [SweepDispatchers3 2026-09-09] 本轮四处修复：①14 处守卫改负向早退 break（复现 ORIG
//   xor$1/test/jne 14 处 + setle/test/jne 14 处；探针 /tmp/probe_gm2.cpp f10 逐条同形）；
//   ②cmd/len/extra/name/text 改 GmCmdArgs pack(1) 视图（成员取址 = ORIG lea 基址+add 偏移
//   53 处；探针 /tmp/probe_view.cpp）；③case4 查找键改回 ORIG 的 name(+0x12)——lookupName
//   的 memset/strncpy 是原码死写入（P171）；④删 len/wLen/extra/target 零初始化。
//   gap 83→20（2005 vs ORIG 2025），残差 = cMyTrace varargs 寄存器缓存（N80/N7 族）。
namespace
{
// ORIG dispatch_sig 的参数缓冲视图（偏移照 ORIG 反汇编 @0x81fa277..）：
// cmd@+0x0d / len@+0x0e，其余成员按 case 复用 +0x12 起区域（union 视角）。
// 取址经「lea 基址; add 偏移」正是 ORIG 4.1 形态（探针 /tmp/probe_view.cpp 实证
// 4.4.7 对局部结构体成员取址同形）。
#pragma pack(push,1)
struct GmCmdArgs
{
    char m_pad0[0xd];
    unsigned char cmd;      // +0x0d
    int len;                // +0x0e
    union
    {
        char text[0x100];                                          // +0x12（case1/2/3 公告）
        struct { char name[0x1e]; } kick;                          // +0x12（case4/0xb/0xc）
        struct { int extra; char name[0x1e]; } chat;               // +0x12/+0x16（case5/6/9/0xd）
        struct { unsigned short wLen; int extra; char name[0x1e]; } item;  // +0x12/+0x14/+0x18（case10）
    } u;
};
#pragma pack(pop)
}
#include "Dispatchers/dispatch_sig_17.inc"

#include "Dispatchers/process_22.inc"

#include "Dispatchers/read_25.inc"


#include "Dispatchers/dtor_Dispatcher_GuildCargo.inc"
#include "Dispatchers/process_23.inc"

#include "Dispatchers/read_26.inc"
#include "Dispatchers/send_5.inc"


#include "Dispatchers/dtor_Dispatcher_Join_Power.inc"
#include "Dispatchers/dispatch_sig_18.inc"

#include "Dispatchers/process_24.inc"
#include "Dispatchers/read_27.inc"

#include "Dispatchers/dtor_Dispatcher_MotionHack.inc"
#include "Dispatchers/read_28.inc"


#include "Dispatchers/process_25.inc"


#include "Dispatchers/dtor_Dispatcher_MoveToGate.inc"
#include "Dispatchers/dispatch_sig_19.inc"

#include "Dispatchers/process_26.inc"
#include "Dispatchers/read_29.inc"


#include "Dispatchers/dtor_Dispatcher_RedeemList.inc"
#include "Dispatchers/dispatch_sig_20.inc"

#include "Dispatchers/process_27.inc"
#include "Dispatchers/read_30.inc"

#include "Dispatchers/dtor_Dispatcher_SelectItem.inc"
#include "Dispatchers/dispatch_sig_21.inc"

#include "Dispatchers/IsEmancipate.inc"


#include "Dispatchers/IsEmancipate_1.inc"

#include "Dispatchers/process_28.inc"
#include "Dispatchers/read_31.inc"

#include "Dispatchers/dtor_Dispatcher_TraceError.inc"
#include "Dispatchers/dispatch_sig_22.inc"

#include "Dispatchers/process_29.inc"
#include "Dispatchers/read_32.inc"

#include "Dispatchers/dtor_Dispatcher_TryEnchant.inc"
#include "Dispatchers/dispatch_sig_23.inc"

#include "Dispatchers/process_30.inc"
#include "Dispatchers/read_33.inc"

#include "Dispatchers/dtor_Dispatcher_VerifyGold.inc"
#include "Dispatchers/dispatch_sig_24.inc"

#include "Dispatchers/process_31.inc"
#include "Dispatchers/read_34.inc"

#include "Dispatchers/dtor_Dispatcher_BingoReward.inc"
#include "Dispatchers/check_error_14.inc"

#include "Dispatchers/process_32.inc"

#include "Dispatchers/read_35.inc"
#include "Dispatchers/send_6.inc"


#include "Dispatchers/dtor_Dispatcher_CranePickUp.inc"
#include "Dispatchers/dispatch_sig_25.inc"

#include "Dispatchers/process_33.inc"
#include "Dispatchers/read_36.inc"

#include "Dispatchers/dtor_Dispatcher_EPLPCommand.inc"
#include "Dispatchers/dispatch_sig_26.inc"

#include "Dispatchers/process_34.inc"
#include "Dispatchers/read_37.inc"

#include "Dispatchers/dtor_Dispatcher_ModItemAttr.inc"



#include "Dispatchers/dtor_Dispatcher_SecurityLog.inc"
#include "Dispatchers/process_35.inc"

#include "Dispatchers/read_38.inc"
{
    // ORIG 0x0825fabc：int 中转 → 0xd；循环 5 个 int → 0x11/0x15/0x19/0x1d/0x21；失败 0x99a/0x9a2
    // [b28d-solo-S206] P0：ORIG 0x0825fabc 无 msg 指针局部（`lea -0x10(%ebp),%eax`
    //   直传 &first，`mov 0x10(%ebp),%eax` 直接用形参槽）+ first 无预清零
    //   （无 movl $0）⇒ 删指针局部、去初始化式。
    int first;
    if (!packet.get_int(first))
        return 0x99a;
    // [b28d-solo-S206] P211 档③ + disp 折叠：ORIG `mov 0x10(%ebp),%eax;
    //   mov -0x10(%ebp),%edx; mov %edx,0xd(%eax)`（LHS msg 先载、值直存 disp）；
    //   裸 cast 解引用恒产 lea+中转 ⇒ 改具名视图成员。
    struct __attribute__((packed)) SecurityLogView
    {
        char m_head[0xd];
        int m_first;              // +0x0d
    };
    reinterpret_cast<SecurityLogView*>((char*)&msg)->m_first = first;
    for (int i = 0; i < 5; ++i)
    {
        // [FL1 2026-09-10 负结论] ORIG 0x0825fafa 加法序 =
        //   `add $0x4,%edx; shl $0x2,%edx; add %edx,%eax; add $0x1,%eax`
        //   （+1 加在「基址+缩放」之后）。三形实测：
        //   ① `msg + (i+4)*4 + 1`                    → 41/41 NEAR（最优，现形）
        //   ② `(char*)&((uint*)&msg)[i+4] + 1`       → 同 ①（+1 仍折索引侧）
        //   ③ `(uint)((uint*)&msg)[i+4] + 1`（P319 整型加法）→ 42 条且
        //      TU ae−1/diff+1（劣化，已回滚）
        // ⇒ 4.4 恒把尾 +1 折进索引侧，属 P205 封顶（旧注释 probe_add2 同结论）。
        if (!packet.get_int(*reinterpret_cast<unsigned int*>(
                (char*)&msg + (i + 4) * 4 + 1)))
            return 0x9a2;
    }
    return 0;
}
#include "Dispatchers/send_7.inc"

#include "Dispatchers/dtor_Dispatcher_Select_Card.inc"
#include "Dispatchers/dispatch_sig_27.inc"

#include "Dispatchers/process_36.inc"
#include "Dispatchers/read_39.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeItem.inc"
#include "Dispatchers/dispatch_sig_28.inc"

#include "Dispatchers/process_37.inc"
#include "Dispatchers/read_40.inc"

#include "Dispatchers/dtor_Dispatcher_AgreeEnchant.inc"
#include "Dispatchers/dispatch_sig_29.inc"

#include "Dispatchers/process_38.inc"
#include "Dispatchers/read_41.inc"

#include "Dispatchers/dtor_Dispatcher_Back2Village.inc"
#include "Dispatchers/dispatch_sig_30.inc"

#include "Dispatchers/process_39.inc"
#include "Dispatchers/read_42.inc"

#include "Dispatchers/check_error_15.inc"


#include "Dispatchers/dtor_Dispatcher_BossDieCheck.inc"
#include "Dispatchers/process_40.inc"

#include "Dispatchers/read_43.inc"

#include "Dispatchers/send_8.inc"


#include "Dispatchers/dtor_Dispatcher_CodeCheckSum.inc"
// [b28c-31] P0 清绑（原 register char* pp asm("eax") + register bool notBak
// asm("al") 已删）；残差 N18a：ORIG 0x0820459a 逐参立即 `lea path→%eax` 后压栈，
// 4.4 提前物化第二 lea 到 %edx（`lea -0x34,%edx`）⇒ 单段装载序不可造。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082044b0 orig/ours=101/99 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN23Dispatcher_CodeCheckSum12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN23Dispatcher_CodeCheckSum12disp
// [P4-p167 符号性] ORIG callee 直证 _ZSt6strstrPcPKc（C++ 链接的 std::strstr 重载）：
//   4.4.6 头集被 __CORRECT_ISO_CPP_STRING_H_PROTO 屏蔽该 inline 重载（cstring:117），
//   源码侧写 std::strstr 只会解析到 C 符号 strstr。故以 asm 标签别名引用同一符号
//   （定义由 game target 的 LinuxService.cpp 以 weak out-of-line 提供；已 AE 10/10）。
//   探针实测：块内写 extern "C++" 会被 4.4 判 `expected unqualified-id`，须置文件作用域。
#if remake_verify
extern "C++" char* dnf_std_strstr(char*, const char*) __asm__("_ZSt6strstrPcPKc");
#else
// 现代 libstdc++ 自带 std::strstr(char*, const char*) inline 重载，无需符号别名。
inline char* dnf_std_strstr(char* s1, const char* s2) { return ::strstr(s1, s2); }
#endif
#include "Dispatchers/dispatch_sig_31.inc"

#include "Dispatchers/process_41.inc"
#include "Dispatchers/read_44.inc"

#include "Dispatchers/check_error_16.inc"


// process 依赖（清单外）。ORIG 0x0822c75a 实证：继承 PacketHeader（先调
// PacketHeaderC1Ett(0x27e6,0x39)），再清 itemIdx(+0x12)/characNo(+0x16)/
// accId(+0xe)/nameLen(+0x1a)，最后 memset name(+0x1b, 0x1e)；总长 0x39。
class Packet_CollectItems : public PacketHeader
{
public:
    Packet_CollectItems();  // ORIG W 0x0822c75a

    unsigned int m_idByChannel;  // +0x0a
    unsigned int m_accId;        // +0x0e
    int m_itemIdx;             // +0x12
    unsigned int m_characNo;     // +0x16
    unsigned char m_nameLen;     // +0x1a
    char m_name[0x1e];           // +0x1b..+0x38
} __attribute__((packed));       // 总长 0x39

#include "Dispatchers/Packet_CollectItems.inc"


#include "Dispatchers/dtor_Dispatcher_CollectItems.inc"
#include "Dispatchers/process_42.inc"

#include "Dispatchers/read_45.inc"

#include "Dispatchers/send_9.inc"


#include "Dispatchers/dtor_Dispatcher_DepositMoney.inc"
#include "Dispatchers/dispatch_sig_32.inc"

#include "Dispatchers/process_43.inc"
#include "Dispatchers/read_46.inc"

#include "Dispatchers/dtor_Dispatcher_EvadeAssault.inc"
#include "Dispatchers/check_error_17.inc"

#include "Dispatchers/process_44.inc"

#include "Dispatchers/read_47.inc"

#include "Dispatchers/dtor_Dispatcher_FairPvPScore.inc"
#include "Dispatchers/dispatch_sig_33.inc"

#include "Dispatchers/process_45.inc"
#include "Dispatchers/read_48.inc"

#include "Dispatchers/dtor_Dispatcher_HumanCertify.inc"
#include "Dispatchers/dispatch_sig_34.inc"

#include "Dispatchers/process_46.inc"
#include "Dispatchers/read_49.inc"

#include "Dispatchers/dtor_Dispatcher_InformNotice.inc"
#include "Dispatchers/check_error_18.inc"

#include "Dispatchers/process_47.inc"

#include "Dispatchers/read_50.inc"

#include "Dispatchers/send_10.inc"


#include "Dispatchers/dtor_Dispatcher_JoinPowerWar.inc"
#include "Dispatchers/dispatch_sig_35.inc"

#include "Dispatchers/process_48.inc"
#include "Dispatchers/read_51.inc"

#include "Dispatchers/dtor_Dispatcher_MailBox_Open.inc"
#include "Dispatchers/check_error_19.inc"

#include "Dispatchers/process_49.inc"

#include "Dispatchers/read_52.inc"
#include "Dispatchers/send_11.inc"




#include "Dispatchers/dtor_Dispatcher_MemberSecede.inc"
#include "Dispatchers/check_error_20.inc"

#include "Dispatchers/process_50.inc"

#include "Dispatchers/read_53.inc"


#include "Dispatchers/dtor_Dispatcher_RequestMatch.inc"
#include "Dispatchers/dispatch_sig_36.inc"

#include "Dispatchers/process_51.inc"
#include "Dispatchers/read_54.inc"

#include "Dispatchers/dtor_Dispatcher_ScanBotByDll.inc"
#include "Dispatchers/check_error_21.inc"

#include "Dispatchers/read_55.inc"


#include "Dispatchers/process_52.inc"


#include "Dispatchers/dtor_Dispatcher_ScanBotByDrv.inc"
#include "Dispatchers/check_error_22.inc"
#include "Dispatchers/process_53.inc"

#include "Dispatchers/read_56.inc"


#include "Dispatchers/dtor_Dispatcher_Secede_Power.inc"
#include "Dispatchers/dispatch_sig_37.inc"

#include "Dispatchers/process_54.inc"
#include "Dispatchers/read_57.inc"

#include "Dispatchers/dtor_Dispatcher_SoloTeleport.inc"

#include "Dispatchers/dtor_Dispatcher_TitleBookGet.inc"
#include "Dispatchers/process_55.inc"

#include "Dispatchers/read_58.inc"


#include "Dispatchers/dtor_Dispatcher_TitleBookPut.inc"
#include "Dispatchers/process_56.inc"

#include "Dispatchers/read_59.inc"


#include "Dispatchers/dtor_Dispatcher_UpgradeCargo.inc"
#include "Dispatchers/dispatch_sig_38.inc"

#include "Dispatchers/process_57.inc"
#include "Dispatchers/read_60.inc"

#include "Dispatchers/dtor_Dispatcher_UseLimitCube.inc"
// process/read/send/check_error 实现于 Dispatcher_UseLimitCube_impl.cpp（ORIG 0x081d3d38 族）。

#include "Dispatchers/dtor_Dispatcher_AssertManager.inc"
#include "Dispatchers/dispatch_sig_39.inc"

#include "Dispatchers/process_58.inc"
#include "Dispatchers/read_61.inc"

#include "Dispatchers/dtor_Dispatcher_CallGuildInfo.inc"
#include "Dispatchers/dispatch_sig_40.inc"

#include "Dispatchers/process_59.inc"
#include "Dispatchers/read_62.inc"

#include "Dispatchers/dtor_Dispatcher_ChangeEmotion.inc"
#include "Dispatchers/check_error_23.inc"

#include "Dispatchers/process_60.inc"


#include "Dispatchers/read_63.inc"

#include "Dispatchers/send_12.inc"



#include "Dispatchers/dtor_Dispatcher_Clear_Used_QP.inc"
#include "Dispatchers/check_error_24.inc"


#include "Dispatchers/process_61.inc"

#include "Dispatchers/read_64.inc"
#include "Dispatchers/send_13.inc"



#include "Dispatchers/dtor_Dispatcher_ConnectP2PPvP.inc"
#include "Dispatchers/dispatch_sig_41.inc"


#include "Dispatchers/process_62.inc"
#include "Dispatchers/read_65.inc"

#include "Dispatchers/dtor_Dispatcher_DenyJoinGuild.inc"
#include "Dispatchers/process_63.inc"
{
    // ORIG 0x81d51e6：bool deny 无槽（mov $1/$0,%eax merge test）；applicant=param+0xd→%esi、
    // guildkey→%ebx 跨 GetUID 调用（register 具名）；master/sub xor $1 直判（bool callee）。
    MSG_BASE* pm = &msg;               // ORIG 顶部死槽 -0xc（0x10(%ebp)=msg，后回读 +0xd）
    if (user->get_state() != 3)
    {
        return 0;
    }
    // ORIG 短路 && 直判形（dA 探针实证 xor1;test;je 双段 + mov $1/$0 merge test）
    if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
    {
        return 0;
    }
    struct MsgView { char pad[0xd]; unsigned int applicant; } __attribute__((packed));
    // ORIG 0x081d5257: mov 0xd(%eax),%esi 单级非对齐装载（packed 视图）
    register const unsigned int applicant = reinterpret_cast<MsgView*>(pm)->applicant;
    register const int guildkey = user->get_charac_guildkey();
    DB_DenyJoinGuild::makeRequest(user->GetUID(), (unsigned int)guildkey, applicant);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x160);
    return 0;
}
#include "Dispatchers/read_66.inc"


#include "Dispatchers/dtor_Dispatcher_EnchantByBead.inc"
#include "Dispatchers/process_64.inc"

#include "Dispatchers/read_67.inc"

#include "Dispatchers/send_14.inc"


#include "Dispatchers/dtor_Dispatcher_GenCeraTicket.inc"
#include "Dispatchers/dispatch_sig_42.inc"

#include "Dispatchers/process_65.inc"
#include "Dispatchers/read_68.inc"

#include "Dispatchers/dtor_Dispatcher_GiveGiftToNPC.inc"

#include "Dispatchers/dispatch_sig_43.inc"

#include "Dispatchers/process_66.inc"

#include "Dispatchers/read_69.inc"


#include "Dispatchers/_getMaxFavorLevelNPCCnt.inc"


#include "Dispatchers/_getResultFavor.inc"


#include "Dispatchers/dtor_Dispatcher_GuildJoinList.inc"
#include "Dispatchers/process_67.inc"

#include "Dispatchers/read_70.inc"

#include "Dispatchers/dtor_Dispatcher_HatchCreature.inc"
#include "Dispatchers/check_error_25.inc"

#include "Dispatchers/process_68.inc"

#include "Dispatchers/read_71.inc"

#include "Dispatchers/send_15.inc"


#include "Dispatchers/dtor_Dispatcher_JoinGuildInfo.inc"
#include "Dispatchers/process_69.inc"

#include "Dispatchers/read_72.inc"

#include "Dispatchers/dtor_Dispatcher_MouseRegister.inc"
#include "Dispatchers/check_error_26.inc"


#include "Dispatchers/checkValiedPassword.inc"


#include "Dispatchers/decryptPassword.inc"


#include "Dispatchers/doPassPadReplay.inc"


#include "Dispatchers/dispatch_sig_44.inc"


#include "Dispatchers/makeRequest_2.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5fc3), true);
    **guard << (int)0x12b;
    **guard << (int)-1;
    SIG_GOBLIN_UPDATE_REWARD* sig = guard->GetInBuffer<SIG_GOBLIN_UPDATE_REWARD>();
    *(unsigned int*)&sig->m_pad[0] = a;
    sig->m_pad[4] = b;
    // P1/探针实证：数组下标 RHS 提前装载；具名 pack(1) 视图的 **bool 标量成员**
    // 赋值使 4.4 保持 ORIG 的「先装载基址指针、后读 RHS 槽」序（0x084244f8）。
    struct __attribute__((packed)) SigView { char m_head[5]; bool m_answer; };
    ((SigView*)sig)->m_answer = c;
    strncpy(&sig->m_pad[6], d, 0x10);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

#include "Dispatchers/dtor_Dispatcher_ObjectBringUp.inc"

#include "Dispatchers/dtor_Dispatcher_Overflow_Info.inc"
#include "Dispatchers/dispatch_sig_45.inc"

#include "Dispatchers/process_70.inc"
#include "Dispatchers/read_73.inc"


#include "Dispatchers/dtor_Dispatcher_QuickJoinRoom.inc"
#include "Dispatchers/dispatch_sig_46.inc"

#include "Dispatchers/process_71.inc"
#include "Dispatchers/read_74.inc"

#include "Dispatchers/dtor_Dispatcher_SelectStriker.inc"
#include "Dispatchers/check_error_27.inc"

#include "Dispatchers/process_72.inc"

#include "Dispatchers/read_75.inc"

#include "Dispatchers/send_16.inc"


#include "Dispatchers/dtor_Dispatcher_UseCraneStart.inc"
#include "Dispatchers/dispatch_sig_47.inc"


#include "Dispatchers/process_73.inc"
#include "Dispatchers/read_76.inc"

#include "Dispatchers/dtor_Dispatcher_UseRenameCard.inc"
#include "Dispatchers/check_error_28.inc"

#include "Dispatchers/process_74.inc"

#include "Dispatchers/read_77.inc"


#include "Dispatchers/getItem.inc"


#include "Dispatchers/dtor_Dispatcher_WithdrawMoney.inc"
#include "Dispatchers/dispatch_sig_48.inc"

#include "Dispatchers/process_75.inc"
#include "Dispatchers/read_78.inc"

#include "Dispatchers/dtor_Dispatcher_AuctionBidding.inc"
#include "Dispatchers/dispatch_sig_49.inc"

#include "Dispatchers/process_76.inc"
#include "Dispatchers/read_79.inc"

#include "Dispatchers/dtor_Dispatcher_BuyAutomatItem.inc"
#include "Dispatchers/dispatch_sig_50.inc"

#include "Dispatchers/process_77.inc"
#include "Dispatchers/read_80.inc"

#include "Dispatchers/dtor_Dispatcher_CallMemberList.inc"
#include "Dispatchers/check_error_29.inc"

#include "Dispatchers/process_78.inc"

#include "Dispatchers/read_81.inc"

#include "Dispatchers/dtor_Dispatcher_CheckJoinGuild.inc"
#include "Dispatchers/process_79.inc"

#include "Dispatchers/read_82.inc"
{
    // ORIG 0x081d4b4c：int@0xd（len）→ str@0x11（max 0x17）
    // P1：int@0xd 成员视图折叠单 disp。
    struct __attribute__((packed)) LenView { char m_head[0xd]; int m_len; };
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0xd)))
        return LineFunc(0x34db, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m + 0x11, 0x17, ((LenView*)m)->m_len))
        return LineFunc(0x34de, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

#include "Dispatchers/dtor_Dispatcher_CompoundEmblem.inc"


#include "Dispatchers/dtor_Dispatcher_DisJointAvatar.inc"
#include "Dispatchers/dispatch_sig_51.inc"

#include "Dispatchers/process_80.inc"
#include "Dispatchers/read_83.inc"

#include "Dispatchers/dtor_Dispatcher_DisguiseCancel.inc"
#include "Dispatchers/dispatch_sig_52.inc"

#include "Dispatchers/process_81.inc"
#include "Dispatchers/read_84.inc"

#include "Dispatchers/dtor_Dispatcher_HellPartyStart.inc"
#include "Dispatchers/dispatch_sig_53.inc"

#include "Dispatchers/process_82.inc"
#include "Dispatchers/read_85.inc"

#include "Dispatchers/dtor_Dispatcher_ItemDictionary.inc"
#include "Dispatchers/check_error_30.inc"

#include "Dispatchers/process_83.inc"

#include "Dispatchers/read_86.inc"

#include "Dispatchers/send_17.inc"


#include "Dispatchers/dtor_Dispatcher_Lag_Statistics.inc"
#include "Dispatchers/dispatch_sig_54.inc"

#undef base
#include "Dispatchers/process_84.inc"
#include "Dispatchers/read_87.inc"

#include "Dispatchers/dtor_Dispatcher_Mercenary_Info.inc"
#include "Dispatchers/process_85.inc"


#include "Dispatchers/read_88.inc"

#include "Dispatchers/send_18.inc"


#include "Dispatchers/dtor_Dispatcher_P2P_Statistics.inc"
#include "Dispatchers/check_error_31.inc"


#include "Dispatchers/process_86.inc"


#include "Dispatchers/read_89.inc"




#include "Dispatchers/dtor_Dispatcher_PvPChannelInfo.inc"
#include "Dispatchers/dispatch_sig_55.inc"

#include "Dispatchers/process_87.inc"
#include "Dispatchers/read_90.inc"

#include "Dispatchers/dtor_Dispatcher_RemovePvPBuddy.inc"
#include "Dispatchers/dispatch_sig_56.inc"


#include "Dispatchers/process_88.inc"
#include "Dispatchers/read_91.inc"

#include "Dispatchers/dtor_Dispatcher_RenameCreature.inc"
#include "Dispatchers/check_error_32.inc"

#include "Dispatchers/process_89.inc"

#include "Dispatchers/read_92.inc"
{
    char* pMsg = (char*)&msg;
    // P1：int@0x10 用成员视图折叠（ORIG `mov 0x10(%eax),%eax`）；short/byte 位保持算术形态。
    struct __attribute__((packed)) LenView { char m_head[0x10]; int m_len; };
    if (!packet.get_short(*(short*)(pMsg + 0x0e)))
    {
        return LineFunc(0x24b4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*(char*)(pMsg + 0x0d)))
    {
        return LineFunc(0x24b7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x10)))
    {
        return LineFunc(0x24ba,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(pMsg + 0x14, 0xd, ((LenView*)pMsg)->m_len))
    {
        return LineFunc(0x24bd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "Dispatchers/send_19.inc"
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stRenameCreature& p = static_cast<stRenameCreature&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x67, (unsigned char)p.m_4);
}

#include "Dispatchers/dtor_Dispatcher_UseBoosterItem.inc"


#include "Dispatchers/dtor_Dispatcher_UserHistoryLog.inc"
#include "Dispatchers/process_90.inc"
{
    // ORIG 0x081da6c0：单一 msg 槽 -0xc；lea 0x14(%eax) 先、movzbl 0xd(%eax) 折叠重读。
    // 具名字段视图（技巧14）：char* 下标反产 add+间接两步，字段直读才折叠单级。
    struct MsgView { char pad0d[0xd]; unsigned char b0d; };
    char* m = reinterpret_cast<char*>(&msg);
    user->LogHistory("act,%d,%s",
                     (int)reinterpret_cast<MsgView*>(m)->b0d, m + 0x14);
    return 0;
}
#include "Dispatchers/read_93.inc"


#include "Dispatchers/dtor_Dispatcher_BuyCeraShopItem.inc"


#include "Dispatchers/dtor_Dispatcher_CallGuildInvite.inc"
#include "Dispatchers/dispatch_sig_57.inc"

#include "Dispatchers/process_91.inc"
#include "Dispatchers/read_94.inc"

#include "Dispatchers/dtor_Dispatcher_CancelJoinGuild.inc"
#include "Dispatchers/process_92.inc"

#include "Dispatchers/read_95.inc"


#include "Dispatchers/dtor_Dispatcher_CeraPackageOpen.inc"

#include "Dispatchers/dtor_Dispatcher_ChangeGuildName.inc"
#include "Dispatchers/dispatch_sig_58.inc"

#include "Dispatchers/process_93.inc"
#include "Dispatchers/read_96.inc"

#include "Dispatchers/dtor_Dispatcher_CompleteDisplay.inc"
#include "Dispatchers/dispatch_sig_59.inc"

#include "Dispatchers/process_94.inc"
#include "Dispatchers/read_97.inc"

#include "Dispatchers/dtor_Dispatcher_CompleteLoadPvP.inc"
#include "Dispatchers/dispatch_sig_60.inc"

#include "Dispatchers/process_95.inc"
#include "Dispatchers/read_98.inc"

#include "Dispatchers/dtor_Dispatcher_Compound_Avatar.inc"
#include "Dispatchers/dispatch_sig_61.inc"

#include "Dispatchers/process_96.inc"
#include "Dispatchers/read_99.inc"

#include "Dispatchers/dtor_Dispatcher_DieBloodMonster.inc"
#include "Dispatchers/dispatch_sig_62.inc"

#include "Dispatchers/process_97.inc"
#include "Dispatchers/read_100.inc"

#include "Dispatchers/dtor_Dispatcher_DisguiseRequest.inc"
#include "Dispatchers/dispatch_sig_63.inc"

#include "Dispatchers/process_98.inc"
#include "Dispatchers/read_101.inc"

#include "Dispatchers/dtor_Dispatcher_DonateGuildFund.inc"
#include "Dispatchers/process_99.inc"

#include "Dispatchers/read_102.inc"


#include "Dispatchers/dtor_Dispatcher_GiveupExpertJob.inc"
#include "Dispatchers/dispatch_sig_64.inc"

#include "Dispatchers/process_100.inc"
#include "Dispatchers/read_103.inc"

#include "Dispatchers/dtor_Dispatcher_GoblinPadStatus.inc"
#include "Dispatchers/dispatch_sig_65.inc"

#include "Dispatchers/process_101.inc"
#include "Dispatchers/read_104.inc"

#include "Dispatchers/dtor_Dispatcher_LimitNpcBuyItem.inc"
#include "Dispatchers/check_error_33.inc"
{
    // ORIG 0x081e1172（docs/class_func_reports/Dispatcher_LimitNpcBuyItem/check_error.md）
    if (user == 0)
        return -1;
    // +0x1b：state<3 或 charac 空 → -1
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        return -1;

    // +0x54：msg+0x0d / msg+0x11 两字段。ORIG 0x081e11c6 段为具名指针槽 -0x10
    //   （mov %eax,-0x10(%ebp)）+ 单级 disp 装载（mov 0xd(%eax),%esi），故用
    //   packed 具名视图 + 指针局部（裸 `char*+0xd` 恒产 add+mov 两段，§4.3）。
    MSG_BASE* pMsg = &msg;
    struct __attribute__((packed)) LimitMsgView
    {
        char m_head[0xd];
        unsigned int m_field0d;    // +0x0d
        unsigned int m_field11;    // +0x11
    };
    // +0x66：限购列表校验。两实参为内联表达式（P0：ORIG 无值槽往返 ⇒ 右值直传，
    //   4.4 自然升格 callee-saved 跨 G_CDataManager() call 保活 = ORIG 的 esi/ebx）。
    if (!G_CDataManager()->checkLimitNpcBuyItemList(
            ((LimitMsgView*)pMsg)->m_field11,
            (unsigned long)((LimitMsgView*)pMsg)->m_field0d))
    {
        // +0x82：疑似黑客用户 → trace（P227 全表达式临时链：ctor 前求值 varargs 保活）
        cMyTrace(__PRETTY_FUNCTION__, 0x55e1, 5)(
            "May be HackUser or Must be HackUser : m_id=%s, charac_no=%u",
            NumberToString(user->get_acc_id(), 0), user->get_charac_no(-1));
        return -1;
    }

    // +0x115：交易中 → 0x13
    if (user->CheckInTrade())
        return 0x13;
    // +0x137：私商占用 → 0x3c
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        return 0x3c;

    // +0x163：查找物品模板（msg+0x0d 经 pMsg 槽重读，ORIG 值先入 ebx 跨 call 保活）
    CItem* item = G_CDataManager()->find_item((int)((LimitMsgView*)pMsg)->m_field0d);
    if (item == 0)
        return 0x11;

    // +0x169：所需材料查询
    Inven_Item matItem;                                           // ctor 080cb854
    // [G3 2026-09-11] P0 具名值局部：ORIG 0x081e12f4 `mov 0x4(%eax),%edx; mov (%eax),%eax;
    //   mov %eax,-0x28(%ebp); mov %edx,-0x24(%ebp)` = 8B 值落双槽（非指针槽）；
    //   使用点 ORIG 为 `mov -0x28(%ebp),%eax; test` / `cmp $-1`（我方原双条解引用）。
    const std::pair<int, int> mat =
        *reinterpret_cast<const std::pair<int, int>*>(
            NeedMaterialDataManager::getInstance()->getNeedMaterial(item));
    if (mat.first == 0 || mat.first == -1)
        return 0x17;

    // +0x1a6：背包内材料数据
    if (((const CInventory*)user->getCurCharacInvenRefR())->GetInvenData(mat.first, matItem) < 0)
        return 0x15;
    // +0x1d8：数量校验（need < 1 或持有 < need → 0x16）
    if (mat.second < 1 || matItem.get_add_info() < mat.second)
        return 0x16;

    // +0x20b：构造购买物品（itemIdx = msg+0x0d）并尝试入包
    Inven_Item buyItem;                                           // ctor 080cb854（-0xa2）
    buyItem.m_addInfo = (int)((LimitMsgView*)pMsg)->m_field0d;    // msg+0x0d → item+0x02
    G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, buyItem, 1);
    if (((const CInventory*)user->getCurCharacInvenRefR())->tryInsertItemIntoInventory(buyItem) < 0)
        return 0x4;
    return 0;
}
#include "Dispatchers/process_102.inc"

#include "Dispatchers/read_105.inc"
{
    // ORIG 0x081e0f60：int@0xd → int@0x11
    struct __attribute__((packed)) BuyItemView
    {
        char m_head[0xd];
        int m_field0d;     // +0x0d
        int m_field11;     // +0x11
    };
    BuyItemView& v = reinterpret_cast<BuyItemView&>(msg);
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(&v.m_field0d)))
        return LineFunc(0x559a, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(&v.m_field11)))
        return LineFunc(0x559d, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
#include "Dispatchers/send_20.inc"


#include "Dispatchers/dtor_Dispatcher_MailBox_Extract.inc"
#include "Dispatchers/check_error_34.inc"


#include "Dispatchers/process_103.inc"

#include "Dispatchers/read_106.inc"

#include "Dispatchers/send_21.inc"



#include "Dispatchers/dtor_Dispatcher_PVPRequestFight.inc"
#include "Dispatchers/dispatch_sig_66.inc"

#include "Dispatchers/process_104.inc"
#include "Dispatchers/read_107.inc"

#include "Dispatchers/dtor_Dispatcher_QueryCharacInfo.inc"
#include "Dispatchers/dispatch_sig_67.inc"


#include "Dispatchers/process_105.inc"
#include "Dispatchers/read_108.inc"

#include "Dispatchers/dtor_Dispatcher_ReportAbuseUser.inc"
#include "Dispatchers/dispatch_sig_68.inc"

#include "Dispatchers/process_106.inc"
#include "Dispatchers/read_109.inc"

#include "Dispatchers/dtor_Dispatcher_RequestItemLock.inc"
#include "Dispatchers/dispatch_sig_69.inc"


#include "Dispatchers/process_107.inc"
#include "Dispatchers/read_110.inc"

#include "Dispatchers/dtor_Dispatcher_SaveQuestNotify.inc"
#include "Dispatchers/check_error_35.inc"


#include "Dispatchers/read_111.inc"


#include "Dispatchers/process_108.inc"


#include "Dispatchers/dtor_Dispatcher_SecuDataControl.inc"
#include "Dispatchers/dispatch_sig_70.inc"

#include "Dispatchers/process_109.inc"
#include "Dispatchers/read_112.inc"

#include "Dispatchers/dtor_Dispatcher_Security_IPScan.inc"
#include "Dispatchers/dispatch_sig_71.inc"

#include "Dispatchers/process_110.inc"
#include "Dispatchers/read_113.inc"

#include "Dispatchers/dtor_Dispatcher_Shop_Coin_Event.inc"
#include "Dispatchers/dispatch_sig_72.inc"

#include "Dispatchers/process_111.inc"
#include "Dispatchers/read_114.inc"

#include "Dispatchers/dtor_Dispatcher_TutorialLevelUp.inc"
#include "Dispatchers/process_112.inc"

#include "Dispatchers/read_115.inc"

#include "Dispatchers/dtor_Dispatcher_ApproveJoinGuild.inc"
#include "Dispatchers/process_113.inc"

#include "Dispatchers/read_116.inc"


#include "Dispatchers/dtor_Dispatcher_CallGuildLevelUp.inc"
#include "Dispatchers/dispatch_sig_73.inc"

#include "Dispatchers/process_114.inc"
#include "Dispatchers/read_117.inc"

#include "Dispatchers/dtor_Dispatcher_CallGuildMembers.inc"
#include "Dispatchers/dispatch_sig_74.inc"

#include "Dispatchers/process_115.inc"
#include "Dispatchers/read_118.inc"

#include "Dispatchers/dtor_Dispatcher_ChangeCharacName.inc"
#include "Dispatchers/process_116.inc"
#include "Dispatchers/read_119.inc"

#include "Dispatchers/dtor_Dispatcher_ChangeLetterStat.inc"
#include "Dispatchers/check_error_36.inc"

#include "Dispatchers/process_117.inc"

#include "Dispatchers/read_120.inc"


#include "Dispatchers/send_22.inc"


#include "Dispatchers/dtor_Dispatcher_DieAssaultPlayer.inc"
#include "Dispatchers/dispatch_sig_75.inc"

#include "Dispatchers/process_118.inc"
#include "Dispatchers/read_121.inc"

#include "Dispatchers/dtor_Dispatcher_Equipslot_Switch.inc"
#include "Dispatchers/check_error_37.inc"

#include "Dispatchers/process_119.inc"

#include "Dispatchers/read_122.inc"

#include "Dispatchers/send_23.inc"


// ============================================================================
// NEEDS_DECL: 建议追加到 Dispatchers.h 的声明（body 引用但现有声明缺失）：
//   class Dispatcher_BossDieCheck 公有段：
//     int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
//       // ORIG _ZN23Dispatcher_BossDieCheck11check_errorEP5CUserR8MSG_BASER9ParamBase T 0x081cd71e
//   class Dispatcher_CollectItems 公有段：
//     int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
//       // ORIG _ZN23Dispatcher_CollectItems11check_errorEP5CUserR8MSG_BASER9ParamBase T 0x081e329e
//   class Dispatcher_ComboSkill 公有段：
//     bool checkComboSkillIndex(short idx);   // [R21bool/T1] int→bool（声明在 Dispatchers.h）
//       // ORIG _ZN21Dispatcher_ComboSkill20checkComboSkillIndexEs T 0x0825f292
//
// NEEDS_DECL: 宿主 include 区需追加：
//   #include <algorithm>          // std::find（BossDieCheck::process）
//   #include "CExpandEquipslot.h" // Equipslot_Switch 调用其方法（CUser.h 仅前向）
//   #include "CTimeGate.h"        // CollectItems check_error isOpen（GlobalData.h 仅前向）
//
// 备注：Packet_CollectItems 类定义在本文件 EXTRA 节内（TU 局部，ORIG W
// 0x0822c75a）；若后续其他 TU 需要请迁共享头并移除本文件定义。
// ============================================================================

#include "Dispatchers/dtor_Dispatcher_ExitPrivateStore.inc"
#include "Dispatchers/dispatch_sig_76.inc"

#include "Dispatchers/process_120.inc"
#include "Dispatchers/read_123.inc"

#include "Dispatchers/dtor_Dispatcher_ExpertExtraction.inc"

#include "Dispatchers/dtor_Dispatcher_InformNotice_2nd.inc"
#include "Dispatchers/check_error_38.inc"

#include "Dispatchers/process_121.inc"

#include "Dispatchers/read_124.inc"

#include "Dispatchers/send_24.inc"


#include "Dispatchers/dtor_Dispatcher_MemberEnterReply.inc"
#include "Dispatchers/check_error_39.inc"

#include "Dispatchers/process_122.inc"

#include "Dispatchers/read_125.inc"


#include "Dispatchers/dtor_Dispatcher_Mercenary_Return.inc"
#include "Dispatchers/process_123.inc"


#include "Dispatchers/read_126.inc"
{
    // msg+0xd byte / msg+0x10 int；失败走 LineFunc(0x4e88/0x4e8a, pretty 名)。
    // ORIG 0x081dd904：arg2 直 `movl $<A>,0x4(%esp)`（rodata 地址立即数，非 static 指针二级）。
    // ORIG 0x81dd90a：顶部 char* m→-0xc 槽，两处地址均经槽 `mov -0xc`+add。
    struct MercView          // 自然对齐即达 m_field10@+0x10（0xd+1+2=16 已 4 对齐）
    {
        char m_head[0xd];
        char m_field0d;          // +0x0d byte
        char m_pad0e[2];         // +0x0e..+0x0f
        unsigned int m_field10;  // +0x10
    };
    MercView& m = reinterpret_cast<MercView&>(msg);
    if (!packet.get_byte(m.m_field0d))
        return LineFunc(0x4e88, __PRETTY_FUNCTION__, 0, 0);
    // ORIG 0x81dd9?：`add $0x10,%eax` 单步地址（同一 m 槽现址）。
    if (!packet.get_int(m.m_field10))
        return LineFunc(0x4e8a, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
#include "Dispatchers/send_25.inc"
{
    // ORIG 0x081dda53：param 槽 -0xc 统一基址，字段全单级 disp
    // （`movzbl 0x4(%eax)`/`movzbl 0x5+movsbl`/`mov 0x8(%eax)`）——
    // 裸偏移 char*+0x04 反产 add 两步覆写（技巧14）。+0x04..+0x08 经
    // 本 TU 局部视图具名（PacketTypes.h 冻结，仅 m_pad04[8] 占位）。
    struct MercRetMapView {
        char m_vptr[4];         // +0x00 ParamBase vptr
        unsigned char m_4;      // +0x04 bool 回执
        signed char m_5;        // +0x05 char 回显
        int m_8;                // +0x08
    };
    SendMercenaryReturnResult& result = static_cast<SendMercenaryReturnResult&>(param);
    // [G1Frest 2026-09-10] ORIG 0x081dda53 区：各字段经 `mov -0xc(%ebp),%eax`（result
    //   引用槽）单级 disp 直取（`movzbl 0x4(%eax)`/`mov 0x8(%eax)`）——**无** mv 指针
    //   具名局部（旧码 -0x10 槽 + 帧 0x30）。
    PacketGuard guard;
    guard.put_header(1, 0x1b9);
    guard.put_byte(reinterpret_cast<MercRetMapView*>(&result)->m_4 ? 1 : 0);   // +0x04 bool（ORIG mov$1/jmp/mov$0 岛）
    guard.put_byte(static_cast<unsigned int>(
        reinterpret_cast<MercRetMapView*>(&result)->m_5));                     // +0x05 char 回显
    guard.put_int(reinterpret_cast<MercRetMapView*>(&result)->m_8);            // +0x08 int
    guard.put_int(result.m_result.m_0);                                     // +0x0c
    guard.put_int(result.m_result.m_4);                                     // +0x10
    guard.put_byte(static_cast<unsigned int>(
        static_cast<char>(result.m_result.m_8)));                           // +0x14
    guard.finalize(true);
    user->Send(guard);
}

#include "Dispatchers/dtor_Dispatcher_MultiMailBoxSend.inc"
#include "Dispatchers/CheckTradeLimitItemCountToMove.inc"


#include "Dispatchers/CheckItemTradeLimitCountToMoveByUser.inc"


#include "Dispatchers/dispatch_sig_77.inc"

#include "Dispatchers/process_124.inc"
#include "Dispatchers/read_127.inc"

#include "Dispatchers/dtor_Dispatcher_Open_Guild_Board.inc"
#include "Dispatchers/process_125.inc"
{
    // ORIG 0x081d30d0：①两指针 dead 槽 —— param→-0x20（先声明占远槽）、
    // msg→-0x1c（技巧21）；②页码取自 **msg**+0xb2（与 read() 的 get_byte(&msg+0xb2)
    // 同一字段；旧写法误取 param+0xb2）：`movzbl 0xb2(%eax),%eax; movsbl %al,%edi`
    // 两步入 %edi（技巧15/16 signedness 两步）；③guildkey→%esi、characNo→%ebx
    // 跨 GetServerProxy call 存活（技巧2 register 类型全等直落 callee-saved）。
    struct __attribute__((packed)) BoardMsgView {     // MSG_BASE +0xb2 = 页码
        char m_head[0xb2];
        unsigned char m_page;                        // +0x0b2
    };
    ParamBase* const pp = &param;                     // -0x20
    BoardMsgView* const pm =
        reinterpret_cast<BoardMsgView*>(&msg);        // -0x1c
    (void)pp;
    // P52 单语句形（register asm 绑定违 P0/N96 已删）：R→L 实参求值使
    // (char)pm->m_page 先物化（ORIG 0x15-0x1f movzbl+movsbl 单段入 %edi），
    // guildkey/characNo call 结果跨 GetServerProxy 存活入 callee-saved；
    // GetServerProxy 结果免槽直用，不建 proxy 中转变量（防 dead 槽推移）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendGuildBoardOpen(user->getCurCharacNo(), user->get_charac_guildkey(),
                             static_cast<char>(pm->m_page));
    return 0;
}
#include "Dispatchers/read_128.inc"


#include "Dispatchers/dtor_Dispatcher_PvPBuddyConnList.inc"
#include "Dispatchers/dispatch_sig_78.inc"

#include "Dispatchers/process_126.inc"
#include "Dispatchers/read_129.inc"

#include "Dispatchers/dtor_Dispatcher_RefreshGuildInfo.inc"
#include "Dispatchers/process_127.inc"

#include "Dispatchers/read_130.inc"

#include "Dispatchers/dtor_Dispatcher_ReplyGuildInvite.inc"
#include "Dispatchers/dispatch_sig_79.inc"

#include "Dispatchers/process_128.inc"
#include "Dispatchers/read_131.inc"

#include "Dispatchers/dtor_Dispatcher_ReportBadP2PUser.inc"
#include "Dispatchers/dispatch_sig_80.inc"

#include "Dispatchers/process_129.inc"
#include "Dispatchers/read_132.inc"

#include "Dispatchers/dtor_Dispatcher_Req_Urgent_Quest.inc"
#include "Dispatchers/process_130.inc"

#include "Dispatchers/read_133.inc"
#include "Dispatchers/send_26.inc"


#include "Dispatchers/dtor_Dispatcher_RequestBlackList.inc"
#include "Dispatchers/check_error_40.inc"

#include "Dispatchers/process_131.inc"

#include "Dispatchers/read_134.inc"

#include "Dispatchers/dtor_Dispatcher_RequestJoinGuild.inc"
#include "Dispatchers/process_132.inc"

#include "Dispatchers/read_135.inc"



#include "Dispatchers/dtor_Dispatcher_ResponseCreature.inc"
#include "Dispatchers/dispatch_sig_81.inc"

#include "Dispatchers/process_133.inc"
#include "Dispatchers/read_136.inc"

#include "Dispatchers/dtor_Dispatcher_SDC_Damage_Check.inc"
#include "Dispatchers/dispatch_sig_82.inc"

#include "Dispatchers/process_134.inc"
#include "Dispatchers/read_137.inc"

#include "Dispatchers/dtor_Dispatcher_SaveGameOption_1.inc"
#include "Dispatchers/check_error_41.inc"

#include "Dispatchers/process_135.inc"

#include "Dispatchers/read_138.inc"


#include "Dispatchers/dtor_Dispatcher_SaveGameOption_2.inc"
#include "Dispatchers/check_error_42.inc"

#include "Dispatchers/process_136.inc"

#include "Dispatchers/read_139.inc"


#include "Dispatchers/dtor_Dispatcher_UpgradeChronicle.inc"
#include "Dispatchers/dispatch_sig_83.inc"

#include "Dispatchers/process_137.inc"
#include "Dispatchers/read_140.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeInventory.inc"
#include "Dispatchers/dispatch_sig_84.inc"

#include "Dispatchers/process_138.inc"
#include "Dispatchers/read_141.inc"

#include "Dispatchers/dtor_Dispatcher_AddSocketToAvatar.inc"
#include "Dispatchers/process_139.inc"
#include "Dispatchers/read_142.inc"

#include "Dispatchers/dtor_Dispatcher_AuctionRegistItem.inc"


#include "Dispatchers/dtor_Dispatcher_ChangeHostWarRoom.inc"
#include "Dispatchers/check_error_43.inc"

#include "Dispatchers/process_140.inc"

#include "Dispatchers/read_143.inc"

#include "Dispatchers/send_27.inc"


#include "Dispatchers/dtor_Dispatcher_ConnectP2PAssault.inc"
#include "Dispatchers/dispatch_sig_85.inc"


#include "Dispatchers/process_141.inc"
#include "Dispatchers/read_144.inc"

#include "Dispatchers/dtor_Dispatcher_Debug_Clear_Quest.inc"
#include "Dispatchers/process_142.inc"

#include "Dispatchers/read_145.inc"

#include "Dispatchers/send_28.inc"


#include "Dispatchers/dtor_Dispatcher_DeleteToBlackList.inc"
#include "Dispatchers/check_error_44.inc"

#include "Dispatchers/process_143.inc"

#include "Dispatchers/read_146.inc"


#include "Dispatchers/dtor_Dispatcher_EnterPrivateStore.inc"
#include "Dispatchers/dispatch_sig_86.inc"

#include "Dispatchers/process_144.inc"
#include "Dispatchers/read_147.inc"

#include "Dispatchers/dtor_Dispatcher_FatigueAttendance.inc"

#include "Dispatchers/dtor_Dispatcher_GuildCargoHistory.inc"
#include "Dispatchers/process_145.inc"

#include "Dispatchers/read_148.inc"
#include "Dispatchers/send_29.inc"


#include "Dispatchers/dtor_Dispatcher_GuildCargoPopItem.inc"
#include "Dispatchers/process_146.inc"


#include "Dispatchers/read_149.inc"

#include "Dispatchers/send_30.inc"


#include "Dispatchers/dtor_Dispatcher_LoadingTimeReport.inc"
#include "Dispatchers/dispatch_sig_87.inc"

#undef base
#include "Dispatchers/process_147.inc"
#include "Dispatchers/read_150.inc"

#include "Dispatchers/dtor_Dispatcher_PeerConnectResult.inc"
#include "Dispatchers/dispatch_sig_88.inc"

#include "Dispatchers/process_148.inc"
#include "Dispatchers/read_151.inc"

#include "Dispatchers/dtor_Dispatcher_PutItemForEnchant.inc"
#include "Dispatchers/dispatch_sig_89.inc"

#include "Dispatchers/process_149.inc"
#include "Dispatchers/read_152.inc"

#include "Dispatchers/dtor_Dispatcher_RequestItemUnlock.inc"
#include "Dispatchers/dispatch_sig_90.inc"


#include "Dispatchers/process_150.inc"
#include "Dispatchers/read_153.inc"

#include "Dispatchers/dtor_Dispatcher_SecretShopBuyItem.inc"
#include "Dispatchers/process_151.inc"

#include "Dispatchers/read_154.inc"

#include "Dispatchers/send_31.inc"


#include "Dispatchers/dtor_Dispatcher_SecurityCardIssue.inc"
#include "Dispatchers/dispatch_sig_91.inc"

#include "Dispatchers/process_152.inc"
#include "Dispatchers/read_155.inc"

#include "Dispatchers/dtor_Dispatcher_SetSubGuildMaster.inc"
#include "Dispatchers/dispatch_sig_92.inc"

#include "Dispatchers/process_153.inc"
#include "Dispatchers/read_156.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeGuildCargo.inc"
#include "Dispatchers/dispatch_sig_93.inc"

#include "Dispatchers/process_154.inc"
#include "Dispatchers/read_157.inc"

#include "Dispatchers/Dispatcher_UseVendingMachine.inc"

#include "Dispatchers/dtor_Dispatcher_UseVendingMachine.inc"


#include "Dispatchers/dtor_Dispatcher_AchievementTrigger.inc"
#include "Dispatchers/process_155.inc"

#include "Dispatchers/read_158.inc"


#include "Dispatchers/dtor_Dispatcher_Antibot_DPCallBack.inc"
#include "Dispatchers/dispatch_sig_94.inc"
{
    typedef int (*CheckFn)(void*, int, char*, unsigned short, char*, short*);  // vtable +0x2c
    // [round5 A01 · 轴 B 反向（Main 判据：以 ORIG 符号性为准）] ORIG 0x0820b188/0x0820b196
    //   均为 `movzbl 0x8d252(%eax),%eax` + `cmp $0x4/%0x5,%al; **ja**`（零扩展读 + **无符号**序）
    //   ⇒ 该字节按 unsigned char 访问；旧码 `char&` 有符号 ⇒ 我方产 setl/jl（统计 u/s=1/3
    //   vs ORIG 4/0）。语义等价（`<5` ⇔ `<=4`、无符号/有符号在 0..5 值域同解）。
    // [G5DsigE] 去引用绑定：ORIG 0x0820b188/0x0820b196 均 `movzbl 0x8d252(%eax),%eax`
    //   直读具名成员（引用绑定先物化地址 ⇒ 我方 add+槽往返 4 条/次）。类型视图用显式
    //   unsigned char 转换（ORIG 零扩展读直证）。
    if (static_cast<unsigned char>(user->m_field5) < 5)
    {
        user->m_field5 = static_cast<char>(
            static_cast<unsigned char>(user->m_field5) + 1);
    }

    // [G3 试] P328⑤ 去预清：ORIG 0x0820b1b1 `lea -0x14(%ebp),%eax`（无 `movl $0x0,
    //   -0x14(%ebp)`）；get_int 失败即 return，成功路径必已赋值 ⇒ 删 `= 0`。
    unsigned int value;                                                        // -0x14
    if (!packet.get_int(value))
    {
        if (static_cast<unsigned char>(user->m_field5) < 6)
        {
            user->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)1,
                                            0x25d,
                                            static_cast<unsigned char>(user->m_field5),
                                            0);
        }

        return 0xa76a;
    }
    const int dummySeq = 0xd;                                                  // -0x10
#if !remake_verify
    // [DNF-FIX-DP-PUNISH] 我们自己重新复刻的源码版本环境：DP(清风复刻版 反外挂)回调包体校验必然失败；原码在此判
    // AbnormalUser(2) 并断线，且把读到的垃圾长度继续当有效值解析（可越界）。
    // 改为记录后按“已处理”返回：不断线、不把垃圾值喂给后续解析。（VERIFY 路线保持原判罚）
    user->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)2,
                                    0x25d,
                                    static_cast<unsigned char>(user->m_field5),
                                    static_cast<int>(value));
    return 0;
#endif
    if (value > 0x567)
    {
        if (static_cast<unsigned char>(user->m_field5) < 6)
        {
            user->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)2,
                                            0x25d,
                                            static_cast<unsigned char>(user->m_field5),
                                            static_cast<int>(value));
        }

        return LineFunc(
            0xa780,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    short hashLen = 0xaf0;                                                     // -0x16
    const int index = packet.get_index();
    char* bufPtr = packet.get_buf_ptr(index);                                  // -0xc
    char outBuf[2800];                                                         // -0xb06（ORIG 栈缓冲）
    // [R5a12 ④] 去多一层解引用：ORIG 0x0820b2ba..0x0820b2c4 =
    //   `mov s_antibotChecker_,%eax; mov (%eax),%eax; add $0x2c,%eax; mov (%eax),%esi`
    //   （2 层：vptr → vtable[0x2c/4] 槽值即函数指针）；旧码 `*(CheckFn*)(…)`
    //   把槽值当地址再读一次 ⇒ 3 层（我方旧 .o 0x1f419 后多 `mov (%eax),%eax`）。
    //   形与 PacketDispatcher/TimerDispatcher 同族：单层解引用 + char* 偏移 + 纯
    //   `register` 存储类提示（P231/P307，非 asm 绑定 ⇒ P0 合规）⇒ fn 直落 %esi；
    //   调用 this 实参按 ORIG `0x0820b2d9 mov 0x941f7e4,%eax` 重载全局（P183 去缓存）。
    register CheckFn fn = *reinterpret_cast<CheckFn*>(
        reinterpret_cast<char*>(
            *reinterpret_cast<void**>(GlobalData::s_antibotChecker_)) + 0x2c);
    const int checkRet =
        fn(GlobalData::s_antibotChecker_, user->getAntibotKey(), bufPtr,
           static_cast<unsigned short>(value), outBuf, &hashLen);
    if (checkRet != 0)
    {
        return 0;
    }
    return 0;
}
#include "Dispatchers/process_156.inc"
#include "Dispatchers/read_159.inc"


#include "Dispatchers/dtor_Dispatcher_BiddingRoutingItem.inc"
#include "Dispatchers/dispatch_sig_95.inc"


#include "Dispatchers/process_157.inc"
#include "Dispatchers/read_160.inc"

#include "Dispatchers/dtor_Dispatcher_Cancel_Quick_Party.inc"
#include "Dispatchers/check_error_45.inc"


#include "Dispatchers/process_158.inc"

#include "Dispatchers/read_161.inc"
#include "Dispatchers/send_32.inc"


#include "Dispatchers/dtor_Dispatcher_ChangeRandomOption.inc"
#include "Dispatchers/process_159.inc"

#include "Dispatchers/read_162.inc"


#include "Dispatchers/dtor_Dispatcher_ChangeTutorialFlag.inc"
#include "Dispatchers/dispatch_sig_96.inc"

#include "Dispatchers/process_160.inc"
#include "Dispatchers/read_163.inc"

#include "Dispatchers/dtor_Dispatcher_Change_Charac_Slot.inc"
#include "Dispatchers/dispatch_sig_97.inc"

#include "Dispatchers/process_161.inc"
#include "Dispatchers/read_164.inc"

#include "Dispatchers/dtor_Dispatcher_CharacterStatistic.inc"
#include "Dispatchers/dispatch_sig_98.inc"

#include "Dispatchers/process_162.inc"
#include "Dispatchers/read_165.inc"

#include "Dispatchers/dtor_Dispatcher_CloseDisjointStore.inc"
#include "Dispatchers/check_error_46.inc"


#include "Dispatchers/process_163.inc"

#include "Dispatchers/read_166.inc"
#include "Dispatchers/send_33.inc"


#include "Dispatchers/dtor_Dispatcher_CompatibilityIndex.inc"
#include "Dispatchers/process_164.inc"

#include "Dispatchers/read_167.inc"



#include "Dispatchers/dtor_Dispatcher_CreateAccountCargo.inc"
#include "Dispatchers/dispatch_sig_99.inc"

#include "Dispatchers/process_165.inc"
#include "Dispatchers/read_168.inc"

#include "Dispatchers/dtor_Dispatcher_CreatePrivateStore.inc"
#include "Dispatchers/dispatch_sig_100.inc"

#include "Dispatchers/process_166.inc"
#include "Dispatchers/read_169.inc"

#include "Dispatchers/dtor_Dispatcher_DungeonNPCBuffInfo.inc"
#include "Dispatchers/dispatch_sig_101.inc"
#include "Dispatchers/process_167.inc"
#include "Dispatchers/read_170.inc"

#include "Dispatchers/dtor_Dispatcher_EnterDisjointStore.inc"
#include "Dispatchers/process_168.inc"

#include "Dispatchers/read_171.inc"

#include "Dispatchers/send_34.inc"


#include "Dispatchers/dtor_Dispatcher_FrameLagStatistics.inc"
#include "Dispatchers/process_169.inc"
#include "Dispatchers/read_172.inc"

#include "Dispatchers/dtor_Dispatcher_GuildCargoMoveItem.inc"
#include "Dispatchers/process_170.inc"


#include "Dispatchers/read_173.inc"

#include "Dispatchers/send_35.inc"


#include "Dispatchers/dtor_Dispatcher_GuildCargoPushItem.inc"
#include "Dispatchers/check_error_47.inc"


#include "Dispatchers/process_171.inc"


#include "Dispatchers/read_174.inc"


#include "Dispatchers/send_36.inc"


#include "Dispatchers/dtor_Dispatcher_RemovePrivateStore.inc"
#include "Dispatchers/dispatch_sig_102.inc"

#include "Dispatchers/process_172.inc"
#include "Dispatchers/read_175.inc"

#include "Dispatchers/dtor_Dispatcher_RequestAddPvPBuddy.inc"
#include "Dispatchers/dispatch_sig_103.inc"


#include "Dispatchers/process_173.inc"
#include "Dispatchers/read_176.inc"

#include "Dispatchers/dtor_Dispatcher_RequestGuildSecede.inc"
#include "Dispatchers/dispatch_sig_104.inc"

#include "Dispatchers/process_174.inc"
#include "Dispatchers/read_177.inc"

#include "Dispatchers/dtor_Dispatcher_RequestMemberEnter.inc"
#include "Dispatchers/check_error_48.inc"

#include "Dispatchers/process_175.inc"

#include "Dispatchers/read_178.inc"

#include "Dispatchers/send_37.inc"


#include "Dispatchers/dtor_Dispatcher_Request_Seria_Buff.inc"
#include "Dispatchers/process_176.inc"

#include "Dispatchers/read_179.inc"
#include "Dispatchers/send_38.inc"


#include "Dispatchers/dtor_Dispatcher_Score_Scroll_State.inc"
#include "Dispatchers/dispatch_sig_105.inc"

#include "Dispatchers/process_177.inc"
#include "Dispatchers/read_180.inc"

#include "Dispatchers/dtor_Dispatcher_SecurityCardDisuse.inc"
#include "Dispatchers/dispatch_sig_106.inc"


#include "Dispatchers/process_178.inc"
#include "Dispatchers/read_181.inc"


#include "Dispatchers/dtor_Dispatcher_TitleBookOtherUser.inc"
#include "Dispatchers/process_179.inc"

#include "Dispatchers/read_182.inc"


#include "Dispatchers/dtor_Dispatcher_UnsealRandomOption.inc"
#include "Dispatchers/process_180.inc"
#include "Dispatchers/read_183.inc"

#include "Dispatchers/dtor_Dispatcher_ValidateScriptHash.inc"
#include "Dispatchers/get_hack_type.inc"


#include "Dispatchers/process_181.inc"

#include "Dispatchers/read_184.inc"


#include "Dispatchers/dtor_Dispatcher_AuctionRegistCancel.inc"
#include "Dispatchers/dispatch_sig_107.inc"

#include "Dispatchers/process_182.inc"
#include "Dispatchers/read_185.inc"

#include "Dispatchers/dtor_Dispatcher_BuyPrivateStoreItem.inc"
#include "Dispatchers/dispatch_sig_108.inc"

#include "Dispatchers/process_183.inc"
#include "Dispatchers/read_186.inc"

#include "Dispatchers/dtor_Dispatcher_CallGuildAllMembers.inc"
#include "Dispatchers/dispatch_sig_109.inc"

#include "Dispatchers/process_184.inc"
#include "Dispatchers/read_187.inc"

#include "Dispatchers/dtor_Dispatcher_CompleteLoadAssault.inc"
#include "Dispatchers/dispatch_sig_110.inc"

#include "Dispatchers/process_185.inc"
#include "Dispatchers/read_188.inc"

#include "Dispatchers/dtor_Dispatcher_CreateDisjointStore.inc"
#include "Dispatchers/process_186.inc"
{
    stCreateDisjointStoreParam& out = static_cast<stCreateDisjointStoreParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x29c4,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    // MSG_CREATE_DISJOINT_STORE 冻结头未建模 payload（仅 m_pad[0x100]）；
    // §8.3 TU 局部视图具名（偏移/宽度证据=本函数 ORIG + read get_* 序列）。
    struct DisjointStoreMsgView    // 自然对齐恰落各偏移（0xd+0x103=0x110 已 4 对齐）
    {
        char m_head[0xd];
        char m_storeName[0x103];   // +0x0d（get_str max 0x100）
        int m_field110;            // +0x110 名字长度（get_int）
        int m_field114;            // +0x114
        short m_field118;          // +0x118
        short m_field11a;          // +0x11a
    };
    expert_job::CExpertJob* job = GlobalData::s_ExpertJobMgr->AcquireExpertJob((ENUM_EXPERT_JOB_TYPE)3);
    if (job == 0)
    {
        // ORIG 0x081cfa29: `mov -0x18(%ebp),%eax; movl $0x15,0x4(%eax)` —— 写回 out 引用
        //（原 `*(int*)((char*)&param+4)=…` 会产 `mov 0x14(%ebp),%eax; add $0x4,%eax` 形）。
        out.m_4 = 0x15;
        return 0;
    }
    // ORIG 0x081cfa3d: `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` —— 具名 msg 视图指针占槽
    //（原内联 `reinterpret_cast<...>(&msg)->…` 无槽，ORIG 每次经 -0x10 重载）。
    const DisjointStoreMsgView* view = reinterpret_cast<const DisjointStoreMsgView*>(&msg);
    user->SetCurCharacExpertJob(job);
    // ORIG 0x081cfa55: `movl $0x0,-0xc(%ebp)` —— result 具名量调用前 0 预清。
    int result = 0;
    result = reinterpret_cast<expert_job::CDisjointer*>(job)->OnCreateDisjointStore(
        user,
        view->m_storeName,
        view->m_field114,
        (int)view->m_field118,
        (int)view->m_field11a);
    // [g02 试形A · P-G7b 负结论 2026-09-11] 改成赋值内嵌 `if ((result = f(...)) != 0)`
    //   实测：目标 setne 段确实消除，但引入 `push %esi` + 帧 0x34→0x30 ⇒ gap −1→−2 净劣化
    //   ⇒ 按「劣化即回滚」还原两句写（本件 gap 仅 −1，属近翻正件）。
    // [L7DispG1 2026-09-11 负结论] ORIG 0x081cfad6 `cmpl $0x0,-0xc(%ebp); je`（直跳，无 setne）
    //   ⇒ 目标形是 `if (result)`；实测本处改成隐式真值后本件读数不变（仍 DIFF 99/100），
    //   且它属于「早段函数伪寄存器编号平移」的嫌疑源（见 dispatch_sig_115 判词）⇒ 回滚保基线形。
    if (result != 0)
    {
        user->SetCurCharacExpertJob(0);
        GlobalData::s_ExpertJobMgr->FreeExpertJob(job);
        // ORIG 0x081cfadc: `mov -0xc(%ebp),%edx; mov -0x18(%ebp),%eax; mov %edx,0x4(%eax)`
        out.m_4 = result;
    }
    return 0;
}
#include "Dispatchers/read_189.inc"
{
    // 同上 §8.3 TU 局部视图（冻结头未建模）。
    struct DisjointStoreMsgView    // 同上：自然对齐即达各字段偏移
    {
        char m_head[0xd];
        char m_storeName[0x103];   // +0x0d
        int m_field110;            // +0x110
        int m_field114;            // +0x114
        short m_field118;          // +0x118
        short m_field11a;          // +0x11a
    };
    DisjointStoreMsgView& base = reinterpret_cast<DisjointStoreMsgView&>(msg);
    if (!packet.get_int(base.m_field110))
    {
        return LineFunc(0x2976,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(base.m_storeName, 0x100,
                        base.m_field110))
    {
        return LineFunc(0x2979,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(base.m_field114))
    {
        return LineFunc(0x297c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(base.m_field118))
    {
        return LineFunc(0x297f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(base.m_field11a))
    {
        return LineFunc(0x2982,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

#include "Dispatchers/send_39.inc"


#include "Dispatchers/dtor_Dispatcher_CreatureSendMessage.inc"
#include "Dispatchers/dispatch_sig_111.inc"
{
    // ORIG 0x08204710 [0x08204710, 0x08204b0b]
    // 局部载荷区（-0x157 起）：+0x0d type / +0x0e target / +0x10 uint /
    // +0x14 msg2len / +0x18 msg2[0x1e] / +0x36 msg1len / +0x3a msg1[0x100]
    // §8.3 TU 局部 packed 视图具名（内联视图指针，零新增槽）。
    struct CSMsgView               // 自然对齐；0x36 非对齐 int 用 4 字节区+显式视图
    {
        char m_head[0xd];
        char m_type;               // +0x0d get_byte(char&)/send_chat_msg char 实参
        unsigned short m_target;   // +0x0e
        unsigned int m_field10;    // +0x10
        int m_msg2Len;             // +0x14
        char m_msg2[0x1e];         // +0x18
        unsigned char m_msg1LenB[4];  // +0x36（ORIG 非对齐 int；字节区宽 4，
                                      //  int 视图经 reinterpret 读写同 ORIG movl）
        char m_msg1[0x100];        // +0x3a
    };
    char buf[0x13a];
    // 内联视图（零新增局部槽；VEXP(f) ≡ buf 上具名字段）
#define VEXP(f) (reinterpret_cast<CSMsgView*>(buf)->f)
    if (user->get_state() < 3)
    {
        return 0;
    }
    if (!packet.get_byte(VEXP(m_type)))
    {
        return LineFunc(0x9c3d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(VEXP(m_target)))
    {
        return LineFunc(0x9c3e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(VEXP(m_field10)))
    {
        return LineFunc(0x9c3f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<int*>(VEXP(m_msg1LenB))))
    {
        return LineFunc(0x9c40,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(VEXP(m_msg1), 0x100,
                        *reinterpret_cast<const int*>(VEXP(m_msg1LenB))))
    {
        return LineFunc(0x9c41,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    G_CDataManager()->rewriteUsableEmoticon(*user, VEXP(m_msg1));
    // type 比较按 ORIG movzbl 无符号形（字段为 char，显式 uchar 读保持指令形态）
    if (*reinterpret_cast<const unsigned char*>(&VEXP(m_type)) == 7 ||
        *reinterpret_cast<const unsigned char*>(&VEXP(m_type)) == 1)
    {
        if (!packet.get_int(VEXP(m_msg2Len)))
        {
            return LineFunc(0x9c53,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_str(VEXP(m_msg2), 0x1e, VEXP(m_msg2Len)))
        {
            return LineFunc(0x9c54,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (*reinterpret_cast<const unsigned char*>(&VEXP(m_type)) == 7)
        {
            Packet_Monitor_Other_Channel_Chat chat;   // ctor ORIG 0x0822b8e6（弱内联）
            chat.m_characNo = user->getCurCharacNo();
            chat.m_buddyNameLen = (char)VEXP(m_msg2Len);
            memcpy(chat.buddy_n_user_id_what, VEXP(m_msg2),
                   VEXP(m_msg2Len));
            chat.m_msgLen =
                (unsigned char)*reinterpret_cast<const int*>(VEXP(m_msg1LenB));
            memcpy(chat.m_msg, VEXP(m_msg1),
                   *reinterpret_cast<const int*>(VEXP(m_msg1LenB)));
            chat.packetSize = (unsigned short)(chat.m_msgLen + 0x3b);
            unsigned int size = (unsigned int)chat.packetSize;
            CMonitorServerProxy* proxy =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
            proxy->SendPacket(reinterpret_cast<char*>(&chat), (int)size);
            return 0;
        }
    }
    G_GameWorld()->send_chat_msg(user,
                                 VEXP(m_type),
                                 VEXP(m_target),
                                 VEXP(m_field10),
                                 VEXP(m_msg1),
                                 *reinterpret_cast<int*>(VEXP(m_msg1LenB)),
                                 0, 0, (ENUM_NOTIPACKET)0x72);
    return 0;
}
#undef VEXP
#include "Dispatchers/process_187.inc"
#include "Dispatchers/read_190.inc"

#include "Dispatchers/dtor_Dispatcher_FightVillageMonster.inc"
#include "Dispatchers/dispatch_sig_112.inc"

#include "Dispatchers/process_188.inc"
#include "Dispatchers/read_191.inc"

#include "Dispatchers/dtor_Dispatcher_GuildAttendanceInfo.inc"
#include "Dispatchers/process_189.inc"

#include "Dispatchers/read_192.inc"


#include "Dispatchers/dtor_Dispatcher_GuildMasterDelegate.inc"
#include "Dispatchers/dispatch_sig_113.inc"

#include "Dispatchers/process_190.inc"
#include "Dispatchers/read_193.inc"

#include "Dispatchers/dtor_Dispatcher_New_Gmdebug_Command.inc"
#include "Dispatchers/dispatch_sig_114.inc"

#include "Dispatchers/process_191.inc"
#include "Dispatchers/read_194.inc"

#include "Dispatchers/dtor_Dispatcher_PowerWarProcessInfo.inc"
#include "Dispatchers/dispatch_sig_115.inc"

#include "Dispatchers/process_192.inc"
#include "Dispatchers/read_195.inc"

#include "Dispatchers/dtor_Dispatcher_PvpMissionHPPercent.inc"
#include "Dispatchers/process_193.inc"

#include "Dispatchers/read_196.inc"


#include "Dispatchers/send_40.inc"
{
    // ORIG：&param 基址入槽 -0x10，每次用点 `mov -0x10,%eax` + `lea/add $0x4` 现址；
    // 结构字段视图（技巧14/32）复现 lea 折叠形。
    struct HPParamView { int m_pad0; std::vector<int> m_vals; };
    HPParamView* p = reinterpret_cast<HPParamView*>(&param);
    PacketGuard guard;
    guard.put_header(0, 0x141);
    guard.put_byte((int)p->m_vals.size());
    for (unsigned int i = 0; i < p->m_vals.size(); ++i)
    {
        guard.put_short(p->m_vals[i]);
    }
    guard.finalize(true);
    user->Send(guard);
}


#include "Dispatchers/dtor_Dispatcher_RegisterToBlackList.inc"
#include "Dispatchers/check_error_49.inc"

#include "Dispatchers/_check_unseal_scroll_item.inc"


#include "Dispatchers/dispatch_sig_116.inc"

#include "Dispatchers/read_197.inc"


#include "Dispatchers/dtor_Dispatcher_RequestAssaultPrice.inc"

#include "Dispatchers/dtor_Dispatcher_RequestDisjointItem.inc"
#include "Dispatchers/process_194.inc"

#include "Dispatchers/read_198.inc"

#include "Dispatchers/send_41.inc"


#include "Dispatchers/dtor_Dispatcher_ResponseAddPvPBuddy.inc"
#include "Dispatchers/dispatch_sig_117.inc"


#include "Dispatchers/process_195.inc"
#include "Dispatchers/read_199.inc"

#include "Dispatchers/dtor_Dispatcher_SaveCharacterOption.inc"
#include "Dispatchers/check_error_50.inc"

#include "Dispatchers/process_196.inc"

#include "Dispatchers/read_200.inc"
{
    // ORIG 0x081ce8bc：同 SaveGameOption_1/2 混合形——typed 指针槽 -0xc、
    // get_int 实参 add $0xd、len typed 单级回读。MSG 头 +0xd 无具名字段
    //（PacketTypes.h 冻结），本 TU 局部视图承接。视图须 packed：ORIG 回读形
    // `mov 0xd(%eax),%eax`（+0x0d 起 len），默认对齐会把 m_len 推到 +0x10。
    struct __attribute__((packed)) SaveCharacOptView {
        char m_head[0xd];
        unsigned int m_len;      // +0x0d
    };
    SaveCharacOptView* m = reinterpret_cast<SaveCharacOptView*>(&msg);
    // [P4-p167 符号性] ORIG callee 直证 get_int(unsigned int&)（视图 m_len 已是
    //   unsigned int @+0x0d）——只换 cast 目标类型。
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(m) + 0xd)) ||
        m->m_len >= 0x201)
        return LineFunc(0x2829, __PRETTY_FUNCTION__, 0, 0);
    // [b28d-solo-Dispatchers] P0：ORIG 无 lv/pbuf 槽 ⇒ 删具名局部改内联实参
    //   （同 SGO_2 形，右→左求值先 len 后 buf）。
    // [b28d-solo-Dispatchers] P319：buf 实参整型加法 + 回 cast（探针 probe_gb2 d）。
    if (!packet.get_binary(
            reinterpret_cast<char*>(reinterpret_cast<unsigned int>(m) + 0x11),
            (int)m->m_len))
        return LineFunc(0x282b, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

#include "Dispatchers/dtor_Dispatcher_SecretShopOpenClose.inc"
#include "Dispatchers/process_197.inc"

#include "Dispatchers/read_201.inc"

#include "Dispatchers/send_42.inc"


#include "Dispatchers/dtor_Dispatcher_SecurityCardAuthReq.inc"
#include "Dispatchers/dispatch_sig_118.inc"

#include "Dispatchers/process_198.inc"
#include "Dispatchers/read_202.inc"


#include "Dispatchers/dtor_Dispatcher_SecurityCardCertKey.inc"
#include "Dispatchers/dispatch_sig_119.inc"

#include "Dispatchers/process_199.inc"
#include "Dispatchers/read_203.inc"

#include "Dispatchers/dtor_Dispatcher_UpdateServerMessage.inc"
#include "Dispatchers/dispatch_sig_120.inc"

#include "Dispatchers/process_200.inc"
#include "Dispatchers/read_204.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeAccountCargo.inc"
#include "Dispatchers/dispatch_sig_121.inc"

#include "Dispatchers/process_201.inc"
#include "Dispatchers/read_205.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeItemSeparate.inc"
#include "Dispatchers/check_error_51.inc"
{
    // ORIG 0x081cbe84（报告 Dispatcher_UpgradeItemSeparate/check_error.md 全文已读）：
    // !(state==3 && curCharac!=0) → 7（&& 短路 bool 物化，jne 双正向跳）；
    // msg 指针局部@-0x10（Check 前绑定）；secuResult@-0xc；!=0 直返；
    // CheckItemLock(1,(int)(ushort)(msg+0x1b)) 真 → 0xd5；假 → 0。
    // +0x1b 暂用具名 ushort 视图（MSG_UPGRADE_ITEM_SEPARATE 未建模，已上报 Main）。
    if (!((user->get_state() == 3) && (user->getCurCharacR() != 0)))
    {
        return 7;
    }
    // P80/P99：协议载荷视图必须 pack(1)，否则 ushort 被自然对齐推到 +0x1c。
    struct __attribute__((packed)) ReqView { char m_head[0x1b]; unsigned short m_itemIdx; };
    const ReqView* pReq = reinterpret_cast<const ReqView*>(&msg);   // ORIG -0x10
    int secuResult = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x27);
    if (secuResult != 0)
    {
        return secuResult;
    }
    if (user->CheckItemLock(1, (int)(unsigned short)pReq->m_itemIdx))
    {
        return 0xd5;
    }
    return 0;
}

#include "Dispatchers/process_202.inc"

#include "Dispatchers/read_206.inc"



#include "Dispatchers/dtor_Dispatcher_UseSharedEffectItem.inc"
#include "Dispatchers/dispatch_sig_122.inc"

#include "Dispatchers/process_203.inc"
#include "Dispatchers/read_207.inc"

#include "Dispatchers/dtor_Dispatcher_VerifyCreatureQuest.inc"
#include "Dispatchers/process_204.inc"

#include "Dispatchers/read_208.inc"

#include "Dispatchers/dtor_Dispatcher_WarRoomWpPerMonster.inc"
#include "Dispatchers/check_error_52.inc"

#include "Dispatchers/process_205.inc"

#include "Dispatchers/read_209.inc"

#include "Dispatchers/send_43.inc"


#include "Dispatchers/dtor_Dispatcher_AbnormalUseStackable.inc"
#include "Dispatchers/check_error_53.inc"
#include "Dispatchers/process_206.inc"

#include "Dispatchers/read_210.inc"


#include "Dispatchers/dtor_Dispatcher_AuctionBuyItemApiece.inc"
#include "Dispatchers/dispatch_sig_123.inc"

#include "Dispatchers/process_207.inc"
#include "Dispatchers/read_211.inc"

#include "Dispatchers/dtor_Dispatcher_AuctionMyBiddingInfo.inc"
#include "Dispatchers/dispatch_sig_124.inc"

#include "Dispatchers/process_208.inc"
#include "Dispatchers/read_212.inc"

#include "Dispatchers/dtor_Dispatcher_BuyCeraShopLimitItem.inc"


#include "Dispatchers/dtor_Dispatcher_CallGuildCreateRight.inc"
#include "Dispatchers/dispatch_sig_125.inc"

#include "Dispatchers/process_209.inc"
#include "Dispatchers/read_213.inc"

#include "Dispatchers/dtor_Dispatcher_Check3rdPartyConcent.inc"
#include "Dispatchers/process_210.inc"

#include "Dispatchers/read_214.inc"

#include "Dispatchers/dtor_Dispatcher_CheckGuildNameDouble.inc"
#include "Dispatchers/dispatch_sig_126.inc"

#include "Dispatchers/process_211.inc"
#include "Dispatchers/read_215.inc"

#include "Dispatchers/dtor_Dispatcher_Compound_ExtreamItem.inc"
#include "Dispatchers/_push_newitem.inc"

#include "Dispatchers/_remove_victim.inc"

// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0822577e orig/ours=37/47 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0xc(%ebp),%edx | mov    0x8(%ebp),%eax | movw   $0x0,0x8(%eax,%edx,2)
// [g6 2026-09-11] ORIG 0x0822577e 对 +0x08 起两数组走 scaled 索引直存
//   （`movw $0x0,0x8(%eax,%edx,2)` / `movl $0x0,0x8(%eax,%edx,4)`）；裸
//   `reinterpret_cast<T*>` 指针算术在 4.4 恒折 add/shl 全链（P311 复原：具名
//   成员数组才出 scaled；探针 /tmp/k1sbx/g6/probe/p1.cpp readArr/readNamed）。
// [g6] 全标量面视图：ORIG 对各字段走单条 disp（`movw $0x0,0x6(%eax)` /
//   `movb $0x2,0xcd(%eax)`）；裸 `(char*)this + N` 产 `add $N,%eax; mov (%eax)`（+1/处）。
struct ExtreamItemSlotView
{
    unsigned char m_pad00[4];   // +0x00..+0x03
    unsigned short m_f4;        // +0x04
    unsigned short m_newSlot;   // +0x06
    unsigned short m_slots[2];  // +0x08
    unsigned char m_pad0c[0xc1];// +0x0c..+0xcc
    unsigned char m_cd;         // +0xcd
    unsigned char m_padCe[6];   // +0xce..+0xd3
    unsigned char m_d4;         // +0xd4
};
struct ExtreamItemWordView { unsigned char m_pad08[8]; unsigned int m_words[0x22]; };
#include "Dispatchers/_reset.inc"

#include "Dispatchers/_send_result.inc"


#include "Dispatchers/dispatch_sig_127.inc"

#include "Dispatchers/process_212.inc"
#include "Dispatchers/read_216.inc"

#include "Dispatchers/dtor_Dispatcher_GatheringPartyStatus.inc"
#include "Dispatchers/dispatch_sig_128.inc"
{
    // §6.2 语义完成：P167 调用点差集双零（59/59）；orig531/our490，gap-41 为
    // 后端帧/槽分配冗余（N82 残余），非缺语义 → byte 层 N 封顶。
    // ORIG 0x08203878：组队状态收集进 DB（PacketDispatcher_Impl_1.cpp）。
    // 接收端包体为 pack(1) 结构（sizeof==0x1b4，与 ORIG memset 尺寸一致）：
#pragma pack(push, 1)
    struct RecvTryEntry        // 每行 10 字节
    {
        unsigned char m_a;     // +0x00
        unsigned char m_b;     // +0x01
        unsigned char m_bin[4];// +0x02..0x05
        unsigned int m_val;    // +0x06..0x09
    };
    struct RecvPartyEntry      // 每行 0xd 字节
    {
        unsigned char m_flag;  // +0x00
        unsigned int m_accId;  // +0x01
        unsigned int m_id2;    // +0x05
        unsigned char m_tag[4];// +0x09..0x0c
    };
    struct RecvPkPartyStatus
    {
        char m_head[0xd];                       // 包头占位
        unsigned char m_pkgType;                // +0x0d
        int m_fieldE;                           // +0x0e
        int m_field12;                          // +0x12
        unsigned char m_field16;                // +0x16
        unsigned char m_field17;                // +0x17
        unsigned int m_field18;                 // +0x18
        unsigned short m_tryNum;                // +0x1c（<=0x1e）
        RecvTryEntry m_tryEntries[30];          // +0x1e..0x14a
        unsigned short m_partyNum;              // +0x14a（pvp<=8 普通<=4）
        RecvPartyEntry m_partyEntries[8];       // +0x14c..0x1b4
    };
#pragma pack(pop)

    typedef char _assert_recv_party_status_size[
        (sizeof(RecvPkPartyStatus) == 0x1b4) ? 1 : -1];

    RecvPkPartyStatus recv;
    memset(&recv, 0, sizeof(recv));   // ORIG memset(local_1d4, 0, 0x1b4)

    if (!packet.get_byte(recv.m_pkgType))
        return LineFunc(0x9aeb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(recv.m_fieldE))
        return LineFunc(0x9aef,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(recv.m_field12))
        return LineFunc(0x9af2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(recv.m_field16))
        return LineFunc(0x9af4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(recv.m_field17))
        return LineFunc(0x9af6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    {
        unsigned int field18 = recv.m_field18;
        if (!packet.get_int(field18))
            return LineFunc(0x9af8,
                            __PRETTY_FUNCTION__,
                            0, 0);
        recv.m_field18 = field18;
    }
    if (!packet.get_short(recv.m_tryNum))
        return LineFunc(0x9afa,
                        __PRETTY_FUNCTION__,
                        0, 0);

    if ((unsigned int)recv.m_tryNum > 0x1e)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp",
            __PRETTY_FUNCTION__,
            0x9aff, "recv_pk_party_status.try_num > %d\n", 0x1e);
        return LineFunc(0x9b00,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    for (int i = 0; (int)recv.m_tryNum > i; ++i)
    {
        // ORIG 0x08203f80 段：每次字段访问重发 imul/lea 地址链（P183 禁引用缓存）。
        if (!packet.get_byte(recv.m_tryEntries[i].m_a))    // ORIG 0x9b09
            return LineFunc(0x9b09,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(recv.m_tryEntries[i].m_b))    // ORIG 0x9b0a
            return LineFunc(0x9b0a,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_binary((char*)recv.m_tryEntries[i].m_bin, 4))  // ORIG 0x9b0b
            return LineFunc(0x9b0b,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_int(recv.m_tryEntries[i].m_val))   // ORIG 0x9b0c
            return LineFunc(0x9b0c,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }

    if (!packet.get_short(recv.m_partyNum))
        return LineFunc(0x9b12,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // ORIG 双分支：PVP 通道 cmp 8 + MAX_PVP_MEM（0x9b1c/0x9b1d），
    // 非 PVP 通道 cmp 4 + PARTY_PLAYER_MAX（0x9b24/0x9b25）。
    if (G_GameWorld()->IsPVPChannel())
    {
        if ((unsigned int)recv.m_partyNum > 8)
        {
            LogManager::logFormat(
                1, "PacketDispatcher_Impl_1.cpp",
                __PRETTY_FUNCTION__,
                0x9b1c, "recv_pk_party_status.party_num > MAX_PVP_MEM\n");
            return LineFunc(0x9b1d,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    else
    {
        if ((unsigned int)recv.m_partyNum > 4)
        {
            LogManager::logFormat(
                1, "PacketDispatcher_Impl_1.cpp",
                __PRETTY_FUNCTION__,
                0x9b24, "recv_pk_party_status.party_num > PARTY_PLAYER_MAX\n");
            return LineFunc(0x9b25,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }

    for (int i = 0; (int)recv.m_partyNum > i; ++i)
    {
        if (!packet.get_byte(recv.m_partyEntries[i].m_flag))   // ORIG 0x9b2e
            return LineFunc(0x9b2e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_int(recv.m_partyEntries[i].m_accId))   // ORIG 0x9b36
            return LineFunc(0x9b36,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_int(recv.m_partyEntries[i].m_id2))     // ORIG 0x9b3e
            return LineFunc(0x9b3e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_binary((char*)recv.m_partyEntries[i].m_tag, 4))   // ORIG 0x9b47
            return LineFunc(0x9b47,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }


    Stream* stream =
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x9b4e);
    CStreamGuard guard(stream, true);
    // ORIG：*guard → operator*（0x80c8c46 返回 this）后 .operator<<，
    // 故源码为 **guard << op；GetInBuffer 走 operator->（0x80c8c4e）。
    **guard << (int)0x8e;
    **guard << (int)-1;
    SIG_PARTY_STATUS* sig = guard->GetInBuffer<SIG_PARTY_STATUS>();
    memset(sig, 0, sizeof(SIG_PARTY_STATUS));   // ORIG memset(sig, 0, 0x1c8)

    user->m_network.GetPeerIP2(sig->m_ip, 0x10);
    sig->m_accId = user->get_acc_id();
    sig->m_natType = (int)user->m_network.get_nat_type();
    sig->m_field18 = recv.m_fieldE;
    sig->m_field1c = recv.m_field12;
    sig->m_field20 = recv.m_field16;
    sig->m_field21 = recv.m_field17;
    sig->m_field22 = recv.m_field18;
    sig->m_tryNum = recv.m_tryNum;
    memcpy(sig->m_tryEntries, recv.m_tryEntries, (size_t)recv.m_tryNum * 10u);
    sig->m_partyNum = recv.m_partyNum;
    memcpy(sig->m_partyEntries, recv.m_partyEntries, (size_t)recv.m_partyNum * 0xdu);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
    return 0;
}
#include "Dispatchers/process_213.inc"
#include "Dispatchers/read_217.inc"

#include "Dispatchers/dtor_Dispatcher_ItemHyperLinkMessage.inc"
#include "Dispatchers/check_error_54.inc"


#include "Dispatchers/read_218.inc"


#include "Dispatchers/send_44.inc"


#include "Dispatchers/process_214.inc"

#undef m

#include "Dispatchers/makeRequest_3.inc"


#include "Dispatchers/dtor_Dispatcher_NotifyMessageToGuild.inc"
#include "Dispatchers/dispatch_sig_129.inc"

#include "Dispatchers/process_215.inc"
#include "Dispatchers/read_219.inc"

#include "Dispatchers/dtor_Dispatcher_OperateRidableObject.inc"
#include "Dispatchers/dispatch_sig_130.inc"

#include "Dispatchers/process_216.inc"
#include "Dispatchers/read_220.inc"

#include "Dispatchers/dtor_Dispatcher_PartyTeleportConfirm.inc"
#include "Dispatchers/check_error_55.inc"

#include "Dispatchers/process_217.inc"

#include "Dispatchers/read_221.inc"

#include "Dispatchers/send_45.inc"


#include "Dispatchers/dtor_Dispatcher_PcRoomPlayTimeReward.inc"
#include "Dispatchers/process_218.inc"

#include "Dispatchers/read_222.inc"
#include "Dispatchers/send_46.inc"



#include "Dispatchers/dtor_Dispatcher_PrecheckSoloTeleport.inc"
#include "Dispatchers/process_219.inc"

#include "Dispatchers/read_223.inc"

#include "Dispatchers/send_47.inc"


#include "Dispatchers/dtor_Dispatcher_PvpMissionComboClear.inc"
#include "Dispatchers/process_220.inc"

#include "Dispatchers/read_224.inc"


#include "Dispatchers/dtor_Dispatcher_Register_Quick_Party.inc"
#include "Dispatchers/check_error_56.inc"


#include "Dispatchers/process_221.inc"

#include "Dispatchers/read_225.inc"

#include "Dispatchers/send_48.inc"


#include "Dispatchers/dtor_Dispatcher_ReportMannerlessUser.inc"
#include "Dispatchers/dispatch_sig_131.inc"

#include "Dispatchers/process_222.inc"
#include "Dispatchers/read_226.inc"

#include "Dispatchers/dtor_Dispatcher_Request_PvpExpOfWeek.inc"
#include "Dispatchers/dispatch_sig_132.inc"

#include "Dispatchers/process_223.inc"
#include "Dispatchers/read_227.inc"

#include "Dispatchers/dtor_Dispatcher_Security_ProcessScan.inc"
#include "Dispatchers/dispatch_sig_133.inc"

#include "Dispatchers/process_224.inc"
#include "Dispatchers/read_228.inc"

#include "Dispatchers/dtor_Dispatcher_SeriaRoom_Deco_Event.inc"
#include "Dispatchers/check_error_57.inc"

#include "Dispatchers/process_225.inc"

#include "Dispatchers/read_229.inc"

#include "Dispatchers/send_49.inc"



#include "Dispatchers/dtor_Dispatcher_WriteGuildMemberMemo.inc"
#include "Dispatchers/dispatch_sig_134.inc"

#include "Dispatchers/process_226.inc"
#include "Dispatchers/read_230.inc"


#include "Dispatchers/dtor_Dispatcher_Client_Spec_Statistic.inc"
#include "Dispatchers/send_50.inc"
{
    // [L7DispG1 2026-09-11] ORIG 0x081ce79f `cmpl $0x0,0xc(%ebp); je 81ce8b0` —— 直跳 epilogue
    //   前 nop（0x081ce8b0），非独立 return 块；且 ORIG 尾部只有**一对** `jmp 81ce8b1; nop`
    //   （0x081ce8ae/8b0）⇒ 原码是单出口 `if (user != 0) { 全体 }`。旧形 `if (user==0) return;`
    //   令 4.4 生成 2 对 return 块（实测 ours 尾部 2 对、+2 条）⇒ 改嵌套形。
    if (user != 0)
    {
    // （缩进不重排：保持最小 patch，语义等价于 if (user==0) return;）
    // [G1Frest 2026-09-10] ORIG 0x081ce7a9 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`
    //   ⇒ param 具名指针局部 -0x10（P0 槽往返，非内联 &param）。
    struct ClientSpecParamView
    {
        char m_pad[4];
        unsigned char m_type;   // +0x04
    };
    const ClientSpecParamView* p = reinterpret_cast<const ClientSpecParamView*>(&param);

    char* hwSpec = user->getClientHWSpec();                        // -0xc
    // ORIG 0x081ce7c0/0x081ce7d2：p[4] 两次独立重算（P48/P0）
    //   ⇒ 禁 specType/special 具名缓存（旧码 -0x21/-0x20 两槽为发明）。
    if (hwSpec[0] != 0 &&
        p->m_type != 1 &&
        p->m_type != 0x81)

    {
        return;
    }
    // ORIG 0x081ce7e1 `lea -0xc16(%ebp),%eax; mov %eax,(%esp); call C1`
    //   ⇒ 栈对象直接构造。旧码 `new (stat) Packet_Client_Spec_Statistic()` 在 4.4 展开
    //   _ZnwjPv/_ZdlPvS_/_Unwind_Resume + push %edi + 帧 +0x1c（N83 我方侧形），
    //   而 ORIG 只有一条 call（4.1 前端内联 placement new）。
    Packet_Client_Spec_Statistic stat;
    if (p->m_type == 1 ||
        p->m_type == 0x81)

    {
        stat.m_type = 2;                                           // ORIG 0x081ce805 `movb $0x2,-0xc0c`
    }
    else
    {
        stat.m_type = 0;                                           // ORIG 0x081ce80e `movb $0x0,-0xc0c`
    }
    // ORIG 0x081ce81c `mov %al,-0xc0b` = 写成员 stat+0xb（非独立局部）
    // [L7DispG1] ORIG 0x081ce819 `movzbl 0x1(%eax),%eax`（disp 形）⇒ 需成员访问而非
    //   指针算术（`hwSpec[1]` 产 `add $0x1,%eax; movzbl (%eax),%eax` 两段）。
    struct HwSpecView
    {
        char m_0;               // +0x00
        unsigned char m_1;      // +0x01
    };
    stat.m_count = (unsigned char)reinterpret_cast<HwSpecView*>(hwSpec)->m_1;
    memcpy(&stat.m_specs[0], hwSpec + 4, (unsigned int)stat.m_count * 0xc);
    // ORIG 0x081ce86a `mov %ax,-0xc14` = 写成员 stat.packetSize(+0x2)
    stat.packetSize = (unsigned short)((unsigned int)stat.m_count * 0xc + 0x12);
    // ORIG 0x081ce89e 区：GetServerGroup → GetServerProxy → SendPacket，
    //   无 proxy 判空（旧码 `if (proxy != 0)` 为发明防御）；实参 &stat/packetSize
    //   先求值并经 esi/ebx 跨调用保活 ⇒ 链式调用形。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&stat), (int)stat.packetSize);
    }
}

#include "Dispatchers/process_227.inc"


#include "Dispatchers/dtor_Dispatcher_CreatureScriptMessage.inc"
#include "Dispatchers/dispatch_sig_135.inc"

#include "Dispatchers/process_228.inc"
#include "Dispatchers/read_231.inc"

#include "Dispatchers/dtor_Dispatcher_Mercenary_Competition.inc"
#include "Dispatchers/process_229.inc"


#include "Dispatchers/read_232.inc"
{
    MsgMercenaryCompetition& req = reinterpret_cast<MsgMercenaryCompetition&>(msg);
    // [P139 2026-09-11] 手写串改 __PRETTY_FUNCTION__（同 GuildCargoPushItem::read）。
    // ORIG 单条 `add $0x10`：m_pad(成员+0xd)+3 会被 4.4 拆成两步 add；直接对基址加常数。
    // 成员视图（自然对齐、非 packed，可绑 unsigned int&）：经 req 绑定回读 -0xc + 单 add $0x10。
    // [P4-p167 符号性] ORIG callee 直证 get_int(unsigned int&) ⇒ 字段改 unsigned int。
    struct LenView { char m_head[0x10]; unsigned int m_len; };
    if (!packet.get_int(((LenView*)&req)->m_len))                              // msg+0x10
        return LineFunc(0x4eda, __PRETTY_FUNCTION__, 0, 0);
    // [g02 2026-09-11 · Main 授权 clean-up] `m_pad` 已按具名成员拆分（MsgMercenaryCompetition:
    //   m_field14@+0x14 / m_field15@+0x15）⇒ 原 `req.m_pad[7]/[8]` 成为越界写法。
    //   地址/语义/mangled 全等价（+0xd+7/+0xd+8 = +0x14/+0x15），零 codegen 变化。
    if (!packet.get_byte(req.m_field14))                                      // msg+0x14
        return LineFunc(0x4edc, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(req.m_field15))                                      // msg+0x15
        return LineFunc(0x4ede, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081dde20 orig/ours=83/87 ndiff-blocks=21
//   帧差 sub $20 vs $30 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x20,%esp | lea    -0x18(%ebp),%eax | lea    -0x18(%ebp),%eax
// [G6-send51] P161/P0 视图指针：ORIG 0x081dde22 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`
//   （param 引用槽一次装载）后各字段单 disp（`movzbl 0x4(%eax),%eax` 等）；
//   旧译 `((char*)&param)[k]` 每点重算 `add $0x4/%x8` 两步。
// 自然对齐（勿 pack！m_8 必须落 +0x08：int 前有 3 字节 pad）。
struct MercenaryCompetitionResultView
{
    int m_pad0;
    char m_4;      // +0x04 成功标志
    int m_8;       // +0x08
    char m_c;      // +0x0c
    char m_d;      // +0x0d
};
#include "Dispatchers/send_51.inc"


#include "Dispatchers/dtor_Dispatcher_OpenGuildCreateWindow.inc"
#include "Dispatchers/dispatch_sig_136.inc"

#include "Dispatchers/process_230.inc"
#include "Dispatchers/read_233.inc"

#include "Dispatchers/dtor_Dispatcher_RepairDisjointMachine.inc"
#include "Dispatchers/check_error_58.inc"


#include "Dispatchers/process_231.inc"

#include "Dispatchers/read_234.inc"
#include "Dispatchers/send_52.inc"


#include "Dispatchers/dtor_Dispatcher_AuctionAskAveragePrice.inc"
#include "Dispatchers/dispatch_sig_137.inc"

#include "Dispatchers/process_232.inc"
#include "Dispatchers/read_235.inc"

#include "Dispatchers/dtor_Dispatcher_AuctionSearchByItemKey.inc"
#include "Dispatchers/dispatch_sig_138.inc"

#include "Dispatchers/process_233.inc"
#include "Dispatchers/read_236.inc"

#include "Dispatchers/dtor_Dispatcher_Break_Away_Quest_Check.inc"
#include "Dispatchers/process_234.inc"

#include "Dispatchers/read_237.inc"

#include "Dispatchers/send_53.inc"


#include "Dispatchers/dtor_Dispatcher_ChangeAnotherSkillTree.inc"
#include "Dispatchers/check_error_59.inc"

#include "Dispatchers/process_235.inc"

#include "Dispatchers/read_238.inc"

#include "Dispatchers/send_54.inc"


#include "Dispatchers/dtor_Dispatcher_CharacSlotExtendEffect.inc"
#include "Dispatchers/process_236.inc"

#include "Dispatchers/read_239.inc"
#include "Dispatchers/send_55.inc"



#include "Dispatchers/dtor_Dispatcher_CheckAssaultMotionHack.inc"
#include "Dispatchers/dispatch_sig_139.inc"

#include "Dispatchers/process_237.inc"
#include "Dispatchers/read_240.inc"

#include "Dispatchers/dtor_Dispatcher_ConnectP2PAfterAssault.inc"
#include "Dispatchers/dispatch_sig_140.inc"


#include "Dispatchers/process_238.inc"
#include "Dispatchers/read_241.inc"

#include "Dispatchers/dtor_Dispatcher_DeathTowerStageCommand.inc"
// §8.3 TU 视图：ORIG 08208cb2 `add $0xb58,%eax` → stMapMonsterKillChecker_t::init
// （dispatch_sig.md:150/262-264）。CDeathTower.h（共享）未具名该内嵌成员——其
// m_timerKeys[10]@+0xb3c 占 0xb3c..0xb64 与本区重叠，建模疑似缺陷已 hub 报 Main 核。
// 本 TU 以视图把 +0xb58 表达为具名成员，避免越界改共享头。
namespace {
struct DTChkView {
    char m_head[0xb58];
    WongWork::CDeathTower::stMapMonsterKillChecker_t m_killChecker;   // +0xb58
};
}
#include "Dispatchers/dispatch_sig_141.inc"

#include "Dispatchers/process_239.inc"
#include "Dispatchers/read_242.inc"

#include "Dispatchers/dtor_Dispatcher_Delete_GuildBoard_Text.inc"
#include "Dispatchers/process_240.inc"

#include "Dispatchers/read_243.inc"


#include "Dispatchers/dtor_Dispatcher_DungeonEventStoryPause.inc"
#include "Dispatchers/dispatch_sig_142.inc"

#include "Dispatchers/process_241.inc"
#include "Dispatchers/read_244.inc"

#include "Dispatchers/dtor_Dispatcher_Float_RDATA_Modulation.inc"
#include "Dispatchers/check_error_60.inc"


#include "Dispatchers/process_242.inc"

#include "Dispatchers/read_245.inc"


#include "Dispatchers/dtor_Dispatcher_LogIngameAdvertisement.inc"
#include "Dispatchers/check_error_61.inc"

// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e4df2 orig/ours=67/66 ndiff-blocks=16
//   帧差 sub $4c vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | sub    $0x4c,%esp
// [G6-243] P161/P311 组件视图：ORIG 0x081e4e75/0x081e4e89/0x081e4e8c 为单 disp 具名成员
//   （movzbl 0x10(%eax),%eax / movzwl 0xe(%eax),%eax / movzbl 0xd(%eax),%eax），
//   指针算术写法恒产 add 链 ⇒ 用 pack(1) 视图对齐 +0x0d..+0x11。
#pragma pack(push, 1)
struct LogIngameAdView
{
    char m_pad[0x0d];
    unsigned char m_c0d;      // +0x0d（ORIG movzbl 0xd(%eax),%eax + movsbl %al,%ebx）
    unsigned short m_s0e;     // +0x0e（ORIG movzwl 0xe(%eax),%eax + movswl %ax,%esi）
    unsigned char m_c10;      // +0x10（ORIG movzbl 0x10(%eax),%eax + movsbl %al,%edi）
    unsigned int m_u11;       // +0x11（ORIG mov 0x11(%eax),%eax）
};
#pragma pack(pop)
#include "Dispatchers/process_243.inc"

#include "Dispatchers/read_246.inc"

#include "Dispatchers/send_56.inc"


#include "Dispatchers/dtor_Dispatcher_QueryCharacInfoMailbox.inc"
#include "Dispatchers/dispatch_sig_143.inc"


#include "Dispatchers/process_244.inc"
#include "Dispatchers/read_247.inc"

#include "Dispatchers/dtor_Dispatcher_RequestCharacSkillInfo.inc"
#include "Dispatchers/check_error_62.inc"

#include "Dispatchers/process_245.inc"

#include "Dispatchers/read_248.inc"
{
    // ORIG 0x081e3914：char@0xd；越界（<0 或 >0x23）logFormat(1,...,0x5b20,...) 后 LineFunc(0x5b22)
    // [b28c-34] ORIG 0x081e396a `mov -0xc(%ebp),%eax; movzbl 0xd(%eax),%eax`：
    // 成员视图才折 disp（指针+常量下标 4.4 恒 add $0xd 两段）。
    struct ReqCharacSkillInfoMsgView
    {
        char m_pad[0xd];
        char m_idx;      // +0x0d
    };
    ReqCharacSkillInfoMsgView* v =
        reinterpret_cast<ReqCharacSkillInfoMsgView*>(&msg);
    if (!packet.get_byte(v->m_idx))
        return LineFunc(0x5b1b, __PRETTY_FUNCTION__, 0, 0);
    // ORIG 0x081e396a/0x081e3978 每次重发 disp 读（无 characIdx 槽，P0 表达式直读）。
    if (v->m_idx < 0 || v->m_idx > 0x23)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__, 0x5b20,
                              "RequestCharacSkillInfo ReadPacket ERROR! : characIdx(%d)",
                              (int)v->m_idx);
        return LineFunc(0x5b22, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
#include "Dispatchers/send_57.inc"


#include "Dispatchers/dtor_Dispatcher_SecurityCardAuthCancel.inc"
#include "Dispatchers/dispatch_sig_144.inc"

#include "Dispatchers/process_246.inc"

#include "Dispatchers/read_249.inc"

#include "Dispatchers/dtor_Dispatcher_SecurityCardRetransfer.inc"
#include "Dispatchers/dispatch_sig_145.inc"

#include "Dispatchers/process_247.inc"
#include "Dispatchers/read_250.inc"

#include "Dispatchers/dtor_Dispatcher_TournamentRewardSelect.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeDisjointMachine.inc"
#include "Dispatchers/check_error_63.inc"


#include "Dispatchers/process_248.inc"

#include "Dispatchers/read_251.inc"
#include "Dispatchers/send_58.inc"


#include "Dispatchers/dtor_Dispatcher_VerifyPrivateStoreItem.inc"
#include "Dispatchers/dispatch_sig_146.inc"

#include "Dispatchers/process_249.inc"
#include "Dispatchers/read_252.inc"

#include "Dispatchers/dtor_Dispatcher_AuctionMyAuctionHistory.inc"
#include "Dispatchers/dispatch_sig_147.inc"

#include "Dispatchers/process_250.inc"
#include "Dispatchers/read_253.inc"

#include "Dispatchers/dtor_Dispatcher_Card_Select_Right_State.inc"
#include "Dispatchers/dispatch_sig_148.inc"

#include "Dispatchers/process_251.inc"
#include "Dispatchers/read_254.inc"

#include "Dispatchers/dtor_Dispatcher_CheckGuildAddressDouble.inc"
#include "Dispatchers/dispatch_sig_149.inc"


#include "Dispatchers/process_252.inc"
#include "Dispatchers/read_255.inc"

#include "Dispatchers/dtor_Dispatcher_CompoundItemByExpertJob.inc"
#include "Dispatchers/dispatch_sig_150.inc"

#include "Dispatchers/process_253.inc"
#include "Dispatchers/read_256.inc"

#include "Dispatchers/dtor_Dispatcher_InvestItemAmplifyOption.inc"


#include "Dispatchers/dtor_Dispatcher_RequestItemUnlockCancel.inc"
#include "Dispatchers/dispatch_sig_151.inc"


#include "Dispatchers/process_254.inc"
#include "Dispatchers/read_257.inc"

#include "Dispatchers/dtor_Dispatcher_RequestPCRoomPlayerList.inc"
#include "Dispatchers/dispatch_sig_152.inc"

#include "Dispatchers/process_255.inc"
#include "Dispatchers/read_258.inc"

#include "Dispatchers/dtor_Dispatcher_Write_On_The_GuildBoard.inc"
#include "Dispatchers/process_256.inc"

#include "Dispatchers/read_259.inc"

#include "Dispatchers/send_59.inc"


#include "Dispatchers/dtor_Dispatcher_AuctionSearchByNoItmeKey.inc"
#include "Dispatchers/dispatch_sig_153.inc"

#include "Dispatchers/process_257.inc"
#include "Dispatchers/read_260.inc"

#include "Dispatchers/dtor_Dispatcher_CheckEnableServerMessage.inc"
#include "Dispatchers/dispatch_sig_154.inc"

#include "Dispatchers/process_258.inc"
#include "Dispatchers/read_261.inc"

#include "Dispatchers/dtor_Dispatcher_CompleteLoadAfterAssault.inc"
#include "Dispatchers/dispatch_sig_155.inc"

#include "Dispatchers/process_259.inc"
#include "Dispatchers/read_262.inc"

#include "Dispatchers/dtor_Dispatcher_Enchant_3rdChronicleItem.inc"
// b22 桥清零（P201，Model22）：WongWork::CItemUpgrade::ProcUpgrade 原 extern"C"
// mangled 直连桥转真成员调用。CDataManager.h:3024 权威虚槽声明、定义
// CItemUpgrade.cpp:410；ORIG 0x081d3808 调用即 devirt 直调 call 同 mangled，
// 真调用同形。

#include "Dispatchers/check_error_64.inc"

#include "Dispatchers/process_260.inc"

#include "Dispatchers/read_263.inc"

#include "Dispatchers/send_60.inc"


#include "Dispatchers/dtor_Dispatcher_GoblinPadRequestCryptKey.inc"
#include "Dispatchers/dispatch_sig_156.inc"

#include "Dispatchers/process_261.inc"
#include "Dispatchers/read_264.inc"

#include "Dispatchers/dtor_Dispatcher_GoldTakeIncreasingAmount.inc"
#include "Dispatchers/check_error_65.inc"

#include "Dispatchers/process_262.inc"

#include "Dispatchers/read_265.inc"

#include "Dispatchers/send_61.inc"


#include "Dispatchers/dtor_Dispatcher_LevelupSupportReqestItem.inc"
#include "Dispatchers/dispatch_sig_157.inc"

#include "Dispatchers/process_263.inc"
#include "Dispatchers/read_266.inc"
#include "Dispatchers/send_62.inc"

#include "Dispatchers/dtor_Dispatcher_RequestOnTimeEventReward.inc"
#include "Dispatchers/dispatch_sig_158.inc"

#include "Dispatchers/process_264.inc"
#include "Dispatchers/read_267.inc"

#include "Dispatchers/dtor_Dispatcher_RequestPCRoomPlayerCount.inc"
#include "Dispatchers/dispatch_sig_159.inc"

#include "Dispatchers/process_265.inc"
#include "Dispatchers/read_268.inc"

#include "Dispatchers/dtor_Dispatcher_Security_MemoryIntegrity.inc"
#include "Dispatchers/dispatch_sig_160.inc"

#include "Dispatchers/process_266.inc"
#include "Dispatchers/read_269.inc"

#include "Dispatchers/dtor_Dispatcher_Security_PacketIntegrity.inc"
#include "Dispatchers/dispatch_sig_161.inc"

#include "Dispatchers/process_267.inc"
#include "Dispatchers/read_270.inc"

#include "Dispatchers/dtor_Dispatcher_SelectUltimateDifficulty.inc"
#include "Dispatchers/dispatch_sig_162.inc"

#include "Dispatchers/read_271.inc"

#include "Dispatchers/dtor_Dispatcher_Select_Item_Grwoth_Power.inc"
#include "Dispatchers/check_error_66.inc"


#include "Dispatchers/process_268.inc"


#include "Dispatchers/read_272.inc"


#include "Dispatchers/send_63.inc"


#include "Dispatchers/dtor_Dispatcher_SeriaRoom_Ani_Deco_Event.inc"
#include "Dispatchers/check_error_67.inc"

#include "Dispatchers/read_273.inc"


#include "Dispatchers/dtor_Dispatcher_UpdateContractOfCubeInfo.inc"
#include "Dispatchers/check_error_68.inc"

#include "Dispatchers/process_269.inc"

#include "Dispatchers/read_274.inc"

#include "Dispatchers/send_64.inc"


#include "Dispatchers/dtor_Dispatcher_UseCreatureEvolutionItem.inc"
#include "Dispatchers/dispatch_sig_163.inc"

#include "Dispatchers/process_270.inc"

#include "Dispatchers/process_271.inc"
#include "Dispatchers/read_275.inc"

#include "Dispatchers/dtor_Dispatcher_UseRightOfChangeGrowType.inc"
#include "Dispatchers/check_error_69.inc"

#include "Dispatchers/process_272.inc"
{
    // ORIG：param 引用槽 -0x14 先声明（`mov 0x14(%ebp),%eax; mov %eax,-0x14(%ebp)` 在函数头），
    //   `mov -0x14(%ebp),%edx; mov %eax,0x4(%edx)` 直存参数槽（无 lea ebx、无 msg 引用前置）
    // param 类型只建模了 m_pad[4]，+0x04 结果槽用具名 packed 视图表达（同偏移同宽度）
    struct __attribute__((packed)) RightOfChangeGrowTypeView
    {
        int m_pad0;
        int m_4;   // +0x04
    };
    RightOfChangeGrowTypeView& out =
        reinterpret_cast<RightOfChangeGrowTypeView&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 >= 1)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp",
                              __PRETTY_FUNCTION__,
                              0x93, "Dispatcher Check_Error No(%d)", out.m_4);
        return LineFunc(0x94,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [G3 试] 声明序=槽序（P331）：ORIG `Check` 返回值占 -0x10、msg 引用占 -0xc
    //   （0x0825dd25 `mov %eax,-0x10(%ebp)` / 0x0825dd41 `mov %eax,-0xc(%ebp)`，后者在 je 之后）
    //   ⇒ secu 先声明、mref 后声明（P-L11c 声明移到守护检查之后）。
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x2a);
    if (secu != 0)
    {
        out.m_4 = secu;
        return 0;
    }
    MSG_RIGHT_OF_CHANGE_GROW_TYPE& mref = reinterpret_cast<MSG_RIGHT_OF_CHANGE_GROW_TYPE&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    // ORIG：(int)(char)msg+0xf（movzbl+movsbl），(short)msg+0xd（movzwl+movswl）
    out.m_4 =
        user->ChangeGrowType_Item(
            static_cast<signed char>(mref.m_0f),
            static_cast<short>(mref.m_0d));
    user->_SaveCharacData();
    return 0;
}
#include "Dispatchers/send_65.inc"


#include "Dispatchers/dtor_Dispatcher_AuctionMyRegistedItemInfo.inc"
#include "Dispatchers/dispatch_sig_164.inc"


#include "Dispatchers/process_273.inc"
#include "Dispatchers/read_276.inc"

#include "Dispatchers/dtor_Dispatcher_BloodRoundUiPrepareFinish.inc"
#include "Dispatchers/dispatch_sig_165.inc"

#include "Dispatchers/process_274.inc"
#include "Dispatchers/read_277.inc"

#include "Dispatchers/dtor_Dispatcher_Skill_Command_All_Default.inc"
#include "Dispatchers/process_275.inc"

#include "Dispatchers/read_278.inc"
#include "Dispatchers/send_66.inc"



#include "Dispatchers/dtor_Dispatcher_Skill_Command_Customizing.inc"

#include "Dispatchers/dtor_Dispatcher_Buy_Charac_Status_Using_QP.inc"
#include "Dispatchers/check_error_70.inc"

#include "Dispatchers/process_276.inc"

#include "Dispatchers/read_279.inc"

#include "Dispatchers/send_67.inc"



#include "Dispatchers/dtor_Dispatcher_Change_PartyMemberPosition.inc"
#include "Dispatchers/dispatch_sig_166.inc"

#include "Dispatchers/process_277.inc"
#include "Dispatchers/read_280.inc"

#include "Dispatchers/dtor_Dispatcher_RequestIngameAdvertisement.inc"

#include "Dispatchers/dtor_Dispatcher_CallPartyMemberRealtimeInfo.inc"
#include "Dispatchers/check_error_71.inc"

#include "Dispatchers/process_278.inc"

#include "Dispatchers/read_281.inc"
#include "Dispatchers/send_68.inc"



#include "Dispatchers/dtor_Dispatcher_Direct_Entrance_Quick_Party.inc"
#include "Dispatchers/check_error_72.inc"


#include "Dispatchers/process_279.inc"

#include "Dispatchers/read_282.inc"
#include "Dispatchers/send_69.inc"


#include "Dispatchers/dtor_Dispatcher_Exchange_Random_Item_Reward.inc"
#include "Dispatchers/process_280.inc"
{
    stExchangeRandomItemReward& out = static_cast<stExchangeRandomItemReward&>(param);
    if ((bool)(user->get_state() != 3))

    {
        out.m_4 = 7;
        return 0;
    }
    // [Q4DispatchBig 2026-09-11 探针] 裸 `(char*)&msg + 0xe` 产 `add $0xe,%eax;
    //   movzwl (%eax)` 链；ORIG 0x081de5xx 为 `mov 0x10(%ebp),%eax; movzwl 0xe(%eax)`
    //   （具名 msg 槽 + 单 disp）⇒ 改本 TU 局部视图引用读（物化基址正是 ORIG 形）。
    struct ExchRandMsgView
    {
        char m_pad0e[0xe];
        unsigned short m_eventIdx;          // +0x0e
    } __attribute__((packed));
    const unsigned int eventIdx =
        static_cast<unsigned int>(reinterpret_cast<const ExchRandMsgView&>(msg).m_eventIdx);
    EventClassify::CEventScriptMng* mng =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    EventClassify::CExchangeRandomItemReward* entity =
        static_cast<EventClassify::CExchangeRandomItemReward*>(
            mng->get_event_entity(static_cast<int>(eventIdx)));
    if (entity == 0)
    {
        out.m_4 = 0xb2;
        return 0;
    }
    if (!entity->is_eventing())
    {
        out.m_4 = 0xb2;
        return 0;
    }
    std::vector<std::pair<int, int> > materials;                       // -0x20
    out.m_4 = entity->verifyMaterialItemByRule(
        user,
        reinterpret_cast<MSG_EXCHANGE_RANDOM_ITEM_REWARD&>(msg).m_rewardItems,
        materials);
    if (out.m_4 != 0)
    {
        return 0;
    }
    std::vector<std::pair<int, int> > rewards;                         // -0x2c
    out.m_4 = entity->getRewardItem(rewards);
    if (out.m_4 != 0)
    {
        return 0;
    }
    out.m_4 = entity->deleteUsedMaterialItem(user, materials, out.m_rewards);
    if (out.m_4 != 0)
    {
        return 0;
    }
    out.m_4 = entity->sendRandomItemReward(user, rewards, out.m_items, out.m_9);
    return 0;
}
#include "Dispatchers/read_283.inc"


#include "Dispatchers/send_70.inc"



#include "Dispatchers/dtor_Dispatcher_RequestConditionEventReward.inc"
#include "Dispatchers/dispatch_sig_167.inc"

#include "Dispatchers/process_281.inc"
#include "Dispatchers/read_284.inc"

#include "Dispatchers/dtor_Dispatcher_TournamentRewardSelectState.inc"
#include "Dispatchers/check_error_73.inc"

#include "Dispatchers/process_282.inc"

#include "Dispatchers/read_285.inc"
#include "Dispatchers/send_71.inc"
{
    // [b23S3C] ORIG 0x081dfd74：param 引用槽(-0x14) put_header 后回读判定（P0 无值槽，
    // 去 m_4 hoist；PacketTypes.h 冻结故用局部 overlay）；get_user 每用点重取
    // （ORIG 3 次 call），==0 call 结果直比出 test;sete;test;je（P197④）；f1 升格
    // %ebx + `cmp %ax,%bx`（P197②/P200）。GetPartyIndex movswl%ax,%ebx 已同形保留。
    struct TRSS { ParamBase hdr; int m_4; };
    TRSS& out = *reinterpret_cast<TRSS*>(&param);
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x1ca);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        CParty* party = G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
        if (party != 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (party->get_user(i) == 0)
                {
                    packet.put_byte(-1);
                }
                else if (party->get_user(i)->getCurCharacTotalFatigue() ==
                         party->get_user(i)->getCurCharacTotalMaxFatigue())
                {
                    packet.put_byte(0);
                }
                else
                {
                    packet.put_byte(1);
                }
            }
            packet.finalize(true);
            party->send_to_party(packet);
            party->SetTournamentDungeonClearState((char)0x02);
            party->InsertTournamentDungeonTimer((TIMER_MESSAGE)0x2f);
        }
    }
}


#include "Dispatchers/dtor_Dispatcher_Expand_Equipslot_Flag_Update.inc"
#include "Dispatchers/process_283.inc"

#include "Dispatchers/read_286.inc"

#include "Dispatchers/send_72.inc"


#include "Dispatchers/dtor_Dispatcher_FinishVillageMonsterFighting.inc"
#include "Dispatchers/dispatch_sig_168.inc"

#include "Dispatchers/process_284.inc"
#include "Dispatchers/read_287.inc"

#include "Dispatchers/dtor_Dispatcher_GrowthWeaponEventUseMaterial.inc"
#include "Dispatchers/check_error_74.inc"

#include "Dispatchers/process_285.inc"

#include "Dispatchers/read_288.inc"

#include "Dispatchers/send_73.inc"



#include "Dispatchers/dtor_Dispatcher_HeroMissionEventState_Reward.inc"
#include "Dispatchers/dispatch_sig_169.inc"

#include "Dispatchers/process_286.inc"
#include "Dispatchers/read_289.inc"
#include "Dispatchers/send_74.inc"

#include "Dispatchers/dtor_Dispatcher_SaveGameOption_QuickChatting.inc"
#include "Dispatchers/process_287.inc"

#include "Dispatchers/read_290.inc"
{
    // ORIG 0x081cf0a6：msg typed 指针槽 -0xc、len 无独立局部槽——
    // get_int 实参 add $0xd、比较/传参一律 `mov 0xd(%eax)` 回读（技巧14/32，
    // 同 SGO_1/2 形）。unsigned len 局部会占 -0x10 并破坏重载链。
    // 视图须 packed：ORIG 回读 `mov 0xd(%eax),%eax`，默认对齐会把 m_len 推到 +0x10。
    struct __attribute__((packed)) QuickChatView {
        char m_head[0xd];
        unsigned int m_len;      // +0x0d
        unsigned char m_flag11;  // +0x11（get_byte 目标；MSG_QUICK_CHAT::m_11 同位）
    };
    QuickChatView* m = reinterpret_cast<QuickChatView*>(&msg);
    if (!packet.get_byte(m->m_flag11))
        return LineFunc(0x290a, __PRETTY_FUNCTION__, 0, 0);
    // [p167 2026-09-11] ORIG 调用点为 `_ZN9PacketBuf7get_intERj`（unsigned int&）；
    //   旧 `int*` cast 选到 `get_intERi`（MISSING/EXTRA 各 1）⇒ 取址类型改 unsigned。
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(m) + 0xd)) ||
        m->m_len >= 0x12d)
        return LineFunc(0x290b, __PRETTY_FUNCTION__, 0, 0);
    // [b28d-solo-Dispatchers] P0：ORIG 无 lv/pbuf 槽（值 %edx / buf 地址 %eax）
    //   ⇒ 删具名局部改内联实参。
    // [b28d-solo-Dispatchers] P319：buf 实参整型加法 + 回 cast（探针 probe_gb2 d）。
    if (!packet.get_binary(
            reinterpret_cast<char*>(reinterpret_cast<unsigned int>(m) + 0x12),
            (int)m->m_len))
        return LineFunc(0x290c, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

#include "Dispatchers/dtor_Dispatcher_Avatar_Disjoint_Random_Reward.inc"
#include "Dispatchers/process_288.inc"

#include "Dispatchers/dtor_Dispatcher_SDC_ActiveStatus_Damage_Check.inc"
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081ea16e orig/ours=309/262 ndiff-blocks=42
//   帧差 sub $47c vs $470 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x47c,%esp | movb   $0x0,-0x55(%ebp)
// [Lane1Dispatchers 2026-09-10] 判据①/jcc 目标下标比对 + P312/P311 形态收敛（详细见函数头后注）：
//   ④ 两缓冲前置 `char[1]` 数组死局部（P312 形态，probe1 cPad1 已证 4.4 可复现慢路）
//      ——**本件失效**：本函数局部众多，4.4 把 1/3 字节 pad 归入标量区、大数组仍从
//      4 对齐槽起（我方 -0x268/-0x45c vs ORIG -0x25d/-0x451），慢路未触发；
//      pad 尺寸 1↔3 实测同读数 ⇒ 余 ~58 条按 N78/N82 封顶（4.1 分配器行为）。
//   ⑤ `data` 改 pack(1) 视图指针 ⇒ `data+K`/`data[k]` 全部折成单条 disp 访存
//      （旧 char* + 常量偏移在 4.4 恒发 add 两步，ORIG 单条 `mov 0x3(%eax),%eax`）；
//   ⑥ 流血伤害缩放改直接 C cast（ORIG `fildl/flds/fmulp/fldcw/fistpl` 截断语义），
//      删旧「四舍五入」辅助形（fldz/fucomip/setae 共 9 条 ORIG 无）。
// [SweepDispatchers 2026-09-09] 真语义修复 + 形态收敛（262→269，ORIG 308；p167 双零）：
//   ① **删循环内 `destLen = 500;`**——ORIG 0x081ea373 段无重置（uncompress 后 destLen 保持
//      上次实际长度），旧码每轮重置改变第二次解压行为（真语义差）；
//   ② 补 ORIG -0x3c/-0x38 两个 dead 500 初始化与 srcLen(-0x34) 具名槽；
//   ③ 两缓冲改块内 `char src[500]={0}`（ORIG 为 4.1 内联对齐剥离慢路，我方 4.4 发简单
//      rep stos；P312 pad 试形未把数组顶到非 4 对齐槽 ⇒ 余 39 条按 N78 封顶）。
// pack(1) 视图：ORIG 对 -0x44 数据缓冲的 +0x03/+0x07/+0x25 非对齐访存为单条 disp；
// sizeof 0x29（typedef 校验在下方）。字段宽度按 ORIG movzbl/movsbl 链定。
struct SDCDamageSyncView
{
    unsigned char m_type;        // +0x00
    unsigned char m_event;       // +0x01
    unsigned char m_eventKind;   // +0x02
    int           m_damage;      // +0x03（非对齐）
    int           m_duration;    // +0x07（非对齐）
    unsigned char m_pad[0x1a];   // +0x0b..+0x24
    int           m_cnt;         // +0x25（非对齐）
} __attribute__((packed));
typedef char sdc_view_size_check[(sizeof(SDCDamageSyncView) == 0x29) ? 1 : -1];

#include "Dispatchers/dispatch_sig_170.inc"

#include "Dispatchers/process_289.inc"
#include "Dispatchers/read_291.inc"

#include "Dispatchers/dtor_Dispatcher_police_queryCharacName_useMid.inc"
#include "Dispatchers/dispatch_sig_171.inc"

#include "Dispatchers/process_290.inc"
#include "Dispatchers/read_292.inc"

#include "Dispatchers/dtor_Dispatcher_ImageCommunicationEquipmentUse.inc"
#include "Dispatchers/send_75.inc"


#include "Dispatchers/check_error_75.inc"


#include "Dispatchers/process_291.inc"


#include "Dispatchers/dtor_Dispatcher_Security_NonClient_GetResponse.inc"
#include "Dispatchers/dispatch_sig_172.inc"

#include "Dispatchers/process_292.inc"
#include "Dispatchers/read_293.inc"

#include "Dispatchers/dtor_Dispatcher_Security_PI_HashModulation_C2C.inc"
#include "Dispatchers/dispatch_sig_173.inc"

#include "Dispatchers/process_293.inc"
#include "Dispatchers/read_294.inc"

#include "Dispatchers/dtor_Dispatcher_Security_PI_HashModulation_S2C.inc"
#include "Dispatchers/dispatch_sig_174.inc"

#include "Dispatchers/process_294.inc"
#include "Dispatchers/read_295.inc"

#include "Dispatchers/dtor_Dispatcher_Compound_Equipment_Upgrade_Card.inc"


#include "Dispatchers/dtor_Dispatcher_GrowthWeaponEventChangeInfinity.inc"
#include "Dispatchers/check_error_76.inc"

#include "Dispatchers/process_295.inc"

#include "Dispatchers/read_296.inc"

#include "Dispatchers/send_76.inc"


#include "Dispatchers/dtor_Dispatcher_SaveGameOption_ChattingEmoticon.inc"
#include "Dispatchers/process_296.inc"

#include "Dispatchers/read_297.inc"


#include "Dispatchers/send_77.inc"


#include "Dispatchers/dtor_Dispatcher_ComboSkillExtensionQuickSlotReset.inc"
#include "Dispatchers/check_error_77.inc"

#include "Dispatchers/process_297.inc"

#include "Dispatchers/read_298.inc"

#include "Dispatchers/send_78.inc"


#include "Dispatchers/dtor_Dispatcher_Select_3rdChronicleItem_ForEnchant.inc"
#include "Dispatchers/process_298.inc"

#include "Dispatchers/read_299.inc"

#include "Dispatchers/dtor_Dispatcher_GrowthCreatureChangeInfinityCreature.inc"
#include "Dispatchers/check_error_event.inc"


#include "Dispatchers/process_event.inc"


#include "Dispatchers/check_error_78.inc"


#include "Dispatchers/process_299.inc"

#include "Dispatchers/read_300.inc"
#include "Dispatchers/send_79.inc"


#include "Dispatchers/dtor_Dispatcher_P2PHolePunchingSuccessRateStatistics.inc"
#include "Dispatchers/dispatch_sig_175.inc"
#include "Dispatchers/process_300.inc"

#include "Dispatchers/read_301.inc"

#include "Dispatchers/send_80.inc"

namespace ARAD {
namespace DISPATCHER {

// 声明于 Arad_InternalDispatcher.h（ORIG 0x081977b6；实现在 Arad_InternalDispatcher.cpp）

// 线格式 Sig 结构（ORIG arad 命名空间；本 TU 按 put_binary 字节数定义）。
// [推断] 字段级语义由各 checkWorkState/dispatch_sig 使用方定义；此处仅保证
// 线格式长度（dispatch_sig 读包与 put_binary 一致）。

#include "Dispatchers/Dispatcher_AvartarRoulette.inc"
#include "Dispatchers/dtor_Dispatcher_AvartarRoulette.inc"
#include "Dispatchers/dispatch_sig_176.inc"

#include "Dispatchers/process_301.inc"

#include "Dispatchers/read_302.inc"


#include "Dispatchers/dtor_Dispatcher_AvartarCoinCount.inc"
#include "Dispatchers/checkWorkState.inc"

#include "Dispatchers/dispatch_sig_177.inc"

#include "Dispatchers/process_302.inc"

#include "Dispatchers/read_303.inc"


#include "Dispatchers/Dispatcher_AradAvatarConvert.inc"
#include "Dispatchers/dtor_Dispatcher_AradAvatarConvert.inc"
#include "Dispatchers/dispatch_sig_178.inc"

#include "Dispatchers/process_303.inc"

#include "Dispatchers/read_304.inc"


#include "Dispatchers/Dispatcher_AradEmblemCompound.inc"
#include "Dispatchers/dtor_Dispatcher_AradEmblemCompound.inc"
#include "Dispatchers/dispatch_sig_179.inc"

#include "Dispatchers/process_304.inc"

#include "Dispatchers/read_305.inc"


#include "Dispatchers/Dispatcher_AradJumpingCharacter.inc"
#include "Dispatchers/dtor_Dispatcher_AradJumpingCharacter.inc"
#include "Dispatchers/dispatch_sig_180.inc"

#include "Dispatchers/process_305.inc"

#include "Dispatchers/read_306.inc"


#include "Dispatchers/Dispatcher_AradUseAvatarRecharge.inc"
#include "Dispatchers/dtor_Dispatcher_AradUseAvatarRecharge.inc"
#include "Dispatchers/dispatch_sig_181.inc"

#include "Dispatchers/process_306.inc"

#include "Dispatchers/read_307.inc"


#include "Dispatchers/dtor_Dispatcher_EventCreateDnfRequest.inc"
#include "Dispatchers/dispatch_sig_182.inc"

#include "Dispatchers/process_307.inc"

#include "Dispatchers/read_308.inc"


#include "Dispatchers/Dispatcher_AvartarHiddenOptionChange.inc"
#include "Dispatchers/dtor_Dispatcher_AvartarHiddenOptionChange.inc"
#include "Dispatchers/dispatch_sig_183.inc"

#include "Dispatchers/process_308.inc"

#include "Dispatchers/read_309.inc"


#include "Dispatchers/dtor_Dispatcher_RequestPrcoomDayilyReward.inc"
#include "Dispatchers/dispatch_sig_184.inc"

#include "Dispatchers/process_309.inc"
{
    return 0;
}
#include "Dispatchers/read_310.inc"
{
    return 0;
}

}
}
namespace ARAD {
// W7_BuildHarmonize 去重：Arad_Dispatcher_Login_Impl 生命周期唯一 owner 为
// ARAD_LoginSecurity.cpp（含 base(1) 初始化）。
}  // namespace ARAD


namespace APSystem {
#include "Dispatchers/dtor_Dispatcher_ClearAction.inc"
#include "Dispatchers/dispatch_sig_185.inc"

#include "Dispatchers/process_310.inc"
#include "Dispatchers/read_311.inc"

#include "Dispatchers/dtor_Dispatcher_GetTodayReward.inc"
#include "Dispatchers/dispatch_sig_186.inc"
{
    // ORIG 0x081251da：state>2 且 curCharacR 非空才受理（否则 0x287）；
    // get_binary 5 字节 → GetTodayRewardAndSendPostal(reward=int@+1)
    // 同 ClearAction：三条件直分支 + %al 常量岛 ⇒ 条件表达式直用（P226）。
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
        return 0x287;
    // ORIG 0x0812520x：buf@-0xd，reward 取值走 `mov -0xc(%ebp),%eax`（+1 处 4 字节
    // 按值传）——用 packed 视图把「buf+1 解引用」变成字段访问，消 lea+add+mov 三步。
    struct __attribute__((packed)) RewardView {
        unsigned char m_head;    // +0x00
        _SIG_LOAD_ACTION_POINT::_TodayRewardItem m_reward;   // +0x01
    };
    RewardView buf;
    if (!packet.get_binary(reinterpret_cast<char*>(&buf), 5))
        return 0x28b;
    CUserProc::GetTodayRewardAndSendPostal(
        user, buf.m_reward);
    return 0;
}
#include "Dispatchers/process_311.inc"
#include "Dispatchers/read_312.inc"

}


// ══════════════ 

// ══════════════ 批次 B1：10 类桩真实化落盘区（DispatchersRestore） ══════════════

/* ==================== AvatarOptionChange_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_AvatarOptionChange 桩真实化（批次 disp_b1）
// 依据 docs/class_func_reports/Dispatcher_AvatarOptionChange/{read,process,check_error,send}.md
//
// 宿主落盘注意（Dispatchers.cpp）：
//   1. 下方"局部线格式视图"为文件级一次性定义，置于类实现之前（匿名 namespace，
//      与 altar_pkt 模式一致），不要随函数重复粘贴。
//   2. Dispatchers.cpp 现有 include 已含 CDataManager.h/CInventory.h/CUserCharacInfo.h/
//      GlobalData.h/CompoundEmblemDeps.h(LineFunc)；还需补
//      #include "CStackableItem.h"、#include "CEquipItem.h"（find_item 结果
//      downcast 与 IsAvatarOptionValid 需要完整类型）。
//   3. PacketTypes.h 现有 MSG_CHANGE_AVATAR_OPTION 字段化（+0xd pad/+0xe u16/+0x10 int/
//      +0x14 u16/+0x16 int/+0x1a u8）与 ORIG 报告偏移（+0xd short/+0xf int/+0x13 short/
//      +0x15 int/+0x19 byte）错位 1 字节，本批不改共享头，统一经视图按报告偏移访问；
//      stChangeAvatarOption 仍为 m_pad[0x40]，同样经视图访问。
// ============================================================================

// ==== 前置：局部线格式视图（宿主一次性置入匿名 namespace） ====
namespace {

// ORIG read(0x081e04f2) 线格式：
//   +0x0d short 消耗品槽位（0<=v<=0x137）、+0x0f int 消耗品 itemIdx（>=0）、
//   +0x13 short 目标 avatar 装备槽位（0<=v<=0x68）、+0x15 int 目标装备 itemIdx（>=0）、
//   +0x19 byte 目标 avatar option 值（bit7==0）。
#pragma pack(push, 1)
struct AocMsgView
{
    char m_head[0xd];            // MSG_BASE（0xd 字节）
    short m_consumeSlot;         // +0x0d 消耗品槽位
    int m_consumeItemIdx;        // +0x0f 消耗品 item idx
    short m_avatarSlot;          // +0x13 目标 avatar 装备槽位
    int m_equipItemIdx;          // +0x15 目标装备 item idx
    char m_option;               // +0x19 目标 avatar option 值（get_byte(char&)）
};
#pragma pack(pop)

// stChangeAvatarOption(ParamBase) 视图：process 写 / send 读。
struct AocParamView
{
    char m_pad0[4];
    int m_errorCode;             // +0x04 check_error 结果
    short m_consumeSlot;         // +0x08
    short m_avatarSlot;          // +0x0a
    unsigned char m_option;      // +0x0c
};

}  // namespace

#include "Dispatchers/read_313.inc"


#include "Dispatchers/check_error_79.inc"

#undef m

#include "Dispatchers/process_312.inc"


#include "Dispatchers/send_81.inc"


/* ==================== BlueMarbleConfirmInfo_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_BlueMarbleConfirmInfo 四函数真实化
// ORIG: check_error 0x081e68c8 / read 0x081e6242 / process 0x081e62da /
//       send 0x081e6904
//
// 宿主落盘注意（Dispatchers.cpp 已有 include 集合核对）：
//   - BlueMarble.h / CGameManager.h(G_CGameManager/getBlueMarble(int)) /
//     CInventory.h(NumberToString 声明) / GlobalData.h：已 include ✓
//   - CEnvironment.h：需补 include（G_CEnvironment()->get_file_name）。
//   - CMyFileLog.h：需补 include（CMyFileLog ctor/operator() 完整类型）。
//   - <cstdio>：需补 include（sprintf；仓库先例 BlueMarble.cpp 同型用法）。
//
// Param：模板第二参 stBlueMarbleConfirmInfo（PacketTypes.h:1720，具名字段
// m_4=+0x04 错误码）。process 不写其余字段，send 仅读 m_4 —— 无偏移访问。
//
// check_error 以汇编为准：Ghidra 反编译把 user 误写为 param_2
// （cmpl $0x0,0xc(%ebp) 检查的是栈上 user 实参），已修正。
// ============================================================================

#include "Dispatchers/check_error_80.inc"


#include "Dispatchers/read_314.inc"


#include "Dispatchers/process_313.inc"


#include "Dispatchers/send_82.inc"


/* ==================== MailBox_Send_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_MailBox_Send —— 还原 ORIG 0x081cc3da..0x081ccdf9（check_error/
// GuildMailCheck/process/read/send）。依据 docs/class_func_reports/
// Dispatcher_MailBox_Send/{read,check_error,GuildMailCheck,process,send}.md。
//
// 宿主落盘注意：
// 1. Dispatcher_MailBox_Send::GuildMailCheck 当前未在 Dispatchers.h 声明，
//    需在类内补 `int GuildMailCheck(CUser* user, MSG_BASE& msg);`。
// 2. PacketTypes.h 的 MSG_MAILBOX_SEND 目前为 char m_pad[0x100]（sizeof 0x10d），
//    而 ORIG 消息布局至少用到 +0x144（m_45[0x100] 文本区）。落盘前必须把
//    m_pad 扩容到 ≥0x141（总长 ≥0x14d），否则下列偏移访问越界。字段语义：
//      +0x0d int   收件人名长度（read 首个 get_int）
//      +0x11 char[0x1d] 收件人名
//      +0x2e unsigned int 邮件金币
//      +0x32 unsigned int 公会邮键（非 0 走公会邮件）
//      +0x36 unsigned char 附件物品空间（GetInvenTypeFromItemSpace 入参）
//      +0x37 unsigned short 附件槽位
//      +0x39 unsigned long 附件物品 id（find_item 入参；32 位 4 字节）
//      +0x3d int 附件附加字段（read 读入、本批函数未消费）
//      +0x41 int 标题长度
//      +0x45 char[0x100] 邮件文本
// 3. BLOCKED 调用（ORIG 真实符号存在、仓库尚无声明，保留占位不影响其余逻辑）：
//      CheckItemTradeLimitCountToMoveByUser(CUser&,int,int,bool&) @0x0812032e
//      CUser::reqSendMailCertify()                                @0x0868a51a
//      CUser::set_MailMsgData(MSG_MAILBOX_SEND*)                  @0x0868a4b6
//      CUser::IncrementTradeGold(unsigned int,int)                @0x0864658a
// ============================================================================


                                     // GlobalData::s_pSecuProtectionField

namespace  // 本 TU 内的偏移视图（MSG_MAILBOX_SEND 字段化前的一致访问点）
{

inline int& mbLen(MSG_BASE& m) { return reinterpret_cast<MSG_MAILBOX_SEND&>(m).m_nameLenD; }                     // +0x0d 收件人名长
inline char* mbName(MSG_BASE& m) { return reinterpret_cast<char*>(&m) + 0x11; }                                 // +0x11 收件人名[0x1d]
inline unsigned int& mbGold(MSG_BASE& m) { return reinterpret_cast<MSG_MAILBOX_SEND&>(m).m_gold2e; }             // +0x2e 金币
inline unsigned int& mbGuildKey(MSG_BASE& m) { return reinterpret_cast<MSG_MAILBOX_SEND&>(m).m_guildKey32; }     // +0x32 公会邮键
inline unsigned char& mbSpace(MSG_BASE& m) { return reinterpret_cast<MSG_MAILBOX_SEND&>(m).m_space36; }          // +0x36 物品空间
inline unsigned short& mbSlot(MSG_BASE& m) { return reinterpret_cast<MSG_MAILBOX_SEND&>(m).m_slot37; }           // +0x37 槽位
inline unsigned long& mbItemId(MSG_BASE& m) { return reinterpret_cast<MSG_MAILBOX_SEND&>(m).m_itemId39; }        // +0x39 物品 id
inline int& mbTitleLen(MSG_BASE& m) { return reinterpret_cast<MSG_MAILBOX_SEND&>(m).m_titleLen41; }              // +0x41 标题长
inline char* mbText(MSG_BASE& m) { return reinterpret_cast<char*>(&m) + 0x45; }                                 // +0x45 文本[0x100]
inline int& mbExtra(MSG_BASE& m) { return reinterpret_cast<MSG_MAILBOX_SEND&>(m).m_extra3d; }                    // +0x3d 附件附加字段

}  // namespace

#include "Dispatchers/read_315.inc"


#include "Dispatchers/GuildMailCheck.inc"


#include "Dispatchers/check_error_81.inc"


#include "Dispatchers/process_314.inc"


#include "Dispatchers/send_83.inc"


/* ==================== MonstercardBind_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_MonstercardBind 桩真实化 —— 函数体交付
// 目标落盘：source/DNFServer/GameServer/Game/Dispatchers.cpp 对应类段
// （替换 2344-2349 行的空 ctor/dtor/桩；dtor ORIG 为基类转发，保持 {}）
//
// 头约束说明（宿主知悉）：
//  - Dispatchers.h 的 Dispatcher_MonstercardBind 仅声明了 ctor/~dtor/
//    check_error/process/read/send，未声明私有 helper
//    (_bind_process/calcurate_rarity/check_need_slot/get_bind_result)、
//    ENUM_MONSTER_CARD_BIND_TYPE 与 m_rand(+0x04) 成员。
//    按任务规则，helper 逻辑以报告为准【内联】进 check_error/process，
//   并在各内联点注明 ORIG 地址。
//  - ORIG ctor (W 0x08595348) 于 this+0x04 new CMTRand(time(0))；因头无成员，
//    以本文件匿名命名空间 holder 承载同一随机数实例（种子与生命周期一致）。
//    若后续头补出 `CMTRand* m_rand; // +0x04` 与 4 个私有方法声明，可将各
//    内联段还原为独立成员定义（ORIG 符号均为 T，见下各节地址注释）。
//  - 依赖符号均已 nm/grep 核实存在：CUserCharacInfo::getCurCharacInvenR(W
//    0x080da27e)、CInventory::GetInvenSlot(int,int) const(0x084fb918)、
//    CDataManager::find_item(0x0835fa32)、CItem::is_stackable/get_rarity(W)、
//    CInventory::get_empty_slot(0x084fb824)/delete_item(0x0850400c)/
//    tryInsertEventItems(0x0850675e)/insert_event_items(0x08506bb2)/setCopy、
//    GameWorld::IsPVPChannel、G_GameWorld、G_CDataManager、LineFunc、
//    CSecu_ProtectionField::CheckItemRarity(0x082887d0)、CMTRand::randInt
//    (0x080cbabe)、STExpertJobScript 字段(m_bindListCnt/m_bindList/
//    m_bindOnlyCnt/m_bindOnly/m_monsterCardBindRateMap)。
// ============================================================================

// （宿主 TU 已含：Dispatchers.h/CDataManager.h/CInventory.h/GameWorld.h/
//   GlobalData.h/CUserCharacInfo.h/PvPTypes.h(CMTRand)/STExpertJobScript.h/
//   CompoundEmblemDeps.h(LineFunc)；cMyTrace 经 GameTypes.h 可见。）

namespace
{

// ORIG Dispatcher_MonstercardBind ctor @0x08595348 (W)：实例级 CMTRand 存
// this+0x04（Dispatchers.h 已补 m_rand 成员）。

} // anonymous namespace

#include "Dispatchers/Dispatcher_MonstercardBind.inc"


#include "Dispatchers/dtor_Dispatcher_MonstercardBind.inc"


#include "Dispatchers/check_need_slot.inc"


#include "Dispatchers/calcurate_rarity.inc"


#include "Dispatchers/get_bind_result.inc"


#include "Dispatchers/_bind_process.inc"


// ==== Dispatcher_MonstercardBind::read (ORIG 0x081d82e8 T) ====

#include "Dispatchers/read_316.inc"


#include "Dispatchers/check_error_82.inc"


#include "Dispatchers/process_315.inc"


#include "Dispatchers/send_84.inc"


/* ==================== PartyTeleport_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_PartyTeleport 四函数真实化
// ORIG: check_error 0x081dc19a / read 0x081dbe70 / process 0x081dbfd6 /
//       send 0x081dc426
//
// 宿主落盘注意（Dispatchers.cpp 已有 include 集合核对）：
//   - GameWorld.h / GlobalData.h / CInventory.h：已 include ✓
//   - Village.h：需补 include（Village::get_gate_area 完整调用，先例
//     CPacketRespondent.cpp:15）。
//   - CPartyTelePort.h：需补 include（CPartyTelePort 成员调用；PacketTypes.h 仅
//     借其 stPartyTelePort(大写 P) 内嵌类型，不引入类本体）。
//   - CGuildServerProxy.h：需补 include（CEventManager::GetRepeatEvent /
//     CEventBase::IsEventing 完整类型）。
//   - CSystemTime.h：需补 include（s_systemTime_.getCurSec() 完整类型）。
//
// Param 视图：模板第二参 stPartyTeleport（PacketTypes.h:2308，ParamBase 派生，
// char m_pad[0x40] 占位）。ORIG process/send 报告实证布局：
//   +0x04 int 错误码、+0x08 byte 回显(msg+0xd)、+0x0c int 光环剩余秒。
// 头未字段化 → 按约束以 reinterpret_cast 偏移访问并注明。
// ============================================================================

#include "Dispatchers/check_error_83.inc"


#include "Dispatchers/read_317.inc"


#include "Dispatchers/process_316.inc"
{
    // [G3 试] P0/P48 去值槽 hoist：ORIG 0x081dbfd6 全程 `mov -0x2c(%ebp),%edx;
    //   mov %eax,0x4(%edx)` / `mov -0x2c(%ebp),%eax; mov %dl,0x8(%eax)` /
    //   `mov -0x2c(%ebp),%edx; mov %eax,0xc(%edx)` —— 只有 param 指针槽（-0x2c）
    //   + 常量 disp，**无** err/echoState/auraRemain 引用槽。旧形 3 个引用变量 ⇒
    //   5 槽、+6 mov +3 add（首差即 `mov %eax,-0x38` 起的 10 条）。
    struct PartyTeleportParamView
    {
        int m_pad0;                // +0x00
        int m_4;                   // +0x04 错误码
        unsigned char m_8;         // +0x08 回显（← msg+0x0d）
        char m_pad9[3];            // +0x09..+0x0b
        int m_c;                   // +0x0c 光环剩余秒
    };
    PartyTeleportParamView* const out =
        reinterpret_cast<PartyTeleportParamView*>(&param);

    out->m_4 = check_error(user, msg);
    // [G4-316 2026-09-11 · P0/P339] ORIG 0x081dc004 `mov 0x10(%ebp),%eax;
    //   mov %eax,-0x28(%ebp)` 位于 0x081dbff9 `call check_error` **之后** ⇒ msg 引用槽
    //   物化点即声明位，声明须在 check_error 调用之后。
    MSG_PARTY_TELEPORT& req = reinterpret_cast<MSG_PARTY_TELEPORT&>(msg);
    out->m_8 = req.m_field0d;
    out->m_c = user->get_aura_avatar_option_value(0);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    if (curSec < out->m_c)
    {
        out->m_c = out->m_c - curSec;
    }
    else
    {
        out->m_c = 0;
    }

    if (out->m_4 > 0)
    {
        return 0;
    }
    if (out->m_4 < 0)
    {
        return LineFunc(19260,
                        __PRETTY_FUNCTION__,
                        out->m_4, 0);
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        out->m_4 = 0x13;
        return 0;
    }
    // ORIG：每次操作重取 GetPartyTelePort（P48 无 CSE）。
    static_cast<CPartyTelePort*>(party->GetPartyTelePort())
        ->set_teleport_data(req.m_field0d, req.m_field0e,
                            static_cast<short>(req.m_field0f),
                            static_cast<short>(req.m_field11),
                            req.m_field13, '\0');
    int slot = static_cast<int>(party->GetMemberSlotNo(user));
    static_cast<CPartyTelePort*>(party->GetPartyTelePort())
        ->set_teleport_member_state(slot, 1);
    static_cast<CPartyTelePort*>(party->GetPartyTelePort())
        ->check_cur_teleport_state();
    return 0;
}

#include "Dispatchers/send_85.inc"
{
    // [b23S3C] ORIG 0x081dc426：param 指针槽物化(-0x10)，m_4/m_state/m_c 各用点
    // 回读（P48 去值槽 hoist，ORIG 无 err/echoState/auraRemain 值槽 P0 律）；
    // GetParty 单取缓存、GetPartyTelePort 每子调用重取（ORIG 3 次 call 实证）；
    // put_byte((uchar)m_4) 复现 ORIG `mov 0x4(%eax),%eax; movzbl %al,%eax` 两段。
    struct StPT { ParamBase hdr; int m_4; unsigned char m_state; char gap[3]; int m_c; };
    StPT* const p = reinterpret_cast<StPT*>(&param);
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x1b3);                         // class=1 cmd=0x1b3
    if (p->m_4 == 0)
    {
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte((unsigned char)p->m_4);
        packet.put_byte(p->m_state);
        packet.put_int(p->m_c);
    }
    packet.finalize(true);
    user->Send(packet);
    if (p->m_4 == 0)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->send_teleport_effect_at_different_place(3);
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->send_teleport_status(0);
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->process_teleport();
        }
    }
}

/* ==================== PcRoomRentItem_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_PcRoomRentItem 四函数 + giveDimensionItems 真实化
// ORIG: check_error 0x081e5762 / read 0x081e512e / process 0x081e52c0 /
//       send 0x081e542e / giveDimensionItems 0x081e583e
//
// 宿主落盘注意（Dispatchers.cpp 已有 include 集合核对）：
//   - InstanceRentalSystem.h：经 PacketTypes.h（stRentItemParam::m_output）可见 ✓
//   - pc_room::GetSelectionIndex 定义在 GlobalData.cpp（T 0x082711f7），头未暴露，
//     本文件给出局部声明（与 _NS_IPScan 先例同型）。
//   - Dispatchers.h 的 Dispatcher_PcRoomRentItem 类内缺 giveDimensionItems 声明，
//     落盘时需补一行私有声明：
//         void giveDimensionItems(CUser* user, InstanceRentalSystem::RentInput& input);
//   - CGuildServerProxy.h：需补 include（check_error 经
//     s_event_manager->GetRepeatEvent(0x5e)->IsEventing 调用，需 CEventManager/
//     CEventBase 完整类型；先例 BattleDepsB2.cpp:41）。

// pc_room::GetSelectionIndex —— T 0x082711f7，定义于 GlobalData.cpp namespace pc_room。
namespace pc_room
{
#include "Dispatchers/GetSelectionIndex.inc"
}

#include "Dispatchers/check_error_84.inc"


#include "Dispatchers/read_318.inc"


#include "Dispatchers/process_317.inc"


#include "Dispatchers/giveDimensionItems.inc"


#include "Dispatchers/send_86.inc"


/* ==================== PurifyItem_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_PurifyItem 桩真实化 —— 函数体交付
// 目标落盘：source/DNFServer/GameServer/Game/Dispatchers.cpp 对应类段
// （替换 1504-1507 行空 ctor/dtor/桩；dtor ORIG 为基类转发，保持 {}）
//
// 头约束说明（宿主知悉）：
//  - Dispatchers.h 的 Dispatcher_PurifyItem 仅声明 ctor/~dtor/process/read，
//    未声明 processPurifyType —— 按任务规则将其逻辑内联进 process 并注明
//    （ORIG T 0x082194b6）。
//  - CItemAmplifier::getPurifyMaterialType/getPurifyMaterialItemIdx/
//    getPurifyMaterialItemCount（ORIG W 0x08234d20/0x08234cd4/0x08234cfa）
//    已由 OrigAmplifier15 在 CItemAmplifier.h/.cpp 落地为真实成员函数
//    （强符号 T，具名字段表查找），process 内直接以成员方式调用。
//  - stAmplifyOption_t::identify 头声明为 void，ORIG W 0x0822b3d8 实返 bool
//    （P3 收窄法）：processPurifyType 内联处按指令语义展开取返回值。
//  - 异常抛掷按 P6：throw static_cast<ENUM_ERROR>(N)，catch(ENUM_ERROR&)
//    回发 SendCmdErrorPacket((ENUM_CMDPACKET)0xcf,(unsigned char)e)。
// ============================================================================

// （宿主 TU 已含：Dispatchers.h/CInventory.h/CItemAmplifier.h/GlobalData.h/
//   GameWorld.h/CUserCharacInfo.h/GameTypes.h(cMyTrace)/
//   CompoundEmblemDeps.h(LineFunc)。）

// ==== Dispatcher_PurifyItem::Dispatcher_PurifyItem (ORIG IPacketDispatcher C1 0x0859879e W 转发) ====

#include "Dispatchers/dtor_Dispatcher_PurifyItem.inc"


#include "Dispatchers/read_319.inc"


#include "Dispatchers/process_318.inc"


/* ==================== RefundSkill_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_RefundSkill —— 还原 ORIG 0x081da6fe..0x081dafa3（read/process/
// check_error/send）。依据 docs/class_func_reports/Dispatcher_RefundSkill/
// {read,process,check_error,send}.md。
//
// 宿主落盘注意：
// 1. PacketTypes.h 的 MSG_REFUND_SKILL 已具名（m_0d 技能 id、m_11 技能树选择、
//    m_12 树种类、m_13 退款数量），直接使用。
// 2. stRefundSkill 目前为 char m_pad[0x40] 占位，ORIG 布局（process 写 / send
//    读实证）如下；落盘前建议字段化，本批先以偏移视图访问并注明：
//      +0x04 unsigned char 技能树序号快照（GetCurCharacSkillTreeIndex）
//      +0x05 bool          特殊技能标记（CSkill::IsSpecialSkill）
//      +0x06 unsigned short 剩余 SP/SFP
//      +0x08 int           SkillSlot::refund_skill 返回值
//      +0x0c unsigned char 树种类（回传 msg m_12）
//      +0x0d unsigned char 技能等级（get_skill_level）
//      +0x10 int           物品 idx（回传 msg m_0d）
//      +0x14 unsigned char 退款数量（回传 msg m_13）
//      +0x18 int           错误码（check_error/内部失败）
// ============================================================================




namespace  // 本 TU 内的 stRefundSkill 偏移视图（头字段化前的唯一访问点）
{
// stRefundSkill 冻结头仅 m_pad[0x40] 占位；§8.3 本 TU 具名字段视图
// （偏移/宽度证据=Dispatcher_RefundSkill 各 ORIG 报告 get_/put_ 序列）。
struct RefundSkillView         // 自然对齐+显式 pad 即达各字段偏移
{
    char m_vptr[4];             // +0x00 ParamBase vptr
    unsigned char m_treeIndex;  // +0x04 树序号
    bool m_special;             // +0x05 特殊技能
    unsigned short m_remain;    // +0x06 剩余 SP/SFP
    int m_result;               // +0x08 refund 结果
    unsigned char m_kind;       // +0x0c 树种类
    unsigned char m_level;      // +0x0d 等级
    char m_pad0E[2];            // +0x0e..+0x0f
    int m_itemIdx;              // +0x10 物品 idx
    unsigned char m_count;      // +0x14 数量
    char m_pad15[3];            // +0x15..+0x17
    int m_err;                  // +0x18 错误码
};

// 视图访问宏（P49：helper 用宏不用额外函数，免多出发射符号）
#define RF(p) (*reinterpret_cast<RefundSkillView*>(&p))

// [DualDispatchers 2026-09-09 p167] 匿名 namespace inline 在 4.4 -O0 仍发
// GLOBAL__N_1 call（RefundSkill 实测 23 处 EXTRA）⇒ 一律宏化（附一已实证）。
#define rfTreeIndex(p) (RF(p).m_treeIndex)   // +0x04 树序号
#define rfSpecial(p)   (RF(p).m_special)     // +0x05 特殊技能
#define rfRemain(p)    (RF(p).m_remain)      // +0x06 剩余SP/SFP
#define rfResult(p)    (RF(p).m_result)      // +0x08 refund 结果
#define rfKind(p)      (RF(p).m_kind)        // +0x0c 树种类
#define rfLevel(p)     (RF(p).m_level)       // +0x0d 等级
#define rfItemIdx(p)   (RF(p).m_itemIdx)     // +0x10 物品idx
#define rfCount(p)     (RF(p).m_count)       // +0x14 数量
#define rfErr(p)       (RF(p).m_err)         // +0x18 错误码

}  // namespace

#include "Dispatchers/read_320.inc"


#include "Dispatchers/check_error_85.inc"


#include "Dispatchers/process_319.inc"


// ==== Dispatcher_RefundSkill::send (ORIG 0x081dae0a) ====
// ORIG 以 param 基址 + 固定偏移直读（-0xc 缓存指针，合并寻址 mov 0xNN(%eax)）。
struct RefundParamView
{
    char m_pad00[0x04];
    char m_treeIndex;            // +0x04 树序号（ORIG movsbl 符号扩展）
    char m_special;              // +0x05 特殊技能（ORIG movsbl）
    unsigned short m_remain;     // +0x06 剩余SP/SFP
    int m_result;                // +0x08 refund 结果
    unsigned char m_kind;        // +0x0c 树种类
    unsigned char m_level;       // +0x0d 等级
    char m_pad0e[2];
    int m_itemIdx;               // +0x10 物品序号
    unsigned char m_count;       // +0x14 数量
    char m_pad15[3];
    int m_err;                   // +0x18 错误码
};
#include "Dispatchers/send_87.inc"


/* ==================== SecurityCardAuthRpy_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_SecurityCardAuthRpy 桩真实化（批次 disp_b1）
// 依据 docs/class_func_reports/Dispatcher_SecurityCardAuthRpy/{read,process,check_error,send}.md
//
// 宿主落盘注意（Dispatchers.cpp）：
//   1. 下方"局部线格式视图"为文件级一次性定义，置于类实现之前（匿名 namespace）。
//   2. 需在 include 区补充：Cipher.h、GameTypes.h(cMyTrace)、CTradeSpace.h
//      （send 分支用 CTradeSpace 完整类型）；GlobalData.h/CDataManager.h/
//      CompoundEmblemDeps.h(LineFunc)/CUserCharacInfo.h 已含。
//   3. 布局告警：ORIG MSG_SECU_RPY 权威尺寸 >= 0x1014（数据区 +0x0f..+0x100e、长度
//      +0x100f、应答类型 +0x1013）；当前 PacketTypes.h 仅 char m_pad[0x100]（总 0x10d），
//      read 按报告偏移写入会越出该对象。需 PacketTypes 批次扩正权威布局；本批按 ORIG
//      报告偏移经视图访问，不改共享头。
//   4. BLOCKED 外部依赖（grep 全仓库 *.h 无声明，按约束注释占位、保留其余逻辑）：
//      - WongWork::CSecurityCard::{checkAnswer,incFailCnt,getCertCnt,setCertified,
//        resetFailCnt}（CUser.h:800 现有声明缺这五个，其余方法已在）
//      - XNuclear::CHades::ChangeSecurity(_ZN8XNuclear6CHades14ChangeSecurityENS_19ENUM_HADES_SECURITYEj @0x084b95a8)
//      - cUserHistoryLog::SecuService(@0x0868479c)
//      - DB_SecurityCardUpdateFailCnt::makeRequest(uint,uint)（@0x0842bfaa）
//      - DB_SecurityCardAddCertCnt::makeRequest(uint)（@0x08436388）
//      - DB_SecurityCardUpdateCancelCnt::makeRequest(uint,uint)（@0x0842c112；
//        DB_AccountCargoSync.h 现声明为非静态成员，与 ORIG 静态调用形态不符）
//      - Dispatcher_MouseRegister::doPassPadReplay（@0x082633c6）
// ============================================================================

// ==== 前置：局部线格式视图（宿主一次性置入匿名 namespace） ====
#pragma pack(push, 1)
namespace {

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

}  // namespace
#pragma pack(pop)

// [SplitC 2026-09-11 batch4 · TU 迁移] read_321 已迁出至 PacketDispatcher_Secu.cpp（ORIG GI 键对齐）。



#include "Dispatchers/check_error_86.inc"


#include "Dispatchers/process_320.inc"


#include "Dispatchers/send_88.inc"


/* ==================== StackableActionUse_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_StackableActionUse 四函数真实化
// ORIG: check_error 0x0825e332 / read 0x0825df72 / process 0x0825e11e /
//       send 0x0825e4ec
//
// 宿主落盘注意（Dispatchers.cpp 已有 include 集合核对）：
//   - CHackAnalyzer.h：需补 include（getHackAnalyzer()->addServerHackCnt 需要
//     WongWork::CHackAnalyzer 完整类型；CUser.h 仅返回指针）。
//   - Inven_Item.h / CInventory.h / CDataManager.h / GlobalData.h：已 include ✓
//   - StackableAction::Type 枚举经 CUser.h 可见（verifyStackableItemProcess 签名）✓
//
// Param 视图：模板第二参 stStackableActionUse（PacketTypes.h:2583，ParamBase 派生，
// char m_pad[0x40] 占位）。ORIG process/send 报告实证布局：
//   +0x04 int 错误码、+0x08 short 回显槽位(msg+0xd)、+0x0a char 回显类型(msg+0xf)。
//
// BLOCKED 汇总：
//   - StackableAction::Action::check(int,input_param&) @0x0827fa26 与
//     StackableAction::Action::run(int,input_param&) @0x0827fa9a 仓库无定义；
//     input_param 仅存在于 GlobalData.cpp TU 内部（头未暴露），无法跨 TU 构造。
//     → process 中 check→run→use_item→_processContinuousItem 主链以 BLOCKED
//       注释占位，错误路径/Param 填充/日志全部保留。
// ============================================================================

#include "Dispatchers/check_error_87.inc"


#include "Dispatchers/read_322.inc"


#include "Dispatchers/process_321.inc"
{
    // stStackableActionUse pad 未字段化 —— ORIG 报告实证偏移视图：
    // [DispA6 P0] ORIG 0x0825e12c 只把 param 地址落**一个**具名引用槽 -0x10
    //   （访问形 `mov -0x10(%ebp),%edx; mov %eax,0x4(%edx)` 单级 disp）；旧码
    //   err/echoSlot/echoType 三个独立引用局部（-0x24/-0x20/-0x1c）ORIG 无
    //   （帧 +0xc）⇒ 合并为单一视图引用，三字段仍具名 +0x4/+0x8/+0xa。
    struct StkParamView
    {
        char m_pad00[0x4];
        int m_4;             // +0x04 错误码
        short m_8;           // +0x08 回显槽位
        char m_a;            // +0x0a 回显类型
        char m_pad0b[0x35];  // 余量（ParamBase 视图总长 0x40）
    };
    // [L7DispG2] P331 声明序=槽序：ORIG 0x0825e126 先 `mov 0x10(%ebp),%eax;
    //   mov %eax,-0x14(%ebp)`（msg 引用占 -0x14），再 `mov 0x14(%ebp),%eax;
    //   mov %eax,-0x10(%ebp)`（param 占 -0x10）⇒ msg 引用先声明。
    MSG_STACKABLE_ACTION_USE& req = reinterpret_cast<MSG_STACKABLE_ACTION_USE&>(msg);
    StkParamView& out = reinterpret_cast<StkParamView&>(param);

    out.m_4 = check_error(user, msg);
    // [L7DispG2 负结论] ORIG 0x0825e154 `movzbl 0xf(%eax),%edx`（直落 edx）；
    //   4.4 产 `movzbl→%eax; mov %eax,%edx`（+1 条）。去 static_cast / 经
    //   unsigned char 中转两形实测均不变 ⇒ N7 寄存器分配封顶。
    out.m_a = static_cast<char>(req.m_0f);
    out.m_8 = req.m_0d;

    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_2.cpp",
            __PRETTY_FUNCTION__,
            0x11f, "Dispatcher Check_Error No(%d)", out.m_4);
        return LineFunc(288,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // [DispA6 P0] ORIG 无 inven 槽（0825e1f3 `call getCurCharacInvenR;
    //   mov %eax,0x4(%esp)` 结果直传 GetInvenSlot）也无 itemIdx 槽（0825e21c
    //   `mov -0x5f(%ebp),%eax; mov %eax,%ebx` 从 item 临时直读 +0x02 入
    //   find_item）⇒ 两具名局部内联。
    Inven_Item item =
        const_cast<CInventory*>(user->getCurCharacInvenR())
            ->GetInvenSlot(1, static_cast<int>(req.m_0d));
    CStackableItem* itemDef = static_cast<CStackableItem*>(
        G_CDataManager()->find_item(static_cast<int>(item.m_addInfo)));
    if (itemDef == 0)
    {
        out.m_4 = 0x11;
        return 0;
    }

    StackableAction::input_param inputs;   // 前 8 字节未初始化（ORIG 忠实形态）
    inputs.m_user = user;                  // +0x08
    inputs.m_item = itemDef;               // +0x0c
    out.m_4 = GlobalData::g_Actions->check((int)(char)req.m_14, inputs);
    if (out.m_4 == 0)
    {
        out.m_4 = GlobalData::g_Actions->run((int)(char)req.m_14, inputs);
    }
    if (out.m_4 == 0)
    {
        out.m_4 = user->getCurCharacInvenW()->use_item(
            (int)req.m_0d, 1, (int)(char)req.m_0f);
        if (out.m_4 == 0)
        {
            // ORIG 0x0825e30d `mov -0x5f(%ebp),%eax` 每使用点重读 item+0x02。
            user->_processContinuousItem(
                static_cast<unsigned int>(item.m_addInfo), itemDef);
        }
    }
    return 0;
}

#include "Dispatchers/send_89.inc"


#include "Dispatchers/getItem_1.inc"


#include "Dispatchers/notifyUpdatedInventoryItemInfo.inc"


#include "Dispatchers/process_322.inc"


#include "Dispatchers/check_error_88.inc"


#include "Dispatchers/process_323.inc"


#include "Dispatchers/send_90.inc"



// ══════════════ 批次 B2：11 类桩真实化落盘区（DispatchersRestore） ══════════════

/* ==================== CeraPackageOpen_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_CeraPackageOpen 桩真实化 —— 函数体交付（B2）
// 目标落盘：source/DNFServer/GameServer/Game/Dispatchers.cpp 类段
// （替换 2225-2238 行空 ctor/dtor 与 check_error/process/read 桩；本类 ORIG 无
//   send —— IPacketDispatcher 基类 send 空实现即 ORIG 形态）
//
// 报告依据（docs/class_func_reports/Dispatcher_CeraPackageOpen/，全部已完整阅读）：
//   check_error.md 0x081e7cc2 / read.md 0x081e76fa / process.md 0x081e788a
//
// 【前置条件 — 宿主必须先扩头，否则越界】PacketTypes.h 现为：
//     class stOpenCeraPackage : public ParamBase { int m_4; char m_pad08[0x3c]; };
//   （sizeof 0x44）。ORIG read 按 msg 写至 +0x148：+0xe short slot、
//   +0x10 byte count(≤0x23)、条目 i∈[0,count)：ulong@+(i+2)*8+4、byte@+(i+2)*8+8。
//   dispatch_template 以 `Param param;` 栈分配，宿主需扩 sizeof ≥ 0x149。
//
// 【BLOCKED 外部调用】（grep 全仓库无声明，按约束注释占位、其余逻辑保留）：
//   _ZN8WongWork9CCeraShop22CallSpecialItemHandlerEiRK19STSpecailItem_ParamR20STSpecailItem_Result @0x08325854
//   —— CCeraShop.h 无该成员声明；STSpecailItem_Param/STSpecailItem_Result 为
//      CCeraShop_Special.cpp 文件局部 struct（ORIG W ctor 0x08234ec2/0x08234f36，
//      布局 +0x00 user +0x04 itemIdx +0x08 value +0x0c items* +0x10 ceraParam*
//      +0x14 count +0x18 time(long) +0x1c flag +0x1d..1f char +0x20 extra=1 /
//      Result: int m_value）。恢复方式：将两 struct 提升为权威头类型、CCeraShop.h
//      补 CallSpecialItemHandler(int, const STSpecailItem_Param&, STSpecailItem_Result&)
//      声明后，去掉 process 内 BLOCKED 占位改为真实调用（含构造序列，注释已给出）。
//
// 头约束：新增 include 无（宿主 TU 已含 CUser.h/CInventory.h/CDataManager.h/
// GameEnums.h/PacketGuard.h/InterfacePacketBuf.h 等）。
// ============================================================================

#include "Dispatchers/check_error_89.inc"


#include "Dispatchers/read_323.inc"

#undef MPK

#include "Dispatchers/process_324.inc"


/* ==================== Debug_Clear_Quest_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_Debug_Clear_Quest —— B2 批次真实化(DispB2_GrpH)
// 报告:docs/class_func_reports/Dispatcher_Debug_Clear_Quest/{read,process,send}.md
//
// 宿主接入说明:
//   - 新增 include:无(宿主 Dispatchers.cpp 已含 Quest.h(MsgClearQuestIndex/
//     ReqDebugClearQuestResult 经 PacketTypes.h、stSelectQuestParam、Quest)、
//     CDataManager.h(G_CDataManager/find_quest)、CUser.h(isGMUser/incQuestPoint/
//     incQuestPiece/sendCharacQp/sendCharacQuestPiece/getCurCharacQuestW/
//     getCurCharacQuestR/send_clear_quest_list)、QuestClear.h(CQuestClear::
//     setClearedQuest(unsigned))、PacketGuard.h。
//   - 新增头声明:无(Dispatchers.h 已声明 process/read/send 三方法)。
//
// 注:宿主 Dispatchers.cpp:2916-3008 已含一份等价实现(逐报告核对语义一致);
// 本文件为报告忠实转写,供宿主统一落盘时对照或覆盖。两处差异已按报告修正:
//   1) find_quest 走 const 重载 _ZNK12CDataManager10find_questEi @0x0835fdc6
//      (ORIG 符号),经 const CDataManager* 调用;
//   2) find_skill 同理见 Skill_Command_Customizing_body.cpp。
// ============================================================================




/* ==================== ExpertExtraction_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_ExpertExtraction 四函数真实化(ORIG 转写)
// 目标宿主:source/DNFServer/GameServer/Game/Dispatchers.cpp 2571-2576 现有桩
//   (check_error/process/read/send)按本文件替换函数体。
// ============================================================================
//
// ---- 新增 include(宿主 Dispatchers.cpp 当前缺失,需补)----
//   #include "STItemScript.h"      // ENUM_EXPERT_JOB_TYPE
// 已含可复用:GlobalData.h(g_ExtractionMgr)、PacketGuard.h、InterfacePacketBuf.h、
//   CompoundEmblemDeps.h(LineFunc)、GameEnums.h(ENUM_CMDPACKET)。
//
// ---- 宿主需补的声明(expert_extraction 域当前无任何头声明/实现)----
// 1) expert_extraction::GetExtracterType — ORIG T _ZN17expert_extraction16GetExtracterTypeE20ENUM_EXPERT_JOB_TYPE @0x084a2904:
//      namespace expert_extraction { int GetExtracterType(ENUM_EXPERT_JOB_TYPE job); }
//    语义:job==2→0(Alchemist)/job==1→1(Enchanter)/job==4→2(DollController)/其它→3。
// 2) expert_extraction::CExtractionMgr::get_extracter — ORIG T ...13get_extracterEj @0x084a3166:
//      class CExtractionMgr { public: CExtraction* get_extracter(unsigned int type); };
//    该类现为 GlobalData.cpp 局部类(m_slots[3]),需提升到 GlobalData.h 供本 TU 调用;
//    get_extracter 即 m_slots[type](type>2 返回 0)。
// 3) expert_extraction::CExtraction 业务虚函数(GlobalData.cpp 局部类现仅 ctor/dtor/m_rand,
//    需按 ORIG vtable 槽序补齐;ORIG vtable @0x8c7ddc0:槽+8/+0xc/+0x10):
//      virtual ~CExtraction();                                                        // +0x00/+0x04
//      virtual bool extraction(CUser*, short, std::vector<std::pair<int,int> >&);      // +0x08 ORIG T 0x084a3182 族
//      virtual int  check_error(CUser*, char, short, short);                          // +0x0c ORIG T 0x084a2a5e
//      virtual bool inven_process(CUser*, short, std::vector<std::pair<int,int> >&, std::vector<int>&); // +0x10 ORIG T 0x084a2cec
// ============================================================================

#include "Dispatchers/read_324.inc"
{
    MSG_EXPERT_EXTRACTION& m = reinterpret_cast<MSG_EXPERT_EXTRACTION&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(m.m_0d)))
    {
        return LineFunc(0x45ac, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(reinterpret_cast<short&>(m.m_0e)))
    {
        return LineFunc(0x45ae, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(reinterpret_cast<short&>(m.m_10)))
    {
        return LineFunc(0x45b0, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

// ==== Dispatcher_ExpertExtraction::process (ORIG 0x081d9412) ====
namespace expert_extraction
{
#include "Dispatchers/GetExtracterType.inc"
}

#include "Dispatchers/process_325.inc"


#include "Dispatchers/check_error_90.inc"


#include "Dispatchers/send_91.inc"


/* ==================== FatigueAttendance_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_FatigueAttendance 函数体批次（disp_restore_b2 / GrpF）
// 来源报告：docs/class_func_reports/Dispatcher_FatigueAttendance/{README,check_error,process,read,send}.md
// 目标落点：source/DNFServer/GameServer/Game/Dispatchers.cpp 替换 3026-3031 行桩段
// ============================================================================
//
// ---- 新增 include（宿主 Dispatchers.cpp 当前缺失）----
                               // EventClassify::CAttendance（get_event_script_mng 视图）

// ---- 宿主需补的头声明（本批次函数体直接使用）----
// CUser.h（现仅有无参 void* get_ingame_event_history() @CUser.h:2111，缺重载）：
//   void get_ingame_event_history(int type, std::pair<int,int>& out);
//       // ORIG _ZN5CUser24get_ingame_event_historyEiRSt4pairIiiE T 0x0868d50c
//   void set_ingame_event_history(int type, int value1, int value2);
//       // ORIG _ZN5CUser24set_ingame_event_historyEiii T 0x0868d580
// 自由位工具（定义已在 BingoEventLink4.cpp，缺公共声明点）：
//   bool isSetBit_Uint(unsigned int value, int bit);        // T 0x0889232e
//   void setBit_Uint(unsigned int& value, bool set, int bit); // T 0x088922d7
//
// ---- BLOCKED 依赖（占位保留，其余逻辑已真实化）----
//   _ZN13EventClassify15CEventScriptMng29process_update_ingame_historyEP5CUser
//     T 0x0810c82e —— CEventScriptMng.h 无此方法声明、全仓无定义。
//     见 process() 内注释；恢复后取消注释该调用行即可。
// ============================================================================

#include "Dispatchers/check_error_91.inc"


#include "Dispatchers/read_325.inc"


#include "Dispatchers/process_326.inc"
{
    MSG_FATIGUE_ATTENDANCE& req = reinterpret_cast<MSG_FATIGUE_ATTENDANCE&>(msg);
    // stFatigueAttendance 字段（现 m_pad[0x40] 占位；建议宿主字段化）：
    //   +0x04 int  errNo（check_error 结果，send 消费）
    //   +0x08 int  当日序号（= req.m_0d 回填，send 以 short 发出）
    //   +0x0c unsigned 打卡位图（setBit 后的 history.first 快照，send put_int）
    //   +0x10 int  今日时间值（get_today_time，send put_int）
    struct stFatigueAttendanceParamView
    {
        int m_pad0;             // +0x00
        int m_errNo;            // +0x04
        int m_day;              // +0x08
        unsigned int m_flags;   // +0x0c
        int m_todayTime;        // +0x10
    };
    stFatigueAttendanceParamView& pres =
        reinterpret_cast<stFatigueAttendanceParamView&>(param);


    // ORIG +0x008：param+8 = *(int*)(msg+0xd)（先于 check_error 回填请求当日序号）
    pres.m_day = req.m_0d;

    // [DispA6 P0] ORIG 0x081db033 `mov -0x2c(%ebp),%edx; mov %eax,0x4(%edx)` +
    //   0x081db039 `mov -0x2c(%ebp),%eax; mov 0x4(%eax),%eax` 重读 ⇒ check_error
    //   结果直存 param+4 后重读，无 err 具名槽（旧码 -0x30 为发明）。
    pres.m_errNo = check_error(user, msg);
    if (pres.m_errNo > 0)

    {
        return 0;
    }
    if (pres.m_errNo < 0)

    {
        return LineFunc(
            0x49c5,
            __PRETTY_FUNCTION__,
            pres.m_errNo, 0);

    }

    // ORIG：每个使用点重取 G_CDataManager()->get_event_script_mng()（P48 无 CSE）。
    unsigned char day = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())->get_today_time_day(); // +0x09d
    int recordType = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())->get_record_type(0x56); // +0x0bd

    // ORIG 0x081daffa：get_ingame_event_history(recordType, history) 读历史
    std::pair<int, int> history;
    user->get_ingame_event_history(recordType, history);
    unsigned int attendFlags = static_cast<unsigned int>(history.first);

    // 已打过当日卡（bit day-1 已置位）→ 0x12
    if (isSetBit_Uint(attendFlags, static_cast<int>(day) - 1))
    {
        pres.m_errNo = 0x12;

        return 0;
    }

    // 疲劳合计：server_fatigue_count(day)（short）+ fatigue_count_db(day)
    // [G4-326 2026-09-11 · P0/P183/P29] ORIG 0x081db125 `movswl %ax,%ebx`（第一项保活
    //   %ebx）→ 0x081db13b `lea (%ebx,%eax,1),%eax; mov %eax,-0x1c` ⇒ 两项**单表达式
    //   初始化**（`+=` 两语句是具名槽 -0x24 + add，帧 +0x14）。
    const int totalFatigue =
        static_cast<int>(user->get_server_fatigue_count(day)) +
        user->get_fatigue_count_db(static_cast<int>(day));

    EventClassify::CEventEntity* entity = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())->get_event_entity(0x56);   // +0x166
    if (entity == 0)
    {
        return 0;
    }

    int todayTime = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())->get_today_time();
    // ORIG：start > today || end < today → 不在活动期 → 7
    if (entity->get_event_start_time() > todayTime ||
        entity->get_event_end_time() < todayTime)
    {
        pres.m_errNo = 7;

        return 0;
    }

    // 疲劳超上限（fatigue_condition < total）→ 0x16
    EventClassify::CAttendance* attendance =
        static_cast<EventClassify::CAttendance*>(entity);
    if (attendance->get_fatigue_condition() < totalFatigue)
    {
        pres.m_errNo = 0x16;

        return 0;
    }

    // 置当日打卡位并回写历史（ORIG 0x081daffa：set_ingame_event_history(recordType, attendFlags, 0)）
    setBit_Uint(attendFlags, true, static_cast<int>(day) - 1);
    user->set_ingame_event_history(recordType, attendFlags, 0);

    pres.m_flags = attendFlags;

    // [L7g5 2026-09-11 试验·负结论] ORIG 段第二次 get_today_time 的结果**无具名槽**
    //   （直接 `mov %eax,0x10(%edx)` 写字段），随后 `mov -0x2c(%ebp),%eax;
    //   mov 0x10(%eax),%esi` 从**已写字段读回**作 get_pass_day 第三实参（P92）。
    //   试「删 nowTime 具名量、直接写字段 + 读回」⇒ 编译后该函数指令数/形状**零变化**
    //   （4.4 本就 CSE 该读回）⇒ 已回滚，维持具名 nowTime（可读性优先）。
    int nowTime = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())->get_today_time();
    pres.m_todayTime = nowTime;


    int passDay = ((EventClassify::CEventScriptMng*)
        G_CDataManager()->get_event_script_mng())
        ->get_pass_day(entity->get_event_start_time(), nowTime);
    attendance->send_attend_reward_item(user, passDay);                  // T 0x0810e878

    // ORIG 0x081daffa：process_update_ingame_history(user) 后 reset_ingame_event_history_update()
    ((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
        ->process_update_ingame_history(user);
    user->reset_ingame_event_history_update();

    int finalCondition = attendance->get_final_day_condition();
    attendance->send_attend_final_rward_item(user, static_cast<int>(attendFlags),
                                             finalCondition);            // T 0x0810e9c0
    return 0;
}

#include "Dispatchers/send_92.inc"
{
    // ORIG 0x081db392：param typed 指针槽 -0xc、字段全单级 disp
    // （`mov 0x4(%eax)`/`mov 0x8(%eax)`/…）——char*+N 反产 add 两步
    // 覆写（技巧14）。PacketTypes.h 冻结，本 TU 局部视图具名。
    struct FatigueSendView {
        char m_vptr[4];   // +0x00 ParamBase vptr
        int m_4;          // +0x04
        int m_8;          // +0x08
        int m_c;          // +0x0c
        int m_10;         // +0x10
    };
    FatigueSendView* pres = reinterpret_cast<FatigueSendView*>(&param);
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1ad);
    if (pres->m_4 == 0)
    {
        guard.put_byte(1);
        // ORIG 读 param+8 整型入栈，由 put_short 取低 16 位发出
        guard.put_short(pres->m_8);
        guard.put_int(pres->m_c);
        guard.put_int(pres->m_10);
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(static_cast<signed char>(pres->m_4));
        guard.put_short(pres->m_8);
    }
    guard.finalize(true);
    user->Send(guard);
}

/* ==================== ObjectBringUp_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_ObjectBringUp 桩真实化 —— 函数体交付（B2）
// 目标落盘：source/DNFServer/GameServer/Game/Dispatchers.cpp 类段
// （替换 1985-1997 行空 ctor/dtor 与 check_error/process/read/send 桩）
//
// 报告依据（docs/class_func_reports/Dispatcher_ObjectBringUp/，全部已完整阅读）：
//   check_error.md 0x081e16ce / read.md 0x081e147e / process.md 0x081e14d4 /
//   send.md 0x081e1704
//
// 【前置条件 — 宿主必须先扩头，否则越界】PacketTypes.h 现为：
//     class stObjectBringUp : public ParamBase {
//         int m_4; short m_8; unsigned char m_a; unsigned char m_b; };  // sizeof 0x10
//   ORIG 另在 +0x0c 内嵌 std::vector<int>（sendBringUpReward 输出、send 迭代回包，
//   共 0xc 字节，总 sizeof 应为 0x18）。dispatch_template 以 `Param param;` 栈分配，
//   宿主需补成员（建议名 m_rewards，+0x0c vector<int>）。本文件以
//   reinterpret_cast 承载该 vector 引用，其余字段用现有具名成员。
//
// 【BLOCKED 外部调用】（ORIG 符号经 docs/class_func_reports/EventClassify__CObjectBringUp/
// 确认真实存在，但仓库无声明无定义 —— 归 EventClassify 实体批次；按约束注释占位、
// 其余逻辑保留）：
//   1. _ZN13EventClassify14CObjectBringUp24verifyMaterialItemByRuleEP5CUserj @0x0810fcea
//      bool verifyMaterialItemByRule(CUser*, unsigned int itemIdx)
//   2. _ZN13EventClassify14CObjectBringUp17sendBringUpRewardEP5CUserRSt6vectorIiSaIiEERh @0x0810fe90
//      bool sendBringUpReward(CUser*, std::vector<int>& rewards, unsigned char& flag)
//   恢复方式：CEventScriptMng.h 的 class CObjectBringUp 补上述两声明（同文件
//   CPurchaseCashItemBonus::sendPurchaseBonusReward「声明先行」先例），实现由
//   EventClassify__CObjectBringUp 两份报告落地后，去掉 process 内对应 BLOCKED 占位。
//
// 头约束：新增 include "CEventScriptMng.h"（宿主 TU 未含；get_event_script_mng/
// get_event_entity/is_eventing/CObjectBringUp 前置可见性）。
// ============================================================================


#include "Dispatchers/check_error_92.inc"


#include "Dispatchers/read_326.inc"


#include "Dispatchers/process_327.inc"


#include "Dispatchers/send_93.inc"


/* ==================== RequestAssaultPrice_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_RequestAssaultPrice 四函数真实化(ORIG 转写)
// 目标宿主:source/DNFServer/GameServer/Game/Dispatchers.cpp 3428-3433 现有桩
//   (check_error/process/read/send)按本文件替换函数体。
// ============================================================================
//
// ---- 新增 include(宿主 Dispatchers.cpp 当前缺失,需补)----
//   #include "CAssaultMgr.h"        // pvp_assault::GetInstanceAssaultMgr/CAssaultMgr
//   #include "CPrivateStoreMgr.h"   // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
//   #include "CEquipItem.h"         // CEquipItem::getUpgradeCostDiscount(0x822ca80)
// 已含可复用:GameWorld.h(G_GameWorld/find_from_world/IsIntegratedPvPBaseChannel)、
//   GlobalData.h(s_pSecuProtectionField/s_pGMAccounts_)、CGuildServerProxy.h(CGMAccounts::isGM)、
//   CInventory.h(CSecu_ProtectionField/SECURITY_PROTCTION/CInventory::GetInvenSlot)、
//   CUserCharacInfo.h(IsBusyDisjointStore/IsCurCharacGhost/SetCurCharacAssault*/getCurCharac*)、
//   CDataManager.h(G_CDataManager/find_item)、Inven_Item.h、PacketGuard.h、
//   InterfacePacketBuf.h、CompoundEmblemDeps.h(LineFunc)、GameEnums.h。
//
// ---- 宿主需补的声明 ----
// 1) pvp_assault::CAssaultMgr(CAssaultMgr.h 补三个成员声明,均为 ORIG T):
//      int  check_party_teleport_user(char type, CUser* a, CUser* b, bool flag); // ORIG T 0x082efe14 _ZN...25check_party_teleport_userEcP5CUserS2_b
//      int  GetRequestAssaultNeedMoney(CUser* a, CUser* b);                      // ORIG T 0x082edc86 _ZN...26GetRequestAssaultNeedMoneyEP5CUserS1_
//      void OnRequestAssault(CUser* a, CUser* b, int* outErr);                   // ORIG T 0x082edfba _ZN...16OnRequestAssaultEP5CUserS1_Pi
// 2) stRequestAssaultPrice(PacketTypes.h:2407 现 char m_pad[0x40],按 ORIG 布局字段化,
//    尺寸不变,仅前 8 字节具名;stExpertExtraction 同型先例):
//      int m_4;    // +0x04 check_error 结果
//      int m_8;    // +0x08 突袭费用(含折扣后)
//      char m_pad10[0x38]; // +0x0c..+0x40 保持原总长
// ============================================================================

#include "Dispatchers/read_327.inc"


#include "Dispatchers/process_328.inc"


#include "Dispatchers/check_error_93.inc"


#include "Dispatchers/send_94.inc"


/* ==================== RequestIngameAdvertisement_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_RequestIngameAdvertisement 桩真实化 —— 函数体交付（B2）
// 目标落盘：source/DNFServer/GameServer/Game/Dispatchers.cpp 类段
// （替换现 ctor/dtor/check_error/process/read/send 空桩；另附第 5 函数
//   sendRewardItemSystemMail —— Dispatchers.h 未声明，见下方头约束）
//
// 报告依据（docs/class_func_reports/Dispatcher_RequestIngameAdvertisement/，
// 全部已完整阅读）：
//   check_error.md 0x081e4662 / read.md 0x081e434c / process.md 0x081e43a2 /
//   send.md 0x081e46c8 / sendRewardItemSystemMail.md 0x081e4b6c
//
// 【前置条件 — 宿主必须先扩头，否则越界】PacketTypes.h 现为：
//     class stRequestIngameAdvertisement : public ParamBase { char m_pad[0x40]; };
//   而 dispatch_template 以 `Param param;` 按 sizeof 栈分配，ORIG process/send
//   访问至 +0x644（错误码 int @+0x640、广告类型 short @+0x644），且 +0x004 起
//   内嵌 stNotifyIngameADInfo（≥0x63c）。宿主需将 m_pad 扩至 0x644
//   （sizeof ≥ 0x648）并按下方偏移注释具名化。本文件全部以
//   reinterpret_cast<char*>(&param)+N 绝对偏移访问，与汇编 disp 一一对应。
//
// 【BLOCKED 外部调用】（grep 全仓库无真实声明，按约束注释占位、其余逻辑保留）：
//   1. _ZN27CInGameAdvertisementManager33CheckIngameAdvertisementConditionEP5CUserR20stNotifyIngameADInfoi @0x080fcfa8
//      —— CInGameAdvertisementManager 仅存在于 GameMain.cpp 局部最小类（仅
//         LoadADExposeInfoAtApp 声明），两方法均无声明。
//   2. _ZN27CInGameAdvertisementManager8WriteLogEP5CUsercscj @0x080fda56
//      —— 同上。process 共 3 处调用点均已注释占位。
//   恢复方式：在权威头声明 CInGameAdvertisementManager 两方法后，去掉对应
//   BLOCKED 块内的占位赋值改为真实调用即可，其余代码无需改动。
//
// 【头约束】
//   - sendRewardItemSystemMail(CUser*, unsigned long, int)：ORIG 真实成员
//     （_ZN37...24sendRewardItemSystemMailEP5CUsermi @0x081e4b6c）。宿主需在
//     Dispatchers.h 该类内补声明；本文件给出完整定义。
//   - 新增 include：无（宿主 TU 已含 RDARScriptStringManager.h/CMailBoxHelper.h/
//     CDataManager.h/CInventory.h/CUserCharacInfo.h/InterfacePacketBuf.h/
//     PacketGuard.h/CompoundEmblemDeps.h(LineFunc)/string.h 等）。
//   - 宿主需补 extern 声明（供去 BLOCKED 后链接）：
//       class CInGameAdvertisementManager;
//       CInGameAdvertisementManager* InGameADManagerInstance(); // _Z23InGameADManagerInstancev 0x080fc9e9
// ============================================================================

#include "Dispatchers/check_error_94.inc"


#include "Dispatchers/read_328.inc"


#include "Dispatchers/process_329.inc"


#include "Dispatchers/send_95.inc"


#include "Dispatchers/sendRewardItemSystemMail.inc"


/* ==================== SeriaRoom_Ani_Deco_Event_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_SeriaRoom_Ani_Deco_Event 函数体批次（disp_restore_b2 / GrpF）
// 来源报告：docs/class_func_reports/Dispatcher_SeriaRoom_Ani_Deco_Event/{README,check_error,process,read,send}.md
// 目标落点：source/DNFServer/GameServer/Game/Dispatchers.cpp 替换 4355（process）与
//           4367（send）两行桩；check_error/read 已有真实实现，本批次不动。
// ============================================================================
//
// ---- 新增 include：无（CDataManager/CUserCharacInfo/CGuildServerProxy/
//      GlobalData/InterfacePacketBuf 宿主均已包含）----
//
// ---- 需补的头声明：无 ----
//
// ---- BLOCKED 依赖（占位保留，其余逻辑已真实化）----
// 1. _ZN21SeriaRoomAniDecoEvent12processEventER5CUser T 0x0827ba52
//    SeriaRoomAniDecoEvent 类（重复事件 0x69 实体）全仓未建模；
//    报告齐备：docs/class_func_reports/SeriaRoomAniDecoEvent/{README,processEvent,
//    SendAniDecoItem,StartEvent,...}.md。恢复该类后取消注释 processEvent 调用。
// 2. _ZN31DB_UpdateSeriaRoomAnimationDeco11makeRequestEjsiss T 0x0844af74
//    DB_UpdateSeriaRoomAnimationDeco 类全仓未建模（vtable 引用见
//    global_constructors_keyed_to_WongWork__DBCommon/GetIdentity.md:3942）。
//    ORIG 实参：(unsigned)user->get_acc_id(), (short)user->GetServerGroupToPvP(),
//               (int)user->getCurCharacNo(), (short)info.m_field0, (short)newField2。
// ============================================================================

#include "Dispatchers/process_330.inc"


#include "Dispatchers/send_96.inc"


/* ==================== Skill_Command_Customizing_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_Skill_Command_Customizing —— B2 批次真实化(DispB2_GrpH)
// 报告:docs/class_func_reports/Dispatcher_Skill_Command_Customizing/{read,process,send}.md
//
// 宿主接入说明:
//   - 新增 include:无(宿主 Dispatchers.cpp 已含全部所需头;
//     LineFunc 经 CompoundEmblemDeps.h、NumberToString 经 SkillSlot.h、
//     LogManager::logFormat 经 GameTypes.h、STSkillCommandData/CSkill/
//     stSkillCommandParam 经既有 include 链可见)。
//   - 新增头声明(归属 CUser.h,CUser 域批次负责):
//       bool customizingSkillCommand(STSkillCommandData&);  // ORIG T 0x08654f74
//                                                           // _ZN5CUser23customizingSkillCommandER18STSkillCommandData
//       void defaultSkillCommand();                         // ORIG T 0x086550ee
//                                                           // _ZN5CUser19defaultSkillCommandEv
//     两符号当前源码树无声明/定义(nm -C df_game_r 实为 T),本文件相应调用处
//     以 BLOCKED 注释占位,其余逻辑已完整转写;producer 落地后按注释恢复调用。
//
// MSG_SKILL_COMMAND 线格式(PacketTypes.h 现仅 m_pad[0x100],+0xNN 为报告实证,
// 非推断):+0x0d char commandCount;条目 i(步长 8):
//   +0xe+i*8 short 命令ID / +0x10+i*8 char keyCount / +0x11+i*8+j char key[j]
// ============================================================================

#include "Dispatchers/read_329.inc"


#include "Dispatchers/process_331.inc"


#include "Dispatchers/send_97.inc"


/* ==================== SoloTeleport_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_SoloTeleport —— B2 批次真实化(DispB2_GrpH)
// 报告:docs/class_func_reports/Dispatcher_SoloTeleport/{read,process,check_error,send}.md
//
// 宿主接入说明:
//   - 新增 include:无(宿主 Dispatchers.cpp 已含 PacketTypes.h(MSG_SOLO_TELEPORT)、
//     CPartyTelePort.h(stPartyTelePort,经 PacketTypes.h)、GlobalData.h、
//     GameWorld.h、GameTypes.h(cMyTrace)、CompoundEmblemDeps.h(LineFunc))。
//   - 新增头声明(Dispatchers.h Dispatcher_SoloTeleport 段,process 调用它):
//       int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
//     (ORIG _ZN23Dispatcher_SoloTeleport11check_errorEP5CUserR8MSG_BASER9ParamBase
//      @0x081e1e98;现头文件缺该声明。)
//
// 阻塞依赖(Teleport 类族未建模——GlobalData.cpp 仅存 NPC_Teleport/User_Teleport
// 的 m_pad[0x14] 占位类,无虚表无基类;STTeleportData/STCheckRuleData 亦未建模):
//   class Teleport        { vptr; stPartyTelePort m_tp(+0x4); int m_endCoolTime(+0x10); }
//                         ORIG C1 0x08283c30 / vtable _ZTV8Teleport @0x08c0b240;
//   class NPC_Teleport : Teleport   C1 0x082840da,_ZTV12NPC_Teleport @0x08c0b228;
//   class User_Teleport : Teleport  C1 0x08284142,_ZTV13User_Teleport @0x08c0b210;
//   struct STTeleportData { int m_field00 /*ctor memset 前4字节*/; stPartyTelePort m_tp; }
//                         ORIG ctor W 0x0822d78e(sizeof>=0x10,NPC set_teleport_data
//                         以 data+4 作 stPartyTelePort& 转发读 12 字节实证);
//   struct STCheckRuleData{ int m_field00; CUser* m_user; } ctor W 0x0822d788。
// 各 BLOCKED 注释处均给出 ORIG 地址/mangled 与恢复形态;宿主建模 Teleport 族后
// 取消注释即可,本文件其余逻辑(read 全部/process 参数回填/send 包构造)已完整落地。
// ============================================================================

#include "Dispatchers/read_330.inc"


#include "Dispatchers/process_332.inc"

#include "Dispatchers/check_error_95.inc"


#include "Dispatchers/send_98.inc"


/* ==================== TournamentRewardSelect_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_TournamentRewardSelect 函数体批次（disp_restore_b2 / GrpF）
// 来源报告：docs/class_func_reports/Dispatcher_TournamentRewardSelect/{README,check_error,process,read,send}.md
// 目标落点：source/DNFServer/GameServer/Game/Dispatchers.cpp 替换 3962-3967 行桩段
// ============================================================================
//
// ---- 新增 include（宿主 Dispatchers.cpp 当前缺失）----
                                        // ENUM_TOURNAMENT_REWARD_CARD_TYPE

// ---- 宿主需补的头声明 ----
// CParty.h:600 现声明 `void CheckRecvFromAllMember(CUser*)` 与 ORIG 不符，须改：
//   bool CheckRecvFromAllMember(CUser* user);
//       // ORIG _ZN6CParty22CheckRecvFromAllMemberEP5CUser T 0x085b3fd8（返回 al）
// CTournamentDungeonReward.h 缺三方法声明（本批次函数体直接使用）：
//   bool isCardSelectComplete(ENUM_TOURNAMENT_REWARD_CARD_TYPE type,
//                             const std::vector<int>& members) const;
//       // ORIG _ZN24CTournamentDungeonReward20isCardSelectCompleteE32ENUM_TOURNAMENT_REWARD_CARD_TYPERKSt6vectorIiSaIiEE T 0x08284d08
//   bool SelectCard(int slotNo, char cardA, char cardB);
//       // ORIG _ZN24CTournamentDungeonReward10SelectCardEicc T 0x08284ee4
//   void MakeSelectPacket(PacketGuard* packet);
//       // ORIG _ZN24CTournamentDungeonReward16MakeSelectPacketEP11PacketGuard T 0x08284fa4
// （party+0x17bc 已具名 CParty::m_tournamentReward @CParty.h:756，直接使用。）
//
// ---- BLOCKED 依赖：无（全部外部调用均有现有真实声明/定义）----
// ============================================================================

#include "Dispatchers/check_error_96.inc"


#include "Dispatchers/read_331.inc"


#include "Dispatchers/process_333.inc"
{
    // stTournamentRewardSelect 字段（现 m_pad[0x40] 占位）：+0x04 int errNo
    // [DispA6 P0] ORIG 0x081e00f8 序言 `mov 0x14(%ebp),%eax; mov %eax,-0x1c(%ebp)`
    //   ⇒ param 具名引用槽 -0x1c，各写点 `mov -0x1c(%ebp),%eax; movl $…,0x4(%eax)`
    //   单级 disp；check_error 结果直存 param+4 后重读（0x081e0117/0x081e0120
    //   `mov -0x1c(%ebp),%edx; mov %eax,0x4(%edx)`）⇒ 无 err 具名槽（旧码 -0x1c
    //   为发明，且旧码每点 `add $0x4` 重算 &param 亦与 ORIG 单级 disp 不符）。
    struct TrsParamView
    {
        char m_pad00[0x4];
        int m_4;             // +0x04 errNo
        char m_pad08[0x38];  // 余量（ParamBase 视图总长 0x40）
    };
    TrsParamView& out = reinterpret_cast<TrsParamView&>(param);
    // [G5·P331/P339 声明序=槽序] ORIG 0x081e00f8 先 param→-0x1c，再
    //   `mov 0x10(%ebp),%eax; mov %eax,-0x18(%ebp)`（msg→-0x18）；我方原序相反
    //   （msg -0x1c / param -0x18，整帧 4B 错位）⇒ 交换引用声明序。
    MsgTournamentRewardSelect& req = static_cast<MsgTournamentRewardSelect&>(msg);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(
            0x5315,
            __PRETTY_FUNCTION__,
            out.m_4, 0);
    }

    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return LineFunc(
            0x531c,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    std::vector<int> members;                                // +0x1c7 默认构造
    party->GetValidPartyMember(members);                     // T 0x085bd1dc

    CTournamentDungeonReward& reward =
        reinterpret_cast<CBattle_Field*>(&party->m_battleField)->m_tournamentReward;   // +0x17bc
    // 全员已选卡 → 0x7fffffff（isCardSelectComplete 第二参=卡组类型 2）
    // [DispA6] ORIG 0x081e0203 `mov %al,-0xa(%ebp); movzbl -0xa(%ebp),%eax;
    //   xor $0x1,%eax; test %al,%al; je` ⇒ 原码有具名 bool 且判 `!complete`
    //   （旧码直接 `if (reward.isCardSelectComplete(...))` 只出 test %al,%al）。
    const bool complete = reward.isCardSelectComplete(
        (ENUM_TOURNAMENT_REWARD_CARD_TYPE)2, members);
    if (!complete)
    {
        out.m_4 = 0x7fffffff;
        return 0;
    }

    // msg+0xd/msg+0xe 为 signed byte 读入（movsbl）
    // [DispA6 P0] ORIG 0x081e0232 `call GetMemberSlotNo; mov %eax,0x4(%esp)` —— 槽号
    //   结果**直传** SelectCard，无 slotNo 具名槽（旧码 -0x10 为发明）。
    // [L7g5] ORIG 0x081e0251 `cmpb $0x0,-0x9(%ebp); je 0x081e025e` ⇒ 该具名量是
    //   **字节宽度 + `== 0` 比较**：同一函数内 bool 取反（-0xa 槽）在 ORIG 里
    //   是 `movzbl; xor $1`，此处却是 cmpb ⇒ 原码非 bool（bool 只 0/1，取值等价）。
    const unsigned char selected = (unsigned char)reward.SelectCard(
        party->GetMemberSlotNo(user), (char)req.m_0d, (char)req.m_0e);
    if (selected == 0)
    {
        out.m_4 = 0x7fffffff;
    }
    return 0;
}

#include "Dispatchers/send_99.inc"


// ==== Wave3Deps：8 个 Dispatcher ctor（ORIG 各 0x1c 字节 W，均为编译器形态：
// 调基类 IPacketDispatcher<MSG,Param,state>::C1 后设 vptr；空体即忠实）====
// Dispatcher_StackableActionUse   ORIG 0x0859583c
// Dispatcher_AvatarOptionChange   ORIG 0x0859591c
// Dispatcher_PartyTeleport        ORIG 0x08595954
// Dispatcher_RefundSkill          ORIG 0x0859598c
// Dispatcher_MailBox_Send         ORIG 0x08595ecc
// Dispatcher_SecurityCardAuthRpy  ORIG 0x08596498
// Dispatcher_BlueMarbleConfirmInfo ORIG 0x08597de2
// Dispatcher_PcRoomRentItem       ORIG 0x08597d8e

// ============================================================================
// DspD1 批 D1 依赖定义区（解阻 4 个 BLOCKED dispatch_sig；ORIG mangled/地址逐条注释）
// ============================================================================

// ---- SDC 伤害计算依赖（均经 ORIG nm 核实；定义归 SDC/伤害域批次，UNLINKED）----
extern float  getDamageInflict(STComputeDamageVariable*, void*);                       // T 0x08095193
// [Lane1Dispatchers 2026-09-10 · P292 档①] 权威定义 SDCLink.cpp:224 与 CreatureScript.cpp:332
//   均已为 `bool`；本 TU 旧镜像声明 `int` 使调用点产 `test %eax`，ORIG 为
//   `call; test %al,%al; je`（无 setcc）⇒ 同步为 bool（返回类型不入 mangled，零 ABI 险）。
extern bool   isValidAttackInfoElement(STComputeDamageVariable*, int);                 // T 0x0809576e
namespace _SDC { bool isObjectType(int, int); }                                       // T 0x080950e8
extern float  getAbilityConvertRateSDC(void*, ENUM_CONVERT_TABLE_TYPE);               // T 0x080950fb
extern unsigned int SDC_Rand(unsigned int*);                                          // T 0x0808bd3f
extern float  GetUniformVelocityFloatSDC(float, float, int, int);                     // T 0x088924c1
extern int    getComputeDamageBasisLevelAttacker(STComputeDamageVariable*, bool);     // T 0x080957c3
extern void   getDefenseRateDamager(float&, float&, STComputeDamageVariable*, void*); // T 0x08095810
extern bool   checkIsCriticalSDC(STComputeDamageVariable*, float*, unsigned int*);     // T 0x08095a16
// 全局常量（BSS 由初始化链填充；extern 引用不定义）
extern float g_physicalRandomBaseConst_;   // B 0x093f5d20
extern float g_physicalRandomRateConst_;   // B 0x093f5d24
extern float g_magicalRandomBaseConst_;    // B 0x093f5d38
extern float g_magicalRandomRateConst_;    // B 0x093f5d3c
extern float g_commonElementBaseConst_;    // B 0x093f5d40
extern float g_maxTotalDefenseRate;        // D 0x0936e4a8，文件值 0.01f

#include "Dispatchers/computeHpDamageSDC_1.inc"


// ---- Packet_Mannerless_User_Accusation ctor ----
// ORIG W _ZN33Packet_Mannerless_User_AccusationC1Ev @0x0822c1e6（0xaf）。
// b22 桥清零（P201，Model22）：PacketHeader(type=0x1b66,size=0x15f) 原 extern"C"
// mangled 桥（ORIG W _ZN12PacketHeaderC1Ett @0x08110b70）转 placement new 真构造
// ——本 TU 编译视图为 SigTypes_E.h POD 承载（+0x00..0x05 m_pad0、m_uid@+0x06 与
// ORIG connNo 覆区），不可继承 PacketHeader（字段偏移整体漂移即破坏 16396 消费
// 点布局），placement new 于 +0 真调 ctor，写区/参序与 ORIG 逐字节同形。

#include "Dispatchers/isNeedTypeForChattingMsg.inc"


#include "Dispatchers/Packet_Mannerless_User_Accusation.inc"


// ---- DB_SecurityCardRetransfer::makeRequest ----
// ORIG T _ZN25DB_SecurityCardRetransfer11makeRequestEiPcS0_jPKcS2_S2_ @0x0842c1f6（0x16b）。
// W8LT_DBTask.cpp 占位类已产生 C1；本 TU 提供唯一定义。静态成员（无 this）【asm 核实】。

#include "Dispatchers/makeRequest_4.inc"
    int type, char* characName, char* targetName, unsigned int secCardKey,
    char const* src1, char const* src2, char const* src3)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6f38), true);
    **guard << (int)0x138;
    **guard << (int)type;
    SIG_SECURITY_CARD_RETRANSFER* sig = guard->GetInBuffer<SIG_SECURITY_CARD_RETRANSFER>();
    // 共享头 SIG_SECURITY_CARD_RETRANSFER 仅 m_pad[0x8c]（其它 TU 亦各持局部
    // 视图）；§8.3 本 TU 具名视图，偏移/宽度证据=ORIG makeRequest @0x0842c28a
    // ..0x0842c309 各 strncpy 立即数。
    struct SecurityCardRetransferView
    {
        unsigned int m_secCardKey;   // +0x00
        char m_src2[0x21];           // +0x04
        char m_src1[6];              // +0x25
        char m_src3[0x27];           // +0x2b
        char m_characName[0xc];      // +0x52
        char m_targetName[0x29];     // +0x5e
        char m_padTail[5];           // +0x87..+0x8b
    };
    static_assert(sizeof(SecurityCardRetransferView) == 0x8c,
                  "SecurityCardRetransferView size");
#define SV (reinterpret_cast<SecurityCardRetransferView*>(sig))
    SV->m_secCardKey = secCardKey;                             // 0x0842c28a
    strncpy(SV->m_src1, src1, 6);                              // 0x0842c295
    strncpy(SV->m_src2, src2, 0x21);                           // 0x0842c2b2
    strncpy(SV->m_src3, src3, 0x27);                           // 0x0842c2cf
    strncpy(SV->m_characName, characName, 0xc);                // 0x0842c2ec
    strncpy(SV->m_targetName, targetName, 0x29);               // 0x0842c309
#undef SV
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);  // 0x0842c320
}

#include "Dispatchers/makeRequest_5.inc"



// ==== StubS3A.S3A_DEPS：check_error×2 + isVaildEmotionValue（ORIG T 0x081d14aa/0x081d18dc/0x081d1ebe）====

#include "Dispatchers/check_error_97.inc"


#include "Dispatchers/check_error_98.inc"


#include "Dispatchers/isVaildEmotionValue.inc"


// ============================================================================
// OnlyOrigBatch13 组 E —— 7 个 Dispatcher 方法 + getRewardGiveItems（ORIG 对照）
// ============================================================================
// ============================================================================
// df_game_r 还原 —— Dispatchers 域组 E（8 项，语义照抄 ORIG 反汇编）
// ORIG 符号与地址（nm 核对，df_game_r elf32-i386）：
//   1. _ZN26Dispatcher_BuyCeraShopItem33useCountDownCoinInFreeCoinDungeonEP5CUserRK21MSG_BUY_CERASHOP_ITEM   T 0x081fe4b8
//   2. _ZNK42Dispatcher_Compound_Equipment_Upgrade_Card20set_ReturnSendPacketER11PacketGuardti              T 0x08228a2e
//   3. _ZN35Dispatcher_Enchant_3rdChronicleItem11check_stateEP5CUserR9PacketBuf                            T 0x082328ec
//   4. _ZN29Dispatcher_GuildCargoPushItem11check_errorEP5CUserR8MSG_BASER9ParamBase                        T 0x081d0f64
//   5. _ZN22Dispatcher_ModItemAttr11_SendResultEP5CUserttt                                                T 0x08201938
//   6. _ZN21Dispatcher_PurifyItem17processPurifyTypeEiR10Inven_Item                                        T 0x082194b6
//   7. _ZN45Dispatcher_Select_3rdChronicleItem_ForEnchant11check_stateEP5CUserR9PacketBuf                  T 0x08232854
//   8. _ZN29DimensionActivationRewardData18getRewardGiveItemsEiii                                          T 0x08915f9a
// 报告：docs/class_func_reports/{Dispatcher_BuyCeraShopItem/useCountDownCoinInFreeCoinDungeon,
//   Dispatcher_Compound_Equipment_Upgrade_Card/set_ReturnSendPacket, Dispatcher_Enchant_3rdChronicleItem,
//   Dispatcher_GuildCargoPushItem/check_error, Dispatcher_ModItemAttr/_SendResult,
//   Dispatcher_PurifyItem/processPurifyType, DimensionActivationRewardData/getRewardGiveItems}.md
//   + /tmp/df_batches/dis/<mangled>.dis（check_state 两枚无独立报告，直接转写）。
//
// 说明：
//   - Dispatchers.h 内 Dispatcher_GuildCargoPushItem::check_error 由 2 参数修正为 ORIG 3 参数
//     (CUser*, MSG_BASE&, ParamBase&)；字段全部自 ParamBase（param+0xd/0xe/0x10）读取。
//   - Dispatcher_BuyCeraShopItem_impl.cpp / Dispatcher_ModItemAttr_impl.cpp /
//     Dispatcher_Compound_Equipment_Upgrade_Card_impl.cpp 中原先的「TU 局部内联」保留不动
//     （本文件给出的是同名 ORIG 成员符号，集成时可替换内联）。
//   - Dispatchers.cpp 中 Dispatcher_PurifyItem::process 内联的 processPurifyType 逻辑同保留。
// ============================================================================

                                 // DimensionActivationRewardData / EventRewardItemInfo
                                 // CSecu_ProtectionField / GlobalData::s_pSecuProtectionField

class CDungeon;  // 仅 CDungeon* 指针使用（同 Dispatcher_BuyCeraShopItem_impl.cpp 先例，
                 // 避免引入 CDungeon.h 触发 QuickParty 重复定义）

// LineFunc（ORIG 自由函数 _Z8LineFunciPKcij @0x85908d7；权威头缺失，extern 声明——
// 同 Dispatcher_ModItemAttr_impl.cpp / Dispatcher_BuyCeraShopItem_impl.cpp 先例）。
extern int LineFunc(int, const char*, int, unsigned int);

#include "Dispatchers/useCountDownCoinInFreeCoinDungeon.inc"
    CUser* user, MSG_BUY_CERASHOP_ITEM const& msg)
{
    if (user == 0)
    {
        return false;
    }
    if (msg.m_scalars.m_itemCount != 1)        // +0x36（movzbl 实证字段）
    {
        return false;
    }
    // [b28d-solo-Dispatchers] P211 档③/P254：ORIG 0x081fe4e7 `mov 0x39(%eax),%eax;
    //   mov %eax,%ebx`（两段）＝「unsigned 视图字段 + FindGoods(int) 形参异型」的
    //   转换中转形；+0x39 具名（P156）改经 unsigned 视图内联实参。
    struct __attribute__((packed)) BuyCeraScalarsView
    {
        char m_pad39[0x39];
        unsigned int m_goodsIdx;      // +0x39
    };
    CCeraShopGoods* goods = reinterpret_cast<CCeraShopGoods*>(
        G_CDataManager()->FindGoods(
            reinterpret_cast<const BuyCeraScalarsView&>(msg).m_goodsIdx));
    if (goods == 0)
    {
        return false;
    }
    if (goods->GetItemIdx() != 1)
    {
        return false;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    // [b28c-32] ORIG 0x081fe53b/0x081fe54d/0x081fe563 三条件共享尾 `mov $0` 出口
    // （P222 正向包裹；平铺早返会各生独立 mov$0/jmp 岛 +2 条）。
    if (party != 0)
    {
        CDungeon* dungeon = party->m_battleField.m_pDungeon;
        if (dungeon != 0)
        {
            if (party->checkFreeRevivalCondition(dungeon))
                return true;
        }
    }
    return false;
}

#include "Dispatchers/set_ReturnSendPacket.inc"


#include "Dispatchers/check_state.inc"


#include "Dispatchers/_SendResult.inc"


#include "Dispatchers/processPurifyType.inc"


#include "Dispatchers/check_state_1.inc"


#include "Dispatchers/getRewardGiveItems.inc"
