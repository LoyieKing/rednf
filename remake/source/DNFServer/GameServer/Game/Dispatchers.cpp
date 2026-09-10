// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Dispatchers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
// ORIG T 0x0889232e / 0x088922d7（定义于 BingoEventLink4.cpp）
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

// ==== StubS3A producer（d_aa body 0a/gmList）====
// ORIG _Z9check_urlPKc T 0x08205791（无报告，objdump 转写；CheckGuildAddressDouble 用）。
#include "Dispatchers/check_url.inc"
static int gmList[5] = { 0x004c2dc8, 0x005cf723, 0x00a9c742, 0x00a2ad85, 0x009ae1ae };

#include "Dispatchers/read.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08205791 orig/ours=54/53 ndiff-blocks=2
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: jne    08205808 <+0x77> | addl   $0x1,-0xc(%ebp) | jmp    0820580f <+0x7e>
#include "Dispatchers/check_url_1.inc"
}
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

// ORIG T 0x088945c6：包键槽高 16 位（段位，makeFairPVP 第二参）减 1 写回后返回整槽；
// 低 16 位（职业）保留；无下溢保护（wrap）。
#include "Dispatchers/decreaseRankOfFairPVPEquipmentPackageKey.inc"
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

// DspD4 B4：DB_JoinGuildInfo / DB_CancelJoinGuild makeRequest 定义
// （ORIG _ZN16DB_JoinGuildInfo11makeRequestEi17ENUM_SERVER_GROUPj @0x0843d5fa、
//  _ZN18DB_CancelJoinGuild11makeRequestEijj @0x0843d78c；nm 核实。包构建范式
//  同 DB_AccountCargoSync.cpp：StreamPool::Acquire → CStreamGuard → DB_Q 入队。）
#include "Dispatchers/makeRequest.inc"
}

#include "Dispatchers/makeRequest_1.inc"
}
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

// 安全分发器的协议检查函数由安全子系统提供；这里只保留 ORIG 的薄包装。
// 这些声明对应 docs/class_func_reports/Dispatcher_Security_*/dispatch_sig.md
// 中的全局 C++ 符号，定义缺失时须在安全子系统批次补齐，不能用 no-op 替代。
#include "Dispatchers/_NS_IPScan.inc"
#include "Dispatchers/_NS_MI_RecvMemoryIntegrityModulation.inc"
#include "Dispatchers/_NS_NonClient_GetResponse.inc"
#include "Dispatchers/_NS_PI_2ND_RecvPacketIntegrityModulation_C2C.inc"
#include "Dispatchers/_NS_PI_2ND_RecvPacketIntegrityModulation_S2C.inc"
#include "Dispatchers/_NS_ProcessScan.inc"
#include "Dispatchers/_NS_PacketIntegrity.inc"

#include "STComputeDamageVariable.h"   // D1 依赖：SDC 伤害变量权威布局（computeHpDamageSDC）
// ============================================================================
// D1 依赖前向声明（定义在本 TU 文件尾的 D1 依赖定义区）：
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08095c32 orig/ours=832/811 ndiff-blocks=141
//   帧差 sub $84 vs $94 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x84,%esp | mov    %eax,-0x5c(%ebp) | movl   $0x0,-0x58(%ebp)
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
struct MSG_MAKE_PVP_ROOM;   // 完整布局仅经偏移视图访问（QuickJoinRoom msgBuf 0x3d）
#include "Dispatchers/MakePVPRoom.inc"
#include "Dispatchers/EnterPVPRoom.inc"
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

// ORIG advancealtar::ProcStage::pause(bool) @ 0x812eaa6：
//   ProcStage+0x04 为 stage state；flag=true 时 state 3→2，false 时 2/1→3。
// ProcStage 位于 StageControl+0x04。
#include "Dispatchers/procStagePause.inc"
}

// ORIG CharacData::summonSlotUnit @ 0x812d278：dm+0x6364 处
// map<uchar,_SlotItem>；_SlotItem +0x00 itemId、+0x04 needGage、+0x08 field8。
#include "Dispatchers/characDataSummonSlotUnit.inc"
}

}  // namespace

namespace advancealtar {

// Wave5 三分解方法共用的请求/响应字节视图
// （CMDPacketStruct::STBaseRequest / STBaseResponse 均为不透明载体，
//  ORIG process_packet 按偏移读写，见各 advancealtar__Dispatcher_*/报告）。
inline char* reqBytes(CMDPacketStruct::STBaseRequest& r) { return reinterpret_cast<char*>(&r); }
inline const char* reqBytes(const CMDPacketStruct::STBaseRequest& r) { return reinterpret_cast<const char*>(&r); }
inline char* resBytes(CMDPacketStruct::STBaseResponse& r) { return reinterpret_cast<char*>(&r); }

// ==================== Dispatcher_Pause（ORIG 0x8140a1c）====================
// dispatch_sig：state/charac 校验失败返回 0x52c；否则 read(0xe) → process →
// send_packet(0x242, 0xc) → 0。
// process：res+0x08 = req+0x0d（pause 标志）；check_error 恒 0；
//          Manager::pause(user, flag) → StageControl+0x04 ProcStage::pause。
#include "Dispatchers/dtor_Dispatcher_Pause.inc"
#include "Dispatchers/dispatch_sig.inc"
}
#include "Dispatchers/process.inc"
}
#include "Dispatchers/read_1.inc"
}
#include "Dispatchers/send_packet.inc"
}

// ---- Wave5 三分解方法（独立 const 成员，指令级转写自报告）----
#include "Dispatchers/check_error.inc"
}
#include "Dispatchers/read_packet.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x08140b3c orig/ours=38/36 ndiff-blocks=2
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,%edx | mov    -0xc(%ebp),%eax | mov    %edx,0x4(%eax)
#include "Dispatchers/process_packet.inc"
}

// ==================== Dispatcher_BuyItem（ORIG 0x813fad0）====================
// dispatch_sig：校验失败返回 0x380；read(0x20) → process → send_packet(0x23d, 0x1c)。
// process：res 回显 req 全部字段；res+0x04 = check_error(0)；buyItem 依赖
//          CharacAdvanceAltarManager 内部 AdvanceAltarData（未建模），标注推断。
#include "Dispatchers/dtor_Dispatcher_BuyItem.inc"
#include "Dispatchers/dispatch_sig_1.inc"
}
#include "Dispatchers/process_1.inc"
}
#include "Dispatchers/read_2.inc"
}
#include "Dispatchers/send_packet_1.inc"
}

// ---- Wave5 三分解方法 ----
#include "Dispatchers/check_error_1.inc"
}
#include "Dispatchers/read_packet_1.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0813fc06 orig/ours=188/190 ndiff-blocks=43
//   帧差 sub $5c vs $6c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x5c,%esp | mov    %eax,-0x34(%ebp) | mov    %eax,-0x30(%ebp)
#include "Dispatchers/process_packet_1.inc"
}

// ==================== Dispatcher_SetSlot（ORIG 0x813ff3e）====================
// dispatch_sig：校验失败返回 0x3fc；read(0x24) → process → send_packet(0x23e, 0x1c)。
// process：res 回显 req（含 _SetSlotData 12B）；slotType 非 1/2 → error=2；
//          setSlot 依赖未建模 AdvanceAltarData，标注推断。
#include "Dispatchers/dtor_Dispatcher_SetSlot.inc"
#include "Dispatchers/dispatch_sig_2.inc"
}
#include "Dispatchers/process_2.inc"
}
#include "Dispatchers/read_3.inc"
}
#include "Dispatchers/send_packet_2.inc"
}

// ---- Wave5 三分解方法 ----
#include "Dispatchers/check_error_2.inc"
}
#include "Dispatchers/read_packet_2.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0814006e orig/ours=67/69 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,%edx | mov    -0xc(%ebp),%eax | mov    %edx,0x4(%eax)
#include "Dispatchers/process_packet_2.inc"
}

// ==================== Dispatcher_ResetStar（ORIG 0x8140f6e）====================
// dispatch_sig：校验失败返回 0x5c0；read(0x14) → process → send_packet(0x244, 0xc)。
// process：res+0x04 = check_error(0)；resetStar 依赖未建模数据，标注推断。
#include "Dispatchers/dtor_Dispatcher_ResetStar.inc"
#include "Dispatchers/dispatch_sig_3.inc"
}
#include "Dispatchers/process_3.inc"
}
#include "Dispatchers/read_4.inc"
}
#include "Dispatchers/send_packet_3.inc"
}

// ---- Wave5 三分解方法 ----
#include "Dispatchers/check_error_3.inc"
}
#include "Dispatchers/read_packet_3.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081410aa orig/ours=97/96 ndiff-blocks=1
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,%edx | mov    -0x10(%ebp),%eax | mov    %edx,0x4(%eax)
#include "Dispatchers/process_packet_3.inc"
}

// ==================== Dispatcher_StartGame（ORIG 0x813f7b0 区）====================
// check_error @ 0x813f7de：isOpenAdvanceAltar（CDailyScheduleManager 4/10）→
//   state==3 → level>=50 → 非竞技佣兵 → 非摆摊 → 非交易 → 非分解商店 →
//   非组队 → 非 PVP 频道 → 0；各失败 1/2/3/4。
// process @ 0x813f8ce：check_error → param+0x04；成功后
//   CharacAdvanceAltarManager::checkAndCreateDefaultRidable（未建模，推断）。
// read @ 0x813f7b0：读 0xd 字节。
// send @ 0x813f92a：error==0 先 StageControl::gotoStage（推断）；发包 0x23c：
//   put_byte(1)+put_binary(param,8)；成功后 set_state(0xd)+put_int(1)+ridable 数据
//   （getAdvanceAltarDataByIndex/putRidableInfoToPacket 未建模，推断）。
#include "Dispatchers/dtor_Dispatcher_StartGame.inc"
#include "Dispatchers/check_error_4.inc"
}
// b28d-solo-Dispatchers verdict=AE(routed) [P0/P211档③] ORIG 0x0813f8ce orig/ours=30/30
//   [S162] 已修：①具名 ec + dst 两局部（帧 +8、槽整体漂移）→ 单语句内联实参
//   `p->m4 = check_error(...)`（P0 右值直传）；②_P4::m4 改 unsigned 与 int 返回值
//   异型 ⇒ 4.4 翻 RHS-first+%edx 中转（P211 档③）。
#include "Dispatchers/process_4.inc"
}
#include "Dispatchers/read_5.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [S206 收敛 2→1 块] ORIG 0x0813f92a
//   [N-P63] do-while→while 形（P25）已消除入口 jmp 差异；残余仅 ORIG 0x813fa6a
//   体尾 `jmp +1(nop 中继)`（4.1 单源尾岛，P223 负例：4.4 恒 jump-thread 合并）。
#include "Dispatchers/send.inc"
}

// ==================== Dispatcher_SummonUnit（ORIG 0x81404a8）====================
// dispatch_sig：校验失败返回 0x496；read(0x18) → process → send_packet(0x240, 0x14)。
// process：res+0x08=req+0x0d、res+0x0c=req+0x10；check_error 0；若 gageLevel 匹配
//   summonSlotUnit（真实偏移实现）；res+0x10 = getGage()。
#include "Dispatchers/dtor_Dispatcher_SummonUnit.inc"
#include "Dispatchers/dispatch_sig_4.inc"
}
#include "Dispatchers/process_5.inc"
}
#include "Dispatchers/read_6.inc"
}
#include "Dispatchers/send_packet_4.inc"
}

// ---- Wave5 三分解方法 ----
#include "Dispatchers/check_error_5.inc"
}
#include "Dispatchers/read_packet_4.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081405b8 orig/ours=82/80 ndiff-blocks=2
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,%edx | mov    -0x10(%ebp),%eax | mov    %edx,0x4(%eax)
#include "Dispatchers/process_packet_4.inc"
}

// ==================== Dispatcher_UpgradeGage（ORIG 0x814020e）====================
// dispatch_sig：校验失败返回 0x446；read(0x18) → process → send_packet(0x23f, 0x14)。
// process：res+0x08=升级前 gageLevel、res+0x04=upgreadGage()、res+0x0c=升级后、
//          res+0x10=getGage()（真实偏移实现）。
#include "Dispatchers/dtor_Dispatcher_UpgradeGage.inc"
#include "Dispatchers/dispatch_sig_5.inc"
}
#include "Dispatchers/process_6.inc"
}
#include "Dispatchers/read_7.inc"
}
#include "Dispatchers/send_packet_5.inc"
}

// ---- Wave5 三分解方法 ----
#include "Dispatchers/check_error_6.inc"
}
#include "Dispatchers/read_packet_5.inc"
}
#include "Dispatchers/process_packet_5.inc"
}

// ==================== Dispatcher_ExchangeSlot（ORIG 0x8140774）====================
// dispatch_sig：校验失败返回 0x4e8；read(0x30) → process → send_packet(0x241, 0x24)。
// process：res+0x08=req+0x10；check_error 0；exchangeSlot 依赖未建模数据，推断。
#include "Dispatchers/dtor_Dispatcher_ExchangeSlot.inc"
#include "Dispatchers/dispatch_sig_6.inc"
}
#include "Dispatchers/process_7.inc"
}
#include "Dispatchers/read_8.inc"
}
#include "Dispatchers/send_packet_6.inc"
}

// ---- Wave5 三分解方法 ----
#include "Dispatchers/check_error_7.inc"
}
#include "Dispatchers/read_packet_6.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081408a4 orig/ours=59/59 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    0x24(%eax),%ecx | lea    0x18(%eax),%edx | mov    0xc(%ebp),%eax
#include "Dispatchers/process_packet_6.inc"
}

// ==================== Dispatcher_GetAchievementReward（ORIG 0x8140c74）====================
// dispatch_sig：校验失败返回 0x571；read(0x14) → process（ORIG 为空）→
//   send_packet(0x243)。send_packet：put_byte(1)+put_binary(res,0xc)+
//   put_int(count)+逐项 _RewardItem（sendAchievementReward 未建模，推断）。
#include "Dispatchers/dtor_Dispatcher_GetAchievementReward.inc"
#include "Dispatchers/dispatch_sig_7.inc"
}
#include "Dispatchers/process_8.inc"
}
#include "Dispatchers/read_9.inc"
}
#include "Dispatchers/send_packet_7.inc"
}

// ---- Wave5 三分解方法 ----
#include "Dispatchers/check_error_8.inc"
}
#include "Dispatchers/read_packet_7.inc"
}
#include "Dispatchers/process_packet_7.inc"
}

}
#include "Dispatchers/dtor_Dispatcher_Redeem.inc"
#include "Dispatchers/dispatch_sig_8.inc"
}
#include "Dispatchers/process_9.inc"
#include "Dispatchers/read_10.inc"

#include "Dispatchers/dtor_Dispatcher_UseDye.inc"
#include "Dispatchers/check_error_9.inc"
}
#include "Dispatchers/read_11.inc"
}

#include "Dispatchers/dtor_Dispatcher_Antibot.inc"
#include "Dispatchers/dispatch_sig_9.inc"
}
#include "Dispatchers/process_10.inc"
#include "Dispatchers/read_12.inc"

#include "Dispatchers/dtor_Dispatcher_ChangeHp.inc"
#include "Dispatchers/dispatch_sig_10.inc"
}
#include "Dispatchers/process_11.inc"
#include "Dispatchers/read_13.inc"

#include "Dispatchers/dtor_Dispatcher_Teleport.inc"

#include "Dispatchers/read_14.inc"
}


#include "Dispatchers/dtor_Dispatcher_UseJewel.inc"
#include "Dispatchers/dispatch_sig_11.inc"
}
#include "Dispatchers/process_12.inc"
#include "Dispatchers/read_15.inc"

#include "Dispatchers/dtor_Dispatcher_BingoQuiz.inc"
struct Err4ParamView { int m_pad0; int m_4; };

struct BingoCheckView { char m_pad[4]; char m_b4; bool m_b5; unsigned char m_b6; };  // bool 免 setne 物化

#include "Dispatchers/check_error_10.inc"
}
// ORIG 0x0825e9b8 T _ZN20Dispatcher_BingoQuiz7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_BingoQuiz/process.md 汇编+反编译 C 全文转写）
// param 为模板实参（+0x04 错误码 / +0x08 bingo 数据槽）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825e9b8 orig/ours=71/73 ndiff-blocks=14
//   帧差 sub $38 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x38,%esp | mov    0x10(%ebp),%eax | mov    %eax,-0x14(%ebp)
#include "Dispatchers/process_13.inc"
}
#include "Dispatchers/read_16.inc"
}
#include "Dispatchers/send_1.inc"
}


#include "Dispatchers/dtor_Dispatcher_DieMobAll.inc"
#include "Dispatchers/dispatch_sig_12.inc"
}
#include "Dispatchers/process_14.inc"
#include "Dispatchers/read_17.inc"

#include "Dispatchers/dtor_Dispatcher_SkillInit.inc"
#include "Dispatchers/check_error_11.inc"
}
#include "Dispatchers/process_15.inc"
}
#include "Dispatchers/read_18.inc"
}
#include "Dispatchers/send_2.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e5bdc orig/ours=49/56 ndiff-blocks=8
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%eax | mov    %eax,-0xc(%ebp)
#include "Dispatchers/process_skill_init.inc"
}

#include "Dispatchers/dtor_Dispatcher_Antibot_DP.inc"
#include "Dispatchers/dispatch_sig_13.inc"
}
#include "Dispatchers/process_16.inc"
#include "Dispatchers/read_19.inc"

#include "Dispatchers/dtor_Dispatcher_AskRematch.inc"
#include "Dispatchers/dispatch_sig_14.inc"
}
#include "Dispatchers/process_17.inc"
#include "Dispatchers/read_20.inc"

#include "Dispatchers/dtor_Dispatcher_BVHackInfo.inc"
#include "Dispatchers/dispatch_sig_15.inc"
}
#include "Dispatchers/process_18.inc"
#include "Dispatchers/read_21.inc"

#include "Dispatchers/dtor_Dispatcher_BlueMarble.inc"
#include "Dispatchers/check_error_12.inc"
}
#include "Dispatchers/process_19.inc"
}
#include "Dispatchers/read_22.inc"
}
#include "Dispatchers/send_3.inc"
}

#include "Dispatchers/dtor_Dispatcher_ChangeHost.inc"
#include "Dispatchers/dispatch_sig_16.inc"
}
#include "Dispatchers/process_20.inc"
#include "Dispatchers/read_23.inc"
#include "Dispatchers/checkComboSkillIndex.inc"
}

#include "Dispatchers/dtor_Dispatcher_ComboSkill.inc"
#include "Dispatchers/check_error_13.inc"
}
// err>0 → 0；err<0 → LineFunc(0x33f)；否则当前角色 SkillSlot 写入组合技映射
// 并校验该树。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825f2fc orig/ours=64/62 ndiff-blocks=13
//   帧差 sub $20 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x20,%esp
#include "Dispatchers/process_21.inc"
}
// 读 MSG_COMBO_SKILL：m_pad0d 树种类、树数量(≤6)、每树 {kind, 技能数(≤6),
// skillIdx...}；checkComboSkillIndex 校验通过才收技能列表；逐项插入
// m_comboMap(kind → vector<int>)。任何 get 失败按行号走 LineFunc；
// 外层 get_short 失败/循环耗尽均归 LineFunc(0x30c)。
// 【batch-17 X1 池收口（§6.2）】orig 213 / ours 210（gap 3）：调用点差集
//   25↔25 **归零**（残差 8/8 全为 pair/vector 的 C2/D2 ↔ C1/D1 同体符号名
//   对称差）；残 3 为迭代器/临时对象落槽步序（N82）⇒ 语义完成。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825ef78 orig/ours=213/210 ndiff-blocks=31
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movw   $0x0,-0x42(%ebp) | movb   $0x0,-0x43(%ebp) | movb   $0x0,-0x44(%ebp)
#include "Dispatchers/read_24.inc"
}
#include "Dispatchers/send_4.inc"
}

#include "Dispatchers/dtor_Dispatcher_GM_Command.inc"
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
}
#include "Dispatchers/process_22.inc"
}
#include "Dispatchers/read_25.inc"
}

#include "Dispatchers/dtor_Dispatcher_GuildCargo.inc"
#include "Dispatchers/process_23.inc"
}
#include "Dispatchers/read_26.inc"
#include "Dispatchers/send_5.inc"
}

#include "Dispatchers/dtor_Dispatcher_Join_Power.inc"
#include "Dispatchers/dispatch_sig_18.inc"
}
#include "Dispatchers/process_24.inc"
#include "Dispatchers/read_27.inc"

#include "Dispatchers/dtor_Dispatcher_MotionHack.inc"
#include "Dispatchers/read_28.inc"
}

