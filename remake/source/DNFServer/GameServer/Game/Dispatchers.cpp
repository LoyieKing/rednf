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
bool isSetBit_Uint(unsigned int value, int bit);
void setBit_Uint(unsigned int& value, bool set, int bit);
#include "CEventScriptMng.h"   // EventClassify::CEventScriptMng / CEventEntity /
#include "CEventScriptMng.h"   // 新增：EventClassify::CEventScriptMng/CObjectBringUp
#include "CTournamentDungeonReward.h"   // CTournamentDungeonReward /
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
#include "CompoundEmblemDeps.h"      // LineFunc(int, char const*, int, unsigned)
#include "CompoundEmblemDeps.h" // LineFunc(int, char const*, int, unsigned)
#include "GameEnums.h"          // ENUM_SKILL_TREE_KIND / eItemDelReason / ENUM_CMDPACKET
#include "GameTypes.h"          // cMyTrace
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

// ==== StubS3A producer（d_aa body 0a/gmList）====
// ORIG _Z9check_urlPKc T 0x08205791（无报告，objdump 转写；CheckGuildAddressDouble 用）。
bool check_url(const char* url);
// ORIG _ZL6gmList 文件级静态 @data 0x093701d8（5×int，objdump -s 实测初值）。
// PacketDispatcher_Impl_1.cpp 静态；AuctionMyRegistedItemInfo 等 GM 迟延 trace 用。
static int gmList[5] = { 0x004c2dc8, 0x005cf723, 0x00a9c742, 0x00a2ad85, 0x009ae1ae };

bool check_url(const char* url)
{
    if (url == 0)
        return false;
    if ((unsigned char)url[0] == 0x5f)                 // cmp $0x5f,%al（首字符 '_' 拒绝）
        return false;
    for (int i = 0; strlen(url) > (unsigned int)i; i++)
    {
        unsigned char c = (unsigned char)url[i];
        if (c > 0x60 && c <= 0x7a)                     // a..z
            continue;
        // ORIG 0x82057d0 控制流：c<=0x2f → jbe 0x57f8（0x57f8 检查 c==0x5f，
        // c<=0x2f 时恒不成立 → 拒绝）；0x30..0x57（数字/大写/部分标点）→ 允许。
        if (c > 0x2f && c <= 0x57)                     // '0'..'W'
            continue;
        if (c == 0x5f)
            continue;
        return false;
    }
    return true;
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
int decreaseRankOfFairPVPEquipmentPackageKey(int key)
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

// DspD4 B4：DB_JoinGuildInfo / DB_CancelJoinGuild makeRequest 定义
// （ORIG _ZN16DB_JoinGuildInfo11makeRequestEi17ENUM_SERVER_GROUPj @0x0843d5fa、
//  _ZN18DB_CancelJoinGuild11makeRequestEijj @0x0843d78c；nm 核实。包构建范式
//  同 DB_AccountCargoSync.cpp：StreamPool::Acquire → CStreamGuard → DB_Q 入队。）
void DB_JoinGuildInfo::makeRequest(int uid, ENUM_SERVER_GROUP serverGroup, unsigned int characNo)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9218), true);
    **guard << 0x219;               // opcode（ORIG lsEi）
    **guard << uid;                 // int
    **guard << serverGroup;         // ENUM_SERVER_GROUP（int 重载）
    **guard << characNo;            // unsigned 重载（ORIG lsEj）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void DB_CancelJoinGuild::makeRequest(int uid, unsigned int requestIdx, unsigned int characNo)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9234), true);
    **guard << 0x218;               // opcode（ORIG lsEi）
    **guard << uid;                 // int
    **guard << requestIdx;          // unsigned（ORIG lsEj）
    **guard << characNo;            // unsigned（ORIG lsEj）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
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
int _NS_IPScan(CUser* user, PacketBuf& packet);
int _NS_MI_RecvMemoryIntegrityModulation(CUser* user, PacketBuf& packet);
int _NS_NonClient_GetResponse(CUser* user, PacketBuf& packet);
int _NS_PI_2ND_RecvPacketIntegrityModulation_C2C(CUser* user, PacketBuf& packet);
int _NS_PI_2ND_RecvPacketIntegrityModulation_S2C(CUser* user, PacketBuf& packet);
int _NS_ProcessScan(CUser* user, PacketBuf& packet);
int _NS_PacketIntegrity(CUser* user, PacketBuf& packet);

#include "STComputeDamageVariable.h"   // D1 依赖：SDC 伤害变量权威布局（computeHpDamageSDC）
// ============================================================================
// D1 依赖前向声明（定义在本 TU 文件尾的 D1 依赖定义区）：
int computeHpDamageSDC(STComputeDamageVariable* v, void* arg2, void* defender);  // ORIG T 0x08095c32
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
int decryptBVHackPacket(char* payload, stBVHackInfo_t& info);

// ORIG T 0x082138f2 _Z27IsAuctionServerDisconnectedh：
// !CAuctionServerProxy::IsRunning() 或 payType==1 判定（0x8234f44 调用点）。
bool IsAuctionServerDisconnected(unsigned char payType);

// ORIG T 0x081f793c _Z11MakePVPRoomP5CUserR17MSG_MAKE_PVP_ROOM14ENUM_CMDPACKET15PVP_BATTLE_MODE
struct MSG_MAKE_PVP_ROOM;   // 完整布局仅经偏移视图访问（QuickJoinRoom msgBuf 0x3d）
bool MakePVPRoom(CUser* user, MSG_MAKE_PVP_ROOM& msg, ENUM_CMDPACKET cmd, PVP_BATTLE_MODE mode);

// ORIG T 0x081ed09a _Z12EnterPVPRoomP5CUserP8PvP_Room14ENUM_CMDPACKETPKc
bool EnterPVPRoom(CUser* user, PvP_Room* room, ENUM_CMDPACKET cmd, char const* password);

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
          m_itemId(0), m_count(0)
    {
        *(unsigned int*)((char*)this + 0x1e) = 0;   // +0x1e..+0x21
        *(unsigned int*)((char*)this + 0x22) = 0;   // +0x22..+0x25
    }
    PacketHeader m_header;                // +0x00..+0x09 包头
    unsigned int m_itemIdReq;             // +0x0a（GetUID）
    unsigned int m_dbid;                  // +0x0e（acc_id）
    unsigned int m_charNo;                // +0x12（charac_no）
    unsigned int m_itemId;                // +0x16（msg+0xd）
    unsigned int m_count;                 // +0x1a
    unsigned char m_tail1e[0x16];         // +0x1e..+0x33（仅前 8 字节清零）
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
};

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
int procStagePause(advancealtar::StageControl* stage, bool flag)
{
    if (stage == 0)
    {
        return 0;
    }
    advancealtar::ProcStage* ps = stage->getProcStage();
    int state = ps->getState();
    if (flag)
    {
        if (state != 3)
        {
            return 0;
        }
        ps->setState(static_cast<advancealtar::ProcStage::State::T>(2));
    }
    else
    {
        if (state != 2 && state != 1)
        {
            return 0;
        }
        ps->setState(static_cast<advancealtar::ProcStage::State::T>(3));
    }
    return 1;
}

// ORIG CharacData 位于 StageControl+0x98；+0x08 gageLevel，+0x0c gage。
advancealtar::CharacData* getCharacData(advancealtar::StageControl* stage)
{
    return stage ? stage->getCharacData() : 0;
}

int characDataGetGageLevel(advancealtar::CharacData* cd)
{
    return cd ? cd->m_gageLevel : 0;
}

int characDataGetGage(advancealtar::CharacData* cd)
{
    return cd ? cd->m_gage : 0;
}

// ORIG CharacData::upgreadGage @ 0x812d176（依赖 CDataManager+0x6364 配置区：
// +0x6368 float 系数、+0x6370 等级花费表、+0x637c 基数指针）。
int characDataUpgreadGage(advancealtar::CharacData* cd)
{
    if (cd == 0)
    {
        return 3;
    }
    int gageLevel = cd->m_gageLevel;
    int gage = cd->m_gage;
    if (gageLevel == 3)
    {
        return 2;
    }
    advancealtar::ConfigParameter& cfg = G_CDataManager()->m_altarConfig;
    int base = *reinterpret_cast<int*>(cfg.m_strPtr);
    float rate = cfg.m_4;
    int costTbl = cfg.m_gageUpAux[gageLevel];
    int cost = base * (int)(rate * (float)costTbl + 0.5f);
    if (gage < cost)
    {
        return 3;
    }
    cd->m_gage = gage - cost;
    if (cd->m_gageLevel == 1)
    {
        cd->m_gageLevel = 2;
    }
    else if (cd->m_gageLevel == 2)
    {
        cd->m_gageLevel = 3;
    }
    else if (cd->m_gageLevel == 0)
    {
        cd->m_gageLevel = 1;
    }
    return 0;
}

// ORIG CharacData::summonSlotUnit @ 0x812d278：dm+0x6364 处
// map<uchar,_SlotItem>；_SlotItem +0x00 itemId、+0x04 needGage、+0x08 field8。
int characDataSummonSlotUnit(advancealtar::CharacData* cd, unsigned char slotType, int unitId)
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

// ==================== Dispatcher_Pause（ORIG 0x8140a1c）====================
// dispatch_sig：state/charac 校验失败返回 0x52c；否则 read(0xe) → process →
// send_packet(0x242, 0xc) → 0。
// process：res+0x08 = req+0x0d（pause 标志）；check_error 恒 0；
//          Manager::pause(user, flag) → StageControl+0x04 ProcStage::pause。
Dispatcher_Pause::Dispatcher_Pause() {}
Dispatcher_Pause::~Dispatcher_Pause() {}
int Dispatcher_Pause::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x242)
        return 0;
    }
    return 0x52c;
}
int Dispatcher_Pause::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Fix6：ORIG 0x814241a 空壳（mov $0x0,%eax; ret），不转调 process_packet。
    (void)user; (void)msg; (void)param;
    return 0;
}
int Dispatcher_Pause::read(PacketBuf& packet, MSG_BASE& msg)
{
    // Fix6：ORIG 0x8142410 空壳（mov $0x0,%eax; ret）；get_binary 在 read_packet 中。
    (void)packet; (void)msg;
    return 0;
}
void Dispatcher_Pause::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140bae
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x242);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0xc);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法（独立 const 成员，指令级转写自报告）----
bool Dispatcher_Pause::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return false;  // [objdump 0x08140b32] mov $0x0,%eax; pop %ebp; ret
}
int Dispatcher_Pause::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140b04] get_binary(&req, 0xe); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0xe);
    return 0;
}
int Dispatcher_Pause::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                     CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x08140b3c]：resp+4 = check_error；==0 时 resp+8 = req+0xd 并 Manager::pause。
    char* sb = resBytes(resp);
    *(reinterpret_cast<int*>(sb + 4)) = static_cast<int>(check_error(user, req));
    if (*(reinterpret_cast<int*>(sb + 4)) == 0)
    {
        const char* rb = reqBytes(req);
        sb[8] = rb[0xd];                                   // pause 标志回显
        Manager::pause(user, rb[0xd] != 0);                // ORIG T 0x0813105c
    }
    return 0;
}

// ==================== Dispatcher_BuyItem（ORIG 0x813fad0）====================
// dispatch_sig：校验失败返回 0x380；read(0x20) → process → send_packet(0x23d, 0x1c)。
// process：res 回显 req 全部字段；res+0x04 = check_error(0)；buyItem 依赖
//          CharacAdvanceAltarManager 内部 AdvanceAltarData（未建模），标注推断。
Dispatcher_BuyItem::Dispatcher_BuyItem() {}
Dispatcher_BuyItem::~Dispatcher_BuyItem() {}
int Dispatcher_BuyItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x23d)
        return 0;
    }
    return 0x380;
}
int Dispatcher_BuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Fix6：ORIG 0x8142230 空壳（mov $0x0,%eax; ret），不转调 process_packet。
    (void)user; (void)msg; (void)param;
    return 0;
}
int Dispatcher_BuyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // Fix6：ORIG 空壳（mov $0x0,%eax; ret）；get_binary 在 read_packet 中。
    (void)packet; (void)msg;
    return 0;
}
void Dispatcher_BuyItem::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x813fe82
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x23d);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0x1c);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
bool Dispatcher_BuyItem::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    // [objdump 0x0813fbe6] return !isOpenAdvanceAltar();
    return !isOpenAdvanceAltar();
}
int Dispatcher_BuyItem::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x0813fbb8] get_binary(&req, 0x20); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x20);
    return 0;
}
int Dispatcher_BuyItem::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                       CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x0813fc06]
    const char* rb = reqBytes(req);
    char* sb = resBytes(resp);
    *(int*)(sb + 0x08) = *(const int*)(rb + 0x10);                    // buyIndex 回显
    *(int*)(sb + 0x0c) = *(const int*)(rb + 0x14);                    // shopType
    *(int*)(sb + 0x10) = *(const int*)(rb + 0x18);                    // level
    *(unsigned short*)(sb + 0x14) = *(const unsigned short*)(rb + 0x1c);  // fromSlot
    *(unsigned short*)(sb + 0x16) = *(const unsigned short*)(rb + 0x1e);  // toSlot
    *(int*)(sb + 4) = static_cast<int>(check_error(user, req));
    if (*(int*)(sb + 4) == 0)
    {
        CDataManager* dm = G_CDataManager();                          // ORIG call G_CDataManager
        std::vector<advancealtar::BuyUpgradeData>* vec =
            dm->m_altarShop.getBuyUpgradeDataVec(
                *(const int*)(rb + 0x10),
                static_cast<AdvanceAltarShopType::T>(*(const int*)(rb + 0x14)));
        if (vec == 0)                              // [objdump 0x813fcbf] err=3
        {
            *(int*)(sb + 4) = 3;
            return 0;
        }
        const BuyUpgradeData* found = 0;           // 线性查找 item 匹配行
        for (std::vector<BuyUpgradeData>::iterator it = vec->begin(); it != vec->end(); ++it)
        {
            if (it->m_nItemId == *(int*)(sb + 0x10))   // [objdump 0x813fd04] cmp *it==resp+0x10
            {
                found = &(*it);
                break;
            }
        }
        if (found == 0)                            // [objdump 0x813fd6b] err=3
        {
            *(int*)(sb + 4) = 3;
            return 0;
        }
        int totalCost = 0;
        for (int lv = (short)*(const unsigned short*)(rb + 0x1c);
             lv <= (short)*(const unsigned short*)(rb + 0x1e); ++lv)
        {
            FieldData* fd = const_cast<BuyUpgradeData*>(found)->getFieldDataPoint(
                lv, static_cast<FieldType::T>(2));                 // ORIG T 0x088a2e4c
            if (fd == 0)                       // [objdump 0x813fdb6] err=3
            {
                *(int*)(sb + 4) = 3;
                return 0;
            }
            totalCost += fd->m_value;          // [objdump 0x813fdca] add +4(int)
        }
        int rewardIdx = 0;
        int rc = user->m_advanceAltar.buyItem(                     // ORIG T 0x081319ae
            static_cast<AdvanceAltarShopType::T>(*(const int*)(rb + 0x14)),
            *(const int*)(rb + 0x18),
            (short)*(const unsigned short*)(rb + 0x1c),
            (short)*(const unsigned short*)(rb + 0x1e),
            totalCost, rewardIdx);
        *(int*)(sb + 4) = rc;
        *(int*)(sb + 0x18) = rewardIdx;
        if (rc == 0)
        {
            HistoryLog::starSub(*user, static_cast<SubStarType::T>(0), totalCost);  // ORIG T 0x08133db0
        }
    }
    return 0;
}

// ==================== Dispatcher_SetSlot（ORIG 0x813ff3e）====================
// dispatch_sig：校验失败返回 0x3fc；read(0x24) → process → send_packet(0x23e, 0x1c)。
// process：res 回显 req（含 _SetSlotData 12B）；slotType 非 1/2 → error=2；
//          setSlot 依赖未建模 AdvanceAltarData，标注推断。
Dispatcher_SetSlot::Dispatcher_SetSlot() {}
Dispatcher_SetSlot::~Dispatcher_SetSlot() {}
int Dispatcher_SetSlot::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x23e)
        return 0;
    }
    return 0x3fc;
}
int Dispatcher_SetSlot::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Fix6：ORIG 0x8142292 空壳（mov $0x0,%eax; ret），不转调 process_packet。
    (void)user; (void)msg; (void)param;
    return 0;
}
int Dispatcher_SetSlot::read(PacketBuf& packet, MSG_BASE& msg)
{
    // Fix6：ORIG 空壳（mov $0x0,%eax; ret）；get_binary 在 read_packet 中。
    (void)packet; (void)msg;
    return 0;
}
void Dispatcher_SetSlot::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140148
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x23e);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0x1c);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
bool Dispatcher_SetSlot::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return !isOpenAdvanceAltar();  // [objdump 0x0814004e]
}
int Dispatcher_SetSlot::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140020] get_binary(&req, 0x24); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x24);
    return 0;
}
int Dispatcher_SetSlot::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                       CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x0814006e]
    const char* rb = reqBytes(req);
    char* sb = resBytes(resp);
    sb[0x0c] = rb[0x14];                                   // isSet 回显（byte）
    *(int*)(sb + 8) = *(const int*)(rb + 0x10);            // field08
    std::memcpy(sb + 0x10, rb + 0x18, 0xc);                // _SetSlotData 整块回显
    *(int*)(sb + 4) = static_cast<int>(check_error(user, req));
    if (*(int*)(sb + 4) == 0)
    {
        int slotType = *(const int*)(rb + 0x18);           // _SetSlotData.m_type
        if (slotType == 2 || slotType == 1)                // [objdump 0x081400f4]
        {
            int rc = user->m_advanceAltar.setSlot(         // ORIG T 0x08131b58
                rb[0x14] != 0,
                *reinterpret_cast<const CMDPacketStruct::_SetSlotData*>(rb + 0x18));
            *(int*)(sb + 4) = rc;
        }
        else
        {
            *(int*)(sb + 4) = 2;
        }
    }
    return 0;
}

// ==================== Dispatcher_ResetStar（ORIG 0x8140f6e）====================
// dispatch_sig：校验失败返回 0x5c0；read(0x14) → process → send_packet(0x244, 0xc)。
// process：res+0x04 = check_error(0)；resetStar 依赖未建模数据，标注推断。
Dispatcher_ResetStar::Dispatcher_ResetStar() {}
Dispatcher_ResetStar::~Dispatcher_ResetStar() {}
int Dispatcher_ResetStar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x244)
        return 0;
    }
    return 0x5c0;
}
int Dispatcher_ResetStar::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Fix6：ORIG 0x81424de 空壳（mov $0x0,%eax; ret），不转调 process_packet。
    (void)user; (void)msg; (void)param;
    return 0;
}
int Dispatcher_ResetStar::read(PacketBuf& packet, MSG_BASE& msg)
{
    // Fix6：ORIG 空壳（mov $0x0,%eax; ret）；get_binary 在 read_packet 中。
    (void)packet; (void)msg;
    return 0;
}
void Dispatcher_ResetStar::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x81411fc
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x244);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0xc);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
bool Dispatcher_ResetStar::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return !isOpenAdvanceAltar();  // [objdump 0x0814108a]
}
int Dispatcher_ResetStar::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x0814105c] get_binary(&req, 0x14); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x14);
    return 0;
}
int Dispatcher_ResetStar::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                         CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x081410aa]
    char* sb = resBytes(resp);
    *(int*)(sb + 4) = static_cast<int>(check_error(user, req));
    if (*(int*)(sb + 4) == 0)
    {
        int rc = user->m_advanceAltar.resetStar(                     // ORIG T 0x081333b2
            user,
            static_cast<CMDPacketStruct::ResetStarType::T>(*(const int*)(reqBytes(req) + 0x10)));
        *(int*)(sb + 4) = rc;
        if (rc == 0)
        {
            AdvanceAltarData* data = user->m_advanceAltar.getCurrentAdvanceAltarData();  // ORIG T 0x081348c4
            if (data != 0)
            {
                NOTIPacketStruct::_NotiAdvanceAltarUpdateRidable noti;   // ORIG ctor 0x08141ff8
                PacketGuard guard;
                guard.put_header(0, 0x210);                          // [objdump 0x08141172] put_header(0,0x210)
                guard.put_binary((char*)&noti, 1);                   // put_binary(&noti,1)
                advancealtar::putRidableInfoToPacket(guard, *data);  // ORIG T 0x08141781
                guard.finalize(true);
                user->Send(guard);                                   // ORIG T 0x086485ba CUser::Send
            }
        }
    }
    return 0;
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
Dispatcher_StartGame::Dispatcher_StartGame() {}
Dispatcher_StartGame::~Dispatcher_StartGame() {}
int Dispatcher_StartGame::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x813f7de：直接以 user 为操作对象（asm 无 target 变量）。
    (void)msg;
    if (!isOpenAdvanceAltar())     // ORIG 0x0812cfea
    {
        return 1;
    }
    if (user->get_state() != 3)
    {
        return 3;
    }
    if (user->get_charac_level() <= 0x31)   // ORIG cmp $0x31; setle
    {
        return 4;
    }
    if (user->isCompetitionMercenary())
    {
        return 3;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 3;
    }
    if (user->CheckInTrade())
    {
        return 2;
    }
    if (user->IsBusyDisjointStore())
    {
        return 3;
    }
    if (user->CheckInParty())
    {
        return 3;
    }
    if (G_GameWorld()->IsPVPChannel())
    {
        return 3;
    }
    return 0;
}
int Dispatcher_StartGame::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x813f8ce
    struct _P4 { int m0; int m1; };   // +0x04 槽（ORIG mov 0x4(%eax) 寻址模拟）
    MSG_BASE& m = msg;                // ORIG -0x10
    ParamBase& p = param;             // ORIG -0xc
    reinterpret_cast<_P4*>(&p)->m1 = check_error(user, m);
    if (reinterpret_cast<_P4*>(&p)->m1 == 0)
    {
        user->m_advanceAltar.checkAndCreateDefaultRidable();  // ORIG this=user+0x8df60
    }
    return 0;
}
int Dispatcher_StartGame::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x813f7b0：读 0xd 字节（msg 引用先存 -0xc）
    MSG_BASE& m = msg;
    packet.get_binary((char*)&m, 0xd);
    return 0;
}
void Dispatcher_StartGame::send(CUser* user, ParamBase& param)
{
    // ORIG 0x813f92a
    struct _P4 { int m0; int m1; };   // +0x04 槽（ORIG mov 0x4(%eax) 寻址模拟）
    ParamBase& p = param;
    _P4& q = *reinterpret_cast<_P4*>(&p);
    if (q.m1 == 0)
    {
        StageControl::gotoStage(user);   // ORIG 0x0812fdd4 静态语义
    }
    PacketGuard guard;
    guard.put_header(1, 0x23c);
    guard.put_byte(1);
    guard.put_binary((char*)&p, 8);
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
        int i = 0;
        do
        {
            AdvanceAltarData* data = user->m_advanceAltar.getAdvanceAltarDataByIndex(i);
            if (data != 0)
            {
                putRidableInfoToPacket(guard, *data);
                if (data->m_field20 != 0)
                {
                    data->m_field20 = 0;
                    data->m_update = 1;      // ORIG +0xcc
                }
            }
            ++i;
        } while (i <= 0);
        guard.finalize(true);
        user->Send(guard);
    }
}

// ==================== Dispatcher_SummonUnit（ORIG 0x81404a8）====================
// dispatch_sig：校验失败返回 0x496；read(0x18) → process → send_packet(0x240, 0x14)。
// process：res+0x08=req+0x0d、res+0x0c=req+0x10；check_error 0；若 gageLevel 匹配
//   summonSlotUnit（真实偏移实现）；res+0x10 = getGage()。
Dispatcher_SummonUnit::Dispatcher_SummonUnit() {}
Dispatcher_SummonUnit::~Dispatcher_SummonUnit() {}
int Dispatcher_SummonUnit::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x240)
        return 0;
    }
    return 0x496;
}
int Dispatcher_SummonUnit::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Fix6：ORIG 0x8142356 空壳（mov $0x0,%eax; ret），不转调 process_packet。
    (void)user; (void)msg; (void)param;
    return 0;
}
int Dispatcher_SummonUnit::read(PacketBuf& packet, MSG_BASE& msg)
{
    // Fix6：ORIG 空壳（mov $0x0,%eax; ret）；get_binary 在 read_packet 中。
    (void)packet; (void)msg;
    return 0;
}
void Dispatcher_SummonUnit::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x81406b8
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x240);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0x14);
    guard.finalize(true);

    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
int Dispatcher_SummonUnit::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return 0;  // [objdump 0x081405ae] mov $0x0,%eax; ret
}
int Dispatcher_SummonUnit::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140580] get_binary(&req, 0x18); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x18);
    return 0;
}
int Dispatcher_SummonUnit::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                          CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x081405b8]
    const char* rb = reqBytes(req);
    char* sb = resBytes(resp);
    sb[8] = rb[0xd];                                       // slotType 回显
    *(int*)(sb + 0x0c) = *(const int*)(rb + 0x10);         // unitId 回显
    *(int*)(sb + 4) = check_error(user, req);
    if (*(int*)(sb + 4) == 0)
    {
        advancealtar::StageControl* stage = user->getAdvanceAltar();   // ORIG T 0x08655262 CUser::getAdvanceAltar
        if (stage != 0)
        {
            advancealtar::CharacData* cd = getCharacData(stage);               // StageControl::getCharacData W 0x081348a0
            if (*(const int*)(rb + 0x14) == characDataGetGageLevel(cd))     // gageLevel 一致性
            {
                *(int*)(sb + 4) = characDataSummonSlotUnit(
                    cd, static_cast<unsigned char>(rb[0xd]), *(const int*)(rb + 0x10));
                *(int*)(sb + 0x10) = characDataGetGage(cd);
            }
            else                                           // [objdump 0x8140650] err=3
            {
                *(int*)(sb + 4) = 3;
            }
        }
    }
    return 0;
}

// ==================== Dispatcher_UpgradeGage（ORIG 0x814020e）====================
// dispatch_sig：校验失败返回 0x446；read(0x18) → process → send_packet(0x23f, 0x14)。
// process：res+0x08=升级前 gageLevel、res+0x04=upgreadGage()、res+0x0c=升级后、
//          res+0x10=getGage()（真实偏移实现）。
Dispatcher_UpgradeGage::Dispatcher_UpgradeGage() {}
Dispatcher_UpgradeGage::~Dispatcher_UpgradeGage() {}
int Dispatcher_UpgradeGage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x23f)
        return 0;
    }
    return 0x446;
}
int Dispatcher_UpgradeGage::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Fix6：ORIG 0x81422f4 空壳（mov $0x0,%eax; ret），不转调 process_packet。
    (void)user; (void)msg; (void)param;
    return 0;
}
int Dispatcher_UpgradeGage::read(PacketBuf& packet, MSG_BASE& msg)
{
    // Fix6：ORIG 空壳（mov $0x0,%eax; ret）；get_binary 在 read_packet 中。
    (void)packet; (void)msg;
    return 0;
}
void Dispatcher_UpgradeGage::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x81403e2
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x23f);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0x14);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
int Dispatcher_UpgradeGage::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return 0;  // [objdump 0x08140314] mov $0x0,%eax; ret
}
int Dispatcher_UpgradeGage::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x081402e6] get_binary(&req, 0x18); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x18);
    return 0;
}
int Dispatcher_UpgradeGage::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                           CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x0814031e]
    char* sb = resBytes(resp);
    *(int*)(sb + 4) = check_error(user, req);
    if (*(int*)(sb + 4) == 0)
    {
        advancealtar::StageControl* stage = user->getAdvanceAltar();   // ORIG T 0x08655262
        if (stage != 0)
        {
            advancealtar::CharacData* cd = getCharacData(stage);               // StageControl::getCharacData W 0x081348a0
            *(int*)(sb + 8) = characDataGetGageLevel(cd);      // 升级前 gageLevel
            *(int*)(sb + 4) = characDataUpgreadGage(cd);       // ORIG CharacData::upgreadGage T 0x0812d176
            *(int*)(sb + 0x0c) = characDataGetGageLevel(cd);   // 升级后 gageLevel
            *(int*)(sb + 0x10) = characDataGetGage(cd);
        }
    }
    return 0;
}

// ==================== Dispatcher_ExchangeSlot（ORIG 0x8140774）====================
// dispatch_sig：校验失败返回 0x4e8；read(0x30) → process → send_packet(0x241, 0x24)。
// process：res+0x08=req+0x10；check_error 0；exchangeSlot 依赖未建模数据，推断。
Dispatcher_ExchangeSlot::Dispatcher_ExchangeSlot() {}
Dispatcher_ExchangeSlot::~Dispatcher_ExchangeSlot() {}
int Dispatcher_ExchangeSlot::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x241)
        return 0;
    }
    return 0x4e8;
}
int Dispatcher_ExchangeSlot::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Fix6：ORIG 0x81423b8 空壳（mov $0x0,%eax; ret），不转调 process_packet。
    (void)user; (void)msg; (void)param;
    return 0;
}
int Dispatcher_ExchangeSlot::read(PacketBuf& packet, MSG_BASE& msg)
{
    // Fix6：ORIG 空壳（mov $0x0,%eax; ret）；get_binary 在 read_packet 中。
    (void)packet; (void)msg;
    return 0;
}
void Dispatcher_ExchangeSlot::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140966
    CMDPacketStruct::STBaseResponse& res = resp;
    PacketGuard guard;
    guard.put_header(1, 0x241);
    guard.put_byte(1);
    guard.put_binary((char*)&res, 0x24);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
bool Dispatcher_ExchangeSlot::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return !isOpenAdvanceAltar();  // [objdump 0x08140884]
}
int Dispatcher_ExchangeSlot::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140856] get_binary(&req, 0x30); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x30);
    return 0;
}
int Dispatcher_ExchangeSlot::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                            CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x081408a4]
    const char* rb = reqBytes(req);
    char* sb = resBytes(resp);
    *(int*)(sb + 8) = *(const int*)(rb + 0x10);            // field08 回显
    *(int*)(sb + 4) = static_cast<int>(check_error(user, req));
    if (*(int*)(sb + 4) == 0)
    {
        int rc = user->m_advanceAltar.exchangeSlot(        // ORIG T 0x08131f0a
            *reinterpret_cast<CMDPacketStruct::_SetSlotData*>(const_cast<char*>(rb) + 0x18),
            *reinterpret_cast<CMDPacketStruct::_SetSlotData*>(const_cast<char*>(rb) + 0x24));
        *(int*)(sb + 4) = rc;
        std::memcpy(sb + 0x0c, rb + 0x18, 0xc);            // slotA 回显
        std::memcpy(sb + 0x18, rb + 0x24, 0xc);            // slotB 回显
    }
    return 0;
}

// ==================== Dispatcher_GetAchievementReward（ORIG 0x8140c74）====================
// dispatch_sig：校验失败返回 0x571；read(0x14) → process（ORIG 为空）→
//   send_packet(0x243)。send_packet：put_byte(1)+put_binary(res,0xc)+
//   put_int(count)+逐项 _RewardItem（sendAchievementReward 未建模，推断）。
Dispatcher_GetAchievementReward::Dispatcher_GetAchievementReward() {}
Dispatcher_GetAchievementReward::~Dispatcher_GetAchievementReward() {}
int Dispatcher_GetAchievementReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet（空体）
        send_packet(user, res);            // ORIG send_packet(0x243)
        return 0;
    }
    return 0x571;
}
int Dispatcher_GetAchievementReward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Fix6：ORIG 0x814247c 空壳（mov $0x0,%eax; ret），不转调 process_packet。
    (void)user; (void)msg; (void)param;
    return 0;
}
int Dispatcher_GetAchievementReward::read(PacketBuf& packet, MSG_BASE& msg)
{
    // Fix6：ORIG 空壳（mov $0x0,%eax; ret）；get_binary 在 read_packet 中。
    (void)packet; (void)msg;
    return 0;
}
void Dispatcher_GetAchievementReward::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140daa：sendAchievementReward(user, res+0x08, &items) 回写 res+0x04；
    // error==0 且 items 非空时追加 items 逐项 _RewardItem(0xc)，否则 put_int(count)。
    altar_pkt::ResGetAchievementReward& res =
        *reinterpret_cast<altar_pkt::ResGetAchievementReward*>(&resp);
    PacketGuard guard;
    guard.put_header(1, 0x243);
    guard.put_byte(1);
    int count = 0;                                  // ORIG -0xc(%ebp) 初始 0
    const std::vector<_RewardItem>* items = 0;      // ORIG -0x28(%ebp)
    res.m_nErrorCode =
        user->m_advanceAltar.sendAchievementReward(user, res.m_nAchievementIdx, &items);
    guard.put_binary((char*)&res, 0xc);
    if (res.m_nErrorCode == 0 && items != 0)
    {
        count = static_cast<int>(items->size());
        guard.put_int(count);
        for (std::vector<_RewardItem>::const_iterator it = items->begin();
             it != items->end(); ++it)
        {
            guard.put_binary((char*)&*it, 0xc);
        }
    }
    else
    {
        guard.put_int(count);
    }
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
bool Dispatcher_GetAchievementReward::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return !isOpenAdvanceAltar();  // [objdump 0x08140d80]
}
int Dispatcher_GetAchievementReward::read_packet(PacketBuf& packet,
                                                 CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140d52] get_binary(&req, 0x14); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x14);
    return 0;
}
int Dispatcher_GetAchievementReward::process_packet(CUser* user,
                                                    CMDPacketStruct::STBaseRequest& req,
                                                    CMDPacketStruct::STBaseResponse& resp) const
{
    (void)user; (void)req; (void)resp;
    // [objdump 0x08140da0] mov $0x0,%eax; ret（ORIG 空体：不读请求、不写响应）
    return 0;
}

}
Dispatcher_Redeem::Dispatcher_Redeem() {}
Dispatcher_Redeem::~Dispatcher_Redeem() {}
// ORIG 0x08226250（报告 Dispatcher_Redeem/dispatch_sig.md）：
// state>2 受理；get_byte 读 1 字节（有符号）作 redeemId 调 Redeem_Item。
int Dispatcher_Redeem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() > 2)
    {
        char redeemId = 0;
        packet.get_byte(redeemId);
        user->Redeem_Item((int)redeemId);
    }
    return 0;
}
int Dispatcher_Redeem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Redeem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseDye::Dispatcher_UseDye() {}
Dispatcher_UseDye::~Dispatcher_UseDye() {}
int Dispatcher_UseDye::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81e727e：仅允许 state==3；交易中返回 1。
    if (user == 0 || user->get_state() != 3)
    {
        return 0x13;
    }
    return user->CheckInTrade() ? 1 : 0;
}
int Dispatcher_UseDye::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_USE_DYE& req = static_cast<MSG_USE_DYE&>(msg);
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_0e)))
    {
        return LineFunc(0x6245,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_10)))
    {
        return LineFunc(0x6248,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

Dispatcher_Antibot::Dispatcher_Antibot() {}
Dispatcher_Antibot::~Dispatcher_Antibot() {}
int Dispatcher_Antibot::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned int local_54 = 0;           // -0x50：包内 4 字节序列号（网络序）
    char cVar3;
    unsigned int local_58 = 0;           // -0x54：宿主序原值
    char* local_30;                      // -0x2c：get_buf_ptr 结果（ORIG char* 视图）
    void* local_28;                      // -0x24：+2 指针
    unsigned int local_24;               // -0x20：ntohl 结果
    int local_20;                        // -0x1c：getCharacAntibotSerialNum
    int local_2c;                        // -0x28：antibotChecker 虚调用结果

    cVar3 = packet.get_int(local_54);
    if ((char)(cVar3 ^ 1) == 0)          // xor $0x1: 只有 cVar3==1 才继续
    {
        local_30 = packet.get_buf_ptr(packet.get_index());
        if (user->m_field8cf00 == 0)
        {
            user->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)0);
            user->m_field8cf00 = 1;
        }
        else if (user->m_field8cf00 != 4)
        {
            user->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b, local_54,
                user->m_field8cf00);
        }
        if (local_30 != 0)
        {
            if (user == 0)
            {
                cMyTrace local_50(__PRETTY_FUNCTION__, 0xa7df, 0);
                local_50("Antibot Packet log will write, but user ptr is null");
                return 0;
            }
            local_28 = (void*)(local_30 + 2);
            local_58 = 0;
            if (local_28 != (void*)0)
            {
                memcpy(&local_58, local_28, 4);
            }
            local_24 = ntohl(local_58);
            local_20 = user->getCharacAntibotSerialNum();
            if (local_20 != 0 && local_20 + 1U != local_24)
            {
                unsigned int uVar4 = (unsigned int)user->getCurCharacNo();
                unsigned int uVar6 = user->get_acc_id();
                unsigned int uVar2 = local_58;
                cMyTrace local_40(__PRETTY_FUNCTION__, 0xa7e9, 0);
                local_40(
                    "Antibot Packet Serial Number host_byte_order:%u, network_byte_order:%u, m_id:%u, charac_no:%d, pre_serial_number:%u",
                    local_24, uVar2, uVar6, uVar4, local_20);
            }
            user->setCharacAntibotSerialNum(local_24);
        }
        // pcVar1 = *(code**)(*(int*)GlobalData::s_antibotChecker_ + 0xc)
        void* checkerObj = *(void**)&GlobalData::s_antibotChecker_;
        typedef int (*CheckerFn)(void*, unsigned int, unsigned int, unsigned int, void*, unsigned int);
        CheckerFn pcVar1 = *(CheckerFn*)(*(int*)checkerObj + 0xc);
        unsigned int uVar7 = local_54 & 0xffff;
        unsigned int uVar4 = user->get_acc_id();
        CEnvironment* this_00 = G_CEnvironment();
        unsigned int uVar6 = (unsigned int)this_00->getProcessSequence();
        local_2c = (*pcVar1)(checkerObj, uVar6, 0, uVar4, local_30, uVar7);
        if (user->m_field8cf00 == 1)
        {
            user->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)1);
            user->m_field8cf00 = 2;
        }
        else if (user->m_field8cf00 != 4)
        {
            user->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b, local_54,
                user->m_field8cf00);
        }
        if (local_2c == -1)
        {
            return 0;
        }
        return 0;
    }
    LineFunc(0xa7c1, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
int Dispatcher_Antibot::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Antibot::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ChangeHp::Dispatcher_ChangeHp() {}
Dispatcher_ChangeHp::~Dispatcher_ChangeHp() {}
// ORIG 0x0820748a（报告 Dispatcher_ChangeHp/dispatch_sig.md）：
// state<=1 → LineFunc；get_int hp；CAssaultMgr::OnChangeHp(user,hp)!=1 → LineFunc。
int Dispatcher_ChangeHp::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa206,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int hp = 0;
    if (!packet.get_int(hp))
    {
        return LineFunc(0xa20a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (pvp_assault::GetInstanceAssaultMgr()->OnChangeHp(user, hp) != 1)
    {
        return LineFunc(0xa20d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_ChangeHp::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeHp::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Teleport::Dispatcher_Teleport() {}
Dispatcher_Teleport::~Dispatcher_Teleport() {}

int Dispatcher_Teleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_TELEPORT& req = static_cast<MSG_TELEPORT&>(msg);
    if (!packet.get_short(req.m_field0e))
    {
        return LineFunc(0x2b34,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_field10))
    {
        return LineFunc(0x2b38,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(&req.m_field14)))
    {
        return LineFunc(0x2b3c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(&req.m_field15)))
    {
        return LineFunc(0x2b3f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}


Dispatcher_UseJewel::Dispatcher_UseJewel() {}
Dispatcher_UseJewel::~Dispatcher_UseJewel() {}
int Dispatcher_UseJewel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    return LineFunc(0xc9e2,
                    __PRETTY_FUNCTION__,
                    0, 0);
}
int Dispatcher_UseJewel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseJewel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BingoQuiz::Dispatcher_BingoQuiz() {}
Dispatcher_BingoQuiz::~Dispatcher_BingoQuiz() {}
int Dispatcher_BingoQuiz::check_error(CUser* user, MSG_BASE& msg)
{
    char isEventing;
    BingoData* bingo;
    int ret;

    (void)msg;
    if (user == 0)
        return 1;          // ORIG 0x0825eaca：user/charac 为空返回 1（非 -1）
    if (user->getCurCharacR() == 0)
        return 1;
    if (user->get_state() != 3)
        return 0x1e;
    {
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x6a);
        // ORIG vtable+0x34 槽直调（CEventBase::IsEventing(0)，与 CBattle_Field_KillChain.cpp
        // bfkcIsEventing 同口径；CGuildServerProxy.h 槽位注释与 ORIG 槽序有出入，用直调保 ABI）。
        typedef char (*IsEventingFn)(void*, CUser*);
        IsEventingFn fn = reinterpret_cast<IsEventingFn>(
            (*reinterpret_cast<void***>(ev))[0x34 / 4]);
        isEventing = fn(ev, reinterpret_cast<CUser*>(0));
        if (isEventing)
            return 0x13;
    }
    // ORIG 0x0825eaca 每次访问均重新调用 getBingoData；byte5/byte4 按有符号 char 比较。
    if (*(reinterpret_cast<char*>(user->getBingoData()) + 5))
        return 0x12;
    if ((int)*(reinterpret_cast<char*>(user->getBingoData()) + 4) > 2)
        return 0x16;
    ret = 0;
    return ret;
}
// ORIG 0x0825e9b8 T _ZN20Dispatcher_BingoQuiz7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_BingoQuiz/process.md 汇编+反编译 C 全文转写）
// param 为模板实参（+0x04 错误码 / +0x08 bingo 数据槽）。
int Dispatcher_BingoQuiz::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    *(int*)((char*)&param + 4) = check_error(user, msg);
    int err = *(int*)((char*)&param + 4);
    if (err >= 1)
    {
        return 0;
    }
    if (err < 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp",
                              __PRETTY_FUNCTION__,
                              0x1f0, "Dispatcher Check_Error No(%d)", err);
        return LineFunc(0x1f1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG GetRepeatEvent(0x6a) → BingoEvent
    BingoEvent* event = reinterpret_cast<BingoEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x6a));
    if (event == 0)
    {
        *(int*)((char*)&param + 4) = 0x13;
    }
    else
    {
        *(int*)((char*)&param + 4) =
            event->reqBingoQuizMark(*user, *(int*)((char*)&msg + 0xd));
    }
    *(int*)((char*)&param + 8) =
        *reinterpret_cast<int*>(user->getBingoData());
    return 0;
}
int Dispatcher_BingoQuiz::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 int(msg+0xd)，失败记 LineFunc(0x1de)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(p + 0xd)))
    {
        return LineFunc(0x1de, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_BingoQuiz::send(CUser* user, ParamBase& param)
{
    stBingoQuiz& out = static_cast<stBingoQuiz&>(param);
    // ORIG 各分支独立 PacketGuard（-0x18 / -0x24 分别构造）。
    if (out.m_result == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x203);
        packet.put_byte(1);
        packet.put_int((int)out.m_bingoData);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x203);
        packet.put_byte(0);
        packet.put_byte(out.m_result);
        packet.put_int((int)out.m_bingoData);
        packet.finalize(true);
        user->Send(packet);
    }
}


Dispatcher_DieMobAll::Dispatcher_DieMobAll() {}
Dispatcher_DieMobAll::~Dispatcher_DieMobAll() {}
int Dispatcher_DieMobAll::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x81ecce6 (89 insn)：战斗中全灭上报；state==5 时检查
    // 副本未清且(非0x2afe图或无副本)记 hack。GetParty(GetPartyIndex()) 为查找，
    // 勿用无参 GetParty()（会新建 party）。
    if (user->get_state() == 5)
    {
        CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party == 0)
        {
            return LineFunc(0x6ef6,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        CBattle_Field& field =
            *reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1);
        if (field.IsClearDungeon())
        {
            return 0;
        }
        if (party->m_dungeon == 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, static_cast<WongWork::ENUM_HACKTYPE>(0xcc), 1, 0, 0);
            return 0;
        }
        if (static_cast<CParty*>(user->GetParty())->m_dungeon->get_index()
            == 0x2afe)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, static_cast<WongWork::ENUM_HACKTYPE>(0xcc), 1, 2, 0);
            return 0;
        }
        user->getHackAnalyzer()->addServerHackCnt(
            user, static_cast<WongWork::ENUM_HACKTYPE>(0xcc), 1, 0, 0);
    }
    return 0;
}
int Dispatcher_DieMobAll::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DieMobAll::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SkillInit::Dispatcher_SkillInit() {}
Dispatcher_SkillInit::~Dispatcher_SkillInit() {}
int Dispatcher_SkillInit::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081e5a2c
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 3)
    {
        return 7;
    }
    MSG_SKILL_INIT& req = static_cast<MSG_SKILL_INIT&>(msg);
    int idx = user->GetCurCharacSkillTreeIndex();
    if (idx == -1 || idx == 0)
    {
        if (req.m_0d == 1)
        {
            return 0x13;
        }
        req.m_0d = 0;
    }
    else
    {
        if (req.m_0d != 1)
        {
            return 0x13;
        }
        req.m_0d = 1;
    }
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        if (user->get_pvp_grade() < 0)
        {
            return 0xc;
        }
    }
    return 0;
}
int Dispatcher_SkillInit::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e598a
    int err = check_error(user, msg);
    stSkillInit& p = static_cast<stSkillInit&>(param);
    p.m_4 = err;
    if (p.m_4 >= 1)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x5f3d,
                        __PRETTY_FUNCTION__,
                        p.m_4, 0);
    }
    MSG_SKILL_INIT& req = static_cast<MSG_SKILL_INIT&>(msg);
    p.m_8 = req.m_0d;
    process_skill_init(user, param);
    return 0;
}
int Dispatcher_SkillInit::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e5934：读一个字节到 msg 的 m_0d
    MSG_SKILL_INIT& req = static_cast<MSG_SKILL_INIT&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x5f26, __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void Dispatcher_SkillInit::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e5aec
    stSkillInit& p = static_cast<stSkillInit&>(param);
    if (p.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1ec,
                                 (unsigned char)(p.m_4 & 0xff));
        return;
    }
    PacketGuard pkt;
    pkt.put_header(1, 0x1ec);
    pkt.put_byte(1);
    pkt.put_byte((int)(char)p.m_8);
    pkt.finalize(true);
    user->Send(pkt);
    user->send_skill_info();
}
void Dispatcher_SkillInit::process_skill_init(CUser* user, ParamBase& param)
{
    // ORIG 0x081e5bdc
    if (user == 0)
    {
        return;
    }
    stSkillInit& p = static_cast<stSkillInit&>(param);
    int kind = (int)(char)p.m_8;
    SkillSlot* slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->clear_all_skills((ENUM_SKILL_TREE_KIND)kind);
    slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->set_parent(user);
    slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->InitSkill(false, (ENUM_SKILL_TREE_KIND)kind);
    slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->resetComboSkill((ENUM_SKILL_TREE_KIND)kind);
    user->setComboSkillSet(true);
}

Dispatcher_Antibot_DP::Dispatcher_Antibot_DP() {}
Dispatcher_Antibot_DP::~Dispatcher_Antibot_DP() {}
int Dispatcher_Antibot_DP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820b322：读嵌套包长度→checker vtbl+0x2c 解密→doDispatch 内层包。
    unsigned int binarySize = 0;
    if (!packet.get_int(binarySize))
    {
        return LineFunc(0xa795, __PRETTY_FUNCTION__, 0, 0);
    }
    char* nestedPacket = packet.get_buf_ptr(packet.get_index());
    int local_28 = 0xd;  // ORIG 局部（写后未用，保持帧形态）
    (void)local_28;
    if (binarySize > 0x567)
    {
        return LineFunc(0xa7a3, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned char outBuf[4];          // +0x00 class / +0x01..+0x03 short
    unsigned short outLen = 0xaf0;
    typedef int (*DecryptFn)(void*, int, void*, unsigned short, unsigned char*, unsigned short*);
    void* checker = GlobalData::s_antibotChecker_;
    DecryptFn fn = *reinterpret_cast<DecryptFn*>(*reinterpret_cast<void***>(checker) + (0x2c / 4));
    int ret = fn(checker, user->getAntibotKey(), nestedPacket,
                 (unsigned short)(binarySize & 0xffff), outBuf, &outLen);
    if (ret != 0)
    {
        int key = user->getAntibotKey();
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa7af, 0);
        trace("[Dispatcher_Antibot_DP] Key : %d, nestedPacket : %x, binarySize : %d, return : %d",
              key, nestedPacket, binarySize, ret);
        return 0;
    }
    PacketDispatcher* dispatcher = G_PacketDispatcher();
    dispatcher->doDispatch(user,
                           (ENUM_PACKETCLASS)outBuf[0],
                           (unsigned short)*reinterpret_cast<unsigned short*>(outBuf + 1),
                           reinterpret_cast<char*>(outBuf),
                           static_cast<int>(outLen),
                           (ENUM_PACKETCLASS)1,
                           (unsigned short)0x25c);
    return 0;
}
int Dispatcher_Antibot_DP::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Antibot_DP::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AskRematch::Dispatcher_AskRematch() {}
Dispatcher_AskRematch::~Dispatcher_AskRematch() {}
int Dispatcher_AskRematch::dispatch_sig(CUser* user, PacketBuf& packet)
{
    bool gate;
    char rematchFlag;
    PvP_Room* room;
    int matched;

    if (user->get_state() > 2)
    {
        if (user->getCurCharacR() != 0)
            gate = false;
        else
            gate = true;
    }
    else
    {
        gate = true;
    }
    if (gate)
        return 0;
    rematchFlag = 0;
    if (!packet.get_byte(rematchFlag))
    {
        LineFunc(0xe3af,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    room = (PvP_Room*)user->GetPVPRoom();
    if (room == 0)
        return 0;
    if (room->m_matching == 0)
        return 0;
    matched = room->m_matching;
    // ORIG vtable+0x68 槽直调（IMatch::OnAskRematch(CUser*, int)，MatchRoomList.h 已声明；
    // m_matching 为 int 存储口径，直调形态与 DisPatcherCore.cpp EndPvPResult 同款）。
    (*(int (**)(void*, void*, int))(*reinterpret_cast<char**>(matched) + 0x68))(
        reinterpret_cast<void*>(matched), user, (int)rematchFlag);
    return 0;
}
int Dispatcher_AskRematch::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AskRematch::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BVHackInfo::Dispatcher_BVHackInfo() {}
Dispatcher_BVHackInfo::~Dispatcher_BVHackInfo() {}
int Dispatcher_BVHackInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08207568..0x08207851 (0x2e9 字节)。
    // 阈值常量 _DAT_08bd6360 = 50.0（已 objdump 核实）。
    static const char* const kPretty =
        "virtual int Dispatcher_BVHackInfo::dispatch_sig(CUser*, PacketBuf&)";

    if (user->get_state() <= 1)   // cmp $0x1 / setle
        return LineFunc(0xa216, kPretty, 0, 0);

    int code = 0;
    if (packet.get_int(code) != 1 || code != 0x40)
        return LineFunc(0xa21a, kPretty, 0, 0);

    char payload[0x40];   // -0x80
    if (packet.get_binary(payload, 0x40) != 1)
        return LineFunc(0xa21c, kPretty, 0, 0);

    stBVHackInfo_t info;   // -0x40（ORIG 至少 0x30 字节，6 个 double）
    if (decryptBVHackPacket(payload, info) != 1)
        return LineFunc(0xa224, kPretty, 0, 0);

    const double v0 = info.m_values[0];
    const double v1 = info.m_values[1];
    const double v2 = info.m_values[2];
    const double v3 = info.m_values[3];
    const double v4 = info.m_values[4];
    const double v5 = info.m_values[5];

    // 任一前五个值非零 → 记录日志（六个值按 +0x28..+0x00 顺序入 varargs）
    if (v0 != 0.0 || v1 != 0.0 || v2 != 0.0 || v3 != 0.0 || v4 != 0.0)
    {
        char* idStr = NumberToString(user->get_acc_id(), 0);
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp", kPretty, 0xa228,
            "(m_id: %s)(%5f)(%5f)(%5f)(%5f)(%5f)(%5f)",
            idStr, v5, v4, v3, v2, v1, v0);
    }

    if (v1 >= 50.0 && (v2 != 0.0 || v4 != 0.0))
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x328, 1, 0, 0);
        if (user->get_state() != 5)
            user->SetTradePunishType(0x10);
    }
    return 0;
}
int Dispatcher_BVHackInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BVHackInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BlueMarble::Dispatcher_BlueMarble() {}
Dispatcher_BlueMarble::~Dispatcher_BlueMarble() {}
int Dispatcher_BlueMarble::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    const MSG_BLUE_MARBLE& req = static_cast<const MSG_BLUE_MARBLE&>(msg);
    if (req.m_0d == 6)
    {
        return 0;
    }
    if (user->getMoveSpace() != (UserSpace::T)1)
    {
        return -2;
    }
    BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
    if (marble == 0)
    {
        return -3;
    }
    if (marble->getUserSlot(user) == -1)
    {
        return -4;
    }
    int state = user->get_state();
    return (state == 0xb || state == 5) ? 0 : -5;
}
int Dispatcher_BlueMarble::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e5fdc
    int err = check_error(user, msg);
    stBlueMarble& p = static_cast<stBlueMarble&>(param);
    p.m_4 = err;
    if (p.m_4 >= 1)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x60a9,
                        __PRETTY_FUNCTION__,
                        p.m_4, 0);
    }
    const MSG_BLUE_MARBLE& req = static_cast<const MSG_BLUE_MARBLE&>(msg);
    BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
    if (marble == 0)
    {
        return 0;
    }
    switch (req.m_0d)
    {
    case 0: marble->startBlueMarble(); break;
    case 1: marble->userThrowDice(user); break;
    case 2: marble->enterDungeon(user); break;
    case 3: marble->giveupDungeon(user); break;
    case 4: marble->clearDungeon(user); break;
    case 5: marble->exitDungeon(user); break;
    case 6: marble->leaveUser(user); break;
    case 7: marble->giveBonusItem(user); break;
    default: break;
    }
    return 0;
}
int Dispatcher_BlueMarble::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BLUE_MARBLE& req = static_cast<MSG_BLUE_MARBLE&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x609a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void Dispatcher_BlueMarble::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e61f4
    stBlueMarble& p = static_cast<stBlueMarble&>(param);
    if (p.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1f0);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1f0,
                                 (unsigned char)(p.m_4 & 0xff));
    }
}

Dispatcher_ChangeHost::Dispatcher_ChangeHost() {}
Dispatcher_ChangeHost::~Dispatcher_ChangeHost() {}
int Dispatcher_ChangeHost::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8204c1e：state>=3 才处理；teleport_state 在 [0,3) 反走城错误 0x9e
    if (user->get_state() <= 2)
    {
        return LineFunc(0x9cbf,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char slot;
    if (!packet.get_byte(slot))
    {
        return LineFunc(0x9cc2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x7c, 0x13);
        return 0;
    }
    if (static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->get_teleport_state()
            >= 0
        && static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->get_teleport_state()
            <= 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x7c, 0x9e);
        return 0;
    }
    int ret = party->change_manager(user, (int)slot);
    if (ret > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x7c, (unsigned char)ret);
    }
    return 0;
}
int Dispatcher_ChangeHost::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeHost::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// read() 直接依赖（tsv 清单外）：G_CDataManager()+0x14 指向的基址 +0x48fc
// 为 vector<pair<int,int>>（ComboSkill.cpp 同源布局视图：6 组组合技表首列），
// 线性查 first==idx 命中返回 1，否则 0。
int Dispatcher_ComboSkill::checkComboSkillIndex(short idx)
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
        if ((*comboList)[i].first == (int)idx)
        {
            return 1;
        }
    }
    return 0;
}

Dispatcher_ComboSkill::Dispatcher_ComboSkill() {}
Dispatcher_ComboSkill::~Dispatcher_ComboSkill() {}
int Dispatcher_ComboSkill::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->get_state() < 3 || user->get_charac_job() != 9)
    {
        return -1;
    }
    const MSG_COMBO_SKILL& req = static_cast<const MSG_COMBO_SKILL&>(msg);
    return req.m_pad0d == 0 || req.m_pad0d == 1 ? 0 : 0x13;
}
// err>0 → 0；err<0 → LineFunc(0x33f)；否则当前角色 SkillSlot 写入组合技映射
// 并校验该树。
int Dispatcher_ComboSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stComboSkill& out = static_cast<stComboSkill&>(param);
    MSG_COMBO_SKILL& req = static_cast<MSG_COMBO_SKILL&>(msg);
    out.m_result = check_error(user, msg);
    if (out.m_result > 0)
    {
        return 0;
    }
    if (out.m_result < 0)
    {
        return LineFunc(0x33f,
                        __PRETTY_FUNCTION__,
                        out.m_result, 0);
    }
    SkillSlot* skillW =
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
    skillW->setComboSkillInfo((ENUM_SKILL_TREE_KIND)(char)req.m_pad0d,
                              req.m_comboMap);
    skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
    skillW->verifyComboSkillTree((ENUM_SKILL_TREE_KIND)(char)req.m_pad0d);
    return 0;
}
// 读 MSG_COMBO_SKILL：m_pad0d 树种类、树数量(≤6)、每树 {kind, 技能数(≤6),
// skillIdx...}；checkComboSkillIndex 校验通过才收技能列表；逐项插入
// m_comboMap(kind → vector<int>)。任何 get 失败按行号走 LineFunc；
// 外层 get_short 失败/循环耗尽均归 LineFunc(0x30c)。
int Dispatcher_ComboSkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_COMBO_SKILL& req = static_cast<MSG_COMBO_SKILL&>(msg);
    std::map<short, std::vector<int>, std::less<short>,
             std::allocator<std::pair<short const, std::vector<int> > > >&
        comboMap = req.m_comboMap;
    std::vector<int> skills;
    comboMap.clear();
    if (!packet.get_byte(req.m_pad0d))
    {
        return LineFunc(0x301,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char treeCount = 0;
    if (!packet.get_byte(treeCount))
    {
        return LineFunc(0x304,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < (int)treeCount && i < 6; ++i)
    {
        short kind = 0;
        char skillCount = 0;
        if (!packet.get_short(kind))
        {
            break;  // 归 LineFunc(0x30c)
        }
        if (checkComboSkillIndex(kind))
        {
            if (!packet.get_byte(skillCount))
            {
                return LineFunc(0x312,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            skills.clear();
            for (int j = 0; j < (int)skillCount && j < 6; ++j)
            {
                short skillIdx = 0;
                if (!packet.get_short(skillIdx))
                {
                    return LineFunc(0x319,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                skills.push_back((int)skillIdx);
            }
            comboMap.insert(std::make_pair(kind, skills));
        }
    }
    return LineFunc(0x30c,
                    __PRETTY_FUNCTION__,
                    0, 0);
}
void Dispatcher_ComboSkill::send(CUser* user, ParamBase& param)
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

Dispatcher_GM_Command::Dispatcher_GM_Command() {}
Dispatcher_GM_Command::~Dispatcher_GM_Command() {}
// ---- GM 命令监控包/信号局部视图（仅本 TU 使用；ORIG W
//      Packet_Monitor_Notice_Message 0x822bbd6 / Packet_Forbid_Chat_By_Monitor
//      0x822bc70，均为清零构造的定长缓冲）----
namespace gm_pkt
{
#pragma pack(push, 1)
// 监控服通知：+0x0a 长度字节、+0x0b 文本（总长 0x10b）
struct MonitorNoticeMessage
{
    MonitorNoticeMessage() { memset(this, 0, sizeof(*this)); }
    unsigned char m_pad[0x0a];
    unsigned char m_len;
    char m_msg[0x100];
};
// 禁言监控包：+0x0a 秒数(int)、+0x12 名字（总长 0x30）
struct ForbidChatByMonitor
{
    ForbidChatByMonitor() { memset(this, 0, sizeof(*this)); }
    unsigned char m_pad[0x0a];
    int m_seconds;
    unsigned char m_pad2[4];
    char m_name[0x1e];
};
// SIG_FORBID_USER_TO_PLAY 前 9 字节视图（ORIG memset 9B 后按 +0/+4/+5 写）
struct SigForbidPlayBody
{
    int m_characNo;
    unsigned char m_flag;
    int m_days;
};
#pragma pack(pop)
}  // namespace gm_pkt

// ORIG 0x81fa19e：GM 命令分发（cmd=0..0xd）。校验链 user!=0/getCurCharacR!=0/
// get_byte 成功；失败走 LineFunc 断言路径（返回 0）。响应包 header 0x6e：
// local_1d0 = put_byte(1)+put_byte(cmd)+payload；local_1c4 = 固定拒绝包
// (put_byte(99),put_byte(1))，目标用户不存在时发给 GM 本人。
int Dispatcher_GM_Command::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        // ORIG 0x81fa1e4：LineFunc(0x88fd, pretty, 0, 0) 断言后返回其值。
        return LineFunc(0x88fd, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        // ORIG 0x81fa224：LineFunc(0x8902, pretty, 0, 0) 断言后返回其值。
        return LineFunc(0x8902, __PRETTY_FUNCTION__, 0, 0);
    }

    {
        const char* accIdStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x8908, 0);
        trace("GM Command : %s", accIdStr);
    }

    unsigned char cmd = 0;
    if (!packet.get_byte(cmd))
    {
        // ORIG 0x81fa2cb：LineFunc(0x890e, pretty, 0, 0) 断言后返回其值。
        return LineFunc(0x890e, __PRETTY_FUNCTION__, 0, 0);
    }

    PacketGuard reject;   // ORIG local_1c4：目标缺失时的固定应答
    reject.put_header(1, 0x6e);
    reject.put_byte(1);
    reject.put_byte(99);
    reject.put_byte(1);
    reject.finalize(true);

    PacketGuard resp;     // ORIG local_1d0
    resp.put_header(1, 0x6e);
    resp.put_byte(1);
    resp.put_byte(cmd);

    unsigned int gmAccId = user->get_acc_id();  // ORIG 仅用于 cMyTrace 日志上下文
    (void)gmAccId;

    char name[30];
    int len = 0;
    short wLen = 0;
    int extra = 0;
    CUser* target = 0;
    GameWorld* world = G_GameWorld();
    switch (cmd)
    {
    case 0:  // 切换 GM 模式（无参数）
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0)
        {
            user->SetGameMasterMode(
                GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1);
            GlobalData::s_GM_Manager->TurnGmMode(user->get_acc_id());
        }
        break;
    case 1:  // 全服公告 GMC <str>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0 &&
            packet.get_int(len) && packet.get_str(name, 0x100, len) && len != 0)
        {
            resp.put_int(len);
            resp.put_str(name, len);
            resp.finalize(true);
            world->send_all(resp);
        }
        break;
    case 2:  // 本区域公告 GMZ <str>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0 &&
            packet.get_int(len) && packet.get_str(name, 0x100, len) && len != 0)
        {
            resp.put_int(len);
            resp.put_str(name, len);
            resp.finalize(true);
            world->send_to_area(user->getCurCharacVill(), user->get_area(false), resp);
        }
        break;
    case 3:  // 监控服公告 GMS <str>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0 &&
            packet.get_int(len) && packet.get_str(name, 0x100, len) && len != 0)
        {
            resp.put_int(len);
            resp.put_str(name, len);
            resp.finalize(true);
            gm_pkt::MonitorNoticeMessage noti;
            noti.m_len = (unsigned char)len;
            strncpy(noti.m_msg, name, len);
            CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                user->GetServerGroup());
            proxy->SendTcpPacket((char*)&noti, 0x10b);
        }
        break;
    case 4:  // 踢人 expel <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0 &&
            packet.get_int(len) && packet.get_str(name, 0x1e, len))
        {
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                resp.put_int(len);
                resp.put_str(name, len);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
                target->DisConnSig((DISCONN_SIG)0x14, true, 0);
            }
        }
        break;
    case 5:  // 禁言 forbid chat <mins> <?> <name>
    case 6:  // 禁移动 forbid move <mins> <?> <name>
    {
        bool isChat = (cmd == 5);
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0 &&
            packet.get_int(len) && packet.get_int(extra) &&
            packet.get_str(name, 0x1e, (short)extra))
        {
            // 分钟→秒钳制：<1 →60，>=0x5a0 →86400
            int sec = (len < 0x5a0) ? ((len < 1) ? 0x3c : len * 0x3c) : 0x15180;
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else if (isChat || target->get_state() == 3)
            {
                if (isChat)
                {
                    world->DisableChat(name, sec);
                }
                else
                {
                    world->DisableMove(name, sec);
                }
                TIMER_MESSAGE timerMsg =
                    isChat ? (TIMER_MESSAGE)0x5d : (TIMER_MESSAGE)0x5e;
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0,
                                            (int)target->GetUID(), timerMsg, sec,
                                            (int)target->GetLoginTick(),
                                            (unsigned int)target->getCurCharacNo());
                resp.put_int(len);
                resp.put_int(extra);
                resp.put_str(name, (short)extra);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
                if (!isChat)
                {
                    // case 6 无监控包；case 5 发 Packet_Forbid_Chat_By_Monitor
                    if (cmd == 5)
                    {
                        gm_pkt::ForbidChatByMonitor moni;
                        moni.m_seconds = sec;
                        strncpy(moni.m_name, name, sizeof(moni.m_name));
                        CMonitorServerProxy* proxy =
                            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                                target->GetServerGroup());
                        proxy->SendTcpPacket((char*)&moni, 0x30);
                    }
                }
            }
        }
        break;
    }
    case 7:  // 隐身
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0)
        {
            user->SetInvisible();
            if (user->get_state() == 3)
            {
                user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
            }
        }
        break;
    case 8:  // 现身
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0)
        {
            user->SetVisible();
            if (user->get_state() == 3)
            {
                user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
            }
        }
        break;
    case 9:  // 调级 setlevel <level> <?> <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0 &&
            packet.get_int(len) && packet.get_int(extra) &&
            packet.get_str(name, 0x1e, (short)extra))
        {
            memset(name, 0, sizeof(name));
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                int diff = len - target->get_charac_level();
                bool ok = true;
                while (diff < 0 && ok)
                {
                    int nextLv = target->get_charac_level() - 1;
                    if (nextLv > 200) nextLv = 200;
                    target->SetCharacLevel(nextLv);
                    ++diff;
                }
                while (diff > 0 && ok)
                {
                    int nextLv = target->get_charac_level() + 1;
                    if (nextLv > 200) nextLv = 200;
                    target->SetCharacLevel(nextLv);
                    --diff;
                }
                if (ok)
                {
                    int st = target->get_state();
                    if (st == 3)
                    {
                        target->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
                        target->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
                        target->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
                    }
                    else if (st == 5)
                    {
                        target->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
                        target->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                    }
                    resp.put_int(len);
                    resp.put_int(extra);
                    resp.put_str(name, (short)extra);
                    resp.finalize(true);
                    user->Send(resp);
                    target->Send(resp);
                }
            }
        }
        break;
    case 10:  // 发物品 grant item <itemIdx> <count> <?> <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0 &&
            packet.get_int(len) && packet.get_short(wLen) &&
            packet.get_int(extra) && packet.get_str(name, 0x1e, (short)extra))
        {
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)0;
                int slot = target->AddItem(len, (int)(unsigned short)wLen,
                                           (eItemAddReason)6, space, 0);
                if (slot >= 0)
                {
                    target->SendUpdateItemList((CUser::eSendTarget)1, space, slot);
                    resp.put_int(len);
                    resp.put_short(wLen);
                    resp.put_int(extra);
                    resp.put_str(name, (short)extra);
                    resp.finalize(true);
                    user->Send(resp);
                }
            }
        }
        break;
    case 0xb:  // 复活 revive <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0 &&
            packet.get_int(len) && packet.get_str(name, 0x1e, len))
        {
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            CParty* party = (target != 0) ? (CParty*)target->GetParty() : 0;
            if (party == 0)
            {
                user->Send(reject);
            }
            else
            {
                party->set_charac_live(target, (ENUM_USER_DUNGEON_STATE)1);
                resp.put_int(len);
                resp.put_str(name, len);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
            }
        }
        break;
    case 0xc:  // 重置疲劳 reset fatigue <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0 &&
            packet.get_int(len) && packet.get_str(name, 0x1e, len))
        {
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                target->RecoverFatigue(0);
                target->SendFatigue();
                resp.put_int(len);
                resp.put_str(name, len);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
            }
        }
        break;
    case 0xd:  // 封号 forbid play <days> <?> <name>（发 SIG_FORBID_USER_TO_PLAY 到 DB 队列）
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > GM_LV0 &&
            packet.get_int(len) && packet.get_int(extra) &&
            packet.get_str(name, 0x1e, (short)extra))
        {
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                Stream* stream = GlobalData::s_stream_pool->Acquire(
                    "PacketDispatcher_Impl_1.cpp", 0x8b28);
                CStreamGuard guard(stream, true);
                guard << 0x93;
                guard << (int)user->GetUID();
                SIG_FORBID_USER_TO_PLAY* sig = guard.GetInBuffer<SIG_FORBID_USER_TO_PLAY>();
                memset(sig, 0, 9);
                gm_pkt::SigForbidPlayBody* body =
                    (gm_pkt::SigForbidPlayBody*)sig;
                body->m_characNo = target->getCurCharacNo();
                body->m_flag = 1;
                body->m_days = (len < 1) ? 1 : ((len < 100) ? len : 100);
                GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
                resp.put_int(len);
                resp.put_int(extra);
                resp.put_str(name, (short)extra);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
            }
        }
        break;
    default:
        break;  // ORIG: "Undifined GM Command : %d by %s"（仅日志）
    }
    return 0;
}
int Dispatcher_GM_Command::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)user; (void)msg; (void)param;
    return 0;  // ORIG 类未声明 process（vtable 纯虚由基类骨架覆盖），恒 0
}
int Dispatcher_GM_Command::read(PacketBuf& packet, MSG_BASE& msg)
{
    (void)packet; (void)msg;
    return 0;  // 同上：ORIG 无 read，逐字段在 dispatch_sig 内读取
}

Dispatcher_GuildCargo::Dispatcher_GuildCargo() {}
Dispatcher_GuildCargo::~Dispatcher_GuildCargo() {}
int Dispatcher_GuildCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d0ae2：param+4 = check_error(...)；>0 直接返 0；<0 LineFunc(0x2c26)；
    // ==0 构造 Packet_Guild_Cargo_Requst，填 guildkey(+6)/characNo(+0xa)，
    // SendTcpPacket(pkt, *(ushort*)(pkt+2))，写历史日志 reason=0 后返 0。
    int err = check_error(user, msg, param);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x2c26,
                        __PRETTY_FUNCTION__,
                        err, user->get_acc_id());
    }
    Packet_Guild_Cargo_Requst pkt;   // ORIG W ctor 0x0822be52（局部帧 -0x1e 起）
    // 帧偏移：-0x1c=pkt+0x2(ushort size)、-0x18=pkt+0x6(guildkey)、-0x14=pkt+0xa(characNo)
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 6) =
        user->get_charac_guildkey();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 0xa) =
        user->getCurCharacNo();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt),
                         *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&pkt) + 2));
    user->m_historyLog.GuildCargo(static_cast<ENUM_GUILD_CARGO_HISTORY_REASON>(0),
                                  user->get_charac_guildkey());
    return 0;
}
int Dispatcher_GuildCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GuildCargo::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stGuildCargoParam& p = static_cast<stGuildCargoParam&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x109, (unsigned char)p.m_4);
}

Dispatcher_Join_Power::Dispatcher_Join_Power() {}
Dispatcher_Join_Power::~Dispatcher_Join_Power() {}
int Dispatcher_Join_Power::dispatch_sig(CUser* user, PacketBuf& packet)
{
    return LineFunc(0xa8a7,
                    __PRETTY_FUNCTION__,
                    0, 0);
}
int Dispatcher_Join_Power::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Join_Power::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_MotionHack::Dispatcher_MotionHack() {}
Dispatcher_MotionHack::~Dispatcher_MotionHack() {}
// ORIG 0x08264574（报告 Dispatcher_MotionHack/read.md）[随行依赖]：
// get_byte(msg+0xd) / get_int(msg+0xe) / get_int(msg+0x12)。
int Dispatcher_MotionHack::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_MOTION_HACK& req = static_cast<MSG_MOTION_HACK&>(msg);
    if (!packet.get_byte(req.m_field0d))
    {
        return LineFunc(0x414, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(req.m_field0e))
    {
        return LineFunc(0x416, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(req.m_field12))
    {
        return LineFunc(0x418, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

// ORIG 0x0826464a（报告 Dispatcher_MotionHack/process.md）：
// msg+0xd byte 非 0 → addServerHackCnt(user, 0x83, 1, msg+0xe, msg+0x12)。
int Dispatcher_MotionHack::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    MSG_MOTION_HACK& req = static_cast<MSG_MOTION_HACK&>(msg);
    if (req.m_field0d != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x83, 1u,
            (unsigned int)req.m_field0e, (unsigned int)req.m_field12);
    }
    return 0;
}

Dispatcher_MoveToGate::Dispatcher_MoveToGate() {}
Dispatcher_MoveToGate::~Dispatcher_MoveToGate() {}
int Dispatcher_MoveToGate::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8203350：state!=3 或 getCurCharacR()==0 时 LineFunc(0x9a0d)；
    // 无 user==0/manager==0/seller==0 防御检查（ORIG 直接调用）。
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9a0d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    private_store::CPrivateStoreMgr* manager =
        private_store::GetInstancePrivateStoreMgr();
    private_store::ISeller* seller = 0;
    manager->GetStoreSeller(&seller, user);
    seller->MoveToGate();
    return 0;
}
int Dispatcher_MoveToGate::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MoveToGate::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_RedeemList::Dispatcher_RedeemList() {}
Dispatcher_RedeemList::~Dispatcher_RedeemList() {}
int Dispatcher_RedeemList::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x822621c：state>2 时向用户发送兑换信息；否则返回 0
    if (user->get_state() <= 2)   // ORIG 0x822621c：无 user==0 检查
    {
        return 0;
    }
    user->send_RedeemInfo();
    return 0;
}
int Dispatcher_RedeemList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RedeemList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SelectItem::Dispatcher_SelectItem() {}
Dispatcher_SelectItem::~Dispatcher_SelectItem() {}
int Dispatcher_SelectItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0822997a：公平 PvP 装备包领取（PacketDispatcher_Impl_1.cpp）。
    if (user == 0 || user->get_state() != 3 || user->getCurCharacR() == 0)
        return 0;

    if (G_GameWorld()->GetChannelType() != 0xf &&
        G_GameWorld()->GetChannelType() != 0x10)
        return 0;
    fair_pvp::CFairPvPScore* score =
        (fair_pvp::CFairPvPScore*)(void*)user->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)7);
    if ((int)score->GetGiveItem() >
        G_CDataManager()->m_serverParameter.get_pvp_item_give(user->get_pvp_grade()))
        return 0;

    unsigned char groupCnt = 0;
    if (!packet.get_byte(groupCnt))
    {
        return LineFunc(0xe785,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    CDataManager* dm = G_CDataManager();
    int key = makeFairPVPEquipmentPackageKey(
        user->get_charac_job(), user->get_pvp_grade());

    const int grade = user->get_pvp_grade();

    // 按段位降序搜索一次命中（i: 0..grade，P2 有符号循环）。
    std::map<int, std::vector<std::vector<STStackablePackageData> > >::iterator it =
        dm->m_stackableMap.end();
    for (int attempt = 0; grade >= attempt; ++attempt)
    {
        it = dm->m_stackableMap.find(key);
        if (it != dm->m_stackableMap.end())
            break;
        key = decreaseRankOfFairPVPEquipmentPackageKey(key);
    }
    if (it == dm->m_stackableMap.end())
        return 0;

    if ((unsigned int)it->second.size() != (unsigned int)groupCnt)
        return 0;

    std::vector<Inven_Item> items;
    for (std::vector<std::vector<STStackablePackageData> >::const_iterator
             grp = it->second.begin();
         grp != it->second.end(); ++grp)
    {
        unsigned char selIdx = 0;
        if (!packet.get_byte(selIdx))
        {
            return LineFunc(0xe7a1,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if ((unsigned int)grp->size() <= (unsigned int)selIdx)
        {
            return LineFunc(0xe7a4,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }

        const STStackablePackageData& data = (*grp)[selIdx];   // ORIG vector::at（先 size 校验）
        Inven_Item newItem;
        newItem.m_addInfo = data.m_field0;
        CItem* item = dm->find_item(data.m_field0);   // 推断：dispatch 域以基类指针持有
        if (item == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x196, 0x11);
            return 0;
        }
        item->make_item(newItem);   // CItem vtable +0x08 槽（虚表实证）
        if (newItem.m_field1 != (unsigned char)Inven_Item::ITEM_TYPE_EQUIP)
        {
            newItem.m_addInfo2 = data.m_field4;
        }
        items.push_back(newItem);
    }

    for (std::vector<Inven_Item>::iterator itemIt = items.begin();
         itemIt != items.end(); ++itemIt)
    {
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            *itemIt, (eItemAddReason)8, true, true);
        if (slot < 0)
        {
            char title[21];
            strncpy(title,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_171", (bool*)0),
                    sizeof(title));
            char msgBuf[0xff];
            strncpy(msgBuf,
                    g_scriptStringManager_.findString(
                        4, "game_server_msg_172", (bool*)0),
                    sizeof(msgBuf));
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, *itemIt, 0u, (unsigned int)user->getCurCharacNo(),
                msgBuf, (int)strlen(msgBuf), 0u, user->GetServerGroup(),
                false, false);
        }
        else
        {
            user->SendUpdateItemList(
                (CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slot);
        }
    }

    score->SetGiveItem(user->get_pvp_grade() + 1);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x196);
    return 0;
}
// ---- IsEmancipate 弱符号内联组（ORIG P9 直返单字节；本 TU 提供定义） ----
unsigned char CStackableItem::IsEmancipate()
{
    return (unsigned char)m_char2f0;
}

unsigned char CEquipItem::IsEmancipate()
{
    return (unsigned char)m_char194;
}
int Dispatcher_SelectItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SelectItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TraceError::Dispatcher_TraceError() {}
Dispatcher_TraceError::~Dispatcher_TraceError() {}
int Dispatcher_TraceError::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char errorType = 0;
    if (!packet.get_byte(errorType))
    {
        return LineFunc(0x9c74,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (errorType == 2)
    {
        char errorCode = 0;
        if (!packet.get_byte(errorCode))
        {
            return LineFunc(0x9cb3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x9cb5, 0);
        trace("KILLBOSS2 client - error type(%d)", (int)errorCode);
    }
    else if (errorType == 3)
    {
        return LineFunc(0x9c85,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_TraceError::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TraceError::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TryEnchant::Dispatcher_TryEnchant() {}
Dispatcher_TryEnchant::~Dispatcher_TryEnchant() {}
int Dispatcher_TryEnchant::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820ae84：按 msg+0xd 类别走 Enchanter 附魔请求链。
    MSG_STATIC_COMMAND* msg = reinterpret_cast<MSG_STATIC_COMMAND*>(packet.get_packet(0));
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xa70b, __PRETTY_FUNCTION__, 0, 0);
    }
    int ret = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x1f);
    if (ret != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xac, (unsigned char)(ret & 0xff));
        return 0;
    }
    switch (msg->m_type0d)
    {
    case 0:
    {
        expert_job::CExpertJob* job = GlobalData::s_ExpertJobMgr->AcquireExpertJob((ENUM_EXPERT_JOB_TYPE)1);
        if (job == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xac, 0x16);
            return 0;
        }
        job->OnStartMission(user, user);
        break;
    }
    case 1:
        if (user->GetCurCharacExpertJob() == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xac, 0x16);
            return 0;
        }
        break;
    case 2:
        if (user->GetCurCharacExpertJob() == 0)
        {
            return 0;
        }
        break;
    default:
        return LineFunc(0xa732, __PRETTY_FUNCTION__, 0, 0);
    }
    expert_job::CExpertJob* curJob = user->GetCurCharacExpertJob();
    if (curJob->GetType() != 1)
    {
        return LineFunc(0xa737, __PRETTY_FUNCTION__, 0, 0);
    }
    static_cast<expert_job::CEnchanter*>(curJob)->OnTryEnchant(user, msg);
    return 0;
}
int Dispatcher_TryEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TryEnchant::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_VerifyGold::Dispatcher_VerifyGold() {}
Dispatcher_VerifyGold::~Dispatcher_VerifyGold() {}
int Dispatcher_VerifyGold::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() < 3)
    {
        return 0;
    }
    unsigned short value = 0;
    if (!packet.get_short(value))
    {
        return LineFunc(0xde71,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PacketGuard response;
    response.put_header(1, 0x11f);
    response.put_byte(1);
    response.put_short(value);
    response.put_int(user->getCurCharacMoney());
    response.finalize(true);
    user->Send(response);
    return 0;
}
int Dispatcher_VerifyGold::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_VerifyGold::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BingoReward::Dispatcher_BingoReward() {}
Dispatcher_BingoReward::~Dispatcher_BingoReward() {}
int Dispatcher_BingoReward::check_error(CUser* user, MSG_BASE& msg)
{
    char isEventing;
    BingoData* bingo;
    int ret;

    (void)msg;
    if (user == 0)
        return 1;          // ORIG 0x0825e1d6：user/charac 为空返回 1（非 -1）
    if (user->getCurCharacR() == 0)
        return 1;
    if (user->get_state() != 3)
        return 0x1e;
    {
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x6a);
        // ORIG vtable+0x34 槽直调（CEventBase::IsEventing(0)，与 CBattle_Field_KillChain.cpp
        // bfkcIsEventing 同口径；CGuildServerProxy.h 槽位注释与 ORIG 槽序有出入，用直调保 ABI）。
        typedef char (*IsEventingFn)(void*, CUser*);
        IsEventingFn fn = reinterpret_cast<IsEventingFn>(
            (*reinterpret_cast<void***>(ev))[0x34 / 4]);
        isEventing = fn(ev, reinterpret_cast<CUser*>(0));
        if (isEventing)
            return 0x13;
    }
    // ORIG 0x0825e1d6 每次访问均重新调用 getBingoData；byte5/byte4 按有符号 char 比较。
    if (*(reinterpret_cast<char*>(user->getBingoData()) + 5))
        return 0x12;
    if ((int)*(reinterpret_cast<char*>(user->getBingoData()) + 4) <= 2)
        return 0x16;
    ret = 0;
    return ret;
}
int Dispatcher_BingoReward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x0825e68c：param+4 = check_error(user,msg)；user!=0 时 param+8 =
    // *(int*)user->getBingoData()；>0：logFormat(0x19a,"Dispatcher Check_Error No(%d)")
    // + LineFunc(0x19b)；<1 且 >=0：getBingoData()+6/+5 置 1，取 repeat 事件 0x6a
    // 非空则 sendMaxMatchLineReward(*user) + sendDecorationObjectCnt(1)，恒返 0。
    int err = check_error(user, msg);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = err;
    if (user != 0)
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 8) =
            *reinterpret_cast<int*>(user->getBingoData());
    }
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_2.cpp",
            __PRETTY_FUNCTION__,
            0x19a, "Dispatcher Check_Error No(%d)",
            *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4));
        return LineFunc(0x19b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char* data = reinterpret_cast<char*>(user->getBingoData());
    data[6] = 1;
    data[5] = 1;
    BingoEvent* event =
        reinterpret_cast<BingoEvent*>(GlobalData::s_event_manager->GetRepeatEvent(0x6a));
    if (event != 0)
    {
        event->sendMaxMatchLineReward(*user);
        user->sendDecorationObjectCnt(1);
    }
    return 0;
}
int Dispatcher_BingoReward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_BingoReward::send(CUser* user, ParamBase& param)
{
    // ORIG 0x0825e87a：param+4==0 → SendCmdOkPacket(0x202)；否则 header(1,0x202)+byte 0+byte err+int data
    stBingoReward& p = static_cast<stBingoReward&>(param);
    if (p.m_result == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x202);
        return;
    }
    PacketGuard pkt;
    pkt.put_header(1, 0x202);
    pkt.put_byte(0);
    pkt.put_byte(p.m_result);
    pkt.put_int(p.m_bingoData);
    pkt.finalize(true);
    user->Send(pkt);
}

Dispatcher_CranePickUp::Dispatcher_CranePickUp() {}
Dispatcher_CranePickUp::~Dispatcher_CranePickUp() {}
int Dispatcher_CranePickUp::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821d508（docs/class_func_reports/Dispatcher_CranePickUp/dispatch_sig.md）
    short slot = -1;                          // local_2a -0x26
    int pickIdx = -1;                         // local_28 -0x24
    unsigned int errCode = 0;                 // local_14 -0x10
    bool sendResultFail = false;              // local_d -0x9

    // +0x00：state != 3 → 错误 0x13
    if (user->get_state() != 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e7, 0x13);
        return 0;
    }
    // +0x42：!isUseCraneStart → 错误 0x11
    if (!user->isUseCraneStart())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e7, 0x11);
        return 0;
    }
    // +0x79：复位抓娃娃启动标志
    user->setUseCraneStart(false);

    // +0x9a：读槽位/抽取序号
    if (!packet.get_short(slot))
        return LineFunc(0xd122,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(pickIdx))
        return LineFunc(0xd123,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x128：判定抓取
    errCode = 0;
    if (G_CGameManager()->GetCraneMinigameManager()->isPickSuccess(slot, pickIdx))
    {
        errCode = (unsigned int)G_CGameManager()->GetCraneMinigameManager()->sendItem(user, (int)slot);
        sendResultFail = (errCode == 0);
    }
    else
    {
        errCode = 0xbe;
    }

    // +0x190：sendItem 成功（errCode==0）→ 应答包；否则失败 trace + 错误包
    if (!sendResultFail)
    {
        PacketGuard guard;                                            // 0858dd4c
        guard.put_header(1, 0x1e7);
        guard.put_byte(1);
        guard.put_int(pickIdx);
        guard.put_short((short)G_CGameManager()->GetCraneMinigameManager()->getItemCnt((int)slot));
        guard.finalize(true);
        user->Send(guard);
        return 0;                                                     // guard 析构 0858de80
    }

    // +0x19f：失败路径
    {
        char* accIdStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xd135, 0);
        trace("ServerMessage : Dispatcher_CranePickUp failed : m_id : %s error %d",
              accIdStr, errCode);
    }
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e7, (unsigned char)(errCode & 0xff));
    return 0;
}
int Dispatcher_CranePickUp::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CranePickUp::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_EPLPCommand::Dispatcher_EPLPCommand() {}
Dispatcher_EPLPCommand::~Dispatcher_EPLPCommand() {}
int Dispatcher_EPLPCommand::dispatch_sig(CUser* user, PacketBuf& packet)
{
    MSG_EPLP_COMMAND local_33;           // 未初始化（_BroadCastPacket 内部填充）
    unsigned char local_26;              // -0x22：命令字节
    unsigned char local_25;              // -0x21：子命令字节
    CParty* local_24;                    // -0x20
    void* local_20;                      // -0x1c：SECRET_SHOP_DATA*
    int local_1c;                        // -0x18：映射后的 EPLP 命令
    unsigned char local_15;              // -0x11：isOpenAdvanceAltar
    WongWork::CBossTower* local_14;      // -0x10
    WongWork::CDeathTower* local_10;     // -0xc
    PacketGuard local_40;                // -0x48：advancealtar 应答包（ORIG PacketGuard ctor 0x0858dd4c）
    char cVar2;
    int iVar4;
    int ret;

    if (!Global::CGlobalFunc::Get_State_Check((ch_state)user->get_state()))
    {
        return 0;
    }
    cVar2 = packet.get_byte(local_26);
    if ((char)(cVar2 ^ 1) != 0)          // cVar2!=1 → LineFunc 0x9238
    {
        ret = LineFunc(0x9238, __PRETTY_FUNCTION__, 0, 0);
        return ret;
    }
    cVar2 = packet.get_byte(local_25);
    if ((char)(cVar2 ^ 1) != 0)          // cVar2!=1 → LineFunc 0x9239
    {
        ret = LineFunc(0x9239, __PRETTY_FUNCTION__, 0, 0);
        return ret;
    }
    iVar4 = user->get_state();
    if (iVar4 == 0xd)                    // advancealtar 场景
    {
        local_15 = (unsigned char)advancealtar::isOpenAdvanceAltar();
        local_40.put_header(1, 0x4b);
        local_40.put_byte((int)local_15);
        local_40.put_byte((int)local_26);
        local_40.put_byte((int)local_25);
        local_40.finalize(true);
        user->Send(local_40);
        if (local_26 == 1)
        {
            advancealtar::Manager::eplp(user, (advancealtar::EplpType::T)local_25);
        }
        return 0;
    }
    local_24 = (CParty*)user->GetParty();
    if (local_24 == 0)
    {
        return 0;
    }
    if (local_24->getManager() != user)
    {
        _SendEPLPError(local_24, local_25, 8);
        return 0;
    }
    cVar2 = local_24->GetEPLPState();
    if (!(cVar2 == 2) && excludeEplpState(user))
    {
        return 0;
    }
    _BroadCastPacket(local_24, local_33);
    if (local_26 != 1)
    {
        return 0;
    }
    local_20 = user->GetSecretShopData();
    bool bVar1 = false;
    if (local_20 != 0 && ((secretshop::SECRET_SHOP_DATA*)local_20)->IsOpen() != 0)
    {
        bVar1 = true;
    }
    if (bVar1 && ((secretshop::SECRET_SHOP_DATA*)local_20)->IsCompleteBuy() == 0)
    {
        _SendEPLPError(local_24, local_25, 0x85);
        return 0;
    }
    local_1c = 3;
    if (local_25 == 2)
    {
        local_1c = 1;
    }
    else if (local_25 > 2)
    {
        if (local_25 == 3)
        {
            local_24->SetSelectedEPLPCmd(3);
            return 0;
        }
        _SendEPLPError(local_24, local_25, 1);
    }
    iVar4 = user->get_state();
    if (iVar4 == 0xc)
    {
        local_14 = user->getBossTower();
        if (local_14 == 0)
        {
            return 0;
        }
        local_14->handleEPLPCommand((int)local_25);
        return 0;
    }
    iVar4 = user->get_state();
    if (iVar4 == 10)
    {
        local_10 = user->getDeathTower();
        if (local_10 == 0)
        {
            ret = LineFunc(0x92b9, __PRETTY_FUNCTION__, 0, 0);
            return ret;
        }
        local_10->handleSelectEPLPCommand(user, local_1c);
        return 0;
    }
    if (!isReTurnToVillage(user))
    {
        local_24->SelectEPLPCommand(local_1c);
    }
    else
    {
        local_24->ReturnToVillage();
    }
    return 0;
}
int Dispatcher_EPLPCommand::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_EPLPCommand::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ModItemAttr::Dispatcher_ModItemAttr() {}
Dispatcher_ModItemAttr::~Dispatcher_ModItemAttr() {}



Dispatcher_SecurityLog::Dispatcher_SecurityLog() {}
Dispatcher_SecurityLog::~Dispatcher_SecurityLog() {}
int Dispatcher_SecurityLog::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x0825fb38：acc_id = user==0 ? 0 : user->get_acc_id()；
    // 当 *(int*)(msg+0x15)!=0 且 *(int*)(msg+0x19)!=0 时
    // LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp", PRETTY, 0x9d0,
    //   "Delay check, mode:%d, value:%d, %d, %d, %d, %d, %d, m_id:%s",
    //   *(msg+0xd), *(msg+0x11), *(msg+0x15), *(msg+0x19), *(msg+0x1d), *(msg+0x21),
    //   *(int*)(msg+0x1d) - *(int*)(msg+0x19), NumberToString(acc_id, 0))；
    // 恒返 0。
    unsigned int accId;
    if (user == 0)
    {
        accId = 0;
    }
    else
    {
        accId = user->get_acc_id();
    }
    const char* m = reinterpret_cast<const char*>(&msg);
    if (*reinterpret_cast<const int*>(m + 0x15) != 0 &&
        *reinterpret_cast<const int*>(m + 0x19) != 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_2.cpp",
            __PRETTY_FUNCTION__,
            0x9d0,
            "Delay check, mode:%d, value:%d, %d, %d, %d, %d, %d, m_id:%s",
            *reinterpret_cast<const int*>(m + 0xd),
            *reinterpret_cast<const int*>(m + 0x11),
            *reinterpret_cast<const int*>(m + 0x15),
            *reinterpret_cast<const int*>(m + 0x19),
            *reinterpret_cast<const int*>(m + 0x1d),
            *reinterpret_cast<const int*>(m + 0x21),
            *reinterpret_cast<const int*>(m + 0x1d) -
                *reinterpret_cast<const int*>(m + 0x19),
            NumberToString(accId, 0));
    }
    return 0;
}
int Dispatcher_SecurityLog::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x0825fabc：int 中转 → 0xd；循环 5 个 int → 0x11/0x15/0x19/0x1d/0x21；失败 0x99a/0x9a2
    char* m = reinterpret_cast<char*>(&msg);
    int first = 0;
    if (!packet.get_int(first))
        return 0x99a;
    *reinterpret_cast<int*>(m + 0xd) = first;
    for (int i = 0; i < 5; ++i)
    {
        if (!packet.get_int(*reinterpret_cast<unsigned int*>(m + 1 + (i + 4) * 4)))
            return 0x9a2;
    }
    return 0;
}
void Dispatcher_SecurityLog::send(CUser* user, ParamBase& param) {}

Dispatcher_Select_Card::Dispatcher_Select_Card() {}
Dispatcher_Select_Card::~Dispatcher_Select_Card() {}
int Dispatcher_Select_Card::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ff7de: state==5 门
    if (user->get_state() != 5)
        return 0;

    // +0x02c: getMoveSpace()==1 → 放弃
    if (user->getMoveSpace() == 1)
        return 0;

    // +0x04b: GetPartyIndex → CGameManager::GetParty
    short partyIndex = user->GetPartyIndex();
    CParty* party = G_CGameManager()->GetParty(partyIndex);
    if (party == 0)
    {
        return LineFunc(0x9371, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x0a8: 副本为 TowerOfDespair → 放弃
    if (party->m_dungeon != 0 && party->m_dungeon->isTowerOfDespairDungeon())
        return 0;

    // +0x0e4: 读 cardType（0..1 有效）
    char cardType = 0;
    if (!packet.get_byte(cardType))
    {
        return LineFunc(0x9382, __PRETTY_FUNCTION__, 0, 0);
    }
    if (cardType < 0 || cardType > 1)
    {
        return LineFunc(0x9384, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x15f: 读 cardNo
    char cardNo = 0;
    if (!packet.get_byte(cardNo))
    {
        return LineFunc(0x9392, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x19a: clear_state > 4 → 放弃
    if (party->get_dungeon_clear_state() > 4)
        return 0;

    // +0x1c0: 全员未选卡完成才登记
    if (!party->IsCompleteCardSelectOfMembers(static_cast<eClearRewardCardType_t>(2)))
    {
        // +0x1ef: SetCardNumber(user, cardNo, cardType)
        if (!party->SetCardNumber(user, cardNo, static_cast<eClearRewardCardType_t>(cardType)))
            return 0;
        party->SendEachCardInfo();
    }

    // +0x21a: 全员已接收 → 若全员选卡完成则推进 clear state 5 并 dungeon_clear
    if (!party->CheckRecvFromAllMember(user))
        return 0;
    if (party->IsCompleteCardSelectOfMembers(static_cast<eClearRewardCardType_t>(2)))
    {
        party->set_dungeon_clear_state(5);
        party->dungeon_clear();
    }
    return 0;
}
int Dispatcher_Select_Card::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Select_Card::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeItem::Dispatcher_UpgradeItem() {}
Dispatcher_UpgradeItem::~Dispatcher_UpgradeItem() {}
int Dispatcher_UpgradeItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->CheckInTrade())
    {
        return 0;                                    // ORIG 空用户/交易中静默丢弃
    }
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0x9518,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (user->CheckInTrade())                        // ORIG 二次检查
    {
        return 0;
    }

    MSG_ITEM_UPGRADE req;                            // ORIG ctor @0x0822dcea

    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(req.m_padD + 0x0d)))   // +0x1a byte
    {
        return LineFunc(
            0x9530,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req.m_padD + 0x0e))) // +0x1b word（槽位）
    {
        return LineFunc(
            0x9532,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(req.m_padD + 0x10)))    // +0x1d dword
    {
        return LineFunc(
            0x9534,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req.m_padD + 0x16))) // +0x23 word
    {
        return LineFunc(
            0x9536,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req.m_padD + 0x14))) // +0x21 word
    {
        return LineFunc(
            0x9539,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    unsigned int strLen = 0;
    memset(req.m_28, 0, sizeof(req.m_28));           // ORIG memset(this+0x28,0,0x81)
    if (!packet.get_int(strLen))                     // 字符串长度前导
    {
        return LineFunc(
            0x9540,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_str(req.m_28, (int)sizeof(req.m_28), (int)strLen))
    {
        return LineFunc(
            0x9542,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    ServiceRestrictManager* restrictMgr = ARAD::Singleton<ServiceRestrictManager>::Get();
    if (restrictMgr->isRestricted(user, RestrictCategory::Enum_1, 7))
    {
        user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x53, 0xd1);
        return 0;
    }

    const unsigned int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x25);
    if (secu != 0)
    {
        user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x53,
                                      (unsigned short)(secu & 0xffff));
        return 0;
    }

    if (user->CheckItemLock(1, *reinterpret_cast<unsigned short*>(req.m_padD + 0x0e)))
    {
        user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x53, 0xd5);
        return 0;
    }

    G_CDataManager()->m_itemUpgrade->item_upgrade_system(user, &req, 0x53);
    return 0;
}
int Dispatcher_UpgradeItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AgreeEnchant::Dispatcher_AgreeEnchant() {}
Dispatcher_AgreeEnchant::~Dispatcher_AgreeEnchant() {}
// ORIG 0x0820adc6（报告 Dispatcher_AgreeEnchant/dispatch_sig.md）：
// get_packet(0) → MSG_STATIC_COMMAND*；curCharac==0 → LineFunc 0xa6f1；
// expertJob==0 / GetType()!=1 → 静默 return 0；否则 CEnchanter::OnAgreeEnchant。
int Dispatcher_AgreeEnchant::dispatch_sig(CUser* user, PacketBuf& packet)
{
    MSG_STATIC_COMMAND* cmd = (MSG_STATIC_COMMAND*)packet.get_packet(0);
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xa6f1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    expert_job::CExpertJob* job = user->GetCurCharacExpertJob();
    if (job == 0)
    {
        return 0;
    }
    if (job->GetType() != 1)
    {
        return 0;
    }
    expert_job::CEnchanter* enchanter = static_cast<expert_job::CEnchanter*>(job);
    enchanter->OnAgreeEnchant(user, cmd);
    return 0;
}
int Dispatcher_AgreeEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AgreeEnchant::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Back2Village::Dispatcher_Back2Village() {}
Dispatcher_Back2Village::~Dispatcher_Back2Village() {}
int Dispatcher_Back2Village::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() == 0xd)
    {
        advancealtar::Manager::leaveUser(user);
        return 0;
    }
    if (user->get_state() != 5)
    {
        return 0;
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0 || party->GetEPLPState() != 0)
    {
        return 0;
    }
    if (party->getManager() == user)
    {
        if (party->get_quick_party_index() != -1)
        {
            G_CGameManager()->CheckOutQuickParty(party, true);
        }
        party->ReturnToVillage();
        return 0;
    }
    if (party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2) != 1)
    {
        PacketGuard notification;
        G_GameWorld()->out_from_dungeon(user);
        notification.clear();
        notification.put_header(0, 2);
        notification.put_byte(0);
        notification.put_short(1);
        user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
        notification.finalize(true);
        G_GameWorld()->send_all(notification, user);
    }
    return 0;
}
int Dispatcher_Back2Village::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Back2Village::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// process 依赖（清单外）：清 param.m_8=0；GetPartyIndex 找不到队伍 → cMyTrace
// "Party == NULL"，返回 8；否则 0。（声明需入 Dispatchers.h，见 NEEDS_DECL）
int Dispatcher_BossDieCheck::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBossDieCheckParam& out = static_cast<stBossDieCheckParam&>(param);
    out.m_8 = 0;
    CParty* party =
        G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
    if (party == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x258b, 0);
        trace("Party == NULL");
        return 8;
    }
    return 0;
}

Dispatcher_BossDieCheck::Dispatcher_BossDieCheck() {}
Dispatcher_BossDieCheck::~Dispatcher_BossDieCheck() {}
// 技能校验和比对（不等记 hack 0x25c）；从战场 m_apcList(party+0xc3c) 删除
// 上报 boss 序号(msg+0x10)，未命中 param=0x15；删空后记时刻、给队长挂 100ms
// 定时器(param3=characNo) 并置 param.m_8=1；msg+0xe==-1 记 hack 0xcb。
int Dispatcher_BossDieCheck::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBossDieCheckParam& out = static_cast<stBossDieCheckParam&>(param);
    MSG_BOSS_DIE_CHECK& req = static_cast<MSG_BOSS_DIE_CHECK&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    unsigned short& bossSeq =
        *reinterpret_cast<unsigned short*>(pReq + 0x10);   // +0x10 word
    short& unkKind =
        *reinterpret_cast<short*>(pReq + 0xe);             // +0x0e word

    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x259e,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }

    if (user->getCurCharacSkillCheckSum() !=
        *reinterpret_cast<int*>(pReq + 0x14))              // +0x14 int
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25c, 1, 0, 0);
    }

    CParty* party =
        G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(
        reinterpret_cast<char*>(party) + 0xb24);           // party+0xc3c = BF+0x118
    std::vector<int>::iterator it = std::find(field->m_apcList.begin(),
                                              field->m_apcList.end(),
                                              bossSeq);
    if (it != field->m_apcList.end())
    {
        field->m_apcList.erase(it);
        out.m_a = (short)bossSeq;
    }
    else
    {
        out.m_4 = 0x15;
    }

    if ((int)field->m_apcList.size() == 0)                 // party+0xc48 = BF+0x124
    {
        field->m_apcAllKilledSec = GlobalData::s_systemTime_.getCurSec();
        int characNo = user->get_charac_no(-1);
        G_TimerQueue()->InsertTimerInMilisecond(
            (TimerEntry::OBJ_TYPE)1, party->GetPartyIndex(),
            (TIMER_MESSAGE)0x64, (long long)1000,
            field->m_apcAllKilledSec, (unsigned int)characNo);
        out.m_8 = 1;
    }

    if (unkKind == -1)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0xcb, 1, 0, 0);
    }
    return 0;
}
int Dispatcher_BossDieCheck::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BOSS_DIE_CHECK& req = static_cast<MSG_BOSS_DIE_CHECK&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    unsigned short& unkE =
        *reinterpret_cast<unsigned short*>(pReq + 0xe);
    unsigned short& bossSeq =
        *reinterpret_cast<unsigned short*>(pReq + 0x10);
    int& checksum =
        *reinterpret_cast<int*>(pReq + 0x14);
    if (!packet.get_short(unkE))
    {
        return LineFunc(0x2578,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(bossSeq))
    {
        return LineFunc(0x2579,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(checksum))
    {
        return LineFunc(0x257b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// param.m_c==2 先发命令错误包 0xb7；再组 header(0,0x73)：成功(m_4==0)回
// {1, m_8, m_a}，失败回 {0, m_4}；有队伍发全队，否则单发。
void Dispatcher_BossDieCheck::send(CUser* user, ParamBase& param)
{
    stBossDieCheckParam& out = static_cast<stBossDieCheckParam&>(param);
    if (out.m_c == 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb7, 2);
    }
    PacketGuard packet;
    packet.put_header(0, 0x73);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_byte((unsigned char)out.m_8);
        packet.put_short((unsigned short)out.m_a);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4);
    }
    packet.finalize(true);
    CParty* party =
        G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
    if (party == 0)
    {
        user->Send(packet);
    }
    else
    {
        party->send_to_party(packet);
    }
}

Dispatcher_CodeCheckSum::Dispatcher_CodeCheckSum() {}
Dispatcher_CodeCheckSum::~Dispatcher_CodeCheckSum() {}
int Dispatcher_CodeCheckSum::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82044b0 (101 insn)：读 int + 0x20 字节校验块 → checkCodeChecksum；
    // 失败且 state>2 时记 hack(0x3c8)
    if (user->get_state() < 2)
    {
        return 0;
    }
    int seq = 0;
    if (!packet.get_int(seq))
    {
        return LineFunc(0x9bfd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char checksum[32];
    if (!packet.get_binary(reinterpret_cast<char*>(checksum), 0x20))
    {
        return LineFunc(0x9bff,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    char path[260] = { 0 };                            // ORIG rep stos 0x104 清零

    // ORIG 0x0820459a：checkCodeChecksum(checksum, path)；失败（返回 0）时
    // failKind = strstr(path,".exe.bak")?2:3 且 state>1 记 hack(0x3c8)。
    const bool checksumOk = user->checkCodeChecksum(
        *reinterpret_cast<CodeHackChecksum*>(checksum), path);
    user->IncChecksumAnswerCount();
    if (!checksumOk)
    {
        int failKind = 2;                                          // ORIG 先置 2
        if (std::strstr(path, ".exe.bak") == 0)                    // strstr 命中保持 2
            failKind = 3;
        (void)failKind;                                            // ORIG 死存储保留
        if (user->get_state() > 1)
        {
            user->getHackAnalyzer()->addServerHackCnt(user,
                                                      static_cast<WongWork::ENUM_HACKTYPE>(0x3c8),
                                                      1, 0, 0);
        }
    }
    return 0;
}
int Dispatcher_CodeCheckSum::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CodeCheckSum::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// process 依赖（清单外）：state!=3→7；count(msg+0x10)!=1→0x16；
// GetInvenSlot(EQUIP,slot) 后 find_item 缺失→0x11、持有数不足→0x16；
// 非 0x12ca 物品→0x17；0x12ca 时摆摊忙→0x3c、时光门开→0x16，否则 0。
// （声明需入 Dispatchers.h，见 NEEDS_DECL）
int Dispatcher_CollectItems::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_COLLECT_ITEMS& req = static_cast<MSG_COLLECT_ITEMS&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    short& slot = *reinterpret_cast<short*>(pReq + 0xe);          // +0xe word
    unsigned short& count = *reinterpret_cast<unsigned short*>(pReq + 0x10);

    if (user->get_state() != 3)
    {
        return 7;
    }
    if ((unsigned short)count != 1)
    {
        return 0x16;
    }
    Inven_Item slotItem =
        user->getCurCharacInvenR()->GetInvenSlot(1, (int)slot);
    CItem* item = G_CDataManager()->find_item(slotItem.m_addInfo);
    if (item == 0)
    {
        return 0x11;
    }
    if (slotItem.m_addInfo2 < (int)(unsigned int)count)
    {
        return 0x16;
    }
    if (slotItem.m_addInfo != 0x12ca)
    {
        return 0x17;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x3c;
    }
    if (GlobalData::s_timeGate_->isOpen())
    {
        return 0x16;
    }
    return 0;
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

Packet_CollectItems::Packet_CollectItems() : PacketHeader(0x27e6, 0x39)
{
    // ORIG 存储顺序 +0x12→+0x16→+0xe→+0x1a（函数体赋值序，非成员声明序）
    m_itemIdx = 0;
    m_characNo = 0;
    m_accId = 0;
    m_nameLen = 0;
    memset(m_name, 0, sizeof(m_name));
}

Dispatcher_CollectItems::Dispatcher_CollectItems() {}
Dispatcher_CollectItems::~Dispatcher_CollectItems() {}
// 删收集物（INVEN_TYPE 1 / reason 3），失败 LineFunc(0x5aa7)；成功把
// slot/count 回填 param，并向监控服代理上报 Packet_CollectItems(0x39 字节)。
int Dispatcher_CollectItems::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stCollectItems& out = static_cast<stCollectItems&>(param);
    MSG_COLLECT_ITEMS& req = static_cast<MSG_COLLECT_ITEMS&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    short& slot = *reinterpret_cast<short*>(pReq + 0xe);          // +0xe word
    unsigned short& count = *reinterpret_cast<unsigned short*>(pReq + 0x10);

    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x5aa2,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }

    CInventory* invenW = user->getCurCharacInvenW();
    if (!invenW->delete_item((INVEN_TYPE)1, (int)slot, (unsigned short)count,
                             (eItemDelReason)3, true))
    {
        return LineFunc(0x5aa7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    out.m_8 = slot;
    out.m_a = (short)count;

    Packet_CollectItems notify;
    notify.m_itemIdx = (unsigned int)count;
    notify.m_characNo = user->get_charac_no(-1);
    notify.m_accId = user->get_acc_id();
    const char* name = user->getCurCharacName();
    notify.m_nameLen = (char)strlen(name);
    strncpy(notify.m_name, name, 0x1d);
    CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&notify), 0x39);
    return 0;
}
int Dispatcher_CollectItems::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_COLLECT_ITEMS& req = static_cast<MSG_COLLECT_ITEMS&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    short& slot = *reinterpret_cast<short*>(pReq + 0xe);
    unsigned short& count = *reinterpret_cast<unsigned short*>(pReq + 0x10);
    if (!packet.get_short(slot))
    {
        return LineFunc(0x5a8f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(count))
    {
        return LineFunc(0x5a90,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// err==0x7fffffff 静默；err==0 回 header(1,0x1e3){1, m_8(signed), m_a}；
// 其余发命令错误包 0x1e3。
void Dispatcher_CollectItems::send(CUser* user, ParamBase& param)
{
    stCollectItems& out = static_cast<stCollectItems&>(param);
    if (out.m_4 == (int)0x7fffffff)
    {
        return;
    }
    if (out.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e3,
                                 (unsigned char)(out.m_4 & 0xff));
        return;
    }
    PacketGuard packet;
    packet.put_header(1, 0x1e3);
    packet.put_byte(1);
    packet.put_short((int)out.m_8);
    packet.put_short((unsigned short)out.m_a);
    packet.finalize(true);
    user->Send(packet);
}

Dispatcher_DepositMoney::Dispatcher_DepositMoney() {}
Dispatcher_DepositMoney::~Dispatcher_DepositMoney() {}
int Dispatcher_DepositMoney::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    unsigned int money = 0;
    if (!packet.get_int(money))
    {
        return LineFunc(0xdfc2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!user->IsExistAccountCargo())
    {
        return 0;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    if (cargo->DepositMoney(money) != 0)
    {
        user->m_historyLog.AccountCargoAddItem(
            0, (int)cargo->GetMoney(), (int)money, (eItemAddReason)0x2d);
    }
    return 0;
}
int Dispatcher_DepositMoney::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DepositMoney::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_EvadeAssault::Dispatcher_EvadeAssault() {}
Dispatcher_EvadeAssault::~Dispatcher_EvadeAssault() {}
int Dispatcher_EvadeAssault::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty = "int Dispatcher_EvadeAssault::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x27c2, pretty, 0, 0);
    }
    if (user->get_state() == 3)
    {
        return 0;
    }
    return LineFunc(0x27c5, pretty, 0, user->get_acc_id());
}
// ORIG 0x081ce4f8（报告 Dispatcher_EvadeAssault/process.md）：
// err>0 → err；err<0 → -1；OnEvadeAssaultPlace==1 → 0；否则 LineFunc(0x27b8)。
int Dispatcher_EvadeAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    pvp_assault::CAssaultMgr* mgr = pvp_assault::GetInstanceAssaultMgr();
    if (mgr->OnEvadeAssaultPlace(user) == 1)
    {
        return 0;
    }
    return LineFunc(0x27b8,
                    __PRETTY_FUNCTION__,
                    0, user->get_acc_id());
}
int Dispatcher_EvadeAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_FairPvPScore::Dispatcher_FairPvPScore() {}
Dispatcher_FairPvPScore::~Dispatcher_FairPvPScore() {}
int Dispatcher_FairPvPScore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0822793c（PacketDispatcher_Impl_1.cpp 0xe439 区）
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    unsigned short uniqueId = 0;
    if (!packet.get_short(uniqueId))
    {
        return LineFunc(0xe439,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char mode = 0;
    if (!packet.get_byte(mode))
    {
        return LineFunc(0xe43c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if ((unsigned short)user->get_unique_id() == uniqueId)
    {
        PacketGuard guard;
        guard.put_header(1, 0x177);
        guard.put_byte(1);
        guard.put_short((int)uniqueId);
        fair_pvp::CFairPvPScore* score =
            (fair_pvp::CFairPvPScore*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
        score->MakePacketSocre(guard, (int)mode);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    CUser* target = G_GameWorld()->find_from_world(uniqueId);
    if (target == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x177, 3);
        return 0;
    }
    PacketGuard guard;
    if (mode == 1)
    {
        CInventory* inven = (CInventory*)user->getCurCharacInvenR();
        int slot = inven->check_item_exist(0xcf0);
        if (slot == -1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x177, 0x11);
            return 0;
        }
        Inven_Item item = inven->GetInvenSlot(1, slot);
        if (item.get_add_info() < 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x177, 0x11);
            return 0;
        }
        if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slot, 1,
                                                    (eItemDelReason)3, true) != 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x177, 0x11);
            return 0;
        }
        guard.clear();
        guard.put_header(0, 0xe);
        guard.put_byte(0);
        guard.put_short(1);
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, guard);
        guard.finalize(true);
        user->Send(guard);
    }
    guard.clear();
    guard.put_header(1, 0x177);
    guard.put_byte(1);
    guard.put_short((int)uniqueId);
    fair_pvp::CFairPvPScore* score =
        (fair_pvp::CFairPvPScore*)target->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)7);
    score->MakePacketSocre(guard, (int)mode);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
int Dispatcher_FairPvPScore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_FairPvPScore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_HumanCertify::Dispatcher_HumanCertify() {}
Dispatcher_HumanCertify::~Dispatcher_HumanCertify() {}
int Dispatcher_HumanCertify::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082064d6: state > 2 才放行
    if (user->get_state() <= 2)
    {
        return LineFunc(0xa007, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x04c: M-CAP 点数过高时概率踢线
    unsigned int capPoint = user->m_mcap.getPoint();
    if (capPoint > 0x251b)
    {
        int randVal = get_rand_int(100);
        if (randVal <= 10)
        {
            unsigned int capPoint2 = user->m_mcap.getPoint();
            user->m_historyLog.RequestCleanPad(capPoint2, '\x02');
            user->DisConnSig(static_cast<DISCONN_SIG>(0x35), true, 0);
        }
    }

    // +0x0c5: 读认证号（1..10）
    int certifyNo = 0;
    int certStep = 10;
    unsigned short answer[5];
    answer[0] = 0; answer[1] = 0; answer[2] = 0; answer[3] = 0; answer[4] = 0;
    if (!(packet.get_int(certifyNo)) || certifyNo > 10)
    {
        return LineFunc(0xa023, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x156: 读 10 字节答案
    if (!packet.get_binary(reinterpret_cast<char*>(answer), 10))
    {
        return LineFunc(0xa024, __PRETTY_FUNCTION__, 0, 0);
    }

    char certValid = 1;
    unsigned short verifyData = 0;
    if (!packet.get_short(verifyData))
    {
        return LineFunc(0xa02f, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x21d: 读认证类型
    unsigned char certifyType = 0;
    if (!packet.get_byte(certifyType))
    {
        return LineFunc(0xa032, __PRETTY_FUNCTION__, 0, 0);
    }

    if (certifyType == 2)
    {
        // +0x233: 类型 2 = 已通过 → 落实状态
        user->setHumanCertified(true);
        user->SendMail(false);
        user->resetCleanpadFailCnt();
        user->resetHumanCertifyData();
    }
    else
    {
        // +0x27a: 校验答案
        if (!user->checkHumanCertify(certifyNo, answer))
        {
            // +0x295: 失败 → 记日志、标记无效
            unsigned int capPoint3 = user->m_mcap.getPoint();
            user->m_historyLog.RequestCleanPad(capPoint3, '\x02');
            certValid = 0;
            // +0x2c1: 未达失败上限
            if (!user->isCleanPadVerifyLimit())
            {
                if (certifyType == 1)
                {
                    // 邮件认证重发
                    if (user->reqSendMailCertify() != 1)
                    {
                        user->SendMail(true);
                    }
                }
                else
                {
                    // +0x36c: 地图清除认证
                    user->setHumanCertified(true);
                    user->reqHumanCertify4ClearMap(false);
                }
            }
            else if (certifyType == 1)
            {
                user->SendMail(false);
                user->resetHumanCertifyData();
                user->resetCleanpadFailCnt();
            }
            else
            {
                user->DisConnSig(static_cast<DISCONN_SIG>(0x1f), true, 0);
            }
        }
        else
        {
            // +0x28f: 通过 → 记日志、短时重放检测
            if (certifyType == 1)
            {
                user->SendMail(true);
            }
            user->resetCleanpadFailCnt();
            unsigned int capPoint4 = user->m_mcap.getPoint();
            user->m_historyLog.RequestCleanPad(capPoint4, '\x01');
            unsigned int nowTick = GlobalData::s_systemTime_.getCurTickCount();
            if (static_cast<unsigned int>(nowTick - user->m_mcap.m_capchaReqTick) < 2000)
            {
                WongWork::CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
                hackAnalyzer->addServerHackCnt(user, static_cast<WongWork::ENUM_HACKTYPE>(0x341), 1, 0, 0);
            }
        }
    }

    // +0x44a: 验证数据异常 → hack 计数
    if (user->VerifyCleanPadVeriData(static_cast<unsigned int>(verifyData)))
    {
        WongWork::CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
        hackAnalyzer->addServerHackCnt(user, static_cast<WongWork::ENUM_HACKTYPE>(0x337), 1, 0, 0);
    }

    // +0x48d: 认证有效则清数据
    if (certValid != 0)
    {
        user->resetHumanCertifyData();
    }
    return 0;
}
int Dispatcher_HumanCertify::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_HumanCertify::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_InformNotice::Dispatcher_InformNotice() {}
Dispatcher_InformNotice::~Dispatcher_InformNotice() {}
int Dispatcher_InformNotice::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 3)
    {
        return 1;
    }
    const MSG_INFORM_NOTICE& req = static_cast<const MSG_INFORM_NOTICE&>(msg);
    return (unsigned int)req.m_noticeFlag < 0x80 ? 0 : -1;
}
// ORIG 0x081e2f78（报告 Dispatcher_InformNotice/process.md）：
// err=check_error → param.m_4；>0 返 0；<0 LineFunc(0x598c)；==0 handleInformNoticeFlag。
int Dispatcher_InformNotice::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    InformNoticeData& out = static_cast<InformNoticeData&>(param);
    MSG_INFORM_NOTICE& req = static_cast<MSG_INFORM_NOTICE&>(msg);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x598c,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    user->handleInformNoticeFlag((unsigned int)req.m_noticeFlag);
    return 0;
}
int Dispatcher_InformNotice::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0xd)，失败记 LineFunc(0x597d)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0xd)))
    {
        return LineFunc(0x597d, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_InformNotice::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e3056 T：err=*(int*)((char*)&param+4)；0→SendCmdOkPacket(0x1de)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1de);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1de, (unsigned char)err);
}

Dispatcher_JoinPowerWar::Dispatcher_JoinPowerWar() {}
Dispatcher_JoinPowerWar::~Dispatcher_JoinPowerWar() {}
int Dispatcher_JoinPowerWar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    return LineFunc(0xc8d3,
                    __PRETTY_FUNCTION__,
                    0, 0);
}
int Dispatcher_JoinPowerWar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_JoinPowerWar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_MailBox_Open::Dispatcher_MailBox_Open() {}
Dispatcher_MailBox_Open::~Dispatcher_MailBox_Open() {}
// ORIG 0x081cc2a2（报告 Dispatcher_MailBox_Open/check_error.md）
int Dispatcher_MailBox_Open::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return -1;   // ORIG 0x081cc2e6：返回 -1（走 LineFunc 错误路径）
    }
    return 0;
}
int Dispatcher_MailBox_Open::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cc196
    int err = check_error(user, msg);
    *(int*)((char*)&param + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x22c8,
                        __PRETTY_FUNCTION__,
                        err, user->get_acc_id());
    }

    WongWork::CMailBox* mailBox = (WongWork::CMailBox*)user->GetMailBox();
    if (mailBox->GetNotLoadedMailCount() != 0)
    {
        int recvSize = mailBox->GetRecvSize();
        int loadedCount = mailBox->GetLoadedLetterCount();
        if (recvSize + loadedCount <= 0x13)   // ORIG lea (ebx+eax); cmp $0x13; setle
        {
            WongWork::CMailBoxHelper::ReqDBLoadMail(user,
                                                    (unsigned int)user->getCurCharacNo());
            *(int*)((char*)&param + 4) = 0x16;
            return 0;
        }
    }
    // ORIG：未发 DB 请求时再次调 GetNotLoadedMailCount 写 param+8
    *(int*)((char*)&param + 8) = mailBox->GetNotLoadedMailCount();
    return 0;
}
int Dispatcher_MailBox_Open::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_MailBox_Open::send(CUser* user, ParamBase& param)
{
    int err = *(int*)((char*)&param + 4);
    if (err != 0x7fffffff)
    {
        if (err != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x63, (unsigned char)err);
        }
        else
        {
            PacketGuard guard;
            guard.put_header(1, 0x63);
            guard.put_byte(1);
            guard.put_short(*(int*)((char*)&param + 8));
            guard.finalize(true);
            user->Send(guard);
        }
    }
}



Dispatcher_MemberSecede::Dispatcher_MemberSecede() {}
Dispatcher_MemberSecede::~Dispatcher_MemberSecede() {}
// ORIG 0x081cbbd0（报告 Dispatcher_MemberSecede/check_error.md）：
// state<=2 或 curCharac==0 → -1；get_charac_memberkey()==0 → 0x7fffffff；否则 0。
int Dispatcher_MemberSecede::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        if (user->get_charac_memberkey() == 0)
        {
            return 0x7fffffff;
        }
        return 0;
    }
    return -1;
}
int Dispatcher_MemberSecede::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cbc2e：param+4 = check_error(user,msg)；>0 返 0；<0 LineFunc(0x2228)；
    // ==0 构造 Packet_Monitor_Member_Secede，UID(+7)/characNo(+0xa)，
    // memcpy(pkt+0xf, msg+0x14, *(size_t*)(msg+0x10))，Monitor 代理 SendTcpPacket(pkt, 0x31)。
    int err = check_error(user, msg);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x2228,
                        __PRETTY_FUNCTION__,
                        err, user->get_acc_id());
    }
    Packet_Monitor_Member_Secede pkt;   // ORIG W ctor 0x0822ba34（局部帧 -0x41 起）
    // 帧偏移：-0x3a=pkt+0x7(UID)、-0x37=pkt+0xa(characNo)、-0x32=pkt+0xf(30 字节名区)
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 7) = user->GetUID();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 0xa) = user->getCurCharacNo();
    std::memcpy(reinterpret_cast<char*>(&pkt) + 0xf,
                reinterpret_cast<const char*>(&msg) + 0x14,
                *reinterpret_cast<const size_t*>(reinterpret_cast<const char*>(&msg) + 0x10));
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x31);
    return 0;
}
int Dispatcher_MemberSecede::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cbb26：int@0x10（len）→ str@0x14（max 0x1e）
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x10)))
        return LineFunc(0x2209, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m + 0x14, 0x1e, *reinterpret_cast<int*>(m + 0x10)))
        return LineFunc(0x220c, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_RequestMatch::Dispatcher_RequestMatch() {}
Dispatcher_RequestMatch::~Dispatcher_RequestMatch() {}
// ORIG 0x08217ad0 T _ZN23Dispatcher_RequestMatch12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/Dispatcher_RequestMatch/dispatch_sig.md 汇编+反编译 C 全文转写）
int Dispatcher_RequestMatch::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return 0;
    }
    if (room->m_matching == 0)
    {
        return 0;
    }
    unsigned char flag = 0;
    if (!packet.get_byte(flag))
    {
        return LineFunc(0xc9c4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (flag != 0)
    {
        // ORIG vtable+0x34 槽直调
        (*(int (**)(void*, void*, void*))(*(char**)room->m_matching + 0x34))(
            (void*)room->m_matching, user, room);
        return 0;
    }
    // ORIG vtable+0x30 槽直调
    (*(int (**)(void*, void*, void*))(*(char**)room->m_matching + 0x30))(
        (void*)room->m_matching, user, room);
    return 0;
}
int Dispatcher_RequestMatch::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestMatch::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ScanBotByDll::Dispatcher_ScanBotByDll() {}
Dispatcher_ScanBotByDll::~Dispatcher_ScanBotByDll() {}
int Dispatcher_ScanBotByDll::check_error(CUser* user, MSG_BASE& msg)
{
    const MSG_SCAN_DLL& req = static_cast<const MSG_SCAN_DLL&>(msg);
    const int value = req.m_field0d;  // ORIG check_error @0x8264538 读 msg+0xd int
    // ORIG 0x0826456f：x=value+0x352；x<=0x351→0；x<=0x383→返回 x；否则 0x407
    //（即 value<0→0；0<=value<=0x31→value+0x352；value>0x31→0x407）
    if (value < 0)
    {
        return 0;
    }
    if (value <= 0x31)
    {
        return value + 0x352;
    }
    return 0x407;
}
// ORIG 0x082643b2（报告 Dispatcher_ScanBotByDll/read.md）[随行依赖]。
int Dispatcher_ScanBotByDll::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SCAN_DLL& req = static_cast<MSG_SCAN_DLL&>(msg);
    if (!packet.get_int(req.m_field0d))
    {
        return LineFunc(0x3d6, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(*(char*)(reinterpret_cast<char*>(&req) + 0x11)))
    {
        return LineFunc(0x3db, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_field12))
    {
        return LineFunc(0x3e0, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

// ORIG 0x08264488（报告 Dispatcher_ScanBotByDll/process.md）：
// check_error==0 后 SetETC(type+0x352, (char)msg+0x11)，再 addServerHackCnt。
int Dispatcher_ScanBotByDll::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    if (check_error(user, msg) != 0)
    {
        return 0;
    }
    MSG_SCAN_DLL& req = static_cast<MSG_SCAN_DLL&>(msg);
    const int hackType = req.m_field0d + 0x352;
    const int setEtcResult = user->SetETC((short)hackType, (int)req.m_field11);
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)hackType, 1u,
        (unsigned int)setEtcResult, (unsigned int)req.m_field12);
    return 0;
}

Dispatcher_ScanBotByDrv::Dispatcher_ScanBotByDrv() {}
Dispatcher_ScanBotByDrv::~Dispatcher_ScanBotByDrv() {}
int Dispatcher_ScanBotByDrv::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ScanBotByDrv::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x08264740
    int err = check_error(user, msg);
    if (err != 0)
    {
        return 0;
    }
    MSG_SCAN_DRV& req = static_cast<MSG_SCAN_DRV&>(msg);
    int scanResult = req.m_field0d;
    WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
    analyzer->setBaseHackType((WongWork::ENUM_HACKTYPE)0x708);
    int setEtcRet = user->SetETC((short)(scanResult + 0x708), (int)(char)req.m_field11);
    analyzer = user->getHackAnalyzer();
    analyzer->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)(scanResult + 0x708),
                               1, (unsigned int)setEtcRet, 0);
    return 0;
}
int Dispatcher_ScanBotByDrv::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x082646ae：读 int(msg+0xd) 失败记 LineFunc(0x42c)；
    // 再读 byte(msg+0x11)（signed char）失败记 LineFunc(0x431)。
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(p + 0xd)))
    {
        return LineFunc(0x42c, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(p + 0x11)))
    {
        return LineFunc(0x431, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_Secede_Power::Dispatcher_Secede_Power() {}
Dispatcher_Secede_Power::~Dispatcher_Secede_Power() {}
int Dispatcher_Secede_Power::dispatch_sig(CUser* user, PacketBuf& packet)
{
    return LineFunc(0xa91a,
                    __PRETTY_FUNCTION__,
                    0, 0);
}
int Dispatcher_Secede_Power::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Secede_Power::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SoloTeleport::Dispatcher_SoloTeleport() {}
Dispatcher_SoloTeleport::~Dispatcher_SoloTeleport() {}

Dispatcher_TitleBookGet::Dispatcher_TitleBookGet() {}
Dispatcher_TitleBookGet::~Dispatcher_TitleBookGet() {}
int Dispatcher_TitleBookGet::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char* pMsg = (char*)&msg;

    if (user->get_state() <= 2)
    {
        return 0;
    }

    // getTitle(user, category=+0x19, slot=+0x1d, itemIdx=+0x15, space=+0xd,
    //          outSlot=&+0x11)
    int ret = CTitleBook::getTitle(
        user,
        (ENUM_TITLE_BOOK_CATEGORY)*(int*)(pMsg + 0x19),
        *(int*)(pMsg + 0x1d),
        *(int*)(pMsg + 0x15),
        (ENUM_ITEMSPACE)*(int*)(pMsg + 0x0d),
        *(int*)(pMsg + 0x11));

    if (ret == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x19d);
        packet.put_byte(1);
        packet.put_int(*(int*)(pMsg + 0x0d));
        packet.put_int(*(int*)(pMsg + 0x11));
        packet.put_int(*(int*)(pMsg + 0x19));
        packet.put_int(*(int*)(pMsg + 0x1d));
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x19d);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.put_int(*(int*)(pMsg + 0x0d));
        packet.put_int(*(int*)(pMsg + 0x19));
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}
int Dispatcher_TitleBookGet::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_int(*(int*)(pMsg + 0x0d)))
    {
        return LineFunc(0x420c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x11)))
    {
        return LineFunc(0x420e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x15)))
    {
        return LineFunc(0x4210,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x19)))
    {
        return LineFunc(0x4212,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x1d)))
    {
        return LineFunc(0x4214,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

Dispatcher_TitleBookPut::Dispatcher_TitleBookPut() {}
Dispatcher_TitleBookPut::~Dispatcher_TitleBookPut() {}
int Dispatcher_TitleBookPut::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char* pMsg = (char*)&msg;

    if (user->get_state() <= 2)
    {
        return 0;
    }

    // putTitle(user, space=+0xd, slot=+0x11, itemIdx=+0x15, category=+0x19,
    //          titleSlot=+0x1d)
    int ret = CTitleBook::putTitle(
        user,
        (ENUM_ITEMSPACE)*(int*)(pMsg + 0x0d),
        *(int*)(pMsg + 0x11),
        *(int*)(pMsg + 0x15),
        (ENUM_TITLE_BOOK_CATEGORY)*(int*)(pMsg + 0x19),
        *(int*)(pMsg + 0x1d));

    if (ret == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x19c);
        packet.put_byte(1);
        packet.put_int(*(int*)(pMsg + 0x0d));
        packet.put_int(*(int*)(pMsg + 0x11));
        packet.put_int(*(int*)(pMsg + 0x19));
        packet.put_int(*(int*)(pMsg + 0x1d));
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x19c);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.put_int(*(int*)(pMsg + 0x0d));
        packet.put_int(*(int*)(pMsg + 0x19));
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}
int Dispatcher_TitleBookPut::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_int(*(int*)(pMsg + 0x0d)))
    {
        return LineFunc(0x41d4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x11)))
    {
        return LineFunc(0x41d6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x15)))
    {
        return LineFunc(0x41d8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x19)))
    {
        return LineFunc(0x41da,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x1d)))
    {
        return LineFunc(0x41dc,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

Dispatcher_UpgradeCargo::Dispatcher_UpgradeCargo() {}
Dispatcher_UpgradeCargo::~Dispatcher_UpgradeCargo() {}
int Dispatcher_UpgradeCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0xe7d8,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    const unsigned int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x1d);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, (unsigned char)(secu & 0xff));
        return 0;
    }

    short slot = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(
            0xe7e2,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    // 扩仓券物品校验：槽内数量字段须 > 0
    Inven_Item ticket =
        user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, slot);
    if (ticket.m_addInfo2 < 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x11);
        return 0;
    }

    // 券动作合法性（StackableAction TYPE_UPGRADE_CARGO=0x2b，按 itemIdx 校验）
    if (!user->verifyStackableItemActionPlace(StackableAction::TYPE_UPGRADE_CARGO,
                                              (unsigned int)ticket.m_addInfo))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x11);
        return 0;
    }

    const CCargo* cargoR = user->getCurCharacCargoR();
    if (!cargoR->isUpgradable(cargoR->GetCapacity() + 0x10))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x13);
        return 0;
    }

    if (user->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, slot, 1, eItemDelReason_3, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x11);
        return 0;
    }

    // 包 1（0x0e）：删除券后的物品更新广播
    PacketGuard pg;                                  // try 域 @0822a32b..0822a482
    pg.put_header(0, 0xe);
    pg.put_byte(0);
    pg.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE_EQUIP, slot, pg);
    pg.finalize(true);
    user->Send(pg);

    // 升级仓库容量（+0x10），失败回错误码 0x13
    if (!user->getCurCharacCargoW()->UpgradeCargo(
            user->getCurCharacCargoR()->GetCapacity() + 0x10))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x13);
        return 0;
    }

    // 包 2（0x42）：新容量通知
    pg.clear();
    pg.put_header(0, 0x42);
    pg.put_short(1);
    pg.put_short(user->getCurCharacCargoR()->GetCapacity());
    pg.finalize(true);
    user->Send(pg);
    return 0;
}
int Dispatcher_UpgradeCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseLimitCube::Dispatcher_UseLimitCube() {}
Dispatcher_UseLimitCube::~Dispatcher_UseLimitCube() {}
// process/read/send/check_error 实现于 Dispatcher_UseLimitCube_impl.cpp（ORIG 0x081d3d38 族）。

Dispatcher_AssertManager::Dispatcher_AssertManager() {}
Dispatcher_AssertManager::~Dispatcher_AssertManager() {}
int Dispatcher_AssertManager::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821e1f4：0x214 连续缓冲（Packet_Assert_Manager_Info 头 + 负载数据区）。
    char buffer[0x214];
    Packet_Assert_Manager_Info* header = new (buffer) Packet_Assert_Manager_Info();
    (void)header;
    int& category = *reinterpret_cast<int*>(buffer + 0xa);
    if (!packet.get_int(category)) {
        return LineFunc(0xd275, __PRETTY_FUNCTION__, 0, 0);
    }
    if (category > 0xff) {
        return LineFunc(0xd279, __PRETTY_FUNCTION__, 0, 0);
    }
    char* desc = buffer + 0xe;
    if (!packet.get_binary(desc, category)) {
        return LineFunc(0xd27a, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned short& field10e = *reinterpret_cast<unsigned short*>(buffer + 0x10e);
    if (!packet.get_short(field10e)) {
        return LineFunc(0xd27d, __PRETTY_FUNCTION__, 0, 0);
    }
    int& field110 = *reinterpret_cast<int*>(buffer + 0x110);
    if (!packet.get_int(field110)) {
        return LineFunc(0xd27f, __PRETTY_FUNCTION__, 0, 0);
    }
    if (field110 > 0xff || field110 < 0) {
        return LineFunc(0xd280, __PRETTY_FUNCTION__, 0, 0);
    }
    char* detail = buffer + 0x114;
    if (field110 > 0) {
        if (!packet.get_binary(detail, field110)) {
            return LineFunc(0xd283, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    else {
        memset(detail, 0, 0x100);
    }
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(buffer, 0x214);
    return 0;
}
int Dispatcher_AssertManager::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AssertManager::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CallGuildInfo::Dispatcher_CallGuildInfo() {}
Dispatcher_CallGuildInfo::~Dispatcher_CallGuildInfo() {}
int Dispatcher_CallGuildInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08200448（docs/class_func_reports/Dispatcher_CallGuildInfo/dispatch_sig.md）
    unsigned int charIds[0x5f];               // local_1ac
    unsigned char count = 0;                  // local_21
    int i;                                    // local_20
    bool fail;                                // bVar1

    // +0x00：state<3 或 charac 空 → fail
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        fail = true;
    else
        fail = false;
    if (fail)
        return LineFunc(0x94e7,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x66：读数量
    if (!packet.get_byte(count))
        return LineFunc(0x94ec,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0xaa：count > 0x5f → 拒绝
    if (count >= 0x60)
        return LineFunc(0x94f0,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0xdd：清零 0x5f 个 uint
    memset(charIds, 0, sizeof(charIds));

    // +0xfa：逐个读取；遇到 0 → 发送 0x4d 错误包
    for (i = 0; i < (int)(unsigned int)count; i = i + 1)
    {
        if (!packet.get_int((unsigned int&)charIds[i]))
            return LineFunc(0x94f7,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (charIds[i] == 0)
        {
            PacketGuard guard;                                        // 0858dd4c
            guard.put_header(1, 0x4d);
            guard.put_byte(0);
            guard.put_byte(0x22);
            guard.finalize(true);
            user->Send(guard);
            return 0;                                                 // guard 析构 0858de80
        }
    }

    // +0x21a：委托公会服务器
    {
        unsigned int uid = user->GetUID();                            // 080c8c96
        unsigned int characNo = user->getCurCharacNo();               // 080cbc4e
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendCallGuildInfo(characNo, uid, count, charIds);
    }
    return 0;
}
int Dispatcher_CallGuildInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ChangeEmotion::Dispatcher_ChangeEmotion() {}
Dispatcher_ChangeEmotion::~Dispatcher_ChangeEmotion() {}
// ORIG 0x081d1cec（报告 Dispatcher_ChangeEmotion/check_error.md）
int Dispatcher_ChangeEmotion::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return LineFunc(0x2e77,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x2e7a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_ChangeEmotion::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const int err = check_error(user, msg);                     // ORIG 0x081d1cec（二参版）
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }

    stChangeEmotionParam& out = static_cast<stChangeEmotionParam&>(param);
    MSG_CHANGE_EMOTION& req = static_cast<MSG_CHANGE_EMOTION&>(msg);
    out.m_4 = req.m_emotionIdx;                                 // msg+0x0d short → param+0x04

    try
    {
        if (user->get_state() <= 2)                             // cmp $0x2,setle
        {
            throw static_cast<ENUM_ERROR>(3);
        }
        if (isVaildEmotionValue((short)out.m_4) != 1)           // ORIG 0x081d1ebe（NEEDS_DECL）
        {
            throw static_cast<ENUM_ERROR>(1);
        }
        user->SetCurCharacEmotion((short)out.m_4);              // CUserCharacInfo::SetCurCharacEmotion
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)3, 0, (ENUM_PACKETCLASS)0);  // ORIG 0x08122390
        return 0;
    }
    catch (ENUM_ERROR e)                                        // catch handler @0x081d1c56
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x102, (unsigned char)e);
        return -1;
    }
}

int Dispatcher_ChangeEmotion::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 short(msg+0xd)，失败记 LineFunc(0x2e3d)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<short*>(p + 0xd)))
    {
        return LineFunc(0x2e3d, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_ChangeEmotion::send(CUser* user, ParamBase& param)
{
    if (user != 0)
    {
        PacketGuard guard;
        guard.clear();
        guard.put_header(1, 0x102);
        guard.put_byte(1);
        guard.put_short((int)*(short*)((char*)&param + 4));
        guard.finalize(true);
        user->Send(guard);

        guard.clear();
        guard.put_header(0, 2);
        guard.put_byte(0);
        guard.put_short(1);
        user->make_basic_info((char*)&guard, '\0');
        guard.finalize(true);
        G_GameWorld()->send_all(guard, user);
    }
}


Dispatcher_Clear_Used_QP::Dispatcher_Clear_Used_QP() {}
Dispatcher_Clear_Used_QP::~Dispatcher_Clear_Used_QP() {}
// ORIG 0x081d6746（报告 Dispatcher_Clear_Used_QP/check_error.md）[随行依赖，
// process 语义必需；不在 55 桩清单]
int Dispatcher_Clear_Used_QP::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return -1;                       // 0x081d6752
    }
    if (user->get_state() != 3)
    {
        return 99;                       // 0x081d676e
    }
    return 0;
}

// ORIG 0x081d6786（报告 Dispatcher_Clear_Used_QP/process.md）：
// ParamBase 实际为 STQuestShopParam，+4 = m_4。
int Dispatcher_Clear_Used_QP::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    if (err > 0)
    {
        return -1;                       // 0x081d67ae
    }
    if (err < 0)
    {
        return LineFunc(0x4031,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CQuestShop* shop =
        (CQuestShop*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop == 0)
    {
        static_cast<STQuestShopParam&>(param).m_4 = 1;
        return 0;
    }
    int ret = (int)shop->clearUsedQp(user);
    if (ret != 0)
    {
        static_cast<STQuestShopParam&>(param).m_4 = ret;
    }
    return 0;
}
int Dispatcher_Clear_Used_QP::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Clear_Used_QP::send(CUser* user, ParamBase& param)
{
    int err = *(int*)((char*)&param + 4);
    PacketGuard guard;
    guard.put_header(1, 400);
    if (err == 0)
    {
        guard.put_byte(1);
        guard.put_int(user->getCurCharacInvenR()->get_money());
        guard.finalize(true);
        user->Send(guard);
        user->sendCharacQp();
        user->sendCharacQuestPiece();
    }
    else
    {
        guard.put_byte(0);
        guard.put_short(err);
        guard.finalize(true);
        user->Send(guard);
    }
}


Dispatcher_ConnectP2PPvP::Dispatcher_ConnectP2PPvP() {}
Dispatcher_ConnectP2PPvP::~Dispatcher_ConnectP2PPvP() {}
int Dispatcher_ConnectP2PPvP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 2)
        return LineFunc(0xe016, __PRETTY_FUNCTION__, 0, 0);
    char count = 0;                                  // conn[8]（-0x11）
    if (!packet.get_byte(count))
        return LineFunc(0xe01a, __PRETTY_FUNCTION__, 0, 0);
    if (count > 8 || count < 0)
        return LineFunc(0xe01e, __PRETTY_FUNCTION__, 0, 0);
    char conn[9];                                    // -0x19（conn[0..7] 清零，conn[8]=count）
    conn[0] = 0; conn[1] = 0; conn[2] = 0; conn[3] = 0;
    conn[4] = 0; conn[5] = 0; conn[6] = 0; conn[7] = 0;
    conn[8] = count;
    for (int i = 0; i < count; ++i)                  // -0xc
    {
        char seat = 0;                               // -0x1a
        if (!packet.get_byte(seat))
            return LineFunc(0xe025, __PRETTY_FUNCTION__, 0, 0);
        if (seat > 7 || seat < 0)
            return LineFunc(0xe028, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_byte(conn[(int)seat]))
            return LineFunc(0xe02b, __PRETTY_FUNCTION__, 0, 0);
    }
    PvP_Room* room = (PvP_Room*)user->GetPVPRoom();
    if (room == 0)
        return 0;
    if (room->m_matching == 0)                       // +0x6e4 matching 子对象指针
        return 0;
    // ORIG：对 room->m_matching（+0x6e4）虚表 +0x5c 槽 thiscall：
    //   (match, user, room, conn, count)。与 DisPatcherCore.cpp:10214 / PvP_Room.cpp:456 同款直调口径。
    (*(int (**)(void*, void*, void*, char*, char))(*(char**)room->m_matching + 0x5c))(
        (void*)room->m_matching, user, room, conn, count);
    return 0;
}

int Dispatcher_ConnectP2PPvP::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ConnectP2PPvP::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DenyJoinGuild::Dispatcher_DenyJoinGuild() {}
Dispatcher_DenyJoinGuild::~Dispatcher_DenyJoinGuild() {}
int Dispatcher_DenyJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    unsigned int applicant;
    bool deny;
    char isMaster;
    char isSubMaster;
    int guildkey;
    int uid;

    (void)param;
    if (user->get_state() != 3)
        return 0;
    isMaster = (char)user->IsGuildMaster();
    if (isMaster == '\x01')
        deny = false;
    else
    {
        isSubMaster = (char)user->IsSubGuildMaster();
        if (isSubMaster == '\x01')
            deny = false;
        else
            deny = true;
    }
    if (deny)
        return 0;
    applicant = *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&msg) + 0xd);
    guildkey = user->get_charac_guildkey();
    uid = user->GetUID();
    DB_DenyJoinGuild::makeRequest(uid, (unsigned int)guildkey, applicant);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x160);
    return 0;
}
int Dispatcher_DenyJoinGuild::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0xd)，失败记 LineFunc(0x357f)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0xd)))
    {
        return LineFunc(0x357f, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_EnchantByBead::Dispatcher_EnchantByBead() {}
Dispatcher_EnchantByBead::~Dispatcher_EnchantByBead() {}
int Dispatcher_EnchantByBead::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d24b8：param+4 = check_error(...)；>0 返 0；<0 LineFunc(0x2f61,err,0)；
    // ==0 调 GlobalData::s_ExpertJobMgr->OnEnchantByBead(user, msg+0xd(char),
    // msg+0xe(short), msg+0x10(char), msg+0x12(short))，非 0 结果写回 param+4，恒返 0。
    int err = check_error(user, msg, param);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x2f61,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    const char* m = reinterpret_cast<const char*>(&msg);
    int result = 0;
    // asm：msg+0xd movsbl、msg+0xe movswl、msg+0x10 movsbl、msg+0x12 movswl
    result = GlobalData::s_ExpertJobMgr->OnEnchantByBead(
        user,
        static_cast<int>(static_cast<signed char>(m[0xd])),
        static_cast<int>(*reinterpret_cast<const short*>(m + 0xe)),
        static_cast<int>(static_cast<signed char>(m[0x10])),
        static_cast<int>(*reinterpret_cast<const short*>(m + 0x12)));
    if (result != 0)
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = result;
    }
    return 0;
}
int Dispatcher_EnchantByBead::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d2218：byte@0xd → short@0xe → byte@0x10 → short@0x12，失败逐级 LineFunc
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(m + 0xd)))
        return LineFunc(0x2f1c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<short*>(m + 0xe)))
        return LineFunc(0x2f1d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(m + 0x10)))
        return LineFunc(0x2f1f, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<short*>(m + 0x12)))
        return LineFunc(0x2f20, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_EnchantByBead::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d25b0 T：err=*(int*)((char*)&param+4)；err==0x7fffffff 或 0 → 不发包，
    // 否则 SendCmdErrorPacket(0x113, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0x7fffffff)
        return;
    if (err != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x113, (unsigned char)err);
}

Dispatcher_GenCeraTicket::Dispatcher_GenCeraTicket() {}
Dispatcher_GenCeraTicket::~Dispatcher_GenCeraTicket() {}
int Dispatcher_GenCeraTicket::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() < 3 || user->getCurCharacR() == 0) {
        return LineFunc(0x91f0, __PRETTY_FUNCTION__, 0, 0);
    }
    int randSeed = get_rand_int(0xffff);
    Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x91f5);
    CStreamGuard guard(stream, true);
    guard << 0x49;
    guard << (int)user->GetUID();
    guard << user->get_acc_id();
    guard << randSeed;
    WongWork::CGenUniqueNo uniqueGen;
    char uniqueNo[32];
    guard << uniqueGen.GenUniqueNo(uniqueNo);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;
}
int Dispatcher_GenCeraTicket::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GenCeraTicket::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// ORIG W 0x0821e474：int cnt = 0; return cnt;
// ---- Dispatcher_GiveGiftToNPC 生命周期与分发表（ORIG W 实证）----
// ctor W 0x08596a9c：基类 + vptr；dtor W 0x08241332/0x08241362：vptr + 基类析构。
Dispatcher_GiveGiftToNPC::Dispatcher_GiveGiftToNPC()
{
}
Dispatcher_GiveGiftToNPC::~Dispatcher_GiveGiftToNPC()
{
}
// dispatch_sig T 0x0821e4ac：恒返 0；read/process W 0x08233e52/0x08233e5c：恒返 0。
int Dispatcher_GiveGiftToNPC::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)user;
    (void)packet;
    return 0;
}
int Dispatcher_GiveGiftToNPC::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)user;
    (void)msg;
    (void)param;
    return 0;
}
int Dispatcher_GiveGiftToNPC::read(PacketBuf& packet, MSG_BASE& msg)
{
    (void)packet;
    (void)msg;
    return 0;
}

int Dispatcher_GiveGiftToNPC::_getMaxFavorLevelNPCCnt(CUser const* user) const
{
    int cnt = 0;
    return cnt;
}

// ORIG W 0x0821e486：返回 (unsigned short)(baseFavor + addFavor)。
unsigned short Dispatcher_GiveGiftToNPC::_getResultFavor(
    CUser const* user, CNPCDynamicInfo const* npcInfo,
    stNPCRelationship_t const& relation, CItem const* item,
    unsigned int count, short addFavor, unsigned short baseFavor,
    ENUM_NPC_FAVOR_CHANGE_CONST& changeConst) const
{
    return static_cast<unsigned short>(baseFavor + addFavor);
}

Dispatcher_GuildJoinList::Dispatcher_GuildJoinList() {}
Dispatcher_GuildJoinList::~Dispatcher_GuildJoinList() {}
int Dispatcher_GuildJoinList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d52a6：state==3 且会长/副会长以外 → SendCmdErrorPacket(0x161,0x24)；
    // 否则按渠道取 server group 并发起 DB_GuildJoinList 请求。
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x161, 0x24);
        return 0;
    }

    // ORIG 分支：整合 PvP 基础渠道 → 用户 server group；否则环境 +0x378。
    int serverGroup;
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup =
            *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    }
    else
    {
        serverGroup = (int)user->GetServerGroup();
    }

    // ORIG 调用顺序：get_charac_guildkey(0x0822f46c) → GetUID(0x080c8c96)。
    int guildKey = user->get_charac_guildkey();
    int uid = (int)user->GetUID();
    DB_GuildJoinList::makeRequest(uid, (unsigned int)guildKey,
                                   (ENUM_SERVER_GROUP)serverGroup);
    return 0;
}
int Dispatcher_GuildJoinList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_HatchCreature::Dispatcher_HatchCreature() {}
Dispatcher_HatchCreature::~Dispatcher_HatchCreature() {}
int Dispatcher_HatchCreature::check_error(CUser* user, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;

    if (user->getCurCharacR() == 0)
    {
        return -1;
    }

    int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x14);
    if (secuErr != 0)
    {
        return secuErr;
    }

    short slot = *(short*)(pMsg + 0x0e);
    int invenType = GetInvenTypeFromItemSpace(
        (ENUM_ITEMSPACE)(unsigned char)pMsg[0x0d]);
    if (user->CheckItemLock(invenType, slot))
    {
        return 0xd5;
    }

    return 0;
}
int Dispatcher_HatchCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stHatchCreature& st = *reinterpret_cast<stHatchCreature*>(&param);
    char* pMsg = (char*)&msg;

    int err = check_error(user, msg);
    st.m_4 = err;

    if (err >= 1)
    {
        return 0;
    }

    if (err < 0)
    {
        return LineFunc(0x252c,
                        __PRETTY_FUNCTION__,
                        err, user->get_acc_id());
    }

    short slot = *(short*)(pMsg + 0x0e);
    int invenType = GetInvenTypeFromItemSpace(
        (ENUM_ITEMSPACE)(unsigned char)pMsg[0x0d]);

    const Inven_Item* item =
        user->getCurCharacInvenR()->GetInvenRef(invenType, slot);
    if (item == 0)
    {
        return LineFunc(0x2533,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }

    int addInfo = item->get_add_info();

    CInventory* invW = user->getCurCharacInvenW();
    user_creature::CCreatureMgr* mgr = invW->GetCreatureMgrW();

    // BLOCKED(user_creature::CCreatureMgr::Hatch(int)
    //         _ZN13user_creature12CCreatureMgr5HatchEi @0x08339fa0)
    // CInventory.h 内 user_creature::CCreatureMgr 尚无该声明（见 NEEDS_DECL）。
    mgr->Hatch(addInfo); // 占位直调，待声明落地

    return 0;
}
int Dispatcher_HatchCreature::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_byte(*(unsigned char*)(pMsg + 0x0d)))
    {
        return LineFunc(0x2518,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*(short*)(pMsg + 0x0e)))
    {
        return LineFunc(0x251b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void Dispatcher_HatchCreature::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stHatchCreature& p = static_cast<stHatchCreature&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x69, (unsigned char)p.m_4);
}

Dispatcher_JoinGuildInfo::Dispatcher_JoinGuildInfo() {}
Dispatcher_JoinGuildInfo::~Dispatcher_JoinGuildInfo() {}
// ORIG 0x081d5398（报告 Dispatcher_JoinGuildInfo/process.md）：
// state==3 且 guildkey==0；非综合 PvP 基础渠道时 serverGroup 取
// G_CEnvironment()->m_serverEnvir.m_serverGroup（env+0x378），否则 GetServerGroup()。
int Dispatcher_JoinGuildInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() == 3 && user->get_charac_guildkey() == 0)
    {
        int serverGroup;
        if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
            serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
        }
        else
        {
            serverGroup = (int)user->GetServerGroup();
        }
        unsigned int characNo = (unsigned int)user->getCurCharacNo();
        unsigned int uid = user->GetUID();
        DB_JoinGuildInfo::makeRequest((int)uid, (ENUM_SERVER_GROUP)serverGroup, characNo);
    }
    return 0;
}
int Dispatcher_JoinGuildInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_MouseRegister::Dispatcher_MouseRegister() {}
Dispatcher_MouseRegister::~Dispatcher_MouseRegister() {}
int Dispatcher_MouseRegister::check_error(CUser* user, MSG_BASE& msg)
{
    (void)user;  // ORIG 0x082632a0：恒返 0
    return 0;
}

// ORIG T 0x082632ca：长度 <7 且仅含 'a'/'0'..'9' 时有效（返回 int，无 bool 规范化 P9）。
int Dispatcher_MouseRegister::checkValiedPassword(char const* pwd)
{
    int len = strlen(pwd);
    if (len > 6)
    {
        return 0;
    }
    for (int i = 0; i < len; ++i)
    {
        char c = pwd[i];
        if (c == 'a' || (c > '/' && c <= '9'))
        {
            continue;
        }
        return 0;
    }
    return 1;
}

// ORIG T 0x0826333a：经 CTEA 虚 Decrypt 解密；失败记日志返回 false。
bool Dispatcher_MouseRegister::decryptPassword(char const* in, char* out, unsigned int len)
{
    int ret = GlobalData::s_pcryptTEA_MousePassword_->Decrypt(in, out, (size_t)len);
    if (ret < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Secu.cpp",
            __PRETTY_FUNCTION__,
            0x1b1, "[%s][%d][%s]", "PacketDispatcher_Secu.cpp", 0x1b1, "Decrypt Fail");
    }
    return ret >= 0;
}

// ORIG T 0x082633c6：密保卡 pad 答案复核链（shake 减扰 + checkAnswer + 计数/DB 上报）。
void Dispatcher_MouseRegister::doPassPadReplay(CUser* user, char* answer,
                                               char answerType, bool& ok,
                                               unsigned char& errCode)
{
    ok = false;   // ORIG *param_4=false
    errCode = 0;  // ORIG *param_5='\0'

    for (int i = 0; i <= 3; ++i)   // setle 循环（P51/P2）
    {
        answer[i] -= user->getPad()->GetShakeData(i);
    }

    // xor $1/test/je：checkAnswer()==1 → 成功分支（asm +0x293）。
    if ((int)user->getPad()->checkAnswer(answer) == 1)
    {
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                       (ENUM_SECU_SERVICE_ACTION)1);
        static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
            (XNuclear::ENUM_HADES_SECURITY)1, 0x23u);
        ok = true;   // asm +0x2d9 movb $1

        PacketGuard packet;
        packet.put_header(1, 0xa7);
        packet.put_byte(2);
        Sanicova::CPad* padA = user->getPad();
        packet.put_byte(padA->getAdvideChangePwd() & 0xff);
        packet.finalize(true);
        user->Send(packet);

        Sanicova::CPad* padB = user->getPad();
        if (padB->getFailCnt() > 0)
        {
            DB_PassPadUpdateFailCnt::makeRequest(user->get_acc_id(), 0,
                                                 (char*)user->getWebAddress());
        }
        user->getPad()->setCertified(true);   // ORIG T 0x0826530e（push $1）
        user->getPad()->resetFailCnt();       // ORIG W 0x082652f2
        DB_GoblinPadUpdateReward::makeRequest(user->get_acc_id(), answerType,
                                              false, (char*)user->getWebAddress());
        return;
    }

    // 失败分支（fallthrough）：SecuService(0,0) + incFailCnt 后按新计数分流。
    user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                   (ENUM_SECU_SERVICE_ACTION)0);
    Sanicova::CPad* pad = user->getPad();
    int newFailCnt = pad->incFailCnt();   // ORIG W 0x082652d8
    if (newFailCnt > 2)
    {
        errCode = 0x88;
        static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
            (XNuclear::ENUM_HADES_SECURITY)1, 0x21u);
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x342, 1u, 0u, 0u);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0xa7);
        packet.put_byte(0);
        packet.put_byte(0x87);
        packet.put_byte(pad->getFailCnt());
        packet.finalize(true);
        user->Send(packet);

        Sanicova::CPad* padC = user->getPad();   // P48：第二次独立 getPad()
        if (padC->getFailCnt() == 1)
            static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
                (XNuclear::ENUM_HADES_SECURITY)1, 0x1fu);
        else
            static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
                (XNuclear::ENUM_HADES_SECURITY)1, 0x20u);
    }
    // 公共尾 +0x251：按当前计数上报失败次数。
    DB_PassPadUpdateFailCnt::makeRequest(user->get_acc_id(),
                                         (unsigned int)user->getPad()->getFailCnt(),
                                         (char*)user->getWebAddress());
}

// ==== Dispatcher_MouseRegister::dispatch_sig (ORIG 0x082637fe) ====
// PacketDispatcher_Secu.cpp 密保卡注册链：包类型分发 → 解密双密码 → shake 减扰
// → checkValiedPassword ×2 → 写 SIG_MOUSE_REGISTER 进 DB_Q。
int Dispatcher_MouseRegister::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 ||
        !(user->get_state() == 3 || user->get_state() == 2))
    {
        return LineFunc(0x27c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    unsigned char pkgType = 0;
    if (!packet.get_byte(pkgType))
    {
        return LineFunc(0x282,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    if ((int)user->getPad()->getFailCnt() > 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74, 0x88);
        return 0;
    }
    if ((int)user->getPad()->getCancelCnt() > 10)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74, 0x8b);
        return 0;
    }

    unsigned char decPwd1[4096] = { '\0' };   // ORIG local_108d（gcc 内联清零 P4）
    unsigned char decPwd2[4096] = { '\0' };   // ORIG local_208d
    int decLen1 = 0x1000;                     // ORIG local_30[1]（第一次 Decrypt 出参）
    int decLen2 = 0x1000;                     // ORIG local_30[0]
    unsigned char cryptoType = 0xe;           // ORIG -0x2d
    unsigned char cryptoTypeCopy = 0xe;       // ORIG -0x2e

    // 未认证的 pad 必须先通过激活 + 认证检查（pkgType != 1 时）。
    if (pkgType != 1 && !user->getPad()->isActivate())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74, 1);
        return 0;
    }
    if (pkgType != 1 && !user->getPad()->isCertified())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74, 0x86);
        return 0;
    }

    // pkgType==3：不携带密文，直接上报 DB（重置请求）后返回。
    if (pkgType == 3)
    {
        Stream* stream =
            GlobalData::s_stream_pool->Acquire("PacketDispatcher_Secu.cpp", 0x2ba);
        CStreamGuard guard(stream, true);
        guard << (int)0xa3;
        guard << (int)user->GetUID();
        SIG_MOUSE_REGISTER* sig = guard.GetInBuffer<SIG_MOUSE_REGISTER>();
        sig->m_accId = user->get_acc_id();
        sig->m_pkgType = (unsigned int)pkgType;
        strncpy(sig->m_password1, (char*)decPwd1, 7);
        strncpy(sig->m_password2, (char*)decPwd2, 7);
        strncpy(sig->m_webAddress, user->getWebAddress(), 0x10);
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                       (ENUM_SECU_SERVICE_ACTION)4);
        static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
            (XNuclear::ENUM_HADES_SECURITY)1, 0x26u);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        return 0;
    }

    if (!packet.get_byte(cryptoType))
    {
        return LineFunc(0x2dc,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!(cryptoType != 0 && cryptoType <= 0xd && cryptoType != 5))
    {
        return LineFunc(0x2e3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // 包体：type(-0x89)+enc32(-0x88..)+encCopy(+0x21 起)；cp=ORIG 双槽操作。
    struct MouseRegDecryptBuf   // pack(1)：帧布局即按字节连续偏移
    {
        unsigned char m_pkgType;             // +0x00 (-0x89)
        unsigned char m_encPwd[32];          // +0x01 (-0x88) get_binary 目标 / Decrypt 源 1
        unsigned char m_encPwdCopy[32];      // +0x21 (-0x68) memcpy 副本 / Decrypt 源 2
    };
    MouseRegDecryptBuf bufView;              // 局部视图（取址用）
    memset(&bufView, 0, sizeof(bufView));    // 清 type/copy 区避免残留栈值入帧

    int encLen = 0;      // ORIG -0x38
    int encLenCopy = 0;  // ORIG -0x34
    if (!packet.get_int(encLen))
    {
        return LineFunc(0x2e8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_binary((char*)bufView.m_encPwd, encLen))
    {
        return LineFunc(0x2ea,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    encLenCopy = encLen;
    cryptoTypeCopy = cryptoType;
    memcpy(bufView.m_encPwdCopy, bufView.m_encPwd, encLen);

    Sanicova::CPad* padCipher = user->getPad();   // P48 双臂各自调用
    Cipher* cipher = GlobalData::s_secu_cipher_keystring_manager_
        [(padCipher->GetSelectedCipherIdx() & 0xffff) * 4];
    const bool firstOk = cipher->Decrypt(cryptoType, bufView.m_encPwd, encLen,
                                         decPwd1, &decLen1);
    Sanicova::CPad* padCipher2 = user->getPad();
    Cipher* cipher2 = GlobalData::s_secu_cipher_keystring_manager_
        [(padCipher2->GetSelectedCipherIdx() & 0xffff) * 4];
    const bool secondOk = cipher2->Decrypt(cryptoTypeCopy, bufView.m_encPwdCopy,
                                           encLenCopy, decPwd2, &decLen2);
    if (!firstOk || !secondOk)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74, 1);
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        decPwd1[i] -= user->getPad()->GetShakeData(i);
        decPwd2[i] -= user->getPad()->GetShakeData(i);
    }

    // P48：两次独立 checkValiedPassword 调用，短路逻辑按 ORIG 形态。
    const bool bad = !checkValiedPassword((char*)decPwd1) ||
                     !checkValiedPassword((char*)decPwd2);
    if (bad)
    {
        return LineFunc(0x343,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    Stream* stream =
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Secu.cpp", 0x346);
    CStreamGuard guard(stream, true);
    guard << (int)0xa3;
    guard << (int)user->GetUID();
    SIG_MOUSE_REGISTER* sig = guard.GetInBuffer<SIG_MOUSE_REGISTER>();
    sig->m_accId = user->get_acc_id();
    sig->m_pkgType = (unsigned int)pkgType;
    strncpy(sig->m_password1, (char*)decPwd1, 7);
    strncpy(sig->m_password2, (char*)decPwd2, 7);
    strncpy(sig->m_webAddress, user->getWebAddress(), 0x10);
    if (pkgType == 1)
    {
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                       (ENUM_SECU_SERVICE_ACTION)2);
    }
    else if (pkgType == 2)
    {
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)0,
                                       (ENUM_SECU_SERVICE_ACTION)3);
    }
    if (pkgType == 1)
    {
        static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
            (XNuclear::ENUM_HADES_SECURITY)1, 0x25u);
    }
    else if (pkgType == 2)
    {
        static_cast<XNuclear::CHades*>(user->getHades())->ChangeSecurity(
            (XNuclear::ENUM_HADES_SECURITY)1, 0x24u);
    }
    Sanicova::CPad* padPass = user->getPad();
    memcpy(sig->m_padPassword, padPass->getPassword(), 0x24);
    Sanicova::CPad* padAdv = user->getPad();
    sig->m_advideChangePwd = (char)(unsigned int)padAdv->getAdvideChangePwd();
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;
}

// ==== DB_GoblinPadUpdateReward::makeRequest (ORIG T 0x08424452) ====
// 报告 DB_GoblinPadUpdateReward/makeRequest.md：Acquire("DBThread.cpp",0x5fc3)、
// 头 0x12b/-1、SIG_GOBLIN_UPDATE_REWARD{accId,+4 byte,+5 bool,strncpy(web,0x10)}。
void DB_GoblinPadUpdateReward::makeRequest(unsigned int a, char b, bool c, char* d)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5fc3), true);
    **guard << (int)0x12b;
    **guard << (int)-1;
    SIG_GOBLIN_UPDATE_REWARD* sig = guard->GetInBuffer<SIG_GOBLIN_UPDATE_REWARD>();
    *(unsigned int*)&sig->m_pad[0] = a;
    sig->m_pad[4] = b;
    sig->m_pad[5] = (char)c;
    strncpy(&sig->m_pad[6], d, 0x10);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

Dispatcher_ObjectBringUp::Dispatcher_ObjectBringUp() {}
Dispatcher_ObjectBringUp::~Dispatcher_ObjectBringUp() {}

Dispatcher_Overflow_Info::Dispatcher_Overflow_Info() {}
Dispatcher_Overflow_Info::~Dispatcher_Overflow_Info() {}
// ORIG 0x0821e4b6（报告 Dispatcher_Overflow_Info/dispatch_sig.md）：
// Packet_Overflow_Statistic_Add(0xc43,0xd) = PacketHeader(10B)+0xa uchar+0xb ushort。
int Dispatcher_Overflow_Info::dispatch_sig(CUser* user, PacketBuf& packet)
{
    Packet_Overflow_Statistic_Add pkt;
    unsigned char& isAdd = *(unsigned char*)((char*)&pkt + 0xa);
    unsigned short& cnt = *(unsigned short*)((char*)&pkt + 0xb);
    if (!packet.get_byte(isAdd))
    {
        return LineFunc(0xd52f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(cnt))
    {
        return LineFunc(0xd530,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    ENUM_SERVER_GROUP group = user->GetServerGroup();
    CStatisticServerProxy* proxy =
        (*GlobalData::s_statistic_proxy_mgr).GetServerProxy(group);
    proxy->SendPacket((char*)&pkt, 0xd);
    return 0;
}
int Dispatcher_Overflow_Info::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Overflow_Info::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_QuickJoinRoom::Dispatcher_QuickJoinRoom() {}
Dispatcher_QuickJoinRoom::~Dispatcher_QuickJoinRoom() {}
int Dispatcher_QuickJoinRoom::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820350e [0x0820350e, 0x082037f9]
    unsigned char kind = 0;                                  // -0x19
    if (!packet.get_byte(kind))
    {
        return LineFunc(0x9a32,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->isCompetitionMercenary())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x65, 0x15);
        return 0;
    }
    if (kind == 0)
    {
        PvP_Room* room = G_CGameManager()->FindQuickJoinablePvPRoom(true, user);
        if (room == 0)
        {
            char msgBuf[0x3d];                               // MSG_MAKE_PVP_ROOM（不完整类型，偏移视图）
            memset(msgBuf, 0, 0x3d);
            *reinterpret_cast<unsigned char*>(msgBuf + 0x0d) = 1;
            *reinterpret_cast<unsigned short*>(msgBuf + 0x2c) = 1;
            if (!MakePVPRoom(user, *reinterpret_cast<MSG_MAKE_PVP_ROOM*>(msgBuf),
                             (ENUM_CMDPACKET)0x65, (PVP_BATTLE_MODE)2))
            {
                return 0;
            }
        }
        else
        {
            char pass[9];                                    // -0x22..-0x1a（空串）
            memset(pass, 0, sizeof(pass));
            if (!EnterPVPRoom(user, room, (ENUM_CMDPACKET)0x65, pass))
            {
                return 0;
            }
        }
    }
    else if (kind == 1)
    {
        PacketGuard guard;
        guard.put_header(1, 0x65);
        guard.put_byte(0);
        guard.put_byte(4);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    else if (kind == 2)
    {
        CEventBase* repeat = GlobalData::s_event_manager->GetRepeatEvent(0x5f);
        if (!repeat->IsEventing(0))                          // ORIG 虚调 vptr+0x34 传 NULL
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x65, 8);
            return 0;
        }
        BlueMarble* marble = G_CGameManager()->findJoinableBlueMarble();
        if (marble == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x65, 0xbe);
            return 0;
        }
        int err = marble->enterBlueMarble(user);
        if (0 < err)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x65, (unsigned char)err);
            return 0;
        }
    }
    return 0;
}
int Dispatcher_QuickJoinRoom::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_QuickJoinRoom::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SelectStriker::Dispatcher_SelectStriker() {}
Dispatcher_SelectStriker::~Dispatcher_SelectStriker() {}
int Dispatcher_SelectStriker::check_error(CUser* user, MSG_BASE& msg)
{
    const MSG_SELECT_STRIKER& m = static_cast<const MSG_SELECT_STRIKER&>(msg);
    if (user == 0)
    {
        return 0xffffffff;
    }
    if (user == 0)
    {
        return 0xffffffff;
    }
    if (user->get_state() != 3)
    {
        return 0x7fffffff;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x7fffffff;
    }
    unsigned int characNo = user->get_charac_no(-1);
    int idx = user->get_charac_idx(characNo);
    if (idx == -1)
    {
        return 0x39;
    }
    char sel = (char)m.m_0d;   // movsbl
    if (sel == idx)
    {
        user->setCurCharacTagCharacNo(0);
        user->setCurCharacStrikerSkillIndex(0);
        user->m_characterView.enableSaveCharacView();           // +0x796f4
        user->setTagCharac(0);
        return 0x7fffffff;
    }
    if (sel < 0 || (char)user->m_characList.size() <= sel)
    {
        return 0x39;
    }
    _Charac_info& entry = user->m_characList[sel];              // vector<_Charac_info>::operator[]
    short level = entry.m_level;                                // +0x27
    if (level < G_CDataManager()->m_characLink.m_field0)        // +0x629c
    {
        return 0xe;
    }
    if (!reinterpret_cast<SkillSlot*>(reinterpret_cast<char*>(&entry) + 0x892)->IsLoadStrikerSkill())
    {
        return 0x13;
    }
    char firstGrow = entry.m_field26;                           // +0x26（movsbl）
    char secondGrowRaw = entry.m_field29;                       // +0x29
    short secondGrow = (short)(char)((secondGrowRaw << 4) >> 4);
    unsigned char deadField1239 = entry.m_field1239;            // +0x1239（ORIG 读取后未使用）
    (void)deadField1239;
    unsigned char skillIdx = m.m_0e;
    StrikerScript* script = G_CDataManager()->m_altarConfig.m_strikerScript;   // +0x6390
    if (!script->checkSkillIndex(firstGrow, secondGrow, (int)skillIdx))
    {
        return 0x15;
    }
    return 0;
}
int Dispatcher_SelectStriker::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const MSG_SELECT_STRIKER& m = static_cast<const MSG_SELECT_STRIKER&>(msg);
    int err = check_error(user, msg);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 0x4) = err;   // ParamBase+0x04
    if (err >= 1)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x5b93,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    char sel = (char)m.m_0d;
    _Charac_info& entry = user->m_characList[sel];
    if (entry.m_pad14bc == 0)                                   // +0x14bc（movzbl 判 0）
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire(
            "PacketDispatcher_Impl_1.cpp", 0x5ba6);
        CStreamGuard guard(stream, true);
        **guard << 0x2fe;
        **guard << user->GetUID();
        SIG_REQUEST_TAG_CHARAC_INFO* sig = guard->GetInBuffer<SIG_REQUEST_TAG_CHARAC_INFO>();
        memset(sig, 0, 0x414c);
        *reinterpret_cast<int*>(sig->m_pad) = entry.m_characNo;   // sig+0x00 = entry 首字段
        *reinterpret_cast<char*>(sig->m_pad + 0x4) =
            m.m_0e;
        // ORIG 0x081e402d: s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard)
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 0;
    }
    user->setCurCharacTagCharacNo(entry.m_characNo);
    user->setCurCharacStrikerSkillIndex(
        m.m_0e);
    user->m_characterView.enableSaveCharacView();               // +0x796f4
    user->setTagCharac(&entry);
    CInventory* inven = user->getTagCharacInvenR();
    inven->set_parent(user);
    SkillSlot* skill = user->getTagCharacSkillR();
    skill->set_parent(user);
    user->SendTagCharacInfo();
    return 0;
}
int Dispatcher_SelectStriker::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e3d32：char@0xd → byte@0xe
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(m + 0xd)))
        return LineFunc(0x5b81, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0xe)))
        return LineFunc(0x5b84, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_SelectStriker::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e42ee T：err=*(int*)((char*)&param+4)；0x7fffffff → 静默，
    // 0 → SendCmdOkPacket(0x1e8)，否则 SendCmdErrorPacket(0x1e8,(unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0x7fffffff)
        return;
    if (err == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1e8);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e8, (unsigned char)err);
}

Dispatcher_UseCraneStart::Dispatcher_UseCraneStart() {}
Dispatcher_UseCraneStart::~Dispatcher_UseCraneStart() {}
int Dispatcher_UseCraneStart::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3)
        return 0;
    short slot = -1;                                       // -0x1e
    if (!packet.get_short(slot))
    {
        return LineFunc(0xd0e8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    try
    {
        CraneMinigameManager* mgr = G_CGameManager()->GetCraneMinigameManager();
        int err = mgr->startUseMaterial(user, (int)slot);
        if (err != 0)
            throw static_cast<ENUM_ERROR>(err);            // __cxa_throw 0x0821d3a0
        CInventory* inven = (CInventory*)user->getCurCharacInvenR();   // 0x080da27e（ORIG R 指针调用非 const GetInvenSlot，同 4226 惯例）
        Inven_Item item = inven->GetInvenSlot(1, (int)slot);
        user->setUseCraneStart(true);                      // 0x0868dd18
        PacketGuard guard;                                 // 0x0858dd4c
        guard.put_header(1, 0x1e6);
        guard.put_byte(1);
        guard.put_short((int)slot);
        guard.put_int(item.m_addInfo2);                    // -0x62 = Inven_Item+0x07
        guard.finalize(true);
        user->Send(guard);
        return 0;                                          // guard 析构 0x0858de80
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e6, (unsigned char)e);
        return 0;
    }
}

int Dispatcher_UseCraneStart::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseCraneStart::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseRenameCard::Dispatcher_UseRenameCard() {}
Dispatcher_UseRenameCard::~Dispatcher_UseRenameCard() {}
int Dispatcher_UseRenameCard::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() != 3)
    {
        return 0x13;
    }

    if (user->CheckInTrade())
    {
        return 0x13;
    }

    GameWorld* world = G_GameWorld();
    int channelType = world->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        return 0x13;
    }

    const _Charac_info* charac = user->getCurCharacR();
    if (charac == 0)
    {
        return 0x13;
    }

    if (!user->IsWaitingStateCharac((unsigned int)charac->m_characNo))
    {
        return 0x12;
    }
    return 0;
}
int Dispatcher_UseRenameCard::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUseRenameCard& st = *reinterpret_cast<stUseRenameCard*>(&param);
    char* pMsg = (char*)&msg;

    int err = check_error(user, msg);
    st.m_4 = err;

    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)err);
        return 0;
    }

    Inven_Item item;
    int ret = getItem(*user, 1, *(short*)(pMsg + 0x0e), item);
    if (ret != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)ret);
        return 0;
    }

    CItem* stackItem = G_CDataManager()->find_item(item.m_addInfo); // +0x02 item idx
    if (stackItem == 0)
    {
        return 0;
    }

    if (!stackItem->is_stackable())
    {
        return 0;
    }

    // TYPE_CHARACTER_NAME_CHANGE = 0x33
    if (!static_cast<CStackableItem*>(stackItem)->verifyStackableAction(
            StackableAction::TYPE_CHARACTER_NAME_CHANGE))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)ret);
        return 0;
    }

    user->SetCharacViewState(0xff);

    CInventory* invW = user->getCurCharacInvenW();
    int delRet = invW->delete_item((INVEN_TYPE)1, *(short*)(pMsg + 0x0e), 1,
                                   (eItemDelReason)3, true);
    if (delRet == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1fe);
        user->ReturnToSelectCharacList(true);
        user->UpdateCharacView();
        return 0;
    }

    user->SetCharacViewState(0);
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, 0x11);
    return LineFunc(0x632e,
                    __PRETTY_FUNCTION__,
                    0, user->get_acc_id());
}
int Dispatcher_UseRenameCard::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_short(*(short*)(pMsg + 0x0e)))
    {
        return LineFunc(0x62f3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

int Dispatcher_UseRenameCard::getItem(const CUser& user, int invenType, short slot,
                                      Inven_Item& out) const
{
    if (user.CheckItemLock(invenType, slot))
    {
        return 1;
    }

    Inven_Item slotItem = user.getCurCharacInvenR()->GetInvenSlot(invenType, slot);

    out = slotItem; // ORIG：0x3d 字节逐字段复制

    if (*(int*)((char*)&out + 0x02) == 0) // m_addInfo(item idx)==0 → 视为空槽失败
    {
        return 1;
    }

    return 0;
}

Dispatcher_WithdrawMoney::Dispatcher_WithdrawMoney() {}
Dispatcher_WithdrawMoney::~Dispatcher_WithdrawMoney() {}
int Dispatcher_WithdrawMoney::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    unsigned int money = 0;
    if (!packet.get_int(money))
    {
        return LineFunc(0xdfda,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!user->IsExistAccountCargo())
    {
        return 0;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    if (cargo->WithdrawMoney(money))
    {
        Inven_Item item;
        item.m_addInfo = 0;
        user->m_historyLog.AccountCargoDelItem(
            item.m_addInfo, (int)cargo->GetMoney(), (int)money,
            (eItemDelReason)0x26);
    }
    return 0;
}
int Dispatcher_WithdrawMoney::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_WithdrawMoney::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionBidding::Dispatcher_AuctionBidding() {}
Dispatcher_AuctionBidding::~Dispatcher_AuctionBidding() {}
int Dispatcher_AuctionBidding::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821522e..0x082159fb (0x7ce 字节)。GM 表 _ZL6gmList @0x093abb4c。
    // 包布局（ServerLab/Auction auction_header）：
    //   PCK_AUCTION_BIDDING_GA: uid@0x12, characNo@0x16, name[13]@0x1a, price(int)@0x27, auctionId[8]@0x2b
    //   PCK_AUCTION_ASK_OWNER_IS_VIP_GP: uid@0x12, auctionId[8]@0x16, characNo@0x1e, name[13]@0x22, price(uint)@0x2f, extra(int)@0x33
    static const char* const kPretty =
        "virtual int Dispatcher_AuctionBidding::dispatch_sig(CUser*, PacketBuf&)";

    unsigned char payType = 0;
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc554, kPretty, 0, 0);
    if (payType > 1)   // cmp $0x1 / jbe
        return LineFunc(0xc557, kPretty, 0, 0);

    const int pay = payType;   // movzbl
    if (pay == 1 && user->get_charac_level() < 0x14)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x74, (PAY_TYPE)pay);
        return 0;
    }

    const int preCheck = user->AuctionPreCheck((ENUM_CMDPACKET)0xbc, (PAY_TYPE)pay);
    if (preCheck >= 0)
        return preCheck;

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 5))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0xd1);
        return 0;
    }

    const unsigned int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x34);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, (unsigned char)(secuErr & 0xff),
                                 (PAY_TYPE)pay);
        return 0;
    }

    if (pay == 1)
    {
        unsigned int price = 0;   // -0x44
        if (packet.get_int(price) != 1)
            return LineFunc(0xc5a9, kPretty, 0, 0);

        if ((unsigned int)user->GetCera() < price)   // asm setb（无符号比较）
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x90, (PAY_TYPE)pay);
            return 0;
        }

        if (G_CPrePayInfoMgr()->FindPrePayInfo(user->getCurCharacNo()) != 0
            || user->GetBuyingGold())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x9e, (PAY_TYPE)pay);
            return 0;
        }

        char auctionId[8];   // -0x74..-0x6d（get_binary 目标 = -0x78+4）
        if (packet.get_binary(auctionId, 8) != 1)
            return LineFunc(0xc5bb, kPretty, 0, 0);
        const unsigned int priceCopy = price;   // ORIG mov -0x44 → -0x78

        int extra = 0;   // -0x48
        if (packet.get_int(extra) != 1)
            return LineFunc(0xc5c1, kPretty, 0, 0);

        // PCK_AUCTION_ASK_OWNER_IS_VIP_GP ctor ORIG W 0x0822ebd8
        PCK_AUCTION_ASK_OWNER_IS_VIP_GP pck;
        char* const body = reinterpret_cast<char*>(&pck);
        *reinterpret_cast<unsigned int*>(body + 0x12) = user->GetUID();
        memcpy(body + 0x16, auctionId, 8);
        *reinterpret_cast<int*>(body + 0x1e) = user->getCurCharacNo();
        strncpy(body + 0x22, user->getCurCharacName(), 0xd);
        *reinterpret_cast<unsigned int*>(body + 0x2f) = price;
        *reinterpret_cast<int*>(body + 0x33) = extra;
        GlobalData::s_cera_auction_proxy->SendPacket(
            reinterpret_cast<nsl::PACKET_HEADER*>(&pck));

        user->SetBuyingGold(true);
        (void)priceCopy;
        return 0;
    }

    static const int gmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

    // ---- gold 分支：PCK_AUCTION_BIDDING_GA ctor ORIG W 0x0822e62c ----
    PCK_AUCTION_BIDDING_GA pck;
    char* const body = reinterpret_cast<char*>(&pck);
    *reinterpret_cast<unsigned int*>(body + 0x12) = user->GetUID();
    *reinterpret_cast<int*>(body + 0x16) = user->getCurCharacNo();
    strncpy(body + 0x1a, user->getCurCharacName(), 0xd);

    int price = 0;   // 读入包 +0x27
    if (packet.get_int(price) != 1)
        return LineFunc(0xc642, kPretty, 0, 0);
    *reinterpret_cast<int*>(body + 0x27) = price;

    char auctionId[8];   // 包 +0x2b（get_binary 直接写包内）
    if (packet.get_binary(body + 0x2b, 8) != 1)
        return LineFunc(0xc643, kPretty, 0, 0);

    char buyerName[14];   // -0x42，13B 有效 + pad，读取前清零
    memset(buyerName, 0, sizeof(buyerName));
    if (packet.get_binary(buyerName, 0xd) != 1)
        return LineFunc(0xc648, kPretty, 0, 0);

    if (user->getCurCharacMoney() < price)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x90, (PAY_TYPE)pay);
        return 0;
    }

    if (!user->getCurCharacInvenW()->use_money(price, (eMoneySubReason)0x1a, true))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x93, (PAY_TYPE)pay);
        return 0;
    }

    user->SaveMoneyCoin();
    GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));

    if (4999 < price)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp",
                                                            0xc678);
        CStreamGuard guard(stream, true);
        **guard << 0x309;
        **guard << (int)user->GetUID();
        SIG_AUCTION_MONEY_SUB* sig = guard->GetInBuffer<SIG_AUCTION_MONEY_SUB>();
        memset(sig, 0, 0x28);
        sig->m_arg04 = price;
        strncpy(sig->m_name, buyerName, 0xd);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }

    for (int i = 0; i <= 4; i++)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(kPretty, 0xc686, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionBidding", user->getCurCharacNo());
            break;
        }
    }
    return 0;
}
int Dispatcher_AuctionBidding::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionBidding::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BuyAutomatItem::Dispatcher_BuyAutomatItem() {}
Dispatcher_BuyAutomatItem::~Dispatcher_BuyAutomatItem() {}
int Dispatcher_BuyAutomatItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081fe578（PacketDispatcher_Impl_1.cpp 0x9145 区）
    if (user == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x9145, 0);
        trace("pUser doesn't exist in Dispatcher_BuyAutomatItem::dispatch_sig");
        return LineFunc(0x9146,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // get_state()<=2 或 getCurCharacR()==0 → 拒绝
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x914a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->CheckInTrade())
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x13, 0, 0, 1, 1, '\0');
        return 0;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x44);
    if (secu != 0)
    {
        int err = (secu == 0x7b) ? 0x76 : 0x86;
        GlobalData::s_pCeraShop->ProcessError(user, (unsigned short)err, 0, 0, 1, 1, '\0');
        return 0;
    }
    // 局部 MSG_BUY_CERASHOP_ITEM（0x3f84），+0x36 计数、条目步长 0x1cf：
    //   +0x37 槽位 uchar / +0x38 标签 char / +0x39 productNo uint
    MSG_BUY_CERASHOP_ITEM buy;
    char* msg = buy.m_pad;
    packet.get_byte(*(unsigned char*)(msg + 0x36));
    unsigned char count = *(unsigned char*)(msg + 0x36);
    if (count == 0 || count == 0 || 0x23 < count)
    {
        GlobalData::s_pCeraShop->ProcessError(user, 1, 0,
                                              *(unsigned int*)(msg + 0x39), 1, 1, '\0');
        return 0;
    }
    PurchaseLimitItem::Info info;   // ORIG 0x081a405e 清零构造（-0x30 区）
    for (int i = 0; i < (int)(unsigned int)count; ++i)
    {
        if (!packet.get_byte(*(unsigned char*)(msg + 0x37 + i * 0x1cf)))
        {
            return LineFunc(0x9172,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(*(char*)(msg + 0x38 + i * 0x1cf)))
        {
            return LineFunc(0x9175,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(*(unsigned int*)(msg + 0x39 + i * 0x1cf)))
        {
            return LineFunc(0x9178,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (info.reserved == 0)
        {
            ARAD::CheckAccountItem(user, *(unsigned int*)(msg + 0x39 + i * 0x1cf), info);
        }
    }
    if (info.reserved == 0)
    {
        if (!GlobalData::s_pItemVendingMachine->BuyItem(user, buy))
        {
            return LineFunc(0x9183,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        return 0;
    }
    // 异步分支：经 DB_Q 流转发给后台购买处理
    Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x9187);
    CStreamGuard guard(stream, true);
    **guard << 0x1fa;                                  // << int
    **guard << (int)user->GetUID();                    // asm 调 << int
    **guard << user->get_acc_id();                     // << unsigned int
    **guard << user->get_charac_no(-1);                // << int
    **guard << (unsigned int)info.reserved;            // << unsigned int
    **guard << (short)info.purchaseType;               // << short
    **guard << (short)info.limitCount;                 // << short
    MSG_BUY_CERASHOP_ITEM* out = guard.GetInBuffer<MSG_BUY_CERASHOP_ITEM>();
    memcpy(out, &buy, 0x3f84);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}
int Dispatcher_BuyAutomatItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BuyAutomatItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CallMemberList::Dispatcher_CallMemberList() {}
Dispatcher_CallMemberList::~Dispatcher_CallMemberList() {}
int Dispatcher_CallMemberList::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81cbd44：当前角色及 memberkey 必须有效（ORIG 无 user 判空）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    return user->get_charac_memberkey() == 0 ? 0x7fffffff : 0;
}
int Dispatcher_CallMemberList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cbda2：check_error 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x225b)；err==0 → 向 monitor 请求成员列表。
    int err = check_error(user, msg);
    *(int*)(reinterpret_cast<char*>(&param) + 0x4) = err;

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LineFunc(0x225b,
                 __PRETTY_FUNCTION__,
                 err, (unsigned int)user->get_acc_id());
        return 0;
    }

    // ORIG 0x0822ba76 ctor；pkt+0x0a = GetUID(0x080c8c96)；
    // pkt+0x0e = getCurCharacNo(0x080cbc4e)；SendPacket(pkt, 0x12)。
    Packet_Monitor_Call_Member_List pkt;
    *(unsigned int*)(reinterpret_cast<char*>(&pkt) + 0x0a) = user->GetUID();
    *(int*)(reinterpret_cast<char*>(&pkt) + 0x0e) = user->getCurCharacNo();

    ENUM_SERVER_GROUP group = user->GetServerGroup();
    CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x12);
    return 0;
}
int Dispatcher_CallMemberList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CheckJoinGuild::Dispatcher_CheckJoinGuild() {}
Dispatcher_CheckJoinGuild::~Dispatcher_CheckJoinGuild() {}
int Dispatcher_CheckJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char isPvP;
    int guildkey;
    GameWorld* world;
    SIG_CHECK_JOIN_GUILD sig;
    int serverGroup;
    int ret;

    (void)param;
    ret = user->get_state();
    if (ret != 3)
        return 0;
    guildkey = user->get_charac_guildkey();
    if (guildkey != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15c, (unsigned char)0x12);
        return 0;
    }
    memset(&sig, 0, 0x1c);
    world = G_GameWorld();
    isPvP = (char)world->IsIntegratedPvPBaseChannel();
    if (isPvP == '\0')
    {
        serverGroup = *(int*)(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    }
    else
    {
        serverGroup = user->GetServerGroup();
    }
    strncpy(reinterpret_cast<char*>(&sig),
            reinterpret_cast<char*>(&msg) + 0x11, 0x16);
    DB_CheckJoinGuild::makeRequest(user->GetUID(), &sig);
    return 0;
}
int Dispatcher_CheckJoinGuild::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d4b4c：int@0xd（len）→ str@0x11（max 0x17）
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0xd)))
        return LineFunc(0x34db, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m + 0x11, 0x17, *reinterpret_cast<int*>(m + 0xd)))
        return LineFunc(0x34de, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_CompoundEmblem::Dispatcher_CompoundEmblem() {}
Dispatcher_CompoundEmblem::~Dispatcher_CompoundEmblem() {}


Dispatcher_DisJointAvatar::Dispatcher_DisJointAvatar() {}
Dispatcher_DisJointAvatar::~Dispatcher_DisJointAvatar() {}
int Dispatcher_DisJointAvatar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08217c06: state 门
    if (user->get_state() != 3)
        return 0;

    // +0x038: isEnableAvatarSocketAction 门
    int avatarSocketAction = user->isEnableAvatarSocketAction();
    if (avatarSocketAction != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xcd),
                                 static_cast<unsigned char>(avatarSocketAction));
        return 0;
    }

    // +0x065: ServiceRestrictManager::isRestricted(user,1,0x14) 门
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, static_cast<RestrictCategory::Enum>(1), 0x14))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xcd), 0xd1);
        return 0;
    }

    // +0x0b2: local_3e=ushort slot, local_44=ulong itemIdx
    unsigned short slot = 0;
    unsigned long itemIdx = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(0xcaa7, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_item_idx(itemIdx))
    {
        return LineFunc(0xcaa8, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x147: CheckItemLock(2, slot)
    if (user->CheckItemLock(2, static_cast<int>(slot)))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xcd), 0xd5);
        return 0;
    }

    // +0x18e: Inven_Item item; CItem* pItem=0; vector<pair<int,int>> results
    Inven_Item item;
    CItem* pItem = static_cast<CItem*>(0);
    std::vector<std::pair<int, int> > results;

    // +0x1ae: GetInvenSlot(2, slot) → item
    item = user->getCurCharacInvenR()->GetInvenSlot(2, static_cast<int>(slot));

    // +0x29d: item.m_addInfo(+0x02) == itemIdx
    if (item.m_addInfo != static_cast<int>(itemIdx))
    {
        throw static_cast<ENUM_ERROR>(0x11);
    }

    // +0x2d1: G_CDataManager()->find_item(item.m_addInfo)
    pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem == static_cast<CItem*>(0))
    {
        throw static_cast<ENUM_ERROR>(0x11);
    }

    // +0x322: get_grade()==0 || get_grade()>3 → error 0x13
    {
        int grade = pItem->get_grade();
        if (grade == 0 || grade > 3)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
    }

    // +0x37a: 虚调用 vtbl+0x24 = isEnableAvatarFunc(2)
    if (!(pItem->isEnableAvatarFunc(static_cast<ENUM_AVATAR_FUNC_FILTER>(2))))
    {
        throw static_cast<ENUM_ERROR>(0x17);
    }

    // +0x3bf: GetAvatarItemMgrR()->GetExpireDate(item.m_addInfo2) != 0 → error 0x17
    if (user->getCurCharacInvenW()->GetAvatarItemMgrR()->GetExpireDate(item.m_addInfo2) != 0)
    {
        throw static_cast<ENUM_ERROR>(0x17);
    }

    // +0x419: item.m_upgradeSep(+0x33).IsTradeRestriction() → error 0x17
    if (item.m_upgradeSep.IsTradeRestriction())
    {
        throw static_cast<ENUM_ERROR>(0x17);
    }

    // +0x45f: getJewelSocketData(item.m_addInfo2) → _selectJewelTypeIndex
    stAvatarEmblemInfo_t* jewelSocketData =
        reinterpret_cast<stAvatarEmblemInfo_t*>(
            const_cast<char*>(user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(item.m_addInfo2)));
    int jewelTypeIndex = _selectJewelTypeIndex(jewelSocketData);

    // +0x49c: pItem+0x234 == 9 → jewelTypeIndex = -1
    if (*reinterpret_cast<const int*>(reinterpret_cast<const char*>(pItem) + 0x234) == 9)
    {
        jewelTypeIndex = -1;
    }

    // +0x4d3: _getResultItems(user, pItem, jewelTypeIndex, results)
    int getResultError = _getResultItems(user, pItem, jewelTypeIndex, results);
    if (getResultError != 0)
    {
        throw static_cast<ENUM_ERROR>(getResultError);
    }

    // +0x50c: results.size(); check_empty_count(ITEM_TYPE 9, size)
    size_t resultCount = results.size();
    if (!(user->getCurCharacInvenR()->check_empty_count(
              static_cast<Inven_Item::ITEM_TYPE>(9), static_cast<int>(resultCount))))
    {
        throw static_cast<ENUM_ERROR>(4);
    }

    // +0x585: delete_item(INVEN 2, slot, 1, eItemDelReason 6, true)
    if (!(user->getCurCharacInvenW()->delete_item(
              static_cast<INVEN_TYPE>(2), static_cast<int>(slot), 1,
              static_cast<eItemDelReason>(6), true)))
    {
        throw static_cast<ENUM_ERROR>(0x16);
    }

    // +0x5fc: GetAvatarItemMgrW()->UnRegistItem(item.m_addInfo2)
    if (!(user->getCurCharacInvenW()->GetAvatarItemMgrW()->UnRegistItem(item.m_addInfo2)))
    {
        throw static_cast<ENUM_ERROR>(0x16);
    }

    // +0x6ca: SendAvatarEvent(0x36, uid, 5, uid+10, 0)
    user->getCurCharacInvenW()->SendAvatarEvent(
        0x36, item.m_addInfo2, 5, item.m_addInfo2 + 10, 0);

    // +0x6f4: DB_UpdateAvatarHistory::makeRequest(characNo, uid, "")
    DB_UpdateAvatarHistory::makeRequest(
        static_cast<unsigned int>(user->getCurCharacNo()), item.m_addInfo2, const_cast<char*>(""));

    // +0x70a: vector<int> updates; insert_event_items(results, updates, 0, 10, 0, "", "")
    std::vector<int> updates;
    int insertResult = user->getCurCharacInvenW()->insert_event_items(
        results, updates, static_cast<eMoneyAddReason>(0), static_cast<eItemAddReason>(10),
        false, "", "");

    // +0x751: PacketGuard pg; put_header(1,0xcd); put_byte(1); put_short(slot); put_short(insertResult)
    PacketGuard pg;
    pg.put_header(1, 0xcd);
    pg.put_byte(1);
    pg.put_short(static_cast<int>(slot));
    pg.put_short(insertResult);

    // +0x7d8: for (it = updates.begin(); it != updates.end();)
    //          { put_short(*it++); put_item_idx(*it++); put_int(*it++); }
    for (std::vector<int>::const_iterator it = updates.begin(); it != updates.end();)
    {
        pg.put_short(*it);
        ++it;
        pg.put_item_idx(static_cast<unsigned long>(*it));
        ++it;
        pg.put_int(*it);
        ++it;
    }

    // +0x8b2: finalize(true); Send(pg)
    pg.finalize(true);
    user->Send(pg);

    // +0x8e4/+0x8f6: SendDisjointAvatarInfo(pItem); SendCreateEmblemInfo(results)
    SendDisjointAvatarInfo(pItem);
    SendCreateEmblemInfo(results);

    return 0;
}
int Dispatcher_DisJointAvatar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DisJointAvatar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DisguiseCancel::Dispatcher_DisguiseCancel() {}
Dispatcher_DisguiseCancel::~Dispatcher_DisguiseCancel() {}
int Dispatcher_DisguiseCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() != 3)
    {
        return 0;
    }
    if (user->isDisguiseCharac() != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd7, 3);
        // ORIG also executes the setter again after this branch.
        user->setDisguiseCharac(false, 0, 0);
    }
    user->setDisguiseCharac(false, 0, 0);
    PacketGuard notification;
    notification.put_header(0, 2);
    notification.put_byte(0);
    notification.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
    notification.finalize(true);
    G_GameWorld()->send_all(notification, user);
    return 0;
}
int Dispatcher_DisguiseCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DisguiseCancel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_HellPartyStart::Dispatcher_HellPartyStart() {}
Dispatcher_HellPartyStart::~Dispatcher_HellPartyStart() {}
int Dispatcher_HellPartyStart::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x821d97a：无 user 判空，state!=5 时返回 LineFunc 结果。
    if (user->get_state() != 5)
    {
        return LineFunc(0xd1bd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_HellPartyStart::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_HellPartyStart::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ItemDictionary::Dispatcher_ItemDictionary() {}
Dispatcher_ItemDictionary::~Dispatcher_ItemDictionary() {}
int Dispatcher_ItemDictionary::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    return user->get_state() < 3 ? 1 : 0;
}
// ORIG 0x081dd7fe（报告 Dispatcher_ItemDictionary/process.md）：
// err>0 直接 0；err<0 记 LineFunc(0x4e52)；否则 0。
int Dispatcher_ItemDictionary::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    *(int*)((char*)&param + 4) = err;
    if (err < 1)
    {
        if (err < 0)
        {
            LineFunc(0x4e52,
                     __PRETTY_FUNCTION__,
                     err, 0);
        }
    }
    return 0;
}
int Dispatcher_ItemDictionary::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ItemDictionary::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081dd8b6 T：&param 存局部，两次读 +0x04（0→OkPacket，否则 ErrorPacket）。
    stItemDictionary& p = static_cast<stItemDictionary&>(param);
    if (p.m_4 == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1b8);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b8, (unsigned char)p.m_4);
}

Dispatcher_Lag_Statistics::Dispatcher_Lag_Statistics() {}
Dispatcher_Lag_Statistics::~Dispatcher_Lag_Statistics() {}
int Dispatcher_Lag_Statistics::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08223de0 [0x08223de0, 0x08224499]
    Packet_Stat_Lag_Statistics local_161;                    // ctor ORIG 0x0822c144（弱内联）
    unsigned char* base = reinterpret_cast<unsigned char*>(&local_161);
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
int Dispatcher_Lag_Statistics::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Lag_Statistics::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Mercenary_Info::Dispatcher_Mercenary_Info() {}
Dispatcher_Mercenary_Info::~Dispatcher_Mercenary_Info() {}
int Dispatcher_Mercenary_Info::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ddb46 (53 insn): !user -> -1; user->get_state() <= ch_state1 -> -1; else 0
    if (user == 0)
        return -1;
    if (user->get_state() <= static_cast<int>(ch_state1))
        return -1;
    return 0;
}

// ORIG 0x081ddb3c (9 insn): 空实现，无消息体
int Dispatcher_Mercenary_Info::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }  // ORIG empty

void Dispatcher_Mercenary_Info::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081ddb7c (0xa7 insn): put_header(1,0x1ba) + put_byte(1) +
    // SetMercenaryInfoPacket + finalize(true) + Send
    PacketGuard guard;
    guard.put_header(1, 0x1ba);
    guard.put_byte(1);
    user->SetMercenaryInfoPacket(guard);
    guard.finalize(true);
    user->Send(guard);
}

Dispatcher_P2P_Statistics::Dispatcher_P2P_Statistics() {}
Dispatcher_P2P_Statistics::~Dispatcher_P2P_Statistics() {}
int Dispatcher_P2P_Statistics::check_error(CUser* user, MSG_BASE& msg)
{
    return LineFunc(0x5970,
                    __PRETTY_FUNCTION__,
                    0, 0);
}

// ---- GuildCargo Pop/Move 与 P2P_Statistics 包视图（StubS3A；ORIG 包字段实证，
//      与 shared 权威包头布局一致，仅类型名不同；待权威头转发后可删） ----
class GuildCargoPopItem_PacketView : public PacketHeader
{
public:
    unsigned int m_guildKey;   // +0x0a
    unsigned int m_charNo;     // +0x0e
    int m_slot;                // +0x12
    int m_id;                  // +0x16
    int m_count;               // +0x1a
    unsigned char m_itemType;  // +0x1e（is_stackable → 0/1）
    unsigned char m_uit;       // +0x1f（msg+0x17 byte）
    GuildCargoPopItem_PacketView() : PacketHeader(0x70e, 0x23),
        m_guildKey(0), m_charNo(0), m_slot(0), m_id(0), m_count(0),
        m_itemType(0), m_uit(0) {}
} __attribute__((packed));

class GuildCargoMoveItem_PacketView : public PacketHeader
{
public:
    unsigned int m_guildKey;   // +0x0a
    unsigned int m_charNo;     // +0x0e
    int m_fromSlot;            // +0x12
    int m_toSlot;              // +0x16
    int m_count;               // +0x1a
    int m_type;                // +0x1e
    GuildCargoMoveItem_PacketView() : PacketHeader(0x70f, 0x22),
        m_guildKey(0), m_charNo(0), m_fromSlot(0), m_toSlot(0),
        m_count(0), m_type(0) {}
} __attribute__((packed));

// ===== Dispatcher_GuildCargoPopItem::process ===== ORIG 0x081d15b2 (0x195)
// 报告 Dispatcher_GuildCargoPopItem/process.md 全文转写。
int Dispatcher_P2P_Statistics::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;                                                // ORIG 不读写 param
    MSG_P2P_STATISTICS& req = static_cast<MSG_P2P_STATISTICS&>(msg);

    Packet_P2P_Statistics pkt;
    pkt.m_p2pUser = (int)(signed char)req.m_0d - (int)(signed char)req.m_0e;  // +0x0a
    pkt.m_relayUser = (int)(signed char)req.m_0e;               // +0x0e
    pkt.m_serverGroup = (unsigned char)user->GetServerGroup();  // +0x12（GetServerGroup @0x080cbc90）
    pkt.m_p2pMinPing = req.m_10;                                // +0x13 ← msg+0x10
    pkt.m_p2pMaxPing = req.m_12;                                // +0x15 ← msg+0x12
    pkt.m_p2pAvgPing = req.m_14;                                // +0x17 ← msg+0x14
    pkt.m_p2pOverPing100 = req.m_18;                            // +0x19 ← msg+0x18
    pkt.m_p2pOverPing200 = req.m_1c;                            // +0x1d ← msg+0x1c
    pkt.m_p2pOverPing300 = req.m_20;                            // +0x21 ← msg+0x20
    pkt.m_p2pOverPing400 = req.m_24;                            // +0x25 ← msg+0x24
    pkt.m_relayMinPing = req.m_28;                              // +0x29 ← msg+0x28
    pkt.m_relayMaxPing = req.m_2a;                              // +0x2b ← msg+0x2a
    pkt.m_relayAvgPing = req.m_2c;                              // +0x2d ← msg+0x2c
    pkt.m_relayOverPing100 = req.m_30;                          // +0x2f ← msg+0x30
    pkt.m_relayOverPing200 = req.m_34;                          // +0x33 ← msg+0x34
    pkt.m_relayOverPing300 = req.m_38;                          // +0x37 ← msg+0x38
    pkt.m_relayOverPing400 = req.m_3c;                          // +0x3b ← msg+0x3c

    cMyTrace trace1(__PRETTY_FUNCTION__,
                    0x5953, 0);
    trace1("p2p statistics");

    cMyTrace trace2(__PRETTY_FUNCTION__,
                    0x5954, 0);
    trace2("num(%d) min(%d) max(%d), avg(%d), 100(%u), 200(%u), 300(%u), 400(%u)",
           (int)pkt.m_p2pUser,                                  // num=diff（ORIG ebx=-0x91）
           (int)pkt.m_p2pMinPing,                               // min（movswl 符号扩展）
           (int)pkt.m_p2pMaxPing,                               // max
           (int)pkt.m_p2pAvgPing,                               // avg
           (unsigned int)pkt.m_p2pOverPing100, (unsigned int)pkt.m_p2pOverPing200,
           (unsigned int)pkt.m_p2pOverPing300, (unsigned int)pkt.m_p2pOverPing400);

    cMyTrace trace3(__PRETTY_FUNCTION__,
                    0x595e, 0);
    trace3("relay statistics");

    cMyTrace trace4(__PRETTY_FUNCTION__,
                    0x595f, 0);
    trace4("num(%d) min(%d) max(%d), avg(%d), 100(%u), 200(%u), 300(%u), 400(%u)",
           (int)pkt.m_relayUser,                                // num（ORIG ebx=-0x8d=(char)msg+0x0e）
           (int)pkt.m_relayMinPing, (int)pkt.m_relayMaxPing, (int)pkt.m_relayAvgPing,
           (unsigned int)pkt.m_relayOverPing100, (unsigned int)pkt.m_relayOverPing200,
           (unsigned int)pkt.m_relayOverPing300, (unsigned int)pkt.m_relayOverPing400);

    CStatisticServerProxy* proxy = GlobalData::s_statistic_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)0);                                  // ORIG GetServerProxy(mgr,0)
    proxy->SendPacket((char*)&pkt, 0x3f);                       // CStatisticServerProxy::SendPacket 0x08472044
    return 0;
}

int Dispatcher_P2P_Statistics::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_P2P_STATISTICS& data = static_cast<MSG_P2P_STATISTICS&>(msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&data.m_0d)))
    {
        return LineFunc(0x591a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&data.m_0e)))
    {
        return LineFunc(0x591b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_10))
    {
        return LineFunc(0x591d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_12))
    {
        return LineFunc(0x591e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_14))
    {
        return LineFunc(0x591f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_18))
    {
        return LineFunc(0x5921,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_1c))
    {
        return LineFunc(0x5922,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_20))
    {
        return LineFunc(0x5923,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_24))
    {
        return LineFunc(0x5924,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_28))
    {
        return LineFunc(0x5926,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_2a))
    {
        return LineFunc(0x5927,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(data.m_2c))
    {
        return LineFunc(0x5928,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_30))
    {
        return LineFunc(0x592a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_34))
    {
        return LineFunc(0x592b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_38))
    {
        return LineFunc(0x592c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_3c))
    {
        return LineFunc(0x592d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}



Dispatcher_PvPChannelInfo::Dispatcher_PvPChannelInfo() {}
Dispatcher_PvPChannelInfo::~Dispatcher_PvPChannelInfo() {}
int Dispatcher_PvPChannelInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() < 3) return 0;
    if (user->getCurCharacR() != 0 && user->isDisguiseCharac()) {
        user->setDisguiseCharac(false, '\0', 0);
    }
    if (user->isCompetitionMercenary()) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc6, (unsigned char)0x15);
        return 0;
    }
    if (user->GetSchoolNo() == 0) {
        PacketGuard response;
        response.put_header(1, 0xc6);
        response.put_byte(1);
        response.put_int(0);
        response.put_byte(0);
        response.finalize(true);
        user->Send(response);
        return 0;
    }
    Packet_PvPChannelInfo channelInfo;
    *(unsigned int*)((char*)&channelInfo + 0x0e) = user->GetUID();
    *(unsigned int*)((char*)&channelInfo + 0x0a) = user->getCurCharacNo();
    *(unsigned char*)((char*)&channelInfo + 0x16) = 9;
    *(unsigned int*)((char*)&channelInfo + 0x12) = (unsigned int)user->GetSchoolNo();
    *(unsigned short*)((char*)&channelInfo + 0x02) = 0x18;
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket((char*)&channelInfo, 0x18);
    return 0;
}
int Dispatcher_PvPChannelInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PvPChannelInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RemovePvPBuddy::Dispatcher_RemovePvPBuddy() {}
Dispatcher_RemovePvPBuddy::~Dispatcher_RemovePvPBuddy() {}
int Dispatcher_RemovePvPBuddy::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    char type = 0;                                   // -0x37
    int nameLen = 0;                                 // -0x3c
    char name[0x1e];                                 // -0x36（ORIG 清零 0x1e 字节）
    memset(name, 0, sizeof(name));
    if (!packet.get_byte(type))
        return LineFunc(0xdf6a, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(nameLen))
        return LineFunc(0xdf6d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(name, 0x1e, nameLen))
        return LineFunc(0xdf70, __PRETTY_FUNCTION__, 0, 0);
    GlobalData::s_community_proxy->SendRemovePvPBuddy(
        user->get_acc_id(),
        (unsigned int)user->getCurCharacNo(),
        type,
        name,
        nameLen);
    return 0;
}

int Dispatcher_RemovePvPBuddy::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RemovePvPBuddy::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RenameCreature::Dispatcher_RenameCreature() {}
Dispatcher_RenameCreature::~Dispatcher_RenameCreature() {}
int Dispatcher_RenameCreature::check_error(CUser* user, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;

    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return -1;
    }

    std::string name(pMsg + 0x14);

    // ORIG 0x081cf7xx: !isUtf8StrInUnicodeRange(name.c_str()) → 错误 0x9f
    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(name.c_str()))
    {
        return 0x9f;
    }

    if (CSyncSlangFilter::GetInstance()->HasSlangName(name) ||
        G_CDataManager()->hasSpecialCharacter(pMsg + 0x14))
    {
        return 0x9f;
    }

    if (CSyncSlangFilter::GetInstance()->HasSlang(name))
    {
        return 0x9f;
    }

    return 0;
}
int Dispatcher_RenameCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRenameCreature& st = *reinterpret_cast<stRenameCreature*>(&param);
    char* pMsg = (char*)&msg;

    int err = check_error(user, msg);
    st.m_4 = err;

    if (err >= 1)
    {
        return 0;
    }

    if (err < 0)
    {
        return LineFunc(0x24d3,
                        __PRETTY_FUNCTION__,
                        err, user->get_acc_id());
    }

    // 名字为默认占位名"이름없음"时清空为空串再改名。
    if (strcmp(pMsg + 0x14, "\xec\x9d\xb4\xeb\xa6\x84\xec\x97\x86\xec\x9d\x8c") == 0)
    {
        memset(pMsg + 0x14, 0, 0xd);
    }

    int slot = (signed char)pMsg[0x0d];      // movsbl
    int creatureIdx = *(short*)(pMsg + 0x0e); // movswl

    CInventory* invW = user->getCurCharacInvenW();
    user_creature::CCreatureMgr* mgr = invW->GetCreatureMgrW();

    // BLOCKED(user_creature::CCreatureMgr::RenameCreature(char const*, int, int)
    //         _ZN13user_creature12CCreatureMgr14RenameCreatureEPKcii @0x08339b6e)
    // CInventory.h 内 user_creature::CCreatureMgr 尚无该声明（见 NEEDS_DECL）。
    mgr->RenameCreature(pMsg + 0x14, creatureIdx, slot); // 占位直调，待声明落地

    return 0;
}
int Dispatcher_RenameCreature::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
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
    if (!packet.get_str(pMsg + 0x14, 0xd, *(int*)(pMsg + 0x10)))
    {
        return LineFunc(0x24bd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void Dispatcher_RenameCreature::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stRenameCreature& p = static_cast<stRenameCreature&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x67, (unsigned char)p.m_4);
}

Dispatcher_UseBoosterItem::Dispatcher_UseBoosterItem() {}
Dispatcher_UseBoosterItem::~Dispatcher_UseBoosterItem() {}


Dispatcher_UserHistoryLog::Dispatcher_UserHistoryLog() {}
Dispatcher_UserHistoryLog::~Dispatcher_UserHistoryLog() {}
int Dispatcher_UserHistoryLog::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081da6c0：LogHistory("act,%d,%s", msg+0xd 字节, param+0x14 字符串)。
    char* m = reinterpret_cast<char*>(&msg);
    char* p = reinterpret_cast<char*>(&param);
    user->LogHistory("act,%d,%s",
                     (int)(unsigned char)m[0xd], p + 0x14);
    return 0;
}
int Dispatcher_UserHistoryLog::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081da5d2：byte@0xd → int@0x10（len）→ str@0x14（max 0x100）
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0xd)))
        return LineFunc(0x48a9, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x10)))
        return LineFunc(0x48ac, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m + 0x14, 0x100, *reinterpret_cast<int*>(m + 0x10)))
        return LineFunc(0x48af, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_BuyCeraShopItem::Dispatcher_BuyCeraShopItem() {}
Dispatcher_BuyCeraShopItem::~Dispatcher_BuyCeraShopItem() {}


Dispatcher_CallGuildInvite::Dispatcher_CallGuildInvite() {}
Dispatcher_CallGuildInvite::~Dispatcher_CallGuildInvite() {}
int Dispatcher_CallGuildInvite::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() < 3 || user->getCurCharacR() == 0) {
        return LineFunc(0xa24f, __PRETTY_FUNCTION__, 0, 0);
    }
    int secuRet = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xb);
    if (secuRet != 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9a, (unsigned char)(secuRet & 0xff));
        return 0;
    }
    int guildKey = 0;
    if (!packet.get_int(guildKey)) {
        return LineFunc(0xa25d, __PRETTY_FUNCTION__, 0, 0);
    }
    char guildName[30];
    if (!packet.get_str(guildName, 0x1e, guildKey)) {
        return LineFunc(0xa261, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->get_charac_guildkey() == 0) {
        send_packet_guild_invite(user, (unsigned char)0x22);
        return 0;
    }
    if (!user->IsSubGuildMaster() && !user->IsGuildMaster()
        && user->getGuildMemberGrade() != 3) {
        send_packet_guild_invite(user, (unsigned char)0x24);
        return 0;
    }
    unsigned int inviteGuildKey = (unsigned int)user->get_charac_guildkey();
    unsigned int characNo = user->getCurCharacNo();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendCallGuildInvite(characNo, inviteGuildKey, guildName);
    return 0;
}
int Dispatcher_CallGuildInvite::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildInvite::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CancelJoinGuild::Dispatcher_CancelJoinGuild() {}
Dispatcher_CancelJoinGuild::~Dispatcher_CancelJoinGuild() {}
// ORIG 0x081d4fcc（报告 Dispatcher_CancelJoinGuild/process.md）：
// state==3 且 guildkey==0；param+0xd(unsigned)=申请索引；
// 成功路径末尾 SendCmdOkPacket(0x15e)。
int Dispatcher_CancelJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    if (user->get_state() == 3 && user->get_charac_guildkey() == 0)
    {
        unsigned int characNo = (unsigned int)user->getCurCharacNo();
        unsigned int reqIdx = *(unsigned int*)((char*)&param + 0xd);
        DB_CancelJoinGuild::makeRequest((int)user->GetUID(), reqIdx, characNo);
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x15e);
    }
    return 0;
}
int Dispatcher_CancelJoinGuild::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0xd)，失败记 LineFunc(0x353c)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0xd)))
    {
        return LineFunc(0x353c, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_CeraPackageOpen::Dispatcher_CeraPackageOpen() {}
Dispatcher_CeraPackageOpen::~Dispatcher_CeraPackageOpen() {}

Dispatcher_ChangeGuildName::Dispatcher_ChangeGuildName() {}
Dispatcher_ChangeGuildName::~Dispatcher_ChangeGuildName() {}
int Dispatcher_ChangeGuildName::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082134a8 [0x082134a8, 0x082138f1]
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return LineFunc(0xc15f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned short slot = 0;                                // -0x1e
    if (!packet.get_short(slot))
    {
        return LineFunc(0xc163,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char space = 0;                                // -0x1f
    if (!packet.get_byte(space))
    {
        return LineFunc(0xc166,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_charac_guildkey() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 100);
        return 0;
    }
    if (!user->isSetGuildCreateFlag(2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 0x6d);
        return 0;
    }
    if (!user->IsGuildMaster())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 0x56);
        return 0;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xf);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, (unsigned char)secu);
        return 0;
    }
    if (user->CheckItemLock(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space), (int)slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 0xd5);
        return 0;
    }
    Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(1, (int)slot);
    if (item.m_addInfo != 0x2e9)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 0x11);
        return 0;
    }
    if ((user->getCurCharacInvenW()->delete_item(
             (INVEN_TYPE)1, (int)slot, 1, (eItemDelReason)3, true) & 0xff) == 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb6, 0x11);
        return 0;
    }
    PacketGuard guard;
    guard.put_header(0, 0xe);
    guard.put_byte(0);
    guard.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, (int)slot, guard);
    guard.finalize(true);
    user->Send(guard);
    char* name = user->getCreateGuildName();
    unsigned int guildkey = (unsigned int)user->get_charac_guildkey();
    unsigned int curNo = (unsigned int)user->getCurCharacNo();
    CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendGuildNameChange(curNo, guildkey, name);
    user->resetGuildCreateFlag();
    user->resetTempGuildNameAddr();
    return 0;
}
int Dispatcher_ChangeGuildName::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeGuildName::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompleteDisplay::Dispatcher_CompleteDisplay() {}
Dispatcher_CompleteDisplay::~Dispatcher_CompleteDisplay() {}
int Dispatcher_CompleteDisplay::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08202cda（docs/class_func_reports/Dispatcher_CompleteDisplay/dispatch_sig.md）
    char subject[0x29];                       // -0x6b：开店名称（get_str 缓冲）
    std::string nameStr;                      // local_44
    private_store::ISeller* seller = 0;       // local_3c
    private_store::PrivateStoreItem item;     // local_bc
    unsigned char itemCount = 0;              // local_36 -0x32
    unsigned short dollSlot;                  // local_46 -0x42
    int slotIdx = 0;                          // local_24 -0x20
    int errDoll;                              // local_1c -0x1c
    bool fail;                                // bVar8
    unsigned char state;

    // +0x00 state/charac 校验：!(state==3 && getCurCharacR()!=0) → fail
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
        fail = false;
    else
        fail = true;
    if (fail)
        return LineFunc(0x9984,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x66：subject 清零 0x29 字节（ORIG 为逐字节展开清零）；itemCount=0；seller=0
    memset(subject, 0, 0x29);
    item.Reset();   // ORIG ctor 0x0823457c 内联体（Reset 0x0823459a 清零位域）

    // +0xe5：读名称长度
    if (!packet.get_int((int&)slotIdx))
        return LineFunc(0x998e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (slotIdx > 0x28)
        return LineFunc(0x9990,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0x15c：读名称串
    if (!packet.get_str(subject, 0x29, slotIdx))
        return LineFunc(0x9993,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x1af：string(subject) → HasSlang / hasSpecialCharacter 检查
    nameStr = subject;
    if (CSyncSlangFilter::GetInstance()->HasSlang(nameStr)
        || G_CDataManager()->hasSpecialCharacter(subject))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x5a, 0x9f);
        return 0;
    }

    // +0x260：取卖家私商
    private_store::GetInstancePrivateStoreMgr()->GetStoreSeller(&seller, user);
    // +0x27b：读 itemCount
    if (!packet.get_byte(itemCount))
        return LineFunc(0x99a4,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x2bf：GetState 调试输出（cMyTrace + printf）
    state = ((private_store::CPrivateStore*)seller)->GetState();
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x99a6, 0);
        trace("(private_store::CPrivateStore*)seller)->GetState()=%d", (unsigned int)state & 0xff);
    }
    printf("(private_store::CPrivateStore*)seller)->GetState()=%d",
           (unsigned int)((private_store::CPrivateStore*)seller)->GetState() & 0xff);

    // +0x32b：state != 1 → return 0
    if (((private_store::CPrivateStore*)seller)->GetState() != 1)
        return 0;

    // +0x343：itemCount == 0 → LineFunc 0x99ad
    if (itemCount == 0)
        return LineFunc(0x99ad,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x376：逐条目读取（i: local_24，setg 无符号比较 itemCnt > i）
    for (slotIdx = 0; slotIdx < (int)(unsigned int)itemCount; slotIdx = slotIdx + 1)
    {
        // +0x382：5 连读，全部成功才通过
        if (!(packet.get_short((short&)item.m_field44)
              && packet.get_int(item.m_field40)
              && packet.get_byte((char&)item.m_field46)
              && packet.get_short((short&)item.m_field48)
              && packet.get_int(item.m_item.m_addInfo2)))
            return LineFunc(0x99bd,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x458：attr != 1 且 count < 1 → 拒绝
        if (item.m_field46 != 1 && item.m_item.m_addInfo2 < 1)
            return LineFunc(0x99c0,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x495：CheckItemLock(user, GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)attr), slot)
        if (user->CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)item.m_field46),
                                (int)(short)item.m_field44))
        {
            ((private_store::CPrivateStore*)seller)->ResetItems();
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x5a, 0xd5);
            return 0;
        }
        // +0x4f2：ISeller 虚槽 +0x10 AddItem(item, prevCountSlot)
        if (!seller->AddItem(item, slotIdx))
        {
            ((private_store::CPrivateStore*)seller)->ResetItems();
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x5a, 0x11);
            return 0;
        }
    }

    // +0x565：读 dollSlot
    if (!packet.get_short((short&)dollSlot))
        return LineFunc(0x99e9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0x5a9：doll 检查
    if (((private_store::CPrivateStore*)seller)->isDollCreated())
    {
        errDoll = ((private_store::CPrivateStore*)seller)->CheckPrivateStoreDoll(dollSlot);
        if (errDoll != 0)
        {
            ((private_store::CPrivateStore*)seller)->ResetItems();
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x5a, (unsigned char)(errDoll & 0xff));
            return 0;
        }
    }
    // +0x606：ISeller 虚槽 +0x08 CompleteDisplay(subject, dollSlot)；失败 → ResetItems
    if (!seller->CompleteDisplay(subject, dollSlot))
        ((private_store::CPrivateStore*)seller)->ResetItems();
    return 0;
}
int Dispatcher_CompleteDisplay::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompleteDisplay::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompleteLoadPvP::Dispatcher_CompleteLoadPvP() {}
Dispatcher_CompleteLoadPvP::~Dispatcher_CompleteLoadPvP() {}
// ORIG 0x082252fe（报告 Dispatcher_CompleteLoadPvP/dispatch_sig.md）：
// state<=1 → LineFunc(0xe005)；matching 非空 → IMatch +0x54 vcall；
// room/matching 为空 → 直接 return 0（无 CAssaultMgr 回退分支）。
int Dispatcher_CompleteLoadPvP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() <= 1)
    {
        LineFunc(0xe005,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    PvP_Room* room = (PvP_Room*)user->GetPVPRoom();
    if (room != 0 && room->m_matching != 0)
    {
        IMatch* matching = (IMatch*)room->m_matching;
        matching->OnCompleteLoad(user);
    }
    return 0;
}
int Dispatcher_CompleteLoadPvP::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompleteLoadPvP::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Compound_Avatar::Dispatcher_Compound_Avatar() {}
Dispatcher_Compound_Avatar::~Dispatcher_Compound_Avatar() {}
int Dispatcher_Compound_Avatar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ead40（PacketDispatcher_Impl_1.cpp 0x6c89 区）
    if (user->get_state() <= 2)
    {
        return 0;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x12);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x66, (unsigned char)secu);
        return 0;
    }
    // 局部 MSG_COMPOUND_AVATAR（ORIG 栈区 -0x2f..-0xc，0x23 字节；类型未建模，按偏移读写）
    char msg[0x23];
    if (!packet.get_short(*(short*)(msg + 0x0d)))
    {
        return LineFunc(0x6c89,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*(short*)(msg + 0x0f)))
    {
        return LineFunc(0x6c8a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(unsigned long*)(msg + 0x11)))
    {
        return LineFunc(0x6c8b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*(short*)(msg + 0x15)))
    {
        return LineFunc(0x6c8c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(unsigned long*)(msg + 0x17)))
    {
        return LineFunc(0x6c8d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(unsigned long*)(msg + 0x1b)))
    {
        return LineFunc(0x6c8e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*(short*)(msg + 0x1f)))
    {
        return LineFunc(0x6c8f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*(unsigned char*)(msg + 0x21)))
    {
        return LineFunc(0x6c90,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*(char*)(msg + 0x22)))
    {
        return LineFunc(0x6c92,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->CheckItemLock(2, (int)*(short*)(msg + 0x0f)))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x66, 0xd5);
        return 0;
    }
    if (user->CheckItemLock(2, (int)*(short*)(msg + 0x15)))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x66, 0xd5);
        return 0;
    }
    ((WongWork::CCompoundAvatar*)GlobalData::s_pCompoundAvatar)
        ->ProcCompound(user, (const MSG_COMPOUND_AVATAR*)msg);
    return 0;
}
int Dispatcher_Compound_Avatar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Compound_Avatar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DieBloodMonster::Dispatcher_DieBloodMonster() {}
Dispatcher_DieBloodMonster::~Dispatcher_DieBloodMonster() {}
int Dispatcher_DieBloodMonster::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return LineFunc(0xda4b, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xda4c, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned char monsterCount = 0;
    unsigned short monsterIdx[256];   // ORIG memset 0x200
    memset(monsterIdx, 0, 0x200);
    if (!packet.get_byte(monsterCount))
    {
        return LineFunc(0xda51, __PRETTY_FUNCTION__, 0, 0);
    }
    for (int i = 0; i < (int)(unsigned int)monsterCount; i++)
    {
        if (!packet.get_short(monsterIdx[i]))
        {
            return LineFunc(0xda54, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    if (user->get_state() == 5)
    {
        short partyIdx = user->GetPartyIndex();
        CGameManager* gameMgr = G_CGameManager();
        CParty* party = gameMgr->GetParty((int)partyIdx);
        if (party == 0)
        {
            return LineFunc(0xda5a, __PRETTY_FUNCTION__, 0, 0);
        }
        party->kill_blood_monster(user, (int)monsterCount, monsterIdx);
    }
    return 0;
}
int Dispatcher_DieBloodMonster::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DieBloodMonster::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DisguiseRequest::Dispatcher_DisguiseRequest() {}
Dispatcher_DisguiseRequest::~Dispatcher_DisguiseRequest() {}
int Dispatcher_DisguiseRequest::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() != 3)
    {
        return 0;
    }
    private_store::CPrivateStoreMgr* manager =
        private_store::GetInstancePrivateStoreMgr();
    if (manager != 0 && manager->IsBusyPrivateStore(user))
    {
        return 0;
    }
    if (user->IsCurCharacGhost() != 0 || user->GetAssaultPlace() != 0)
    {
        return 0;
    }
    unsigned char kind = 0;
    if (!packet.get_byte(kind))
    {
        return LineFunc(0xcf00,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned short index = 0;
    if (!packet.get_short(index))
    {
        return LineFunc(0xcf02,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->isDisguiseCharac() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd6, 0x12);
        return 0;
    }
    user->setDisguiseCharac(true, kind, index);
    PacketGuard notification;
    notification.put_header(0, 2);
    notification.put_byte(0);
    notification.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
    notification.finalize(true);
    G_GameWorld()->send_all(notification, user);
    return 0;
}
int Dispatcher_DisguiseRequest::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DisguiseRequest::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DonateGuildFund::Dispatcher_DonateGuildFund() {}
Dispatcher_DonateGuildFund::~Dispatcher_DonateGuildFund() {}
int Dispatcher_DonateGuildFund::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() > 2)
    {
        int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xa);
        if (secu != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15b,
                                     (unsigned char)(secu & 0xff));
            return 0;
        }
        if (user->get_charac_guildkey() != 0)
        {
            MSG_DONATE_GUILD_FUND& donateMsg =
                static_cast<MSG_DONATE_GUILD_FUND&>(msg);
            if (user->getCurCharacMoney() < donateMsg.m_donateMoney)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15b, 0xa);
            }
            else if (user->IsAddableGuildFund(donateMsg.m_donateMoney) == 1)
            {
                if (user->getCurCharacInvenW()->use_money(
                        donateMsg.m_donateMoney, (eMoneySubReason)0x2a, true) == 1)
                {
                    unsigned int characNo =
                        (unsigned int)user->get_charac_no(-1);
                    unsigned int guildKey =
                        (unsigned int)user->get_charac_guildkey();
                    ENUM_SERVER_GROUP group = user->GetServerGroup();
                    CGuildServerProxy* proxy =
                        GlobalData::s_guild_proxy_mgr->GetServerProxy(group);
                    proxy->SendAddGuildFund(
                        characNo, guildKey, donateMsg.m_donateMoney,
                        (Packet_Add_Guild_Fund::eFundType)1);
                }
                else
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15b, 0xa);
                }
            }
            else
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15b, 0x5f);
            }
        }
    }
    return 0;
}
int Dispatcher_DonateGuildFund::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 int(msg+0xd)，失败记 LineFunc(0x3491)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(p + 0xd)))
    {
        return LineFunc(0x3491, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_GiveupExpertJob::Dispatcher_GiveupExpertJob() {}
Dispatcher_GiveupExpertJob::~Dispatcher_GiveupExpertJob() {}
// ORIG 0x0821eed2（报告 Dispatcher_GiveupExpertJob/dispatch_sig.md）：
// state<=2 或 curCharac==0 → LineFunc 0xd797；否则 s_ExpertJobMgr->OnGiveup。
int Dispatcher_GiveupExpertJob::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xd797,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    GlobalData::s_ExpertJobMgr->OnGiveup(user);
    return 0;
}
int Dispatcher_GiveupExpertJob::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GiveupExpertJob::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GoblinPadStatus::Dispatcher_GoblinPadStatus() {}
Dispatcher_GoblinPadStatus::~Dispatcher_GoblinPadStatus() {}
int Dispatcher_GoblinPadStatus::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82177ae (81 insn)：状态字节 0=未激活 / 1=失败≥3 / 2=已认证 / 3=未认证
    Sanicova::CPad* pad = user->getPad();
    unsigned char status = 0;
    if (pad->isActivate())
    {
        if (pad->getFailCnt() >= 3)
        {
            status = 1;
        }
        else
        {
            status = pad->isCertified() ? 2 : 3;
        }
    }
    PacketGuard response;
    response.put_header(1, 0xc4);
    response.put_byte(1);
    response.put_byte(status);
    response.finalize(true);
    user->Send(response);
    return 0;
}
int Dispatcher_GoblinPadStatus::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GoblinPadStatus::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_LimitNpcBuyItem::Dispatcher_LimitNpcBuyItem() {}
Dispatcher_LimitNpcBuyItem::~Dispatcher_LimitNpcBuyItem() {}
int Dispatcher_LimitNpcBuyItem::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081e1172（docs/class_func_reports/Dispatcher_LimitNpcBuyItem/check_error.md）
    if (user == 0)
        return -1;
    // +0x1b：state<3 或 charac 空 → -1
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        return -1;

    // +0x54：msg+0x0d / msg+0x11 两字段（ORIG 无具名证据，保留裸偏移）
    unsigned int msgD = *(unsigned int*)((char*)&msg + 0xd);
    unsigned int msg11 = *(unsigned int*)((char*)&msg + 0x11);
    // +0x66：限购列表校验
    if (!G_CDataManager()->checkLimitNpcBuyItemList(msg11, (unsigned long)msgD))
    {
        // +0x82：疑似黑客用户 → trace
        int characNo = user->get_charac_no(-1);
        char* accIdStr = NumberToString(user->get_acc_id(), 0);
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0x55e1, 5);
        trace("May be HackUser or Must be HackUser : m_id=%s, charac_no=%u",
              accIdStr, characNo);
        return -1;
    }

    // +0x115：交易中 → 0x13
    if (user->CheckInTrade())
        return 0x13;
    // +0x137：私商占用 → 0x3c
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        return 0x3c;

    // +0x163：查找物品模板
    CItem* item = G_CDataManager()->find_item((int)msgD);
    if (item == 0)
        return 0x11;

    // +0x169：所需材料查询
    Inven_Item matItem;                                           // ctor 080cb854
    std::pair<int, int>* mat = reinterpret_cast<std::pair<int, int>*>(
        NeedMaterialDataManager::getInstance()->getNeedMaterial(item));  // ORIG 0x081b5b70 返回 m_map 值指针
    if (mat->first == 0 || mat->first == -1)
        return 0x17;

    // +0x1a6：背包内材料数据
    if (((const CInventory*)user->getCurCharacInvenRefR())->GetInvenData(mat->first, matItem) < 0)
        return 0x15;
    // +0x1d8：数量校验（need < 1 或持有 < need → 0x16）
    if (mat->second < 1 || matItem.get_add_info() < mat->second)
        return 0x16;

    // +0x20b：构造购买物品（itemIdx = msg+0x0d）并尝试入包
    Inven_Item buyItem;                                           // ctor 080cb854（-0xa2）
    buyItem.m_addInfo = (int)msgD;                                // msg+0x0d → item+0x02
    G_CDataManager()->getItemList()->create_item((ENUM_ITEM_CREATE_TYPE)0, buyItem, 1);
    if (((const CInventory*)user->getCurCharacInvenRefR())->tryInsertItemIntoInventory(buyItem) < 0)
        return 0x4;
    return 0;
}
// ORIG 0x081e0ff8 T _ZN26Dispatcher_LimitNpcBuyItem7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_LimitNpcBuyItem/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 stLimitNpcBuyItemParam，ORIG 裸访问 +0x04 错误码槽（m_4）。
// local_3a = LimitNpcBuyItemInfo（ORIG 0x26 字节，Monitor/LimitNpcBuyItem.h 权威布局）。
int Dispatcher_LimitNpcBuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    *(int*)((char*)&param + 4) = check_error(user, msg);
    int err = *(int*)((char*)&param + 4);
    if (err >= 1)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x55ae,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    LimitNpcBuyItemInfo info;   // local_3a（栈对象 0x26）
    info.m_itemIdReq = user->GetUID();             // local_30（+0xa）
    info.m_dbid = user->get_acc_id();              // local_2c（+0xe）
    info.m_charNo = (unsigned int)user->get_charac_no(-1);   // local_28（+0x12）
    info.m_itemId = *(unsigned int*)((char*)&msg + 0xd);     // local_24（+0x16）
    info.m_count = 1;                              // local_20（+0x1a）
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(&info), 0x26);
    return 0;
}
int Dispatcher_LimitNpcBuyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e0f60：int@0xd → int@0x11
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(m + 0xd)))
        return LineFunc(0x559a, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(m + 0x11)))
        return LineFunc(0x559d, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_LimitNpcBuyItem::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e1100：p4>0 → SendCmdErrorPacket(0x1d1,p4&0xff)（PacketGuard 生命周期忠实保留）
    LimitNpcBuyItem& p = static_cast<LimitNpcBuyItem&>(param);
    {
        PacketGuard pkt;
        if (p.m_4 > 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1d1,
                                     (unsigned char)(p.m_4 & 0xff));
        }
    }
}

Dispatcher_MailBox_Extract::Dispatcher_MailBox_Extract() {}
Dispatcher_MailBox_Extract::~Dispatcher_MailBox_Extract() {}
// ORIG 0x081cceea（报告 Dispatcher_MailBox_Extract/check_error.md）：
// 汇编内全部 CUser 操作以第二实参（user）为对象（this@esp+8、user@esp+0xc）。
int Dispatcher_MailBox_Extract::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
    {
        if (user == 0 || user->CheckInTrade())
        {
            return -1;
        }
        if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        {
            return 0x3c;
        }
        return 0;
    }
    return -1;
}

// ORIG 0x081cce50（报告 Dispatcher_MailBox_Extract/process.md）：
// msg+0xd（unsigned）为邮箱取件索引。
int Dispatcher_MailBox_Extract::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    *(int*)((char*)&param + 4) = err;
    if (err < 1)
    {
        if (err < 0)
        {
            LineFunc(0x245d,
                     __PRETTY_FUNCTION__,
                     err, user->get_acc_id());
        }
        else
        {
            WongWork::CMailBoxHelper::GetPackege(user, *(unsigned int*)((char*)&msg + 0xd));
        }
    }
    return 0;
}
int Dispatcher_MailBox_Extract::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0xd)，失败记 LineFunc(0x244c)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0xd)))
    {
        return LineFunc(0x244c, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_MailBox_Extract::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081ccf94 T：err=*(int*)((char*)&param+4)；err==0x7fffffff 或 0 → 不发包，
    // 否则 SendCmdErrorPacket(0x62, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0x7fffffff)
        return;
    if (err != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, (unsigned char)err);
}


Dispatcher_PVPRequestFight::Dispatcher_PVPRequestFight() {}
Dispatcher_PVPRequestFight::~Dispatcher_PVPRequestFight() {}
int Dispatcher_PVPRequestFight::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() != 7)
    {
        return LineFunc(0x9c25,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x9c2b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (room->request_fight(user) != 1)
    {
        return LineFunc(0x9c30,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_PVPRequestFight::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PVPRequestFight::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_QueryCharacInfo::Dispatcher_QueryCharacInfo() {}
Dispatcher_QueryCharacInfo::~Dispatcher_QueryCharacInfo() {}
int Dispatcher_QueryCharacInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9f60,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char name[0x1e];                                       // -0x2a
    memset(name, 0, sizeof(name));
    int len = 0;                                           // -0xc
    if (!packet.get_int(len))
    {
        return LineFunc(0x9f65,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(name, 0x1e, len))
    {
        return LineFunc(0x9f66,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (strncmp(name, user->getCurCharacName(), 0x1d) == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8b, 7);
        return 0;
    }
    DB_ReqQueryCharacInfo::makeRequest((int)user->GetUID(), name);
    return 0;
}

int Dispatcher_QueryCharacInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_QueryCharacInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ReportAbuseUser::Dispatcher_ReportAbuseUser() {}
Dispatcher_ReportAbuseUser::~Dispatcher_ReportAbuseUser() {}
// ORIG 0x0821ef48（报告 Dispatcher_ReportAbuseUser/dispatch_sig.md）：
// state<3 || curCharacR==0 → LineFunc(0xd7a1)；OnAbuseUser==1 → 0；否则 LineFunc(0xd7b1)。
int Dispatcher_ReportAbuseUser::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xd7a1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    pvp_assault::CAssaultMgr* mgr = pvp_assault::GetInstanceAssaultMgr();
    if (mgr->OnAbuseUser(user) == 1)
    {
        return 0;
    }
    return LineFunc(0xd7b1,
                    __PRETTY_FUNCTION__,
                    0, 0);
}
int Dispatcher_ReportAbuseUser::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ReportAbuseUser::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestItemLock::Dispatcher_RequestItemLock() {}
Dispatcher_RequestItemLock::~Dispatcher_RequestItemLock() {}
int Dispatcher_RequestItemLock::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
        return 0;
    char lock = 0;                                         // -0x11
    short slot = 0;                                        // -0x14
    if (!packet.get_byte(lock))
    {
        return LineFunc(0xdd26,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(slot))
    {
        return LineFunc(0xdd27,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int err = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x17);
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10f, (unsigned char)err);
        return 0;
    }
    err = user->IsAbleItemLock((int)lock, (int)slot);      // ORIG 0x08646a9c
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10f, (unsigned char)err);
        return 0;
    }
    user->DoItemLock((int)lock, (int)slot);                // 0x08646d4a
    return 0;
}

int Dispatcher_RequestItemLock::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestItemLock::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SaveQuestNotify::Dispatcher_SaveQuestNotify() {}
// ORIG _ZN26Dispatcher_SaveQuestNotifyD1Ev W 0x08260a8e：vptr 复位后调基类模板
// D1（0x08262482）+ 恒假 delete 守卫；D0Ev W 0x08260abe = D1 + operator delete。
Dispatcher_SaveQuestNotify::~Dispatcher_SaveQuestNotify() {}
int Dispatcher_SaveQuestNotify::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x0825eec4：user/当前角色空 → -1；state<=2 → -2；
    // 扫 msg+0xd 起 10 槽 int，非 0 且 find_quest 失败 → -3；否则 0。
    if (user == 0 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -2;
    }
    const MSG_SAVE_QUEST_NOTIFY& req = static_cast<const MSG_SAVE_QUEST_NOTIFY&>(msg);
    for (int i = 0; i <= 9; ++i)
    {
        int questIdx = *reinterpret_cast<const int*>(req.m_0d + i * 4); // +0x0d+i*4
        if (questIdx != 0 && G_CDataManager()->find_quest(questIdx) == 0)
        {
            return -3;
        }
    }
    return 0;
}

// ORIG 0x0825ed06：读 1 字节 count（须 0..5），再按 count 读 int 写入
// msg+0xd 起任务 idx 区；任一步失败走 LineFunc（ORIG 返回其断言值）。
int Dispatcher_SaveQuestNotify::read(PacketBuf& packet, MSG_BASE& msg)
{
    char count = 0;
    if (!packet.get_byte(count))
    {
        return LineFunc(0x2b3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (count < 0 || count > 5)
    {
        return LineFunc(0x2b6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    MSG_SAVE_QUEST_NOTIFY& req = static_cast<MSG_SAVE_QUEST_NOTIFY&>(msg);
    for (int i = 0; i < (int)count; ++i)
    {
        unsigned int value = 0;
        if (!packet.get_int(value))
        {
            return LineFunc(0x2bb,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        *reinterpret_cast<int*>(req.m_0d + i * 4) = (int)value; // +0x0d+i*4
    }
    return 0;
}

// ORIG 0x0825edfc：err=check_error(user,msg)；*(int*)(param+4)=err；
// err>0 → 0；err<0 → LogManager::logFormat(1,"PacketDispatcher_Impl_2.cpp",
// <PRETTY>,0x2cb,"Dispatcher Check_Error No(%d)",err) 后 return 0x2cc；
// err==0 → memcpy(getCurCharacQuestW()->m_authenData.m_authen, msg+0xd, 0x28)
// （UserQuest+0x75f4，10×int 任务通知区）后 return 0。
int Dispatcher_SaveQuestNotify::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    saveQuestNotifyParam& res = static_cast<saveQuestNotifyParam&>(param);
    const MSG_SAVE_QUEST_NOTIFY& req = static_cast<const MSG_SAVE_QUEST_NOTIFY&>(msg);

    int err = check_error(user, msg);
    res.m_errorCode = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp",
                              __PRETTY_FUNCTION__,
                              0x2cb, "Dispatcher Check_Error No(%d)", err);
        return 0x2cc;
    }
    // err==0：msg+0xd 起 0x28 字节写入 m_authenData.m_authen（+0x75f4）。
    memcpy(user->getCurCharacQuestW()->m_authenData.m_authen,
           req.m_0d, 0x28);
    return 0;
}

Dispatcher_SecuDataControl::Dispatcher_SecuDataControl() {}
Dispatcher_SecuDataControl::~Dispatcher_SecuDataControl() {}
int Dispatcher_SecuDataControl::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821f612：byte + 两个 short 逐段读取；任一失败走对应行号 LineFunc；
    // 全部成功 → GlobalData::s_pSecuDataControl->SetControlData(user, b, s1, s2)。
    unsigned char b = 0;
    unsigned short s1 = 0;
    unsigned short s2 = 0;

    if (!packet.get_byte(b))
    {
        return LineFunc(0xd838,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(s1))
    {
        return LineFunc(0xd839,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(s2))
    {
        return LineFunc(0xd83a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    GlobalData::s_pSecuDataControl->SetControlData(user, b, s1, s2);
    return 0;
}
int Dispatcher_SecuDataControl::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecuDataControl::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_IPScan::Dispatcher_Security_IPScan() {}
Dispatcher_Security_IPScan::~Dispatcher_Security_IPScan() {}
int Dispatcher_Security_IPScan::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (_NS_IPScan(user, packet) == 1)
    {
        return LineFunc(0xde94,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_IPScan::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_IPScan::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Shop_Coin_Event::Dispatcher_Shop_Coin_Event() {}
Dispatcher_Shop_Coin_Event::~Dispatcher_Shop_Coin_Event() {}
int Dispatcher_Shop_Coin_Event::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ea622（PacketDispatcher_Impl_1.cpp，每日金币事件领取）
    if (user->get_state() <= 2)
    {
        return 0;
    }
    int coinLimit = 1;
    CCoinEventPerDay* event =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    if (event != 0 && event->IsEventing(0))   // 虚槽 +0x34（CEventBase::IsEventing）
    {
        coinLimit = event->GetCoinNoPerDay(1);
    }
    if (user->IsHavePremiumAdvantage())
    {
        coinLimit = user->GetPremiumInfo()->GetAdvantageCoin() + coinLimit;
    }
    unsigned int coin = user->getCurCharacInvenR()->GetCoin();
    bool open;
    if ((int)coin < coinLimit && user->GetOpencoin() != 1)
    {
        open = false;
    }
    else
    {
        open = true;
    }
    if (open)
    {
        PacketGuard guard;
        guard.put_header(1, 0xd2);
        guard.put_byte(1);
        guard.put_byte(0);
        guard.finalize(true);
        user->Send(guard);
        user->SetOpencoin(1);
        return 0;
    }
    int diff = coinLimit - (int)user->getCurCharacInvenR()->GetCoin();
    user->getCurCharacInvenW()->SetCoin((unsigned int)coinLimit);
    user->SetOpencoin(1);
    int curCoin = user->getCurCharacInvenR()->GetCoin();
    user->m_historyLog.CoinAdd(curCoin, diff, (eCoinAddReason)4);
    PacketGuard guard;
    guard.put_header(1, 0xd2);
    guard.put_byte(1);
    guard.put_byte(diff);
    guard.finalize(true);
    user->Send(guard);
    user->SendCoin();
    return 0;
}
int Dispatcher_Shop_Coin_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Shop_Coin_Event::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TutorialLevelUp::Dispatcher_TutorialLevelUp() {}
Dispatcher_TutorialLevelUp::~Dispatcher_TutorialLevelUp() {}
int Dispatcher_TutorialLevelUp::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user == 0 || user->get_state() != 5)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    if (user->get_charac_level() != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    CBattle_Field& bf =
        *reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);
    if (bf.get_cur_map() != 0xee49 && bf.get_cur_map() != 0xee51 &&
        bf.get_cur_map() != 0xee58)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    char itemState = 0;                          // -0x0d
    int needExp = 0;                             // -0x14
    if (!user->CalLevelUpItemState(itemState, needExp, 1, 0x3b))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e4, 0x13);
        return 0;
    }
    int spBefore = 0;                            // -0x18（ORIG -0x18/-0x1c 置 0）
    int spAfter = 0;                             // -0x1c
    int expBonus1;                               // -0x20（ORIG 未出线）
    int expBonus2;                               // -0x24（ORIG 未出线）
    user->rewardExp(needExp, spBefore, spAfter, expBonus1, expBonus2,
                    (eExpAddReason)0xd, true);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x1e4);
    return 0;
}
int Dispatcher_TutorialLevelUp::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ApproveJoinGuild::Dispatcher_ApproveJoinGuild() {}
Dispatcher_ApproveJoinGuild::~Dispatcher_ApproveJoinGuild() {}
// ORIG 0x081d50b4 T _ZN27Dispatcher_ApproveJoinGuild7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_ApproveJoinGuild/process.md 汇编+反编译 C 全文转写）
// local_26 = Packet_Approve_Join_Guild（ORIG 0x16 字节，shared/packet 权威布局）。
int Dispatcher_ApproveJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
    {
        return 0;
    }
    Packet_Approve_Join_Guild packet;   // local_26（ORIG 栈对象 0x16；头仅导出 ctor，字段经裸偏移）
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&packet) + 0x0a) =
        (unsigned int)user->get_charac_guildkey();                      // +0xa guildKey
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&packet) + 0x0e) =
        (unsigned int)user->getCurCharacNo();                           // +0xe charNo
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&packet) + 0x12) =
        *(unsigned int*)((char*)&msg + 0xd);                            // +0x12 no
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&packet), (unsigned int)0x16);
    return 0;
}
int Dispatcher_ApproveJoinGuild::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0xd)，失败记 LineFunc(0x355a)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0xd)))
    {
        return LineFunc(0x355a, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_CallGuildLevelUp::Dispatcher_CallGuildLevelUp() {}
Dispatcher_CallGuildLevelUp::~Dispatcher_CallGuildLevelUp() {}
int Dispatcher_CallGuildLevelUp::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned int local_2c = 0;           // -0x28：升级价格输出
    unsigned int local_24;               // -0x20：CheckGuildLevelUp 结果
    int local_28;                        // -0x24：Secu Check
    char local_2f;                       // -0x2b：临时 level
    unsigned char local_2e;              // -0x2a：临时 powerSide
    char cVar2;
    bool bVar1;

    int iVar3 = user->get_state();
    if (iVar3 <= 2 || user->getCurCharacR() == 0)
    {
        LineFunc(0x947f, __PRETTY_FUNCTION__, 0, 0);
        return 0;
    }
    cVar2 = (char)((ServiceRestrictManager*)ARAD::Singleton<ServiceRestrictManager>::Get())
                ->isRestricted(user, (RestrictCategory::Enum)1, 0x1d);
    if (cVar2 != 0)
    {
        send_packet_guild_levelup(user, 0xd1);
        return 0;
    }
    local_28 = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)7);
    if (local_28 != 0)
    {
        send_packet_guild_levelup(user, (unsigned char)(local_28 & 0xff));
        return 0;
    }
    local_24 = user->CheckGuildLevelUp(local_2c);
    if (local_24 != 0)
    {
        send_packet_guild_levelup(user, (unsigned char)(local_24 & 0xff));
        return 0;
    }
    send_packet_guild_levelup(user, 0);
    unsigned int uVar9 = local_2c;
    unsigned int uVar6 = user->get_charac_guildkey();
    unsigned int uVar7 = user->getCurCharacNo();
    int iVar3b = (int)user->GetUID();
    ENUM_SERVER_GROUP uVar4 = user->GetServerGroup();
    CGuildServerProxy* pCVar8 =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(uVar4);
    pCVar8->SendGuildLevelup(iVar3b, uVar7, uVar6, uVar9);
    void* local_20 = user->GetGuildDBInfo();
    STGuildProxy local_46;
    STGuildDBInfoOnly* guildDb = reinterpret_cast<STGuildDBInfoOnly*>(local_20);
    local_2f = (char)guildDb->m_fundLevel + 1;
    local_46.m_level = local_2f;
    memcpy(local_46.m_guildName, local_20, 0x16);
    local_2e = guildDb->m_field95;
    local_46.m_powerSide = local_2e;
    unsigned int uVar9b = user->get_charac_guildkey();
    ENUM_SERVER_GROUP uVar4b = user->GetServerGroup();
    CGuildServerProxy* pCVar8b =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(uVar4b);
    pCVar8b->ModifyGuildInfoProxy(uVar9b, local_46);
    return 0;
}
int Dispatcher_CallGuildLevelUp::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildLevelUp::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CallGuildMembers::Dispatcher_CallGuildMembers() {}
Dispatcher_CallGuildMembers::~Dispatcher_CallGuildMembers() {}
int Dispatcher_CallGuildMembers::dispatch_sig(CUser* user, PacketBuf& packet)
{
    bool gate;
    int guildkey;
    unsigned int characNo;
    unsigned int serverGroup;
    CGuildServerProxy* proxy;

    (void)packet;
    if (user->get_state() > 2)
    {
        if (user->getCurCharacR() != 0)
            gate = false;
        else
            gate = true;
    }
    else
    {
        gate = true;
    }
    if (gate)
    {
        LineFunc(0x93c0,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    guildkey = user->get_charac_guildkey();
    if (guildkey != 0)
    {
        characNo = (unsigned int)user->getCurCharacNo();
        serverGroup = (unsigned int)user->GetServerGroup();
        proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)serverGroup);
        proxy->SendPacketCallGuildMembers(characNo, (unsigned int)guildkey);
    }
    return 0;
}
int Dispatcher_CallGuildMembers::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildMembers::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ChangeCharacName::Dispatcher_ChangeCharacName() {}
Dispatcher_ChangeCharacName::~Dispatcher_ChangeCharacName() {}
int Dispatcher_ChangeCharacName::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeCharacName::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ChangeLetterStat::Dispatcher_ChangeLetterStat() {}
Dispatcher_ChangeLetterStat::~Dispatcher_ChangeLetterStat() {}
int Dispatcher_ChangeLetterStat::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0 ? 0 : -1;
}
int Dispatcher_ChangeLetterStat::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err;
    int mailIdx;
    int letterStat;
    int result;

    err = check_error(user, msg);
    *(reinterpret_cast<int*>(&param) + 1) = err;
    if (*(reinterpret_cast<int*>(&param) + 1) > 0)
        return 0;
    if (*(reinterpret_cast<int*>(&param) + 1) < 0)
    {
        LineFunc(0x2744,
                 __PRETTY_FUNCTION__,
                 *(reinterpret_cast<int*>(&param) + 1), user->get_acc_id());
        return 0;
    }
    mailIdx = *reinterpret_cast<int*>(reinterpret_cast<char*>(&msg) + 0xd);
    letterStat = (int)(short)*reinterpret_cast<unsigned short*>(
        reinterpret_cast<char*>(&msg) + 0x11);
    result = WongWork::CMailBoxHelper::ReqChangeLetterStat(user, mailIdx, letterStat);
    *(reinterpret_cast<int*>(&param) + 1) = result;
    if (*(reinterpret_cast<int*>(&param) + 1) == 0)
        return 0;
    return 0;
}
int Dispatcher_ChangeLetterStat::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081ce152：先清 int@0xd/short@0x11，再 get_int@0xd → get_short@0x11
    char* m = reinterpret_cast<char*>(&msg);
    *reinterpret_cast<int*>(m + 0xd) = 0;
    *reinterpret_cast<short*>(m + 0x11) = 0;
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0xd)))
        return LineFunc(0x2732, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<short*>(m + 0x11)))
        return LineFunc(0x2734, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_ChangeLetterStat::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081ce304 T：err=*(int*)((char*)&param+4)；err==0x7fffffff 或 0 → 不发包，
    // 否则 SendCmdErrorPacket(0x89, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0x7fffffff)
        return;
    if (err != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x89, (unsigned char)err);
}

Dispatcher_DieAssaultPlayer::Dispatcher_DieAssaultPlayer() {}
Dispatcher_DieAssaultPlayer::~Dispatcher_DieAssaultPlayer() {}
int Dispatcher_DieAssaultPlayer::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 2) {
        return LineFunc(0xa1d2, __PRETTY_FUNCTION__, 0, 0);
    }
    char killerIdx = (char)0xff;
    if (!packet.get_byte(killerIdx)) {
        return LineFunc(0xa1d6, __PRETTY_FUNCTION__, 0, 0);
    }
    MSG_DIE_ASSAULT_PLAYER msg;
    if (!packet.get_int((unsigned int&)msg.m_damage)) {
        return LineFunc(0xa1dc, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(msg.m_attackCount)) {
        return LineFunc(0xa1df, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(msg.m_field08)) {
        return LineFunc(0xa1e2, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(msg.m_field0a)) {
        return LineFunc(0xa1e5, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(msg.m_field0c)) {
        return LineFunc(0xa1e8, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(msg.m_field0e)) {
        return LineFunc(0xa1eb, __PRETTY_FUNCTION__, 0, 0);
    }
    pvp_assault::CAssaultMgr* assaultMgr = pvp_assault::GetInstanceAssaultMgr();
    if (!assaultMgr->OnDieUser(user, (int)killerIdx, msg)) {
        return LineFunc(0xa1f1, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
int Dispatcher_DieAssaultPlayer::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DieAssaultPlayer::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Equipslot_Switch::Dispatcher_Equipslot_Switch() {}
Dispatcher_Equipslot_Switch::~Dispatcher_Equipslot_Switch() {}
int Dispatcher_Equipslot_Switch::check_error(CUser* user, MSG_BASE& msg)
{
    if (&msg == 0)
    {
        return LineFunc(0x3e1c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() <= 2)
    {
        return LineFunc(0x3e1d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// err>0 → 错误包(0x18d,err) 返 -1；err<0 → 错误包(0x18d,0x15)+LineFunc(0x3e2c)；
// 否则对扩展槽视图(#9)执行 EquipslotSwitch(msg+0xf,msg+0xd,msg+0xe)，结果字节
// 回写 param.m_4。
int Dispatcher_Equipslot_Switch::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stEquipslotSwitch& out = static_cast<stEquipslotSwitch&>(param);
    MSG_EQUIPSLOT_SWITCH& req = static_cast<MSG_EQUIPSLOT_SWITCH&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    int err = check_error(user, msg);
    if (err > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18d,
                                 (unsigned char)(err & 0xff));
        return -1;
    }
    if (err < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18d, 0x15);
        return LineFunc(0x3e2c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CExpandEquipslot* expand =
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
    out.m_result = (unsigned char)(char)expand->EquipslotSwitch(
        user, pReq[0xf], pReq[0xd], pReq[0xe]);
    return 0;
}
int Dispatcher_Equipslot_Switch::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_EQUIPSLOT_SWITCH& req = static_cast<MSG_EQUIPSLOT_SWITCH&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    char& flagD = pReq[0xd];
    char& flagE = pReq[0xe];
    char& flagF = pReq[0xf];
    if (!packet.get_byte(flagD))
    {
        return LineFunc(0x3e12,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(flagE))
    {
        return LineFunc(0x3e14,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(flagF))
    {
        return LineFunc(0x3e16,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// 成功先补发两份装备槽视图，再统一回 header(1,0x18d)：成功 {1}，失败 {0,m_4}。
void Dispatcher_Equipslot_Switch::send(CUser* user, ParamBase& param)
{
    stEquipslotSwitch& out = static_cast<stEquipslotSwitch&>(param);
    if (out.m_result == 0)
    {
        CExpandEquipslot* expand =
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        expand->Send_Equip_Info(user);
        expand->Send_Expand_Equip_Info(user);
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x18d);
    if (out.m_result == 0)
    {
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte((char)out.m_result);
    }
    packet.finalize(true);
    user->Send(packet);
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

Dispatcher_ExitPrivateStore::Dispatcher_ExitPrivateStore() {}
Dispatcher_ExitPrivateStore::~Dispatcher_ExitPrivateStore() {}
// ORIG 0x0820269c（报告 Dispatcher_ExitPrivateStore/dispatch_sig.md）：
// state<3 || curCharacR==0 → 0；GetStoreBuyer 后经 IBuyer 调 ExitPrivateStore()。
int Dispatcher_ExitPrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    private_store::IBuyer* buyer = 0;
    private_store::CPrivateStoreMgr* mgr = private_store::GetInstancePrivateStoreMgr();
    mgr->GetStoreBuyer(&buyer, user, static_cast<CUser*>(0));
    if (buyer != 0)
    {
        buyer->ExitPrivateStore();      // ORIG IBuyer vtable 槽 +0x0c
    }
    return 0;
}
int Dispatcher_ExitPrivateStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ExitPrivateStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ExpertExtraction::Dispatcher_ExpertExtraction() {}
Dispatcher_ExpertExtraction::~Dispatcher_ExpertExtraction() {}

Dispatcher_InformNotice_2nd::Dispatcher_InformNotice_2nd() {}
Dispatcher_InformNotice_2nd::~Dispatcher_InformNotice_2nd() {}
int Dispatcher_InformNotice_2nd::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 3)
    {
        return 1;
    }
    const MSG_INFORM_NOTICE_2ND& req = static_cast<const MSG_INFORM_NOTICE_2ND&>(msg);
    if ((unsigned int)req.m_noticeFlag > 0x7f)
    {
        return -1;
    }
    return 0;
}
int Dispatcher_InformNotice_2nd::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e30fa：param+4=check_error；<0 → LineFunc(0x59c7,err,0)；
    // ==0 → GetCharacExpandData(0x13)->handleInformNoticeFlag(msg+0xd)
    int err = check_error(user, msg);
    InformNoticeData2nd& p = static_cast<InformNoticeData2nd&>(param);
    p.m_4 = err;
    if (p.m_4 < 1)
    {
        if (p.m_4 < 0)
        {
            return LineFunc(0x59c7,
                            __PRETTY_FUNCTION__,
                            p.m_4, 0);
        }
        CCharacInformNotice* notice = reinterpret_cast<CCharacInformNotice*>(
            user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_19));
        if (notice != 0)
        {
            notice->handleInformNoticeFlag(
                *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&msg) + 0xd));
        }
        return 0;
    }
    return 0;
}
int Dispatcher_InformNotice_2nd::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0xd)，失败记 LineFunc(0x59b8)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0xd)))
    {
        return LineFunc(0x59b8, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_InformNotice_2nd::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e31f4 T：err=*(int*)((char*)&param+4)；0→SendCmdOkPacket(0x1de)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1de);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1de, (unsigned char)err);
}

Dispatcher_MemberEnterReply::Dispatcher_MemberEnterReply() {}
Dispatcher_MemberEnterReply::~Dispatcher_MemberEnterReply() {}
int Dispatcher_MemberEnterReply::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0 ? 0 : -1;
}
// ORIG 0x081cba34 T _ZN27Dispatcher_MemberEnterReply7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_MemberEnterReply/process.md 汇编+反编译 C 全文转写）
// param 为模板实参，ORIG 裸访问 +0x04 错误码槽（m_4）。
// local_27 = Packet_Monitor_Member_Enter_Reply（ORIG 0x13 字节，shared/packet 权威布局）。
int Dispatcher_MemberEnterReply::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    *(int*)((char*)&param + 4) = check_error(user, msg);
    int err = *(int*)((char*)&param + 4);
    if (err >= 1)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x21ef,
                        __PRETTY_FUNCTION__,
                        err, user->get_acc_id());
    }
    Packet_Monitor_Member_Enter_Reply reply;   // local_27（栈对象 0x13）
    reply.m_idByChannel = user->GetUID();      // local_1d（+0xa）
    reply.m_charNo = (unsigned int)user->getCurCharacNo();  // local_19（+0xe）
    reply.m_code = *(unsigned char*)((char*)&msg + 0xd);    // local_15（+0x12）
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&reply), 0x13);
    return 0;
}
int Dispatcher_MemberEnterReply::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned char(msg+0xd)，失败记 LineFunc(0x21d4)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xd)))
    {
        return LineFunc(0x21d4, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_Mercenary_Return::Dispatcher_Mercenary_Return() {}
Dispatcher_Mercenary_Return::~Dispatcher_Mercenary_Return() {}
int Dispatcher_Mercenary_Return::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MsgMercenaryReturn& req = reinterpret_cast<MsgMercenaryReturn&>(msg);
    SendMercenaryReturnResult& result = static_cast<SendMercenaryReturnResult&>(param);
    if (user == 0)
        return -1;
    if (user->get_state() != 3)
        return -1;

    char* pResult = reinterpret_cast<char*>(&result);
    // ORIG: Return_Mercenary_Charac(user, *(uint*)(msg+0x10), m_result@+0xc 引用传入)
    // （ORIG 0x0868e692 声明 bool、实现 void 转发无 return，%al 残留直写 pResult+0x04）
    pResult[0x04] = static_cast<char>(
        user->Return_Mercenary_Charac(
            *reinterpret_cast<unsigned int*>(
                reinterpret_cast<char*>(&msg) + 0x10),
            result.m_result));                                  // +0x04 bool 归还成功
    pResult[0x05] = req.m_pad[0];                               // +0x05 char 回显(msg+0xd)
    *reinterpret_cast<int*>(pResult + 0x08) =
        *reinterpret_cast<unsigned int*>(
            reinterpret_cast<char*>(&msg) + 0x10);              // +0x08 int = msg+0x10
    return 0;
}

int Dispatcher_Mercenary_Return::read(PacketBuf& packet, MSG_BASE& msg)   // ORIG 0x081dd904 (39 insn)
{
    MsgMercenaryReturn& req = reinterpret_cast<MsgMercenaryReturn&>(msg);
    // msg+0xd byte / msg+0x10 int；失败走 LineFunc(0x4e88/0x4e8a, 本函数 pretty 名)
    // gcc 4.4：__PRETTY_FUNCTION__ 是静态数组，不能初始化 const char[]（数组不可拷贝）→ 指针形式。
    static const char* kFunc = __PRETTY_FUNCTION__;
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_pad[0])))
        return LineFunc(0x4e88, kFunc, 0, 0);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(req.m_pad + 3)))
        return LineFunc(0x4e8a, kFunc, 0, 0);
    return 0;
}
void Dispatcher_Mercenary_Return::send(CUser* user, ParamBase& param)     // ORIG 0x081dda1c (86 insn)
{
    SendMercenaryReturnResult& result = static_cast<SendMercenaryReturnResult&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x1b9);
    guard.put_byte(*reinterpret_cast<char*>(reinterpret_cast<char*>(&result) + 0x04) != 0);  // +0x04 bool
    guard.put_byte(static_cast<unsigned int>(
        *reinterpret_cast<char*>(reinterpret_cast<char*>(&result) + 0x05)));                 // +0x05 char 回显
    guard.put_int(*reinterpret_cast<int*>(reinterpret_cast<char*>(&result) + 0x08));         // +0x08 int
    guard.put_int(result.m_result.m_0);                                     // +0x0c
    guard.put_int(result.m_result.m_4);                                     // +0x10
    guard.put_byte(static_cast<unsigned int>(
        static_cast<char>(result.m_result.m_8)));                           // +0x14
    guard.finalize(true);
    user->Send(guard);
}

Dispatcher_MultiMailBoxSend::Dispatcher_MultiMailBoxSend() {}
Dispatcher_MultiMailBoxSend::~Dispatcher_MultiMailBoxSend() {}
// ORIG T 0x081202a9：交易限次物品移动资格。
bool CheckTradeLimitItemCountToMove(const Inven_Item& item, bool& ok)
{
    ok = true;
    CItem* proto = G_CDataManager()->find_item(item.m_addInfo);
    if (proto == 0)
        return ok;
    if (proto->is_stackable())
        return ok;
    if (proto->GetAttachType() != 7)
        return ok;
    if (item.GetTradeLimitCount() != 0)
        ok = false;
    return ok;
}

// ORIG T 0x0812032e：
bool CheckItemTradeLimitCountToMoveByUser(CUser& user, int space, int slot,
                                          bool& ok)
{
    ok = true;
    if (user.getCurCharacR() == 0)
        return ok;
    Inven_Item item =
        ((CInventory*)user.getCurCharacInvenR())->GetInvenSlot(space, slot);
    if (*(const int*)&item.m_addInfo != 0)
        CheckTradeLimitItemCountToMove(item, ok);
    return ok;
}

// ORIG 0x082262a2：多附件邮件发送（PacketDispatcher_Impl_1.cpp 0xe1ea 区）。
int Dispatcher_MultiMailBoxSend::dispatch_sig(CUser* user, PacketBuf& packet)
{
    MSG_MULTI_MAILBOX_SEND msg;   // sizeof 0x1a6（set_MultimailMsgData memcpy 定案）
    memset(&msg, 0, sizeof(msg));

    if (user == 0 || user->get_state() != 3 || user->getCurCharacR() == 0)
        return 0;

    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x40);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c,
                                 (unsigned char)(secuErr & 0xff));
        return 0;
    }
    if (user->isHackUser())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x17);
        return 0;
    }

    if (!packet.get_int(msg.m_flagD))
        return LineFunc(0xe1ea,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (msg.m_flagD < 1)
        return LineFunc(0xe1ed,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_str(msg.m_title11, 0x1e, msg.m_flagD))
        return LineFunc(0xe1f0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int((int&)msg.m_gold2f))
        return LineFunc(0xe1f3,
                        __PRETTY_FUNCTION__,
                        0, 0);

    msg.m_count33 = 1;   // ORIG movb $1 初始化后被覆盖
    if (!packet.get_byte(msg.m_count33))
        return LineFunc(0xe1f7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (msg.m_count33 > 10)
        return LineFunc(0xe1fa,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // 条目区 stride 0xb：space@+0、slot@+1、seq u64@+3、price int 与 seq 低 32 位同址
    for (unsigned char i = 0; i < msg.m_count33; ++i)
    {
        char* e = msg.m_body34 + i * 0xb;
        unsigned char spaceV = 0;
        if (!packet.get_byte(spaceV))
            return LineFunc(0xe1ff,
                            __PRETTY_FUNCTION__,
                            0, 0);
        *e = spaceV;
        if (!packet.get_short(*(unsigned short*)(e + 1)))
            return LineFunc(0xe202,
                            __PRETTY_FUNCTION__,
                            0, 0);
        unsigned long seqV = 0;
        if (!packet.get_int(seqV))
            return LineFunc(0xe205,
                            __PRETTY_FUNCTION__,
                            0, 0);
        memcpy(e + 3, &seqV, sizeof(seqV));
        if (*(const unsigned long*)(e + 3) == 0)
            break;
        if (!packet.get_int(*(int*)(e + 7)))
            return LineFunc(0xe20e,
                            __PRETTY_FUNCTION__,
                            0, 0);

        const unsigned short curSlot = *(const unsigned short*)(e + 1);
        const int invenType =
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned char)*e);
        if (user->CheckItemLock(invenType, (int)curSlot) != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0xd5);
            return 0;
        }
        bool movable = true;
        CheckItemTradeLimitCountToMoveByUser(*user, invenType,
                                             (int)curSlot, movable);
        if (!movable)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x72);
            return 0;
        }
    }

    if (!packet.get_int(msg.m_msgLenA2))
        return LineFunc(0xe229,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_str(msg.m_msgTextA6, 0x100, msg.m_msgLenA2))
        return LineFunc(0xe22b,
                        __PRETTY_FUNCTION__,
                        0, 0);

    std::string msgText(msg.m_msgTextA6);
    if (CSyncSlangFilter::GetInstance()->HasSlang(msgText))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x9f);
        return 0;
    }
    if ((int)strlen(msg.m_msgTextA6) != msg.m_msgLenA2)
        return LineFunc(0xe237,
                        __PRETTY_FUNCTION__,
                        0, 0);

    if (user->GetReliablePerson() != 1 &&
        !user->IsAvailableCurCharacTradeGoldDaily(msg.m_gold2f))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0x46);
        return 0;
    }

    bool needCertify = false;
    if (msg.m_gold2f != 0 && user->reqSendMailCertify() != 0)
        needCertify = true;   // reqSendMailCertify 定义属 W7-CUser 域（Main 已排期）
    if (needCertify)
    {
        user->set_MultimailMsgData(&msg);
        return 0;
    }

    MSG_CHECK_MAIL_RECEIVER check;
    check.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_2>(msg);
    int rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(user, msg.m_title11,
                                                          &check);
    if (rc != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c,
                                 (unsigned char)(rc & 0xff));
        return 0;
    }

    unsigned char hackFlag = 0;
    for (unsigned char i = 0; i < msg.m_count33; ++i)
    {
        char* e = msg.m_body34 + i * 0xb;
        const int protoIdx = *(const int*)(e + 3);   // seq 槽低 32 位
        CItem* proto = G_CDataManager()->find_item(protoIdx);
        if (proto != 0 && !(proto->IsAvatarItem() == 1 ||
                            proto->get_rarity() > 1))
        {
            hackFlag = 1;
        }
        if (protoIdx == 0)
            hackFlag = 1;
        user->IncrementTradeGold(msg.m_gold2f, (int)hackFlag);
    }
    return 0;
}
int Dispatcher_MultiMailBoxSend::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MultiMailBoxSend::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Open_Guild_Board::Dispatcher_Open_Guild_Board() {}
Dispatcher_Open_Guild_Board::~Dispatcher_Open_Guild_Board() {}
// ORIG 0x081d30d0（报告 Dispatcher_Open_Guild_Board/process.md）：
// param+0xb2(unsigned char)=公告板页码；无状态门槛。
int Dispatcher_Open_Guild_Board::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    unsigned int guildkey = (unsigned int)user->get_charac_guildkey();
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    char page = (char)*(unsigned char*)((char*)&param + 0xb2);
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendGuildBoardOpen(characNo, guildkey, page);
    return 0;
}
int Dispatcher_Open_Guild_Board::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 char(msg+0xb2)，失败记 LineFunc(0x30aa)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(p + 0xb2)))
    {
        return LineFunc(0x30aa, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_PvPBuddyConnList::Dispatcher_PvPBuddyConnList() {}
Dispatcher_PvPBuddyConnList::~Dispatcher_PvPBuddyConnList() {}
// ORIG 0x08224e08（报告 Dispatcher_PvPBuddyConnList/dispatch_sig.md）：
// IsIntegratedPvPBaseChannel==1 → SendPvPBuddyList(accId, characNo)；恒返 0。
int Dispatcher_PvPBuddyConnList::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() == 0)
        return 0;
    GlobalData::s_community_proxy->SendPvPBuddyList(
        user->get_acc_id(),
        static_cast<unsigned int>(user->getCurCharacNo()));
    return 0;
}
int Dispatcher_PvPBuddyConnList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PvPBuddyConnList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RefreshGuildInfo::Dispatcher_RefreshGuildInfo() {}
Dispatcher_RefreshGuildInfo::~Dispatcher_RefreshGuildInfo() {}
// ORIG 0x081d4886（报告 Dispatcher_RefreshGuildInfo/process.md）：
// state>2 且 guildkey!=0；characNo 经 get_charac_no(-1)。
int Dispatcher_RefreshGuildInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() > 2 && user->get_charac_guildkey() != 0)
    {
        unsigned int guildkey = (unsigned int)user->get_charac_guildkey();
        unsigned int characNo = (unsigned int)user->get_charac_no(-1);
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendRefreshGuildInfo(characNo, guildkey);
    }
    return 0;
}
int Dispatcher_RefreshGuildInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ReplyGuildInvite::Dispatcher_ReplyGuildInvite() {}
Dispatcher_ReplyGuildInvite::~Dispatcher_ReplyGuildInvite() {}
int Dispatcher_ReplyGuildInvite::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned char reply = 0;
    if (user != 0 && user->get_state() > 2 && user->getCurCharacR() == 0)
    {
        if (!packet.get_byte(reply))
        {
            return LineFunc(
                0xa27c,
                __PRETTY_FUNCTION__,
                0, 0);
        }
        if (G_GameWorld()->IsIntegratedPvPBaseChannel() != 0)
        {
            PacketGuard response;
            response.put_header(1, 0x9b);
            response.put_byte(0);
            response.put_int(99);
            response.finalize(true);
            user->Send(response);
            return 0;
        }
        const int secuRet =
            GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xc);
        if (secuRet != 0)
        {
            reply = static_cast<unsigned char>(
                GlobalData::s_pSecuProtectionField->GetOppositeErr(secuRet));
        }
        const unsigned int characNo = user->getCurCharacNo();
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendReplyGuildInvite(characNo, reply);
        return 0;
    }
    return LineFunc(
        0xa278,
        __PRETTY_FUNCTION__,
        0, 0);
}
int Dispatcher_ReplyGuildInvite::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ReplyGuildInvite::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ReportBadP2PUser::Dispatcher_ReportBadP2PUser() {}
Dispatcher_ReportBadP2PUser::~Dispatcher_ReportBadP2PUser() {}
int Dispatcher_ReportBadP2PUser::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char seat = 0;
    if (!packet.get_byte(seat))
    {
        return LineFunc(0x8ddb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user == 0)
    {
        return 0;
    }
    if (!user->CheckInPvp())
    {
        if (user->get_state() == 5)
        {
            CParty* party = (CParty*)user->GetParty();
            if (party != 0)
            {
                party->VoteLackUser(user, (int)seat);
            }
        }
    }
    else
    {
        PvP_Room* room = (PvP_Room*)user->GetPVPRoom();
        if (room != 0)
        {
            room->VoteLackUser(user, (int)seat);
        }
    }
    return 0;
}
int Dispatcher_ReportBadP2PUser::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ReportBadP2PUser::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Req_Urgent_Quest::Dispatcher_Req_Urgent_Quest() {}
Dispatcher_Req_Urgent_Quest::~Dispatcher_Req_Urgent_Quest() {}
// ORIG 0x081df42c：state>2 → param+4=0/param+8=-1；等级 27..69 且
// getCurCharacQuestR()->m_authenData.m_urgentQuest==-1 才继续；
// hasEpicQuest → err=1；否则静态 selected_quest 清空后
// getQuestNPCList(8)+allowable_questlist_as_npc(…,true) 取候选：
// 空 → err=4；非空 → 静态 CMTRand randInt(size-1) 抽一个写回
// m_urgentQuest 与 param+8，param+4=1。
int Dispatcher_Req_Urgent_Quest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    ReqResult& res = static_cast<ReqResult&>(param);
    if (user->get_state() <= 2)
    {
        return 0;
    }

    res.m_found = 0;
    res.m_questIdx = -1;

    int level = user->get_charac_level();
    UserQuest* questR = user->getCurCharacQuestR();
    bool eligible = level > 0x1a && level <= 0x45 &&
                    questR->m_authenData.m_urgentQuest == -1;
    if (!eligible)
    {
        res.m_errorCode = 1;
        return 0;
    }
    if (user->getCurCharacQuestW()->hasEpicQuest())
    {
        res.m_errorCode = 1;
        return 0;
    }

    // 函数局部静态（ORIG __cxa_guard 保护）：候选任务列表 + 随机骰。
    static std::list<int> selected_quest;
    selected_quest.clear();

    char cleared[0x7531];  // ORIG local_7589：m_cleared(0x7530)+save-flag 逐字节快照
    memcpy(cleared, &user->getCurCharacQuestR()->m_cleared, 0x7531);

    stSelectQuestParam selectParam(user);
    QuestList* list = G_CDataManager()->m_questList;
    std::multimap<int, int>* npcList =
        list->getQuestNPCList(ENUM_QUEST_GRADE_8);
    list->allowable_questlist_as_npc(
        selected_quest, *npcList, selectParam,
        *reinterpret_cast<const WongWork::CQuestClear*>(cleared), true);

    if (selected_quest.empty())
    {
        res.m_errorCode = 4;
    }
    else
    {
        static CMTRand urgentRandomDice;  // ORIG __cxa_guard 静态
        unsigned long range = (unsigned long)(selected_quest.size() - 1);
        int dice = urgentRandomDice.randInt(range);
        if ((unsigned int)dice < selected_quest.size())
        {
            std::list<int>::iterator it = selected_quest.begin();
            std::advance(it, dice);
            int selectedIdx = *it;
            user->getCurCharacQuestW()->m_authenData.m_urgentQuest = selectedIdx;
            res.m_questIdx = selectedIdx;
            res.m_found = 1;
        }
    }
    return 0;
}
// ORIG 0x081df422：字面 return 0（无包内容需解析）。
int Dispatcher_Req_Urgent_Quest::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
// ORIG 0x081df75a：PacketGuard put_header(1,0x1c6)；put_byte(param.m_found!=0)；
// found → put_short(param.m_questIdx)，否则 put_byte(param.m_errorCode)；
// finalize(true) 后 CUser::Send。
void Dispatcher_Req_Urgent_Quest::send(CUser* user, ParamBase& param)
{
    const ReqResult& res = static_cast<const ReqResult&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1c6);
    packet.put_byte(res.m_found != 0 ? 1 : 0);
    if (res.m_found)
    {
        packet.put_short(res.m_questIdx);
    }
    else
    {
        packet.put_byte(res.m_errorCode);
    }
    packet.finalize(true);
    user->Send(packet);
}

Dispatcher_RequestBlackList::Dispatcher_RequestBlackList() {}
Dispatcher_RequestBlackList::~Dispatcher_RequestBlackList() {}
int Dispatcher_RequestBlackList::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() >= 3 ? 0 : -1;
}
int Dispatcher_RequestBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ce070：check_error 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x2717)；err==0 → 向 monitor 请求黑名单。
    int err = check_error(user, msg);
    *(int*)(reinterpret_cast<char*>(&param) + 0x4) = err;

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LineFunc(0x2717,
                 __PRETTY_FUNCTION__,
                 err, (unsigned int)user->get_acc_id());
        return 0;
    }

    // ORIG 0x0822bbb2 ctor；pkt+0x0a = get_acc_id(0x080da36e)；
    // pkt+0x0e = GetUID(0x080c8c96)；SendPacket(pkt, 0x12)。
    Packet_Request_BlackList pkt;
    *(unsigned int*)(reinterpret_cast<char*>(&pkt) + 0x0a) = user->get_acc_id();
    *(unsigned int*)(reinterpret_cast<char*>(&pkt) + 0x0e) = user->GetUID();

    ENUM_SERVER_GROUP group = user->GetServerGroup();
    CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x12);
    return 0;
}
int Dispatcher_RequestBlackList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestJoinGuild::Dispatcher_RequestJoinGuild() {}
Dispatcher_RequestJoinGuild::~Dispatcher_RequestJoinGuild() {}
int Dispatcher_RequestJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d4e28 (0x14c)：state==3 且 get_charac_guildkey(user)==0 才填
    // SIG_REQUEST_JOIN_GUILD 并 DB_RequestJoinGuild::makeRequest(GetUID,&sig)；
    // guildkey!=0 → SendCmdErrorPacket(0x15d, 0x12)；恒返 0。
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (user->get_charac_guildkey() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15d, 0x12);
        return 0;
    }

    SIG_REQUEST_JOIN_GUILD sig;                 // 栈 0x44（SigTypes_D.h m_pad[0x44]）
    memset(&sig, 0, 0x44);
    // +0x00 char name[0x16] ← msg+0x11
    strncpy((char*)&sig.m_pad[0x00], (char*)&((char*)&msg)[0x11], 0x16);
    // +0x24 characNo = getCurCharacNo(user)
    *reinterpret_cast<int*>(&sig.m_pad[0x24]) = user->getCurCharacNo();
    // +0x28 accId = get_acc_id(user)
    *reinterpret_cast<int*>(&sig.m_pad[0x28]) = user->get_acc_id();
    // +0x2c serverGroup
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        *reinterpret_cast<int*>(&sig.m_pad[0x2c]) = user->GetServerGroup();
    }
    else
    {
        *reinterpret_cast<int*>(&sig.m_pad[0x2c]) =
            *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);  // m_serverEnvir.m_serverGroup
    }
    // +0x30 ssn[2] ← GetSsnString(user)
    memcpy(&sig.m_pad[0x30], user->GetSsnString(), 2);
    // +0x32 char name[0x19] ← msg+0x2c
    strncpy((char*)&sig.m_pad[0x32], (char*)&((char*)&msg)[0x2c], 0x19);

    DB_RequestJoinGuild::makeRequest(user->GetUID(), &sig);
    return 0;
}
int Dispatcher_RequestJoinGuild::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_REQUEST_GUILD_JOIN& data = static_cast<MSG_REQUEST_GUILD_JOIN&>(msg);
    if (!packet.get_int(data.m_0d))
    {
        return LineFunc(0x3506,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(data.m_11, 0x17, data.m_0d))
    {
        return LineFunc(0x3509,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(data.m_28))
    {
        return LineFunc(0x350d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(data.m_2c, 0x1a, data.m_28))
    {
        return LineFunc(0x3510,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}


Dispatcher_ResponseCreature::Dispatcher_ResponseCreature() {}
Dispatcher_ResponseCreature::~Dispatcher_ResponseCreature() {}
int Dispatcher_ResponseCreature::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9acf,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    user->getCurCharacInvenW()->GetCreatureMgrW()->ResponseCreature();
    return 0;
}
int Dispatcher_ResponseCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ResponseCreature::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SDC_Damage_Check::Dispatcher_SDC_Damage_Check() {}
Dispatcher_SDC_Damage_Check::~Dispatcher_SDC_Damage_Check() {}
int Dispatcher_SDC_Damage_Check::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned char count = 0;                            // -0x2d
    int party = 0;                                      // -0x2c 初值 0 后被 var 指针复用（ORIG 同槽）
    if (!packet.get_byte(count))
    {
        return LineFunc(0x6ab5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int capA = 500;                                     // -0x24（ORIG 赋 500，后未再读）
    int capB = 500;                                     // -0x20（同上）
    unsigned long destLen = 500;                        // -0x34
    int srcLen = 500;                                   // -0x1c（循环内被长度覆盖）
    char compBuf[500];                                  // -0x22c 压缩数据缓冲
    long varBuf[125];                                   // -0x420 解压目标（500 字节，ORIG memset 清零而非 ctor）
    memset(compBuf, 0, 0x1f4);
    memset(varBuf, 0, 0x1f4);
    STComputeDamageVariable* var =
        reinterpret_cast<STComputeDamageVariable*>(varBuf);   // -0x2c
    unsigned char loopIdx = 0;                          // -0x25
    for (; loopIdx < count; loopIdx += 1)               // setb：unsigned byte 比较
    {
        int len = 0;                                    // -0x38
        if (!packet.get_int(len))
        {
            return LineFunc(0x6ac9,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (len > 500)
        {
            return LineFunc(0x6acb,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_binary(compBuf, len))
        {
            return LineFunc(0x6ace,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        srcLen = len;                                   // -0x1c ← len
        int uncompressRet = uncompress(
            (unsigned char*)varBuf, &destLen,
            (const unsigned char*)compBuf, (unsigned long)srcLen);   // -0x18
        if (uncompressRet != 0)
        {
            return LineFunc(0x6ad8,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        int hackGate = 0;                               // -0x14
        int expectedHp = var->m_expectedHp;             // -0x10 ← +0x1e6
        hackGate = computeHpDamageSDC(var, 0, 0);
        if (expectedHp < hackGate - 1 || hackGate + 1 < expectedHp)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x25d, 1, 0, 0);
        }
        short partyIdx = user->GetPartyIndex();         // movswl
        CParty* party2 = G_CGameManager()->GetParty((int)partyIdx);   // -0xc
        if (party2 != 0)
        {
            CBattle_Field* field = reinterpret_cast<CBattle_Field*>(
                reinterpret_cast<char*>(party2) + 0xb24);
            field->SetDamagedPacketCnt(var->m_uid);           // +0x1ca
            field->SetDamageValue(var->m_uid, hackGate);
        }
    }
    return 0;
}
int Dispatcher_SDC_Damage_Check::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SDC_Damage_Check::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SaveGameOption_1::Dispatcher_SaveGameOption_1() {}
Dispatcher_SaveGameOption_1::~Dispatcher_SaveGameOption_1() {}
int Dispatcher_SaveGameOption_1::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty = "int Dispatcher_SaveGameOption_1::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x28ad, pretty, 0, 0);
    }
    return user->get_state() < 2 ? -1 : 0;
}
int Dispatcher_SaveGameOption_1::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    CETCOption& etc = *reinterpret_cast<CETCOption*>(reinterpret_cast<char*>(&msg) + 0x11);
    if ((short)etc.getETCOption((ENUM_OPTION_ETC)1) !=
        (unsigned char)user->get_charac_visible_grow_avatar())
    {
        short visible = (short)etc.getETCOption((ENUM_OPTION_ETC)1);
        user->set_charac_visible_grow_avatar(visible != 0);
        user->send_charac_visible_data();
        user->m_characterView.enableSaveCharacView();
    }
    CGameOption* gameOption = user->GetGameOptionRef();
    if ((unsigned char)gameOption->IsEtcOptionChanged(etc) == 1)
    {
        gameOption->SetEtcOption(etc);
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x2899),
                           true);
        guard << 0x130;
        guard << (int)user->GetUID();
        SIG_SAVE_GAME_OPTION_1* save = guard.GetInBuffer<SIG_SAVE_GAME_OPTION_1>();
        *(unsigned int*)save = user->get_acc_id();
        memcpy(reinterpret_cast<char*>(save) + 4, &etc, 0x48);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
    return 0;
}
int Dispatcher_SaveGameOption_1::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cea7a：int@0xd（len）须 <0x49，否则 LineFunc(0x2860)；binary len 字节至 0x11
    char* m = reinterpret_cast<char*>(&msg);
    unsigned int len = 0;
    if (!packet.get_int(len) || len >= 0x49)
        return LineFunc(0x2860, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_binary(m + 0x11, (int)len))
        return LineFunc(0x2862, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_SaveGameOption_2::Dispatcher_SaveGameOption_2() {}
Dispatcher_SaveGameOption_2::~Dispatcher_SaveGameOption_2() {}
int Dispatcher_SaveGameOption_2::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty = "int Dispatcher_SaveGameOption_2::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x28fa, pretty, 0, 0);
    }
    return user->get_state() < 2 ? -1 : 0;
}
int Dispatcher_SaveGameOption_2::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int ret = check_error(user, msg);
    if (ret > 0) {
        return ret;
    }
    if (ret < 0) {
        return -1;
    }
    CGameOption* gameOption = user->GetGameOptionRef();
    int characJob = user->get_charac_job();
    unsigned char hotKeyType = (unsigned char)user->GetCharacHotKeyType(characJob);
    CHotkeyOption* hotkeyOption = reinterpret_cast<CHotkeyOption*>(reinterpret_cast<char*>(&param) + 0x11);
    if (!gameOption->IsHotKeyOptionChanged(hotKeyType, *hotkeyOption)) {
        return 0;
    }
    gameOption->SetHotKeyOption(hotKeyType, *hotkeyOption);
    Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x28e1);
    CStreamGuard guard(stream, true);
    guard << 0x131;
    guard << (int)user->GetUID();
    SIG_LOAD_CHARAC_HOTKEY_OPTION* sig = guard.operator->()->GetInBuffer<SIG_LOAD_CHARAC_HOTKEY_OPTION>();
    char* sigBody = sig->m_pad;
    *(unsigned int*)sigBody = user->get_acc_id();
    int characJob2 = user->get_charac_job();
    sigBody[4] = (char)user->GetCharacHotKeyType(characJob2);
    memcpy(sigBody + 5, hotkeyOption, 0xbc);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;
}
int Dispatcher_SaveGameOption_2::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081ceda4：int@0xd（len）须 <0xbd，否则 LineFunc(0x28bc)；binary len 字节至 0x11
    char* m = reinterpret_cast<char*>(&msg);
    unsigned int len = 0;
    if (!packet.get_int(len) || len >= 0xbd)
        return LineFunc(0x28bc, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_binary(m + 0x11, (int)len))
        return LineFunc(0x28bd, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_UpgradeChronicle::Dispatcher_UpgradeChronicle() {}
Dispatcher_UpgradeChronicle::~Dispatcher_UpgradeChronicle() {}
int Dispatcher_UpgradeChronicle::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08223330：三代编年史强化（WongWork::CItemUpgrade::ProcUpgradeChronicle）。
    if (user == 0 || user->get_state() < 3)
        return 0;

    short slotA = 0;
    if (!packet.get_short(*(unsigned short*)&slotA))
        return LineFunc(0xdda4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned int seqA = 0;
    if (!packet.get_int((int&)seqA))
        return LineFunc(0xdda5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    short slotB = 0;
    if (!packet.get_short(*(unsigned short*)&slotB))
        return LineFunc(0xdda6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned int seqB = 0;
    if (!packet.get_int((int&)seqB))
        return LineFunc(0xdda7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    char chanceGrade = 0;
    if (!packet.get_byte(*(unsigned char*)&chanceGrade))
        return LineFunc(0xdda8,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // ORIG 0x08223484：CSecu_ProtectionField::Check(user, 0x1a)，
    // 非 0 则 SendCmdErrorPacket(0x112, 错误码) 返回 0。
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x1a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, (unsigned char)secuErr);
        return 0;
    }

    if ((int)user->CheckItemLock(1, (int)slotB) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0xd5);
        return 0;
    }

    CInventory* inven = user->getCurCharacInvenW();
    Inven_Item itemA = inven->GetInvenSlot(1, (int)slotA);
    Inven_Item itemB = inven->GetInvenSlot(1, (int)slotB);

    CStackableItem* chronicle =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(itemA.m_addInfo));
    CItem* equipItem = G_CDataManager()->find_item(itemB.m_addInfo);
    if (chronicle == 0 || equipItem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 1);
        return 0;
    }
    if (itemA.m_addInfo == 0 || itemB.m_addInfo == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 4);
        return 0;
    }
    if (!(itemA.m_addInfo == (int)seqA && itemB.m_addInfo == (int)seqB))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 1);
        return 0;
    }

    if (chronicle->IsEmancipate() != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0x11);
        return 0;
    }
    if (equipItem->is_stackable())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 1);
        return 0;
    }
    CEquipItem* equip = static_cast<CEquipItem*>(equipItem);
    if (equip == 0 || equip->IsEmancipate() != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0x11);
        return 0;
    }

    int start = 0;
    int end = 0;
    inven->getStartEndPos(Inven_Item::ITEM_TYPE_EQUIP, start, end);
    if (slotB < start || end < slotB)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0x13);
        return 0;
    }
    if ((unsigned int)itemB.m_fieldb < (unsigned int)equip->get_endurance())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 7);
        return 0;
    }

    stAmplifyOption_t& ampView = itemB.m_amp;   // ORIG asStack_f1 即 itemB 内嵌 amp 视图
    (void)ampView.getAbilityType();             // local_21：调用后弃用（ORIG 留痕）
    if (!ampView.isIdentified())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0xae);
        return 0;
    }

    std::vector<int> updated;
    int rc = G_CDataManager()->m_itemUpgrade->ProcUpgradeChronicle(
        user, slotA, seqA, slotB, seqB, chanceGrade, updated);
    if (rc != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0xae);
        return 0;
    }
    if (updated.size() % 3 == 0)
    {
        PacketGuard packetOut;
        packetOut.put_header(1, 0x112);
        packetOut.put_byte(1);
        packetOut.put_byte((int)((char)(updated.size() / 3) + '\x01'));
        inven->GetInvenSlot(1, (int)slotB);   // ORIG 第二次取槽（写包前置读取）
        packetOut.put_byte(0);
        packetOut.put_short((int)slotA);
        packetOut.put_int(1);
        for (std::vector<int>::const_iterator vIt = updated.begin();
             vIt != updated.end(); ++vIt)
        {
            packetOut.put_byte((int)(char)*vIt);
            ++vIt;
            packetOut.put_short((int)(short)*vIt);
            ++vIt;
            packetOut.put_int(*vIt);
        }
        packetOut.finalize(true);
        user->Send(packetOut);
    }
    return 0;
}
int Dispatcher_UpgradeChronicle::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeChronicle::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeInventory::Dispatcher_UpgradeInventory() {}
Dispatcher_UpgradeInventory::~Dispatcher_UpgradeInventory() {}
int Dispatcher_UpgradeInventory::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0xe828,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    const unsigned int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x1d);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, (unsigned char)(secu & 0xff));
        return 0;
    }

    short slot = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(
            0xe832,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    Inven_Item ticket =
        user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, slot);
    if (ticket.m_addInfo2 < 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x11);
        return 0;
    }

    // 扩背包券仅两种道具 idx：0x2897ca / 0x203b
    if (!(ticket.m_addInfo == 0x2897ca || ticket.m_addInfo == 0x203b))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x11);
        return 0;
    }

    // 背包容量上限 16 格
    if (user->getCurCharacInvenR()->get_inventory_capacity() > 0xf)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x13);
        return 0;
    }

    if (user->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, slot, 1, eItemDelReason_3, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x11);
        return 0;
    }

    // 包 1（0x0e）：删除券后的物品更新广播
    PacketGuard pg;                                  // try 域 @0822a71a..0822a8db
    pg.put_header(0, 0xe);
    pg.put_byte(0);
    pg.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE_EQUIP, slot, pg);
    pg.finalize(true);
    user->Send(pg);

    // 扩容日志（旧容量 → +8），再落实际容量与 DB 请求
    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());
    const int capOld = invenR->get_inventory_capacity();
    user->m_historyLog.ExpandInventory(capOld, capOld + 8, 1);
    invenR->set_inventory_capacity(capOld + 8);
    const int capNew = user->getCurCharacInvenR()->get_inventory_capacity();
    DB_UpdateInventoryCapacity::makeRequest(
        user->GetServerGroup(), (int)user->GetUID(),
        (unsigned int)user->getCurCharacNo(), capNew);

    // 包 2（0x42）：subtype 0xb + 新容量
    pg.clear();
    pg.put_header(0, 0x42);
    pg.put_short(0xb);
    pg.put_short(capNew);
    pg.finalize(true);
    user->Send(pg);
    return 0;
}
int Dispatcher_UpgradeInventory::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeInventory::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AddSocketToAvatar::Dispatcher_AddSocketToAvatar() {}
Dispatcher_AddSocketToAvatar::~Dispatcher_AddSocketToAvatar() {}
int Dispatcher_AddSocketToAvatar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AddSocketToAvatar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionRegistItem::Dispatcher_AuctionRegistItem() {}
Dispatcher_AuctionRegistItem::~Dispatcher_AuctionRegistItem() {}


Dispatcher_ChangeHostWarRoom::Dispatcher_ChangeHostWarRoom() {}
Dispatcher_ChangeHostWarRoom::~Dispatcher_ChangeHostWarRoom() {}
int Dispatcher_ChangeHostWarRoom::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty =
        "int Dispatcher_ChangeHostWarRoom::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x41b1, pretty, 0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x41b4, pretty, 0, 0);
    }
    const MSG_CHANGE_HOST_WARROOM& req =
        static_cast<const MSG_CHANGE_HOST_WARROOM&>(msg);
    signed char warRoom = static_cast<signed char>(req.m_pad[0]);
    if (warRoom < 0 || warRoom > 5)
    {
        return LineFunc(0x41b8, pretty, 0, user->get_acc_id());
    }
    return user->get_state() == 8 ? 0 : -1;
}
int Dispatcher_ChangeHostWarRoom::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err;
    short roomIdx;
    CGameManager* mgr;
    WarRoom* room;
    int result;

    err = check_error(user, msg);
    if (err > 0)
        return err;
    if (err < 0)
        return -1;
    roomIdx = (short)user->GetWarRoomIndex();
    mgr = G_CGameManager();
    room = mgr->GetWarRoom(roomIdx);
    if (room == 0)
    {
        LineFunc(0x41a7,
                 __PRETTY_FUNCTION__,
                 0, user->get_acc_id());
    }
    else
    {
        result = room->HostChangeWarRoom(
            user,
            *(reinterpret_cast<char*>(&msg) + 0xd),
            *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(&msg) + 0xe));
        *(reinterpret_cast<int*>(&param) + 1) = result;
    }
    return 0;
}
int Dispatcher_ChangeHostWarRoom::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(p + 0xd)))
    {
        return LineFunc(0x418e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0xe)))
    {
        return LineFunc(0x4191,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void Dispatcher_ChangeHostWarRoom::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stChangeHostWarRoom& p = static_cast<stChangeHostWarRoom&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x194, (unsigned char)p.m_4);
}

Dispatcher_ConnectP2PAssault::Dispatcher_ConnectP2PAssault() {}
Dispatcher_ConnectP2PAssault::~Dispatcher_ConnectP2PAssault() {}
int Dispatcher_ConnectP2PAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa1b0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char count = -1;                                       // -0xd（local_19[8]）
    if (!packet.get_byte(count))
    {
        return LineFunc(0xa1b4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (count > 8 || count < 0)
    {
        return LineFunc(0xa1b8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char slots[8];                                         // -0x15..-0xe（base -0x15）
    slots[0] = 0; slots[1] = 0; slots[2] = 0; slots[3] = 0;
    slots[4] = 0; slots[5] = 0; slots[6] = 0; slots[7] = 0;
    char team = -1;                                        // -0x16（local_1a）
    for (int i = 0; i < (int)count; i++)
    {
        if (!packet.get_byte(team))
        {
            return LineFunc(0xa1bf,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (team > 7 || team < 0)
        {
            return LineFunc(0xa1c2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(slots[(int)team]))
        {
            return LineFunc(0xa1c5,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    pvp_assault::CAssaultMgr* mgr = pvp_assault::GetInstanceAssaultMgr();   // 0x082350a0
    if (mgr->OnConnectP2P(user, slots, (int)count) != 1)
    {
        return LineFunc(0xa1cb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

int Dispatcher_ConnectP2PAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ConnectP2PAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Debug_Clear_Quest::Dispatcher_Debug_Clear_Quest() {}
Dispatcher_Debug_Clear_Quest::~Dispatcher_Debug_Clear_Quest() {}
// ORIG 0x081df91c：isGMUser 否 → LineFunc(0x51f3) 返回其值；
// res.m_index = msg（MsgClearQuestIndex::operator=，ORIG W 0x08235266）；
// 遍历 m_questIdxs：find_quest 失败 → return 1；exposeQuest &&
// check_possible → getCurCharacQuestW()->m_cleared.setClearedQuest(idx)、
// m_field138>0 → incQuestPoint(值,idx,true)、m_field13a>0 →
// incQuestPiece(值,idx,true)，计数；循环后 res.m_index.m_count=实际清空数，
// sendCharacQp() + sendCharacQuestPiece()，return 0。
int Dispatcher_Debug_Clear_Quest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (!user->isGMUser())
    {
        return LineFunc(0x51f3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    MsgClearQuestIndex& req = static_cast<MsgClearQuestIndex&>(msg);
    ReqDebugClearQuestResult& res = static_cast<ReqDebugClearQuestResult&>(param);

    // ORIG 0x08235266：res.m_index = msg（逐标量 + vector operator=）。
    res.m_index = req;

    int clearedCount = 0;
    for (int i = 0; i < req.m_count; ++i)
    {
        int questIdx = req.m_questIdxs[i];
        Quest* quest = G_CDataManager()->find_quest(questIdx);
        if (quest == 0)
        {
            return 1;
        }
        stSelectQuestParam selectParam(user);
        if (quest->exposeQuest() && quest->check_possible(selectParam))
        {
            user->getCurCharacQuestW()->m_cleared.setClearedQuest(
                (unsigned int)questIdx);
            if (quest->m_field138 > 0)
            {
                user->incQuestPoint((int)quest->m_field138, questIdx, true);
            }
            if (quest->m_field13a > 0)
            {
                user->incQuestPiece((int)quest->m_field13a, questIdx, true);
            }
            ++clearedCount;
        }
    }
    res.m_index.m_count = clearedCount;   // ORIG 写 *(int*)(param+0x14)
    user->sendCharacQp();
    user->sendCharacQuestPiece();
    return 0;
}
// ORIG 0x081df840：get_short 读数量写入 msg.m_count（+0x10），再逐个
// get_short push_back 进 m_questIdxs；失败走 LineFunc(0x51e1/0x51e8)。
int Dispatcher_Debug_Clear_Quest::read(PacketBuf& packet, MSG_BASE& msg)
{
    MsgClearQuestIndex& req = static_cast<MsgClearQuestIndex&>(msg);
    short count = 0;
    if (!packet.get_short(count))
    {
        return LineFunc(0x51e1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    req.m_count = count;
    for (int i = 0; i < req.m_count; ++i)
    {
        short value = 0;
        if (!packet.get_short(value))
        {
            return LineFunc(0x51e8,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        req.m_questIdxs.push_back((int)value);
    }
    return 0;
}
// ORIG 0x081dfb40：PacketGuard put_header(1,0x1c9)+put_byte(1)+
// put_short(res.m_index.m_count)；按 count put_short 每个 idx；
// finalize(true)→Send→send_clear_quest_list()；随后第二个 PacketGuard 经
// UserQuest::get_quest_info 填充任务列表包并发送。
void Dispatcher_Debug_Clear_Quest::send(CUser* user, ParamBase& param)
{
    const ReqDebugClearQuestResult& res = static_cast<const ReqDebugClearQuestResult&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1c9);
    packet.put_byte(1);
    packet.put_short(res.m_index.m_count);
    for (int i = 0; i < res.m_index.m_count; ++i)
    {
        packet.put_short(res.m_index.m_questIdxs[i]);
    }
    packet.finalize(true);
    user->Send(packet);
    user->send_clear_quest_list();

    PacketGuard questList;   // ORIG local_2c：UserQuest 任务列表包
    user->getCurCharacQuestR()->get_quest_info(reinterpret_cast<char*>(&questList));
    user->Send(questList);
}

Dispatcher_DeleteToBlackList::Dispatcher_DeleteToBlackList() {}
Dispatcher_DeleteToBlackList::~Dispatcher_DeleteToBlackList() {}
int Dispatcher_DeleteToBlackList::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() >= 3 ? 0 : -1;
}
int Dispatcher_DeleteToBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cdf0a
    int err = check_error(user, msg);
    *(int*)((char*)&param + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x26ea,
                        __PRETTY_FUNCTION__,
                        err, user->get_acc_id());
    }

    // Packet_Delete_To_BlackList local_40（ORIG 栈对象 sizeof 0x2c；字段为裸偏移）
    Packet_Delete_To_BlackList packet;
    // ORIG 实测 2026-08-30：accId → packet+0x0a（C1 memset 区 +0x0e 起 0x1e 之前）；
    // name → packet+0x0e（C1 memset 的 0x1e 字节区）。NEW 曾误写 +0x06/+0x0a（覆盖 connNo/accId），已修正。
    *(unsigned int*)((char*)&packet + 0x0a) = user->get_acc_id();
    strncpy((char*)&packet + 0x0e,
            (const char*)((char*)&msg + 0x14),
            (size_t)*(unsigned int*)((char*)&msg + 0x10));

    ENUM_SERVER_GROUP group = (ENUM_SERVER_GROUP)user->GetServerGroup();
    CMonitorServerProxy* monitor = GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
    monitor->SendPacket((char*)&packet, 0x2c);
    CGuildServerProxy* guild = GlobalData::s_guild_proxy_mgr->GetServerProxy(group);
    guild->SendPacket((char*)&packet, 0x2c);
    return 0;
}
int Dispatcher_DeleteToBlackList::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cde36：int@0x10（len）→ str@0x14（max 0x1e）
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x10)))
        return LineFunc(0x26ce, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m + 0x14, 0x1e, *reinterpret_cast<int*>(m + 0x10)))
        return LineFunc(0x26d1, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_EnterPrivateStore::Dispatcher_EnterPrivateStore() {}
Dispatcher_EnterPrivateStore::~Dispatcher_EnterPrivateStore() {}
int Dispatcher_EnterPrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    bool gate;
    short sellerIndex;
    private_store::IBuyer* buyer;
    private_store::CPrivateStoreMgr* mgr;
    bool ok;

    if (user->get_state() == 3)
    {
        if (user->getCurCharacR() != 0)
            gate = false;
        else
            gate = true;
    }
    else
    {
        gate = true;
    }
    if (gate)
    {
        LineFunc(0x98d4,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    sellerIndex = 0;
    buyer = 0;
    ok = packet.get_short(sellerIndex);
    if (!ok)
    {
        LineFunc(0x98d8,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    mgr = private_store::GetInstancePrivateStoreMgr();
    mgr->GetStoreBuyer(&buyer, user, (unsigned short)sellerIndex);
    buyer->EnterPrivateStore();
    return 0;
}
int Dispatcher_EnterPrivateStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_EnterPrivateStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_FatigueAttendance::Dispatcher_FatigueAttendance() {}
Dispatcher_FatigueAttendance::~Dispatcher_FatigueAttendance() {}

Dispatcher_GuildCargoHistory::Dispatcher_GuildCargoHistory() {}
Dispatcher_GuildCargoHistory::~Dispatcher_GuildCargoHistory() {}
int Dispatcher_GuildCargoHistory::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d0cb4：param+4 = check_error(...)；>0 返 0；<0 LineFunc(0x2c6d,err,0)；
    // ==0 构造 Packet_Guild_Cargo_History_Request，guildkey(+6)/characNo(+0xa)，
    // SendTcpPacket(pkt, *(ushort*)(pkt+2))，历史日志 reason=3 后返 0。
    int err = check_error(user, msg, param);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x2c6d,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    Packet_Guild_Cargo_History_Request pkt;   // ORIG W ctor 0x0822be8a（局部帧 -0x1e 起）
    // 帧偏移：-0x1c=pkt+0x2(ushort size)、-0x18=pkt+0x6(guildkey)、-0x14=pkt+0xa(characNo)
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 6) =
        user->get_charac_guildkey();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 0xa) =
        user->getCurCharacNo();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt),
                         *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&pkt) + 2));
    user->m_historyLog.GuildCargoHistory(static_cast<ENUM_GUILD_CARGO_HISTORY_REASON>(3),
                                         user->get_charac_guildkey());
    return 0;
}
int Dispatcher_GuildCargoHistory::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GuildCargoHistory::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stGuildCargoHistoryParam& p = static_cast<stGuildCargoHistoryParam&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x109, (unsigned char)p.m_4);
}

Dispatcher_GuildCargoPopItem::Dispatcher_GuildCargoPopItem() {}
Dispatcher_GuildCargoPopItem::~Dispatcher_GuildCargoPopItem() {}
int Dispatcher_GuildCargoPopItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stGuildCargoPopItemParam& out = static_cast<stGuildCargoPopItemParam&>(param);
    out.m_4 = check_error(user, msg, param);            // ORIG 0x081d14aa
    if (out.m_4 > 0)
    {
        return 0;                                        // jle 取反分支：err>0 直接返回
    }
    if (out.m_4 < 0)
    {
        LineFunc(0x2d8f,
                 __PRETTY_FUNCTION__,
                 out.m_4, 0);
        return 0;
    }

    MSG_GUILD_CARGO_POP_ITEM& req = static_cast<MSG_GUILD_CARGO_POP_ITEM&>(msg);
    CItem* item = G_CDataManager()->find_item(req.m_itemIdx);   // msg+0x0f

    GuildCargoPopItem_PacketView pkt;
    pkt.m_guildKey = user->get_charac_guildkey();               // → +0x0a
    pkt.m_charNo = user->getCurCharacNo();                      // → +0x0e
    pkt.m_slot = (int)req.m_field0d;                            // msg+0x0d ushort → +0x12
    pkt.m_id = req.m_itemIdx;                                   // msg+0x0f → +0x16
    pkt.m_count = req.m_field13;                                // msg+0x13 → +0x1a
    pkt.m_itemType = item->is_stackable() ? 1 : 0;              // CItem::is_stackable → +0x1e
    pkt.m_uit = req.m_field17;                                  // msg+0x17 → +0x1f

    CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(
        user->GetServerGroup());
    proxy->SendTcpPacket((char*)&pkt, (int)pkt.packetSize);     // 长度=*(ushort*)(pkt+2)=0x23

    user->m_historyLog.GuildCargoPopItem(
        (ENUM_GUILD_CARGO_HISTORY_REASON)0xa,
        (int)req.m_field0d, req.m_itemIdx, req.m_field13,
        (int)pkt.m_itemType, (int)req.m_field17);               // ORIG 0x086866ea
    return 0;
}

int Dispatcher_GuildCargoPopItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d1388：ushort@0xd → int@0xf → int@0x13 → byte@0x17
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(m + 0xd)))
        return LineFunc(0x2d3b, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0xf)))
        return LineFunc(0x2d3c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x13)))
        return LineFunc(0x2d3d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0x17)))
        return LineFunc(0x2d3e, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_GuildCargoPopItem::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d1748 T：err=*(int*)((char*)&param+4)；0 → 静默；0x64/0xb5 →
    // SendCmdErrorPacket(0x109,(unsigned char)err)；其余 → SendCmdErrorPacket(0xfc,…)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
        return;
    if (err == 0x64 || err == 0xb5)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x109, (unsigned char)err);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfc, (unsigned char)err);
}

Dispatcher_LoadingTimeReport::Dispatcher_LoadingTimeReport() {}
Dispatcher_LoadingTimeReport::~Dispatcher_LoadingTimeReport() {}
int Dispatcher_LoadingTimeReport::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821efec [0x0821efec, 0x0821f611]
    Packet_Loading_Time_Report_Statistics local_824;   // ctor ORIG 0x0822c040（弱内联）
    // 布局无头文件证据，按 ORIG 裸偏移访问（对象栈帧 -0x820 起，payload 尺寸 0x806）。
    unsigned char* base = reinterpret_cast<unsigned char*>(&local_824);
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
int Dispatcher_LoadingTimeReport::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_LoadingTimeReport::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PeerConnectResult::Dispatcher_PeerConnectResult() {}
Dispatcher_PeerConnectResult::~Dispatcher_PeerConnectResult() {}
int Dispatcher_PeerConnectResult::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082033e6：连读 get_byte(result)/get_short(uniqueId)，任一失败
    // → LineFunc(0x9a1c / 0x9a1f, 0, 0)；find_from_world(uniqueId)==0 返 0；
    // 取双方 nat_type（movsbl 扩展）+ result 字节 → 统计器 IncreaseNatType 后返 0。
    unsigned char result = 0;
    unsigned short uniqueId = 0;
    if (!packet.get_byte(result))
    {
        return LineFunc(0x9a1c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&uniqueId)))
    {
        return LineFunc(0x9a1f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CUser* peer = G_GameWorld()->find_from_world(uniqueId);
    if (peer == 0)
    {
        return 0;
    }
    // asm：movsbl nat 值 → int（有符号字节扩展）
    int localNat = static_cast<int>(static_cast<signed char>(user->m_network.get_nat_type()));
    int peerNat = static_cast<int>(static_cast<signed char>(peer->m_network.get_nat_type()));
    StatisticsNatType* stats = G_GameWorld()->GetStatisticsNatType();
    stats->IncreaseNatType(localNat, peerNat, (unsigned char)result);
    return 0;
}
int Dispatcher_PeerConnectResult::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PeerConnectResult::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PutItemForEnchant::Dispatcher_PutItemForEnchant() {}
Dispatcher_PutItemForEnchant::~Dispatcher_PutItemForEnchant() {}
int Dispatcher_PutItemForEnchant::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820b09a
    MSG_STATIC_COMMAND* msg = (MSG_STATIC_COMMAND*)packet.get_packet(0);
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xa744,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    expert_job::CExpertJob* job = user->GetCurCharacExpertJob();
    if (job == 0)
    {
        return 0;
    }
    if (job->GetType() != 1)
    {
        return LineFunc(0xa74b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    expert_job::CEnchanter* enchanter = static_cast<expert_job::CEnchanter*>(
        user->GetCurCharacExpertJob());
    enchanter->OnPutItemForEnchant(user, msg);
    return 0;
}
int Dispatcher_PutItemForEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PutItemForEnchant::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestItemUnlock::Dispatcher_RequestItemUnlock() {}
Dispatcher_RequestItemUnlock::~Dispatcher_RequestItemUnlock() {}
int Dispatcher_RequestItemUnlock::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3)
        return 0;
    char slot = 0;                                   // -0x11
    short itemIdx = 0;                               // -0x14
    if (!packet.get_byte(slot))
        return LineFunc(0xdd4c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(itemIdx))
        return LineFunc(0xdd4d, __PRETTY_FUNCTION__, 0, 0);
    // SECURITY_PROTCTION 0x18（ItemUnlock 门禁）
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x18);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x110, (unsigned char)secu);
        return 0;
    }
    int err = 0;                                     // -0x18（IsAbleItemUnlock 出参）
    int able = user->IsAbleItemUnlock(slot, itemIdx, err);
    if (able != 0)
    {
        PacketGuard packet_;                         // -0x24
        packet_.put_header(1, 0x110);
        packet_.put_byte(0);
        packet_.put_byte(able);
        packet_.put_int(err);
        packet_.finalize(true);
        user->Send(packet_);
        return 0;
    }
    user->DoItemUnlock(slot, itemIdx);
    return 0;
}

int Dispatcher_RequestItemUnlock::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestItemUnlock::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SecretShopBuyItem::Dispatcher_SecretShopBuyItem() {}
Dispatcher_SecretShopBuyItem::~Dispatcher_SecretShopBuyItem() {}
// ORIG 0x081d26ea T _ZN28Dispatcher_SecretShopBuyItem7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_SecretShopBuyItem/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 stSecretShopBuyItemParam，ORIG 裸访问 +0x04 错误码槽（m_4）。
// 调用同 TU 模板 check_error(CUser*, MSG_BASE&, ParamBase&)（ORIG check_error(CUser*,MSG_BASE&,ParamBase&)）。
int Dispatcher_SecretShopBuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    *(int*)((char*)&param + 4) = check_error(user, msg, param);
    int err = *(int*)((char*)&param + 4);
    if (err >= 1)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x2fa1,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    secretshop::SECRET_SHOP_DATA* data =
        static_cast<secretshop::SECRET_SHOP_DATA*>(user->GetSecretShopData());
    secretshop::SECRET_SHOP_INFO* info =
        static_cast<secretshop::SECRET_SHOP_INFO*>(user->GetUserSecretShop());
    if (data == 0 || info == 0)
    {
        *(int*)((char*)&param + 4) = 0x7fffffff;
        return 0;
    }
    if (data->IsOpen() != 0)
    {
        GlobalData::s_secret_shop->BuyItem(user, *info,
                                           *(int*)((char*)&msg + 0xd),
                                           *(int*)((char*)&msg + 0x11));
    }
    return 0;
}
int Dispatcher_SecretShopBuyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d25f8：int@0xd → int@0x11（须 >=1 否则 LineFunc 0x2f84）
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0xd)))
        return LineFunc(0x2f80, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x11)))
        return LineFunc(0x2f81, __PRETTY_FUNCTION__, 0, 0);
    if (*reinterpret_cast<int*>(m + 0x11) < 1)
        return LineFunc(0x2f84, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_SecretShopBuyItem::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d27ec T：err=*(int*)((char*)&param+4)；err==0x7fffffff 或 0 → 不发包，
    // 否则 SendCmdErrorPacket(0x129, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0x7fffffff)
        return;
    if (err != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, (unsigned char)err);
}

Dispatcher_SecurityCardIssue::Dispatcher_SecurityCardIssue() {}
Dispatcher_SecurityCardIssue::~Dispatcher_SecurityCardIssue() {}
int Dispatcher_SecurityCardIssue::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08209c8c（docs/class_func_reports/Dispatcher_SecurityCardIssue/dispatch_sig.md）
    CSHA1 sha1;                               // local_168 -0x164
    char sha1Hex[0x29];                       // local_a3 -0x9f
    char cardData[0x27];                      // local_7a -0x76
    char serverKey[0x21];                     // local_53/local_52 -0x4f
    char certKeyPhone[0x12];                  // local_32 -0x2e：[0..5]=certKey、[6..]=phone(0xc)
    unsigned int phoneLen = 0;                // local_20 -0x1c
    bool fail;                                // bVar11

    // +0x00：state != 3 → 拒绝
    if (user->get_state() != 3)
        return LineFunc(0xa596,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x4c：清零局部
    phoneLen = 0;
    memset(certKeyPhone + 6, 0, 0xc);         // local_32[6..0x11]

    // +0x6f：读 phone 长度
    if (!packet.get_int((unsigned int&)phoneLen))
        return LineFunc(0xa59f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0xac：读 phone
    if (!packet.get_str(certKeyPhone + 6, 0xc, phoneLen))
        return LineFunc(0xa5a0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0xff：号码格式检查
    if (!GlobalData::s_securityCardCenter->checkPhoneNumber(certKeyPhone + 6))
        return LineFunc(0xa5a3,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x145：SHA1(phone) → 40 字符十六进制
    memset(sha1Hex, 0, 0x29);                 // 0807dcc0
    sha1.Update((unsigned char const*)certKeyPhone + 6, phoneLen);
    sha1.Final();
    sha1.GetHexHash(sha1Hex);

    // +0x1b3：安全卡状态检查
    if (user->getSecurityCard()->isActivate())
    {
        if (!user->getSecurityCard()->isCertified())
            return LineFunc(0xa5b3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (2 < user->getSecurityCard()->getFailCnt())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x7a);
            return 0;
        }
    }
    // +0x2f1：Pad 状态检查
    if (user->getPad()->isActivate())
    {
        if (2 < user->getPad()->getFailCnt())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x88);
            return 0;
        }
        if (!user->getPad()->isCertified())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x86);
            return 0;
        }
    }

    // +0x391：清零 certKey（前 6 字节）/serverKey/cardData 缓冲（ORIG 展开清零）
    memset(certKeyPhone, 0, 6);
    memset(serverKey, 0, 0x21);
    memset(cardData, 0, 0x27);
    // +0x39d：生成卡数据/服务器密钥/证书密钥
    char* pCardData = GlobalData::s_securityCardCenter->generateCardData(cardData);
    char* pServerKey = GlobalData::s_securityCardCenter->generateServerKey(serverKey);
    char* pCertKey = GlobalData::s_securityCardCenter->generateCertKey(certKeyPhone);
    // +0x3e3：DB 请求
    DB_SecurityCardIssue::makeRequest((unsigned int)user->GetUID(),
                                      user->get_acc_id(),
                                      pCertKey, pServerKey,
                                      certKeyPhone + 6, sha1Hex, pCardData);
    return 0;
}
int Dispatcher_SecurityCardIssue::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardIssue::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SetSubGuildMaster::Dispatcher_SetSubGuildMaster() {}
Dispatcher_SetSubGuildMaster::~Dispatcher_SetSubGuildMaster() {}
int Dispatcher_SetSubGuildMaster::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3 || user->getCurCharacR() == 0) {
        return LineFunc(0x9d97, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x082053e6：0x31 连续缓冲（Packet_Monitor_Set_Sub_Guild_Master 头 + 负载数据区）。
    char buffer[0x31];
    Packet_Monitor_Set_Sub_Guild_Master* header = new (buffer) Packet_Monitor_Set_Sub_Guild_Master();
    (void)header;
    int& characNo = *reinterpret_cast<int*>(buffer + 0x6);
    int& guildKey = *reinterpret_cast<int*>(buffer + 0xa);
    char* name = buffer + 0xe;
    char& flag = *reinterpret_cast<char*>(buffer + 0x2c);
    characNo = user->get_charac_no(-1);
    guildKey = user->get_charac_guildkey();
    int nameLen = 0;
    if (!packet.get_int(nameLen)) {
        return LineFunc(0x9da1, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_str(name, 0x1e, nameLen)) {
        return LineFunc(0x9da4, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(flag)) {
        return LineFunc(0x9da7, __PRETTY_FUNCTION__, 0, 0);
    }
    if (flag == (char)0x01) {
        return 0;
    }
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(buffer, 0x31);
    return 0;
}
int Dispatcher_SetSubGuildMaster::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SetSubGuildMaster::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeGuildCargo::Dispatcher_UpgradeGuildCargo() {}
Dispatcher_UpgradeGuildCargo::~Dispatcher_UpgradeGuildCargo() {}
int Dispatcher_UpgradeGuildCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    Inven_Item local_99;                 // -0x95：GetInvenData 输出
    PacketGuard local_5c;                // -0x58
    Packet_Guild_Guild_Cargo_Upgrade local_4e;   // -0x4a
    char cVar2;
    int local_2c;                        // -0x2c：getGuildCargoUpradeInfo 结果
    unsigned int local_28;               // -0x28：info+8（扣减点数）
    int local_24;                        // -0x24：info+0xc（所需物品数量）
    unsigned int local_20;               // -0x20：info+4（所需 PowerWar 点数）
    bool bVar1;

    int iVar3 = user->get_state();
    if (!(iVar3 == 3 && user->getCurCharacR() != 0))
    {
        LineFunc(0xdc7c, __PRETTY_FUNCTION__, 0, 0);
        return 0;
    }
    if (user->get_charac_guildkey() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 100);
        return 0;
    }
    cVar2 = (char)user->IsExistGuildAgit();
    if (cVar2 != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xb5);
        return 0;
    }
    cVar2 = (char)user->IsGuildMaster();
    if (cVar2 != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0x24);
        return 0;
    }
    int iVar5 = user->GetGuildCargoCapacity();
    // GuildParameterScript::getGuildCargoUpradeInfo（CDataManager::m_guildParameter +0xa50c）
    local_2c = (int)G_CDataManager()->m_guildParameter.getGuildCargoUpradeInfo(iVar5);
    if (local_2c == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 1);
        return 0;
    }
    local_28 = *(unsigned int*)(local_2c + 8);
    local_24 = *(int*)(local_2c + 0xc);
    local_20 = *(unsigned int*)(local_2c + 4);
    unsigned int local_2c_val = local_28;
    unsigned int uVar6 = (unsigned int)user->GetGuildPowerWarPoint();
    if (uVar6 < local_28)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xb3);
        return 0;
    }
    Inven_Item local_99v;
    CInventory* pCVar7 = (CInventory*)user->getCurCharacInvenR();
    int local_20v = pCVar7->GetInvenData(0xcbd, local_99v);
    if ((local_24 != 0) && (local_20v == -1 || local_99v.m_addInfo2 < local_24))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xb9);
        return 0;
    }
    int iVar3b = (int)user->GetGuildDBInfo();
    if (*(unsigned int*)(iVar3b + 0xa0) < local_20)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xba);
        return 0;
    }
    if (local_24 != 0)
    {
        pCVar7 = user->getCurCharacInvenW();
        cVar2 = (char)pCVar7->delete_item((INVEN_TYPE)1, local_20v, local_24, (eItemDelReason)0x20, 1);
        if (cVar2 != 1)
        {
            bVar1 = true;
        }
        else
        {
            bVar1 = false;
        }
    }
    else
    {
        bVar1 = false;
    }
    if (bVar1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xb9);
        return 0;
    }
    PacketGuard local_5cv;
    local_5cv.put_header(0, 0xe);
    local_5cv.put_byte(0);
    local_5cv.put_short(1);
    pCVar7 = (CInventory*)user->getCurCharacInvenR();
    pCVar7->MakeItemPacket((INVEN_TYPE)1, local_20v, local_5cv);
    local_5cv.finalize(true);
    user->Send(local_5cv);
    user->DecreaseGuildPowerWarPoint(local_2c_val);
    int iVar3c = -(int)local_2c_val;
    unsigned int uVar8 = (unsigned int)user->get_charac_no(-1);
    unsigned int uVar6b = user->get_charac_guildkey();
    ENUM_SERVER_GROUP uVar4 = user->GetServerGroup();
    CGuildServerProxy* pCVar9 =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(uVar4);
    pCVar9->SendIncreaseDecreasePowerWarPoint(uVar6b, uVar8, iVar3c);
    Packet_Guild_Guild_Cargo_Upgrade local_4ev;
    local_4ev.m_guildKey = user->get_charac_guildkey();
    local_4ev.m_charNo = user->getCurCharacNo();
    local_4ev.m_curCapacity = user->GetGuildCargoCapacity();
    local_4ev.m_newCapacity = user->GetGuildCargoCapacity() + *(int*)(local_2c + 0x10);
    local_4ev.m_field1a = *(unsigned int*)(local_2c + 4);
    pCVar9 = GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    pCVar9->SendTcpPacket((char*)&local_4ev,
                          (unsigned int)*(unsigned short*)((char*)&local_4ev + 2));
    return 0;
}
int Dispatcher_UpgradeGuildCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeGuildCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseVendingMachine::Dispatcher_UseVendingMachine()
{
    // ORIG 0x0821c25c：基类 IPacketDispatcher + vtable + this+0x04 = new CMTRand(time(0))
    // （ORIG D1 0x082415f0 不 delete 该指针，忠实保留）
    unsigned long seed = time(0);
    m_rand = new CMTRand(seed);
}
Dispatcher_UseVendingMachine::~Dispatcher_UseVendingMachine() {}


Dispatcher_AchievementTrigger::Dispatcher_AchievementTrigger() {}
Dispatcher_AchievementTrigger::~Dispatcher_AchievementTrigger() {}
// ORIG 0x081d7f1e（报告 Dispatcher_AchievementTrigger/process.md）：
// msg 为 MSG_ACHIEVEMENT_TRIGGER（m_field0d/11/13/15）；state<=2 → 0。
int Dispatcher_AchievementTrigger::process(CUser* user, MSG_BASE& msg,
                                           ParamBase& param)
{
    (void)param;
    if (user->get_state() <= 2)
    {
        return 0;
    }
    MSG_ACHIEVEMENT_TRIGGER& m = static_cast<MSG_ACHIEVEMENT_TRIGGER&>(msg);
    unsigned short t15 = (unsigned short)m.m_field15;
    unsigned short t13 = (unsigned short)m.m_field13;
    unsigned short t11 = (unsigned short)m.m_field11;
    unsigned int id = m.m_field0d;
    CAchievement* ach =
        (CAchievement*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xf);
    ach->setTrigger(id, t11, t13, t15);
    return 0;
}
int Dispatcher_AchievementTrigger::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d7dfc：uint@0xd → ushort@0x11 → ushort@0x13 → ushort@0x15
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(m + 0xd)))
        return LineFunc(0x4265, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(m + 0x11)))
        return LineFunc(0x4267, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(m + 0x13)))
        return LineFunc(0x4269, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(m + 0x15)))
        return LineFunc(0x426b, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_Antibot_DPCallBack::Dispatcher_Antibot_DPCallBack() {}
Dispatcher_Antibot_DPCallBack::~Dispatcher_Antibot_DPCallBack() {}
int Dispatcher_Antibot_DPCallBack::dispatch_sig(CUser* user, PacketBuf& packet)
{
    typedef int (*CheckFn)(void*, int, char*, unsigned short, char*, short*);  // vtable +0x2c
    char& retryCnt = user->m_expReward.m_field5;                               // +0x8d252（ORIG 有符号字节）
    if (retryCnt < 5)
    {
        retryCnt = static_cast<unsigned char>(retryCnt + 1);
    }
    unsigned int value = 0;                                                    // -0x14
    if (!packet.get_int(value))
    {
        if (retryCnt < 6)
        {
            user->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)1,
                                            0x25d, retryCnt, 0);
        }
        return 0xa76a;
    }
    const int dummySeq = 0xd;                                                  // -0x10
    if (value > 0x567)
    {
        if (retryCnt < 6)
        {
            user->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)2,
                                            0x25d, retryCnt,
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
    void* checker = GlobalData::s_antibotChecker_;
    CheckFn fn = *(CheckFn*)((*(void***)checker)[0x2c / 4]);
    const int checkRet =
        fn(checker, user->getAntibotKey(), bufPtr,
           static_cast<unsigned short>(value), outBuf, &hashLen);
    if (checkRet != 0)
    {
        return 0;
    }
    return 0;
}
int Dispatcher_Antibot_DPCallBack::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Antibot_DPCallBack::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_BiddingRoutingItem::Dispatcher_BiddingRoutingItem() {}
Dispatcher_BiddingRoutingItem::~Dispatcher_BiddingRoutingItem() {}
int Dispatcher_BiddingRoutingItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int state = user->get_state();
    if (state != 5 && state != 0xc)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x12d, 0x13);
        return 0;
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x12d, 0x13);
        return 0;
    }
    char selection = 0;                              // -0x11
    if (!packet.get_byte(selection))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x12d, 0x13);
        return 0;
    }
    CSpecialItemRoutingManager* routing = G_CGameManager()->GetSpecialItemRoutingManager();
    if (!routing->ArriveMemberSelection(user, party, selection))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x12d, 0x13);
        return 0;
    }
    PacketGuard packet_;                             // -0x20
    packet_.put_header(1, 0x12d);
    packet_.put_byte(1);
    packet_.finalize(true);
    user->Send(packet_);
    return 0;
}

int Dispatcher_BiddingRoutingItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BiddingRoutingItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Cancel_Quick_Party::Dispatcher_Cancel_Quick_Party() {}
Dispatcher_Cancel_Quick_Party::~Dispatcher_Cancel_Quick_Party() {}
// ORIG 0x081de1b4（nm 实测，sz 0x43）：state>3 且 getCurCharacR()!=0 → 0，否则 -1。
int Dispatcher_Cancel_Quick_Party::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() <= 3 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x081de1f8（报告 Dispatcher_Cancel_Quick_Party/process.md）：
// check_error 同型；cancel_quick_party(party, true, user) != 1 → param+4 = 1。
int Dispatcher_Cancel_Quick_Party::process(CUser* user, MSG_BASE& msg,
                                           ParamBase& param)
{
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    CParty* party = (CParty*)user->GetParty();
    CGameManager* gm = G_CGameManager();
    QuickParty::CQuickPartySystemManager* mgr = gm->GetQuickPartySystemManager();
    if (mgr->cancel_quick_party(party, true, user) != 1)
    {
        *(int*)((char*)&param + 0x4) = 1;
    }
    return 0;
}
int Dispatcher_Cancel_Quick_Party::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Cancel_Quick_Party::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081de298 T：err=*(int*)((char*)&param+4)；0→SendCmdOkPacket(0x1bd)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1bd);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1bd, (unsigned char)err);
}

Dispatcher_ChangeRandomOption::Dispatcher_ChangeRandomOption() {}
Dispatcher_ChangeRandomOption::~Dispatcher_ChangeRandomOption() {}
int Dispatcher_ChangeRandomOption::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_CHANGE_RANDOM_OPTION* local_38 = (MSG_CHANGE_RANDOM_OPTION*)&msg;
    Inven_Item* local_34;                // -0x30：GetInvenRef 结果
    CItem* local_30;                     // -0x2c：find_item 结果
    unsigned char local_29;              // -0x25：选项序号（msg+0x12）
    unsigned int local_28;               // -0x24：同上（uint 视图）
    unsigned int local_24;               // -0x20：getOptionModificationCost
    int local_20;                        // -0x1c：Secu Check
    char cVar4;
    bool bVar3;

    int iVar6 = user->get_state();
    if (iVar6 <= 2 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->CheckInTrade())
    {
        return 0;
    }
    if (user->CheckItemLock(1, (unsigned int)local_38->m_0e) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0xd5);
        return 0;
    }
    unsigned int uVar1 = (unsigned int)local_38->m_0e;
    CInventory* pCVar7 = user->getCurCharacInvenW();
    local_34 = pCVar7->GetInvenRef(1, (int)uVar1);
    if (local_34 == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x11);
        return 0;
    }
    local_30 = G_CDataManager()->find_item(local_34->m_addInfo);   // *(int*)(local_34+2)
    if (local_30 == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x11);
        return 0;
    }
    local_29 = local_38->m_12;           // movzbl 0x12(eax)→-0x25
    if (((const unsigned char*)local_34)[(unsigned int)(unsigned char)local_29 * 3 + 0x25] == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x11);
        return 0;
    }
    if (((ServiceRestrictManager*)ARAD::Singleton<ServiceRestrictManager>::Get())
            ->isRestricted(user, (RestrictCategory::Enum)1, 0xf))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0xd1);
        return 0;
    }
    local_20 = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x25);
    if (local_20 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, (unsigned char)(local_20 & 0xff));
        return 0;
    }
    cVar4 = (char)local_34->m_random.m_random.empty();
    if (cVar4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x13);
        return 0;
    }
    local_28 = (unsigned int)(unsigned char)local_29;
    if (local_28 >= 3)                   // js/jle：<3 继续，否则 err 0x13
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x13);
        return 0;
    }
    cVar4 = (char)local_34->m_random.m_random.m_fielda.empty();
    if ((cVar4 == 1) ||
        ((unsigned int)(local_34->m_random.m_random.m_seedd.m_field & 3) == local_28))
    {
        bVar3 = false;
    }
    else
    {
        bVar3 = true;
    }
    if (bVar3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 4);
        return 0;
    }
    int uVar9 = local_30->get_rarity();
    short sVar5 = (short)local_30->getUsableLevel();
    local_24 = (unsigned int)G_CDataManager()->m_randomOption.getOptionModificationCost(
        sVar5, (ENUM_RARITY)uVar9);
    if (local_24 == 0xffffffff)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x11);
        return 0;
    }
    local_38->m_10 = 0xffff;
    if ((short)local_38->m_10 == -1)
    {
        pCVar7 = (CInventory*)user->getCurCharacInvenR();
        if (pCVar7->get_money() < (int)local_24)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 10);
            return 0;
        }
    }
    else
    {
        unsigned int uVar14 = (unsigned int)local_38->m_10;
        const CInventory* invR = user->getCurCharacInvenR();
        Inven_Item local_81 = invR->GetInvenSlot(1, (int)uVar14);   // 返回值落于 -0x7d 区，仅取 +0x2 字段判定
        if (local_81.m_addInfo < 1)      // -0x76 = local_81+0x2（mov -0x76,test,jg）
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x16);
            return 0;
        }
    }
    // static local：ORIG __cxa_guard(0x0930???)/CRandomOptionItemHandle C1 + __cxa_atexit
    static random_option::CRandomOptionItemHandle handle;
    random_option::CRandomOptionItemHandle* pHandle = &handle;
    pHandle->set_random_option_give(
        (CRandomOptionGive*)((char*)random_option::GetRandomOption() + 0x154));  // ORIG 0x085f0eb0/0x085f0ea2

    int uVar12 = local_30->GetRandomOptionGrade();       // vtable +0x50 槽虚调用
    int uVar10 = local_30->getItemGroupName();
    int uVar11 = local_30->getUsableLevel();
    int uVar9b = local_30->get_rarity();
    // ORIG 栈序（081dce5a..081dceeb）：
    //   &handle, itemIdx=*(int*)(local_34+2), eax=get_rarity, ebx=getUsableLevel,
    //   esi=getItemGroupName, edi=vtable+0x50, local_24=msg+0x12 选项序号,
    //   opts=local_34+0x25
    cVar4 = (char)pHandle->change_option(
        (unsigned long)local_34->m_addInfo,
        (ENUM_RARITY)uVar9b,
        uVar11,
        uVar10,
        uVar12,
        (int)local_28,
        &local_34->m_random.m_random);
    if ((char)(cVar4 ^ 1) == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x13);
        return 0;
    }
    user->m_historyLog.RandomOption(*local_34, (randomOptionReason::T)2);
    {
        char* uVar9c = GlobalData::s_systemTime_.getCurDate();
        int uVar10c = user->get_charac_10_level_section();
        statistc_proxy::add(user, "log_random_option", "change_cnt", 1,
                            "%d,'%s'", uVar10c, uVar9c);
    }
    local_38->m_10 = 0xffff;
    if ((short)local_38->m_10 == -1)
    {
        char* uVar9c = GlobalData::s_systemTime_.getCurDate();
        int uVar10c = user->get_charac_10_level_section();
        statistc_proxy::add(user, "log_random_option", "change_gold", local_24,
                            "%d,'%s'", uVar10c, uVar9c);
        pCVar7 = user->getCurCharacInvenW();
        cVar4 = (char)pCVar7->use_money((int)local_24, (eMoneySubReason)0x2c, 1);
        if ((char)(cVar4 ^ 1) != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 10);
            return 0;
        }
    }
    else
    {
        unsigned int uVar1b = (unsigned int)local_38->m_10;
        pCVar7 = user->getCurCharacInvenW();
        int iVar6b = pCVar7->use_item((int)uVar1b, 1, 1);
        if (iVar6b != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b6, 0x16);
            return 0;
        }
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, (int)local_38->m_10);
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
    {
        PacketGuard local_44;
        local_44.put_header(0, 0xe);
        local_44.put_byte(0);
        local_44.put_short(1);
        unsigned int uVar2 = (unsigned int)local_38->m_0e;
        pCVar7 = (CInventory*)user->getCurCharacInvenR();
        pCVar7->MakeItemPacket((INVEN_TYPE)1, (int)uVar2, local_44);
        local_44.finalize(true);
        user->Send(local_44);
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1b6);
    }
    return 0;
}
int Dispatcher_ChangeRandomOption::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081dc8de：ushort@0xe → ushort@0x10 → byte@0x12
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(m + 0xe)))
        return LineFunc(0x4cd8, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(m + 0x10)))
        return LineFunc(0x4cdb, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0x12)))
        return LineFunc(0x4cde, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_ChangeTutorialFlag::Dispatcher_ChangeTutorialFlag() {}
Dispatcher_ChangeTutorialFlag::~Dispatcher_ChangeTutorialFlag() {}
int Dispatcher_ChangeTutorialFlag::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 2)
    {
        return LineFunc(0xa115,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned int flag = 0;
    if (!packet.get_int(flag))
    {
        return LineFunc(0xa118,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char rewardFlag = 0;
    if (!packet.get_byte(rewardFlag))
    {
        return LineFunc(0xa11b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (flag > 0x4d)
    {
        return LineFunc(0xa120,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (flag == 0x4d)
    {
        for (unsigned int i = 0; i < 0x4e; i = i + 1)
        {
            user->setCurCharacTutorialFlag(i);
        }
        return 0;
    }
    if (user->setCurCharacTutorialFlag(flag) != 1)
    {
        return 0;
    }
    if (rewardFlag != 0)
    {
        user->RewardTutorial(flag);
    }
    if (flag == 0x1f)
    {
        CParty* party = (CParty*)user->GetParty();
        if (party != 0)
        {
            party->giveup_game(user, true, false, false);
        }
    }
    return 0;
}
int Dispatcher_ChangeTutorialFlag::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeTutorialFlag::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Change_Charac_Slot::Dispatcher_Change_Charac_Slot() {}
Dispatcher_Change_Charac_Slot::~Dispatcher_Change_Charac_Slot() {}
int Dispatcher_Change_Charac_Slot::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082251f4：state!=2 直接返 0；连读两个 uint（失败任一 → LineFunc(0xdff5)）；
    // 锁链：isLocked4DataLoad||isLockedCreateCharac 再 ||isLockedDeleteCharac，
    // 全未锁时 ChangeCharacSlot(第一个 uint, 第二个 uint)，返 0。
    if (user->get_state() != 2)
    {
        return 0;
    }
    unsigned int first = 0;
    unsigned int second = 0;
    char readOk;
    if (packet.get_int(first) && packet.get_int(second))
    {
        readOk = 0;
    }
    else
    {
        readOk = 1;
    }
    if (readOk != 0)
    {
        return LineFunc(0xdff5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char locked;
    if (user->isLocked4DataLoad() == 0 && user->isLockedCreateCharac() == 0)
    {
        locked = 0;
    }
    else
    {
        locked = 1;
    }
    if (locked == 0 && user->isLockedDeleteCharac() == 0)
    {
        locked = 0;
    }
    else
    {
        locked = 1;
    }
    if (locked != 1)
    {
        user->ChangeCharacSlot(first, second);
    }
    return 0;
}
int Dispatcher_Change_Charac_Slot::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Change_Charac_Slot::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CharacterStatistic::Dispatcher_CharacterStatistic() {}
Dispatcher_CharacterStatistic::~Dispatcher_CharacterStatistic() {}
int Dispatcher_CharacterStatistic::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82051d0：get_state < 3 → 0；依次 get_int 6 个字段（任一失败 →
    // LineFunc(0x9d1f+k, __PRETTY_FUNCTION__, 0, 0) 并返回其返回值），
    // 成功后写 CUser+0x8e03c..0x8e050（6 个 int，Character 统计区），并置
    // CUser+0x8e074 = 1。零外部调用（自包含）。
    if (user->get_state() < 3)
    {
        return 0;
    }

    int v0, v1, v2, v3, v4, v5;
    if (packet.get_int(v0) != 1) return LineFunc(0x9d1f, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(v1) != 1) return LineFunc(0x9d20, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(v2) != 1) return LineFunc(0x9d21, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(v3) != 1) return LineFunc(0x9d22, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(v4) != 1) return LineFunc(0x9d23, __PRETTY_FUNCTION__, 0, 0);
    if (packet.get_int(v5) != 1) return LineFunc(0x9d24, __PRETTY_FUNCTION__, 0, 0);

    char* base = reinterpret_cast<char*>(user);
    *(int*)(base + 0x8e03c) = v0;  // ORIG *(int*)(param_1+0x8e03c) = local_24
    *(int*)(base + 0x8e044) = v1;  // local_1c
    *(int*)(base + 0x8e048) = v2;  // local_18
    *(int*)(base + 0x8e040) = v3;  // local_20
    *(int*)(base + 0x8e04c) = v4;  // local_14
    *(int*)(base + 0x8e050) = v5;  // local_10[0]
    *(base + 0x8e074) = 1;
    return 0;
}
int Dispatcher_CharacterStatistic::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CharacterStatistic::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CloseDisjointStore::Dispatcher_CloseDisjointStore() {}
Dispatcher_CloseDisjointStore::~Dispatcher_CloseDisjointStore() {}
// ORIG 0x081d02c8（nm 实测，sz 0x9e）：state>2 且 curCharac!=0；ExpertJobType!=3
// → -2；job 空 → 0x13；GetType()!=3 → 0x13；否则 0。
int Dispatcher_CloseDisjointStore::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        if (user->GetCurCharacExpertJobType() != 3)
        {
            return -2;
        }
        expert_job::CExpertJob* job = user->GetCurCharacExpertJob();
        if (job == 0)
        {
            return 0x13;
        }
        if (job->GetType() != 3)
        {
            return 0x13;
        }
        return 0;
    }
    return -1;
}

// ORIG 0x081d0366（报告 Dispatcher_CloseDisjointStore/process.md）：
// 通过后调当前专家职业 OnCloseMission(user, 0)。
int Dispatcher_CloseDisjointStore::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stCloseDisjointStoreParam& out = static_cast<stCloseDisjointStoreParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2b1b,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    expert_job::CExpertJob* job = user->GetCurCharacExpertJob();
    job->OnCloseMission(user, (expert_job::ENUM_CLOSE_MISSION_TYPE)0);
    return 0;
}
int Dispatcher_CloseDisjointStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_CloseDisjointStore::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stCloseDisjointStoreParam& p = static_cast<stCloseDisjointStoreParam&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xf6, (unsigned char)p.m_4);
}

Dispatcher_CompatibilityIndex::Dispatcher_CompatibilityIndex() {}
Dispatcher_CompatibilityIndex::~Dispatcher_CompatibilityIndex() {}
int Dispatcher_CompatibilityIndex::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e2664：local = check_error(user, msg, param)（不写 param）；
    // >0 返 0；<0 LineFunc(0x58f8,err,0)；==0 构造 Packet_Stat_Compatibility_Index，
    // 逐字段拷 msg 值后经 statistic 代理 SendPacket(pkt, 0x1e)。
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x58f8,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    Packet_Stat_Compatibility_Index pkt;   // ORIG W ctor 0x0822c61e（局部帧 -0x2e 起）
    // 帧偏移换算（base -0x2e）：+0x6 acc_id、+0xa msg[0xd]、+0xb msg[0xe]、
    // +0xc *(int*)(msg+0x10)、+0x10 u16@msg+0x14、+0x12 u16@msg+0x16、
    // +0x14 u16@msg+0x1a、+0x16 u16@msg+0x18、+0x18 msg[0x1c]、+0x19 msg[0x1d]
    char* p = reinterpret_cast<char*>(&pkt);
    const char* m = reinterpret_cast<const char*>(&msg);
    *reinterpret_cast<int*>(p + 6) = user->get_acc_id();
    p[0xa] = m[0xd];
    p[0xb] = m[0xe];
    *reinterpret_cast<int*>(p + 0xc) = *reinterpret_cast<const int*>(m + 0x10);
    *reinterpret_cast<unsigned short*>(p + 0x10) =
        *reinterpret_cast<const unsigned short*>(m + 0x14);
    *reinterpret_cast<unsigned short*>(p + 0x12) =
        *reinterpret_cast<const unsigned short*>(m + 0x16);
    *reinterpret_cast<unsigned short*>(p + 0x14) =
        *reinterpret_cast<const unsigned short*>(m + 0x1a);
    *reinterpret_cast<unsigned short*>(p + 0x16) =
        *reinterpret_cast<const unsigned short*>(m + 0x18);
    p[0x18] = m[0x1c];
    p[0x19] = m[0x1d];
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(0));
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x1e);
    return 0;
}
int Dispatcher_CompatibilityIndex::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_COMPATIBILITY_INDEX& data = static_cast<MSG_COMPATIBILITY_INDEX&>(msg);
    if (!packet.get_byte(data.m_0d))
    {
        return LineFunc(0x58df,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(data.m_0e))
    {
        return LineFunc(0x58e0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(&data.m_10)))
    {
        return LineFunc(0x58e1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&data.m_14)))
    {
        return LineFunc(0x58e2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&data.m_16)))
    {
        return LineFunc(0x58e3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&data.m_18)))
    {
        return LineFunc(0x58e4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&data.m_1a)))
    {
        return LineFunc(0x58e5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(data.m_1c))
    {
        return LineFunc(0x58e6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(data.m_1d))
    {
        return LineFunc(0x58e7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}


Dispatcher_CreateAccountCargo::Dispatcher_CreateAccountCargo() {}
Dispatcher_CreateAccountCargo::~Dispatcher_CreateAccountCargo() {}
int Dispatcher_CreateAccountCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08224f7c：state<=2 → 0；否则 CAccountCargo::CreateAccountCargo(user)（静态）。
    if (user->get_state() <= 2)
    {
        return 0;
    }
    CAccountCargo::CreateAccountCargo(user);
    return 0;
}
int Dispatcher_CreateAccountCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CreateAccountCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CreatePrivateStore::Dispatcher_CreatePrivateStore() {}
Dispatcher_CreatePrivateStore::~Dispatcher_CreatePrivateStore() {}
int Dispatcher_CreatePrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820271c: state==3 且 getCurCharacR()!=0 才放行
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
    {
        // 通过
    }
    else
    {
        return LineFunc(0x98f1, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x066: IsIntegratedPvPBaseChannel || GetChannelType()==7 → error 0x3e
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() || G_GameWorld()->GetChannelType() == 7)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x58), 0x3e);
        return 0;
    }

    // +0x0c8: getUserVillage 空则 LineFunc
    Village* village = G_GameWorld()->getUserVillage(user);
    if (village == 0)
    {
        return LineFunc(0x9905, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x10d: get_area(false) == get_gate_area() → error 0x3e
    if (user->get_area(false) == village->get_gate_area())
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x58), 0x3e);
        return 0;
    }

    // +0x16c: isRestricted(user,1,0xb) → error 0xd1
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, static_cast<RestrictCategory::Enum>(1), 0xb))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x58), 0xd1);
        return 0;
    }

    // +0x1a1: CSecu_ProtectionField::Check(user,0x3c)
    int secuCheck = GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x3c));
    if (secuCheck != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x58),
                                 static_cast<unsigned char>(secuCheck));
        return 0;
    }

    // +0x1ed: 读包五参
    private_store::ISeller* seller = 0;
    unsigned char villageNo = 0;
    unsigned char areaNo = 0;
    short posX = 0;
    short posY = 0;
    unsigned short dollSlot = 0;
    if (!packet.get_byte(villageNo))
    {
        return LineFunc(0x9944, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(areaNo))
    {
        return LineFunc(0x9945, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(posX))
    {
        return LineFunc(0x9946, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(posY))
    {
        return LineFunc(0x9947, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(dollSlot))
    {
        return LineFunc(0x994b, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x33d: 角色村庄/区域与包内一致才放行
    if (static_cast<int>(user->getCurCharacVill()) == static_cast<unsigned int>(villageNo) &&
        static_cast<unsigned int>(user->get_area(false)) == static_cast<unsigned int>(areaNo))
    {
        // 一致 → 创建
    }
    else
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x58), 0x3e);
        return 0;
    }

    // +0x3a3: GetStoreSeller → 虚 CreatePrivateStore(villageNo,areaNo,posX,posY,dollSlot)
    private_store::CPrivateStoreMgr* storeMgr = private_store::GetInstancePrivateStoreMgr();
    storeMgr->GetStoreSeller(&seller, user);
    if (!seller->CreatePrivateStore(villageNo, areaNo, posX, posY, dollSlot))
    {
        // +0x41a: CreatePrivateStore 失败但状态已建立 → hack 计数
        if (reinterpret_cast<private_store::CPrivateStore*>(seller)->GetState() != 0)
        {
            WongWork::CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
            hackAnalyzer->addServerHackCnt(user, static_cast<WongWork::ENUM_HACKTYPE>(0x32d), 1, 0, 0);
        }
        // +0x45d: 状态已建立 → 返回；否则释放 seller
        if (reinterpret_cast<private_store::CPrivateStore*>(seller)->GetState() != 0)
        {
            return 0;
        }
        private_store::GetInstancePrivateStoreMgr()->FreeStoreSeller(user);
    }
    return 0;
}
int Dispatcher_CreatePrivateStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CreatePrivateStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DungeonNPCBuffInfo::Dispatcher_DungeonNPCBuffInfo() {}
Dispatcher_DungeonNPCBuffInfo::~Dispatcher_DungeonNPCBuffInfo() {}
int Dispatcher_DungeonNPCBuffInfo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DungeonNPCBuffInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DungeonNPCBuffInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_EnterDisjointStore::Dispatcher_EnterDisjointStore() {}
Dispatcher_EnterDisjointStore::~Dispatcher_EnterDisjointStore() {}
int Dispatcher_EnterDisjointStore::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d01ae：check_error(三参) 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x2adf)；err==0 → 分解机进入商店。
    int err = check_error(user, msg, param);
    *(int*)(reinterpret_cast<char*>(&param) + 0x4) = err;

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LineFunc(0x2adf,
                 __PRETTY_FUNCTION__,
                 err, (unsigned int)user->get_acc_id());
        return 0;
    }

    // msg+0xe：ushort 对象 uid → find_from_world；目标用户分解机 OnEnterDisjointStore。
    unsigned short uid =
        *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&msg) + 0xe);
    CUser* target = G_GameWorld()->find_from_world(uid);

    expert_job::CExpertJob* job = target->GetCurCharacExpertJob();
    expert_job::CDisjointer* disjointer = (expert_job::CDisjointer*)job;
    disjointer->OnEnterDisjointStore(user);
    return 0;
}
int Dispatcher_EnterDisjointStore::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 short(msg+0xe)，失败记 LineFunc(0x2aa8)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<short*>(p + 0xe)))
    {
        return LineFunc(0x2aa8, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_EnterDisjointStore::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stEnterDisjointStoreParam& p = static_cast<stEnterDisjointStoreParam&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xf5, (unsigned char)p.m_4);
}

Dispatcher_FrameLagStatistics::Dispatcher_FrameLagStatistics() {}
Dispatcher_FrameLagStatistics::~Dispatcher_FrameLagStatistics() {}
int Dispatcher_FrameLagStatistics::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_FrameLagStatistics::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GuildCargoMoveItem::Dispatcher_GuildCargoMoveItem() {}
Dispatcher_GuildCargoMoveItem::~Dispatcher_GuildCargoMoveItem() {}
int Dispatcher_GuildCargoMoveItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stGuildCargoMoveItemParam& out = static_cast<stGuildCargoMoveItemParam&>(param);
    out.m_4 = check_error(user, msg, param);                    // ORIG 0x081d18dc
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        LineFunc(0x2dff,
                 __PRETTY_FUNCTION__,
                 out.m_4, 0);
        return 0;
    }

    MSG_GUILD_CARGO_MOVE_ITEM& req = static_cast<MSG_GUILD_CARGO_MOVE_ITEM&>(msg);

    GuildCargoMoveItem_PacketView pkt;
    pkt.m_guildKey = user->get_charac_guildkey();               // → +0x0a
    pkt.m_charNo = user->getCurCharacNo();                      // → +0x0e
    pkt.m_fromSlot = (int)req.m_field0d;                        // msg+0x0d ushort → +0x12
    pkt.m_toSlot = (int)req.m_field0f;                          // msg+0x0f ushort → +0x16
    pkt.m_count = req.m_field11;                                // msg+0x11 → +0x1a
    pkt.m_type = req.m_field15;                                 // msg+0x15 → +0x1e

    CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(
        user->GetServerGroup());
    proxy->SendTcpPacket((char*)&pkt, (int)pkt.packetSize);     // 0x22

    user->m_historyLog.GuildCargoMoveItem(
        (ENUM_GUILD_CARGO_HISTORY_REASON)0xd,
        (int)req.m_field0d, (int)req.m_field0f);                // ORIG 0x086867ce
    return 0;
}

int Dispatcher_GuildCargoMoveItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d17ba：ushort@0xd → int@0x11 → short@0xf → int@0x15
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(m + 0xd)))
        return LineFunc(0x2dd2, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x11)))
        return LineFunc(0x2dd4, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<short*>(m + 0xf)))
        return LineFunc(0x2dd6, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x15)))
        return LineFunc(0x2dd8, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_GuildCargoMoveItem::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d1a96 T：err=*(int*)((char*)&param+4)；0 → 静默；0x64/0xb5 →
    // SendCmdErrorPacket(0x109,(unsigned char)err)；其余 → SendCmdErrorPacket(0xfd,…)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
        return;
    if (err == 0x64 || err == 0xb5)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x109, (unsigned char)err);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfd, (unsigned char)err);
}

Dispatcher_GuildCargoPushItem::Dispatcher_GuildCargoPushItem() {}
Dispatcher_GuildCargoPushItem::~Dispatcher_GuildCargoPushItem() {}
// ORIG 0x081d0f64（OnlyOrigBatch13 组 E 转写；3 参数符号，字段自 ParamBase 读）：
// state==3 且 curCharac 有效；guildkey==0→100；无公会驻地→0xb5；Secu Check 非 0
// 透传；物品锁→0xd5；param+0x10 与槽内 m_addInfo 不符或 find_item 失败→0xc5。
int Dispatcher_GuildCargoPushItem::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    (void)msg;  // ORIG 不读 MSG_BASE，字段全部来自 param

    if (user->get_state() != 3 || user->getCurCharacR() == 0)
        return -1;
    if (user->get_charac_guildkey() == 0)
        return 100;
    if (user->IsExistGuildAgit() != 1)
        return 0xb5;

    const unsigned char itemSpace =
        *reinterpret_cast<const unsigned char*>(
            reinterpret_cast<const char*>(&param) + 0x0d);
    const unsigned short slot =
        *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const char*>(&param) + 0x0e);
    const int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)itemSpace);
    const CInventory* inven = user->getCurCharacInvenR();
    const Inven_Item slotItem = inven->GetInvenSlot(invenType, (int)slot);

    const int err =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)8);
    if (err != 0)
        return err;
    if (user->CheckItemLock(invenType, (int)slot))
        return 0xd5;
    const int reqAddInfo =
        *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(&param) + 0x10);
    if (reqAddInfo == 0 || slotItem.m_addInfo != reqAddInfo)
        return 0xc5;
    if (G_CDataManager()->find_item(slotItem.m_addInfo) == 0)
        return 0xc5;
    return 0;
}

int Dispatcher_GuildCargoPushItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d10d4 (0x242 insn)
    stGuildCargoPushItemParam& result = static_cast<stGuildCargoPushItemParam&>(param);
    const char* const M = reinterpret_cast<const char*>(&msg);
    const unsigned char itemSpace =
        *reinterpret_cast<const unsigned char*>(reinterpret_cast<const char*>(&msg) + 0x0d);
    const unsigned short slot =
        *reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&msg) + 0x0e);

    int err = check_error(user, msg, param);
    result.m_4 = err;                                   // +0x04 错误码
    if (err >= 1)
        return 0;
    if (err < 0)
        return LineFunc(0x2cf0, __PRETTY_FUNCTION__, err, 0);

    const CInventory* inven = user->getCurCharacInvenR();
    const int invenType =
        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned char)itemSpace);
    const Inven_Item slotItem = inven->GetInvenSlot(invenType, static_cast<int>(slot));
    CDataManager* dataMgr = G_CDataManager();
    CItem* item = dataMgr->find_item(slotItem.m_addInfo);

    // ---- ORIG 0x081d11c9..0x081d1305（Fix7_I 恢复真实调用）----
    const bool isStackable = item->is_stackable();
    const int stackLimit =
        isStackable ? static_cast<CStackableItem*>(item)->getStackableLimit() : 1;

    Packet_Guild_Check_Guild_Cargo_Push_Item pkt;            // ctor 0x0822bec2
    pkt.m_guildKey = (unsigned int)user->get_charac_guildkey();   // +0xa
    pkt.m_charNo = (unsigned int)user->getCurCharacNo();          // +0xe
    pkt.m_slot = (int)(unsigned short)*reinterpret_cast<const unsigned short*>(M + 0x18);  // +0x12
    pkt.m_itemId = *reinterpret_cast<const int*>(M + 0x10);      // +0x16
    pkt.m_count = *reinterpret_cast<const int*>(M + 0x14);        // +0x1a
    pkt.m_maxStack = stackLimit;                                  // +0x1e
    pkt.m_stackable = (unsigned char)(isStackable ? 1 : 0);       // +0x22
    pkt.m_invenType = *reinterpret_cast<const unsigned char*>(M + 0x0d);  // +0x23
    pkt.m_invenSlot = *reinterpret_cast<const unsigned short*>(M + 0x0e); // +0x24

    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt),
                         (int)*(unsigned short*)((char*)&pkt + 2));

    user->m_historyLog.GuildCargoCheckPushItem(
        (ENUM_GUILD_CARGO_HISTORY_REASON)5,
        (int)(unsigned short)*reinterpret_cast<const unsigned short*>(M + 0x18),
        *reinterpret_cast<const int*>(M + 0x10),
        *reinterpret_cast<const int*>(M + 0x14),
        (int)*reinterpret_cast<const unsigned char*>(M + 0x0d),
        (int)(unsigned short)*reinterpret_cast<const unsigned short*>(M + 0x0e),
        (int)(isStackable ? 1 : 0));
    return 0;
}

int Dispatcher_GuildCargoPushItem::read(PacketBuf& packet, MSG_BASE& msg)   // ORIG 0x081d0dfe (83 insn)
{
    static const char kFunc[] =
        "virtual int Dispatcher_GuildCargoPushItem::read(PacketBuf&, MSG_BASE&)";
    char* pMsg = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(pMsg + 0x0d)))
        return LineFunc(0x2c91, kFunc, 0, 0);           // msg+0xd byte itemSpace
    if (!packet.get_short(*reinterpret_cast<short*>(pMsg + 0x0e)))
        return LineFunc(0x2c92, kFunc, 0, 0);           // msg+0xe short slot
    if (!packet.get_int(*reinterpret_cast<int*>(pMsg + 0x10)))
        return LineFunc(0x2c93, kFunc, 0, 0);           // msg+0x10 int
    if (!packet.get_int(*reinterpret_cast<int*>(pMsg + 0x14)))
        return LineFunc(0x2c94, kFunc, 0, 0);           // msg+0x14 int
    if (!packet.get_short(*reinterpret_cast<short*>(pMsg + 0x18)))
        return LineFunc(0x2c95, kFunc, 0, 0);           // msg+0x18 short count
    return 0;
}

void Dispatcher_GuildCargoPushItem::send(CUser* user, ParamBase& param)     // ORIG 0x081d1316 (36 insn)
{
    stGuildCargoPushItemParam& result = static_cast<stGuildCargoPushItemParam&>(param);
    if (result.m_4 != 0)
    {
        if (result.m_4 == 100 || result.m_4 == 0xb5)
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x109),
                                     static_cast<unsigned char>(result.m_4 & 0xff));
        else
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xfb),
                                     static_cast<unsigned char>(result.m_4 & 0xff));
    }
}

Dispatcher_RemovePrivateStore::Dispatcher_RemovePrivateStore() {}
Dispatcher_RemovePrivateStore::~Dispatcher_RemovePrivateStore() {}
int Dispatcher_RemovePrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0;
    }
    if (user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        PacketGuard response;
        response.put_header(1, 0x59);
        response.put_byte(0);
        response.put_byte(0x15);
        response.finalize(true);
        user->Send(response);
        return 0;
    }
    private_store::CPrivateStoreMgr* manager =
        private_store::GetInstancePrivateStoreMgr();
    private_store::ISeller* seller = 0;
    if (manager != 0)
    {
        manager->GetStoreSeller(&seller, user);
    }
    if (seller != 0)
    {
        seller->RemovePrivateStore(false);
    }
    return 0;
}
int Dispatcher_RemovePrivateStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RemovePrivateStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestAddPvPBuddy::Dispatcher_RequestAddPvPBuddy() {}
Dispatcher_RequestAddPvPBuddy::~Dispatcher_RequestAddPvPBuddy() {}
int Dispatcher_RequestAddPvPBuddy::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    char type = 0;                                   // -0x37
    int nameLen = 0;                                 // -0x3c
    char name[0x1e];                                 // -0x36（ORIG 清零 0x1e 字节）
    memset(name, 0, sizeof(name));
    if (!packet.get_byte(type))
        return LineFunc(0xdf3c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(nameLen))
        return LineFunc(0xdf3f, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(name, 0x1e, nameLen))
        return LineFunc(0xdf42, __PRETTY_FUNCTION__, 0, 0);
    GlobalData::s_community_proxy->SendReqAddPvPBuddy(
        user->get_acc_id(),
        (unsigned int)user->getCurCharacNo(),
        type,
        name,
        nameLen);
    return 0;
}

int Dispatcher_RequestAddPvPBuddy::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestAddPvPBuddy::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestGuildSecede::Dispatcher_RequestGuildSecede() {}
Dispatcher_RequestGuildSecede::~Dispatcher_RequestGuildSecede() {}
int Dispatcher_RequestGuildSecede::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int local_38 = 0;                    // -0x34：目标模式（0=仅自己退会）
    char local_32[30];                   // -0x2e：目标角色名
    unsigned int local_14;               // -0x10：guildkey
    unsigned int local_10;               // -0xc：Secu Check 结果
    char cVar2;
    int iVar3;

    iVar3 = user->get_state();
    if ((iVar3 <= 2) || (user->getCurCharacR() == 0))
    {
        LineFunc(0xa29f, __PRETTY_FUNCTION__, 0, 0);
        return 0;
    }
    local_14 = user->get_charac_guildkey();
    if (local_14 == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, 0x22);
        return 0;
    }
    cVar2 = user->getCurCharacVill();
    if ((char)cVar2 == 7)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, 0x13);
        return 0;
    }
    cVar2 = packet.get_int(local_38);
    if ((char)(cVar2 ^ 1) == 0)          // cVar2==1 继续
    {
        if (local_38 != 0 && local_38 <= 0x1d)   // jg 0x1d 越过 → (0,0x1d] 走目标分支
        {
            cVar2 = packet.get_str(local_32, 0x1e, local_38);
            if ((char)(cVar2 ^ 1) == 0)
            {
                cVar2 = user->IsSubGuildMaster();
                if (((char)(cVar2 ^ 1) != 0) &&
                    (cVar2 = user->IsGuildMaster(), (char)(cVar2 ^ 1) != 0))
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, 0x24);
                    return 0;
                }
                cVar2 = GlobalData::s_power_manager->IsPowerWarEventOn();
                if (cVar2 != 0)
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, 0x13);
                    return 0;
                }
            }
            else
            {
                LineFunc(0xa2b8, __PRETTY_FUNCTION__, 0, 0);
                return 0;
            }
        }
        local_10 = (unsigned int)GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xd);
        if (local_10 == 0)
        {
            if (local_38 == 0)
            {
                user->deleteGuildExpBook();
            }
            unsigned int uVar5 = user->getCurCharacNo();
            ENUM_SERVER_GROUP uVar4 = user->GetServerGroup();
            CGuildServerProxy* this_00 =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(uVar4);
            this_00->SendRequestGuildSecede(local_14, uVar5, local_38, local_32);
        }
        else
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, (unsigned char)(local_10 & 0xff));
        }
    }
    else
    {
        LineFunc(0xa2b3, __PRETTY_FUNCTION__, 0, 0);
        return 0;
    }
    return 0;
}
int Dispatcher_RequestGuildSecede::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestGuildSecede::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestMemberEnter::Dispatcher_RequestMemberEnter() {}
Dispatcher_RequestMemberEnter::~Dispatcher_RequestMemberEnter() {}
int Dispatcher_RequestMemberEnter::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0 ? 0 : -1;
}
int Dispatcher_RequestMemberEnter::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cb874：param+4 = check_error(user,msg)；>0 返 0；<0 LineFunc(0x2192)；
    // ==0 构造 Packet_Monitor_Request_Member_Enter，UID(+6)/characNo(+0xa)，
    // strncpy(pkt+0xe, msg+0x14, *(size_t*)(msg+0x10))，Monitor 代理 SendTcpPacket(pkt, 0x30)。
    int err = check_error(user, msg);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x2192,
                        __PRETTY_FUNCTION__,
                        err, user->get_acc_id());
    }
    Packet_Monitor_Request_Member_Enter pkt;   // ORIG W ctor 0x0822b9ce（局部帧 -0x40 起）
    // 帧偏移：-0x3a=pkt+0x6(UID)、-0x36=pkt+0xa(characNo)、-0x32=pkt+0xe(30 字节名区)
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 6) = user->GetUID();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pkt) + 0xa) = user->getCurCharacNo();
    std::strncpy(reinterpret_cast<char*>(&pkt) + 0xe,
                 reinterpret_cast<const char*>(&msg) + 0x14,
                 *reinterpret_cast<const size_t*>(reinterpret_cast<const char*>(&msg) + 0x10));
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x30);
    return 0;
}
int Dispatcher_RequestMemberEnter::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cb780：int@0x10（len）→ str@0x14（max 0x1e）
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x10)))
        return LineFunc(0x2169, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m + 0x14, 0x1e, *reinterpret_cast<int*>(m + 0x10)))
        return LineFunc(0x216c, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_RequestMemberEnter::send(CUser* user, ParamBase& param)
{
    (void)user;
    // ORIG 0x19：读 +0x04 与 0x7fffffff 比较，条件真则提前返回（无副作用）。
    stRequestMemberEnterParam& p = static_cast<stRequestMemberEnterParam&>(param);
    if (p.m_4 == 0x7fffffff)
        return;
}

Dispatcher_Request_Seria_Buff::Dispatcher_Request_Seria_Buff() {}
Dispatcher_Request_Seria_Buff::~Dispatcher_Request_Seria_Buff() {}
// ORIG 0x081dbda0（报告 Dispatcher_Request_Seria_Buff/process.md）：
// err=check_error(user, msg)（ORIG 0x081dbcd8 三参，声明已补 Dispatchers.h）；
// >0 返 err；<0 返 -1；==0 set_growth_power_exp_reward_right(false)；返 0。
int Dispatcher_Request_Seria_Buff::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    user->set_growth_power_exp_reward_right(false);
    return 0;
}
int Dispatcher_Request_Seria_Buff::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Request_Seria_Buff::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081dbe08 T：err=*(int*)((char*)&param+4)；0 → set_growth_power_exp_reward(
    // get_charac_level()) + SendCmdOkPacket(0x1b1)；否则 SendCmdErrorPacket(0x1b1,…)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
    {
        user->set_growth_power_exp_reward(user->get_charac_level());
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1b1);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b1, (unsigned char)err);
    }
}

Dispatcher_Score_Scroll_State::Dispatcher_Score_Scroll_State() {}
Dispatcher_Score_Scroll_State::~Dispatcher_Score_Scroll_State() {}
int Dispatcher_Score_Scroll_State::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() != 5)
    {
        return 0;
    }
    int partyIdx = (int)user->GetPartyIndex();
    CGameManager* gameMgr = G_CGameManager();
    CParty* party = gameMgr->GetParty(partyIdx);
    if (party == 0)
    {
        return LineFunc(0x92fa,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!party->CheckRecvFromAllMember(user))
    {
        return 0;
    }
    if ((int)party->get_dungeon_clear_state() > 2)
    {
        WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
        analyzer->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x197, 1, 0, 0);
        return 0;
    }
    PacketGuard response;
    response.clear();
    response.put_header(1, 0x48);
    response.put_byte(1);
    response.finalize(true);
    party->send_to_party(response);
    party->set_dungeon_clear_state((char)3);
    party->insert_time_dungeon_score_scroll_msg(0x1e);
    return 0;
}
int Dispatcher_Score_Scroll_State::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Score_Scroll_State::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SecurityCardDisuse::Dispatcher_SecurityCardDisuse() {}
Dispatcher_SecurityCardDisuse::~Dispatcher_SecurityCardDisuse() {}
int Dispatcher_SecurityCardDisuse::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;                                          // ORIG 不读包体
    if (user->get_state() != 3)
    {
        return LineFunc(0xa5dc,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    WongWork::CSecurityCard* card = user->getSecurityCard();
    if (!card->isActivate())
    {
        return LineFunc(0xa5e3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!card->isCertified())
    {
        return LineFunc(0xa5e6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if ((unsigned int)card->getFailCnt() > 2)              // cmp $0x2; seta（无符号 >2）
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa5, 0x7a);
        return 0;
    }
    PacketGuard guard;                                     // 0x0858dd4c
    guard.put_header(1, 0xa5);
    guard.put_byte(1);
    guard.finalize(true);
    user->Send(guard);
    DB_SecurityCardDisuse::makeRequest(user->get_acc_id());
    card->init();                                          // 0x085fdd68
    return 0;                                              // guard 析构 0x0858de80
}

int Dispatcher_SecurityCardDisuse::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardDisuse::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_TitleBookOtherUser::Dispatcher_TitleBookOtherUser() {}
Dispatcher_TitleBookOtherUser::~Dispatcher_TitleBookOtherUser() {}
// ORIG 0x081d7d56（报告 Dispatcher_TitleBookOtherUser/process.md）：
// msg+0xd（unsigned short）为目标 uniqueId；两侧均要求 get_state() > 2。
int Dispatcher_TitleBookOtherUser::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    if (user->get_state() > 2)
    {
        unsigned short targetId = *(unsigned short*)((char*)&msg + 0xd);
        CUser* target = G_GameWorld()->find_from_world(targetId);
        if (target != 0 && target->get_state() > 2)
        {
            CTitleBook* titleBook =
                (CTitleBook*)target->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xe);
            titleBook->sendListOtherUser(user, (ENUM_TITLE_BOOK_INFO_TYPE)1);
        }
    }
    return 0;
}
int Dispatcher_TitleBookOtherUser::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned short(msg+0xd)，失败记 LineFunc(0x4244)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(p + 0xd)))
    {
        return LineFunc(0x4244, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_UnsealRandomOption::Dispatcher_UnsealRandomOption() {}
Dispatcher_UnsealRandomOption::~Dispatcher_UnsealRandomOption() {}
int Dispatcher_UnsealRandomOption::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UnsealRandomOption::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ValidateScriptHash::Dispatcher_ValidateScriptHash() {}
Dispatcher_ValidateScriptHash::~Dispatcher_ValidateScriptHash() {}
// ORIG 0x08264364（报告 Dispatcher_ValidateScriptHash/get_hack_type.md）[随行依赖]：
// switch(1..5) → 0x344..0x348；0 与越界分支 ORIG 未初始化 local_8（忠实保留）。
int Dispatcher_ValidateScriptHash::get_hack_type(char type)
{
    int hackType;  // ORIG local_8：无 default 分支时不写入
    switch (type)
    {
    case 1: hackType = 0x344; break;
    case 2: hackType = 0x345; break;
    case 3: hackType = 0x346; break;
    case 4: hackType = 0x347; break;
    case 5: hackType = 0x348; break;
    }
    return hackType;
}

// ORIG 0x082642b8（报告 Dispatcher_ValidateScriptHash/process.md）：
// check_error 恒 0；get_hack_type(msg+0xd) → addServerHackCnt(type,1,0xe,0xf)。
int Dispatcher_ValidateScriptHash::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    if (check_error(user, msg) != 0)
    {
        return 0;
    }
    MSG_SCRIPT_HASH& req = static_cast<MSG_SCRIPT_HASH&>(msg);
    const int hackType = get_hack_type((char)req.m_field0d);
    user->getHackAnalyzer()->addServerHackCnt(
        user, (WongWork::ENUM_HACKTYPE)hackType, 1u,
        (unsigned int)req.m_field0e, (unsigned int)req.m_field0f);
    return 0;
}
int Dispatcher_ValidateScriptHash::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SCRIPT_HASH& req = static_cast<MSG_SCRIPT_HASH&>(msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_field0d)))
    {
        return LineFunc(0x37d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_field0e)))
    {
        return LineFunc(0x382,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_field0f))
    {
        return LineFunc(0x387,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

Dispatcher_AuctionRegistCancel::Dispatcher_AuctionRegistCancel() {}
Dispatcher_AuctionRegistCancel::~Dispatcher_AuctionRegistCancel() {}
int Dispatcher_AuctionRegistCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08214b96..0x08214e43 (0x2ae 字节)。GM 表 _ZL6gmList @0x093abb4c。
    // 包：PCK_AUCTION_REGIST_CANCEL_GP（ctor W 0x0822ea1c）/ _GA（ctor W 0x0822e5c4）
    //   uid@0x12, characNo@0x16, auctionId[8]@0x1a。
    static const char* const kPretty =
        "virtual int Dispatcher_AuctionRegistCancel::dispatch_sig(CUser*, PacketBuf&)";
    static const int gmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

    unsigned char payType = 0;
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc47f, kPretty, 0, 0);
    if (payType > 1)
        return LineFunc(0xc482, kPretty, 0, 0);

    const int pay = payType;
    const int preCheck = user->AuctionPreCheck((ENUM_CMDPACKET)0xbb, (PAY_TYPE)pay);
    if (preCheck >= 0)
        return preCheck;

    const unsigned int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x33);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbb, (unsigned char)(secuErr & 0xff));
        return 0;
    }

    if (pay == 1)
    {
        PCK_AUCTION_REGIST_CANCEL_GP pck;
        char* const body = reinterpret_cast<char*>(&pck);
        *reinterpret_cast<unsigned int*>(body + 0x12) = user->GetUID();
        *reinterpret_cast<int*>(body + 0x16) = user->getCurCharacNo();
        if (packet.get_binary(body + 0x1a, 8) != 1)
            return LineFunc(0xc4b1, kPretty, 0, 0);
        GlobalData::s_cera_auction_proxy->SendPacket(
            reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }
    else
    {
        PCK_AUCTION_REGIST_CANCEL_GA pck;
        char* const body = reinterpret_cast<char*>(&pck);
        *reinterpret_cast<unsigned int*>(body + 0x12) = user->GetUID();
        *reinterpret_cast<int*>(body + 0x16) = user->getCurCharacNo();
        if (packet.get_binary(body + 0x1a, 8) != 1)
            return LineFunc(0xc4c4, kPretty, 0, 0);
        GlobalData::s_auction_proxy->SendPacket(
            reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }

    for (int i = 0; i <= 4; i++)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(kPretty, 0xc4d4, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionRegistCancel",
                  user->getCurCharacNo());
            break;
        }
    }
    return 0;
}
int Dispatcher_AuctionRegistCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionRegistCancel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BuyPrivateStoreItem::Dispatcher_BuyPrivateStoreItem() {}
Dispatcher_BuyPrivateStoreItem::~Dispatcher_BuyPrivateStoreItem() {}
int Dispatcher_BuyPrivateStoreItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
    {
        if (user != 0 && user->CheckInTrade() == 0)
        {
            ServiceRestrictManager* srm = ARAD::Singleton<ServiceRestrictManager>::Get();
            if (srm->isRestricted(user, (RestrictCategory::Enum)1, 0xb))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x55, 0xd1);
                return 0;
            }
            int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x3d);
            if (secu != 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x55,
                                         (unsigned char)(secu & 0xff));
                return 0;
            }
            if (user->isHackUser() != 0 || user->isHackUserTimer() != 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x55, 0x11);
                return 0;
            }
            short sellerIndex = 0;
            int count = 0;
            private_store::IBuyer* buyer = 0;
            if (packet.get_short(sellerIndex))
            {
                if (packet.get_int(count))
                {
                    if (count < 0)
                    {
                        return LineFunc(
                            0x98ca,
                            __PRETTY_FUNCTION__,
                            0, 0);
                    }
                    private_store::CPrivateStoreMgr* mgr =
                        private_store::GetInstancePrivateStoreMgr();
                    // ORIG 0x08202543：调 3 参 CUser* 版（直接传 (CUser*)0），
                    // 非 unsigned short 重载（后者会多一次 find_from_world 解析）。
                    mgr->GetStoreBuyer(&buyer, user, (CUser*)0);
                    buyer->BuyPrivateStoreItem((int)sellerIndex, count);
                    return 0;
                }
                return LineFunc(
                    0x98c9,
                    __PRETTY_FUNCTION__,
                    0, 0);
            }
            return LineFunc(
                0x98c8,
                __PRETTY_FUNCTION__,
                0, 0);
        }
        return 0;
    }
    return LineFunc(
        0x9891,
        __PRETTY_FUNCTION__,
        0, 0);
}
int Dispatcher_BuyPrivateStoreItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BuyPrivateStoreItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CallGuildAllMembers::Dispatcher_CallGuildAllMembers() {}
Dispatcher_CallGuildAllMembers::~Dispatcher_CallGuildAllMembers() {}
int Dispatcher_CallGuildAllMembers::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ffb1a
    int result = 0;
    if (user->get_state() > 2)
    {
        if (user->getCurCharacR() != 0)
        {
            // state>2 且当前角色存在 → 允许发送
            int guildkey = user->get_charac_guildkey();
            if (guildkey != 0)
            {
                unsigned int characNo = (unsigned int)user->getCurCharacNo();
                ENUM_SERVER_GROUP group = (ENUM_SERVER_GROUP)user->GetServerGroup();
                CGuildServerProxy* proxy =
                    GlobalData::s_guild_proxy_mgr->GetServerProxy(group);
                proxy->SendPacketCallGuildAllMembers(characNo, (unsigned int)guildkey);
            }
            result = 0;
        }
        else
        {
            result = 1;
        }
    }
    else
    {
        result = 1;
    }
    if (result)
    {
        return LineFunc(0x93cd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_CallGuildAllMembers::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildAllMembers::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompleteLoadAssault::Dispatcher_CompleteLoadAssault() {}
Dispatcher_CompleteLoadAssault::~Dispatcher_CompleteLoadAssault() {}
// ORIG 0x08206f10（报告 Dispatcher_CompleteLoadAssault/dispatch_sig.md）：
// state<=1 → LineFunc(0xa199)；room+0x6e4 matching 非空 → IMatch 虚表 +0x54
// （OnCompleteLoad）vcall；否则 GetInstanceAssaultMgr()->OnCompleteLoad(user)
// 返回值 != 1 → LineFunc(0xa1a8)；恒 return 0。
int Dispatcher_CompleteLoadAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() <= 1)
    {
        LineFunc(0xa199,
                 __PRETTY_FUNCTION__,
                 0, 0);
        return 0;
    }
    PvP_Room* room = (PvP_Room*)user->GetPVPRoom();
    if (room != 0 && room->m_matching != 0)
    {
        IMatch* matching = (IMatch*)room->m_matching;
        matching->OnCompleteLoad(user);
        return 0;
    }
    pvp_assault::CAssaultMgr* mgr = pvp_assault::GetInstanceAssaultMgr();
    if (mgr->OnCompleteLoad(user) != 1)
    {
        LineFunc(0xa1a8,
                 __PRETTY_FUNCTION__,
                 0, 0);
    }
    return 0;
}
int Dispatcher_CompleteLoadAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompleteLoadAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CreateDisjointStore::Dispatcher_CreateDisjointStore() {}
Dispatcher_CreateDisjointStore::~Dispatcher_CreateDisjointStore() {}
int Dispatcher_CreateDisjointStore::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg, param);
    *(int*)((char*)&param + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x29c4,
                        __PRETTY_FUNCTION__,
                        err, user->get_acc_id());
    }
    expert_job::CExpertJob* job = GlobalData::s_ExpertJobMgr->AcquireExpertJob((ENUM_EXPERT_JOB_TYPE)3);
    if (job == 0)
    {
        *(int*)((char*)&param + 4) = 0x15;
        return 0;
    }
    user->SetCurCharacExpertJob(job);
    int result = reinterpret_cast<expert_job::CDisjointer*>(job)->OnCreateDisjointStore(
        user,
        reinterpret_cast<char*>(&msg) + 0xd,
        *(int*)(reinterpret_cast<char*>(&msg) + 0x114),
        (int)(short)*(unsigned short*)(reinterpret_cast<char*>(&msg) + 0x118),
        (int)(short)*(unsigned short*)(reinterpret_cast<char*>(&msg) + 0x11a));
    if (result != 0)
    {
        user->SetCurCharacExpertJob(0);
        GlobalData::s_ExpertJobMgr->FreeExpertJob(job);
        *(int*)((char*)&param + 4) = result;
    }
    return 0;
}
int Dispatcher_CreateDisjointStore::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* base = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(base + 0x110)))
    {
        return LineFunc(0x2976,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(base + 0xd, 0x100,
                        *reinterpret_cast<int*>(base + 0x110)))
    {
        return LineFunc(0x2979,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<int*>(base + 0x114)))
    {
        return LineFunc(0x297c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(base + 0x118)))
    {
        return LineFunc(0x297f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(base + 0x11a)))
    {
        return LineFunc(0x2982,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

void Dispatcher_CreateDisjointStore::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stCreateDisjointStoreParam& p = static_cast<stCreateDisjointStoreParam&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xee, (unsigned char)p.m_4);
}

Dispatcher_CreatureSendMessage::Dispatcher_CreatureSendMessage() {}
Dispatcher_CreatureSendMessage::~Dispatcher_CreatureSendMessage() {}
int Dispatcher_CreatureSendMessage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08204710 [0x08204710, 0x08204b0b]
    // 局部载荷区（-0x157 起）：+0x0d type / +0x0e target / +0x10 uint /
    // +0x14 msg2len / +0x18 msg2[0x1e] / +0x36 msg1len / +0x3a msg1[0x100]
    char buf[0x13a];
    if (user->get_state() < 3)
    {
        return 0;
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(buf + 0x0d)))
    {
        return LineFunc(0x9c3d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(buf + 0x0e)))
    {
        return LineFunc(0x9c3e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(buf + 0x10)))
    {
        return LineFunc(0x9c3f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<int*>(buf + 0x36)))
    {
        return LineFunc(0x9c40,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(buf + 0x3a, 0x100, *reinterpret_cast<int*>(buf + 0x36)))
    {
        return LineFunc(0x9c41,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    G_CDataManager()->rewriteUsableEmoticon(*user, buf + 0x3a);
    if (*reinterpret_cast<unsigned char*>(buf + 0x0d) == 7 ||
        *reinterpret_cast<unsigned char*>(buf + 0x0d) == 1)
    {
        if (!packet.get_int(*reinterpret_cast<int*>(buf + 0x14)))
        {
            return LineFunc(0x9c53,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_str(buf + 0x18, 0x1e, *reinterpret_cast<int*>(buf + 0x14)))
        {
            return LineFunc(0x9c54,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (*reinterpret_cast<unsigned char*>(buf + 0x0d) == 7)
        {
            Packet_Monitor_Other_Channel_Chat chat;   // ctor ORIG 0x0822b8e6（弱内联）
            chat.m_characNo = user->getCurCharacNo();
            chat.m_buddyNameLen = (char)*reinterpret_cast<int*>(buf + 0x14);
            memcpy(chat.buddy_n_user_id_what, buf + 0x18,
                   *reinterpret_cast<int*>(buf + 0x14));
            chat.m_msgLen = (unsigned char)*reinterpret_cast<int*>(buf + 0x36);
            memcpy(chat.m_msg, buf + 0x3a, *reinterpret_cast<int*>(buf + 0x36));
            chat.packetSize = (unsigned short)(chat.m_msgLen + 0x3b);
            unsigned int size = (unsigned int)chat.packetSize;
            CMonitorServerProxy* proxy =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
            proxy->SendPacket(reinterpret_cast<char*>(&chat), (int)size);
            return 0;
        }
    }
    G_GameWorld()->send_chat_msg(user,
                                 *reinterpret_cast<char*>(buf + 0x0d),
                                 *reinterpret_cast<unsigned short*>(buf + 0x0e),
                                 *reinterpret_cast<unsigned int*>(buf + 0x10),
                                 buf + 0x3a,
                                 *reinterpret_cast<int*>(buf + 0x36),
                                 0, 0, (ENUM_NOTIPACKET)0x72);
    return 0;
}
int Dispatcher_CreatureSendMessage::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CreatureSendMessage::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_FightVillageMonster::Dispatcher_FightVillageMonster() {}
Dispatcher_FightVillageMonster::~Dispatcher_FightVillageMonster() {}
int Dispatcher_FightVillageMonster::dispatch_sig(CUser* user, PacketBuf& packet)
{
    GameWorld* world = G_GameWorld();
    if (world->IsIntegratedPvPBaseChannel()) {
        return 0;
    }
    int serverType = G_CEnvironment()->m_serverEnvir.m_serverType;
    if (serverType == 1 || serverType == 2) {
        return 0;
    }
    if (user == 0) {
        return LineFunc(0xdc18, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->get_state() != 3) {
        return 0;
    }
    unsigned short monsterIdx = 0;
    if (!packet.get_short(monsterIdx)) {
        return LineFunc(0xdc26, __PRETTY_FUNCTION__, 0, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0) {
        party = G_CGameManager()->GetParty();
        if (party != 0) {
            party->set_single_play(user);
        }
    }
    if (party != 0) {
        int ret = party->OnFightVillageMonster(user, (int)monsterIdx);
        if (ret != 0) {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10b, (unsigned char)(ret & 0xff));
        }
    }
    return 0;
}
int Dispatcher_FightVillageMonster::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_FightVillageMonster::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GuildAttendanceInfo::Dispatcher_GuildAttendanceInfo() {}
Dispatcher_GuildAttendanceInfo::~Dispatcher_GuildAttendanceInfo() {}
int Dispatcher_GuildAttendanceInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int guildkey;
    unsigned int characNo;
    char local_11;
    unsigned short len;
    unsigned int serverGroup;
    CGuildServerProxy* proxy;

    (void)param;
    if (user->get_state() <= 2)
        return 0;
    guildkey = user->get_charac_guildkey();
    if (guildkey == 0)
        return 0;
    // ORIG ctor Packet_Guild_Attendance_Info(local_23) @0x0822c384（W，唯一导出符号）。
    Packet_Guild_Attendance_Info local_23;
    *(reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&local_23) + 0x4)) =
        (unsigned int)guildkey;
    characNo = (unsigned int)user->getCurCharacNo();
    *(reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&local_23) + 0x8)) =
        characNo;
    local_11 = *(reinterpret_cast<char*>(&msg) + 0xd);
    len = *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&local_23) + 0x2);
    serverGroup = (unsigned int)user->GetServerGroup();
    proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)serverGroup);
    proxy->SendTcpPacket(reinterpret_cast<char*>(&local_23), len);
    return 0;
}
int Dispatcher_GuildAttendanceInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 char(msg+0xd)，失败记 LineFunc(0x35d9)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(p + 0xd)))
    {
        return LineFunc(0x35d9, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_GuildMasterDelegate::Dispatcher_GuildMasterDelegate() {}
Dispatcher_GuildMasterDelegate::~Dispatcher_GuildMasterDelegate() {}
int Dispatcher_GuildMasterDelegate::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        unsigned int guildKey = (unsigned int)user->get_charac_guildkey();
        if (guildKey == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9e, 0x22);
            return 0;
        }
        if (user->IsGuildMaster() == 1)
        {
            int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xe);
            if (secu != 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9e,
                                         (unsigned char)(secu & 0xff));
                return 0;
            }
            int masterNameLen = 0;
            if (!packet.get_int(masterNameLen))
            {
                return LineFunc(
                    0xa35b,
                    __PRETTY_FUNCTION__,
                    0, 0);
            }
            char masterName[30];
            if (!packet.get_str(masterName, 0x1e, masterNameLen))
            {
                return LineFunc(
                    0xa35e,
                    __PRETTY_FUNCTION__,
                    0, 0);
            }
            unsigned int characNo = (unsigned int)user->get_charac_no(-1);
            ENUM_SERVER_GROUP group = user->GetServerGroup();
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(group);
            proxy->SendGuildMasterDelegate(guildKey, characNo, masterNameLen,
                                           masterName);
            return 0;
        }
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9e, 0x24);
        return 0;
    }
    return LineFunc(
        0xa337,
        __PRETTY_FUNCTION__,
        0, 0);
}
int Dispatcher_GuildMasterDelegate::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GuildMasterDelegate::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_New_Gmdebug_Command::Dispatcher_New_Gmdebug_Command() {}
Dispatcher_New_Gmdebug_Command::~Dispatcher_New_Gmdebug_Command() {}
int Dispatcher_New_Gmdebug_Command::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char local_1a = 1;                   // -0x1a（后被 string 复用，语义上为临时）
    char local_19 = 1;                   // -0x19：GM 通过标志
    char* local_1c;                      // -0x18：get_packet(0) 结果
    int local_18;                        // -0x14：*(int*)(pkt+0xd)
    int local_14;                        // -0x10：vector size（填充前）
    game_master::CCommand* local_c;      // -0xc：GetCommand 结果
    char cVar1;

    local_1c = packet.get_packet(0);
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xa9bc, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned int uVar4 = user->get_acc_id();
    cVar1 = (char)GlobalData::s_pGMAccounts_->isGM(uVar4);
    if (cVar1 != 1)
    {
        local_19 = 0;
    }
    if (local_19 != 1)
    {
        return 0;
    }
    cMyTrace local_34(__PRETTY_FUNCTION__, 0xa9c4, 0);
    local_34("GM Pass Complete!");
    std::vector<std::string> local_40;
    local_18 = *(int*)(local_1c + 0xd);
    *(unsigned char*)(local_1c + local_18 + 0x11) = 0;
    user->getSeparatedTextUnits(local_40, local_1c + 0xd, " ", 0);
    while (local_40.size() < 10)
    {
        local_40.push_back("");
    }
    PacketGuard local_4c;
    DisPatcher_DebugCommand local_50;
    if (user->isGMUser())
    {
        game_master::CParser local_178;
        local_178.m_user = user;
        local_178.m_extra = local_40;
        local_c = GlobalData::s_GameMasterMgr->GetCommand(local_40[0]);
        if (local_c == 0)
        {
            oldGmRoutine(user, packet, local_40);
        }
        else
        {
            local_c->SetParser(&local_178);
            GlobalData::s_GameMasterMgr->HandleRequest(local_c);
        }
    }
    return 0;
}
int Dispatcher_New_Gmdebug_Command::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_New_Gmdebug_Command::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PowerWarProcessInfo::Dispatcher_PowerWarProcessInfo() {}
Dispatcher_PowerWarProcessInfo::~Dispatcher_PowerWarProcessInfo() {}
int Dispatcher_PowerWarProcessInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x821ea32 (83 insn)
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return LineFunc(0xd721,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    const unsigned int characNo = static_cast<unsigned int>(user->get_charac_no(-1));
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(user->GetServerGroup()));
    if (proxy != 0)
    {
        proxy->SendPowerWarProcessInfo(characNo);
    }
    PacketGuard response;
    response.put_header(1, 0xec);
    response.put_byte(1);
    response.finalize(true);
    user->Send(response);
    return 0;
}
int Dispatcher_PowerWarProcessInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PowerWarProcessInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PvpMissionHPPercent::Dispatcher_PvpMissionHPPercent() {}
Dispatcher_PvpMissionHPPercent::~Dispatcher_PvpMissionHPPercent() {}
// ORIG 0x081d5864（报告 Dispatcher_PvpMissionHPPercent/process.md）：
// 仅 curCharacR 门禁；UpdateMissionCondition(m_hpPercents, m_missionIdxs)。
int Dispatcher_PvpMissionHPPercent::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x3930,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    MSG_PVP_MISSION_HP_PERCENT& req = static_cast<MSG_PVP_MISSION_HP_PERCENT&>(msg);
    stSendClearedMission& cleared = static_cast<stSendClearedMission&>(param);
    user->UpdateMissionCondition(req.m_hpPercents, cleared.m_missionIdxs);
    return 0;
}
int Dispatcher_PvpMissionHPPercent::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PVP_MISSION_HP_PERCENT& data = static_cast<MSG_PVP_MISSION_HP_PERCENT&>(msg);
    packet.get_byte(data.m_0d);
    if (data.m_0d == 0 || data.m_0d > 4)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3924, 5);
        trace("[@missionSystem] HackUser: <clearMissionCount : %d>",
              static_cast<unsigned int>(data.m_0d));
        return LineFunc(0x3925,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    data.m_hpPercents.resize(static_cast<size_t>(data.m_0d), 0);
    for (int i = 0; i < static_cast<int>(data.m_0d); ++i)
    {
        packet.get_short(data.m_hpPercents[i]);
    }
    return 0;
}

void Dispatcher_PvpMissionHPPercent::send(CUser* user, ParamBase& param)
{
    std::vector<int>& vals =
        *reinterpret_cast<std::vector<int>*>((char*)&param + 4);
    PacketGuard guard;
    guard.put_header(0, 0x141);
    guard.put_byte((int)vals.size());
    for (unsigned int i = 0; i < vals.size(); ++i)
    {
        guard.put_short(vals[i]);
    }
    guard.finalize(true);
    user->Send(guard);
}


Dispatcher_RegisterToBlackList::Dispatcher_RegisterToBlackList() {}
Dispatcher_RegisterToBlackList::~Dispatcher_RegisterToBlackList() {}
int Dispatcher_RegisterToBlackList::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081cdc70：仅 get_state()<=2 → -1（无 user 空指针检查）。
    if (user->get_state() <= 2)
    {
        return -1;
    }
    return 0;
}
// ORIG T 0x08229644：解封卷轴资格链（0xffff=金币通道直接通过）。
int Dispatcher_UnsealRandomOption::_check_unseal_scroll_item(
    CUser* user, const Inven_Item* item, unsigned short slot)
{
    if (slot != 0xffff)
    {
    CInventory* inven = (CInventory*)(const CInventory*)user->getCurCharacInvenR();
        Inven_Item scroll = inven->GetInvenSlot(1, (int)slot);
        CItem* proto =
            G_CDataManager()->find_item(*(const int*)&scroll.m_addInfo);
        if (proto == 0)
        {
            return 0x15;
        }
        if (!proto->is_stackable())
        {
            return 0x11;
        }
        if (*(int*)((char*)proto + 0x328) == -1)   // ORIG +0x328 卷轴限等字段
        {
            return 1;
        }
        int script =
            (int)(size_t)G_CDataManager()->GetExpertJobScript(2);   // ORIG 参数常量 2
        int possibleLevel =
            ((STExpertJobScript*)script)->GetUnsealScrollPossibleLevel(1 /*占位*/ );
        // GetUnsealScrollPossibleLevel 无参版本按报告签名还原见下（重载澄清）：
        int limit = possibleLevel;
        if (limit == -1)
        {
            return 1;
        }
        CItem* base = G_CDataManager()->find_item(item->m_addInfo);
        if (base != 0 && limit < base->getUsableLevel())
        {
            return 0x11;
        }
    }
    return 0;
}

int Dispatcher_UnsealRandomOption::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08228e5e：随机选项解封（金币/卷轴双通道）。
    if (user == 0 || user->get_state() < 3 || user->getCurCharacR() == 0)
        return 0;
    if (user->CheckInTrade())
        return 0;

    short slotA = 0;                       // 目标装备槽位（ORIG get_short(short&)）
    unsigned short paySlot = 0;            // 支付槽位（0xffff=金币，ORIG get_short(unsigned short&)）
    if (!packet.get_short(slotA))
        return LineFunc(0xe67c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(paySlot))
        return LineFunc(0xe681,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if ((int)user->CheckItemLock(1, (int)slotA) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0xd5);
        return 0;
    }

    CInventory* invenW = user->getCurCharacInvenW();
    Inven_Item* item = invenW->GetInvenRef(1, (int)slotA);
    if (item == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x11);
        return 0;
    }
    CItem* proto = G_CDataManager()->find_item(item->m_addInfo);
    if (proto == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x11);
        return 0;
    }
    const bool unlockGateFail =
        !(proto->get_rarity() < 3 && item->GetUpgrade() < 0xb);
    if (unlockGateFail)
    {
        int secuErr = GlobalData::s_pSecuProtectionField->Check(
            user, (SECURITY_PROTCTION)0x25);
        if (secuErr != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191,
                                     (unsigned char)(secuErr & 0xff));
            return 0;
        }
    }

    if (!item->m_random.m_random.empty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x13);
        return 0;
    }

    int checkErr = _check_unseal_scroll_item(user, item, paySlot);
    if (checkErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191,
                                 (unsigned char)(checkErr & 0xff));
        return 0;
    }

    // key 打包：rarity@b0 | optionGrade@b1 | usableLevel@b2..3（ORIG 帧连续视图）。
    const int rarity = proto->get_rarity();
    const unsigned short usableLevel = (unsigned short)proto->getUsableLevel();
    const unsigned char optionGrade =
        (unsigned char)proto->GetRandomOptionGrade();
    const int key = rarity | (optionGrade << 8) | ((int)usableLevel << 16);
    std::map<int, int>& priceTable = G_CDataManager()->m_randomOption.m_map178;
    std::map<int, int>::const_iterator found = priceTable.find(key);
    if (found == priceTable.end())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x11);
        return 0;
    }

    if (paySlot == 0xffff)
    {
        const int goldCost = found->second;
        if (user->getCurCharacInvenR()->get_money() < goldCost)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 10);
            return 0;
        }
    }
    else
    {
        Inven_Item scroll = user->getCurCharacInvenR()->GetInvenSlot(1, paySlot);
        if (scroll.m_addInfo < 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x16);
            return 0;
        }
    }

    RandomOption* opts = &item->m_random.m_random;
    const bool gave = GlobalData::g_randomOptionHandle->give_option(
        (unsigned long)item->m_addInfo, (ENUM_RARITY)rarity,
        (int)usableLevel, proto->getItemGroupName(),
        (int)optionGrade, opts);
    if (!gave)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x13);
        return 0;
    }

    user->m_historyLog.RandomOption(*item, randomOptionReason::T_REGENERATION);
    if (paySlot == 0xffff)
    {
        statistc_proxy::add(user, "log_random_option", "unseal_gold",
                            (unsigned int)found->second, "%d,'%s'",
                            user->get_charac_10_level_section(),
                            GlobalData::s_systemTime_.getCurDate());
        if (user->getCurCharacInvenW()->use_money(found->second,
                                                  (eMoneySubReason)0x2c, true) != 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 10);
            return 0;
        }
    }
    else
    {
        statistc_proxy::add(user, "log_random_option", "unseal_cnt", 1u,
                            "%d,'%s'", user->get_charac_10_level_section(),
                            GlobalData::s_systemTime_.getCurDate());
        if (user->getCurCharacInvenW()->use_item((int)paySlot, 1, 1) != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x16);
            return 0;
        }
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             (int)paySlot);
    }

    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x191);
    PacketGuard packetOut;
    packetOut.put_header(0, 0xe);
    packetOut.put_byte(0);
    packetOut.put_short(1);   // ORIG put_short(1)（NEW 缺此字段，回包格式差异）
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, (int)slotA,
                                               packetOut);
    packetOut.finalize(true);
    user->Send(packetOut);
    return 0;
}
int Dispatcher_RegisterToBlackList::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cdbc6：int@0x10（len）→ str@0x14（max 0x1e）
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x10)))
        return LineFunc(0x267d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m + 0x14, 0x1e, *reinterpret_cast<int*>(m + 0x10)))
        return LineFunc(0x2680, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_RequestAssaultPrice::Dispatcher_RequestAssaultPrice() {}
Dispatcher_RequestAssaultPrice::~Dispatcher_RequestAssaultPrice() {}

Dispatcher_RequestDisjointItem::Dispatcher_RequestDisjointItem() {}
Dispatcher_RequestDisjointItem::~Dispatcher_RequestDisjointItem() {}
int Dispatcher_RequestDisjointItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081cfcac：check_error(三参) 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x2a1f)；err==0 → 按 msg 字段分解物品。
    int err = check_error(user, msg, param);
    *(int*)(reinterpret_cast<char*>(&param) + 0x4) = err;

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LineFunc(0x2a1f,
                 __PRETTY_FUNCTION__,
                 err, (unsigned int)user->get_acc_id());
        return 0;
    }

    // msg+0xe：ushort 对象 uid → find_from_world；msg+0x10：short 槽；
    // msg+0x12：char（有符号）背包类型。命令字 0xef，区域 0xffff。
    unsigned short uid =
        *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&msg) + 0xe);
    CUser* target = G_GameWorld()->find_from_world(uid);

    DisPatcher_DisJointItem::_disjoint(
        user,
        (int)(*reinterpret_cast<short*>(reinterpret_cast<char*>(&msg) + 0x10)),
        (int)(signed char)(*reinterpret_cast<unsigned char*>(
            reinterpret_cast<char*>(&msg) + 0x12)),
        (ENUM_CMDPACKET)0xef,
        target,
        0xffff);
    return 0;
}
int Dispatcher_RequestDisjointItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cfb28：short@0xe → short@0x10 → char@0x12
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<short*>(m + 0xe)))
        return LineFunc(0x29ed, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<short*>(m + 0x10)))
        return LineFunc(0x29f0, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(m + 0x12)))
        return LineFunc(0x29f3, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_RequestDisjointItem::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stRequestDisjointItemParam& p = static_cast<stRequestDisjointItemParam&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xef, (unsigned char)p.m_4);
}

Dispatcher_ResponseAddPvPBuddy::Dispatcher_ResponseAddPvPBuddy() {}
Dispatcher_ResponseAddPvPBuddy::~Dispatcher_ResponseAddPvPBuddy() {}
int Dispatcher_ResponseAddPvPBuddy::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    char type = 0;                                   // -0x37
    int nameLen = 0;                                 // -0x3c
    char name[0x1e];                                 // -0x36（ORIG 清零 0x1e 字节）
    memset(name, 0, sizeof(name));
    if (!packet.get_byte(type))
        return LineFunc(0xdf53, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(nameLen))
        return LineFunc(0xdf56, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(name, 0x1e, nameLen))
        return LineFunc(0xdf59, __PRETTY_FUNCTION__, 0, 0);
    GlobalData::s_community_proxy->SendResAddPvPBuddy(
        user->get_acc_id(),
        (unsigned int)user->getCurCharacNo(),
        type,
        name,
        nameLen);
    return 0;
}

int Dispatcher_ResponseAddPvPBuddy::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ResponseAddPvPBuddy::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SaveCharacterOption::Dispatcher_SaveCharacterOption() {}
Dispatcher_SaveCharacterOption::~Dispatcher_SaveCharacterOption() {}
int Dispatcher_SaveCharacterOption::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 2)
    {
        return -2;
    }
    return user->getCurCharacNo() == 0 ? -3 : 0;
}
// ORIG 0x081ce97c（报告 Dispatcher_SaveCharacterOption/process.md）：
// param+4=check_error；>0 → 0；<0 LineFunc(0x283d)；==0 setSaveCharacOption(true)
// + SetCurCharacOption(msg+0x11)。
int Dispatcher_SaveCharacterOption::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const int err = check_error(user, msg);
    *(int*)((char*)&param + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LineFunc(0x283d,
                 __PRETTY_FUNCTION__,
                 err, (unsigned int)user->get_acc_id());
        return 0;
    }
    user->setSaveCharacOption(true);
    MSG_SAVE_CHARACTER_OPTION& req = static_cast<MSG_SAVE_CHARACTER_OPTION&>(msg);
    user->SetCurCharacOption(*reinterpret_cast<CharacterOption*>(
        reinterpret_cast<char*>(&req) + 0x11));
    return 0;
}
int Dispatcher_SaveCharacterOption::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081ce8bc：int@0xd（len）须 <0x201，否则 LineFunc(0x2829)；binary len 字节至 0x11
    char* m = reinterpret_cast<char*>(&msg);
    unsigned int len = 0;
    if (!packet.get_int(len) || len >= 0x201)
        return LineFunc(0x2829, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_binary(m + 0x11, (int)len))
        return LineFunc(0x282b, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_SecretShopOpenClose::Dispatcher_SecretShopOpenClose() {}
Dispatcher_SecretShopOpenClose::~Dispatcher_SecretShopOpenClose() {}
int Dispatcher_SecretShopOpenClose::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d28b4：check_error(三参) 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x2fdc)；err==0 → 设置/清除秘密商店购买位。
    int err = check_error(user, msg, param);
    *(int*)(reinterpret_cast<char*>(&param) + 0x4) = err;

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LineFunc(0x2fdc,
                 __PRETTY_FUNCTION__,
                 err, 0);
        return 0;
    }

    void* shopData = user->GetSecretShopData();
    if (shopData == 0)
    {
        *(int*)(reinterpret_cast<char*>(&param) + 0x4) = 0x7fffffff;
        return 0;
    }

    // msg+0xd：购买开关字节（非 0 → true）。
    bool buy =
        *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&msg) + 0xd) != 0;
    int pos = user->GetUserPosInParty();
    reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(shopData)->SetBuying(pos, buy);
    return 0;
}
int Dispatcher_SecretShopOpenClose::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 char(msg+0xd)，失败记 LineFunc(0x2fc4)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(p + 0xd)))
    {
        return LineFunc(0x2fc4, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_SecretShopOpenClose::send(CUser* user, ParamBase& param)
{
    (void)user;
    // ORIG 0x19：读 +0x04 与 0x7fffffff 比较，条件真则提前返回（无副作用）。
    stSecretShopOpenCloseParam& p = static_cast<stSecretShopOpenCloseParam&>(param);
    if (p.m_4 == 0x7fffffff)
        return;
}

Dispatcher_SecurityCardAuthReq::Dispatcher_SecurityCardAuthReq() {}
Dispatcher_SecurityCardAuthReq::~Dispatcher_SecurityCardAuthReq() {}
int Dispatcher_SecurityCardAuthReq::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820a2c4
    // ORIG cmp $0x2,get_state; setle → state<=2 走断言
    if (user->get_state() <= 2)
    {
        return LineFunc(0xa5fd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // card.isActivate() || pad.isActivate()
    bool authing;
    if (user->getSecurityCard()->isActivate() || user->getPad()->isActivate())
    {
        authing = true;
    }
    else
    {
        authing = false;
    }
    if (!authing)
    {
        return LineFunc(0xa625,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    if (user->getSecurityCard()->isActivate())
    {
        if (user->getSecurityCard()->getFailCnt() > 2)   // ORIG cmp $0x2; seta
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa6, 0x7a);
            return 0;
        }
    }
    else
    {
        if (user->getPad()->getFailCnt() > 2)            // ORIG cmp $0x2; setg
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa6, 0x88);
            return 0;
        }
    }

    // ORIG：Check 返回值丢弃，恒 return 0
    GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0);
    return 0;
}
int Dispatcher_SecurityCardAuthReq::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardAuthReq::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_SecurityCardCertKey::Dispatcher_SecurityCardCertKey() {}
Dispatcher_SecurityCardCertKey::~Dispatcher_SecurityCardCertKey() {}
int Dispatcher_SecurityCardCertKey::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820a6bc（docs/class_func_reports/Dispatcher_SecurityCardCertKey/dispatch_sig.md）
    char certKey[6];                          // local_1a -0x16
    unsigned int keyLen = 0;                  // local_14 -0x10
    int idx;                                  // local_10 -0xc

    // +0x00：state < 3 → 拒绝
    if (user->get_state() < 3)
        return LineFunc(0xa675,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x46：清零
    keyLen = 0;
    memset(certKey, 0, 6);

    // +0x5a：读长度与 6 位认证码
    if (!packet.get_int((unsigned int&)keyLen))
        return LineFunc(0xa681,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_str(certKey, 6, keyLen))
        return LineFunc(0xa683,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0xed：已激活且认证标志已开 → 拒绝
    if (user->getSecurityCard()->isActivate()
        && user->getSecurityCard()->isCertFlagOn())
        return LineFunc(0xa688,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0x144：已认证 → 拒绝
    if (user->getSecurityCard()->isCertified())
        return LineFunc(0xa68c,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x184：前 5 位必须为数字（'0'..'9'；ORIG 为 movsbl 减 0x30 后无符号 ≤9 判定）
    for (idx = 0; idx <= 4; idx = idx + 1)
    {
        if ((unsigned int)((int)(signed char)certKey[idx] - 0x30) > 9)
            return LineFunc(0xa691,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }

    // +0x1da：行为日志（action：未激活=2、已激活=3）
    if (!user->getSecurityCard()->isActivate())
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)1, (ENUM_SECU_SERVICE_ACTION)2);
    else
        user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)1, (ENUM_SECU_SERVICE_ACTION)3);

    // +0x233：Hades 安全等级切换（1=ENUM_HADES_SECURITY；value：未激活=0x1a、已激活=0x1b）
    if (!user->getSecurityCard()->isActivate())
        ((XNuclear::CHades*)user->getHades())->ChangeSecurity((XNuclear::ENUM_HADES_SECURITY)1, 0x1a);
    else
        ((XNuclear::CHades*)user->getHades())->ChangeSecurity((XNuclear::ENUM_HADES_SECURITY)1, 0x1b);

    // +0x292：DB 激活请求
    DB_SecurityCardActivate::makeRequest((unsigned int)user->GetUID(),
                                         user->get_acc_id(),
                                         certKey,
                                         user->getSecurityCard()->isActivate());
    return 0;
}
int Dispatcher_SecurityCardCertKey::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardCertKey::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpdateServerMessage::Dispatcher_UpdateServerMessage() {}
Dispatcher_UpdateServerMessage::~Dispatcher_UpdateServerMessage() {}
int Dispatcher_UpdateServerMessage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821d9ee..0x0821df69 (0x57c 字节)。
    static const char* const kPretty =
        "virtual int Dispatcher_UpdateServerMessage::dispatch_sig(CUser*, PacketBuf&)";

    if (user->get_state() != 3)
    {
        const unsigned int accId = user->get_acc_id();
        char* idStr = NumberToString(accId, 0);
        cMyTrace trace(kPretty, 0xd1c6, 0);
        trace("ServerMessage : Dispatcher_UpdateServerMessage => User State : %d, m_id : %s",
              user->get_state(), idStr);
        return LineFunc(0xd1c7, kPretty, 0, 0);
    }

    PacketGuard guard;
    guard.put_header(1, 0xdd);

    unsigned char msgIdx = 0;
    if (packet.get_byte(msgIdx) != 1)
    {
        const int r = LineFunc(0xd1d0, kPretty, 0, 0);
        return r;
    }
    int titleLen = 0;
    if (packet.get_int(titleLen) != 1)
    {
        const int r = LineFunc(0xd1d5, kPretty, 0, 0);
        return r;
    }

    if (titleLen < 0 || 0x31 < titleLen)
    {
        guard.put_byte(0);
        guard.put_byte(1);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }

    char title[50];   // -0x8e，0x32
    if (packet.get_str(title, 0x32, titleLen) != 1)
    {
        const int r = LineFunc(0xd1e4, kPretty, 0, 0);
        return r;
    }
    if (G_CDataManager()->hasPreventString(title))
    {
        guard.put_byte(0);
        guard.put_byte(0x17);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }

    int contentLen = 0;
    if (packet.get_int(contentLen) != 1)
    {
        const int r = LineFunc(0xd1f4, kPretty, 0, 0);
        return r;
    }

    char content[30];   // -0x56，0x1e，读取前清零
    memset(content, 0, sizeof(content));
    if (packet.get_str(content, 0x1e, contentLen) != 1)
    {
        const int r = LineFunc(0xd1f9, kPretty, 0, 0);
        return r;
    }

    if (strcmp(content, user->getCurCharacName()) != 0)
        return LineFunc(0xd1fd, kPretty, 0, 0);

    const int curSec = GlobalData::s_systemTime_.getCurSec();
    CSharedServerMessageManager* mgr = G_CGameManager()->GetSharedServerMessageManager();
    if (mgr != 0)
    {
        int condResult = 0;
        if (!mgr->CheckUserCondition(user, (int)msgIdx, condResult))
        {
            guard.put_byte(0);
            if (condResult == 0)
                guard.put_byte(3);
            else
                guard.put_byte(condResult);
        }
        else if (!mgr->IsChangableMessage((int)msgIdx, curSec))
        {
            guard.put_byte(0);
            guard.put_byte(1);
        }
        else
        {
            mgr->UpdateServerMessage(user, (int)msgIdx, title, content, curSec);
            guard.put_byte(1);
        }
    }
    guard.put_byte((int)msgIdx);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
int Dispatcher_UpdateServerMessage::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpdateServerMessage::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeAccountCargo::Dispatcher_UpgradeAccountCargo() {}
Dispatcher_UpgradeAccountCargo::~Dispatcher_UpgradeAccountCargo() {}
int Dispatcher_UpgradeAccountCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8224fb0：state<=2 → 0；否则 CAccountCargo::UpgradeAccountCargo(user)（静态）。
    if (user->get_state() <= 2)
    {
        return 0;
    }
    CAccountCargo::UpgradeAccountCargo(user);
    return 0;
}
int Dispatcher_UpgradeAccountCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeAccountCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeItemSeparate::Dispatcher_UpgradeItemSeparate() {}
Dispatcher_UpgradeItemSeparate::~Dispatcher_UpgradeItemSeparate() {}
// ORIG 0x081cbe84（报告 Dispatcher_UpgradeItemSeparate/check_error.md）：
// state==3 且 curCharac!=0 否则 7；Secu Check(user,0x27) 非 0 原值返回；
// CheckItemLock(1, msg+0x1b word) → 0xd5；否则 0。
int Dispatcher_UpgradeItemSeparate::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
    {
        int secuResult = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x27);
        if (secuResult != 0)
        {
            return secuResult;
        }
        unsigned short itemIdx =
            *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&msg) + 0x1b);
        if (user->CheckItemLock(1, (int)itemIdx))
        {
            return 0xd5;
        }
        return 0;
    }
    return 7;
}

// ORIG 0x081cc106（报告 Dispatcher_UpgradeItemSeparate/process.md）：
// err>0 → SendCmdErrorPacket((ENUM_CMDPACKET)0x1b7, err&0xff) 后 return -1；
// 否则 m_itemUpgradeSeparate(+0x24)->item_upgrade_system(user, msg, 0x1b7)。
int Dispatcher_UpgradeItemSeparate::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    int err = check_error(user, msg);
    if (err > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b7, (unsigned char)err);
        return -1;
    }
    WongWork::IItemUpgrade* system =
        (WongWork::IItemUpgrade*)G_CDataManager()->m_itemUpgradeSeparate;
    system->item_upgrade_system(user, static_cast<MSG_ITEM_UPGRADE*>(&msg), 0x1b7);
    return 0;
}
int Dispatcher_UpgradeItemSeparate::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* base = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0x1b)))
    {
        return LineFunc(0x228e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(base + 0x1d)))
    {
        return LineFunc(0x2290,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(base + 0x23)))
    {
        return LineFunc(0x2292,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned int strLen = 0;
    if (!packet.get_int(strLen))
    {
        return LineFunc(0x229a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(base + 0x28, 0x81, static_cast<int>(strLen)))
    {
        return LineFunc(0x229d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}


Dispatcher_UseSharedEffectItem::Dispatcher_UseSharedEffectItem() {}
Dispatcher_UseSharedEffectItem::~Dispatcher_UseSharedEffectItem() {}
int Dispatcher_UseSharedEffectItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int itemIdx = 0;                    // -0x18
    short slot = 0;                     // -0x1a
    if (!packet.get_int(itemIdx))
    {
        return LineFunc(0xd919,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(slot))
    {
        return LineFunc(0xd91a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() != 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 1);
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 1);
        return 0;
    }
    CInventory* inven = user->getCurCharacInvenW();
    Inven_Item item = inven->GetInvenSlot(1, (int)slot);
    if (item.isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 0x16);
        return 0;
    }
    if (!(0 < item.m_addInfo2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 0x16);
        return 0;
    }
    if (item.m_addInfo != itemIdx)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 1);
        return 0;
    }
    CStackableItem* proto =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(itemIdx));
    CStackableItem* found = proto;
    if (found == 0 || !found->IsSharedEffectItem())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 0x11);
        return 0;
    }
    if (inven->delete_item((INVEN_TYPE)1, (int)slot, 1, (eItemDelReason)3, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xff, 1);
        return 0;
    }
    PacketGuard guard;
    guard.put_header(1, 0xff);
    guard.put_byte(1);
    guard.put_short((int)slot);
    guard.finalize(true);
    user->Send(guard);
    guard.clear();
    guard.put_header(0, 0xdb);
    guard.put_short(user->get_unique_id() & 0xffff);
    guard.put_int(itemIdx);
    guard.finalize(true);
    G_GameWorld()->send_all_with_state(guard, 3);
    return 0;
}
int Dispatcher_UseSharedEffectItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseSharedEffectItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_VerifyCreatureQuest::Dispatcher_VerifyCreatureQuest() {}
Dispatcher_VerifyCreatureQuest::~Dispatcher_VerifyCreatureQuest() {}
// ORIG 0x081e324c：user 非空且 state>2 时，对当前角色背包的装备宠物
// 调 VerifyEquipedCreatureQuest（进化任务资格校验/任务列表包补发）。
int Dispatcher_VerifyCreatureQuest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user != 0 && user->get_state() > 2)
    {
        user->getCurCharacInvenR()->GetCreatureMgrR()->VerifyEquipedCreatureQuest();
    }
    return 0;
}
int Dispatcher_VerifyCreatureQuest::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_WarRoomWpPerMonster::Dispatcher_WarRoomWpPerMonster() {}
Dispatcher_WarRoomWpPerMonster::~Dispatcher_WarRoomWpPerMonster() {}
int Dispatcher_WarRoomWpPerMonster::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty = "int Dispatcher_WarRoomWpPerMonster::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x39af, pretty, 0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x39b2, pretty, 0, 0);
    }
    return user->get_state() == 8 ? 0 : -1;
}
int Dispatcher_WarRoomWpPerMonster::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d5ca8
    int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }

    // ORIG GetWarRoomIndex → movswl（short）→ CGameManager::GetWarRoom(int)
    short warRoomIndex = user->GetWarRoomIndex();
    WarRoom* warRoom = G_CGameManager()->GetWarRoom((int)warRoomIndex);
    if (warRoom == 0)
    {
        return LineFunc(0x3999,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }

    CUser* target = 0;
    if (*(unsigned short*)((char*)&msg + 0x15) != 0xffff)   // ORIG movzwl 0x15; cmp $0xffff
    {
        target = G_GameWorld()->find_from_world(*(unsigned short*)((char*)&msg + 0x15));
        if (target == 0)
        {
            return LineFunc(0x39a4,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }
    }

    *(int*)((char*)&param + 4) = warRoom->HandleWpPerMonster(
        user,
        *(int*)((char*)&msg + 0xd),
        *(int*)((char*)&msg + 0x11),
        target);
    return 0;
}
int Dispatcher_WarRoomWpPerMonster::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d5bcc：int@0xd → int@0x11 → ushort@0x15
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0xd)))
        return LineFunc(0x397d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x11)))
        return LineFunc(0x3980, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(m + 0x15)))
        return LineFunc(0x3983, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_WarRoomWpPerMonster::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stWarRoomWp& p = static_cast<stWarRoomWp&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x179, (unsigned char)p.m_4);
}

Dispatcher_AbnormalUseStackable::Dispatcher_AbnormalUseStackable() {}
Dispatcher_AbnormalUseStackable::~Dispatcher_AbnormalUseStackable() {}
int Dispatcher_AbnormalUseStackable::check_error(CUser* user, MSG_BASE& msg) { return 0; }
// ORIG 0x082649ae（报告 Dispatcher_AbnormalUseStackable/process.md）：
// check_error==0 → addServerHackCnt(user, 0xfa0, 1, msg+0x10, msg+0x14)。
int Dispatcher_AbnormalUseStackable::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    MSG_ABNORMAL_USE_STACKABLE& req = static_cast<MSG_ABNORMAL_USE_STACKABLE&>(msg);
    int err = check_error(user, msg);
    if (err == 0)
    {
        WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
        analyzer->addServerHackCnt(user, (WongWork::ENUM_HACK_TYPE)0xfa0, 1,
                                   (unsigned int)req.m_field10, (unsigned int)req.m_field14);
    }
    return 0;
}
int Dispatcher_AbnormalUseStackable::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0x10)，失败记 LineFunc(0x530)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0x10)))
    {
        return LineFunc(0x530, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x T：读 unsigned int(msg+0x14)，失败记 LineFunc(0x534)
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0x14)))
    {
        return LineFunc(0x534, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_AuctionBuyItemApiece::Dispatcher_AuctionBuyItemApiece() {}
Dispatcher_AuctionBuyItemApiece::~Dispatcher_AuctionBuyItemApiece() {}
int Dispatcher_AuctionBuyItemApiece::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08214e44: AuctionPreCheck(0x14f, PAY_TYPE 0)
    int result = user->AuctionPreCheck(static_cast<ENUM_CMDPACKET>(0x14f),
                                       static_cast<PAY_TYPE>(0));
    if (result >= 0)
        return result;

    // +0x03a: PCK_AUCTION_BUY_ITEM_APIECE_GA 请求包
    PCK_AUCTION_BUY_ITEM_APIECE_GA pck;
    // +0x045: pck+0x12 = GetUID；pck+0x16 = getCurCharacNo
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&pck) + 0x12) = user->GetUID();
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&pck) + 0x16) = user->getCurCharacNo();

    int price = 0;
    if (!packet.get_int(price))
    {
        return LineFunc(0xc501, __PRETTY_FUNCTION__, 0, 0);
    }
    // pck+0x26 = 竞拍单价/件数 int
    if (!packet.get_int(*reinterpret_cast<int*>(reinterpret_cast<char*>(&pck) + 0x26)))
    {
        return LineFunc(0xc502, __PRETTY_FUNCTION__, 0, 0);
    }
    // pck+0x1e = char[8] 竞拍键
    if (!packet.get_binary(reinterpret_cast<char*>(&pck) + 0x1e, 8))
    {
        return LineFunc(0xc503, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x139: char itemKey[13] 清零后读 0xd 字节
    char itemKey[13];
    itemKey[0] = 0;  itemKey[1] = 0;  itemKey[2] = 0;  itemKey[3] = 0;
    itemKey[4] = 0;  itemKey[5] = 0;  itemKey[6] = 0;  itemKey[7] = 0;
    itemKey[8] = 0;  itemKey[9] = 0;  itemKey[10] = 0; itemKey[11] = 0;
    itemKey[12] = 0;
    if (!packet.get_binary(itemKey, 13))
    {
        return LineFunc(0xc508, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x1a0: 余额不足 → error 0xbc,0x90
    if (user->getCurCharacMoney() < price)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xbc), 0x90);
        return 0;
    }

    // +0x1fc: use_money(price, 0x1a, true)
    if (!(user->getCurCharacInvenW()->use_money(
              price, static_cast<eMoneySubReason>(0x1a), true)))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xbc), 0x93);
        return 0;
    }

    // +0x233: SaveMoneyCoin → 发 GA 包
    user->SaveMoneyCoin();
    GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));

    // +0x24f: price > 4999 → 统计流 SIG_AUCTION_MONEY_SUB
    if (price > 4999)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xc529);
        CStreamGuard guard(stream, true);
        **guard << 0x309;
        **guard << static_cast<int>(user->GetUID());
        SIG_AUCTION_MONEY_SUB* sig = guard->GetInBuffer<SIG_AUCTION_MONEY_SUB>();
        memset(sig, 0, 0x28);
        *reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 4) = price;
        strncpy(reinterpret_cast<char*>(sig) + 8, itemKey, 13);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    }

    return 0;
}
int Dispatcher_AuctionBuyItemApiece::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionBuyItemApiece::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionMyBiddingInfo::Dispatcher_AuctionMyBiddingInfo() {}
Dispatcher_AuctionMyBiddingInfo::~Dispatcher_AuctionMyBiddingInfo() {}
int Dispatcher_AuctionMyBiddingInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08216b9a
    // ORIG _ZL6gmList：TU 内 static int[5]（0x093701d8，0x14 字节 .data，初值忠实转写）
    static int s_gmList[5] = {0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae};
    int result;

    // call CAuctionServerProxy::IsRunning(s_auction_proxy); xor $1 → !IsRunning
    if (!GlobalData::s_auction_proxy->IsRunning())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc0, 0x8c);
        return 0;
    }

    // cmp $0x2,get_state; jle → state<=2 即 invalid；否则 getCurCharacR==0 亦 invalid
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xc836,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // PCK_AUCTION_MY_BIDDING_INFO_GA local_3a（ORIG 栈对象，字段 +0x0e/+0x12 为裸偏移）
    PCK_AUCTION_MY_BIDDING_INFO_GA req;
    *(unsigned int*)((char*)&req + 0x0e) = user->GetUID();               // local_28
    *(unsigned int*)((char*)&req + 0x12) =
        (unsigned int)user->getCurCharacNo();                            // local_24
    GlobalData::s_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&req);

    // ORIG cmpl $0x4,-0xc; setle → i <= 4（含 4，共 5 轮）
    for (int i = 0; i <= 4; ++i)
    {
        if (s_gmList[i] == (int)user->getCurCharacNo())
        {
            int characNo = (int)user->getCurCharacNo();
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xc84e, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionMyBiddingInfo", characNo);
            break;
        }
    }
    result = 0;
    return result;
}
int Dispatcher_AuctionMyBiddingInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionMyBiddingInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BuyCeraShopLimitItem::Dispatcher_BuyCeraShopLimitItem() {}
Dispatcher_BuyCeraShopLimitItem::~Dispatcher_BuyCeraShopLimitItem() {}


Dispatcher_CallGuildCreateRight::Dispatcher_CallGuildCreateRight() {}
Dispatcher_CallGuildCreateRight::~Dispatcher_CallGuildCreateRight() {}
int Dispatcher_CallGuildCreateRight::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ffcc2..0x08200125 (0x464 字节)。packet 未读取（ORIG 不消费包）。
    static const char* const kPretty =
        "virtual int Dispatcher_CallGuildCreateRight::dispatch_sig(CUser*, PacketBuf&)";

    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return LineFunc(0x93f4, kPretty, 0, 0);

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x1c))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0xd1);
        return 0;
    }

    const unsigned int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)6);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, (unsigned char)(secuErr & 0xff));
        return 0;
    }

    // 公会名（getCreateGuildName 返回 char*，构造局部 std::string）
    std::string guildName(user->getCreateGuildName());

    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(guildName.c_str()))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x9f);
        return 0;
    }
    if (CSyncSlangFilter::GetInstance()->HasSlangName(guildName))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x9f);
        return 0;
    }

    bool badName = false;
    if (CSyncSlangFilter::GetInstance()->HasSlang(guildName))
    {
        badName = true;
    }
    else if (G_CDataManager()->hasSpecialCharacter(user->getCreateGuildName()))
    {
        badName = true;
    }
    if (badName)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x9f);
        return 0;
    }

    if (user->get_charac_guildkey() != 0 || user->get_guild_create_right() == 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x20);
        return 0;
    }

    if (!user->isSetGuildCreateFlag(2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x6d);
        return 0;
    }
    if (!user->isSetGuildCreateFlag(4))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x71);
        return 0;
    }

    const int money = user->getCurCharacInvenR()->get_money();
    const int cost = G_CDataManager()->m_guildParameter.m_field0;   // ORIG dm+0xa50c
    if (money < cost)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x1f);
        return 0;
    }

    user->getCurCharacInvenW()->use_money(cost, (eMoneySubReason)0xb, true);
    send_packet_guildcreateright(user, 1);
    user->set_guild_create_right(1);
    user->UpdateData();

    const char* guildAddr = user->getCreateGuildAddr();
    const char* name = user->getCreateGuildName();
    const unsigned int characNo = (unsigned int)user->getCurCharacNo();
    const ENUM_SERVER_GROUP group = user->GetServerGroup();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(group);
    proxy->SendGuildCreate(characNo, const_cast<char*>(name), const_cast<char*>(guildAddr));

    user->resetGuildCreateFlag();
    user->resetTempGuildNameAddr();
    return 0;
}
int Dispatcher_CallGuildCreateRight::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildCreateRight::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Check3rdPartyConcent::Dispatcher_Check3rdPartyConcent() {}
Dispatcher_Check3rdPartyConcent::~Dispatcher_Check3rdPartyConcent() {}
int Dispatcher_Check3rdPartyConcent::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081df3d0 T：state==3 时 DB_Check3rdPartyConcent::makeRequest(
    // user->GetUID(), user->get_acc_id())；恒返回 0。
    if (user->get_state() == 3)
    {
        DB_Check3rdPartyConcent::makeRequest(user->GetUID(),
                                            user->get_acc_id());
    }
    return 0;
}
int Dispatcher_Check3rdPartyConcent::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CheckGuildNameDouble::Dispatcher_CheckGuildNameDouble() {}
Dispatcher_CheckGuildNameDouble::~Dispatcher_CheckGuildNameDouble() {}
int Dispatcher_CheckGuildNameDouble::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082084e8 [0x082084e8, 0x082087a3]
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return LineFunc(0xa369,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int len = 0;                                             // -0x14（size_t 视图）
    if (!packet.get_int(len))
    {
        return LineFunc(0xa37e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (len < 0 || 0x16 < len)
    {
        return LineFunc(0xa381,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char* name = user->getCreateGuildName();                 // -0x10
    if (name == 0)
    {
        return LineFunc(0xa386,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(name, 0x17, len))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9f, 0x6a);
        return 0;
    }
    if (!checkValidCharacName(name))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9f, 0x6c);
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "PacketDispatcher_Impl_1.cpp", 0xa39d), true);
    **guard << 0xeb;
    **guard << static_cast<int>(user->GetUID());
    SIG_DOUBLE_CHECK_GUILD_NAME* sig = guard->GetInBuffer<SIG_DOUBLE_CHECK_GUILD_NAME>();
    memset(sig, 0, 0x17);
    memcpy(sig, name, len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}
int Dispatcher_CheckGuildNameDouble::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CheckGuildNameDouble::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Compound_ExtreamItem::Dispatcher_Compound_ExtreamItem() {}
Dispatcher_Compound_ExtreamItem::~Dispatcher_Compound_ExtreamItem() {}
int Dispatcher_Compound_ExtreamItem::_push_newitem()
{
    // ORIG 0x08225b54：inven=getCurCharacInvenW(m_user) → insertItemIntoInventory(
    // m_resultItem, 0x30, 1, 1) → m_newSlot=返回值；返回 m_newSlot > -1
    CInventory* inven = m_user->getCurCharacInvenW();
    m_newSlot = (short)inven->insertItemIntoInventory(m_resultItem,
                                                      (eItemAddReason)0x30, true, true);
    return m_newSlot > -1 ? 1 : 0;
}
int Dispatcher_Compound_ExtreamItem::_remove_victim()
{
    // ORIG 0x08225a88：先删 m_slots[0..1]（0x2a 原因），任一失败返 0；
    // 再删 m_field4，成功返 1
    CInventory* inven = m_user->getCurCharacInvenW();
    for (int i = 0; i < 2; ++i)
    {
        if (inven->delete_item((INVEN_TYPE)1, m_slots[i], 1,
                               (eItemDelReason)0x2a, true) != 1)
        {
            return 0;
        }
    }
    return inven->delete_item((INVEN_TYPE)1, m_field4, 1, (eItemDelReason)0x2a, true) == 1
               ? 1
               : 0;
}
int Dispatcher_Compound_ExtreamItem::_reset()
{
    // ORIG 0x0822577e T：i=0..1 → ushort(this+8+i*2)=0、Inven_Item::reset(this+0xc+i*0x3d)、
    // uint(this+0x88+i*4)=0；随后 ushort(this+6)=0、Inven_Item::reset(this+0x90)、
    // ushort(this+4)=0、byte(this+0xcd)=2、byte(this+0xd4)=2。
    char* p = reinterpret_cast<char*>(this);
    for (int i = 0; i < 2; ++i)
    {
        *reinterpret_cast<unsigned short*>(p + 8 + i * 2) = 0;
        reinterpret_cast<Inven_Item*>(p + 0xc + i * 0x3d)->reset();
        *reinterpret_cast<unsigned int*>(p + 0x88 + i * 4) = 0;
    }
    *reinterpret_cast<unsigned short*>(p + 6) = 0;
    reinterpret_cast<Inven_Item*>(p + 0x90)->reset();
    *reinterpret_cast<unsigned short*>(p + 4) = 0;
    p[0xcd] = 2;
    p[0xd4] = 2;
    return 0;
}
int Dispatcher_Compound_ExtreamItem::_send_result()
{
    PacketGuard guard;                                           // ORIG local_1c
    InterfacePacketBuf& pkt = *(InterfacePacketBuf*)&guard;
    pkt.put_header(1, 0x146);
    pkt.put_byte(1);
    for (int i = 0; i < 2; ++i)                                  // ORIG local_10 < 2
    {
        pkt.put_byte(0);
        pkt.put_short((int)(unsigned short)m_slots[i]);          // +0x08/+0x0a
        pkt.put_int(1);
    }
    pkt.put_byte(0);
    pkt.put_short((int)(unsigned short)m_field4);                // +0x04
    pkt.put_int(1);
    pkt.put_short((int)m_newSlot);                               // +0x06（short 符号扩展）
    pkt.put_int(m_resultItem.m_addInfo);                         // +0x92（m_resultItem+0x02）
    pkt.put_int(m_resultItem.m_addInfo2);                        // +0x97（m_resultItem+0x07）
    pkt.put_byte((int)m_resultItem.GetItemAttr() & 0xff);        // m_resultItem（+0x90）
    pkt.put_short((int)(unsigned short)m_resultItem.m_fieldb);   // +0x9b（endurance）
    pkt.put_byte((int)(unsigned char)m_resultItem.m_field0);     // m_resultItem+0x00
    if (!G_GameWorld()->IsEnchantRevisionChannel())
    {
        pkt.put_int(m_resultItem.m_fieldd);                      // +0x9d（m_resultItem+0x0d）
    }
    else
    {
        pkt.put_int(0);
    }
    pkt.put_byte((int)m_resultItem.m_amp.getAbilityType() & 0xff);       // +0xa1（m_amp）
    pkt.put_short((int)m_resultItem.m_amp.getAbilityValue() & 0xffff);   // getAbilityValue
    pkt.put_packet(m_resultItem);
    pkt.finalize(true);
    m_user->Send(guard);                                         // m_user（+0xd0）
    return 0;                                                    // ORIG 尾部未设 eax，按调用方口径返 0
}

int Dispatcher_Compound_ExtreamItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char cVar3;
    bool bVar2;
    int local_1c;                        // -0x1c：ReSeal 合计
    unsigned char rounded;               // -0x22：fistps 结果
    unsigned short cw;                   // -0x1e/-0x20：x87 控制字

    if (user->get_state() <= 2)          // setle：<=2 直接返回
    {
        return 0;
    }
    m_user = user;                       // +0xd0
    cVar3 = _read_packet(packet);
    if ((char)(cVar3 ^ 1) == 0)          // cVar3==1 继续
    {
        cVar3 = (char)((ExtreamDungeon::CCompound_ExtreamDun_Item*)GlobalData::s_pCompoundBindCube)
                    ->checkCompoundable(&m_materials[0], (const CEquipItem**)m_itemRefs, (char)m_targetKind);
        if (cVar3 == 0)
        {
            cVar3 = (char)user->CheckItemLock(1, (unsigned short)m_slots[0]);
            if ((cVar3 == 0) &&
                (cVar3 = (char)user->CheckItemLock(1, (unsigned short)m_slots[1]), cVar3 == 0))
            {
                bVar2 = false;
            }
            else
            {
                bVar2 = true;
            }
            if (bVar2)
            {
                m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0xd5);
            }
            else
            {
                cVar3 = (char)((ServiceRestrictManager*)ARAD::Singleton<ServiceRestrictManager>::Get())
                            ->isRestricted(user, (RestrictCategory::Enum)1, 0xd);
                if (cVar3 == 0)
                {
                    cVar3 = (char)((ExtreamDungeon::CCompound_ExtreamDun_Item*)GlobalData::s_pCompoundBindCube)
                                ->makeCompoundItem((const CEquipItem**)m_itemRefs, (char)m_select, m_resultItem,
                                                   (ExtreamDungeon::eBindSphereType)(m_targetKind == 3 ? 1 : 0));
                    if (cVar3 == 1)
                    {
                        unsigned int uVar6 = (unsigned int)m_materials[0].GetReSealCount();
                        unsigned int uVar7 = (unsigned int)m_materials[1].GetReSealCount();
                        local_1c = (unsigned char)uVar6 + (unsigned char)uVar7;
                        // flds 2.0f; fdivrp; flds 0.5f; faddp; fldcw 截断; fistps（ORIG 0x8bd6348=2.0f, 0x8bd634c=0.5f）
                        float f = ((float)local_1c / 2.0f) + 0.5f;
                        rounded = (unsigned char)(unsigned short)(int)f;
                        m_resultItem.SetReSealCount(rounded);
                        if (m_resultItem.m_addInfo == 0)   // +0x92（m_resultItem 内偏移）
                        {
                            m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0x11);
                        }
                        else
                        {
                            cVar3 = (char)_remove_victim();
                            if (cVar3 == 0)
                            {
                                m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0x11);
                            }
                            else
                            {
                                _push_newitem();
                                _send_result();
                            }
                        }
                    }
                    else
                    {
                        m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0x11);
                    }
                }
                else
                {
                    m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0xd1);
                }
            }
        }
        else
        {
            m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, (unsigned char)(int)(char)(cVar3));
        }
    }
    else
    {
        m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0);
    }
    return 0;
}
int Dispatcher_Compound_ExtreamItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Compound_ExtreamItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GatheringPartyStatus::Dispatcher_GatheringPartyStatus() {}
Dispatcher_GatheringPartyStatus::~Dispatcher_GatheringPartyStatus() {}
int Dispatcher_GatheringPartyStatus::dispatch_sig(CUser* user, PacketBuf& packet)
{
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

    for (unsigned short i = 0; (unsigned int)i < (unsigned int)recv.m_tryNum; ++i)
    {
        RecvTryEntry& e = recv.m_tryEntries[i];
        if (!packet.get_byte(e.m_a) || !packet.get_byte(e.m_b) ||
            !packet.get_binary((char*)e.m_bin, 4) ||
            !packet.get_int(e.m_val))
        {
            return LineFunc(0x9b09,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }

    if (!packet.get_short(recv.m_partyNum))
        return LineFunc(0x9b12,
                        __PRETTY_FUNCTION__,
                        0, 0);

    const bool isPvp = G_GameWorld()->IsPVPChannel();
    if (isPvp ? ((unsigned int)recv.m_partyNum > 8)
              : ((unsigned int)recv.m_partyNum > 4))
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp",
            __PRETTY_FUNCTION__,
            isPvp ? 0x9b1c : 0x9b24,
            isPvp ? "recv_pk_party_status.party_num > MAX_PVP_MEM\n"
                  : "recv_pk_party_status.party_num > PARTY_PLAYER_MAX\n");
        return LineFunc(isPvp ? 0x9b1d : 0x9b25,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    for (unsigned short i = 0; (unsigned int)i < (unsigned int)recv.m_partyNum; ++i)
    {
        RecvPartyEntry& p = recv.m_partyEntries[i];
        if (!packet.get_byte(p.m_flag) || !packet.get_int(p.m_accId) ||
            !packet.get_int(p.m_id2) || !packet.get_binary((char*)p.m_tag, 4))
        {
            return LineFunc(0x9b2e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }


    Stream* stream =
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x9b4e);
    CStreamGuard guard(stream, true);
    guard << (int)0x8e;
    guard << (int)-1;
    SIG_PARTY_STATUS* sig = guard.GetInBuffer<SIG_PARTY_STATUS>();
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
int Dispatcher_GatheringPartyStatus::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GatheringPartyStatus::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ItemHyperLinkMessage::Dispatcher_ItemHyperLinkMessage() {}
Dispatcher_ItemHyperLinkMessage::~Dispatcher_ItemHyperLinkMessage() {}
int Dispatcher_ItemHyperLinkMessage::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->get_state() < 3)
    {
        return -1;
    }
    const MSG_ITEM_HYPER_LINK_MESSAGE& req =
        static_cast<const MSG_ITEM_HYPER_LINK_MESSAGE&>(msg);
    unsigned char type = req.m_typeD;
    return type == 0x24 || type == 0x25 || type == 0x21 || type == 7 ? -1 : 0;
}

// ORIG T 0x081d98c4：超链接聊天消息读取（MSG_ITEM_HYPER_LINK_MESSAGE）。
int Dispatcher_ItemHyperLinkMessage::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_ITEM_HYPER_LINK_MESSAGE& m =
        static_cast<MSG_ITEM_HYPER_LINK_MESSAGE&>(msg);
    if (!packet.get_byte(m.m_typeD))
        return LineFunc(0x4765,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(m.m_itemSpaceE))
        return LineFunc(0x4768,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(m.m_slot10))
        return LineFunc(0x476b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(m.m_textLen14))
        return LineFunc(0x476e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_str(m.m_text18, 0x100, m.m_textLen14))
        return LineFunc(0x4771,
                        __PRETTY_FUNCTION__,
                        0, 0);

    if (m.m_typeD == 1 || m.m_typeD == 0x26)
    {
        if (!packet.get_int(m.m_nameLen118))
            return LineFunc(0x4785,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_str(m.m_name11c, 0x1e, m.m_nameLen118))
            return LineFunc(0x4788,
                            __PRETTY_FUNCTION__,
                            0, 0);
        char field13a = m.m_field13a;
        if (!packet.get_byte(field13a))
            return LineFunc(0x478c,
                            __PRETTY_FUNCTION__,
                            0, 0);
        m.m_field13a = field13a;
    }

    if (!packet.get_byte(m.m_linkCount13b))
        return LineFunc(0x4792,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (m.m_linkCount13b == 0 || m.m_linkCount13b > 3)
        return LineFunc(0x4795,
                        __PRETTY_FUNCTION__,
                        0, 0);
    for (unsigned char i = 0; i < m.m_linkCount13b; ++i)
    {
        if (!packet.get_binary((char*)m.m_links13c[i], 0x68))
            return LineFunc(0x479a,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }
    return 0;
}

// ORIG T 0x081da584：应答分发（param 无错误码 → OK 包，否则错误包）。
void Dispatcher_ItemHyperLinkMessage::send(CUser* user, ParamBase& param)
{
    stItemHyperLinkMessage& p = static_cast<stItemHyperLinkMessage&>(param);
    if (p.m_4 == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1a9);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a9,
                                 (unsigned char)(p.m_4 & 0xff));
}

// ORIG T 0x081d9c4a：超链接聊天处理（PacketDispatcher_Impl_1.cpp 0x47aa 区）。
int Dispatcher_ItemHyperLinkMessage::process(CUser* user, MSG_BASE& msg,
                                             ParamBase& param)
{
    stItemHyperLinkMessage& outParam =
        static_cast<stItemHyperLinkMessage&>(param);
    MSG_ITEM_HYPER_LINK_MESSAGE& m =
        static_cast<MSG_ITEM_HYPER_LINK_MESSAGE&>(msg);

    int err = check_error(user, msg);
    outParam.m_4 = err;
    if (err >= 1)
        return 0;
    if (err < 0)
    {
        return LineFunc(0x47aa,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }

    G_CDataManager()->rewriteUsableEmoticon(*user, m.m_text18);

    const bool ghostGateFail =
        user->IsCurCharacGhost() &&
        !(m.m_typeD < 9 && ((1u << (m.m_typeD & 0x1f)) & 0x144U) != 0);
    if (ghostGateFail)
        return 0;

    if (m.m_typeD == 0x26)
    {
        // 跨服频道转发（Packet_Monitor_Other_Channel_Chat_Hyper_Link，
        // ORIG 0x081d9eaf 区；ctor W 0x0822c3e6）。
        Packet_Monitor_Other_Channel_Chat_Hyper_Link pkt;
        pkt.m_chatType = 1;
        pkt.m_id = user->get_acc_id();
        pkt.m_characNo = user->getCurCharacNo();
        pkt.server_id = m.m_field13a;
        pkt.m_recverCharId = m.m_slot10;
        pkt.m_buddyNameLen = (char)m.m_nameLen118;
        pkt.m_msgLen = (unsigned char)m.m_textLen14;
        memcpy(pkt.buddy_n_user_id_what, m.m_name11c, m.m_nameLen118);
        memcpy(pkt.m_msg, m.m_text18, m.m_textLen14);
        pkt.m_itemCount = m.m_linkCount13b;
        for (int i = 0; i < m.m_linkCount13b; i++)
            memcpy(pkt.m_items[i] + 0xb, m.m_links13c[i] + 0xc, 0x68);
        pkt.packetSize = (unsigned short)(0x174 + m.m_textLen14);
        GlobalData::s_community_proxy->SendTcpPacket(
            reinterpret_cast<char*>(&pkt), pkt.packetSize);
        return 0;
    }

    unsigned int itemSpace = m.m_itemSpaceE;
    int slot = (int)m.m_slot10;
    if (m.m_typeD == 0xb || m.m_typeD == 0xc || m.m_typeD == 0x23)
    {
        const CInventory* invenR = user->getCurCharacInvenR();
        const Inven_Item* ref =
            invenR->GetInvenRef(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned int)itemSpace), slot);
        const unsigned int addInfo = *(const unsigned int*)&ref->m_addInfo;
        if (ref == 0 ||
            !(*(const int*)&ref->m_addInfo == 0x24 ||
              *(const int*)&ref->m_addInfo == 0x382 ||
              *(const int*)&ref->m_addInfo == 0x28d150 ||
              *(const int*)&ref->m_addInfo == 0x292090dd ||
              *(const int*)&ref->m_addInfo == 0x29209138))
        {
            (void)addInfo;
            return LineFunc(0x4801,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }
        int secuErr = GlobalData::s_pSecuProtectionField->Check(
            user, (SECURITY_PROTCTION)0x24);
        if (secuErr != 0)
        {
            outParam.m_4 = secuErr;
            return 0;
        }
        if (user->CheckItemLock(
                (int)GetInvenTypeFromItemSpace(
                    (ENUM_ITEMSPACE)(unsigned int)itemSpace),
                slot) != 0)
        {
            outParam.m_4 = 0xd5;
            return 0;
        }
        if (!user->use_stackable((short)slot, (int)itemSpace, -1, -1))
        {
            outParam.m_4 = 0x11;
            return 0;
        }
    }
    else if (m.m_typeD == 0xd || m.m_typeD == 0xf)
    {
        const CInventory* invenR = user->getCurCharacInvenR();
        const Inven_Item* ref =
            invenR->GetInvenRef(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned int)itemSpace), slot);
        if (ref == 0)
            return LineFunc(0x4821,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        const int idxVal = *(const int*)&ref->m_addInfo;
        if (!((idxVal == 0x25 && m.m_typeD == 0xd) ||
              (idxVal == 0x28992f && m.m_typeD == 0xf)))
            return LineFunc(0x4826,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        int secuErr = GlobalData::s_pSecuProtectionField->Check(
            user, (SECURITY_PROTCTION)0x24);
        if (secuErr != 0)
        {
            outParam.m_4 = secuErr;
            return 0;
        }
        if (user->CheckItemLock(
                (int)GetInvenTypeFromItemSpace(
                    (ENUM_ITEMSPACE)(unsigned int)itemSpace),
                slot) != 0)
        {
            outParam.m_4 = 0xd5;
            return 0;
        }
        if (!user->use_stackable((short)slot, (int)itemSpace, -1, -1))
        {
            outParam.m_4 = 0x11;
            return 0;
        }
    }
    else if (m.m_typeD == 0xe)
    {
        const CInventory* invenR = user->getCurCharacInvenR();
        const Inven_Item* ref =
            invenR->GetInvenRef(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned int)itemSpace), slot);
        if (ref == 0 || *(const int*)&ref->m_addInfo != 0x460)
            return LineFunc(0x4847,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
            return LineFunc(0x484b,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        if (!user->use_stackable((short)slot, (int)itemSpace, -1, -1))
        {
            outParam.m_4 = 0x11;
            return 0;
        }
    }
    else if (m.m_typeD == 0x22)
    {
        if (user->get_state() != 3)
            return 0;
        const CInventory* invenR = user->getCurCharacInvenR();
        const Inven_Item* ref =
            invenR->GetInvenRef(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned int)itemSpace), slot);
        if (ref == 0 || *(const int*)&ref->m_addInfo != 0x1d9e)
            return LineFunc(0x485d,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        CItem* proto = G_CDataManager()->find_item(*(const int*)&ref->m_addInfo);
        if (proto == 0 || !proto->is_stackable())
        {
            outParam.m_4 = 0x11;
            return 0;
        }
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)itemSpace), slot, 1,
                (eItemDelReason)3, true))
        {
            outParam.m_4 = 0x11;
            return 0;
        }
    }

    if (GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
    {
        DB_InsertPoliceSaveChatting::makeRequest(
            user->getCurCharacNo(), (char)m.m_typeD, m.m_text18);
    }
    G_GameWorld()->send_chat_msg_hyper_link(user, static_cast<MSG_ITEM_HYPER_LINK_MESSAGE&>(m),
                                            (ENUM_NOTIPACKET)0x172);
    return 0;
}

// ==== DB_InsertPoliceSaveChatting::makeRequest (ORIG T 0x0843c6aa) ====
void DB_InsertPoliceSaveChatting::makeRequest(unsigned int a, char b, char* c)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x904c), true);
    **guard << (int)0x20c;
    **guard << (int)-1;
    **guard << (unsigned int)a;
    **guard << b;
    **guard << c;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
}

Dispatcher_NotifyMessageToGuild::Dispatcher_NotifyMessageToGuild() {}
Dispatcher_NotifyMessageToGuild::~Dispatcher_NotifyMessageToGuild() {}
int Dispatcher_NotifyMessageToGuild::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08207ffe（PacketDispatcher_Impl_1.cpp 0xa2f4 区）
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xa2f4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int guildKey = user->get_charac_guildkey();
    if (guildKey == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9d, 0x22);
        return 0;
    }
    if (!user->IsSubGuildMaster() && !user->IsGuildMaster())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9d, 0x24);
        return 0;
    }
    int len = 0;
    if (!packet.get_int(len))
    {
        return LineFunc(0xa309,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char message[101];
    if (!packet.get_str(message, 0x65, len))
    {
        return LineFunc(0xa30c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    bool invalid = false;
    if (G_CDataManager()->hasPreventString(message))
    {
        invalid = true;
    }
    else if (G_CDataManager()->hasSpecialCharacterExceptSpace(message))
    {
        invalid = true;
    }
    if (invalid)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9d, 0x6c);
        return 0;
    }
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendMessageToGuild((unsigned int)guildKey, len, message);
    PacketGuard guard;
    guard.put_header(1, 0x9d);
    guard.put_byte(1);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
int Dispatcher_NotifyMessageToGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_NotifyMessageToGuild::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_OperateRidableObject::Dispatcher_OperateRidableObject() {}
Dispatcher_OperateRidableObject::~Dispatcher_OperateRidableObject() {}
int Dispatcher_OperateRidableObject::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08226d34：仅检查 get_state()==5（无 user==0 防御分支）。
    if (user->get_state() != 5)
    {
        return 0;
    }
    char operateType[2];
    operateType[0] = 0;
    operateType[1] = 0;
    int ridableIdx = 0;
    if (!packet.get_byte(operateType[0]))
    {
        return LineFunc(0xe2a4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(ridableIdx))
    {
        return LineFunc(0xe2a7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
    {
        return 0;
    }
    int type = (int)(signed char)operateType[0];
    if (type == 2)
    {
        user->m_historyLog.StartRidable((cUserHistoryLog::eRidableType)0, ridableIdx);
    }
    else if (type == 1 || type == 3)
    {
        user->m_historyLog.EndRidable((cUserHistoryLog::eRidableType)0, ridableIdx);
    }
    if (party->SetRidableObjectState(ridableIdx, (eRidableObjectState)type) == 1)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x13d);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13d, 0x13);
    }
    return 0;
}
int Dispatcher_OperateRidableObject::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_OperateRidableObject::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PartyTeleportConfirm::Dispatcher_PartyTeleportConfirm() {}
Dispatcher_PartyTeleportConfirm::~Dispatcher_PartyTeleportConfirm() {}
int Dispatcher_PartyTeleportConfirm::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081dc75c：user 空 → -1；state!=3 → 0x13；
    // PVP/SkillTree 渠道 → 0x13；无队伍 → 0；
    // 传送状态在 (-∞,-2] 或 [4,∞)（即 state<-1 || state>3）→ 0x13；否则 0。
    if (user == 0)
    {
        return 0xffffffff;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    // ORIG 两次 G_GameWorld() 调用（0x080da3a7）形态保留。
    if (G_GameWorld()->IsPVPChannel() != 0)
    {
        return 0x13;
    }
    if (G_GameWorld()->IsPvPSkilTreeChannel() != 0)
    {
        return 0x13;
    }

    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
    {
        return 0;
    }

    // ORIG 首次取 state（cmp $0xff,%al; jge → 有符号 < -1 才进入内层）。
    CPartyTelePort* tp = (CPartyTelePort*)party->GetPartyTelePort();
    if (tp->get_teleport_state() < -1)
    {
        // ORIG 内层重新取 GetPartyTelePort + get_teleport_state。
        CPartyTelePort* tp2 = (CPartyTelePort*)party->GetPartyTelePort();
        if (tp2->get_teleport_state() > 3)
        {
            return 0x13;
        }
    }
    return 0;
}
int Dispatcher_PartyTeleportConfirm::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char* out = reinterpret_cast<char*>(&param);
    const int err = check_error(user, msg);
    *reinterpret_cast<int*>(out + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x4be5,
            __PRETTY_FUNCTION__,
            err, 0);
    }
    *reinterpret_cast<unsigned char*>(out + 8) =
        *reinterpret_cast<const unsigned char*>(
            reinterpret_cast<const char*>(&msg) + 0xd);
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        const int slotNo = party->GetMemberSlotNo(user);
        const unsigned char confirm =
            *reinterpret_cast<const unsigned char*>(out + 8);
        if (confirm == 1)
        {
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->set_teleport_member_state(slotNo, 1);
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->check_cur_teleport_state();
        }
        else if (confirm == 0)
        {
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->set_teleport_member_state(slotNo, 0);
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->set_teleport_state(2);
        }
        else if (confirm == 3)
        {
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->set_teleport_state(3);
        }
    }
    return 0;
}
int Dispatcher_PartyTeleportConfirm::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned char(msg+0xd)，失败记 LineFunc(0x4bd7)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xd)))
    {
        return LineFunc(0x4bd7, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_PartyTeleportConfirm::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081dc830：p4==0 → Ok(0x1b4)+party teleport 状态/处理/复位；否则 Error(0x1b4,p4&0xff)
    char* p = reinterpret_cast<char*>(&param);
    if (*reinterpret_cast<int*>(p + 4) == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1b4);
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            CPartyTelePort* tp = static_cast<CPartyTelePort*>(party->GetPartyTelePort());
            tp->send_teleport_status(0);
            tp = static_cast<CPartyTelePort*>(party->GetPartyTelePort());
            tp->process_teleport();
            if (p[8] != 1)
            {
                tp = static_cast<CPartyTelePort*>(party->GetPartyTelePort());
                tp->reset_teleport_data();
            }
        }
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b4,
                                 (unsigned char)(*(unsigned int*)(p + 4) & 0xff));
    }
}

Dispatcher_PcRoomPlayTimeReward::Dispatcher_PcRoomPlayTimeReward() {}
Dispatcher_PcRoomPlayTimeReward::~Dispatcher_PcRoomPlayTimeReward() {}
int Dispatcher_PcRoomPlayTimeReward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e4f5c
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return 3;
    }
    PcRoomPlayTimeRewardParam& p = static_cast<PcRoomPlayTimeRewardParam&>(param);
    CDataManager* dm = G_CDataManager();
    EventClassify::CEventScriptMng* mng =
        static_cast<EventClassify::CEventScriptMng*>(dm->get_event_script_mng());
    EventClassify::CPcRoomPlayTime* entity =
        static_cast<EventClassify::CPcRoomPlayTime*>(mng->get_event_entity(0x5c));
    if (entity == 0)
    {
        p.m_result = 0xb2;
        return 0;
    }
    p.m_result = entity->verifyUserPcRoomPlayEvent(user);
    if (p.m_result == 0)
    {
        p.m_result = entity->processEventPcRoomPlayTime(user);
    }
    return 0;
}
int Dispatcher_PcRoomPlayTimeReward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_PcRoomPlayTimeReward::send(CUser* user, ParamBase& param)
{
    PcRoomPlayTimeRewardParam& out = static_cast<PcRoomPlayTimeRewardParam&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1ed);
    if (out.m_result == 0)
    {
        packet.put_byte(1);
        packet.put_byte(user->getPcRoomPlayTimeRewardCnt());
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_result);
        if (out.m_result == 0x13)
        {
            packet.put_int(user->getTotalPcRoomPlayTime());
        }
    }
    packet.finalize(true);
    user->Send(packet);
}


Dispatcher_PrecheckSoloTeleport::Dispatcher_PrecheckSoloTeleport() {}
Dispatcher_PrecheckSoloTeleport::~Dispatcher_PrecheckSoloTeleport() {}
int Dispatcher_PrecheckSoloTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e19ca：check_error(三参)；
    // err>0 → 写 param+0x04（继续）；err<0 → 返回 -1；err==0 → 发 monitor 预检包。
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        *(int*)(reinterpret_cast<char*>(&param) + 0x4) = err;
    }
    else if (err < 0)
    {
        return 0xffffffff;
    }

    if (err == 0)
    {
        // ORIG 0x0822c5d2 ctor；+0x0a 起 21 字节名字区 strncpy(msg+0xd, 长度
        // *(int*)(msg+0x2c))；+0x1f int = getCurCharacNo(0x080cbc4e)；SendPacket 0x23。
        Packet_Monitor_Request_Charac_Info pkt;
        strncpy(reinterpret_cast<char*>(&pkt) + 0xa,
                reinterpret_cast<char*>(&msg) + 0xd,
                *(int*)(reinterpret_cast<char*>(&msg) + 0x2c));
        *(int*)(reinterpret_cast<char*>(&pkt) + 0x1f) = user->getCurCharacNo();

        ENUM_SERVER_GROUP group = user->GetServerGroup();
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
        proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x23);
    }
    return 0;
}
int Dispatcher_PrecheckSoloTeleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e1920：int@0x2c（len）→ str@0xd（max 0x1e）
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0x2c)))
        return LineFunc(0x572d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m + 0xd, 0x1e, *reinterpret_cast<int*>(m + 0x2c)))
        return LineFunc(0x5730, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_PrecheckSoloTeleport::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e1acc：p4!=0 → clear+header(1,0x1d6)+byte 0+byte(char)p4
    stSoloTeleportParam& p = static_cast<stSoloTeleportParam&>(param);
    if (p.m_4 != 0)
    {
        PacketGuard pkt;
        pkt.clear();
        pkt.put_header(1, 0x1d6);
        pkt.put_byte(0);
        pkt.put_byte((int)(char)p.m_4);
        pkt.finalize(true);
        user->Send(pkt);
    }
}

Dispatcher_PvpMissionComboClear::Dispatcher_PvpMissionComboClear() {}
Dispatcher_PvpMissionComboClear::~Dispatcher_PvpMissionComboClear() {}
int Dispatcher_PvpMissionComboClear::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d5a5e
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x3958,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    unsigned char comboIdx = *(unsigned char*)((char*)&msg + 0xd);
    if (comboIdx <= 8)   // ORIG cmp $0x8; jbe
    {
        // ORIG 实参序：第1参 = msg+0xe（byte→int），第2参 = msg+0xd（byte→short）
        user->Update_ComboMission(
            (int)*(unsigned char*)((char*)&msg + 0xe),
            (short)(unsigned char)comboIdx);
        return 0;
    }

    // ORIG rep stos 0x40×4 字节清零 char[256]
    char logBuf[256];
    memset(logBuf, 0, 0x100);
    OS_API::snprintf(logBuf, 0x100,
                     "[@missionSystem] invalid Combo Mission index:%d",
                     (unsigned int)comboIdx);
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x3963, 5);
    trace(logBuf);

    if (user->isGMUser())
    {
        user->SendNotiPacketMessage(logBuf, (ENUM_MESSAGE_TYPE)0);
        return 0;
    }
    return LineFunc(0x396a,
                    __PRETTY_FUNCTION__,
                    0, user->get_acc_id());
}
int Dispatcher_PvpMissionComboClear::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d59c6：byte@0xd → byte@0xe
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0xd)))
        return LineFunc(0x394e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0xe)))
        return LineFunc(0x3950, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_Register_Quick_Party::Dispatcher_Register_Quick_Party() {}
Dispatcher_Register_Quick_Party::~Dispatcher_Register_Quick_Party() {}
// ORIG 0x081ddfce（nm 实测，sz 0x43）：state>3 且 getCurCharacR()!=0 → 0，否则 -1。
int Dispatcher_Register_Quick_Party::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() <= 3 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x081de012（报告 Dispatcher_Register_Quick_Party/process.md）：
// 以 MSG+0xe(short)/+0x10(byte) 构造 STQuickPartyPoolMap_Key（默认 0/0xff 后覆盖，
// 与 ORIG 0x082344ca 一致），match_proper_party(key, party, user) 写 param+0x4。
int Dispatcher_Register_Quick_Party::process(CUser* user, MSG_BASE& msg,
                                             ParamBase& param)
{
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    QuickParty::STQuickPartyPoolMap_Key key;
    key.m_dungeonIndex = *(short*)((char*)&msg + 0xe);
    key.m_dungeonDiff = *(unsigned char*)((char*)&msg + 0x10);
    CParty* party = (CParty*)user->GetParty();
    CGameManager* gm = G_CGameManager();
    QuickParty::CQuickPartySystemManager* mgr = gm->GetQuickPartySystemManager();
    *(int*)((char*)&param + 0x4) = mgr->match_proper_party(key, party, user);
    return 0;
}
int Dispatcher_Register_Quick_Party::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081ddf36：short@0xe → char@0x10
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<short*>(m + 0xe)))
        return LineFunc(0x4f1e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(m + 0x10)))
        return LineFunc(0x4f20, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_Register_Quick_Party::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081de0c6：p4==0 → SendCmdOkPacket(0x1bc)；p4==1 → header(1,0x1bc)+byte 0+int p4
    stQuickPartyParam& p = static_cast<stQuickPartyParam&>(param);
    if (p.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1bc);
    }
    else if (p.m_4 == 1)
    {
        PacketGuard pkt;
        pkt.put_header(1, 0x1bc);
        pkt.put_byte(0);
        pkt.put_int(p.m_4);
        pkt.finalize(true);
        user->Send(pkt);
    }
}

Dispatcher_ReportMannerlessUser::Dispatcher_ReportMannerlessUser() {}
Dispatcher_ReportMannerlessUser::~Dispatcher_ReportMannerlessUser() {}
int Dispatcher_ReportMannerlessUser::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9f84,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    Packet_Mannerless_User_Accusation pkt;
    pkt.m_type = user->getCurCharacNo();            // +0x0e ← getCurCharacNo
    pkt.m_characNo = user->GetUID();                // +0x0a ← GetUID（ORIG 字段写入序）
    int type = 4;                                   // -0x50
    if (!packet.get_int(type) || type > 3)
    {
        return LineFunc(0x9f8e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    pkt.m_serverGroup = type;                       // +0x12
    pkt.m_accIdHi = user->get_acc_id();             // +0x1e ← get_acc_id
    unsigned int nameLen = (unsigned int)strlen(user->getCurCharacName());  // -0x54
    unsigned int minLen = 0x1d;                     // -0x4c
    unsigned int copyLen = std::min(nameLen, minLen);
    strncpy(pkt.m_name, user->getCurCharacName(), copyLen);                 // +0x22
    unsigned char groupByte = 0;                    // -0x55
    packet.get_byte(groupByte);
    pkt.m_accIdLo = (unsigned int)groupByte;        // +0x1a ← byte
    if ((unsigned int)groupByte > 0x64)
    {
        return LineFunc(0x9fa0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned int targetLen = 0;                     // -0x5c
    if (!packet.get_int(targetLen))
    {
        return 0;
    }
    if (!packet.get_str(pkt.m_targetName, 0x1e, (int)targetLen))            // +0x40
    {
        return 0;
    }
    if (Packet_Mannerless_User_Accusation::isNeedTypeForChattingMsg(type))
    {
        unsigned int chatLen = 0;                   // -0x60
        if (!packet.get_int(chatLen))
        {
            return 0;
        }
        if (!packet.get_str(pkt.m_chatMsg, 0x100, (int)chatLen))            // +0x5e
        {
            return 0;
        }
    }
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    Inven_Item item = inven->GetInvenSlot(0, 0xb);
    if (item.m_addInfo == 0x677e)
    {
        *((char*)&pkt + 0x15e) = 0x70;              // 包末字节（-0x61）
    }
    // reporterGroup(+0x16) 与 claimed(+0x1a)：
    //   byte==0 → 两者均置 G_CEnvironment()->get_server_group()；
    //   byte!=0 → reporterGroup=user->GetServerGroup()，claimed=byte。
    if (groupByte == 0)
    {
        pkt.m_accIdLo =
            (unsigned int)G_CEnvironment()->get_server_group();
        pkt.m_reporterGroup = pkt.m_accIdLo;
    }
    else
    {
        pkt.m_reporterGroup =
            (unsigned int)user->GetServerGroup();
    }
    if (pkt.m_reporterGroup == pkt.m_accIdLo &&
        targetLen == nameLen &&
        strncmp(pkt.m_name, pkt.m_targetName, targetLen) == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8c, 3);
        return 0;
    }
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket((char*)&pkt, 0x15f);
    return 0;
}
int Dispatcher_ReportMannerlessUser::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ReportMannerlessUser::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Request_PvpExpOfWeek::Dispatcher_Request_PvpExpOfWeek() {}
Dispatcher_Request_PvpExpOfWeek::~Dispatcher_Request_PvpExpOfWeek() {}
// ORIG 0x081fecaa（报告 Dispatcher_Request_PvpExpOfWeek/dispatch_sig.md）：
// PacketGuard：put_header(0,0x39) → put_int(get_pvp_exp_of_week()) →
// finalize(true) → user->Send(pg)；return 0。
int Dispatcher_Request_PvpExpOfWeek::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    PacketGuard pg;
    pg.put_header(0, 0x39);
    pg.put_int(user->get_pvp_exp_of_week());
    pg.finalize(true);
    user->Send(pg);
    return 0;
}
int Dispatcher_Request_PvpExpOfWeek::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Request_PvpExpOfWeek::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_ProcessScan::Dispatcher_Security_ProcessScan() {}
Dispatcher_Security_ProcessScan::~Dispatcher_Security_ProcessScan() {}
int Dispatcher_Security_ProcessScan::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_ProcessScan(user, packet);
    if (result == 2)
    {
        return LineFunc(0xde86,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (result == 3)
    {
        return LineFunc(0xde88,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (result == 1)
    {
        return LineFunc(0xde84,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_ProcessScan::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_ProcessScan::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SeriaRoom_Deco_Event::Dispatcher_SeriaRoom_Deco_Event() {}
Dispatcher_SeriaRoom_Deco_Event::~Dispatcher_SeriaRoom_Deco_Event() {}
int Dispatcher_SeriaRoom_Deco_Event::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81e5e50：state==3 才允许当前活动请求。
    if (user == 0)
    {
        return -1;
    }
    return user->get_state() == 3 ? 0 : 7;
}
int Dispatcher_SeriaRoom_Deco_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stSeriaRoomDecoEvent& p = static_cast<stSeriaRoomDecoEvent&>(param);
    int err = check_error(user, msg);
    p.m_4 = err;
    if (p.m_4 >= 1)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x6010,
                        __PRETTY_FUNCTION__,
                        p.m_4, 0);
    }
    MSG_SERIAROOM_DECO_EVENT& req = static_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    int slot = (int)req.m_decoEventId;
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    Inven_Item item = inven->GetInvenSlot(1, slot);
    CDataManager* dataMgr = G_CDataManager();
    CItem* pItem = dataMgr->find_item(item.m_addInfo);
    if (pItem == 0)
    {
        p.m_4 = 0x11;
        return 0;
    }
    if (dataMgr->getVaildEventItem(item.m_addInfo))
    {
        p.m_8 = req.m_decoEventId;
        bool deleteFlag = false;
        if (deleteFlag)
        {
            user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, slot, 1, (eItemDelReason)3, true);
        }
        return 0;
    }
    p.m_4 = 0x17;
    return 0;
}
int Dispatcher_SeriaRoom_Deco_Event::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SERIAROOM_DECO_EVENT& req = static_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_decoEventId)))
    {
        return LineFunc(0x5fff,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void Dispatcher_SeriaRoom_Deco_Event::send(CUser* user, ParamBase& param)
{
    stSeriaRoomDecoEvent& out = static_cast<stSeriaRoomDecoEvent&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1ef);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_short((unsigned int)(unsigned short)out.m_8);
        packet.put_short((int)out.m_a);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4);
    }
    packet.finalize(true);
    user->Send(packet);
}


Dispatcher_WriteGuildMemberMemo::Dispatcher_WriteGuildMemberMemo() {}
Dispatcher_WriteGuildMemberMemo::~Dispatcher_WriteGuildMemberMemo() {}
int Dispatcher_WriteGuildMemberMemo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821e748: state>2 且角色非空
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xd55c, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x068: 无公会门
    unsigned int guildKey = static_cast<unsigned int>(user->get_charac_guildkey());
    if (guildKey == 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xe3), 0x22);
        return 0;
    }

    // +0x09b: char memo[0x15] 清零
    char memo[21];
    memset(memo, 0, sizeof(memo));

    // +0x110: 读 memo 长度 + 内容
    int memoLen = 0;
    if (!packet.get_int(memoLen))
    {
        return LineFunc(0xd56a, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_str(memo, 0x15, memoLen))
    {
        return LineFunc(0xd56d, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x1a9: 敏感词/特殊字符过滤
    bool invalid = G_CDataManager()->hasPreventString(memo) ||
                   G_CDataManager()->hasSpecialCharacterExceptSpace(memo);
    if (invalid)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xe3), 0x6c);
        return 0;
    }

    // +0x20d: 发公会备忘录
    int characNo = user->get_charac_no(-1);
    CGuildServerProxy* guildProxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    guildProxy->SendMemoToGuild(guildKey, characNo, memoLen, memo);

    // +0x25c: 0xe3 成功回执
    PacketGuard pg;
    pg.put_header(1, 0xe3);
    pg.put_byte(1);
    pg.finalize(true);
    user->Send(pg);
    return 0;
}
int Dispatcher_WriteGuildMemberMemo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_WriteGuildMemberMemo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_Client_Spec_Statistic::Dispatcher_Client_Spec_Statistic() {}
Dispatcher_Client_Spec_Statistic::~Dispatcher_Client_Spec_Statistic() {}
// ORIG 0x081ce794（报告 Dispatcher_Client_Spec_Statistic/send.md）[随行依赖]：
// 构造 Packet_Client_Spec_Statistic（ORIG W 0x0822bc18），写 flag/count/len，
// memcpy hwSpec 条目后经统计代理 SendPacket。
void Dispatcher_Client_Spec_Statistic::send(CUser* user, ParamBase& param)
{
    if (user == 0)
    {
        return;
    }
    char* hwSpec = user->getClientHWSpec();
    const unsigned char specType = *(unsigned char*)((char*)&param + 4);
    const bool special = (specType == 1 || specType == 0x81);
    if (hwSpec[0] != 0 && !special)
    {
        return;
    }
    // ORIG 栈帧对象 0xc06：PacketHeader(0x9ce,0xc06)+flag@0xa+count@0xb+HWSpec[254]@0x12。
    static const unsigned int kSpecStatSize = 0xc06;
    unsigned char stat[kSpecStatSize];
    new (stat) Packet_Client_Spec_Statistic();   // NEEDS_DEP: ORIG W 0x0822bc18
    const unsigned char count = (unsigned char)hwSpec[1];
    stat[0x0a] = special ? 2 : 0;
    stat[0x0b] = count;
    const unsigned short len = (unsigned short)(count * 0xc + 0x12);
    *(unsigned short*)(stat + 2) = len;
    memcpy(stat + 0x12, hwSpec + 4, (unsigned int)count * 0xc);
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
    if (proxy != 0)
    {
        proxy->SendPacket(reinterpret_cast<char*>(stat), (int)len);
    }
}

// ORIG 0x081ce6d4（报告 Dispatcher_Client_Spec_Statistic/process.md）：
// check_error（仅判 user 空）；param+4=msg+0xd byte；memcpy(getClientHWSpec(),msg+0xe,0xbf8)。
int Dispatcher_Client_Spec_Statistic::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    MSG_CLIENT_SPEC_STATISTIC& req = static_cast<MSG_CLIENT_SPEC_STATISTIC&>(msg);
    *(unsigned char*)((char*)&param + 4) =
        *(unsigned char*)(reinterpret_cast<char*>(&req) + 0xd);
    memcpy(user->getClientHWSpec(), reinterpret_cast<char*>(&req) + 0xe, 0xbf8);
    return 0;
}

Dispatcher_CreatureScriptMessage::Dispatcher_CreatureScriptMessage() {}
Dispatcher_CreatureScriptMessage::~Dispatcher_CreatureScriptMessage() {}
int Dispatcher_CreatureScriptMessage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08204d8e（docs/class_func_reports/Dispatcher_CreatureScriptMessage/dispatch_sig.md）
    // -0x157 起为局部聊天消息缓冲（ORIG 独立字段布局）
    unsigned char chatType;                   // base+0x0d  local_14e
    unsigned short target;                    // base+0x0e  local_14d
    unsigned int fromId;                      // base+0x10  local_14b（unsigned get_int）
    int recvNameLen;                          // base+0x14  local_147
    char recvName[0x1e];                      // base+0x18  acStack_143
    int msgLen;                               // base+0x36  local_125
    char msg[0x100];                          // base+0x3a  acStack_121

    // +0x00：state < 3 → return 0
    if (user->get_state() < 3)
        return 0;
    // +0x2b：getCurCharacVill() == 0 → return 0
    if (user->getCurCharacVill() == 0)
        return 0;
    // +0x49：getCurCharacR() == 0 → return 0
    if (user->getCurCharacR() == 0)
        return 0;
    // +0x67：IsEquippedCreature(GetCreatureMgrR(getCurCharacInvenR())) != 1 → return 0
    if (!user->getCurCharacInvenR()->GetCreatureMgrR()->IsEquippedCreature())
        return 0;

    // +0x93：5 段读取（每段失败 → LineFunc 对应行号后 jmp 尾部）
    if (!packet.get_byte((char&)chatType))
        goto L_9cf3;
    if (!packet.get_short(target))
        goto L_9cf4;
    if (!packet.get_int((unsigned int&)fromId))
        goto L_9cf5;
    if (!packet.get_int(msgLen))
        goto L_9cf6;
    if (!packet.get_str(msg, 0x100, msgLen))
        goto L_9cf7;

    // +0x20d：chatType==7 或 ==1 → 继续读接收方信息
    if (chatType == 7 || chatType == 1)
    {
        if (!packet.get_int(recvNameLen))
            return LineFunc(0x9cfb,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_str(recvName, 0x1e, recvNameLen))
        if (chatType == 7)
        {
            Packet_Monitor_Other_Channel_Chat notify;                     // ctor 0822b8e6
            notify.m_characNo = user->getCurCharacNo();                   // +0x13
            notify.m_buddyNameLen = (char)recvNameLen;                    // +0x1b
            memcpy(notify.buddy_n_user_id_what, recvName, recvNameLen);   // +0x1c
            notify.m_msgLen = (unsigned char)msgLen;                      // +0x3a
            memcpy(notify.m_msg, msg, msgLen);                            // +0x3b
            {
                unsigned short total = (unsigned short)((unsigned char)msgLen + 0x3b);  // local_294
                CMonitorServerProxy* proxy =
                    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
                proxy->SendPacket((char*)&notify, (int)(unsigned int)total);
            }
            return 0;
        }
    }

    // +0x3bd：普通频道聊天
    G_GameWorld()->send_chat_msg(user, (char)chatType, target, fromId,
                                 msg, msgLen, 0, 0, (ENUM_NOTIPACKET)0x77);
    return 0;

L_9cf3:
    return LineFunc(0x9cf3,
                    __PRETTY_FUNCTION__,
                    0, 0);
L_9cf4:
    return LineFunc(0x9cf4,
                    __PRETTY_FUNCTION__,
                    0, 0);
L_9cf5:
    return LineFunc(0x9cf5,
                    __PRETTY_FUNCTION__,
                    0, 0);
L_9cf6:
    return LineFunc(0x9cf6,
                    __PRETTY_FUNCTION__,
                    0, 0);
L_9cf7:
    return LineFunc(0x9cf7,
                    __PRETTY_FUNCTION__,
                    0, 0);
}
int Dispatcher_CreatureScriptMessage::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CreatureScriptMessage::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Mercenary_Competition::Dispatcher_Mercenary_Competition() {}
Dispatcher_Mercenary_Competition::~Dispatcher_Mercenary_Competition() {}
int Dispatcher_Mercenary_Competition::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ddd00 (0xd4 insn)
    MsgMercenaryCompetition& req = reinterpret_cast<MsgMercenaryCompetition&>(msg);
    SendMercenaryCompetitionResult& result = static_cast<SendMercenaryCompetitionResult&>(param);
    if (user == 0)
        return -1;
    if (user->get_state() != 3)
        return -1;

    unsigned int targetNo = *reinterpret_cast<unsigned int*>(req.m_pad + 3);   // msg+0x10
    const char b0x15 = static_cast<char>(req.m_pad[8]);                        // msg+0x15
    const char b0x14 = static_cast<char>(req.m_pad[7]);                        // msg+0x14

    if (user->GetParty() != 0 && user->getCurCharacNo() == static_cast<int>(targetNo))
    {
        *reinterpret_cast<char*>(reinterpret_cast<char*>(&result) + 0x04) = 0; // param+4 = 0
        return -1;
    }

    char* pResult = reinterpret_cast<char*>(&result);
    // ORIG: Competition_Mercenary_Charac(user, msg+0x10, (char)msg+0x15, (char)msg+0x14)
    const int ret = user->Competition_Mercenary_Charac(
        targetNo, b0x15, b0x14);
    pResult[0x04] = static_cast<char>(ret & 0xff);   // ORIG mov %al,0x4(%edx)
    *reinterpret_cast<int*>(pResult + 0x08) = static_cast<int>(targetNo);
    pResult[0x0c] = b0x15;
    pResult[0x0d] = b0x14;
    // ORIG: cUserHistoryLog::MercenaryCompetition(user->m_historyLog, targetNo, b0x15, b0x14)
    user->m_historyLog.MercenaryCompetition(targetNo, b0x15, b0x14);
    return 0;
}

int Dispatcher_Mercenary_Competition::read(PacketBuf& packet, MSG_BASE& msg)  // ORIG 0x081ddc24 (81 insn)
{
    MsgMercenaryCompetition& req = reinterpret_cast<MsgMercenaryCompetition&>(msg);
    static const char kFunc[] =
        "virtual int Dispatcher_Mercenary_Competition::read(PacketBuf&, MSG_BASE&)";
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(req.m_pad + 3)))     // msg+0x10
        return LineFunc(0x4eda, kFunc, 0, 0);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_pad[7])))              // msg+0x14
        return LineFunc(0x4edc, kFunc, 0, 0);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_pad[8])))              // msg+0x15
        return LineFunc(0x4ede, kFunc, 0, 0);
    return 0;
}

void Dispatcher_Mercenary_Competition::send(CUser* user, ParamBase& param)    // ORIG 0x081dde20 (83 insn)
{
    SendMercenaryCompetitionResult& result = static_cast<SendMercenaryCompetitionResult&>(param);
    char* pResult = reinterpret_cast<char*>(&result);
    PacketGuard guard;
    guard.put_header(1, 0x1bb);
    guard.put_byte(pResult[0x04] != 0);
    if (pResult[0x04] == 0)
    {
        guard.put_byte(0x15);                                                  // 失败错误码
    }
    else
    {
        guard.put_int(*reinterpret_cast<int*>(pResult + 0x08));
        guard.put_byte(static_cast<unsigned int>(static_cast<char>(pResult[0x0d])));
        guard.put_byte(static_cast<unsigned int>(static_cast<char>(pResult[0x0c])));
    }
    guard.finalize(true);
    user->Send(guard);
}

Dispatcher_OpenGuildCreateWindow::Dispatcher_OpenGuildCreateWindow() {}
Dispatcher_OpenGuildCreateWindow::~Dispatcher_OpenGuildCreateWindow() {}
int Dispatcher_OpenGuildCreateWindow::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2 ||
        user->getCurCharacR() == 0)
    {
        return LineFunc(0xa3e3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    user->resetGuildCreateFlag();
    user->resetTempGuildNameAddr();
    return 0;
}
int Dispatcher_OpenGuildCreateWindow::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_OpenGuildCreateWindow::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RepairDisjointMachine::Dispatcher_RepairDisjointMachine() {}
Dispatcher_RepairDisjointMachine::~Dispatcher_RepairDisjointMachine() {}
// ORIG 0x081cfddc（nm 实测，sz 0x5f）：state>2 且 curCharac!=0；ExpertJobType!=3 → -2。
int Dispatcher_RepairDisjointMachine::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        if (user->GetCurCharacExpertJobType() != 3)
        {
            return -2;
        }
        return 0;
    }
    return -1;
}

// ORIG 0x081cfe3c（报告 Dispatcher_RepairDisjointMachine/process.md）。
int Dispatcher_RepairDisjointMachine::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRepairDisjointMachineParam& out = static_cast<stRepairDisjointMachineParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2a54,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    int result = expert_job::CDisjointer::OnRepairDisjointMachine(user);
    if (result != 0)
    {
        out.m_4 = result;
    }
    return 0;
}
int Dispatcher_RepairDisjointMachine::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_RepairDisjointMachine::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stRepairDisjointMachineParam& p = static_cast<stRepairDisjointMachineParam&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xf0, (unsigned char)p.m_4);
}

Dispatcher_AuctionAskAveragePrice::Dispatcher_AuctionAskAveragePrice() {}
Dispatcher_AuctionAskAveragePrice::~Dispatcher_AuctionAskAveragePrice() {}
int Dispatcher_AuctionAskAveragePrice::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08213940..0x08213e14 (0x4d5 字节)。GM 表 _ZL6gmList @0x093abb4c。
    // 包偏移（asm 权威）：GA/PCK_AUCTION_ASK_AVERAGE_PRICE_GA（ctor W 0x0822e4d8）
    //   uid@0x12, characNo@0x16, itemIdx@0x1a, flag@0x1e,
    //   循环 i=0..2: byte@0x1f+i, short@0x22+2i, char@0x2a+i, 尾 byte@0x2e；ROI sort@0x22。
    // GP/PCK_AUCTION_ASK_AVERAGE_PRICE_GP（ctor W 0x0822e94c）
    //   uid@0x12, characNo@0x16, itemIdx@0x1a, flag@0x1e。
    static const char* const kPretty =
        "virtual int Dispatcher_AuctionAskAveragePrice::dispatch_sig(CUser*, PacketBuf&)";
    static const int gmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

    unsigned char payType = 0;
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc1e5, kPretty, 0, 0);
    if (payType > 1)
        return LineFunc(0xc1e8, kPretty, 0, 0);

    if (IsAuctionServerDisconnected(payType))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb9, 0x8c, (PAY_TYPE)(int)payType);
        return 0;
    }

    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return LineFunc(0xc207, kPretty, 0, 0);

    // GA 包 ctor + 头两字段
    PCK_AUCTION_ASK_AVERAGE_PRICE_GA pck;
    char* const body = reinterpret_cast<char*>(&pck);
    *reinterpret_cast<unsigned int*>(body + 0x12) = user->GetUID();
    *reinterpret_cast<int*>(body + 0x16) = user->getCurCharacNo();

    unsigned int itemIdx = 0;
    if (packet.get_int(itemIdx) != 1)
        return LineFunc(0xc217, kPretty, 0, 0);
    *reinterpret_cast<unsigned int*>(body + 0x1a) = itemIdx;

    unsigned char flag = 0;
    if (packet.get_byte(flag) != 1)
        return LineFunc(0xc21a, kPretty, 0, 0);
    body[0x1e] = static_cast<char>(flag);

    if (payType == 1)
    {
        PCK_AUCTION_ASK_AVERAGE_PRICE_GP pckGp;   // ctor W 0x0822e94c
        char* const gp = reinterpret_cast<char*>(&pckGp);
        *reinterpret_cast<unsigned int*>(gp + 0x12) = user->GetUID();
        *reinterpret_cast<int*>(gp + 0x16) = user->getCurCharacNo();
        *reinterpret_cast<unsigned int*>(gp + 0x1a) = itemIdx;
        gp[0x1e] = body[0x1e];   // movzbl -0x35 / mov %al,-0x35（GA→GP 包 flag 拷贝）
        GlobalData::s_cera_auction_proxy->SendPacket(
            reinterpret_cast<nsl::PACKET_HEADER*>(&pckGp));
    }
    else
    {
        for (int i = 0; i <= 2; i++)   // cmpl $0x2 / setle
        {
            unsigned char upgrade = 0;
            if (packet.get_byte(upgrade) != 1)
                return LineFunc(0xc22f, kPretty, 0, 0);
            body[0x1f + i] = static_cast<char>(upgrade);

            short category = 0;
            if (packet.get_short(category) != 1)
                return LineFunc(0xc232, kPretty, 0, 0);
            *reinterpret_cast<short*>(body + 0x22 + i * 2) = category;

            char refine = 0;
            if (packet.get_byte(refine) != 1)
                return LineFunc(0xc235, kPretty, 0, 0);
            body[0x2a + i] = refine;
        }

        CItem* item = G_CDataManager()->find_item((int)itemIdx);
        if (item == 0)
            return LineFunc(0xc23a, kPretty, 0, 0);

        if (item->IsRandomOption())   // ORIG 虚调用 vptr+0x4c
        {
            for (int i = 0; i <= 2; i++)
            {
                short* const v = reinterpret_cast<short*>(body + 0x22 + i * 2);
                if (*v == 0)
                    *v = 0x7530;   // 30000
            }
        }

        reinterpret_cast<ROI_Category*>(body + 0x22)->_sort();

        unsigned char tail = 0;
        if (packet.get_byte(tail) != 1)
            return LineFunc(0xc247, kPretty, 0, 0);
        body[0x2e] = static_cast<char>(tail);

        GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }

    for (int i = 0; i <= 4; i++)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(kPretty, 0xc258, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionAskAveragePrice",
                  user->getCurCharacNo());
            break;
        }
    }
    return 0;
}
int Dispatcher_AuctionAskAveragePrice::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionAskAveragePrice::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionSearchByItemKey::Dispatcher_AuctionSearchByItemKey() {}
Dispatcher_AuctionSearchByItemKey::~Dispatcher_AuctionSearchByItemKey() {}
// ============================================================================
// ORIG 0x082159fc（docs/class_func_reports/Dispatcher_AuctionSearchByItemKey/dispatch_sig.md）
// 包布局依 ORIG ctor 0x0822e6aa(GA)/0x0822ea84(GP) 反汇编还原：
//   PACKET_HEADER(0x12) + memset(0,0x81) + setCategory(0x06 GA / 0x12 GP)
//   + setPacketID(0x06) + setSize(0x81)；TSearchByItemId_C 子对象 ctor（0x0822e694）
//   位于 memset 之前，其内容被 memset 覆盖（等效无操作）。

// 权威布局（PCK_AUCTION_SEARCH_BY_ITEMKEY_GA/GP.h 目前为空壳）落地后应替换本 TU
// 本地镜像；字段偏移 +0x12 uid、+0x16 characNo、+0x1a int、+0x1e/1f/20 byte、
// +0x21 short、ROI_Category@+0x23(0xc)、itemKeys[0x14]@+0x31。
// ============================================================================
#pragma pack(push, 1)
struct DspAuctionSearchByItemKeyPck : public nsl::PACKET_HEADER
{
    DspAuctionSearchByItemKeyPck(int category)
    {
        memset(this, 0, 0x81);
        setCategory(category);           // GA: 0x06 / GP: 0x12（ORIG ctor 常量）
        setPacketID(0x06);
        setSize(0x81);
    }

    unsigned int uid;                    // +0x12
    unsigned int characNo;               // +0x16
    unsigned int field1a;                // +0x1a（TSearchByItemId_C +0x00）
    unsigned char b1e;                   // +0x1e
    unsigned char b1f;                   // +0x1f
    unsigned char b20;                   // +0x20
    unsigned short s21;                  // +0x21
    unsigned short roiKey0;              // +0x23（ROI_Category）
    unsigned short roiKey2;              // +0x25
    unsigned short roiKey4;              // +0x27
    unsigned char pad29[0x6];            // +0x29..+0x2e
    unsigned char b2f;                   // +0x2f
    unsigned char b30;                   // +0x30
    unsigned int itemKeys[0x14];         // +0x31
};                                       // sizeof 0x81
#pragma pack(pop)

int Dispatcher_AuctionSearchByItemKey::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned char channel;               // local_2d -0x29
    int channelIdx;                      // local_1c -0x18
    bool fail;                           // bVar1

    // +0x0a：读拍卖频道
    if (!packet.get_byte(channel))
        return LineFunc(0xc690,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // +0x4c：channel > 1 → 拒绝
    if (1 < channel)
        return LineFunc(0xc693,
                        __PRETTY_FUNCTION__,
                        0, 0);
    channelIdx = (int)(unsigned int)channel;
    // +0x84：服务器未连接 → 错误包（PAY_TYPE=channel）
    if (IsAuctionServerDisconnected(channel))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbd, 0x8c, (PAY_TYPE)channelIdx);
        return 0;
    }
    // +0xc5：state 门禁
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        fail = true;
    else
        fail = false;
    if (fail)
        return LineFunc(0xc6b7,
                        __PRETTY_FUNCTION__,
                        0, 0);

    if (channelIdx == 1)
    {
        // ---- 金币拍卖（GP；ORIG ctor 0x0822ea84，category 0x12）----
        DspAuctionSearchByItemKeyPck pck(0x12);
        pck.uid = user->GetUID();
        pck.characNo = user->getCurCharacNo();
        if (!packet.get_int((unsigned int&)pck.field1a))            // +0x1a
            return LineFunc(0xc6c0,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.b1e))                              // +0x1e
            return LineFunc(0xc6c1,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.b1f))                              // +0x1f
            return LineFunc(0xc6c2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.b20))                              // +0x20
            return LineFunc(0xc6c3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short((short&)pck.s21))                     // +0x21
            return LineFunc(0xc6c4,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x2bf：itemKey 数量 ≤ 0x14
        if (0x14 < pck.b20)
            return LineFunc(0xc6c7,
                            __PRETTY_FUNCTION__,
                            0, 0);
        {
            unsigned int key;                                       // local_34 -0x30
            for (int i = 0; i < (int)(unsigned int)pck.b20; i = i + 1)
            {
                if (!packet.get_int((unsigned int&)key))
                    return LineFunc(0xc6cc,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                pck.itemKeys[i] = key;
            }
        }
        GlobalData::s_cera_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pck);
    }
    else
    {
        // ---- 点券拍卖（GA；ORIG ctor 0x0822e6aa，category 0）----
        DspAuctionSearchByItemKeyPck pck(0);
        pck.uid = user->GetUID();
        pck.characNo = user->getCurCharacNo();
        if (!packet.get_int((unsigned int&)pck.field1a))            // +0x1a
            return LineFunc(0xc6e1,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.b1e))                              // +0x1e
            return LineFunc(0xc6e2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.b1f))                              // +0x1f
            return LineFunc(0xc6e3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.b20))                              // +0x20
            return LineFunc(0xc6e4,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short((short&)pck.s21))                     // +0x21
            return LineFunc(0xc6e5,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (0x14 < pck.b20)
            return LineFunc(0xc6e8,
                            __PRETTY_FUNCTION__,
                            0, 0);
        {
            unsigned int key;                                       // local_38 -0x34
            for (int i = 0; i < (int)(unsigned int)pck.b20; i = i + 1)
            {
                if (!packet.get_int((unsigned int&)key))
                    return LineFunc(0xc6ed,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                pck.itemKeys[i] = key;
            }
        }
        if (!packet.get_short((short&)pck.roiKey0))                 // +0x23
            return LineFunc(0xc6f3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short((short&)pck.roiKey2))                 // +0x25
            return LineFunc(0xc6f5,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short((short&)pck.roiKey4))                 // +0x27
            return LineFunc(0xc6f7,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x699：ROI_Category::_sort（this = 包+0x23）
        ((ROI_Category*)((char*)&pck + 0x23))->_sort();
        if (!packet.get_byte(pck.b2f))                              // +0x2f
            return LineFunc(0xc6fd,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(pck.b30))                              // +0x30
            return LineFunc(0xc6fe,
                            __PRETTY_FUNCTION__,
                            0, 0);
        GlobalData::s_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pck);
    }

    // +0x751：GM 延迟 trace（static gmList[5]，ORIG _ZL6gmList d @0x093701d8）
    for (int i = 0; i <= 4; i = i + 1)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xc70f, 0);
            trace("Trace Auction Delay, %s(%d)",
                  "REQ AuctionSearchByItemKey", user->getCurCharacNo());
            break;
        }
    }
    return 0;
}
int Dispatcher_AuctionSearchByItemKey::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionSearchByItemKey::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Break_Away_Quest_Check::Dispatcher_Break_Away_Quest_Check() {}
Dispatcher_Break_Away_Quest_Check::~Dispatcher_Break_Away_Quest_Check() {}
// ORIG 0x081d390a：msg.m_checkFlag==0（+0xd）→ setBreakAwayDungeonClearCnt(-1)、
// setBreakAwayQuestTime(0)、StreamPool::Acquire("PacketDispatcher_Impl_1.cpp",
// 0x31d9)+CStreamGuard(stream,true) 写 0x144/UID/acc_id 入 DB_Q；
// 否则 getBreakAwayDungeonClearCnt()==0 →
// GlobalData::s_BreakAwaySys->giveBreakAwayDungeonClearReward(user)，
// 非 0 → param.m_resultCode=1。
// 【blocked：giveBreakAwayDungeonClearReward 及其 selectReward/
// Inter_LoadEtc::giveBreakAwayReward 生产链尚未还原（报告齐备，独立批次），
// 已 hub 通知 Main；不写猜测实现。】
int Dispatcher_Break_Away_Quest_Check::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_BREAK_AWAY& req = static_cast<MSG_BREAK_AWAY&>(msg);
    if (req.m_checkFlag == 0)                                   // msg+0x0d byte
    {
        user->setBreakAwayDungeonClearCnt(-1);                  // CUser::setBreakAwayDungeonClearCnt(-1)
        user->setBreakAwayQuestTime(0);                         // CUser::setBreakAwayQuestTime(0)

        Stream* stream = GlobalData::s_stream_pool->Acquire(
            "PacketDispatcher_Impl_1.cpp", 0x31d9);             // StreamPool::Acquire 0x0828fa86
        CStreamGuard guard(stream, true);                       // CStreamGuard C1 0x080c8c26
        **guard << (int)0x144;                                   // operator<<(int) 0x080c8c56
        **guard << (unsigned int)user->GetUID();                 // operator<<(unsigned int) 0x080e2292
        **guard << (unsigned int)user->get_acc_id();             // 同上
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);  // MsgQueueMgr::put 0x08570fde
        // guard 析构（RAII）
        return 0;
    }

    if (user->getBreakAwayDungeonClearCnt() == 0)
    {
        GlobalData::s_BreakAwaySys->giveBreakAwayDungeonClearReward(user);  // ORIG 0x0831d164（NEEDS_DECL）
    }
    else
    {
        static_cast<stBreakAwayParam&>(param).m_resultCode = 1; // param+0x04 = 1
    }
    return 0;
}
// ORIG 0x081d38ba：get_byte 读入 msg.m_checkFlag（+0xd）；失败走
// LineFunc(0x3197) 返回其值。
int Dispatcher_Break_Away_Quest_Check::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BREAK_AWAY& req = static_cast<MSG_BREAK_AWAY&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_checkFlag)))
    {
        return LineFunc(0x3197,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// ORIG 0x081d3a72：m_resultCode==0 → put_header(1,0x16d)+put_byte(1)+
// finalize(true)；否则 put_header(1,0x16d)+put_byte(0)+
// put_short(m_resultCode)+finalize(true)。两分支均 CUser::Send。
void Dispatcher_Break_Away_Quest_Check::send(CUser* user, ParamBase& param)
{
    const stBreakAwayParam& res = static_cast<const stBreakAwayParam&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x16d);
    if (res.m_resultCode == 0)
    {
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_short(res.m_resultCode);
    }
    packet.finalize(true);
    user->Send(packet);
}

Dispatcher_ChangeAnotherSkillTree::Dispatcher_ChangeAnotherSkillTree() {}
Dispatcher_ChangeAnotherSkillTree::~Dispatcher_ChangeAnotherSkillTree() {}
int Dispatcher_ChangeAnotherSkillTree::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81d2094：state 3/6 才允许切换，并校验请求树索引。
    if (user == 0)
    {
        return -1;
    }
    int state = user->get_state();
    if (state != 3 && state != 6)
    {
        return 0x13;
    }
    const MSG_CHANGE_ANOTHER_SKILL_TREE& req =
        static_cast<const MSG_CHANGE_ANOTHER_SKILL_TREE&>(msg);
    int requested = static_cast<signed char>(req.m_0d);
    int current = user->GetCurCharacSkillTreeIndex();
    if (requested != current)
    {
        return -2;
    }
    return current == -1 ? -3 : 0;
}
int Dispatcher_ChangeAnotherSkillTree::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d1fca
    stChangeAnotherSkillTree& p = static_cast<stChangeAnotherSkillTree&>(param);
    p.m_result = check_error(user, msg);
    if (p.m_result >= 1)
    {
        return 0;
    }
    if (p.m_result < 0)
    {
        return LineFunc(0x2ecf,
                        __PRETTY_FUNCTION__,
                        p.m_result, 0);
    }
    MSG_CHANGE_ANOTHER_SKILL_TREE& req = static_cast<MSG_CHANGE_ANOTHER_SKILL_TREE&>(msg);
    unsigned char treeIdx = *(unsigned char*)((char*)&req + 0xd);
    if (treeIdx == 0)
    {
        user->SetCurCharacSkillTreeIndex((char)1);
        p.m_skillTreeIndex = 1;
    }
    else if (treeIdx == 1)
    {
        user->SetCurCharacSkillTreeIndex((char)0);
        p.m_skillTreeIndex = 0;
    }
    return 0;
}
int Dispatcher_ChangeAnotherSkillTree::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CHANGE_ANOTHER_SKILL_TREE& req =
        static_cast<MSG_CHANGE_ANOTHER_SKILL_TREE&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x2ec2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void Dispatcher_ChangeAnotherSkillTree::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d2134：p4==0 → header(1,0x108)+byte 1+byte treeIdx；否则 SendCmdErrorPacket(0x108,p4&0xff)
    stChangeAnotherSkillTree& p = static_cast<stChangeAnotherSkillTree&>(param);
    if (p.m_result == 0)
    {
        PacketGuard pkt;
        pkt.put_header(1, 0x108);
        pkt.put_byte(1);
        pkt.put_byte((int)(char)p.m_skillTreeIndex);
        pkt.finalize(true);
        user->Send(pkt);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x108,
                                 (unsigned char)(p.m_result & 0xff));
    }
}

Dispatcher_CharacSlotExtendEffect::Dispatcher_CharacSlotExtendEffect() {}
Dispatcher_CharacSlotExtendEffect::~Dispatcher_CharacSlotExtendEffect() {}
int Dispatcher_CharacSlotExtendEffect::process(CUser* user, MSG_BASE& msg, ParamBase& paramBase)
{
    (void)msg;
    stCharacSlotExtendEffect& param = *reinterpret_cast<stCharacSlotExtendEffect*>(&paramBase);
    // ORIG 0x081d7fb6：state==2 才处理；slotEffectCount < characSlotLimit 时扩槽并入 DB_Q。
    if (user->get_state() != 2)
    {
        param.m_4 = 7;
        return 0;
    }
    if (user->getSlotEffectCount() < user->getCharacSlotLimit())
    {
        user->setSlotEffectCount(user->getCharacSlotLimit());
        param.m_8 = static_cast<unsigned char>(user->getSlotEffectCount());
        param.m_9 = static_cast<unsigned char>(user->getCharacSlotLimit());
        Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x429a);
        CStreamGuard guard(stream, true);
        guard << 0x28a;
        guard << -1;
        char* buf = reinterpret_cast<char*>(guard->GetInBuffer<SIG_EXTEND_CHARAC_SLOT>());
        *reinterpret_cast<int*>(buf + 0) = static_cast<int>(user->get_acc_id());
        buf[4] = static_cast<char>(user->getSlotEffectCount());
        buf[5] = static_cast<char>(user->getCharacSlotLimit());
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    }
    else
    {
        unsigned char limit = user->getCharacSlotLimit();
        unsigned char count = user->getSlotEffectCount();
        unsigned int accId = user->get_acc_id();
        char* id = NumberToString(accId, 0);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x4291, 0);
        trace("CharacSlotExtendEffect m_id(%s), SlotEffectCount(%u), CharacSlotLimit(%u)",
              id, (unsigned int)(count & 0xff), (unsigned int)(limit & 0xff));
        param.m_4 = 0x12;
    }
    return 0;
}
int Dispatcher_CharacSlotExtendEffect::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_CharacSlotExtendEffect::send(CUser* user, ParamBase& param)
{
    stCharacSlotExtendEffect& out = static_cast<stCharacSlotExtendEffect&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x19f);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_byte((unsigned int)(unsigned char)out.m_9);
        packet.put_byte((unsigned int)(unsigned char)out.m_8);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4);
    }
    packet.finalize(true);
    user->Send(packet);
}


Dispatcher_CheckAssaultMotionHack::Dispatcher_CheckAssaultMotionHack() {}
Dispatcher_CheckAssaultMotionHack::~Dispatcher_CheckAssaultMotionHack() {}
int Dispatcher_CheckAssaultMotionHack::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08221f3c..0x0822226f (0x334 字节)。
    // 双精度常量：_DAT_08bd6320=0.5、_DAT_08bd6328=0.8（已 objdump 核实）。
    static const char* const kPretty =
        "virtual int Dispatcher_CheckAssaultMotionHack::dispatch_sig(CUser*, PacketBuf&)";

    if (user == 0)
        return LineFunc(0xdb73, kPretty, 0, 0);

    if (user->getCurCharacR() == 0)
        return 0;

    unsigned int moveCnt = 0;    // -0x18
    unsigned int limitCnt = 0;   // -0x1c
    if (packet.get_int(moveCnt) != 1)
        return LineFunc(0xdb7e, kPretty, 0, 0);
    if (packet.get_int(limitCnt) != 1)
        return LineFunc(0xdb7f, kPretty, 0, 0);

    // Power War 事件开启且当前村庄 == 7 → power war 场景
    const bool inPowerWar =
        (GlobalData::s_power_manager->IsPowerWarEventOn()
         && user->getCurCharacVill() == 7);

    if (10000 < moveCnt)   // cmp $0x2710 / jbe
    {
        const unsigned int overCnt = moveCnt / 10000;   // -0x10
        const unsigned int overRem = moveCnt % 10000;   // -0xc
        if (inPowerWar)
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x7c, 1, overCnt, 0);
        else
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x7a, 1, overCnt, 0);
        moveCnt = overRem;
    }

    // 职业 == 3，或 ==4 且初生 growtype == 2 → 系数 0.5，否则 0.8（截断转换）
    const int job = user->get_charac_job();
    const bool half =
        (job == 3) || (job == 4 && user->getCurCharFirstGrowType() == 2);
    if (half)
        moveCnt = (unsigned int)((double)(unsigned long long)moveCnt * 0.5);
    else
        moveCnt = (unsigned int)((double)(unsigned long long)moveCnt * 0.8);

    if (limitCnt < moveCnt)
    {
        if (!inPowerWar)
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x79, 1, moveCnt, 0);
        else
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x7b, 1, moveCnt, 0);
    }
    return 0;
}
int Dispatcher_CheckAssaultMotionHack::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CheckAssaultMotionHack::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ConnectP2PAfterAssault::Dispatcher_ConnectP2PAfterAssault() {}
Dispatcher_ConnectP2PAfterAssault::~Dispatcher_ConnectP2PAfterAssault() {}
int Dispatcher_ConnectP2PAfterAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa16f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char count = -1;                                       // -0xd（local_15[4]）
    if (!packet.get_byte(count))
    {
        return LineFunc(0xa173,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (count > 4 || count < 0)
    {
        return LineFunc(0xa177,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char slots[4];                                         // -0x11..-0xe（base -0x11）
    slots[0] = 0; slots[1] = 0; slots[2] = 0; slots[3] = 0;
    char team = -1;                                        // -0x12（local_16）
    for (int i = 0; i < (int)count; i++)
    {
        if (!packet.get_byte(team))
        {
            return LineFunc(0xa17e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (team > 3 || team < 0)
        {
            return LineFunc(0xa182,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(slots[(int)team]))
        {
            return LineFunc(0xa189,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    pvp_assault::CAssaultMgr* mgr = pvp_assault::GetInstanceAssaultMgr();   // 0x082350a0
    if (mgr->OnConnectP2PAfterAssault(user, slots, (int)count) != 1)
    {
        return LineFunc(0xa18f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

int Dispatcher_ConnectP2PAfterAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ConnectP2PAfterAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DeathTowerStageCommand::Dispatcher_DeathTowerStageCommand() {}
Dispatcher_DeathTowerStageCommand::~Dispatcher_DeathTowerStageCommand() {}
int Dispatcher_DeathTowerStageCommand::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned short local_22[4];          // -0x1e：4 个玩家用时（ushort）
    unsigned char local_19;              // -0x15：命令字节
    WongWork::CDeathTower* local_18;     // -0x14
    CParty* local_14;                    // -0x10
    int local_c;                         // -0xc：循环索引
    char cVar3;
    int iVar4;
    bool bVar2;
    unsigned int uVar9;

    iVar4 = user->get_state();
    if (iVar4 != 10)
    {
        return 0;
    }
    cVar3 = packet.get_byte(local_19);
    if ((char)(cVar3 ^ 1) != 0)
    {
        return LineFunc(0xa3f2, __PRETTY_FUNCTION__, 0, 0);
    }
    local_18 = user->getDeathTower();
    if (local_18 != 0)
    {
        cVar3 = (char)local_18->handleStageCommand(user, local_19);
        if (cVar3 != 1)
        {
            return LineFunc(0xa3f9, __PRETTY_FUNCTION__, 0, 0);
        }
        local_14 = (CParty*)user->GetParty();
        if (local_14 != 0 && local_19 == 2)
        {
            WongWork::CDeathTower::CStage* pCVar6 = local_18->getCStage();
            pCVar6->onFinishStage();
            for (local_c = 0; local_c <= 3; ++local_c)   // setle：<=3 继续
            {
                cVar3 = packet.get_short(local_22[local_c]);
                if ((char)(cVar3 ^ 1) != 0)
                {
                    return LineFunc(0xa405, __PRETTY_FUNCTION__, 0, 0);
                }
                cVar3 = (char)local_14->checkValidUser(local_c);
                if (cVar3 != 0)
                {
                    if (local_22[local_c] == 0)
                    {
                        cVar3 = (char)local_14->IsSinglePlay();
                        bVar2 = (cVar3 != 0);
                    }
                    else
                    {
                        bVar2 = false;
                    }
                    if (bVar2)
                    {
                        CUser* uVar5 = local_14->get_user(local_c);
                        CUser* pCVar7 = local_14->get_user(local_c);
                        WongWork::CHackAnalyzer* pCVar8 = pCVar7->getHackAnalyzer();
                        pCVar8->addServerHackCnt(uVar5, (WongWork::ENUM_HACKTYPE)0xd5, 1, 0, 0);
                    }
                    CUser* pCVar7b = local_14->get_user(local_c);
                    WongWork::CDeathTower* iVar4b = pCVar7b->getDeathTower();
                    ((WongWork::CDeathTower::stMapMonsterKillChecker_t*)((char*)iVar4b + 0xb58))->init();
                    WongWork::CDeathTower::CStage* pCVar6b = local_18->getCStage();
                    uVar9 = (unsigned int)pCVar6b->getStageClearTime();
                    unsigned short uVar1 = local_22[local_c];
                    CUser* iVar4c = local_14->get_user(local_c);
                    iVar4c->m_historyLog.MapClear(uVar1, (int)(uVar9 / 1000), 0);
                }
            }
        }
    }
    return 0;
}
int Dispatcher_DeathTowerStageCommand::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DeathTowerStageCommand::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Delete_GuildBoard_Text::Dispatcher_Delete_GuildBoard_Text() {}
Dispatcher_Delete_GuildBoard_Text::~Dispatcher_Delete_GuildBoard_Text() {}
// ORIG 0x081d33a0（报告 Dispatcher_Delete_GuildBoard_Text/process.md）：
// param+0x89(unsigned)=待删公告文本索引；无状态门槛。
int Dispatcher_Delete_GuildBoard_Text::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    unsigned int guildkey = (unsigned int)user->get_charac_guildkey();
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    unsigned int textIdx = *(unsigned int*)((char*)&param + 0x89);
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendGuildBoardDelete(textIdx, characNo, guildkey);
    return 0;
}
int Dispatcher_Delete_GuildBoard_Text::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0x89)，失败记 LineFunc(0x3101)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0x89)))
    {
        return LineFunc(0x3101, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_DungeonEventStoryPause::Dispatcher_DungeonEventStoryPause() {}
Dispatcher_DungeonEventStoryPause::~Dispatcher_DungeonEventStoryPause() {}
int Dispatcher_DungeonEventStoryPause::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08216dcc: state==5 门
    if (user->get_state() != 5)
        return 0;

    // +0x034: 读 flag1（paused 字段）/ flag2（resume 全体标志）
    char flag1 = 0;
    if (!packet.get_byte(flag1))
    {
        return LineFunc(0xc886, __PRETTY_FUNCTION__, 0, 0);
    }
    char flag2 = 0;
    if (!packet.get_byte(flag2))
    {
        return LineFunc(0xc88b, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x0b1: 角色非空才继续
    if (user->getCurCharacR() != 0)
    {
        CParty* party = (CParty*)user->GetParty();
        UserQuest* quest = user->getCurCharacQuestR();
        if (quest == 0)
            return 0;
        if (party == 0)
            return 0;

        // +0x103: 判定是否广播暂停
        char pauseValid = 0;
        if (flag2 == 0)
        {
            // party+0xb24 内嵌 CBattle_Field：getMaze / GetCurrentMapIndex
            CBattle_Field* battleField =
                reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);
            if (battleField->getMaze() != 0)
            {
                int mapIndex = battleField->GetCurrentMapIndex();
                int dungeonNo = user->getPlayingDungeonIndex();
                if (quest->CheckDungeonEventStoryPause(dungeonNo, flag2, mapIndex))
                {
                    pauseValid = 1;
                }
            }
        }
        else if (flag2 == 1)
        {
            pauseValid = 1;
        }

        // +0x186: 广播 0xaa 通知
        if (pauseValid != 0)
        {
            PacketGuard pg;
            pg.put_header(0, 0xaa);
            pg.put_short(static_cast<int>(static_cast<unsigned short>(user->get_unique_id())));
            pg.put_byte(static_cast<int>(flag1));
            pg.put_byte(static_cast<int>(flag2));
            pg.finalize(true);
            party->send_to_party(pg);
        }
    }
    return 0;
}
int Dispatcher_DungeonEventStoryPause::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DungeonEventStoryPause::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Float_RDATA_Modulation::Dispatcher_Float_RDATA_Modulation() {}
Dispatcher_Float_RDATA_Modulation::~Dispatcher_Float_RDATA_Modulation() {}
// ORIG 0x08264b5c（报告 Dispatcher_Float_RDATA_Modulation/check_error.md）
// [随行依赖]：ORIG 逐指令即合法恒 0，忠实保留。
int Dispatcher_Float_RDATA_Modulation::check_error(CUser* user, MSG_BASE& msg)
{
    (void)user;
    (void)msg;
    return 0;                            // 0x08264b5f mov $0x0,%eax
}

// ORIG 0x08264acc（报告 Dispatcher_Float_RDATA_Modulation/process.md）：
// 解码两 dword 异或常量后记 hack 计数（0x349）。
int Dispatcher_Float_RDATA_Modulation::process(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    (void)param;
    if (check_error(user, msg) != 0)
    {
        return 0;
    }
    MSG_FLOAT_RDATA_MODULATION& m = static_cast<MSG_FLOAT_RDATA_MODULATION&>(msg);
    unsigned int d1 = m.m_encoded1 ^ 0x59ab991f;
    unsigned int d2 = m.m_encoded2 ^ 0x1f93712b;
    WongWork::CHackAnalyzer* ha = user->getHackAnalyzer();
    ha->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x349, 1, d1, d2);
    return 0;
}
int Dispatcher_Float_RDATA_Modulation::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned int(msg+0x10)，失败记 LineFunc(0x582)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0x10)))
    {
        return LineFunc(0x582, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x T：读 unsigned int(msg+0x14)，失败记 LineFunc(0x586)
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(p + 0x14)))
    {
        return LineFunc(0x586, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

Dispatcher_LogIngameAdvertisement::Dispatcher_LogIngameAdvertisement() {}
Dispatcher_LogIngameAdvertisement::~Dispatcher_LogIngameAdvertisement() {}
int Dispatcher_LogIngameAdvertisement::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    return user->get_state() < 2 ? 0x7fffffff : 0;
}
int Dispatcher_LogIngameAdvertisement::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e4df2：check_error 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x5d83)；err==0 → 广告管理器写日志。
    int err = check_error(user, msg);
    *(int*)(reinterpret_cast<char*>(&param) + 0x4) = err;

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LineFunc(0x5d83,
                 __PRETTY_FUNCTION__,
                 err, 0);
        return 0;
    }

    // ORIG 字段读取顺序：+0x11 uint → +0x10 char(movsbl) → +0xe short(movswl)
    // → +0xd char(movsbl)。
    unsigned int extra =
        *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&msg) + 0x11);
    signed char flag =
        (signed char)(*reinterpret_cast<unsigned char*>(
            reinterpret_cast<char*>(&msg) + 0x10));
    short val =
        *reinterpret_cast<short*>(reinterpret_cast<char*>(&msg) + 0xe);
    signed char type =
        (signed char)(*reinterpret_cast<unsigned char*>(
            reinterpret_cast<char*>(&msg) + 0xd));

    CInGameAdvertisementManager* mgr = InGameADManagerInstance();
    mgr->WriteLog(user, (char)type, val, (char)flag, extra);
    return 0;
}
int Dispatcher_LogIngameAdvertisement::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e4cd0：char@0xd → short@0xe → char@0x10 → uint@0x11
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(m + 0xd)))
        return LineFunc(0x5d6f, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(*reinterpret_cast<short*>(m + 0xe)))
        return LineFunc(0x5d71, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(m + 0x10)))
        return LineFunc(0x5d73, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(m + 0x11)))
        return LineFunc(0x5d75, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_LogIngameAdvertisement::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e4f04 T：err=*(int*)((char*)&param+4)；0→SendCmdOkPacket(0x1ea)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1ea);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1ea, (unsigned char)err);
}

Dispatcher_QueryCharacInfoMailbox::Dispatcher_QueryCharacInfoMailbox() {}
Dispatcher_QueryCharacInfoMailbox::~Dispatcher_QueryCharacInfoMailbox() {}
int Dispatcher_QueryCharacInfoMailbox::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        return LineFunc(0xe289, __PRETTY_FUNCTION__, 0, 0);
    int nameLen = 0;                                 // -0xc
    char name[0x1e];                                 // -0x2a（ORIG 清零 0x1e 字节）
    memset(name, 0, sizeof(name));
    if (!packet.get_int(nameLen))
        return LineFunc(0xe28e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(name, 0x1e, nameLen))
        return LineFunc(0xe28f, __PRETTY_FUNCTION__, 0, 0);
    // ORIG：trim(name) 后与当前角色名 strncmp(name, cur, 0x1d)；相同则错误 7。
    // ORIG：trim(char*) 为 void 就地修剪（_Z4trimPc），随后 strncmp(name, cur, 0x1d)；相同则错误 7。
    trim(name);
    if (strncmp(name, user->getCurCharacName(), 0x1d) == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x145, 7);
        return 0;
    }
    DB_ReqQueryCharacInfoMailbox::makeRequest((int)user->GetUID(), name);
}

int Dispatcher_QueryCharacInfoMailbox::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_QueryCharacInfoMailbox::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestCharacSkillInfo::Dispatcher_RequestCharacSkillInfo() {}
Dispatcher_RequestCharacSkillInfo::~Dispatcher_RequestCharacSkillInfo() {}
// ORIG 0x081e3c08 T _ZN33Dispatcher_RequestCharacSkillInfo11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/Dispatcher_RequestCharacSkillInfo/check_error.md 汇编+反编译 C 全文转写）
// user+0x796e8 = m_characList（vector<_Charac_info>）；槽 char 截断按 movsbl；
// _Charac_info+0x27 = m_level（short）；G_CDataManager()+0x629c = m_characLink.m_field0。
int Dispatcher_RequestCharacSkillInfo::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    int slot = static_cast<signed char>(*(unsigned char*)((char*)&msg + 0xd));
    if (slot < (int)user->m_characList.size())
    {
        if (user->m_characList[slot].m_characNo == 0)
        {
            return 0x15;
        }
        short level = user->m_characList[slot].m_level;
        if (level < G_CDataManager()->m_characLink.m_field0)
        {
            return 0xe;
        }
        return 0;
    }
    return 0x39;
}
int Dispatcher_RequestCharacSkillInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    *(int*)((char*)&param + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x5b32,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    int characIdx = (int)(char)*(char*)((char*)&msg + 0xd);
    _Charac_info& info = user->m_characList[characIdx];
    SkillSlot* skillSlot = reinterpret_cast<SkillSlot*>(reinterpret_cast<char*>(&info) + 0x892);
    if (!skillSlot->IsLoadStrikerSkill())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x5b40),
                           true);
        guard << 0x2fd;
        guard << (int)user->GetUID();
        SIG_REQUEST_CHARAC_SKILL_INFO* req = guard.GetInBuffer<SIG_REQUEST_CHARAC_SKILL_INFO>();
        *(int*)req = info.m_characNo;
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
    else
    {
        int skillTreeKind = (int)(char)info.m_field1239;
        skillSlot->SendSkillInfo(characIdx, (ENUM_SKILL_TREE_KIND)skillTreeKind);
    }
    return 0;
}
int Dispatcher_RequestCharacSkillInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e3914：char@0xd；越界（<0 或 >0x23）logFormat(1,...,0x5b20,...) 后 LineFunc(0x5b22)
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(m + 0xd)))
        return LineFunc(0x5b1b, __PRETTY_FUNCTION__, 0, 0);
    char characIdx = *reinterpret_cast<char*>(m + 0xd);
    if (characIdx < 0 || characIdx > 0x23)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__, 0x5b20,
                              "RequestCharacSkillInfo ReadPacket ERROR! : characIdx(%d)",
                              (int)characIdx);
        return LineFunc(0x5b22, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_RequestCharacSkillInfo::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stRequestCharacSkillInfo& p = static_cast<stRequestCharacSkillInfo&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e5, (unsigned char)p.m_4);
}

Dispatcher_SecurityCardAuthCancel::Dispatcher_SecurityCardAuthCancel() {}
Dispatcher_SecurityCardAuthCancel::~Dispatcher_SecurityCardAuthCancel() {}
int Dispatcher_SecurityCardAuthCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820a42c
    // ORIG cmp $0x1,get_state; setle → state<=1 走断言
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa62d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    if (user->getSecurityCard()->isActivate())
    {
        if (user->getSecurityCard()->getRequestState() != 1)
        {
            return 0;
        }
        if (process(user) == 0)   // ORIG call process(CUser*) @0x0820a580
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xaf, 0x7d);
        }
        return 0;
    }

    if (!user->getPad()->isActivate())
    {
        return LineFunc(0xa650,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->getPad()->getRequestState() != 1)
    {
        return 0;
    }
    if (process(user) == 0)       // ORIG call process(CUser*) @0x0820a580
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xaf, 0x8b);
    }
    return 0;
}
int Dispatcher_SecurityCardAuthCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x0820a580 W：push ebp; mov esp,ebp; mov $0x0,%eax; pop ebp; ret —— 空体。
    // 实际业务逻辑在 static process(CUser*)（DispLinkA3.cpp，ORIG 同址重载）。
    (void)user; (void)msg; (void)param;
    return 0;
}
int Dispatcher_SecurityCardAuthCancel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SecurityCardRetransfer::Dispatcher_SecurityCardRetransfer() {}
Dispatcher_SecurityCardRetransfer::~Dispatcher_SecurityCardRetransfer() {}
int Dispatcher_SecurityCardRetransfer::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3)
    {
        return LineFunc(0xa6ac,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned int type = 0;              // -0x1c
    char phoneBuf[0xc] = {0};           // -0x28 起清零（-0x28/-0x24/-0x20 三个 dword）
    if (!packet.get_int(type))
    {
        return LineFunc(0xa6b1,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(phoneBuf, 0xc, (int)type))
    {
        return LineFunc(0xa6b2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!GlobalData::s_securityCardCenter->checkPhoneNumber(phoneBuf))
    {
        return LineFunc(0xa6b5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char phoneHex[0x29];                // -0x9f
    memset(phoneHex, 0, 0x29);
    CSHA1 sha1;
    sha1.Update((unsigned char const*)phoneBuf, (unsigned int)type);
    sha1.Final();
    sha1.GetHexHash(phoneHex);

    if (user->getSecurityCard()->isActivate())
    {
        if ((unsigned int)user->getSecurityCard()->getFailCnt() > 2)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x7a);
            return 0;
        }
    }
    if (user->getPad()->isActivate())
    {
        if (user->getPad()->getFailCnt() > 2)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x88);
            return 0;
        }
        if (!user->getPad()->isCertified())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4, 0x86);
            return 0;
        }
    }

    int certKeyLen = 0;                 // -0x2e（int + word 清零）
    short certKeyPad = 0;               // -0x2a
    char serverKey[0x21];               // -0x4f
    memset(serverKey, 0, 0x21);
    char cardData[0x27];                // -0x76
    memset(cardData, 0, 0x27);
    char* pCardData = GlobalData::s_securityCardCenter->generateCardData(cardData);
    char* pServerKey = GlobalData::s_securityCardCenter->generateServerKey(serverKey);
    char* pCertKey = GlobalData::s_securityCardCenter->generateCertKey((char*)&certKeyLen);
    int accId = user->get_acc_id();
    int uid = user->GetUID();
    DB_SecurityCardRetransfer::makeRequest(
        uid, phoneBuf, phoneHex, (unsigned int)accId,
        pCertKey, pServerKey, pCardData);
    return 0;
}
int Dispatcher_SecurityCardRetransfer::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardRetransfer::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TournamentRewardSelect::Dispatcher_TournamentRewardSelect() {}
Dispatcher_TournamentRewardSelect::~Dispatcher_TournamentRewardSelect() {}

Dispatcher_UpgradeDisjointMachine::Dispatcher_UpgradeDisjointMachine() {}
Dispatcher_UpgradeDisjointMachine::~Dispatcher_UpgradeDisjointMachine() {}
// ORIG 0x081cff34（nm 实测，sz 0x5f）：state>2 且 curCharac!=0；ExpertJobType!=3 → -2。
int Dispatcher_UpgradeDisjointMachine::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        if (user->GetCurCharacExpertJobType() != 3)
        {
            return -2;
        }
        return 0;
    }
    return -1;
}

// ORIG 0x081cff94（报告 Dispatcher_UpgradeDisjointMachine/process.md）。
int Dispatcher_UpgradeDisjointMachine::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUpgradeDisjointMachineParam& out = static_cast<stUpgradeDisjointMachineParam&>(param);
    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2a89,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    int result = expert_job::CDisjointer::OnUpgradeDisjointMachine(user);
    if (result != 0)
    {
        out.m_4 = result;
    }
    return 0;
}
int Dispatcher_UpgradeDisjointMachine::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_UpgradeDisjointMachine::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：读响应 +0x04 错误码字节，非 0 则 SendCmdErrorPacket。
    stUpgradeDisjointMachineParam& p = static_cast<stUpgradeDisjointMachineParam&>(param);
    if (p.m_4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xf4, (unsigned char)p.m_4);
}

Dispatcher_VerifyPrivateStoreItem::Dispatcher_VerifyPrivateStoreItem() {}
Dispatcher_VerifyPrivateStoreItem::~Dispatcher_VerifyPrivateStoreItem() {}
int Dispatcher_VerifyPrivateStoreItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xe740,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x3d);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x55, (unsigned char)(secuErr & 0xff));
        return 0;
    }
    if (user->isHackUser() || user->isHackUserTimer())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x195, 0x11);
        return 0;
    }
    short slot = 0;
    int itemIdx = 0;
    private_store::IBuyer* buyer = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(0xe76a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(itemIdx))
    {
        return LineFunc(0xe76b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (itemIdx < 0)
    {
        return LineFunc(0xe76c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    private_store::CPrivateStoreMgr* mgr = private_store::GetInstancePrivateStoreMgr();
    mgr->GetStoreBuyer(&buyer, user, (CUser*)0);
    buyer->VerifyPrivateStoreItem((int)slot, itemIdx);
    return 0;
}
int Dispatcher_VerifyPrivateStoreItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_VerifyPrivateStoreItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionMyAuctionHistory::Dispatcher_AuctionMyAuctionHistory() {}
Dispatcher_AuctionMyAuctionHistory::~Dispatcher_AuctionMyAuctionHistory() {}
// ORIG 0x08216d02（报告 Dispatcher_AuctionMyAuctionHistory/dispatch_sig.md）：
// IsRunning!=1 → 0；state<3 || curCharacR==0 → LineFunc(0xc864)；否则
// PCK 包 +0x0e/+0x12/+0x16 三 dword（GetUID/getCurCharacNo/get_acc_id）→ SendPacket。
int Dispatcher_AuctionMyAuctionHistory::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (GlobalData::s_auction_proxy->IsRunning() != 1)
    {
        return 0;
    }
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xc864,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PCK_AUCTION_MY_AUCTION_HISTORY_GA ga;
    // ORIG ctor 仅填包头+memset 0x1e；三个 dword 无具名字段，按包体偏移写。
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&ga) + 0x0e) = user->GetUID();
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&ga) + 0x12) =
        static_cast<unsigned int>(user->getCurCharacNo());
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&ga) + 0x16) = user->get_acc_id();
    GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&ga));
    return 0;
}
int Dispatcher_AuctionMyAuctionHistory::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionMyAuctionHistory::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Card_Select_Right_State::Dispatcher_Card_Select_Right_State() {}
Dispatcher_Card_Select_Right_State::~Dispatcher_Card_Select_Right_State() {}
int Dispatcher_Card_Select_Right_State::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ff504..0x081ff7dd (0x2d9 字节)。packet 未读取（ORIG 不消费包）。
    static const char* const kPretty =
        "virtual int Dispatcher_Card_Select_Right_State::dispatch_sig(CUser*, PacketBuf&)";

    if (user->get_state() != 5)
        return 0;
    if (user->getMoveSpace() == 1)
        return 0;

    const short partyIdx = user->GetPartyIndex();
    CParty* party = G_CGameManager()->GetParty((int)partyIdx);
    if (party == 0)
        return LineFunc(0x9328, kPretty, 0, 0);

    if (party->m_dungeon != 0 && party->m_dungeon->isTowerOfDespairDungeon())
        return 0;

    if (!party->CheckRecvFromAllMember(user))
        return 0;

    if (party->get_dungeon_clear_state() > 3)   // cmp $0x3 / setg
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x197, 1, 0, 0);
        return 0;
    }

    PacketGuard guard;
    guard.put_header(1, 0x49);
    guard.put_byte(1);
    for (int i = 0; i <= 3; i++)   // cmpl $0x3 / setle
    {
        CUser* member = party->get_user(i);
        if (member == 0)
        {
            guard.put_short(-1);
        }
        else
        {
            const unsigned short fatigue = member->getCurCharacTotalFatigue();
            const unsigned short maxFatigue = member->getCurCharacTotalMaxFatigue();
            if (fatigue == maxFatigue)
                guard.put_short(0);
            else
                guard.put_short(1);
        }
    }
    guard.finalize(true);
    party->send_to_party(guard);
    party->set_dungeon_clear_state(4);
    party->insert_time_card_select_right_msg(0x1e);
    party->insert_time_card_select_msg(4);
    return 0;
}
int Dispatcher_Card_Select_Right_State::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Card_Select_Right_State::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CheckGuildAddressDouble::Dispatcher_CheckGuildAddressDouble() {}
Dispatcher_CheckGuildAddressDouble::~Dispatcher_CheckGuildAddressDouble() {}
int Dispatcher_CheckGuildAddressDouble::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xa3af,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int len = 0;                                           // -0x14
    if (!packet.get_int(len))
    {
        return LineFunc(0xa3be,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char* addr = user->getCreateGuildAddr();               // -0x10
    if (addr == 0)
    {
        return LineFunc(0xa3c2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(addr, 0xd, len))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa0, 0x6e);
        return 0;
    }
    if (!check_url(addr))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa0, 0x70);
        return 0;
    }
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "PacketDispatcher_Impl_1.cpp", 0xa3d1);            // 0x0828fa86
    CStreamGuard guard(stream, true);
    **guard << (int)0xec;
    **guard << (int)user->GetUID();
    SIG_DOUBLE_CHECK_GUILD_ADDRESS* sig =
        guard->GetInBuffer<SIG_DOUBLE_CHECK_GUILD_ADDRESS>();
    memset(sig, 0, 0xd);
    memcpy(sig, addr, len);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;                                              // guard 析构 0x0861c8d2
}

int Dispatcher_CheckGuildAddressDouble::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CheckGuildAddressDouble::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompoundItemByExpertJob::Dispatcher_CompoundItemByExpertJob() {}
Dispatcher_CompoundItemByExpertJob::~Dispatcher_CompoundItemByExpertJob() {}
int Dispatcher_CompoundItemByExpertJob::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821eb66: 角色非空门
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xd73c, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x04d: 有专家职业门
    if (user->GetCurCharacExpertJobType() == 0)
    {
        return LineFunc(0xd73f, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x0ab: state==3 且交易中直接放弃
    if (user->get_state() != 3)
        return 0;
    if (user->CheckInTrade())
        return 0;

    // +0x0dd: isRestricted(user,1,0x13) → error 0xf2,0xd1
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, static_cast<RestrictCategory::Enum>(1), 0x13))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf2), 0xd1);
        return 0;
    }

    // +0x122: Secu Check(0x1c) → error 0xf2,ret
    int secuCheck = GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x1c));
    if (secuCheck != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf2),
                                 static_cast<unsigned char>(secuCheck));
        return 0;
    }

    // +0x167: 读 ulong itemIdx / ushort count
    unsigned long itemIdx = 0;
    if (!packet.get_int(itemIdx))
    {
        return LineFunc(0xd762, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned short count = 0;
    if (!packet.get_short(count))
    {
        return LineFunc(0xd764, __PRETTY_FUNCTION__, 0, 0);
    }
    if (count == 0)
    {
        return LineFunc(0xd76a, __PRETTY_FUNCTION__, 0, 0);
    }

    // +0x241: compound_item(itemIdx, true, count, 0xf2)
    int compoundResult = user->compound_item(static_cast<int>(itemIdx), true, count, 0xf2);
    if (compoundResult < 1)
    {
        // +0x2f3: 失败且专家职业 2/4 → 清对应行动组
        if (user != 0 && compoundResult == 0)
        {
            int expertJobType = user->GetCurCharacExpertJobType();
            if (expertJobType == 2)
            {
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    user, static_cast<APSystem::_ActionGroupIndex>(0x1d), 0, static_cast<ENUM_PACKETCLASS>(0));
            }
            else if (expertJobType == 4)
            {
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    user, static_cast<APSystem::_ActionGroupIndex>(0x1e), 0, static_cast<ENUM_PACKETCLASS>(0));
            }
        }
        return 0;
    }

    // +0x259: 成功 → 0xf2 响应包（byte 0, byte result）
    PacketGuard pg;
    pg.put_header(1, 0xf2);
    pg.put_byte(0);
    pg.put_byte(compoundResult);
    pg.finalize(true);
    user->Send(pg);
    return 0;
}
int Dispatcher_CompoundItemByExpertJob::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompoundItemByExpertJob::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_InvestItemAmplifyOption::Dispatcher_InvestItemAmplifyOption() {}
Dispatcher_InvestItemAmplifyOption::~Dispatcher_InvestItemAmplifyOption() {}


Dispatcher_RequestItemUnlockCancel::Dispatcher_RequestItemUnlockCancel() {}
Dispatcher_RequestItemUnlockCancel::~Dispatcher_RequestItemUnlockCancel() {}
int Dispatcher_RequestItemUnlockCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3)
        return 0;
    char slot = 0;                                   // -0x11
    short itemIdx = 0;                               // -0x14
    if (!packet.get_byte(slot))
        return LineFunc(0xdd7b, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(itemIdx))
        return LineFunc(0xdd7c, __PRETTY_FUNCTION__, 0, 0);
    // SECURITY_PROTCTION 0x19（ItemUnlockCancel 门禁）
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x19);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x111, (unsigned char)secu);
        return 0;
    }
    int able = user->IsAbleItemUnlockCancel(slot, itemIdx);
    if (able != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x111, (unsigned char)able);
        return 0;
    }
    user->DoItemUnlockCancel(slot, itemIdx);
    return 0;
}

int Dispatcher_RequestItemUnlockCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestItemUnlockCancel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestPCRoomPlayerList::Dispatcher_RequestPCRoomPlayerList() {}
Dispatcher_RequestPCRoomPlayerList::~Dispatcher_RequestPCRoomPlayerList() {}
int Dispatcher_RequestPCRoomPlayerList::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        return 0;
    }
    const unsigned int accountId = user != 0 ? user->get_acc_id() : 0;
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xd14e, 5);
    trace("PCROOM : Dispatcher_RequestPCRoomPlayerList => User State : %d, m_id : %s",
          user != 0 ? user->get_state() : 0, NumberToString(accountId, 0));
    return LineFunc(0xd14f,
                    __PRETTY_FUNCTION__,
                    0, 0);
}
int Dispatcher_RequestPCRoomPlayerList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestPCRoomPlayerList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Write_On_The_GuildBoard::Dispatcher_Write_On_The_GuildBoard() {}
Dispatcher_Write_On_The_GuildBoard::~Dispatcher_Write_On_The_GuildBoard() {}
int Dispatcher_Write_On_The_GuildBoard::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d31ee
    // msg+0xd：公会板标题文本区首字节（非 0 才有效）
    if (*(unsigned char*)((char*)&msg + 0xd) == 0)
    {
        *(int*)((char*)&param + 4) = 1;
        return 0;
    }

    // hasPreventString || hasSpecialCharacterExceptSpace → invalid（ORIG 两链 jne/je 合流）
    bool invalid;
    if (!G_CDataManager()->hasPreventString((const char*)((char*)&msg + 0xd)) &&
        !G_CDataManager()->hasSpecialCharacterExceptSpace((const char*)((char*)&msg + 0xd)))
    {
        invalid = false;
    }
    else
    {
        invalid = true;
    }
    if (invalid)
    {
        *(int*)((char*)&param + 4) = 1;
        return 0;
    }

    // 实参求值顺序按 ORIG：guildkey → growType(movsbl) → job → characNo → accId → GetServerGroup
    unsigned int guildkey = (unsigned int)user->get_charac_guildkey();
    char growType = (char)user->getCurCharacGrowType();
    int job = user->get_charac_job();
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    unsigned int accId = user->get_acc_id();
    ENUM_SERVER_GROUP group = (ENUM_SERVER_GROUP)user->GetServerGroup();
    CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(group);
    proxy->SendGuildBoardWrite((char*)((char*)&msg + 0xd), accId, characNo, job, growType,
                               guildkey);
    return 0;
}
int Dispatcher_Write_On_The_GuildBoard::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d314a：局部 int 清零读入（len）→ str@0xd（max 0x78）
    char* m = reinterpret_cast<char*>(&msg);
    int len = 0;
    if (!packet.get_int(len))
        return LineFunc(0x30c5, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m + 0xd, 0x78, len))
        return LineFunc(0x30c8, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_Write_On_The_GuildBoard::send(CUser* user, ParamBase& param)
{
    // ORIG 0x38：无条件读响应 +0x04 错误码字节并 SendCmdErrorPacket（无判 0 分支）。
    stGuildBoardParam& p = static_cast<stGuildBoardParam&>(param);
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x156, (unsigned char)p.m_4);
}

Dispatcher_AuctionSearchByNoItmeKey::Dispatcher_AuctionSearchByNoItmeKey() {}
Dispatcher_AuctionSearchByNoItmeKey::~Dispatcher_AuctionSearchByNoItmeKey() {}
int Dispatcher_AuctionSearchByNoItmeKey::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082161e4..0x08216965 (0x782 字节)。GM 表 _ZL6gmList @0x093abb4c。
    static const char* const kPretty =
        "virtual int Dispatcher_AuctionSearchByNoItmeKey::dispatch_sig(CUser*, PacketBuf&)";

    unsigned char payType = 0;
    if (packet.get_byte(payType) != 1)
        return LineFunc(0xc719, kPretty, 0, 0);
    if (payType > 1)   // cmp $0x1 / jbe
        return LineFunc(0xc71c, kPretty, 0, 0);

    const int pay = payType;   // movzbl 零扩展到 int
    if (IsAuctionServerDisconnected(payType))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbd, 0x8c, (PAY_TYPE)pay);
        return 0;
    }

    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return LineFunc(0xc73f, kPretty, 0, 0);

    // 包布局（ServerLab/Auction/AuctionServer/auction_header/AuctionPacket.h）：
    //   char_idx@0x12, character_id@0x16,
    //   TSearchByCategory@0x1a: startIndex@0x1a, category@0x1e, rarity@0x20,
    //   upgradeStart@0x21, upgradeEnd@0x22, lvStart@0x23, lvEnd@0x24,
    //   ROI_Search_category_key@0x25(12B，asm 仅填前 3 short), refineStart@0x31, refineEnd@0x32。
    if (pay == 1)
    {
        // PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP ctor ORIG W 0x0822eafa：
        // PACKET_HEADER C1 + TSearchByCategory_ C1@0x1a + memset(this,0,0x33)
        // + setCategory(0x12) + setPacketID(7) + setSize(0x33)。
        PCK_AUCTION_SEARCH_BY_NOITEMKEY_GP pck;
        char* const body = reinterpret_cast<char*>(&pck);
        *reinterpret_cast<unsigned int*>(body + 0x12) = user->GetUID();
        *reinterpret_cast<int*>(body + 0x16) = user->getCurCharacNo();

        unsigned int startIndex = 0;
        if (packet.get_int(startIndex) != 1)
            return LineFunc(0xc748, kPretty, 0, 0);
        *reinterpret_cast<unsigned int*>(body + 0x1a) = startIndex;

        unsigned short category = 0;
        if (packet.get_short(category) != 1)
            return LineFunc(0xc749, kPretty, 0, 0);
        *reinterpret_cast<unsigned short*>(body + 0x1e) = category;

        unsigned char rarity = 0;
        if (packet.get_byte(rarity) != 1)
            return LineFunc(0xc74a, kPretty, 0, 0);
        body[0x20] = static_cast<char>(rarity);

        unsigned char upgradeStart = 0;
        if (packet.get_byte(upgradeStart) != 1)
            return LineFunc(0xc74b, kPretty, 0, 0);
        body[0x21] = static_cast<char>(upgradeStart);

        unsigned char upgradeEnd = 0;
        if (packet.get_byte(upgradeEnd) != 1)
            return LineFunc(0xc74c, kPretty, 0, 0);
        body[0x22] = static_cast<char>(upgradeEnd);

        unsigned char lvStart = 0;
        if (packet.get_byte(lvStart) != 1)
            return LineFunc(0xc74d, kPretty, 0, 0);
        body[0x23] = static_cast<char>(lvStart);

        unsigned char lvEnd = 0;
        if (packet.get_byte(lvEnd) != 1)
            return LineFunc(0xc74e, kPretty, 0, 0);
        body[0x24] = static_cast<char>(lvEnd);

        GlobalData::s_cera_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }
    else
    {
        // PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA ctor ORIG W 0x0822e736：
        // PACKET_HEADER C1 + TSearchByCategory_ C1@0x1a + memset(this,0,0x33)
        // + setCategory(0) + setPacketID(7) + setSize(0x33)。
        PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA pck;
        char* const body = reinterpret_cast<char*>(&pck);
        *reinterpret_cast<unsigned int*>(body + 0x12) = user->GetUID();
        *reinterpret_cast<int*>(body + 0x16) = user->getCurCharacNo();

        unsigned int startIndex = 0;
        if (packet.get_int(startIndex) != 1)
            return LineFunc(0xc760, kPretty, 0, 0);
        *reinterpret_cast<unsigned int*>(body + 0x1a) = startIndex;

        unsigned short category = 0;
        if (packet.get_short(category) != 1)
            return LineFunc(0xc761, kPretty, 0, 0);
        *reinterpret_cast<unsigned short*>(body + 0x1e) = category;

        unsigned char rarity = 0;
        if (packet.get_byte(rarity) != 1)
            return LineFunc(0xc762, kPretty, 0, 0);
        body[0x20] = static_cast<char>(rarity);

        unsigned char upgradeStart = 0;
        if (packet.get_byte(upgradeStart) != 1)
            return LineFunc(0xc763, kPretty, 0, 0);
        body[0x21] = static_cast<char>(upgradeStart);

        unsigned char upgradeEnd = 0;
        if (packet.get_byte(upgradeEnd) != 1)
            return LineFunc(0xc764, kPretty, 0, 0);
        body[0x22] = static_cast<char>(upgradeEnd);

        unsigned char lvStart = 0;
        if (packet.get_byte(lvStart) != 1)
            return LineFunc(0xc765, kPretty, 0, 0);
        body[0x23] = static_cast<char>(lvStart);

        unsigned char lvEnd = 0;
        if (packet.get_byte(lvEnd) != 1)
            return LineFunc(0xc766, kPretty, 0, 0);
        body[0x24] = static_cast<char>(lvEnd);

        short roi0 = 0;
        if (packet.get_short(roi0) != 1)
            return LineFunc(0xc76a, kPretty, 0, 0);
        *reinterpret_cast<short*>(body + 0x25) = roi0;

        short roi1 = 0;
        if (packet.get_short(roi1) != 1)
            return LineFunc(0xc76c, kPretty, 0, 0);
        *reinterpret_cast<short*>(body + 0x27) = roi1;

        short roi2 = 0;
        if (packet.get_short(roi2) != 1)
            return LineFunc(0xc76e, kPretty, 0, 0);
        *reinterpret_cast<short*>(body + 0x29) = roi2;

        unsigned char refineStart = 0;
        if (packet.get_byte(refineStart) != 1)
            return LineFunc(0xc772, kPretty, 0, 0);
        body[0x31] = static_cast<char>(refineStart);

        unsigned char refineEnd = 0;
        if (packet.get_byte(refineEnd) != 1)
            return LineFunc(0xc773, kPretty, 0, 0);
        body[0x32] = static_cast<char>(refineEnd);

        GlobalData::s_auction_proxy->SendPacket(reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    }

    // 尾部 GM 延迟追踪（_ZL6gmList ORIG @0x093abb4c，5 项，数据已 objdump 核实）
    static const int gmList[5] = { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };
    for (int i = 0; i <= 4; i++)
    {
        if (gmList[i] == user->getCurCharacNo())
        {
            cMyTrace trace(kPretty, 0xc784, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionSearchByNoItmeKey",
                  user->getCurCharacNo());
            break;
        }
    }
    return 0;
}
int Dispatcher_AuctionSearchByNoItmeKey::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionSearchByNoItmeKey::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CheckEnableServerMessage::Dispatcher_CheckEnableServerMessage() {}
Dispatcher_CheckEnableServerMessage::~Dispatcher_CheckEnableServerMessage() {}
int Dispatcher_CheckEnableServerMessage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821df6a（PacketDispatcher_Impl_1.cpp 0xd236 区）
    if (user->get_state() != 3)
    {
        char* idStr = NumberToString(user->get_acc_id(), 0);
        int state = user->get_state();
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0xd236, 0);
        trace("ServerMessage : Dispatcher_CheckEnableServerMessage => User State : %d, m_id : %s",
              state, idStr);
        return LineFunc(0xd237,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char msgIdx = 0;
    if (!packet.get_byte(msgIdx))
    {
        return LineFunc(0xd23d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int curSec = GlobalData::s_systemTime_.getCurSec();
    PacketGuard guard;
    guard.put_header(1, 0xde);
    CSharedServerMessageManager* mgr =
        G_CGameManager()->GetSharedServerMessageManager();
    if (mgr != 0)
    {
        int cond = 0;
        if (!mgr->CheckUserCondition(user, (int)msgIdx, cond))
        {
            guard.put_byte(0);
            guard.put_byte(3);
        }
        else if (!mgr->IsChangableMessage((int)msgIdx, curSec))
        {
            guard.put_byte(0);
            guard.put_byte(1);
        }
        else
        {
            guard.put_byte(1);
        }
    }
    guard.put_byte((int)msgIdx);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
int Dispatcher_CheckEnableServerMessage::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CheckEnableServerMessage::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompleteLoadAfterAssault::Dispatcher_CompleteLoadAfterAssault() {}
Dispatcher_CompleteLoadAfterAssault::~Dispatcher_CompleteLoadAfterAssault() {}
int Dispatcher_CompleteLoadAfterAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    // ORIG 0x08206b5c：完整加载后登记 super power 状态定时器并向全队广播确认。
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa151, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->GetCurCharacSuperState() != 0)
    {
        CDataManager* dm = G_CDataManager();
        int sec = dm->m_powerParameter.m_timeAssaultSuperPowerStateAfter;  // CDataManager +0xa688
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0,
                                    (int)user->GetUID(),
                                    (TIMER_MESSAGE)0x7f, sec,
                                    (int)user->GetLoginTick(),
                                    (unsigned int)user->getCurCharacNo());
    }
    CParty* party = (CParty*)user->GetParty();
    if (party != 0 && party->CheckRecvFromAllMember(user))
    {
        PacketGuard guard;
        guard.clear();
        guard.put_header(1, 0xf8);
        guard.put_byte(1);
        guard.finalize(true);
        party->send_to_party(guard);
        party->SetRecvResultFlag(true);
    }
    return 0;
}
int Dispatcher_CompleteLoadAfterAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompleteLoadAfterAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Enchant_3rdChronicleItem::Dispatcher_Enchant_3rdChronicleItem() {}
Dispatcher_Enchant_3rdChronicleItem::~Dispatcher_Enchant_3rdChronicleItem() {}
extern "C" int _ZN8WongWork12CItemUpgrade11ProcUpgradeEP5CUserP11MSG_ENCHANTP11PacketGuard(
    void* self, CUser* user, void* msg, void* packet);

int Dispatcher_Enchant_3rdChronicleItem::check_error(CUser* user, MSG_BASE& msg)
{
    // 仅允许 state==3 且当前角色有效；否则错误码 7
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return 7;
    }

    MSG_ENCHANT& req = static_cast<MSG_ENCHANT&>(msg);

    const int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x27);
    if (secu != 0)
    {
        return secu;
    }

    if (user->CheckItemLock(1, (unsigned short)req.m_0d))
    {
        return 0xd5;
    }

    // msg+0x15：请求强化的物品 idx → 物品脚本必须存在
    CItem* itemScript = G_CDataManager()->find_item(req.m_15);
    if (itemScript == 0)
    {
        return 4;
    }

    CInventory* invenW = user->getCurCharacInvenW();
    if (invenW == 0)
    {
        return 4;
    }

    // ORIG Inven_Item ctor @0x080cb854 后整体拷贝 GetInvenSlot 结果
    Inven_Item slotItem;
    slotItem = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, (unsigned short)req.m_13);

    // 背包槽内物品须与请求脚本一致，否则 0x17
    if (slotItem.m_addInfo != itemScript->get_index())
    {
        return 0x17;
    }
    return 0;
}
int Dispatcher_Enchant_3rdChronicleItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const int err = check_error(user, msg);
    if (err > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x173, (unsigned char)(err & 0xff));
        return -1;
    }

    stEnchantParam& out = static_cast<stEnchantParam&>(param);   // +0x04 结果槽
    MSG_ENCHANT& req = static_cast<MSG_ENCHANT&>(msg);

    PacketGuard pg;                                              // try 域 @081d37e3..081d3846
    pg.put_header(1, 0x173);

    out.m_4 = _ZN8WongWork12CItemUpgrade11ProcUpgradeEP5CUserP11MSG_ENCHANTP11PacketGuard(
        G_CDataManager()->m_itemUpgrade, user, &req, &pg);

    if (out.m_4 == 0)
    {
        pg.finalize(true);
        user->Send(pg);
    }
    return 0;
}
int Dispatcher_Enchant_3rdChronicleItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_ENCHANT& req = static_cast<MSG_ENCHANT&>(msg);

    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&req.m_13)))
    {
        return LineFunc(
            0x3158,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(&req.m_15)))
    {
        return LineFunc(
            0x315a,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&req.m_0d)))
    {
        return LineFunc(
            0x315d,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(&req.m_0f)))
    {
        return LineFunc(
            0x315f,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_byte(req.m_19))
    {
        return LineFunc(
            0x3162,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
}
void Dispatcher_Enchant_3rdChronicleItem::send(CUser* user, ParamBase& param)
{
    stEnchantParam& out = static_cast<stEnchantParam&>(param);
    if (out.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x173,
                                 (unsigned char)(out.m_4 & 0xff));
    }
}

Dispatcher_GoblinPadRequestCryptKey::Dispatcher_GoblinPadRequestCryptKey() {}
Dispatcher_GoblinPadRequestCryptKey::~Dispatcher_GoblinPadRequestCryptKey() {}
int Dispatcher_GoblinPadRequestCryptKey::dispatch_sig(CUser* user, PacketBuf& packet)
{
    Sanicova::CPad* pad = user->getPad();
    pad->GenerateRandomData();
    PacketGuard response;
    response.put_header(1, 0xe2);
    response.put_byte(1);
    response.put_short(pad->GetKeyIdx(0) & 0xffff);
    response.put_short(pad->GetKeyIdx(1) & 0xffff);
    response.put_byte(pad->GetShakeData(0) & 0xff);
    response.put_byte(pad->GetShakeData(1) & 0xff);
    response.put_byte(pad->GetShakeData(2) & 0xff);
    response.put_byte(pad->GetShakeData(3) & 0xff);
    response.finalize(true);
    user->Send(response);
    return 0;
}
int Dispatcher_GoblinPadRequestCryptKey::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GoblinPadRequestCryptKey::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GoldTakeIncreasingAmount::Dispatcher_GoldTakeIncreasingAmount() {}
Dispatcher_GoldTakeIncreasingAmount::~Dispatcher_GoldTakeIncreasingAmount() {}
int Dispatcher_GoldTakeIncreasingAmount::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty = "int Dispatcher_GoldTakeIncreasingAmount::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x3903, pretty, 0, 0);
    }
    return user->get_state() == 5 ? 0 : -1;
}
int Dispatcher_GoldTakeIncreasingAmount::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d55a4
    int err = check_error(user, msg);
    if (err < 1)
    {
        if (err < 0)
        {
            return -1;
        }
        stGoldTakeIncreasingAmount& p = static_cast<stGoldTakeIncreasingAmount&>(param);
        MSG_GOLD_TAKE_INCREASING_AMOUNT& req = static_cast<MSG_GOLD_TAKE_INCREASING_AMOUNT&>(msg);
        p.m_4 = *(int*)((char*)&req + 0xd);
        if (p.m_4 == 0xf68a)
        {
            CInventory* inven = (CInventory*)user->getCurCharacInvenW();
            user_creature::CCreatureMgr* mgr = inven->GetCreatureMgrW();
            int ret = mgr->SetOverSkillTime((const int&)p.m_4);
            if (ret == 0)
            {
                p.m_8 = 1;
            }
            else
            {
                p.m_8 = 0;
            }
            return 0;
        }
        return 0x15;
    }
    return err;
}
int Dispatcher_GoldTakeIncreasingAmount::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 int(msg+0xd)，失败记 LineFunc(0x38d6)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(p + 0xd)))
    {
        return LineFunc(0x38d6, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_GoldTakeIncreasingAmount::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d56ac：header(1,0x174)+byte p8
    stGoldTakeIncreasingAmount& p = static_cast<stGoldTakeIncreasingAmount&>(param);
    PacketGuard pkt;
    pkt.put_header(1, 0x174);
    pkt.put_byte(p.m_8);
    pkt.finalize(true);
    user->Send(pkt);
}

Dispatcher_LevelupSupportReqestItem::Dispatcher_LevelupSupportReqestItem() {}
Dispatcher_LevelupSupportReqestItem::~Dispatcher_LevelupSupportReqestItem() {}
int Dispatcher_LevelupSupportReqestItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0825fc3a（docs/class_func_reports/Dispatcher_LevelupSupportReqestItem/dispatch_sig.md）
    int requestLevel = 0;                     // local_14 -0x10
    ENUM_ERROR err = (ENUM_ERROR)0;           // local_18 -0x14
    int stepIdx;                              // local_10 -0xc

    // +0x00：user 为空 → 0xa49
    if (user == 0)
        return (int)0xa49;
    // +0x18：state < 3 → 0
    if (user->get_state() < 3)
        return 0;

    // +0x37：0x9e 重复事件未进行中 → 错误 1 + 日志（ORIG 无判空直调 vptr+0x34 IsEventing(0)）
    CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0x9e);
    if (!repeatEvent->IsEventing(0))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x255, 1);
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_2.cpp",
            __PRETTY_FUNCTION__,
            0xa50, "charac_no:%d, error:%d",
            user->getCurCharacNo(), 1);
        return 0;
    }

    // +0xcf：读请求等级
    if (!packet.get_int(requestLevel))
        return (int)0xa56;

    // +0xf9：档位判定
    err = (ENUM_ERROR)0;
    {
        LevelupSupportEventManger* mgr = ARAD::Singleton<LevelupSupportEventManger>::Get();
        stepIdx = ARAD::Singleton<LevelupSupportEventManger>::Get()->GetStepIndex(requestLevel);  // 0x08147362
        if (stepIdx == -1)
            err = (ENUM_ERROR)1;
        // +0x124：user->m_levelupEvents[stepIdx].m_state（+0x02）
        LevelupSupportEventManger::EventInfo& info =
            user->m_levelupEvents[stepIdx];
        unsigned short state = info.m_state;
        if (state == 1)
        {
            mgr->InsertItem(user, stepIdx, err);   // 0x081473cc
        }
        else if (state == 2)
        {
            err = (ENUM_ERROR)0x12;
        }
        else if (state == 0)
        {
            err = (ENUM_ERROR)0x13;
        }
    }

    // +0x17c：err==0 → 应答包；否则错误包 + 日志
    if (err == (ENUM_ERROR)0)
    {
        PacketGuard guard;                                            // 0858dd4c
        guard.clear();                                                // 080cb8e6
        guard.put_header(1, 0x255);
        guard.put_byte(1);
        guard.put_int(requestLevel);
        guard.finalize(true);
        user->Send(guard);
        return 0;                                                     // guard 析构 0858de80
    }

    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x255, (unsigned char)err & 0xff);
    LogManager::logFormat(
        1, "PacketDispatcher_Impl_2.cpp",
        __PRETTY_FUNCTION__,
        0xa78, "charac_no:%d, request_level:%d, error:%d",
        user->getCurCharacNo(), requestLevel, (unsigned int)err);
    return 0;
}
int Dispatcher_LevelupSupportReqestItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_LevelupSupportReqestItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_LevelupSupportReqestItem::send(CUser* user, ParamBase& param) {}

Dispatcher_RequestOnTimeEventReward::Dispatcher_RequestOnTimeEventReward() {}
Dispatcher_RequestOnTimeEventReward::~Dispatcher_RequestOnTimeEventReward() {}
int Dispatcher_RequestOnTimeEventReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0822458c（PacketDispatcher_Impl_1.cpp 0xdf01 区）
    if (G_GameWorld()->IsPVPChannel())
    {
        return 0;
    }
    if (user->get_state() != 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0xd8);
        return 0;
    }
    COnTimeEvent* event =
        (COnTimeEvent*)GlobalData::s_event_manager->GetRepeatEvent(0x33);
    if (event == 0)
    {
        return 0;
    }
    if (!event->IsEventing(0))   // 虚槽 +0x34（CEventBase::IsEventing）
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0x13);
        char logBuf[256];
        memset(logBuf, 0, 0x100);
        sprintf(logBuf, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());
        unsigned int characNo = user->get_charac_no(-1);
        char* idStr = NumberToString(user->get_acc_id(), 0);
        CMyFileLog log("virtual int Dispatcher_RequestOnTimeEventReward::dispatch_sig(CUser*, PacketBuf&)",
                       0xdf01);
        log(logBuf,
            "On Time Event:Request Reward IsEventing error m_id(%s) charac_no(%u)",
            idStr, characNo);
        return 0;
    }
    if (event->get_reward_state() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 0x13);
        char logBuf[256];
        memset(logBuf, 0, 0x100);
        sprintf(logBuf, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());
        unsigned int characNo = user->get_charac_no(-1);
        char* idStr = NumberToString(user->get_acc_id(), 0);
        CMyFileLog log("virtual int Dispatcher_RequestOnTimeEventReward::dispatch_sig(CUser*, PacketBuf&)",
                       0xdf14);
        log(logBuf,
            "On Time Event:Request Reward ENUM_ONTIME_EVENT_ON_REWARD_ON error m_id(%s) charac_no(%u)",
            idStr, characNo);
        return 0;
    }
    if (user->get_ontime_last_recv_idx() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x120, 2);
        char logBuf[256];
        memset(logBuf, 0, 0x100);
        sprintf(logBuf, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());
        unsigned int characNo = user->get_charac_no(-1);
        char* idStr = NumberToString(user->get_acc_id(), 0);
        CMyFileLog log("virtual int Dispatcher_RequestOnTimeEventReward::dispatch_sig(CUser*, PacketBuf&)",
                       0xdf20);
        log(logBuf,
            "On Time Event:Request Reward user db error m_id(%s) charac_no(%u)",
            idStr, characNo);
        return 0;
    }
    event->insert_reward_user(user);
    return 0;
}
int Dispatcher_RequestOnTimeEventReward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestOnTimeEventReward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestPCRoomPlayerCount::Dispatcher_RequestPCRoomPlayerCount() {}
Dispatcher_RequestPCRoomPlayerCount::~Dispatcher_RequestPCRoomPlayerCount() {}
int Dispatcher_RequestPCRoomPlayerCount::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        return 0;
    }
    const unsigned int accountId = user != 0 ? user->get_acc_id() : 0;
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xd186, 5);
    trace("PCROOM : Dispatcher_RequestPCRoomPlayerCount => User State : %d, m_id : %s",
          user != 0 ? user->get_state() : 0, NumberToString(accountId, 0));
    return LineFunc(0xd187,
                    __PRETTY_FUNCTION__,
                    0, 0);
}
int Dispatcher_RequestPCRoomPlayerCount::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestPCRoomPlayerCount::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_MemoryIntegrity::Dispatcher_Security_MemoryIntegrity() {}
Dispatcher_Security_MemoryIntegrity::~Dispatcher_Security_MemoryIntegrity() {}
int Dispatcher_Security_MemoryIntegrity::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_MI_RecvMemoryIntegrityModulation(user, packet);
    if (result == 2)
    {
        return LineFunc(0xdf97,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (result == 3)
    {
        return LineFunc(0xdf99,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (result == 1)
    {
        return LineFunc(0xdf95,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_MemoryIntegrity::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_MemoryIntegrity::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_PacketIntegrity::Dispatcher_Security_PacketIntegrity() {}
Dispatcher_Security_PacketIntegrity::~Dispatcher_Security_PacketIntegrity() {}
int Dispatcher_Security_PacketIntegrity::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_PacketIntegrity(user, packet);
    switch (result)
    {
    default:
        return 0;
    case 1:
        return LineFunc(0xded7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 2:
        return LineFunc(0xded9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 3:
        return LineFunc(0xdedb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 4:
        return LineFunc(0xdedd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    case 5:
        return LineFunc(0xdedf,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
}
int Dispatcher_Security_PacketIntegrity::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_PacketIntegrity::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SelectUltimateDifficulty::Dispatcher_SelectUltimateDifficulty() {}
Dispatcher_SelectUltimateDifficulty::~Dispatcher_SelectUltimateDifficulty() {}
int Dispatcher_SelectUltimateDifficulty::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08226f30（PacketDispatcher_Impl_1.cpp 0xe2d9 区）
    if (user->get_state() != 5)
    {
        return 0;
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0 || party->getManager() != user)
    {
        return 0;
    }
    CBattle_Field* battleField =
        reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);
    if (2 < battleField->getBloodState())
    {
        return 0;
    }
    char difficulty = 0;
    if (!packet.get_byte(difficulty))
    {
        return LineFunc(0xe2d9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if ((signed char)difficulty != 1 && (signed char)difficulty != 2)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__,
                              0xe2dd, "ULTIMATE_LOG : Difficulty ERROR!! difficulty (%d)\n",
                              (int)(signed char)difficulty);
        return 0;
    }
    if (party->m_dungeon == 0 || party->m_dungeon->get_index() != 0x2aff)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__,
                              0xe2e3,
                              "ULTIMATE_LOG : Dispatcher_SelectUltimateDifficulty ERROR!!\n");
    }
    else if (battleField->getUltimateDifficylty() == 0)
    {
        battleField->setUltimateDifficulty(difficulty);
        PacketGuard guard;
        guard.put_header(1, 0x13e);
        guard.put_byte(1);
        guard.put_byte((int)(signed char)difficulty);
        guard.finalize(true);
        party->send_to_party(guard);
        battleField->startUltimateRound();
    }
    return 0;
}
int Dispatcher_SelectUltimateDifficulty::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Select_Item_Grwoth_Power::Dispatcher_Select_Item_Grwoth_Power() {}
Dispatcher_Select_Item_Grwoth_Power::~Dispatcher_Select_Item_Grwoth_Power() {}
int Dispatcher_Select_Item_Grwoth_Power::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081db580：state>2 且当前角色非空才放行；随后要求
    // (growth_power_reward & 0xff) < 当前段下界（get_break_away_section_low_level）。
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return -1;
    }
    int level = user->get_charac_level();
    int low = G_CDataManager()->m_growthPower.get_break_away_section_low_level(level);
    unsigned char reward = user->get_growth_power_reward();
    if (static_cast<int>(reward) < low)
    {
        return 0;
    }
    return -1;
}

int Dispatcher_Select_Item_Grwoth_Power::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }

    MSG_GROWTH_POWER& req = static_cast<MSG_GROWTH_POWER&>(msg);
    stGrowthPowerParam& out = static_cast<stGrowthPowerParam&>(param);
    CDataManager* dm = G_CDataManager();

    STBreakAwaySection section =
        dm->m_growthPower.get_break_away_section_data(user->get_charac_level());
    int key = makeFairPVPEquipmentPackageKey(
        user->get_charac_job(),
        static_cast<int>(static_cast<unsigned char>(section.m_field0)));
    std::map<int, std::vector<std::vector<STStackablePackageData> > >::iterator it =
        dm->m_growthPower.m_map18.find(key);
    if (it == dm->m_growthPower.m_map18.end())
    {
        return 0;
    }

    std::vector<Inven_Item> items;
    int idx = 0;
    for (std::vector<std::vector<STStackablePackageData> >::const_iterator oit = it->second.begin();
         oit != it->second.end(); ++oit)
    {
        unsigned char sel = *reinterpret_cast<unsigned char*>(&req.m_0e[idx]);
        if (oit->size() <= static_cast<unsigned int>(sel))
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x4a8b, 0);
            trace("[GrowthPower](%d)(j:%d)(l:%d)(s:%d)(p:%d)",
                  user->get_acc_id(), user->get_charac_job(), user->get_charac_level(),
                  static_cast<int>(oit->size()), static_cast<int>(sel));
            return LineFunc(0x4a8c,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }
        if (idx > 4)
        {
            return LineFunc(0x4a8f,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }

        Inven_Item item;
        // ORIG 用 vector::at(sel)（_ZNKSt6vectorI22STStackablePackageDataSaIS0_EE2atEj）。
        item.m_addInfo = oit->at(sel).m_field0;
        CItem* info = dm->find_item(item.m_addInfo);
        if (info == 0)
        {
            out.m_4 = 0x11;
            return 0;
        }
        info->make_item(item);
        if (item.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
        {
            item.m_addInfo2 = (*oit)[sel].m_field4;
        }
        items.push_back(item);
        ++idx;
    }

    for (std::vector<Inven_Item>::const_iterator iit = items.begin(); iit != items.end(); ++iit)
    {
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            *iit, static_cast<eItemAddReason>(8), true, true);
        if (slot < 0)
        {
            char title[0x15];
            char body[0x100];
            strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_213", 0), 0x14);
            strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_214", 0), 0xff);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, *iit, 0, user->getCurCharacNo(), body,
                static_cast<int>(strlen(body)), 0, user->GetServerGroup(), false, false);
        }
        else
        {
            user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                                     static_cast<ENUM_ITEMSPACE>(0), slot);
        }
    }
    return 0;
}

int Dispatcher_Select_Item_Grwoth_Power::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_GROWTH_POWER& req = static_cast<MSG_GROWTH_POWER&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x4a55,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < static_cast<int>(req.m_0d); ++i)
    {
        if (!packet.get_byte(*reinterpret_cast<unsigned char*>(&req.m_0e[i])))
        {
            return LineFunc(0x4a5a,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
}

void Dispatcher_Select_Item_Grwoth_Power::send(CUser* user, ParamBase& param)
{
    stGrowthPowerParam& out = static_cast<stGrowthPowerParam&>(param);
    if (out.m_4 == 0)
    {
        int level = user->get_charac_level();
        user->set_growth_power_reward(level);
        user->SendCmdOkPacket(static_cast<ENUM_CMDPACKET>(0x1b0));
    }
    else
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1b0),
                                 static_cast<unsigned char>(out.m_4));
    }
}

Dispatcher_SeriaRoom_Ani_Deco_Event::Dispatcher_SeriaRoom_Ani_Deco_Event() {}
Dispatcher_SeriaRoom_Ani_Deco_Event::~Dispatcher_SeriaRoom_Ani_Deco_Event() {}
int Dispatcher_SeriaRoom_Ani_Deco_Event::check_error(CUser* user, MSG_BASE& msg)
{
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x69);
    return event->IsEventing(0) ? 0 : 1;
}
int Dispatcher_SeriaRoom_Ani_Deco_Event::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SERIAROOM_DECO_EVENT& req = static_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_decoEventId)))
    {
        return LineFunc(0x6540,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

Dispatcher_UpdateContractOfCubeInfo::Dispatcher_UpdateContractOfCubeInfo() {}
Dispatcher_UpdateContractOfCubeInfo::~Dispatcher_UpdateContractOfCubeInfo() {}
int Dispatcher_UpdateContractOfCubeInfo::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x825fa8e：premium 类型 0x5c 未受影响才返回错误 8（无 user==0 防御检查）。
    return user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x5c) ? 0 : 8;
}
// ORIG 0x0825f900（报告 Dispatcher_UpdateContractOfCubeInfo/process.md）：
// ParamBase 实际为 stUpdateContractOfCubeInfo（m_4/m_8/m_9）；check_error
//（ORIG 0x0825fa8e）已由既有实现提供。
int Dispatcher_UpdateContractOfCubeInfo::process(CUser* user, MSG_BASE& msg,
                                                 ParamBase& param)
{
    MSG_UPDATE_CONTRACT_OF_CUBE_INFO& m =
        static_cast<MSG_UPDATE_CONTRACT_OF_CUBE_INFO&>(msg);
    stUpdateContractOfCubeInfo& p = static_cast<stUpdateContractOfCubeInfo&>(param);
    p.m_8 = m.m_useFlag;
    p.m_9 = m.m_selectIdx;
    int err = check_error(user, msg);
    p.m_4 = err;
    if (err != 0)
    {
        return 0;
    }
    // ORIG：setConctractOfCubePremiumInfo(useFlag!=0, selectIdx) 后 SelectCubeForPremiumService。
    user->setConctractOfCubePremiumInfo(m.m_useFlag != 0,
                                        (unsigned char)m.m_selectIdx);
    user->m_historyLog.SelectCubeForPremiumService(
        (int)(unsigned char)m.m_selectIdx);
    return 0;
}
int Dispatcher_UpdateContractOfCubeInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x0825f868：byte@0xd → byte@0xe
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0xd)))
        return LineFunc(0x95e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0xe)))
        return LineFunc(0x961, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_UpdateContractOfCubeInfo::send(CUser* user, ParamBase& param)
{
    // ORIG 0x0825f9ae：header(1,0x20e)+byte(p4==0)+byte p8+byte p9
    stUpdateContractOfCubeInfo& p = static_cast<stUpdateContractOfCubeInfo&>(param);
    PacketGuard pkt;
    pkt.put_header(1, 0x20e);
    pkt.put_byte(p.m_4 == 0 ? 1 : 0);
    pkt.put_byte(p.m_8);
    pkt.put_byte(p.m_9);
    pkt.finalize(true);
    user->Send(pkt);
}

Dispatcher_UseCreatureEvolutionItem::Dispatcher_UseCreatureEvolutionItem() {}
Dispatcher_UseCreatureEvolutionItem::~Dispatcher_UseCreatureEvolutionItem() {}
int Dispatcher_UseCreatureEvolutionItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08228a76
    // ORIG cmp $0x3,get_state; setne → state!=3 恒返 0
    if (user->get_state() != 3)
    {
        return 0;
    }

    unsigned char evolveSlot = 0;   // local_11（-0xd）
    short itemCount = 0;            // local_14（-0x10）
    int itemIdx = 0;                // local_18（-0x14）

    if (!packet.get_byte(evolveSlot))
    {
        return LineFunc(0xe609,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(itemCount))
    {
        return LineFunc(0xe60a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(itemIdx))
    {
        return LineFunc(0xe60b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    int err = process(user, evolveSlot, itemCount, itemIdx);
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x144, (unsigned char)err);
    }
    return 0;
}
// ORIG 0x08228bcc（报告 UseCreatureEvolutionItem/process.md）：使用生物进化道具。
// 流程：物品空间→inven 槽位取物 → find_item → 非 stackable/非本生物(0x360) 报错；
// 通过则 CCreatureMgr::ItemEvolute。日志 cMyTrace(0xe621/0xe627/0xe62f)、
// cHistoryTrace("ItemEvolute ...") 与 ORIG rodata 逐字节一致。
int Dispatcher_UseCreatureEvolutionItem::process(CUser* user,
                                                 unsigned char evolveSlot,
                                                 short itemCount, int itemIdx)
{
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)evolveSlot);
    const Inven_Item* item =
        user->getCurCharacInvenR()->GetInvenRef(invenType, itemCount);
    if (item == 0)
        return 4;

    CItem* itemInfo = G_CDataManager()->find_item(item->m_addInfo);
    if (itemInfo == 0)
    {
        cMyTrace local_58(__PRETTY_FUNCTION__, 0xe621, 0);
        local_58("Dispatcher_UseCreatureEvolutionItem not found item_id(%d), charac_no(%d)\n",
                 item->m_addInfo, user->getCurCharacNo());
        return 0x15;
    }
    if (!itemInfo->is_stackable())
    {
        cMyTrace local_48(__PRETTY_FUNCTION__, 0xe627, 0);
        local_48("Dispatcher_UseCreatureEvolutionItem item_id(%d) is not stackable charac_no(%d)\n",
                 item->m_addInfo, user->getCurCharacNo());
        return 0x17;
    }

    CStackableItem* stackable = static_cast<CStackableItem*>(itemInfo);
    int creatureIdx =
        user->getCurCharacInvenW()->GetCreatureMgrW()->GetCreatureIndex();
    if (creatureIdx != stackable->m_int360)
    {
        cMyTrace local_38(__PRETTY_FUNCTION__, 0xe62f, 0);
        local_38("Dispatcher_UseCreatureEvolutionItem cur_creatureid(%d) != item_base_creature_id(%d) charac_no(%d)\n",
                 creatureIdx, stackable->m_int360, user->getCurCharacNo());
        return 7;
    }

    if (user->getCurCharacInvenRefW()->GetCreatureMgrW()->ItemEvolute(itemIdx))
    {
        cHistoryTrace local_26;
        local_26("ItemEvolute Succ evolution_creature_id(%d), charac_no(%d)",
                 itemIdx, user->getCurCharacNo());
        return 0;
    }
    cHistoryTrace local_25;
    local_25("ItemEvolute Fail evolution_creature_id(%d), charac_no(%d)",
             itemIdx, user->getCurCharacNo());
    return 1;
}
int Dispatcher_UseCreatureEvolutionItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseCreatureEvolutionItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseRightOfChangeGrowType::Dispatcher_UseRightOfChangeGrowType() {}
Dispatcher_UseRightOfChangeGrowType::~Dispatcher_UseRightOfChangeGrowType() {}
int Dispatcher_UseRightOfChangeGrowType::check_error(CUser* user, MSG_BASE& msg)
{
    if (user != 0 && user->getCurCharacR() != 0)
    {
        return 0xffffffff;
    }
    if (user == 0 || user->get_state() != 3)
    {
        return 0x1e;
    }
    if (user->get_charac_job() == 9)
    {
        return 0xea;
    }
    if (user->get_charac_job() == 10)
    {
        return 0xea;
    }
    if (user->getCurCharFirstGrowType() == 0)
    {
        return 0x13;
    }
    if (user->CheckInTrade() != 0)
    {
        return 0xd8;
    }
    const unsigned char growCount =
        *reinterpret_cast<const unsigned char*>(
            reinterpret_cast<const char*>(&msg) + 0xf);
    if (static_cast<signed char>(growCount) < 0 || growCount > 5)
    {
        return 1;
    }
    const signed short slotIdx =
        static_cast<signed short>(
            *reinterpret_cast<const unsigned short*>(
                reinterpret_cast<const char*>(&msg) + 0xd));
    const Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(1, static_cast<int>(slotIdx));
    CItem* citem = G_CDataManager()->find_item(item.m_addInfo);
    if (citem == 0)
    {
        return 0x11;
    }
    UserQuest* quest = user->getCurCharacQuestRefR();
    if (quest->isDoingJobChangeQuest())
    {
        return 0x16;
    }
    if (!user->verifyStackableItemActionPlace((StackableAction::Type)0x2e, citem))
    {
        return 0x17;
    }
    return 0;
}
// ORIG 0x0825dc64 T _ZN35Dispatcher_UseRightOfChangeGrowType7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_UseRightOfChangeGrowType/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 stUseRightOfChangeGrowTypeParam，ORIG 裸访问 +0x04 错误码槽（m_4）。
int Dispatcher_UseRightOfChangeGrowType::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    *(int*)((char*)&param + 4) = check_error(user, msg);
    int err = *(int*)((char*)&param + 4);
    if (err >= 1)
    {
        return 0;
    }
    if (err < 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp",
                              __PRETTY_FUNCTION__,
                              0x93, "Dispatcher Check_Error No(%d)", err);
        return LineFunc(0x94,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x2a);
    if (secu != 0)
    {
        *(int*)((char*)&param + 4) = secu;
        return 0;
    }
    // ORIG：(int)(char)msg+0xf（movzbl+movsbl），(short)msg+0xd（movzwl+movswl）
    *(int*)((char*)&param + 4) =
        user->ChangeGrowType_Item(
            static_cast<signed char>(*(unsigned char*)((char*)&msg + 0xf)),
            static_cast<short>(*(unsigned short*)((char*)&msg + 0xd)));
    user->_SaveCharacData();
    return 0;
}
int Dispatcher_UseRightOfChangeGrowType::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x0825dbcc：short@0xd → char@0xf
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<short*>(m + 0xd)))
        return LineFunc(0x81, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(m + 0xf)))
        return LineFunc(0x84, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_UseRightOfChangeGrowType::send(CUser* user, ParamBase& param)
{
    // ORIG 0x0825df24 T：err=*(int*)((char*)&param+4)；0→SendCmdOkPacket(0x1f7)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1f7);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1f7, (unsigned char)err);
}

Dispatcher_AuctionMyRegistedItemInfo::Dispatcher_AuctionMyRegistedItemInfo() {}
Dispatcher_AuctionMyRegistedItemInfo::~Dispatcher_AuctionMyRegistedItemInfo() {}
int Dispatcher_AuctionMyRegistedItemInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned char channel = 0;                             // -0x21
    if (!packet.get_byte(channel))
    {
        return LineFunc(0xc78e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (channel > 1)
    {
        return LineFunc(0xc791,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (IsAuctionServerDisconnected(channel))              // 0x082138f2
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xbf, 0x8c, (PAY_TYPE)(unsigned int)channel);
        return 0;
    }
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xc7b6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (channel == 1)
    {
        PCK_AUCTION_MY_REGISTED_ITEM_INFO_GP pkt;          // -0x3b ctor 0x0822eb70
        *(unsigned int*)((char*)&pkt + 0xe) = user->GetUID();        // local_2d
        *(unsigned int*)((char*)&pkt + 0x12) = (unsigned int)user->getCurCharacNo();  // local_29
        GlobalData::s_cera_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pkt);
        return 0;
    }
    PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA pkt;              // -0x55 ctor 0x0822e7ac
    *(unsigned int*)((char*)&pkt + 0xe) = user->GetUID();            // local_47
    *(unsigned int*)((char*)&pkt + 0x12) = (unsigned int)user->getCurCharacNo();    // local_43
    GlobalData::s_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&pkt);
    for (int i = 0; i < 5; i++)                            // gmList 0x093701d8
    {
        if (gmList[i] == (int)user->getCurCharacNo())
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xc823, 0);
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionMyRegistedItemInfo",
                  (int)user->getCurCharacNo());
            break;
        }
    }
    return 0;
}

int Dispatcher_AuctionMyRegistedItemInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionMyRegistedItemInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BloodRoundUiPrepareFinish::Dispatcher_BloodRoundUiPrepareFinish() {}
Dispatcher_BloodRoundUiPrepareFinish::~Dispatcher_BloodRoundUiPrepareFinish() {}
int Dispatcher_BloodRoundUiPrepareFinish::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8222270：战斗态+血轮地下城且非（bloodType<1 或物品等级==1）→ 血轮生成计时器
    if (user == 0 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xdbc0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() != 5)
    {
        return LineFunc(0xdbc2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0 || party->m_dungeon == 0)
    {
        return 0;
    }
    CBattle_Field& field = *reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1);
    MapInfo* mi = field.GetMapInfo();
    bool skip = (party->m_dungeon->m_bloodDungeonType < 1) ||
                (mi != 0 && mi->m_visited == 1);
    if (!skip)
    {
        field.BloodSpawnTimerManager(false, false);
    }
    return 0;
}
int Dispatcher_BloodRoundUiPrepareFinish::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BloodRoundUiPrepareFinish::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Skill_Command_All_Default::Dispatcher_Skill_Command_All_Default() {}
Dispatcher_Skill_Command_All_Default::~Dispatcher_Skill_Command_All_Default() {}
// ORIG 0x081d2ef8（报告 Dispatcher_Skill_Command_All_Default/process.md）：
// defaultSkillCommand() 假 → param.m_4=0xfc；返 0。
int Dispatcher_Skill_Command_All_Default::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    stSkillCommandParam& out = static_cast<stSkillCommandParam&>(param);
    if (!user->defaultSkillCommand())
    {
        out.m_4 = 0xfc;
    }
    return 0;
}
int Dispatcher_Skill_Command_All_Default::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Skill_Command_All_Default::send(CUser* user, ParamBase& param)
{
    int err = *(int*)((char*)&param + 4);
    PacketGuard guard;
    guard.put_header(1, 0x14d);
    if (err == 0)
    {
        guard.put_byte(1);
    }
    else
    {
        guard.put_byte(0);
        guard.put_short(err);
    }
    guard.finalize(true);
    user->Send(guard);
}


Dispatcher_Skill_Command_Customizing::Dispatcher_Skill_Command_Customizing() {}
Dispatcher_Skill_Command_Customizing::~Dispatcher_Skill_Command_Customizing() {}

Dispatcher_Buy_Charac_Status_Using_QP::Dispatcher_Buy_Charac_Status_Using_QP() {}
Dispatcher_Buy_Charac_Status_Using_QP::~Dispatcher_Buy_Charac_Status_Using_QP() {}
int Dispatcher_Buy_Charac_Status_Using_QP::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    return user->get_state() == 3 ? 0 : 99;
}
// ORIG 0x081d64b4 T _ZN37Dispatcher_Buy_Charac_Status_Using_QP7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_Buy_Charac_Status_Using_QP/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 STQuestShopBuyParam（+0x04 byte/+0x06 short/+0x08 int）。
int Dispatcher_Buy_Charac_Status_Using_QP::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    if (err >= 1)
    {
        return -1;
    }
    if (err < 0)
    {
        return LineFunc(0x3fc9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG GetCharacExpandData(param_1, 0xb)（扩展数据 0xb 槽）
    CQuestShop* shop = reinterpret_cast<CQuestShop*>(
        user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xb)));
    if (shop == 0)
    {
        *(int*)((char*)&param + 8) = 1;
        return 0;
    }
    short result = 0;   // local_22
    int ok = shop->buyStatus(user,   // ORIG 返回码 0/1/0xe（param+8 需完整值）
                             *(unsigned char*)((char*)&msg + 0xd),
                             *(unsigned char*)((char*)&msg + 0xe),
                             result);
    if (ok)
    {
        *(unsigned char*)((char*)&param + 4) =
            *(unsigned char*)((char*)&msg + 0xd);   // param+4 ← msg+0xd
        *(short*)((char*)&param + 6) = result;      // param+6 ← local_22
        return 0;
    }
    *(int*)((char*)&param + 8) = ok;   // ORIG param+8 = buyStatus 返回码
    return 0;
}
int Dispatcher_Buy_Charac_Status_Using_QP::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 unsigned char(msg+0xd)，失败记 LineFunc(0x3fb8)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xd)))
    {
        return LineFunc(0x3fb8, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x T：读 unsigned char(msg+0xe)，失败记 LineFunc(0x3fba)
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xe)))
    {
        return LineFunc(0x3fba, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_Buy_Charac_Status_Using_QP::send(CUser* user, ParamBase& param)
{
    int err = *(int*)((char*)&param + 8);
    PacketGuard guard;
    guard.put_header(1, 399);
    if (err == 0)
    {
        guard.put_byte(1);
        guard.put_byte((int)*(unsigned char*)((char*)&param + 4));
        guard.put_short((int)*(short*)((char*)&param + 6));
        guard.finalize(true);
        user->Send(guard);
        user->sendCharacQp();
    }
    else
    {
        guard.put_byte(0);
        guard.put_short(err);
        guard.finalize(true);
        user->Send(guard);
    }
}


Dispatcher_Change_PartyMemberPosition::Dispatcher_Change_PartyMemberPosition() {}
Dispatcher_Change_PartyMemberPosition::~Dispatcher_Change_PartyMemberPosition() {}
int Dispatcher_Change_PartyMemberPosition::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0822759c [0x0822759c, 0x08227861]
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return 0;
    }
    if (party->getManager() != user)
    {
        return 0;
    }
    char from = 4;                                           // -0xd（ctor 初始 4）
    char to = 0;                                             // -0xe
    if (!packet.get_byte(from))
    {
        return LineFunc(0xe384,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(to))
    {
        return LineFunc(0xe385,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PacketGuard guard;
    if (*reinterpret_cast<unsigned char*>(&from) > 0x7f || 3 < (int)(char)from ||
        (to != 3 && to != 1))
    {
        guard.put_header(1, 0x150);
        guard.put_byte(0);
        guard.put_byte((int)from);
        guard.put_byte((int)(char)to);
        guard.finalize(true);
        user->Send(guard);
    }
    else
    {
        to = (char)(from + to) & 3;
        party->switchPostion((char)from, (char)to);
        party->send_party_realtime_info();
        party->send_party_ipinfo();
        guard.put_header(1, 0x150);
        guard.put_byte(1);
        guard.put_byte((int)from);
        guard.put_byte((int)(char)to);
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
}
int Dispatcher_Change_PartyMemberPosition::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Change_PartyMemberPosition::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestIngameAdvertisement::Dispatcher_RequestIngameAdvertisement() {}
Dispatcher_RequestIngameAdvertisement::~Dispatcher_RequestIngameAdvertisement() {}

Dispatcher_CallPartyMemberRealtimeInfo::Dispatcher_CallPartyMemberRealtimeInfo() {}
Dispatcher_CallPartyMemberRealtimeInfo::~Dispatcher_CallPartyMemberRealtimeInfo() {}
int Dispatcher_CallPartyMemberRealtimeInfo::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return LineFunc(0x2781,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return user->get_state() == 3 ? 0 : -1;
}
// ORIG 0x081ce356（报告 Dispatcher_CallPartyMemberRealtimeInfo/process.md）：
// 不写 param+4；仅按 check_error 结果归一返回。
int Dispatcher_CallPartyMemberRealtimeInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    int err = check_error(user, msg);
    if (err < 1)
    {
        if (err < 0)
        {
            return -1;
        }
        return 0;
    }
    return err;
}
int Dispatcher_CallPartyMemberRealtimeInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_CallPartyMemberRealtimeInfo::send(CUser* user, ParamBase& param)
{
    CParty* party = (CParty*)user->GetParty();
    PacketGuard guard;
    guard.put_header(1, 0xa9);
    if (party == 0)
    {
        guard.put_byte(0);
        guard.put_byte(0x13);
        guard.finalize(true);
        user->Send(guard);
    }
    else
    {
        party->send_party_realtime_info();
    }
}


Dispatcher_Direct_Entrance_Quick_Party::Dispatcher_Direct_Entrance_Quick_Party() {}
Dispatcher_Direct_Entrance_Quick_Party::~Dispatcher_Direct_Entrance_Quick_Party() {}
// ORIG 0x081de2e6（nm 实测，sz 0x43）：state>3 且 getCurCharacR()!=0 → 0，否则 -1。
int Dispatcher_Direct_Entrance_Quick_Party::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() <= 3 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x081de334（报告 Dispatcher_Direct_Entrance_Quick_Party/process.md）：
// direct_entrance_quick_party(user) 返回值写 param+0x4；两侧分支均 return 0。
int Dispatcher_Direct_Entrance_Quick_Party::process(CUser* user, MSG_BASE& msg,
                                                    ParamBase& param)
{
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    CGameManager* gm = G_CGameManager();
    QuickParty::CQuickPartySystemManager* mgr = gm->GetQuickPartySystemManager();
    *(int*)((char*)&param + 0x4) = mgr->direct_entrance_quick_party(user);
    return 0;
}
int Dispatcher_Direct_Entrance_Quick_Party::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Direct_Entrance_Quick_Party::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081de3b8 T：err=*(int*)((char*)&param+4)；0→SendCmdOkPacket(0x1be)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1be);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1be, (unsigned char)err);
}

Dispatcher_Exchange_Random_Item_Reward::Dispatcher_Exchange_Random_Item_Reward() {}
Dispatcher_Exchange_Random_Item_Reward::~Dispatcher_Exchange_Random_Item_Reward() {}
int Dispatcher_Exchange_Random_Item_Reward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stExchangeRandomItemReward& out = static_cast<stExchangeRandomItemReward&>(param);
    if (user == 0 || user->get_state() != 3)
    {
        out.m_4 = 7;
        return 0;
    }
    const unsigned int eventIdx =
        static_cast<unsigned int>(
            *reinterpret_cast<const unsigned short*>(
                reinterpret_cast<const char*>(&msg) + 0xe));
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
        user, *reinterpret_cast<std::vector<int>*>(
                  reinterpret_cast<char*>(&msg) + 0x10),
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
int Dispatcher_Exchange_Random_Item_Reward::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_EXCHANGE_RANDOM_ITEM_REWARD& data =
        static_cast<MSG_EXCHANGE_RANDOM_ITEM_REWARD&>(msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(
            reinterpret_cast<char*>(&msg) + 0xe)))
    {
        return LineFunc(0x4fc8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char count = 0;
    if (!packet.get_byte(count))
    {
        return LineFunc(0x4fcc,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        unsigned short item = 0;
        if (!packet.get_short(item))
        {
            return LineFunc(0x4fd2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        data.m_rewardItems.push_back(static_cast<int>(item));
    }
    return 0;
}

void Dispatcher_Exchange_Random_Item_Reward::send(CUser* user, ParamBase& param)
{
    stExchangeRandomItemReward& out = static_cast<stExchangeRandomItemReward&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1c1);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_byte((unsigned int)out.m_rewards.size());
        for (size_t i = 0; i < out.m_rewards.size(); ++i)
        {
            packet.put_int(out.m_rewards[i].first);
            packet.put_int(out.m_rewards[i].second);
        }
        packet.put_byte((unsigned int)(unsigned char)out.m_9);
        if (out.m_9 != 0)
        {
            // ORIG 迭代器循环推进 4 次、循环头判断 !=end，每组写 short/int/int/short。
            // 数据应为 4 的倍数；此处加 i+3< size 边界防越界（ORIG 无保护，语义仅在
            // 异常尺寸时不同）【推断】。
            for (size_t i = 0; i + 3 < out.m_items.size(); i += 4)
            {
                packet.put_short(out.m_items[i]);
                packet.put_int(out.m_items[i + 1]);
                packet.put_int(out.m_items[i + 2]);
                packet.put_short(out.m_items[i + 3]);
            }
        }
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4);
    }
    packet.finalize(true);
    user->Send(packet);
}


Dispatcher_RequestConditionEventReward::Dispatcher_RequestConditionEventReward() {}
Dispatcher_RequestConditionEventReward::~Dispatcher_RequestConditionEventReward() {}
int Dispatcher_RequestConditionEventReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082223a6..0x082226bf (0x31a 字节)。
    static const char* const kPretty =
        "virtual int Dispatcher_RequestConditionEventReward::dispatch_sig(CUser*, PacketBuf&)";

    if (user == 0)
        return LineFunc(0xdbda, kPretty, 0, 0);
    if (user->get_state() != 3)
        return 0;

    short step = 0;       // -0x16（get_short(short&)）
    char useQuickPartyAdd = 0;   // -0x17（get_byte(char&)）
    if (packet.get_short(step) != 1)
        return LineFunc(0xdbe1, kPretty, 0, 0);
    if (packet.get_byte(useQuickPartyAdd) != 1)
        return LineFunc(0xdbe2, kPretty, 0, 0);

    int result = 0;   // -0x10
    CConditionEventManager* mgr = G_CGameManager()->GetConditionEventManager();
    std::map<int, int> rewardMap;   // -0x30
    if (mgr != 0)
        result = mgr->RequestConditionEventReward(user, step, useQuickPartyAdd, rewardMap);

    PacketGuard guard;
    guard.put_header(1, 0x107);
    if (result == 0)
    {
        guard.put_byte(1);
        guard.put_byte((int)(char)step);
        guard.put_byte((int)useQuickPartyAdd);
        guard.put_byte((int)rewardMap.size());
        for (std::map<int, int>::const_iterator it = rewardMap.begin(); it != rewardMap.end(); ++it)
        {
            guard.put_int(it->first);
            guard.put_int(it->second);
        }
        guard.finalize(true);
        user->Send(guard);
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(result);
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
}
int Dispatcher_RequestConditionEventReward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestConditionEventReward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TournamentRewardSelectState::Dispatcher_TournamentRewardSelectState() {}
Dispatcher_TournamentRewardSelectState::~Dispatcher_TournamentRewardSelectState() {}
// ORIG 0x081dff32（报告 Dispatcher_TournamentRewardSelectState/check_error.md）：
// state!=5 → 0x7fffffff；GetParty(GetPartyIndex)==0 → -1；
// TournamentDungeonClearState!=1 → 0x7fffffff；!CheckRecvFromAllMember → 0x7fffffff。
int Dispatcher_TournamentRewardSelectState::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user->get_state() != 5)
    {
        return 0x7fffffff;
    }
    int partyIdx = user->GetPartyIndex();
    CParty* party = G_CGameManager()->GetParty(partyIdx);
    if (party == 0)
    {
        return -1;
    }
    if (party->GetTournamentDungeonClearState() != 1)
    {
        return 0x7fffffff;
    }
    if (!party->CheckRecvFromAllMember(user))
    {
        return 0x7fffffff;
    }
    return 0;
}
int Dispatcher_TournamentRewardSelectState::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081dfca2
    stTournamentRewardSelectState& p = static_cast<stTournamentRewardSelectState&>(param);
    *(int*)((char*)&p + 4) = check_error(user, msg);
    if (*(int*)((char*)&p + 4) >= 1)
    {
        return 0;
    }
    if (*(int*)((char*)&p + 4) < 0)
    {
        return LineFunc(0x52af,
                        __PRETTY_FUNCTION__,
                        *(int*)((char*)&p + 4), 0);
    }
    int partyIndex = user->GetPartyIndex();
    CParty* party = G_CGameManager()->GetParty(partyIndex);
    if (party == 0)
    {
        return LineFunc(0x52b4,
                        __PRETTY_FUNCTION__,
                        0, (unsigned int)user->get_acc_id());
    }
    return 0;
}
int Dispatcher_TournamentRewardSelectState::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_TournamentRewardSelectState::send(CUser* user, ParamBase& param)
{
    stTournamentRewardSelectState& out = static_cast<stTournamentRewardSelectState&>(param);
    // +0x04 int（ORIG *(int*)(param+4)）；头文件补具名 m_4 后应直接引用。
    const int m_4 = *reinterpret_cast<const int*>(out.m_pad);
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x1ca);
    if (m_4 == 0)
    {
        packet.put_byte(1);
        CParty* party = G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
        if (party != 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = party->get_user(i);
                if (member == 0)
                {
                    packet.put_byte(-1);
                }
                else if (member->getCurCharacTotalFatigue() ==
                         member->getCurCharacTotalMaxFatigue())
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


Dispatcher_Expand_Equipslot_Flag_Update::Dispatcher_Expand_Equipslot_Flag_Update() {}
Dispatcher_Expand_Equipslot_Flag_Update::~Dispatcher_Expand_Equipslot_Flag_Update() {}
// ORIG 0x081d5f10（报告 Dispatcher_Expand_Equipslot_Flag_Update/process.md）：
// user==0 → 1；state<3 → 0x13；UpdateEquipslotFlag==0x13 → LineFunc(0x3df3)；
// 否则 param.m_4=(unsigned char)结果；返 0。
int Dispatcher_Expand_Equipslot_Flag_Update::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stExpandEquipslotFlagUpdate& out = static_cast<stExpandEquipslotFlagUpdate&>(param);
    MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE& req =
        static_cast<MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE&>(msg);
    if (user == 0)
    {
        return 1;
    }
    if (user->get_state() < 3)
    {
        return 0x13;
    }
    int result = user->UpdateEquipslotFlag((char)req.m_flag);
    if (result == 0x13)
    {
        return LineFunc(0x3df3,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    out.m_4 = (unsigned char)result;
    return 0;
}
int Dispatcher_Expand_Equipslot_Flag_Update::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 char(msg+0xd)，失败记 LineFunc(0x3de4)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(p + 0xd)))
    {
        return LineFunc(0x3de4, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_Expand_Equipslot_Flag_Update::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d5fae：clear+header(1,0x18e)+byte 1+byte(char)p4
    stExpandEquipslotFlagUpdate& p = static_cast<stExpandEquipslotFlagUpdate&>(param);
    PacketGuard pkt;
    pkt.clear();
    pkt.put_header(1, 0x18e);
    pkt.put_byte(1);
    pkt.put_byte((int)(char)p.m_4);
    pkt.finalize(true);
    user->Send(pkt);
}

Dispatcher_FinishVillageMonsterFighting::Dispatcher_FinishVillageMonsterFighting() {}
Dispatcher_FinishVillageMonsterFighting::~Dispatcher_FinishVillageMonsterFighting() {}
// ORIG 0x0822284a T _ZN39Dispatcher_FinishVillageMonsterFighting12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/Dispatcher_FinishVillageMonsterFighting/dispatch_sig.md 汇编+反编译 C 全文转写）
// ORIG G_CEnvironment()+0x1a8 = m_serverEnvir.m_serverType（Server_Envir 基址 +0x198 + 0x10）。
int Dispatcher_FinishVillageMonsterFighting::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return 0;
    }
    int serverType = G_CEnvironment()->m_serverEnvir.m_serverType;
    if (serverType == 1 || serverType == 2)
    {
        return 0;
    }
    if (&packet == 0)
    {
        return LineFunc(0xdc66,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() != 5)
    {
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        int result = party->OnFinishVillageMonsterFighting(user, false);
        if (result != 0)
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x10c),
                                     static_cast<unsigned char>(result));
        }
    }
    return 0;
}
int Dispatcher_FinishVillageMonsterFighting::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_FinishVillageMonsterFighting::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GrowthWeaponEventUseMaterial::Dispatcher_GrowthWeaponEventUseMaterial() {}
Dispatcher_GrowthWeaponEventUseMaterial::~Dispatcher_GrowthWeaponEventUseMaterial() {}
int Dispatcher_GrowthWeaponEventUseMaterial::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return 0xffffffff;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    const int slotIdx =
        *reinterpret_cast<const int*>(reinterpret_cast<const char*>(&msg) + 0x11);
    const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(1, slotIdx);
    if (item->m_addInfo == 0)
    {
        return 0x15;
    }
    if (item->m_addInfo != G_CDataManager()->get_GrowthWeaponMaterialItem())
    {
        return 0x11;
    }
    char* ed = static_cast<char*>(user->getGrowthWeaponEventdata());
    if (*reinterpret_cast<int*>(ed + 4) != user->get_charac_no(-1))
    {
        return 3;
    }
    int serverGroup;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() == 0)
    {
        serverGroup =
            *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    }
    else
    {
        serverGroup = user->GetServerGroup();
    }
    if (static_cast<signed short>(*reinterpret_cast<unsigned short*>(ed + 0x12)) !=
        serverGroup)
    {
        return 3;
    }
    if (static_cast<signed short>(*reinterpret_cast<unsigned short*>(ed + 0x10)) >=
        G_CDataManager()->get_GrowthWeaponMaterialMax())
    {
        return 0x11;
    }
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x64);
    if (!event->IsEventing(0))
    {
        return 1;
    }
    return 0;
}
int Dispatcher_GrowthWeaponEventUseMaterial::process(CUser* user, MSG_BASE& msgBase, ParamBase& paramBase)
{
    // ORIG 0x081e81a4：消耗成长武器活动材料，累加怀表计数并发 DB 更新。
    MSG_GROWTH_EQUIPMENT& msg = *reinterpret_cast<MSG_GROWTH_EQUIPMENT*>(&msgBase);
    stGrowthEquipment& param = *reinterpret_cast<stGrowthEquipment*>(&paramBase);
    param.m_4 = check_error(user, msgBase);
    if (param.m_4 >= 1)
    {
        return 0;
    }
    if (param.m_4 < 0)
    {
        return LineFunc(0x647c, __PRETTY_FUNCTION__, param.m_4, 0);
    }
    CInventory* inven = user->getCurCharacInvenW();
    int ret = inven->delete_item((INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)msg.m_0d),
                                 msg.m_11, 1, (eItemDelReason)0x36, true);
    if (ret != 1)
    {
        param.m_4 = 0x11;
        return 0;
    }
    short* timepiece = reinterpret_cast<short*>(reinterpret_cast<char*>(user->getGrowthWeaponEventdata()) + 0x10);
    *timepiece = static_cast<short>(*timepiece + 1);
    DB_EventUpdateGrowthWeaponTimepiece::makeRequest(user->get_acc_id(), *timepiece, 0);
    param.m_10 = *timepiece;
    param.m_8 = msg.m_0d;
    param.m_c = msg.m_11;
    return 0;
}
int Dispatcher_GrowthWeaponEventUseMaterial::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 int(msg+0xd)，失败记 LineFunc(0x646a)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(p + 0xd)))
    {
        return LineFunc(0x646a, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x T：读 int(msg+0x11)，失败记 LineFunc(0x646d)
    if (!packet.get_int(*reinterpret_cast<int*>(p + 0x11)))
    {
        return LineFunc(0x646d, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_GrowthWeaponEventUseMaterial::send(CUser* user, ParamBase& param)
{
    stGrowthEquipment& out = static_cast<stGrowthEquipment&>(param);
    if (out.m_4 == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x1fa);
        packet.put_byte(1);
        packet.put_int(out.m_8);
        packet.put_int(out.m_c);
        packet.put_short((int)out.m_10);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fa,
                                 (unsigned char)(out.m_4 & 0xff));
    }
}


Dispatcher_HeroMissionEventState_Reward::Dispatcher_HeroMissionEventState_Reward() {}
Dispatcher_HeroMissionEventState_Reward::~Dispatcher_HeroMissionEventState_Reward() {}
int Dispatcher_HeroMissionEventState_Reward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int missionState = 0;
    if (!packet.get_short(*(short*)&missionState))
    {
        return LineFunc(0xb69,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int missionNo = 0;
    if (!packet.get_short(*(short*)&missionNo))
    {
        return LineFunc(0xb6d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (missionState == 1 && missionNo < 0 && missionNo > 6)
    {
        return LineFunc(0xb71,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (missionState == 0)
    {
        HeroMissionEvent* event = ARAD::Singleton<HeroMissionEvent>::Get();
        event->sendMissionData(user);
    }
    else
    {
        HeroMissionEvent* event = ARAD::Singleton<HeroMissionEvent>::Get();
        char rewarded = (char)event->rewardMission(
            user, (HeroMissionCondition::MissionNo::T)missionNo);
        PacketGuard response;
        response.put_header(1, 0x25a);
        if (rewarded == 0)
        {
            response.put_byte(0);
            response.put_byte(0);
            response.put_short(missionState);
        }
        else
        {
            response.put_byte(1);
            response.put_short(missionState);
        }
        response.finalize(true);
        user->Send(response);
    }
    return 0;
}
int Dispatcher_HeroMissionEventState_Reward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_HeroMissionEventState_Reward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_HeroMissionEventState_Reward::send(CUser* user, ParamBase& param) {}

Dispatcher_SaveGameOption_QuickChatting::Dispatcher_SaveGameOption_QuickChatting() {}
Dispatcher_SaveGameOption_QuickChatting::~Dispatcher_SaveGameOption_QuickChatting() {}
int Dispatcher_SaveGameOption_QuickChatting::process(CUser* user, MSG_BASE& msgBase, ParamBase& param)
{
    (void)param;
    // ORIG 0x081cf1ac：保存快捷聊天选项；isLastMessage 且有变更才组 SIG 入 DB_Q。
    MSG_QUICK_CHAT& msg = *reinterpret_cast<MSG_QUICK_CHAT*>(&msgBase);
    CGameOption* gameOption = user->GetGameOptionRef();
    gameOption->SetQuickChatOption(msg.m_quick, (unsigned int)msg.m_11);
    bool save = false;
    if (msg.isLastMessage() && gameOption->isChanged_QuickChatData())
    {
        save = true;
    }
    if (save)
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x291b), true);
        guard.operator*()->operator<<(0x239);
        guard.operator*()->operator<<(static_cast<int>(user->GetUID()));
        SIG_SAVE_GAME_OPTION_QUICKCHAT* sig = guard->GetInBuffer<SIG_SAVE_GAME_OPTION_QUICKCHAT>();
        char* buf = reinterpret_cast<char*>(sig);
        *reinterpret_cast<int*>(buf + 0) = static_cast<int>(user->get_acc_id());
        gameOption->CopyQuickChatting(*reinterpret_cast<CQuickChattingOption*>(buf + 4), 0);
        gameOption->CopyQuickChatting(*reinterpret_cast<CQuickChattingOption*>(buf + 0x130), 1);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
        gameOption->resetFlag_QuickChatChnage();
    }
    return 0;
}
int Dispatcher_SaveGameOption_QuickChatting::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cf0a6：byte@0x11 → int@0xd（len，须 <0x12d）→ binary@0x12
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0x11)))
        return LineFunc(0x290a, __PRETTY_FUNCTION__, 0, 0);
    unsigned int len = 0;
    if (!packet.get_int(len) || len >= 0x12d)
        return LineFunc(0x290b, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_binary(m + 0x12, (int)len))
        return LineFunc(0x290c, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

Dispatcher_Avatar_Disjoint_Random_Reward::Dispatcher_Avatar_Disjoint_Random_Reward() {}
Dispatcher_Avatar_Disjoint_Random_Reward::~Dispatcher_Avatar_Disjoint_Random_Reward() {}
int Dispatcher_Avatar_Disjoint_Random_Reward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stAvatarDisjointRandomReward* local_18 = (stAvatarDisjointRandomReward*)&param;
    MSG_AVATAR_DISJOINT_RANDOM_REWARD* local_1c = (MSG_AVATAR_DISJOINT_RANDOM_REWARD*)&msg;
    Inven_Item local_59;                 // -0x55：GetInvenSlot 输出
    char cVar1;

    if (user->get_state() != 3)
    {
        local_18->m_4 = 7;
        return 0;
    }
    EventClassify::CEventScriptMng* pCVar3 =
        (EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng();
    EventClassify::CAvatarDisjointRandomReward* local_14 =
        (EventClassify::CAvatarDisjointRandomReward*)pCVar3->get_event_entity(0x197);
    if (local_14 == 0)
    {
        local_18->m_4 = 0xb2;
        return 0;
    }
    cVar1 = (char)local_14->is_eventing();   // vtable +0x08 槽
    if (cVar1 != 1)
    {
        local_18->m_4 = 0xb2;
        return 0;
    }
    CInventory* pCVar7 = (CInventory*)user->getCurCharacInvenR();
    local_59 = pCVar7->GetInvenSlot(2, (unsigned int)local_1c->m_slotE);
    if (local_59.m_addInfo != local_1c->m_itemIdx10)
    {
        local_18->m_4 = 0x11;
        return 0;
    }
    int local_57 = local_59.m_addInfo;
    CItem* local_c = G_CDataManager()->find_item(local_57);
    if (local_c == 0)
    {
        local_18->m_4 = 0x11;
        return 0;
    }
    unsigned int uVar4 =
        (unsigned int)local_14->checkAvatarDisjointError(user, (unsigned short)local_1c->m_slotE,
                                                         local_59, local_c);
    CInventory* inven = (CInventory*)user->getCurCharacInvenR();
    local_18->m_4 = (int)uVar4;
    if (local_18->m_4 != 0)
    {
        return 0;
    }
    uVar4 = (unsigned int)local_14->deleteAvatarItem(user, (unsigned short)local_1c->m_slotE,
                                                     local_59, local_c);
    local_18->m_4 = (int)uVar4;
    if (local_18->m_4 != 0)
    {
        return 0;
    }
    uVar4 = (unsigned int)local_14->getRewardItem(local_c, local_18->m_rewards);
    local_18->m_4 = (int)uVar4;
    if (local_18->m_4 != 0)
    {
        return 0;
    }
    local_14->SendCreateEmblemStatistic(local_18->m_rewards);
    uVar4 = (unsigned int)local_14->sendRandomItemReward(user, local_18->m_rewards);
    local_18->m_4 = (int)uVar4;
    if (local_18->m_4 != 0)
    {
        return 0;
    }
    local_18->m_8 = (unsigned int)local_1c->m_slotE;
    return 0;
}
Dispatcher_SDC_ActiveStatus_Damage_Check::~Dispatcher_SDC_ActiveStatus_Damage_Check() {}
int Dispatcher_SDC_ActiveStatus_Damage_Check::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ea16e（docs/class_func_reports/Dispatcher_SDC_ActiveStatus_Damage_Check/dispatch_sig.md）
    unsigned char count = 0;                  // local_59 -0x55
    char src[500];                            // local_261 区 -0x25d（压缩数据）
    char dest[500];                           // local_455 区 -0x451（解压数据）
    char* data = dest;                        // local_44/48
    int srcLen;                               // local_38 -0x34
    unsigned long destLen = 500;              // local_5c -0x5c
    int uncompreRet;                          // local_30 -0x30
    int clientDamage;                         // local_2c -0x2c
    int clientDuration;                       // local_28 -0x28
    int svrDuration = 0;                      // local_68 -0x68（outA）
    int svrDamage = 0;                        // local_64[0] -0x64（outB）
    unsigned char i;                          // local_3d -0x3d
    int ret;                                  // local_30 复用
    int adjDamage;                            // local_1c -0x1c
    CParty* party;                            // local_20 -0x20

    (void)srcLen; (void)uncompreRet;

    // +0x0c：读条目数
    if (!packet.get_byte(count))
        return LineFunc(0x6b36,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // +0x59：4 个 500 上限初始化（ORIG 局部槽位）；两缓冲清零
    memset(src, 0, 500);
    memset(dest, 0, 500);
    data = dest;

    // +0x134：逐条目（byte 计数，setb 无符号比较）
    for (i = 0; i < count; i = (unsigned char)(i + 1))
    {
        int compressedSize = 0;               // local_64[0] 复用槽（get_int 目标）
        // +0x13d：读压缩长度
        if (!packet.get_int(compressedSize))
            return LineFunc(0x6b4a,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x186：> 500 → 拒绝
        if (500 < compressedSize)
            return LineFunc(0x6b4d,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x1b9：读压缩数据
        if (!packet.get_binary(src, compressedSize))
            return LineFunc(0x6b50,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // +0x205：zlib 解压
        destLen = 500;
        ret = uncompress((unsigned char*)dest, &destLen,
                         (unsigned char*)src, (unsigned long)compressedSize);
        if (ret != 0)
            return LineFunc(0x6b59,
                            __PRETTY_FUNCTION__,
                            0, 0);

        // +0x263：客户端上报值
        clientDamage = *(int*)(data + 3);
        clientDuration = *(int*)(data + 7);
        svrDuration = 0;
        svrDamage = 0;
        SDCActiveStatusSyncServer syncServer;                         // ctor 0823487c
        syncServer.getActiveStatusDamageDurationForSDC(
            (char)data[1], (int)(unsigned int)(unsigned char)data[2],
            &svrDuration, &svrDamage,
            (STSDCActiveStatusSyncDataSend*)data, 0);
        // +0x2d9：不匹配 → 黑客计数 + trace
        if (svrDamage != clientDamage || svrDuration != clientDuration)
        {
            user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x25d, 1, 0, 0);
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x6b6d, 0);
                trace("[TYPE : %d][EVENT : %d] [Svr] Damage : %d, Duration : %d, [Client] Damage : %d, Duration : %d",
                      (int)(char)data[1], (int)(char)data[0],
                      svrDamage, svrDuration, clientDamage, clientDuration);
            }
        }
        // +0x3a0：队伍战场伤害记录
        party = G_CGameManager()->GetParty((int)user->GetPartyIndex());
        if (party != 0)
        {
            if (data[1] == 0xb)
            {
                // +0x3de：流血事件 → 伤害按 ORIG 全局 BLEEDING_DAMAGE_IN_ACTION_RATE 缩放
                float scaled = BLEEDING_DAMAGE_IN_ACTION_RATE * (float)clientDamage;
                adjDamage = (int)(scaled + (scaled >= 0.0f ? 0.5f : -0.5f));   // ROUND（fistpl 舍入）
            }
            else
            {
                adjDamage = clientDamage;
            }
            CBattle_Field* battleField = (CBattle_Field*)((char*)party + 0xb24);
            battleField->SetDamagedPacketCnt(*(int*)(data + 0x25));
            battleField->SetDamageValue(*(int*)(data + 0x25), adjDamage);
        }
        // syncServer 析构（08234890）
    }
    return 0;
}
int Dispatcher_SDC_ActiveStatus_Damage_Check::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SDC_ActiveStatus_Damage_Check::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_police_queryCharacName_useMid::Dispatcher_police_queryCharacName_useMid() {}
Dispatcher_police_queryCharacName_useMid::~Dispatcher_police_queryCharacName_useMid() {}
int Dispatcher_police_queryCharacName_useMid::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2 || user->getCurCharacR() == 0
        || !user->isGMUser())
    {
        return 0;
    }
    Pakcet_FindUserName_use_Charac_no request;
    // ORIG 布局（asm 栈偏移-基址）：sizeof 0x34；+0x0a GetUID（-0x42）、
    // +0x0e get_int 目标（-0x3e）、+0x16 角色名区 0x1e（-0x36..-0x19）。
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&request) + 0x0a) =
        user->GetUID();
    unsigned int characNo = 0;
    packet.get_int(characNo);   // ORIG 未检查返回值
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&request) + 0x0e) = characNo;
    GameWorld* world = G_GameWorld();
    CUser* target = world->find_from_world((unsigned short)characNo);
    if (target == 0)
    {
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendTcpPacket(reinterpret_cast<char*>(&request), 0x34);
        return 0;
    }
    const char* characName = target->getCurCharacName();
    size_t nameLen = strlen(characName);
    if (nameLen > 0x1d)
    {
        nameLen = 0;
    }
    if (nameLen != 0)
    {
        strncpy(reinterpret_cast<char*>(&request) + 0x16, characName, nameLen);
        PacketGuard response;
        response.put_header(1, 0x14b);
        response.put_int((int)characNo);
        response.put_int((int)nameLen);
        response.put_str(reinterpret_cast<char*>(&request) + 0x16, (int)nameLen);
        response.finalize(true);
        user->Send(response);
    }
    return 0;
}
int Dispatcher_police_queryCharacName_useMid::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_police_queryCharacName_useMid::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ImageCommunicationEquipmentUse::Dispatcher_ImageCommunicationEquipmentUse() {}
Dispatcher_ImageCommunicationEquipmentUse::~Dispatcher_ImageCommunicationEquipmentUse() {}
// ORIG 0x081e22e6（报告 Dispatcher_ImageCommunicationEquipmentUse/send.md）[随行依赖]。
void Dispatcher_ImageCommunicationEquipmentUse::send(CUser* user, ParamBase& param)
{
    stImageCommunicationEquipmentUse& out =
        static_cast<stImageCommunicationEquipmentUse&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x1dc);
    if (out.m_4 == 0)
    {
        guard.put_byte(1);
        guard.put_int(out.m_8);
        guard.put_int(out.m_c);
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(out.m_4);
    }
    guard.finalize(true);
    user->Send(guard);
}

// ORIG 0x081e2280（报告 Dispatcher_ImageCommunicationEquipmentUse/check_error.md）：
// !user → -1；state<=2 → -2；getCurCharacMoney < GetImageCommnicationData[0] → 10。
int Dispatcher_ImageCommunicationEquipmentUse::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -2;
    }
    const int price = *(int*)G_CDataManager()->GetImageCommnicationData();
    if (user->getCurCharacMoney() < price)
    {
        return 10;
    }
    return 0;
}

// ORIG 0x081e21d6（报告 Dispatcher_ImageCommunicationEquipmentUse/process.md）：
// param+4=check_error；==0 时 use_money(price,0x30,true)、param+8=price、
// param+0xc=getCurCharacMoney。
int Dispatcher_ImageCommunicationEquipmentUse::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stImageCommunicationEquipmentUse& out =
        static_cast<stImageCommunicationEquipmentUse&>(param);
    const int err = check_error(user, msg);
    out.m_4 = err;
    if (err == 0)
    {
        const int price = *(int*)G_CDataManager()->GetImageCommnicationData();
        user->getCurCharacInvenW()->use_money(price, (eMoneySubReason)0x30, true);
        out.m_8 = *(int*)G_CDataManager()->GetImageCommnicationData();
        out.m_c = user->getCurCharacMoney();
    }
    return 0;
}

Dispatcher_Security_NonClient_GetResponse::Dispatcher_Security_NonClient_GetResponse() {}
Dispatcher_Security_NonClient_GetResponse::~Dispatcher_Security_NonClient_GetResponse() {}
int Dispatcher_Security_NonClient_GetResponse::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_NonClient_GetResponse(user, packet);
    if (result == 1)
    {
        return LineFunc(0xdf88,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (result == 2)
    {
        return LineFunc(0xdf8a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_NonClient_GetResponse::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_NonClient_GetResponse::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_PI_HashModulation_C2C::Dispatcher_Security_PI_HashModulation_C2C() {}
Dispatcher_Security_PI_HashModulation_C2C::~Dispatcher_Security_PI_HashModulation_C2C() {}
int Dispatcher_Security_PI_HashModulation_C2C::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_PI_2ND_RecvPacketIntegrityModulation_C2C(user, packet);
    if (result == 1)
    {
        return LineFunc(0xe32f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (result == 2)
    {
        return LineFunc(0xe331,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_PI_HashModulation_C2C::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_PI_HashModulation_C2C::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_PI_HashModulation_S2C::Dispatcher_Security_PI_HashModulation_S2C() {}
Dispatcher_Security_PI_HashModulation_S2C::~Dispatcher_Security_PI_HashModulation_S2C() {}
int Dispatcher_Security_PI_HashModulation_S2C::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_PI_2ND_RecvPacketIntegrityModulation_S2C(user, packet);
    if (result == 2)
    {
        return LineFunc(0xe320,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (result == 3)
    {
        return LineFunc(0xe322,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (result == 1)
    {
        return LineFunc(0xe31e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_PI_HashModulation_S2C::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_PI_HashModulation_S2C::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Compound_Equipment_Upgrade_Card::Dispatcher_Compound_Equipment_Upgrade_Card() {}
Dispatcher_Compound_Equipment_Upgrade_Card::~Dispatcher_Compound_Equipment_Upgrade_Card() {}


Dispatcher_GrowthWeaponEventChangeInfinity::Dispatcher_GrowthWeaponEventChangeInfinity() {}
Dispatcher_GrowthWeaponEventChangeInfinity::~Dispatcher_GrowthWeaponEventChangeInfinity() {}
int Dispatcher_GrowthWeaponEventChangeInfinity::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    MSG_GROWTH_EQUIPMENT& req = static_cast<MSG_GROWTH_EQUIPMENT&>(msg);
    if (user->CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)req.m_0d), req.m_11))
    {
        return 0xd5;
    }
    unsigned char* eventData = static_cast<unsigned char*>(user->getGrowthWeaponEventdata());
    if (*reinterpret_cast<int*>(eventData + 0x04) != user->get_charac_no(-1))
    {
        return 3;
    }
    if (*reinterpret_cast<int*>(eventData + 0x0c) != 0)
    {
        return 3;
    }
    int serverGroup;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup = (int)user->GetServerGroup();
    }
    else
    {
        // ORIG *(int*)(G_CEnvironment()+0x378)（SPremiumInfo.h:33 同址证据：目标服务器代码）
        serverGroup = *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    }
    if ((int)*reinterpret_cast<short*>(eventData + 0x12) != serverGroup)
    {
        return 3;
    }
    if (*reinterpret_cast<short*>(eventData + 0x10)
        < (short)G_CDataManager()->get_GrowthWeaponMaterialMax())
    {
        return 0x11;
    }
    CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0x64);
    // ORIG 无判空直接虚调 vtable+0x34（slot IsEventing）传 0。
    if (repeatEvent->IsEventing(0))
    {
        return 0;
    }
    return 1;
}
int Dispatcher_GrowthWeaponEventChangeInfinity::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stGrowthEquipment& out = static_cast<stGrowthEquipment&>(param);
    MSG_GROWTH_EQUIPMENT& req = static_cast<MSG_GROWTH_EQUIPMENT&>(msg);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x640f,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    // ORIG _ZNK10CInventory11GetInvenRefEii（const 视图）。
    const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)req.m_0d), req.m_11);
    if (item == 0)
    {
        out.m_4 = 0x15;
        return 0;
    }
    if (item->m_addInfo == 0)   // ORIG mov 0x2(%eax),%eax
    {
        out.m_4 = 0x15;
        return 0;
    }
    CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0x64);
    // ORIG 无判空直接虚调 vtable+0x34（slot IsEventing）传 0。
    if (repeatEvent->IsEventing(0))
    {
        CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x64);
        if (event != 0)
        {
            out.m_4 = static_cast<GrowthEquipEvent*>(event)->SendInfinityGrowthWeapon(
                user, item, req.m_0d, req.m_11);
            return 0;
        }
        return 0;
    }
    return 0;
}
int Dispatcher_GrowthWeaponEventChangeInfinity::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 int(msg+0xd)，失败记 LineFunc(0x63fc)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(p + 0xd)))
    {
        return LineFunc(0x63fc, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x T：读 int(msg+0x11)，失败记 LineFunc(0x63ff)
    if (!packet.get_int(*reinterpret_cast<int*>(p + 0x11)))
    {
        return LineFunc(0x63ff, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_GrowthWeaponEventChangeInfinity::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e80be T：err=*(int*)((char*)&param+4)；0→SendCmdOkPacket(0x1f9)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1f9);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1f9, (unsigned char)err);
}

Dispatcher_SaveGameOption_ChattingEmoticon::Dispatcher_SaveGameOption_ChattingEmoticon() {}
Dispatcher_SaveGameOption_ChattingEmoticon::~Dispatcher_SaveGameOption_ChattingEmoticon() {}
int Dispatcher_SaveGameOption_ChattingEmoticon::process(CUser* user, MSG_BASE& msgBase, ParamBase& param)
{
    (void)param;
    // ORIG 0x081cf52a：把 msg 的表情槽字符串逐条 strncpy 进 CChattingEmoticonConfig，
    // 与当前 GameOption 不同才 SetEmoticonInfo 并组 SIG_SAVE_CHATTING_EMOTICON 入 DB_Q。
    MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON& msg =
        *reinterpret_cast<MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON*>(&msgBase);
    CGameOption* gameOption = user->GetGameOptionRef();
    CChattingEmoticonConfig config = gameOption->getEmoticonInfo();
    for (unsigned int i = 0; i < msg.m_slots.size(); ++i)
    {
        MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String& elem = msg.m_slots[i];
        const char* str = elem.m_str.c_str();
        int slot = static_cast<int>(*reinterpret_cast<short*>(&elem));
        strncpy(reinterpret_cast<char*>(&config) + slot * 0x16, str, 0x14);
    }
    if (!gameOption->isEqualEmoticon(config))
    {
        gameOption->SetEmoticonInfo(config);
        Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x2959);
        CStreamGuard guard(stream, true);
        guard << 0x2d7;
        guard << static_cast<int>(user->GetUID());
        SIG_SAVE_CHATTING_EMOTICON* sig = guard->GetInBuffer<SIG_SAVE_CHATTING_EMOTICON>();
        char* buf = reinterpret_cast<char*>(sig);
        *reinterpret_cast<int*>(buf + 0) = static_cast<int>(user->get_acc_id());
        memcpy(buf + 4, reinterpret_cast<const char*>(&config), 0x210);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    }
    return 0;
}
int Dispatcher_SaveGameOption_ChattingEmoticon::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON& data =
        static_cast<MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON&>(msg);
    if (!packet.get_short(data.m_count))
    {
        return LineFunc(0x2934,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < static_cast<int>(data.m_count); ++i)
    {
        MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON::_slot_String slot;
        if (!packet.get_short(*reinterpret_cast<short*>(&slot.m_slot)))
        {
            return LineFunc(0x293a,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        int strLen = 0;
        if (!packet.get_int(strLen))
        {
            return LineFunc(0x2940,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        char strBuf[0x14];
        if (!packet.get_str(strBuf, 0x14, strLen))
        {
            return LineFunc(0x2943,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        slot.m_str = strBuf;   // ORIG std::string::operator=(asStack_18, local_2e)
        data.m_slots.push_back(slot);
    }
    return 0;
}

void Dispatcher_SaveGameOption_ChattingEmoticon::send(CUser* user, ParamBase& param)
{
    (void)param;
    // ORIG 0x31：三连调用（错误码 0x1d9 应答 + 重算 + 广播表情）。
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x1d9);
    user->ReCalcChattingEmoticon();
    user->SendChattingEmoticon();
}

Dispatcher_ComboSkillExtensionQuickSlotReset::Dispatcher_ComboSkillExtensionQuickSlotReset() {}
Dispatcher_ComboSkillExtensionQuickSlotReset::~Dispatcher_ComboSkillExtensionQuickSlotReset() {}
int Dispatcher_ComboSkillExtensionQuickSlotReset::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->get_state() < 3 || user->get_charac_job() != 9)
    {
        return -1;
    }
    const MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET& req =
        static_cast<const MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET&>(msg);
    return req.m_0d == 0 || req.m_0d == 1 ? 0 : 0x13;
}
// ORIG 0x0825f4e8（报告 Dispatcher_ComboSkillExtensionQuickSlotReset/process.md）：
// err=check_error → param.m_4；>0 返 0；<0 LineFunc(0x384)；
// ==0 pullComboSkillExtensionQuickSlot(msg+0xd) → send_skill_info。
int Dispatcher_ComboSkillExtensionQuickSlotReset::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stComboSkillExtensionQuickSlotReset& out =
        static_cast<stComboSkillExtensionQuickSlotReset&>(param);
    MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET& req =
        static_cast<MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET&>(msg);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x384,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }
    SkillSlot* skillSlot = (SkillSlot*)user->getCurCharacSkillW();
    skillSlot->pullComboSkillExtensionQuickSlot((ENUM_SKILL_TREE_KIND)(char)req.m_0d);
    user->send_skill_info();
    return 0;
}
int Dispatcher_ComboSkillExtensionQuickSlotReset::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 char(msg+0xd)，失败记 LineFunc(0x372)
    char* p = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(p + 0xd)))
    {
        return LineFunc(0x372, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void Dispatcher_ComboSkillExtensionQuickSlotReset::send(CUser* user, ParamBase& param)
{
    // ORIG 0x0825f60a T：err=*(int*)((char*)&param+4)；0→SendCmdOkPacket(0x1ff)，
    // 否则 SendCmdErrorPacket(同码, (unsigned char)err)。
    int err = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    if (err == 0)
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1ff);
    else
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1ff, (unsigned char)err);
}

Dispatcher_Select_3rdChronicleItem_ForEnchant::Dispatcher_Select_3rdChronicleItem_ForEnchant() {}
Dispatcher_Select_3rdChronicleItem_ForEnchant::~Dispatcher_Select_3rdChronicleItem_ForEnchant() {}
int Dispatcher_Select_3rdChronicleItem_ForEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Select_3rdChronicleItem_ForEnchant::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GrowthCreatureChangeInfinityCreature::Dispatcher_GrowthCreatureChangeInfinityCreature() {}
Dispatcher_GrowthCreatureChangeInfinityCreature::~Dispatcher_GrowthCreatureChangeInfinityCreature() {}
// ORIG 0x081e86fc（报告 check_error_event.md）[随行依赖]：
// 经 GiveGrowCreatureEvent vtable+0x34 槽（空实现）后 test %al 残值，
// 等价事件指针非零弱检查 [推断]。
int Dispatcher_GrowthCreatureChangeInfinityCreature::check_error_event(CUser* user)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x67);
    ev->deliveryEnd(0);                  // 0x081e8729 虚槽 +0x34（空）
    ev = GlobalData::s_event_manager->GetRepeatEvent(0x67);
    if (ev != 0)
    {
        return static_cast<GiveGrowCreatureEvent*>(ev)->CheckEventInfo(*user);
    }
    return 1;
}

// ORIG 0x081e863a（报告 process_event.md）[随行依赖]，结构同上。
int Dispatcher_GrowthCreatureChangeInfinityCreature::process_event(CUser* user)
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x67);
    ev->deliveryEnd(0);                  // 0x081e8667 虚槽 +0x34（空）
    ev = GlobalData::s_event_manager->GetRepeatEvent(0x67);
    if (ev != 0)
    {
        return static_cast<GiveGrowCreatureEvent*>(ev)->processEvent(*user);
    }
    return 0;
}

// ORIG 0x081e86a6（报告 Dispatcher_GrowthCreatureChangeInfinityCreature/check_error.md）
int Dispatcher_GrowthCreatureChangeInfinityCreature::check_error(CUser* user,
                                                                 MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    int err = check_error_event(user);
    if (err != 0)
    {
        return err;
    }
    return 0;
}

// ORIG 0x081e85ac（报告 Dispatcher_GrowthCreatureChangeInfinityCreature/process.md）：
// ParamBase 实际为 stGrowthCreature（m_4）。
int Dispatcher_GrowthCreatureChangeInfinityCreature::process(CUser* user,
                                                             MSG_BASE& msg,
                                                             ParamBase& param)
{
    int err = check_error(user, msg);
    static_cast<stGrowthCreature&>(param).m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x64e2,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    static_cast<stGrowthCreature&>(param).m_4 = process_event(user);
    return 0;
}
int Dispatcher_GrowthCreatureChangeInfinityCreature::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GrowthCreatureChangeInfinityCreature::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e8768：p4==0 → Ok(0x200)+事件(0x67) IsEventing(+0x34)→SendGrowthCreatureEventInfo；否则 Error
    stGrowthCreature& p = static_cast<stGrowthCreature&>(param);
    if (p.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x200);
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x67);
        if (ev != 0 && ev->IsEventing((CUser*)0))
        {
            CEventBase* ev2 = GlobalData::s_event_manager->GetRepeatEvent(0x67);
            if (ev2 != 0)
            {
                static_cast<GiveGrowCreatureEvent*>(ev2)->SendGrowthCreatureEventInfo(*user);
            }
        }
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x200,
                                 (unsigned char)(p.m_4 & 0xff));
    }
}

Dispatcher_P2PHolePunchingSuccessRateStatistics::Dispatcher_P2PHolePunchingSuccessRateStatistics() {}
Dispatcher_P2PHolePunchingSuccessRateStatistics::~Dispatcher_P2PHolePunchingSuccessRateStatistics() {}
int Dispatcher_P2PHolePunchingSuccessRateStatistics::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_P2PHolePunchingSuccessRateStatistics::process(CUser* user, MSG_BASE& msg,
                                                             ParamBase& param)
{
    (void)param;
    // ORIG 0x0825ffb6：Acquire("PacketDispatcher_Impl_2.cpp",0xb4c) → internal_stream(0xf,uid)
    // → GetInBuffer<MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE> ← memcpy(&msg,0x16) → QUEUE 1
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_2.cpp", 0xb4c), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0xf, (int)user->GetUID());
    MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE* in =
        guard->GetInBuffer<MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE>();
    memcpy(in, &msg, 0x16);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_P2PHolePunchingSuccessRateStatistics::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x0825feda：byte@0xd → uint@0xe → uint@0x12
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(m + 0xd)))
        return LineFunc(0xb3d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(m + 0xe)))
        return LineFunc(0xb40, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(m + 0x12)))
        return LineFunc(0xb43, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
void Dispatcher_P2PHolePunchingSuccessRateStatistics::send(CUser* user, ParamBase& param) {}

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
Dispatcher_AvartarRoulette::Dispatcher_AvartarRoulette() {}
Dispatcher_AvartarRoulette::~Dispatcher_AvartarRoulette() {}
int Dispatcher_AvartarRoulette::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x312;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() < 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 7);
        return 0;
    }
    arad::SigAvatarRoulette sig;      // ORIG 栈 -0xb，C1 清 3 字节
    if (packet.get_byte(sig.m_type) != 1)
    {
        return 0x32f;
    }
    if (packet.get_byte(sig.m_slotIdx) != 1)
    {
        return 0x331;
    }
    if (sig.m_type > 0xa)
    {
        return 0x334;
    }
    if (sig.m_slotIdx > 9)
    {
        return 0x337;
    }
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x347), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2c, (int)user->GetUID());
    guard->put_binary((void*)&sig, 3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AvartarRoulette::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AvartarRoulette::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AvartarCoinCount（ORIG 0x819e660 区）====================
// 当前仅 checkWorkState/dispatch_sig 桩（工作列表不含）；保留签名，补最小骨架。
Dispatcher_AvartarCoinCount::Dispatcher_AvartarCoinCount() {}
Dispatcher_AvartarCoinCount::~Dispatcher_AvartarCoinCount() {}
int Dispatcher_AvartarCoinCount::checkWorkState(CUser* user)
{
    // [推断] 未建模服务检查，暂按通过。
    return 1;
}
int Dispatcher_AvartarCoinCount::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x819e660：AvatarCoin::SendSyncPacket(user)；成功 → 0，失败 → 0x353。
    (void)packet;
    return AvatarCoin::SendSyncPacket(user) ? 0 : 0x353;
}
int Dispatcher_AvartarCoinCount::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AvartarCoinCount::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AradAvatarConvert（ORIG 0x81a00be）====================
// dispatch_sig：user==0→0x74a；getCurCharacR==0→0；state∉[3,4]→0；PVP→
//   SendCmdErrorPacket((ENUM_CMDPACKET)0x24f,7)；CheckInTrade→同上；isEnableAvatarSocketAction
//   （未建模，推断 0）；读 short+short+int；发内部流 0x33（0xe 字节）。
Dispatcher_AradAvatarConvert::Dispatcher_AradAvatarConvert() {}
Dispatcher_AradAvatarConvert::~Dispatcher_AradAvatarConvert() {}
int Dispatcher_AradAvatarConvert::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x74a;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() <= 2 || user->get_state() > 4)
    {
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 7);
        return 0;
    }
    const int avatarSocketErr = user->isEnableAvatarSocketAction();
    if (avatarSocketErr != 0)
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x24f,
            (unsigned char)(avatarSocketErr & 0xff));
        return 0;
    }
    arad::SigAradAvatarConvert sig;      // ORIG 栈 -0x1a，C1 清 0x10
    if (packet.get_short(sig.m_selectNo) != 1)
    {
        return 0x76d;
    }
    if (packet.get_short(*reinterpret_cast<unsigned short*>(&sig.m_slot)) != 1)
    {
        return 0x771;
    }
    if (packet.get_int(*reinterpret_cast<unsigned long*>(&sig.m_itemId)) != 1)
    {
        return 0x775;
    }
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x780), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x33, (int)user->GetUID());
    guard->put_binary((void*)&sig, 0xe);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AradAvatarConvert::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AradAvatarConvert::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AradEmblemCompound（ORIG 0x819fd7e）====================
// dispatch_sig：user==0→0x6f1；getCurCharacR==0→0；state∉[3,4]→
//   SendCmdErrorPacket((ENUM_CMDPACKET)0x24e,7)；PVP/FreePvP→同上；CheckInTrade→同上；
//   isEnableAvatarSocketAction（推断 0）；读 3 个 short；发内部流 0x32（0x16 字节）。
Dispatcher_AradEmblemCompound::Dispatcher_AradEmblemCompound() {}
Dispatcher_AradEmblemCompound::~Dispatcher_AradEmblemCompound() {}
int Dispatcher_AradEmblemCompound::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x6f1;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() <= 2 || user->get_state() > 4)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 7);
        return 0;
    }
    const int avatarSocketErr = user->isEnableAvatarSocketAction();
    if (avatarSocketErr != 0)
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x24e,
            (unsigned char)(avatarSocketErr & 0xff));
        return 0;
    }
    arad::SigAradEmblemCompound sig;     // ORIG 栈 -0x22，C1 清 0x18
    if (packet.get_short(sig.m_slot0) != 1)
    {
        return 0x713;
    }
    if (packet.get_short(sig.m_slot1) != 1)
    {
        return 0x717;
    }
    if (packet.get_short(sig.m_selectIdx) != 1)
    {
        return 0x71b;
    }
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x723), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x32, (int)user->GetUID());
    guard->put_binary((void*)&sig, 0x16);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AradEmblemCompound::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AradEmblemCompound::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AradJumpingCharacter（ORIG 0x819f1ec）====================
// dispatch_sig：user==0→0x59b；getCurCharacR==0→0；读 byte+byte+short+int
//   （错误码 0x5a9/0x5ac/0x5af/0x5b1/0x5b4/0x5b6）；state∉[3,4]→0；
//   通过后发内部流 0x2d（0x11 字节，含 acc_id/charac_no）。
Dispatcher_AradJumpingCharacter::Dispatcher_AradJumpingCharacter() {}
Dispatcher_AradJumpingCharacter::~Dispatcher_AradJumpingCharacter() {}
int Dispatcher_AradJumpingCharacter::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x59b;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    // ORIG 0x0819f1e0：SigAradJumpingCharacter sig（C1 0x081a0990 仅清 m_head[0..8]）；
    // 布局 +0x01 acc_id、+0x05 charac_no、+0x09 eventKind、+0x0a growType、
    // +0x0b itemIndex、+0x0d tail(int)；put_binary(&sig, 0x11)。
    arad::SigAradJumpingCharacter sig;
    if (packet.get_byte(reinterpret_cast<unsigned char&>(sig.m_eventKind)) != 1)   // +0x09
    {
        return 0x5a9;
    }
    // ORIG：b1==0 放行（test/je 早跳），仅无符号 >2 报错。
    if ((unsigned char)sig.m_eventKind != 0 && (unsigned char)sig.m_eventKind > 2)
    {
        return 0x5ac;
    }
    if (packet.get_byte(sig.m_growType) != 1)   // +0x0a
    {
        return 0x5af;
    }
    // ORIG：b2==0 放行，仅无符号 >4 报错。
    if (sig.m_growType != 0 && sig.m_growType > 4)
    {
        return 0x5b1;
    }
    if (packet.get_short(sig.m_itemIndex) != 1)   // +0x0b
    {
        return 0x5b4;
    }
    if (packet.get_int(reinterpret_cast<unsigned int&>(sig.m_tail[0])) != 1)   // +0x0d
    {
        return 0x5b6;
    }
    // ORIG 0x0819f2fd：两次 get_state() 调用（未缓存），OR 链 bool 形态。
    if (user->get_state() < 3 || user->get_state() > 4)
    {
        return 0;
    }
    // ORIG 0x0819f44c checkWorkState：失败（返回 false）→ 0。
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    *(unsigned int*)&sig.m_head[1] = (unsigned int)user->get_acc_id();      // +0x01
    *(unsigned int*)&sig.m_head[5] = (unsigned int)user->get_charac_no(-1); // +0x05
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x5c8);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2d, (int)user->GetUID());
    guard.put_binary((void*)&sig, 0x11);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AradJumpingCharacter::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AradJumpingCharacter::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AradUseAvatarRecharge（ORIG 0x819fa56）====================
// dispatch_sig：user==0→0x69c；getCurCharacR==0→0；state∉[3,4]→
//   SendCmdErrorPacket((ENUM_CMDPACKET)0x24d,7)；PVP/FreePvP→同上；CheckInTrade→同上；
//   读 short+short；发内部流 0x30（0x10 字节）。
Dispatcher_AradUseAvatarRecharge::Dispatcher_AradUseAvatarRecharge() {}
Dispatcher_AradUseAvatarRecharge::~Dispatcher_AradUseAvatarRecharge() {}
int Dispatcher_AradUseAvatarRecharge::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x69c;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() <= 2 || user->get_state() > 4)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 7);
        return 0;
    }
    arad::SigAradUseAvatarRecharge sig;  // ORIG 栈 -0x18，C1 清 0x10
    // ORIG 读序：先 sig+0x02（stoneSlot），后 sig+0x00（avatarSlot）。
    if (packet.get_short(sig.stoneSlot) != 1)
    {
        return 0x6b6;
    }
    if (packet.get_short(sig.avatarSlot) != 1)
    {
        return 0x6ba;
    }
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x6c2), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x30, (int)user->GetUID());
    guard->put_binary((void*)&sig, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AradUseAvatarRecharge::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AradUseAvatarRecharge::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_EventCreateDnfRequest（ORIG 0x81a0362）====================
// dispatch_sig：packet==0→0x7b3；state∉[3,4]→0；CEventCreateDnf 事件（GetRepeatEvent
//   /GetRate/IsRewardCondition/SendReward，未建模）标注推断：发 0x22d 响应包
//   （rate 0 + flag 0）并返回。
Dispatcher_EventCreateDnfRequest::Dispatcher_EventCreateDnfRequest() {}
Dispatcher_EventCreateDnfRequest::~Dispatcher_EventCreateDnfRequest() {}
int Dispatcher_EventCreateDnfRequest::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x7b3;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    int st = user->get_state();
    if (st < 3 || st > 4)
    {
        return 0;
    }
    // [推断] ORIG 经 CEventManager::GetRepeatEvent(0xa0) 取 CEventCreateDnf 并
    // 判定奖励条件/发放奖励（CEventCreateDnf 未建模）。此处仅发送 0x22d 响应
    // 骨架：put_header(0,0x22d) + put_short(rate) + put_byte(0)。
    PacketGuard guard;
    guard.put_header(0, 0x22d);
    guard.put_short(0);  // [推断] CEventCreateDnf::GetRate 未建模，置 0
    guard.put_byte(0);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
int Dispatcher_EventCreateDnfRequest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_EventCreateDnfRequest::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AvartarHiddenOptionChange（ORIG 0x819f6f8）====================
// dispatch_sig：user==0→0x632；getCurCharacR==0→0；state∉[3,4]→
//   SendCmdErrorPacket((ENUM_CMDPACKET)0x24c,7)；PVP/FreePvP→同上；CheckInTrade→同上；
//   读 short+short；发内部流 0x2e（6 字节）。
Dispatcher_AvartarHiddenOptionChange::Dispatcher_AvartarHiddenOptionChange() {}
Dispatcher_AvartarHiddenOptionChange::~Dispatcher_AvartarHiddenOptionChange() {}
int Dispatcher_AvartarHiddenOptionChange::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x632;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() <= 2 || user->get_state() > 4)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 7);
        return 0;
    }
    arad::SigAvatarHiddenOptionChange sig;  // ORIG 栈 -0xe，C1 清 6 字节
    if (packet.get_short(sig.m_materialSlot) != 1)
    {
        return 0x64d;
    }
    if (packet.get_short(sig.m_avatarSlot) != 1)
    {
        return 0x651;
    }
    if (!checkWorkState(user, sig))
    {
        return 0;
    }
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x659);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2e, (int)user->GetUID());
    guard.put_binary((void*)&sig, 6);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AvartarHiddenOptionChange::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AvartarHiddenOptionChange::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_RequestPrcoomDayilyReward（ORIG 0x81a0612）====================
// dispatch_sig：user==0→0x7e8；getCurCharacR==0→0；state∉[3,4]→0；
//   每日奖励剩余次数（CUser+0x8ec32）<1→SendCmdErrorPacket((ENUM_CMDPACKET)0x259,0x16)；
//   读 int itemId（0x212f/0x2130 限定，错误 0x7fb/0x800）；find_item 后
//   make_item 注入背包（invenType=0xe），成功扣减次数并发
//   InterSelectPcroomDailyReward::SendPacket / DBUpdatePcroomDailyReward::makeRequest。
Dispatcher_RequestPrcoomDayilyReward::Dispatcher_RequestPrcoomDayilyReward() {}
Dispatcher_RequestPrcoomDayilyReward::~Dispatcher_RequestPrcoomDayilyReward() {}
int Dispatcher_RequestPrcoomDayilyReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x7e8;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    int st = user->get_state();
    if (st < 3 || st > 4)
    {
        return 0;
    }
    if (user->m_field8ec32 < 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x259, 0x16);
        return 0;
    }
    int itemId = 0;
    if (packet.get_int(itemId) != 1)
    {
        return 0x7fb;
    }
    if (itemId != 0x212f && itemId != 0x2130)
    {
        return 0x800;
    }
    CItem* item = G_CDataManager()->find_item(itemId);
    if (item == 0 || item->GetItemType() != 0)
    {
        return 0;
    }
    Inven_Item inven;
    item->make_item(inven);
    inven.set_add_info(1);
    CInventory* inv = user->getCurCharacInvenW();
    int slot = inv->insertItemIntoInventory(
        inven, (eItemAddReason)0xe, true, true);
    if (slot < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x259, 4);
    }
    else
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x259);
        user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, slot);
        user->m_field8ec32 = (char)(user->m_field8ec32 - 1);
        InterSelectPcroomDailyReward::SendPacket(user);
        DBUpdatePcroomDailyReward req;
        req.makeRequest(user);
    }
    return 0;
}
int Dispatcher_RequestPrcoomDayilyReward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_RequestPrcoomDayilyReward::read(PacketBuf& packet, MSG_BASE& msg)
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
Dispatcher_ClearAction::Dispatcher_ClearAction() {}
Dispatcher_ClearAction::~Dispatcher_ClearAction() {}
int Dispatcher_ClearAction::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0812513c：state>2 且 curCharacR 非空才受理（否则 0x278）；
    // get_binary 0xb 字节 → ClearActionAndSendtoUser(group@0, value@4, packetClass 1)
    bool bad = true;
    if (user != 0)
    {
        if (user->get_state() > 2 && user->getCurCharacR() != 0)
            bad = false;
    }
    if (bad)
        return 0x278;
    _MSG_ACTION_POINT_ACTION_CLEAR msg;
    if (!packet.get_binary(reinterpret_cast<char*>(&msg), 0xb))
        return 0x27c;
    CUserProc::ClearActionAndSendtoUser(user, (_ActionGroupIndex)msg.m_group,
                                        msg.m_field4, (ENUM_PACKETCLASS)1);
    return 0;
}
int Dispatcher_ClearAction::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ClearAction::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GetTodayReward::Dispatcher_GetTodayReward() {}
Dispatcher_GetTodayReward::~Dispatcher_GetTodayReward() {}
int Dispatcher_GetTodayReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081251da：state>2 且 curCharacR 非空才受理（否则 0x287）；
    // get_binary 5 字节 → GetTodayRewardAndSendPostal(reward=int@+1)
    bool bad = true;
    if (user != 0)
    {
        if (user->get_state() > 2 && user->getCurCharacR() != 0)
            bad = false;
    }
    if (bad)
        return 0x287;
    unsigned char buf[5];
    if (!packet.get_binary(reinterpret_cast<char*>(buf), 5))
        return 0x28b;
    CUserProc::GetTodayRewardAndSendPostal(
        user, *reinterpret_cast<_SIG_LOAD_ACTION_POINT::_TodayRewardItem*>(buf + 1));
    return 0;
}
int Dispatcher_GetTodayReward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GetTodayReward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

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
int Dispatcher_AvatarOptionChange::read(PacketBuf& packet, MSG_BASE& msg)
{
    AocMsgView& m = *reinterpret_cast<AocMsgView*>(&msg);
    if (!packet.get_short(m.m_consumeSlot))
        return LineFunc(0x5380,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (m.m_consumeSlot < 0 || m.m_consumeSlot > 0x137)
        return LineFunc(0x5383,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(m.m_consumeItemIdx))
        return LineFunc(0x5386,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (m.m_consumeItemIdx < 0)
        return LineFunc(0x5389,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(m.m_avatarSlot))
        return LineFunc(0x538c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (m.m_avatarSlot < 0 || m.m_avatarSlot > 0x68)
        return LineFunc(0x5390,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(m.m_equipItemIdx))
        return LineFunc(0x5393,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (m.m_equipItemIdx < 0)
        return LineFunc(0x5396,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(m.m_option))
        return LineFunc(0x5399,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (((unsigned char)m.m_option & 0x80) != 0)
        return LineFunc(0x539c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    return 0;
}

// ==== Dispatcher_AvatarOptionChange::check_error (ORIG 0x081e0a40) ====
// 校验链：user/状态门 → Secu 保护域(0x2a) → 消耗品槽一致性/数量 → 消耗品为可堆叠的
// avatar option change 道具 → 目标槽物品一致性 → 目标为非堆叠 avatar 装备 → 锁定检查
// → job/type/grade 条件或限定集合 → option 值合法性。
int Dispatcher_AvatarOptionChange::check_error(CUser* user, MSG_BASE& msg)
{
    AocMsgView& m = *reinterpret_cast<AocMsgView*>(&msg);
    if (user == 0)
        return -1;
    if (user->get_state() <= 2)
        return -1;

    int secuErr = GlobalData::s_pSecuProtectionField->Check(user,
                                                            (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
        return secuErr;

    const CInventory* invR = user->getCurCharacInvenR();
    Inven_Item consumeSlot = invR->GetInvenSlot(INVEN_TYPE_EQUIP, m.m_consumeSlot);
    if (consumeSlot.m_addInfo != m.m_consumeItemIdx)
        return 0x11;
    if (consumeSlot.m_addInfo2 <= 0)
        return 0x16;

    CStackableItem* consume =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(consumeSlot.m_addInfo));
    if (consume == 0)
        return 0x11;
    if (!consume->is_stackable())
        return 0x11;
    if (!consume->IsAvatarOptionChangeItem())
        return 0x11;

    Inven_Item avatarSlot = invR->GetInvenSlot(INVEN_TYPE_AVATAR, m.m_avatarSlot);
    if (avatarSlot.m_addInfo != m.m_equipItemIdx)
        return 0x11;

    CItem* equip = G_CDataManager()->find_item(m.m_equipItemIdx);
    if (equip == 0)
        return 0x11;
    if (equip->is_stackable())  // 目标必须是非堆叠装备（ORIG 判断取反）
        return 0x11;
    if (!equip->IsAvatarItem())  // ORIG vtable+0x10 无参虚调用（IsAvatarItem 槽）
        return 0x11;
    if (user->CheckItemLock(INVEN_TYPE_AVATAR, m.m_avatarSlot))
        return 0xd5;

    // 消耗品尾部三字节选项条件区：+0x365 job、+0x366 type、+0x367 grade
    // （AvatarOptionChangeType 3 字节视图：低 ushort = job/type，高字节 = grade）
    const unsigned short tail365 =
        *reinterpret_cast<const unsigned short*>(&consume->m_avatar365.m_field0);
    const signed char optJob = (signed char)(tail365 & 0xff);
    const signed char optType = (signed char)((tail365 >> 8) & 0xff);
    const signed char optGrade = (signed char)consume->m_avatar365.m_field2;

    if (consume->m_set368.empty())
    {
        if ((equip->check_job_type((char)optJob) != 1) && optJob != 0xb)
            return 0xfe;
        if (equip->GetItemType() != (int)optType && optType != 0x1a)
            return 0xfd;
        if (equip->get_grade() != (int)optGrade && optGrade != 0)
            return 0xfc;
    }
    else
    {
        // ORIG 调 const set::find（_ZNKSt3setIiSt4lessIiESaIiEE4findERKi）→ 经 const 引用。
        const std::set<int>& set368 = consume->m_set368;
        if (set368.find(avatarSlot.m_addInfo) == set368.end())
        {
            return 0xff;
        }
    }

    // ORIG m_set380 同样走 const find。
    const std::set<int>& set380 = consume->m_set380;
    if (!set380.empty() &&
        set380.find((int)(signed char)m.m_option) != set380.end())
    {
        return 0xfb;
    }

    if (!static_cast<const CEquipItem*>(equip)->IsAvatarOptionValid(m.m_option))
        return 0xfa;

    return 0;
}

// ==== Dispatcher_AvatarOptionChange::process (ORIG 0x081e0774) ====
// 影子背包预演删除消耗品成功后，才落到真实背包；改写目标 avatar 装备的 option 字节并回包。
int Dispatcher_AvatarOptionChange::process(CUser* user, MSG_BASE& msg, ParamBase& paramBase)
{
    AocMsgView& m = *reinterpret_cast<AocMsgView*>(&msg);
    AocParamView& param = *reinterpret_cast<AocParamView*>(&paramBase);

    int err = check_error(user, msg);
    param.m_errorCode = err;
    if (param.m_errorCode > 0)
        return 0;
    if (param.m_errorCode < 0)
        return LineFunc(0x53ab,
                        __PRETTY_FUNCTION__,
                        param.m_errorCode, 0);

    CInventory* invW = user->getCurCharacInvenW();

    // ORIG 在栈上构造影子背包（CInventory + InventoryMemory 0xc140）做删除预演。
    CInventory shadowInv;
    InventoryMemory shadowMem;
    shadowInv.SetInventoryMemory(&shadowMem);
    // ORIG 0x081e083d：setCopy 参数经 getCurCharacInvenRefR 取得（与 R 同址，别名视图）。
    shadowInv.setCopy(*reinterpret_cast<const CInventory*>(user->getCurCharacInvenRefR()));

    if (shadowInv.delete_item(INVEN_TYPE_EQUIP, m.m_consumeSlot, 1,
                              (eItemDelReason)0x2f, false) != 1)
    {
        param.m_errorCode = 0x16;
        return 0;
    }
    Inven_Item* shadowRef = shadowInv.GetInvenRef(INVEN_TYPE_AVATAR, m.m_avatarSlot);
    if (shadowRef == 0)
    {
        param.m_errorCode = 0x11;
        return 0;
    }

    invW->delete_item(INVEN_TYPE_EQUIP, m.m_consumeSlot, 1, (eItemDelReason)0x2f, true);
    Inven_Item* avatarRef = invW->GetInvenRef(INVEN_TYPE_AVATAR, m.m_avatarSlot);

    // ORIG 0x081e0986：_ZN15cUserHistoryLog21UseAvatarOptionChangeEiiii（实现见
    // cUserHistoryLog.cpp:1365，CUser.h:2077 已声明）。
    user->m_historyLog.UseAvatarOptionChange(avatarRef->m_addInfo2,
        avatarRef->m_addInfo, avatarRef->m_fieldb, (int)(signed char)m.m_option);

    avatarRef->m_fieldb = (unsigned short)(signed char)m.m_option;
    invW->SendAvatarEvent(0x2c3, avatarRef->m_addInfo2,
                          (int)(signed char)m.m_option, 0, 0);

    param.m_consumeSlot = m.m_consumeSlot;
    param.m_avatarSlot = m.m_avatarSlot;
    param.m_option = (unsigned char)m.m_option;
    return 0;
}

// ==== Dispatcher_AvatarOptionChange::send (ORIG 0x081e0e32) ====
void Dispatcher_AvatarOptionChange::send(CUser* user, ParamBase& paramBase)
{
    AocParamView& param = *reinterpret_cast<AocParamView*>(&paramBase);
    if (param.m_errorCode == 0)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1cc);
        packet.put_byte(1);
        packet.put_short(param.m_avatarSlot);
        packet.put_byte((int)(signed char)param.m_option);
        packet.finalize(true);
        user->Send(packet);
        user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                                 param.m_consumeSlot);
        return;
    }
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1cc, (unsigned char)param.m_errorCode);
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
int Dispatcher_BlueMarbleConfirmInfo::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    return 0;
}

// ==== Dispatcher_BlueMarbleConfirmInfo::read (ORIG 0x081e6242) ====
int Dispatcher_BlueMarbleConfirmInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BLUE_MARBLE_CONFIRM_INFO& req =
        static_cast<MSG_BLUE_MARBLE_CONFIRM_INFO&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))        // +0x0d flag
    {
        return LineFunc(24845,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_0e)))    // +0x0e index
    {
        return LineFunc(24848,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_BlueMarbleConfirmInfo::process (ORIG 0x081e62da) ====
int Dispatcher_BlueMarbleConfirmInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBlueMarbleConfirmInfo& out = static_cast<stBlueMarbleConfirmInfo&>(param);
    MSG_BLUE_MARBLE_CONFIRM_INFO& req =
        static_cast<MSG_BLUE_MARBLE_CONFIRM_INFO&>(msg);

    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(24863,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }

    // P4：1024 字节栈数组内联清零（rep stos 形态）
    char path[1024] = { '\0' };
    sprintf(path, "./log/%s/BlueMarbleConfirmInfo",
            G_CEnvironment()->get_file_name());

    short clientIdx = static_cast<short>(user->getBlueMarbleIndex());   // u16→s16
    short serverIdx = static_cast<short>(req.m_0e);                     // u16→s16
    int space = static_cast<int>(user->getMoveSpace());
    int state = user->get_state();
    int characNo = user->get_charac_no(-1);
    char* idStr = NumberToString(user->get_acc_id(), 0);

    CMyFileLog clientLog(
        "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
        0x6127);
    clientLog(path,
              "Client m_id(%s) charac_no(%u) userState(%d) userSpace(%d)"
              "client_blueMarble_Index(%d) server_bluemarble_index(%d)",
              idStr, characNo, state, space,
              static_cast<int>(clientIdx), static_cast<int>(serverIdx));

    // ORIG 此处读 scriptMgr+0x30 至未消费局部（死值），省略不影响语义。
    if (req.m_0d == 0)                                   // 用户仍在蓝弹珠内存池
    {
        if (serverIdx == clientIdx)
        {
            BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
            if (marble != 0)
            {
                CMyFileLog sameLog(
                    "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
                    0x614d);
                sameLog(path,
                        "OverLap User index same m_id(%s) charac_no(%u) userState(%d)"
                        " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                        idStr, characNo, state, space,
                        static_cast<int>(clientIdx), static_cast<int>(serverIdx));
                marble->checkBlueMarbleUserConfirm(user);
            }
        }
        else
        {
            BlueMarble* gmMarble =
                G_CGameManager()->getBlueMarble(static_cast<int>(serverIdx));
            if (gmMarble != 0)
            {
                CMyFileLog gmLog(
                    "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
                    0x6135);
                gmLog(path,
                      "OverLap User index different m_id(%s) charac_no(%u) userState(%d)"
                      " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                      idStr, characNo, state, space,
                      static_cast<int>(clientIdx), static_cast<int>(serverIdx));
                gmMarble->checkBlueMarbleUserConfirm(user);
            }
            BlueMarble* userMarble =
                static_cast<BlueMarble*>(user->getBlueMarble());
            if (userMarble != 0)
            {
                CMyFileLog userLog(
                    "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
                    0x6140);
                userLog(path,
                        "OverLap User index different m_id(%s) charac_no(%u) userState(%d)"
                        " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                        idStr, characNo, state, space,
                        static_cast<int>(clientIdx), static_cast<int>(serverIdx));
                userMarble->checkBlueMarbleUserConfirm(user);
            }
        }
    }
    else                                                 // 用户不在池：仅日志+确认
    {
        CMyFileLog noneLog(
            "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
            0x6158);
        noneLog(path,
                "Not Exist User m_id(%s) charac_no(%u) userState(%d)"
                " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                idStr, characNo, state, space,
                static_cast<int>(clientIdx), static_cast<int>(serverIdx));
        BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
        if (marble != 0)
        {
            marble->checkBlueMarbleUserConfirm(user);
        }
    }
    return 0;
}

// ==== Dispatcher_BlueMarbleConfirmInfo::send (ORIG 0x081e6904) ====
void Dispatcher_BlueMarbleConfirmInfo::send(CUser* user, ParamBase& param)
{
    stBlueMarbleConfirmInfo& out = static_cast<stBlueMarbleConfirmInfo&>(param);
    if (out.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1fc);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fc,
                                 static_cast<unsigned char>(out.m_4 & 0xff));
    }
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

inline int& mbLen(MSG_BASE& m) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&m) + 0x0d); }          // +0x0d 收件人名长
inline char* mbName(MSG_BASE& m) { return reinterpret_cast<char*>(&m) + 0x11; }                                 // +0x11 收件人名[0x1d]
inline unsigned int& mbGold(MSG_BASE& m) { return *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&m) + 0x2e); } // +0x2e 金币
inline unsigned int& mbGuildKey(MSG_BASE& m) { return *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&m) + 0x32); } // +0x32 公会邮键
inline unsigned char& mbSpace(MSG_BASE& m) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&m) + 0x36); } // +0x36 物品空间
inline unsigned short& mbSlot(MSG_BASE& m) { return *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&m) + 0x37); } // +0x37 槽位
inline unsigned long& mbItemId(MSG_BASE& m) { return *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(&m) + 0x39); } // +0x39 物品 id
inline int& mbTitleLen(MSG_BASE& m) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&m) + 0x41); }     // +0x41 标题长
inline char* mbText(MSG_BASE& m) { return reinterpret_cast<char*>(&m) + 0x45; }                                 // +0x45 文本[0x100]
inline int& mbExtra(MSG_BASE& m) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&m) + 0x3d); }      // +0x3d 附件附加字段

}  // namespace

// ==== Dispatcher_MailBox_Send::read (ORIG 0x081cc3da) ====
int Dispatcher_MailBox_Send::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cc3da：依次解析 10 个字段；失败分支各自 LineFunc 并返回其返回值。
    // 收件人名/金币/空间/槽位/物品 id/附加字段 共用 0x2315（ORIG 同一跳转目标），
    // 标题长 0x231b、文本 0x231e、公会邮键 0x2321 独立。
    // 字段经 MSG_MAILBOX_SEND 成员直访（O0 下 inline helper 不内联会多出 call，
    // 故本函数不走 mbXxx helper）。
    MSG_MAILBOX_SEND& m = reinterpret_cast<MSG_MAILBOX_SEND&>(msg);
    if (!packet.get_int(m.m_nameLenD))                                // +0x0d 收件人名长
        return LineFunc(0x2309, __PRETTY_FUNCTION__, 0, 0);

    if (!packet.get_str(m.m_name11, 0x1d, m.m_nameLenD) ||           // +0x11 收件人名
        !packet.get_int(m.m_gold2e) ||                               // +0x2e 金币
        !packet.get_byte(m.m_space36) ||                             // +0x36 物品空间
        !packet.get_short(m.m_slot37) ||                             // +0x37 槽位
        !packet.get_int(m.m_itemId39) ||                             // +0x39 物品 id
        !packet.get_int(m.m_extra3d))                                // +0x3d 附加字段
        return LineFunc(0x2315, __PRETTY_FUNCTION__, 0, 0);

    m.m_titleLen41 = 0;                                              // +0x41 标题长清零
    m.m_guildKey32 = 0;                                              // +0x32 公会邮键清零
    memset(m.m_text45, 0, 0x100);                                    // +0x45 文本区清零

    if (!packet.get_int(m.m_titleLen41))                             // +0x41 标题长
        return LineFunc(0x231b, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m.m_text45, 0x100, m.m_titleLen41))          // +0x45 邮件文本
        return LineFunc(0x231e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_guildKey32))                             // +0x32 公会邮键
        return LineFunc(0x2321, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

// ==== Dispatcher_MailBox_Send::GuildMailCheck (ORIG 0x081cc8c2) ====
int Dispatcher_MailBox_Send::GuildMailCheck(CUser* user, MSG_BASE& msg)
{
    if (user->get_charac_guildkey() == 0)   // ORIG：取角色公会键（非 msg 字段）
        return 0x22;

    if (!user->IsSubGuildMaster() && !user->IsGuildMaster())    // 仅会长/副会长可发
        return 0x24;

    const int fee = *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CDataManager()) + 0xa624);  // +0xa624 公会邮件费（推断：ORIG 全局费用字段）
    const CInventory* inv = user->getCurCharacInvenR();
    if (inv->get_money() < fee)
        return 0x23;
    return 0;
}

// ==== Dispatcher_MailBox_Send::check_error (ORIG 0x081cc636) ====
int Dispatcher_MailBox_Send::check_error(CUser* user, MSG_BASE& msg)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
        return -1;

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 4))
        return 0xd1;

    const int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x3f));
    if (secuErr != 0)
        return secuErr;

    const int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)static_cast<int>(mbSpace(msg)));
    const int slot = static_cast<int>(mbSlot(msg));
    if (user->CheckItemLock(invenType, slot))
        return 0xd5;

    {
        bool movable = true;
        CheckItemTradeLimitCountToMoveByUser(*user, invenType, slot, movable);
        if (!movable)
            return 0x72;
    }

    {
        const std::string title(mbText(msg));                   // +0x45 文本入 std::string
        if (CSyncSlangFilter::GetInstance()->HasSlang(title))
            return 0x9f;
    }

    if (strlen(mbText(msg)) != static_cast<size_t>(mbTitleLen(msg)))
        return -3;                                              // 标题长与实际串长不符

    if (mbItemId(msg) == 0 &&
        mbGold(msg) == 0 &&
        mbText(msg)[0] == '\0')
        return 0x16;                                            // 纯空邮件

    if (mbGuildKey(msg) != 0) {
        const int guildErr = GuildMailCheck(user, msg);
        if (guildErr != 0)
            return guildErr;
    }

    if (user->GetReliablePerson() == 1)
        return 0;
    if (!user->IsAvailableCurCharacTradeGoldDaily(mbGold(msg)))
        return 0x46;                                            // 当日交易金币额度不足
    return 0;
}

// ==== Dispatcher_MailBox_Send::process (ORIG 0x081cc958) ====
int Dispatcher_MailBox_Send::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stMailboxSendParam& out = static_cast<stMailboxSendParam&>(param);

    out.m_4 = check_error(user, msg);                           // +0x04 错误码
    if (out.m_4 > 0)
        return 0;
    if (out.m_4 < 0)
        return LineFunc(0x23ce,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());

    if (mbGuildKey(msg) != 0) {                                 // ---- 公会邮件分支 ----
        const int fee = *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CDataManager()) + 0xa624);  // +0xa624 公会邮件费（推断）
        CInventory* invW = user->getCurCharacInvenW();
        invW->use_money(fee, static_cast<eMoneySubReason>(0x16), true);

        CGuildServerProxy* proxy =
            reinterpret_cast<CServerProxyMgr<CGuildServerProxy>*>(GlobalData::s_guild_proxy_mgr)
                ->GetServerProxy(user->GetServerGroup());
        proxy->SendGuildMail(static_cast<unsigned int>(user->getCurCharacNo()),
                             static_cast<unsigned int>(user->get_charac_guildkey()),
                             mbTitleLen(msg),
                             mbText(msg));
        return 0;
    }

    // ---- 附带金币时先走客户端确认回执（certify）路径 ----
    bool needCertify = false;
    if (mbGold(msg) != 0) {
        // ORIG 0x081cca92: needCertify = user->reqSendMailCertify();
        needCertify = user->reqSendMailCertify() != 0;
    }
    if (needCertify) {
        // ORIG 0x081ccaa6: user->set_MailMsgData(&msg) 后返回 0（等待回执）
        user->set_MailMsgData(static_cast<MSG_MAILBOX_SEND*>(&msg));
        return 0;
    }

    // ---- DB 收件人校验请求 ----
    int rc = 0;
    {
        MSG_CHECK_MAIL_RECEIVER chk;
        chk.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_0, MSG_MAILBOX_SEND>(
            static_cast<const MSG_MAILBOX_SEND&>(msg));
        rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(user, mbName(msg), &chk);
    }
    if (rc != 0) {
        out.m_4 = rc;                                           // 校验失败码透传
    }
    else {
        // 交易金币计数：普通低稀有度附件才计入特殊标记
        unsigned char specialMark = 0;
        if (mbItemId(msg) != 0) {
            CItem* item = G_CDataManager()->find_item(static_cast<int>(mbItemId(msg)));
            if (item != 0) {
                // ORIG 经 vtable+0x10 间接调用（= IsAvatarItem 槽，ORIG vtable 已核实）
                if (!item->IsAvatarItem() && !(item->get_rarity() > 1))
                    specialMark = 1;
            }
        }
        if (mbItemId(msg) == 0)
            specialMark = 1;
        // ORIG 0x081ccc26: user->IncrementTradeGold(mbGold(msg), specialMark);
        user->IncrementTradeGold(mbGold(msg), static_cast<int>(specialMark));
    }

    // ---- 邮件金币上限统计（SIG_FIND_FACTORY_HUB 入 NETWORK 统计队列）----
    {
        const unsigned short usedFatigue = user->getCurCharacUsedFatigue();
        const int rnd = get_rand_int(0x19);
        const int money = user->getCurCharacInvenR()->get_money();
        // ORIG 浮点序：(long long)(money * (0.7 + (float)((double)rnd * 0.01)))
        // （.rodata 0x08bd6338=0.01、0x08bd6340=0.7，已从二进制核实）
        const double factor = 0.7 + static_cast<double>(static_cast<float>(static_cast<double>(rnd) * 0.01));
        const unsigned long long goldCap =
            static_cast<unsigned long long>(static_cast<double>(money) * factor);
        if (goldCap <= static_cast<unsigned long long>(mbGold(msg))) {
            Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x241d);
            CStreamGuard guard(stream, true);
            guard << static_cast<int>(0x18a);                   // 统计包命令字
            guard << static_cast<int>(user->GetUID());
            SIG_FIND_FACTORY_HUB* sig = guard.GetInBuffer<SIG_FIND_FACTORY_HUB>();
            memset(sig, 0, 0x24);                               // SIG 总长 0x24（当前头为 pad 视图）
            *reinterpret_cast<unsigned int*>(sig) = user->get_acc_id();          // +0x00 accId
            strncpy(reinterpret_cast<char*>(sig) + 4, mbName(msg),
                    static_cast<size_t>(mbLen(msg)));                            // +0x04 收件人名
            *(reinterpret_cast<char*>(sig) + 0x21) = 0;                          // +0x21 疲劳标记清零
            const int fatigueRoll = get_rand_int(0x14);
            if (fatigueRoll + 0x78 <= static_cast<int>(static_cast<unsigned int>(usedFatigue)))
                *(reinterpret_cast<char*>(sig) + 0x21) = 1;              // +0x21 疲劳标记
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
    }
    return 0;
}

// ==== Dispatcher_MailBox_Send::send (ORIG 0x081ccdb2) ====
void Dispatcher_MailBox_Send::send(CUser* user, ParamBase& param)
{
    stMailboxSendParam& p = static_cast<stMailboxSendParam&>(param);
    if (p.m_4 == 0x7fffffff)                                    // ORIG 哨兵值：静默
        return;
    if (p.m_4 == 0)
        return;
    user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x61),
                             static_cast<unsigned char>(p.m_4 & 0xff));
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
Dispatcher_MonstercardBind::Dispatcher_MonstercardBind()
{
    // ORIG：call IPacketDispatcher<MSG_MONSTERCARD_BIND,stMonstercardBind,
    // ch_state3>::IPacketDispatcher(); 置 vtable; time(0); new(0x9c8) 后
    // CMTRand(const unsigned long&) 种子构造，指针存 this+0x04。
    unsigned long seed = (unsigned long)time(0);
    m_rand = new CMTRand(seed);
}

// ORIG D1 0x08244b42：设回基类 vptr、调用模板基类析构；m_rand 不释放（ORIG test 0 路径）。
Dispatcher_MonstercardBind::~Dispatcher_MonstercardBind()
{
}

// ==== Dispatcher_MonstercardBind::~Dispatcher_MonstercardBind (ORIG D1 0x08244b42 / D0 0x08244b72，基类转发) ====
// ==== Dispatcher_MonstercardBind::check_need_slot (ORIG 0x081d83c4 T) ====
int Dispatcher_MonstercardBind::check_need_slot(CUser* user)
{
    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());
    const int emptyCard =
        invenR->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)10);
    const int emptyAny =
        invenR->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)0);
    if (emptyCard < 0 && emptyAny >= 0)
    {
        return 1;
    }
    return 0;
}

// ==== Dispatcher_MonstercardBind::calcurate_rarity (ORIG 0x081d842e T) ====
int Dispatcher_MonstercardBind::calcurate_rarity(ENUM_RARITY rarity,
                                                 unsigned short rate)
{
    unsigned long range = 10000;
    const unsigned int roll = m_rand->randInt(range);
    if (roll < rate)
    {
        switch (rarity)
        {
        case RARITY_COMMON:
            return 1;
        case RARITY_UNCOMMON:
            return 2;
        case RARITY_RARE:
        case RARITY_UNIQUE:
            return 3;
        default:
            return 0;
        }
    }
    return rarity;
}

// ==== Dispatcher_MonstercardBind::get_bind_result (ORIG 0x081d84a0 T) ====
int Dispatcher_MonstercardBind::get_bind_result(
    CUser* user, short slot, ENUM_RARITY rarity,
    std::vector<std::pair<int, int> >& results)
{
    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());
    Inven_Item inven = invenR->GetInvenSlot(1, slot);
    CItem* card = G_CDataManager()->find_item(inven.m_addInfo);
    // +0x330 推断：怪物卡绑定数据索引（同 check_error 注）。
    const int cardDataIdx = *reinterpret_cast<const int*>(
        reinterpret_cast<const char*>(card) + 0x330);
    STExpertJobScript* script = static_cast<STExpertJobScript*>(
        G_CDataManager()->GetExpertJobScript(1));
    stMonsterCardBindRate rate;
    if (!script->GetMonsterCardBindRate(cardDataIdx, rate))
    {
        return 0;
    }
    results.clear();
    const int newRarity = calcurate_rarity(rarity, rate.m_field0);
    unsigned long range = 10000;
    const unsigned int roll = m_rand->randInt(range);
    if (roll < rate.m_field2)
    {
        return _bind_process(BIND_TYPE_SPECIAL, (ENUM_RARITY)newRarity,
                             results);
    }
    return _bind_process(BIND_TYPE_COMMON, (ENUM_RARITY)newRarity, results);
}

// ==== Dispatcher_MonstercardBind::_bind_process (ORIG 0x081d85fc T) ====
int Dispatcher_MonstercardBind::_bind_process(
    ENUM_MONSTER_CARD_BIND_TYPE type, ENUM_RARITY rarity,
    std::vector<std::pair<int, int> >& results)
{
    STExpertJobScript* script = static_cast<STExpertJobScript*>(
        G_CDataManager()->GetExpertJobScript(1));
    STExpertJobScript::stMonsterCardBindInfo* info;
    if (type == BIND_TYPE_SPECIAL)
    {
        info = script->GetSpecialBindInfo();
    }
    else if (type == BIND_TYPE_COMMON)
    {
        info = script->GetCommonBindInfo();
    }
    else
    {
        results.clear();
        return 0;
    }
    const std::vector<stMonsterCardBindResult>& table = info->m_vec[rarity];
    const int total = info->m_count[rarity];
    const size_t entryCount = table.size();
    if (entryCount == 0)
    {
        cMyTrace trace(
            "bool Dispatcher_MonstercardBind::_bind_process("
            "Dispatcher_MonstercardBind::ENUM_MONSTER_CARD_BIND_TYPE, "
            "ENUM_RARITY, IntPairVector&)",
            0x44b1, 5);
        trace("MonstercardBind Error : size(%d)", (int)entryCount);
        return 0;
    }
    if (total == 0)
    {
        cMyTrace trace(
            "bool Dispatcher_MonstercardBind::_bind_process("
            "Dispatcher_MonstercardBind::ENUM_MONSTER_CARD_BIND_TYPE, "
            "ENUM_RARITY, IntPairVector&)",
            0x44b6, 5);
        trace("MonstercardBind Error : size(%d), sum(%d)", (int)entryCount,
              total);
        return 0;
    }
    unsigned long range = (unsigned long)(total - 1);
    const int pick = m_rand->randInt(range) + 1;
    int accWeight = 0;
    for (size_t i = 0; i < entryCount; ++i)
    {
        const unsigned short weight = (unsigned short)table[i].m_field4;
        if (accWeight < pick && pick <= accWeight + (int)weight)
        {
            results.push_back(std::make_pair(table[i].m_field0, 1));
            break;
        }
        accWeight += (int)weight;
    }
    if (results.empty())
    {
        cMyTrace trace(
            "bool Dispatcher_MonstercardBind::_bind_process("
            "Dispatcher_MonstercardBind::ENUM_MONSTER_CARD_BIND_TYPE, "
            "ENUM_RARITY, IntPairVector&)",
            0x44ca, 5);
        trace("Dispatcher_MonstercardBind::_bind_process ERROR : result is "
              "empty");
        return 0;
    }
    return 1;
}

// ==== Dispatcher_MonstercardBind::read (ORIG 0x081d82e8 T) ====

// ==== Dispatcher_MonstercardBind::read (ORIG 0x081d82e8 T) ====
int Dispatcher_MonstercardBind::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_MONSTERCARD_BIND& m = static_cast<MSG_MONSTERCARD_BIND&>(msg);
    short value = 0;
    if (!packet.get_short(value))
    {
        return LineFunc(
            0x444a,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    m.m_0d = (unsigned short)value;
    if (!packet.get_short(value))
    {
        return LineFunc(
            0x444c,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    m.m_0f = (unsigned short)value;
    if (!packet.get_short(value))
    {
        return LineFunc(
            0x444e,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    m.m_11 = (unsigned short)value;
    return 0;
}

// ==== Dispatcher_MonstercardBind::check_error (ORIG 0x081d8de2 T) ====
int Dispatcher_MonstercardBind::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }

    MSG_MONSTERCARD_BIND& m = static_cast<MSG_MONSTERCARD_BIND&>(msg);
    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());

    // 卡 1（msg+0x0d）：须为非堆叠怪物卡且绑定数据索引有效
    Inven_Item slotA = invenR->GetInvenSlot(1, m.m_0d);
    CItem* cardA = G_CDataManager()->find_item(slotA.m_addInfo);
    if (cardA == 0)
    {
        return 0x11;
    }
    if (cardA->is_stackable())
    {
        return 0x11;
    }
    {
        // +0x330 推断：怪物卡绑定数据索引（ORIG mov 0x330(%eax),%eax；
        // CItem.h 当前布局止于 +0x148，按任务规则偏移访问）。
        const int bindInfoIdx = *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(cardA) + 0x330);
        if (bindInfoIdx == -1)
        {
            return 0x11;
        }
    }
    if (slotA.m_addInfo2 < 1)
    {
        return 0x16;
    }

    // 卡 2（msg+0x0f）：非堆叠 + 数量校验（无绑定索引检查，与 ORIG 一致）
    Inven_Item slotB = invenR->GetInvenSlot(1, m.m_0f);
    CItem* cardB = G_CDataManager()->find_item(slotB.m_addInfo);
    if (cardB == 0)
    {
        return 0x11;
    }
    if (cardB->is_stackable())
    {
        return 0x11;
    }
    if (slotB.m_addInfo2 < 1)
    {
        return 0x16;
    }

    // 卡 3（msg+0x11）：同卡 2 口径
    Inven_Item slotC = invenR->GetInvenSlot(1, m.m_11);
    CItem* cardC = G_CDataManager()->find_item(slotC.m_addInfo);
    if (cardC == 0)
    {
        return 0x11;
    }
    if (cardC->is_stackable())
    {
        return 0x11;
    }
    if (slotC.m_addInfo2 < 1)
    {
        return 0x16;
    }

    // 卡 2/卡 3 稀有度必须一致
    if (cardB->get_rarity() != cardC->get_rarity())
    {
        return 0x13;
    }

    // ---- check_need_slot @0x081d83c4 T ----
    if (!check_need_slot(user))
    {
        return 4;
    }

    // 非 PvP 通道且处于村庄态(3)才允许继续
    if (G_GameWorld()->IsPVPChannel() || user->get_state() != 3)
    {
        return -1;
    }

    // 类别校验（ORIG call CItem::hasCategory W 0x0822c97a = m_set128 查询；
    // CItem.h 未声明该方法，按其实现语义直查 m_set128）：
    // 两卡均须含类别 1，且均不得含类别 2。
    if (!(cardB->m_set128.count((STItemScript::ENUM_ITEM_CATEGORY)1) != 0 &&
          cardC->m_set128.count((STItemScript::ENUM_ITEM_CATEGORY)1) != 0))
    {
        return 0x17;
    }
    if (cardB->m_set128.count((STItemScript::ENUM_ITEM_CATEGORY)2) != 0 ||
        cardC->m_set128.count((STItemScript::ENUM_ITEM_CATEGORY)2) != 0)
    {
        return 0x17;
    }

    // 保护场稀有度检查；非 0 错误码透传
    return GlobalData::s_pSecuProtectionField->CheckItemRarity(
        user, (SECURITY_PROTCTION)0x28, cardB, &slotB);
}

// ==== Dispatcher_MonstercardBind::process (ORIG 0x081d88b0 T) ====
int Dispatcher_MonstercardBind::process(CUser* user, MSG_BASE& msg,
                                        ParamBase& param)
{
    stMonstercardBind& res = static_cast<stMonstercardBind&>(param);
    MSG_MONSTERCARD_BIND& m = static_cast<MSG_MONSTERCARD_BIND&>(msg);

    res.m_4 = check_error(user, msg);
    if (res.m_4 > 0)
    {
        return 0;
    }
    if (res.m_4 < 0)
    {
        return LineFunc(0x44db,
                        "virtual int Dispatcher_MonstercardBind::process(CUser*, "
                        "MSG_BASE&, ParamBase&)",
                        res.m_4, 0);
    }

    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());
    Inven_Item slotA = invenR->GetInvenSlot(1, m.m_0d);
    CItem* cardA = G_CDataManager()->find_item(slotA.m_addInfo);
    Inven_Item slotB = invenR->GetInvenSlot(1, m.m_0f);
    CItem* cardB = G_CDataManager()->find_item(slotB.m_addInfo);
    Inven_Item slotC = invenR->GetInvenSlot(1, m.m_11);
    CItem* cardC = G_CDataManager()->find_item(slotC.m_addInfo);

    std::vector<std::pair<int, int> > results;
    results.clear();   // ORIG 0x081d8a32：构造后显式 clear

    bool bindOk = false;
    {
        // ---- get_bind_result @0x081d84a0 T：主卡绑定率 → 轮盘选表 ----
        const int rarity = cardB->get_rarity();
        bindOk = (get_bind_result(user, (short)m.m_0d, (ENUM_RARITY)rarity,
                                  results) != 0);
    }

    if (!bindOk)
    {
        res.m_4 = 0x13;
        return 0;
    }

    // 影子背包试删试插（失败不影响真背包）
    CInventory tmpInv;
    InventoryMemory tmpMem;
    tmpInv.SetInventoryMemory(&tmpMem);
    // ORIG 081d8ac2：实参为 getCurCharacInvenRefR()(W 0x081803ca，charac+0xda
    // 槽区视图)，直接按 CInventory 传入 setCopy —— 类型双关，忠实转写。
    tmpInv.setCopy(*reinterpret_cast<const CInventory*>(
        user->getCurCharacInvenRefR()));

    const bool delA = tmpInv.delete_item((INVEN_TYPE)1, m.m_0d, 1,
                                         (eItemDelReason)0x2c, false);
    if (!delA)
    {
        res.m_4 = 0x11;
        return 0;
    }
    const bool delB = tmpInv.delete_item((INVEN_TYPE)1, m.m_0f, 1,
                                         (eItemDelReason)0x2c, false);
    if (!delB)
    {
        res.m_4 = 0x11;
        return 0;
    }
    const bool delC = tmpInv.delete_item((INVEN_TYPE)1, m.m_11, 1,
                                         (eItemDelReason)0x2c, false);
    if (!delC)
    {
        res.m_4 = 0x11;
        return 0;
    }
    if ((int)tmpInv.tryInsertEventItems(results) != (int)results.size())
    {
        res.m_4 = 0x11;
        return 0;
    }

    // 真背包扣三张卡并插入绑定结果
    CInventory* invenW = user->getCurCharacInvenW();
    invenW->delete_item((INVEN_TYPE)1, m.m_0d, 1, (eItemDelReason)0x2c, true);
    invenW->delete_item((INVEN_TYPE)1, m.m_0f, 1, (eItemDelReason)0x2c, true);
    invenW->delete_item((INVEN_TYPE)1, m.m_11, 1, (eItemDelReason)0x2c, true);

    std::vector<int> outSlots;
    outSlots.clear();   // ORIG 0x081d8cdb：构造后显式 clear
    invenW->insert_event_items(results, outSlots, (eMoneyAddReason)0,
                               (eItemAddReason)0x34, false, "", "");

    res.m_8 = m.m_0d;
    res.m_a = m.m_0f;
    res.m_c = m.m_11;
    res.m_list = outSlots;
    return 0;
}

// ==== Dispatcher_MonstercardBind::send (ORIG 0x081d9148 T) ====
void Dispatcher_MonstercardBind::send(CUser* user, ParamBase& param)
{
    stMonstercardBind& res = static_cast<stMonstercardBind&>(param);
    if (res.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x19e,
                                 (unsigned char)res.m_4);
        return;
    }

    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x19e);
    guard.put_byte(1);
    guard.put_short(res.m_8);
    guard.put_short(res.m_a);
    guard.put_short(res.m_c);

    const size_t entryTotal = res.m_list.size();
    const int groups = (int)(entryTotal / 3);
    guard.put_byte(groups);

    size_t idx = 0;
    for (int g = 0; g < groups; ++g)
    {
        guard.put_short(res.m_list[idx]);
        ++idx;
        guard.put_int(res.m_list[idx]);
        ++idx;
        guard.put_int(res.m_list[idx]);
        ++idx;
    }

    guard.finalize(true);
    user->Send(guard);
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
int Dispatcher_PartyTeleport::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    MSG_PARTY_TELEPORT& req = static_cast<MSG_PARTY_TELEPORT&>(msg);
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    GameWorld* world = G_GameWorld();
    if (world->IsPVPChannel() || world->IsPvPSkilTreeChannel())
    {
        return 0x13;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());   // GetParty 返回 void*
    if (party == 0)
    {
        return 0x13;
    }
    if (party->getManager() != user)                     // 仅队长可发起
    {
        return 0x17;
    }
    Village* village = G_GameWorld()->getUserVillage(static_cast<int>(req.m_field0d));
    if (village == 0)
    {
        return 0x13;
    }
    if (static_cast<int>(req.m_field0e) == village->get_gate_area())   // 目标=城门禁传
    {
        return 0x13;
    }
    int auraSlotCount = 0;
    if (!user->isGMUser())
    {
        // 返回值>0 时直接作为错误码透传（光环头像占用槽位检查）
        int auraEquip = user->is_equip_aura_avatar('\0', auraSlotCount);
        if (auraEquip > 0)
        {
            return auraEquip;
        }
    }
    int auraExpire = user->get_aura_avatar_option_value(0);
    if (!user->isGMUser() &&
        GlobalData::s_systemTime_.getCurSec() < auraExpire)
    {
        return 0x16;                                     // 光环传送券未过期→冷却
    }
    for (int i = 0; i < 4; ++i)                          // P2：i<=3 有符号循环
    {
        if (!party->checkValidUser(i))
        {
            continue;
        }
        CUser* member = party->get_user(i);
        if (member == 0)
        {
            continue;
        }
        int moveErr = G_GameWorld()->check_move_area(
            member, static_cast<int>(req.m_field0d), static_cast<int>(req.m_field0e),
            static_cast<int>(static_cast<short>(req.m_field0f)),
            static_cast<int>(static_cast<short>(req.m_field11)),
            static_cast<int>(req.m_field13), true, 0);
        if (moveErr > 0)
        {
            return moveErr;
        }
    }
    return 0;
}

// ==== Dispatcher_PartyTeleport::read (ORIG 0x081dbe70) ====
int Dispatcher_PartyTeleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PARTY_TELEPORT& req = static_cast<MSG_PARTY_TELEPORT&>(msg);
    if (!packet.get_byte(req.m_field0d))                      // +0x0d village（uchar& 重载）
    {
        return LineFunc(19231,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_field0e))                      // +0x0e area
    {
        return LineFunc(19232,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_field0f)))   // +0x0f posx
    {
        return LineFunc(19233,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_field11)))   // +0x11 posy
    {
        return LineFunc(19234,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_field13))                      // +0x13 direction
    {
        return LineFunc(19235,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_PartyTeleport::process (ORIG 0x081dbfd6) ====
int Dispatcher_PartyTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // stPartyTeleport(Param) pad 未字段化 —— ORIG 报告实证偏移视图：
    char* pbase = reinterpret_cast<char*>(&param);
    int& err = *reinterpret_cast<int*>(pbase + 0x4);              // +0x04 错误码
    unsigned char& echoState = *reinterpret_cast<unsigned char*>(pbase + 0x8); // +0x08 回显
    int& auraRemain = *reinterpret_cast<int*>(pbase + 0xc);       // +0x0c 光环剩余秒

    MSG_PARTY_TELEPORT& req = static_cast<MSG_PARTY_TELEPORT&>(msg);

    err = check_error(user, msg);
    echoState = req.m_field0d;
    auraRemain = user->get_aura_avatar_option_value(0);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    if (curSec < auraRemain)
    {
        auraRemain = auraRemain - curSec;
    }
    else
    {
        auraRemain = 0;
    }

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(19260,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        err = 0x13;
        return 0;
    }
    CPartyTelePort* tp =
        static_cast<CPartyTelePort*>(party->GetPartyTelePort());   // void* → 类视图
    tp->set_teleport_data(req.m_field0d, req.m_field0e,
                          static_cast<short>(req.m_field0f),
                          static_cast<short>(req.m_field11),
                          req.m_field13, '\0');
    int slot = static_cast<int>(party->GetMemberSlotNo(user));
    tp->set_teleport_member_state(slot, 1);
    tp->check_cur_teleport_state();
    return 0;
}

// ==== Dispatcher_PartyTeleport::send (ORIG 0x081dc426) ====
void Dispatcher_PartyTeleport::send(CUser* user, ParamBase& param)
{
    // 偏移视图同 process（stPartyTeleport pad 未字段化）
    char* pbase = reinterpret_cast<char*>(&param);
    int err = *reinterpret_cast<int*>(pbase + 0x4);               // +0x04
    unsigned char echoState = *reinterpret_cast<unsigned char*>(pbase + 0x8); // +0x08
    int auraRemain = *reinterpret_cast<int*>(pbase + 0xc);        // +0x0c

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x1b3);                         // class=1 cmd=0x1b3
    if (err == 0)
    {
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(err & 0xff);
        packet.put_byte(static_cast<int>(echoState));
        packet.put_int(auraRemain);
    }
    packet.finalize(true);
    user->Send(packet);

    if (err == 0)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            CPartyTelePort* tp =
                static_cast<CPartyTelePort*>(party->GetPartyTelePort());
            tp->send_teleport_effect_at_different_place(3);
            tp->send_teleport_status(0);
            tp->process_teleport();
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
bool GetSelectionIndex(CUser& user, int& outIndex, int type);
}

// ==== Dispatcher_PcRoomRentItem::check_error (ORIG 0x081e5762) ====
int Dispatcher_PcRoomRentItem::check_error(CUser* user, MSG_BASE& msg)
{
    MSG_RENT_ITEM& req = static_cast<MSG_RENT_ITEM&>(msg);
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
    {
        char type = static_cast<char>(req.m_0e);          // +0x0e 租借类型（movzbl+sbyte）
        if (type < 1 || type > 2)
        {
            return 8;
        }
        if (user->GetPCRoomNo() == 0 && type == 1)
        {
            return 8;
        }
        // type==2：重复租借事件(0x5e)未进行中 → 错误。vptr+0x34 槽 = IsEventing。
        if (type == 2 &&
            !GlobalData::s_event_manager->GetRepeatEvent(0x5e)->IsEventing((CUser*)0))
        {
            return 8;
        }
        return 0;
    }
    return 3;
}

// ==== Dispatcher_PcRoomRentItem::read (ORIG 0x081e512e) ====
int Dispatcher_PcRoomRentItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_RENT_ITEM& req = static_cast<MSG_RENT_ITEM&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0e)))          // +0x0e type
    {
        return LineFunc(24124,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))          // +0x0d count
    {
        return LineFunc(24128,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char count = static_cast<char>(req.m_0d);
    if (count < 1 || count > 1)                                       // 恒为 1 项
    {
        return LineFunc(24131,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        // 元素对 @+0x0f+2i/+0x10+2i（RentInElem sectionIdx/itemIdx；count==1 不越界）
        if (!packet.get_byte(req.m_0f[i * 2]))
        {
            return LineFunc(24136,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(req.m_0f[i * 2 + 1]))
        {
            return LineFunc(24138,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
}

// ==== Dispatcher_PcRoomRentItem::process (ORIG 0x081e52c0) ====
int Dispatcher_PcRoomRentItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRentItemParam& out = static_cast<stRentItemParam&>(param);
    MSG_RENT_ITEM& req = static_cast<MSG_RENT_ITEM&>(msg);

    out.m_4 = check_error(user, msg);
    if (out.m_4 != 0)
    {
        return 0;
    }

    InstanceRentalSystem::RentInput input;               // ctor 清 vector/machineKey
    input.m_logFlag = 1;                                 // +0x10 log 开关（PcRoom 报告实证）
    input.m_machineKey = static_cast<int>(static_cast<char>(req.m_0e));
    for (int i = 0; i < static_cast<int>(static_cast<char>(req.m_0d)); ++i)
    {
        InstanceRentalSystem::RentInElem elem;
        elem.m_sectionIdx = static_cast<unsigned char>(req.m_0f[i * 2]);      // +0xf+2i
        elem.m_itemIdx = static_cast<unsigned char>(req.m_0f[i * 2 + 1]);     // +0x10+2i
        input.m_elems.push_back(elem);
    }

    giveDimensionItems(user, input);

    out.m_4 = static_cast<int>(
        static_cast<InstanceRentalSystem*>(GlobalData::s_rentalSystem)
            ->Rent(*user, input, out.m_output));
    if (out.m_4 == 0)
    {
        user->UpdateCurrentCharacRentalInfo(
            out.m_output, (int)(char)req.m_0e, false);
    }
    return 0;                                            // ORIG RentInput dtor 于帧尾
}

// ==== Dispatcher_PcRoomRentItem::giveDimensionItems (ORIG 0x081e583e) ====
// 注：ORIG 为类成员（见文件头宿主声明补充说明）。
void Dispatcher_PcRoomRentItem::giveDimensionItems(CUser* user,
                                                   InstanceRentalSystem::RentInput& input)
{
    if (user == 0 || input.m_machineKey != 2)            // 仅 dimension(machineKey==2)
    {
        return;
    }
    int selectionIdx = 0;
    if (!pc_room::GetSelectionIndex(*user, selectionIdx, 2))
    {
        return;
    }
    int growType = static_cast<int>(user->getCurCharFirstGrowType());   // W 0x08110c94
    int job = user->get_charac_job();                                    // W 0x080fdf20

    // ORIG 0x081e58c3: rewardData->getRewardGiveItems(selectionIdx, job, growType)，
    // 非空则逐项组装 RentInElem 入 input.m_elems（m_type=1 GiveList 标记）
    DimensionActivationRewardData* rewardData =
        reinterpret_cast<DimensionActivationRewardData*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x4b34);
    std::vector<EventRewardItemInfo>* items =
        rewardData->getRewardGiveItems(selectionIdx, job, growType);
    if (items != 0)
    {
        for (int i = 0; i < static_cast<int>(items->size()); ++i)
        {
            InstanceRentalSystem::RentInput::RentInElem elem;
            elem.m_sectionIdx = static_cast<unsigned char>(selectionIdx);
            elem.m_itemIdx = static_cast<unsigned char>(i);
            elem.m_type = 1;                             // GiveList 路径标记
            input.m_elems.push_back(elem);
        }
    }
}

// ==== Dispatcher_PcRoomRentItem::send (ORIG 0x081e542e) ====
void Dispatcher_PcRoomRentItem::send(CUser* user, ParamBase& param)
{
    stRentItemParam& out = static_cast<stRentItemParam&>(param);
    bool sendEquipSlot10 = false;

    PacketGuard packet;
    packet.put_header(1, 0x1ee);                         // class=1 cmd=0x1ee
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_byte(static_cast<int>(out.m_output.m_deleted.size()));
        for (std::vector<InstanceRentalSystem::DeletedRentItem>::iterator it =
                 out.m_output.m_deleted.begin();
             it != out.m_output.m_deleted.end(); ++it)
        {
            short slot = static_cast<short>(it->m_slot);              // +0x02
            if (it->m_bEquip == 0)                                    // +0x00
            {
                packet.put_byte(0);
            }
            else
            {
                packet.put_byte(3);
                if (slot > 9 && slot < 0x16)
                {
                    sendEquipSlot10 = true;                           // 装备位 9..0x15 变动
                }
            }
            packet.put_short(static_cast<int>(slot));
        }
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4 & 0xff);
    }
    packet.finalize(true);
    user->Send(packet);

    if (out.m_4 == 0)
    {
        PacketGuard infoPacket;
        infoPacket.put_header(0, 0xe);                   // class=0 cmd=0xe 租借列表刷新
        infoPacket.put_byte(0);
        infoPacket.put_short(static_cast<int>(out.m_output.m_infos.size()));
        for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it =
                 out.m_output.m_infos.begin();
             it != out.m_output.m_infos.end(); ++it)
        {
            const CInventory* inven = const_cast<CInventory*>(user->getCurCharacInvenR());
            inven->MakeItemPacket(INVEN_TYPE_EQUIP,
                                  static_cast<int>(static_cast<short>(it->m_slot)),
                                  infoPacket);
        }
        infoPacket.finalize(true);
        user->Send(infoPacket);
    }

    if (sendEquipSlot10)
    {
        user->send_equip(10);
    }
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
Dispatcher_PurifyItem::Dispatcher_PurifyItem()
{
}

// ==== Dispatcher_PurifyItem::~Dispatcher_PurifyItem (ORIG D1 0x0825afba / D0 0x0825afea，基类转发) ====
Dispatcher_PurifyItem::~Dispatcher_PurifyItem()
{
}

// ==== Dispatcher_PurifyItem::read (ORIG 0x08218b94 T) ====
int Dispatcher_PurifyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PURIFY_ITEM& m = static_cast<MSG_PURIFY_ITEM&>(msg);

    unsigned short word = 0;
    unsigned long itemIdx = 0;

    if (!packet.get_short(word))
    {
        return LineFunc(
            0xcbbf,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    m.m_0d = word;

    if (!packet.get_item_idx(itemIdx))  // PacketBuf::get_item_idx W 0x0822b6e8
    {
        return LineFunc(
            0xcbc1,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    m.m_0f = (int)itemIdx;

    if (!packet.get_short(word))
    {
        return LineFunc(
            0xcbc3,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    m.m_13 = word;

    if (!packet.get_item_idx(itemIdx))
    {
        return LineFunc(
            0xcbc5,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    m.m_15 = (int)itemIdx;
    return 0;
}

// ==== Dispatcher_PurifyItem::process (ORIG 0x08218cb6 T) ====
int Dispatcher_PurifyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;  // ORIG 不读 param（local_18 仅赋值未使用）

    if (user->get_state() != 3)
    {
        return LineFunc(0xcbcb,
                        "virtual int Dispatcher_PurifyItem::process(CUser*, "
                        "MSG_BASE&, ParamBase&)",
                        0, 0);
    }

    MSG_PURIFY_ITEM& m = static_cast<MSG_PURIFY_ITEM&>(msg);

    if (G_GameWorld()->GetChannelType() == 7)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, 0x11);
        return 0;
    }

    if (user->CheckItemLock(1, m.m_0d))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, 0xd5);
        return 0;
    }

    try
    {
        Inven_Item targetItem;
        Inven_Item materialItem;

        const int purifyType =
            GlobalData::s_itemAmplifier_->getPurifyMaterialType(m.m_15);
        if (purifyType == -1)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }
        if (GlobalData::s_itemAmplifier_->getPurifyMaterialItemIdx(
                purifyType) != (int)m.m_15)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }

        targetItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_0d);
        if (targetItem.m_addInfo != (int)m.m_0f)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }
        // 已鉴定（bit7==0）的强化选项不允许净化
        if (targetItem.m_amp.isIdentified())
        {
            throw static_cast<ENUM_ERROR>(0x17);
        }

        materialItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_13);
        const int needCount =
            GlobalData::s_itemAmplifier_->getPurifyMaterialItemCount(
                purifyType);
        if (materialItem.m_addInfo != (int)m.m_15 ||
            materialItem.m_addInfo2 < needCount)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }

        CInventory* invenW = user->getCurCharacInvenW();
        if (!invenW->delete_item((INVEN_TYPE)1, m.m_13, needCount,
                                 (eItemDelReason)3, true))
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }

        // ---- 内联：processPurifyType @0x082194b6 T ----
        // type 0：鉴定强化选项（stAmplifyOption_t::identify W 0x0822b3d8，
        //         未鉴定→清 bit7 返真；头内 void 声明，按指令语义展开）。
        // type 1：重置强化选项（stAmplifyOption_t::reset W 0x080cb75c）后返真。
        bool purified = false;
        if (purifyType == 0)
        {
            purified = !targetItem.m_amp.isIdentified();
            if (purified)
            {
                targetItem.m_amp.m_abilityType = (unsigned char)(
                    targetItem.m_amp.m_abilityType & 0x7f);
            }
        }
        else if (purifyType == 1)
        {
            targetItem.m_amp.reset();
            purified = true;
        }
        if (!purified)
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }

        user->m_historyLog.apply_Item(materialItem.m_addInfo,
                                      targetItem.m_addInfo,
                                      (eApplyItemReason)2);

        invenW->update_item((INVEN_TYPE)1, m.m_0d, targetItem);

        // 刷新材料槽（删除后剩余数量入包）
        materialItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_13);

        PacketGuard guard;
        guard.put_header(1, 0xcf);
        guard.put_byte(1);
        guard.put_short(m.m_13);
        guard.put_int(materialItem.m_addInfo2);
        guard.put_short(m.m_0d);
        guard.put_byte((unsigned char)targetItem.m_amp.getAbilityType());
        guard.put_short(targetItem.m_amp.getAbilityValue());
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    catch (ENUM_ERROR& e)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, (unsigned char)e);
        return 0;
    }
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

inline unsigned char& rfTreeIndex(ParamBase& p) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&p) + 0x04); }   // +0x04 树序号
inline bool& rfSpecial(ParamBase& p) { return *reinterpret_cast<bool*>(reinterpret_cast<char*>(&p) + 0x05); }                       // +0x05 特殊技能
inline unsigned short& rfRemain(ParamBase& p) { return *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&p) + 0x06); }    // +0x06 剩余SP/SFP
inline int& rfResult(ParamBase& p) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&p) + 0x08); }                          // +0x08 refund 结果
inline unsigned char& rfKind(ParamBase& p) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&p) + 0x0c); }        // +0x0c 树种类
inline unsigned char& rfLevel(ParamBase& p) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&p) + 0x0d); }       // +0x0d 等级
inline int& rfItemIdx(ParamBase& p) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&p) + 0x10); }                         // +0x10 物品idx
inline unsigned char& rfCount(ParamBase& p) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&p) + 0x14); }       // +0x14 数量
inline int& rfErr(ParamBase& p) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&p) + 0x18); }                             // +0x18 错误码

}  // namespace

// ==== Dispatcher_RefundSkill::read (ORIG 0x081da6fe) ====
int Dispatcher_RefundSkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_REFUND_SKILL& req = static_cast<MSG_REFUND_SKILL&>(msg);
    if (!packet.get_int(req.m_0d))  // 技能 id
        return LineFunc(0x48e5, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_11)))  // 技能树选择（signed byte 读入）
        return LineFunc(0x48e7, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(req.m_12))  // 树种类
        return LineFunc(0x48e9, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(req.m_13))  // 退款数量
        return LineFunc(0x48eb, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}

// ==== Dispatcher_RefundSkill::check_error (ORIG 0x081dad1c) ====
int Dispatcher_RefundSkill::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
        return -1;
    const MSG_REFUND_SKILL& req = static_cast<const MSG_REFUND_SKILL&>(msg);

    if (user->getCurCharacR() == 0)
        return -1;

    if (user->get_state() != 3)
        return 7;

    // PvPSkilTree 频道只允许对应树技能 id（0x2897c7 普通 / 0x2897c6 PvP）
    if (G_GameWorld()->IsPvPSkilTreeChannel()) {
        if (req.m_0d != 0x2897c7)
            return 0x16;
    }
    else {
        if (req.m_0d != 0x2897c6)
            return 0x16;
    }

    // 技能树序号与包内选择必须一致：-1/0 树禁选 1，其余必选 1
    const int treeIndex = user->GetCurCharacSkillTreeIndex();
    if (treeIndex == -1 || treeIndex == 0) {
        if (req.m_11 == 1)
            return 0x13;
    }
    else {
        if (req.m_11 != 1)
            return 0x13;
    }

    if (req.m_13 == 0)
        return 0x13;                                            // 数量不得为 0
    return 0;
}

// ==== Dispatcher_RefundSkill::process (ORIG 0x081da820) ====
int Dispatcher_RefundSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_REFUND_SKILL& req = static_cast<MSG_REFUND_SKILL&>(msg);

    rfErr(param) = check_error(user, msg);                      // +0x18 错误码
    if (rfErr(param) > 0)
        return 0;
    if (rfErr(param) < 0)
        return LineFunc(0x48fa,
                        __PRETTY_FUNCTION__,
                        rfErr(param), 0);

    // ORIG 帧：std::vector<int> 局部仅构造/clear/析构（无副作用消费），以等大
    // dead[] 复原帧空间。
    char deadVec[12];                                           // 复原 vector<int> 帧槽（未用局部）

    Inven_Item item;
    item.reset();
    const CInventory* invR = user->getCurCharacInvenR();
    const int slot = invR->GetInvenData(req.m_0d, item);
    if (!(item.m_addInfo == req.m_0d &&                          // +0x02 物品 idx 一致
          req.m_13 <= item.m_addInfo2 &&                        // +0x07 持有数 ≥ 请求数
          slot >= 0)) {
        rfErr(param) = 0x16;
        return 0;
    }

    {
        CInventory snapshot;                                    // 库存副本上先行试删
        InventoryMemory mem;
        snapshot.SetInventoryMemory(&mem);
        snapshot.setCopy(*invR);
        if (snapshot.delete_item(INVEN_TYPE_EQUIP, slot, req.m_13, eItemDelReason_3, false)) {
            rfErr(param) = 0x16;                                // 副本删除失败
            return 0;
        }

        rfErr(param) = 0;                                       // refund_skill 的 needGold 出参
        SkillSlot* skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
        const int result = skillW->refund_skill(
            static_cast<int>(req.m_12),                         // skillIdx（树内技能）
            user->get_charac_level(),
            user->get_charac_job(),
            static_cast<int>(user->getCurCharFirstGrowType()),
            static_cast<int>(user->getCurCharSecondGrowType()),
            static_cast<int>(req.m_13),                         // refundCount
            rfErr(param));
        rfResult(param) = result;                               // +0x08

        if (result > 0xcc)
            return 0;                                           // 超出可退上限
        if (result < 0)
            return 0;

        // 真实删除背包中的退款材料书
        if (user->delete_item(static_cast<char>(INVEN_TYPE_INVEN), slot,
                              static_cast<unsigned long>(req.m_0d),
                              static_cast<int>(req.m_13), eItemDelReason_3) != 0) {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x4932, 5);
            trace("delete_item failed - refund skill %d/%d",
                  static_cast<int>(req.m_0d), static_cast<int>(req.m_13));
        }

        rfSpecial(param) = false;                               // +0x05 特殊技能标记
        CSkill* skill = G_CDataManager()->find_skill(user->get_charac_job(),
                                                     static_cast<int>(req.m_12));
        if (skill != 0 && skill->IsSpecialSkill())
            rfSpecial(param) = true;

        rfTreeIndex(param) = static_cast<unsigned char>(user->GetCurCharacSkillTreeIndex());  // +0x04
        rfKind(param) = req.m_12;                               // +0x0c
        rfLevel(param) = static_cast<unsigned char>(
            reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                ->get_skill_level(rfResult(param)));            // +0x0d

        if (!rfSpecial(param)) {
            rfRemain(param) = static_cast<unsigned short>(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                    ->get_remain_sp_at_index(
                        static_cast<ENUM_SKILL_TREE_KIND>(
                            static_cast<char>(rfTreeIndex(param)))));  // +0x06
        }
        else {
            const int ti = user->GetCurCharacSkillTreeIndex();
            const ENUM_SKILL_TREE_KIND sfp =
                (ti == -1 || ti == 0) ? SKILL_TREE_SFP_1 : SKILL_TREE_SFP_2;
            rfRemain(param) = static_cast<unsigned short>(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                    ->get_remain_sfp_at_index(sfp));
        }

        rfItemIdx(param) = req.m_0d;                            // +0x10
        rfCount(param) = req.m_13;                              // +0x14
    }
    return 0;
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
void Dispatcher_RefundSkill::send(CUser* user, ParamBase& param)
{
    RefundParamView& v = *reinterpret_cast<RefundParamView*>(&param);
    if (v.m_err != 0) {                                         // +0x18 错误码
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1ab),
                                 static_cast<unsigned char>(v.m_err & 0xff));
    }
    else
    {
        PacketGuard pg;
        pg.clear();
        pg.put_header(1, 0x1ab);
        pg.put_byte(1);                                         // 成功标志
        pg.put_byte(static_cast<int>(v.m_special));                    // +0x05
        pg.put_byte(static_cast<int>(v.m_treeIndex));               // +0x04
        pg.put_short(static_cast<int>(v.m_remain));             // +0x06
        pg.put_byte(static_cast<int>(static_cast<unsigned char>(v.m_result)));   // +0x08 低字节
        pg.put_byte(static_cast<int>(v.m_kind));                // +0x0c
        pg.put_byte(static_cast<int>(v.m_level));               // +0x0d
        pg.put_int(v.m_itemIdx);                                // +0x10
        pg.put_byte(static_cast<int>(v.m_count));               // +0x14
        pg.finalize(true);
        user->Send(pg);
    }
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
int Dispatcher_SecurityCardAuthRpy::read(PacketBuf& packet, MSG_BASE& msg)
{
    ScrMsgView& m = *reinterpret_cast<ScrMsgView*>(&msg);
    if (!packet.get_byte(m.m_secuType))
        return LineFunc(0x2b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(m.m_cryptoType))
        return LineFunc(0x2e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(m.m_dataLen))
        return LineFunc(0x32,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_binary(m.m_data, m.m_dataLen))
        return LineFunc(0x35,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(m.m_answerType))
        return LineFunc(0x3a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    return 0;
}

// ==== Dispatcher_SecurityCardAuthRpy::check_error (ORIG 0x0826314c) ====
int Dispatcher_SecurityCardAuthRpy::check_error(CUser* user, MSG_BASE& msg)
{
    ScrMsgView& m = *reinterpret_cast<ScrMsgView*>(&msg);
    if (m.m_cryptoType == 0 || m.m_cryptoType > 0xd || m.m_cryptoType == 5)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0x14c, 0);
        trace("ERROR [TRACE AUTH RPY] : crypto_type2");
        return 0x14d;
    }
    if (user->get_state() <= 1)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0x154, 0);
        trace("ERROR [TRACE AUTH RPY] : get_state()<LOGIN");
        return 0x155;
    }
    return 0;
}

// ==== Dispatcher_SecurityCardAuthRpy::process (ORIG 0x082627ca) ====
// 安全卡/密保卡认证应答：按 secuType 选密钥管理器解密 0x1000 内缓冲，
// 安卡走 checkAnswer/失败计数/DB 上报链，pad 走 doPassPadReplay 链。
int Dispatcher_SecurityCardAuthRpy::process(CUser* user, MSG_BASE& msg, ParamBase& paramBase)
{
    ScrMsgView& m = *reinterpret_cast<ScrMsgView*>(&msg);
    SecuCardRpyParam& param = static_cast<SecuCardRpyParam&>(paramBase);

    if (check_error(user, msg) > 0)
        return -1;

    param.m_4 = 0;
    param.m_5 = 0;

    unsigned char decrypted[0x400];
    memset(decrypted, 0, sizeof(decrypted));
    int decryptedLen = 0x1000;

    WongWork::CSecurityCard* card = user->getSecurityCard();
    Sanicova::CPad* pad = user->getPad();
    Cipher* cipher = GlobalData::s_secu_cipher_keystring_manager_
        [(pad->GetSelectedCipherIdx() & 0xffff) * 4];

    const bool cardActive = (card->isActivate() && m.m_secuType == 0);
    const bool padActive = (pad->isActivate() && m.m_secuType == 1);

    if (!cardActive && !padActive)
        return LineFunc(0x76,
                        __PRETTY_FUNCTION__,
                        0, 0);

    if (!cardActive)
    {
        // pad 分支：解密失败即 trace + 错误出参后返回。
        if (cipher->Decrypt((int)m.m_cryptoType,
                            reinterpret_cast<const unsigned char*>(m.m_data),
                            m.m_dataLen, decrypted, &decryptedLen) != 1)
        {
            param.m_5 = 1;
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x71, 0);
            trace("ERROR [TRACE AUTH RPY] : ENUM_CMDPACKET_SECURITY_CARD_AUTH_RPY,ENUM_ERROR_ETC");
            return 0;
        }
    }
    else
    {
        // card 分支：ORIG 不检查 Decrypt 返回值。
        cipher->Decrypt((int)m.m_cryptoType,
                        reinterpret_cast<const unsigned char*>(m.m_data),
                        m.m_dataLen, decrypted, &decryptedLen);
    }

    // 解密明文前 6 字节（安卡用前两字节作答案；pad 用全部 6 字节组 7 字节串）。
    const unsigned char answer0 = decrypted[0];
    const unsigned char answer1 = decrypted[1];
    const unsigned char answer2 = decrypted[2];
    const unsigned char answer3 = decrypted[3];
    const unsigned char answer4 = decrypted[4];
    const unsigned char answer5 = decrypted[5];

    if (cardActive)
    {
        // ORIG：cardActive 分支先 setRequestState(0)（ORIG 0x0822ef64），
        // 再预检 failCnt>2 → param.m_5=0x7a 直接拒绝（ORIG 0x08262a44 段）。
        card->setRequestState(WongWork::CSecurityCard::eRequestStateNone);
        if ((unsigned int)card->getFailCnt() > 2)
        {
            param.m_5 = 0x7a;
            return 0;
        }
        // ORIG checkAnswer(answer0, answer1)：非 1 走失败链。
        if (card->checkAnswer(answer0, answer1) != 1)
        {
            // ORIG SecuService(1, 0) → incFailCnt → 新值 >2 走 0x7a/0x17/hack，
            // 否则 0x79 + ChangeSecurity(1, failCnt==1 ? 0x15 : 0x16) → DB 上报后返回。
            user->m_historyLog.SecuService(
                (ENUM_SECU_SERVICE_TYPE)1, (ENUM_SECU_SERVICE_ACTION)0);
            unsigned int newFailCnt = (unsigned int)card->incFailCnt();
            if (newFailCnt > 2)
            {
                param.m_5 = 0x7a;
                user->getHades()->ChangeSecurity(
                    (XNuclear::ENUM_HADES_SECURITY)1, 0x17);
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x343, 1, 0, 0);
            }
            else
            {
                param.m_5 = 0x79;
                if (newFailCnt == 1)
                {
                    user->getHades()->ChangeSecurity(
                        (XNuclear::ENUM_HADES_SECURITY)1, 0x15);
                }
                else
                {
                    user->getHades()->ChangeSecurity(
                        (XNuclear::ENUM_HADES_SECURITY)1, 0x16);
                }
            }
            DB_SecurityCardUpdateFailCnt::makeRequest(
                user->get_acc_id(), (unsigned int)card->getFailCnt());
            return 0;
        }

        // 认证成功：SecuService(1, 1) → ChangeSecurity(1, 0x19) →
        // 发包（0xa7, byte1, certCnt>0x3b?1:0）→ DB 清零 fail/cancel → setCertified/resetFailCnt。
        user->m_historyLog.SecuService(
            (ENUM_SECU_SERVICE_TYPE)1, (ENUM_SECU_SERVICE_ACTION)1);
        user->getHades()->ChangeSecurity((XNuclear::ENUM_HADES_SECURITY)1, 0x19);

        PacketGuard packet;
        packet.put_header(1, 0xa7);
        packet.put_byte(1);
        int certCnt = card->getCertCnt();
        packet.put_byte(certCnt > 0x3b ? 1 : 0);
        packet.finalize(true);
        user->Send(packet);

        DB_SecurityCardUpdateFailCnt::makeRequest(user->get_acc_id(), 0);
        if (card->getCancelCnt() != 0)
        {
            DB_SecurityCardUpdateCancelCnt::makeRequest(user->get_acc_id(), 0);
        }
        DB_SecurityCardAddCertCnt::makeRequest(user->get_acc_id());
        card->setCertified(true);
        card->resetFailCnt();
    }
    else  // padActive
    {
        pad->setRequestState(Sanicova::CPad::eRequestStateNone);
        if (pad->getFailCnt() > 2)
        {
            param.m_5 = 0x88;
            return 0;
        }

        char passAnswer[7];
        passAnswer[0] = (char)answer0;
        passAnswer[1] = (char)answer1;
        passAnswer[2] = (char)answer2;
        passAnswer[3] = (char)answer3;
        passAnswer[4] = (char)answer4;
        passAnswer[5] = (char)answer5;
        passAnswer[6] = '\0';

        // ORIG doPassPadReplay(user, passAnswer, answerType, ok, errCode)
        // （Dispatcher_MouseRegister，0x082633c6）。
        Dispatcher_MouseRegister::doPassPadReplay(
            user, passAnswer, (char)m.m_answerType,
            *reinterpret_cast<bool*>(&param.m_4), param.m_5);
        if (param.m_4 == 0)
            return 0;
        param.m_5 = 1;
    }

    param.m_4 = 1;
    return 0;
}

// ==== Dispatcher_SecurityCardAuthRpy::send (ORIG 0x08262f4e) ====
void Dispatcher_SecurityCardAuthRpy::send(CUser* user, ParamBase& paramBase)
{
    SecuCardRpyParam& param = static_cast<SecuCardRpyParam&>(paramBase);
    if (param.m_4 != 0)
    {
        // 认证通过后通知交易对方断开私聊通道（0xb0 包）。
        CTradeSpace* trade = G_CGameManager()->GetTradeSpace(user->GetTradeSpace());
        if (trade != 0 && trade->is_empty() == 0)
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(0, 0xb0);
            packet.put_byte(0);
            packet.finalize(true);
            CUser* opposite = trade->GetOppositeUser(user);
            if (opposite != 0)
                opposite->Send(packet);
        }
        return;
    }
    if (param.m_5 == 0)
        return;

    PacketGuard packet;
    packet.put_header(1, 0xa7);
    packet.put_byte(0);
    packet.put_byte(param.m_5);
    if (param.m_5 == 0x87)
        packet.put_byte(user->getPad()->getFailCnt());
    packet.finalize(true);
    user->Send(packet);
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
int Dispatcher_StackableActionUse::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    MSG_STACKABLE_ACTION_USE& req = static_cast<MSG_STACKABLE_ACTION_USE&>(msg);

    // 背包校验和不一致仅记 hack 计数，不中断（ORIG 形态）
    if (user->getCurCharacInvenCheckSum() != req.m_10)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
    }
    if (user->CheckInTrade())
    {
        return 0xd8;
    }
    if (req.m_14 == 0)                                   // +0x14 action type 非零
    {
        return 0x17;
    }

    short slot = req.m_0d;                               // +0x0d 槽位
    // ORIG 0x0825e3xx：经 getCurCharacInvenRefR（_ZNK…21getCurCharacInvenRefREv）取背包。
    const CInventory* inven = reinterpret_cast<const CInventory*>(user->getCurCharacInvenRefR());
    Inven_Item item = inven->GetInvenSlot(1, static_cast<int>(slot));
    unsigned int itemIdx = static_cast<unsigned int>(item.m_addInfo);   // +0x02

    CItem* itemDef = G_CDataManager()->find_item(static_cast<int>(itemIdx));
    if (itemDef == 0)
    {
        return 0x11;
    }
    // ORIG test %al 单字节检验（P3 收窄）：verify 通过返回 al==1
    if ((unsigned char)user->verifyStackableItemProcess(
            static_cast<StackableAction::Type>(static_cast<char>(req.m_14)),
            itemDef) != 1u)
    {
        return 0x17;
    }
    if (user->CheckCoolTimeItem(static_cast<unsigned long>(itemIdx)))
    {
        return 0x13;
    }
    int ret = inven->try_use_item(static_cast<int>(slot), 1,
                                  static_cast<int>(static_cast<char>(req.m_0f)));
    if (ret != 0)
    {
        return ret;
    }
    return 0;
}

// ==== Dispatcher_StackableActionUse::read (ORIG 0x0825df72) ====
int Dispatcher_StackableActionUse::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_STACKABLE_ACTION_USE& req = static_cast<MSG_STACKABLE_ACTION_USE&>(msg);
    if (!packet.get_short(req.m_0d))                     // +0x0d slot
    {
        return LineFunc(251,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0f)))        // +0x0f invenType
    {
        return LineFunc(254,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_10))                       // +0x10 checksum
    {
        return LineFunc(257,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_14)))        // +0x14 type
    {
        return LineFunc(260,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_15))                       // +0x15 param1
    {
        return LineFunc(263,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_19))                       // +0x19 param2
    {
        return LineFunc(266,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_StackableActionUse::process (ORIG 0x0825e11e) ====
int Dispatcher_StackableActionUse::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // stStackableActionUse pad 未字段化 —— ORIG 报告实证偏移视图：
    char* pbase = reinterpret_cast<char*>(&param);
    int& err = *reinterpret_cast<int*>(pbase + 0x4);              // +0x04 错误码
    short& echoSlot = *reinterpret_cast<short*>(pbase + 0x8);     // +0x08 回显槽位
    char& echoType = *reinterpret_cast<char*>(pbase + 0xa);       // +0x0a 回显类型

    MSG_STACKABLE_ACTION_USE& req = static_cast<MSG_STACKABLE_ACTION_USE&>(msg);

    err = check_error(user, msg);
    echoType = static_cast<char>(req.m_0f);
    echoSlot = req.m_0d;

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_2.cpp",
            __PRETTY_FUNCTION__,
            0x11f, "Dispatcher Check_Error No(%d)", err);
        return LineFunc(288,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    CInventory* inven = const_cast<CInventory*>(user->getCurCharacInvenR());
    Inven_Item item = inven->GetInvenSlot(1, static_cast<int>(req.m_0d));
    unsigned int itemIdx = static_cast<unsigned int>(item.m_addInfo);
    CStackableItem* itemDef =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(static_cast<int>(itemIdx)));
    if (itemDef == 0)
    {
        err = 0x11;
        return 0;
    }

    StackableAction::input_param inputs;   // 前 8 字节未初始化（ORIG 忠实形态）
    inputs.m_user = user;                  // +0x08
    inputs.m_item = itemDef;               // +0x0c
    err = GlobalData::g_Actions->check((int)(char)req.m_14, inputs);
    if (err == 0)
    {
        err = GlobalData::g_Actions->run((int)(char)req.m_14, inputs);
    }
    if (err == 0)
    {
        err = user->getCurCharacInvenW()->use_item(
            (int)req.m_0d, 1, (int)(char)req.m_0f);
        if (err == 0)
        {
            user->_processContinuousItem(itemIdx, itemDef);
        }
    }
    return 0;
}

// ==== Dispatcher_StackableActionUse::send (ORIG 0x0825e4ec) ====
void Dispatcher_StackableActionUse::send(CUser* user, ParamBase& param)
{
    // 偏移视图同 process（stStackableActionUse pad 未字段化）
    char* pbase = reinterpret_cast<char*>(&param);
    int err = *reinterpret_cast<int*>(pbase + 0x4);               // +0x04
    short echoSlot = *reinterpret_cast<short*>(pbase + 0x8);      // +0x08
    char echoType = *reinterpret_cast<char*>(pbase + 0xa);        // +0x0a

    if (err != 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x204);                 // class=1 cmd=0x204
        packet.put_byte(0);
        packet.put_byte(err & 0xff);
        packet.put_byte(static_cast<int>(echoType));
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x204);
        packet.put_byte(1);
        packet.put_short(static_cast<int>(echoSlot));
        packet.put_byte(static_cast<int>(echoType));
        packet.finalize(true);
        user->Send(packet);
    }
}

// ==== Dispatcher_UseDye::getItem (ORIG 0x081e71c0) ====
// 取 (invenType, slot) 槽位物品；返回 true=槽空（m_addInfo==0）。
bool Dispatcher_UseDye::getItem(const CUser& user, int invenType, short slot,
                                Inven_Item& out) const
{
    out = const_cast<CInventory*>(user.getCurCharacInvenR())
              ->GetInvenSlot(invenType, slot);
    return out.m_addInfo == 0;
}

// ==== Dispatcher_UseDye::notifyUpdatedInventoryItemInfo (ORIG 0x081e70e6) ====
void Dispatcher_UseDye::notifyUpdatedInventoryItemInfo(CUser& user, short slot) const
{
    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    user.getCurCharacInvenR()->MakeItemPacket(
        static_cast<INVEN_TYPE>(1), slot, packet);
    packet.finalize(true);
    user.Send(packet);
}

// ==== Dispatcher_UseDye::process (ORIG 0x081e69ea) ====
namespace
{
// DyeInfo（STItemScript.h）字段私有，按 ORIG 布局组件视图访问（P1）：
// +0x00 int 计数/标志、+0x04 vector<pair<int,int>>。
struct UseDyeDyeInfoView
{
    int m_field0;                                 // +0x00
    std::vector<std::pair<int, int> > m_vec4;     // +0x04
};
} // anonymous namespace

int Dispatcher_UseDye::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUseDye& out = static_cast<stUseDye&>(param);
    MSG_USE_DYE& req = static_cast<MSG_USE_DYE&>(msg);

    int err = check_error(user, msg);
    out.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3),
                                 static_cast<unsigned char>(out.m_4));
        return LineFunc(0x625a,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }

    Inven_Item dyeItem;
    if (getItem(*user, 1, req.m_0e, dyeItem))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3),
                                 static_cast<unsigned char>(dyeItem.m_addInfo != 0));
        return 1;
    }
    Inven_Item targetItem;
    if (getItem(*user, 2, req.m_10, targetItem))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3), 1);
        return 1;
    }

    CItem* dyeDef = G_CDataManager()->find_item(dyeItem.m_addInfo);
    if (dyeDef == 0)
    {
        return 1;
    }
    if (!dyeDef->is_stackable())
    {
        return 1;
    }

    const UseDyeDyeInfoView& dye =
        *reinterpret_cast<const UseDyeDyeInfoView*>(&dyeDef->m_dyeInfo9c);
    if (dye.m_field0 == 0 || dye.m_vec4.empty())
    {
        return 1;
    }
    std::map<int, AvatarVariation::colorRGB>::const_iterator colorIt =
        G_CDataManager()->m_avatarColor.m_colorMap.find(dye.m_vec4[0].first);
    if (colorIt == G_CDataManager()->m_avatarColor.m_colorMap.end())
    {
        return 1;
    }

    CEquipItem* avatarDef =
        static_cast<CEquipItem*>(G_CDataManager()->find_item(targetItem.m_addInfo));
    if (!avatarDef->IsAvatarItem())          // vtable +0x10 槽（P5 直调）
    {
        return 1;
    }
    if (avatarDef->IsAvatarColorVariation(0) != 1)
    {
        return 1;
    }

    // ORIG 以目标物品 idx 为 avatar key
    const WongWork::CAvatarItemMgr* mgrR =
        user->getCurCharacInvenR()->GetAvatarItemMgrR();
    const void* expansion = mgrR->GetExpansionInfo(targetItem.m_addInfo);
    if (expansion == 0)
    {
        return 1;
    }
    const short oldColorA = reinterpret_cast<const short*>(expansion)[0];
    const short oldColorB = reinterpret_cast<const short*>(expansion)[1];

    WongWork::CAvatarItemMgr* mgrW = user->getCurCharacInvenW()->GetAvatarItemMgrW();
    const bool applied =
        mgrW->SetAvatarColor(targetItem.m_addInfo,
                             static_cast<short>(dye.m_vec4[0].first), 0);
    if (!applied)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3), 1);
        return LineFunc(0x6292,
                        __PRETTY_FUNCTION__,
                        1, user->get_acc_id());
    }

    const bool deleted = user->getCurCharacInvenW()->delete_item(
        static_cast<INVEN_TYPE>(1), req.m_0e, 1,
        static_cast<eItemDelReason>(3), true);
    if (!deleted)
    {
        // 删除失败：回滚头像颜色并报错
        mgrW->SetAvatarColor(targetItem.m_addInfo, oldColorA, oldColorB);
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3), 0x11);
        return LineFunc(0x629b,
                        __PRETTY_FUNCTION__,
                        1, user->get_acc_id());
    }

    notifyUpdatedInventoryItemInfo(*user, req.m_0e);

    PacketGuard packet;
    packet.put_header(1, 0x1f3);
    packet.put_byte(1);
    packet.put_short(req.m_10);
    packet.put_int(4);
    {
        // pair.first 低 16 位 + 2 字节零填充（local_6c/local_6a）
        const unsigned char raw[4] = {
            static_cast<unsigned char>(dye.m_vec4[0].first & 0xff),
            static_cast<unsigned char>((dye.m_vec4[0].first >> 8) & 0xff),
            0, 0 };
        packet.put_binary(const_cast<char*>(reinterpret_cast<const char*>(raw)), 4);
    }
    packet.finalize(true);
    user->Send(packet);

    /* ORIG 0x81e6fa0..0x81e6fc8：发包后先通知头像变化
       SendAvatarEvent(0x31a, targetItem.m_addInfo, 颜色, 0, 0)，再取头像信息。 */
    user->getCurCharacInvenW()->SendAvatarEvent(
        0x31a, targetItem.m_addInfo, dye.m_vec4[0].first, 0, 0);

    /* ORIG 0x81e6fed..0x81e7062：GetAvatarInfo(targetItem.m_addInfo) →
       UseDye(dyeItem.m_addInfo, targetItem.m_addInfo, targetItem.m_addInfo,
       string(GetIPGAgencyNo()), req.m_10)（cUserHistoryLog @ user+0x79700）。 */
    WongWork::Avatar_Item* avatar =
        user->getCurCharacInvenW()->GetAvatarItemMgrR()->GetAvatarInfo(
            targetItem.m_addInfo);
    user->m_historyLog.UseDye(
        dyeItem.m_addInfo, targetItem.m_addInfo, targetItem.m_addInfo,
        std::string(avatar->GetIPGAgencyNo()), req.m_10);
    return 0;
}

// ==== Dispatcher_Teleport::check_error (ORIG 0x081d056c，三参形态) ====
int Dispatcher_Teleport::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_TELEPORT& req = static_cast<MSG_TELEPORT&>(msg);
    stTeleportParam& out = static_cast<stTeleportParam&>(param);

    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return -1;
    }
    if (user->get_state() > 4)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf1), 0x13);
        return -1;
    }

    const short slot = req.m_field0e;
    const int invenType =
        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)static_cast<int>(req.m_field14));
    Inven_Item* ref = const_cast<Inven_Item*>(
        user->getCurCharacInvenR()->GetInvenRef(invenType, slot));
    if (ref == 0)
    {
        return -2;
    }
    const int itemIdx = ref->m_addInfo;
    if (itemIdx != 0x27ac4e && itemIdx != 0x28e7d0 && itemIdx != 0x29f288)
    {
        return -2;
    }

    const unsigned char vill =
        static_cast<unsigned char>(req.m_field15);
    if (static_cast<char>(vill) < 1)
    {
        return 0x7fffffff;
    }
    if (user->CheckMoveTown(static_cast<int>(static_cast<char>(vill))) != 0)
    {
        return 0x7fffffff;
    }

    CPartyTelePort* teleport = 0;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        teleport = static_cast<CPartyTelePort*>(party->GetPartyTelePort());
        const char state = teleport->get_teleport_state();
        if (state >= 0 && state <= 3)
        {
            return 0x9e;   // 组队传送进行中
        }
    }

    out.m_14 = static_cast<int>(itemIdx);
    // ORIG 0x081d0726：use_stackable((short)m_field0e, (signed char)m_field14,
    // -1, -1)，失败返 0x11。
    if (!user->use_stackable(slot, static_cast<int>(req.m_field14), -1, -1))
    {
        return 0x11;
    }
    // ORIG 0x081d0769：expert_job::CAlchemist::OnTeleportCharacter(user, (signed char)vill,
    // 0, 0, 0, true)，失败返 0x7fffffff。
    if (!expert_job::CAlchemist::OnTeleportCharacter(
            user, static_cast<int>(req.m_field15), 0, 0, 0, true))
    {
        return 0x7fffffff;
    }
    return 0;
}

// ==== Dispatcher_Teleport::process (ORIG 0x081d07bc) ====
int Dispatcher_Teleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stTeleportParam& out = static_cast<stTeleportParam&>(param);
    MSG_TELEPORT& req = static_cast<MSG_TELEPORT&>(msg);

    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2bcc,
                        __PRETTY_FUNCTION__,
                        out.m_4, user->get_acc_id());
    }
    *reinterpret_cast<short*>(&out.m_gapA[0]) = req.m_field0e;
    *reinterpret_cast<unsigned int*>(&out.m_gapA[2]) =
        req.m_field10;
    out.m_10 = req.m_field14;
    out.m_11 = req.m_field15;
    return 0;
}

// ==== Dispatcher_Teleport::send (ORIG 0x081d087c) ====
void Dispatcher_Teleport::send(CUser* user, ParamBase& param)
{
    stTeleportParam& out = static_cast<stTeleportParam&>(param);
    if (out.m_4 == static_cast<int>(0x7fffffff))
    {
        return;
    }
    if (out.m_4 != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf1),
                                 static_cast<unsigned char>(out.m_4));
        return;
    }
    PacketGuard itemPacket;
    itemPacket.put_header(0, 0xe);
    itemPacket.put_byte(out.m_10);
    itemPacket.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket(
        static_cast<INVEN_TYPE>(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)static_cast<int>(static_cast<char>(out.m_10)))),
        out.m_8, itemPacket);
    itemPacket.finalize(true);
    user->Send(itemPacket);

    PacketGuard packet;
    packet.put_header(1, 0xf1);
    packet.put_byte(1);
    packet.put_short(out.m_8);
    packet.put_int(out.m_14);
    packet.finalize(true);
    user->Send(packet);
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
int Dispatcher_CeraPackageOpen::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    if (user->CheckInTrade())
    {
        return 0x13;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    return 0;
}

// ==== Dispatcher_CeraPackageOpen::read (ORIG 0x081e76fa) ====
int Dispatcher_CeraPackageOpen::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* const M = reinterpret_cast<char*>(&msg);

    if (!packet.get_short(*reinterpret_cast<short*>(M + 0xe)))
    {
        return LineFunc(
            0x636b,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(M + 0x10)))
    {
        return LineFunc(
            0x636e,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    // cmp $0x23,%al; jbe → unsigned byte ≤ 0x23 才通过
    if (*reinterpret_cast<const unsigned char*>(M + 0x10) > 0x23)
    {
        return LineFunc(
            0x6371,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    for (unsigned int i = 0; i < *reinterpret_cast<const unsigned char*>(M + 0x10); ++i)
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

// ==== Dispatcher_CeraPackageOpen::process (ORIG 0x081e788a) ====
int Dispatcher_CeraPackageOpen::process(CUser* user, MSG_BASE& msg,
                                        ParamBase& param)
{
    char* const M = reinterpret_cast<char*>(&msg);
    char* const P = reinterpret_cast<char*>(&param);

    const int err = check_error(user, msg);
    *reinterpret_cast<int*>(P + 4) = err;                   // stOpenCeraPackage::m_4
    if (err != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207),
                                 static_cast<unsigned char>(err & 0xff));
        return 0;
    }

    const short slot = *reinterpret_cast<const short*>(M + 0xe);
    if (user->CheckItemLock(1, static_cast<int>(slot)))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207), 4);
        return 0;
    }

    const Inven_Item slotItem =
        user->getCurCharacInvenR()->GetInvenSlot(1, static_cast<int>(slot));
    const int itemIdx = slotItem.m_addInfo;                 // Inven_Item+0x02
    if (itemIdx == 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207), 1);
        return 0;
    }

    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
    {
        return 1;                                           // 静默返回（ORIG ebx=1）
    }
    if (!item->is_stackable() || item->GetItemType() != 0x26)
    {
        // vtable+0xc 直调 = GetItemType()（P5；CItem.h 虚序 D1/D0/make_item/GetItemType）
        return 1;
    }

    {
        // ---- 构造特殊商品处理入参（ORIG 0x081e7a1c..0x081e7b31 逐指令转写）----
        WongWork::CCeraShop::stCeraShopItemParam_t ceraParam;   // ORIG ctor 0x0817a244
        const int subCount = (int)(unsigned int)(unsigned char)*(M + 0x10);
        for (int i = 0; i < subCount; ++i)
        {
            // ORIG pair<int,int> ctor（Rm/Rh）：first=*(ulong*)(M+(i+2)*8+4) 低 32 位，
            // second=*(uchar*)(M+(i+2)*8+8)。
            ceraParam.m_pairs.push_back(std::make_pair(
                *(int*)(M + (i + 2) * 8 + 4),
                (int)*(unsigned char*)(M + (i + 2) * 8 + 8)));
        }

        std::vector<Inven_Item> items;                      // ORIG vector<Inven_Item> C1

        STSpecailItem_Param sp(items, ceraParam);           // W 0x08234ec2
        sp.m_user = user;
        sp.m_itemIdx = itemIdx;
        sp.m_count = 0;
        sp.m_time = 0;
        sp.m_flag = 0;
        sp.m_char1d = 0;
        sp.m_char1e = 0;
        STSpecailItem_Result result;                        // W 0x08234f36（清零首 4 字节）
        const bool handlerOk =
            GlobalData::s_pCeraShop->CallSpecialItemHandler(5, sp, result);
        const int resultValue = result.m_value;

        if (!handlerOk)
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207),
                                     static_cast<unsigned char>(resultValue & 0xff));
            return 0;
        }

        if (resultValue == 0)
        {
            // 扣除礼包本体并回 ACK 包
            user->getCurCharacInvenW()->delete_item(
                static_cast<INVEN_TYPE>(1), static_cast<int>(slot), 1,
                static_cast<eItemDelReason>(3), true);
            PacketGuard pg;                                 // ORIG 此处无 clear（同汇编）
            pg.put_header(1, 0x207);
            pg.put_byte(1);
            pg.put_short(slot);
            pg.finalize(true);
            user->Send(pg);
        }
        else
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207),
                                     static_cast<unsigned char>(resultValue & 0xff));
        }
        return 0;
    }
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
int Dispatcher_ExpertExtraction::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_EXPERT_EXTRACTION& m = static_cast<MSG_EXPERT_EXTRACTION&>(msg);
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
int GetExtracterType(ENUM_EXPERT_JOB_TYPE job);  // ORIG T 0x084a2904（ExpertExtraction.cpp）
}

int Dispatcher_ExpertExtraction::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stExpertExtraction& p = static_cast<stExpertExtraction&>(param);

    p.m_4 = check_error(user, msg);
    if (p.m_4 > 0)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x45bf, __PRETTY_FUNCTION__, p.m_4, 0);
    }

    MSG_EXPERT_EXTRACTION& m = static_cast<MSG_EXPERT_EXTRACTION&>(msg);
    expert_extraction::CExtraction* ex = GlobalData::g_ExtractionMgr->get_extracter(
        expert_extraction::GetExtracterType((ENUM_EXPERT_JOB_TYPE)m.m_0d));
    if (ex == 0)
    {
        return LineFunc(0x45cb, __PRETTY_FUNCTION__, 0, user->get_acc_id());
    }

    std::vector<std::pair<int, int> > materials;
    materials.clear();
    if (!ex->extraction(user, (short)m.m_10, materials))
    {
        p.m_4 = 1;
        return 0;
    }

    std::vector<int> results;
    results.clear();
    if (!ex->inven_process(user, (short)m.m_10, materials, results))
    {
        p.m_4 = 0x11;
    }
    else
    {
        p.m_8 = (short)m.m_10;
        p.m_list = results;
    }
    return 0;
}

// ==== Dispatcher_ExpertExtraction::check_error (ORIG 0x081d962e) ====
int Dispatcher_ExpertExtraction::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    MSG_EXPERT_EXTRACTION& m = static_cast<MSG_EXPERT_EXTRACTION&>(msg);

    const int type = expert_extraction::GetExtracterType((ENUM_EXPERT_JOB_TYPE)m.m_0d);
    if (type == 3)
    {
        return 1;
    }
    expert_extraction::CExtraction* ex =
        GlobalData::g_ExtractionMgr->get_extracter((unsigned int)type);
    if (ex == 0)
    {
        return 1;
    }
    return ex->check_error(user, (char)m.m_0d, (short)m.m_0e, (short)m.m_10);
}

// ==== Dispatcher_ExpertExtraction::send (ORIG 0x081d9700) ====
void Dispatcher_ExpertExtraction::send(CUser* user, ParamBase& param)
{
    stExpertExtraction& p = static_cast<stExpertExtraction&>(param);
    if (p.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a0, (unsigned char)p.m_4);
        return;
    }

    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1a0);
    guard.put_byte(1);
    guard.put_short((int)p.m_8);

    const size_t count = p.m_list.size() / 3;
    guard.put_byte((int)count);
    for (size_t i = 0; i < count; ++i)
    {
        guard.put_short(p.m_list[i * 3 + 0]);
        guard.put_int(p.m_list[i * 3 + 1]);
        guard.put_int(p.m_list[i * 3 + 2]);
    }
    guard.finalize(true);
    user->Send(guard);
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
int Dispatcher_FatigueAttendance::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG：0xc(%ebp)=user 判空 → -1；msg 引用存帧后未消费（死存储，省略）。
    if (user == 0)
    {
        return -1;
    }
    if (user->getCurCharacR() == 0)
    {
        return -1;
    }
    if (user->get_state() != 3)
    {
        return 99;
    }
    EventClassify::CEventScriptMng* mng =
        (EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng();
    // ORIG：is_eventing(0x56) 取反检验；真 → 0，假 → 7。
    if (!mng->is_eventing(0x56))
    {
        return 7;
    }
    return 0;
}

// ==== Dispatcher_FatigueAttendance::read (ORIG 0x081dafa4) ====
int Dispatcher_FatigueAttendance::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_FATIGUE_ATTENDANCE& req = static_cast<MSG_FATIGUE_ATTENDANCE&>(msg);
    // ORIG：get_int(&msg+0xd)；失败走 LineFunc(0x49b3)。
    if (!packet.get_int(req.m_0d))
    {
        return LineFunc(0x49b3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_FatigueAttendance::process (ORIG 0x081daffa) ====
int Dispatcher_FatigueAttendance::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_FATIGUE_ATTENDANCE& req = static_cast<MSG_FATIGUE_ATTENDANCE&>(msg);
    // stFatigueAttendance 字段（现 m_pad[0x40] 占位；建议宿主字段化）：
    //   +0x04 int  errNo（check_error 结果，send 消费）
    //   +0x08 int  当日序号（= req.m_0d 回填，send 以 short 发出）
    //   +0x0c unsigned 打卡位图（setBit 后的 history.first 快照，send put_int）
    //   +0x10 int  今日时间值（get_today_time，send put_int）
    char* pres = reinterpret_cast<char*>(&param);

    // ORIG +0x008：param+8 = *(int*)(msg+0xd)（先于 check_error 回填请求当日序号）
    *reinterpret_cast<int*>(pres + 8) = req.m_0d;

    int err = check_error(user, msg);
    *reinterpret_cast<int*>(pres + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x49c5,
            __PRETTY_FUNCTION__,
            err, 0);
    }

    EventClassify::CEventScriptMng* mng =
        (EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng();

    unsigned char day = mng->get_today_time_day();          // +0x09d（uchar）
    int recordType = mng->get_record_type(0x56);            // +0x0bd

    // ORIG 0x081daffa：get_ingame_event_history(recordType, history) 读历史
    std::pair<int, int> history;
    user->get_ingame_event_history(recordType, history);
    unsigned int attendFlags = static_cast<unsigned int>(history.first);

    // 已打过当日卡（bit day-1 已置位）→ 0x12
    if (isSetBit_Uint(attendFlags, static_cast<int>(day) - 1))
    {
        *reinterpret_cast<int*>(pres + 4) = 0x12;
        return 0;
    }

    // 疲劳合计：server_fatigue_count(day)（short）+ fatigue_count_db(day)
    int totalFatigue = static_cast<int>(
        user->get_server_fatigue_count(day));
    totalFatigue += user->get_fatigue_count_db(static_cast<int>(day));

    EventClassify::CEventEntity* entity = mng->get_event_entity(0x56);   // +0x166
    if (entity == 0)
    {
        return 0;
    }

    int todayTime = mng->get_today_time();
    // ORIG：start > today || end < today → 不在活动期 → 7
    if (entity->get_event_start_time() > todayTime ||
        entity->get_event_end_time() < todayTime)
    {
        *reinterpret_cast<int*>(pres + 4) = 7;
        return 0;
    }

    // 疲劳超上限（fatigue_condition < total）→ 0x16
    EventClassify::CAttendance* attendance =
        static_cast<EventClassify::CAttendance*>(entity);
    if (attendance->get_fatigue_condition() < totalFatigue)
    {
        *reinterpret_cast<int*>(pres + 4) = 0x16;
        return 0;
    }

    // 置当日打卡位并回写历史（ORIG 0x081daffa：set_ingame_event_history(recordType, attendFlags, 0)）
    setBit_Uint(attendFlags, true, static_cast<int>(day) - 1);
    user->set_ingame_event_history(recordType, attendFlags, 0);

    *reinterpret_cast<unsigned int*>(pres + 0xc) = attendFlags;          // +0x24a
    int nowTime = mng->get_today_time();
    *reinterpret_cast<int*>(pres + 0x10) = nowTime;                      // +0x265

    int passDay = mng->get_pass_day(entity->get_event_start_time(), nowTime);
    attendance->send_attend_reward_item(user, passDay);                  // T 0x0810e878

    // ORIG 0x081daffa：process_update_ingame_history(user) 后 reset_ingame_event_history_update()
    mng->process_update_ingame_history(user);
    user->reset_ingame_event_history_update();

    int finalCondition = attendance->get_final_day_condition();
    attendance->send_attend_final_rward_item(user, static_cast<int>(attendFlags),
                                             finalCondition);            // T 0x0810e9c0
    return 0;
}

// ==== Dispatcher_FatigueAttendance::send (ORIG 0x081db392) ====
void Dispatcher_FatigueAttendance::send(CUser* user, ParamBase& param)
{
    char* pres = reinterpret_cast<char*>(&param);
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1ad);
    if (*reinterpret_cast<const int*>(pres + 4) == 0)
    {
        guard.put_byte(1);
        // ORIG 读 param+8 整型入栈，由 put_short 取低 16 位发出
        guard.put_short(*reinterpret_cast<const int*>(pres + 8));
        guard.put_int(*reinterpret_cast<const int*>(pres + 0xc));
        guard.put_int(*reinterpret_cast<const int*>(pres + 0x10));
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(static_cast<signed char>(*reinterpret_cast<const int*>(pres + 4)));
        guard.put_short(*reinterpret_cast<const int*>(pres + 8));
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


// ==== Dispatcher_ObjectBringUp::check_error (ORIG 0x081e16ce) ====
int Dispatcher_ObjectBringUp::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)                             // cmp $2,setle（P2）
    {
        return -2;
    }
    return 0;
}

// ==== Dispatcher_ObjectBringUp::read (ORIG 0x081e147e) ====
int Dispatcher_ObjectBringUp::read(PacketBuf& packet, MSG_BASE& msg)
{
    // PacketBuf::get_short(unsigned short&)（零扩展口径）
    if (!packet.get_short(
            *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&msg) + 0xe)))
    {
        return LineFunc(
            0x56aa,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
}

// ==== Dispatcher_ObjectBringUp::process (ORIG 0x081e14d4) ====
int Dispatcher_ObjectBringUp::process(CUser* user, MSG_BASE& msg,
                                      ParamBase& param)
{
    stObjectBringUp& out = static_cast<stObjectBringUp&>(param);

    EventClassify::CEventScriptMng* scriptMng = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    EventClassify::CObjectBringUp* entity =
        static_cast<EventClassify::CObjectBringUp*>(scriptMng->get_event_entity(0x19b));
    if (entity == 0)
    {
        out.m_4 = 0xb2;
        return 0;
    }

    if (!entity->is_eventing())                             // vtable+0x08 虚槽直调（P5）
    {
        out.m_4 = 0xb2;
        return 0;
    }

    const int err = check_error(user, msg);
    out.m_4 = err;
    if (err != 0)
    {
        return 0;
    }

    const unsigned short slot =
        *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const char*>(&msg) + 0xe);
    const Inven_Item slotItem =
        user->getCurCharacInvenR()->GetInvenSlot(1, static_cast<int>(slot));
    const unsigned int itemIdx = static_cast<unsigned int>(slotItem.m_addInfo);

    if (G_CDataManager()->find_item(static_cast<int>(itemIdx)) == 0)
    {
        out.m_4 = 0x11;
        return 0;
    }

    // ORIG 0x081e15e4: materialOk = entity->verifyMaterialItemByRule(user, itemIdx)
    const bool materialOk = entity->verifyMaterialItemByRule(user, itemIdx);
    if (!materialOk)
    {
        out.m_4 = 0x17;
        return 0;
    }
    user->incCurUseBringUpMaterialCount();                  // ORIG 调用点 0x0822fac4
    out.m_a = user->getCurUseBringUpMaterialTotal();        // ORIG 调用点 0x08110d12

    user->getCurCharacInvenW()->delete_item(
        static_cast<INVEN_TYPE>(1), static_cast<int>(slot), 1,
        static_cast<eItemDelReason>(3), true);

    // +0x0c 处 vector<int>：sendBringUpReward 输出（每轮消费 4 个 int）
    std::vector<int>& rewards =
        *reinterpret_cast<std::vector<int>*>(
            reinterpret_cast<char*>(&out) + 0xc);
    // ORIG 0x081e1660: rewardOk = entity->sendBringUpReward(user, rewards, out.m_b)
    const bool rewardOk = entity->sendBringUpReward(user, rewards, out.m_b);

    if (!rewardOk)
    {
        out.m_4 = 1;
        return 0;
    }

    out.m_8 = slot;
    return 0;
}

// ==== Dispatcher_ObjectBringUp::send (ORIG 0x081e1704) ====
void Dispatcher_ObjectBringUp::send(CUser* user, ParamBase& param)
{
    const stObjectBringUp& out = static_cast<const stObjectBringUp&>(param);

    PacketGuard pg;                                         // ORIG 无 clear（同汇编）
    pg.put_header(1, 0x1d5);

    if (out.m_4 == 0)
    {
        pg.put_byte(1);
        pg.put_short(static_cast<unsigned short>(out.m_8));
        pg.put_byte(static_cast<unsigned int>(out.m_a));
        pg.put_byte(static_cast<unsigned int>(out.m_b));

        if (out.m_b != 0)
        {
            // +0x0c 处 vector<int>：每轮消费 4 个 int（short,int,int,short）
            const std::vector<int>& rewards =
                *reinterpret_cast<const std::vector<int>*>(
                    reinterpret_cast<const char*>(&out) + 0xc);
            std::vector<int>::const_iterator it = rewards.begin();
            while (it != rewards.end())
            {
                const short v0 = static_cast<short>(*it); ++it;
                const int v1 = *it; ++it;
                const int v2 = *it; ++it;
                const short v3 = static_cast<short>(*it); ++it;
                pg.put_short(v0);
                pg.put_int(v1);
                pg.put_int(v2);
                pg.put_short(v3);
            }
        }
    }
    else
    {
        pg.put_byte(0);
        pg.put_byte(out.m_4 & 0xff);
    }

    pg.finalize(true);
    user->Send(pg);
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
int Dispatcher_RequestAssaultPrice::read(PacketBuf& packet, MSG_BASE& msg)
{
    MsgRequestAssaultPrice& m = static_cast<MsgRequestAssaultPrice&>(msg);
    if (!packet.get_short(reinterpret_cast<unsigned short&>(m.m_0d)))
    {
        return LineFunc(0x50bd, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}

// ==== Dispatcher_RequestAssaultPrice::process (ORIG 0x081dee80) ====
int Dispatcher_RequestAssaultPrice::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRequestAssaultPrice& p = static_cast<stRequestAssaultPrice&>(param);
    MsgRequestAssaultPrice& m = static_cast<MsgRequestAssaultPrice&>(msg);

    CUser* target = G_GameWorld()->find_from_world((unsigned short)m.m_0d);
    if (target == 0)
    {
        p.m_4 = 3;
        return 0;
    }
    if (target->get_state() <= 2 || target->getCurCharacR() == 0)
    {
        p.m_4 = 3;
        return 0;
    }

    const int needMoney = pvp_assault::GetInstanceAssaultMgr()
        ->GetRequestAssaultNeedMoney(user, target);
    p.m_8 = needMoney;
    if (p.m_8 > 0)
    {
        // ORIG:GetInvenSlot(inven, 0, 0xb),取返回 Inven_Item 的 +0x02(item idx)查物品,
        // 命中则按装备折扣重算费用:money + discount*money/(-100.0f)(float 截断取整)。
        Inven_Item slot = user->getCurCharacInvenR()->GetInvenSlot(0, 0xb);
        CItem* item = G_CDataManager()->find_item(slot.m_addInfo);
        if (item != 0)
        {
            const float discount = static_cast<CEquipItem*>(item)->getUpgradeCostDiscount();
            const float base = (float)p.m_8;
            p.m_8 = (int)(base + (discount * base) / -100.0f);
        }
    }

    user->SetCurCharacAssaultMoney(p.m_8);
    user->SetCurCharacAssaultedUserId((unsigned short)target->get_unique_id());

    const int err = check_error(user, msg);
    p.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x50f4, __PRETTY_FUNCTION__, err, 0);
    }
    return 0;
}

// ==== Dispatcher_RequestAssaultPrice::check_error (ORIG 0x081df084) ====
int Dispatcher_RequestAssaultPrice::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    MsgRequestAssaultPrice& m = static_cast<MsgRequestAssaultPrice&>(msg);

    CUser* target = G_GameWorld()->find_from_world((unsigned short)m.m_0d);

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x3c;
    }
    if (user->IsBusyDisjointStore())
    {
        return 0xbf;
    }
    if (user->IsCurCharacGhost())
    {
        return 0x13;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return 0x13;
    }
    if (target == 0)
    {
        return 3;
    }
    if (target->IsBusyDisjointStore())
    {
        return 0xc0;
    }

    // ORIG 0x082efe14：GetInstanceAssaultMgr()->check_party_teleport_user(4, user, target, false)
    int tpErr = pvp_assault::GetInstanceAssaultMgr()->check_party_teleport_user(4, user, target, false);
    if (tpErr > 0)
    {
        return tpErr;
    }

    const int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x37);
    if (secuErr != 0)
    {
        return secuErr;
    }

    if (target->get_state() > 4)
    {
        return 0x7fffffff;
    }

    // ORIG 分支语义:请求者非 GM 且目标为 GM → 拒绝（ORIG 无 bool 栈变量,直接判断）。
    if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id())
        && GlobalData::s_pGMAccounts_->isGM(target->get_acc_id()))
    {
        return 0x7fffffff;
    }

    // ORIG 0x082edfba：GetInstanceAssaultMgr()->OnRequestAssault(user, target, &tpErr)；
    // tpErr 与 check_party_teleport_user 返回值复用同一栈槽（ORIG -0x18）。
    pvp_assault::GetInstanceAssaultMgr()->OnRequestAssault(user, target, &tpErr);
    if (tpErr != 0)
    {
        return tpErr;
    }
    return 0;
}

// ==== Dispatcher_RequestAssaultPrice::send (ORIG 0x081df294) ====
void Dispatcher_RequestAssaultPrice::send(CUser* user, ParamBase& param)
{
    stRequestAssaultPrice& p = static_cast<stRequestAssaultPrice&>(param);

    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1bf);

    if (p.m_4 == 0)
    {
        guard.put_byte(1);
        guard.put_int(p.m_8);
    }
    else if (p.m_4 != (int)0x7fffffff)
    {
        guard.put_byte(0);
        guard.put_byte(p.m_4);
        if (p.m_4 == 10)
        {
            guard.put_int(p.m_8 - user->getCurCharacMoney());
        }
    }
    // ORIG:err==0x7fffffff 时既不 finalize 也不 Send,直接析构返回。

    if (p.m_4 != (int)0x7fffffff)
    {
        guard.finalize(true);
        user->Send(guard);
    }
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
int Dispatcher_RequestIngameAdvertisement::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;                                          // 0xffffffff
    }

    if (user->get_state() <= 1)                             // cmp $1,setle（P2 有符号比较）
    {
        return 0x7fffffff;
    }

    // msg+0xd：movzwl 零扩展后与 6 比较 → unsigned short 口径
    const unsigned short adType =
        *reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&msg) + 0xd);
    if (adType == 6 && user->get_multiboxLotteryItemFailCnt() < 3)
    {
        return -2;                                          // 0xfffffffe
    }

    return 0;
}

// ==== Dispatcher_RequestIngameAdvertisement::read (ORIG 0x081e434c) ====
int Dispatcher_RequestIngameAdvertisement::read(PacketBuf& packet, MSG_BASE& msg)
{
    if (!packet.get_short(
            *reinterpret_cast<short*>(reinterpret_cast<char*>(&msg) + 0xd)))
    {
        return LineFunc(
            0x5c10,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
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
int Dispatcher_RequestIngameAdvertisement::process(CUser* user, MSG_BASE& msg,
                                                   ParamBase& param)
{
    stRequestIngameAdvertisement* R =
        reinterpret_cast<stRequestIngameAdvertisement*>(&param);
    MSG_REQUEST_INGAME_ADVERTISEMENT& M =
        reinterpret_cast<MSG_REQUEST_INGAME_ADVERTISEMENT&>(msg);

    R->m_errorCode = check_error(user, msg);

    if (R->m_errorCode >= 1)
    {
        return 0;
    }
    if (R->m_errorCode < 0)
    {
        LineFunc(
            0x5c20,
            __PRETTY_FUNCTION__,
            R->m_errorCode, 0);
        return 0;
    }

    // err == 0 主流程
    R->m_adType = M.m_field0d;

    if (InGameADManagerInstance()->CheckIngameAdvertisementCondition(
            user, *reinterpret_cast<stNotifyIngameADInfo*>(&R->m_code),
            (int)M.m_field0d))
    {
        if (R->m_adShape == 4 && user->get_multiboxLotteryItemFailCnt() > 2)
        {
            if (R->m_prizeShape != 0 && R->m_itemID != 0)
            {
                sendRewardItemSystemMail(user, R->m_itemID, 1);
                if (R->m_logFlag != 0)
                {
                    InGameADManagerInstance()->WriteLog(
                        user, '\x03', M.m_field0d,
                        (char)R->m_adShape, R->m_code);
                }
            }
            else if (R->m_prizeShape == 0 && R->m_itemID2 != 0)
            {
                sendRewardItemSystemMail(user, R->m_itemID2, 1);
                if (R->m_logFlag != 0)
                {
                    InGameADManagerInstance()->WriteLog(
                        user, '\x03', M.m_field0d,
                        (char)R->m_adShape, R->m_code);
                }
            }

            user->set_multiboxLotteryItemFailCnt(0);        // 0x080fdf44
        }

        if (R->m_logFlag != 0)
        {
            InGameADManagerInstance()->WriteLog(
                user, '\0', M.m_field0d,
                (char)R->m_adShape, R->m_code);
        }
    }
    else
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(R) + 0x640) = 4;             // 条件不满足固定错误码
    }

    return 0;
}

// ==== Dispatcher_RequestIngameAdvertisement::send (ORIG 0x081e46c8) ====
void Dispatcher_RequestIngameAdvertisement::send(CUser* user, ParamBase& param)
{
    char* const P = reinterpret_cast<char*>(&param);
    const int err = *reinterpret_cast<const int*>(P + 0x640);

    if (err == 0x7fffffff)
    {
        return;                                             // check_error 未跑（state 门早退）
    }

    PacketGuard pg;
    if (err != 0)
    {
        // ---- 错误回包：byte 0 + err + type ----
        pg.clear();                                         // InterfacePacketBuf::clear
        pg.put_header(1, 0x1e9);
        pg.put_byte(0);
        pg.put_byte(err & 0xff);
        pg.put_short(*reinterpret_cast<const short*>(P + 0x644));
        pg.finalize(true);
        user->Send(pg);
        return;
    }

    // ---- 成功回包 ----
    pg.clear();
    pg.put_header(1, 0x1e9);
    pg.put_byte(1);
    pg.put_int(*reinterpret_cast<const int*>(P + 0x004));
    pg.put_short(*reinterpret_cast<const short*>(P + 0x644));
    // 以下 +0x33b/+0x33c/+0x33d 与文本区偏移均为 stNotifyIngameADInfo 视图（基址 P+4）
    pg.put_byte(static_cast<signed char>(*reinterpret_cast<const unsigned char*>(P + 0x33b)));

    {
        const int descLen =
            static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x008)));
        pg.put_int(descLen);
        pg.put_str(reinterpret_cast<const char*>(P + 0x008), descLen);
    }

    pg.put_byte(static_cast<signed char>(*reinterpret_cast<const unsigned char*>(P + 0x33d)));

    if (*reinterpret_cast<const unsigned char*>(P + 0x33d) == 0)
    {
        const int textLen =
            static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x33e)));
        pg.put_int(textLen);
        pg.put_str(reinterpret_cast<const char*>(P + 0x33e), textLen);

        const unsigned char kind =
            *reinterpret_cast<const unsigned char*>(P + 0x33b);
        if (kind == 0)
        {
            pg.put_int(*reinterpret_cast<const int*>(P + 0x53c));
        }
        else if (kind == 1)
        {
            const int len =
                static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x43d)));
            pg.put_int(len);
            pg.put_str(reinterpret_cast<const char*>(P + 0x43d), len);
        }
        else if (kind == 3)
        {
            const int len =
                static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x540)));
            pg.put_int(len);
            pg.put_str(reinterpret_cast<const char*>(P + 0x540), len);
        }
    }
    else
    {
        const int textLen =
            static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x03a)));
        pg.put_int(textLen);
        pg.put_str(reinterpret_cast<const char*>(P + 0x03a), textLen);

        const unsigned char kind =
            *reinterpret_cast<const unsigned char*>(P + 0x33b);
        if (kind == 0)
        {
            pg.put_int(*reinterpret_cast<const int*>(P + 0x238));
        }
        else if (kind == 1)
        {
            const int len =
                static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x139)));
            pg.put_int(len);
            pg.put_str(reinterpret_cast<const char*>(P + 0x139), len);
        }
        else if (kind == 3)
        {
            const int len =
                static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x23c)));
            pg.put_int(len);
            pg.put_str(reinterpret_cast<const char*>(P + 0x23c), len);
        }
    }

    pg.put_byte(static_cast<signed char>(*reinterpret_cast<const unsigned char*>(P + 0x33c)));
    pg.finalize(true);
    user->Send(pg);
}

// ==== Dispatcher_RequestIngameAdvertisement::sendRewardItemSystemMail (ORIG 0x081e4b6c) ====
// 头声明缺失：宿主需在 Dispatchers.h 补
//   void sendRewardItemSystemMail(CUser* user, unsigned long itemIdx, int count);
void Dispatcher_RequestIngameAdvertisement::sendRewardItemSystemMail(
    CUser* user, unsigned long itemIdx, int count)
{
    if (user == 0)
    {
        return;
    }

    CDataManager* dm = G_CDataManager();
    if (dm->find_item(static_cast<int>(itemIdx)) == 0)
    {
        return;
    }

    Inven_Item item;                                        // ORIG C1 W 0x080cb854
    item.m_addInfo = static_cast<int>(itemIdx);             // +0x02（item idx）
    dm->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, item, count);

    // RDAR 字符串：发件人名（≤0x1d）/ 正文（≤0xff），strncpy 不手动补 NUL（同 ORIG）
    char sender[0x1d];
    strncpy(sender,
            g_scriptStringManager_.findString(4, "game_server_msg_295", 0),
            0x1d);
    char body[0x100];
    strncpy(body,
            g_scriptStringManager_.findString(4, "game_server_msg_296", 0),
            0xff);

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item, 0u,
        static_cast<unsigned int>(user->getCurCharacNo()),
        body, static_cast<int>(strlen(body)), 0u,
        user->GetServerGroup(), false, false);
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
int Dispatcher_SeriaRoom_Ani_Deco_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_SERIAROOM_DECO_EVENT& req = static_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    stSeriaRoomDecoEvent& res = static_cast<stSeriaRoomDecoEvent&>(param);
    int err = check_error(user, msg);
    res.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x6551,
            __PRETTY_FUNCTION__,
            err, 0);
    }

    CDataManager* dcm = G_CDataManager();
    seriaRoom_AniDeco info = user->getSeriaRoomAniDecoInfo();   // +0x08d（4 字节值返回）

    // 进度上限：info.m_field2(有符号 word) >= maxAniDecoItem() → 0x14
    // （ORIG cmp %ax,%bx / setge：双方取低 16 位有符号比较）
    if (static_cast<short>(info.m_field2) >= static_cast<short>(dcm->maxAniDecoItem()))
    {
        res.m_4 = 0x14;
        return 0;
    }

    // 客户端指定槽位 = msg+0xe word（即 read 写入的 req.m_pad+1）
    unsigned short slot =
        req.m_decoEventId;

    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item slotItem = invenR->GetInvenSlot(static_cast<int>(INVEN_TYPE(1)),
                                               static_cast<int>(slot));

    // find_item(item idx) 缺定义 → 1
    CItem* itemDef = dcm->find_item(slotItem.m_addInfo);
    if (itemDef == 0)
    {
        res.m_4 = 1;
        return 0;
    }

    // info.m_field2 == -1 哨兵 → 1（ORIG movzwl -0x2e / cmpw $0xffff）
    if (info.m_field2 == -1)
    {
        res.m_4 = 1;
        return 0;
    }

    // 槽位有效性校验：(idx=info.m_field2, value=itemIdx)；假 → 0x17
    // （ORIG xor $1,%eax：真继续、假报错）
    if (!dcm->invalidAniDecoItem(static_cast<int>(static_cast<short>(info.m_field2)),
                                 slotItem.m_addInfo))
    {
        res.m_4 = 0x17;
        return 0;
    }

    // 成功路径：回显槽位 → 删除道具 → 进度+1 → 落库 → 事件处理
    res.m_8 = static_cast<short>(slot);                         // +0x18d

    CInventory* invenW = user->getCurCharacInvenW();
    invenW->delete_item(INVEN_TYPE(1), static_cast<int>(slot), 1,
                        static_cast<eItemDelReason>(3), true);

    seriaRoom_AniDeco updated = info;                           // m_field0 不变
    updated.m_field2 =
        static_cast<short>(static_cast<unsigned short>(info.m_field2 + 1));
    user->setSeriaRoomAniDecoInfo(updated);

    DB_UpdateSeriaRoomAnimationDeco::makeRequest(
        user->get_acc_id(), user->GetServerGroupToPvP(),
        user->getCurCharacNo(), info.m_field0, updated.m_field2);

    CEventManager* eventMgr = GlobalData::s_event_manager;
    CEventBase* evt = eventMgr->GetRepeatEvent(0x69);
    if (evt != 0 && evt->IsEventing(0))                         // vtable +0x34 槽
    {
        CEventBase* aniDecoEvent = eventMgr->GetRepeatEvent(0x69);
        if (aniDecoEvent != 0)
        {
            res.m_4 = static_cast<SeriaRoomAniDecoEvent*>(aniDecoEvent)
                          ->processEvent(*user);
        }
    }
    return 0;
}

// ==== Dispatcher_SeriaRoom_Ani_Deco_Event::send (ORIG 0x081e8b64) ====
void Dispatcher_SeriaRoom_Ani_Deco_Event::send(CUser* user, ParamBase& param)
{
    stSeriaRoomDecoEvent& res = static_cast<stSeriaRoomDecoEvent&>(param);
    PacketGuard guard;
    // ORIG 无 clear()（ctor 后直接 put_header），忠实保留
    guard.put_header(1, 0x201);
    if (res.m_4 == 0)
    {
        guard.put_byte(1);
        // param+8 word 零扩展入栈（movzwl/movzwl）
        guard.put_short(static_cast<int>(
            static_cast<unsigned short>(res.m_8)));
        seriaRoom_AniDeco info = user->getSeriaRoomAniDecoInfo();
        // -0xe word（m_field2）零扩展后 cwtl 符号扩展为 int 实参
        guard.put_short(static_cast<int>(static_cast<short>(info.m_field2)));
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(res.m_4);
    }
    guard.finalize(true);
    user->Send(guard);
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
int Dispatcher_Skill_Command_Customizing::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(m[0xd]))
    {
        return LineFunc(0x2ff8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    const char commandCount = m[0xd];
    for (int i = 0; i < (int)commandCount; ++i)
    {
        if (!packet.get_short(*reinterpret_cast<short*>(m + i * 8 + 0xe)))
        {
            return LineFunc(0x2fff,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(m[i * 8 + 0x10]))
        {
            return LineFunc(0x3002,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        const char keyCount = (char)m[i * 8 + 0x10];
        for (int j = 0; j < (int)keyCount; ++j)
        {
            if (!packet.get_byte(m[i * 8 + j + 0x11]))
            {
                return LineFunc(0x3009,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
        }
    }
    return 0;
}

// ==== Dispatcher_Skill_Command_Customizing::process (ORIG 0x081d2b42) ====
int Dispatcher_Skill_Command_Customizing::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char* m = reinterpret_cast<char*>(&msg);
    stSkillCommandParam& res = static_cast<stSkillCommandParam&>(param);

    const int commandCount = (int)(char)m[0xd];
    if (commandCount > 0x31)
    {
        // ORIG:count>49 → err=0xfc 后直接返回(不经错误日志/default 分支)
        res.m_4 = 0xfc;
        return 0;
    }

    for (int i = 0; i < commandCount; ++i)
    {
        // STSkillCommandData ctor ORIG W 0x0822db66(CUser.h 内嵌定义已一致)
        STSkillCommandData data;
        data.m_command = *reinterpret_cast<unsigned short*>(m + i * 8 + 0xe);
        data.m_count   = (unsigned char)m[i * 8 + 0x10];

        const int job = user->get_charac_job();   // CUserCharacInfo::get_charac_job
        // ORIG 走 const 重载 _ZNK12CDataManager10find_skillEii @0x0835fda2,
        // 实参=(job, 命令ID);经 const 指针调用以匹配 _ZNK 符号。
        const CDataManager* dm = G_CDataManager();
        CSkill* skill = dm->find_skill(job, (int)(short)data.m_command);
        if (skill == 0)
        {
            res.m_4 = 0xfc;
            break;
        }
        if (skill->isCommandCustomizing() != 1)   // CSkill::isCommandCustomizing W 0x0822b5d2
        {
            res.m_4 = 0xf9;
            break;
        }

        const char keyCount = (char)data.m_count;
        if (keyCount > 5 || keyCount < 1)
        {
            res.m_4 = 0xfb;
            break;
        }

        unsigned char keys[8];   // ORIG 帧 -0x29 起的命令键缓存(keyCount<=5 已证)
        bool keyRangeError = false;
        for (int j = 0; j < (int)keyCount; ++j)
        {
            keys[j] = (unsigned char)m[i * 8 + j + 0x11];
            if ((char)keys[j] < 0 || (char)keys[j] > 8)
            {
                res.m_4 = 0xfa;
                keyRangeError = true;
                break;   // ORIG:跳出内层后仍无条件执行末位键检查序列
            }
        }

        // 末位键检查:keys[keyCount-1] ∉ {0,1,2,3,7} 才放行(ORIG 0x081d2c92..0x081d2cfd)
        const unsigned char lastKey = keys[(int)keyCount - 1];
        if (lastKey == 0 || lastKey == 1 || lastKey == 2 ||
            lastKey == 3 || lastKey == 7)
        {
            res.m_4 = 0xfa;
            break;
        }

        // ORIG: cVar1 = customizingSkillCommand(user, &data);
        //       if (cVar1 != 1) { res.m_4 = 0xfc; break; }
        if (user->customizingSkillCommand(data) != 1)
        {
            res.m_4 = 0xfc;
            break;
        }
    }

    if (res.m_4 != 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp",
            __PRETTY_FUNCTION__,
            0x3051,
            "SKILL_COMMAND_LOG : CUSTOMIZING ERROR!! ALL COMMAND DEFAULT!! m_id (%s)",
            NumberToString(user->get_acc_id(), 0));
        user->defaultSkillCommand();
    }
    return 0;
}

// ==== Dispatcher_Skill_Command_Customizing::send (ORIG 0x081d2da4) ====
void Dispatcher_Skill_Command_Customizing::send(CUser* user, ParamBase& param)
{
    const stSkillCommandParam& res = static_cast<const stSkillCommandParam&>(param);
    if (res.m_4 == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x14c);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x14c);
        packet.put_byte(0);
        packet.put_short((int)res.m_4);
        packet.finalize(true);
        user->Send(packet);
    }
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
int Dispatcher_SoloTeleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SOLO_TELEPORT& req = static_cast<MSG_SOLO_TELEPORT&>(msg);
    unsigned int field30 = 0;
    if (!packet.get_int(field30))
    {
        return LineFunc(0x5778,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    req.m_30 = (int)field30;

    if (!packet.get_int(req.m_34))
    {
        return LineFunc(0x577b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (req.m_34 == -1)
    {
        return 0;   // 目标为 -1 时后续坐标字段不读
    }

    // +0x39..+0x40 即 stPartyTelePort 字段(PacketTypes.h MSG_SOLO_TELEPORT 布局)
    if (!packet.get_byte(req.m_tp.m_village))       // +0x39 unsigned char&
    {
        return LineFunc(0x577f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_tp.m_areaIndex))     // +0x3a unsigned char&
    {
        return LineFunc(0x5780,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_tp.m_posx))         // +0x3c short&
    {
        return LineFunc(0x5781,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_tp.m_posy))         // +0x3e short&
    {
        return LineFunc(0x5782,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_tp.m_direction))     // +0x40 unsigned char&
    {
        return LineFunc(0x5783,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_SoloTeleport::process (ORIG 0x081e1d9c) ====
int Dispatcher_SoloTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_SOLO_TELEPORT& req = static_cast<MSG_SOLO_TELEPORT&>(msg);
    stSoloTeleportParam& res = static_cast<stSoloTeleportParam&>(param);

    // ORIG:m_44 存全局槽地址(&g_pTeleport[0] 或 &g_pTeleport[1],即 Teleport**;
    // GlobalData.h:namespace GlobalData { extern void* g_pTeleport[2]; } @0x0941f818)
    req.m_44 = (int)(req.m_34 != -1 ? &GlobalData::g_pTeleport[0]
                                    : &GlobalData::g_pTeleport[1]);

    // ORIG 0x081e1dc7: STTeleportData data; data.m_field00=req.m_30;
    //   data.m_tp 前 10 字节 = req.m_tp(state..posy 按 4B/4B/2B 三段展开拷贝);
    //   (*Teleport**)(req.m_44)->set_teleport_data(data)（vtable 槽3 间接调用）
    STTeleportData data;
    data.m_field00 = req.m_30;
    data.m_tp = req.m_tp;
    (*reinterpret_cast<Teleport**>(req.m_44))->set_teleport_data(data);

    const int err = check_error(user, msg, param);
    if (err > 0)
    {
        res.m_4 = err;
    }
    else if (err < 0)
    {
        return -1;   // ORIG:err<0 → 返回 -1(dispatch_template result<0 → 不发包)
    }

    if (err == 0)
    {
        // ORIG 0x081e1e7b: (*Teleport**)(req.m_44)->set_end_cool_time(user)
        (*reinterpret_cast<Teleport**>(req.m_44))->set_end_cool_time(user);
        res.m_8 = req.m_30;   // ORIG:param+8 = msg+0x30(err==0 时)
    }

    res.m_c = req.m_44;       // ORIG:param+0xc = msg+0x44(无条件)
    return 0;
}
// ==== Dispatcher_SoloTeleport::check_error (ORIG 0x081e1e98) ====
// 注意:Dispatchers.h 现缺本声明,接入时须在类内补(见文件头)。
int Dispatcher_SoloTeleport::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    if (user == 0)
    {
        return -1;
    }
    MSG_SOLO_TELEPORT& req = static_cast<MSG_SOLO_TELEPORT&>(msg);
    if (req.m_30 == -1 && req.m_34 == -1)
    {
        return -1;
    }
    STCheckRuleData rule;
    rule.m_field00 = req.m_30;
    rule.m_user = user;
    Teleport* tp = *reinterpret_cast<Teleport**>(req.m_44);
    const int err = tp->check_rule(rule);
    if (err != 0)
    {
        return err;
    }
    stPartyTelePort& party = tp->get_party_teleport();
    return G_GameWorld()->check_move_area(
        user,
        static_cast<int>(party.get_teleport_village()),
        static_cast<int>(party.get_teleport_area_index()),
        static_cast<int>(party.get_teleport_posx()),
        static_cast<int>(party.get_teleport_posy()),
        static_cast<int>(party.get_teleport_direction()),
        true, 2);      // ORIG 末参 2（param==2 强制通过同村检查）
}

// ==== Dispatcher_SoloTeleport::send (ORIG 0x081e1ff6) ====
void Dispatcher_SoloTeleport::send(CUser* user, ParamBase& param)
{
    const stSoloTeleportParam& res = static_cast<const stSoloTeleportParam&>(param);
    PacketGuard packet;
    packet.clear();                 // InterfacePacketBuf::clear ORIG 0x080cb8e6
    packet.put_header(1, 0x1d7);
    if (res.m_4 == 0)
    {
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);

        Teleport* tp = *reinterpret_cast<Teleport**>(res.m_c);
        const bool ok = tp->move_area(user);
        const int charNo = user->getCurCharacNo();
        if (ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x57dd, 0);
            trace("SOLO_TELEPORT : teleport_success:(char_no:%d)", charNo);
        }
        else
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x57d9, 5);
            trace("SOLO_TELEPORT : teleport_error:(char_no:%d)", charNo);
        }
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte((int)(char)res.m_4);
        packet.put_int((*reinterpret_cast<Teleport**>(res.m_c))->get_end_cool_time());
        packet.finalize(true);
        user->Send(packet);
    }
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
int Dispatcher_TournamentRewardSelect::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 5)
    {
        return 0x7fffffff;
    }
    // GetPartyIndex 返回 short（ORIG movswl %ax 后入栈）
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return -1;
    }
    // ORIG cmp $0x2,%al：clear state == 2（选卡阶段）→ 0，否则 0x7fffffff
    if (party->GetTournamentDungeonClearState() != 2)
    {
        return 0x7fffffff;
    }
    return 0;
}

// ==== Dispatcher_TournamentRewardSelect::read (ORIG 0x081dffda) ====
int Dispatcher_TournamentRewardSelect::read(PacketBuf& packet, MSG_BASE& msg)
{
    MsgTournamentRewardSelect& req = static_cast<MsgTournamentRewardSelect&>(msg);
    // get_byte(char&) 直写 msg+0xd；随后域检验 [0,1]（js/cmp $1,jle 形态）
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))
    {
        return LineFunc(0x52fd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (static_cast<char>(req.m_0d) < 0 || static_cast<char>(req.m_0d) > 1)
    {
        return LineFunc(0x5300,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0e)))
    {
        return LineFunc(0x5303,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (static_cast<char>(req.m_0e) < 0 || static_cast<char>(req.m_0e) > 1)
    {
        return LineFunc(0x5306,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_TournamentRewardSelect::process (ORIG 0x081e00f0) ====
int Dispatcher_TournamentRewardSelect::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MsgTournamentRewardSelect& req = static_cast<MsgTournamentRewardSelect&>(msg);
    // stTournamentRewardSelect 字段（现 m_pad[0x40] 占位）：+0x04 int errNo
    int err = check_error(user, msg);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x5315,
            __PRETTY_FUNCTION__,
            err, 0);
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
        reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1)->m_tournamentReward;   // +0x17bc
    // 全员已选卡 → 0x7fffffff（isCardSelectComplete 第二参=卡组类型 2）
    if (reward.isCardSelectComplete(
            (ENUM_TOURNAMENT_REWARD_CARD_TYPE)2, members))
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = 0x7fffffff;
        return 0;
    }

    // msg+0xd/msg+0xe 为 signed byte 读入（movsbl）
    const unsigned int slotNo = party->GetMemberSlotNo(user);
    const bool selected = reward.SelectCard(
        (int)slotNo, (char)req.m_0d, (char)req.m_0e);
    if (!selected)
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = 0x7fffffff;
    }
    return 0;
}

// ==== Dispatcher_TournamentRewardSelect::send (ORIG 0x081e02b0) ====
void Dispatcher_TournamentRewardSelect::send(CUser* user, ParamBase& param)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1cb);
    if (*reinterpret_cast<const int*>(reinterpret_cast<const char*>(&param) + 4) == 0)
    {
        guard.put_byte(1);
        CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party != 0)
        {
            CTournamentDungeonReward& reward =
                reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1)->m_tournamentReward;   // +0x17bc
            reward.MakeSelectPacket(&guard);
            guard.finalize(true);
            party->send_to_party(guard);

            // 全员已接收后结算（ORIG test %al 前取反：假 → 直接析构返回）
            if (!party->CheckRecvFromAllMember(user))
            {
                return;   // 未收齐：仅析构返回，不结算
            }
            std::vector<int> members;
            party->GetValidPartyMember(members);
            if (reward.isCardSelectComplete(
                    (ENUM_TOURNAMENT_REWARD_CARD_TYPE)2, members))
            {
                bool victory = party->isTournamentVictory();
                party->SetTournamentDungeonClearState(3);
                party->TournamentDungeonClear(victory);
            }
        }
    }
    // err != 0 或 party 缺失：仅析构返回（不 finalize、不发送）
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
Dispatcher_StackableActionUse::Dispatcher_StackableActionUse() {}
Dispatcher_AvatarOptionChange::Dispatcher_AvatarOptionChange() {}
Dispatcher_PartyTeleport::Dispatcher_PartyTeleport() {}
Dispatcher_RefundSkill::Dispatcher_RefundSkill() {}
Dispatcher_MailBox_Send::Dispatcher_MailBox_Send() {}
Dispatcher_SecurityCardAuthRpy::Dispatcher_SecurityCardAuthRpy() {}
Dispatcher_BlueMarbleConfirmInfo::Dispatcher_BlueMarbleConfirmInfo() {}
Dispatcher_PcRoomRentItem::Dispatcher_PcRoomRentItem() {}

// ============================================================================
// DspD1 批 D1 依赖定义区（解阻 4 个 BLOCKED dispatch_sig；ORIG mangled/地址逐条注释）
// ============================================================================

// ---- SDC 伤害计算依赖（均经 ORIG nm 核实；定义归 SDC/伤害域批次，UNLINKED）----
extern float  getDamageInflict(STComputeDamageVariable*, void*);                       // T 0x08095193
extern int    isValidAttackInfoElement(STComputeDamageVariable*, int);                 // T 0x0809576e
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
int computeHpDamageSDC(STComputeDamageVariable* v, void* /*unusedArg2*/, void* defender)
{
    unsigned int randState = (unsigned int)v->m_randSeed;      // mov 0x1d9 → -0x5c
    int convertArg0 = 0;                                       // -0x58，恒 0【asm：仅初始化与读取】
    int convertArg1 = 0;                                       // -0x54，恒 0【asm：仅初始化与读取】

    float dmg = getDamageInflict(v, defender);                 // 0x08095c56；fstps -0x60

    // 0x08095c6b: m_attackElem != 4 且元素有效 → *1.05f（0x08b12f68）
    if (v->m_attackElem != 4 && isValidAttackInfoElement(v, (int)v->m_attackElem))
        dmg *= 1.05f;

    // 0x08095cb7: 防御方对象 0x11 且攻击元素无效 → 四元素取 max
    if (_SDC::isObjectType(v->m_defenderObjType, 0x11) && !isValidAttackInfoElement(v, 4))
    {
        float best = 1.0f;                                     // -0x4c
        for (int i = 0; i <= 3; ++i)                           // -0x48
        {
            if (v->m_elemEnable[i] == 0)
                continue;
            float x = dmg * ((g_commonElementBaseConst_
                              - (float)v->m_elemAbility[i] * v->m_f08F) * 0.01f);  // 0x08095d3a
            if (x > best)
                best = x;                                      // 0x08095d74
        }
        dmg = best;                                            // 0x08095d89
    }

    // 0x08095d8f: 随机基
    float randomBase = 0.0f;                                   // -0x50
    if (v->m_damageType == 0)
    {
        randomBase = g_physicalRandomBaseConst_;               // 0x08095da2
        int lvl = getComputeDamageBasisLevelAttacker(v, v->m_char004 != 0);  // -0x3c
        randomBase += (float)lvl * g_physicalRandomRateConst_;
    }
    else if (v->m_damageType == 1)
    {
        randomBase = g_magicalRandomBaseConst_;                // 0x08095deb
        int lvl = getComputeDamageBasisLevelAttacker(v, v->m_char004 != 0);  // -0x38
        randomBase += (float)lvl * g_magicalRandomRateConst_;
    }
    else if (v->m_damageType == 2)
    {
        randomBase = dmg * 0.07f;                              // 0x08095e34（0x08b12f6c）
    }
    if (v->m_objTypeByte == 0xE)                               // 0x08095e42
        randomBase = 0.0f;

    int r = (int)SDC_Rand(&randState);                         // 0x08095e5e
    int rMod = r % 1000;                                       // imul 0x10624dd3 有符号除法
    dmg += GetUniformVelocityFloatSDC(-randomBase, randomBase, rMod, 999);  // 0x08095ea3

    // 0x08095eb0: 防御率链
    if (_SDC::isObjectType(v->m_defenderObjType, 0x11) && v->m_damageType != 2 && v->m_char00E == 0)
    {
        float drA = 0.0f;                                      // -0x64
        float drB = 0.0f;                                      // -0x68
        getDefenseRateDamager(drA, drB, v, 0);                 // 0x08095f23
        float m = (1.0f - drA) - v->m_f05F * drB;              // -0x34
        // 0x08095f3f/0x08095f60: 两处 ORIG 等值自赋（fucomip 形态）【asm 事实】
        if (m == g_maxTotalDefenseRate)
            m = g_maxTotalDefenseRate;
        if (m == 0.0f)
            m = 0.0f;
        dmg *= m;                                              // 0x08095f7b

        if (v->m_damageType == 0)
        {
            float s1 = v->m_f0AB;                              // 0x08095f8f；-0x30
            if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
                s1 *= getAbilityConvertRateSDC((void*)convertArg0, CONVERT_TABLE_4);
            float s2 = v->m_f03F;                              // 0x0809602d；-0x28
            if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
                s2 *= getAbilityConvertRateSDC((void*)convertArg1, CONVERT_TABLE_3);
        }
        else if (v->m_damageType == 1)
        {
            float s1 = v->m_f0AF;                              // 0x08095fe2；-0x2c
            if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
                s1 *= getAbilityConvertRateSDC((void*)convertArg0, CONVERT_TABLE_6);
            float s2 = v->m_f043;                              // 0x0809607e；-0x24
            if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
                s2 *= getAbilityConvertRateSDC((void*)convertArg1, CONVERT_TABLE_5);
            (void)v->m_char1EA;                                // 0x080960be 读取未用【asm 事实】
        }
    }

    if (v->m_damageType == 0)                                  // 0x080960ca 物理分支
    {
        float final1 = v->m_f00F[0];                           // 0x080960d9
        if (v->m_char004 != 0)
            final1 += (float)v->m_field063 * v->m_f0B3;
        final1 *= (v->m_f006 * 0.01f + 1.0f);                  // 0x0809610c
        final1 *= (v->m_f037 * 0.01f + 1.0f);                  // 0x08096126
        final1 *= getAbilityConvertRateSDC((void*)convertArg1, CONVERT_TABLE_7);  // 0x08096140
        dmg += final1;

        if (_SDC::isObjectType(v->m_defenderObjType, 0x11) && v->m_char00E == 0)
        {
            float mdef = (float)v->m_physMDefBase * v->m_f05F + (float)v->m_physMDefAdd;  // 0x08096196
            dmg = dmg - mdef * getAbilityConvertRateSDC((void*)convertArg0, CONVERT_TABLE_8);
        }
        dmg = dmg - dmg * v->m_physReduceRate;                 // 0x080961ef
    }
    else if (v->m_damageType == 1)                             // 0x08096211 魔法分支（对称）
    {
        float final1 = v->m_f00F[0];                           // 0x08096219
        if (v->m_char004 != 0)
            final1 += (float)v->m_field067 * v->m_f0B3;
        final1 *= (v->m_f006 * 0.01f + 1.0f);                  // 0x0809624c
        final1 *= (v->m_f03B * 0.01f + 1.0f);                  // 0x08096266
        final1 *= getAbilityConvertRateSDC((void*)convertArg1, CONVERT_TABLE_9);  // 0x08096280
        dmg += final1;

        if (_SDC::isObjectType(v->m_defenderObjType, 0x11) && v->m_char00E == 0)
        {
            float mdef = (float)v->m_magicMDefBase * v->m_f05F + (float)v->m_magicMDefAdd;  // 0x080962d6
            dmg = dmg - mdef * getAbilityConvertRateSDC((void*)convertArg0, CONVERT_TABLE_A);
        }
        dmg = dmg - dmg * v->m_magicReduceRate;                // 0x0809632f
    }

    if (dmg == 0.0f)                                           // 0x08096345 等值自赋【asm 事实】
        dmg = 0.0f;

    v->m_isCritical = (unsigned char)checkIsCriticalSDC(v, &dmg, &randState);  // 0x08096360

    if (v->m_char1CF != 0)                                     // 0x08096384 暴击扣血段
    {
        // ORIG 0x08096392：m_critDamage = (int)(dmg*0.05f - dmg)（fsubrp 下-上实测定案，负值忠实保留）
        v->m_critDamage = (int)(dmg * 0.05f - dmg);
        dmg = dmg * 0.05f;                                     // 0x080963c7
    }

    dmg *= v->m_f00F[3] * 0.01f;                               // 0x080963d0（0x1b = f[3]）
    dmg *= (float)v->m_field1D5 * 0.01f;                       // 0x080963e6
    if (v->m_char0D3 != 0)                                     // 0x08096405 f[4]=0x1f
        dmg *= v->m_f00F[4] * 0.01f;
    if (v->m_char0D4 != 0)                                     // 0x08096429 f[5]=0x23
        dmg *= v->m_f00F[5] * 0.01f;
    if ((unsigned char)v->m_field1D5 != 0)                     // 0x0809644d f[6]=0x27（movzbl 低字节）
        dmg *= v->m_f00F[6] * 0.01f;
    dmg *= v->m_f06B;                                          // 0x08096471

    if (dmg == 1.0f)                                           // 0x0809647f 等值自赋【asm 事实】
        dmg = 1.0f;

    if (_SDC::isObjectType(v->m_field1C6, 0x111))              // 0x0809649a 暴击矩阵 26 项
    {
        const int row = v->m_isCritical ? 1 : 0;
        for (int i = 0; i <= 0x19; ++i)                        // i ≤ 25
            dmg *= v->m_critMatrix[row][i];
    }

    if (_SDC::isObjectType(v->m_attackerObjType, 0x11) && v->m_f077 != 1.0f)  // 0x08096506（NaN 计入不等）
        dmg *= v->m_f077;                                      // 0x08096546
    if (_SDC::isObjectType(v->m_attackerObjType, 0x11) && v->m_field07B != 0)
        dmg += (float)v->m_field07B;                           // 0x0809658b
    dmg *= v->m_f1A6;                                          // 0x0809659f

    if (v->m_char1DF != 0 && _SDC::isObjectType(v->m_attackerObjType, 0x111))  // 0x080965b0
    {
        dmg *= 0.25f;                                          // 0x080965eb（0x08b12f70）
        int n = (int)dmg;                                      // trunc；-0x70
        dmg += (float)n;                                       // 0x0809660c
    }
    if (v->m_char1E0 != 0)
        dmg += (float)v->m_field1E1;                           // 0x0809662b
    if (v->m_char1EF != 0 && _SDC::isObjectType(v->m_attackerObjType, 0x111))
        dmg *= v->m_f1F0;                                      // 0x0809667d
    dmg += dmg * v->m_f1F8 / 100.0f;                           // 0x0809668e（fdivrp 下/上实测）
    dmg += v->m_f1F4;                                          // 0x080966ac

    v->m_expectedHp = (int)dmg;                                // 0x080966bd trunc 写入
    return (int)dmg;                                           // 0x080966e2 trunc 返回
}

// ---- Packet_Mannerless_User_Accusation ctor ----
// ORIG W _ZN33Packet_Mannerless_User_AccusationC1Ev @0x0822c1e6（0xaf）。
// PacketHeader(type=0x1b66, size=0x15f) 经 asm 桥（ORIG W _ZN12PacketHeaderC1Ett @0x08110b70）。
extern "C" void _ZN12PacketHeaderC1Ett(void* self, unsigned short type, unsigned short size);

// ORIG W 0x0822c1ca（_ZN 非 const）：(unsigned)type <= 2。
bool Packet_Mannerless_User_Accusation::isNeedTypeForChattingMsg(int type)   // ORIG 静态：无 this
{
    if ((unsigned int)type <= 2)   // ORIG: ja 分支结构（残留 add $0 为编译器形式差）
        return true;
    return false;
}

Packet_Mannerless_User_Accusation::Packet_Mannerless_User_Accusation()
{
    _ZN12PacketHeaderC1Ett(this, 0x1b66, 0x15f);               // 0x0822c202
    m_characNo = -1;                                           // 0x0822c20a：+0xa = -1
    m_type = 0;                                                // 0x0822c214：+0xe = 0
    m_serverGroup = 4;                                         // 0x0822c21e：+0x12 = 4
    m_accIdHi = 0;                                             // 0x0822c228：+0x1e = 0
        memset(m_name, 0, 0x1e);                                   // 0x0822c235：+0x22[30] 清零
    memset(m_targetName, 0, 0x1e);                             // 0x0822c24a：+0x40[30] 清零（NEW 曾漏）
            memset(reinterpret_cast<char*>(this) + 0x5e, 0, 0x100);    // 0x0822c26b：+0x5e[0x100]（field5d+chatMsg）
    reinterpret_cast<char*>(this)[0x15e] = 0;                  // ORIG movb $0,0x15e
}

// ---- DB_SecurityCardRetransfer::makeRequest ----
// ORIG T _ZN25DB_SecurityCardRetransfer11makeRequestEiPcS0_jPKcS2_S2_ @0x0842c1f6（0x16b）。
// W8LT_DBTask.cpp 占位类已产生 C1；本 TU 提供唯一定义。静态成员（无 this）【asm 核实】。

void DB_SecurityCardRetransfer::makeRequest(
    int type, char* characName, char* targetName, unsigned int secCardKey,
    char const* src1, char const* src2, char const* src3)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6f38), true);
    **guard << (int)0x138;
    **guard << (int)type;
    SIG_SECURITY_CARD_RETRANSFER* sig = guard->GetInBuffer<SIG_SECURITY_CARD_RETRANSFER>();
    *(unsigned int*)sig = secCardKey;                          // 0x0842c28a：sig+0x00
    strncpy((char*)sig + 0x25, src1, 6);                       // 0x0842c295
    strncpy((char*)sig + 0x04, src2, 0x21);                    // 0x0842c2b2
    strncpy((char*)sig + 0x2b, src3, 0x27);                    // 0x0842c2cf
    strncpy((char*)sig + 0x52, characName, 0xc);               // 0x0842c2ec
    strncpy((char*)sig + 0x5e, targetName, 0x29);              // 0x0842c309
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);  // 0x0842c320
}

// ---- D3 解阻依赖：DB_RequestJoinGuild::makeRequest(int, SIG_REQUEST_JOIN_GUILD*) ----
// ORIG T _ZN19DB_RequestJoinGuild11makeRequestEiP22SIG_REQUEST_JOIN_GUILD @0x0843d19a（0xec）。
// W8LT_DBTask.cpp:378 占位类已产生 C1；类声明见 TU 顶部（§8 全禁），本 TU 提供唯一定义。
void DB_RequestJoinGuild::makeRequest(int userIdx, SIG_REQUEST_JOIN_GUILD* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x91a8), true);
    guard.operator*()->operator<<((int)0x217);             // 0x0843d1c0：opcode
    guard.operator*()->operator<<((int)userIdx);           // 0x0843d1cc
    SIG_REQUEST_JOIN_GUILD* inbuf = guard->GetInBuffer<SIG_REQUEST_JOIN_GUILD>();
    memcpy(inbuf, sig, 0x44);                              // 0x0843d1e2
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard); // 0x0843d1f6：QUEUE_IDX 2 = DB_Q
}


// ==== StubS3A.S3A_DEPS：check_error×2 + isVaildEmotionValue（ORIG T 0x081d14aa/0x081d18dc/0x081d1ebe）====

int Dispatcher_GuildCargoPopItem::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
    {
        int guildkey = user->get_charac_guildkey();     // ORIG 0x080da38c 后链
        if (guildkey != 0)
        {
            if (user->IsExistGuildAgit())               // ORIG 0x082301d8
            {
                int secu = GlobalData::s_pSecuProtectionField->Check(
                    user, (SECURITY_PROTCTION)9);       // ORIG 0x08288a02
                if (secu != 0)
                    return secu;
                int itemIdx =
                    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 0xf);
                const CInventory* inven = user->getCurCharacInvenR();
                if (!inven->possible_to_insert(itemIdx))   // ORIG 0x0850562a
                    return 0xcb;
                if (G_CDataManager()->find_item(itemIdx) == 0)   // ORIG 0x0835fa32
                    return 0xc5;
                return 0;
            }
            return 0xb5;
        }
        return 100;
    }
    return -1;
}

int Dispatcher_GuildCargoMoveItem::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
    {
        if (user->get_charac_guildkey() != 0)
        {
            if (user->IsExistGuildAgit())
                return 0;
            return 0xb5;
        }
        return 100;
    }
    return -1;
}

int Dispatcher_ChangeEmotion::isVaildEmotionValue(short value)
{
    const std::vector<int>& tags = G_CDataManager()->m_emotionTags.m_tags;
    for (std::vector<int>::const_iterator it = tags.begin(); it != tags.end(); ++it)
    {
        if (*it == (int)value)
            return 1;
    }
    return 0;
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
bool Dispatcher_BuyCeraShopItem::useCountDownCoinInFreeCoinDungeon(
    CUser* user, MSG_BUY_CERASHOP_ITEM const& msg)
{
    if (user == 0)
    {
        return false;
    }
    if (*reinterpret_cast<const unsigned char*>(
            reinterpret_cast<const char*>(&msg) + 0x36) != 1)
    {
        return false;
    }
    CCeraShopGoods* goods = reinterpret_cast<CCeraShopGoods*>(G_CDataManager()->FindGoods(
        *reinterpret_cast<const int*>(reinterpret_cast<const char*>(&msg) + 0x39)));
    if (goods == 0)
    {
        return false;
    }
    if (goods->GetItemIdx() != 1)
    {
        return false;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return false;
    }
    CDungeon* dungeon = party->m_dungeon;
    if (dungeon == 0)
    {
        return false;
    }
    return party->checkFreeRevivalCondition(dungeon) ? true : false;
}

// ===== Dispatcher_Compound_Equipment_Upgrade_Card::set_ReturnSendPacket —— ORIG 0x08228a2e =====
// ORIG：put_byte(0) → put_short(param_2=slot) → put_int(param_3=quantity)。
void Dispatcher_Compound_Equipment_Upgrade_Card::set_ReturnSendPacket(
    PacketGuard& pg, unsigned short slot, int quantity) const
{
    pg.put_byte(0);
    pg.put_short((int)slot);
    pg.put_int(quantity);
}

// ===== Dispatcher_Enchant_3rdChronicleItem::check_state —— ORIG 0x082328ec =====
// ORIG：state!=3 或 getCurCharacR()==0 → LineFunc(0x5f6)；CheckInTrade() → LineFunc(0x5f9)；
// 通过 → 0（失败路径返回值即 LineFunc 返回）。
int Dispatcher_Enchant_3rdChronicleItem::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(
            0x5f6,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (user->CheckInTrade())
    {
        return LineFunc(
            0x5f9,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
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
void Dispatcher_ModItemAttr::_SendResult(CUser* user, unsigned short resultType,
                                         unsigned short matSlot, unsigned short equipSlot)
{
    Inven_Item item =
        user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (int)matSlot);

    PacketGuard packet;
    packet.put_header(1, 0x54);
    packet.put_byte(1);
    packet.put_short((int)matSlot);
    packet.put_int(item.get_add_info());
    packet.put_short((int)resultType);
    packet.finalize(true);
    user->Send(packet);
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)equipSlot);
}

// ===== Dispatcher_PurifyItem::processPurifyType —— ORIG 0x082194b6 =====
// ORIG：type==0 → identify(amp@item+0x11) 的返回值（W 0x0822b3d8 实返 bool：
//   已鉴定→0；未鉴定→清 bit7 后 1）；type==1 → reset(amp) 后 1；其它 → 0。
int Dispatcher_PurifyItem::processPurifyType(int purifyType, Inven_Item& item)
{
    if (purifyType == 0)
    {
        // ORIG 0x082194b6：直接 call stAmplifyOption_t::identify 后原样返回 eax
        return item.m_amp.identify() ? 1 : 0;
    }
    if (purifyType == 1)
    {
        item.m_amp.reset();
        return 1;
    }
    return 0;
}

// ===== Dispatcher_Select_3rdChronicleItem_ForEnchant::check_state —— ORIG 0x08232854 =====
// ORIG：state!=3 或 getCurCharacR()==0 → LineFunc(0x5e1)；CheckInTrade() → LineFunc(0x5e4)；
// 通过 → 0（失败路径返回值即 LineFunc 返回）。
int Dispatcher_Select_3rdChronicleItem_ForEnchant::check_state(
    CUser* user, PacketBuf& /*packet*/)
{
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(
            0x5e1,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (user->CheckInTrade())
    {
        return LineFunc(
            0x5e4,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
}

// ===== DimensionActivationRewardData::getRewardGiveItems —— ORIG 0x08915f9a =====
// ORIG：list=getRewardGiveItemList(job, growType)；list 非空且
//   selectionIdx>=0 且 selectionIdx<=list->size()（ORIG cmp/jl 边界）→
//   返回 &list->at(selectionIdx)（ORIG 调 at，idx==size 时由 at 抛异常）；否则 0。
std::vector<EventRewardItemInfo>* DimensionActivationRewardData::getRewardGiveItems(
    int selectionIdx, int job, int growType)
{
    std::vector<std::vector<EventRewardItemInfo> >* list =
        getRewardGiveItemList(job, growType);
    if (list != 0 && selectionIdx >= 0 && selectionIdx <= (int)list->size())
    {
        return &list->at(selectionIdx);
    }
    return 0;
}