// ORIG 0x0826464a（报告 Dispatcher_MotionHack/process.md）：
// msg+0xd byte 非 0 → addServerHackCnt(user, 0x83, 1, msg+0xe, msg+0x12)。
#include "Dispatchers/process_25.inc"
}

#include "Dispatchers/dtor_Dispatcher_MoveToGate.inc"
#include "Dispatchers/dispatch_sig_19.inc"
}
#include "Dispatchers/process_26.inc"
#include "Dispatchers/read_29.inc"


#include "Dispatchers/dtor_Dispatcher_RedeemList.inc"
#include "Dispatchers/dispatch_sig_20.inc"
}
#include "Dispatchers/process_27.inc"
#include "Dispatchers/read_30.inc"

#include "Dispatchers/dtor_Dispatcher_SelectItem.inc"
#include "Dispatchers/dispatch_sig_21.inc"
}
// ---- IsEmancipate 弱符号内联组（ORIG P9 直返单字节；本 TU 提供定义） ----
#include "Dispatchers/IsEmancipate.inc"
}

#include "Dispatchers/IsEmancipate_1.inc"
}
#include "Dispatchers/process_28.inc"
#include "Dispatchers/read_31.inc"

#include "Dispatchers/dtor_Dispatcher_TraceError.inc"
#include "Dispatchers/dispatch_sig_22.inc"
}
#include "Dispatchers/process_29.inc"
#include "Dispatchers/read_32.inc"

#include "Dispatchers/dtor_Dispatcher_TryEnchant.inc"
#include "Dispatchers/dispatch_sig_23.inc"
}
#include "Dispatchers/process_30.inc"
#include "Dispatchers/read_33.inc"

#include "Dispatchers/dtor_Dispatcher_VerifyGold.inc"
#include "Dispatchers/dispatch_sig_24.inc"
}
#include "Dispatchers/process_31.inc"
#include "Dispatchers/read_34.inc"

#include "Dispatchers/dtor_Dispatcher_BingoReward.inc"
#include "Dispatchers/check_error_14.inc"
}
#include "Dispatchers/process_32.inc"
}
#include "Dispatchers/read_35.inc"
#include "Dispatchers/send_6.inc"
}

#include "Dispatchers/dtor_Dispatcher_CranePickUp.inc"
#include "Dispatchers/dispatch_sig_25.inc"
}
#include "Dispatchers/process_33.inc"
#include "Dispatchers/read_36.inc"

#include "Dispatchers/dtor_Dispatcher_EPLPCommand.inc"
#include "Dispatchers/dispatch_sig_26.inc"
}
#include "Dispatchers/process_34.inc"
#include "Dispatchers/read_37.inc"

#include "Dispatchers/dtor_Dispatcher_ModItemAttr.inc"



#include "Dispatchers/dtor_Dispatcher_SecurityLog.inc"
#include "Dispatchers/process_35.inc"
}
// b28d-solo-Dispatchers verdict=NEAR(routed) [S206 收敛 DIFF→NEAR] ORIG 0x0825fabc
//   [P0/P211/P205] 已修：删 msg 指针局部（形参槽直用）、first 去预清零、+0xd 改具名
//   视图（disp 直存）、循环地址改 `msg+(i+4)*4+1` 加法序；残余仅寄存器/槽名差
//   （P75/P112 结构性，助记符序列已全等）。
#include "Dispatchers/read_38.inc"
}
#include "Dispatchers/send_7.inc"

#include "Dispatchers/dtor_Dispatcher_Select_Card.inc"
#include "Dispatchers/dispatch_sig_27.inc"
}
#include "Dispatchers/process_36.inc"
#include "Dispatchers/read_39.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeItem.inc"
#include "Dispatchers/dispatch_sig_28.inc"
}
#include "Dispatchers/process_37.inc"
#include "Dispatchers/read_40.inc"

#include "Dispatchers/dtor_Dispatcher_AgreeEnchant.inc"
#include "Dispatchers/dispatch_sig_29.inc"
}
#include "Dispatchers/process_38.inc"
#include "Dispatchers/read_41.inc"

#include "Dispatchers/dtor_Dispatcher_Back2Village.inc"
#include "Dispatchers/dispatch_sig_30.inc"
}
#include "Dispatchers/process_39.inc"
#include "Dispatchers/read_42.inc"
#include "Dispatchers/check_error_15.inc"
}

#include "Dispatchers/dtor_Dispatcher_BossDieCheck.inc"
#include "Dispatchers/process_40.inc"
}
// b28d-solo-Dispatchers verdict=AE(routed) [S206 翻正] ORIG 0x081cd642
//   [N82→P0/P183] 根因=引用别名/msg 引用中间槽各占指针槽（帧 0x28→0x38）；
//   删引用别名改表达式直读 + 只留 pReq 单槽 ⇒ compare_tu clean 归零。
#include "Dispatchers/read_43.inc"
}
// param.m_c==2 先发命令错误包 0xb7；再组 header(0,0x73)：成功(m_4==0)回
// {1, m_8, m_a}，失败回 {0, m_4}；有队伍发全队，否则单发。
#include "Dispatchers/send_8.inc"
}

#include "Dispatchers/dtor_Dispatcher_CodeCheckSum.inc"
#include "Dispatchers/dispatch_sig_31.inc"
}
#include "Dispatchers/process_41.inc"
#include "Dispatchers/read_44.inc"
#include "Dispatchers/check_error_16.inc"
}

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
}

#include "Dispatchers/dtor_Dispatcher_CollectItems.inc"
#include "Dispatchers/process_42.inc"
}
#include "Dispatchers/read_45.inc"
}
// err==0x7fffffff 静默；err==0 回 header(1,0x1e3){1, m_8(signed), m_a}；
// 其余发命令错误包 0x1e3。
#include "Dispatchers/send_9.inc"
}

#include "Dispatchers/dtor_Dispatcher_DepositMoney.inc"
#include "Dispatchers/dispatch_sig_32.inc"
}
#include "Dispatchers/process_43.inc"
#include "Dispatchers/read_46.inc"

#include "Dispatchers/dtor_Dispatcher_EvadeAssault.inc"
#include "Dispatchers/check_error_17.inc"
}
// ORIG 0x081ce4f8（报告 Dispatcher_EvadeAssault/process.md）：
// err>0 → err；err<0 → -1；OnEvadeAssaultPlace==1 → 0；否则 LineFunc(0x27b8)。
#include "Dispatchers/process_44.inc"
}
#include "Dispatchers/read_47.inc"

#include "Dispatchers/dtor_Dispatcher_FairPvPScore.inc"
#include "Dispatchers/dispatch_sig_33.inc"
}
#include "Dispatchers/process_45.inc"
#include "Dispatchers/read_48.inc"

#include "Dispatchers/dtor_Dispatcher_HumanCertify.inc"
#include "Dispatchers/dispatch_sig_34.inc"
}
#include "Dispatchers/process_46.inc"
#include "Dispatchers/read_49.inc"

#include "Dispatchers/dtor_Dispatcher_InformNotice.inc"
#include "Dispatchers/check_error_18.inc"
}
// ORIG 0x081e2f78（报告 Dispatcher_InformNotice/process.md）：
// err=check_error → param.m_4；>0 返 0；<0 LineFunc(0x598c)；==0 handleInformNoticeFlag。
#include "Dispatchers/process_47.inc"
}
#include "Dispatchers/read_50.inc"
}
#include "Dispatchers/send_10.inc"
}

#include "Dispatchers/dtor_Dispatcher_JoinPowerWar.inc"
#include "Dispatchers/dispatch_sig_35.inc"
}
#include "Dispatchers/process_48.inc"
#include "Dispatchers/read_51.inc"

#include "Dispatchers/dtor_Dispatcher_MailBox_Open.inc"
#include "Dispatchers/check_error_19.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cc196 orig/ours=81/79 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
#include "Dispatchers/process_49.inc"
}
#include "Dispatchers/read_52.inc"
#include "Dispatchers/send_11.inc"
}



#include "Dispatchers/dtor_Dispatcher_MemberSecede.inc"
#include "Dispatchers/check_error_20.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cbc2e orig/ours=77/77 ndiff-blocks=12
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
#include "Dispatchers/process_50.inc"
}
#include "Dispatchers/read_53.inc"
}

#include "Dispatchers/dtor_Dispatcher_RequestMatch.inc"
#include "Dispatchers/dispatch_sig_36.inc"
}
#include "Dispatchers/process_51.inc"
#include "Dispatchers/read_54.inc"

#include "Dispatchers/dtor_Dispatcher_ScanBotByDll.inc"
#include "Dispatchers/check_error_21.inc"
}
// ORIG 0x082643b2（报告 Dispatcher_ScanBotByDll/read.md）[随行依赖]。
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x082643b2 orig/ours=51/51
#include "Dispatchers/read_55.inc"
}

// ORIG 0x08264488（报告 Dispatcher_ScanBotByDll/process.md）：
// check_error==0 后 SetETC(type+0x352, (char)msg+0x11)，再 addServerHackCnt。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08264488 orig/ours=56/59 ndiff-blocks=12
//   帧差 sub $30 vs $3c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0x14(%ebp) | cmpl   $0x0,-0x14(%ebp)
#include "Dispatchers/process_52.inc"
}

#include "Dispatchers/dtor_Dispatcher_ScanBotByDrv.inc"
#include "Dispatchers/check_error_22.inc"
#include "Dispatchers/process_53.inc"
}
#include "Dispatchers/read_56.inc"
}

#include "Dispatchers/dtor_Dispatcher_Secede_Power.inc"
#include "Dispatchers/dispatch_sig_37.inc"
}
#include "Dispatchers/process_54.inc"
#include "Dispatchers/read_57.inc"

#include "Dispatchers/dtor_Dispatcher_SoloTeleport.inc"

#include "Dispatchers/dtor_Dispatcher_TitleBookGet.inc"
#include "Dispatchers/process_55.inc"
}
#include "Dispatchers/read_58.inc"
}

#include "Dispatchers/dtor_Dispatcher_TitleBookPut.inc"
#include "Dispatchers/process_56.inc"
}
#include "Dispatchers/read_59.inc"
}

#include "Dispatchers/dtor_Dispatcher_UpgradeCargo.inc"
#include "Dispatchers/dispatch_sig_38.inc"
}
#include "Dispatchers/process_57.inc"
#include "Dispatchers/read_60.inc"

#include "Dispatchers/dtor_Dispatcher_UseLimitCube.inc"

#include "Dispatchers/dtor_Dispatcher_AssertManager.inc"
#include "Dispatchers/dispatch_sig_39.inc"
}
#include "Dispatchers/process_58.inc"
#include "Dispatchers/read_61.inc"

#include "Dispatchers/dtor_Dispatcher_CallGuildInfo.inc"
#include "Dispatchers/dispatch_sig_40.inc"
}
#include "Dispatchers/process_59.inc"
#include "Dispatchers/read_62.inc"

#include "Dispatchers/dtor_Dispatcher_ChangeEmotion.inc"
#include "Dispatchers/check_error_23.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d1b5e orig/ours=108/108 ndiff-blocks=5
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZTI10ENUM_ERROR,0x4(%esp) | movl   $&_ZTI10ENUM_ERROR,0x4(%esp) | mov    -0x10(%ebp),%eax
#include "Dispatchers/process_60.inc"
}

#include "Dispatchers/read_63.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d1d60 orig/ours=93/90 ndiff-blocks=18
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0xc(%ebp) | lea    -0x18(%ebp),%eax
#include "Dispatchers/send_12.inc"
}


#include "Dispatchers/dtor_Dispatcher_Clear_Used_QP.inc"
#include "Dispatchers/check_error_24.inc"
}

// ORIG 0x081d6786（报告 Dispatcher_Clear_Used_QP/process.md）：
// ParamBase 实际为 STQuestShopParam，+4 = m_4。
#include "Dispatchers/process_61.inc"
}
#include "Dispatchers/read_64.inc"
#include "Dispatchers/send_13.inc"
}


#include "Dispatchers/dtor_Dispatcher_ConnectP2PPvP.inc"
#include "Dispatchers/dispatch_sig_41.inc"
}

#include "Dispatchers/process_62.inc"
#include "Dispatchers/read_65.inc"

#include "Dispatchers/dtor_Dispatcher_DenyJoinGuild.inc"
#include "Dispatchers/process_63.inc"
}
#include "Dispatchers/read_66.inc"
}

#include "Dispatchers/dtor_Dispatcher_EnchantByBead.inc"
#include "Dispatchers/process_64.inc"
}
#include "Dispatchers/read_67.inc"
}
#include "Dispatchers/send_14.inc"
}

#include "Dispatchers/dtor_Dispatcher_GenCeraTicket.inc"
#include "Dispatchers/dispatch_sig_42.inc"
}
#include "Dispatchers/process_65.inc"
#include "Dispatchers/read_68.inc"
#include "Dispatchers/dtor_Dispatcher_GiveGiftToNPC.inc"
}
// dispatch_sig T 0x0821e4ac：恒返 0；read/process W 0x08233e52/0x08233e5c：恒返 0。
#include "Dispatchers/dispatch_sig_43.inc"
}
#include "Dispatchers/process_66.inc"
}
#include "Dispatchers/read_69.inc"
}

#include "Dispatchers/_getMaxFavorLevelNPCCnt.inc"
}

// ORIG W 0x0821e486：返回 (unsigned short)(baseFavor + addFavor)。
#include "Dispatchers/_getResultFavor.inc"
}

#include "Dispatchers/dtor_Dispatcher_GuildJoinList.inc"
#include "Dispatchers/process_67.inc"
}
#include "Dispatchers/read_70.inc"

#include "Dispatchers/dtor_Dispatcher_HatchCreature.inc"
#include "Dispatchers/check_error_25.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081cd446 orig/ours=86/89 ndiff-blocks=17
//   帧差 sub $20 vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x20,%esp | mov    %eax,-0x14(%ebp)
#include "Dispatchers/process_68.inc"
}
#include "Dispatchers/read_71.inc"
}
#include "Dispatchers/send_15.inc"
}

#include "Dispatchers/dtor_Dispatcher_JoinGuildInfo.inc"
#include "Dispatchers/process_69.inc"
}
#include "Dispatchers/read_72.inc"

#include "Dispatchers/dtor_Dispatcher_MouseRegister.inc"
#include "Dispatchers/check_error_26.inc"
}

// ORIG T 0x082632ca：长度 <7 且仅含 'a'/'0'..'9' 时有效（返回 int，无 bool 规范化 P9）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082632ca orig/ours=39/35 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | cmpl   $0x6,-0x10(%ebp) | movl   $0x0,-0xc(%ebp)
#include "Dispatchers/checkValiedPassword.inc"
}

// ORIG T 0x0826333a：经 CTEA 虚 Decrypt 解密；失败记日志返回 false。
// [b28c-31] N7：虚调用 callee ORIG %ecx vs 4.4 %eax + 实参装载寄存器 %edx↔%ecx
// （0x0826335x 段），探针/写法穷举不可翻（N7 判词）。
#include "Dispatchers/decryptPassword.inc"
}

// ORIG T 0x082633c6：密保卡 pad 答案复核链（shake 减扰 + checkAnswer + 计数/DB 上报）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082633c6 orig/ours=279/286 ndiff-blocks=22
//   帧差 sub $50 vs $60 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x50,%esp | mov    %al,-0x2c(%ebp) | movl   $0x0,-0xc(%ebp)
#include "Dispatchers/doPassPadReplay.inc"
}

// ==== Dispatcher_MouseRegister::dispatch_sig (ORIG 0x082637fe) ====
// §6.2 巨件核销：P167 89/89 [0缺0多]；gap 636→662（+26 全帧 N82 槽号错位，本帧
// 4096×2 解密缓冲使 4.4 vs 4.1 槽分配天然漂移）；残差归因 N82（帧/槽号+寄存器），
// 语义完整（详见 docs/asm_translation_patterns.md 负例表）。真修：
// ① pkgType==3 分支补 operator* ×2→operator<< ×2→operator-> 写入链（P103/P5，
//   原简写致 P167 -2operator*/-1operator->）；主成功路径同形补齐；
// ② 删我方多缺 memset(&bufView)（ORIG 无——m_encPwd 由 get_binary、m_encPwdCopy
//   由 memcpy 覆盖，pkgType 槽不被读）；
// ③ 始段删 user==0 守卫（ORIG 无，get_state 两次独立调用双 je 同目标，P133）；
// ④ cryptoType 三守卫 !(a&&b&&c) 还原为 ORIG 独立 ==0/>0xd/==5 判（P133）；
// ⑤ Decrypt 双调用返回处理还原 ORIG「首次 jne 汇聚1、次次 je 汇聚0」单出口发错
//   （P48 独立无 setne 物化）；
// ⑥ checkValiedPassword ×2 还原 bool 汇聚物化（xor$1/test 首项 jne 尾项 je，P22）。
// PacketDispatcher_Secu.cpp 密保卡注册链：包类型分发 → 解密双密码 → shake 减扰
// → checkValiedPassword ×2 → 写 SIG_MOUSE_REGISTER 进 DB_Q。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082637fe orig/ours=636/660 ndiff-blocks=82
//   帧差 sub $20bc vs $20cc + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: movl   $&_ZZN24Dispatcher_MouseRegister12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | lea    -0x89(%ebp),%eax | movl   $&_ZZN24Di
#include "Dispatchers/dispatch_sig_44.inc"
}

// ==== DB_GoblinPadUpdateReward::makeRequest (ORIG T 0x08424452) ====
// 报告 DB_GoblinPadUpdateReward/makeRequest.md：Acquire("DBThread.cpp",0x5fc3)、
// 头 0x12b/-1、SIG_GOBLIN_UPDATE_REWARD{accId,+4 byte,+5 bool,strncpy(web,0x10)}。
#include "Dispatchers/makeRequest_2.inc"
}

#include "Dispatchers/dtor_Dispatcher_ObjectBringUp.inc"

#include "Dispatchers/dtor_Dispatcher_Overflow_Info.inc"
#include "Dispatchers/dispatch_sig_45.inc"
}
#include "Dispatchers/process_70.inc"
#include "Dispatchers/read_73.inc"


#include "Dispatchers/dtor_Dispatcher_QuickJoinRoom.inc"
#include "Dispatchers/dispatch_sig_46.inc"
}
#include "Dispatchers/process_71.inc"
#include "Dispatchers/read_74.inc"

#include "Dispatchers/dtor_Dispatcher_SelectStriker.inc"
#include "Dispatchers/check_error_27.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e3dca orig/ours=178/165 ndiff-blocks=22
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0x14(%ebp)
#include "Dispatchers/process_72.inc"
}
#include "Dispatchers/read_75.inc"
}
#include "Dispatchers/send_16.inc"
}

#include "Dispatchers/dtor_Dispatcher_UseCraneStart.inc"
#include "Dispatchers/dispatch_sig_47.inc"
}

#include "Dispatchers/process_73.inc"
#include "Dispatchers/read_76.inc"

#include "Dispatchers/dtor_Dispatcher_UseRenameCard.inc"
#include "Dispatchers/check_error_28.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e731a orig/ours=149/151 ndiff-blocks=22
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | mov    0x14(%ebp),%eax | mov    %eax,-0x18(%ebp)
#include "Dispatchers/process_74.inc"
}
#include "Dispatchers/read_77.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e756c orig/ours=76/73 ndiff-blocks=8
//   帧差 sub $60 vs $64 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x60,%esp | mov    %ax,-0xc(%ebp)
#include "Dispatchers/getItem.inc"
}

#include "Dispatchers/dtor_Dispatcher_WithdrawMoney.inc"
#include "Dispatchers/dispatch_sig_48.inc"
}
#include "Dispatchers/process_75.inc"
#include "Dispatchers/read_78.inc"

#include "Dispatchers/dtor_Dispatcher_AuctionBidding.inc"
#include "Dispatchers/dispatch_sig_49.inc"
}
#include "Dispatchers/process_76.inc"
#include "Dispatchers/read_79.inc"

#include "Dispatchers/dtor_Dispatcher_BuyAutomatItem.inc"
#include "Dispatchers/dispatch_sig_50.inc"
}
#include "Dispatchers/process_77.inc"
#include "Dispatchers/read_80.inc"

#include "Dispatchers/dtor_Dispatcher_CallMemberList.inc"
#include "Dispatchers/check_error_29.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cbda2 orig/ours=66/69 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0xc(%ebp) | mov    %eax,%edx
#include "Dispatchers/process_78.inc"
}
#include "Dispatchers/read_81.inc"

#include "Dispatchers/dtor_Dispatcher_CheckJoinGuild.inc"
#include "Dispatchers/process_79.inc"
}
#include "Dispatchers/read_82.inc"
}

#include "Dispatchers/dtor_Dispatcher_CompoundEmblem.inc"


#include "Dispatchers/dtor_Dispatcher_DisJointAvatar.inc"
#include "Dispatchers/dispatch_sig_51.inc"
}
#include "Dispatchers/process_80.inc"
#include "Dispatchers/read_83.inc"

#include "Dispatchers/dtor_Dispatcher_DisguiseCancel.inc"
#include "Dispatchers/dispatch_sig_52.inc"
}
#include "Dispatchers/process_81.inc"
#include "Dispatchers/read_84.inc"

#include "Dispatchers/dtor_Dispatcher_HellPartyStart.inc"
#include "Dispatchers/dispatch_sig_53.inc"
}
#include "Dispatchers/process_82.inc"
#include "Dispatchers/read_85.inc"

#include "Dispatchers/dtor_Dispatcher_ItemDictionary.inc"
#include "Dispatchers/check_error_30.inc"
}
// ORIG 0x081dd7fe（报告 Dispatcher_ItemDictionary/process.md）：
// err>0 直接 0；err<0 记 LineFunc(0x4e52)；否则 0。
// b28c-L20: AE — 具名 stItemDictionary* 局部（-0x10 槽）+ 三次重读 p->m_4；
//   >0 早返 0 / <0 return LineFunc / ==0 落 msg 死槽后 return 0（P0 槽往返判读）。
#include "Dispatchers/process_83.inc"
}
#include "Dispatchers/read_86.inc"
}
#include "Dispatchers/send_17.inc"
}

#include "Dispatchers/dtor_Dispatcher_Lag_Statistics.inc"
#include "Dispatchers/dispatch_sig_54.inc"
#define base (reinterpret_cast<unsigned char*>(&local_161))
    unsigned char count = 0;                                 // -0x153 = base+0xa
    if (!packet.get_byte(count))
    {
        return LineFunc(0xde9f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (8 <= count)
    {
        return LineFunc(0xdea1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char idx = 0;                                   // -0x11（循环内局部）
    for (int i = 0; i < (int)(unsigned int)count; ++i)
    {
        if (!packet.get_byte(idx))
        {
            return LineFunc(0xdea7,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (7 < idx)
        {
            return LineFunc(0xdeab,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(*reinterpret_cast<int*>(base + 0xb + idx * 10)))
        {
            return LineFunc(0xdead,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(*reinterpret_cast<int*>(base + 0xb + idx * 10 + 4)))
        {
            return LineFunc(0xdeae,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0xb + idx * 10 + 8)))
        {
            return LineFunc(0xdeaf,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (idx == 1)
        {
            unsigned char count2 = 0;                        // -0x102 = base+0x5b
            if (!packet.get_byte(count2))
            {
                return LineFunc(0xdeb4,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            if (10 < count2)
            {
                return LineFunc(0xdeb6,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            for (int j = 0; j < (int)(unsigned int)count2; ++j)
            {
                if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0x50 + j * 0x18 + 0xc)))
                {
                    return LineFunc(0xdebc,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_byte(*reinterpret_cast<unsigned char*>(base + 0x50 + j * 0x18 + 0xe)))
                {
                    return LineFunc(0xdebd,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(*reinterpret_cast<int*>(base + 0x50 + j * 0x18 + 0xf)))
                {
                    return LineFunc(0xdebe,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(*reinterpret_cast<int*>(base + 0x50 + j * 0x18 + 0x13)))
                {
                    return LineFunc(0xdebf,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0x50 + j * 0x18 + 0x17)))
                {
                    return LineFunc(0xdec0,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_byte(*reinterpret_cast<unsigned char*>(base + 0x50 + j * 0x18 + 0x19)))
                {
                    return LineFunc(0xdec1,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(*reinterpret_cast<int*>(base + 0x50 + j * 0x18 + 0x1a)))
                {
                    return LineFunc(0xdec2,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(*reinterpret_cast<int*>(base + 0x50 + j * 0x18 + 0x1e)))
                {
                    return LineFunc(0xdec3,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0x50 + j * 0x18 + 0x22)))
                {
                    return LineFunc(0xdec4,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
            }
        }
    }
    CStatisticServerProxy* proxy = GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(base), 0x14c);
    return 0;
}
#undef base
#include "Dispatchers/process_84.inc"
#include "Dispatchers/read_87.inc"

#include "Dispatchers/dtor_Dispatcher_Mercenary_Info.inc"
#include "Dispatchers/process_85.inc"
}

// ORIG 0x081ddb3c (9 insn): 空实现，无消息体
#include "Dispatchers/read_88.inc"

#include "Dispatchers/send_18.inc"
}

#include "Dispatchers/dtor_Dispatcher_P2P_Statistics.inc"
#include "Dispatchers/check_error_31.inc"
}

// ===== Dispatcher_GuildCargoPopItem::process ===== ORIG 0x081d15b2 (0x195)
// 报告 Dispatcher_GuildCargoPopItem/process.md 全文转写。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e2bde orig/ours=185/179 ndiff-blocks=30
//   帧差 sub $fc vs $dc + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0xfc,%esp | mov    %eax,-0x1c(%ebp) | lea    -0x9b(%ebp),%eax
#include "Dispatchers/process_86.inc"
}

#include "Dispatchers/read_89.inc"
}



#include "Dispatchers/dtor_Dispatcher_PvPChannelInfo.inc"
#include "Dispatchers/dispatch_sig_55.inc"
}
#include "Dispatchers/process_87.inc"
#include "Dispatchers/read_90.inc"

#include "Dispatchers/dtor_Dispatcher_RemovePvPBuddy.inc"
#include "Dispatchers/dispatch_sig_56.inc"
}

#include "Dispatchers/process_88.inc"
#include "Dispatchers/read_91.inc"

#include "Dispatchers/dtor_Dispatcher_RenameCreature.inc"
#include "Dispatchers/check_error_32.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081cd110 orig/ours=79/78 ndiff-blocks=14
//   帧差 sub $2c vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | push   %ebx
#include "Dispatchers/process_89.inc"
}
#include "Dispatchers/read_92.inc"
}
#include "Dispatchers/send_19.inc"
}

#include "Dispatchers/dtor_Dispatcher_UseBoosterItem.inc"


#include "Dispatchers/dtor_Dispatcher_UserHistoryLog.inc"
#include "Dispatchers/process_90.inc"
}
#include "Dispatchers/read_93.inc"
}

#include "Dispatchers/dtor_Dispatcher_BuyCeraShopItem.inc"


#include "Dispatchers/dtor_Dispatcher_CallGuildInvite.inc"
#include "Dispatchers/dispatch_sig_57.inc"
}
#include "Dispatchers/process_91.inc"
#include "Dispatchers/read_94.inc"

#include "Dispatchers/dtor_Dispatcher_CancelJoinGuild.inc"
#include "Dispatchers/process_92.inc"
}
#include "Dispatchers/read_95.inc"
}

#include "Dispatchers/dtor_Dispatcher_CeraPackageOpen.inc"

#include "Dispatchers/dtor_Dispatcher_ChangeGuildName.inc"
#include "Dispatchers/dispatch_sig_58.inc"
}
#include "Dispatchers/process_93.inc"
#include "Dispatchers/read_96.inc"

#include "Dispatchers/dtor_Dispatcher_CompleteDisplay.inc"
#include "Dispatchers/dispatch_sig_59.inc"
}
#include "Dispatchers/process_94.inc"
#include "Dispatchers/read_97.inc"

#include "Dispatchers/dtor_Dispatcher_CompleteLoadPvP.inc"
#include "Dispatchers/dispatch_sig_60.inc"
}
#include "Dispatchers/process_95.inc"
#include "Dispatchers/read_98.inc"

#include "Dispatchers/dtor_Dispatcher_Compound_Avatar.inc"
#include "Dispatchers/dispatch_sig_61.inc"
}
#include "Dispatchers/process_96.inc"
#include "Dispatchers/read_99.inc"

#include "Dispatchers/dtor_Dispatcher_DieBloodMonster.inc"
#include "Dispatchers/dispatch_sig_62.inc"
}
#include "Dispatchers/process_97.inc"
#include "Dispatchers/read_100.inc"

#include "Dispatchers/dtor_Dispatcher_DisguiseRequest.inc"
#include "Dispatchers/dispatch_sig_63.inc"
}
#include "Dispatchers/process_98.inc"
#include "Dispatchers/read_101.inc"

#include "Dispatchers/dtor_Dispatcher_DonateGuildFund.inc"
#include "Dispatchers/process_99.inc"
}
#include "Dispatchers/read_102.inc"
}

#include "Dispatchers/dtor_Dispatcher_GiveupExpertJob.inc"
#include "Dispatchers/dispatch_sig_64.inc"
}
#include "Dispatchers/process_100.inc"
#include "Dispatchers/read_103.inc"

#include "Dispatchers/dtor_Dispatcher_GoblinPadStatus.inc"
#include "Dispatchers/dispatch_sig_65.inc"
}
#include "Dispatchers/process_101.inc"
#include "Dispatchers/read_104.inc"

#include "Dispatchers/dtor_Dispatcher_LimitNpcBuyItem.inc"
#include "Dispatchers/check_error_33.inc"
}
// ORIG 0x081e0ff8 T _ZN26Dispatcher_LimitNpcBuyItem7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_LimitNpcBuyItem/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 stLimitNpcBuyItemParam，ORIG 裸访问 +0x04 错误码槽（m_4）。
// local_3a = LimitNpcBuyItemInfo（ORIG 0x26 字节，Monitor/LimitNpcBuyItem.h 权威布局）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e0ff8 orig/ours=73/71 ndiff-blocks=15
//   帧差 sub $44 vs $54 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x44,%esp | mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%edx
#include "Dispatchers/process_102.inc"
}
#include "Dispatchers/read_105.inc"
}
#include "Dispatchers/send_20.inc"
}

#include "Dispatchers/dtor_Dispatcher_MailBox_Extract.inc"
#include "Dispatchers/check_error_34.inc"
}

// ORIG 0x081cce50（报告 Dispatcher_MailBox_Extract/process.md）：
// msg+0xd（unsigned）为邮箱取件索引。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cce50 orig/ours=48/40 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,%edx | mov    -0x10(%ebp),%eax
#include "Dispatchers/process_103.inc"
}
#include "Dispatchers/read_106.inc"
}
#include "Dispatchers/send_21.inc"
}


#include "Dispatchers/dtor_Dispatcher_PVPRequestFight.inc"
#include "Dispatchers/dispatch_sig_66.inc"
}
#include "Dispatchers/process_104.inc"
#include "Dispatchers/read_107.inc"

#include "Dispatchers/dtor_Dispatcher_QueryCharacInfo.inc"
#include "Dispatchers/dispatch_sig_67.inc"
}

#include "Dispatchers/process_105.inc"
#include "Dispatchers/read_108.inc"

#include "Dispatchers/dtor_Dispatcher_ReportAbuseUser.inc"
#include "Dispatchers/dispatch_sig_68.inc"
}
#include "Dispatchers/process_106.inc"
#include "Dispatchers/read_109.inc"

#include "Dispatchers/dtor_Dispatcher_RequestItemLock.inc"
#include "Dispatchers/dispatch_sig_69.inc"
}

#include "Dispatchers/process_107.inc"
#include "Dispatchers/read_110.inc"
#include "Dispatchers/dtor_Dispatcher_SaveQuestNotify.inc"
#include "Dispatchers/check_error_35.inc"
}

// ORIG 0x0825ed06：读 1 字节 count（须 0..5），再按 count 读 int 写入
// msg+0xd 起任务 idx 区；任一步失败走 LineFunc（ORIG 返回其断言值）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825ed06 orig/ours=62/67 ndiff-blocks=8
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x10(%ebp) | movl   $&_ZZN26Dispatcher_SaveQuestNotify4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp
#include "Dispatchers/read_111.inc"
}

// ORIG 0x0825edfc：err=check_error(user,msg)；*(int*)(param+4)=err；
// err>0 → 0；err<0 → LogManager::logFormat(1,"PacketDispatcher_Impl_2.cpp",
// <PRETTY>,0x2cb,"Dispatcher Check_Error No(%d)",err) 后 return 0x2cc；
// err==0 → memcpy(getCurCharacQuestW()->m_authenData.m_authen, msg+0xd, 0x28)
// （UserQuest+0x75f4，10×int 任务通知区）后 return 0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825edfc orig/ours=55/50 ndiff-blocks=9
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0x14(%ebp),%edx | mov    %eax,0x4(%edx) | mov    0x4(%eax),%eax
#include "Dispatchers/process_108.inc"
}

#include "Dispatchers/dtor_Dispatcher_SecuDataControl.inc"
#include "Dispatchers/dispatch_sig_70.inc"
}
#include "Dispatchers/process_109.inc"
#include "Dispatchers/read_112.inc"

#include "Dispatchers/dtor_Dispatcher_Security_IPScan.inc"
#include "Dispatchers/dispatch_sig_71.inc"
}
#include "Dispatchers/process_110.inc"
#include "Dispatchers/read_113.inc"

#include "Dispatchers/dtor_Dispatcher_Shop_Coin_Event.inc"
#include "Dispatchers/dispatch_sig_72.inc"
}
#include "Dispatchers/process_111.inc"
#include "Dispatchers/read_114.inc"

#include "Dispatchers/dtor_Dispatcher_TutorialLevelUp.inc"
#include "Dispatchers/process_112.inc"
}
#include "Dispatchers/read_115.inc"

#include "Dispatchers/dtor_Dispatcher_ApproveJoinGuild.inc"
#include "Dispatchers/process_113.inc"
}
#include "Dispatchers/read_116.inc"
}

#include "Dispatchers/dtor_Dispatcher_CallGuildLevelUp.inc"
#include "Dispatchers/dispatch_sig_73.inc"
}
#include "Dispatchers/process_114.inc"
#include "Dispatchers/read_117.inc"

#include "Dispatchers/dtor_Dispatcher_CallGuildMembers.inc"
#include "Dispatchers/dispatch_sig_74.inc"
}
#include "Dispatchers/process_115.inc"
#include "Dispatchers/read_118.inc"

#include "Dispatchers/dtor_Dispatcher_ChangeCharacName.inc"
#include "Dispatchers/process_116.inc"
#include "Dispatchers/read_119.inc"

#include "Dispatchers/dtor_Dispatcher_ChangeLetterStat.inc"
#include "Dispatchers/check_error_36.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081ce1fc orig/ours=59/69 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
#include "Dispatchers/process_117.inc"
}
#include "Dispatchers/read_120.inc"
}

#include "Dispatchers/send_22.inc"
}

#include "Dispatchers/dtor_Dispatcher_DieAssaultPlayer.inc"
#include "Dispatchers/dispatch_sig_75.inc"
}
#include "Dispatchers/process_118.inc"
#include "Dispatchers/read_121.inc"

#include "Dispatchers/dtor_Dispatcher_Equipslot_Switch.inc"
#include "Dispatchers/check_error_37.inc"
}
// err>0 → 错误包(0x18d,err) 返 -1；err<0 → 错误包(0x18d,0x15)+LineFunc(0x3e2c)；
// 否则对扩展槽视图(#9)执行 EquipslotSwitch(msg+0xf,msg+0xd,msg+0xe)，结果字节
// 回写 param.m_4。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d61be orig/ours=68/72 ndiff-blocks=11
//   帧差 sub $38 vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x38,%esp | mov    %eax,-0x18(%ebp) | cmpl   $0x0,-0x18(%ebp)
#include "Dispatchers/process_119.inc"
}
// b28d-solo-Dispatchers verdict=AE(routed) [S206 翻正] ORIG 0x081d606e
//   [N82→P0/P183] 同 BossDieCheck::read：删 msg 引用与三字段引用别名 ⇒ 归零。
#include "Dispatchers/read_122.inc"
}
// 成功先补发两份装备槽视图，再统一回 header(1,0x18d)：成功 {1}，失败 {0,m_4}。
#include "Dispatchers/send_23.inc"
}

// ============================================================================
// NEEDS_DECL: 建议追加到 Dispatchers.h 的声明（body 引用但现有声明缺失）：
//   class Dispatcher_BossDieCheck 公有段：
//     int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
//       // ORIG _ZN23Dispatcher_BossDieCheck11check_errorEP5CUserR8MSG_BASER9ParamBase T 0x081cd71e
//   class Dispatcher_CollectItems 公有段：
//     int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
//       // ORIG _ZN23Dispatcher_CollectItems11check_errorEP5CUserR8MSG_BASER9ParamBase T 0x081e329e
//   class Dispatcher_ComboSkill 公有段：
//     int checkComboSkillIndex(short idx);
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
}
#include "Dispatchers/process_120.inc"
#include "Dispatchers/read_123.inc"

#include "Dispatchers/dtor_Dispatcher_ExpertExtraction.inc"

#include "Dispatchers/dtor_Dispatcher_InformNotice_2nd.inc"
#include "Dispatchers/check_error_38.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e30fa orig/ours=50/51 ndiff-blocks=9
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x14(%ebp) | mov    %eax,0x4(%edx)
#include "Dispatchers/process_121.inc"
}
#include "Dispatchers/read_124.inc"
}
#include "Dispatchers/send_24.inc"
}

#include "Dispatchers/dtor_Dispatcher_MemberEnterReply.inc"
#include "Dispatchers/check_error_39.inc"
}
// ORIG 0x081cba34 T _ZN27Dispatcher_MemberEnterReply7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_MemberEnterReply/process.md 汇编+反编译 C 全文转写）
// param 为模板实参，ORIG 裸访问 +0x04 错误码槽（m_4）。
// local_27 = Packet_Monitor_Member_Enter_Reply（ORIG 0x13 字节，shared/packet 权威布局）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cba34 orig/ours=71/69 ndiff-blocks=14
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | mov    %eax,%edx | mov    -0x10(%ebp),%eax
#include "Dispatchers/process_122.inc"
}
#include "Dispatchers/read_125.inc"
}

#include "Dispatchers/dtor_Dispatcher_Mercenary_Return.inc"
#include "Dispatchers/process_123.inc"
}

#include "Dispatchers/read_126.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081dda1c orig/ours=86/87 ndiff-blocks=19
//   帧差 sub $20 vs $30 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x20,%esp | lea    -0x18(%ebp),%eax | lea    -0x18(%ebp),%eax
#include "Dispatchers/send_25.inc"
}

#include "Dispatchers/dtor_Dispatcher_MultiMailBoxSend.inc"
#include "Dispatchers/CheckTradeLimitItemCountToMove.inc"
}

// ORIG T 0x0812032e：
// [b28c-lane24] ORIG 尾 `leave; ret` 无返回值设置 + Ghidra C `void`（P159：
// 以 ORIG 函数尾判定真实返回类型）⇒ void；原 bool + `return ok` 使 4.4 在
// 两出口各读 ok 引用（+3 条）。全部 4 个调用点均为语句调用（不消费返回值）。
#include "Dispatchers/CheckItemTradeLimitCountToMoveByUser.inc"
}

// ORIG 0x082262a2：多附件邮件发送（PacketDispatcher_Impl_1.cpp 0xe1ea 区）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x082262a2 orig/ours=593/543 ndiff-blocks=68
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 8 个，4.4 libstdc++ 全内联不可复现
//   首差: mov    &_ZN10GlobalData22s_pSecuProtectionFieldE,%eax | mov    %eax,-0x20(%ebp) | cmpl   $0x0,-0x20(%ebp)
#include "Dispatchers/dispatch_sig_77.inc"
}
#include "Dispatchers/process_124.inc"
#include "Dispatchers/read_127.inc"

#include "Dispatchers/dtor_Dispatcher_Open_Guild_Board.inc"
#include "Dispatchers/process_125.inc"
}
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x081d307e orig/ours=21/21
#include "Dispatchers/read_128.inc"
}

#include "Dispatchers/dtor_Dispatcher_PvPBuddyConnList.inc"
#include "Dispatchers/dispatch_sig_78.inc"
}
#include "Dispatchers/process_126.inc"
#include "Dispatchers/read_129.inc"

#include "Dispatchers/dtor_Dispatcher_RefreshGuildInfo.inc"
#include "Dispatchers/process_127.inc"
}
#include "Dispatchers/read_130.inc"

#include "Dispatchers/dtor_Dispatcher_ReplyGuildInvite.inc"
#include "Dispatchers/dispatch_sig_79.inc"
}
#include "Dispatchers/process_128.inc"
#include "Dispatchers/read_131.inc"

#include "Dispatchers/dtor_Dispatcher_ReportBadP2PUser.inc"
#include "Dispatchers/dispatch_sig_80.inc"
}
#include "Dispatchers/process_129.inc"
#include "Dispatchers/read_132.inc"

#include "Dispatchers/dtor_Dispatcher_Req_Urgent_Quest.inc"
#include "Dispatchers/process_130.inc"
}
// ORIG 0x081df422：字面 return 0（无包内容需解析）。
#include "Dispatchers/read_133.inc"
#include "Dispatchers/send_26.inc"
}

#include "Dispatchers/dtor_Dispatcher_RequestBlackList.inc"
#include "Dispatchers/check_error_40.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081ce070 orig/ours=66/69 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0xc(%ebp) | mov    %eax,%edx
#include "Dispatchers/process_131.inc"
}
#include "Dispatchers/read_134.inc"

#include "Dispatchers/dtor_Dispatcher_RequestJoinGuild.inc"
#include "Dispatchers/process_132.inc"
}
#include "Dispatchers/read_135.inc"
}


#include "Dispatchers/dtor_Dispatcher_ResponseCreature.inc"
#include "Dispatchers/dispatch_sig_81.inc"
}
#include "Dispatchers/process_133.inc"
#include "Dispatchers/read_136.inc"

#include "Dispatchers/dtor_Dispatcher_SDC_Damage_Check.inc"
#include "Dispatchers/dispatch_sig_82.inc"
}
#include "Dispatchers/process_134.inc"
#include "Dispatchers/read_137.inc"

#include "Dispatchers/dtor_Dispatcher_SaveGameOption_1.inc"
#include "Dispatchers/check_error_41.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081ceb38 orig/ours=149/143 ndiff-blocks=30
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x18(%ebp) | cmpl   $0x0,-0x18(%ebp) | mov    -0x18(%ebp),%ebx
#include "Dispatchers/process_135.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cea7a orig/ours=51/54 ndiff-blocks=6
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax | mov    -0xc(%ebp),%eax
#include "Dispatchers/read_138.inc"
}

#include "Dispatchers/dtor_Dispatcher_SaveGameOption_2.inc"
#include "Dispatchers/check_error_42.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cee64 orig/ours=138/138 ndiff-blocks=8
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %al,-0xd(%ebp) | movzbl -0xd(%ebp),%eax | xor    $0x1,%eax
#include "Dispatchers/process_136.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081ceda4 orig/ours=51/54 ndiff-blocks=6
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax | mov    -0xc(%ebp),%eax
#include "Dispatchers/read_139.inc"
}

#include "Dispatchers/dtor_Dispatcher_UpgradeChronicle.inc"
#include "Dispatchers/dispatch_sig_83.inc"
}
#include "Dispatchers/process_137.inc"
#include "Dispatchers/read_140.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeInventory.inc"
#include "Dispatchers/dispatch_sig_84.inc"
}
#include "Dispatchers/process_138.inc"
#include "Dispatchers/read_141.inc"

#include "Dispatchers/dtor_Dispatcher_AddSocketToAvatar.inc"
#include "Dispatchers/process_139.inc"
#include "Dispatchers/read_142.inc"

#include "Dispatchers/dtor_Dispatcher_AuctionRegistItem.inc"


#include "Dispatchers/dtor_Dispatcher_ChangeHostWarRoom.inc"
#include "Dispatchers/check_error_43.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d73f6 orig/ours=64/64 ndiff-blocks=14
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    %eax,-0x18(%ebp)
#include "Dispatchers/process_140.inc"
}
#include "Dispatchers/read_143.inc"
}
#include "Dispatchers/send_27.inc"
}

#include "Dispatchers/dtor_Dispatcher_ConnectP2PAssault.inc"
#include "Dispatchers/dispatch_sig_85.inc"
}

#include "Dispatchers/process_141.inc"
#include "Dispatchers/read_144.inc"

#include "Dispatchers/dtor_Dispatcher_Debug_Clear_Quest.inc"
#include "Dispatchers/process_142.inc"
}
// ORIG 0x081df840：get_short 读数量写入 msg.m_count（+0x10），再逐个
// get_short push_back 进 m_questIdxs；失败走 LineFunc(0x51e1/0x51e8)。
#include "Dispatchers/read_145.inc"
}
// ORIG 0x081dfb40：PacketGuard put_header(1,0x1c9)+put_byte(1)+
// put_short(res.m_index.m_count)；按 count put_short 每个 idx；
// finalize(true)→Send→send_clear_quest_list()；随后第二个 PacketGuard 经
// UserQuest::get_quest_info 填充任务列表包并发送。
#include "Dispatchers/send_28.inc"
}

#include "Dispatchers/dtor_Dispatcher_DeleteToBlackList.inc"
#include "Dispatchers/check_error_44.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cdf0a orig/ours=85/84 ndiff-blocks=4
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,%edx | mov    -0x10(%ebp),%eax | mov    %edx,0x4(%eax)
#include "Dispatchers/process_143.inc"
}
#include "Dispatchers/read_146.inc"
}

#include "Dispatchers/dtor_Dispatcher_EnterPrivateStore.inc"
#include "Dispatchers/dispatch_sig_86.inc"
}
#include "Dispatchers/process_144.inc"
#include "Dispatchers/read_147.inc"

#include "Dispatchers/dtor_Dispatcher_FatigueAttendance.inc"

#include "Dispatchers/dtor_Dispatcher_GuildCargoHistory.inc"
#include "Dispatchers/process_145.inc"
}
#include "Dispatchers/read_148.inc"
#include "Dispatchers/send_29.inc"
}

#include "Dispatchers/dtor_Dispatcher_GuildCargoPopItem.inc"
#include "Dispatchers/process_146.inc"
}

#include "Dispatchers/read_149.inc"
}
#include "Dispatchers/send_30.inc"
}

#include "Dispatchers/dtor_Dispatcher_LoadingTimeReport.inc"
#include "Dispatchers/dispatch_sig_87.inc"
#define base (reinterpret_cast<unsigned char*>(&local_824))
    unsigned short idx = 0;                            // -0x16
    int i = 0;                                         // -0x14（循环计数）
    while (i <= 8)
    {
        if (!packet.get_short(idx))
        {
            return LineFunc(0xd7fe,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (8 < idx)
        {
            return LineFunc(0xd800,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (idx < 7)
        {
            if (!packet.get_int(*reinterpret_cast<unsigned int*>(base + 0xa + idx * 4)))
            {
                return LineFunc(0xd806,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
        }
        *reinterpret_cast<unsigned int*>(base + 0x2e) = user->get_acc_id();  // -0x7f2
        if (idx == 7)
        {
            short count = 0;                           // -0x18
            if (!packet.get_short(count))
            {
                return LineFunc(0xd80c,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            if (99 < count)
            {
                return LineFunc(0xd80d,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            *reinterpret_cast<short*>(base + 0x32) = count;   // -0x7ee
            for (int j = 0; j < count; ++j)            // -0x10；ORIG cwtl 后有符号 setg
            {
                if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0x30 + j * 9 + 4)))
                {
                    return LineFunc(0xd811,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_byte(*reinterpret_cast<unsigned char*>(base + 0x30 + j * 9 + 6)))
                {
                    return LineFunc(0xd812,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0x30 + j * 9 + 7)))
                {
                    return LineFunc(0xd813,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0x30 + j * 9 + 9)))
                {
                    return LineFunc(0xd814,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0x30 + j * 9 + 0xb)))
                {
                    return LineFunc(0xd815,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
            }
            *reinterpret_cast<unsigned int*>(base + 0xa + 7 * 4) = 0;   // auStack_81a[7]=0
        }
        if (idx == 8)
        {
            short count = 0;                           // -0x1a
            if (!packet.get_short(count))
            {
                return LineFunc(0xd81c,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            if (99 < count)
            {
                return LineFunc(0xd81d,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            *reinterpret_cast<short*>(base + 0x3b8) = count;   // -0x468
            for (int j = 0; j < count; ++j)            // -0xc；ORIG cwtl 后有符号 setg
            {
                if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0x3b0 + j * 0xb + 0xa)))
                {
                    return LineFunc(0xd821,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_byte(*reinterpret_cast<unsigned char*>(base + 0x3b0 + j * 0xb + 0xc)))
                {
                    return LineFunc(0xd822,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(*reinterpret_cast<unsigned int*>(base + 0x3b0 + j * 0xb + 0xd)))
                {
                    return LineFunc(0xd823,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                if (!packet.get_int(*reinterpret_cast<unsigned int*>(base + 0x3b0 + j * 0xb + 0x11)))
                {
                    return LineFunc(0xd824,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
            }
            *reinterpret_cast<unsigned int*>(base + 0xa + 8 * 4) = 0;   // auStack_81a[8]=0
        }
        ++i;
    }
    CStatisticServerProxy* proxy = GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(base), 0x806);
    return 0;
}
#undef base
#include "Dispatchers/process_147.inc"
#include "Dispatchers/read_150.inc"

#include "Dispatchers/dtor_Dispatcher_PeerConnectResult.inc"
#include "Dispatchers/dispatch_sig_88.inc"
}
#include "Dispatchers/process_148.inc"
#include "Dispatchers/read_151.inc"

#include "Dispatchers/dtor_Dispatcher_PutItemForEnchant.inc"
#include "Dispatchers/dispatch_sig_89.inc"
}
#include "Dispatchers/process_149.inc"
#include "Dispatchers/read_152.inc"

#include "Dispatchers/dtor_Dispatcher_RequestItemUnlock.inc"
#include "Dispatchers/dispatch_sig_90.inc"
}

#include "Dispatchers/process_150.inc"
#include "Dispatchers/read_153.inc"

#include "Dispatchers/dtor_Dispatcher_SecretShopBuyItem.inc"
#include "Dispatchers/process_151.inc"
}
#include "Dispatchers/read_154.inc"
}
#include "Dispatchers/send_31.inc"
}

#include "Dispatchers/dtor_Dispatcher_SecurityCardIssue.inc"
#include "Dispatchers/dispatch_sig_91.inc"
}
#include "Dispatchers/process_152.inc"
#include "Dispatchers/read_155.inc"

#include "Dispatchers/dtor_Dispatcher_SetSubGuildMaster.inc"
#include "Dispatchers/dispatch_sig_92.inc"
}
#include "Dispatchers/process_153.inc"
#include "Dispatchers/read_156.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeGuildCargo.inc"
#include "Dispatchers/dispatch_sig_93.inc"
}
#include "Dispatchers/process_154.inc"
#include "Dispatchers/read_157.inc"

#include "Dispatchers/Dispatcher_UseVendingMachine.inc"
}
#include "Dispatchers/dtor_Dispatcher_UseVendingMachine.inc"


#include "Dispatchers/dtor_Dispatcher_AchievementTrigger.inc"
#include "Dispatchers/process_155.inc"
}
#include "Dispatchers/read_158.inc"
}

#include "Dispatchers/dtor_Dispatcher_Antibot_DPCallBack.inc"
#include "Dispatchers/dispatch_sig_94.inc"
}
#include "Dispatchers/process_156.inc"
#include "Dispatchers/read_159.inc"


#include "Dispatchers/dtor_Dispatcher_BiddingRoutingItem.inc"
#include "Dispatchers/dispatch_sig_95.inc"
}

#include "Dispatchers/process_157.inc"
#include "Dispatchers/read_160.inc"

#include "Dispatchers/dtor_Dispatcher_Cancel_Quick_Party.inc"
#include "Dispatchers/check_error_45.inc"
}

// ORIG 0x081de1f8（报告 Dispatcher_Cancel_Quick_Party/process.md）：
// check_error 同型；cancel_quick_party(party, true, user) != 1 → param+4 = 1。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081de1f8 orig/ours=50/49 ndiff-blocks=12
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    %eax,-0x14(%ebp)
#include "Dispatchers/process_158.inc"
}
#include "Dispatchers/read_161.inc"
#include "Dispatchers/send_32.inc"
}

#include "Dispatchers/dtor_Dispatcher_ChangeRandomOption.inc"
#include "Dispatchers/process_159.inc"
}
#include "Dispatchers/read_162.inc"
}

#include "Dispatchers/dtor_Dispatcher_ChangeTutorialFlag.inc"
#include "Dispatchers/dispatch_sig_96.inc"
}
#include "Dispatchers/process_160.inc"
#include "Dispatchers/read_163.inc"

#include "Dispatchers/dtor_Dispatcher_Change_Charac_Slot.inc"
#include "Dispatchers/dispatch_sig_97.inc"
}
#include "Dispatchers/process_161.inc"
#include "Dispatchers/read_164.inc"

#include "Dispatchers/dtor_Dispatcher_CharacterStatistic.inc"
#include "Dispatchers/dispatch_sig_98.inc"
}
#include "Dispatchers/process_162.inc"
#include "Dispatchers/read_165.inc"

#include "Dispatchers/dtor_Dispatcher_CloseDisjointStore.inc"
#include "Dispatchers/check_error_46.inc"
}

// ORIG 0x081d0366（报告 Dispatcher_CloseDisjointStore/process.md）：
// 通过后调当前专家职业 OnCloseMission(user, 0)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081d0366 orig/ours=53/54 ndiff-blocks=10
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0xc(%ebp) | mov    %eax,%edx | mov    -0xc(%ebp),%eax
#include "Dispatchers/process_163.inc"
}
#include "Dispatchers/read_166.inc"
#include "Dispatchers/send_33.inc"
}

#include "Dispatchers/dtor_Dispatcher_CompatibilityIndex.inc"
#include "Dispatchers/process_164.inc"
}
#include "Dispatchers/read_167.inc"
}


#include "Dispatchers/dtor_Dispatcher_CreateAccountCargo.inc"
#include "Dispatchers/dispatch_sig_99.inc"
}
#include "Dispatchers/process_165.inc"
#include "Dispatchers/read_168.inc"

#include "Dispatchers/dtor_Dispatcher_CreatePrivateStore.inc"
#include "Dispatchers/dispatch_sig_100.inc"
}
#include "Dispatchers/process_166.inc"
#include "Dispatchers/read_169.inc"

#include "Dispatchers/dtor_Dispatcher_DungeonNPCBuffInfo.inc"
#include "Dispatchers/dispatch_sig_101.inc"
#include "Dispatchers/process_167.inc"
#include "Dispatchers/read_170.inc"

#include "Dispatchers/dtor_Dispatcher_EnterDisjointStore.inc"
#include "Dispatchers/process_168.inc"
}
#include "Dispatchers/read_171.inc"
}
#include "Dispatchers/send_34.inc"
}

#include "Dispatchers/dtor_Dispatcher_FrameLagStatistics.inc"
#include "Dispatchers/process_169.inc"
#include "Dispatchers/read_172.inc"

#include "Dispatchers/dtor_Dispatcher_GuildCargoMoveItem.inc"
#include "Dispatchers/process_170.inc"
}

#include "Dispatchers/read_173.inc"
}
#include "Dispatchers/send_35.inc"
}

#include "Dispatchers/dtor_Dispatcher_GuildCargoPushItem.inc"
#include "Dispatchers/check_error_47.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d10d4 orig/ours=163/175 ndiff-blocks=25
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x2c(%ebp) | mov    %eax,%edx | mov    -0x2c(%ebp),%eax
#include "Dispatchers/process_171.inc"
}

#include "Dispatchers/read_174.inc"
}

#include "Dispatchers/send_36.inc"
}

#include "Dispatchers/dtor_Dispatcher_RemovePrivateStore.inc"
#include "Dispatchers/dispatch_sig_102.inc"
}
#include "Dispatchers/process_172.inc"
#include "Dispatchers/read_175.inc"

#include "Dispatchers/dtor_Dispatcher_RequestAddPvPBuddy.inc"
#include "Dispatchers/dispatch_sig_103.inc"
}

#include "Dispatchers/process_173.inc"
#include "Dispatchers/read_176.inc"

#include "Dispatchers/dtor_Dispatcher_RequestGuildSecede.inc"
#include "Dispatchers/dispatch_sig_104.inc"
}
#include "Dispatchers/process_174.inc"
#include "Dispatchers/read_177.inc"

#include "Dispatchers/dtor_Dispatcher_RequestMemberEnter.inc"
#include "Dispatchers/check_error_48.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cb874 orig/ours=77/77 ndiff-blocks=12
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
#include "Dispatchers/process_175.inc"
}
#include "Dispatchers/read_178.inc"
}
#include "Dispatchers/send_37.inc"
}

#include "Dispatchers/dtor_Dispatcher_Request_Seria_Buff.inc"
#include "Dispatchers/process_176.inc"
}
#include "Dispatchers/read_179.inc"
#include "Dispatchers/send_38.inc"
}

#include "Dispatchers/dtor_Dispatcher_Score_Scroll_State.inc"
#include "Dispatchers/dispatch_sig_105.inc"
}
#include "Dispatchers/process_177.inc"
#include "Dispatchers/read_180.inc"

#include "Dispatchers/dtor_Dispatcher_SecurityCardDisuse.inc"
#include "Dispatchers/dispatch_sig_106.inc"
}

#include "Dispatchers/process_178.inc"
#include "Dispatchers/read_181.inc"


#include "Dispatchers/dtor_Dispatcher_TitleBookOtherUser.inc"
#include "Dispatchers/process_179.inc"
}
#include "Dispatchers/read_182.inc"
}

#include "Dispatchers/dtor_Dispatcher_UnsealRandomOption.inc"
#include "Dispatchers/process_180.inc"
#include "Dispatchers/read_183.inc"

#include "Dispatchers/dtor_Dispatcher_ValidateScriptHash.inc"
#include "Dispatchers/get_hack_type.inc"
}

// ORIG 0x082642b8（报告 Dispatcher_ValidateScriptHash/process.md）：
// check_error 恒 0；get_hack_type(msg+0xd) → addServerHackCnt(type,1,0xe,0xf)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082642b8 orig/ours=51/54 ndiff-blocks=11
//   帧差 sub $30 vs $3c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0x14(%ebp) | cmpl   $0x0,-0x14(%ebp)
#include "Dispatchers/process_181.inc"
}
#include "Dispatchers/read_184.inc"
}

#include "Dispatchers/dtor_Dispatcher_AuctionRegistCancel.inc"
#include "Dispatchers/dispatch_sig_107.inc"
}
#include "Dispatchers/process_182.inc"
#include "Dispatchers/read_185.inc"

#include "Dispatchers/dtor_Dispatcher_BuyPrivateStoreItem.inc"
#include "Dispatchers/dispatch_sig_108.inc"
}
#include "Dispatchers/process_183.inc"
#include "Dispatchers/read_186.inc"

#include "Dispatchers/dtor_Dispatcher_CallGuildAllMembers.inc"
#include "Dispatchers/dispatch_sig_109.inc"
}
#include "Dispatchers/process_184.inc"
#include "Dispatchers/read_187.inc"

#include "Dispatchers/dtor_Dispatcher_CompleteLoadAssault.inc"
#include "Dispatchers/dispatch_sig_110.inc"
}
#include "Dispatchers/process_185.inc"
#include "Dispatchers/read_188.inc"

#include "Dispatchers/dtor_Dispatcher_CreateDisjointStore.inc"
#include "Dispatchers/process_186.inc"
}
#include "Dispatchers/read_189.inc"
}

#include "Dispatchers/send_39.inc"
}

#include "Dispatchers/dtor_Dispatcher_CreatureSendMessage.inc"
#include "Dispatchers/dispatch_sig_111.inc"
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
}
#include "Dispatchers/process_188.inc"
#include "Dispatchers/read_191.inc"

#include "Dispatchers/dtor_Dispatcher_GuildAttendanceInfo.inc"
#include "Dispatchers/process_189.inc"
}
#include "Dispatchers/read_192.inc"
}

#include "Dispatchers/dtor_Dispatcher_GuildMasterDelegate.inc"
#include "Dispatchers/dispatch_sig_113.inc"
}
#include "Dispatchers/process_190.inc"
#include "Dispatchers/read_193.inc"

#include "Dispatchers/dtor_Dispatcher_New_Gmdebug_Command.inc"
#include "Dispatchers/dispatch_sig_114.inc"
}
#include "Dispatchers/process_191.inc"
#include "Dispatchers/read_194.inc"

#include "Dispatchers/dtor_Dispatcher_PowerWarProcessInfo.inc"
#include "Dispatchers/dispatch_sig_115.inc"
}
#include "Dispatchers/process_192.inc"
#include "Dispatchers/read_195.inc"

#include "Dispatchers/dtor_Dispatcher_PvpMissionHPPercent.inc"
#include "Dispatchers/process_193.inc"
}
#include "Dispatchers/read_196.inc"
}

#include "Dispatchers/send_40.inc"
}


#include "Dispatchers/dtor_Dispatcher_RegisterToBlackList.inc"
#include "Dispatchers/check_error_49.inc"
}
// ORIG T 0x08229644：解封卷轴资格链（0xffff=金币通道直接通过）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08229644 orig/ours=81/89 ndiff-blocks=14
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movzwl -0x6c(%ebp),%ebx | lea    -0x5d(%ebp),%edx | mov    %ebx,0xc(%esp)
#include "Dispatchers/_check_unseal_scroll_item.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x08228e5e orig/ours=499/500 ndiff-blocks=47
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 6 个，4.4 libstdc++ 全内联不可复现
//   首差: sub    $0xcc,%esp | movw   $0x0,-0x36(%ebp) | lea    -0x36(%ebp),%eax
#include "Dispatchers/dispatch_sig_116.inc"
}
#include "Dispatchers/read_197.inc"
}

#include "Dispatchers/dtor_Dispatcher_RequestAssaultPrice.inc"

#include "Dispatchers/dtor_Dispatcher_RequestDisjointItem.inc"
#include "Dispatchers/process_194.inc"
}
#include "Dispatchers/read_198.inc"
}
#include "Dispatchers/send_41.inc"
}

#include "Dispatchers/dtor_Dispatcher_ResponseAddPvPBuddy.inc"
#include "Dispatchers/dispatch_sig_117.inc"
}

#include "Dispatchers/process_195.inc"
#include "Dispatchers/read_199.inc"

#include "Dispatchers/dtor_Dispatcher_SaveCharacterOption.inc"
#include "Dispatchers/check_error_50.inc"
}
// ORIG 0x081ce97c（报告 Dispatcher_SaveCharacterOption/process.md）：
// param+4=check_error；>0 → 0；<0 LineFunc(0x283d)；==0 setSaveCharacOption(true)
// + SetCurCharacOption(msg+0x11)。
#include "Dispatchers/process_196.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081ce8bc orig/ours=51/54 ndiff-blocks=6
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax | mov    -0xc(%ebp),%eax
#include "Dispatchers/read_200.inc"
}

#include "Dispatchers/dtor_Dispatcher_SecretShopOpenClose.inc"
#include "Dispatchers/process_197.inc"
}
#include "Dispatchers/read_201.inc"
}
#include "Dispatchers/send_42.inc"
}

#include "Dispatchers/dtor_Dispatcher_SecurityCardAuthReq.inc"
#include "Dispatchers/dispatch_sig_118.inc"
}
#include "Dispatchers/process_198.inc"
#include "Dispatchers/read_202.inc"


#include "Dispatchers/dtor_Dispatcher_SecurityCardCertKey.inc"
#include "Dispatchers/dispatch_sig_119.inc"
}
#include "Dispatchers/process_199.inc"
#include "Dispatchers/read_203.inc"

#include "Dispatchers/dtor_Dispatcher_UpdateServerMessage.inc"
#include "Dispatchers/dispatch_sig_120.inc"
}
#include "Dispatchers/process_200.inc"
#include "Dispatchers/read_204.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeAccountCargo.inc"
#include "Dispatchers/dispatch_sig_121.inc"
}
#include "Dispatchers/process_201.inc"
#include "Dispatchers/read_205.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeItemSeparate.inc"
#include "Dispatchers/check_error_51.inc"
}

// ORIG 0x081cc106（报告 Dispatcher_UpgradeItemSeparate/process.md）：
// err>0 → SendCmdErrorPacket((ENUM_CMDPACKET)0x1b7, err&0xff) 后 return -1；
// 否则 m_itemUpgradeSeparate(+0x24)->item_upgrade_system(user, msg, 0x1b7)。
// [b28c-33 判词 N] ORIG 081cc152 `mov 0x14(%ebp),%eax; mov %eax,-0x10(%ebp)` 读第 4
// 形参槽（3 参函数越界读调用者栈）并落死局部 -0x10，合法 C++ 不可复现；残 2 条即此。
// 我方 36/ORIG 38，其余调用序列与槽全等（p167 CLEAR）。
#include "Dispatchers/process_202.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081cbf24 orig/ours=124/86 ndiff-blocks=13
//   帧差 sub $a0 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %ebx | sub    $0xa0,%esp
// [SweepDispatchers 2026-09-09] 真缺口修复（86→124 = ORIG 124，p167 10/10 CLEAR）：
//   ① ORIG 0x081cbf84 调 get_int(unsigned long&)（Rm）——旧码传 unsigned int 字段选 Rj
//      致 MISSING 1；调用点 reinterpret_cast<unsigned long&> 取回同址传参；
//   ② ORIG 0x081cbffe/0x081cc00c：-0xc=0x80（dead）、-0x91..-0x11 的 0x81 字节 dead 缓冲
//      （对齐剥离 rep stos 慢路）缺失 ⇒ 补 dummyLen/deadBuf；
//   ③ 删 msg 引用别名（P0）：ORIG 每次从 0x10(%ebp) 重读。
#include "Dispatchers/read_206.inc"
}


#include "Dispatchers/dtor_Dispatcher_UseSharedEffectItem.inc"
#include "Dispatchers/dispatch_sig_122.inc"
}
#include "Dispatchers/process_203.inc"
#include "Dispatchers/read_207.inc"

#include "Dispatchers/dtor_Dispatcher_VerifyCreatureQuest.inc"
#include "Dispatchers/process_204.inc"
}
#include "Dispatchers/read_208.inc"

#include "Dispatchers/dtor_Dispatcher_WarRoomWpPerMonster.inc"
#include "Dispatchers/check_error_52.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d5ca8 orig/ours=89/88 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | cmpl   $0x0,-0x1c(%ebp) | mov    -0x1c(%ebp),%eax
#include "Dispatchers/process_205.inc"
}
#include "Dispatchers/read_209.inc"
}
#include "Dispatchers/send_43.inc"
}

#include "Dispatchers/dtor_Dispatcher_AbnormalUseStackable.inc"
#include "Dispatchers/check_error_53.inc"
#include "Dispatchers/process_206.inc"
}
#include "Dispatchers/read_210.inc"
}

#include "Dispatchers/dtor_Dispatcher_AuctionBuyItemApiece.inc"
#include "Dispatchers/dispatch_sig_123.inc"
}
#include "Dispatchers/process_207.inc"
#include "Dispatchers/read_211.inc"

#include "Dispatchers/dtor_Dispatcher_AuctionMyBiddingInfo.inc"
#include "Dispatchers/dispatch_sig_124.inc"
}
#include "Dispatchers/process_208.inc"
#include "Dispatchers/read_212.inc"

#include "Dispatchers/dtor_Dispatcher_BuyCeraShopLimitItem.inc"


#include "Dispatchers/dtor_Dispatcher_CallGuildCreateRight.inc"
#include "Dispatchers/dispatch_sig_125.inc"
}
#include "Dispatchers/process_209.inc"
#include "Dispatchers/read_213.inc"

#include "Dispatchers/dtor_Dispatcher_Check3rdPartyConcent.inc"
#include "Dispatchers/process_210.inc"
}
#include "Dispatchers/read_214.inc"

#include "Dispatchers/dtor_Dispatcher_CheckGuildNameDouble.inc"
#include "Dispatchers/dispatch_sig_126.inc"
}
#include "Dispatchers/process_211.inc"
#include "Dispatchers/read_215.inc"

#include "Dispatchers/dtor_Dispatcher_Compound_ExtreamItem.inc"
#include "Dispatchers/_push_newitem.inc"
}
#include "Dispatchers/_remove_victim.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0822577e orig/ours=37/47 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0xc(%ebp),%edx | mov    0x8(%ebp),%eax | movw   $0x0,0x8(%eax,%edx,2)
#include "Dispatchers/_reset.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08225c54 orig/ours=166/169 ndiff-blocks=27
//   帧差 sub $20 vs $30 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x20,%esp | lea    -0x18(%ebp),%eax | lea    -0x18(%ebp),%eax
#include "Dispatchers/_send_result.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08225ec4 orig/ours=217/222 ndiff-blocks=30
//   帧差 sub $40 vs $50 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x40,%esp | movb   $0x0,-0xd(%ebp) | xor    $0x1,%eax
#include "Dispatchers/dispatch_sig_127.inc"
}
#include "Dispatchers/process_212.inc"
#include "Dispatchers/read_216.inc"

#include "Dispatchers/dtor_Dispatcher_GatheringPartyStatus.inc"
#include "Dispatchers/dispatch_sig_128.inc"
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
}

// ORIG T 0x081d98c4：超链接聊天消息读取（MSG_ITEM_HYPER_LINK_MESSAGE）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d98c4 orig/ours=203/206 ndiff-blocks=18
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN31Dispatcher_ItemHyperLinkMessage4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN31Dispatcher_ItemHyperLinkMes
#include "Dispatchers/read_218.inc"
}

// ORIG T 0x081da584：应答分发（param 无错误码 → OK 包，否则错误包）。
#include "Dispatchers/send_44.inc"
}

// ORIG T 0x081d9c4a：超链接聊天处理（PacketDispatcher_Impl_1.cpp 0x47aa 区）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d9c4a orig/ours=583/583 ndiff-blocks=105
//   帧差 sub $2e0 vs $2f0 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x2e0,%esp | mov    -0x54(%ebp),%edx | mov    %eax,0x4(%edx)
#include "Dispatchers/process_214.inc"
}

// ==== DB_InsertPoliceSaveChatting::makeRequest (ORIG T 0x0843c6aa) ====
#include "Dispatchers/makeRequest_3.inc"
}

#include "Dispatchers/dtor_Dispatcher_NotifyMessageToGuild.inc"
#include "Dispatchers/dispatch_sig_129.inc"
}
#include "Dispatchers/process_215.inc"
#include "Dispatchers/read_219.inc"

#include "Dispatchers/dtor_Dispatcher_OperateRidableObject.inc"
#include "Dispatchers/dispatch_sig_130.inc"
}
#include "Dispatchers/process_216.inc"
#include "Dispatchers/read_220.inc"

#include "Dispatchers/dtor_Dispatcher_PartyTeleportConfirm.inc"
#include "Dispatchers/check_error_55.inc"
}
#include "Dispatchers/process_217.inc"
}
#include "Dispatchers/read_221.inc"
}
#include "Dispatchers/send_45.inc"
}

#include "Dispatchers/dtor_Dispatcher_PcRoomPlayTimeReward.inc"
#include "Dispatchers/process_218.inc"
}
#include "Dispatchers/read_222.inc"
#include "Dispatchers/send_46.inc"
}


#include "Dispatchers/dtor_Dispatcher_PrecheckSoloTeleport.inc"
#include "Dispatchers/process_219.inc"
}
#include "Dispatchers/read_223.inc"
}
#include "Dispatchers/send_47.inc"
}

#include "Dispatchers/dtor_Dispatcher_PvpMissionComboClear.inc"
#include "Dispatchers/process_220.inc"
}
#include "Dispatchers/read_224.inc"
}

#include "Dispatchers/dtor_Dispatcher_Register_Quick_Party.inc"
#include "Dispatchers/check_error_56.inc"
}

// ORIG 0x081de012（报告 Dispatcher_Register_Quick_Party/process.md）：
// 以 MSG+0xe(short)/+0x10(byte) 构造 STQuickPartyPoolMap_Key（默认 0/0xff 后覆盖，
// 与 ORIG 0x082344ca 一致），match_proper_party(key, party, user) 写 param+0x4。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081de012 orig/ours=57/58 ndiff-blocks=14
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    %eax,-0x14(%ebp)
#include "Dispatchers/process_221.inc"
}
#include "Dispatchers/read_225.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P63] ORIG 0x081de0c6 orig/ours=67/65 ndiff-blocks=1
//   ORIG 正常路径尾 jmp+nop 中继岛（4.1 EH 尾块布局），4.4 fall-through/jump-thread 恒消除（P223 尾空标签试形实测无效）
//   首差: jmp    081de1a3 <+0xdd> | nop
#include "Dispatchers/send_48.inc"
}

#include "Dispatchers/dtor_Dispatcher_ReportMannerlessUser.inc"
#include "Dispatchers/dispatch_sig_131.inc"
}
#include "Dispatchers/process_222.inc"
#include "Dispatchers/read_226.inc"

#include "Dispatchers/dtor_Dispatcher_Request_PvpExpOfWeek.inc"
#include "Dispatchers/dispatch_sig_132.inc"
}
#include "Dispatchers/process_223.inc"
#include "Dispatchers/read_227.inc"

#include "Dispatchers/dtor_Dispatcher_Security_ProcessScan.inc"
#include "Dispatchers/dispatch_sig_133.inc"
}
#include "Dispatchers/process_224.inc"
#include "Dispatchers/read_228.inc"

#include "Dispatchers/dtor_Dispatcher_SeriaRoom_Deco_Event.inc"
#include "Dispatchers/check_error_57.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e5cde orig/ours=98/100 ndiff-blocks=16
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x18(%ebp) | mov    %eax,-0x14(%ebp) | mov    -0x14(%ebp),%edx
#include "Dispatchers/process_225.inc"
}
#include "Dispatchers/read_229.inc"
}
#include "Dispatchers/send_49.inc"
}


#include "Dispatchers/dtor_Dispatcher_WriteGuildMemberMemo.inc"
#include "Dispatchers/dispatch_sig_134.inc"
}
#include "Dispatchers/process_226.inc"
#include "Dispatchers/read_230.inc"


#include "Dispatchers/dtor_Dispatcher_Client_Spec_Statistic.inc"
#include "Dispatchers/send_50.inc"
}

// ORIG 0x081ce6d4（报告 Dispatcher_Client_Spec_Statistic/process.md）：
// check_error（仅判 user 空）；param+4=msg+0xd byte；memcpy(getClientHWSpec(),msg+0xe,0xbf8)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081ce6d4 orig/ours=42/42 ndiff-blocks=10
//   帧差 sub $28 vs $24 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x28,%esp | mov    %eax,-0x18(%ebp) | cmpl   $0x0,-0x18(%ebp)
#include "Dispatchers/process_227.inc"
}

#include "Dispatchers/dtor_Dispatcher_CreatureScriptMessage.inc"
#include "Dispatchers/dispatch_sig_135.inc"
}
#include "Dispatchers/process_228.inc"
#include "Dispatchers/read_231.inc"

#include "Dispatchers/dtor_Dispatcher_Mercenary_Competition.inc"
#include "Dispatchers/process_229.inc"
}

// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x081ddc24 orig/ours=53/53
#include "Dispatchers/read_232.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081dde20 orig/ours=83/87 ndiff-blocks=21
//   帧差 sub $20 vs $30 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x20,%esp | lea    -0x18(%ebp),%eax | lea    -0x18(%ebp),%eax
#include "Dispatchers/send_51.inc"
}

#include "Dispatchers/dtor_Dispatcher_OpenGuildCreateWindow.inc"
#include "Dispatchers/dispatch_sig_136.inc"
}
#include "Dispatchers/process_230.inc"
#include "Dispatchers/read_233.inc"

#include "Dispatchers/dtor_Dispatcher_RepairDisjointMachine.inc"
#include "Dispatchers/check_error_58.inc"
}

// ORIG 0x081cfe3c（报告 Dispatcher_RepairDisjointMachine/process.md）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cfe3c orig/ours=56/59 ndiff-blocks=10
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: sub    $0x28,%esp | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
#include "Dispatchers/process_231.inc"
}
#include "Dispatchers/read_234.inc"
#include "Dispatchers/send_52.inc"
}

#include "Dispatchers/dtor_Dispatcher_AuctionAskAveragePrice.inc"
#include "Dispatchers/dispatch_sig_137.inc"
}
#include "Dispatchers/process_232.inc"
#include "Dispatchers/read_235.inc"

#include "Dispatchers/dtor_Dispatcher_AuctionSearchByItemKey.inc"
#include "Dispatchers/dispatch_sig_138.inc"
}
#include "Dispatchers/process_233.inc"
#include "Dispatchers/read_236.inc"

#include "Dispatchers/dtor_Dispatcher_Break_Away_Quest_Check.inc"
#include "Dispatchers/process_234.inc"
}
// ORIG 0x081d38ba：get_byte 读入 msg.m_checkFlag（+0xd）；失败走
// LineFunc(0x3197) 返回其值。
#include "Dispatchers/read_237.inc"
}
// ORIG 0x081d3a72：m_resultCode==0 → put_header(1,0x16d)+put_byte(1)+
// finalize(true)；否则 put_header(1,0x16d)+put_byte(0)+
// put_short(m_resultCode)+finalize(true)。两分支均 CUser::Send。
#include "Dispatchers/send_53.inc"
}

#include "Dispatchers/dtor_Dispatcher_ChangeAnotherSkillTree.inc"
#include "Dispatchers/check_error_59.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d1fca orig/ours=58/58 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%edx | mov    -0x10(%ebp),%eax
#include "Dispatchers/process_235.inc"
}
#include "Dispatchers/read_238.inc"
}
#include "Dispatchers/send_54.inc"
}

#include "Dispatchers/dtor_Dispatcher_CharacSlotExtendEffect.inc"
#include "Dispatchers/process_236.inc"
}
#include "Dispatchers/read_239.inc"
#include "Dispatchers/send_55.inc"
}


#include "Dispatchers/dtor_Dispatcher_CheckAssaultMotionHack.inc"
#include "Dispatchers/dispatch_sig_139.inc"
}
#include "Dispatchers/process_237.inc"
#include "Dispatchers/read_240.inc"

#include "Dispatchers/dtor_Dispatcher_ConnectP2PAfterAssault.inc"
#include "Dispatchers/dispatch_sig_140.inc"
}

#include "Dispatchers/process_238.inc"
#include "Dispatchers/read_241.inc"

#include "Dispatchers/dtor_Dispatcher_DeathTowerStageCommand.inc"
namespace {
struct DTChkView {
    char m_head[0xb58];
    WongWork::CDeathTower::stMapMonsterKillChecker_t m_killChecker;   // +0xb58
};
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08208a9e orig/ours=173/186 ndiff-blocks=35
//   帧差 sub $50 vs $88 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x50,%esp
#include "Dispatchers/dispatch_sig_141.inc"
}
#include "Dispatchers/process_239.inc"
#include "Dispatchers/read_242.inc"

#include "Dispatchers/dtor_Dispatcher_Delete_GuildBoard_Text.inc"
#include "Dispatchers/process_240.inc"
}
#include "Dispatchers/read_243.inc"
}

#include "Dispatchers/dtor_Dispatcher_DungeonEventStoryPause.inc"
#include "Dispatchers/dispatch_sig_142.inc"
}
#include "Dispatchers/process_241.inc"
#include "Dispatchers/read_244.inc"

#include "Dispatchers/dtor_Dispatcher_Float_RDATA_Modulation.inc"
#include "Dispatchers/check_error_60.inc"
}

// ORIG 0x08264acc（报告 Dispatcher_Float_RDATA_Modulation/process.md）：
// 解码两 dword 异或常量后记 hack 计数（0x349）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08264acc orig/ours=41/44 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x18(%ebp) | cmpl   $0x0,-0x18(%ebp) | mov    %eax,-0x14(%ebp)
#include "Dispatchers/process_242.inc"
}
#include "Dispatchers/read_245.inc"
}

#include "Dispatchers/dtor_Dispatcher_LogIngameAdvertisement.inc"
#include "Dispatchers/check_error_61.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e4df2 orig/ours=67/66 ndiff-blocks=16
//   帧差 sub $4c vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | sub    $0x4c,%esp
#include "Dispatchers/process_243.inc"
}
#include "Dispatchers/read_246.inc"
}
#include "Dispatchers/send_56.inc"
}

#include "Dispatchers/dtor_Dispatcher_QueryCharacInfoMailbox.inc"
#include "Dispatchers/dispatch_sig_143.inc"
}

#include "Dispatchers/process_244.inc"
#include "Dispatchers/read_247.inc"

#include "Dispatchers/dtor_Dispatcher_RequestCharacSkillInfo.inc"
#include "Dispatchers/check_error_62.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e39e0 orig/ours=150/119 ndiff-blocks=26
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0x18(%ebp)
// [SweepDispatchers 2026-09-09] 真缺口修复（119→142，ORIG 149；p167 仅剩 C1/C2 同址别名）：
//   ① ORIG 4 次独立 vector<_Charac_info>::operator[]（0x081e3a78/aa8/ade/ba9）——旧码用
//      info/skillSlot 引用缓存只调 1 次 ⇒ 删缓存改表达式直读（P183）；
//   ② ORIG 先调 CStreamGuard::operator*()（0x081e3b3a/0x081e3b62）再 operator<<、operator->()
//      （0x081e3b79）再 GetInBuffer（P132/P103）；旧码直调 guard<< 缺 de/pt 共 3 个 call；
//   ③ 保留 ORIG -0xc 的 kind 具名槽（P0）；正极性 if/else（P59/P214①）。
#include "Dispatchers/process_245.inc"
}
#include "Dispatchers/read_248.inc"
}
#include "Dispatchers/send_57.inc"
}

#include "Dispatchers/dtor_Dispatcher_SecurityCardAuthCancel.inc"
#include "Dispatchers/dispatch_sig_144.inc"
}
#include "Dispatchers/process_246.inc"
}
#include "Dispatchers/read_249.inc"

#include "Dispatchers/dtor_Dispatcher_SecurityCardRetransfer.inc"
#include "Dispatchers/dispatch_sig_145.inc"
}
#include "Dispatchers/process_247.inc"
#include "Dispatchers/read_250.inc"

#include "Dispatchers/dtor_Dispatcher_TournamentRewardSelect.inc"

#include "Dispatchers/dtor_Dispatcher_UpgradeDisjointMachine.inc"
#include "Dispatchers/check_error_63.inc"
}

// ORIG 0x081cff94（报告 Dispatcher_UpgradeDisjointMachine/process.md）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cff94 orig/ours=56/63 ndiff-blocks=11
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: sub    $0x28,%esp | mov    %eax,-0x10(%ebp) | mov    %eax,%edx
#include "Dispatchers/process_248.inc"
}
#include "Dispatchers/read_251.inc"
#include "Dispatchers/send_58.inc"
}

#include "Dispatchers/dtor_Dispatcher_VerifyPrivateStoreItem.inc"
#include "Dispatchers/dispatch_sig_146.inc"
}
#include "Dispatchers/process_249.inc"
#include "Dispatchers/read_252.inc"

#include "Dispatchers/dtor_Dispatcher_AuctionMyAuctionHistory.inc"
#include "Dispatchers/dispatch_sig_147.inc"
}
#include "Dispatchers/process_250.inc"
#include "Dispatchers/read_253.inc"

#include "Dispatchers/dtor_Dispatcher_Card_Select_Right_State.inc"
#include "Dispatchers/dispatch_sig_148.inc"
}
#include "Dispatchers/process_251.inc"
#include "Dispatchers/read_254.inc"

#include "Dispatchers/dtor_Dispatcher_CheckGuildAddressDouble.inc"
#include "Dispatchers/dispatch_sig_149.inc"
}

#include "Dispatchers/process_252.inc"
#include "Dispatchers/read_255.inc"

#include "Dispatchers/dtor_Dispatcher_CompoundItemByExpertJob.inc"
#include "Dispatchers/dispatch_sig_150.inc"
}
#include "Dispatchers/process_253.inc"
#include "Dispatchers/read_256.inc"

#include "Dispatchers/dtor_Dispatcher_InvestItemAmplifyOption.inc"


#include "Dispatchers/dtor_Dispatcher_RequestItemUnlockCancel.inc"
#include "Dispatchers/dispatch_sig_151.inc"
}

#include "Dispatchers/process_254.inc"
#include "Dispatchers/read_257.inc"

#include "Dispatchers/dtor_Dispatcher_RequestPCRoomPlayerList.inc"
#include "Dispatchers/dispatch_sig_152.inc"
}
#include "Dispatchers/process_255.inc"
#include "Dispatchers/read_258.inc"

#include "Dispatchers/dtor_Dispatcher_Write_On_The_GuildBoard.inc"
#include "Dispatchers/process_256.inc"
}
#include "Dispatchers/read_259.inc"
}
#include "Dispatchers/send_59.inc"
}

#include "Dispatchers/dtor_Dispatcher_AuctionSearchByNoItmeKey.inc"
#include "Dispatchers/dispatch_sig_153.inc"
}
#include "Dispatchers/process_257.inc"
#include "Dispatchers/read_260.inc"

#include "Dispatchers/dtor_Dispatcher_CheckEnableServerMessage.inc"
#include "Dispatchers/dispatch_sig_154.inc"
}
#include "Dispatchers/process_258.inc"
#include "Dispatchers/read_261.inc"

#include "Dispatchers/dtor_Dispatcher_CompleteLoadAfterAssault.inc"
#include "Dispatchers/dispatch_sig_155.inc"
}
#include "Dispatchers/process_259.inc"
#include "Dispatchers/read_262.inc"

#include "Dispatchers/dtor_Dispatcher_Enchant_3rdChronicleItem.inc"
#include "Dispatchers/check_error_64.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d375e orig/ours=85/81 ndiff-blocks=15
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: mov    %eax,-0x18(%ebp) | cmpl   $0x0,-0x18(%ebp) | mov    -0x18(%ebp),%eax
#include "Dispatchers/process_260.inc"
}
#include "Dispatchers/read_263.inc"
}
#include "Dispatchers/send_60.inc"
}

#include "Dispatchers/dtor_Dispatcher_GoblinPadRequestCryptKey.inc"
#include "Dispatchers/dispatch_sig_156.inc"
}
#include "Dispatchers/process_261.inc"
#include "Dispatchers/read_264.inc"

#include "Dispatchers/dtor_Dispatcher_GoldTakeIncreasingAmount.inc"
#include "Dispatchers/check_error_65.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d55a4 orig/ours=58/59 ndiff-blocks=12
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    %eax,-0x18(%ebp)
#include "Dispatchers/process_262.inc"
}
#include "Dispatchers/read_265.inc"
}
#include "Dispatchers/send_61.inc"
}

#include "Dispatchers/dtor_Dispatcher_LevelupSupportReqestItem.inc"
#include "Dispatchers/dispatch_sig_157.inc"
}
#include "Dispatchers/process_263.inc"
#include "Dispatchers/read_266.inc"
#include "Dispatchers/send_62.inc"

#include "Dispatchers/dtor_Dispatcher_RequestOnTimeEventReward.inc"
#include "Dispatchers/dispatch_sig_158.inc"
}
#include "Dispatchers/process_264.inc"
#include "Dispatchers/read_267.inc"

#include "Dispatchers/dtor_Dispatcher_RequestPCRoomPlayerCount.inc"
#include "Dispatchers/dispatch_sig_159.inc"
}
#include "Dispatchers/process_265.inc"
#include "Dispatchers/read_268.inc"

#include "Dispatchers/dtor_Dispatcher_Security_MemoryIntegrity.inc"
#include "Dispatchers/dispatch_sig_160.inc"
}
#include "Dispatchers/process_266.inc"
#include "Dispatchers/read_269.inc"

#include "Dispatchers/dtor_Dispatcher_Security_PacketIntegrity.inc"
#include "Dispatchers/dispatch_sig_161.inc"
}
#include "Dispatchers/process_267.inc"
#include "Dispatchers/read_270.inc"

#include "Dispatchers/dtor_Dispatcher_SelectUltimateDifficulty.inc"
#include "Dispatchers/dispatch_sig_162.inc"
}
#include "Dispatchers/read_271.inc"

#include "Dispatchers/dtor_Dispatcher_Select_Item_Grwoth_Power.inc"
#include "Dispatchers/check_error_66.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081db60c orig/ours=410/389 ndiff-blocks=55
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x30(%ebp) | cmpl   $0x0,-0x30(%ebp) | mov    -0x30(%ebp),%ebx
#include "Dispatchers/process_268.inc"
}

#include "Dispatchers/read_272.inc"
}

#include "Dispatchers/send_63.inc"
}

#include "Dispatchers/dtor_Dispatcher_SeriaRoom_Ani_Deco_Event.inc"
#include "Dispatchers/check_error_67.inc"
}
#include "Dispatchers/read_273.inc"
}

#include "Dispatchers/dtor_Dispatcher_UpdateContractOfCubeInfo.inc"
#include "Dispatchers/check_error_68.inc"
}
// ORIG 0x0825f900（报告 Dispatcher_UpdateContractOfCubeInfo/process.md）：
// ParamBase 实际为 stUpdateContractOfCubeInfo（m_4/m_8/m_9）；check_error
//（ORIG 0x0825fa8e）已由既有实现提供。
#include "Dispatchers/process_269.inc"
}
#include "Dispatchers/read_274.inc"
}
#include "Dispatchers/send_64.inc"
}

#include "Dispatchers/dtor_Dispatcher_UseCreatureEvolutionItem.inc"
#include "Dispatchers/dispatch_sig_163.inc"
}
// ORIG 0x08228bcc（报告 UseCreatureEvolutionItem/process.md）：使用生物进化道具。
// 流程：物品空间→inven 槽位取物 → find_item → 非 stackable/非本生物(0x360) 报错；
// 通过则 CCreatureMgr::ItemEvolute。日志 cMyTrace(0xe621/0xe627/0xe62f)、
// cHistoryTrace("ItemEvolute ...") 与 ORIG rodata 逐字节一致。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08228bcc orig/ours=172/165 ndiff-blocks=23
//   帧差 sub $7c vs $ffffff80 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x7c,%esp | movswl -0x60(%ebp),%esi
#include "Dispatchers/process_270.inc"
}
#include "Dispatchers/process_271.inc"
#include "Dispatchers/read_275.inc"

#include "Dispatchers/dtor_Dispatcher_UseRightOfChangeGrowType.inc"
#include "Dispatchers/check_error_69.inc"
}
// ORIG 0x0825dc64 T _ZN35Dispatcher_UseRightOfChangeGrowType7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_UseRightOfChangeGrowType/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 stUseRightOfChangeGrowTypeParam，ORIG 裸访问 +0x04 错误码槽（m_4）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825dc64 orig/ours=75/77 ndiff-blocks=16
//   帧差 sub $38 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x38,%esp | mov    %eax,-0x14(%ebp) | mov    -0x14(%ebp),%edx
#include "Dispatchers/process_272.inc"
}
#include "Dispatchers/send_65.inc"
}

#include "Dispatchers/dtor_Dispatcher_AuctionMyRegistedItemInfo.inc"
#include "Dispatchers/dispatch_sig_164.inc"
}

#include "Dispatchers/process_273.inc"
#include "Dispatchers/read_276.inc"

#include "Dispatchers/dtor_Dispatcher_BloodRoundUiPrepareFinish.inc"
#include "Dispatchers/dispatch_sig_165.inc"
}
#include "Dispatchers/process_274.inc"
#include "Dispatchers/read_277.inc"

#include "Dispatchers/dtor_Dispatcher_Skill_Command_All_Default.inc"
#include "Dispatchers/process_275.inc"
}
#include "Dispatchers/read_278.inc"
#include "Dispatchers/send_66.inc"
}


#include "Dispatchers/dtor_Dispatcher_Skill_Command_Customizing.inc"

#include "Dispatchers/dtor_Dispatcher_Buy_Charac_Status_Using_QP.inc"
#include "Dispatchers/check_error_70.inc"
}
// ORIG 0x081d64b4 T _ZN37Dispatcher_Buy_Charac_Status_Using_QP7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_Buy_Charac_Status_Using_QP/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 STQuestShopBuyParam（+0x04 byte/+0x06 short/+0x08 int）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d64b4 orig/ours=73/74 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | cmpl   $0x0,-0x1c(%ebp) | cmpl   $0x0,-0x1c(%ebp)
#include "Dispatchers/process_276.inc"
}
#include "Dispatchers/read_279.inc"
}
#include "Dispatchers/send_67.inc"
}


#include "Dispatchers/dtor_Dispatcher_Change_PartyMemberPosition.inc"
#include "Dispatchers/dispatch_sig_166.inc"
}
#include "Dispatchers/process_277.inc"
#include "Dispatchers/read_280.inc"

#include "Dispatchers/dtor_Dispatcher_RequestIngameAdvertisement.inc"

#include "Dispatchers/dtor_Dispatcher_CallPartyMemberRealtimeInfo.inc"
#include "Dispatchers/check_error_71.inc"
}
// ORIG 0x081ce356（报告 Dispatcher_CallPartyMemberRealtimeInfo/process.md）：
// 不写 param+4；仅按 check_error 结果归一返回。
#include "Dispatchers/process_278.inc"
}
#include "Dispatchers/read_281.inc"
#include "Dispatchers/send_68.inc"
}


#include "Dispatchers/dtor_Dispatcher_Direct_Entrance_Quick_Party.inc"
#include "Dispatchers/check_error_72.inc"
}

// ORIG 0x081de334（报告 Dispatcher_Direct_Entrance_Quick_Party/process.md）：
// direct_entrance_quick_party(user) 返回值写 param+0x4；两侧分支均 return 0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081de334 orig/ours=43/39 ndiff-blocks=9
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | cmpl   $0x0,-0x10(%ebp) | mov    -0x10(%ebp),%eax
#include "Dispatchers/process_279.inc"
}
#include "Dispatchers/read_282.inc"
#include "Dispatchers/send_69.inc"
}

#include "Dispatchers/dtor_Dispatcher_Exchange_Random_Item_Reward.inc"
#include "Dispatchers/process_280.inc"
}
#include "Dispatchers/read_283.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081de72c orig/ours=197/173 ndiff-blocks=17
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax | mov    -0xc(%ebp),%eax
// [SweepDispatchers3 2026-09-09] 修真缺口：两循环改 ORIG 迭代器形（begin 一次 + 每轮
//   end() + it->first/second；m_items 体 4 组「*it 后 ++it」）+ size()!=0 前置物化
//   （setne/test/je）。gap 23→0（196=196），DIFF→AE（P175/P167 族，报告 send.md 已读）。
#include "Dispatchers/send_70.inc"
}


#include "Dispatchers/dtor_Dispatcher_RequestConditionEventReward.inc"
#include "Dispatchers/dispatch_sig_167.inc"
}
#include "Dispatchers/process_281.inc"
#include "Dispatchers/read_284.inc"

#include "Dispatchers/dtor_Dispatcher_TournamentRewardSelectState.inc"
#include "Dispatchers/check_error_73.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081dfca2 orig/ours=58/64 ndiff-blocks=8
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%edx | mov    %eax,0x4(%edx)
#include "Dispatchers/process_282.inc"
}
#include "Dispatchers/read_285.inc"
#include "Dispatchers/send_71.inc"
}


#include "Dispatchers/dtor_Dispatcher_Expand_Equipslot_Flag_Update.inc"
#include "Dispatchers/process_283.inc"
}
#include "Dispatchers/read_286.inc"
}
#include "Dispatchers/send_72.inc"
}

#include "Dispatchers/dtor_Dispatcher_FinishVillageMonsterFighting.inc"
#include "Dispatchers/dispatch_sig_168.inc"
}
#include "Dispatchers/process_284.inc"
#include "Dispatchers/read_287.inc"

#include "Dispatchers/dtor_Dispatcher_GrowthWeaponEventUseMaterial.inc"
#include "Dispatchers/check_error_74.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e81a4 orig/ours=98/99 ndiff-blocks=21
//   帧差 sub $30 vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x30,%esp | mov    %eax,-0x14(%ebp)
#include "Dispatchers/process_285.inc"
}
#include "Dispatchers/read_288.inc"
}
#include "Dispatchers/send_73.inc"
}


#include "Dispatchers/dtor_Dispatcher_HeroMissionEventState_Reward.inc"
#include "Dispatchers/dispatch_sig_169.inc"
}
#include "Dispatchers/process_286.inc"
#include "Dispatchers/read_289.inc"
#include "Dispatchers/send_74.inc"

#include "Dispatchers/dtor_Dispatcher_SaveGameOption_QuickChatting.inc"
#include "Dispatchers/process_287.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x081cf0a6 orig/ours=67/69 ndiff-blocks=9
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax | movl   $&_ZZN39Dispatcher_SaveGameOption_QuickChatting4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTIO
#include "Dispatchers/read_290.inc"
}

#include "Dispatchers/dtor_Dispatcher_Avatar_Disjoint_Random_Reward.inc"
#include "Dispatchers/process_288.inc"
}
#include "Dispatchers/dtor_Dispatcher_SDC_ActiveStatus_Damage_Check.inc"
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
}
#include "Dispatchers/process_289.inc"
#include "Dispatchers/read_291.inc"

#include "Dispatchers/dtor_Dispatcher_police_queryCharacName_useMid.inc"
#include "Dispatchers/dispatch_sig_171.inc"
}
#include "Dispatchers/process_290.inc"
#include "Dispatchers/read_292.inc"

#include "Dispatchers/dtor_Dispatcher_ImageCommunicationEquipmentUse.inc"
#include "Dispatchers/send_75.inc"
}

// ORIG 0x081e2280（报告 Dispatcher_ImageCommunicationEquipmentUse/check_error.md）：
// !user → -1；state<=2 → -2；getCurCharacMoney < GetImageCommnicationData[0] → 10。
#include "Dispatchers/check_error_75.inc"
}

// ORIG 0x081e21d6（报告 Dispatcher_ImageCommunicationEquipmentUse/process.md）：
// param+4=check_error；==0 时 use_money(price,0x30,true)、param+8=price、
// param+0xc=getCurCharacMoney。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e21d6 orig/ours=51/46 ndiff-blocks=8
//   帧差 sub $24 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    0x10(%ebp),%eax
#include "Dispatchers/process_291.inc"
}

#include "Dispatchers/dtor_Dispatcher_Security_NonClient_GetResponse.inc"
#include "Dispatchers/dispatch_sig_172.inc"
}
#include "Dispatchers/process_292.inc"
#include "Dispatchers/read_293.inc"

#include "Dispatchers/dtor_Dispatcher_Security_PI_HashModulation_C2C.inc"
#include "Dispatchers/dispatch_sig_173.inc"
}
#include "Dispatchers/process_293.inc"
#include "Dispatchers/read_294.inc"

#include "Dispatchers/dtor_Dispatcher_Security_PI_HashModulation_S2C.inc"
#include "Dispatchers/dispatch_sig_174.inc"
}
#include "Dispatchers/process_294.inc"
#include "Dispatchers/read_295.inc"

#include "Dispatchers/dtor_Dispatcher_Compound_Equipment_Upgrade_Card.inc"


#include "Dispatchers/dtor_Dispatcher_GrowthWeaponEventChangeInfinity.inc"
#include "Dispatchers/check_error_76.inc"
}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e7db4 orig/ours=108/112 ndiff-blocks=23
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x30,%esp | mov    -0x18(%ebp),%edx | mov    -0x18(%ebp),%eax
#include "Dispatchers/process_295.inc"
}
#include "Dispatchers/read_296.inc"
}
#include "Dispatchers/send_76.inc"
}

#include "Dispatchers/dtor_Dispatcher_SaveGameOption_ChattingEmoticon.inc"
#include "Dispatchers/process_296.inc"
}
#include "Dispatchers/read_297.inc"
}

#include "Dispatchers/send_77.inc"
}

#include "Dispatchers/dtor_Dispatcher_ComboSkillExtensionQuickSlotReset.inc"
#include "Dispatchers/check_error_77.inc"
}
// ORIG 0x0825f4e8（报告 Dispatcher_ComboSkillExtensionQuickSlotReset/process.md）：
// err=check_error → param.m_4；>0 返 0；<0 LineFunc(0x384)；
// ==0 pullComboSkillExtensionQuickSlot(msg+0xd) → send_skill_info。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825f4e8 orig/ours=52/51 ndiff-blocks=10
//   帧差 sub $24 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    -0x10(%ebp),%edx
#include "Dispatchers/process_297.inc"
}
#include "Dispatchers/read_298.inc"
}
#include "Dispatchers/send_78.inc"
}

#include "Dispatchers/dtor_Dispatcher_Select_3rdChronicleItem_ForEnchant.inc"
#include "Dispatchers/process_298.inc"
}
#include "Dispatchers/read_299.inc"

#include "Dispatchers/dtor_Dispatcher_GrowthCreatureChangeInfinityCreature.inc"
#include "Dispatchers/check_error_event.inc"
}

// ORIG 0x081e863a（报告 process_event.md）[随行依赖]，结构同上，失败返回 0。
// b28c-L20: 同 check_error_event 形，失败返回 0。
#include "Dispatchers/process_event.inc"
}

// ORIG 0x081e86a6（报告 Dispatcher_GrowthCreatureChangeInfinityCreature/check_error.md）
#include "Dispatchers/check_error_78.inc"
}

// ORIG 0x081e85ac（报告 Dispatcher_GrowthCreatureChangeInfinityCreature/process.md）：
// ParamBase 实际为 stGrowthCreature（m_4）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e85ac orig/ours=43/40 ndiff-blocks=10
//   帧差 sub $28 vs $24 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x28,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0xc(%ebp)
#include "Dispatchers/process_299.inc"
}
#include "Dispatchers/read_300.inc"
#include "Dispatchers/send_79.inc"
}

#include "Dispatchers/dtor_Dispatcher_P2PHolePunchingSuccessRateStatistics.inc"
#include "Dispatchers/dispatch_sig_175.inc"
#include "Dispatchers/process_300.inc"
}
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x0825feda orig/ours=53/53
#include "Dispatchers/read_301.inc"
}
#include "Dispatchers/send_80.inc"

namespace ARAD {
namespace DISPATCHER {

// 声明于 Arad_InternalDispatcher.h（ORIG 0x081977b6；实现在 Arad_InternalDispatcher.cpp）

// 线格式 Sig 结构（ORIG arad 命名空间；本 TU 按 put_binary 字节数定义）。
// [推断] 字段级语义由各 checkWorkState/dispatch_sig 使用方定义；此处仅保证
// 线格式长度（dispatch_sig 读包与 put_binary 一致）。

// ==================== Dispatcher_AvartarRoulette（ORIG 0x819ef54）====================
// dispatch_sig：user==0→0x312；getCurCharacR==0→0；state<3→SendCmdErrorPacket
//   (0x24a,7) 返回 0；CheckInTrade→同上；PVP/FreePvP→同上；读 2 字节
//   （slotType<0xb、count<10）；通过后发内部流 0x2c（3 字节）。
//   checkWorkState 安全检查依赖未建模服务（ServiceRestrictManager 等），推断为通过。
#include "Dispatchers/Dispatcher_AvartarRoulette.inc"
#include "Dispatchers/dtor_Dispatcher_AvartarRoulette.inc"
#include "Dispatchers/dispatch_sig_176.inc"
}
#include "Dispatchers/process_301.inc"
}
#include "Dispatchers/read_302.inc"
}

// ==================== Dispatcher_AvartarCoinCount（ORIG 0x819e660 区）====================
// 当前仅 checkWorkState/dispatch_sig 桩（工作列表不含）；保留签名，补最小骨架。
#include "Dispatchers/dtor_Dispatcher_AvartarCoinCount.inc"
#include "Dispatchers/checkWorkState.inc"
}
#include "Dispatchers/dispatch_sig_177.inc"
}
#include "Dispatchers/process_302.inc"
}
#include "Dispatchers/read_303.inc"
}

// ==================== Dispatcher_AradAvatarConvert（ORIG 0x81a00be）====================
// dispatch_sig：user==0→0x74a；getCurCharacR==0→0；state∉[3,4]→0；PVP→
//   SendCmdErrorPacket((ENUM_CMDPACKET)0x24f,7)；CheckInTrade→同上；isEnableAvatarSocketAction
//   （未建模，推断 0）；读 short+short+int；发内部流 0x33（0xe 字节）。
#include "Dispatchers/Dispatcher_AradAvatarConvert.inc"
#include "Dispatchers/dtor_Dispatcher_AradAvatarConvert.inc"
#include "Dispatchers/dispatch_sig_178.inc"
}
#include "Dispatchers/process_303.inc"
}
#include "Dispatchers/read_304.inc"
}

// ==================== Dispatcher_AradEmblemCompound（ORIG 0x819fd7e）====================
// dispatch_sig：user==0→0x6f1；getCurCharacR==0→0；state∉[3,4]→
//   SendCmdErrorPacket((ENUM_CMDPACKET)0x24e,7)；PVP/FreePvP→同上；CheckInTrade→同上；
//   isEnableAvatarSocketAction（推断 0）；读 3 个 short；发内部流 0x32（0x16 字节）。
#include "Dispatchers/Dispatcher_AradEmblemCompound.inc"
#include "Dispatchers/dtor_Dispatcher_AradEmblemCompound.inc"
#include "Dispatchers/dispatch_sig_179.inc"
}
#include "Dispatchers/process_304.inc"
}
#include "Dispatchers/read_305.inc"
}

// ==================== Dispatcher_AradJumpingCharacter（ORIG 0x819f1ec）====================
// dispatch_sig：user==0→0x59b；getCurCharacR==0→0；读 byte+byte+short+int
//   （错误码 0x5a9/0x5ac/0x5af/0x5b1/0x5b4/0x5b6）；state∉[3,4]→0；
//   通过后发内部流 0x2d（0x11 字节，含 acc_id/charac_no）。
#include "Dispatchers/Dispatcher_AradJumpingCharacter.inc"
#include "Dispatchers/dtor_Dispatcher_AradJumpingCharacter.inc"
#include "Dispatchers/dispatch_sig_180.inc"
}
#include "Dispatchers/process_305.inc"
}
#include "Dispatchers/read_306.inc"
}

// ==================== Dispatcher_AradUseAvatarRecharge（ORIG 0x819fa56）====================
// dispatch_sig：user==0→0x69c；getCurCharacR==0→0；state∉[3,4]→
//   SendCmdErrorPacket((ENUM_CMDPACKET)0x24d,7)；PVP/FreePvP→同上；CheckInTrade→同上；
//   读 short+short；发内部流 0x30（0x10 字节）。
#include "Dispatchers/Dispatcher_AradUseAvatarRecharge.inc"
#include "Dispatchers/dtor_Dispatcher_AradUseAvatarRecharge.inc"
#include "Dispatchers/dispatch_sig_181.inc"
}
#include "Dispatchers/process_306.inc"
}
#include "Dispatchers/read_307.inc"
}

// ==================== Dispatcher_EventCreateDnfRequest（ORIG 0x81a0362）====================
// dispatch_sig：packet==0→0x7b3；state∉[3,4]→0；CEventCreateDnf 事件（GetRepeatEvent
//   /GetRate/IsRewardCondition/SendReward，未建模）标注推断：发 0x22d 响应包
//   （rate 0 + flag 0）并返回。
#include "Dispatchers/dtor_Dispatcher_EventCreateDnfRequest.inc"
#include "Dispatchers/dispatch_sig_182.inc"
}
#include "Dispatchers/process_307.inc"
}
#include "Dispatchers/read_308.inc"
}

// ==================== Dispatcher_AvartarHiddenOptionChange（ORIG 0x819f6f8）====================
// dispatch_sig：user==0→0x632；getCurCharacR==0→0；state∉[3,4]→
//   SendCmdErrorPacket((ENUM_CMDPACKET)0x24c,7)；PVP/FreePvP→同上；CheckInTrade→同上；
//   读 short+short；发内部流 0x2e（6 字节）。
#include "Dispatchers/Dispatcher_AvartarHiddenOptionChange.inc"
#include "Dispatchers/dtor_Dispatcher_AvartarHiddenOptionChange.inc"
#include "Dispatchers/dispatch_sig_183.inc"
}
#include "Dispatchers/process_308.inc"
}
#include "Dispatchers/read_309.inc"
}

// ==================== Dispatcher_RequestPrcoomDayilyReward（ORIG 0x81a0612）====================
// dispatch_sig：user==0→0x7e8；getCurCharacR==0→0；state∉[3,4]→0；
//   每日奖励剩余次数（CUser+0x8ec32）<1→SendCmdErrorPacket((ENUM_CMDPACKET)0x259,0x16)；
//   读 int itemId（0x212f/0x2130 限定，错误 0x7fb/0x800）；find_item 后
//   make_item 注入背包（invenType=0xe），成功扣减次数并发
//   InterSelectPcroomDailyReward::SendPacket / DBUpdatePcroomDailyReward::makeRequest。
#include "Dispatchers/dtor_Dispatcher_RequestPrcoomDayilyReward.inc"
#include "Dispatchers/dispatch_sig_184.inc"
}
#include "Dispatchers/process_309.inc"
}
#include "Dispatchers/read_310.inc"
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
}
#include "Dispatchers/process_310.inc"
#include "Dispatchers/read_311.inc"

#include "Dispatchers/dtor_Dispatcher_GetTodayReward.inc"
#include "Dispatchers/dispatch_sig_186.inc"
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

// ==== Dispatcher_AvatarOptionChange::read (ORIG 0x081e04f2) ====
#include "Dispatchers/read_313.inc"
}

// ==== Dispatcher_AvatarOptionChange::check_error (ORIG 0x081e0a40) ====
// 校验链：user/状态门 → Secu 保护域(0x2a) → 消耗品槽一致性/数量 → 消耗品为可堆叠的
// avatar option change 道具 → 目标槽物品一致性 → 目标为非堆叠 avatar 装备 → 锁定检查
// → job/type/grade 条件或限定集合 → option 值合法性。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e0a40 orig/ours=289/298 ndiff-blocks=29
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0xc4,%esp | mov    &_ZN10GlobalData22s_pSecuProtectionFieldE,%eax | mov    %eax,-0x24(%ebp)
#include "Dispatchers/check_error_79.inc"
}

// ==== Dispatcher_AvatarOptionChange::process (ORIG 0x081e0774) ====
// 影子背包预演删除消耗品成功后，才落到真实背包；改写目标 avatar 装备的 option 字节并回包。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e0774 orig/ours=183/184 ndiff-blocks=27
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x20(%ebp) | mov    -0x20(%ebp),%edx | mov    %eax,0x4(%edx)
#include "Dispatchers/process_312.inc"
}

// ==== Dispatcher_AvatarOptionChange::send (ORIG 0x081e0e32) ====
#include "Dispatchers/send_81.inc"
}

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

// ==== Dispatcher_BlueMarbleConfirmInfo::check_error (ORIG 0x081e68c8) ====
#include "Dispatchers/check_error_80.inc"
}

// ==== Dispatcher_BlueMarbleConfirmInfo::read (ORIG 0x081e6242) ====
#include "Dispatchers/read_314.inc"
}

// ==== Dispatcher_BlueMarbleConfirmInfo::process (ORIG 0x081e62da) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e62da orig/ours=372/364 ndiff-blocks=37
//   帧差 sub $4bc vs $4ac + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x4bc,%esp | movl   $&_ZZN32Dispatcher_BlueMarbleConfirmInfo7processEP5CUserR8MSG_BASER9ParamBaseE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $".
#include "Dispatchers/process_313.inc"
}

// ==== Dispatcher_BlueMarbleConfirmInfo::send (ORIG 0x081e6904) ====
#include "Dispatchers/send_82.inc"
}

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

// ==== Dispatcher_MailBox_Send::read (ORIG 0x081cc3da) ====
#include "Dispatchers/read_315.inc"
}

// ==== Dispatcher_MailBox_Send::GuildMailCheck (ORIG 0x081cc8c2) ====
#include "Dispatchers/GuildMailCheck.inc"
}

// ==== Dispatcher_MailBox_Send::check_error (ORIG 0x081cc636) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081cc636 orig/ours=196/226 ndiff-blocks=25
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 7 个，4.4 libstdc++ 全内联不可复现
//   首差: mov    &_ZN10GlobalData22s_pSecuProtectionFieldE,%eax | mov    %eax,-0x14(%ebp) | cmpl   $0x0,-0x14(%ebp)
#include "Dispatchers/check_error_81.inc"
}

// ==== Dispatcher_MailBox_Send::process (ORIG 0x081cc958) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081cc958 orig/ours=300/363 ndiff-blocks=49
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x40(%ebp) | mov    %eax,%edx | mov    -0x40(%ebp),%eax
#include "Dispatchers/process_314.inc"
}

// ==== Dispatcher_MailBox_Send::send (ORIG 0x081ccdb2) ====
#include "Dispatchers/send_83.inc"
}

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

// ==== Dispatcher_MonstercardBind::Dispatcher_MonstercardBind (ORIG 0x08595348 W) ====
#include "Dispatchers/Dispatcher_MonstercardBind.inc"
}

// ORIG D1 0x08244b42：设回基类 vptr、调用模板基类析构；m_rand 不释放（ORIG test 0 路径）。
#include "Dispatchers/dtor_Dispatcher_MonstercardBind.inc"
}

// ==== Dispatcher_MonstercardBind::~Dispatcher_MonstercardBind (ORIG D1 0x08244b42 / D0 0x08244b72，基类转发) ====
// ==== Dispatcher_MonstercardBind::check_need_slot (ORIG 0x081d83c4 T) ====
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x081d83c4 orig/ours=28/28
#include "Dispatchers/check_need_slot.inc"
}

// ==== Dispatcher_MonstercardBind::calcurate_rarity (ORIG 0x081d842e T) ====
#include "Dispatchers/calcurate_rarity.inc"
}

// ==== Dispatcher_MonstercardBind::get_bind_result (ORIG 0x081d84a0 T) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d84a0 orig/ours=96/95 ndiff-blocks=13
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: movswl -0x6c(%ebp),%ebx | lea    -0x61(%ebp),%edx | mov    %ebx,0xc(%esp)
#include "Dispatchers/get_bind_result.inc"
}

// ==== Dispatcher_MonstercardBind::_bind_process (ORIG 0x081d85fc T) ====
// b28c-L20 SIGFIX: bool Dispatcher_MonstercardBind::_bind_process(...)（P174 串定）
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d85fc orig/ours=187/180 ndiff-blocks=17
//   帧差 sub $74 vs $78 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x74,%esp | movl   $0x0,-0x20(%ebp)
#include "Dispatchers/_bind_process.inc"
}

// ==== Dispatcher_MonstercardBind::read (ORIG 0x081d82e8 T) ====

// ==== Dispatcher_MonstercardBind::read (ORIG 0x081d82e8 T) ====
#include "Dispatchers/read_316.inc"
}

// ==== Dispatcher_MonstercardBind::check_error (ORIG 0x081d8de2 T) ====
// 【batch-17 X1 池收口（§6.2）】orig 235 / ours 229（gap 6）：调用点差集
//   27↔27、indirect 0↔0 **归零**；残 6 为 4.1 的寄存器重载/岛步序（N76/N82）
//   ⇒ 语义完成、byte 层封顶，勿再改写。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081d8de2 orig/ours=236/228 ndiff-blocks=20
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 5 个，4.4 libstdc++ 全内联不可复现
//   首差: movswl %ax,%ebx | xor    $0x1,%eax | cmp    $0xffffffff,%eax
#include "Dispatchers/check_error_82.inc"
}

// ==== Dispatcher_MonstercardBind::process (ORIG 0x081d88b0 T) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081d88b0 orig/ours=332/347 ndiff-blocks=66
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 4 个，4.4 libstdc++ 全内联不可复现
//   首差: mov    %eax,-0x20(%ebp) | mov    -0x20(%ebp),%edx | mov    -0x20(%ebp),%eax
#include "Dispatchers/process_315.inc"
}

// ==== Dispatcher_MonstercardBind::send (ORIG 0x081d9148 T) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d9148 orig/ours=143/146 ndiff-blocks=12
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,(%esp) | call   0808e1c0 <_GLOBAL__I_ACTIVESTATUS_MAXLEVEL+0x30>  ; global constructors keyed to ACTIVESTATUS_MAXLEVEL+0x30 | addl   $0x1,
#include "Dispatchers/send_84.inc"
}

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

// ==== Dispatcher_PartyTeleport::check_error (ORIG 0x081dc19a) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081dc19a orig/ours=185/189 ndiff-blocks=8
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $0x0,-0x3c(%ebp) | movl   $&_ZN10GlobalData13s_systemTime_E,(%esp)
#include "Dispatchers/check_error_83.inc"
}

// ==== Dispatcher_PartyTeleport::read (ORIG 0x081dbe70) ====
#include "Dispatchers/read_317.inc"
}

// ==== Dispatcher_PartyTeleport::process (ORIG 0x081dbfd6) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081dbfd6 orig/ours=129/138 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0x2c(%ebp),%edx | mov    %eax,0x4(%edx) | mov    0x10(%ebp),%eax
#include "Dispatchers/process_316.inc"
}

// ==== Dispatcher_PartyTeleport::send (ORIG 0x081dc426) ====
#include "Dispatchers/send_85.inc"
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

// ==== Dispatcher_PcRoomRentItem::check_error (ORIG 0x081e5762) ====
// b28d-solo-Dispatchers verdict=AE(routed) [S206 翻正] ORIG 0x081e5762
//   [N7→P224/P45] 根因=合取形 `A&&B{body}` 产 je 反向首跳；改否定短路
//   `state!=3 || characR==0 → return 3`（ORIG cmp/jne + mov$1/$0 汇聚岛）⇒ 归零。
#include "Dispatchers/check_error_84.inc"
}

// ==== Dispatcher_PcRoomRentItem::read (ORIG 0x081e512e) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e512e orig/ours=96/94 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%eax | movl   $&_ZZN25Dispatcher_PcRoomRentItem4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp
#include "Dispatchers/read_318.inc"
}

// ==== Dispatcher_PcRoomRentItem::process (ORIG 0x081e52c0) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e52c0 orig/ours=113/115 ndiff-blocks=8
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,%edx | mov    -0x14(%ebp),%eax | mov    %edx,0x4(%eax)
#include "Dispatchers/process_317.inc"
}

// ==== Dispatcher_PcRoomRentItem::giveDimensionItems (ORIG 0x081e583e) ====
// 注：ORIG 为类成员（见文件头宿主声明补充说明）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e583e orig/ours=78/74 ndiff-blocks=14
//   帧差 sub $3c vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | push   %ebx
#include "Dispatchers/giveDimensionItems.inc"
}

// ==== Dispatcher_PcRoomRentItem::send (ORIG 0x081e542e) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e542e orig/ours=229/221 ndiff-blocks=51
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | mov    -0x1c(%ebp),%eax | add    $0x8,%eax
#include "Dispatchers/send_86.inc"
}

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

// ==== Dispatcher_PurifyItem::~Dispatcher_PurifyItem (ORIG D1 0x0825afba / D0 0x0825afea，基类转发) ====
#include "Dispatchers/dtor_Dispatcher_PurifyItem.inc"
}

// ==== Dispatcher_PurifyItem::read (ORIG 0x08218b94 T) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08218b94 orig/ours=69/80 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0xc(%ebp),%eax | add    $0xd,%eax | movl   $&_ZZN21Dispatcher_PurifyItem4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp)
#include "Dispatchers/read_319.inc"
}

// ==== Dispatcher_PurifyItem::process (ORIG 0x08218cb6 T) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08218cb6 orig/ours=488/492 ndiff-blocks=61
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN21Dispatcher_PurifyItem7processEP5CUserR8MSG_BASER9ParamBaseE19__PRETTY_FUNCTION__,0x4(%esp) | mov    0x10(%ebp),%eax | mov    %eax,-0x14
#include "Dispatchers/process_318.inc"
}

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

// ==== Dispatcher_RefundSkill::read (ORIG 0x081da6fe) ====
#include "Dispatchers/read_320.inc"
}

// ==== Dispatcher_RefundSkill::check_error (ORIG 0x081dad1c) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081dad1c orig/ours=77/75 ndiff-blocks=4
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0xc(%ebp),%eax | cmp    $0xffffffff,%eax | mov    -0xc(%ebp),%eax
#include "Dispatchers/check_error_85.inc"
}

// ==== Dispatcher_RefundSkill::process (ORIG 0x081da820) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081da820 orig/ours=347/370 ndiff-blocks=51
//   帧差 sub $c9ac vs $c9bc + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: mov    %eax,-0x2c(%ebp) | mov    -0x2c(%ebp),%edx | mov    %eax,0x18(%edx)
#include "Dispatchers/process_319.inc"
}

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
}

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

// ==== Dispatcher_SecurityCardAuthRpy::read (ORIG 0x08262658) ====
#include "Dispatchers/read_321.inc"
}

// ==== Dispatcher_SecurityCardAuthRpy::check_error (ORIG 0x0826314c) ====
#include "Dispatchers/check_error_86.inc"
}

// ==== Dispatcher_SecurityCardAuthRpy::process (ORIG 0x082627ca) ====
// 安全卡/密保卡认证应答：按 secuType 选密钥管理器解密 0x1000 内缓冲，
// 安卡走 checkAnswer/失败计数/DB 上报链，pad 走 doPassPadReplay 链。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082627ca orig/ours=486/400 ndiff-blocks=47
//   帧差 sub $108c vs $470 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x108c,%esp | mov    %eax,-0x2c(%ebp)
#include "Dispatchers/process_320.inc"
}

// ==== Dispatcher_SecurityCardAuthRpy::send (ORIG 0x08262f4e) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08262f4e orig/ours=143/146 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x20(%ebp),%eax | lea    -0x20(%ebp),%eax | lea    -0x20(%ebp),%eax
#include "Dispatchers/send_88.inc"
}

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

// ==== Dispatcher_StackableActionUse::check_error (ORIG 0x0825e332) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825e332 orig/ours=127/128 ndiff-blocks=13
//   帧差 sub $70 vs $4 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x70,%esp | mov    %eax,-0x14(%ebp) | mov    -0x14(%ebp),%edx
#include "Dispatchers/check_error_87.inc"
}

// ==== Dispatcher_StackableActionUse::read (ORIG 0x0825df72) ====
#include "Dispatchers/read_322.inc"
}

// ==== Dispatcher_StackableActionUse::process (ORIG 0x0825e11e) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825e11e orig/ours=145/158 ndiff-blocks=25
//   帧差 sub $4 vs $90 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: add    $0xffffff80,%esp | mov    %eax,-0x14(%ebp) | mov    0x14(%ebp),%eax
#include "Dispatchers/process_321.inc"
}

// ==== Dispatcher_StackableActionUse::send (ORIG 0x0825e4ec) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825e4ec orig/ours=114/116 ndiff-blocks=26
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0xc(%ebp) | mov    -0xc(%ebp),%eax
#include "Dispatchers/send_89.inc"
}

// ==== Dispatcher_UseDye::getItem (ORIG 0x081e71c0) ====
// 取 (invenType, slot) 槽位物品；返回 true=槽空（m_addInfo==0）。
#include "Dispatchers/getItem_1.inc"
}

// ==== Dispatcher_UseDye::notifyUpdatedInventoryItemInfo (ORIG 0x081e70e6) ====
#include "Dispatchers/notifyUpdatedInventoryItemInfo.inc"
}

// ==== Dispatcher_UseDye::process (ORIG 0x081e69ea) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081e69ea orig/ours=462/475 ndiff-blocks=78
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 8 个，4.4 libstdc++ 全内联不可复现
//   首差: push   %edi | sub    $0xfc,%esp | mov    %eax,-0x48(%ebp)
#include "Dispatchers/process_322.inc"
}

// ==== Dispatcher_Teleport::check_error (ORIG 0x081d056c，三参形态) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d056c orig/ours=169/167 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: je     081d063c <+0xd0> | mov    -0x14(%ebp),%eax | mov    0x2(%eax),%eax
#include "Dispatchers/check_error_88.inc"
}

// ==== Dispatcher_Teleport::process (ORIG 0x081d07bc) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d07bc orig/ours=60/62 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,%edx | mov    -0x10(%ebp),%eax | mov    %edx,0x4(%eax)
#include "Dispatchers/process_323.inc"
}

// ==== Dispatcher_Teleport::send (ORIG 0x081d087c) ====
// b28d-solo-Dispatchers verdict=AE(routed) [S162] ORIG 0x081d087c orig/ours=135/135
//   [S162] 已修：stTeleportParam::m_14 偏移 0x12→0x14（PacketTypes.h m_pad12[2]，
//   ORIG ctor 0x08231362 写 +0x14 实证）⇒ 唯一残差（读 +0x12 vs +0x14）归零。
#include "Dispatchers/send_90.inc"
}


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

// ==== Dispatcher_CeraPackageOpen::check_error (ORIG 0x081e7cc2) ====
#include "Dispatchers/check_error_89.inc"
}

// ==== Dispatcher_CeraPackageOpen::read (ORIG 0x081e76fa) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e76fa orig/ours=95/98 ndiff-blocks=12
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0x10(%ebp),%eax | movl   $&_ZZN26Dispatcher_CeraPackageOpen4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp) | mov    -0x10(%ebp),%e
#include "Dispatchers/read_323.inc"
#define MPK (reinterpret_cast<MSG_OPEN_CERAPACKAGE*>(&msg))
    if (!packet.get_short(*reinterpret_cast<short*>(&MPK->m_0e)))
    {
        return LineFunc(
            0x636b,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(&MPK->m_pad10[0])))
    {
        return LineFunc(
            0x636e,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    // cmp $0x23,%al; jbe → unsigned byte ≤ 0x23 才通过
    if (*reinterpret_cast<const unsigned char*>(&MPK->m_pad10[0]) > 0x23)
    {
        return LineFunc(
            0x6371,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    for (unsigned int i = 0;
         i < *reinterpret_cast<const unsigned char*>(&MPK->m_pad10[0]); ++i)
    {
        if (!packet.get_int(
                *reinterpret_cast<unsigned long*>(M + (i + 2) * 8 + 4)))
        {
            return LineFunc(
                0x6376,
                __PRETTY_FUNCTION__,
                0, 0);
        }
        if (!packet.get_byte(
                *reinterpret_cast<unsigned char*>(M + (i + 2) * 8 + 8)))
        {
            return LineFunc(
                0x6379,
                __PRETTY_FUNCTION__,
                0, 0);
        }
    }
    return 0;
}
#undef MPK

// ==== Dispatcher_CeraPackageOpen::process (ORIG 0x081e788a) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e788a orig/ours=289/287 ndiff-blocks=45
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x28(%ebp) | mov    -0x28(%ebp),%edx
#include "Dispatchers/process_324.inc"
}

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

// ==== Dispatcher_ExpertExtraction::read (ORIG 0x081d9336) ====
#include "Dispatchers/read_324.inc"
}

// ==== Dispatcher_ExpertExtraction::process (ORIG 0x081d9412) ====
namespace expert_extraction
{
#include "Dispatchers/GetExtracterType.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081d9412 orig/ours=162/159 ndiff-blocks=32
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x18(%ebp) | mov    -0x18(%ebp),%edx | mov    -0x18(%ebp),%eax
#include "Dispatchers/process_325.inc"
}

// ==== Dispatcher_ExpertExtraction::check_error (ORIG 0x081d962e) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081d962e orig/ours=67/64 ndiff-blocks=11
//   虚调用目标寄存器 N7：ORIG `mov (%eax),%ebx; call *%ebx`（push ebx ⇒ 全槽深 4）；
//   4.4 恒目标 %eax 无 ebx ⇒ 帧差 4。已修段：uchar 读 + (char) 显式转换复现 ORIG
//   0x081d966d `movzbl; movsbl` 两段（原缺 movsbl）。N7 属后端族，路由表由 Main 管。
#include "Dispatchers/check_error_90.inc"
}

// ==== Dispatcher_ExpertExtraction::send (ORIG 0x081d9700) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d9700 orig/ours=130/137 ndiff-blocks=25
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | mov    -0x1c(%ebp),%eax | jne    081d989d <+0x19d>
#include "Dispatchers/send_91.inc"
}

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

// ==== Dispatcher_FatigueAttendance::check_error (ORIG 0x081db310) ====
#include "Dispatchers/check_error_91.inc"
}

// ==== Dispatcher_FatigueAttendance::read (ORIG 0x081dafa4) ====
#include "Dispatchers/read_325.inc"
}

// ==== Dispatcher_FatigueAttendance::process (ORIG 0x081daffa) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081daffa orig/ours=215/217 ndiff-blocks=29
//   帧差 sub $40 vs $54 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x40,%esp | mov    %eax,-0x2c(%ebp)
#include "Dispatchers/process_326.inc"
}

// ==== Dispatcher_FatigueAttendance::send (ORIG 0x081db392) ====
#include "Dispatchers/send_92.inc"
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


// ==== Dispatcher_ObjectBringUp::check_error (ORIG 0x081e16ce) ====
#include "Dispatchers/check_error_92.inc"
}

// ==== Dispatcher_ObjectBringUp::read (ORIG 0x081e147e) ====
#include "Dispatchers/read_326.inc"
}

// ==== Dispatcher_ObjectBringUp::process (ORIG 0x081e14d4) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e14d4 orig/ours=137/144 ndiff-blocks=26
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x74,%esp | mov    0x10(%ebp),%eax | mov    %eax,-0x18(%ebp)
#include "Dispatchers/process_327.inc"
}

// ==== Dispatcher_ObjectBringUp::send (ORIG 0x081e1704) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e1704 orig/ours=154/163 ndiff-blocks=34
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0xc(%ebp) | lea    -0x1c(%ebp),%eax
#include "Dispatchers/send_93.inc"
}

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

// ==== Dispatcher_RequestAssaultPrice::read (ORIG 0x081dee2a) ====
#include "Dispatchers/read_327.inc"
}

// ==== Dispatcher_RequestAssaultPrice::process (ORIG 0x081dee80) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081dee80 orig/ours=146/141 ndiff-blocks=25
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | mov    %eax,-0x18(%ebp) | mov    -0x18(%ebp),%eax
#include "Dispatchers/process_328.inc"
}

// ==== Dispatcher_RequestAssaultPrice::check_error (ORIG 0x081df084) ====
#include "Dispatchers/check_error_93.inc"
}

// ==== Dispatcher_RequestAssaultPrice::send (ORIG 0x081df294) ====
#include "Dispatchers/send_94.inc"
}

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

// ==== Dispatcher_RequestIngameAdvertisement::check_error (ORIG 0x081e4662) ====
// b28d-solo-Dispatchers verdict=AE(routed) [S206 翻正] ORIG 0x081e4662
//   [N96→P316/P226] 根因=具名 bool ok 落槽（-0x9 存取对）且 p 槽 -0xc→-0x10；
//   改语句表达式 `if (({ failCnt<=2; }))` 零槽寄存器物化（setle;test;je）⇒ 归零。
#include "Dispatchers/check_error_94.inc"
}

// ==== Dispatcher_RequestIngameAdvertisement::read (ORIG 0x081e434c) ====
#include "Dispatchers/read_328.inc"
}

// ==== Dispatcher_RequestIngameAdvertisement::process (ORIG 0x081e43a2) ====
// ParamBase 绝对偏移（汇编 disp 原样）：
//   +0x640 int 错误码 | +0x644 short 广告类型（回显 msg+0xd）
//   +0x004 起内嵌 stNotifyIngameADInfo（未建模，全偏移承载）：
//     +0x004 int（send put_int 首字段/WriteLog 第 5 参 uint）
//     +0x008 char[]（描述文本，strlen 口径）| +0x03a char[]（备选文本）
//     +0x139 char[] / +0x238 int / +0x23c char[] / +0x33b byte 广告形态 /
//     +0x33c byte 日志开关 / +0x33d byte 奖品形态 / +0x33e char[] /
//     +0x43d char[] / +0x53c ulong 奖品 idx A / +0x540 char[] / +0x23c 同理 B 区
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081e43a2 orig/ours=182/186 ndiff-blocks=12
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: movl   $&_ZZN37Dispatcher_RequestIngameAdvertisement7processEP5CUserR8MSG_BASER9ParamBaseE19__PRETTY_FUNCTION__,0x4(%esp) | mov    0x10(%ebp),%eax | m
#include "Dispatchers/process_329.inc"
}

// ==== Dispatcher_RequestIngameAdvertisement::send (ORIG 0x081e46c8) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e46c8 orig/ours=319/292 ndiff-blocks=26
//   帧差 sub $40 vs $50 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x40,%esp | mov    %eax,-0x14(%ebp) | mov    -0x14(%ebp),%eax
#include "Dispatchers/send_95.inc"
}

// ==== Dispatcher_RequestIngameAdvertisement::sendRewardItemSystemMail (ORIG 0x081e4b6c) ====
// 头声明缺失：宿主需在 Dispatchers.h 补
//   void sendRewardItemSystemMail(CUser* user, unsigned long itemIdx, int count);
#include "Dispatchers/sendRewardItemSystemMail.inc"
}

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

// ==== Dispatcher_SeriaRoom_Ani_Deco_Event::process (ORIG 0x081e886a) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e886a orig/ours=187/199 ndiff-blocks=48
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x9c,%esp | mov    %eax,-0x2c(%ebp) | mov    %eax,-0x28(%ebp)
#include "Dispatchers/process_330.inc"
}

// ==== Dispatcher_SeriaRoom_Ani_Deco_Event::send (ORIG 0x081e8b64) ====
#include "Dispatchers/send_96.inc"
}

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

// ==== Dispatcher_Skill_Command_Customizing::read (ORIG 0x081d29ac) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d29ac orig/ours=103/101 ndiff-blocks=16
//   帧差 sub $28 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x28,%esp | movl   $&_ZZN36Dispatcher_Skill_Command_Customizing4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp) | mov    0x10(%ebp)
#include "Dispatchers/read_329.inc"
}

// ==== Dispatcher_Skill_Command_Customizing::process (ORIG 0x081d2b42) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d2b42 orig/ours=174/161 ndiff-blocks=42
//   帧差 sub $50 vs $68 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x50,%esp
#include "Dispatchers/process_331.inc"
}

// ==== Dispatcher_Skill_Command_Customizing::send (ORIG 0x081d2da4) ====
#include "Dispatchers/send_97.inc"
}

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

// ==== Dispatcher_SoloTeleport::read (ORIG 0x081e1b9c) ====
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x081e1b9c orig/ours=117/117
#include "Dispatchers/read_330.inc"
}

// ==== Dispatcher_SoloTeleport::process (ORIG 0x081e1d9c) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e1d9c orig/ours=81/83 ndiff-blocks=18
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x14(%ebp) | mov    0x10(%ebp),%eax | mov    -0x10(%ebp),%eax
#include "Dispatchers/process_332.inc"
}
// ==== Dispatcher_SoloTeleport::check_error (ORIG 0x081e1e98) ====
// 注意:Dispatchers.h 现缺本声明,接入时须在类内补(见文件头)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e1e98 orig/ours=107/111 ndiff-blocks=23
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x20(%ebp) | mov    -0x20(%ebp),%eax | mov    -0x20(%ebp),%eax
#include "Dispatchers/check_error_95.inc"
}

// ==== Dispatcher_SoloTeleport::send (ORIG 0x081e1ff6) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e1ff6 orig/ours=123/126 ndiff-blocks=26
//   帧差 sub $40 vs $50 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x40,%esp | mov    %eax,-0xc(%ebp) | lea    -0x38(%ebp),%eax
#include "Dispatchers/send_98.inc"
}

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

// ==== Dispatcher_TournamentRewardSelect::check_error (ORIG 0x081e046c) ====
#include "Dispatchers/check_error_96.inc"
}

// ==== Dispatcher_TournamentRewardSelect::read (ORIG 0x081dffda) ====
#include "Dispatchers/read_331.inc"
}

// ==== Dispatcher_TournamentRewardSelect::process (ORIG 0x081e00f0) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e00f0 orig/ours=126/122 ndiff-blocks=17
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0x1c(%ebp)
#include "Dispatchers/process_333.inc"
}

// ==== Dispatcher_TournamentRewardSelect::send (ORIG 0x081e02b0) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e02b0 orig/ours=126/120 ndiff-blocks=19
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x18(%ebp) | lea    -0x24(%ebp),%eax
#include "Dispatchers/send_99.inc"
}

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

// computeHpDamageSDC —— ORIG T _Z18computeHpDamageSDCP23STComputeDamageVariablePvS1_
//   @0x08095c32 size 0xac8（0x08095c32..0x080966f9）逐指令转写；gcc4.4.7 -m32 -O0 口径，C++98。
// FPU 语义已用 32 位汇编实测定案：fsubrp=st(1)-st(0)，fdivrp=st(1)/st(0)；x87 (int) 即 trunc。
// 返回 (int)trunc(dmg) 并写 v->m_expectedHp；第二参全程未用【asm】。
#include "Dispatchers/computeHpDamageSDC_1.inc"
}

// ---- Packet_Mannerless_User_Accusation ctor ----
// ORIG W _ZN33Packet_Mannerless_User_AccusationC1Ev @0x0822c1e6（0xaf）。
// b22 桥清零（P201，Model22）：PacketHeader(type=0x1b66,size=0x15f) 原 extern"C"
// mangled 桥（ORIG W _ZN12PacketHeaderC1Ett @0x08110b70）转 placement new 真构造
// ——本 TU 编译视图为 SigTypes_E.h POD 承载（+0x00..0x05 m_pad0、m_uid@+0x06 与
// ORIG connNo 覆区），不可继承 PacketHeader（字段偏移整体漂移即破坏 16396 消费
// 点布局），placement new 于 +0 真调 ctor，写区/参序与 ORIG 逐字节同形。

// ORIG W 0x0822c1ca（_ZN 非 const）：(unsigned)type <= 2。
#include "Dispatchers/isNeedTypeForChattingMsg.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG  orig/ours=38/63 ndiff-blocks=3
//   帧差 sub $18 vs $2c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x18,%esp | mov    0x8(%ebp),%eax | leave
#include "Dispatchers/Packet_Mannerless_User_Accusation.inc"
}

// ---- DB_SecurityCardRetransfer::makeRequest ----
// ORIG T _ZN25DB_SecurityCardRetransfer11makeRequestEiPcS0_jPKcS2_S2_ @0x0842c1f6（0x16b）。
// W8LT_DBTask.cpp 占位类已产生 C1；本 TU 提供唯一定义。静态成员（无 this）【asm 核实】。

#include "Dispatchers/makeRequest_4.inc"
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

// ---- D3 解阻依赖：DB_RequestJoinGuild::makeRequest(int, SIG_REQUEST_JOIN_GUILD*) ----
// ORIG T _ZN19DB_RequestJoinGuild11makeRequestEiP22SIG_REQUEST_JOIN_GUILD @0x0843d19a（0xec）。
// W8LT_DBTask.cpp:378 占位类已产生 C1；类声明见 TU 顶部（§8 全禁），本 TU 提供唯一定义。
#include "Dispatchers/makeRequest_5.inc"
}


// ==== StubS3A.S3A_DEPS：check_error×2 + isVaildEmotionValue（ORIG T 0x081d14aa/0x081d18dc/0x081d1ebe）====

// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d14aa orig/ours=81/80 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: jne    081d14d7 <+0x2d> | mov    $0xffffffff,%eax | jmp    081d15ab <+0x101>
#include "Dispatchers/check_error_97.inc"
}

#include "Dispatchers/check_error_98.inc"
}

// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d1ebe orig/ours=54/45 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    0x4d44(%eax),%edx | lea    -0x10(%ebp),%eax | lea    -0x10(%ebp),%eax
#include "Dispatchers/isVaildEmotionValue.inc"
}

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

// ===== Dispatcher_BuyCeraShopItem::useCountDownCoinInFreeCoinDungeon —— ORIG 0x081fe4b8 =====
// ORIG：user!=0；msg+0x36(byte)==1；FindGoods(msg+0x39 int) 非空且 GetItemIdx()==1；
// GetParty() 非空；party+0xcac(CDungeon*) 非空；checkFreeRevivalCondition(dungeon) 非 0 → true。
// [b28c-32] verdict=DIFF：+0x39 具名化+P222 正向包裹；余 N18a 单条 eax→ebx 中转（59=60）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081fe4b8 orig/ours=61/59 ndiff-blocks=2
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x39(%eax),%eax | mov    %eax,%ebx | nop
#include "Dispatchers/useCountDownCoinInFreeCoinDungeon.inc"
}

// ===== Dispatcher_Compound_Equipment_Upgrade_Card::set_ReturnSendPacket —— ORIG 0x08228a2e =====
// ORIG：put_byte(0) → put_short(param_2=slot) → put_int(param_3=quantity)。
#include "Dispatchers/set_ReturnSendPacket.inc"
}

// ===== Dispatcher_Enchant_3rdChronicleItem::check_state —— ORIG 0x082328ec =====
// ORIG：state!=3 或 getCurCharacR()==0 → LineFunc(0x5f6)；CheckInTrade() → LineFunc(0x5f9)；
// 通过 → 0（失败路径返回值即 LineFunc 返回）。
#include "Dispatchers/check_state.inc"
}

// ===== Dispatcher_GuildCargoPushItem::check_error —— ORIG 0x081d0f64 =====
// ORIG（3 参数符号；字段自 ParamBase 读）：
//   state!=3 或 getCurCharacR()==0 → -1；guildkey==0 → 100；IsExistGuildAgit()!=1 → 0xb5；
//   invenType=GetInvenTypeFromItemSpace(param+0xd)；slot=param+0xe(ushort)；
//   GetInvenSlot(inven, invenType, slot)；Secu Check(user,8) 非 0 → 透传；
//   CheckItemLock(invenType,slot) → 0xd5；param+0x10==0 或 != 槽内 m_addInfo → 0xc5；
//   find_item(m_addInfo)==0 → 0xc5；通过 → 0。
// ===== Dispatcher_ModItemAttr::_SendResult —— ORIG 0x08201938 =====
// ORIG 参数映射：(user, param_2=resultType, param_3=matSlot, param_4=equipSlot)。
// 包：header(1,0x54) | byte(1) | short(matSlot) | int(matSlot 槽 get_add_info()) |
//     short(resultType) → finalize(true) → CUser::Send → SendUpdateItemList(1,0,equipSlot)。
#include "Dispatchers/_SendResult.inc"
}

// ===== Dispatcher_PurifyItem::processPurifyType —— ORIG 0x082194b6 =====
// ORIG：type==0 → 直接 return identify()（bool 值原样穿过 %eax，无 test/movzbl 物化）；
// type==1 → reset 后 return 1；其它 → 0。caller process @0x8219166 `xor $0x1,%eax`
// 全字反证 %eax∈{0,1}：bool 返回 ABI 下成立。
#include "Dispatchers/processPurifyType.inc"
}

// ===== Dispatcher_Select_3rdChronicleItem_ForEnchant::check_state —— ORIG 0x08232854 =====
// ORIG：state!=3 或 getCurCharacR()==0 → LineFunc(0x5e1)；CheckInTrade() → LineFunc(0x5e4)；
// 通过 → 0（失败路径返回值即 LineFunc 返回）。
#include "Dispatchers/check_state_1.inc"
}

// ===== DimensionActivationRewardData::getRewardGiveItems —— ORIG 0x08915f9a =====
// ORIG：list=getRewardGiveItemList(job, growType)；list 非空且
//   selectionIdx>=0 且 selectionIdx<=list->size()（ORIG cmp/jl 边界）→
//   返回 &list->at(selectionIdx)（ORIG 调 at，idx==size 时由 at 抛异常）；否则 0。
// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x08915f9a orig/ours=34/34
#include "Dispatchers/getRewardGiveItems.inc"
}
