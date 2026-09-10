#include "InterDispatcher.h"
#include "Inter_DispatcherAradOnly.h"  // Inter_DispatcherAradOnly（槽 732）
#include "Inter_DispatcherTaiwan.h"  // Inter_DispatcherTaiwan（槽 846）

// ============================================================================
// b20 hardcode 裸偏移清零账（本 TU，§4.3）
// 已字段化·零扰动（check_tu 计数不变，12 处）：
//   Inter_MailBox_Req_CheckReceiver +0x3fb9→m_resultFlag；Inter_LoadInven +0x14758→&m_14758、
//   +0x4044→&m_4044（拆 pad 保 sizeof）；Inter_MonitorCallMemberList +0x12→&m_12；
//   Inter_GuildCargoPushItem +0x42→m_42、Inter_GuildCargoPopItem +0x41→m_41（UpgradeSeparateInfo）；
//   Inter_LoadDeathTowerBestRecord +0x10→&m_10、Inter_DeathTowerLoadTopRanker +0xfb0→&m_padfb0；
//   Inter_NoticeGuildNameOnGuildCreate +0x16→m_pad16、Inter_UpdateSecuServiceState +0x10→m_pad10。
// 记 N·保留裸偏移（字段化实测破坏 AE/无法零扰动，各因如下，逐处见行内注释）：
//   · 视图边界外子结构基址（补字段使 sizeof(view) 变化→尺寸敏感装载破坏 AE）：
//     LoadCharac +0x68b(TAG_CHARAC_DATA)、LoadDungeonClear +0x10、NotifyAuctionMail +0x18(RandomOption)、
//     MonitorCallGuildMembers/AllMembers +0x34、MembersNext/AllMembersNext +0x17（ST_Guild_Mem_Info*）、
//     RequestTagCharacInfo +0x7d(TAG_CHARAC_INVEN_DATA)/+0x40a1(CHARAC_QUEST_SHOP_DATA)。
//   · 透传/外部结构视图：MailBox CheckReceiver m_payload(+0x35/0x46/0x63/0x64/0x67/0x68/0x6c/0x6e，
//     MSG_MAILBOX_SEND/MULTI 视图)、IPGResult reqData+0x2d8(SIPGData)、GameOption etcOpt+0x48/0x174/0x2a0
//     (CETCOption 内 CQuickChattingOption/CChattingEmoticonConfig)。
//   · 工具链求值序依赖：MonitorAddBuddy strlen (char*)pPkt+0x12（ORIG add 形 vs put_str lea 形，AE 依赖此差）。
//   · 多指针参 reload：SecurityCardIssue +0x37/+0x5e（拆 pad 使 setSecurityCardInfo 双指针参装载重排破坏 AE）。
//   · 最小镜像类隔离：NpcLimitBuyItemInfo checkError +0x16（LimitNpcBuyItemInfo 无字段，§4.3 例外同 CItemLock）。
//   · 共享头占位待属主：AuctionResultBidding PrePayInfo +0x08/0x0c/0x10/0x18/0x1c/0x24
//     （CPrePayInfoMgr.h m_data[0x28]，字段化需 pack(1)+u64 对齐验证、超本 TU 证据，交属主批次）。
// ============================================================================

// ============================================================================
// b28d-lane2 round1（2026-09-10）—— 本 TU 反模式收敛账（子代理 L2，唯一写者）
// 四数：identical 135 / AE 1354→1363 / near 5→1 / diff 127→122（check_tu_routed 实测）。
// 手法（均已 fresh 复编转 AE）：
//  ① **N22 比较操作数方向旧判词作废**：ORIG `mov <field>,%ebx; …call…; cmp %eax,%ebx`
//     只在「字段读**内联进比较**、无具名/register 局部」时复现（探针 /tmp/l2p/p2.cpp fC：
//     裸形 fC 产 `cmp %eax,%ebx`，register/int 具名局部 fA/fB/fD/fF 恒产 `cmp %ebx,%eax`
//     或 mem 直比）。属 P0（ORIG 无槽往返 ⇒ 原码无该局部）。已修 5 件：
//     Inter_NoticeGuildMemo 0x084dad96 / Inter_PowerWarProcessInfo 0x084daf30 /
//     Inter_GuildNoticeGuildMessage 0x084cc650 / Inter_MonitorGuildMemberNamingReply 0x084cc23c /
//     Inter_TodayGuildMember 0x084e37d0（后者还需视图字段 int→unsigned 同型化，
//     P248 档②：ORIG `mov 0xe(%eax),%ebx` 单段 = 字段与 getCurCharacNo() 同型）。
//  ② **删冗余具名 proxy 局部**（P183/P0）：ORIG 的 GetServerProxy 结果直落 (%esp)，
//     具名指针占 4B 并多发 store/load ⇒ Inter_DeleteCharac 0x084bfa2e、
//     Inter_FindFactoryHub 0x084dc9f4、Inter_ChangeGuildName 0x084d50f6（三件转 AE）。
//  ③ **P198 孪生判据**：`int len = 0x33;` 初始化式被 4.4 折成立即数（丢槽）；声明与赋值
//     分两句复现 ORIG `movl $0x33,-0xc(%ebp)` + `mov -0xc(%ebp),%edx`（Inter_FindFactoryHub）。
//  ④ 比较内联化（logFormat 类）：Inter_ReplyJoinPower 0x084d47f4 / Inter_SecedePower 0x084d49f6
//     的 ORIG 比较点无 characNo 槽（`call; mov -0x10(%ebp),%eax; mov 0x12(%eax),%eax; cmp`），
//     旧具名 characNo 多发 store+load 各一。
// 残余族（判据⑪ 判别后归 N；每条附 ORIG 地址 + 指令形）：
//  · R1 视图 disp 折叠族：ORIG 对 `base+i*0x14+K` 发 `lea (%ecx,%eax,1); add $K; mov (%eax)`，
//    4.4 折 `mov K(%eax)`（每处 1 条）。实证 Inter_AutoPunishRuleHackType 0x084d44e0 段
//    （11×add；ORIG 源码为 uint* 索引形）、Inter_NoticeGuildCargo 0x084dd4b2
//    （`add $0x10,%eax; mov 0x3(%eax)` 子结构基址 + disp）。修需把常量并入数组下标或嵌套
//    子结构视图（本波未收敛，P30/N107 族）。
//  · R2 N7/N18a 寄存器角色族：ORIG `mov %eax,%edx`（call 结果落 edx）+ 字段落 %eax，
//    4.4 恒相反（Inter_SecedePower 0x084d49f6）；Inter_MailBox_Req_List 0x084cf845
//    `mov 0x3332(%eax),%ebx` 单段 vs 我方两段（字段类型在权威头 SIG_MAILBOX_LIST，
//    跨 TU 改动需属主 —— 属 P265 反向，后端不成立）。
//  · R3 分支布局族：ORIG 后向 goto 用真跳转 `setg; test %al,%al; jne <body>`，
//    4.4 恒取反 `je <exit>; jmp <body>`（语义等价、仅 jcc 助记符与 jmp 目标互换）。
//    实证 Inter_TcpServerConnect：ORIG 0x084c8e44 `jne 0x84c8dfa` / 我方 `je <exit>`。
//  · R4 N126 整帧平移族：帧内相对间距不变、整体差常数 ⇒ 声明重排无效。
//    Inter_MonitorEventItemUpdate 0x084cefd2：ORIG pPkt@-0x1c / v1a@-0x40 / v16@-0x3c
//    vs 我方 -0x24/-0x20/-0x1c（帧 0x6c vs 0x5c，整组浅移 8）。
//  · R5 P292/P168a③ 跨版本 bool 归一化：ORIG 多一条 `xor $1,%eax; test %al,%al` 死物化，
//    4.4 以 `movzbl` 加宽顶掉（本 TU 表现为 `movzbl`/`setne` 助记符差）。
// ============================================================================

// ORIG 各 TU 一份 _ZL26GUILD_ADDRESS_DOUBLE_CHECK（r 0x2）：公会地址重复检查通过标志。
static const int GUILD_ADDRESS_DOUBLE_CHECK = 4;

// P254 视图字段枚型（本 TU）：ORIG 0x084d2fa6 / 0x084ca02f / 0x084ca1xx / 0x084c9798 等段
// 对包视图 4 字节字段发 `mov disp(%eax),%eax; mov %eax,%edx`（4.1 对枚举字段保留 eax→edx
// 中转）；同址字段声明为 int 时 4.4 恒折单段 `mov disp(%eax),%edx`（每处 -1 条 ⇒ DIFF）。
// 枚举类型不入 mangled、不改布局、不发射符号（P254 三例实证）。
enum ENUM_VIEW_FIELD { ENUM_VIEW_FIELD_0 = 0 };

// dispatch 主体依赖（均为 ORIG 真实符号；权威声明处 include）
#include "CUser.h"             // CUser::DisConnSig / get_acc_id / DISCONN_SIG
#include "CGameManager.h"      // G_CGameManager()->getUser(int,int)
#include "LogManager.h"        // LogManager::logFormat
#include "cHistoryTrace.h"     // cMyTrace

// getUser(unsigned int) —— ORIG T _Z7getUserj @0x084bb9cf（LinkE2 批转写）。
// G_CGameManager()->m_userByAccId2（CGameManager +0x244，map<unsigned,CUser*>）
// find(uid)；end → 0，否则 it->second。
CUser* getUser(unsigned int uid)
{
    // ORIG 0x084bb9cf：无 m 引用槽，find/end 各自 call G_CGameManager + lea 0x244(%eax)（P48 双臂独立求值）
    std::map<unsigned int, CUser*>::const_iterator it = G_CGameManager()->m_userByAccId2.find(uid);
    if (it == G_CGameManager()->m_userByAccId2.end())
        return 0;
    return it->second;
}

// NumberToString —— ORIG T（_Z14NumberToStringji）；全局声明见 CInventory.h，
// 此处按 CGameManager.cpp 同例作 TU 内声明避免引入重型头。
char* NumberToString(unsigned int value, int radix);
#include <new>
#include <cstdio>              // puts (Inter_Terminate)
#include <cstdlib>             // exit (Inter_Terminate)
#include "GlobalData.h"        // B2 dispatcher singleton pointers
#include "nexon_cash.h"       // nexon::cash::MAX_NEXON_ID_LENTH
#include "CPowerManager.h"     // PowerWar result consumers
#include "CInventory.h"         // Inven_Item/RandomOption/CInventory 动作族 + user_creature::CCreatureMgr
#include "CExpandEquipslot.h"   // CExpandEquipslot::loadData 虚槽转真调用（GetCharacExpandData(2/9)）
#include "CAvatarItemMgr.h"     // WongWork::CAvatarItemMgr（jewel/expansion/tempKey）
#include "CItemLimitEdition.h"  // CItemLimitEdition/Mgr（ImplementItemLimitEdition 批权威实现）
#include "CCeraShop.h"          // WongWork::CCeraShop::IsSpecialItem
#include "CStreamGuard.h"       // CStreamGuard/StreamPool::Acquire
#include "SigTypes.h"           // stCeraShopIPGRequest_t（GetInBuffer 视图）
#include "TaiwanCash.h"         // WongWork::IPG::CIPGHelper 虚接口 IPGOutput/…Simulation
#include "CEventScriptMng.h"      // EventClassify::CEventScriptMng 完整类型（is_eventing）
#include "CTitleBook.h"          // CItemLock/stItemLockRef（GetItemLockRefVec 完整类型）
#include "AuctionProxy.h"       // CAuctionServerProxy/CCeraAuctionServerProxy::SendPacket（含 PacketDesign.h）
#include "ROI_Category.h"       // ROI_Category（m_keys/m_marks，_sort@0x0822ac30；PCK_AUCTION_REGIST_ITEM_GA._reg_roi_category_key 权威类型）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_（GuildCargoPopItem 邮件文案）
#include "CMyFileLog.h"          // JumpingCharacter 活动日志
#include "HistoryLog.h"          // WriteCreateCh（ORIG T 三参版）

#include "CDataManager.h"         // CAutoMarketConditionsControl / GetAutoMarketContitionsControl(ORIG 实名拼写)
#include "DBSelectCreateDnfUserInfo.h"
#include "DBSelectPcroomDailyReward.h"
// checkNewMemberHacklog —— 全局自由函数（ORIG T 0x08275315，定义 GameSecurity.cpp）。
void checkNewMemberHacklog(unsigned int hackType, CUser* user, unsigned int p3);

// CAradRyosikaEvent 权威声明见 CAradRyosikaEvent.h（LinkClosureB-2 建立，
// DoEvent 定义在 Inter_LoadEtc_impl.cpp）。
#include "CAradRyosikaEvent.h"
// CheckDailyScheduleTime —— 全局自由函数（ORIG T _Z22CheckDailyScheduleTimeill；
// 各调用 TU 按仓库先例以 TU 内 bool 声明引用，定义见某 Booster/Deps TU）。
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime);
// break_away_prevent::CBreakAwayPreventSystem —— 唯一权威声明见
// BreakAwayPreventSystem.h（B2 批次与 BAFix 批次共建）。
// DB_LoadCleanPadPoint::makeRequest 仅调用视图（完整类与定义在 GlobalData.cpp；
// makeRequest 为 static，不访问对象成员，静态 ABI 精确匹配 ORIG 符号）。
class DB_LoadCleanPadPoint
{
public:
    static void makeRequest();   // ORIG T 0x0842efa0
};
#include "BreakAwayPreventSystem.h"  // loadDbData/updateUvList/startBreakAwayPreventSystem

// CUser::SetMileage/SendCashData/AddSecuRewardItem/SendCmdErrorPacket —— 直接经 CUser.h 真实成员调用。
#include "CMCAPManager.h"         // WongWork::CMCAPManager::setPointPerHackType(int*)
#include "CMonitorServerProxy.h"  // Inter_Disconnect GetServerProxy 完整类型（CServerProxyMgr<T> 实例化需要）
#include "CPrivateStoreMgr.h"     // private_store::GetInstancePrivateStoreMgr()->Enter(user)
#include "DB_InterDeps.h"         // DB_SaveCleanpadPoint/DB_ArrangeUserLoginInfo/…::makeRequest
#include "DB_SaveEventLevelupSupport.h"   // DB_SaveEventLevelupSupport::makeRequest(int,int,int,int)（ORIG Eiiii）
#include "FunctionGuildMember.h"  // FunctionGuildMember::MakePacketGuildMembersInfo/LogMake（权威类声明）
#include "ItemVendingMachine.h"  // GlobalData::s_pItemVendingMachine 完整类型（ProcessIPGUserData）
#include "CServerEvent.h"        // CServerEvent::AddExpRate/TurnOnEvent
#include "Packet_Monitor_Member_Enter_Reply.h"     // shared/packet 权威包
#include "Packet_Monitor_UDP_Logout_Complete.h"    // shared/packet 权威包
#include <algorithm>              // std::min/std::max（SaveClientHackLog 掉率钳制）
#include "CDeathTowerRanking.h"   // WongWork::CDeathTowerRanking getRankTable/registRanker 族
#include "ServiceRestrictManager.h"  // ARAD::Singleton<ServiceRestrictManager>/RestrictCategory
#include "CTimeGate.h"            // GlobalData::s_timeGate_（Inter_LoadQuest）
#include "APSystemDbDispatchers.h"  // APSystem::DB_UpdateRewardMedal + _SIG_LOAD_REWARD_MEDAL
#include "Packet_Monitor_Charac_Delete.h"  // shared/packet 权威包（Inter_DeleteCharac）
#include "CMailBoxHelper.h"       // W0 批 MailBox 链
#include "PacketTypes.h"          // MSG_MAILBOX_SEND 权威布局（Inter_MultiMailBoxReqSend 栈上组包缓冲）
#include "Arad_DataManager.h"     // W0 批 ARAD 数据管理器
#include "ServerParameterScript.h"  // W0 批 conditionEnterRevenge 视图
#include "CPremiumLetheManager.h" // W0 批
#include "CHandlePremium.h"       // W0 批
#include "CShutdowManager.h"          // GetInstanceShutdowManager（Inter_LastMsgResponse）
#include "CSyncSlangFilter.h"         // CSyncSlangFilter::GetInstance()->AddSlang
#include "CInGameAdvertisementManager.h"  // InGameADManagerInstance/stInGameADExposeCondition
#include "CSchoolMgr.h"              // g_schoolMgr/CSchoolMgr::GetUserCount（PvPChannelUserCount）
#include "CLeagueMatch.h"            // CLeagueMatch::GetPlayCountPerOneTeam/GetBattleMode
#include "COnTimeEvent.h"            // COnTimeEvent（OnTimeEventReward*）
#include "CQuestShop.h"              // CQuestShop/CHARAC_QUEST_SHOP_DATA（RequestTagCharacInfo）
#include "SkillSlot.h"               // STGuildDBInfoPacketView（GetGuildDBInfo 视图字段化）
#include "TowerOfDespairMgr.h"       // TowerOfDespairMgr/_APCInfo（TOD_Reload_APC）
#include "CHackAnalyzer.h"           // ENUM_HACKTYPE（SetARSInfo）
#include "CCirculationStatistic.h"   // GetInstanceCirculationStatistic/CIRCULATION_STATISTIC_FIELD
#include "Packet_No_Cache.h"         // shared/packet（GuildExpBookDelete 等）
#include "Packet_TOD_DoRandomSelect.h"          // shared/packet 权威包
#include "Packet_Guild_Guild_Cargo_Push_Item.h" // shared/packet 权威包
#include "DnfItemInfo.h"         // Packet_Guild_Guild_Cargo_Push_Item ctor（m_item +0x1b 完整构造）
#include "GameServerDemonManager.h"  // GameserverDemonManagerInstance（EmergencyPatchMessage）
#include "Packet_Guild_Guild_Cargo_Push_Item.h" // shared/packet 权威包
#include "DnfItemInfo.h"         // Packet_Guild_Guild_Cargo_Push_Item ctor（m_item +0x1b 完整构造）
#include "DnfItemInfo.h"         // Packet_Guild_Guild_Cargo_Push_Item ctor（m_item +0x1b 完整构造）
#include "GameServerDemonManager.h"  // GameserverDemonManagerInstance（EmergencyPatchMessage）
#include "Packet_Monitor_Add_Buddy.h"             // shared/packet 权威包（CheckAddFriend）
#include "Packet_Monitor_Find_Factory_Hub_User.h" // shared/packet 权威包（FindFactoryHub）
#include "Packet_Monitor_Server_Message_Info.h"   // shared/packet（UpdateServerMessage）
#include "GameWorld.h"               // CUserModifier/CUserResetTimeGateQuest 基类（已含则幂等）
#include "Area.h"                    // CBelong（CUserTeen TU 局部声明基类）
#include "CVillageMonsterMgr.h"      // ENUM_VILLAGE_ATTACKED_REWARD
#include "LimitNpcBuyItemUpdate.h"   // 新建权威包（NpcLimitBuyItemInfo 回执）
#include "NpServerXml.h"             // np_server_xml::g_ServerString_（跨 TU extern）
#include "Secu_DataControl.h"        // Secu_DataControl::UpdateSecurityServiceState
#include "CNexonMsgTable.h"          // g_nexonMsgTable（Inter_PcroomResponse）
#include "CPrePayInfoMgr.h"          // G_CPrePayInfoMgr/PrePayInfo（Inter_OwnerisVIP）

#include "TimerDispatcher.h"      // Timer_HadesHeartBeat/TimerConnectMonitorServer registNextTimer
#include "CServerProxyMgr.h"      // CServerProxyMgr<T>::GetServerProxy（模板成员，头内已内联）
#include "CGuildServerProxy.h"    // SendGuildExpLimit

#include "GameWorld.h"            // GameWorld 全量成员 + G_GameWorld()（ORIG T _Z11G_GameWorldv）


// ======== 主分发器（逐指令转写自 docs/class_func_reports/InterDispatcher/）=====

// 注：ORIG 整函数仅 3 处 _Unwind_Resume（共享 EH 清理），我们 g++ 4.4.7 每-new 生成
// cleanup 块（编译器 EH 生成差异，语义等价，无法源码复现）；throw() 无效已回滚。
InterDispatcher::InterDispatcher()
{
    for (int i = 0; i <= 0x353; ++i)
    {
        m_dispatchers[i] = 0;
    }
    m_dispatchers[1] = new Inter_Disconnect();  // slot +0x004
    m_dispatchers[2] = new Inter_SessionClose();  // slot +0x008
    m_dispatchers[3] = new Inter_LoginInfo();  // slot +0x00c
    m_dispatchers[4] = new Inter_CreateCharac();  // slot +0x010
    m_dispatchers[5] = new Inter_DeleteCharac();  // slot +0x014
    m_dispatchers[7] = new Inter_LoadCharac();  // slot +0x01c
    m_dispatchers[8] = new Inter_LoadInven();  // slot +0x020
    m_dispatchers[9] = new Inter_LoadSkill();  // slot +0x024
    m_dispatchers[10] = new Inter_LoadQuest();  // slot +0x028
    m_dispatchers[11] = new Inter_LoadTicket();  // slot +0x02c
    m_dispatchers[12] = new Inter_LoadDungeonClear();  // slot +0x030
    m_dispatchers[17] = new APSystem::Inter_LoadActionPoint();  // slot +0x044
    m_dispatchers[18] = new APSystem::Inter_EmptyActionPoint();  // slot +0x048
    m_dispatchers[20] = new APSystem::Inter_LoadRewardMedal();  // slot +0x050
    m_dispatchers[25] = new advancealtar::Inter_Load();  // slot +0x064
    m_dispatchers[32] = new Inter_LoadEtc();  // slot +0x080
    m_dispatchers[47] = new Inter_DeleteChannel();  // slot +0x0bc
    m_dispatchers[61] = new Inter_Notice();  // slot +0x0f4
    m_dispatchers[63] = new Inter_Terminate();  // slot +0x0fc
    m_dispatchers[62] = new Inter_ReadyToTerminate();  // slot +0x0f8
    m_dispatchers[68] = new Inter_MonitorBuddyInOut();  // slot +0x110
    m_dispatchers[65] = new Inter_MonitorLoginOK();  // slot +0x104
    m_dispatchers[66] = new Inter_MonitorCallUserInfo();  // slot +0x108
    m_dispatchers[301] = new Inter_CutOffCallUserInfo();  // slot +0x4b4
    m_dispatchers[67] = new Inter_MonitorUserGetout();  // slot +0x10c
    m_dispatchers[759] = new Inter_WebNoticeIngameAdvertisement();  // slot +0xbdc
    m_dispatchers[761] = new Inter_ReloadIngameAdvertisement();  // slot +0xbe4
    m_dispatchers[289] = new Inter_TcpServerConnect();  // slot +0x484
    m_dispatchers[70] = new Inter_MonitorReplyBuddyList();  // slot +0x118
    m_dispatchers[71] = new Inter_MonitorCeraUpdate();  // slot +0x11c
    m_dispatchers[72] = new Inter_IPGResult();  // slot +0x120
    m_dispatchers[73] = new Inter_GenCeraTicket();  // slot +0x124
    m_dispatchers[75] = new Inter_LoadEventItem();  // slot +0x12c
    m_dispatchers[74] = new Inter_MonitorEventItemUpdate();  // slot +0x128
    m_dispatchers[85] = new Inter_MonitorServerEventStart();  // slot +0x154
    m_dispatchers[79] = new Inter_FatigueResetDBReply();  // slot +0x13c
    m_dispatchers[81] = new Inter_SuicideWhenQueueIsEmpty();  // slot +0x144
    m_dispatchers[86] = new Inter_MonitorNoticeGuildEnter();  // slot +0x158
    m_dispatchers[87] = new Inter_MonitorNoticeGuildSecede();  // slot +0x15c
    m_dispatchers[88] = new Inter_MonitorNoticeGuildMarkChange();  // slot +0x160
    m_dispatchers[89] = new Inter_MonitorNoticeGuildDismiss();  // slot +0x164
    m_dispatchers[90] = new Inter_MonitorNoticeGuildChatMsg();  // slot +0x168
    m_dispatchers[91] = new Inter_MonitorCallGuildMembers();  // slot +0x16c
    m_dispatchers[198] = new Inter_MonitorCallGuildMembersNext();  // slot +0x318
    m_dispatchers[199] = new Inter_MonitorCallGuildAllMembers();  // slot +0x31c
    m_dispatchers[200] = new Inter_MonitorCallGuildAllMembersNext();  // slot +0x320
    m_dispatchers[92] = new Inter_MonitorNoticeGuildNames();  // slot +0x170
    m_dispatchers[93] = new Inter_MonitorNoticeGuildLoginOut();  // slot +0x174
    m_dispatchers[94] = new Inter_MonitorNoticeOtherChannelChat();  // slot +0x178
    m_dispatchers[95] = new Inter_MonitorCallGuildInfo();  // slot +0x17c
    m_dispatchers[96] = new Inter_MonitorNoticeGuildInfo();  // slot +0x180
    m_dispatchers[97] = new Inter_MonitorSetGuildKey();  // slot +0x184
    m_dispatchers[98] = new Inter_MonitorNoticeGuildCreate();  // slot +0x188
    m_dispatchers[100] = new Inter_MonitorGuildMemberNamingReply();  // slot +0x190
    m_dispatchers[101] = new Inter_MonitorSetSubGuildMasterReply();  // slot +0x194
    m_dispatchers[102] = new Inter_MonitorNoticeGuildMemberInfo();  // slot +0x198
    m_dispatchers[215] = new Inter_MonitorNoticeChangeGuildMemberGrade();  // slot +0x35c
    m_dispatchers[216] = new Inter_GuildNoticeGuildMessage();  // slot +0x360
    m_dispatchers[108] = new Inter_MonitorNoticeMemberLoginOut();  // slot +0x1b0
    m_dispatchers[109] = new Inter_MonitorNoticeRequestMemberEnterToRequester();  // slot +0x1b4
    m_dispatchers[110] = new Inter_MonitorNoticeRequestMemberEnterToResponser();  // slot +0x1b8
    m_dispatchers[111] = new Inter_MonitorNoticeMemberEnterOk();  // slot +0x1bc
    m_dispatchers[112] = new Inter_MonitorNoticeMemberEnterReply();  // slot +0x1c0
    m_dispatchers[113] = new Inter_MonitorNoticeMemberSecede();  // slot +0x1c4
    m_dispatchers[114] = new Inter_MonitorNoticeMemberSecedeToSeceder();  // slot +0x1c8
    m_dispatchers[115] = new Inter_MonitorNoticeDeleteMemberId();  // slot +0x1cc
    m_dispatchers[116] = new Inter_MonitorNoticeMemberChatMsg();  // slot +0x1d0
    m_dispatchers[117] = new Inter_MonitorCallMemberList();  // slot +0x1d4
    m_dispatchers[118] = new Inter_MonitorPayTaxToUpper();  // slot +0x1d8
    m_dispatchers[120] = new Inter_LoadPrivateStore();  // slot +0x1e0
    m_dispatchers[44] = new Inter_UpdateLogout();  // slot +0x0b0
    m_dispatchers[122] = new Inter_UserRepel();  // slot +0x1e8
    m_dispatchers[49] = new Inter_AvatarInsert();  // slot +0x0c4
    m_dispatchers[54] = new Inter_AvatarChangeStat();  // slot +0x0d8
    m_dispatchers[133] = new Inter_TriggerEventStart();  // slot +0x214
    m_dispatchers[134] = new Inter_TriggerEventEnd();  // slot +0x218
    m_dispatchers[127] = new Inter_MailBox_Req_List();  // slot +0x1fc
    m_dispatchers[126] = new Inter_MailBox_Req_CheckReceiver();  // slot +0x1f8
    m_dispatchers[128] = new Inter_MailBox_Req_Send();  // slot +0x200
    m_dispatchers[130] = new Inter_Monitor_Notify_New_Mail();  // slot +0x208
    m_dispatchers[136] = new Inter_InsertCreatureItemResult();  // slot +0x220
    m_dispatchers[139] = new Inter_UpdateCreatureItemResult();  // slot +0x22c
    m_dispatchers[146] = new Inter_MonitorNoticeMessage();  // slot +0x248
    m_dispatchers[151] = new Inter_LastMsgResponse();  // slot +0x25c
    m_dispatchers[152] = new Inter_MonitorGuildPointDel();  // slot +0x260
    m_dispatchers[153] = new Inter_MonitorGuildWarInfo();  // slot +0x264
    m_dispatchers[159] = new Inter_CodeChecksumRange();  // slot +0x27c
    m_dispatchers[148] = new Inter_MonitorForbidChat();  // slot +0x250
    m_dispatchers[163] = new Inter_MouseRegister();  // slot +0x28c
    m_dispatchers[170] = new Inter_Monitor_MegaphoneMsg();  // slot +0x2a8
    m_dispatchers[172] = new Inter_Monitor_RegisterBlackListResult();  // slot +0x2b0
    m_dispatchers[173] = new Inter_Monitor_DeleteBlackListResult();  // slot +0x2b4
    m_dispatchers[171] = new Inter_Monitor_RequestBlackListResult();  // slot +0x2ac
    m_dispatchers[179] = new Inter_DB_RequestBlackCount();  // slot +0x2cc
    m_dispatchers[174] = new Inter_MonitorNoticeBlackList();  // slot +0x2b8
    m_dispatchers[183] = new Inter_WebNoticeSingle();  // slot +0x2dc
    m_dispatchers[186] = new Inter_MemberExpLevelUp();  // slot +0x2e8
    m_dispatchers[188] = new Inter_MonitorAddBuddy();  // slot +0x2f0
    m_dispatchers[189] = new Inter_MonitorDelBuddy();  // slot +0x2f4
    m_dispatchers[194] = new Inter_ChangeLetterStat();  // slot +0x308
    m_dispatchers[154] = new Inter_SaveClientHackLog();  // slot +0x268
    m_dispatchers[195] = new Inter_ReqChangeCharacName();  // slot +0x30c
    m_dispatchers[196] = new Inter_ReqQueryCharacInfo();  // slot +0x310
    m_dispatchers[202] = new Inter_ReportMannerless();  // slot +0x328
    m_dispatchers[206] = new Inter_ChagneUserHandicap();  // slot +0x338
    m_dispatchers[211] = new Inter_GMRequestMid();  // slot +0x34c
    m_dispatchers[214] = new Inter_UDP_Incoming_Capcha_Data();  // slot +0x358
    m_dispatchers[293] = new Inter_Add_Slang();  // slot +0x494
    m_dispatchers[218] = new Inter_ReplyGuildLetter();  // slot +0x368
    m_dispatchers[219] = new Inter_CallGuildInviteToCaller();  // slot +0x36c
    m_dispatchers[220] = new Inter_CallGuildInviteToInvited();  // slot +0x370
    m_dispatchers[221] = new Inter_ReplyGuildInviteToInvited();  // slot +0x374
    m_dispatchers[222] = new Inter_ReplyGuildInviteToCaller();  // slot +0x378
    m_dispatchers[226] = new Inter_LoadDeathTowerBestRecord();  // slot +0x388
    m_dispatchers[228] = new Inter_DeathTowerLoadTopRanker();  // slot +0x390
    m_dispatchers[229] = new Inter_MonitorDeathTowerRanking();  // slot +0x394
    m_dispatchers[230] = new Inter_MonitorTowerReloadTopRanker();  // slot +0x398
    m_dispatchers[234] = new Inter_ReplyGuildSecede();  // slot +0x3a8
    m_dispatchers[231] = new Inter_NoticeGuildMasterDelegate();  // slot +0x39c
    m_dispatchers[232] = new Inter_GuildReplyGuildMasterDelegate();  // slot +0x3a0
    m_dispatchers[233] = new Inter_NoticeHasBeenGuildMaster();  // slot +0x3a4
    m_dispatchers[235] = new Inter_DBCheckDoubleGuildName();  // slot +0x3ac
    m_dispatchers[236] = new Inter_DBCheckDoubleGuildAddress();  // slot +0x3b0
    m_dispatchers[237] = new Inter_ReplyGuildCreate();  // slot +0x3b4
    m_dispatchers[298] = new Inter_NoticeGuildNameOnGuildCreate();  // slot +0x4a8
    m_dispatchers[253] = new Inter_SecurityCardIssue();  // slot +0x3f4
    m_dispatchers[255] = new Inter_SecurityCardActivate();  // slot +0x3fc
    m_dispatchers[312] = new Inter_SecurityCardRetransfer();  // slot +0x4e0
    m_dispatchers[262] = new Inter_LoadDnfHackLog();  // slot +0x418
    m_dispatchers[264] = new Inter_AutoPunishRuleBlackIP();  // slot +0x420
    m_dispatchers[265] = new Inter_AutoPunishRuleHackType();  // slot +0x424
    m_dispatchers[271] = new Inter_ReplyPowerWarScore();  // slot +0x43c
    m_dispatchers[284] = new Inter_ReplyJoinPower();  // slot +0x470
    m_dispatchers[285] = new Inter_SecedePower();  // slot +0x474
    m_dispatchers[286] = new Inter_NoticePowerWarStartTime();  // slot +0x478
    m_dispatchers[287] = new Inter_NoticePowerWarEndTime();  // slot +0x47c
    m_dispatchers[288] = new Inter_NoticePowerWarInfo();  // slot +0x480
    m_dispatchers[272] = new Inter_NoticePowerWarProlong();  // slot +0x440
    m_dispatchers[291] = new Inter_ChangeGuildName();  // slot +0x48c
    m_dispatchers[292] = new Inter_NoticeChangeGuildNameToMembers();  // slot +0x490
    m_dispatchers[388] = new Inter_SendAllUserInfoMiniumForGuildSystem();  // slot +0x610
    m_dispatchers[294] = new Inter_LoadCleanPadPointReq();  // slot +0x498
    m_dispatchers[295] = new Inter_LoadCleanPadPoint();  // slot +0x49c
    m_dispatchers[239] = new Inter_NotifyAuctionMail();  // slot +0x3bc
    m_dispatchers[240] = new Inter_AuctionResultAskPrice();  // slot +0x3c0
    m_dispatchers[241] = new Inter_AuctionResultAskRegistedItemNum();  // slot +0x3c4
    m_dispatchers[242] = new Inter_AuctionResultRegist();  // slot +0x3c8
    m_dispatchers[243] = new Inter_AuctionResultBidding();  // slot +0x3cc
    m_dispatchers[244] = new Inter_AuctionResultRegistCancel();  // slot +0x3d0
    m_dispatchers[245] = new Inter_AuctionResultItemList();  // slot +0x3d4
    m_dispatchers[246] = new Inter_AuctionResultMyRegistedItems();  // slot +0x3d8
    m_dispatchers[247] = new Inter_AuctionResultMyBidding();  // slot +0x3dc
    m_dispatchers[248] = new Inter_AuctionLogMessage();  // slot +0x3e0
    m_dispatchers[249] = new Inter_AuctionNotifyAuctionService();  // slot +0x3e4
    m_dispatchers[250] = new Inter_PointAuctionLogMessage();  // slot +0x3e8
    m_dispatchers[251] = new Inter_PointAuctionChargeCeraPoint();  // slot +0x3ec
    m_dispatchers[525] = new Inter_AuctionResultBuyItemApiece();  // slot +0x834
    m_dispatchers[303] = new Inter_LoadGameOption();  // slot +0x4bc
    m_dispatchers[307] = new Inter_PvPChannelInfo();  // slot +0x4cc
    m_dispatchers[308] = new Inter_PvPChannelUserCount();  // slot +0x4d0
    m_dispatchers[319] = new Inter_LoadBloodDungeonBestRecord();  // slot +0x4fc
    m_dispatchers[321] = new Inter_LoadBreakAwayPreventSystem();  // slot +0x504
    m_dispatchers[323] = new Inter_LoadBreakAwayPreventMessage();  // slot +0x50c
    m_dispatchers[334] = new Inter_LoadDimensionInout();  // slot +0x538
    m_dispatchers[339] = new Inter_BlackIPMonitorReq();  // slot +0x54c
    m_dispatchers[342] = new Inter_BlackIPMonitor();  // slot +0x558
    m_dispatchers[341] = new Inter_BlackIPMonitorPartLoadReq();  // slot +0x554
    m_dispatchers[343] = new Inter_BlackIPMonitorDeleteIP();  // slot +0x55c
    m_dispatchers[346] = new Inter_LoadAutoMarketConditionsControl();  // slot +0x568
    m_dispatchers[348] = new Inter_UpdateOneDayLethe();  // slot +0x570
    m_dispatchers[351] = new Inter_DisconnectMonitorServer();  // slot +0x57c
    m_dispatchers[352] = new Inter_ResposncePCRoomPlyerList();  // slot +0x580
    m_dispatchers[353] = new Inter_ResposncePCRoomPlyerCount();  // slot +0x584
    m_dispatchers[354] = new Inter_NoticePCRoomPlayerLoginOut();  // slot +0x588
    m_dispatchers[372] = new Inter_NoticePowerWarRank();  // slot +0x5d0
    m_dispatchers[357] = new Inter_LoadPowerWarStatueRanker();  // slot +0x594
    m_dispatchers[358] = new Inter_LoadPowerWarStatueInfo();  // slot +0x598
    m_dispatchers[364] = new Inter_UpdateServerMessage();  // slot +0x5b0
    m_dispatchers[365] = new Inter_LoadServerMessage();  // slot +0x5b4
    m_dispatchers[368] = new Inter_NoticeServerMessageInfo();  // slot +0x5c0
    m_dispatchers[374] = new Inter_NoticeGuildMemo();  // slot +0x5d8
    m_dispatchers[378] = new Inter_PowerWarProcessInfo();  // slot +0x5e8
    m_dispatchers[379] = new Inter_PowerWarEndInfo();  // slot +0x5ec
    m_dispatchers[369] = new Inter_NoticeCreateGuildAgit();  // slot +0x5c4
    m_dispatchers[370] = new Inter_NoticeDeleteGuildAgit();  // slot +0x5c8
    m_dispatchers[371] = new Inter_NoticeLoadGuildAgit();  // slot +0x5cc
    m_dispatchers[384] = new Inter_LoadExpertJob();  // slot +0x600
    m_dispatchers[381] = new Inter_MonitorPunishUserReq();  // slot +0x5f4
    m_dispatchers[382] = new Inter_PcroomResponse();  // slot +0x5f8
    m_dispatchers[389] = new Inter_ItemLimitEdition_Sell_Start();  // slot +0x614
    m_dispatchers[390] = new Inter_ItemLimitEdition_Sell_End();  // slot +0x618
    m_dispatchers[391] = new Inter_ItemLimitEdition_Update();  // slot +0x61c
    m_dispatchers[392] = new Inter_ItemLimitEdition_Reply();  // slot +0x620
    m_dispatchers[394] = new Inter_FindFactoryHub();  // slot +0x628
    m_dispatchers[398] = new Inter_GuildExpLimit();  // slot +0x638
    m_dispatchers[402] = new Inter_SetIPCount();  // slot +0x648
    m_dispatchers[404] = new Inter_SetFullIPCount();  // slot +0x650
    m_dispatchers[405] = new Inter_CancelRestingUser();  // slot +0x654
    m_dispatchers[408] = new Inter_ApplyTempPowerSide();  // slot +0x660
    m_dispatchers[409] = new Inter_ReloadPowerWarRanker();  // slot +0x664
    m_dispatchers[411] = new Inter_LoadPowerWarInfo();  // slot +0x66c
    m_dispatchers[416] = new Inter_SetFactoryHubUser();  // slot +0x680
    m_dispatchers[417] = new Inter_SetFactoryHubUserNotConn();  // slot +0x684
    m_dispatchers[418] = new Inter_Set_CleanPad_Point();  // slot +0x688
    m_dispatchers[423] = new Inter_LoadNoCacheData();  // slot +0x69c
    m_dispatchers[426] = new Inter_GuildCargo();  // slot +0x6a8
    m_dispatchers[427] = new Inter_NoticeGuildCargo();  // slot +0x6ac
    m_dispatchers[428] = new Inter_GuildCargoHistory();  // slot +0x6b0
    m_dispatchers[429] = new Inter_GuildCargoCheckPushItem();  // slot +0x6b4
    m_dispatchers[430] = new Inter_GuildCargoPushItem();  // slot +0x6b8
    m_dispatchers[431] = new Inter_GuildCargoPopItem();  // slot +0x6bc
    m_dispatchers[432] = new Inter_GuildCargoMoveItem();  // slot +0x6c0
    m_dispatchers[434] = new Inter_GuildCargoUpgrade();  // slot +0x6c8
    m_dispatchers[435] = new Inter_GuildBoardData();  // slot +0x6cc
    m_dispatchers[436] = new Inter_GuildBoardDelete();  // slot +0x6d0
    m_dispatchers[437] = new Inter_TakeScreenShot();  // slot +0x6d4
    m_dispatchers[441] = new Inter_VillageAttackedStart();  // slot +0x6e4
    m_dispatchers[442] = new Inter_VillageAttackedEnd();  // slot +0x6e8
    m_dispatchers[443] = new Inter_VillageAttackedCountdown();  // slot +0x6ec
    m_dispatchers[444] = new Inter_VillageAttackedUpdate();  // slot +0x6f0
    m_dispatchers[445] = new Inter_VillageAttackedScore();  // slot +0x6f4
    m_dispatchers[446] = new Inter_VillageAttackedReward();  // slot +0x6f8
    m_dispatchers[447] = new Inter_VillageAttackedRewardServer();  // slot +0x6fc
    m_dispatchers[448] = new Inter_FullLevelBroadCast();  // slot +0x700
    m_dispatchers[396] = new Inter_Hack_CheckMyAccount();  // slot +0x630
    m_dispatchers[458] = new Inter_SetSecounUserData();  // slot +0x728
    m_dispatchers[461] = new Inter_SetARSInfo();  // slot +0x734
    m_dispatchers[463] = new Inter_OnTimeEventRewardStart();  // slot +0x73c
    m_dispatchers[465] = new Inter_OnTimeEventRewardEnd();  // slot +0x744
    m_dispatchers[469] = new Inter_LoadPvPBuddy();  // slot +0x754
    m_dispatchers[470] = new Inter_PvPBuddyConnList();  // slot +0x758
    m_dispatchers[471] = new Inter_ReqAddPvPBuddy();  // slot +0x75c
    m_dispatchers[472] = new Inter_AddPvPBuddyResult();  // slot +0x760
    m_dispatchers[473] = new Inter_RemovePvPBuddyResult();  // slot +0x764
    m_dispatchers[474] = new Inter_NoticePvPBuddyInOut();  // slot +0x768
    m_dispatchers[480] = new Inter_LoadPvPLiveEventData();  // slot +0x780
    m_dispatchers[594] = new Inter_Antibot_SendBufferClient();  // slot +0x948
    m_dispatchers[595] = new Inter_Antibot_PunishClient();  // slot +0x94c
    m_dispatchers[596] = new Inter_Antibot_ProcessProtectMsg();  // slot +0x950
    m_dispatchers[486] = new Inter_LoadOnlinePreliminaryMember();  // slot +0x798
    m_dispatchers[488] = new Inter_LoadOnlinePreliminaryTeam();  // slot +0x7a0
    m_dispatchers[490] = new Inter_LoadOnlinePreliminaryMemberMatchList();  // slot +0x7a8
    m_dispatchers[492] = new Inter_LoadOnlinePreliminaryTeamMatchList();  // slot +0x7b0
    m_dispatchers[331] = new Inter_LoadCharacLinkBonus();  // slot +0x52c
    m_dispatchers[494] = new Inter_CreateAccountCargo();  // slot +0x7b8
    m_dispatchers[495] = new Inter_UpgradeAccountCargo();  // slot +0x7bc
    m_dispatchers[498] = new Inter_LoadAccountCargo();  // slot +0x7c8
    m_dispatchers[501] = new Inter_MultiMailBoxReqSend();  // slot +0x7d4
    m_dispatchers[502] = new Inter_ReqQueryCharacInfoMailbox();  // slot +0x7d8
    m_dispatchers[503] = new Inter_ReqGameServerInfo();  // slot +0x7dc
    m_dispatchers[504] = new Inter_MonitorRegistGameServerAck();  // slot +0x7e0
    m_dispatchers[509] = new Inter_LoadMileage();  // slot +0x7f4
    m_dispatchers[521] = new Inter_GuildExpBookDelete();  // slot +0x824
    m_dispatchers[522] = new Inter_NoCache();  // slot +0x828
    m_dispatchers[523] = new Inter_Send_Charac_no_from_monitor();  // slot +0x82c
    m_dispatchers[528] = new Inter_SendPeriodicMessage();  // slot +0x840
    m_dispatchers[529] = new Inter_SendTimeSync();  // slot +0x844
    m_dispatchers[530] = new Inter_SendTimeSyncLogin();  // slot +0x848
    m_dispatchers[532] = new Inter_AddGuildFundResult();  // slot +0x850
    m_dispatchers[533] = new Inter_TodayGuildMember();  // slot +0x854
    m_dispatchers[534] = new Inter_CheckJoinGuild();  // slot +0x858
    m_dispatchers[535] = new Inter_RequestJoinGuild();  // slot +0x85c
    m_dispatchers[537] = new Inter_JoinGuildInfo();  // slot +0x864
    m_dispatchers[539] = new Inter_ApproveJoinGuild();  // slot +0x86c
    m_dispatchers[538] = new Inter_GuildJoinList();  // slot +0x868
    m_dispatchers[541] = new Inter_GuildAttendanceInfo();  // slot +0x874
    m_dispatchers[542] = new Inter_AchieveGuildAttendance();  // slot +0x878
    m_dispatchers[572] = new Inter_UpdatePurchaseCashItemBonus();  // slot +0x8f0
    m_dispatchers[570] = new Inter_HadesDisConnect();  // slot +0x8e8
    m_dispatchers[581] = new Inter_GoblinPadChagePwdReward();  // slot +0x914
    m_dispatchers[582] = new Inter_GoblinPadSamePwdCheck();  // slot +0x918
    m_dispatchers[603] = new Inter_PunishCancel();  // slot +0x96c
    m_dispatchers[608] = new Inter_BroadcastMsg();  // slot +0x980
    m_dispatchers[630] = new Inter_TOD_ReturnToMonitor_APCSettingComplete();  // slot +0x9d8
    m_dispatchers[631] = new Inter_TOD_Reload_APC();  // slot +0x9dc
    m_dispatchers[628] = new Inter_UpdateSecuServiceState();  // slot +0x9d0
    m_dispatchers[642] = new Inter_LoadKillMonsterInfo();  // slot +0xa08
    m_dispatchers[654] = new Inter_LevelUpReward();  // slot +0xa38
    m_dispatchers[673] = new Inter_GetUserRegdate();  // slot +0xa84
    m_dispatchers[675] = new Inter_GoldControlMoneyActionCancel();  // slot +0xa8c
    m_dispatchers[677] = new Inter_MonitorNoticeGuildChatMsgHyperLink();  // slot +0xa94
    m_dispatchers[679] = new Inter_MonitorNoticeMemberChatMsgHyperLink();  // slot +0xa9c
    m_dispatchers[678] = new Inter_MonitorNoticeOtherChannelChatHyperLink();  // slot +0xa98
    m_dispatchers[680] = new Inter_Monitor_MegaphoneMsgHyperLink();  // slot +0xaa0
    m_dispatchers[683] = new Inter_SendSecuReward();  // slot +0xaac
    m_dispatchers[694] = new Inter_HadesAction();  // slot +0xad8
    m_dispatchers[695] = new Inter_Check3rdPartyConcent();  // slot +0xadc
    m_dispatchers[697] = new Inter_SelectCharacMid();  // slot +0xae4
    m_dispatchers[709] = new Inter_NpcLimitBuyItemInfo();  // slot +0xb14（ORIG 0xb14，非 0xb10）
    m_dispatchers[710] = new Inter_NpcLimitBuyItemInfoAll();  // slot +0xb18
    m_dispatchers[711] = new Inter_NpcLimitBuyItemChangeInfo();  // slot +0xb1c
    m_dispatchers[712] = new Inter_NpcLimitBuyItemInfoRenew();  // slot +0xb20
    m_dispatchers[718] = new Inter_MonitorReplyCharacInfo();  // slot +0xb38
    m_dispatchers[729] = new Inter_CeraShopPurchaseCount();  // slot +0xb64
    m_dispatchers[730] = new Inter_OwnerisVIP();  // slot +0xb68（ORIG 0xb68 注册，NEW 原缺失）
    m_dispatchers[732] = new Inter_DispatcherAradOnly();  // slot +0xb70（ORIG new 0x1c）
    m_dispatchers[737] = new Inter_LoadPuUserCheck();  // slot +0xb84
    m_dispatchers[762] = new Inter_CollectItemsResult();  // slot +0xbe8
    m_dispatchers[763] = new Inter_CollectItemsReward();  // slot +0xbec
    m_dispatchers[764] = new Inter_CollectItemsRewardBroadcast();  // slot +0xbf0
    m_dispatchers[765] = new Inter_RequestCharacSkillInfo();  // slot +0xbf4
    m_dispatchers[766] = new Inter_RequestTagCharacInfo();  // slot +0xbf8
    m_dispatchers[770] = new Inter_EmergencyPatchMessage();  // slot +0xc08
    m_dispatchers[777] = new Inter_AuctionSubMoney();  // slot +0xc24
    m_dispatchers[778] = new Inter_UpdateServerSnapShot();  // slot +0xc28
    m_dispatchers[800] = new Inter_LoadCharacTypeHotKey();  // slot +0xc80
    m_dispatchers[815] = new Inter_UpdateMiniCraneSeed();  // slot +0xcbc
    m_dispatchers[818] = new Inter_GetPremiumSelectedCubeInfo();  // slot +0xcc8
    m_dispatchers[830] = new Inter_CheckAddFriend();  // slot +0xcf8
    m_dispatchers[833] = new Inter_LoadCerashopAddRestrictType();  // slot +0xd04（ORIG 0xd04 注册，NEW 原缺失）
    m_dispatchers[846] = new Inter_DispatcherTaiwan();  // slot +0xd38（ORIG new 0x1c）
    m_dispatchers[848] = new Inter_LoadEventLevelupSupport();  // slot +0xd40
    m_dispatchers[850] = new Inter_SelectAllowProxyUser();  // slot +0xd48
}

// ORIG 0x84be89e：非空槽经虚表[2]（D0）删除后置 0。
InterDispatcher::~InterDispatcher()
{
    for (int i = 0; i <= 0x353; ++i)
    {
        if (m_dispatchers[i] != 0)
        {
            delete m_dispatchers[i];
        }
        m_dispatchers[i] = 0;
    }
}

// ORIG 0x84be906 逐指令转写。
ISigDispatcher* InterDispatcher::get_dispatcher(int code)
{
    if (m_dispatchers[code] != 0)
    {
        return m_dispatchers[code];
    }
    LogManager::logFormat(1, "InterDispatcher.cpp",
                          __PRETTY_FUNCTION__,
                          0x5e5, "cmd_map.find( code %d )", code);
    return 0;
}

// ORIG 0x84be95e 逐指令转写。
void InterDispatcher::register_dispatcher(int code, ISigDispatcher* dispatcher)
{
    m_dispatchers[code] = dispatcher;
}

// ORIG 0x84be970 逐指令转写（getUser 参数序、DisConnSig 与双路径日志均按汇编）。
// [b28d-lane2] verdict=DIFF ndiff=42（ORIG 0x084be970）：混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=37（ORIG 0x84be970）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
bool InterDispatcher::dispatch(int code, int userId, char* data, int len)
{
    CUser* user = 0;
    if (userId != -1)
    {
        user = G_CGameManager()->getUser(userId, code + 0x3e8);
        if (user == 0)
        {
            return false;
        }
    }
    ISigDispatcher* d = get_dispatcher(code);
    if (d == 0)
    {
        LogManager::logFormat(1, "InterDispatcher.cpp",
                              __PRETTY_FUNCTION__,
                              0x602, "get_dispatcher(%d)", code);
        return false;
    }
    int result = 0;
    if ((result = d->dispatch_sig(user, data, len)) != 0)
    {
        if (user != 0)
        {
            user->DisConnSig((DISCONN_SIG)2, true, result + 0xf4240);
            cMyTrace(__PRETTY_FUNCTION__, 0x632, 0)("discon interdispatcher return false: %d line(%d)(m_id: %s)",
                  code, result, NumberToString(user->get_acc_id(), 0));
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x636, 0)("discon interdispatcher return false: %d line(%d)(m_id: 0)",
                  code, result);
        }
        // ORIG 0x84beb29/0x84beb30：if 体内与函数尾各一枚 `mov $0x1,%eax`（早退型带
        //   `jmp 0x84beb35` 跳过尾块；尾块直落 epilogue）⇒ 两支 return true 照抄（P171）。
        return true;
    }
    return true;
}

// ======== 具体分发器生命周期 + dispatch_sig stub ========
// stub 体待后续批次按 docs/class_func_reports 还原；地址=该函数 ORIG 起点。

Inter_Disconnect::~Inter_Disconnect() {}
// ORIG 0x084beb3c T dispatch_sig —— 用户断开清理：状态门/扩展槽切换/付费税上报/各管理器离开通知/DB 收尾。
// 报告 docs/class_func_reports/Inter_Disconnect/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=176（ORIG 0x084beb3c）：寄存器中转(N18a/N7)+混合残差；gap-4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=52（ORIG 0x84beb3c）：N18a 单段 regmove+N82/P112 帧槽平移。
// [lane2 2026-09-10] verdict=DIFF→AE：真 bug 修复 —— ① private_store vtable 槽错位（旧 `->Enter(user)`
//   走槽+1=0x85caa8a；ORIG 084beb3c 段内为槽+0=`CPrivateStoreMgr::Exit` 0x85ca994，nm T 直证）改 `->Exit(user)`；
//   ② SendMemberPayTax 后两实参（is_connect_upper_member/getCurCharacNo）由具名局部改内联实参
//   （ORIG %esi/%ebx 跨 GetServerGroup/GetServerProxy 保活，P52/P200）；③ assaultMgr 去具名局部。
int Inter_Disconnect::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x651, "State(NONE)");
        return 0;
    }

    if (user->get_state() > 2)                         // cmp $2; setg
    {
        if (G_GameWorld()->IsEquipSlotSwitchChannel())
        {
            if (user->getCurChannelEquipslotSwitch() == '\x01')
            {
                CExpandEquipslot* expand =
                    (CExpandEquipslot*)user->GetCharacExpandData(
                        (ENUM_CHARAC_EXPAND_TYPE)9);
                expand->EquipslotSwitch(
                    user, '\0',
                    user->getCurChannelEquipslotSwitch(),
                    user->getCurExpandEquipslotSwitch());
            }
        }
    }

    user->WorkPerFiveMin();

    if (user->get_charac_memberkey() != 0)
    {
        const int payMoney = user->get_member_pay_tex_money_to_upper();
        const int payFatigue = user->get_member_pay_tex_fatigue_to_upper();
        // ORIG 084bf3xx：characNo→%ebx / isUpper→%esi 跨 GetServerGroup/GetServerProxy
        //   保活（无栈槽）⇒ 后两实参内联到调用点（P52/P200）；receiver 链不设具名局部。
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->SendMemberPayTax(user->is_connect_upper_member() != 0,
                               user->getCurCharacNo(), payMoney, payFatigue);
        if (0 < payMoney)
            user->set_member_pay_tex_money_to_upper(0);
        if (0 < payFatigue)
            user->set_member_pay_tex_fatigue_to_upper(0);
    }

    // 真 bug 修正（P193/NM 实证）：ORIG 084bf4xx 调用 vtable 槽+0 = CPrivateStoreMgr::Exit
    //   （ORIG 0x85ca994 nm T `_ZN13private_store16CPrivateStoreMgr4ExitEP5CUser`）；
    //   旧写 Enter 走槽+1（0x85caa8a）多一条 `add $0x4,%edx`。
    private_store::GetInstancePrivateStoreMgr()->Exit(user);
    pvp_assault::GetInstanceAssaultMgr()->OnLeaveAssaultPlace(user, true);
    GlobalData::s_ExpertJobMgr->OnLeaveUser(user);              // expert_job::CExpertJobMgr（ORIG T 0x0849e214）
    GlobalData::s_onlinePreliminaryTeamMgr->OnLeaveUser(user);  // online_preliminary（ORIG T 0x08588ae8）

    const unsigned int accId = user->get_acc_id();
    const unsigned int uid = user->GetUID();
    if (user->m_mcap.IsSaveCleanPadPoint())
    {
        // [b23S3C] P48：ORIG 此处二次 call get_acc_id（uVar6 缓存供尾部三件，本点重新取）。
        DB_SaveCleanpadPoint::makeRequest(user->m_mcap.getPoint(), user->get_acc_id());
        user->m_mcap.disableCleanPadPoint();
    }
    G_CGameManager()->user_disconnect(user);
    DB_ArrangeUserLoginInfo::makeRequest(accId, true, uid);
    DB_SecurityCardUpdateValidityTime::makeRequest(accId, 0);
    DB_GoblinPadUpdateValidityTime::makeRequest(accId, 0);
    return 0;
}

Inter_SessionClose::~Inter_SessionClose() {}
// ORIG 0x084c7af0 T dispatch_sig —— 监控侧会话强制关闭。
// 报告 docs/class_func_reports/Inter_SessionClose/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SessionClosePktView —— Inter_SessionClose::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SessionClosePktView
{
    unsigned int m_0;         // +0x00
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=59（ORIG 0x084c7af0）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=51（ORIG 0x84c7af0）：P264 .o 重定位/立即数。
// [lane2 2026-09-10] verdict=DIFF→AE（identical=135/ae=1348→1352）：三处源形修复 ——
//   ① accId 去具名（P0/P48 两处各自重读 pPkt->m_0）；② P59 guard 极性（if(target==0)return 0;
//   + 块内 return 0）复现 ORIG `jne <then>`+`mov $0,%ebx;jmp <共享尾>`；③ P227 全表达式临时链。
int Inter_SessionClose::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SessionClosePktView* pPkt = reinterpret_cast<Inter_SessionClosePktView*>(data);
    // §P0/P48：ORIG 084c7b01 `mov (%eax),%ebx` 直传（accId 无栈槽），084c7b98 第二处
    //   独立重读 pPkt->m_0 ⇒ 不设具名 accId，两处各写表达式。
    CUser* target = G_CGameManager()->GetUserByAccId(pPkt->m_0);
    // P59 guard 极性：ORIG 084c7b1b `jne <if 体>` + 084c7b1d `mov $0,%ebx; jmp <共享尾>`
    //   ⇒ 写 `if (target == 0) return 0;`（fail 块直落 jcc），then 体（guard 块）后置。
    if (target == 0)
        return 0;
    {
        PacketGuard packet;
        packet.put_header(1, 1);
        packet.put_byte(0);
        packet.put_byte(0x12);
        packet.finalize(true);
        target->Send(packet);
        // P227：ORIG 084c7bad NumberToString 结果留 %ebx 跨 ctor（084c7bcd）⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x2004, 0)(
            "강제 종료요청 ACCID: %s", NumberToString(pPkt->m_0, 0));
        target->DisConnSig((DISCONN_SIG)0xf, true, 0);
        // 块内 return：ORIG 084c7c0c `mov $0x0,%ebx` + dtor 后 `jmp <共享尾>`
        //   （探针 /tmp/probe_eh3.cpp f12 命中同形）。
        return 0;
    }
}

#include "W6_HackPunishDeps.h"     // WongWork::CAutoPunishRuleBlackIPMgr::isBlackIP
#include "DB_AccountCargoSync.h"   // DB_PassPadUpdateFailCnt::makeRequest(static)

// Inter_LoginInfo 下游 DB/Timer/GameWorld 入口改经真实类型声明调用：
// Timer_ReturnUserTimeout::registNextTimer → TimerDispatcher.h（已 include）
// break_away_prevent 三入口 → BreakAwayPreventSystem.h（权威头，见文件尾 include 区）

// 仅调用视图：makeRequest 在 ORIG 为 static（调用点无 this，见
// df_game_r 0x84355ca/0x843b6ec/0x815bee0 首槽均传显式实参）；完整类与
// 实现见 W8LT_DBTask.cpp / GlobalData.cpp。
class DB_BlackipPrivateSecurityValuation { public: static void makeRequest(unsigned int); };     // T 0x084355ca
class DB_LoadAccountCargo              { public: static void makeRequest(int, unsigned int); }; // T 0x0843b6ec
#include "Inter_MonitorGuildPointDel.h"  // setLastDeleteTime/getLastDeleteTime（Inter_MonitorGuildPointDel 调用）

#include "RestrictGeolocation.h"   // 地区限制检测
#include "CBlackIPMonitor.h"       // 黑名单 IP 位标志查询
#include "TaiwanQueries.h"         // Taiwan::GarenaAuthData / GarenaCyberCafe
#include "CHandlePremium.h"        // handleSetUserPremium
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
#include "CServerProxyMgr.h"          // GetServerProxy 模板完整类型
#include "CDoubleConnCheckServerProxy.h"
#include "Packet_Monitor_UDP_Login.h"

// GameWorld 局部视图补充本函数所需成员（签名与 GameWorld.h/ORIG mangled 一致）
static const char g_interLoginInfoSig[] =
// [b28d-lane2] verdict=DIFF ndiff=2300（ORIG 0x084c5294）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+66。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=1326（ORIG 0x084c5294）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N18a 单段 regmove。
// [BigParty/P331] verdict=DIFF ndiff 2352→1691、our 2396→2441（ORIG 2462，gap 66→21）：
//   已核销的缺口 = 两个处罚回放循环各删一处 `SIG_LOGIN_PUNISH_INFO& entry =
//   pInfo->m_punishInfos[i]` 起基址引用（宏化为每次直取），复现 ORIG 14 处逐次物化
//   （ORIG 0x84c662b `lea (%ecx,%eax,1),%eax; add $0x3a54,%eax; mov (%eax),%eax` 9 处、
//   `add $0x3a68` 5 处；-0x60(%ebp) 唯一写入是 `mov 0x10(%ebp),%eax` = 第 2 参数 data，
//   与本方 pInfo 同物；offsetof(SIG_LOGIN_DATA,m_punishInfos)=0x3a54、stride 0x18 与
//   `imul 3;shl 3` 一致 ⇒ 非布局错）。
//   剩余判 N4 常量折叠 + N82：同一寻址两侧折叠路径不同 ——
//   ORIG `add $0x3a54,%eax; mov (%eax),%eax`  vs  本方 `add $0x3a50,%eax; mov 0x4(%eax),%eax`；
//   ORIG `add $0x3a68,%eax; mov (%eax),%eax`  vs  本方 `add $0x3a60,%eax; mov 0x8(%eax),%eax`。
//   指令条数相同、仅立即数/位移拆分不同 ⇒ 4.4 从 m_punishInfoCount(+0x3a50) 起算，
//   ORIG 编译器从 m_punishInfos(+0x3a54) 起算，非源级可控（勿以手拆常量去凑，P321-T5）。
// [BigInterLogin/2026-09-09] 逐段语义核对（Ghidra C + ORIG 汇编 2462 条逐区段）发现并修复 4 类真缺口：
//   ① `if (m_breakAwayRewardBoundA < m_breakAwayRewardBoundB)` **方向反**（真 bug）：
//      ORIG 0x84c771d `mov 0x3f40(A),%edx; mov 0x3f44(B),%eax; cmp %eax,%edx; jbe 0x84c7736`
//      ⇒ A<=B 走 order3、A>B 走 order1（Ghidra C 同结论：0x3f44<0x3f40→order1）；
//      旧码 A<B 走 order1 ⇒ A<B 时结果与 ORIG 相反。已改 `A > B`。
//   ② SetServerGroup×2 / setOnlinePreliminaryServerGroup 三处 `(char)` 截断：
//      ORIG 0x84c52c0/0x84c52ef/0x84c6fd5 均 `mov 0x39d0|0x39a8(%eax),%eax` 直传 32 位
//      （无 movsbl；字段 unsigned int）⇒ 旧码把值截成低字节（≥0x100 时传错）。已删 cast。
//   ③ setTotalHackUserTradeGold 只传低 32 位：ORIG 0x84c68de `mov 0x3a2c(%eax),%edx;
//      mov 0x3a28(%eax),%eax; mov %eax,0x4(%esp); mov %edx,0x8(%esp)` ⇒ 传完整 64 位
//      （低@+0x3a28、高@+0x3a2c）；旧码高位恒 0（真值差）。已改 ull 视图读。
//   ④ restrictedGoods 循环变量 unsigned short ⇒ 16 位 `cmp %ax; seta`（count>0x7fff 边界异）；
//      ORIG 0x84c6419 `movzwl 0x293a; movzwl %ax,%eax; cmp i; setg`（32 位有符号）。已改 int。
//   ⑤ m_lastPlayTime 比较：ORIG 0x84c754f `cmp %eax,%ebx; setl`（T2 有符号直证）；
//      字段是 unsigned int ⇒ 直比生 setb（方向同、符号性异）。写
//      `static_cast<long>(m_lastPlayTime) + 0x3b53800` 复现 ORIG `lea`+`setl`（0 额外指令）。
//   复核：p167 385/385（差集仅 C1↔C2/D1↔D2 镜像）、gap +21→+23（删 3 movsbl +1 movzwl
//   +1 add，均为纠错副作用）、ndiff 1277→1270、TU 四数 121/1329/5/135 与改动前一致。
//   试形回退记录：stream/monStream/expire/repeatEvent 内联（P0 方向）实测帧 0x28c→0x26c、
//   ndiff 1270→1319（ORIG 0x84c616b 有 gameOption -0x54 具名槽 ⇒ 原码该处有局部；
//   槽基由聚合对象排布决定，N82/P212）⇒ 已回退。
//   剩余 N：N4（0x3a54/0x3a68 vs 0x3a50+4/0x3a60+8 折叠路径）、N82/P112（帧 0x28c vs 0x27c 槽平移）、
//   N7/N18a（寄存器中转）、P227/P328⑩（trace 两语句 vs 全表达式临时链）、
//   P168a/P194（具名 bool punishNow/lockNow/inWindow/startBreakAway 落槽）、P196 无（⑤ 已修）。
    "virtual int Inter_LoginInfo::dispatch_sig(CUser*, char*, int)";

int Inter_LoginInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_LOGIN_DATA* pInfo = static_cast<SIG_LOGIN_DATA*>(
        static_cast<void*>(data));

    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        user->SetServerGroup(static_cast<ENUM_SERVER_GROUP>(
            pInfo->m_onlinePrelimServerGroup));
    }
    if (G_GameWorld()->GetChannelType() == 7)
    {
        user->SetServerGroup(static_cast<ENUM_SERVER_GROUP>(
            pInfo->m_tournamentServerGroup));
    }
    user->setWebAddress(pInfo->m_webAddress);

    PacketGuard guard;
    guard.put_header(1, 1);
        if (pInfo->m_errCode == 1)                       // E_CANTFIND
        {
            guard.put_byte(0);
            guard.put_byte(6);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1b0a, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_CANTFIND");
            user->DisConnSig(DISCONN_SIG(0xb), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 8)                       // 双重登录互踢
        {
            guard.put_byte(0);
            guard.put_byte(0x60);
            guard.finalize(true);
            user->Send(guard);
            user->DisConnSig(DISCONN_SIG(0x21), true, 0);
            return 0;
        }
        if (9 < pInfo->m_errCode && pInfo->m_errCode < 0xd)   // PUNISH_REASON
        {
            guard.put_byte(0);
            guard.put_byte(static_cast<unsigned char>(pInfo->m_errCode - 0x26));
            guard.put_byte(pInfo->m_loginErrDetail);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1b67, 0)("pInfo->err_code == SIG_LOGIN_DATA::PUNISH_REASON");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 4)                       // E_AUTH
        {
            guard.put_byte(0);
            guard.put_byte(8);
            guard.put_int(pInfo->m_punishInfos[pInfo->m_punishInfoCount].m_endTime);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1b77, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_AUTH");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 5)                       // E_AUTH_MOUSE
        {
            guard.put_byte(0);
            guard.put_byte(0x48);
            guard.put_byte(pInfo->m_passPadFailCnt);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1ba0, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_AUTH_MOUSE");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 6)                       // E_AUTH_MOUSE_DENY
        {
            guard.put_byte(0);
            guard.put_byte(0x49);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1baa, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_AUTH_MOUSE_DENY");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 9)                       // E_PASSPAD_WRONG_VERSION
        {
            guard.put_byte(0);
            guard.put_byte(0xa6);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1bb4, 0)("pInfo->err_code == SIG_LOGIN_DATA::E_PASSPAD_WRONG_VERSION");
            user->DisConnSig(DISCONN_SIG(0x1a), true, 0);
            return 0;
        }
        if (pInfo->m_errCode == 7)                       // 版本不符
        {
            guard.put_byte(0);
            guard.put_byte(0x5f);
            guard.finalize(true);
            user->Send(guard);
            user->DisConnSig(DISCONN_SIG(0x20), true, 0);
            return 0;
        }

        user->setHangameUser(pInfo->m_hangameUser != false);
        if (pInfo->m_errCode != 0)                       // 其余未分类错误
        {
            guard.put_byte(0);
            guard.put_byte(1);
            guard.finalize(true);
            user->Send(guard);
            cMyTrace(g_interLoginInfoSig, 0x1bf2, 0)("[Inter_LoginInfo::dispatch_sig] Abnormal LoginError: "
                  "SIG_LOGIN_DATA::ERROR_OK(%d)", pInfo->m_errCode);
            user->DisConnSig(DISCONN_SIG(0xe), true, 0);
            return 0;
        }

        cMyTrace(g_interLoginInfoSig, 0x1bf6, 0)("[USER LOGIN] DB ID : %s (UID: %d)(SlotIDX: %d)(IncreID: %d)"
                   "(SocketID: %d)\n",
                   NumberToString(pInfo->m_accId, 0),
                   user->GetUID(), user->GetSlotIDX(), user->GetIncreID(),
                   user->get_unique_id() & 0xffff);

        // 同账号重复登录拒绝
        CUser* existing = G_CGameManager()->GetUserByAccId(pInfo->m_accId);
        if (existing != 0 && existing->m_network.get_socket() != -1 &&
            existing->get_state() != 0)
        {
            existing->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(1), 0x12);
            existing->DisConnSig(DISCONN_SIG(0xc), true, 0);
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(1), 0x12);
            return 0x1c18;
        }

        user->set_acc_info(pInfo->m_accId, reinterpret_cast<char*>(pInfo));
        char peerIp[16] = {'\0'};
        user->m_network.GetPeerIP2(peerIp, 0x10);

        // 地区限制检测（跳过标志=1 时免检）
        // ORIG 084c5cd3..084c5f1c：两个 std::string 均为调用点临时对象，由
        // _ZNSsC1EPKcRKSaIcE 直接从 char* 构造（无 _ZNSsC1ERKSs 拷贝构造）；
        // Singleton::Get() 在两个临时对象之后调用；&& 短路使临时对象析构走
        // 标志位公共清理块（报告 C 行 2774-2819 的 bVar6/7/9/11 + bVar8）。
        if (pInfo->m_geoAllowSkipFlag != 1 &&
            ARAD::Singleton<RestrictGeolocation>::Get()->isAllow(
                std::string(user->get_acc_name()), std::string(peerIp)) != true)
        {
            guard.put_byte(0);
            guard.put_byte(0x41);
            guard.finalize(true);
            user->Send(guard);
            user->DisConnSig(DISCONN_SIG(0x15), true, 0);
        }

        // 综合 PvP 渠道的地区码记录
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
            char countryCode[16] = {'\0'};
            // ORIG 084c5f5a..084c60ad：两处 std::string 同为调用点临时对象
            // （报告 C 行 2839-2857），无中间局部 string、无 bool 中转。
            ARAD::Singleton<RestrictGeolocation>::Get()
                ->getCountryCode(countryCode, 0x10, std::string(peerIp));
            user->setAllowLocationGeoState(
                ARAD::Singleton<RestrictGeolocation>::Get()
                    ->isAllow(std::string(countryCode)));
        }

        user->set_state(static_cast<ch_state>(2));
        G_GameWorld()->InsertLoginUser(user);

        // 向 DB 线程请求游戏选项加载
        // ORIG 0x84c60f1 Acquire 结果直进 guard 实参（无 stream 槽），但
        // 0x84c616b `mov %eax,-0x54(%ebp)` 保留 GetInBuffer 结果的具名槽
        // ⇒ 实测内联 stream 后全帧槽基右移、ndiff 1277→1319（回退，P321-T5）。
        Stream* stream = GlobalData::s_stream_pool->Acquire(
            "InterDispatcher.cpp", 0x1c7e);
        CStreamGuard streamGuard(stream, true);
        streamGuard.operator*()->operator<<(0x12e);
        streamGuard.operator*()->operator<<(static_cast<int>(user->GetUID()));
        SIG_REQ_LOAD_GAME_OPTION* gameOption =
            streamGuard->GetInBuffer<SIG_REQ_LOAD_GAME_OPTION>();
        gameOption->m_accId = user->get_acc_id();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, streamGuard);

        G_CGameManager()->insertUserByAccID(user);
        user->SetUserDetailInfo(pInfo->m_userDetailIpPort, pInfo->m_birth,
                                pInfo->m_userDetailSsn);
        user->SetBirth(pInfo->m_birth);

        // premium 生效表回放（末项触发重算）
        for (int i = 0; i < pInfo->m_premiumCount; ++i)
        {
            bool last = (pInfo->m_premiumCount - 1 == i);
            WongWork::CHandlePremium::handleSetUserPremium(
                user, static_cast<ENUM_PREMIUM_TYPE>(pInfo->m_premiums[i].m_type),
                pInfo->m_premiums[i].m_startTime,
                pInfo->m_premiums[i].m_endTime,
                pInfo->m_premiums[i].m_flag, last);
        }

        user->setRegDate(pInfo->m_regDate);
        user->setRegTime(pInfo->m_regTime);
        if (0 < pInfo->m_characCount)
        {
            user->SetCharacInfo(pInfo->m_characCount, pInfo->m_characData);
            user->SetNeedCheckOverEquipItem();
        }

        // 雇佣兵残留竞技态 → 累积服务器侧 hack 计数
        if (0 < pInfo->m_mercenaryCount)
        {
            user->SetCharacMercenaryInfo(pInfo->m_mercenaryCount,
                                         &pInfo->m_mercenaries[0]);
            bool hasCompetition = true;
            for (int i = 0; i < pInfo->m_mercenaryCount && i < 0x24; ++i)
            {
                if (pInfo->m_mercenaries[i].m_field28 != 0)
                {
                    hasCompetition = false;
                    break;
                }
            }
            if (hasCompetition)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE(0x1aa), 1, 0, 0);
            }
        }

        for (int i = 0; i < pInfo->m_restrictedGoodsCount; ++i)
        {
            user->SetRestrictedGoods(pInfo->m_restrictedGoods[i]);
        }

        const char* ssn = user->GetSsnString();
        unsigned char sex = user->getSex();
        char* ipRet = user->m_network.GetPeerIP2(peerIp, 0x10);
        int channelNo = G_CEnvironment()->get_channel_no();
        CMonitorServerProxy* monitorProxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                static_cast<ENUM_SERVER_GROUP>(user->GetServerGroup()));
        monitorProxy->SendLogin(user->GetUID(), pInfo->m_accId, channelNo,
                                ipRet, sex, const_cast<char*>(ssn));
        ssn = user->GetSsnString();
        sex = user->getSex();
        ipRet = user->m_network.GetPeerIP2(peerIp, 0x10);
        channelNo = G_CEnvironment()->get_channel_no();
        CGuildServerProxy* guildProxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(
                static_cast<ENUM_SERVER_GROUP>(user->GetServerGroup()));
        guildProxy->SendLogin(user->GetUID(), pInfo->m_accId, channelNo,
                              ipRet, sex, const_cast<char*>(ssn));
        GlobalData::s_double_check_proxy->SendLogin(
            pInfo->m_accId, G_CEnvironment()->get_channel_no(),
            G_CEnvironment()->m_serverEnvir.m_serverGroup);

        user->GetGoldControl()->SetInfo(pInfo->m_goldInfo);

        // 处罚信息回放 A：有效期内的道具掉率提升（type==0x65）。
        // ORIG:每处使用重新调用 getCurSec()（无缓存）。
// §P183/P331：ORIG 对 data + i*0x18 + 0x3a54 的每个字段访问都从包基址重新物化
// （`mov -0x60(%ebp),%ecx; imul 3; shl 3; lea; add $0x3a54; mov` 共 14 处，其中
// add $0x3a54×9、add $0x3a68×5），我方原以 `SIG_LOGIN_PUNISH_INFO& entry` 起基址
// 一次 ⇒ 缺 66 条地址计算（orig 2462 / our 2396）。引用改宏直取，两循环的 i 均为
// 循环变量且循环体不改 i/pInfo ⇒ 寻址结果逐次相同，无可观察行为变化。
#define entry (pInfo->m_punishInfos[i])
        for (int i = 0; i < pInfo->m_punishInfoCount; ++i)
        {
            if (99 < entry.m_type && entry.m_type < 200)
            {
                bool active = !(entry.m_endTime < GlobalData::s_systemTime_.getCurSec() &&
                                entry.m_endTime != 0);
                if (active && entry.m_type == 0x65)
                {
                    user->setStdDropRate(entry.m_stdDropRate);
                }
            }
        }
        // 处罚信息回放 B：交易处罚类型映射
        for (int i = 0; i < pInfo->m_punishInfoCount; ++i)
        {
            bool active = !(entry.m_endTime < GlobalData::s_systemTime_.getCurSec() &&
                                entry.m_endTime != 0);
            if (!active)
            {
                continue;
            }
            switch (entry.m_type)
            {
            case 4:   user->SetTradePunishType(8);   break;
            case 0xb: user->SetTradePunishType(1);   break;
            case 0xc: user->SetTradePunishType(4);   break;
            case 0xd: user->SetTradePunishType(0x20); break;
            case 0xe: user->SetTradePunishType(2);   break;
            case 0x67:user->SetTradePunishType(0x80); break;
            default: break;
            }
        }
#undef entry

        user->setTotalHackUserTradeCnt(pInfo->m_totalHackUserTradeCnt);
        // ORIG 0x84c68de：`mov 0x3a2c(%eax),%edx; mov 0x3a28(%eax),%eax; mov
        // %eax,0x4(%esp); mov %edx,0x8(%esp)` ⇒ 传完整 64 位（低@+0x3a28、
        // 高@+0x3a2c）。只传低 32 位会丢失高位（真值差）。
        user->setTotalHackUserTradeGold(
            *reinterpret_cast<unsigned long long*>(
                &pInfo->m_totalHackUserTradeGold));
        if (GlobalData::g_autoPunishRuleBlackIPMgr->isBlackIP(
                pInfo->m_blackIpCheckBuf))
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, WongWork::ENUM_HACKTYPE(0x32a), 1, 0, 0);
        }

        user->setRestingUserRestrict(pInfo->m_restingUserRestrict != false);
        user->setHumanCertifyErrorCnt(0);
        user->setHumanCertified(true);

        time_t now = GlobalData::s_systemTime_.getCurSec();
        struct tm localNow;
        struct tm* pNow = localtime_r(&now, &localNow);
        int curYear = pNow->tm_year;
        int curMon = pNow->tm_mon;
        int curMday = pNow->tm_mday;

        user->getDungeonGainedGold()->setGainedGold(pInfo->m_dungeonGainedGold);
        user->SetReliablePerson(true);
        user->SetReliablePerson(pInfo->m_reliablePersonFlag != 0);

        // 当日 06:00 窗口外的超限金币清零
        pNow->tm_hour = 6;
        time_t dayStart = mktime(pNow);
        if (dayStart < static_cast<long>(pInfo->m_lastPlayTime))
        {
            user->SetOverTradeGoldCount(pInfo->m_overTradeGoldCount);
        }
        else
        {
            user->SetOverTradeGoldCount(0);
        }

        // 同日举报数保留，跨日清零
        struct tm lastPlayTm;
        struct tm* pLast = localtime_r(
            reinterpret_cast<time_t*>(&pInfo->m_lastPlayTime), &lastPlayTm);
        if (pLast->tm_year == curYear && pLast->tm_mon == curMon &&
            pLast->tm_mday == curMday)
        {
            user->SetReportMannerlessUserCount(pInfo->m_reportMannerlessUserCount);
        }
        else
        {
            user->SetReportMannerlessUserCount(0);
        }

        user->SetSchoolNo(pInfo->m_schoolNo);
        user->SetRating(pInfo->m_rating);
        user->SetSchoolName(pInfo->m_schoolName);
        user->SetDailyBadge(pInfo->m_dailyBadge);
        user->SetTutorialSkipable(pInfo->m_tutorialSkipable);
        CommonTime commonTime;
        commonTime.SetCurTime();
        user->SetAge(static_cast<unsigned int>(
            static_cast<int>(*reinterpret_cast<char*>(commonTime.m_pad)) -
            pInfo->m_birthYear + 0x7d1));
        user->setSecuReward(pInfo->m_secuReward != false);
        user->setSecuType(pInfo->m_secuType);
        user->setRewardType(pInfo->m_rewardType);
        user->setSecuGrade(pInfo->m_secuType & 0x27);

        if (pInfo->m_secCardActivate != false)
        {
            user->getSecurityCard()->setSecurityCardInfo(
                pInfo->m_secCardSerial, pInfo->m_secCardActivate != false,
                pInfo->m_secCardCertified != false, pInfo->m_secCardCertCnt,
                pInfo->m_secCardCertFlag, pInfo->m_secCardCancelCnt,
                pInfo->m_secCardAnswers);
        }
        user->getPad()->setVersionInfo(pInfo->m_passPadVersion);

        if (pInfo->m_passPadEnable != false)             // GoblinPad 主流程
        {
            // ORIG:每处使用重新调用 getPad()（无缓存）。
            user->getPad()->setActivate(true);
            user->getPad()->setCancelCnt(pInfo->m_passPadCancelCnt);
            user->getPad()->setVersionInfo(pInfo->m_passPadVersion);
            user->getPad()->setCertified(pInfo->m_passPadCertified != false);
            user->getPad()->setCheckTime(pInfo->m_passPadCheckTime);
            bool inWindow = CheckDailyScheduleTime(
                G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                pInfo->m_lastPlayTime,
                GlobalData::s_systemTime_.getCurSec());
            if (!inWindow)
            {
                user->getPad()->setFailCnt(pInfo->m_passPadFailCnt);
            }
            else if (0 < pInfo->m_passPadFailCnt)
            {
                user->getPad()->setFailCnt(0);
                user->getPad()->setCancelCnt(0);
                DB_PassPadUpdateFailCnt::makeRequest(user->get_acc_id(),
                                              static_cast<unsigned int>(user->getPad()->getFailCnt()),
                                              const_cast<char*>(user->getWebAddress()));
                cMyTrace(g_interLoginInfoSig, 0x1df5, 0)("GOBLIN PAD RESET OK!!!");
            }
            user->getPad()->setPassword(pInfo->m_passPadPassword);
            // ORIG:每次使用重新调用 getHades()。
            static_cast<XNuclear::CHades*>(user->getHades())->Send_Login();
            static_cast<XNuclear::CHades*>(user->getHades())->StdCateUpdate(3, 0);
            user->getPad()->setAdvideChangePwd(
                pInfo->m_passPadAdvideChangePwd != false);
        }
        user->getPad()->setCertified(true);

        switch (pInfo->m_punishTradeAlertType)
        {
        case 0xd: user->setPunishTradeAlert(0xe7); break;
        case 0xe: user->setPunishTradeAlert(0xe8); break;
        default:  user->setPunishTradeAlert(0);    break;
        }

        user->setOnlinePreliminaryCharacNo(pInfo->m_onlinePrelimCharacNo);
        user->setOnlinePreliminaryServerGroup(static_cast<ENUM_SERVER_GROUP>(
            pInfo->m_onlinePrelimServerGroup));
        user->setOnlinePreliminaryPlayType(pInfo->m_onlinePrelimPlayType);
        {
            cMyTrace(g_interLoginInfoSig, 0x1e33, 0)("OnlinePreliminary m_id(%s), play_type(%d)",
                    NumberToString(pInfo->m_accId, 0),
                    pInfo->m_onlinePrelimPlayType);
        }
        user->setTournamentAccount(pInfo->m_tournamentAccountFlag != 0 &&
                                   pInfo->m_tournamentServerGroup != 0);

        if (G_GameWorld()->GetChannelType() == 7)       // 综合赛事渠道上报
        {
            Packet_Monitor_UDP_Login pkt;
            Stream* monStream = GlobalData::s_stream_pool->Acquire(
                "InterDispatcher.cpp", 0x1e3b);
            CStreamGuard monGuard(monStream, true);
            monGuard.operator*()->operator<<(0x41);
            monGuard.operator*()->operator<<(static_cast<int>(user->GetUID()));
            pkt.m_channelId = user->GetUID();
            pkt.m_dbid = user->get_acc_id();
            pkt.m_channel =
                static_cast<unsigned char>(G_CEnvironment()->get_channel_no());
            pkt.m_sex = 1;
            pkt.m_cache0 = 0;
            pkt.m_cache4 = 0;
            monGuard->put_binary(&pkt, sizeof(pkt));
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, monGuard);
        }

        user->setClientVersion(pInfo->m_clientVersion);
        user->SetAccountLastPlayTime(pInfo->m_lastPlayTime);
        user->setJoinedSchoolPointEvent(
            pInfo->m_joinedSchoolPointEventFlag != false);
        user->m_field8ec32 = 0;

        char* punIp = user->m_network.GetPeerIP2(user->m_mcap.m_pcRoomIp, 0x14);
        user->m_mcap.m_blackIpPunishMask =
            GlobalData::g_blackIPMonitor->getPunishType(punIp);
        if ((user->m_mcap.m_blackIpPunishMask & 1) != 0)
        {
            bool punishNow = false;
            char* maskIp = user->m_network.GetPeerIP2(user->m_mcap.m_pcRoomIp, 0x14);
            if (!(GlobalData::g_blackIPMonitor->GetEndTime(maskIp,
                      WongWork::ENUM_BLACKIPMONITOR_PUNISH_TYPE(1)) <
                  GlobalData::s_systemTime_.getCurSec()))
            {
                punishNow = true;
            }
            if (punishNow)
            {
                user->SetTradePunishType(0x10);
                DB_BlackipPrivateSecurityValuation::makeRequest(user->get_acc_id());
            }
        }

        int cleanPadBase = pInfo->m_cleanPadDecreaseCnt;
        if (!CheckDailyScheduleTime(
                G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                pInfo->m_lastPlayTime,
                GlobalData::s_systemTime_.getCurSec()))
        {
            --pInfo->m_cleanPadDecreaseCnt;
        }
        else
        {
            pInfo->m_cleanPadDecreaseCnt = 0;
        }
        user->m_mcap.incPoint(pInfo->m_cleanPadDecreaseCnt);
        if (user->m_mcap.getPoint() ==
            static_cast<unsigned int>(cleanPadBase))
        {
            user->m_mcap.disableCleanPadPoint();
        }
        if ((user->m_mcap.m_blackIpPunishMask & 2) != 0)
        {
            bool lockNow = false;
            char* maskIp = user->m_network.GetPeerIP2(user->m_mcap.m_pcRoomIp, 0x14);
            if (!(GlobalData::g_blackIPMonitor->GetEndTime(maskIp,
                      WongWork::ENUM_BLACKIPMONITOR_PUNISH_TYPE(2)) <
                  GlobalData::s_systemTime_.getCurSec()))
            {
                lockNow = true;
            }
            if (lockNow)
            {
                user->m_mcap.m_blackIpTradeLockFlag = true;
                if (user->m_mcap.getPoint() < 0x251c)
                {
                    user->m_mcap.incPoint(0x251c);
                }
            }
        }

        if (pInfo->m_returnUserDbFlag == false)          // 归还用户判定
        {
            if (pInfo->m_lastPlayTime == 0)
            {
                user->ResetReturnUser();
            }
            // ORIG 0x84c754f 为 `cmp %eax,%ebx; setl`（有符号）⇒ 原码按 int
            // 语义比较（我方字段 unsigned int 直比会生 setb，P196/T2 直证）。
            else if (static_cast<long>(pInfo->m_lastPlayTime) + 0x3b53800 <
                     GlobalData::s_systemTime_.getCurSec())
            {
                user->SetReturnUser();
                int expire = GlobalData::s_systemTime_.getCurSec() + 0x127500;
                user->SetReturnUserExpireTime(expire);
                user->SetReturnUserFirstUserLogin(true);
                Timer_ReturnUserTimeout::registNextTimer(
                    user->GetUID(), user->get_unique_id() & 0xffff,
                    user->GetReturnUserExpireTime());
            }
            else
            {
                user->ResetReturnUser();
            }
        }
        else
        {
            user->SetReturnUser();
            user->SetReturnUserExpireTime(pInfo->m_returnUserExpireTime);
            user->SetReturnUserFirstUserLogin(pInfo->m_returnUserFirstLogin !=
                                              false);
            Timer_ReturnUserTimeout::registNextTimer(
                user->GetUID(), user->get_unique_id() & 0xffff,
                user->GetReturnUserExpireTime());
        }

        user->resetAccountMemberBonusFatigue();
        int fatigueTime = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
        if (CheckDailyScheduleTime(fatigueTime,
                pInfo->m_memberBonusSaveTime,
                GlobalData::s_systemTime_.getCurSec()))
        {
            user->EnableSaveMemberBonusFatigue();
        }
        else
        {
            user->incAccountMemberBonusFatigue(pInfo->m_memberBonusFatigueInc);
        }

        user->setUserEventCharacterFlag(pInfo->m_userEventCharacterFlag);
        user->SetMileage(pInfo->m_mileage);
        user->setCharacLinkMessageFlag(pInfo->m_characLinkMessageFlag != false);

        if (pInfo->m_breakAwayUvListFlag != false)       // 断线回归名单刷新
        {
            pInfo->m_breakAwayUvListFlag =
                GlobalData::s_BreakAwaySys->updateUvList(pInfo) ? true : false;
        }
        if (0 < pInfo->m_breakAwayLuckPoint)
        {
            user->setBreakAwayRewardData(true);
            user->setUserBreakAwayRewardLuckPoint(pInfo->m_breakAwayLuckPoint);
        }
        if (0 < pInfo->m_breakAwayDungeonClearCnt)
        {
            // ORIG:if/else 两分支各一次调用（静态双站点）。
            // ORIG 0x84c771d：`cmp %eax,%edx`(edx=A,eax=B)+`jbe` → A<=B 走
            // order3、A>B 走 order1（Ghidra 同结论：0x3f44<0x3f40→order1）。
            if (pInfo->m_breakAwayRewardBoundA > pInfo->m_breakAwayRewardBoundB)
            {
                user->setBreakAwayRewardOrder(static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(1));
            }
            else
            {
                user->setBreakAwayRewardOrder(static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(3));
            }
            user->setBreakAwayDungeonClearCnt(pInfo->m_breakAwayDungeonClearCnt);
            user->setBreakAwayQuestTime(pInfo->m_breakAwayQuestTime);
        }
        if (-1 < pInfo->m_breakAwayAccureCera)
        {
            user->setBreakAwayAccureCera(pInfo->m_breakAwayAccureCera);
        }

        CEventBase* repeatEvent =
            GlobalData::s_event_manager->GetRepeatEvent(0x34);
        bool startBreakAway =
            repeatEvent->IsEventing(0) && pInfo->m_breakAwayUvListFlag != false;
        if (startBreakAway)
        {
            GlobalData::s_BreakAwaySys->startBreakAwayPreventSystem(pInfo, user);
        }

        DB_LoadAccountCargo::makeRequest(user->GetUID(), user->get_acc_id());
        user->SetPuUser(pInfo->m_puUser != false);
        user->setCharacSlotCountInfo(pInfo->m_characSlotCurCount,
                                     pInfo->m_characSlotMaxCount);
        user->set_ontime_last_recv_idx(pInfo->m_ontimeLastRecvIdx);
        user->setWebAddress(pInfo->m_webAddress);
        if (pInfo->m_checkPickUpRandomOptionItemFlag != false)
        {
            user->setCheckPickUpRandomOptionItem(true);
        }
        user->reset_server_fatigue_data();
        user->set_fatigue_count_db(pInfo->m_fatigueKind, pInfo->m_fatigueValue);
        user->set_ingame_event_history(&pInfo->m_ingameEventHistory);
        if (pInfo->m_ingameHackItemFlag != false)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, WongWork::ENUM_HACKTYPE(0x261), 1, 0, 0);
        }
        user->getAcccountHackInfo()->setLoginTime();
        user->loadInformNoticeFlag(pInfo->m_informNoticeBuf);
        user->ValidateStrikerInfo();
        user->CalcurateManageLevel();
        user->CalcurateUserMaxLevel();
        user->SetBingoData(*pInfo);
        user->initBingoData();
        user->sendReturnUserFirstLogin();
        user->SetGrowthWeaponEventInfo(pInfo->m_growthWeaponEvent);
        user->SetGrowthCreatureEventInfo(pInfo->m_growthCreatureEvent);
        user->setCharacLevelUpGift_AccountOnce(
            pInfo->m_levelupGiftAccountOnce);
        user->sendEventInfo();
        user->SetEventCreateDnfReward(false);
        DBSelectCreateDnfUserInfo::makeRequest(user->GetUID(), user->get_acc_id());
        user->m_field8ec32 = 0;

        if (Taiwan::GarenaCyberCafe(
                reinterpret_cast<Taiwan::GarenaAuthData*>(
                    user->getGarenaAuthData())->getGcaType()))
        {
            DBSelectPcroomDailyReward::makeRequest(user);
        }
    return 0;
}

Inter_LoginInfo::~Inter_LoginInfo() {}

Inter_CreateCharac::~Inter_CreateCharac() {}
// [b28d-lane2] verdict=DIFF ndiff=380（ORIG 0x084bee06）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84bee06）：本轮修 P167 六分支 put_byte
// （补 4 调用点）+ m_serverEnvir.m_serverGroup 字段读（消 get_server_group EXTRA）；
// 余 N18a 单段 regmove + N82/P112 帧槽平移（大函数）。
int Inter_CreateCharac::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x084bee06 报告 docs/class_func_reports/Inter_CreateCharac/dispatch_sig.md
    // 两段全读。SIG_CREATE_CHARAC 布局见 SigTypes.h（m_slot/m_field23/m_eventCharacFlag/
    // m_resultCode/m_characNo 全具名）。
    if (user->get_state() == 0)
        return 0x6c0;
    if (!user->unlockCreateCharac())
        return 0x6c4;

    SIG_CREATE_CHARAC* sig = reinterpret_cast<SIG_CREATE_CHARAC*>(data);
    PacketGuard packet;                    // ORIG 局部包对象全程复用
    packet.put_header(1, 5);

    if (sig->m_resultCode != 0)
    {   // DB 失败应答：put_byte(0) + 错误映射字节（ORIG 每分支各自 put_byte，p167 6 个调用点）
        packet.put_byte(0);
        if (sig->m_resultCode == 1)
            packet.put_byte(0x12);
        else if (sig->m_resultCode == 4)
            packet.put_byte(0x14);
        else if (sig->m_resultCode == 5)
            packet.put_byte(0x5f);
        else if (sig->m_resultCode == 3)
            packet.put_byte(0x18);
        else
            packet.put_byte(2);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    if ((char)sig->m_field23 > '\0')
    {   // 跳跳角色活动日志（ORIG localtime/sprintf/inet_ntoa/CMyFileLog 链）
        const long curSec = GlobalData::s_systemTime_.getCurSec();
        struct tm localTm;
        struct tm* pTm = localtime_r(&curSec, &localTm);
        char timeBuf[256];
        sprintf(timeBuf, "%02d:%02d:%02d",
                pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
        char ipStr[16] = {
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
            '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
        in_addr addr;
        addr.s_addr = static_cast<unsigned int>(user->get_public_ip_address());
        strcpy(ipStr, inet_ntoa(addr));
        char logPath[255];
        for (int i = 0; i < 255; ++i)
            logPath[i] = '\0';
        sprintf(logPath, "./log/%s/JumpingCharacter",
                G_CEnvironment()->get_file_name());

        const unsigned int characNo = sig->m_characNo;
        const int serverGroup = G_CEnvironment()->get_server_group();
        const bool hangame = user->isHangameUser() ? true : false;
        char* accStr =
            NumberToString(static_cast<unsigned int>(user->get_acc_id()), 0);
        CMyFileLog log(
            "virtual int Inter_CreateCharac::dispatch_sig(CUser*, char*, int)",
            0x703);
        log(logPath, ",%s,%d,%d,%d,%s,%s", accStr, hangame, serverGroup,
            static_cast<int>(characNo), ipStr, timeBuf);

        user->setUserEventCharacterFlag(sig->m_eventCharacFlag);
    }

    user->CreateCharac(sig);
    HistoryLog::WriteCreateCh(
        *reinterpret_cast<FILE**>(&user->m_field796f8),   // +0x796f8 文件句柄槽
        sig->m_name, static_cast<int>((char)sig->m_slot));
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(2);
    user->make_basic_info(reinterpret_cast<char*>(&packet), 2);
    packet.finalize(true);
    user->Send(packet);
    user->UpdateCharacView();
    process_event(user, sig);

    // 跳跳角色奖励判定：服务器组==10 且 2011-01-01 活动期内且 sig+0x534d 非 0
    // （ORIG 0x84bf255 cmp $0x478e7050;ja → x<=0x478e7050 含边界）
    // [b28d-solo-InterDispatcher] P226 语句表达式：ORIG 0x84bf255..0x84bf268 短路物化
    // 岛（mov$1/mov$0/test al）零槽直测；具名 bool 初值+置位形 4.4 恒落槽（+3 条）。
    // ORIG 0x84bf239 内联字段读 `mov 0x378(%eax),%eax`（0x198+0x1e0）⇒ 字段访问，非 getter 调用。
    if (({ G_CEnvironment()->m_serverEnvir.m_serverGroup == 10 &&
          GlobalData::s_systemTime_.getCurSec() <= 0x478e7050; }) &&
        *reinterpret_cast<unsigned char*>(
            &sig->m_pad534d[0]) != 0)   // +0x534d
    {   // 活动创建礼（按职业表 0x1c70..0x1c74 取道具）
        Inven_Item item;
        int giftIdx = 0;
        switch ((char)sig->m_slot)
        {
        case 0: giftIdx = 0x1c70; break;
        case 1: giftIdx = 0x1c71; break;
        case 2: giftIdx = 0x1c72; break;
        case 3: giftIdx = 0x1c73; break;
        case 4: giftIdx = 0x1c74; break;
        }
        item.m_field1 = 2;
        item.m_addInfo2 = 1;
        item.m_fieldb = 0;
        item.ResetItemAttr();
        if (giftIdx != 0 && G_CDataManager()->find_item(giftIdx) != 0)
        {
            const char* titleRaw = g_scriptStringManager_.findString(
                4, "game_server_msg_10", reinterpret_cast<bool*>(0));
            const int titleLen = static_cast<int>(strlen(titleRaw));
            const char* body = g_scriptStringManager_.findString(
                4, "game_server_msg_10", reinterpret_cast<bool*>(0));
            const char* sender = g_scriptStringManager_.findString(
                4, "game_server_msg_09", reinterpret_cast<bool*>(0));
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                sender, item, 0u, sig->m_characNo, body, titleLen,
                0u, user->GetServerGroup(), false, false);
        }
    }

    CEventBase* ryosika = GlobalData::s_event_manager->GetRepeatEvent(0x74);
    if (ryosika != 0)
        reinterpret_cast<CAradRyosikaEvent*>(ryosika)->DoEvent(
            user, static_cast<int>(sig->m_characNo), 1,
            static_cast<int>((char)sig->m_slot));
    return 0;
}

// ORIG T _ZN18Inter_CreateCharac13process_eventEP5CUserP17SIG_CREATE_CHARAC
// @0x084bf4ac（报告 process_event.md 两段全读；薄转发）。
void Inter_CreateCharac::process_event(CUser* user, SIG_CREATE_CHARAC* sig)
{
    process_createGift(user, sig);
}

// ORIG T _ZN18Inter_CreateCharac18process_createGiftEP5CUserP17SIG_CREATE_CHARAC
// @0x084bf4ce（报告 process_createGift.md 两段全读）：取角色创建礼包脚本，
// 按 m_startTime/m_endTime 过滤后逐道具 find_item → make_item → 系统邮件投递。
// [b28d-lane2] verdict=DIFF ndiff=204（ORIG 0x084bf4ce）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84bf4ce）：本轮 P28 后置 it++（ppEi，p167 CLEAR）；余 N18a 单段 regmove+N82/P112 帧槽平移。
void Inter_CreateCharac::process_createGift(CUser* user, SIG_CREATE_CHARAC* sig)
{
    if (user == 0)
        return;

    const int job = static_cast<int>((char)sig->m_slot);
    createChracScript::rewardInfoList gifts;
    G_CDataManager()->get_createCharacterGift(job, gifts);
    if (gifts.m_list.empty())
        return;

    const long regTime = static_cast<long>(user->getRegTime());
    const ENUM_SERVER_GROUP group =
        static_cast<ENUM_SERVER_GROUP>(user->GetServerGroupToPvP());
    const size_t rewardCount = gifts.m_list.size();
    for (size_t r = 0; r < rewardCount; ++r)
    {
        createChracScript::rewardInfo& info = gifts.m_list[r];
        // 时间窗过滤：(from<1 或 from<=regTime) && (to<1 或 regTime<to)
        if (!((info.m_startTime < 1 || info.m_startTime <= regTime) &&
              (info.m_endTime < 1 || regTime < info.m_endTime)))
            continue;
        for (std::vector<int>::iterator it = info.m_items.begin();
             it != info.m_items.end(); it++)   // P28：ORIG 增量段 ppEi ⇒ 后置++
        {
            Inven_Item item;                       // ORIG 局部 Inven_Item C1
            CItem* ci = G_CDataManager()->find_item(*it);
            if (ci == 0)
                continue;
            const int itemIdx = ci->get_index();
            ci->make_item(item);                   // vtable 槽 +0x08
            item.set_add_info(1);
            char body[256];
            strncpy(body,
                    g_scriptStringManager_.findString(
                        4, info.m_text.c_str(), reinterpret_cast<bool*>(0)),
                    0xff);
            if (itemIdx == 0)
                continue;
            const int expire = ci->getExpirationDate();
            const bool hasExpire = expire == 0;
            const bool noExpire = expire != 0;
            const unsigned int bodyLen = static_cast<unsigned int>(strlen(body));
            const char* sender = g_scriptStringManager_.findString(
                4, info.m_title.c_str(), reinterpret_cast<bool*>(0));
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                sender, item, 0u, sig->m_characNo, body,
                static_cast<int>(bodyLen),
                static_cast<unsigned int>(expire), group,
                hasExpire, noExpire);
        }
    }
}

Inter_DeleteCharac::~Inter_DeleteCharac() {}
// ORIG 0x084bf810 T dispatch_sig —— 删除角色应答分发：结果码 1/2/3 直接回执，
// 其余构造 Packet_Monitor_Charac_Delete 上报 monitor/guild 后本地删角色并刷基础信息。
// 报告 docs/class_func_reports/Inter_DeleteCharac/dispatch_sig.md 全文转写。
// msg 布局：+4 charNo(int) +8 result(int) +0xc guildKey(int) +0x10 eventCharacFlag(char)。
#pragma pack(push, 1)
// Inter_DeleteCharacPktView —— Inter_DeleteCharac::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_DeleteCharacPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned int m_4;               // +0x04
    int m_8;                        // +0x08
    unsigned int m_guildKey;        // +0x0c 语义名←函数头注释
    unsigned char m_eventCharacFlag;// +0x10 语义名←函数头注释
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=143（ORIG 0x084bf810）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=106（ORIG 0x84bf810）：P264 .o 重定位/立即数。
int Inter_DeleteCharac::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0x812;
    if (!user->unlockDeleteCharac())                   // xor $1；!=1 → 拒绝
        return 0x817;

    // [b28d-lane2] ORIG 084bf9bb 段：data spill 在两道守卫之后（`mov 0x10(%ebp),%eax;
    // mov %eax,-0xc(%ebp)`），声明位置与 ORIG 一致。
    Inter_DeleteCharacPktView* pPkt = reinterpret_cast<Inter_DeleteCharacPktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 6);

    if (pPkt->m_8 == 1)
    {   // 正在删除保护期
        packet.put_byte(0);
        packet.put_byte(2);
        packet.finalize(true);
        user->Send(packet);
        return 0x825;
    }
    else if (pPkt->m_8 == 2)
    {
        packet.put_byte(0);
        packet.put_byte(0x24);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else if (pPkt->m_8 == 3)
    {
        packet.put_byte(0);
        packet.put_byte(0xd5);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    Packet_Monitor_Charac_Delete delMsg;               // ORIG C1 0x084e9318 W
    delMsg.m_accId = (unsigned int)user->get_acc_id();
    delMsg.m_charNo = pPkt->m_4;
    delMsg.m_guildKey = pPkt->m_guildKey;

    // [b28d-lane2] ORIG 084bfa2e 段：GetServerProxy 结果直落 (%esp)（无具名 proxy 槽，
    // P183/P0）——旧具名 monitor/guild 各占 4B 槽并多发 2×2 条 mov（-0x10/-0xc 双槽）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup())->SendTcpPacket((char*)&delMsg, 0x16);
    if (delMsg.m_guildKey != 0)
    {
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->SendTcpPacket((char*)&delMsg, 0x16);
    }

    user->DeleteCharac(pPkt->m_4);
    packet.put_byte(1);
    // 块序：ORIG 084bfa67 `test %al; je <else 块>` + then（setFlag+put_byte(1)）直落
    //   ⇒ 正极性 `if (flag != 0) {then} else {else}`（P59/P214① 反写）。
    // [lane2 2026-09-10] 原 `if (flag == 0) {…} else {…}` 产 4.4 反向块序
    //   （jne <then> + else 直落）；极性反写后 jcc 目标下标与 ORIG 逐位一致。
    if (pPkt->m_eventCharacFlag != 0)
    {
        user->setUserEventCharacterFlag(0);
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
    }
    packet.finalize(true);
    user->Send(packet);

    packet.clear();                                    // ORIG InterfacePacketBuf::clear
    packet.put_header(0, 2);
    packet.put_byte(2);
    user->make_basic_info((char*)&packet, 2);
    packet.finalize(true);
    user->Send(packet);
    user->UpdateCharacView();
    return 0;
}

Inter_LoadCharac::~Inter_LoadCharac() {}
// ORIG 0x084c3398 T dispatch_sig —— 登录加载角色。
// 报告 docs/class_func_reports/Inter_LoadCharac/dispatch_sig.md 全文转写
// （## 汇编 + ## 反编译 C）。
#pragma pack(push, 1)
// Inter_LoadCharacPktView —— Inter_LoadCharac::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadCharacPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned int m_4;               // +0x04
    int m_8;                        // +0x08
    unsigned int m_c;               // +0x0c
    char m_pad10[0x1e];  // +0x10..未消费
    char m_2e;                      // +0x2e
    char m_pad2f[0xf9];  // +0x2f..未消费
    short m_128;                    // +0x128
    char m_pad12a[0x1];
    signed char m_12b;              // +0x12b
    char m_pad12c[0xc];  // +0x12c..未消费
    bool m_138;                     // +0x138
    unsigned char m_139;            // +0x139
    char m_13a;                     // +0x13a
    char m_pad13b[0x5];  // +0x13b..未消费
    unsigned int m_140;             // +0x140
    char m_pad144[0x542];  // +0x144..未消费
    unsigned int m_686;             // +0x686
    unsigned char m_68a;            // +0x68a
};
#pragma pack(pop)

// §6.2 巨件核销：P167 106/106 [0缺0多]；gap -16→-16；残差归因 N82（4.4 槽复用全帧错位，
// 详见 docs/asm_translation_patterns.md 负例表）。batch-20「Inter 巨兽」已核销，本次维持。
// [b28d-lane2] verdict=DIFF ndiff=578（ORIG 0x084c3398）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-16。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=394（ORIG 0x84c3398）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N101 imul/lea 折叠。
// [lane2 2026-09-10] 真 bug 修复：`*(unsigned char*)(&user->m_characList[slotIdx]) = pPkt->m_68a;`
//   （写偏移 0）→ `user->m_characList[slotIdx].m_field14b7`（CUser.h:719 权威成员 +0x14b7；
//   ORIG 084c364e `mov %dl,0x14b7(%eax)` 单条 disp 直证）。
int Inter_LoadCharac::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadCharacPktView* pPkt = reinterpret_cast<Inter_LoadCharacPktView*>(data);
    if (user->get_state() == 0)                        // state==0 → 0x13dd
        return 0x13dd;
    if (pPkt->m_8 != 0)        // req field @+0x8 → 0x13e3
        return 0x13e3;

    const unsigned int curAccId = (unsigned int)user->get_acc_id();
    const unsigned int reqAccId =                      // msg+0x140
        pPkt->m_140;
    if (curAccId != reqAccId)
    {   // 异号 MID 记账：SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK
        Stream* stream =
            GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x13f9);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<(0x9d);
        guard.operator*()->operator<<(-1);
        SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK* sig =
            guard->GetInBuffer<SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK>();
        sig->m_curAccId = static_cast<unsigned int>(user->get_acc_id());
        sig->m_reqAccId = reqAccId;
        sig->m_characNo = pPkt->m_c;
        user->m_network.GetPeerIP2(sig->m_peerIp, 0x14);   // +0x0c
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
        return 0x1405;
    }

    if (pPkt->m_2e == '\0')  // charac list 空检查
        return 0x140b;

    const unsigned int reqCharacNo =                   // msg+0x04
        pPkt->m_4;
    const int slotIdx = user->get_charac_idx(reqCharacNo);
    if (slotIdx < 0 ||
        !user->setCharacInfoDetail(slotIdx, reqCharacNo,
                                   reinterpret_cast<CHARAC_DATA*>(data + 0xc)))
        return 0x1415;

    unsigned int tagCharacNo =                         // msg+0x686
        pPkt->m_686;
    int tagIdx = user->get_charac_idx(tagCharacNo);
    if (tagCharacNo != 0 && tagIdx != -1)
    {
        if (tagIdx < 0 ||
            !user->setTagCharacInfo(tagIdx, tagCharacNo,
                                    reinterpret_cast<TAG_CHARAC_DATA*>(data + 0x68b)))
            return 0x141f;
        // +0x14b7 strikerSkillIndex（_Charac_info::m_field14b7，CUser.h:719 权威成员；
        //   ORIG 084c364e `mov %dl,0x14b7(%eax)` 单条 disp 直证）。
        user->m_characList[slotIdx].m_field14b7 = pPkt->m_68a;
    }
    user->SelectCharac(slotIdx, tagIdx);

    const int characCount = user->get_charac_count();
    user->decideLinkCharacSlotIndexes((char)slotIdx, characCount);
    if (!user->checkCharacLinkType((char)slotIdx, user->get_charac_count()))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1430, 5)("LINK_LOG : ABNOMAL LINK CHARAC!! m_id (%s), slot_index(%d)",
              NumberToString((unsigned int)user->get_acc_id(), 0), slotIdx);
        return 0x1431;
    }

    if (user->isCharacLinkBonusSetting((char)slotIdx))
        user->setCharacLinkBonusSetting(slotIdx, false);
    user->SetCurCharacEmotion(
        pPkt->m_128);
    {   // 扩展槽状态域 msg+0x12b：<0 或 >3 归零
        const signed char exSlot = pPkt->m_12b;
        // ORIG 两个独立 SetCurExpEquipSlotStat 调用点（越界 → 0，否则 → 原值）。
        if (exSlot < 0 || exSlot > 3)
            user->SetCurExpEquipSlotStat(0);
        else
            user->SetCurExpEquipSlotStat(exSlot);
    }
    {   // 公会代理信息拉取
        // ORIG get_charac_guildkey 两次独立调用（非 GM 门判 + 代理查找实参）。
        if (user->get_charac_guildkey() != 0)
        {
            STGuildProxy proxy;                        // ORIG 默认构造局部
            CGuildServerProxy* proxyMgr =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(
                    user->GetServerGroup());
            if (proxyMgr->GetGuildInfoProxy(
                    user->get_charac_guildkey(), proxy))
                user->SetGuildInfoProxy(proxy);
        }
    }
    user->setCurCharacHelpAbuseComputedRatio(
        static_cast<unsigned int>(user->calc_disp_help_abuse_ratio(0)));
    user->EnableCharacInfo(CACHE_CHARAC);          // ORIG 0x84c386b（calc 后、ReqDBLoadMail 前）
    WongWork::CMailBoxHelper::ReqDBLoadMail(user, reqCharacNo);
    if (!G_GameWorld()->InsertUserToIDMap(data + 0x10,
                                          (unsigned int)user->get_acc_id()))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1464, 5)("Insert into IDMap Failed %s [%s]",
              data + 0x10,
              NumberToString((unsigned int)user->get_acc_id(), 0));
    }

    // ---- 当日回访/幽灵状态判定 ----
    const long nowSec = GlobalData::s_systemTime_.getCurSec();
    time_t tNow = nowSec;
    struct tm today;
    struct tm* pToday = localtime_r(&tNow, &today);
    const int todayYear = pToday->tm_year;
    const int todayMon = pToday->tm_mon;
    const int todayMday = pToday->tm_mday;

    if ((long)nowSec >= user->GetCurCharacChaosResponTime())
    {
        // ORIG 0x84c3963：responTime <= now → ghost=false 并清零计时。
        user->SetCurCharacGhost(false);
        user->SetCurCharacChaosDieTime(0);
        user->SetCurCharacChaosResponTime(0);
    }
    else
    {
        // ORIG 0x84c39b2：ghost=true；dieLeft=responTime-dieTime、
        // responLeft=responTime-now，NotifyGhostTime(user, dieLeft, responLeft)。
        user->SetCurCharacGhost(true);
        const int dieLeft = static_cast<int>(
            user->GetCurCharacChaosResponTime() -
            user->GetCurCharacChaosDieTime());
        const int responLeft = static_cast<int>(
            user->GetCurCharacChaosResponTime() - nowSec);
        pvp_assault::CAssaultMgr::NotifyGhostTime(user, dieLeft, responLeft);
    }
    user->SetCurCharacLeaveTimeout(false);

    {
        const long lastPlayTick = user->getCurCharacLastPlayTick();
        if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                                   lastPlayTick,
                                   GlobalData::s_systemTime_.getCurSec()))
        {
            user->setCurCharacTradeGoldDaily(0);
        }
        user->SetCurCharacLuckPointUpdateTimeCount(0);
    }
    {
        time_t lastPlay = user->getCurCharacLastPlayTick();
        struct tm lastDay;
        struct tm* pLast = localtime_r(&lastPlay, &lastDay);
        if (pLast->tm_year != todayYear || pLast->tm_mon != todayMon ||
            pLast->tm_mday != todayMday)
        {
            GetInstanceValueStatistic()
                ->AddValueStatistic((VALUE_STATISTIC_FIELD)0, user, 1);
        }
    }

    int ComputeDaysScheduleTime(int hour, long lastTime, long curTime);  // ORIG 0x846c144（GameUtils.cpp）
    unsigned long StringToNumber(const char* s);          // ORIG T 0x081090fa（DNFFunctionLib.cpp 定义）

    {   // antibot 用户信息上报（vtable+0x38 槽，类未建模——asm 桥残留保留，按 §8 协议待类建模后清理）
        user->fillUserInfo4Antibot();
        const char* userInfo = user->getUserInfo4Antibot();
        unsigned char payload[0x48];   // ORIG local_150/local_14c/acStack_148 连续区
        *reinterpret_cast<int*>(&payload[0]) =
            static_cast<int>(StringToNumber(user->get_acc_name()));
        *reinterpret_cast<int*>(&payload[4]) = -1;
        strncpy(reinterpret_cast<char*>(&payload[8]), userInfo, 0x40);
        typedef void (*ReportFn)(void*, int, int, int, void*, int);
        void* checker = GlobalData::s_antibotChecker_;
        ReportFn report = *(ReportFn*)((*(void***)checker)[0x38 / 4]);
        report(checker, G_CEnvironment()->getProcessSequence(), 0, 100000,
               &payload[0], 0x48);
    }
    {   // PowerWar 未连接天数扣点链（lastPlayTickPowerWar = curCharac +0x7b long）
        // ORIG 4 次 `_ZNK getCurCharacR`（门判 + 读 +0x7b + trace 两处）、
        // 2 次 getCurSec（扣点判定 + trace）。+0x7b 直接 long 读，非 memcpy。
        if (*reinterpret_cast<const int*>(
                reinterpret_cast<const char*>(user->getCurCharacR()) + 0x7b) != 0)
        {
            const long cur = GlobalData::s_systemTime_.getCurSec();
            const long lastPlayTickPw =
                *reinterpret_cast<const long*>(
                    reinterpret_cast<const char*>(user->getCurCharacR()) + 0x7b);
            const int days = ComputeDaysScheduleTime(
                G_CEnvironment()->m_serverEnvir.m_fatigueTime, lastPlayTickPw, cur);
            // ORIG 读 CDataManager +0xa6b4（powerwar point per day；
            // 暂无具名成员 [blocked: CDataManager 布局批补名]）
            const int perDayPoint =
                *reinterpret_cast<const int*>(
                    reinterpret_cast<const char*>(G_CDataManager()) + 0xa6b4);
            if (days > 0)
            {
                user->DecreasePowerWarPoint((short)((short)perDayPoint * (short)days));
                cMyTrace(__PRETTY_FUNCTION__,
                    0x14e4, 0)("POWERWAR POINT DECREASE : user:%d, last_play_tme:%d, "
                      "current_time:%d, Unconnected Days:%d, Per Day Point : %d, "
                      "Decrease powerwar point:%d",
                      *reinterpret_cast<const int*>(
                          reinterpret_cast<const char*>(user->getCurCharacR())),
                      *reinterpret_cast<const int*>(
                          reinterpret_cast<const char*>(user->getCurCharacR()) + 0x7b),
                      (int)GlobalData::s_systemTime_.getCurSec(),
                      days, perDayPoint, days * perDayPoint);
            }
        }
    }

    if (GlobalData::s_power_manager->IsPowerWarEventOn() == false)
    {
        if (user->GetPowerWarAssaultCount() != 0 ||
            user->GetPowerWarAssaultVictoryCount() != 0)
        {
            user->ResetPowerWarAssaultRecord();
        }
    }

    EventClassify::CEventScriptMng* eventMng = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    if (eventMng->is_eventing(0x19b))
    {
        user->setCurUseBringUpMaterialTotal(pPkt->m_139);
        user->setBringUpRewardFlag(pPkt->m_138 ? true : false);
        user->SendEventBringUpInfo();
    }
    user->select_charac_set_visible_values(pPkt->m_13a);
    user->decide_growth_power_reward_system();
    return 0;
}

// ============================================================================
// Inter_LoadInven::dispatch_sig ORIG 0x084c3f92 T（402 insn）——登录加载背包。
bool _verifyInventory(SIG_LOAD_INVENTORY* sig,
                      std::vector<item_lock::stItemLockRef>& lockRefs,
                      CUser* user);   // 定义 LinkClosureD.cpp（ORIG W 0x084f0145）
int GetItemIdByAddInfo(Inven_Item* pool, int poolCount, int itemIdx);   // 定义 LC_Misc.cpp（ORIG _Z18GetItemIdByAddInfoP10Inven_Itemii：非 const 形参）
// 报告 docs/class_func_reports/Inter_LoadInven/dispatch_sig.md 两段全读。
// 错误码：0x1835/0x183c/0x1841/0x1872/0x1892。
#pragma pack(push, 1)
// Inter_LoadInvenPktView —— Inter_LoadInven::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadInvenPktView
{
    char m_pad0[0x8];  // +0x0..未消费
    unsigned int m_8;               // +0x08
    char m_padc[0x4024];  // +0xc..未消费
    int m_4030;                     // +0x4030
    char m_pad4034[0x10];           // +0x4034..未消费
    Inven_Item m_4044;              // +0x4044 §4.3 字段化（Inven_Item 0x3d；拆 pad 保 sizeof 不变）
    char m_pad4081[0x8c9b];  // +0x4081..未消费
    int m_cd1c;                     // +0xcd1c
    char m_padcd20[0x5b5c];  // +0xcd20..未消费
    int m_1287c;                    // +0x1287c
    char m_pad12880[0x1ed8];  // +0x12880..未消费
    unsigned long m_14758;          // +0x14758
    char m_pad1475c[0xbe0];  // +0x1475c..未消费
    int m_1533c;                    // +0x1533c
    int m_15340;                    // +0x15340
    char m_pad15344[0x104];  // +0x15344..未消费
    int m_15448;                    // +0x15448
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=410（ORIG 0x084c3f92）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-26。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=288（ORIG 0x84c3f92）：N101 imul/lea 折叠+N7 间接调用寄存器+N82/P112 帧槽平移。
int Inter_LoadInven::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadInvenPktView* pPkt = reinterpret_cast<Inter_LoadInvenPktView*>(data);
    if (user->get_state() == 0)
        return 0x1835;
    if (pPkt->m_4030 == 1)          // 重复加载保护位
        return 0x183c;
    if (user->getCurCharacR() == 0)
        return 0x1841;

    // P48 去缓存：ORIG 每用点重取 getCurCharacInvenW；set_inventory_capacity 走 InvenR
    // （ORIG (CInventory*)getCurCharacInvenR 丢 const 调非 const 方法）。
    const_cast<CInventory*>(user->getCurCharacInvenR())->set_inventory_capacity(pPkt->m_1533c);
    {   // 背包校验：失败断连（ORIG _Z16_verifyInventory W 0x084f0145）
        CExpandEquipslot* expandData =
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)2);
        item_lock::CItemLock* lockData =
            reinterpret_cast<item_lock::CItemLock*>(expandData);
        std::vector<item_lock::stItemLockRef>& lockVec = lockData->GetItemLockRefVec();
        if (!_verifyInventory(reinterpret_cast<SIG_LOAD_INVENTORY*>(data), lockVec, user))
        {
            user->DisConnSig((DISCONN_SIG)0x24, true, 0);
        }
    }

    user->getCurCharacInvenW()->GetCreatureMgrW()->Reset();
    user->getCurCharacInvenW()->GetCreatureMgrW()->SetUser(user);
    {
        const int creatureCount = pPkt->m_1287c;
        for (int i = 0; i < creatureCount; ++i)
        {
            const int itemOffset = *reinterpret_cast<int*>(data + 0x12880 + i * 0x38);
            Inven_Item* item = reinterpret_cast<Inven_Item*>(
                data + 0xeed1 + itemOffset * 0x3d);
            SIG_CREATURE_ITEM_DETAIL_INFO* detail =
                reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
                    data + 0x12880 + i * 0x38);
            user->getCurCharacInvenW()->GetCreatureMgrW()->LoadCreatureItem(item, detail);
        }
        const int tempPeriodCount = pPkt->m_15340;
        for (int i = 0; i < tempPeriodCount; ++i)
        {
            SIG_POSTAL_CREATURE_TEMP_PERIOD* period =
                // 权威槽：SIG_LOAD_INVENTORY::m_tempPeriods[0x14] @+0x15344
                //（= (0x2a68*8+4)，元素 8 字节 = sizeof(SIG_POSTAL_CREATURE_TEMP_PERIOD)）。
                &reinterpret_cast<SIG_LOAD_INVENTORY*>(data)->m_tempPeriods[i];
            user->getCurCharacInvenW()->GetCreatureMgrW()->LoadCreatureTempPeriod(period);
        }
    }
    if (!user->SetInventory(reinterpret_cast<SIG_LOAD_INVENTORY*>(data)))
        return 0x1872;

    const unsigned int tagCharacNo =
        pPkt->m_8;
    const int tagIdx = user->get_charac_idx(tagCharacNo);
    if (tagCharacNo != 0 && tagIdx != -1 &&
        !user->setTagCharacInven(reinterpret_cast<TAG_CHARAC_INVEN_DATA*>(data + 0xc)))
        return 0x1892;

    user->getCurCharacInvenW()->SetClearAvatar(
        &pPkt->m_14758);   // §4.3 字段化：视图 m_14758(unsigned long) 取址，ORIG lea 0x14758(%eax)
    (void)tagIdx;
    {
        const int avatarCount = pPkt->m_cd1c;
        for (int i = 0; i < avatarCount; ++i)
        {
            const int avatarItemIdx =
                *reinterpret_cast<int*>(data + i * 0x4b + 0xcd45);
            int addInfo = 0;
            if (avatarItemIdx != 0)
            {
                addInfo = GetItemIdByAddInfo(&pPkt->m_4044,
                                             0x69, avatarItemIdx);
                for (int j = 0; j < 10; ++j)
                {
                    if (*reinterpret_cast<int*>(data + j * 0x3d + 0xa3a8) ==
                        *reinterpret_cast<int*>(data + i * 0x4b + 0xcd20))
                    {
                        user->getCurCharacInvenW()->SetClearAvatar(j, addInfo);
                    }
                }
            }
            AvatarDetailInfo* detail = reinterpret_cast<AvatarDetailInfo*>(
                data + i * 0x4b + 0xcd20);
            user->getCurCharacInvenW()->GetAvatarItemMgrW()->RegistItem(*detail, false);
        }
    }
    {   // maxEquipLevel msg+0x15448 非 0 时设置
        const int maxEquipLevel = pPkt->m_15448;
        if (maxEquipLevel != 0)
            user->SetCurCharacMaxEquipLevel(maxEquipLevel);
    }
    _CheckOverEquipItem(user);
    _CheckUsableExpertJobEquipment(user);
    user->EnableCharacInfo((CACHE_SECTION)1);
    if (G_GameWorld()->GetChannelType() == 7)
    {   // 战场频道强制卸下强化
        Inven_Item* ref = user->getCurCharacInvenW()->GetInvenRef(0, 10);
        ref->SetUpgrade(0);
        for (int slot = 0xb; slot < 0x16; ++slot)
        {
            user->getCurCharacInvenW()->GetInvenRef(0, slot)->SetUpgrade(0);
        }
        for (int slot = 3; slot < 0x39; ++slot)
        {
            Inven_Item* it = user->getCurCharacInvenW()->GetInvenRef(1, slot);
            if (it->m_field1 == '\x01')
            {
                if (G_CDataManager()->find_item(it->m_addInfo) != 0)
                {
                    it->SetUpgrade(0);
                }
            }
        }
    }
    // 扩展槽族槽表导入（ORIG vptr+4 = CExpandEquipslot::loadData 虚槽；类型完整，转真调用）
    // P48：ORIG 两独立显式调用（section 2 / 9），非循环（循环合并发明 helper 形）。
    {
        CExpandEquipslot* e2 =
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)2);
        e2->loadData(user, data + 0x14784);
    }
    {
        CExpandEquipslot* e9 =
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        e9->loadData(user, data + 0x14aa4);
    }
    user->disableSaveInven();
    user->disableSaveCargo();
    _UpdateRentalItems(user, reinterpret_cast<SIG_LOAD_INVENTORY*>(data));
    _UpdateDimensionRentalItems(user, reinterpret_cast<SIG_LOAD_INVENTORY*>(data));
    return 0;
}
// ---- ORIG 私有辅助实现（本 TU 唯一 owner；其余两个后续批转写）----

// ORIG T _ZN15Inter_LoadInven19_CheckOverEquipItemEP5CUser @0x084c4d38
// （报告 docs/class_func_reports/Inter_LoadInven/_CheckOverEquipItem.md 两段全读）
// [b28d-lane2] verdict=DIFF ndiff=216（ORIG 0x084c4d38）：槽序/帧差(N82/P212/P221)+混合残差；gap-13。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=157（ORIG 0x84c4d38）：N18a 单段 regmove+N82/P112 帧槽平移。
void Inter_LoadInven::_CheckOverEquipItem(CUser* user)
{
    if (!user->needCurCharacCheckOverEquip())
        return;
    if (user->getCurCharacR() == 0)
        return;

    CInventory* inven = user->getCurCharacInvenW();
    for (int slot = 0; slot <= 0x15; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot(0, slot);
        if (item.m_addInfo == 0)
            continue;
        CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
        if (ci == 0 || ci->is_stackable())
            continue;

        int overLevel = user->GetPremiumInfo()->GetOverEquipableLevel(
            (ENUM_EQUIPMENTTYPE)slot);   // ORIG 传槽号（0..0x15）
        const int maxEquip = user->GetCurCharacMaxEquipLevel();
        const int levelGap = maxEquip - user->get_charac_level();
        overLevel = std::max(overLevel, levelGap);       // ORIG +0x0f5 call std::max<int>(int&,int&)

        // ORIG 3 次 get_charac_level（O0 不 CSE）：levelGap + check_low_level + GetUsableMaxLevel
        bool needMove = false;                          // bVar1
        if (ci->check_low_level(user->get_charac_level() + overLevel))
            needMove = true;
        else if (ci->GetAttachType() == 5)
        {
            CEquipItem* equip = static_cast<CEquipItem*>(ci);
            if (equip->GetUsableMaxLevel() < user->get_charac_level())
                needMove = true;
        }
        if (!needMove)
            continue;

        if (ci->IsAvatarItem())                         // vtable+0x10 槽
        {   // 化身位 → 移入 avatar 仓空槽
            const int emptySlot =
                inven->get_empty_slot((INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8);
            if (emptySlot != -1)
                inven->move_item((INVEN_TYPE)0, slot, (INVEN_TYPE)2, emptySlot);
        }
        else
        {
            inven->ResetSlot(0, slot);                  // 仓库重置 → 回背包并邮寄兜底
            const bool insertFailed =
                (inven->insertItemIntoInventory(item, (eItemAddReason)0xe, false, false)
                 & 0x80000000) == 0;
            if (!insertFailed)
            {
                WongWork::CMailBoxHelper::ReqDBSendNewMail(
                    user, item, 0,
                    user->getCurCharacNo(), true, 0, 0);
            }
        }
    }
    user->resetCurCharacOverEquipFlag();
}

// ORIG T _ZN15Inter_LoadInven30_CheckUsableExpertJobEquipmentEP5CUser @0x084c4b38
// （报告 docs/class_func_reports/Inter_LoadInven/_CheckUsableExpertJobEquipment.md
// 两段全读）：槽 10..0x15 装备经 isExpertJobUsable 校验，不可用则清槽回插背包，
// 插入失败走 ReqDBSendNewMail 兜底。
void Inter_LoadInven::_CheckUsableExpertJobEquipment(CUser* user)
{
    if (user->getCurCharacR() == 0)
        return;
    CInventory* inven = user->getCurCharacInvenW();
    for (int slot = 0xa; slot <= 0x15; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot(0, slot);
        if (item.m_addInfo == 0)
            continue;
        CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
        if (ci == 0)
            continue;
        if (ci->is_stackable())
            continue;
        if (!ci->isExpertJobUsable(
                (ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(),
                user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp())))
        {   // 专家职业不匹配 → 清槽并回收进背包
            inven->ResetSlot(0, slot);
            if (inven->insertItemIntoInventory(item, (eItemAddReason)0xe, false, false)
                & 0x80000000)
            {   // ORIG shr $0x1f 位测：插入失败 → 邮件兜底
                WongWork::CMailBoxHelper::ReqDBSendNewMail(
                    user, item, 0u,
                    static_cast<unsigned int>(user->getCurCharacNo()),
                    true, reinterpret_cast<const char*>(0), 0);
            }
        }
    }
}

// ORIG T _ZN15Inter_LoadInven18_UpdateRentalItemsEP5CUserP18SIG_LOAD_INVENTORY
// @0x084c455c（报告 docs/class_func_reports/Inter_LoadInven/_UpdateRentalItems.md
// 两段全读）：PC 房/维度频道直接清租借；否则机器键 1 按 exchange 标志取消息表或
// 当前角色租借清单装配 RentInput，空表删除记录，非空走 Rent 成功后回写。
// [b28d-lane2] verdict=DIFF ndiff=201（ORIG 0x084c455c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-8。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84c455c）：本轮 P167/P0 具名引用 r=*it 单次 de+字段回读（p167 CLEAR，pt×4→de×1）；余 N18a/N82/P112 帧槽。
void Inter_LoadInven::_UpdateRentalItems(CUser* user, SIG_LOAD_INVENTORY* sig)
{
    if (user == 0 || sig == 0)
        return;

    const int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        user->DeleteRentalItemInfo(1);
        return;
    }

    InstanceRentalSystem::RentInput input;
    input.m_machineKey = 1;
    input.m_logFlag = 1;
    if (!user->CheckLoadRentalInfoFromExchange())
    {
        std::vector<InstanceRentalSystem::RentalInfo> rentalList;
        if (user->GetCurrentCharacRentalInfo(rentalList, 1))
        {
            for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it =
                     rentalList.begin();
                 it != rentalList.end(); ++it)
            {
                // P167/P0：ORIG 单次 operator*（de）→ 槽往返 → 4 字段回读 ⇒ 具名引用局部。
                const InstanceRentalSystem::RentalInfo& r = *it;
                InstanceRentalSystem::RentInElem elem;      // ← RentalInfo+8..+0xf
                elem.m_sectionIdx = r.m_field8;             // +0x08
                elem.m_itemIdx = r.m_field9;                // +0x09
                elem.m_type = 0;
                elem.m_bEquip = r.m_inEquip != 0 ? 1 : 0;   // +0x0a
                elem.m_rentItemId = r.m_rentItemId;         // +0x0c
                input.m_elems.push_back(elem);
            }
        }
    }
    else
    {   // exchange 表：msg + i*0xc + 0x153e4，仅本轮 1 条；{int!=0, byte4, byte5,
        // byte6(bEquip), int8}（ORIG 逐字节 movzbl 读）
        for (int i = 0; i < 1; ++i)
        {
            char* p = reinterpret_cast<char*>(sig) + i * 0xc + 0x153e4;
            if (*reinterpret_cast<int*>(p) == 0)
                continue;
            InstanceRentalSystem::RentInElem elem;
            elem.m_sectionIdx = p[4];
            elem.m_itemIdx = p[5];
            elem.m_bEquip = p[6] != 0 ? 1 : 0;
            elem.m_type = 0;
            elem.m_rentItemId = *reinterpret_cast<int*>(p + 8);
            input.m_elems.push_back(elem);
        }
    }

    if (input.m_elems.empty())
    {
        user->DeleteCurrentCharacRentalItemInfo(1);
    }
    else
    {
        InstanceRentalSystem::RentOutput output;
        const int rentRet =
            static_cast<int>(GlobalData::s_rentalSystem->Rent(*user, input, output));
        if (rentRet == 0)
            user->UpdateCurrentCharacRentalInfo(output, 1, true);
    }
}

// ORIG T _ZN15Inter_LoadInven24_UpdateDimensionRentalItemsEP5CUserP18SIG_LOAD_INVENTORY
// @0x084c4858（报告 _UpdateDimensionRentalItems.md 两段全读）：维度租借机器键 2、
// exchange 表 0xb 条 stride 8（msg+(i+0x2a7e)*8）、elem.type 取自表第 7 字节或
// RentalInfo+0xb；其余与 PC 房租借同构。m_rentItemId 表路径无数据，按 ORIG
// RentInElem ctor 0x0822f02e 置 -1 [推断：movl $0xffffffff 实证]。
// [b28d-lane2] verdict=DIFF ndiff=194（ORIG 0x084c4858）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-18。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84c4858）：本轮 P167/P0 具名引用 r=*it 单次 de+字段回读（p167 CLEAR，pt×5→de×1）；余 N18a/N82/P112 帧槽。
void Inter_LoadInven::_UpdateDimensionRentalItems(CUser* user,
                                                  SIG_LOAD_INVENTORY* sig)
{
    if (user == 0 || sig == 0)
        return;

    const int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        user->DeleteRentalItemInfo(2);
        return;
    }

    InstanceRentalSystem::RentInput input;
    input.m_machineKey = 2;
    if (!user->CheckLoadRentalInfoFromExchange())
    {
        std::vector<InstanceRentalSystem::RentalInfo> rentalList;
        if (user->GetCurrentCharacRentalInfo(rentalList, 2))
        {
            for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it =
                     rentalList.begin();
                 it != rentalList.end(); ++it)
            {
                // P167/P0：ORIG 单次 operator*（de）→ 槽往返 → 字段回读 ⇒ 具名引用局部。
                const InstanceRentalSystem::RentalInfo& r = *it;
                InstanceRentalSystem::RentInElem elem;      // ← RentalInfo+8..+0xf
                elem.m_sectionIdx = r.m_field8;             // +0x08
                elem.m_itemIdx = r.m_field9;                // +0x09
                elem.m_bEquip = r.m_inEquip != 0 ? 1 : 0;   // +0x0a
                elem.m_type = r.m_fieldB;                   // +0x0b
                elem.m_rentItemId = r.m_rentItemId;         // +0x0c
                input.m_elems.push_back(elem);
            }
        }
    }
    else
    {
        for (int i = 0; i <= 0xa; ++i)
        {
            char* p = reinterpret_cast<char*>(sig) + (i + 0x2a7e) * 8;
            if (*reinterpret_cast<int*>(p) == 0)
                continue;
            InstanceRentalSystem::RentInElem elem;
            elem.m_sectionIdx = p[4];
            elem.m_itemIdx = p[5];
            elem.m_bEquip = p[6] != 0 ? 1 : 0;
            elem.m_type = p[7];
            elem.m_rentItemId = -1;
            input.m_elems.push_back(elem);
        }
    }

    if (input.m_elems.empty())
    {
        user->DeleteCurrentCharacRentalItemInfo(2);
    }
    else
    {
        InstanceRentalSystem::RentOutput output;
        const int rentRet =
            static_cast<int>(GlobalData::s_rentalSystem->Rent(*user, input, output));
        if (rentRet == 0)
            user->UpdateCurrentCharacRentalInfo(output, 2, true);
    }
}

Inter_LoadInven::~Inter_LoadInven() {}

Inter_LoadSkill::~Inter_LoadSkill() {}
// ORIG 0x084c5152 T dispatch_sig —— 纯 ret 空函数。
// 报告 docs/class_func_reports/Inter_LoadSkill/dispatch_sig.md 全文转写（汇编 9 字节，mov $0x0,%eax; ret）。
int Inter_LoadSkill::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    return 0;
}

Inter_LoadQuest::~Inter_LoadQuest() {}
// ORIG 0x084c5054 T dispatch_sig —— DB 任务数据装载。
// 报告 docs/class_func_reports/Inter_LoadQuest/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadQuestPktView —— Inter_LoadQuest::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadQuestPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    int m_4;                        // +0x04
};
#pragma pack(pop)

int Inter_LoadQuest::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0x1a13;
    // ORIG 0x084c5079：pPkt 在 state 岛后才物化 -0x10（技巧13 声明位置）。
    Inter_LoadQuestPktView* pPkt = reinterpret_cast<Inter_LoadQuestPktView*>(data);
    if (pPkt->m_4 == 1)
        return 0x1a1a;
    if (user->getCurCharacR() == 0)
        return 0x1a1d;

    // ORIG 0x084c50bd：questR 具名槽 -0xc（+0xc3 先物化 pPkt 实参、+0xc7 槽装 this）。
    UserQuest* questR = user->getCurCharacQuestR();
    questR->set_quest(
        reinterpret_cast<const SIG_LOAD_QUEST*>(pPkt));

    if (GlobalData::s_timeGate_->isOpen())
    {
        // b28c-l23 P0 清绑：ORIG 0x084c50f0/0x084c511a 两段 `mov %eax,%ebx` 跨 call
        // 缓存（4.1 直落 callee-saved）+ 两次独立 getQuestIndex 重读（P48 无 CSE）。
        // 绑定形与 P0（-O0 局部恒占栈槽）相悖已删；4.4 自然形把值落槽（N96/N18a
        // 反向判词，P168a④），语义不变。[b28c-l23 修正] 保留**裸 register**：
        // P0 禁的是 `register T x asm("reg")` 寄存器绑定，而 4.4 对带 register 的
        // 标量局部不给栈槽、值留 callee-saved 跨 call——正是 ORIG 形（lane9/lane17
        // 同实测：删关键字反而槽化退化）。
        register int questIdx = GlobalData::s_timeGate_->getQuestIndex();
        if (user->getCurCharacQuestR()->IsDoingQuest(questIdx))
        {
            register int questIdx2 = GlobalData::s_timeGate_->getQuestIndex();
            user->getCurCharacQuestW()->giveup_quest(questIdx2);
        }
    }
    user->EnableCharacInfo((CACHE_SECTION)3);
    return 0;
}

Inter_LoadTicket::~Inter_LoadTicket() {}
// ORIG 0x084c515c T dispatch_sig —— 点券 DB 加载。
// 报告 docs/class_func_reports/Inter_LoadTicket/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadTicketPktView —— Inter_LoadTicket::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadTicketPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    int m_4;                        // +0x04
    int m_8;                        // +0x08
    int m_ids[1];                   // +0x0c 线格式数组区（ORIG 0x084c51c3 mov 0xc(%eax,%edx,4) 下标 i 循环消费）
};
#pragma pack(pop)

int Inter_LoadTicket::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0x1a70;
    if (user->get_state() == 0)
        return 0x1a72;
    // ORIG 0x084c515a：pPkt 在 state 检查后才物化 -0x10；count 无槽（每次重载 +0x8）
    Inter_LoadTicketPktView* pPkt = reinterpret_cast<Inter_LoadTicketPktView*>(data);
    if (pPkt->m_4 == 1)
        return 0x1a79;
    if (pPkt->m_8 > 99)
        return 0x1a7e;
    // ORIG cond 重载 n 且方向 n>i（cmp MEM,%eax; setg）；体走数组视图折叠寻址
    for (int i = 0; pPkt->m_8 > i; ++i)
        user->AddTicket(pPkt->m_ids[i], false);
    user->EnableCharacInfo((CACHE_SECTION)4);
    return 0;
}

Inter_LoadDungeonClear::~Inter_LoadDungeonClear() {}
// ORIG 0x084c7c42 T dispatch_sig —— 副本通关记录 DB 加载。
// 报告 docs/class_func_reports/Inter_LoadDungeonClear/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadDungeonClearPktView —— Inter_LoadDungeonClear::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadDungeonClearPktView
{
    char m_pad0[0x8];  // +0x0..未消费
    bool m_8;                       // +0x08（ORIG `xor $0x1` 判非 ⇒ bool，非 unsigned char）
    char m_pad9[0x3];  // +0x9..未消费
    int m_c;                        // +0x0c
};
#pragma pack(pop)

int Inter_LoadDungeonClear::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0x200e;
    if (user->get_state() == 0)
        return 0x2012;
    if (user->IsProgLogout())
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2018,
            "[Inter_LoadDungeonClear::dispatch_sig] State(PROG_LOGOUT)\n");
        return 0;
    }
    // ORIG：包基址槽在 PROG_LOGOUT 门之后才物化。
    Inter_LoadDungeonClearPktView* pPkt = reinterpret_cast<Inter_LoadDungeonClearPktView*>(data);
    if (pPkt->m_c == 1)
        return 0x2020;
    user->getDungeonClearRefW()->fromString(user, reinterpret_cast<char*>(pPkt) + 0x10);
    // ORIG +0x5a：`movzbl 0x8(%eax),%eax`+`xor $0x1,%eax`+`test %al,%al` ⇒ bool 字段取反。
    if (!pPkt->m_8)
        user->disableSaveDungeonClear();
    user->EnableCharacInfo((CACHE_SECTION)5);
    return 0;
}

APSystem::Inter_LoadActionPoint::~Inter_LoadActionPoint() {}
// ORIG 0x08124d2a T dispatch_sig —— 行动点 DB 加载写入 CActionPointManager。
// 报告 docs/class_func_reports/APSystem__Inter_LoadActionPoint/dispatch_sig.md 全文转写。
int APSystem::Inter_LoadActionPoint::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || user->get_state() == 0 || user->getCurCharacR() == 0)
        return 0x203;
    // ORIG 0x08124d6e：包基址落 -0xc 具名槽后判空；08124d7e-87 段 `mov 形参,%eax;
    // lea 0x8d264(%eax),%edx; mov -0xc(%ebp),%eax` —— this-lea 先于实参装载仅在
    // 实参为「同型结构指针局部解引用 lvalue」时出现（probe lea1 f7；char*+cast
    // 实参会反序且退化 add）。
    _SIG_LOAD_ACTION_POINT* pPkt = reinterpret_cast<_SIG_LOAD_ACTION_POINT*>(data);
    if (pPkt == 0)
        return 0x208;
    user->m_actionPoint.Set(*pPkt);
    user->EnableCharacInfo((CACHE_SECTION)8);
    return 0;
}

APSystem::Inter_EmptyActionPoint::~Inter_EmptyActionPoint() {}
// ORIG 0x08124db0 T dispatch_sig —— 行动点空载/当日奖励检查。
// 报告 docs/class_func_reports/APSystem__Inter_EmptyActionPoint/dispatch_sig.md 全文转写。
int APSystem::Inter_EmptyActionPoint::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || user->get_state() == 0 || user->getCurCharacR() == 0)
        return 0x214;
    // ORIG +0x3f：`mov 0x10(%ebp),%eax`+`mov %eax,-0xc(%ebp)`+`cmpl $0x0,-0xc(%ebp)`
    // ⇒ 包基址先落具名局部再判空（直接比 `data` 会退化为 cmp 参数槽）。
    char* pPkt = data;
    if (pPkt == 0)
        return 0x219;
    APSystem::CUserProc::SetTodayActionAndCheckMedalReward(user);
    user->EnableCharacInfo((CACHE_SECTION)8);
    return 0;
}

APSystem::Inter_LoadRewardMedal::~Inter_LoadRewardMedal() {}
// ORIG 0x08124e2a T dispatch_sig —— 奖章 DB 更新请求。
// 报告 docs/class_func_reports/APSystem__Inter_LoadRewardMedal/dispatch_sig.md 全文转写。
int APSystem::Inter_LoadRewardMedal::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || user->get_state() == 0 || user->getCurCharacR() == 0)
        return 0x226;
    // ORIG 0x08124exx：包基址先落具名局部（-0xc）再判空；makeRequest 第 2 实参
    // `mov -0xc(%ebp),%edx; mov %edx,0x4(%esp)` 晚装填 ⇒ 需同型指针局部解引用
    // （char*+cast 形 gcc 提前缓存 %ebx，probe lea1 f1/f7 实证）。
    _SIG_LOAD_REWARD_MEDAL* pPkt = reinterpret_cast<_SIG_LOAD_REWARD_MEDAL*>(data);
    if (pPkt == 0)
        return 0x22b;
    DB_UpdateRewardMedal::makeRequest(
        (int)user->GetUID(),
        *pPkt);
    return 0;
}

advancealtar::Inter_Load::~Inter_Load() {}
// ORIG 0x0813f72a T dispatch_sig —— 祭坛角色数据 DB 加载写入管理器。
// 报告 docs/class_func_reports/advancealtar__Inter_Load/dispatch_sig.md 全文转写。
int advancealtar::Inter_Load::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || user->get_state() == 0 || user->getCurCharacR() == 0)
        return 0x2de;  // ORIG 0x0813f764 `mov $0x2de,%eax`
    // ORIG 0x0813f76b-0813f78e：包基址落 -0xc 判空后 `mov 0xc,%eax; lea 0x8df60(%eax),%edx;
    // mov -0xc(%ebp),%eax` —— this-lea 先于实参装载需实参为同型结构指针局部解引用
    // （probe lea1 f7；const 版本 set(const T&) 亦兼容）。
    const _CharacAdvanceAltarDbData* pPkt =
        reinterpret_cast<const _CharacAdvanceAltarDbData*>(data);
    if (pPkt == 0)
        return 0x2e3;
    user->m_advanceAltar.set(*pPkt);
    user->EnableCharacInfo((CACHE_SECTION)10);
    return 0;
}

Inter_DeleteChannel::~Inter_DeleteChannel() {}
#pragma pack(push, 1)
// Inter_DeleteChannelPktView —— Inter_DeleteChannel::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DeleteChannelPktView
{
    char m_pad0[0x8];  // 未消费
    int m_8;                  // +0x08
};
#pragma pack(pop)

int Inter_DeleteChannel::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    const Inter_DeleteChannelPktView* pPkt = reinterpret_cast<const Inter_DeleteChannelPktView*>(data);
    // ORIG +0x0b：单级带偏移读（无中间 int* 槽）。
    if (pPkt->m_8 == 1)
    {
        return 0x806;
    }
    return 0;
}

int Inter_Notice::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    struct NoticePacketView
    {
        char m_pad[0x51];
        char m_len;
    };
    char* packet = data;
    G_GameWorld()->send_chat_msg(0, 0, 0, 0, packet + 1,
                                 static_cast<int>(reinterpret_cast<NoticePacketView*>(packet)->m_len), 0, 0,
                                 static_cast<ENUM_NOTIPACKET>(0x0c));
    return 0;
}
// Inter_Notice ctor/dtor：ORIG C1/D1 W（对象尺寸 4，仅 vptr；ISigDispatcher 基类
// vptr 覆写），与同文件 Inter_* 家族同构。
Inter_Notice::~Inter_Notice() {}

Inter_Terminate::~Inter_Terminate() {}
int Inter_Terminate::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    puts("Game Server is Terminated");
    exit(0);
}

Inter_ReadyToTerminate::~Inter_ReadyToTerminate() {}
// ORIG 0x084c7d2c T dispatch_sig —— 关服就绪信号入 DB 队列。
// 报告 docs/class_func_reports/Inter_ReadyToTerminate/dispatch_sig.md 全文转写。
int Inter_ReadyToTerminate::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    // ORIG +0x1c：Acquire 的返回值（%eax）直接作 CStreamGuard 实参，无具名 stream 槽
    // —— 具名局部会多 `mov %eax,-0xc`+`mov -0xc,%eax` 两行并把 guard 槽推到 -0x14。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x2055), true);
    guard.operator*()->operator<<(0x3f);
    guard.operator*()->operator<<(-1);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;
}

Inter_MonitorBuddyInOut::~Inter_MonitorBuddyInOut() {}
// ORIG 0x084c8ec0 T dispatch_sig —— 好友上下线通知（0x36/0x37 包），上线且当前角色存在时补发系统邮件。
// 报告 docs/class_func_reports/Inter_MonitorBuddyInOut/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorBuddyInOutPktView —— Inter_MonitorBuddyInOut::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorBuddyInOutPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    unsigned char m_12;       // +0x12
    unsigned char m_13;       // +0x13
    char m_14;                // +0x14
    char m_15[1];             // +0x15 串/字节区首元素（容量未建模）
    char m_pad16[0x1d];  // 未消费
    char m_33;                // +0x33
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=162（ORIG 0x084c8ec0）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-12。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=102（ORIG 0x84c8ec0）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MonitorBuddyInOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorBuddyInOutPktView* pPkt = reinterpret_cast<Inter_MonitorBuddyInOutPktView*>(data);
    PacketGuard packet;
    if (pPkt->m_14 == '\x01')
        packet.put_header(0, 0x36);
    else
        packet.put_header(0, 0x37);
    packet.put_byte(
        (unsigned int)pPkt->m_12);
    packet.put_int(pPkt->m_a);
    const int local_20 = (int)strlen(pPkt->m_15);
    packet.put_int(local_20);
    packet.put_str(pPkt->m_15, local_20);
    packet.put_byte(
        (unsigned int)pPkt->m_13);
    packet.finalize(true);
    user->Send(packet);

    // [b28d-solo-InterDispatcher] P226 语句表达式：ORIG 0x084c8ff2..0x084c900f 短路物化
    // 岛（mov$1/mov$0/test al）零槽直测；具名 bool 初值+置位形 4.4 恒落槽。
    if (({ pPkt->m_33 == '\x01' &&
          user->getCurCharacR() != 0; }))
    {
        Inven_Item item;
        char* iVar2 = data + 0x15;
        const char* characName = user->getCurCharacName();
        const char* pcVar4 =
            g_scriptStringManager_.findString(4, "return_user_msg_3", 0);
        char local_16d[256];
        OS_API::snprintf(local_16d, 0xff, pcVar4, characName, iVar2);
        const ENUM_SERVER_GROUP uVar3 = user->GetServerGroup();
        const size_t sVar5 = strlen(local_16d);
        const unsigned int uVar6 = user->getCurCharacNo();
        const char* uVar7 =
            g_scriptStringManager_.findString(4, "return_user_msg_title", 0);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            uVar7, item, 0, uVar6, local_16d, sVar5, 0, uVar3, 0, 0);
    }
    return 0;
}

Inter_MonitorLoginOK::~Inter_MonitorLoginOK() {}
#include "CTimeGate.h"           // CTimeGate::isOpen/getNpcIndex（GlobalData 前向的完整类型）
#include "TimerQueue.h"          // TimerEntry::OBJ_TYPE/TIMER_MESSAGE/G_TimerQueue()
#include "CEnvironment.h"        // getProcessSequence/m_serverEnvir 字段（get_server_group）
// ---- ORIG 辅助符号（本 TU 唯一 owner，其余批未实现）----

// ORIG T _Z21_NS_NonClient_RequestP5CUser @0x08274bc5（0xfe，报告缺失，objdump 逐指令转写）：
// rand 序号 → put_header(0,0x110)/put_int/rand → Send → SetNonClientRandInt →
// G_TimerQueue()->InsertTimer(0,GetUID,0x8c,60,get_unique_id,0xffffffff)。
// [b28d-lane2] verdict=DIFF ndiff=15（ORIG 0x08274bc5）：混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=17（ORIG 0x8274bc5）：P30/N4 add-vs-lea。
// [b28c-L23 N(块序/jump threading) 残余 16 行] 唯一结构差 = PacketGuard 正常出口
// 块的位置：ORIG @0x08274c... 在 `call InsertTimer` 之后发 `jmp <统一尾>`，把
// `~PacketGuard / add $0x30 / pop×3 / ret` 留在**异常清理块之后**（4.1 为 EH
// landing pad 生成共享尾），4.4 因正常路径唯一而就地展开析构、把 EH 块推到函数尾
// ⇒ 同指令同序、块位置互换（68/69 条数差 1 源于 4.1 的额外 jmp）。压参序、
// [复原] 该块序可用「guard 包进内层作用域块 + 块为函数体末语句」复现（/tmp/probe_eh.cpp
//   f4 探针：4.4 -O0 对 `{ G g; … }` 块尾同样发 `jmp L; <unwind>; L: ~G + epilogue`）。
void _NS_NonClient_Request(CUser* user)
{
    const int randInt = rand();
    PacketGuard packet;
    packet.put_header(0, 0x110);
    packet.put_int(randInt);
    packet.finalize(true);
    user->Send(packet);
    user->SetNonClientRandInt(randInt);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                (TIMER_MESSAGE)0x8c, 60,
                                (unsigned short)user->get_unique_id(), 0xffffffff);
    // [实测 2026-09-10] ORIG 的 `jmp <共享尾>`（同指令同序、仅块位互换）在本函数未能用
    // 裸 return / 块+return / goto 尾标签复现（探针 f6/f9 在小函数命中、本函数恒就地展开）
    // ⇒ 维持 N104 记法，待后续与同类件统一复算。
}

// ORIG T DB_LoadDnfHackLog::makeRequest(int,uint) @0x084224ce（本 TU 唯一 owner；
// 报告 docs/class_func_reports/DB_LoadDnfHackLog/makeRequest.md 全文转写）：
// Acquire("DBThread.cpp",0x5abf) + guard<<0x106<<uid；GetInBuffer 前四字节=accId；
// MsgQueueMgr::put(DB_Q, guard)。
class DB_LoadDnfHackLog
{
public:
    static void makeRequest(int uid, unsigned int accId);   // ORIG _ZN17DB_...Eij
};

void DB_LoadDnfHackLog::makeRequest(int uid, unsigned int accId)
{
    // ORIG 0x084224ce：Acquire 结果从未落栈槽（P0 右值内联），无 Stream* 具名局部。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5abf), true);
    guard.operator*()->operator<<(0x106);
    guard.operator*()->operator<<(uid);
    // P103：ORIG 先调 operator-> 再 GetInBuffer（Ghidra C 明确 pCVar2=operator->(local_18);
    // local_10=GetInBuffer<T>(pCVar2)）。
    SIG_LOAD_DNF_HACK_LOG* sig =
        guard.operator->()->GetInBuffer<SIG_LOAD_DNF_HACK_LOG>();
    *reinterpret_cast<unsigned int*>(sig) = accId;   // ORIG *(uint*)local_10 = param_2
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
}

// ORIG 0x084c7e0c T dispatch_sig —— 监控服登录应答处理。
// 报告 docs/class_func_reports/Inter_MonitorLoginOK/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorLoginOKPktView —— Inter_MonitorLoginOK::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorLoginOKPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e
    char m_pad12[0x11];  // +0x12..未消费
    unsigned char m_23;             // +0x23
    unsigned int m_24;              // +0x24
    unsigned int m_28;              // +0x28
};
#pragma pack(pop)

// §6.2 巨件核销：P167 139/139 [0缺0多]；gap -36→-24；残差归因 N82（详见
// docs/asm_translation_patterns.md 负例表）。真修 3 处：
// ① getpeername 目标改对齐具名 struct 成员读 sa.ip（消非对齐 punning 引发的 memcpy call；
//    ORIG 单条 mov 4B 整装 @0x084c8738）；
// ② 删多余 memset(&sa,0,sizeof(sa))（ORIG sockaddr 槽不清零）；
// ③ ipBuf 清零改显式 memset(ipBuf,0,0x4c)（复现 ORIG call memset @0x084c8500；
//    `={'\0'}` 在 4.4-O0 内联 rep stos 形不符）。
// [b28d-lane2] verdict=DIFF ndiff=637（ORIG 0x084c7e0c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-23。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=481（ORIG 0x84c7e0c）：P226 具名 bool/char 落槽+N18a 单段 regmove+N7 间接调用寄存器。
int Inter_MonitorLoginOK::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_MonitorLoginOKPktView* pPkt = reinterpret_cast<Inter_MonitorLoginOKPktView*>(data);
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0x206a;

    const unsigned int characNoMsg =                   // msg+0x24
        pPkt->m_24;
    const unsigned int channelNoMsg =                  // msg+0x28
        pPkt->m_28;

    user->SetLastLoginCharacNo(0);                     // 上一角色/频道重置
    user->SetLastLoginChannelNo(0);
    user->SetValidLastLoginCharac(false);

    if (G_GameWorld()->GetChannelType() == 8 &&
        characNoMsg == 0 && channelNoMsg == 0)
    {   // 战场频道异常空载追踪
        cMyTrace(__PRETTY_FUNCTION__, 0x2075, 0)("Inter_MonitorLoginOK() mid(%s), charac_no(%d), channel_no(%d)",
              NumberToString(pPkt->m_e, 0),
              (int)characNoMsg, (int)channelNoMsg);
        return 0x2076;
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x207a, 0)("Inter_MonitorLoginOK() mid(%s), charac_no(%d), channel_no(%d)",
          NumberToString(pPkt->m_e, 0),
          (int)characNoMsg, (int)channelNoMsg);

    PacketGuard packet;
    packet.put_header(1, 1);

    switch (pPkt->m_23)            // errCode
    {
    case '\x02':                                       // 重复登录断开
        packet.put_byte(0);
        packet.put_byte(0x12);
        packet.finalize(true);
        user->Send(packet);
        user->DisConnSig((DISCONN_SIG)0xc, true, 0);
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2088, 0)("info.err_code == SIG_LOGIN_DATA::ERROR_ALREADY");
        }
        return 0;
    case '\x03':                                       // 封停账号断开
        packet.put_byte(0);
        packet.put_byte(0x41);
        packet.finalize(true);
        user->Send(packet);
        user->DisConnSig((DISCONN_SIG)0x15, true, 0);
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2095, 0)("info.err_code == SIG_LOGIN_DATA::ERROR_ALREADY");
        }
        return 0;
    case '\x01':                                       // 正常登录分支
        break;
    default:
        cMyTrace(__PRETTY_FUNCTION__,
            0x2209, 5)("[%s][%d]",
                __PRETTY_FUNCTION__,
                0x2209);
        return 0x220d;
    }

    packet.put_byte(1);
    const int accNameLen = strnlen(user->get_acc_name(), 0x14);
    const bool padActivate = user->getPad()->isActivate();
    packet.put_int(accNameLen);
    packet.put_str(user->get_acc_name(), accNameLen);
    packet.put_byte(padActivate ? 1 : 0);
    packet.put_byte(0);
    if (G_CEnvironment()->get_server_group() == 3)     // garena 区/其他区分支
    {
        packet.put_byte(1);
    }
    else
    {
        Taiwan::GarenaAuthData* garena =
            reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData());
        packet.put_byte(garena->getMobileAuth() ? 1 : 0);
    }
    packet.put_byte(user->GetAge());
    packet.put_byte(user->IsPCRoomUser() ? 1 : 0);
    packet.put_byte(G_GameWorld()->GetChannelType());
    packet.put_byte(G_CDataManager()->GetUpgradeRevisionPvPChannel());
    packet.put_byte(user->isTournamentAccount() ? 1 : 0);
    packet.put_int(user->get_acc_id());
    // P48/P0：ORIG 6 次独立 G_CEnvironment()（strlen/put_str 双臂各一，TCPPort/UDPPort 各一）。
    packet.put_int(strlen(G_CEnvironment()->m_serverEnvir.m_relayIP));
    packet.put_str(G_CEnvironment()->m_serverEnvir.m_relayIP, 0x10);
    packet.put_int(G_CEnvironment()->m_serverEnvir.m_relayTCPPort);
    packet.put_int(G_CEnvironment()->m_serverEnvir.m_relayUDPPort);
    int sex = user->getSex();
    if (sex == -1)
        sex = 1;
    packet.put_byte(sex & 0xff);
    packet.put_byte(user->GetReliablePerson());
    packet.put_byte(user->getSecurityCard()->isActivate() ? 1 : 0);
    packet.put_byte(user->getSecurityCard()->isCertified() ? 1 : 0);
    packet.put_byte(user->getPad()->getVersionInfo());
    packet.put_byte(user->getPad()->isCertified() ? 1 : 0);
    packet.put_byte(user->isRestingUser() ? 1 : 0);

    unsigned char ipBuf[0x4c];
    memset(ipBuf, 0, 0x4c);                        // ORIG call memset 0x4c（rep stos 内联形≠ORIG）
    socklen_t saLen = sizeof(sockaddr);
    struct SockAddrIp { unsigned short family; unsigned char pad[2]; unsigned int ip; unsigned char rest[10]; } sa;
    const int ret = getpeername(user->m_network.get_socket(),
                                reinterpret_cast<sockaddr*>(&sa), &saLen);
    if (ret != 0)
        return 0x2114;                                 // socket 取 IP 失败

    {
        unsigned int clientIp = 0;
        clientIp = sa.ip;                        // ORIG mov -0xbc→-0xc4 单条 4B 整装
        void* checker = GlobalData::s_antibotChecker_;          // antibiotic 类未建模——asm 桥残留（vtable+0x04 AddClient，按 §8 协议保留）
        typedef int (*AddClientFn)(void*, int, int, unsigned int,
                                   unsigned int, void*);
        AddClientFn addClient = *(AddClientFn*)((*(void***)checker)[0x04 / 4]);
        const int antibotRet =
            addClient(checker, G_CEnvironment()->getProcessSequence(), 0,   // P48 独立求值
                      (unsigned int)user->get_acc_id(),
                      user->getClientVersion(), ipBuf);
        (void)clientIp;
        if (antibotRet < 0)
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x211b, 0)("AddClient ProcessSequence : %d, qq_id : %s, ClientVersion : %d, return : %d",
              G_CEnvironment()->getProcessSequence(),   // P48 独立求值
              NumberToString((unsigned int)user->get_acc_id(), 0),
              user->getClientVersion(), antibotRet);
        }
        user->m_field8cf00 = 0;
        user->m_field4 = 0;
        user->m_field5 = 0;
        typedef int (*P2PAddClientFn)(void*, unsigned int, int, unsigned int);  // 同上残留：vtable+0x20 P2PAddClient
        P2PAddClientFn p2pAdd =
            *(P2PAddClientFn*)((*(void***)checker)[0x20 / 4]);
        const int p2pRet =
            p2pAdd(checker, (unsigned int)user->get_acc_id(), -1,
                   user->getClientVersion());
        if (p2pRet == -1)
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x212c, 0)("P2PAddClient ProcessSequence : %d, qq_id : %s, ClientVersion : %d, return : -1",
              G_CEnvironment()->getProcessSequence(),   // P48 独立求值
              NumberToString((unsigned int)user->get_acc_id(), 0),
              user->getClientVersion(), p2pRet);
        }
        user->setAntibotKey(antibotRet);
    }

    packet.put_byte(user->getUserEventCharacterFlag() ? 1 : 0);
    packet.put_byte(user->getOnlinePreliminaryPlayType());
    packet.put_byte(online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam());
    packet.put_byte(GlobalData::s_pGMAccounts_->isGM(
                        (unsigned int)user->get_acc_id()) ? 1 : 0);
    if (!GlobalData::s_timeGate_->isOpen())
        packet.put_short(0);
    else
        packet.put_short(GlobalData::s_timeGate_->getNpcIndex());
    packet.finalize(true);
    user->Send(packet);
    user->m_field796f8 = 0;                    // ORIG 0x84c8854 Send 后清零
    {   // ORIG vtable+0x2c 两参槽（IPGHelper IPGQuery）：类未建模——asm 桥残留保留，按 §8 协议待建模清理
        typedef void (*QueryFn)(void*, CUser*);
        void* helper = GlobalData::s_pIPGHelper;
        QueryFn query = *(QueryFn*)((*(void***)helper)[0x2c / 4]);
        query(helper, user);
    }
    const unsigned short uniqueId = (unsigned short)user->get_unique_id();
    // P48：ORIG 两独立 GetUID 调用点（InsertTimer 与 makeRequest 各一），禁缓存 uid。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                (TIMER_MESSAGE)0, 300, uniqueId, 0);
    _NS_NonClient_Request(user);
    user->SetLastLoginCharacNo(characNoMsg);
    user->SetLastLoginChannelNo(channelNoMsg);
    DB_LoadDnfHackLog::makeRequest((unsigned int)user->GetUID(),
                                   (unsigned int)user->get_acc_id());
    return 0;
}

Inter_MonitorCallUserInfo::~Inter_MonitorCallUserInfo() {}
// ORIG 0x084c89b2 T dispatch_sig —— 监控服索取用户信息上报。
// 报告 docs/class_func_reports/Inter_MonitorCallUserInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallUserInfoPktView —— Inter_MonitorCallUserInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCallUserInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
};
#pragma pack(pop)

// [b28c-32] verdict=NEAR：N7 + movzbl 两段（ORIG %eax 中转 vs 4.4 直装 %ebx）（48=48）。
// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084c89b2）：混合残差；gap0。
int Inter_MonitorCallUserInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    if (G_CGameManager() == 0)
    {
        // ORIG 族 0x084c89ff：null 支 trace 槽 -0x2c（深）、主支 -0x1c（浅）。
        // 技巧3：null 支临时链式形占深槽（同 CutOffCallUserInfo 闭合形）。
        cMyTrace(__PRETTY_FUNCTION__,
                 0x2220, 5)(
            "Inter_MonitorCallUserInfo::dispatch_sig()  G_CGameManager() is null");
    }
    else
    {
        // ORIG 0x084c89ff：pkt 物化在主路径；group 两次独立读（P48 无 CSE），
        // ORIG 0x084c8a2a/0x084c8a58：`movzbl 0xa(%eax),%eax; movzbl %al,%ebx`
        // 两步零扩展（unsigned int 槽 = char→int 宽度转换先落 %eax 再转 %ebx）。
        Inter_MonitorCallUserInfoPktView* pPkt = reinterpret_cast<Inter_MonitorCallUserInfoPktView*>(data);
        // [b28d-lane2] P226/P48：ORIG 0x084c8a2a/0x084c8a58 两处均为
        // `movzbl 0xa(%eax),%eax; movzbl %al,%ebx`（uchar→uint 显式转换两段装载）。
        // 具名 register 局部使 4.4 融合为 `movzbl 0xa(%eax),%ebx`（N18a 变体）；
        // 改「内联实参表达式」后 4.4 复现两段（探针 /tmp/l2probe/p14.cpp f7 实证），
        // 第二处经 cMyTrace 全表达式临时链以 %ebx 跨 ctor 保活（P227）。
        G_CGameManager()->Send_userinfos_to_upper_server(
            (unsigned char)(unsigned int)pPkt->m_a);
        // ORIG 0x084c8a58：重读无 CSE（P48 两次独立装载）。
        cMyTrace(__PRETTY_FUNCTION__,
                 0x2230, 0)(
            "%d Server Call UserInfo Arrived\n",
            (unsigned char)(unsigned int)pPkt->m_a);
    }
    return 0;
}

Inter_CutOffCallUserInfo::~Inter_CutOffCallUserInfo() {}
// ORIG 0x084c8a70 T dispatch_sig —— 截断服索取用户信息。
// 报告 docs/class_func_reports/Inter_CutOffCallUserInfo/dispatch_sig.md 全文转写。
int Inter_CutOffCallUserInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    if (G_CGameManager() == 0)
    {
        // ORIG 0x084c8a84-084c8aa2：null 支 trace 槽=-0x2c（深）、send 支=-0x1c（浅）
        // ⇒ 临时对象注册逆序分配（技巧3；probe cutoff w3：具名局部则正序、反 ORIG）。
        cMyTrace(__PRETTY_FUNCTION__, 0x2249, 5)(
            "Inter_MonitorCallUserInfo::dispatch_sig()  G_CGameManager() is null");
    }
    else
    {
        char* p = data;
        G_CGameManager()->send_userinfos_to_cutoff_server();
        cMyTrace(__PRETTY_FUNCTION__, 0x2252, 0)(
            "Cut Off Server Call UserInfo Arrived\n");
    }
    return 0;
}

Inter_MonitorUserGetout::~Inter_MonitorUserGetout() {}
// ORIG 0x084c8b0c T dispatch_sig —— 监控侧用户强制退出。
// 报告 docs/class_func_reports/Inter_MonitorUserGetout/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorUserGetoutPktView —— Inter_MonitorUserGetout::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorUserGetoutPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=51（ORIG 0x084c8b0c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=44（ORIG 0x84c8b0c）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
// [lane2 2026-09-10] verdict=DIFF→AE：与 Inter_SessionClose 同三形（P0 去 accId/P59 guard 极性/P227 临时链）。
int Inter_MonitorUserGetout::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorUserGetoutPktView* pPkt = reinterpret_cast<Inter_MonitorUserGetoutPktView*>(data);
    // §P0/P48：ORIG 084c8b1d `mov 0xa(%eax),%ebx` 直传（无 accId 槽）、084c8bd8 第二处
    //   独立重读 pPkt->m_a ⇒ 两处各写表达式，不设具名局部。
    CUser* target = G_CGameManager()->GetUserByAccId(pPkt->m_a);
    // P59 guard 极性：ORIG 084c8b38 `jne <if 体>` + 084c8b3a `mov $0,%ebx; jmp <共享尾>`。
    if (target == 0)
        return 0;
    {
        PacketGuard packet;
        packet.put_header(1, 1);
        packet.put_byte(0);
        packet.put_byte(0x41);
        packet.finalize(true);
        target->Send(packet);
        target->DisConnSig((DISCONN_SIG)0xf, true, 0);
        // P227：ORIG 084c8bdb m_a 重读后留 %ebx 跨 ctor（084c8bfc）。
        cMyTrace(__PRETTY_FUNCTION__, 0x226c, 0)(
            "강제 종료요청 ACCID: %d", pPkt->m_a);
        return 0;
    }
}

Inter_WebNoticeIngameAdvertisement::~Inter_WebNoticeIngameAdvertisement() {}
// ORIG 0x084c8c4e T dispatch_sig —— 网页游戏内广告通知入 DB 队列。
// 报告 docs/class_func_reports/Inter_WebNoticeIngameAdvertisement/dispatch_sig.md 全文转写。
int Inter_WebNoticeIngameAdvertisement::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    // ORIG 同形：Acquire 返回值直接进 guard 实参。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x2277), true);
    guard.operator*()->operator<<(0x2f8);
    guard.operator*()->operator<<(-1);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;
}

Inter_ReloadIngameAdvertisement::~Inter_ReloadIngameAdvertisement() {}
// ORIG 0x084c8d10 T dispatch_sig —— 游戏内广告条件重载。
// 报告 docs/class_func_reports/Inter_ReloadIngameAdvertisement/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=5（ORIG 0x084c8d10）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=10（ORIG 0x84c8d10）：N101 imul/lea 折叠+P30/N4 add-vs-lea。
int Inter_ReloadIngameAdvertisement::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG：InitADExposeInfo 前 local_14=param_3（-0x10）；循环体每迭代重调 Instance
    // （不缓存局部，asm +0x4c call）；-1 检查在循环体顶部（break）、计数器 setle 在底部。
    // [b28c-34] ORIG 0x084c8d17 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` = 具名
    // recv 局部（P0 槽往返）；循环内两处经该槽装载（lea/add 形）。
    stInGameADExposeCondition* cond =
        reinterpret_cast<stInGameADExposeCondition*>(data);
    InGameADManagerInstance()->InitADExposeInfo();
    int count;
    for (count = 0; count < 200; ++count)
    {
        // [b28d-PoolInterDisp] int 加法链形（probe_g 实证，10→4）：POINTER_PLUS 形被
        // 4.4 fold 成 base+(prod+const)（恒残 10）；纯 int PLUS 不折 ⇒ 复现
        // imul/lea/add $0x210 三段。字段访问形 cond[count].m_sentinel 实测折成
        // `add MEM+mov 0x210(,)`（gap+2、ndiff 6，劣于本形）。残 4 = imul 目标寄存器
        // 与 base 装载先后对调（LRA 形，N18a 族）。
        if (*reinterpret_cast<int*>(
                (reinterpret_cast<int>(cond) + count * 0x84c) + 0x210) == -1)
            break;                                       // ORIG +0x3c je（顶部门槛）
        InGameADManagerInstance()->InsertADExposeInfo(
            cond[count]);
    }
    cMyTrace(__PRETTY_FUNCTION__,
        0x2290, 0)("In Game Advertisement condition Reload OK, count(%d)", count);
    return 0;
}

Inter_TcpServerConnect::~Inter_TcpServerConnect() {}
// ORIG 0x084c8dca T dispatch_sig —— 公会/监控服代理 TCP 重连。
// 报告 docs/class_func_reports/Inter_TcpServerConnect/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_TcpServerConnectPktView —— Inter_TcpServerConnect::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_TcpServerConnectPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    char m_a;                       // +0x0a
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=58（ORIG 0x084c8dca）：槽序/帧差(N82/P212/P221)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=78（ORIG 0x84c8dca）：N22 cmp 操作数方向+N18a 单段 regmove。
int Inter_TcpServerConnect::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_TcpServerConnectPktView* pPkt = reinterpret_cast<Inter_TcpServerConnectPktView*>(data);
    if (pPkt->m_a == '\xcb')
    {
        // [b28c-30] ORIG 0x084c8dd0 段：index 槽 -0x18 预清、循环变量 i 独立槽
        // -0x10（`movl $0x0,-0x18; call GetStartIndex; mov %eax,-0x10`）；单变量形
        // 把 startIndex 存回 -0x18 且 GetServerProxy 结果落临时槽（+6 insn）。
        int index = 0;
        // [b28d-solo-InterDispatcher] CServerProxyMgr<CGuildServerProxy>::GetStartIndex
        // verdict=AE ndiff=0（ORIG 0x084c8dd0 段）：本波已归零（AE）。
        int i = GlobalData::s_guild_proxy_mgr->GetStartIndex();
        // ORIG 084c8e3a `mov -0x18(%ebp),%edx; cmp %edx,%eax; setg`：条件测 index
        //（GetNextIndex 引用更新者，取址 ⇒ 载入寄存器），非 i。while 形复现
        // ORIG 084c8e44 `jne <body 后向>` + 084c8e46 `jmp <跳过 else 分支链>`。
        while (GlobalData::s_guild_proxy_mgr->GetEndIndex() > index)
        {
            // ORIG 084c8e09/084c8e0e：GetServerProxy 结果直落 (%esp) 调 InitTcp，
            // 无具名 proxy 槽（旧具名形多出 -0x14 临时槽 +2 insn）。
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)i)->InitTcp();
            i = GlobalData::s_guild_proxy_mgr->GetNextIndex(index);
        }
    }
    else if (pPkt->m_a == '\xc9')
    {
        int index = 0;
        // [b28d-solo-InterDispatcher] CServerProxyMgr<CMonitorServerProxy>::GetStartIndex
        // verdict=AE ndiff=0（ORIG 0x084c8dd0 段）：本波已归零（AE）。
        int i = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
        while (GlobalData::s_monitor_proxy_mgr->GetEndIndex() > index)
        {
            GlobalData::s_monitor_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)i)->InitTcp();
            i = GlobalData::s_monitor_proxy_mgr->GetNextIndex(index);
        }
    }
    return 0;
}

Inter_MonitorReplyBuddyList::~Inter_MonitorReplyBuddyList() {}
// ORIG 0x084c9148 T dispatch_sig —— 监控服好友在线列表应答广播。
// 报告 docs/class_func_reports/Inter_MonitorReplyBuddyList/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorReplyBuddyListPktView —— Inter_MonitorReplyBuddyList::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorReplyBuddyListPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    unsigned char m_e;              // +0x0e
};
#pragma pack(pop)

int Inter_MonitorReplyBuddyList::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorReplyBuddyListPktView* pPkt = reinterpret_cast<Inter_MonitorReplyBuddyListPktView*>(data);
    G_GameWorld()->send_buddy_conn_list(
        pPkt->m_a,
        reinterpret_cast<BuddyList*>((char*)pPkt + 0xf),
        (int)(unsigned char)pPkt->m_e);
    return 0;
}

Inter_MonitorCeraUpdate::~Inter_MonitorCeraUpdate() {}
// ORIG 0x084cea92 T dispatch_sig —— IPG 塞拉更新：按 accId 定位用户并触发 IPG 查询。
// 报告 docs/class_func_reports/Inter_MonitorCeraUpdate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCeraUpdatePktView —— Inter_MonitorCeraUpdate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCeraUpdatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
};
#pragma pack(pop)

int Inter_MonitorCeraUpdate::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG vfn→%ecx vs 4.4 恒 %eax（N7 探针
    // 实证）；ndiff 14 封顶。
    Inter_MonitorCeraUpdatePktView* pPkt = reinterpret_cast<Inter_MonitorCeraUpdatePktView*>(data);
    register unsigned int accId = pPkt->m_a;   // ORIG 0x084ceaa2 mov 0xa(%eax),%ebx（callee-saved 跨 call 缓存）
    CUser* target = G_CGameManager()->GetUserByAccId(accId);   // ORIG 0x084ceab6 结果落 -0xc 具名槽
    if (target == 0)                           // ORIG 0x084ceabd cmpl $0; jne body —— null 岛 fall-through 在前（技巧17）
        return 0;                              // ORIG 0x084ceabf mov $0; jmp end
    // ORIG 0x084ceae1 call *%ecx：vptr+0x2c = IIPGHelperInterface slot11 = IPGQuery(CUser*)
    // （TaiwanCash.h 声明序 slot11 注释 +0x34 基表偏移 = vptr+0x2c 同槽）
    GlobalData::s_pIPGHelper->IPGQuery(target);
    return 0;
}

Inter_IPGResult::~Inter_IPGResult() {}
// ORIG 0x084ceaee T dispatch_sig —— IPG 计费结果分发：数据流分支（ProcessIPGUserData）
// 与查询结果分支（SendCashData / 点数查询 / CERA 查询 / 错误状态包）。
// 报告 docs/class_func_reports/Inter_IPGResult/dispatch_sig.md 全文转写。
// msg 布局：+0 isData(char)；数据分支 data+1 req / data+0x2dd res；查询分支 data+1 起。
#pragma pack(push, 1)
// Inter_IPGResultPktView —— Inter_IPGResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_IPGResultPktView
{
    char m_0;                 // +0x00
    char m_pad1[0x1c];  // 未消费
    int m_1d;                 // +0x1d
    int m_21;                 // +0x21
    char m_pad25[0x8d];  // 未消费
    char m_b2;                // +0xb2
    char m_padb3[0x1bb];  // 未消费
    unsigned char m_26e;      // +0x26e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=221（ORIG 0x084ceaee）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=228（ORIG 0x84ceaee）：P226 具名 bool/char 落槽+N18a 单段 regmove+N7 间接调用寄存器。
int Inter_IPGResult::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_IPGResultPktView* pPkt = reinterpret_cast<Inter_IPGResultPktView*>(data);
    if (pPkt->m_0 != 0)
    {   // IPG 数据流结果：转 ItemVendingMachine 处理后归还 Stream
        char* reqData = data + 1;
        char* resData = data + 0x2dd;
        CUser* target =
            G_CGameManager()->getUser(pPkt->m_21, 11);
        Stream* stream = *reinterpret_cast<Stream**>(reqData + 0x2d8);
        if (target != 0)
        {
            GlobalData::s_pItemVendingMachine->ProcessIPGUserData(
                target,
                reinterpret_cast<WongWork::IPG::SIPGData*>(resData),
                reinterpret_cast<WongWork::IPG::SIPGData*>(reqData),
                stream);
        }
        GlobalData::s_stream_pool->Free(stream);
        return 0;
    }

    char* msg = data + 1;
    CUser* user =
        G_CGameManager()->getUser(pPkt->m_21, 10);
    if (user == 0)
        return 0;

    PacketGuard packet;
    if (pPkt->m_1d == 4)
    {   // 余额变更 → 重发现金数据
        user->SendCashData(true);
    }
    else if (pPkt->m_1d == 6)
    {   // 点数查询结果：经 CIPGHelper 虚槽 +0x2c 回填（类未建模——asm 桥残留保留，按 §8 协议待建模清理）
        typedef void (*QueryFn)(void*, CUser*, int);
        void* helper = GlobalData::s_pIPGHelper;
        QueryFn query = *(QueryFn*)((*(void***)helper)[0x2c / 4]);
        query(helper, user, 10);
        // P328⑩/P227：ORIG 0x84ceba4..0x84cebea 先求 msg+0xb3→槽、m_b2→%edi、
        // msg+0x64→%esi、msg+0x3c→%ebx 再 ctor ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x2b65, 0)(
            "[Taiwan, Billing] ChargePointQuery Result:%s, %s, %d, %s",
            msg + 0x3c, msg + 0x64, (int)(char)pPkt->m_b2, msg + 0xb3);
        return 0;
    }
    else if (pPkt->m_1d == 0)
    {   // CERA 查询结果日志
        // ORIG 0x84cec29..0x84cec6f 先求 msg+0x28→%ebx、msg+0x50→%esi、msg+0x26e→槽、
        // m_26e→%edi 再 ctor ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x2b6e, 0)(
            "[Taiwan, Billing] ChargeCeraQuery Result:%s, %s, %s, %d",
            msg + 0x28, msg + 0x50, msg + 0x26e,
            (int)pPkt->m_26e);
        return 0;
    }
    else
    {   // 其余状态 → 0x43 错误状态包
        packet.put_header(1, 0x43);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_short(0x19);
        packet.put_byte(0);
        packet.put_byte(0x19);
        packet.put_int(0);
        packet.put_int(0);
        packet.put_int(0);
        packet.put_int(0);
        packet.put_int(-1);
        packet.finalize(true);
        user->Send(packet);
    }

    LogManager::logFormat(
        1, "InterDispatcher.cpp",
        __PRETTY_FUNCTION__,
        0x2b96, "IPG ERROR (%s)", "IPG Network Error");
    return 0;
}

Inter_GenCeraTicket::~Inter_GenCeraTicket() {}
// ORIG 0x084ceea2 T dispatch_sig —— 生成 Cera 票据并回发。
// 报告 docs/class_func_reports/Inter_GenCeraTicket/dispatch_sig.md 全文转写。
int Inter_GenCeraTicket::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0;

    int mid;
    memcpy(&mid, data, 4);
    char name[32];
    // 窗口指针形态保留（具名视图会使 strncpy 实参经 pPkt 局部多一代指令，
    // 破坏本函数 AE 对齐；ORIG 084ceea2 直接 disp(%reg) 传址）。
    strncpy(name, data + 4, 0x20);

    PacketGuard packet;
    packet.put_header(1, 0x44);
    packet.put_byte(1);
    packet.put_int(strlen(name));
    packet.put_str(name, strlen(name));
    packet.put_int(mid);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_LoadEventItem::~Inter_LoadEventItem() {}
// ORIG 0x084cf0c4 T dispatch_sig —— Web 活动物品入库：RecvInfoFromDB 逐项校验后入 DB 队列。
// 报告 docs/class_func_reports/Inter_LoadEventItem/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadEventItemPktView —— Inter_LoadEventItem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadEventItemPktView
{
    unsigned int m_0;         // +0x00
    int m_4;                  // +0x04
    int m_8;                  // +0x08
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=146（ORIG 0x084cf0c4）：槽序/帧差(N82/P212/P221)+混合残差；gap-18。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=120（ORIG 0x84cf0c4）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_LoadEventItem::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadEventItemPktView* pPkt = reinterpret_cast<Inter_LoadEventItemPktView*>(data);
    CUser* target = G_CGameManager()->GetUserByAccId(
        pPkt->m_0);        // msg+0x0 accId
    if (target == 0)
        return 0;

    int eventItems[6];                                  // local_30 计数 + 5 项
    memset(eventItems, 0, 0x18);
    int count = 0;
    for (int i = 0;
         i < pPkt->m_8; ++i)  // msg+0x8 条数
    {
        bool skip = false;
        if (*reinterpret_cast<int*>(data + i * 0x10 + 0x10) == 999)
        {
            skip = false;
        }
        else if (target->get_state() <= 2 ||
                 target->getCurCharacNo() !=
                     pPkt->m_4)
        {
            skip = true;
        }
        if (!skip)
        {
            if ((unsigned char)WongWork::CWebEvent::RecvInfoFromDB(
                    target,
                    (unsigned int)*reinterpret_cast<int*>(data + i * 0x10 + 0x10),
                    (unsigned int)*reinterpret_cast<int*>(data + i * 0x10 + 0x14),
                    (unsigned int)*reinterpret_cast<int*>(data + i * 0x10 + 0x18),
                    (unsigned int)*reinterpret_cast<int*>(data + i * 0x10 + 0xc)))
            {
                eventItems[count] =
                    *reinterpret_cast<int*>(data + i * 0x10 + 0xc);
                count = count + 1;
            }
        }
    }

    if (count != 0)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire(
            "InterDispatcher.cpp", 0x2c1f);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<(0x4d);
        guard.operator*()->operator<<(-1);
        guard->put_binary(eventItems, 0x18);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
    return 0;
}

Inter_MonitorEventItemUpdate::~Inter_MonitorEventItemUpdate() {}
// ORIG 0x084cefd2 T dispatch_sig —— 网页活动道具更新（CWebEvent::RecvInfoFromWeb）。
// 报告 docs/class_func_reports/Inter_MonitorEventItemUpdate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorEventItemUpdatePktView —— Inter_MonitorEventItemUpdate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorEventItemUpdatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    unsigned int m_e;               // +0x0e
    unsigned int m_12;              // +0x12
    unsigned int m_16;              // +0x16
    unsigned int m_1a;              // +0x1a
    unsigned int m_1e;              // +0x1e
};
#pragma pack(pop)

// [b28d-lane2] verdict=NEAR ndiff=20（ORIG 0x084cefd2）：槽序/帧差(N82/P212/P221)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=NEAR ndiff=42（ORIG 0x84cefd2）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MonitorEventItemUpdate::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorEventItemUpdatePktView* pPkt = reinterpret_cast<Inter_MonitorEventItemUpdatePktView*>(data);
    // ORIG 0x84cefd2：m_1a/m_16 落槽（-0x40/-0x3c），m_e/m_12/NumberToString
    // 经 block-scope register 直落 %edi/%esi/%ebx 跨 cMyTrace ctor 缓存（P36）。
    unsigned int v1a = pPkt->m_1a;
    unsigned int v16 = pPkt->m_16;
    register unsigned int v0e = pPkt->m_e;
    register unsigned int v12 = pPkt->m_12;
    register char* mid = NumberToString(pPkt->m_a, 0);
    cMyTrace(__PRETTY_FUNCTION__,
        0x2bff, 0)("[%s][%d],이벤트 유저 DB ID : %s [EventType: %d][CharacNo: %d][ItemID: %d][Stack: %d]\n",
          __PRETTY_FUNCTION__,
          0x2bff,
          mid,
          v12,
          v0e,
          v16,
          v1a);
    WongWork::CWebEvent::RecvInfoFromWeb(
        pPkt->m_a,
        pPkt->m_e,
        pPkt->m_16,
        pPkt->m_1a,
        pPkt->m_12,
        pPkt->m_1e);
    return 0;
}

Inter_MonitorServerEventStart::~Inter_MonitorServerEventStart() {}
// ORIG 0x084cf3ca T dispatch_sig —— 服务端事件启动：圣诞事件开启、广播与定时器注册。
// 报告 docs/class_func_reports/Inter_MonitorServerEventStart/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorServerEventStartPktView —— Inter_MonitorServerEventStart::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorServerEventStartPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    unsigned short m_e;       // +0x0e
    short m_10;               // +0x10
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=64（ORIG 0x084cf3ca）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=37（ORIG 0x84cf3ca）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MonitorServerEventStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorServerEventStartPktView* pPkt = reinterpret_cast<Inter_MonitorServerEventStartPktView*>(data);

    PacketGuard packet;                                // ORIG 构造后未使用，分支外统一析构

    if (pPkt->m_a == 1)
    {
        SendEventNotiPacket(true,
                            (short)pPkt->m_a,
                            (short)pPkt->m_10,
                            reinterpret_cast<CUser*>(0));
        CServerEvent().AddExpRate((int)pPkt->m_e);
        CServerEvent().TurnOnEvent(pPkt->m_a,
                                   (int)pPkt->m_10);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x53,
                                    (int)pPkt->m_10,
                                    (int)pPkt->m_e,
                                    (unsigned int)pPkt->m_a);
        cMyTrace(__PRETTY_FUNCTION__,
            0x2c48, 0)("X-Mas Event Start");
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x2c4b, 5)("Unknown ServerEvent (%d)", pPkt->m_a);
    }
    return 0;
}

Inter_FatigueResetDBReply::~Inter_FatigueResetDBReply() {}
// ORIG 0x084cf546 T dispatch_sig —— 纯 ret 空函数。
// 报告 docs/class_func_reports/Inter_FatigueResetDBReply/dispatch_sig.md 全文转写（汇编 9 字节，mov $0x0,%eax; ret）。
int Inter_FatigueResetDBReply::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    return 0;
}

Inter_SuicideWhenQueueIsEmpty::~Inter_SuicideWhenQueueIsEmpty() {}
// ORIG 0x084cf550 T dispatch_sig —— 纯 ret 空函数。
// 报告 docs/class_func_reports/Inter_SuicideWhenQueueIsEmpty/dispatch_sig.md 全文转写（汇编 9 字节，mov $0x0,%eax; ret）。
int Inter_SuicideWhenQueueIsEmpty::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    return 0;
}

Inter_MonitorNoticeGuildEnter::~Inter_MonitorNoticeGuildEnter() {}
// ORIG 0x084c9194 T dispatch_sig —— 公会加入通知（0x3a 包 + GuildJoin 历史日志）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildEnter/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildEnterPktView —— Inter_MonitorNoticeGuildEnter::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildEnterPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    unsigned short m_12;      // +0x12
    int m_14;                 // +0x14
    char m_pad18[0x4];  // 未消费
    unsigned int m_1c;        // +0x1c
    char m_20[1];             // +0x20 串/字节区首元素（容量未建模）
    char m_pad21[0x16];  // 未消费
    char m_37[1];             // +0x37 串/字节区首元素（容量未建模）
    char m_pad38[0x1d];  // 未消费
    int m_55;                 // +0x55
    unsigned char m_59;       // +0x59
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=166（ORIG 0x084c9194）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=117（ORIG 0x84c9194）：N22 cmp 操作数方向+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MonitorNoticeGuildEnter::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorNoticeGuildEnterPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildEnterPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2305,
            "Inter_MonitorNoticeGuildEnter::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (pPkt->m_55 == user->getCurCharacNo())
    {
        // [b28d-solo-InterDispatcher] P0：ORIG 0x084c9259..0x084c92b4 中 cVar1→%ebx、
        // uVar2→%edi、uVar4→%esi 均无栈槽（内联右值）；仅 pcVar8 落 -0x3c（具名局部）。
        char* pcVar8 = data + 0x20;
        user->m_historyLog.GuildJoin(
            user->get_charac_guildkey(), pcVar8, pPkt->m_1c, pPkt->m_12,
            pPkt->m_59 != 0 ? '\x01' : '\0');
    }

    PacketGuard packet;
    packet.put_header(0, 0x3a);
    packet.put_int(pPkt->m_14);
    packet.put_int(pPkt->m_1c);
    const int local_24 = (int)strlen(pPkt->m_20);
    packet.put_int(local_24);
    packet.put_str(pPkt->m_20, local_24);
    const int local_20 = (int)strlen(pPkt->m_37);
    packet.put_int(local_20);
    packet.put_str(pPkt->m_37, local_20);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeGuildSecede::~Inter_MonitorNoticeGuildSecede() {}
// ORIG 0x084c957e T dispatch_sig —— 公会退出通知：校验角色号后组 0x3b 包回发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildSecede/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildSecedePktView —— Inter_MonitorNoticeGuildSecede::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildSecedePktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（ORIG 比较处两步 spill＝unsigned 字段→get_charac_no(int) 失配）
    int m_12;                 // +0x12
    char m_pad16[0x4];  // 未消费
    int m_1a;                 // +0x1a
    unsigned short m_1e;      // +0x1e
    char m_20[1];             // +0x20 串/字节区首元素（容量未建模）
    char m_pad21[0x16];  // 未消费
    char m_37[1];             // +0x37 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_MonitorNoticeGuildSecede::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x14(%ebp)
    Inter_MonitorNoticeGuildSecedePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildSecedePktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号不符 → 日志
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2348,
            "Inter_MonitorNoticeGuildSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x3b);
    packet.put_int(pPkt->m_12);
    packet.put_int(pPkt->m_1a);
    packet.put_byte(                                 // msg+0x1e ushort（movzwl 零扩展）
        static_cast<int>(pPkt->m_1e));
    const int secedeNameLen =                        // msg+0x20 退会角色名
        static_cast<int>(strlen(pPkt->m_20));
    packet.put_int(secedeNameLen);
    packet.put_str(pPkt->m_20, secedeNameLen);
    const int guildNameLen =                         // msg+0x37 公会名
        static_cast<int>(strlen(pPkt->m_37));
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_37, guildNameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeGuildMarkChange::~Inter_MonitorNoticeGuildMarkChange() {}
// ORIG 0x084ca1e6 T dispatch_sig —— 公会徽标变更：校验 charac_no 后转发公会 key。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildMarkChange/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildMarkChangePktView —— Inter_MonitorNoticeGuildMarkChange::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildMarkChangePktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    int m_e;                  // +0x0e（P254 枚型实测劣化 ⇒ 回退 int，N22 封顶）
    unsigned int m_12;        // +0x12
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=105（ORIG 0x084ca1e6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=57（ORIG 0x84ca1e6）：N22 cmp 操作数方向+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MonitorNoticeGuildMarkChange::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG 0x084ca1xx：包基址在 state 门之后物化（`jmp` 之后的
    //   `mov 0x10(%ebp),%eax; mov %eax,-0x1c(%ebp)`），声明位置照抄（P0）。
    Inter_MonitorNoticeGuildMarkChangePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildMarkChangePktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_e)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x244a,
            "Inter_MonitorNoticeGuildMarkChange::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    cMyTrace(__PRETTY_FUNCTION__,
        0x244f, 0)("[GUILD MARK CHANGE] Send to client. (channel:%d, character:%u, guildkey:%u)",
          pPkt->m_a,
          pPkt->m_e,
          pPkt->m_12);

    PacketGuard packet;
    packet.put_header(0, 0x3c);
    packet.put_int(pPkt->m_12);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeGuildDismiss::~Inter_MonitorNoticeGuildDismiss() {}
// ORIG 0x084ca00c T dispatch_sig —— 公会解散通知：清公会标记并组 0x3d 包回发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildDismiss/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildDismissPktView —— Inter_MonitorNoticeGuildDismiss::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorNoticeGuildDismissPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e（P254 枚型实测劣化 ⇒ 回退 int，N22 封顶）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=98（ORIG 0x084ca00c）：寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=28（ORIG 0x84ca00c）：N22 cmp 操作数方向。
int Inter_MonitorNoticeGuildDismiss::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG 0x084ca02f：包基址在 get_state 检查之后物化入 -0xc(%ebp)
    Inter_MonitorNoticeGuildDismissPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildDismissPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2429,
            "Inter_MonitorNoticeGuildDismiss::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    cMyTrace(__PRETTY_FUNCTION__,
        0x242d, 0)("Guild Create Right Init! Char id : %d, Master Id : %d",
          user->get_charac_no(-1),
          reinterpret_cast<STGuildDBInfoPacketView*>(user->GetGuildDBInfo())->m_masterNo);

    // 会长本人额外清创建权（GmCmds_C.cpp 同型先例）
    if (reinterpret_cast<STGuildDBInfoPacketView*>(user->GetGuildDBInfo())->m_masterNo ==
        user->get_charac_no(-1))
    {
        user->set_guild_create_right(0);
    }
    user->set_charac_guildkey(0);

    PacketGuard packet;
    packet.put_header(0, 0x3d);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeGuildChatMsg::~Inter_MonitorNoticeGuildChatMsg() {}
// ORIG 0x084c9e30 T dispatch_sig —— 公会聊天消息转发：校验角色号后组 0x41 包回发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildChatMsg/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildChatMsgPktView —— Inter_MonitorNoticeGuildChatMsg::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildChatMsgPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    unsigned char m_30;       // +0x30
    char m_31[1];             // +0x31 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=118（ORIG 0x084c9e30）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=79（ORIG 0x84c9e30）：N22 cmp 操作数方向+N18a 单段 regmove+P264 .o 重定位/立即数。
int Inter_MonitorNoticeGuildChatMsg::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG 该件包基址槽在 state 门之后物化（同 Inter_Monitor_DeleteBlackListResult
    // 配方：`mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)` 落于守卫之后）⇒ 声明下移。
    Inter_MonitorNoticeGuildChatMsgPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildChatMsgPktView*>(data);

    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x23f1,
            "Inter_MonitorNoticeGuildChatMsg::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x41);
    packet.put_byte(6);
    packet.put_byte(0);
    const int msgLen =                               // msg+0x12 聊天文本
        static_cast<int>(strlen(pPkt->m_12));
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_12, msgLen);
    packet.put_byte(0);
    const unsigned int nameLen =                     // msg+0x30 发言者名长
        static_cast<unsigned int>(pPkt->m_30);
    packet.put_int(static_cast<int>(nameLen));
    packet.put_str(pPkt->m_31, static_cast<int>(nameLen));
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorCallGuildMembers::~Inter_MonitorCallGuildMembers() {}
// ORIG 0x084ca7e6 T dispatch_sig —— 公会成员列表下发（0x46 包 + MakePacketGuildMembersInfo）。
// 报告 docs/class_func_reports/Inter_MonitorCallGuildMembers/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallGuildMembersPktView —— Inter_MonitorCallGuildMembers::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorCallGuildMembersPktView
{
    char m_pad0[0xe];  // 未消费
    // §4.3 N（记不回退）：+0x34 ST_Guild_Mem_Info* 数组基址——补 view 具名基址槽会使
    // sizeof(view) 变化，触发本函数内对 view 尺寸敏感的立即数装载（movl $size），破坏 AE，故保留 reinterpret_cast。
    unsigned int m_e;         // +0x0e（unsigned→int 返回失配的 4 行 spill 形态）
    int m_12;                 // +0x12
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
    char m_pad17[0x16];  // 未消费
    unsigned char m_2d;       // +0x2d
    unsigned int m_2e;                 // +0x2e
    unsigned short m_32;      // +0x32
};
#pragma pack(pop)

int Inter_MonitorCallGuildMembers::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_MonitorCallGuildMembersPktView* pPkt = reinterpret_cast<Inter_MonitorCallGuildMembersPktView*>(data);

    // ORIG：unsigned 字段失配 4 行 spill（同 ReplyGuildLetter）。
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x24b9,
            "Inter_MonitorCallGuildMembers::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    if (user->get_charac_guildkey() !=
        pPkt->m_12)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x24bf,
            "Inter_MonitorCallGuildMembers::dispatch_sig() : pUser->get_charac_guildkey() != recv->m_uGuildKey");
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x46);
    packet.put_byte(1);
    packet.put_int(pPkt->m_12);
    // ORIG：strlen 结果 int 槽复用；put_str 的指针实参（`lea 0x16(%eax),%edx`）先求值。
    const int msgLen = (int)strlen(pPkt->m_16);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_16, msgLen);
    packet.put_int(pPkt->m_2e);
    packet.put_short(
        (int)(unsigned short)pPkt->m_32);
    FunctionGuildMember::MakePacketGuildMembersInfo(
        packet,
        (int)pPkt->m_2d,
        reinterpret_cast<ST_Guild_Mem_Info*>(reinterpret_cast<char*>(pPkt) + 0x34));
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorCallGuildMembersNext::~Inter_MonitorCallGuildMembersNext() {}
// ORIG 0x084caa16 T dispatch_sig —— 公会成员信息续查：双校验后组 0x8e 包回发。
// 报告 docs/class_func_reports/Inter_MonitorCallGuildMembersNext/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallGuildMembersNextPktView —— Inter_MonitorCallGuildMembersNext::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCallGuildMembersNextPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    // §4.3 N：+0x17 ST_Guild_Mem_Info* 基址——补 view 基址槽改 sizeof(view) 触发尺寸敏感装载破 AE，保留 reinterpret_cast。
    unsigned int m_e;               // +0x0e（同上 unsigned→int 失配两步 spill 形态）
    int m_12;                       // +0x12
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

int Inter_MonitorCallGuildMembersNext::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0xc(%ebp)
    Inter_MonitorCallGuildMembersNextPktView* pPkt = reinterpret_cast<Inter_MonitorCallGuildMembersNextPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x24e7,
            "Inter_MonitorCallGuildMembersNext::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (user->get_charac_guildkey() !=
        pPkt->m_12)        // msg+0x12 公会 key 校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x24ed,
            "Inter_MonitorCallGuildMembersNext::dispatch_sig() : pUser->get_charac_guildkey() != recv->m_uGuildKey");
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x8e);
    packet.put_byte(1);
    packet.put_int(pPkt->m_12);
    FunctionGuildMember::MakePacketGuildMembersInfo( // ORIG 0x084ca37a T
        packet,
        static_cast<int>(pPkt->m_16),
        reinterpret_cast<ST_Guild_Mem_Info*>(
            reinterpret_cast<char*>(pPkt) + 0x17));   // ORIG 经 -0xc 槽
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorCallGuildAllMembers::~Inter_MonitorCallGuildAllMembers() {}
// ORIG 0x084cabda T dispatch_sig —— 公会全成员列表下发（0x8f 包 + MakePacketGuildMembersInfo）。
// 报告 docs/class_func_reports/Inter_MonitorCallGuildAllMembers/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallGuildAllMembersPktView —— Inter_MonitorCallGuildAllMembers::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorCallGuildAllMembersPktView
{
    char m_pad0[0xe];  // 未消费
    // §4.3 N：+0x34 ST_Guild_Mem_Info* 基址——同上，view 基址槽破坏 AE，保留 reinterpret_cast。
    unsigned int m_e;         // +0x0e（同上）
    int m_12;                 // +0x12
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
    char m_pad17[0x16];  // 未消费
    int m_2d;                 // +0x2d
    unsigned short m_31;      // +0x31
    unsigned char m_33;       // +0x33
};
#pragma pack(pop)

int Inter_MonitorCallGuildAllMembers::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_MonitorCallGuildAllMembersPktView* pPkt = reinterpret_cast<Inter_MonitorCallGuildAllMembersPktView*>(data);

    // ORIG：同 MonitorCallGuildMembers。
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x250c,
            "Inter_MonitorCallGuildAllMembers::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    if (user->get_charac_guildkey() !=
        pPkt->m_12)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2512,
            "Inter_MonitorCallGuildAllMembers::dispatch_sig() : pUser->get_charac_guildkey() != recv->m_uGuildKey");
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x8f);
    packet.put_byte(1);
    packet.put_int(pPkt->m_12);
    // ORIG 0x084cacxx `mov -0x10(%ebp),%eax; lea 0x16(%eax),%edx` 先于 `mov -0xc(%ebp),%eax`
    // ⇒ put_str 实参1（串址）先求值；strlen 结果 int 槽直传（size_t+(int)cast 形反序退化 add）。
    const int msgLen = strlen(pPkt->m_16);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_16, msgLen);
    packet.put_int(pPkt->m_2d);
    packet.put_short(
        (int)(unsigned short)pPkt->m_31);
    FunctionGuildMember::MakePacketGuildMembersInfo(
        packet,
        // ORIG 0x084cadxx：arg3 `mov -0x10(%ebp),%eax; lea 0x34(%eax),%edx`（槽寻址、先于
        // arg2 的 movzbl 0x33）⇒ 基址改用 pPkt 槽而非 data 形参。
        (int)pPkt->m_33,
        reinterpret_cast<ST_Guild_Mem_Info*>(reinterpret_cast<char*>(pPkt) + 0x34));
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorCallGuildAllMembersNext::~Inter_MonitorCallGuildAllMembersNext() {}
// ORIG 0x084cae0a T dispatch_sig —— 公会全体成员信息续查：双校验后组 0x90 包回发。
// 报告 docs/class_func_reports/Inter_MonitorCallGuildAllMembersNext/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallGuildAllMembersNextPktView —— Inter_MonitorCallGuildAllMembersNext::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCallGuildAllMembersNextPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    // §4.3 N：+0x17 ST_Guild_Mem_Info* 基址——同上，view 基址槽破坏 AE，保留 reinterpret_cast。
    unsigned int m_e;               // +0x0e（同上 unsigned→int 失配两步 spill 形态）
    int m_12;                       // +0x12
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

int Inter_MonitorCallGuildAllMembersNext::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化
    Inter_MonitorCallGuildAllMembersNextPktView* pPkt = reinterpret_cast<Inter_MonitorCallGuildAllMembersNextPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2539,
            "Inter_MonitorCallGuildAllMembersNext::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (user->get_charac_guildkey() !=
        pPkt->m_12)        // msg+0x12 公会 key 校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x253f,
            "Inter_MonitorCallGuildAllMembersNext::dispatch_sig() : pUser->get_charac_guildkey() != recv->m_uGuildKey");
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x90);
    packet.put_byte(1);
    packet.put_int(pPkt->m_12);
    FunctionGuildMember::MakePacketGuildMembersInfo( // ORIG 0x084ca37a T
        packet,
        static_cast<int>(pPkt->m_16),
        reinterpret_cast<ST_Guild_Mem_Info*>(
            reinterpret_cast<char*>(pPkt) + 0x17));   // ORIG 经包基址槽
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeGuildNames::~Inter_MonitorNoticeGuildNames() {}
// ORIG 0x084cafce T dispatch_sig —— 公会成员名列表通知：双重校验后组 0x3f 包回发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildNames/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildNamesPktView —— Inter_MonitorNoticeGuildNames::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
#pragma pack(push, 1)
// MonitorNoticeGuildNames 名区元素视图（stride 0x1e；ORIG 0x084cb11f/0x084cb125
// 两步位移 `lea 0x10(%edx)` + `add $0x7`：名绝对基 = union@0x12 + 元素内 +5 =
// 0x17，4.4 按 (0x10 | 余量) 切分常量）。
struct MonitorGuildNameElemView { char m_pad0[5]; char m_name[0x17]; char m_padt[2]; };
#pragma pack(pop)
struct Inter_MonitorNoticeGuildNamesPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（ORIG 0x084cb012 mov 0xe(%eax)）
    union
    {
        struct { int m_12; unsigned char m_16; };  // +0x12 / +0x16
        MonitorGuildNameElemView m_names[1];       // union@0x12（容量未建模）
    };
};
#pragma pack(pop)

int Inter_MonitorNoticeGuildNames::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x14(%ebp)
    Inter_MonitorNoticeGuildNamesPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildNamesPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x255f,
            "Packet_Monitor_Notice_Guild_Char_Names::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (user->get_charac_guildkey() !=
        pPkt->m_12)        // msg+0x12 公会 key 校验（ORIG 0x084cb083 mov 0x12(%edx)）
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2565,
            "Inter_MonitorCallGuildMembers::dispatch_sig() : pUser->get_charac_guildkey() != recv->m_uGuildKey");
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x3f);
    // ORIG：count 缓存槽 -0x10(%ebp) int；循环 i<count 有符号 setl 尾测
    const int count =
        static_cast<int>(static_cast<unsigned int>(pPkt->m_16));
    packet.put_byte(count);
    for (int i = 0; i < count; ++i)
        // ORIG：数组寻址两步位移（每名 0x1e 定长，名起于元素 +7）。
        packet.put_str(pPkt->m_names[i].m_name, 0x1e);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeGuildLoginOut::~Inter_MonitorNoticeGuildLoginOut() {}
// ORIG 0x084c93e4 T dispatch_sig —— 公会成员登录/登出广播：校验 charac_no 后转发登出包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildLoginOut/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildLoginOutPktView —— Inter_MonitorNoticeGuildLoginOut::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildLoginOutPktView
{
    char m_pad0[0xa];  // 未消费
    signed char m_a;          // +0x0a
    char m_padb[0x4];  // 未消费
    unsigned int m_f;         // +0x0f（同上 unsigned→int 失配两步 spill 形态）
    unsigned char m_13;       // +0x13
    char m_14[1];             // +0x14 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_MonitorNoticeGuildLoginOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete → 门
        return 0;
    // ORIG：包基址槽在 get_state 守卫之后才物化（声明位置决定落栈时机）。
    Inter_MonitorNoticeGuildLoginOutPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildLoginOutPktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_f)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x232c,
            "Inter_MonitorNoticeGuildLoginOut::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_f);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x40);
    packet.put_byte((int)pPkt->m_a);   // movzbl+movsbl
    packet.put_byte((unsigned int)pPkt->m_13);
    const int nameLen = strlen(pPkt->m_14);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_14, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeOtherChannelChat::~Inter_MonitorNoticeOtherChannelChat() {}
// ORIG 0x084cb1ac T dispatch_sig —— 其它频道聊天通知：按消息类型（1/2/3/私聊回执）
// 构造 0x41 聊天包转发给目标用户。
// 报告 docs/class_func_reports/Inter_MonitorNoticeOtherChannelChat/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeOtherChannelChatPktView —— Inter_MonitorNoticeOtherChannelChat::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeOtherChannelChatPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    signed char m_e;          // +0x0e
    char m_padf[0x4];  // 未消费
    int m_13;                 // +0x13
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
    char m_pad18[0x1d];  // 未消费
    unsigned char m_35;       // +0x35
    unsigned char m_36;       // +0x36
    char m_37[1];             // +0x37 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=354（ORIG 0x084cb1ac）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=293（ORIG 0x84cb1ac）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_MonitorNoticeOtherChannelChat::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    char* msg = data;                                 // -0x1c

    if (reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_35 == 1)
    {
        PacketGuard packet;                            // -0x34
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_a);
        packet.put_byte(0x15);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17);         // -0x14
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17, len);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else if (reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_35 == 2)
    {
        PacketGuard packet;                            // -0x40
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_a);
        packet.put_byte(0x4d);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17);         // -0x10
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17, len);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else if (reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_35 == 3)
    {
        PacketGuard packet;                            // -0x4c
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_a);
        packet.put_byte(0x5a);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17);         // -0xc
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17, len);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else
    {
        if (user->get_charac_no(-1) !=
            reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_13)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x25ee,
                "Inter_MonitorNoticeBuddyChat::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
                user->get_charac_no(-1),
                reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_13);
            return 0;
        }

        PacketGuard packet;                            // -0x28
        packet.put_header(0, 0x41);
        packet.put_byte(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_a);
        packet.put_byte(0);
        const int len = (int)strlen(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17);         // -0x18
        packet.put_int(len);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_17, len);
        packet.put_byte((int)reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_e);
        packet.put_int((int)(unsigned int)reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_36);
        packet.put_str(reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_37,
                       (int)(unsigned int)reinterpret_cast<Inter_MonitorNoticeOtherChannelChatPktView*>(msg)->m_36);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_MonitorCallGuildInfo::~Inter_MonitorCallGuildInfo() {}
// ORIG 0x084cc006 T dispatch_sig —— 公会信息查询结果下发（0x4d 包，0x1d 步长成员数组）。
// 报告 docs/class_func_reports/Inter_MonitorCallGuildInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorCallGuildInfoPktView —— Inter_MonitorCallGuildInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCallGuildInfoPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（同上）
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=90（ORIG 0x084cc006）：寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=47（ORIG 0x84cc006）：N101 imul/lea 折叠+N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_MonitorCallGuildInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_MonitorCallGuildInfoPktView* pPkt = reinterpret_cast<Inter_MonitorCallGuildInfoPktView*>(data);

    // ORIG：同 MonitorCallGuildMembers。
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x26e4,
            "Inter_MonitorCallGuildInfo::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x4d);
    packet.put_byte(1);
    packet.put_byte(
        (int)pPkt->m_12);
    // ORIG 0x84cc10e/0x84cc12f 等 `lea (%edx,%eax,1)` 的 edx = -0x14 具名槽（pPkt）。
#define pData (reinterpret_cast<char*>(pPkt))
    for (int i = 0;
         i < (int)(unsigned int)pPkt->m_12;
         ++i)
    {
        packet.put_int(*reinterpret_cast<int*>(pData + i * 0x1d + 0x13));
        packet.put_byte(
            (int)*reinterpret_cast<unsigned char*>(pData + i * 0x1d + 0x17));
        packet.put_byte(
            (int)*reinterpret_cast<unsigned char*>(pData + i * 0x1d + 0x18));
        const int nameLen = (int)strlen(pData + i * 0x1d + 0x19);
        packet.put_int(nameLen);
        packet.put_str(pData + i * 0x1d + 0x19, nameLen);
    }
#undef pData
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeGuildInfo::~Inter_MonitorNoticeGuildInfo() {}
// ORIG 0x084cb726 T dispatch_sig —— 监控服公会信息通知：落 SetGuildDBInfo 后组
// 0x46 公会信息包（成员等级过滤、公告/说明串）回发用户。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildInfoPktView —— Inter_MonitorNoticeGuildInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildInfoPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（ORIG 比较点 `mov %eax,%edx` 中转，P211/P265 档③）
    char m_pad12[0x4];  // 未消费
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
    char m_pad17[0x16];  // 未消费
    unsigned int m_2d;        // +0x2d（ORIG 比较点 `mov %eax,%edx` 中转，P211/P265 档③）
    unsigned char m_31;       // +0x31
    unsigned char m_32;       // +0x32
    char m_pad33[0x5];  // 未消费
    unsigned short m_38;      // +0x38
    int m_3a;                 // +0x3a
    unsigned char m_3e;       // +0x3e
    int m_3f;                 // +0x3f
    char m_pad43[0x15];  // 未消费
    unsigned short m_58;      // +0x58
    unsigned char m_5a;       // +0x5a
    char m_pad5b[0x50];  // 未消费
    unsigned char m_ab;       // +0xab
    char m_padac[0x9];  // 未消费
    unsigned char m_b5;       // +0xb5
    int m_b6;                 // +0xb6
    char m_ba[1];             // +0xba 串/字节区首元素（容量未建模）
    char m_padbb[0x19];  // 未消费
    char m_d4[1];             // +0xd4 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=293（ORIG 0x084cb726）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=225（ORIG 0x84cb726）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→3（314 vs 311，TU 121/1331/5/133 不回退）：真修三处——
//   ① 成员过滤条件改正极性（P214①：`grade!=0 && grade!=0 && level>=200`，ORIG `jle` 假路直落）；
//   ② m_e/m_2d int→unsigned int（P211/P265 档③比较点中转，sizeof/偏移/mangled 零影响）；
//   ③ `(m_32>>1)&1` 补 `(unsigned char)` 包裹。残余=N6 位折叠（ORIG `shr %al` vs 4.4 `movzbl;sar`）+N95/N126 槽基平移 4（pPkt -0x20 vs -0x1c）。
int Inter_MonitorNoticeGuildInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    // ORIG 0x084****：state 岛后 pPkt 才物化 -0x1c（技巧13）。
    Inter_MonitorNoticeGuildInfoPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildInfoPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2613,
            "Inter_MonitorNoticeGuildInfo::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    user->SetGuildDBInfo(                              // ORIG E 引用（头为 const&）
        *reinterpret_cast<STGuildDBInfoOnly*>(data + 0x16));

    PacketGuard packet;                                // -0x28
    packet.put_header(0, 0x46);

    const int guildNameLen = (int)strlen(pPkt->m_16);   // -0x18
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_16, guildNameLen);

    if (user->get_charac_no(-1) ==
        pPkt->m_2d)          // 会长标记
        packet.put_byte(1);
    else
        packet.put_byte(0);

    packet.put_byte((int)pPkt->m_31);
    packet.put_byte((int)((((unsigned char)pPkt->m_32) >> 1) & 1));
    packet.put_short((int)pPkt->m_38);
    packet.put_byte((int)(user->get_guild_create_right() & 0xff));
    packet.put_int(pPkt->m_3a);
    packet.put_byte((int)pPkt->m_3e);
    packet.put_int(pPkt->m_3f);
    packet.put_short((int)pPkt->m_58);

    int memberCountPos = packet.get_index();           // -0x2c（put_byte(int&,int) 回填需左值）
    int memberCount = 0;                               // -0x14
    packet.put_byte(0);
    // ORIG 循环体 `mov -0xSLOT(%ebp),%ecx; lea (%ecx,%eax,1),%eax`：基址走 pPkt 具名槽。
#define pData (reinterpret_cast<char*>(pPkt))
    for (int i = 0;                                    // -0x10
         i < (int)(unsigned int)pPkt->m_5a;
         ++i)
    {
        if (user->getGuildMemberGrade() != 0 &&        // ORIG 双调用短路原样（正极性 P214①）
            user->getGuildMemberGrade() != 0 &&
            *reinterpret_cast<int*>(pData + i * 5 + 0x5b) >= 200)
        {
            packet.put_int(*reinterpret_cast<int*>(pData + i * 5 + 0x5b));
            packet.put_byte((int)*reinterpret_cast<signed char*>(pData + i * 5 + 0x5f));
            ++memberCount;
        }
    }
#undef pData
    packet.put_byte(memberCountPos, memberCount);      // ORIG put_byte(int&,int) 回填

    const int noticeLen = (int)strlen(pPkt->m_d4);      // 公告
    packet.put_int(noticeLen);
    packet.put_str(pPkt->m_d4, noticeLen);
    packet.put_byte((int)pPkt->m_ab);
    packet.put_byte((int)pPkt->m_b5);
    packet.put_int(pPkt->m_b6);
    const int introLen = (int)strlen(pPkt->m_ba);       // 说明
    packet.put_int(introLen);
    packet.put_str(pPkt->m_ba, introLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorSetGuildKey::~Inter_MonitorSetGuildKey() {}
// ORIG 0x084cbd26 T dispatch_sig —— 设置公会 key：state==2 登录期直写，state>2 校验角色号后发包。
// 报告 docs/class_func_reports/Inter_MonitorSetGuildKey/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorSetGuildKeyPktView —— Inter_MonitorSetGuildKey::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorSetGuildKeyPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    unsigned int m_12;        // +0x12
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=175（ORIG 0x084cbd26）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=68（ORIG 0x84cbd26）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_MonitorSetGuildKey::dispatch_sig(CUser* user, char* data, int len)
{
    Inter_MonitorSetGuildKeyPktView* pPkt = reinterpret_cast<Inter_MonitorSetGuildKeyPktView*>(data);
    const int local_10 = len;

    if (user->get_state() == 2)                        // test eax,eax; sete
    {
        user->set_charac_guildkey_in_login(            // msg+0x12, msg+0xe
            pPkt->m_12,
            pPkt->m_e);
        return 0;
    }

    if (user->get_state() <= 2)                        // cmp $2; setg → !(state>2)
        return 0x26d4;

    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x26a7,
            "Inter_MonitorSetGuildKey::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (pPkt->m_12 == 0)
    {
        user->resetGuildDBInfo();
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x97);
        packet.finalize(true);
        user->Send(packet);
        user->send_skill_info();

        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(1);
        packet.put_short(1);
        user->make_basic_info(reinterpret_cast<char*>(&packet), '\x01');
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        user->set_charac_guildkey(                     // CUserCharacInfo::set_charac_guildkey
            pPkt->m_12);
    }

    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(2);
        user->make_basic_info(reinterpret_cast<char*>(&packet), '\x02');
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_MonitorNoticeGuildCreate::~Inter_MonitorNoticeGuildCreate() {}
// ORIG 0x084cbb94 T dispatch_sig —— 公会创建通知：写 guildkey 后转发公会名。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildCreate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildCreatePktView —— Inter_MonitorNoticeGuildCreate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildCreatePktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;         // +0x0a
    unsigned int m_e;         // +0x0e（同上 unsigned→int 失配两步 spill 形态）
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_MonitorNoticeGuildCreate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0x267c;                                 // 门失败返回 0x267c
    // ORIG：包基址槽在守卫之后物化。
    Inter_MonitorNoticeGuildCreatePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildCreatePktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_e)
    {   // m_uCharID 不匹配 → 日志后返回 0
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2682,
            "Packet_Monitor_Notice_Guild_Create_ToUser::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    user->set_charac_guildkey(pPkt->m_a);

    PacketGuard packet;
    packet.put_header(0, 0x47);
    packet.put_int(pPkt->m_a);
    const int nameLen = strlen(pPkt->m_12);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_12, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorGuildMemberNamingReply::~Inter_MonitorGuildMemberNamingReply() {}
// ORIG 0x084cc23c T dispatch_sig —— 公会成员命名应答。
// 报告 docs/class_func_reports/Inter_MonitorGuildMemberNamingReply/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorGuildMemberNamingReplyPktView —— Inter_MonitorGuildMemberNamingReply::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorGuildMemberNamingReplyPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

// [b28d-lane2] verdict=NEAR ndiff=1（ORIG 0x084cc23c）：寄存器中转(N18a/N7)；gap0。
// [b28d-solo-InterDispatcher] verdict=NEAR ndiff=2（ORIG 0x84cc23c）：N22 有参 callee 封顶（同上）。
int Inter_MonitorGuildMemberNamingReply::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    // ORIG 0x084cc262：pPkt 物化在 null 岛之后（技巧13：声明位置=落栈时机）。
    Inter_MonitorGuildMemberNamingReplyPktView* pPkt = reinterpret_cast<Inter_MonitorGuildMemberNamingReplyPktView*>(data);
    // [b28d-lane2] N22 判词推翻（探针 /tmp/l2p/p2.cpp fC）：ORIG 0x084cc26b 字段值直落
    // %ebx 跨 call 存活 + `cmp %eax,%ebx` 只在字段读**内联进比较**（无具名局部）时复现；
    // 具名（register/非 register）局部必产 `cmp %ebx,%eax`（P0：无槽往返 ⇒ 原码无该局部）。
    if (pPkt->m_12 != user->get_charac_no(-1))
        return 0;

    // ORIG 0x084cc2ba/0x084cc2ec：m_a 两处独立 reload，无具名槽（技巧37）。
    PacketGuard packet;
    packet.put_header(1, 0x80);
    if (pPkt->m_a == 0)
        packet.put_byte(1);
    else
        packet.put_byte(0);
    packet.put_byte(pPkt->m_a);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorSetSubGuildMasterReply::~Inter_MonitorSetSubGuildMasterReply() {}
// ORIG 0x084cc35c T dispatch_sig —— 副会长设置应答（0x81 包 + GuildChangeGrade 日志）。
// 报告 docs/class_func_reports/Inter_MonitorSetSubGuildMasterReply/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorSetSubGuildMasterReplyPktView —— Inter_MonitorSetSubGuildMasterReply::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorSetSubGuildMasterReplyPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    int m_12;                 // +0x12
    unsigned char m_16;       // +0x16
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
    char m_pad18[0x1d];  // 未消费
    unsigned char m_35;       // +0x35
    unsigned int m_36;        // +0x36
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=102（ORIG 0x084cc35c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=95（ORIG 0x84cc35c）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_MonitorSetSubGuildMasterReply::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    Inter_MonitorSetSubGuildMasterReplyPktView* pPkt = reinterpret_cast<Inter_MonitorSetSubGuildMasterReplyPktView*>(data);
    if (pPkt->m_12 !=        // msg+0x12 characNo
        user->get_charac_no(-1))
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x81);
    if (pPkt->m_a == 0)
    {
        packet.put_byte(1);
        packet.put_byte(pPkt->m_a);
        const int msgLen = (int)strlen(pPkt->m_17);
        packet.put_int(msgLen);
        packet.put_str(pPkt->m_17, msgLen);
        packet.put_byte(
            (int)pPkt->m_16);

        const unsigned char newGrade =                 // msg+0x16
            pPkt->m_16;
        const unsigned char oldGrade =                 // msg+0x35
            pPkt->m_35;
        const unsigned int grade =                     // msg+0x36
            pPkt->m_36;
        const unsigned int characNo =                  // msg+0x12
            pPkt->m_12;
        cUserHistoryLog* hist = &user->m_historyLog;
        hist->GuildChangeGrade(
            (unsigned int)user->get_charac_guildkey(),
            static_cast<const char*>(user->GetGuildDBInfo()),
            characNo, grade, oldGrade, newGrade);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_a);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeGuildMemberInfo::~Inter_MonitorNoticeGuildMemberInfo() {}
// ORIG 0x084cc768 T dispatch_sig —— 公会成员信息通知：设成员等级并发 0x7f/0x02 包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildMemberInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildMemberInfoPktView —— Inter_MonitorNoticeGuildMemberInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorNoticeGuildMemberInfoPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    char m_pad12[0x15];  // +0x12..未消费
    unsigned char m_27;             // +0x27
};
#pragma pack(pop)

int Inter_MonitorNoticeGuildMemberInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    // ORIG 0x084cc78f：包基址在 get_state 检查之后才物化入栈槽 -0xc(%ebp)
    Inter_MonitorNoticeGuildMemberInfoPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildMemberInfoPktView*>(data);
    if (pPkt->m_e !=       // msg+0x0e 角色号校验
        user->get_charac_no(-1))
        return 0;

    user->setGuildMemberGrade(                       // msg+0x27 成员等级
        pPkt->m_27);

    PacketGuard packet;
    packet.put_header(0, 0x7f);
    packet.put_byte(                                 // movzbl 零扩展
        static_cast<int>(pPkt->m_27));
    packet.finalize(true);
    user->Send(packet);

    user->send_skill_info();

    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(1);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\x01');
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeChangeGuildMemberGrade::~Inter_MonitorNoticeChangeGuildMemberGrade() {}
// ORIG 0x084cc55e T dispatch_sig —— 监控服公会成员等级变更通知。
// 报告 docs/class_func_reports/Inter_MonitorNoticeChangeGuildMemberGrade/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeChangeGuildMemberGradePktView —— Inter_MonitorNoticeChangeGuildMemberGrade::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorNoticeChangeGuildMemberGradePktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

int Inter_MonitorNoticeChangeGuildMemberGrade::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG +0x4c：包基址槽 -0xc 的写入点在 get_state 守卫之后（声明随之下移）。
    Inter_MonitorNoticeChangeGuildMemberGradePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeChangeGuildMemberGradePktView*>(data);
    if (pPkt->m_e != user->get_charac_no(-1))
        return 0;
    PacketGuard packet;
    packet.put_header(0, 0x8c);
    packet.put_byte(pPkt->m_12);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_GuildNoticeGuildMessage::~Inter_GuildNoticeGuildMessage() {}
// ORIG 0x084cc650 T dispatch_sig —— 公会公告消息转发。
// 报告 docs/class_func_reports/Inter_GuildNoticeGuildMessage/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildNoticeGuildMessagePktView —— Inter_GuildNoticeGuildMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildNoticeGuildMessagePktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=NEAR ndiff=1（ORIG 0x084cc650）：寄存器中转(N18a/N7)；gap0。
// [b28d-solo-InterDispatcher] verdict=NEAR ndiff=2（ORIG 0x84cc650）：N22 有参 callee 封顶（同上）。
int Inter_GuildNoticeGuildMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_GuildNoticeGuildMessagePktView* pPkt = reinterpret_cast<Inter_GuildNoticeGuildMessagePktView*>(data);
    // [b28d-lane2] N22 判词推翻（探针 /tmp/l2p/p2.cpp fC）：ORIG +0x2f 一步
    // `mov 0xe(%eax),%ebx; cmp %eax,%ebx` 只在字段读**内联进比较**（无具名局部）时复现；
    // 用 register/普通具名局部必产 `cmp %ebx,%eax`（P0：无槽往返 ⇒ 原码无该局部）。
    if (pPkt->m_e != user->get_charac_no(-1))         // recv charId @ msg+0xe
        return 0;

    PacketGuard packet;
    packet.put_header(0, 0x8d);
    // ORIG：strlen 只调一次，结果落 -0xc 供 put_int/put_str 复用。
    const int msgLen = strlen(pPkt->m_12);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_12, msgLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeMemberLoginOut::~Inter_MonitorNoticeMemberLoginOut() {}
// ORIG 0x084cc90a T dispatch_sig —— 公会员登录/登出通知（0x48 包 + 上线成员连接字段设置）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberLoginOut/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberLoginOutPktView —— Inter_MonitorNoticeMemberLoginOut::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberLoginOutPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_a;        // +0x0a（ORIG 0x084cc8xx put_byte 处 movzbl %al,%eax 零扩展 ⇒ unsigned）
    char m_padb[0x4];  // 未消费
    unsigned int m_f;         // +0x0f（同上 unsigned→int 失配两步 spill 形态）
    unsigned char m_13;       // +0x13
    unsigned char m_14;       // +0x14（同上 movzbl %al）
    char m_15[1];             // +0x15 串/字节区首元素（容量未建模）
    char m_pad16[0x1d];  // 未消费
    unsigned char m_33;       // +0x33
    unsigned int m_34;        // +0x34
};
#pragma pack(pop)

int Inter_MonitorNoticeMemberLoginOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x27b5,
            "MEMBER : Inter_MonitorNoticeMemberLoginOut::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    // ORIG：包基址在两道 guard 之后物化入 -0x10(%ebp)
    Inter_MonitorNoticeMemberLoginOutPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberLoginOutPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_f)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x27bc,
            "MEMBER : Inter_MonitorNoticeMemberLoginOut::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_f);
        return 0;
    }

    if (pPkt->m_a == '\x01' &&
        pPkt->m_14 == '\x01')
    {
        user->set_connect_upper_member(
            pPkt->m_34);
        user->set_uppermember_exp_level(
            pPkt->m_33);
    }
    else if (pPkt->m_a == '\0' &&
             pPkt->m_14 == '\x01')
    {
        user->set_connect_upper_member(0);
        user->set_uppermember_exp_level('\0');
    }

    PacketGuard packet;
    packet.put_header(0, 0x48);
    packet.put_byte(
        (unsigned int)pPkt->m_a);
    packet.put_byte(
        (unsigned int)pPkt->m_13);
    packet.put_byte(
        (unsigned int)pPkt->m_14);
    // len 槽 int 精确匹配 put_str(char*,int)（size_t 致 decay 参数降级 add+反序）
    const int local_10 = (int)strlen(pPkt->m_15);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_15, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeRequestMemberEnterToRequester::~Inter_MonitorNoticeRequestMemberEnterToRequester() {}
// ORIG 0x084ccba0 T dispatch_sig —— 会员入会申请通知（发给申请者）：组 0x4f 包回发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeRequestMemberEnterToRequester/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeRequestMemberEnterToRequesterPktView —— Inter_MonitorNoticeRequestMemberEnterToRequester::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeRequestMemberEnterToRequesterPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（unsigned 字段→get_charac_no(int) 失配两步 spill 形态）
    unsigned char m_12;       // +0x12（ORIG 0x084cc9xx movzbl %al,%eax 零扩展 ⇒ unsigned）
    char m_13[1];             // +0x13 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_MonitorNoticeRequestMemberEnterToRequester::dispatch_sig(
    CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化
    Inter_MonitorNoticeRequestMemberEnterToRequesterPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeRequestMemberEnterToRequesterPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x27e8,
            "MEMBER : Inter_MonitorNoticeRequestMemberEnterToRequester::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x4f);
    if (pPkt->m_12 == 0)   // msg+0x12 结果码
    {
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(                             // movzbl 零扩展
            static_cast<int>(pPkt->m_12));
    }
    const int guildNameLen =                         // msg+0x13 公会名
        static_cast<int>(strlen(pPkt->m_13));
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_13, guildNameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeRequestMemberEnterToResponser::~Inter_MonitorNoticeRequestMemberEnterToResponser() {}
// ORIG 0x084ccd54 T dispatch_sig —— 公会员进入请求应答：单人模式/游戏选项→monitor 0x4b9 包，否则 0x49 拒绝包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeRequestMemberEnterToResponser/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeRequestMemberEnterToResponserPktView —— Inter_MonitorNoticeRequestMemberEnterToResponser::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeRequestMemberEnterToResponserPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    short m_12;               // +0x12
    char m_14[1];             // +0x14 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=159（ORIG 0x084ccd54）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=77（ORIG 0x84ccd54）：N22 cmp 操作数方向+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MonitorNoticeRequestMemberEnterToResponser::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorNoticeRequestMemberEnterToResponserPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeRequestMemberEnterToResponserPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x280c,
            "MEMBER : Inter_MonitorNoticeRequestMemberEnterToResponser::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (user->alonePlay_Mode())
    {
        Packet_Monitor_Member_Enter_Reply reply;      // PacketHeader(0x4b9, 0x13)
        reply.m_idByChannel = user->GetUID();     // +0xa（权威头名）
        reply.m_charNo = user->getCurCharacNo();  // +0xe
        reply.m_code = 3;                         // +0x12
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                user->GetServerGroup());
        proxy->SendTcpPacket(reinterpret_cast<char*>(&reply), 0x13);
        return 0;
    }

    if (user->GetGameOptionRef()->get_etc_option((ENUM_OPTION_ETC)0x1d) == 1)
    {
        Packet_Monitor_Member_Enter_Reply reply;      // PacketHeader(0x4b9, 0x13)
        reply.m_idByChannel = user->GetUID();
        reply.m_charNo = user->getCurCharacNo();
        reply.m_code = 4;
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                user->GetServerGroup());
        proxy->SendTcpPacket(reinterpret_cast<char*>(&reply), 0x13);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x49);
    packet.put_short((int)pPkt->m_12);
    const int local_10 = (int)strlen(pPkt->m_14);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_14, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeMemberEnterOk::~Inter_MonitorNoticeMemberEnterOk() {}
// ORIG 0x084cd218 T dispatch_sig —— 会员进入应答分发：solo 拒绝 / 上线会员设置 / 0x4a 状态包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberEnterOk/dispatch_sig.md 全文转写。
// msg 布局：+0xa result(byte) +0xb sub(byte) +0xc flag(byte) +0x11 charID(int)
//           +0x15 u8 +0x16 字符串 +0x34 u8 +0x35 uint。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberEnterOkPktView —— Inter_MonitorNoticeMemberEnterOk::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberEnterOkPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_result;   // +0x0a
    unsigned char m_sub;      // +0x0b
    unsigned char m_flag;     // +0x0c
    char m_padd[0x4];  // 未消费
    unsigned int m_11;        // +0x11（同上 unsigned→int 失配两步 spill 形态）
    unsigned char m_15;       // +0x15
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
    char m_pad17[0x1d];  // 未消费
    unsigned char m_34;       // +0x34
    unsigned int m_uint;      // +0x35
};
#pragma pack(pop)

int Inter_MonitorNoticeMemberEnterOk::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x18(%ebp)
    Inter_MonitorNoticeMemberEnterOkPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberEnterOkPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_11)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x288b,
            "MEMBER : Inter_MonitorNoticeMemberEnterOk::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_11);
        return 0;
    }

    if (pPkt->m_result == 2)
    {   // 进入确认 → 0x4a 状态包
        PacketGuard packet;
        packet.put_header(0, 0x4a);
        packet.put_byte((int)pPkt->m_result);
        packet.put_byte((int)pPkt->m_sub);
        packet.put_byte((int)pPkt->m_15);
        const int nameLen = strlen(pPkt->m_16);
        packet.put_int(nameLen);
        packet.put_str(pPkt->m_16, nameLen);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else if (pPkt->m_result == 3)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            reinterpret_cast<char*>(pPkt) + 0x16,      // ORIG 经 -0x18 槽
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(1),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(6));
        return 0;
    }
    else if (pPkt->m_result == 4)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            reinterpret_cast<char*>(pPkt) + 0x16,      // ORIG 经 -0x18 槽
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(0),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(6));
        return 0;
    }

    if (pPkt->m_sub == 1 &&
        pPkt->m_flag == 1)
    {   // 上线会员标记：设置连接会员/经验等级并刷新队伍信息
        user->set_connect_upper_member(pPkt->m_uint);
        user->set_uppermember_exp_level(
            pPkt->m_34);
        CParty* party = (CParty*)user->GetParty();   // CUser.h 冻结：GetParty 返回 void*
        if (party != 0)
            party->send_party_realtime_info();
    }
    user->set_charac_memberkey(pPkt->m_11);

    PacketGuard packet;
    packet.put_header(0, 0x4a);
    packet.put_byte((int)pPkt->m_result);
    packet.put_byte((int)pPkt->m_sub);
    packet.put_byte((int)pPkt->m_15);
    const int nameLen = strlen(pPkt->m_16);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_16, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeMemberEnterReply::~Inter_MonitorNoticeMemberEnterReply() {}
// ORIG 0x084cd5ca T dispatch_sig —— 公会员进入应答（0x50 包 + 上线成员字段设置）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberEnterReply/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberEnterReplyPktView —— Inter_MonitorNoticeMemberEnterReply::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberEnterReplyPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_a;        // +0x0a
    unsigned char m_b;        // +0x0b（ORIG `movzbl %al` 无符号扩展）
    unsigned char m_c;        // +0x0c
    char m_padd[0x4];  // 未消费
    unsigned int m_11;        // +0x11（ORIG set_charac_memberkey 实参单段 `mov 0x11(%eax),%edx`）
    unsigned char m_15;       // +0x15
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
    char m_pad17[0x1d];  // 未消费
    unsigned char m_34;       // +0x34
    unsigned int m_35;        // +0x35
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=156（ORIG 0x084cd5ca）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=83（ORIG 0x84cd5ca）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N18a 单段 regmove。
// [b28d-SweepInterDispatcher2] verdict=AE（ORIG 0x084cd5ca，176=176，TU 121/1331/5/133）：真修三处——
//   ① 删 `const unsigned char type` 具名局部，m_a 守卫拆两段独立（P133/P45：`if(m_a==2)return 0; if(m_a==3||m_a==4)return 0;`）；
//   ② m_b/m_c char→unsigned char 消 `movsbl`（ORIG `movzbl %al`）；③ m_11 int→unsigned int：比较点出 `mov %eax,%edx` 中转（P211/P265）
//   且 set_charac_memberkey 实参单段 `mov 0x11(%eax),%edx`（P265 unsigned 恒单段）。
int Inter_MonitorNoticeMemberEnterReply::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorNoticeMemberEnterReplyPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberEnterReplyPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_11)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x28d9,
            "MEMBER : Inter_MonitorNoticeMemberEnterReply::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_11);
        return 0;
    }

    if (pPkt->m_a == '\x02')
        return 0;
    if (pPkt->m_a == '\x03' || pPkt->m_a == '\x04')
        return 0;

    if (pPkt->m_b == '\x01' ||
        pPkt->m_b == '\x02')
    {
        if (pPkt->m_b == '\x01' &&
            pPkt->m_c == '\x01')
        {
            user->set_uppermember_exp_level(
                pPkt->m_34);
            user->set_connect_upper_member(
                pPkt->m_35);
            CParty* local_10 = (CParty*)user->GetParty();
            if (local_10 != 0)
                local_10->send_party_realtime_info();
        }
        user->set_charac_memberkey(
            pPkt->m_11);
    }

    PacketGuard packet;
    packet.put_header(1, 0x50);
    packet.put_byte(1);
    packet.put_byte(
        (unsigned int)pPkt->m_b);
    packet.put_byte(
        (unsigned int)pPkt->m_15);
    const int local_14 = (int)strlen(pPkt->m_16);
    packet.put_int(local_14);
    packet.put_str(pPkt->m_16, local_14);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeMemberSecede::~Inter_MonitorNoticeMemberSecede() {}
// ORIG 0x084cd83a T dispatch_sig —— 会员退订通知（重置上位会员状态 + 0x51 包）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberSecede/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberSecedePktView —— Inter_MonitorNoticeMemberSecede::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberSecedePktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    unsigned char m_12;       // +0x12
    char m_13[1];             // +0x13 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=136（ORIG 0x084cd83a）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：`const int msgLen=(int)strlen(...)`
// + 无 cast 实参 ⇒ 4.4 对 `put_str(&pPkt->m_13[0], msgLen)` 发 ORIG 的 lea 形，探针 q7 实证）。
int Inter_MonitorNoticeMemberSecede::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_MonitorNoticeMemberSecedePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberSecedePktView*>(data);

    // ORIG：unsigned 字段失配 4 行 spill（同 ReplyGuildLetter）。
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2914,
            "MEMBER : Inter_MonitorNoticeMemberSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCallerCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    // ORIG：m_12 各处独立 movzbl reload（无具名槽，槽数决定 pPkt=-0x14/PG=-0x20）。
    if (pPkt->m_12 == 1)
    {
        user->set_connect_upper_member(0);
        user->set_uppermember_exp_level(0);
        CParty* party = (CParty*)user->GetParty();
        if (party != 0)
            party->send_party_realtime_info();
    }

    PacketGuard packet;
    packet.put_header(1, 0x51);
    if (pPkt->m_12 == 1 || pPkt->m_12 == 2)
        packet.put_byte(1);
    else
        packet.put_byte(0);
    const int msgLen = (int)strlen(pPkt->m_13);
    packet.put_int(msgLen);
    packet.put_str(&pPkt->m_13[0], msgLen);   // 探针 q7 实证：int 局部 + 无 cast 实参 ⇒ 4.4 发 lea
    if (pPkt->m_12 == 1 || pPkt->m_12 == 2)
        packet.put_byte(1);
    else
        packet.put_byte((int)pPkt->m_12);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeMemberSecedeToSeceder::~Inter_MonitorNoticeMemberSecedeToSeceder() {}
// ORIG 0x084cda74 T dispatch_sig —— 会员退会通知（发给退会者）：清标记并组 0x4d 包。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberSecedeToSeceder/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberSecedeToSecederPktView —— Inter_MonitorNoticeMemberSecedeToSeceder::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberSecedeToSecederPktView
{
    char m_pad0[0xe];  // 未消费
    // ORIG +0x5c 两步 spill（`mov %eax,%edx`+`mov -0x14(%ebp),%eax`）＝本字段 unsigned 与
    // get_charac_no(int) 返回类型 signedness 失配形态（探针 t1/g1 实证）。
    unsigned int m_e;         // +0x0e
    unsigned char m_12;       // +0x12
    char m_13[1];             // +0x13 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_MonitorNoticeMemberSecedeToSeceder::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化
    Inter_MonitorNoticeMemberSecedeToSecederPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberSecedeToSecederPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 退会角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2947,
            "MEMBER : Inter_MonitorNoticeMemberSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uSecededCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (pPkt->m_12 == 1)   // msg+0x12 标志
    {
        user->set_connect_upper_member(0);
        user->set_uppermember_exp_level(0);
        CParty* party = (CParty*)user->GetParty();
        if (party != 0)
            party->send_party_realtime_info();
    }

    PacketGuard packet;
    packet.put_header(0, 0x4d);
    const int guildNameLen =                         // msg+0x13 公会名
        static_cast<int>(strlen(pPkt->m_13));
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_13, guildNameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeDeleteMemberId::~Inter_MonitorNoticeDeleteMemberId() {}
// ORIG 0x084cdc2c T dispatch_sig —— 删除公会成员通知：清成员字段并刷新队伍信息。
// 报告 docs/class_func_reports/Inter_MonitorNoticeDeleteMemberId/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeDeleteMemberIdPktView —— Inter_MonitorNoticeDeleteMemberId::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorNoticeDeleteMemberIdPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（同上 unsigned→int 失配两步 spill 形态）
};
#pragma pack(pop)

int Inter_MonitorNoticeDeleteMemberId::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG：包基址唯一槽 -0x10(%ebp)（get_state 检查后物化）
    Inter_MonitorNoticeDeleteMemberIdPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeDeleteMemberIdPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x296d,
            "MEMBER : Inter_MonitorNoticeDeleteMemberId::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    user->set_connect_upper_member(0);
    user->set_uppermember_exp_level(0);
    user->set_charac_memberkey(0);

    CParty* party = (CParty*)user->GetParty();
    if (party != 0)
        party->send_party_realtime_info();

    PacketGuard packet;
    packet.put_header(0, 0x4b);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeMemberChatMsg::~Inter_MonitorNoticeMemberChatMsg() {}
// ORIG 0x084ccfda T dispatch_sig —— 会员聊天消息下发（memberkey 门 + 0x41 包）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberChatMsg/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberChatMsgPktView —— Inter_MonitorNoticeMemberChatMsg::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberChatMsgPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（ORIG 比较点 `mov %eax,%edx` 中转，P211/P265 档③）
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    unsigned char m_30;       // +0x30
    char m_31[1];             // +0x31 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=139（ORIG 0x084ccfda）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=83（ORIG 0x84ccfda）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 3→3（147 vs 144，TU 121/1331/5/133 不回退）：真修三处——
//   ① pPkt 声明移到 memberkey 门之后（复现 ORIG 0x084cd061 `mov 0x10(%ebp),%eax; mov %eax,-0x10`）；
//   ② 删 `register int characNo` 具名局部，比较/日志两处直读 `pPkt->m_e`（P0）；
//   ③ m_e int→unsigned int（P211/P265 档③，比较点中转）。残余=槽基平移 4 + N7 寄存器角色。
int Inter_MonitorNoticeMemberChatMsg::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2852,
            "MEMBER : Inter_MonitorNoticeMemberChatMsg::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    Inter_MonitorNoticeMemberChatMsgPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberChatMsgPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2859,
            "MEMBER : Inter_MonitorNoticeMemberChatMsg::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x41);
    packet.put_byte(8);
    packet.put_byte(0);
    const int msgLen = (int)strlen(pPkt->m_12);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_12, msgLen);
    packet.put_byte(0);
    const unsigned int nameLen =
        (unsigned int)pPkt->m_30;
    packet.put_int((int)nameLen);
    packet.put_str(pPkt->m_31, (int)nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorCallMemberList::~Inter_MonitorCallMemberList() {}
// ORIG 0x084cddac T dispatch_sig —— 公会成员列表下发：头条目 + 0x2a 字节 stride 成员循环。
// 报告 docs/class_func_reports/Inter_MonitorCallMemberList/dispatch_sig.md 全文转写。
// msg 布局：+0xe charID(int)；+0x12 起条目区。
#pragma pack(push, 1)
// Inter_MonitorCallMemberListPktView —— Inter_MonitorCallMemberList::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCallMemberListPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_charID;                   // +0x0e 语义名←函数头注释
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_MonitorCallMemberListMemberWinView —— 同环成员记录（entry+0x2b 起，stride 0x2a；
//   字段宽度按 ORIG 084d0f30 段 disp 指令核实）。
struct Inter_MonitorCallMemberListMemberWinView
{
    unsigned char m_flags0;   // +0x00
    unsigned char m_flags1;   // +0x01
    char m_name[1];           // +0x02 名字串区首元素
    char m_pad3[0x1d];
    unsigned char m_grade20;  // +0x20
    unsigned char m_grade21;  // +0x21
    int m_22;                 // +0x22
    int m_26;                 // +0x26
};
static_assert(sizeof(Inter_MonitorCallMemberListMemberWinView) == 0x2a,
              "member record stride");
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_MonitorCallMemberListEntryWinView —— Inter_MonitorCallMemberList::dispatch_sig 窗口 entry 元素视图
//   （entry 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_MonitorCallMemberListEntryWinView
{
    unsigned char m_flags0;   // +0x00（ORIG entry[0] 直读）
    unsigned char m_flags1;   // +0x01（ORIG entry[1] 直读）
    char m_name[1];           // +0x02 成员名字符串区首元素（strlen/put_str 实参）
    char m_pad3[0x1d];  // 未消费
    unsigned char m_grade20;  // +0x20（ORIG entry[0x20]）
    unsigned char m_grade21;  // +0x21（ORIG entry[0x21]）
    int m_22;                 // +0x22
    int m_26;                 // +0x26
    unsigned char m_memberCnt;// +0x2a 同环成员数
    Inter_MonitorCallMemberListMemberWinView
        m_members[1];         // +0x2b 成员记录区（动态长度；[1]=首元素）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=233（ORIG 0x084cddac）：混合残差；gap-12。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=106（ORIG 0x84cddac）：N22 cmp 操作数方向+N101 imul/lea 折叠+N18a 单段 regmove。
int Inter_MonitorCallMemberList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG 0x084e****：state 岛后 pPkt 才物化 -0x20（技巧13）。
    Inter_MonitorCallMemberListPktView* pPkt = reinterpret_cast<Inter_MonitorCallMemberListPktView*>(data);
    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x298c,
            "MEMBER : Inter_MonitorCallMemberList::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    if (user->get_charac_no(-1) != pPkt->m_charID)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2993,
            "MEMBER : Inter_MonitorCallMemberList::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_charID);
        return 0;
    }

    unsigned char* entry = &pPkt->m_12;   // §4.3 字段化：视图 m_12(uchar) 取址，ORIG lea 0x12(%eax)
    Inter_MonitorCallMemberListEntryWinView* pWin_entry = reinterpret_cast<Inter_MonitorCallMemberListEntryWinView*>(entry);
    PacketGuard packet;
    packet.put_header(1, 0x52);
    packet.put_byte(1);
    packet.put_byte((int)pWin_entry->m_flags0);
    packet.put_byte((int)pWin_entry->m_flags1);
    const int nameLen = strlen(pWin_entry->m_name);
    packet.put_int(nameLen);
    packet.put_str(pWin_entry->m_name, nameLen);
    packet.put_byte((int)(char)pWin_entry->m_grade20);
    packet.put_byte((int)pWin_entry->m_grade21);
    packet.put_int(pWin_entry->m_22);
    packet.put_int(pWin_entry->m_26);

    packet.put_byte((int)pWin_entry->m_memberCnt);
    for (int i = 0; i < (int)pWin_entry->m_memberCnt; ++i)
    {
        packet.put_byte((int)pWin_entry->m_members[i].m_flags0);
        packet.put_byte((int)pWin_entry->m_members[i].m_flags1);
        const int memberNameLen = strlen(pWin_entry->m_members[i].m_name);
        packet.put_int(memberNameLen);
        packet.put_str(pWin_entry->m_members[i].m_name, memberNameLen);
        packet.put_byte((int)(char)pWin_entry->m_members[i].m_grade20);
        packet.put_byte((int)pWin_entry->m_members[i].m_grade21);
        packet.put_int(pWin_entry->m_members[i].m_22);
        packet.put_int(pWin_entry->m_members[i].m_26);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorPayTaxToUpper::~Inter_MonitorPayTaxToUpper() {}
// ORIG 0x084ce164 T dispatch_sig —— 会员税额金币入账 + 会员奖励疲劳发放（上限钳制）。
// 报告 docs/class_func_reports/Inter_MonitorPayTaxToUpper/dispatch_sig.md 全文转写。
// msg 布局：+0xe charID(int) +0x12 税额金币(int) +0x16 奖励疲劳(uint) +0x1a 字符串。
#pragma pack(push, 1)
// Inter_MonitorPayTaxToUpperPktView —— Inter_MonitorPayTaxToUpper::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorPayTaxToUpperPktView
{
    char m_pad0[0xe];  // 未消费
    int m_charID;             // +0x0e
    int m_12;                 // +0x12
    unsigned int m_16;        // +0x16
    char m_1a[1];             // +0x1a 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=226（ORIG 0x084ce164）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=26（ORIG 0x84ce164）：N22 cmp 操作数方向。
int Inter_MonitorPayTaxToUpper::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG 0x084d****：state 岛后、memberkey call 前 pPkt 才物化 -0x28（技巧13）。
    Inter_MonitorPayTaxToUpperPktView* pPkt = reinterpret_cast<Inter_MonitorPayTaxToUpperPktView*>(data);
    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x29e7,
            "MEMBER : Inter_MonitorPayTaxToUpper::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    if (user->get_charac_no(-1) != pPkt->m_charID)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x29ed,
            "MEMBER : Inter_MonitorCallMemberList::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_charID);
        return 0;
    }

    if (pPkt->m_12 != 0)     // 税额金币直接入账
    {
        user->getCurCharacInvenW()->gain_money(
            pPkt->m_12, (eMoneyAddReason)0xa, true, 0);
    }

    unsigned int fatigue = 0;
    bool capped = false;                               // 是否触达疲劳上限（回执 0x128 包）
    if (pPkt->m_16 != 0)
    {
        short memberBonus = user->getCurCharacMemberBonusFatigue();
        short accountBonus = user->getAccountMemberBonusFatigue();
        int maxCur = G_CDataManager()->m_serverParameter.m_488c;   // 当前角色奖励疲劳上限
        int maxAcc = G_CDataManager()->m_serverParameter.m_4890;   // 账号奖励疲劳上限
        int a = maxCur - memberBonus;
        int b = maxAcc - accountBonus;
        int v = std::min<int>(a, b);
        int n = std::max<int>(v, 0);
        if (n == 0)
        {
            fatigue = 0;
        }
        else if (pPkt->m_16 < (unsigned int)n)
        {
            fatigue = pPkt->m_16;
        }
        else
        {
            capped = true;
            fatigue = (unsigned int)n;
        }

        if ((int)fatigue > 0)
        {
            user->gainCurCharacFatigue((int)fatigue);
            user->incCurCharacMemberBonusFatigue((short)fatigue);
            user->EnableSaveMemberBonusFatigue();
            user->incAccountMemberBonusFatigue((short)fatigue);
        }
    }

    if (pPkt->m_12 != 0 || fatigue != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x4c);
        packet.put_int(pPkt->m_12);
        packet.put_short((int)fatigue);
        const int strLen = strlen(pPkt->m_1a);
        packet.put_int(strLen);
        packet.put_str(pPkt->m_1a, strLen);
        packet.finalize(true);
        user->Send(packet);

        if (capped)
        {   // 奖励疲劳被上限截断 → 单独 0x128 回执
            PacketGuard capPacket;
            capPacket.put_header(0, 0x128);
            capPacket.finalize(true);
            user->Send(capPacket);
        }
    }
    return 0;
}

Inter_LoadPrivateStore::~Inter_LoadPrivateStore() {}
// ORIG 0x084cf55a T dispatch_sig —— 个人商店加载应答转发给 PrivateStoreMgr。
// 报告 docs/class_func_reports/Inter_LoadPrivateStore/dispatch_sig.md 全文转写。
int Inter_LoadPrivateStore::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;
    // ORIG +0x27：包指针先落具名局部（-0xc），call 之后再 `mov -0xc(%ebp),%edx` 填槽
    // （直接把 reinterpret_cast 作实参会让 gcc 提前放进 %ebx 并省掉栈槽）。
    private_store::SIG_PRIVATE_STORE* pPkt =
        reinterpret_cast<private_store::SIG_PRIVATE_STORE*>(data);
    private_store::GetInstancePrivateStoreMgr()->LoadPrivateStoreResponse(user, pPkt);
    return 0;
}

Inter_UpdateLogout::~Inter_UpdateLogout() {}
// ORIG 0x084ce548 T dispatch_sig —— 用户登出完成通知监控服。
// 报告 docs/class_func_reports/Inter_UpdateLogout/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_UpdateLogoutPktView —— Inter_UpdateLogout::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_UpdateLogoutPktView
{
    int m_0;                  // +0x00
    char m_pad4[0xa];  // 未消费
    // [b28d-lane2] 档②字段类型旋钮（P211/P254）：ORIG 0x084ce581 段
    // `mov 0xe(%eax),%eax; mov %eax,%edx` 两段中转，探针 /tmp/l2probe/p6.cpp
    // 实证 int 字段 + cast 到 enum 实参才复现（unsigned 字段 4.4 恒折单段）。
    int m_e;                  // +0x0e
};
#pragma pack(pop)

// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084ce548）：混合残差；gap0。
int Inter_UpdateLogout::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_UpdateLogoutPktView* pPkt = reinterpret_cast<Inter_UpdateLogoutPktView*>(data);
    if (pPkt->m_0 == 0)
        return 0;
    Packet_Monitor_UDP_Logout_Complete packet;
    packet.m_dbid = pPkt->m_0;
    packet.m_channel = (unsigned char)G_CEnvironment()->get_channel_no();
    // ORIG 0x084ce588：&packet 先物化 %ebx（lea -0x1b 跨 GetServerProxy 存活）；
    // proxy 结果免槽直链（技巧37，+0x68 mov %eax,(%esp)）
    // b28c-28：ORIG 该处为 `mov 0xe(%eax),%eax; mov %eax,%edx`（mem→scratch→
    // 目标两段），4.4 恒一步直落 %edx（N18a 反向：单段直落不可造回两段，
    // 加 (int) 中转实测无效，ndiff 仍 3）⇒ 保持裸形，记 N18a。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)pPkt->m_e)->SendTcpPacket(
            reinterpret_cast<char*>(&packet), 0xf);
    return 0;
}

Inter_UserRepel::~Inter_UserRepel() {}
int Inter_UserRepel::dispatch_sig(CUser* user, char* /*data*/, int /*len*/)
{
    user->DisConnSig(static_cast<DISCONN_SIG>(0x14), true, 0);
    return 0;
}

Inter_AvatarInsert::~Inter_AvatarInsert() {}
// ORIG 0x084cf5a4 T dispatch_sig —— 跨服插入时装：槽位寻位+赋 key+回写槽。
// 报告 docs/class_func_reports/Inter_AvatarInsert/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AvatarInsertPktView —— Inter_AvatarInsert::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_AvatarInsertPktView
{
    char m_pad0[0x8];  // +0x0..未消费
    unsigned char m_8;              // +0x08
    char m_pad9[0x42];  // +0x9..未消费
    unsigned int m_4b;              // +0x4b
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=79（ORIG 0x084cf5a4）：槽序/帧差(N82/P212/P221)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=98（ORIG 0x84cf5a4）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_AvatarInsert::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AvatarInsertPktView* pPkt = reinterpret_cast<Inter_AvatarInsertPktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    const int slot = (int)pPkt->m_8 - 10;
    const unsigned int addInfoKey = pPkt->m_4b;

    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item item = invenR->GetInvenSlot(2, slot);

    CInventory* invenW = user->getCurCharacInvenW();
    WongWork::CAvatarItemMgr* mgr = invenW->GetAvatarItemMgrW();
    mgr->AssignAvatarItemKey(item.m_addInfo2, (int)addInfoKey);
    item.m_addInfo2 = (int)addInfoKey;

    // [b23S3C] P48：ORIG update_item 前重新 call getCurCharacInvenW（共 2 次），不复用 invenW。
    user->getCurCharacInvenW()->update_item((INVEN_TYPE)2, slot, item);
    return 0;
}

Inter_AvatarChangeStat::~Inter_AvatarChangeStat() {}
// ORIG 0x084cf6ec T dispatch_sig —— 跨服时装属性变更登记。
// 报告 docs/class_func_reports/Inter_AvatarChangeStat/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AvatarChangeStatPktView —— Inter_AvatarChangeStat::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AvatarChangeStatPktView
{
    ENUM_VIEW_FIELD m_0;      // +0x00（P254 枚型：ORIG 0x084cf7xx 三处保留 eax→edx 中转）
    char m_pad4[0x4];  // 未消费
    unsigned char m_8;        // +0x08
    char m_pad9[0x1];  // 未消费
    ENUM_VIEW_FIELD m_a;      // +0x0a
    ENUM_VIEW_FIELD m_e;      // +0x0e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=75（ORIG 0x084cf6ec）：槽序/帧差(N82/P212/P221)+混合残差；gap+8。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=72（ORIG 0x84cf6ec）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 8→4（84 vs 80，TU 121/1330/5/134 不回退）：真修——
//   删 key/itemIdx/charId/mgrR/mgrW 具名局部改链式内联（P89/P183/P329），pPkt 声明移到 get_state 检查后（P0 声明位置）。
//   残余=N18a×4（ORIG `mov mem,%eax; mov %eax,%ebx/%edi/%esi` 两段升格，4.4 恒折单段 `mov mem,%reg`）；语句表达式/字段类型旋钮实测无效。
int Inter_AvatarChangeStat::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;

    Inter_AvatarChangeStatPktView* pPkt = reinterpret_cast<Inter_AvatarChangeStatPktView*>(data);
    if (user->getCurCharacInvenW()->GetAvatarItemMgrR()->GetExpireDate(pPkt->m_0) == -1)
    {
        user->getCurCharacInvenW()->GetAvatarItemMgrW()->RegistItem(
            pPkt->m_0,
            pPkt->m_e,
            pPkt->m_a,
            reinterpret_cast<char*>(pPkt) + 0x12,
            *reinterpret_cast<const stAvatarEmblemInfo_t*>(reinterpret_cast<char*>(pPkt) + 0x2f),
            *reinterpret_cast<const stAvatarExpansionInfo_t*>(reinterpret_cast<char*>(pPkt) + 0x4d),
            false);
    }
    user->SendUpdateItemList(
        (CUser::eSendTarget)1, (ENUM_ITEMSPACE)1,
        (int)pPkt->m_8 - 10);
    return 0;
}

Inter_TriggerEventStart::~Inter_TriggerEventStart() {}
// ORIG 0x084ce5f4 T dispatch_sig —— 事件开始触发。
// 报告 docs/class_func_reports/Inter_TriggerEventStart/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_TriggerEventStartPktView —— Inter_TriggerEventStart::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_TriggerEventStartPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    // ORIG +0x15 两步装载（Ghidra `*(undefined4 *)(param_3 + 10)` → TriggerEventStart(int,...)）
    // ＝unsigned 字段→int 形参的 signedness 失配形态（探针 t1 实证）。
    unsigned int m_a;               // +0x0a
    Word_Param m_wp;                // +0x0e（ORIG `mov 0xe(%ecx),%ecx` 单指令折叠，P1）
};
#pragma pack(pop)

// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084ce5f4）：混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本波已归零；AE 归一化后逐条相等）
int Inter_TriggerEventStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_TriggerEventStartPktView* pPkt = reinterpret_cast<Inter_TriggerEventStartPktView*>(data);
    if (GlobalData::s_event_manager != 0)
    {
        if (GlobalData::s_event_manager->TriggerEventStart(
                pPkt->m_a,
                pPkt->m_wp))
            return 0;
        return 0x2a8b;
    }
    return 0;
}

Inter_TriggerEventEnd::~Inter_TriggerEventEnd() {}
// ORIG 0x084ce646 T dispatch_sig —— 事件结束触发。
// 报告 docs/class_func_reports/Inter_TriggerEventEnd/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_TriggerEventEndPktView —— Inter_TriggerEventEnd::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_TriggerEventEndPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;         // +0x0a
};
#pragma pack(pop)

// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084ce646）：混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本波已归零；AE 归一化后逐条相等）
int Inter_TriggerEventEnd::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_TriggerEventEndPktView* pPkt = reinterpret_cast<Inter_TriggerEventEndPktView*>(data);
    // ORIG 0x084ce656：字段读入 %ebx 跨 cMyTrace ctor 缓存（block-scope register）。
    register unsigned int evCode = pPkt->m_a;
    cMyTrace(__PRETTY_FUNCTION__, 0x2a96, 0)("Inter_TriggerEventEnd::dispatch_sig\tevent code(%d), parameter(%d)",
          evCode);
    if (GlobalData::s_event_manager != 0)
    {
        if (GlobalData::s_event_manager->TriggerEventEnd(
                (int)pPkt->m_a))
            return 0;
        return 0x2a9a;
    }
    return 0;
}

Inter_MailBox_Req_List::~Inter_MailBox_Req_List() {}
// ORIG 0x084cf7fa T dispatch_sig —— 邮箱列表装载：多重校验后设置邮箱信息。
// 报告 docs/class_func_reports/Inter_MailBox_Req_List/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=46（ORIG 0x084cf7fa）：寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=37（ORIG 0x84cf7fa）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MailBox_Req_List::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 1 || user->getCurCharacR() == 0)
        return 0;

    SIG_MAILBOX_LIST* recv =
        reinterpret_cast<SIG_MAILBOX_LIST*>(data);     // -0x20(%ebp) 缓存消息指针

    // ORIG 0x084cf845 `mov 0x3332(%eax),%ebx`：具名字段单段 disp（§4.3），值跨
    // getCurCharacNo 调用保活于 %ebx 且无栈槽 ⇒ 裸 register 局部（P248 四条件全满足）。
    register int characNo = recv->m_curCharacNo;
    if (characNo != user->getCurCharacNo())
        return 0x2ca0;                                 // 角色号不匹配

    WongWork::CMailBox* mailBox = (WongWork::CMailBox*)user->GetMailBox();  // CUser.h 冻结：GetMailBox 返回 void*
    if (mailBox == 0)
        return 0x2ca4;                                 // 无邮箱

    // ORIG 0x084cf8??：计数两处独立读（4.1 -0x3c 槽物化被 4.4 折并，实测具名反而
    // 少 2 行失真，取直读形）。
    if (recv->m_mailLoadCount != mailBox->getMailLoadCount())
    {   // 重复装载追踪
        cMyTrace(__PRETTY_FUNCTION__,
            0x2ca9, 5)("Postal loaded twice. %s(%d)(%d->%d)",
              NumberToString(user->get_acc_id(), 0),
              user->getCurCharacNo(),
              mailBox->getMailLoadCount(),
              recv->m_mailLoadCount);
        return 0x2caa;
    }

    WongWork::CMailBoxHelper::SetMailBoxInfo(user, recv);
    return 0;
}

Inter_MailBox_Req_CheckReceiver::~Inter_MailBox_Req_CheckReceiver() {}
// ORIG 0x084cf95a T dispatch_sig —— 邮箱查验收件人请求：按检查类型（普通邮件 /
// cera 直购 / 多附件邮件）完成校验、购买或入 DB 线程，并回 ACK 错误包。
// 报告 docs/class_func_reports/Inter_MailBox_Req_CheckReceiver/dispatch_sig.md 全文转写。
// SendErrorPacket/SendDBThread_forchina 为本类既有方法（NEEDS_DECL 见 meta）。
// [b28d-lane2] verdict=DIFF ndiff=373（ORIG 0x084cf95a）：槽序/帧差(N82/P212/P221)+混合残差；gap-9。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=303（ORIG 0x84cf95a）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_MailBox_Req_CheckReceiver::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // cmp $0x2,setle
        return 0;

    SIG_MAILBOX_CHECK_RECEIVER* sig =                  // -0x30
        reinterpret_cast<SIG_MAILBOX_CHECK_RECEIVER*>(data);

    // ORIG 0x084cf95a：cmd/type 判定读 sig+0x31（m_payload 首 int，邮件类型
    // 1=cera 直购/2=多附件），不是 m_resultFlag(+0x3fb9，DB 结果 0/0x4d)。
    const int mailType = *reinterpret_cast<const int*>(sig->m_payload);
    int cmd = 0x61;                                    // -0x2c
    if (mailType == 1)
        cmd = 0x43;
    else if (mailType == 2)
        cmd = 0x13c;

    if (sig->m_receiverCharacNo == 0)      // 收件角色号
    {
        SendErrorPacket(user, (ENUM_CMDPACKET)cmd, 3);
        return 0;
    }
    if (sig->m_receiverDeleteFlag == 0)      // 附件标志：无附件走 GM 线
    {
        if (!GlobalData::s_pGMAccounts_->isGM(
                sig->m_receiverAccId))
        {
            if (sig->m_resultFlag == 0x4d)   // §4.3 字段化：SIG_MAILBOX_CHECK_RECEIVER::m_resultFlag @+0x3fb9
            {
                SendErrorPacket(user, (ENUM_CMDPACKET)cmd, 0x4d);
                return 0;
            }

            unsigned int err = 0;                      // -0x28
            const int type = *reinterpret_cast<const int*>(sig->m_payload);
            if (type == 1)
            {   // cera 直购：sig+0x35 载荷按 MSG_BUY_CERASHOP_ITEM 视图
                *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x67) =
                    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x1d);
                *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x63) =
                    sig->m_receiverAccId;
                if (!GlobalData::s_pItemVendingMachine->BuyItem(
                        user, *reinterpret_cast<const MSG_BUY_CERASHOP_ITEM*>(
                                  reinterpret_cast<const char*>(sig) + 0x35)))
                    return 0x2d12;
                return 0;
            }
            if (type == 0)
            {
                const unsigned int itemIdx =           // -0x24（单附件）
                    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x6e);
                if (itemIdx != 0)
                {
                    CItem* ci = G_CDataManager()->find_item((int)itemIdx);  // -0x20
                    if (ci != 0)
                    {
                        if (GlobalData::s_rentalSystem->CheckRentableItem(1, itemIdx))
                            return 0x11;
                        if (GlobalData::s_rentalSystem->CheckRentableItem(2, itemIdx))
                            return 0x11;
                        if (ci->GetAttachType() == 5 &&
                            (int)user->get_acc_id() !=
                                *reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x29))
                        {
                            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x61, 0xd8);
                            return 0;
                        }
                    }
                }
                if (*reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x63) != 0)
                {
                    int limit = 0x7fffffff;            // -0x1c
                    limit = G_CDataManager()->GetMoneyLimitPerLevel(
                        *reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x21),
                        reinterpret_cast<const char*>(sig) + 0x46);
                    if ((unsigned int)limit <
                        *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x63))
                    {
                        SendErrorPacket(user, (ENUM_CMDPACKET)0x61, 0xe);
                        return 0;
                    }
                }
                err = (unsigned int)WongWork::CMailBoxHelper::ReqDBSendNewMail(
                    user, reinterpret_cast<MSG_MAILBOX_SEND*>(
                              reinterpret_cast<char*>(sig) + 0x35),
                    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x1d), false,
                    sig->m_receiverAccId, 0);
                if (err == 0)
                {
                    PacketGuard packet;                // -0x3c
                    packet.put_header(1, cmd);
                    packet.put_byte(1);
                    packet.finalize(true);
                    user->Send(packet);
                }
                else
                {
                    SendErrorPacket(user, (ENUM_CMDPACKET)cmd,
                                    (unsigned short)(err & 0xffff));
                }
            }
            else if (type == 2)
            {   // 多附件邮件
                for (int i = 0;                                        // -0x18
                     i < (int)(unsigned int)*reinterpret_cast<unsigned char*>(
                             reinterpret_cast<char*>(sig) + 0x68);
                     ++i)
                {
                    const unsigned int itemIdx =       // -0x14
                        *reinterpret_cast<unsigned int*>(
                            reinterpret_cast<char*>(sig) + 0x6c + i * 0xb);
                    if (itemIdx != 0)
                    {
                        CItem* ci = G_CDataManager()->find_item((int)itemIdx);  // -0x10
                        if (ci != 0)
                        {
                            if (GlobalData::s_rentalSystem->CheckRentableItem(1, itemIdx))
                                return 0x11;
                            if (GlobalData::s_rentalSystem->CheckRentableItem(2, itemIdx))
                                return 0x11;
                            if (ci->GetAttachType() == 5 &&
                                (int)user->get_acc_id() !=
                                    *reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x29))
                            {
                                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0xd8);
                                return 0;
                            }
                        }
                    }
                }
                if (*reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x64) != 0)
                {
                    int limit = 0x7fffffff;            // -0xc
                    limit = G_CDataManager()->GetMoneyLimitPerLevel(
                        *reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + 0x21),
                        reinterpret_cast<const char*>(sig) + 0x46);
                    if ((unsigned int)limit <
                        *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig) + 0x64))
                    {
                        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c, 0xe);
                        return 0;
                    }
                }
                // b28c-l23 SIGFIX 消费点：bool callee ⇒ 裸形（P168a①）
                if (!SendDBThread_forchina(user, sig))
                    return 0;
            }
        }
        else
        {
            SendErrorPacket(user, (ENUM_CMDPACKET)cmd, 0x5a);
        }
    }
    else
    {
        SendErrorPacket(user, (ENUM_CMDPACKET)cmd, 0x18);
    }
    return 0;
}
// ORIG 0x084d008a T _ZN31Inter_MailBox_Req_CheckReceiver15SendErrorPacketEP5CUser14ENUM_CMDPACKETt
// 报告 docs/class_func_reports/Inter_MailBox_Req_CheckReceiver/SendErrorPacket.md：
// 记日志后按命令 0x43 分流 2 字节/1 字节错误包。
void Inter_MailBox_Req_CheckReceiver::SendErrorPacket(
    CUser* user, ENUM_CMDPACKET cmd, unsigned short err)
{
    // ORIG 0x084d008a：err 先 2B spill（+0x09 mov %ax,-0xc）供两支 reload，再
    // movzwl -0xc → %ebx 跨 logFormat 缓存（+0x0f，push ebx/sub 0x34）；
    // get_charac_no 结果免槽直用（技巧37）。
    register unsigned int regCode = err;
    LogManager::logFormat(
        1, "InterDispatcher.cpp",
        __PRETTY_FUNCTION__,
        0x2e12,
        "[ITEM_GIFT] Fail Command[%d] charac_no[%u] error_code[%d]",
        cmd,
        user->get_charac_no(-1),
        regCode);
    if (cmd == 0x43)
        user->SendCmdErrorPacket2Byte(cmd, err);
    else
        user->SendCmdErrorPacket(cmd, static_cast<unsigned char>(err));
}

// ORIG 0x084cff18 T _ZN31Inter_MailBox_Req_CheckReceiver21SendDBThread_forchinaEP5CUserP26SIG_MAILBOX_CHECK_RECEIVER
// 报告 SendDBThread_forchina.md：CStreamGuard(BigStreamPool) →写 0x1f5→写 UID→
// GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER> memcpy 0x3fde→+0x3fc1 角色名 0x1d→
// +0x2d 角色号→MsgQueueMgr::put(2)→返回 true。
// b28c-l23 SIGFIX：int→bool（ORIG __PRETTY_FUNCTION__ 串，P174；体尾
// `mov $0x1,%ebx` 直返，bool 免归一）。
// [b28d-lane2] verdict=DIFF ndiff=28（ORIG 0x084cff18）：寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=5（ORIG 0x84cff18）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
bool Inter_MailBox_Req_CheckReceiver::SendDBThread_forchina(
    CUser* user, SIG_MAILBOX_CHECK_RECEIVER* sig)
{
    // [b28c-34] ORIG 0x084cff18 段：Acquire 结果无槽往返（P0 右值直传实参）⇒ 内联；
    // cMyTrace 用全表达式临时链复现 ORIG 槽方向（guard -0x24 深 / trace -0x1c 浅，
    // P221/P227；同 TU DB_Multi_MailBox_Req_Send::_sendResult 同形实证）。
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire(
        "InterDispatcher.cpp", 0x2dfa), true);
    guard.operator*()->operator<<(0x1f5);
    cMyTrace(__PRETTY_FUNCTION__, 0x2dfd, 0)(
        "E_MULTI_MAILBOX_REQ_SEND : %d", 0x1f5);
    guard.operator*()->operator<<(
        static_cast<int>(user->GetUID()));
    SIG_MAILBOX_CHECK_RECEIVER* dst = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    memcpy(dst, sig, 0x3fde);
    strncpy(dst->m_senderName, user->getCurCharacName(), 0x1d);
    // [b28c-34] N76 封顶（P211 同型性两端冲突）：ORIG 0x084cffc9 store 段
    // `call; mov -0xc(%ebp),%edx; mov %eax,0x2d(%edx)` 要求字段与 int RHS 同型；
    // 但同字段 load 段（C28L20 DB_Multi_MailBox_Req_Send::dispatch 0x0843bb89
    // `mov 0x2d(%eax),%eax; mov %eax,%edx`）要求与 int 形参异型 ⇒ 共享头字段
    // 维持 unsigned int（多数消费段+零回归），本 store 点余 1 条 `mov %eax,%edx`
    // 中转。已试：共享头 int 化（C28L20 回归 13→16）、源侧 int& 视图（4.4 把
    // 地址提前缓存到 %ebx，+3 条）、*(int*)& 形同。
    dst->m_senderCharacNo = user->getCurCharacNo();   // +0x2d
    GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    return true;
}

Inter_MailBox_Req_Send::~Inter_MailBox_Req_Send() {}
// ORIG 0x084d0130 T dispatch_sig —— 邮箱新邮件通知转发监控服。
// 报告 docs/class_func_reports/Inter_MailBox_Req_Send/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MailBox_Req_SendPktView —— Inter_MailBox_Req_Send::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MailBox_Req_SendPktView
{
    int m_0;                        // +0x00
    unsigned int m_4;               // +0x04
    // [b28d-lane2] 档②字段类型旋钮（P211/P254）：ORIG 0x084d014f 段为
    // `mov 0x8(%eax),%eax; mov %eax,%edx`（两段中转），探针 /tmp/l2probe/p6.cpp
    // 实证 4.4.7 仅「int 字段 + cast 到 enum 实参」产两段（unsigned 字段恒折单段）。
    int m_8;                        // +0x08
};
#pragma pack(pop)

// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084d0130）：混合残差；gap0。
int Inter_MailBox_Req_Send::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [b28c-30] N18a 封顶：ORIG 0x084d0161 段 `mov 0x8(%eax),%eax; mov %eax,%edx;
    // mov global,%eax; …`（字段装载 + edx 中转）；4.4 恒折单段
    // `mov 0x8(%eax),%edx`（unsigned 视图字段与 ENUM_SERVER_GROUP 形参视同型）。
    // 少 1 条 mov，P211 字段/callee 两旋钮均无中转形。
    Inter_MailBox_Req_SendPktView* pPkt = reinterpret_cast<Inter_MailBox_Req_SendPktView*>(data);
    if (pPkt->m_0 == 0)
    {
        // ORIG 0x084d017c：m_4 先物化 %ebx（跨 call 缓存），GetServerProxy 结果免槽直链
        register unsigned int mailNo = pPkt->m_4;   // ORIG +0x1a mov 0x4(%eax),%ebx
        // ORIG 中段三段 `mov 0x8(%eax),%eax; mov %eax,%edx; mov mgr,%eax`（4.1 先占
        // %eax 再挤 edx）；4.4 恒折单段 mem→edx（N18a 镜像），register asm 变体
        // 引 mgr 覆盖序错乱，blocked。
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)pPkt->m_8)->SendNotifyNewMail(mailNo);
    }
    return 0;
}

Inter_Monitor_Notify_New_Mail::~Inter_Monitor_Notify_New_Mail() {}
// ORIG 0x084d017c T dispatch_sig —— 监控侧新邮件到达通知。
// 报告 docs/class_func_reports/Inter_Monitor_Notify_New_Mail/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_Notify_New_MailPktView —— Inter_Monitor_Notify_New_Mail::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_Monitor_Notify_New_MailPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

int Inter_Monitor_Notify_New_Mail::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_Monitor_Notify_New_MailPktView* pPkt = reinterpret_cast<Inter_Monitor_Notify_New_MailPktView*>(data);
    if (user->getCurCharacR() == 0 ||
        user->getCurCharacNo() != pPkt->m_a)
        return 0;

    reinterpret_cast<WongWork::CMailBox*>(user->GetMailBox())
        ->IncNotLoadedMailCount();

    PacketGuard packet;
    packet.put_header(0, 99);
    packet.put_short(reinterpret_cast<WongWork::CMailBox*>(user->GetMailBox())
                         ->GetNotLoadedMailCount());
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_InsertCreatureItemResult::~Inter_InsertCreatureItemResult() {}
// ORIG 0x084d027e T dispatch_sig —— 生物蛋孵化结果插入（校验当前角色后透传）。
// 报告 docs/class_func_reports/Inter_InsertCreatureItemResult/dispatch_sig.md 全文转写。
// 包视图直接用 SIG_CREATURE_ITEM（m_characNo@+0x48 单级带偏移读，ORIG 0x084d02a4）。

int Inter_InsertCreatureItemResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x084d02xx：包基址先落具名局部（-0xc），+0x48 用 `mov 0x48(%edx),%edx` 单级
    // 带偏移读；第二次调用实参为 `mov -0xc(%ebp),%edx; mov %edx,0x4(%esp)` 晚装填
    // （同型 lvalue 实参 ⇒ 不提前缓存；异型 cast 实参 gcc 提前入 %ebx 跨 call，probe 实证）。
    SIG_CREATURE_ITEM* pPkt = reinterpret_cast<SIG_CREATURE_ITEM*>(data);
    if (user->getCurCharacR() == 0 ||
        user->getCurCharacNo() != pPkt->m_characNo)
        return 0x2e45;
    user->getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItemResult(
        pPkt);
    return 0;
}

Inter_UpdateCreatureItemResult::~Inter_UpdateCreatureItemResult() {}
// ORIG 0x084d02ee T dispatch_sig —— 生物蛋孵化结果更新（校验当前角色后透传）。
// 报告 docs/class_func_reports/Inter_UpdateCreatureItemResult/dispatch_sig.md 全文转写。
int Inter_UpdateCreatureItemResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x084d02xx：同 InsertCreatureItemResult —— 同型 lvalue 实参晚装填（见彼处注释）。
    SIG_CREATURE_ITEM* pPkt = reinterpret_cast<SIG_CREATURE_ITEM*>(data);
    if (user->getCurCharacR() == 0 ||
        user->getCurCharacNo() != pPkt->m_characNo)
        return 0x2e5d;
    user->getCurCharacInvenW()->GetCreatureMgrW()->UpdateCreatureItemResult(
        pPkt);
    return 0;
}

Inter_MonitorNoticeMessage::~Inter_MonitorNoticeMessage() {}
// ORIG 0x084d035e T dispatch_sig —— 监控侧公告全员播发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMessage/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMessagePktView —— Inter_MonitorNoticeMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMessagePktView
{
    char m_pad0[0xa];  // 未消费
    char m_a;                 // +0x0a
    char m_b[1];              // +0xb 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_MonitorNoticeMessage::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorNoticeMessagePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMessagePktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 0x6e);
    packet.put_byte(1);
    packet.put_byte(1);
    if (pPkt->m_a == '\0')
        return 0x2e82;                                 // 无公告内容，包废弃

    packet.put_int((unsigned int)(unsigned char)pPkt->m_a);
    packet.put_str(pPkt->m_b,
                   (unsigned int)(unsigned char)pPkt->m_a);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_LastMsgResponse::~Inter_LastMsgResponse() {}
// ORIG 0x084d0598 T dispatch_sig —— 关服末条消息应答计数。
// 报告 docs/class_func_reports/Inter_LastMsgResponse/dispatch_sig.md 全文转写。
int Inter_LastMsgResponse::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    // ORIG 0x084d0598：mgr 结果免槽直用（+0x08 mov %eax,(%esp)）；两计数跨 trace 调用缓存
    // %esi/%ebx（push esi/ebx，技巧2/24）；末比较 setle 物化（+0x8f）。
    GetInstanceShutdowManager()->IncrementLastMsgResponseCount();
    register unsigned int responseCnt =
        GetInstanceShutdowManager()->GetLastMsgResponseCount();
    register unsigned int msgCnt = GetInstanceShutdowManager()->GetLastMsgCount();
    cMyTrace(__PRETTY_FUNCTION__,
        0x2eb2, 0)("last msg (%d) response(%d)\n", msgCnt, responseCnt);
    // ORIG +0x7e 第 4 次 call 结果复用 %ebx，+0x8d cmp %eax,%ebx; setle %al; test; je（bool 物化）
    if ((int)GetInstanceShutdowManager()->GetLastMsgCount() <=
        (int)GetInstanceShutdowManager()->GetLastMsgResponseCount())
    {
        GetInstanceShutdowManager()->OnAllUserLogout();
    }
    return 0;
}

Inter_MonitorGuildPointDel::~Inter_MonitorGuildPointDel() {}
// ORIG C1 W 0x084eb11a / D1 W 0x084eb136（D0 0x084eb166）—— 仅置 vptr。
// ORIG T 0x084ce6d2 —— 月初 05:00 基准时刻；7 月（tm_mon==6）且年 ==0x6a 时特例 6 日。
// 报告 docs/class_func_reports/Inter_MonitorGuildPointDel/_makeDefaultTime.md 全文转写。
time_t Inter_MonitorGuildPointDel::_makeDefaultTime()
{
    time_t now = time(0);
    struct tm localTm;
    struct tm* pTm = localtime_r(&now, &localTm);
    pTm->tm_mday = 1;
    pTm->tm_hour = 5;
    pTm->tm_min = 0;
    pTm->tm_sec = 0;
    if (pTm->tm_mon == 6 && pTm->tm_year == 0x6a)
        pTm->tm_mday = 6;
    now = mktime(pTm);
    return now;
}
// ORIG 0x084ce756 T dispatch_sig —— 监控服公会点删除物品扣减。
// 报告 docs/class_func_reports/Inter_MonitorGuildPointDel/dispatch_sig.md 全文转写。
int Inter_MonitorGuildPointDel::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    std::vector<std::pair<int, int> > items;
    items.push_back(std::make_pair(0xc84, 0));
    G_GameWorld()->deleteSpecificItems(&items);
    Inter_MonitorGuildPointDel::setLastDeleteTime();
    return 0;
}

Inter_MonitorGuildWarInfo::~Inter_MonitorGuildWarInfo() {}
// ORIG 0x084ce7f6 T dispatch_sig —— 公会战信息包（0x70，频道 6 专用）。
// 报告 docs/class_func_reports/Inter_MonitorGuildWarInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorGuildWarInfoPktView —— Inter_MonitorGuildWarInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorGuildWarInfoItemView
{
    int m_12;                         // +0x00（abs +0x12，ORIG `add $0x10; mov 0x2(%eax)`）
    int m_16;                         // +0x04（abs +0x16）
    unsigned short m_1a;              // +0x08（abs +0x1a）
    char m_1c[0x17];                  // +0x0a（abs +0x1c，stride 0x21）
};
struct Inter_MonitorGuildWarInfoPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;                  // +0x0e
    Inter_MonitorGuildWarInfoItemView m_items[10];  // +0x12，stride 0x21
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=153（ORIG 0x084ce7f6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=125（ORIG 0x84ce7f6）：N22 cmp 操作数方向+N18a 单段 regmove+N82/P112 帧槽平移。
// [b28d-SweepInterDispatcher2] verdict=AE（ORIG 0x084ce7f6，192=192，TU 121/1330/5/134）：真修四处——
//   ① ItemView(+0x12 起 stride 0x21，字段 +0/+4/+8/+0xa) 复现 ORIG `lea (%ecx,%eax,1); add $0x10; mov 0x2(%eax)` 两步寻址；
//   ② 循环 1 负条件 goto 尾空标签复现 4.1 nop 岛（P223）；③ pPkt 声明移到 GetChannelType 检查后（P0/声明位置）；
//   ④ 删死局部 local_20（P171）+ m_e int→unsigned int 触发比较点 `mov %eax,%edx` 中转（P211/P265 档③，sizeof/偏移/mangled 零影响）。
int Inter_MonitorGuildWarInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    if (G_GameWorld()->GetChannelType() != 6)
        return 0;

    Inter_MonitorGuildWarInfoPktView* pPkt = reinterpret_cast<Inter_MonitorGuildWarInfoPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2ad4,
            "GuildWarInfo : Inter_MonitorGuildWarInfo::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x70);
    packet.put_byte(1);

    int local_1c = 0;
    for (int local_18 = 0; local_18 < 10; ++local_18)
    {
        if (pPkt->m_items[local_18].m_12 == 0)
            goto count_done;
        local_1c = local_1c + 1;
    count_done:;
    }
    packet.put_byte(local_1c);

    for (int local_14 = 0; local_14 < local_1c; ++local_14)
    {
        packet.put_int(pPkt->m_items[local_14].m_12);
        packet.put_int(pPkt->m_items[local_14].m_16);
        packet.put_short((unsigned int)pPkt->m_items[local_14].m_1a);
        const int local_10 = (int)strlen(pPkt->m_items[local_14].m_1c);
        packet.put_int(local_10);
        packet.put_str(pPkt->m_items[local_14].m_1c, local_10);
    }

    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_CodeChecksumRange::~Inter_CodeChecksumRange() {}
// ORIG 0x084d0648 T dispatch_sig —— 校验码区间比对：对所有登录用户执行校验并计数作弊。
// 报告 docs/class_func_reports/Inter_CodeChecksumRange/dispatch_sig.md 全文转写。
// [b28d-PoolInterDisp] 收口：getUserByAccIdMap 发明 getter 已删（ORIG 无该符号）；
// 本类经 CGameManager.h `friend class Inter_CodeChecksumRange` 直访 m_userByAccId，
// 复现 ORIG 0x084d069c `call G_CGameManager; lea 0x22c(%eax)` 无 call 形。
int Inter_CodeChecksumRange::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    CodeHackCheckRangeEx range;
    CodeHackChecksumEx checksum;

    const char* next = range.setData(data);
    checksum.setData(next);

    // ORIG 0x084d069c：`call G_CGameManager; lea 0x22c(%eax),%edx`（无具名槽）。
    for (std::map<unsigned int, CUser*>::iterator it =
             G_CGameManager()->m_userByAccId.begin();
         it != G_CGameManager()->m_userByAccId.end(); ++it)
    {
        CUser* target = it->second;                    // pair second
        if (!target->checkCodeChecksumRange(checksum, range))   // xor $1 取反
        {
            if (target->get_state() > 1)               // cmp $1,setg
            {
                // ORIG 0x084d0715 `mov %eax,-0xc`：结果落槽但从未使用（技巧8 死槽）。
                int answerCnt = target->GetChecksumAnswerCount();
                (void)answerCnt;
                target->getHackAnalyzer()->addServerHackCnt(
                    target, (WongWork::ENUM_HACKTYPE)0x3b7, 1, 0, 0);
            }
        }
    }
    return 0;
}

Inter_MonitorForbidChat::~Inter_MonitorForbidChat() {}
// ORIG 0x084d0466 T dispatch_sig —— 监控侧禁言指令落地。
// 报告 docs/class_func_reports/Inter_MonitorForbidChat/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorForbidChatPktView —— Inter_MonitorForbidChat::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorForbidChatPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    size_t m_e;               // +0x0e
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=67（ORIG 0x084d0466）：槽序/帧差(N82/P212/P221)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=55（ORIG 0x84d0466）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MonitorForbidChat::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorForbidChatPktView* pPkt = reinterpret_cast<Inter_MonitorForbidChatPktView*>(data);
    cMyTrace(__PRETTY_FUNCTION__, 0x2e96, 0)("Inter_MonitorForbidChat::dispatch_sig recv\n");

    const size_t nameLen = pPkt->m_e;
    char name[30];                                              // ORIG call memset 清零
    memset(name, 0, sizeof(name));
    strncpy(name, pPkt->m_12, nameLen);

    if (!G_GameWorld()->IsForbiddenChat(name))
    {
        G_GameWorld()->InsertUserToIDMap(name, 0);
        const int secs = pPkt->m_a;
        G_GameWorld()->DisableChat(name, secs);
        cMyTrace(__PRETTY_FUNCTION__, 0x2ea2, 0)("Inter_MonitorForbidChat::dispatch_sig %s for %d secs\n", name, secs);
    }
    return 0;
}

Inter_MouseRegister::~Inter_MouseRegister() {}
// ORIG 0x084d07d2 T dispatch_sig —— 密保鼠标注册/注销结果处理，回发 0x74 应答。
// 报告 docs/class_func_reports/Inter_MouseRegister/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MouseRegisterPktView —— Inter_MouseRegister::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MouseRegisterPktView
{
    char m_pad0[0x4];  // 未消费
    int m_4;                  // +0x04
    char m_pad8[0xe];  // 未消费
    int m_16;                 // +0x16
    char m_pad1a[0x1];  // 未消费
    int m_1b;                 // +0x1b
    int m_1f;                 // +0x1f
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=119（ORIG 0x084d07d2）：槽序/帧差(N82/P212/P221)+混合残差；gap-15。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=235（ORIG 0x84d07d2）：N82/P112 帧槽平移+P30/N4 add-vs-lea+P264 .o 重定位/立即数。
int Inter_MouseRegister::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_MouseRegisterPktView* pPkt = reinterpret_cast<Inter_MouseRegisterPktView*>(data);
    if (pPkt->m_16 != 0)   // msg+0x16 错误码
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x74,
                                 static_cast<unsigned char>(
                                     pPkt->m_16));
        return 0;
    }

    const int action = pPkt->m_4;   // msg+0x04 动作码
    if (action == 1 || action == 2)
    {
        Sanicova::CPad* pad = user->getPad();
        pad->setActivate(true);
        Sanicova::CPad* pad2 = user->getPad();
        pad2->setCertified(true);
        Sanicova::CPad* pad3 = user->getPad();
        pad3->setPassword(data + 0x23);              // msg+0x23 密码
        Sanicova::CPad* pad4 = user->getPad();
        pad4->setCancelCnt(pPkt->m_1b);
        Sanicova::CPad* pad5 = user->getPad();
        pad5->setFailCnt(pPkt->m_1f);
    }
    else if (action == 3)
    {
        Sanicova::CPad* pad = user->getPad();
        pad->setActivate(false);
        Sanicova::CPad* pad2 = user->getPad();
        pad2->setCertified(false);
        Sanicova::CPad* pad3 = user->getPad();
        pad3->setPassword(data + 0x23);
    }

    PacketGuard packet;
    packet.put_header(1, 0x74);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_Monitor_MegaphoneMsg::~Inter_Monitor_MegaphoneMsg() {}
// ORIG 0x084d09aa T dispatch_sig —— 大喇叭广播：构造广播包后按频道类型全服/分组发送。
// 报告 docs/class_func_reports/Inter_Monitor_MegaphoneMsg/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_MegaphoneMsgPktView —— Inter_Monitor_MegaphoneMsg::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Monitor_MegaphoneMsgPktView
{
    char m_pad0[0xa];  // 未消费
    char m_a;                 // +0x0a 服务器组（send_server_group 第二实参，ORIG +0x126 movzbl/movsbl）
    unsigned char m_b;        // +0x0b
    unsigned short m_c;       // +0x0c
    unsigned char m_e;        // +0x0e
    char m_f[1];              // +0xf 串/字节区首元素（容量未建模）
    char m_pad10[0x1d];  // 未消费
    unsigned char m_2d;       // +0x2d
    char m_2e[1];             // +0x2e 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-solo-InterDispatcher] 真语义修复：ORIG 0x084d0acd 的 group 实参来自 recv+0xa
//   （`movzbl 0xa(%eax); movsbl %al,%ebx`），非 IsIntegratedPvPBaseChannel() 返回值；
//   条件改内联正向（callee bool ⇒ ORIG `test %al,%al; je` 两条形）。
int Inter_Monitor_MegaphoneMsg::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Monitor_MegaphoneMsgPktView* pPkt = reinterpret_cast<Inter_Monitor_MegaphoneMsgPktView*>(data);

    PacketGuard packet;
    packet.put_header(0, 0x76);
    packet.put_byte((unsigned int)pPkt->m_e);
    packet.put_byte((unsigned int)pPkt->m_b);
    packet.put_short((unsigned int)pPkt->m_c);
    packet.put_int(strlen(pPkt->m_f));
    packet.put_str(pPkt->m_f, strlen(pPkt->m_f));    // ORIG 两次独立 strlen（P48）
    packet.put_int((unsigned int)pPkt->m_2d);
    packet.put_str(pPkt->m_2e,
                   (unsigned int)pPkt->m_2d);
    packet.finalize(true);

    // ORIG 0x084d0ac9：bool 直测；非 0 → send_server_group(group=recv+0xa)，0 → send_all
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        G_GameWorld()->send_server_group(packet, pPkt->m_a);
    }
    else
    {
        G_GameWorld()->send_all(packet);
    }
    return 0;
}

Inter_Monitor_RegisterBlackListResult::~Inter_Monitor_RegisterBlackListResult() {}
// ORIG 0x084d0de0 T dispatch_sig —— 黑名单登记结果通知（0x79 包，按结果码填错误值）。
// 报告 docs/class_func_reports/Inter_Monitor_RegisterBlackListResult/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_RegisterBlackListResultPktView —— Inter_Monitor_RegisterBlackListResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Monitor_RegisterBlackListResultPktView
{
    char m_pad0[0xe];  // 未消费
    char m_e[1];              // +0xe 串/字节区首元素（容量未建模）
    char m_padf[0x1d];  // 未消费
    unsigned int m_2c;        // +0x2c
    unsigned char m_30;       // +0x30
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=129（ORIG 0x084d0de0）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+6。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=74（ORIG 0x84d0de0）：P226 具名 bool/char 落槽+N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_Monitor_RegisterBlackListResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_Monitor_RegisterBlackListResultPktView* pPkt = reinterpret_cast<Inter_Monitor_RegisterBlackListResultPktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 0x79);

    if (pPkt->m_30 == '\x01')
    {
        if (!user->registerToBlackList(
                pPkt->m_2c))
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x2f90,
                "_BLACK_LIST_ :: false == pUser->registerToBlackList( recv->m_uCharacNo )");
            packet.put_byte(0);
            packet.put_int(0x4a);
        }
        else
        {
            packet.put_byte(1);
        }
    }
    else if (pPkt->m_30 == '\x02')
    {
        packet.put_byte(0);
        packet.put_int(0x4a);
    }
    else if (pPkt->m_30 == '\x03')
    {
        packet.put_byte(0);
        packet.put_int(0x4c);
    }
    else if (pPkt->m_30 == '\x04')
    {
        packet.put_byte(0);
        packet.put_int(0x4e);
    }
    else if (pPkt->m_30 == '\x05')
    {
        packet.put_byte(0);
        packet.put_int(0x5a);
    }

    const int local_10 = (int)strlen(pPkt->m_e);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_e, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_Monitor_DeleteBlackListResult::~Inter_Monitor_DeleteBlackListResult() {}
// ORIG 0x084d1034 T dispatch_sig —— 黑名单删除结果应答：按结果码组 0x7a 包回发。
// 报告 docs/class_func_reports/Inter_Monitor_DeleteBlackListResult/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_DeleteBlackListResultPktView —— Inter_Monitor_DeleteBlackListResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Monitor_DeleteBlackListResultPktView
{
    char m_pad0[0xe];  // 未消费
    char m_e[1];              // +0xe 串/字节区首元素（容量未建模）
    char m_padf[0x1d];  // 未消费
    unsigned int m_2c;        // +0x2c
    unsigned char m_30;       // +0x30
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=116（ORIG 0x084d1034）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=58（ORIG 0x84d1034）：P226 具名 bool/char 落槽+N18a 单段 regmove。
int Inter_Monitor_DeleteBlackListResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG 0x084d1042 段：包基址在 state 门之后（且晚于首个分支块）才物化
    //（`mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` 落于 jmp 之后）⇒ 声明位置照抄（P0）。
    Inter_Monitor_DeleteBlackListResultPktView* pPkt = reinterpret_cast<Inter_Monitor_DeleteBlackListResultPktView*>(data);

    PacketGuard packet;
    packet.put_header(1, 0x7a);
    // ORIG 0x84d1034 段：无 result 具名槽（每比较点重发 `mov -0xSLOT(%ebp),%eax;
    // movzbl 0x30(%eax),%eax; cmp $K,%al`）⇒ 内联 pPkt->m_30（P0/P226）。
    if (pPkt->m_30 == 1)
    {
        // deleteToBlackList 返回 bool：ORIG xor $0x1,%eax; test %al 取反检验
        if (!user->deleteToBlackList(
                pPkt->m_2c))
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x2fd7,
                "_BLACK_LIST_ :: false == pUser->deleteToBlackList( recv->m_uCharacNo )");
            packet.put_byte(0);
            packet.put_int(0x4b);
        }
        else
        {
            packet.put_byte(1);
        }
    }
    else if (pPkt->m_30 == 2)
    {
        packet.put_byte(0);
        packet.put_int(0x4b);
    }
    else if (pPkt->m_30 == 3)
    {
        packet.put_byte(0);
        packet.put_int(0x4c);
    }

    const int nameLen =                              // msg+0x0e 目标名
        static_cast<int>(strlen(pPkt->m_e));
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_e, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_Monitor_RequestBlackListResult::~Inter_Monitor_RequestBlackListResult() {}
// ORIG 0x084d0ba6 T dispatch_sig —— 黑名单查询结果下发（名字+时间+注册黑名单）。
// 报告 docs/class_func_reports/Inter_Monitor_RequestBlackListResult/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_RequestBlackListResultPktView —— Inter_Monitor_RequestBlackListResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_Monitor_RequestBlackListResultPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned char m_e;              // +0x0e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=142（ORIG 0x084d0ba6）：寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=56（ORIG 0x84d0ba6）：P30/N4 add-vs-lea。
int Inter_Monitor_RequestBlackListResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    Inter_Monitor_RequestBlackListResultPktView* pPkt = reinterpret_cast<Inter_Monitor_RequestBlackListResultPktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 0x7b);
    packet.put_byte(1);
    if (pPkt->m_e <= 0xa)
    {
        packet.put_byte(
            (int)pPkt->m_e);
        for (int i = 0;
             i < (int)(unsigned int)pPkt->m_e;
             ++i)
        {
            const int nameLen = (int)strlen(data + i * 0x28 + 0x13);
            if (nameLen == 0)
            {
                LogManager::logFormat(
                    1, "InterDispatcher.cpp",
                    __PRETTY_FUNCTION__,
                    0x2f6d, "BlackListNameLen Err charno(%d)",
                    user->getCurCharacNo());
            }
            packet.put_int((int)nameLen);
            packet.put_str(data + i * 0x28 + 0x13, (int)nameLen);
            struct tm tmVal;                               // P43：经返回值访问
            struct tm* ptm = localtime_r(
                reinterpret_cast<time_t*>(data + i * 0x28 + 0x33), &tmVal);
            packet.put_byte(ptm->tm_year);
            packet.put_byte(ptm->tm_mon);
            packet.put_byte(ptm->tm_mday);
            user->registerToBlackList(
                *reinterpret_cast<unsigned int*>(data + i * 0x28 + 0xf));
        }
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_DB_RequestBlackCount::~Inter_DB_RequestBlackCount() {}
// ORIG 0x084d1218 T dispatch_sig —— DB 黑名单计数应答。
// 报告 docs/class_func_reports/Inter_DB_RequestBlackCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_DB_RequestBlackCountPktView —— Inter_DB_RequestBlackCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DB_RequestBlackCountPktView
{
    char m_pad0[0x4];  // 未消费
    int m_4;                  // +0x04
    int m_8;                  // +0x08（ORIG +0x2f 两步 spill＝signed 字段→setCurrCharacBlackCount(unsigned) 失配）
    int m_c;                  // +0x0c
};
#pragma pack(pop)

int Inter_DB_RequestBlackCount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_DB_RequestBlackCountPktView* pPkt = reinterpret_cast<Inter_DB_RequestBlackCountPktView*>(data);

    if (pPkt->m_4 == 0)
        user->setCurrCharacBlackCount(pPkt->m_8);
    else
        user->setCurrCharacBlackCount(0);
    user->EnableCharacInfo((CACHE_SECTION)7);

    if (pPkt->m_c != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0xec);
        packet.put_int(pPkt->m_c);
        packet.put_int(pPkt->m_8);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_MonitorNoticeBlackList::~Inter_MonitorNoticeBlackList() {}
// ORIG 0x084d0b3a T dispatch_sig —— 监控服黑名单列表登记。
// 报告 docs/class_func_reports/Inter_MonitorNoticeBlackList/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeBlackListPktView —— Inter_MonitorNoticeBlackList::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
// [b28d-lane2] P235 双视角：id 数组与头字段同址重叠，合入一个完整类型的匿名 union
//   （原两个独立视图经 reinterpret_cast 访问使 4.4 恒「base 先载」；union 完整类型成员
//   直访问复现 ORIG 0x084d0b73 的「index 先载」序，探针 /tmp/l2probe/p11.cpp fa 实证）。
struct Inter_MonitorNoticeBlackListPktView
{
    char m_pad0[3];               // +0x0
    union
    {
        // P311 二维视图：`m_ids[1][i]` 地址等价旧 `m_ids[i+4]`，但 4.4 对常量首维发
        // `add $0x4,%edx`（ORIG 形）而非 `lea 0x4(%eax),%edx`；sizeof 1040 不变。
        unsigned int m_ids[65][4];  // +0x3（ORIG `mov 0x3(%eax,%edx,4),%eax`；容量上界 len/4）
        struct
        {
            char m_pad3[0xf];     // +0x3..+0x11
            unsigned char m_12;   // +0x12 id 个数
        } m_hdr;
    };
};
#pragma pack(pop)

// [b28d-lane2] verdict=NEAR ndiff=2（ORIG 0x084d0b3a）：块序重排(N104/P223)+槽序/帧差(N82/P212/P221)；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：P311 二维数组视图 m_ids[1][i]
// 复现 ORIG `add $0x4,%edx`+`mov 0x3(%eax,%edx,4)`；类型化 recv 出 index-first 装载序）。
int Inter_MonitorNoticeBlackList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG 0x084d0b5b：recv 在 state 检查后才物化 -0x10；count 每轮 cond 重读（movzbl 双扩展 + cmp/setg）
    // [b28d-lane2] 实测三形：①两独立视图 cast=NEAR 2（装载序反）；②union 视图+char* recv=NEAR 2；
    //   ③union 视图+类型化 recv=DIFF 3（装载序正但 `lea 0x4(%eax),%edx` vs ORIG `add $0x4,%edx`
    //   + i 落 %eax vs %edx）。取②（残差最小）。add-vs-lea 与寄存器名属 P30/N4 结构性伪影。
    Inter_MonitorNoticeBlackListPktView* recv =
        reinterpret_cast<Inter_MonitorNoticeBlackListPktView*>(data);
    for (int i = 0; i < (int)(unsigned char)recv->m_hdr.m_12; ++i)
    {   // ORIG 0x084d0b73 mov 0x3(%eax,%edx,4),%eax：具名数组视图 [i+4] 保 add $4+disp3 折叠形
        user->registerToBlackList(recv->m_ids[1][i]);
    }
    return 0;
}

Inter_WebNoticeSingle::~Inter_WebNoticeSingle() {}
// ORIG 0x084d1344 T dispatch_sig —— 网页单条公告全员播发。
// 报告 docs/class_func_reports/Inter_WebNoticeSingle/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_WebNoticeSinglePktView —— Inter_WebNoticeSingle::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_WebNoticeSinglePktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_a;        // +0x0a
    char m_b[1];              // +0xb 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_WebNoticeSingle::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_WebNoticeSinglePktView* pPkt = reinterpret_cast<Inter_WebNoticeSinglePktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int((unsigned int)pPkt->m_a);
    packet.put_str(pPkt->m_b,
                   (unsigned int)pPkt->m_a);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_MemberExpLevelUp::~Inter_MemberExpLevelUp() {}
// ORIG 0x084d1448 T dispatch_sig —— 会员经验等级上限同步。
// 报告 docs/class_func_reports/Inter_MemberExpLevelUp/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MemberExpLevelUpPktView —— Inter_MemberExpLevelUp::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MemberExpLevelUpPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

int Inter_MemberExpLevelUp::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG：门1 = get_state()==0 早退（`test %eax,%eax`+`je`，无 sete 物化）；
    // 包基址槽在门1 之后才落 -0xc；门2 用 (bool) 显式物化（`setne`+`test`）。
    if (user->get_state() == 0)
        return 0;
    Inter_MemberExpLevelUpPktView* pPkt = reinterpret_cast<Inter_MemberExpLevelUpPktView*>(data);
    if ((bool)(user->getCurCharacNo() != pPkt->m_e))
        return 0;
    user->set_uppermember_exp_level(
        pPkt->m_12);
    return 0;
}

Inter_MonitorAddBuddy::~Inter_MonitorAddBuddy() {}
// ORIG 0x084d14b0 T dispatch_sig —— 好友添加结果通知（0x3f 包，成功带好友信息/失败带错误码）。
// 报告 docs/class_func_reports/Inter_MonitorAddBuddy/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorAddBuddyPktView —— Inter_MonitorAddBuddy::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorAddBuddyPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    short m_30;               // +0x30
    signed char m_32;         // +0x32
    signed char m_33;         // +0x33
    int m_34;                 // +0x34
    signed char m_38;         // +0x38
    unsigned char m_39;       // +0x39
    char m_3a;                // +0x3a
};
#pragma pack(pop)

int Inter_MonitorAddBuddy::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x10(%ebp)
    Inter_MonitorAddBuddyPktView* pPkt = reinterpret_cast<Inter_MonitorAddBuddyPktView*>(data);
    if (user->getCurCharacNo() != pPkt->m_a)
        return 0;

    PacketGuard packet;
    if (pPkt->m_3a == '\0')
    {   // 成功
        packet.clear();
        packet.put_header(1, 0x3f);
        packet.put_byte(1);
        // ORIG 0x084d15xx：strlen 参走指针算术（add），put_str 参走数组 decay（lea）；
        // len 槽 int 精确匹配 put_str(char*,int)，避免隐式转换重排求值序。
        const int local_10 =
            (int)strlen(reinterpret_cast<char*>(pPkt) + 0x12);
        packet.put_int(local_10);
        packet.put_str(pPkt->m_12, local_10);
        packet.put_byte((int)pPkt->m_30);
        packet.put_byte((int)pPkt->m_32);
        packet.put_byte((int)pPkt->m_33);
        packet.put_int(pPkt->m_34);
        packet.put_byte((int)pPkt->m_38);
        packet.put_byte(
            (unsigned int)pPkt->m_39);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {   // 失败（错误码）
        packet.clear();
        packet.put_header(1, 0x3f);
        packet.put_byte(0);
        packet.put_byte((int)pPkt->m_3a);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_MonitorDelBuddy::~Inter_MonitorDelBuddy() {}
// ORIG 0x084d16f6 T dispatch_sig —— 好友删除结果应答：按错误码组 0x40 包回发。
// 报告 docs/class_func_reports/Inter_MonitorDelBuddy/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorDelBuddyPktView —— Inter_MonitorDelBuddy::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorDelBuddyPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    signed char m_30;         // +0x30
};
#pragma pack(pop)

int Inter_MonitorDelBuddy::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG 0x084d13xx：包基址在 get_state 检查之后物化入 -0x10(%ebp)
    Inter_MonitorDelBuddyPktView* pPkt = reinterpret_cast<Inter_MonitorDelBuddyPktView*>(data);
    if (user->getCurCharacNo() !=
        pPkt->m_a)         // msg+0x0a 角色号校验
        return 0;

    PacketGuard packet;
    if (pPkt->m_30 == 0)   // msg+0x30 成功
    {
        packet.clear();
        packet.put_header(1, 0x40);
        packet.put_byte(1);
        const int nameLen =                          // msg+0x12 被删好友名
            static_cast<int>(strlen(pPkt->m_12));
        packet.put_int(nameLen);
        packet.put_str(pPkt->m_12, nameLen);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        packet.clear();
        packet.put_header(1, 0x40);
        packet.put_byte(0);
        packet.put_byte(                             // msg+0x30 char（movsbl）
            static_cast<int>(pPkt->m_30));
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_ChangeLetterStat::~Inter_ChangeLetterStat() {}
// ORIG 0x084d18ac T dispatch_sig —— 信件状态变更（keep 计数增删 + 0x89 回包）。
// 报告 docs/class_func_reports/Inter_ChangeLetterStat/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ChangeLetterStatPktView —— Inter_ChangeLetterStat::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ChangeLetterStatPktView
{
    int m_0;                  // +0x00
    int m_4;                  // +0x04
    unsigned short m_8;       // +0x08
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=76（ORIG 0x084d18ac）：寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（ORIG 0x084d18ac）：P248 档①（callee
// DeleteLetterKeepCount int→bool + 裸 `if (!f())`，Main 代落头/定义）⇒ 本波归零。
int Inter_ChangeLetterStat::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ChangeLetterStatPktView* pPkt;
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    pPkt = reinterpret_cast<Inter_ChangeLetterStatPktView*>(data);
    if (user->getCurCharacR() == 0 ||
        user->getCurCharacNo() != pPkt->m_0)
        return 0x30a2;

    if ((short)pPkt->m_8 == 3)
    {
        if (static_cast<WongWork::CMailBox*>(user->GetMailBox()) != 0)
        {
            register int letterIdx = pPkt->m_4;
            static_cast<WongWork::CMailBox*>(user->GetMailBox())
                ->InsertLetterKeepCount(letterIdx);
            static_cast<WongWork::CMailBox*>(user->GetMailBox())
                ->DecLoadedLetterCount();
        }
    }
    else if ((short)pPkt->m_8 == 0)
    {
        if (static_cast<WongWork::CMailBox*>(user->GetMailBox()) != 0)
        {
            register int deleteIdx = pPkt->m_4;
            // [b28d-solo-InterDispatcher] P248 档①：ORIG 0x084d19aa 调用点为
            // `call; xor $0x1,%eax; test %al,%al; je`（3 条）⇒ callee 返回 bool；
            // 裸 `if (!f())` 即复现（Main 已代落 CMailBoxHelper.h/W6_RankMailDeps.cpp int→bool）。
            if (!static_cast<WongWork::CMailBox*>(user->GetMailBox())
                     ->DeleteLetterKeepCount(deleteIdx))
                static_cast<WongWork::CMailBox*>(user->GetMailBox())
                    ->DecLoadedLetterCount();
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0x89);
    if (pPkt->m_4 == 0)
    {
        packet.put_byte(0);
        packet.put_byte(2);
    }
    else
    {
        packet.put_byte(1);
        packet.put_int(pPkt->m_4);
        packet.put_short((int)(short)pPkt->m_8);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_SaveClientHackLog::~Inter_SaveClientHackLog() {}
// ORIG 0x084d1ab6 T dispatch_sig —— 客户端外挂日志：按类型表计算掉率惩罚并入库。
// 报告 docs/class_func_reports/Inter_SaveClientHackLog/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SaveClientHackLogPktView —— Inter_SaveClientHackLog::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SaveClientHackLogPktView
{
    unsigned int m_0;         // +0x00
    char m_pad4[0x4];  // 未消费
    unsigned short m_8;       // +0x08
    char m_pada[0x4];  // 未消费
    int m_e;                  // +0x0e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=127（ORIG 0x084d1ab6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=117（ORIG 0x84d1ab6）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_SaveClientHackLog::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SaveClientHackLogPktView* pPkt = reinterpret_cast<Inter_SaveClientHackLogPktView*>(data);
    // §P0/P52：ORIG 084d1ac6 把 m_0 读入 %ebx 跨 G_GameWorld call（无槽）⇒ 不设具名 accId。
    CUser* target = G_CGameManager()->getUser(
        static_cast<int>(G_GameWorld()->find_session(pPkt->m_0)), 0x30cd);
    if (target == 0)
        return 0;

    if (!ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            RestrictCategory::Enum_3, 1))
        return 0;                                    // asm xor $0x1,test 取反提前返回

    int penalty = 0;                                 // local_20[0]/asm -0x1c(%ebp)
    // ORIG 084d1b46..084d1b69：二分决策树（cmp $0x1f7 / jg / cmp $0x193 / cmp $0x326 /
    //   cmp $0x3ba）⇒ 原码是 switch（§P17/P46，case 按数值升序书写）；m_8/m_e 均无槽往返。
    switch (pPkt->m_8)
    {
    case 0x193:
        if (pPkt->m_e > 0xfa)
            penalty += 0x14;
        break;
    case 0x1f7:
        if (pPkt->m_e > 0x32)
            penalty += 0x28;
        break;
    case 0x326:
        if (pPkt->m_e > 0x19)
            penalty += 0x1e;
        break;
    case 0x3ba:
        if (pPkt->m_e > 0x4b0)
            penalty += 0x14;
        break;
    }

    // [b23S3C] P48：ORIG 两次 getStdDropRate（local_20[2] 落槽供 min 取址；
    // 比较点重新 call 入寄存器 iVar5），勿具名缓存复用。
    penalty = std::min(100, penalty);                // std::min(&local_20[1],&local_20[0])
    int rate = 100 - penalty;                        // local_20[0] = 100 - *min
    int stdDropRate = target->getStdDropRate();      // local_20[2]
    rate = std::min(rate, stdDropRate);              // std::min(&local_20[0],&local_20[2])
    if (target->getStdDropRate() != rate)
    {
        DB_SaveUserHandicap::makeRequest(
            target->get_acc_id(), 0x9c41, 0x65, rate, 3, 0);
        target->setStdDropRate(rate);
    }
    return 0;
}

Inter_ReqChangeCharacName::~Inter_ReqChangeCharacName() {}
#pragma pack(push, 1)
// Inter_ReqChangeCharacNamePktView —— Inter_ReqChangeCharacName::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReqChangeCharacNamePktView
{
    int m_0;                  // +0x00
    char m_pad4[0x20];  // 未消费
    int m_24;                 // +0x24
    int m_28;                 // +0x28
    int m_2c;                 // +0x2c
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=384（ORIG 0x084d1c8a）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+27。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84d1c8a）：本轮三处 findString 内联（P48，p167 M 归零）；余 N91 EH landing（EXTRA _Unwind_Resume×2）+P30/N4。
int Inter_ReqChangeCharacName::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ReqChangeCharacNamePktView* pPkt = reinterpret_cast<Inter_ReqChangeCharacNamePktView*>(data);
    // ORIG 0x084d1c8a 报告 docs/class_func_reports/Inter_ReqChangeCharacName/dispatch_sig.md
    // 两段全读。msg：+0 characNo / +4 newName / +0x24 errFlag / +0x28 rewardFlag /
    // +0x2c guildKey。
    if (user->get_state() <= 1)
        return 0;

    // §P0：ORIG 每处使用都重读 msg+0x0（084d1ce8/1d21/1de2/1e68 等，无槽往返）⇒ 不设具名 characNo。
    if (pPkt->m_24 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a,
                                 static_cast<unsigned char>(
                                     pPkt->m_24 & 0xff));
        user->SetCharacViewStateByCharacNo(pPkt->m_0, 0xff);
        user->UpdateCharacView();
        return 0;
    }

    if (!user->setCharacName(pPkt->m_0, data + 4))
        return 0x31ae;

    {   // 成功应答包
        PacketGuard packet;
        packet.put_header(1, 0x8a);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    user->SetCharacViewStateByCharacNo(pPkt->m_0, 1);
    {   // 雇佣兵名同步（ORIG std::string(data+4) 局部 + UpdateCharacNameInMercenary）
        std::string name(data + 4);
        user->UpdateCharacNameInMercenary(
            static_cast<unsigned int>(pPkt->m_0), name);
    }
    user->UpdateCharacView();

    // ORIG 0x0840b96b 两段：第一段 if(guildKey) guild 1 次 else monitor 1 次；
    // 第二段 if(guildKey) { guild 1 次 + monitor 1 次 }。NEW 原实现把第一段
    // guild 重复两次且缺第二段，属转写错误。
    if (pPkt->m_2c != 0)
    {
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                  static_cast<unsigned int>(pPkt->m_0),
                                  static_cast<unsigned int>(pPkt->m_2c),
                                  data + 4);
    }
    else
    {
        CMonitorServerProxy* monitor =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        monitor->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                    static_cast<unsigned int>(pPkt->m_0),
                                    static_cast<unsigned int>(pPkt->m_2c),
                                    data + 4);
    }
    if (pPkt->m_2c != 0)
    {
        CGuildServerProxy* proxyG =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxyG->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                   static_cast<unsigned int>(pPkt->m_0),
                                   static_cast<unsigned int>(pPkt->m_2c),
                                   data + 4);
        CMonitorServerProxy* monitorM =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        monitorM->SendChangeCharName(static_cast<unsigned int>(user->get_acc_id()),
                                     static_cast<unsigned int>(pPkt->m_0),
                                     static_cast<unsigned int>(pPkt->m_2c),
                                     data + 4);
    }

    if (pPkt->m_28 == 0)
    {   // 改名活动奖励期（ORIG 2007-05 活动阈值 0x464a1f50）
        const long curSec = GlobalData::s_systemTime_.getCurSec();
        if (curSec > 0x464a1f50)
            return 0;
        const short level =
            (short)user->getCharacLevelByCharacNo(pPkt->m_0);
        if (level <= 0)
            return 0x31b7;

        // ORIG 三处 ReqDBSendNewSystemMail 各自独立 findString（p167 M=findString×2 ⇒ 无缓存）。
        if (level > 0x31)
        {
            Inven_Item item;                       // 改名卡 36 号 ×5
            item.m_addInfo = 0x24;
            item.m_field1 = 2;
            item.m_addInfo2 = 5;
            item.m_fieldb = 0;
            item.ResetItemAttr();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(
                    4, "game_server_msg_01", reinterpret_cast<bool*>(0)),
                item, 0u,
                static_cast<unsigned int>(pPkt->m_0),
                reinterpret_cast<const char*>(0), 0, 0u,
                user->GetServerGroup(), false, false);
        }
        if (level > 0x27)
        {
            Inven_Item item;                       // 16 号 ×1
            item.m_addInfo = 0x10;
            item.m_field1 = 2;
            item.m_addInfo2 = 1;
            item.m_fieldb = 0;
            item.ResetItemAttr();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(
                    4, "game_server_msg_01", reinterpret_cast<bool*>(0)),
                item, 0u,
                static_cast<unsigned int>(pPkt->m_0),
                reinterpret_cast<const char*>(0), 0, 0u,
                user->GetServerGroup(), false, false);
        }
        {
            Inven_Item item;                       // 1 号 ×10（无条件）
            item.m_addInfo = 1;
            item.m_field1 = 2;
            item.m_addInfo2 = 0xa;
            item.m_fieldb = 0;
            item.ResetItemAttr();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(
                    4, "game_server_msg_01", reinterpret_cast<bool*>(0)),
                item, 0u,
                static_cast<unsigned int>(pPkt->m_0),
                reinterpret_cast<const char*>(0), 0, 0u,
                user->GetServerGroup(), false, false);
        }
    }

    GlobalData::s_event_manager->changeCharacName(
        user, pPkt->m_0, data + 4,
        pPkt->m_28);
    return 0;
}

Inter_ReqQueryCharacInfo::~Inter_ReqQueryCharacInfo() {}
// ORIG 0x084d22fc T dispatch_sig —— 角色信息查询应答：组 0x8b 包回发。
// 报告 docs/class_func_reports/Inter_ReqQueryCharacInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReqQueryCharacInfoPktView —— Inter_ReqQueryCharacInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReqQueryCharacInfoPktView
{
    char m_pad0[0x20];  // 未消费
    int m_20;                 // +0x20
    char m_pad24[0x4];  // 未消费
    signed char m_28;         // +0x28
    char m_pad29[0x1];  // 未消费
    short m_2a;               // +0x2a
    signed char m_2c;         // +0x2c
    signed char m_2d;         // +0x2d
    signed char m_2e;         // +0x2e
};
#pragma pack(pop)

// [b22判词·N-EH] 正常路径指令序列与 ORIG 逐条一致（get_state 门/m_20 错误分支/
// PacketGuard+put_header/put_byte/strlen×2/put_int/put_str/put_short/put_byte×4/
// finalize/Send/dtor 全齐）。残余 diff 仅为编译器 EH 产物：ORIG 用共享
// `mov $0x0,%eax` 出口 + landing-pad 内 `_Unwind_Resume` 后再接无条件 dtor，
// gcc 对本函数选 %ebx 缓存返回值 0；返回值寄存器与 cleanup 块布局为跨版本
// 编译器结构，源码不可复现，语义等价。
// [b28c-31] N78：ORIG 早退 `mov $0x0,%eax; jmp` 用 %eax 汇聚，4.4 用 %ebx
// 汇聚 + 尾 `mov %ebx,%eax`（0x084d23xx 实证，P78 族）。
// [b28d-lane2] verdict=DIFF ndiff=96（ORIG 0x084d22fc）：寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=12（ORIG 0x84d22fc）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_ReqQueryCharacInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    // local_10 = param_3 在 state 守卫后才赋值（ORIG +0x027 mov 0x10(%ebp),-0xc）
    Inter_ReqQueryCharacInfoPktView* pPkt = reinterpret_cast<Inter_ReqQueryCharacInfoPktView*>(data);
    if (pPkt->m_20 != 0)                             // asm +0x033 test je 主块（错误分支落盘）
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x8b,
            static_cast<unsigned char>(
                pPkt->m_20 & 0xff));
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x8b);
    packet.put_byte(1);
    packet.put_int(static_cast<int>(strlen(reinterpret_cast<const char*>(pPkt))));
    // ORIG put_str(char*,int)（Pc 非 const 重载）：实参 cast 成 char* 精确选中该重载。
    packet.put_str(reinterpret_cast<char*>(pPkt),
                   static_cast<int>(strlen(reinterpret_cast<const char*>(pPkt))));
    packet.put_short(                                // msg+0x2a short（cwtl 符号扩展）
        static_cast<int>(pPkt->m_2a));
    packet.put_byte(                                 // msg+0x28（movsbl）
        static_cast<int>(pPkt->m_28));
    packet.put_byte(                                 // msg+0x2c（movsbl）
        static_cast<int>(pPkt->m_2c));
    packet.put_byte(                                 // msg+0x2d（movsbl）
        static_cast<int>(pPkt->m_2d));
    packet.put_byte(                                 // msg+0x2e（movsbl）
        static_cast<int>(pPkt->m_2e));
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_ReportMannerless::~Inter_ReportMannerless() {}
// ORIG 0x084d24a2 T dispatch_sig —— 无礼行为举报：按类型回错误码或转发 LOGDB 队列 + 确认包。
// 报告 docs/class_func_reports/Inter_ReportMannerless/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReportMannerlessPktView —— Inter_ReportMannerless::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReportMannerlessPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    char m_pad12[0xc];  // +0x12..未消费
    unsigned int m_1e;              // +0x1e
    char m_pad22[0x13c];  // +0x22..未消费
    unsigned char m_15e;            // +0x15e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=127（ORIG 0x084d24a2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+8。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=102（ORIG 0x84d24a2）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N18a 单段 regmove。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 8→-7（164 vs 157，TU 121/1330/5/134 不回退）：真修四处——
//   ① 删 `const unsigned char type`/`Stream* stream` 具名局部（P0/P259，类型检查改逐次 `pPkt->m_15e` 重读、Acquire 内联进 CStreamGuard 实参）；
//   ② `if (data==0)`→`if (pPkt==0)` 使判空读槽（ORIG `cmpl $0,-0x10(%ebp)`）；③ 'o'/'c' 分支改早返形（P16/P59：`if(local_10==0)return 0; Send...; return 0;`）；
//   ④ put_binary 实参回 data。残余=4.1 EH 状态标志 esi（`mov $0/%esi` + `test %esi;je;mov $0,%ebx`）在 4.4 无对应（N104/N7 族），语义等价。
int Inter_ReportMannerless::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ReportMannerlessPktView* pPkt = reinterpret_cast<Inter_ReportMannerlessPktView*>(data);
    if (pPkt == 0)
        return 0;

    CUser* local_10 = 0;
    if (user == 0 ||
        pPkt->m_e != user->get_charac_no(-1))
    {
        local_10 = G_CGameManager()->GetUserByAccId(
            pPkt->m_1e);
    }
    else
    {
        local_10 = user;
    }

    if (pPkt->m_15e == 'o')
    {
        if (local_10 == 0)
            return 0;
        local_10->SendCmdErrorPacket((ENUM_CMDPACKET)0x8c, 0x62);
        return 0;
    }
    else if (pPkt->m_15e == 'c')
    {
        if (local_10 == 0)
            return 0;
        local_10->SendCmdErrorPacket((ENUM_CMDPACKET)0x8c, 0x5f);
        return 0;
    }

    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "InterDispatcher.cpp", 0x326d), true);
        guard.operator*()->operator<<(0xca);
        guard.operator*()->operator<<(-1);
        guard.operator->()->put_binary(data, 0x15f);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);

        if (local_10 != 0)
        {
            PacketGuard packet;
            packet.put_header(1, 0x8c);
            packet.put_byte(1);
            packet.finalize(true);
            local_10->Send(packet);
        }
    }
    return 0;
}

Inter_ChagneUserHandicap::~Inter_ChagneUserHandicap() {}
// ORIG 0x084d271e T dispatch_sig —— 用户掉落率调整（handicap）。
// 报告 docs/class_func_reports/Inter_ChagneUserHandicap/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ChagneUserHandicapPktView —— Inter_ChagneUserHandicap::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ChagneUserHandicapPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
};
#pragma pack(pop)

int Inter_ChagneUserHandicap::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ChagneUserHandicapPktView* pPkt = reinterpret_cast<Inter_ChagneUserHandicapPktView*>(data);
    CUser* target = G_CGameManager()->GetUserByAccId(
        pPkt->m_a);
    /* [b28b P0/N96 清绑复测] ORIG 0x084d2745-084d276b 守卫 =「target==0 短路 je →
     * get_state; cmp $1; jg →mov$0；汇合 test %al,%al → je 早退」的 OR 直判形（bool 落
     * %al 寄存器、无独立槽）。P168a① 裸形复测：直接 `if (target == 0 || state <= 1) return 0;`
     * 即由 4.4 复现 mov$1/mov$0 + test %al 汇聚，与 ORIG 全等——存量 register bool asm("al")
     * 绑定违反 P0，已删；无需约束形。 */
    if (target == 0 || target->get_state() <= 1)
        return 0;
    if (pPkt->m_e == 1)
    {
        target->setStdDropRate(pPkt->m_12);
    }
    return 0;
}

Inter_GMRequestMid::~Inter_GMRequestMid() {}
// ORIG 0x084d279e T dispatch_sig —— GM mid 请求转发。
// 报告 docs/class_func_reports/Inter_GMRequestMid/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GMRequestMidPktView —— Inter_GMRequestMid::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GMRequestMidPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    int m_12;                 // +0x12
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_GMRequestMid::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // b28c-28（P29/P183 复验）：ORIG 0x084d27c4 段序为「早退岛之后」才
    // `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`（pPkt 初始化在 get_state
    // 判定之后），且 strlen 结果有**槽往返**
    // （0x084d283x：`call strlen; mov %eax,-0xc(%ebp); mov -0xc(%ebp),%eax`
    // → 后两点各 `mov -0xc(%ebp),%eax` 回读）⇒ 原码是具名长度局部、
    // strlen 只求值一次（P0：有往返即有具名局部）；旧「前置 dead gapPad
    // 推槽 + 两次 strlen」实为错位建模（多 1 条 call strlen + pPkt 槽差）。
    Inter_GMRequestMidPktView* pPkt = reinterpret_cast<Inter_GMRequestMidPktView*>(data);

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x7e);
    packet.put_int(pPkt->m_e);
    packet.put_int(pPkt->m_12);
    int nameLen = strlen(pPkt->m_16);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_16, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_UDP_Incoming_Capcha_Data::~Inter_UDP_Incoming_Capcha_Data() {}
// ORIG 0x084d28c0 T dispatch_sig —— 纯 ret 空函数。
// 报告 docs/class_func_reports/Inter_UDP_Incoming_Capcha_Data/dispatch_sig.md 全文转写（汇编 9 字节，mov $0x0,%eax; ret）。
int Inter_UDP_Incoming_Capcha_Data::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    return 0;
}

Inter_Add_Slang::~Inter_Add_Slang() {}
int Inter_Add_Slang::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    char* packet = data;
    std::string slang(packet + 0xa);
    CSyncSlangFilter::GetInstance()->AddSlang(slang);
    return 0;
}

Inter_ReplyGuildLetter::~Inter_ReplyGuildLetter() {}
// ORIG 0x084d28ca T dispatch_sig —— 公会信件回复应答转发。
// 报告 docs/class_func_reports/Inter_ReplyGuildLetter/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyGuildLetterPktView —— Inter_ReplyGuildLetter::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReplyGuildLetterPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    char m_12;                      // +0x12
};
#pragma pack(pop)

int Inter_ReplyGuildLetter::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_ReplyGuildLetterPktView* pPkt = reinterpret_cast<Inter_ReplyGuildLetterPktView*>(data);

    // ORIG：比较处 4 行 spill（`mov %eax,%edx`+`mov -0xc(%ebp),%eax`+`mov 0xa(%eax),%eax`
    // +`cmp %eax,%edx`）＝unsigned 字段→int 返回失配；log 处再独立装载（`mov 0xa(%eax),%ebx`）。
    if (user->get_charac_no(-1) != pPkt->m_a)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x3340,
            "MEMBER : Inter_ReplyGuildLetter::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_a);   // ORIG 一步 `mov 0xa(%eax),%ebx`（无 cast 免双级）
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x61);
    if (pPkt->m_12 == '\0')
        packet.put_byte(1);
    else
        packet.put_byte(0);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_CallGuildInviteToCaller::~Inter_CallGuildInviteToCaller() {}
// ORIG 0x084d2a26 T dispatch_sig —— 公会邀请回执：按结果码回执发起者。
// 报告 docs/class_func_reports/Inter_CallGuildInviteToCaller/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CallGuildInviteToCallerPktView —— Inter_CallGuildInviteToCaller::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_CallGuildInviteToCallerPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a（同上 unsigned→int 失配两步 spill 形态）
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

int Inter_CallGuildInviteToCaller::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_CallGuildInviteToCallerPktView* pPkt = reinterpret_cast<Inter_CallGuildInviteToCallerPktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_a)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x335c,
            "MEMBER : Inter_CallGuildInviteToCaller::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_a);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x9a);
    if (pPkt->m_12 == 0)
    {   // 结果码为零 → 成功
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_12);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_CallGuildInviteToInvited::~Inter_CallGuildInviteToInvited() {}
// ORIG 0x084d2b96 T dispatch_sig —— 被邀请方公会邀请应答转发（0x93 包 + 拒绝码上报）。
// 报告 docs/class_func_reports/Inter_CallGuildInviteToInvited/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CallGuildInviteToInvitedPktView —— Inter_CallGuildInviteToInvited::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_CallGuildInviteToInvitedPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    char m_30[1];             // +0x30 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=147（ORIG 0x084d2b96）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=29（ORIG 0x84d2b96）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_CallGuildInviteToInvited::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    // ORIG 0x084d2bbc：state 岛后 pPkt 才物化 -0x14（技巧13）。
    Inter_CallGuildInviteToInvitedPktView* pPkt = reinterpret_cast<Inter_CallGuildInviteToInvitedPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_a)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x337b,
            "MEMBER : Inter_CallGuildInviteToInvited::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_a);
        return 0;
    }

    if (user->alonePlay_Mode())
    {
        // b28c-l23 P0 清绑：ORIG 0x084d2c4c 把 mode 常量直落 %esi、characNo 直落
        // %ebx（跨链式 call 保活）；绑定违反 P0 已删，改普通具名局部（4.4 落槽
        // ⇒ N96/N18a，P168a④ 判词），语义不变。[b28c-l23 修正] 仅删 asm("esi"/"ebx")
        // 绑定，裸 register 保留（免槽直落形）。
        register unsigned char modeE = 2;
        register unsigned int characNo = user->getCurCharacNo();
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->SendReplyGuildInvite(characNo, modeE);
        return 0;
    }

    if (user->GetGameOptionRef()->get_etc_option((ENUM_OPTION_ETC)0x1e) != 0)
    {
        // b28c-l23 P0 清绑：同族第二份（ORIG 0x084d2cbd，slot -0x9）。
        register unsigned char modeD = 3;
        register unsigned int characNo = user->getCurCharacNo();
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->SendReplyGuildInvite(characNo, modeD);
    }

    PacketGuard packet;
    packet.put_header(0, 0x93);
    const int local_14 = (int)strlen(pPkt->m_30);
    packet.put_int(local_14);
    packet.put_str(pPkt->m_30, local_14);
    const int local_10 = (int)strlen(pPkt->m_12);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_12, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_ReplyGuildInviteToInvited::~Inter_ReplyGuildInviteToInvited() {}
// ORIG 0x084d2dfe T dispatch_sig —— 公会邀请回应：被邀者按结果码回执。
// 报告 docs/class_func_reports/Inter_ReplyGuildInviteToInvited/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyGuildInviteToInvitedPktView —— Inter_ReplyGuildInviteToInvited::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReplyGuildInviteToInvitedPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a（同上 unsigned→int 失配两步 spill 形态）
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

int Inter_ReplyGuildInviteToInvited::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_ReplyGuildInviteToInvitedPktView* pPkt = reinterpret_cast<Inter_ReplyGuildInviteToInvitedPktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_a)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x33ae,
            "MEMBER : Inter_CallGuildInviteToInvited::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_a);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x9b);
    if (pPkt->m_12 == 0)
    {   // 结果码为零 → 成功
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_int(pPkt->m_12);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_ReplyGuildInviteToCaller::~Inter_ReplyGuildInviteToCaller() {}
// ORIG 0x084d2f6e T dispatch_sig —— 公会邀请应答转发（SoloPlay 拒绝包 / 0x94 通知包）。
// 报告 docs/class_func_reports/Inter_ReplyGuildInviteToCaller/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyGuildInviteToCallerPktView —— Inter_ReplyGuildInviteToCaller::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReplyGuildInviteToCallerPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a（P254 枚型实测劣化 ⇒ 回退 int，N22 封顶）
    char m_pade[0x4];  // 未消费
    int m_12;                 // +0x12
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=127（ORIG 0x084d2f6e）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=19（ORIG 0x84d2f6e）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_ReplyGuildInviteToCaller::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    Inter_ReplyGuildInviteToCallerPktView* pPkt = reinterpret_cast<Inter_ReplyGuildInviteToCallerPktView*>(data);
    // ORIG 0x084d2f**：m_a/m_12 各处独立 reload（P48 无 CSE，无具名槽）。
    if (user->get_charac_no(-1) != pPkt->m_a)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x33cf,
            "MEMBER : Inter_ReplyGuildInviteToCaller::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_a);
        return 0;
    }

    if (pPkt->m_12 == 2)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            reinterpret_cast<char*>(pPkt) + 0x16,   // ORIG 0x084d2f8x 从 pPkt 槽 -0x10 取址
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(1),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(4));
        return 0;
    }
    if (pPkt->m_12 == 3)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            reinterpret_cast<char*>(pPkt) + 0x16,   // 同上（P0：ORIG 有具名槽往返）
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(0),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(4));
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x94);
    packet.put_byte(pPkt->m_12);
    // ORIG ja：unsigned 比较（signed jg 形不符）。
    if ((unsigned int)pPkt->m_12 < 2u)
    {
        const int msgLen = (int)strlen(pPkt->m_16);
        packet.put_int(msgLen);
        packet.put_str(pPkt->m_16, msgLen);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_LoadDeathTowerBestRecord::~Inter_LoadDeathTowerBestRecord() {}
// ORIG 0x084d316a T dispatch_sig —— 死亡之塔个人最优纪录批量登记。
// 报告 docs/class_func_reports/Inter_LoadDeathTowerBestRecord/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadDeathTowerBestRecordPktView —— Inter_LoadDeathTowerBestRecord::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadDeathTowerBestRecordPktView
{
    unsigned int m_0;         // +0x00
    unsigned int m_4;         // +0x04
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_LoadDeathTowerBestRecordRecWinView —— Inter_LoadDeathTowerBestRecord::dispatch_sig 窗口 rec 元素视图
//   （rec 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_LoadDeathTowerBestRecordRecWinView
{
    char m_pad0[0x10];  // 未消费
    unsigned char m_10;         // +0x10 §4.3 成员数组基址槽（stDeathTowerRecordMemberInfo_t*）
    char m_pad11[0x5b];  // 未消费
    int m_6c;                 // +0x6c
    unsigned int m_70;        // +0x70
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=88（ORIG 0x084d316a）：槽序/帧差(N82/P212/P221)+混合残差；gap+18。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=112（ORIG 0x84d316a）：N101 imul/lea 折叠+N18a 单段 regmove+N82/P112 帧槽平移。
// [BigParty/P331] 已实测本件 P331 第二形**不成立**（同 .o 复测）：删 rec + pWin_rec 两个
//   具名指针、宏化每次直取后 our 72→83（更贴 ORIG 90）但 ndiff **112→115 反劣** ⇒ 已回滚，
//   保留缓存形。同构建点基线读数：orig=90 our=72 ndiff=112。
// [b28d-solo2] P0 重判：ORIG 中 best(-0x30)/members(-0x2c) 落 spill 槽、count→%edi/accId→%esi/
//   stage→%ebx 走 callee-saved，getRankTable 返回值 eax 直压 (%esp) 无槽 ⇒ 原码是「单语句链式
//   调用 + 全内联实参」，无 rec/pWin_rec/best/count/accId/members/table 具名局部（5 对 store-reload
//   即 gap' 的 +5P 来源）。data 以 pPkt 具名槽（ORIG -0x24）作全部地址基址。
int Inter_LoadDeathTowerBestRecord::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadDeathTowerBestRecordPktView* pPkt = reinterpret_cast<Inter_LoadDeathTowerBestRecordPktView*>(data);
#define pRec (reinterpret_cast<Inter_LoadDeathTowerBestRecordRecWinView*>( \
    reinterpret_cast<char*>(pPkt) + i * 0x1b0 + j * 0x6c))
    for (unsigned int i = 0; i < pPkt->m_0; ++i)
    {
        for (int j = 0; j <= 3; ++j)
        {
            if (pRec->m_6c != 0)
            {
                ((WongWork::CDeathTowerRanking*)
                     WongWork::CDeathTowerRanking::getRankTable(j + 1))->registBestRecord(
                    (unsigned int)*reinterpret_cast<unsigned short*>(
                        reinterpret_cast<char*>(pPkt) + i * 0x1b0 + 0xc),
                    pPkt->m_4,
                    reinterpret_cast<const stDeathTowerRecordMemberInfo_t*>(&pRec->m_10),
                    pRec->m_6c,
                    pRec->m_70);
            }
        }
    }
#undef pRec
    return 0;
}

Inter_DeathTowerLoadTopRanker::~Inter_DeathTowerLoadTopRanker() {}
// ORIG 0x084d3364 T dispatch_sig —— 死亡之塔榜首记录批量装载。
// 报告 docs/class_func_reports/Inter_DeathTowerLoadTopRanker/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_DeathTowerLoadTopRankerPktView —— Inter_DeathTowerLoadTopRanker::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DeathTowerLoadTopRankerPktView
{
    unsigned int m_0;         // +0x00
    char m_4;                 // +0x04
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_DeathTowerLoadTopRankerRecWinView —— Inter_DeathTowerLoadTopRanker::dispatch_sig 窗口 rec 元素视图
//   （rec 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_DeathTowerLoadTopRankerRecWinView
{
    char m_pad0[0xfac];  // 未消费
    unsigned short m_fac;     // +0xfac
    unsigned short m_fae;     // +0xfae
    char m_padfb0[0x5c];  // 未消费
    unsigned int m_100c;      // +0x100c
    unsigned int m_1010;      // +0x1010
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=86（ORIG 0x084d3364）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+8。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=110（ORIG 0x84d3364）：N101 imul/lea 折叠+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_DeathTowerLoadTopRanker::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_DeathTowerLoadTopRankerPktView* pPkt = reinterpret_cast<Inter_DeathTowerLoadTopRankerPktView*>(data);
    if (pPkt->m_4 == '\0')
    {
        for (unsigned int t = 1; t <= 4; ++t)
        {
            WongWork::CDeathTowerRanking* table =
                (WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(t);
            table->resetRanking();
        }
    }

// §P183/P331：ORIG 每次字段访问都从 data 重物化 `imul $0x6c,%eax` + `add $0xfa0`
// （imul$0x6c 5 处 / add$0xfa0 3 处，我方原 1/0 —— 被 rec + pWin_rec 两个具名指针
// 吸收掉 5×(3~6) 条地址计算，our_insns 80 vs ORIG 88）。改宏每次直取后 our=89；
// i 为循环变量、循环体不改 i/data ⇒ 寻址结果逐次相同，无可观察行为变化。
#define pWin_rec \
    (reinterpret_cast<Inter_DeathTowerLoadTopRankerRecWinView*>(data + i * 0x6c))
    for (unsigned int i = 0; i < pPkt->m_0; ++i)
    {
        const unsigned int stage = pWin_rec->m_1010;
        const unsigned int playTime = pWin_rec->m_100c;
        const unsigned int key =
            *reinterpret_cast<unsigned short*>(data + 6 + i * 2);
        const unsigned int towerIdx = pWin_rec->m_fac;
        const unsigned int tableIdx = pWin_rec->m_fae;
        WongWork::CDeathTowerRanking* table =
            (WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(tableIdx);
        table->registRanker(
            towerIdx, key,
            reinterpret_cast<const stDeathTowerRecordMemberInfo_t*>(&pWin_rec->m_padfb0),   // §4.3 具名槽替代裸偏移 rec+0xfb0
            playTime, stage);
    }
#undef pWin_rec
    return 0;
}

Inter_MonitorDeathTowerRanking::~Inter_MonitorDeathTowerRanking() {}
// ORIG 0x084d3290 T dispatch_sig —— 死亡之塔排行登记。
// 报告 docs/class_func_reports/Inter_MonitorDeathTowerRanking/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=59（ORIG 0x084d3290）：槽序/帧差(N82/P212/P221)+混合残差；gap+12。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=78（ORIG 0x84d3290）：N82/P112 帧槽平移+P30/N4 add-vs-lex。
// [lane2 2026-09-10] 裸偏移指针算术 → 视图数组成员（`pPkt->m_ranks[i*4+j+4]`）复现
//   ORIG 084d32c3 `add $0x4,%edx; mov 0x2(%eax,%edx,4)`；补包基址具名局部（ORIG -0x24）。
//   残余 = 索引计算寄存器分配/label 顺序（gap -6，N18a/LRA 族）。
#pragma pack(push, 1)
// Inter_MonitorDeathTowerRankingPktView —— 排行表（ORIG 084d32c3 `add $0x4,%edx;
//   mov 0x2(%eax,%edx,4)` ⇒ base=msg、数组元素 @+0x02、索引 (i*4+j+4) 单条 scaled）。
struct Inter_MonitorDeathTowerRankingPktView
{
    char m_pad0[0x2];
    unsigned int m_ranks[20];
};
#pragma pack(pop)

int Inter_MonitorDeathTowerRanking::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 084d3331/084d3320：循环界 `cmp $0x4/%eax; setbe` ⇒ 原码 unsigned `<=` 形。
    // ORIG 084d32c6/084d32e0：条件与 registRanking 第 3 实参各自重物化取值（value 无槽，
    //   留 %edi 跨 getCurCharacNo call）⇒ §P0 内联，不设具名 value。
    // ORIG 084d3299 `mov 0x10(%ebp),%eax; mov %eax,-0x24` ⇒ 包基址具名局部。
    Inter_MonitorDeathTowerRankingPktView* pPkt =
        reinterpret_cast<Inter_MonitorDeathTowerRankingPktView*>(data);
    for (unsigned int i = 0; i <= 4; ++i)
    {
        for (unsigned int j = 0; j <= 3; ++j)
        {
            if (pPkt->m_ranks[i * 4 + j + 4] != 0)
            {
                static_cast<WongWork::CDeathTowerRanking*>(
                    WongWork::CDeathTowerRanking::getRankTable(j + 1))
                    ->registRanking(
                        i, user->getCurCharacNo(),
                        pPkt->m_ranks[i * 4 + j + 4]);
            }
        }
    }
    return 0;
}

Inter_MonitorTowerReloadTopRanker::~Inter_MonitorTowerReloadTopRanker() {}
// ORIG 0x084d334a T dispatch_sig —— 触发死亡塔排行榜重载 DB 请求。
// 报告 docs/class_func_reports/Inter_MonitorTowerReloadTopRanker/dispatch_sig.md 全文转写。
int Inter_MonitorTowerReloadTopRanker::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    DB_DeathTowerLoadTopRanker::makeRequest(true);
    return 0;
}

Inter_ReplyGuildSecede::~Inter_ReplyGuildSecede() {}
// ORIG 0x084c9768 T dispatch_sig —— 退出公会应答：校验角色号后发 0x9c 包并刷 item lock 列表。
// 报告 docs/class_func_reports/Inter_ReplyGuildSecede/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyGuildSecedePktView —— Inter_ReplyGuildSecede::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReplyGuildSecedePktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;         // +0x0a
    short m_e;                // +0x0e
    int m_10;                 // +0x10（P254 枚型实测劣化 ⇒ 回退 int，N22 封顶）
    char m_pad14[0x4];  // 未消费
    int m_18;                 // +0x18
    signed char m_1c;         // +0x1c
    char m_1d[1];             // +0x1d 串/字节区首元素（容量未建模）
    char m_pad1e[0x1d];  // 未消费
    char m_3b[1];             // +0x3b 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=132（ORIG 0x084c9768）：寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=24（ORIG 0x84c9768）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_ReplyGuildSecede::dispatch_sig(CUser* user, char* data, int len)
{
    // ORIG 无 len 槽（Ghidra local_28 系 PG 内部视图误标）；pPkt 入口即物化 -0x24。
    if (user->get_state() == 0)
        return 0;

    Inter_ReplyGuildSecedePktView* pPkt = reinterpret_cast<Inter_ReplyGuildSecedePktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_10)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x236b,
            "Inter_ReplyGuildSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_10);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x9c);
    if (pPkt->m_18 != 0)
    {   // 退会失败（原因码）
        packet.put_byte(0);
        packet.put_byte(pPkt->m_18);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {   // 退会成功
        packet.put_byte(1);
        packet.put_byte(pPkt->m_1c);
        const int local_24 = (int)strlen(pPkt->m_3b);
        packet.put_int(local_24);
        packet.put_str(pPkt->m_3b, local_24);
        const int local_20 = (int)strlen(pPkt->m_1d);
        packet.put_int(local_20);
        packet.put_str(pPkt->m_1d, local_20);
        packet.finalize(true);
        user->Send(packet);
        user->m_historyLog.GuildOut(
            pPkt->m_a,
            data + 0x3b,
            pPkt->m_10,
            pPkt->m_1c,
            static_cast<unsigned short>(
                pPkt->m_e - 1));
    }

    user->getCurCharacInvenR()->SendItemLockListInven();
    user->getCurCharacInvenR()->SendItemLockListAvatar();
    user->getCurCharacCargoR()->SendItemLockList();
    user->getCurCharacInvenR()->SendItemLockListCreature();
    return 0;
}

Inter_NoticeGuildMasterDelegate::~Inter_NoticeGuildMasterDelegate() {}
// ORIG 0x084c9a32 T dispatch_sig —— 会长委任通知：转发新会长名字。
// 报告 docs/class_func_reports/Inter_NoticeGuildMasterDelegate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeGuildMasterDelegatePktView —— Inter_NoticeGuildMasterDelegate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticeGuildMasterDelegatePktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;       // +0x0a（同上 unsigned→int 失配两步 spill 形态）
    char m_pade[0x4];  // 未消费
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_NoticeGuildMasterDelegate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_NoticeGuildMasterDelegatePktView* pPkt = reinterpret_cast<Inter_NoticeGuildMasterDelegatePktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_a)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x23a0,
            "Inter_NoticeGuildMasterDelegate::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_a);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x95);
    const int nameLen = strlen(pPkt->m_12);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_12, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_GuildReplyGuildMasterDelegate::~Inter_GuildReplyGuildMasterDelegate() {}
// ORIG 0x084c9b9a T dispatch_sig —— 会长委任回应：按结果码回执。
// 报告 docs/class_func_reports/Inter_GuildReplyGuildMasterDelegate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildReplyGuildMasterDelegatePktView —— Inter_GuildReplyGuildMasterDelegate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GuildReplyGuildMasterDelegatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a（同上 unsigned→int 失配两步 spill 形态）
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

int Inter_GuildReplyGuildMasterDelegate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_GuildReplyGuildMasterDelegatePktView* pPkt = reinterpret_cast<Inter_GuildReplyGuildMasterDelegatePktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_a)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x23ba,
            "Inter_NoticeGuildMasterDelegate::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_a);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x9e);
    if (pPkt->m_12 == 0)
    {   // 结果码为零 → 成功
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_12);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_NoticeHasBeenGuildMaster::~Inter_NoticeHasBeenGuildMaster() {}
// ORIG 0x084c9d0a T dispatch_sig —— 已成为公会会长通知。
// 报告 docs/class_func_reports/Inter_NoticeHasBeenGuildMaster/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeHasBeenGuildMasterPktView —— Inter_NoticeHasBeenGuildMaster::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeHasBeenGuildMasterPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
};
#pragma pack(pop)

int Inter_NoticeHasBeenGuildMaster::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_NoticeHasBeenGuildMasterPktView* pPkt = reinterpret_cast<Inter_NoticeHasBeenGuildMasterPktView*>(data);

    // ORIG：同 ReplyGuildLetter（unsigned 字段失配 4 行 spill + log 独立装载）。
    if (user->get_charac_no(-1) != pPkt->m_a)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x23da,
            "Inter_NoticeHasBeenGuildMaster::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_a);   // ORIG 一步装载，同上
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x96);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_DBCheckDoubleGuildName::~Inter_DBCheckDoubleGuildName() {}
// ORIG 0x084d347e T dispatch_sig —— 公会名重复检查应答。
// 报告 docs/class_func_reports/Inter_DBCheckDoubleGuildName/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_DBCheckDoubleGuildNamePktView —— Inter_DBCheckDoubleGuildName::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DBCheckDoubleGuildNamePktView
{
    char m_0;                 // +0x00
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=11（ORIG 0x084d347e）：块序重排(N104/P223)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=6（ORIG 0x84d347e）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_DBCheckDoubleGuildName::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_DBCheckDoubleGuildNamePktView* pPkt = reinterpret_cast<Inter_DBCheckDoubleGuildNamePktView*>(data);

    // ORIG 0x084d3546：err 支尾无 `mov $0x0,%eax`（直接 jmp 汇合尾 0x084d3581）
    // ⇒ err 支不带 return；正常支 dtor 内联块尾（0x084d355f lea/mov/call D1Ev，
    // 0x084d3564 jmp 汇合）⇒ PacketGuard 直接声明于 else 块（无额外 {}，
    // 额外作用域会使 gcc 把 dtor hoist 到汇合点共享尾块，产生行序差）。
    if (pPkt->m_0 == '\x02')
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9f, 0x6b);
    }
    else
    {
        user->setGuildCreateFlag(2);
        PacketGuard packet;
        packet.put_header(1, 0x9f);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
        user->setGuildCreateFlag(GUILD_ADDRESS_DOUBLE_CHECK);
    }
    return 0;
}

Inter_DBCheckDoubleGuildAddress::~Inter_DBCheckDoubleGuildAddress() {}
// ORIG 0x084d358e T dispatch_sig —— 公会地址重复检查应答。
// 报告 docs/class_func_reports/Inter_DBCheckDoubleGuildAddress/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_DBCheckDoubleGuildAddressPktView —— Inter_DBCheckDoubleGuildAddress::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DBCheckDoubleGuildAddressPktView
{
    char m_0;                 // +0x00
};
#pragma pack(pop)

int Inter_DBCheckDoubleGuildAddress::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_DBCheckDoubleGuildAddressPktView* pPkt = reinterpret_cast<Inter_DBCheckDoubleGuildAddressPktView*>(data);

    if (pPkt->m_0 == '\x02')
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa0, 0x6f);
    }
    // ORIG：同 DBCheckDoubleGuildName 形态（else 支内 PacketGuard，汇合后统一 return）。
    else
    {
        user->setGuildCreateFlag(GUILD_ADDRESS_DOUBLE_CHECK);
        PacketGuard packet;
        packet.put_header(1, 0xa0);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_ReplyGuildCreate::~Inter_ReplyGuildCreate() {}
// ORIG 0x084d368a T dispatch_sig —— 建会应答：成功 → GuildCreate 日志 + 公会代理缓存 +
// 全服基础信息广播；失败 → 退还建会费用并刷新物品列表。
// 报告 docs/class_func_reports/Inter_ReplyGuildCreate/dispatch_sig.md 全文转写。
// msg 布局：+0xa charID(int) +0x12 result(int) +0x16 公会名(0x16)。
#pragma pack(push, 1)
// Inter_ReplyGuildCreatePktView —— Inter_ReplyGuildCreate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReplyGuildCreatePktView
{
    char m_pad0[0xa];  // 未消费
    unsigned int m_a;       // +0x0a（同上 unsigned→int 失配两步 spill 形态）
    char m_pade[0x4];  // 未消费
    int m_result;             // +0x12
};
#pragma pack(pop)

int Inter_ReplyGuildCreate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ReplyGuildCreatePktView* pPkt = reinterpret_cast<Inter_ReplyGuildCreatePktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x3464,
            "GUILD : Inter_ReplyGuildCreate:: recv->m_uCharID(%d)",
            pPkt->m_a);
        return 0;
    }

    if (user->get_charac_no(-1) != pPkt->m_a)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x346a,
            "GUILD : Inter_ReplyGuildCreate:: : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_a);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x47);
    if (pPkt->m_result == 0)
    {   // 建会成功
        packet.put_byte(1);
        user->m_historyLog.GuildCreate(
            (unsigned int)user->get_charac_guildkey(),
            reinterpret_cast<char*>(pPkt) + 0x16,   // ORIG 经 -0x10 槽
            pPkt->m_a);
    }
    else
    {   // 建会失败：退还建会费用
        packet.put_byte(0);
        packet.put_byte(pPkt->m_result);
        int createCost = G_CDataManager()->m_guildParameter.m_field0;   // +0xa50c（建会费用）
        user->getCurCharacInvenW()->gain_money(createCost,
                                               (eMoneyAddReason)0x19, true, 0);
        user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
    }
    packet.finalize(true);
    user->Send(packet);

    if (pPkt->m_result == 0)
    {   // 公会代理缓存 + 全服广播
        STGuildProxy proxy;
        proxy.m_level = 0;
        memcpy(proxy.m_guildName, reinterpret_cast<char*>(pPkt) + 0x16, 0x16);  // ORIG 经 -0x10 槽
        proxy.m_powerSide = 0;
        user->SetGuildInfoProxy(proxy);

        packet.clear();                                // ORIG InterfacePacketBuf::clear
        packet.put_header(0, 2);
        packet.put_byte(0);
        packet.put_short(1);
        user->make_basic_info((char*)&packet, 0);
        packet.finalize(true);
        G_GameWorld()->send_all(packet, user);
    }
    return 0;
}

Inter_NoticeGuildNameOnGuildCreate::~Inter_NoticeGuildNameOnGuildCreate() {}
// ORIG 0x084d39ae T dispatch_sig —— 公会创建时公会名登记到代理。
// 报告 docs/class_func_reports/Inter_NoticeGuildNameOnGuildCreate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeGuildNameOnGuildCreatePktView —— Inter_NoticeGuildNameOnGuildCreate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeGuildNameOnGuildCreatePktView
{
    char m_pad0[0x12];  // +0x0..未消费
    unsigned int m_12;              // +0x12
    char m_pad16[0x17];  // +0x16..未消费
    unsigned char m_2d;             // +0x2d
};
#pragma pack(pop)

int Inter_NoticeGuildNameOnGuildCreate::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticeGuildNameOnGuildCreatePktView* pPkt = reinterpret_cast<Inter_NoticeGuildNameOnGuildCreatePktView*>(data);
    STGuildProxy proxy;
    memcpy(&proxy, pPkt->m_pad16, 0x16);   // §4.3 具名数组基址替代裸偏移 (char*)pPkt+0x16
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)pPkt->m_2d
        )->InsertGuildInfoProxy(
            pPkt->m_12, proxy);
    return 0;
}

Inter_SecurityCardIssue::~Inter_SecurityCardIssue() {}
// ORIG 0x084d3a22 T dispatch_sig —— 安全卡发放：加密序列号/账号/答案串并下发 0xa4 包，
// 本地初始化 CSecurityCard。msg 布局：+4 serial +0x25 answers +0x37 serial 原文
// +0x5e answers 原文 +0x8c 错误码(int)。
// 报告 docs/class_func_reports/Inter_SecurityCardIssue/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SecurityCardIssuePktView —— Inter_SecurityCardIssue::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SecurityCardIssuePktView
{
    char m_pad0[0x8c];  // 未消费（N：+0x04/+0x25/+0x37/+0x5e 串区基址——拆 pad 建具名槽使 setSecurityCardInfo 多指针参装载重排、破坏 AE，保留 reinterpret_cast）
    int m_8c;                 // +0x8c
};
#pragma pack(pop)

int Inter_SecurityCardIssue::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x1c(%ebp)
    Inter_SecurityCardIssuePktView* pPkt = reinterpret_cast<Inter_SecurityCardIssuePktView*>(data);
    if (pPkt->m_8c == 0)
    {
        char encSerial[0x42] = { '\0' };               // 密文 serial
        char idStr[0x30] = { '\0' };                   // 账号串
        char encAnswers[0x42];                         // 密文 answers（显式 memset）

        OS_API::snprintf(idStr, 0x30, "%s",
                         NumberToString((unsigned int)user->get_acc_id(), 0));
        GlobalData::s_securityCardCenter->encryptString(
            reinterpret_cast<char*>(pPkt) + 4, encSerial);   // ORIG 经 -0x1c 槽
        GlobalData::s_securityCardCenter->encryptString(idStr, idStr);
        memset(encAnswers, 0, 0x42);
        GlobalData::s_securityCardCenter->encryptString(
            reinterpret_cast<char*>(pPkt) + 0x25, encAnswers);  // ORIG 经 -0x1c 槽

        PacketGuard packet;
        packet.put_header(1, 0xa4);
        packet.put_byte(1);
        packet.put_int(strlen(idStr));
        packet.put_str(idStr, strlen(idStr));
        packet.put_int(strlen(encSerial));
        packet.put_str(encSerial, strlen(encSerial));
        packet.put_int(strlen(encAnswers));
        packet.put_str(encAnswers, strlen(encAnswers));
        packet.finalize(true);
        user->Send(packet);

        user->getSecurityCard()->init();
        user->getSecurityCard()->setSecurityCardInfo(
            reinterpret_cast<char*>(pPkt) + 0x37, false, false, 0, 0, 0,
            reinterpret_cast<char*>(pPkt) + 0x5e);    // ORIG 经 -0x1c 槽
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa4,
                                 (unsigned char)pPkt->m_8c);
    }
    return 0;
}

Inter_SecurityCardActivate::~Inter_SecurityCardActivate() {}
// ORIG 0x084d3d56 T dispatch_sig —— 安全卡激活应答。
// 报告 docs/class_func_reports/Inter_SecurityCardActivate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SecurityCardActivatePktView —— Inter_SecurityCardActivate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SecurityCardActivatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    char m_a;                       // +0x0a
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=70（ORIG 0x084d3d56）：寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=30（ORIG 0x84d3d56）：P264 .o 重定位/立即数。
int Inter_SecurityCardActivate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SecurityCardActivatePktView* pPkt = reinterpret_cast<Inter_SecurityCardActivatePktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    if (pPkt->m_a == '\0')
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa8, 1);
        return 0;
    }

    user->getSecurityCard()->setActivate(true);
    user->getSecurityCard()->setCertFlag('\x01');

    PacketGuard packet;
    packet.put_header(1, 0xa8);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_SecurityCardRetransfer::~Inter_SecurityCardRetransfer() {}
// ORIG 0x084d3e78 T dispatch_sig —— 安全卡补发：加密串下发 0xca 包并重置 CSecurityCard。
// msg 布局：+4 serial +0x2b serial 原文 +0x52 answers +0x88 错误码(int)。
// 报告 docs/class_func_reports/Inter_SecurityCardRetransfer/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SecurityCardRetransferPktView —— Inter_SecurityCardRetransfer::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SecurityCardRetransferPktView
{
    char m_pad0[0x88];  // 未消费
    int m_88;                 // +0x88
};
#pragma pack(pop)

int Inter_SecurityCardRetransfer::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // ORIG 0x084d44xx：包基址在 get_state 检查后物化入 -0x1c(%ebp)
    Inter_SecurityCardRetransferPktView* pPkt = reinterpret_cast<Inter_SecurityCardRetransferPktView*>(data);
    if (pPkt->m_88 == 0)
    {
        char encSerial[0x42] = { '\0' };               // 密文 serial
        char idStr[0x30] = { '\0' };                   // 账号串
        char encAnswers[0x42];                         // 密文 answers（显式 memset）

        OS_API::snprintf(idStr, 0x30, "%s",
                         NumberToString((unsigned int)user->get_acc_id(), 0));
        GlobalData::s_securityCardCenter->encryptString(
            reinterpret_cast<char*>(pPkt) + 4, encSerial);   // ORIG 经 -0x1c 槽
        GlobalData::s_securityCardCenter->encryptString(idStr, idStr);
        memset(encAnswers, 0, 0x42);
        GlobalData::s_securityCardCenter->encryptString(
            reinterpret_cast<char*>(pPkt) + 0x52, encAnswers);  // ORIG 经 -0x1c 槽

        PacketGuard packet;
        packet.put_header(1, 0xca);
        packet.put_byte(1);
        packet.put_int(strlen(idStr));
        packet.put_str(idStr, strlen(idStr));
        packet.put_int(strlen(encSerial));
        packet.put_str(encSerial, strlen(encSerial));
        packet.put_int(strlen(encAnswers));
        packet.put_str(encAnswers, strlen(encAnswers));
        packet.finalize(true);
        user->Send(packet);

        user->getSecurityCard()->init();
        user->getSecurityCard()->setSecurityCardInfo(
            reinterpret_cast<char*>(pPkt) + 0x2b, true, false, 0, 0, 0,
            (char*)0);                                   // ORIG 经 -0x1c 槽
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xca,
                                 (unsigned char)pPkt->m_88);
    }
    return 0;
}

Inter_LoadDnfHackLog::~Inter_LoadDnfHackLog() {}
// ORIG 0x084d41aa T dispatch_sig —— DNF hack 日志累计计数同步。
// 报告 docs/class_func_reports/Inter_LoadDnfHackLog/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadDnfHackLogPktView —— Inter_LoadDnfHackLog::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadDnfHackLogPktView
{
    int m_0;                        // +0x00
    unsigned int m_4[1];            // +0x04 累计计数表（ORIG `mov 0x4(%eax,%edx,4)` scaled-index）
};
#pragma pack(pop)

int Inter_LoadDnfHackLog::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadDnfHackLogPktView* pPkt = reinterpret_cast<Inter_LoadDnfHackLogPktView*>(data);
    // ORIG +0x27：`setne`+`mov $0x0,%eax`+`jmp` ⇒ 不等时早退（== 写法会翻转条件码）。
    if (user->get_acc_id() != pPkt->m_0)
        return 0;
    for (int i = 0x65; i <= 0xfa0; ++i)
    {
        user->getHackAnalyzer()->setServerHackAccumulatedCnt(
            (WongWork::ENUM_HACKTYPE)i,
            pPkt->m_4[i - 0x65]);
    }
    return 0;
}

Inter_AutoPunishRuleBlackIP::~Inter_AutoPunishRuleBlackIP() {}
// ORIG 0x084d422a T dispatch_sig —— 黑名单 IP 表载入（htonl/inet_ntoa + addBlackIP）。
// 报告 docs/class_func_reports/Inter_AutoPunishRuleBlackIP/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AutoPunishRuleBlackIPPktView —— Inter_AutoPunishRuleBlackIP::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AutoPunishRuleBlackIPPktView
{
    unsigned short m_0;       // +0x00
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=128（ORIG 0x084d422a）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-15。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=117（ORIG 0x84d422a）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_AutoPunishRuleBlackIP::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_AutoPunishRuleBlackIPPktView* pPkt = reinterpret_cast<Inter_AutoPunishRuleBlackIPPktView*>(data);
    GlobalData::g_autoPunishRuleBlackIPMgr->reset();

    const unsigned short totalCount =                 // msg+0x0
        (unsigned short)pPkt->m_0;
    cMyTrace(__PRETTY_FUNCTION__,
        0x3561, 0)("Black IP Load Start (TotalCount: %d)", (int)totalCount);

    for (unsigned int i = 0; i < totalCount; ++i)     // cmp; seta
    {
        char ipStr[16] = { '\0' };
        unsigned int addrPart =                       // msg+i*8+4
            *reinterpret_cast<unsigned int*>(data + i * 8 + 4);
        unsigned int netIp = htonl(
            (unsigned int)*reinterpret_cast<unsigned char*>(data + i * 8 + 8));
        strncpy(ipStr, inet_ntoa(
                    *reinterpret_cast<in_addr*>(&(netIp |= addrPart))), 0x10);

        const bool added =
            GlobalData::g_autoPunishRuleBlackIPMgr->addBlackIP(
                (unsigned long)addrPart,
                *reinterpret_cast<unsigned char*>(data + i * 8 + 8),
                *reinterpret_cast<unsigned char*>(data + i * 8 + 9));
        if (added != 1)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x356d,
                "Black IP Load fail(StartIP:%s -> %d)", ipStr,
                (int)*reinterpret_cast<unsigned char*>(data + i * 8 + 9));
        }
        const unsigned char maxIp =
            *reinterpret_cast<unsigned char*>(data + i * 8 + 9);
        cMyTrace(__PRETTY_FUNCTION__,
            0x356e, 0)("Black IP Load (StartIP:%s -> %d)", ipStr, (int)maxIp);
    }

    const int finalCount =
        GlobalData::g_autoPunishRuleBlackIPMgr->getBlackIPCount();
    cMyTrace(__PRETTY_FUNCTION__,
        0x3570, 0)("Black IP Load End (TotalCount: %d)", finalCount);
    return 0;
}

Inter_AutoPunishRuleHackType::~Inter_AutoPunishRuleHackType() {}
// ORIG 0x084d4440 T dispatch_sig —— 从 monitor 消息装载外挂自动惩罚规则表（HackType 规则）。
// 报告 docs/class_func_reports/Inter_AutoPunishRuleHackType/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AutoPunishRuleHackTypePktView —— Inter_AutoPunishRuleHackType::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AutoPunishRuleHackTypePktView
{
    unsigned int m_0;         // +0x00
    unsigned long long m_4;   // +0x04
    unsigned short m_c;       // +0x0c
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_AutoPunishRuleHackTypeEntryWinView —— Inter_AutoPunishRuleHackType::dispatch_sig 窗口 entry 元素视图
//   （entry 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_AutoPunishRuleHackTypeEntryWinView
{
    char m_pad0[0x10];  // 未消费
    unsigned short m_10;      // +0x10
    unsigned short m_12;      // +0x12
    unsigned short m_14;      // +0x14
    unsigned short m_16;      // +0x16
    unsigned short m_18;      // +0x18
    char m_pad1a[0x2];  // 未消费
    unsigned int m_1c;        // +0x1c
    unsigned int m_20;        // +0x20
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=202（ORIG 0x084d4440）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+68。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=234（ORIG 0x84d4440）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_AutoPunishRuleHackType::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    GlobalData::g_autoPunishRuleHackTypeMgr->reset();
    Inter_AutoPunishRuleHackTypePktView* pPkt = reinterpret_cast<Inter_AutoPunishRuleHackTypePktView*>(data);
    GlobalData::g_autoPunishRuleHackTypeMgr->setTotalTrade(pPkt->m_0,
                       pPkt->m_4);

    // P328⑩/P227：ORIG 0x84d4484..0x84d44b0 先求 m_0→%edi、m_4 低→%ebx/高→%esi，
    // 其后才 cMyTrace ctor ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x3585, 0)(
        "HackType Load Start. (TotalGold: %llu)(TotalCount: %d)",
        pPkt->m_4,
        pPkt->m_0);

    for (int local_20 = 0;
         local_20 < (int)(unsigned int)(unsigned short)pPkt->m_c;
         ++local_20)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->setHackCnt(
            (WongWork::ENUM_HACKTYPE)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_10,
            ((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_12,
            (WongWork::ENUM_HACKTYPE)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_14,
            ((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_16,
            ((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_18,
            ((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_1c,
            ((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_20);
        // ORIG 0x84d45db..0x84d466f 先求 +0x16→-0x5c、+0x14→%edi、+0x12→%esi、+0x10→%ebx 再 ctor。
        cMyTrace(__PRETTY_FUNCTION__, 0x3596, 0)(
            "HackType Load (Type: %d -> %d)(SubType: %d -> %d)",
            (unsigned int)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_10,
            (unsigned int)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_12,
            (unsigned int)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_14,
            (unsigned int)((Inter_AutoPunishRuleHackTypeEntryWinView*)((char*)pPkt + local_20 * 0x14))->m_16);
    }

    cMyTrace(__PRETTY_FUNCTION__,
        0x3599, 0)("HackType Load End");
    return 0;
}

Inter_ReplyPowerWarScore::~Inter_ReplyPowerWarScore() {}
// ORIG 0x084d46fa T dispatch_sig —— 力量战争得分全网广播。
// 报告 docs/class_func_reports/Inter_ReplyPowerWarScore/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyPowerWarScorePktView —— Inter_ReplyPowerWarScore::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReplyPowerWarScorePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    int m_e;                        // +0x0e
};
#pragma pack(pop)

int Inter_ReplyPowerWarScore::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ReplyPowerWarScorePktView* pPkt = reinterpret_cast<Inter_ReplyPowerWarScorePktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x9b);
    packet.put_int(pPkt->m_a);
    packet.put_int(pPkt->m_e);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    GlobalData::s_power_manager->SetPowerWarPoint(
        pPkt->m_a,
        pPkt->m_e);
    return 0;
}

Inter_ReplyJoinPower::~Inter_ReplyJoinPower() {}
// ORIG 0x084d47d4 T dispatch_sig —— 加入势力应答（0xb4 包 + 公会代理刷新）。
// 报告 docs/class_func_reports/Inter_ReplyJoinPower/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReplyJoinPowerPktView —— Inter_ReplyJoinPower::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReplyJoinPowerPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=124（ORIG 0x084d47d4）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=81（ORIG 0x84d47d4）：N22 cmp 操作数方向+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_ReplyJoinPower::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ReplyJoinPowerPktView* pPkt = reinterpret_cast<Inter_ReplyJoinPowerPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // [b28d-lane2] ORIG 084d47f4 段：比较为内联形（`call get_charac_no; mov -0x10(%ebp),%eax;
    // mov 0x12(%eax),%eax; cmp %eax,%edx`），logFormat 两处亦逐点重读 ⇒ 原码无 characNo
    // 具名局部（P0：无槽往返），旧具名形多发 store+load 各一。
    if (user->get_charac_no(-1) != pPkt->m_12)         // msg+0x12
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x35fc,
            "Inter_ReplyJoinPower::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharNo(%d)",
            user->get_charac_no(-1), pPkt->m_12);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0xb4);
    if (pPkt->m_a == 0)
    {
        packet.put_byte(1);
        packet.put_byte(
            (int)pPkt->m_16);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_a);
    }
    packet.finalize(true);
    user->Send(packet);

    {   // STGuildProxy(GetGuildDBInfo(), 名随写字节+0x1b, msg+0x16)
        const char* guildDbInfo =
            static_cast<const char*>(user->GetGuildDBInfo());
        STGuildProxy proxy(
            guildDbInfo,
            *(reinterpret_cast<const unsigned char*>(guildDbInfo) + 0x1b),
            pPkt->m_16);
        CGuildServerProxy* proxySrv =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxySrv->ModifyGuildInfoProxy(
            (unsigned int)user->get_charac_guildkey(), proxy);
    }
    return 0;
}

Inter_SecedePower::~Inter_SecedePower() {}
// ORIG 0x084d49d6 T dispatch_sig —— 退出势力应答（0xb5 包 + 公会代理刷新，name 置空）。
// 报告 docs/class_func_reports/Inter_SecedePower/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SecedePowerPktView —— Inter_SecedePower::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SecedePowerPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=110（ORIG 0x084d49d6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84d49d6）：本轮 GetGuildDBInfo 单次调用+具名缓存（p167 CLEAR）；余 N22/N18a/N82。
int Inter_SecedePower::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SecedePowerPktView* pPkt = reinterpret_cast<Inter_SecedePowerPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // [b28d-lane2] ORIG 084d49f6 段：比较内联 + logFormat 逐点重读 ⇒ 原码无 characNo 具名
    // 局部（P0：无槽往返），旧具名形多发 store+load 各一。
    if (user->get_charac_no(-1) != pPkt->m_12)         // msg+0x12
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x3620,
            "Inter_SecedePower::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharNo(%d)",
            user->get_charac_no(-1), pPkt->m_12);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0xb5);
    if (pPkt->m_a == 0)
    {
        packet.put_byte(1);
        packet.put_byte(
            (int)pPkt->m_16);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_a);
    }
    packet.finalize(true);
    user->Send(packet);

    {   // STGuildProxy(GetGuildDBInfo(), 名随写字节+0x1b, 0)
        // ORIG 0x84d4b2f-0x84d4b41：单次调用 + `mov %eax,-0xc` 槽往返 ⇒ 具名局部（p167 E=GetGuildDBInfo×1）。
        const char* guildDbInfo = static_cast<const char*>(user->GetGuildDBInfo());
        STGuildProxy proxy(
            guildDbInfo,
            *(reinterpret_cast<const unsigned char*>(guildDbInfo) + 0x1b),
            (unsigned char)0);
        CGuildServerProxy* proxySrv =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxySrv->ModifyGuildInfoProxy(
            (unsigned int)user->get_charac_guildkey(), proxy);
    }
    return 0;
}

Inter_NoticePowerWarStartTime::~Inter_NoticePowerWarStartTime() {}
// ORIG 0x084d4bd2 T dispatch_sig —— 权力战开始时间：设置时间并全员广播。
// 报告 docs/class_func_reports/Inter_NoticePowerWarStartTime/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePowerWarStartTimePktView —— Inter_NoticePowerWarStartTime::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarStartTimePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
    unsigned char m_b;              // +0x0b
    unsigned char m_c;              // +0x0c
    unsigned char m_d;              // +0x0d
};
#pragma pack(pop)

int Inter_NoticePowerWarStartTime::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarStartTimePktView* pPkt = reinterpret_cast<Inter_NoticePowerWarStartTimePktView*>(data);

    // CEnvironment+0x1a8 环境槽未在权威头建模（沿 CUserOnlyOrig.cpp:9027 先例，
    // §4.3 例外保留地址形式；ORIG 两次独立 G_CEnvironment 调用，P48）
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;

    GlobalData::s_power_manager->SetPowerWarStartTime(
        (unsigned char)pPkt->m_a,
        (unsigned char)pPkt->m_b,
        (unsigned char)pPkt->m_c,
        (unsigned char)pPkt->m_d);

    PacketGuard packet;
    packet.put_header(0, 0xa1);
    packet.put_byte((unsigned int)pPkt->m_a);
    packet.put_byte((unsigned int)pPkt->m_b);
    packet.put_byte((unsigned int)pPkt->m_c);
    packet.put_byte((unsigned int)pPkt->m_d);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_NoticePowerWarEndTime::~Inter_NoticePowerWarEndTime() {}
// ORIG 0x084d4d46 T dispatch_sig —— 力量战争结束时间全网广播。
// 报告 docs/class_func_reports/Inter_NoticePowerWarEndTime/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePowerWarEndTimePktView —— Inter_NoticePowerWarEndTime::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarEndTimePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
    int m_b;                        // +0x0b
    int m_f;                        // +0x0f
};
#pragma pack(pop)

int Inter_NoticePowerWarEndTime::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarEndTimePktView* pPkt = reinterpret_cast<Inter_NoticePowerWarEndTimePktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0xa2);
    packet.put_byte(pPkt->m_a);
    packet.put_int(pPkt->m_b);
    packet.put_int(pPkt->m_f);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_NoticePowerWarInfo::~Inter_NoticePowerWarInfo() {}
// ORIG 0x084d4e18 T dispatch_sig —— 权力战信息通知全员广播。
// 报告 docs/class_func_reports/Inter_NoticePowerWarInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePowerWarInfoPktView —— Inter_NoticePowerWarInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    int m_e;                        // +0x0e
    signed char m_12;               // +0x12
};
#pragma pack(pop)

// [b28c-31] AE：两守卫改正向早退形（P16/P59），86=86 仅剩重定位显示差。
int Inter_NoticePowerWarInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarInfoPktView* pPkt = reinterpret_cast<Inter_NoticePowerWarInfoPktView*>(data);
    GlobalData::s_power_manager->SetPowerInfo(
        (char)pPkt->m_12,
        pPkt->m_a,
        pPkt->m_e);

    // [b28c-31] ORIG 0x084d4e70/0x084d4e90：两守卫均为正向早退
    // （`test %al,%al; je <继续>` / `cmp $1;je <true>; cmp $2;jne <false>`），
    // 4.4 对 `if(!f()){...}` 嵌套产 xor$1 反形 ⇒ 改写正向 return 0 形（P16/P59）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    // ORIG 0x84d4e58：G_CEnvironment() 短路双调（O0 不 CSE）。==1||==2 → 不广播。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;
    PacketGuard packet;
    packet.put_header(0, 0xbc);
    packet.put_byte((int)pPkt->m_12);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_NoticePowerWarProlong::~Inter_NoticePowerWarProlong() {}
// ORIG 0x084d4fce T dispatch_sig —— 力量战争延长公告广播。
// 报告 docs/class_func_reports/Inter_NoticePowerWarProlong/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePowerWarProlongPktView —— Inter_NoticePowerWarProlong::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarProlongPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

int Inter_NoticePowerWarProlong::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarProlongPktView* pPkt = reinterpret_cast<Inter_NoticePowerWarProlongPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0xa3);
    packet.put_int(pPkt->m_a);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_ChangeGuildName::~Inter_ChangeGuildName() {}
// ORIG 0x084d5072 T dispatch_sig —— 公会改名：成功改本地与 proxy 或回执错误码。
// 报告 docs/class_func_reports/Inter_ChangeGuildName/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ChangeGuildNamePktView —— Inter_ChangeGuildName::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ChangeGuildNamePktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    char m_pad12[0x4];  // 未消费
    unsigned char m_16;       // +0x16
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=94（ORIG 0x084d5072）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=56（ORIG 0x84d5072）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_ChangeGuildName::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    char* recv = data;                                 // -0x10(%ebp) 缓存消息指针

    PacketGuard packet;
    packet.put_header(1, 0xb6);
    if (reinterpret_cast<Inter_ChangeGuildNamePktView*>(recv)->m_16 == 0)
    {   // 成功 → 本地改名并同步 proxy
        packet.put_byte(1);
        const int nameLen = strlen(reinterpret_cast<Inter_ChangeGuildNamePktView*>(recv)->m_17);
        packet.put_int(nameLen);
        packet.put_str(reinterpret_cast<Inter_ChangeGuildNamePktView*>(recv)->m_17, nameLen);
        user->ChangeGuildName(recv + 0x17);
        // [b28d-lane2] ORIG 084d50f6 段：GetServerProxy 结果直落 (%esp)（无具名 proxy 槽，
        // P183/P0）——旧具名 proxy 占 4B 使全帧槽号深移 4。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->ChangeGuildNameInProxy(
                reinterpret_cast<Inter_ChangeGuildNamePktView*>(recv)->m_e,
                recv + 0x17);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte((unsigned int)reinterpret_cast<Inter_ChangeGuildNamePktView*>(recv)->m_16);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_NoticeChangeGuildNameToMembers::~Inter_NoticeChangeGuildNameToMembers() {}
// ORIG 0x084d51dc T dispatch_sig —— 公会改名成员通知：校验 charac_no 后转发新名并改名。
// 报告 docs/class_func_reports/Inter_NoticeChangeGuildNameToMembers/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeChangeGuildNameToMembersPktView —— Inter_NoticeChangeGuildNameToMembers::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticeChangeGuildNameToMembersPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（P211：与 get_charac_no 返回值异型触发 regmove 中转）
    char m_pad12[0x4];  // 未消费
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_NoticeChangeGuildNameToMembers::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    char* recv = data;                                 // ORIG 0x084d51e4 入口单槽 -0x10（技巧36）

    if (user->get_charac_no(-1)
        != reinterpret_cast<Inter_NoticeChangeGuildNameToMembersPktView*>(recv)->m_e)
    {   // m_uCharId 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x36ca,
            "Inter_NoticeChangeGuildNameToMembers::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharId(%d)",
            user->get_charac_no(-1),
            reinterpret_cast<Inter_NoticeChangeGuildNameToMembersPktView*>(recv)->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0xda);
    int nameLen = strlen(
        reinterpret_cast<Inter_NoticeChangeGuildNameToMembersPktView*>(recv)->m_16);
    packet.put_int(nameLen);
    packet.put_str(
        reinterpret_cast<Inter_NoticeChangeGuildNameToMembersPktView*>(recv)->m_16,
        nameLen);
    packet.finalize(true);
    user->Send(packet);
    user->ChangeGuildName(recv + 0x16);
    return 0;
}

Inter_SendAllUserInfoMiniumForGuildSystem::~Inter_SendAllUserInfoMiniumForGuildSystem() {}
// ORIG 0x084d533a T dispatch_sig —— 公会系统最小用户信息全员广播。
// 报告 docs/class_func_reports/Inter_SendAllUserInfoMiniumForGuildSystem/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SendAllUserInfoMiniumForGuildSystemPktView —— Inter_SendAllUserInfoMiniumForGuildSystem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SendAllUserInfoMiniumForGuildSystemPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e
};
#pragma pack(pop)

int Inter_SendAllUserInfoMiniumForGuildSystem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SendAllUserInfoMiniumForGuildSystemPktView* pPkt = reinterpret_cast<Inter_SendAllUserInfoMiniumForGuildSystemPktView*>(data);
    // ORIG：同 ReplyGuildLetter。
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x36e3,
            "Inter_SendAllUserInfoMiniumForGuildSystem::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharId(%d)",
            user->get_charac_no(-1), pPkt->m_e);   // ORIG 一步装载，同上
        return 0;
    }

    PacketGuard packet;
    packet.clear();                                    // InterfacePacketBuf::clear
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, 0);          // pack 至 packet 缓冲
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);
    return 0;
}

Inter_LoadCleanPadPointReq::~Inter_LoadCleanPadPointReq() {}
int Inter_LoadCleanPadPointReq::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    DB_LoadCleanPadPoint::makeRequest();
    cMyTrace(__PRETTY_FUNCTION__,
        0x371d, 0)("Get CleanPad point Request");
    return 0;
}

Inter_LoadCleanPadPoint::~Inter_LoadCleanPadPoint() {}
int Inter_LoadCleanPadPoint::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    int* point = reinterpret_cast<int*>(data);       // 整包首槽指针视图（非成员访问）
    WongWork::CMCAPManager::setPointPerHackType(point);
    return 0;
}

Inter_NotifyAuctionMail::~Inter_NotifyAuctionMail() {}
// ORIG 0x084d55a4 T dispatch_sig —— 拍卖邮件通知：未读邮件计数 +1 并组 0xa9 包回发。
// 报告 docs/class_func_reports/Inter_NotifyAuctionMail/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NotifyAuctionMailPktView —— Inter_NotifyAuctionMail::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NotifyAuctionMailPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    unsigned char m_12;             // +0x12
    int m_13;                       // +0x13
    unsigned char m_17;             // +0x17
};
#pragma pack(pop)

int Inter_NotifyAuctionMail::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NotifyAuctionMailPktView* pPkt = reinterpret_cast<Inter_NotifyAuctionMailPktView*>(data);
    if (user->getCurCharacR() == 0 ||                // asm 双失败归并 true
        user->getCurCharacNo() !=
            pPkt->m_a)     // msg+0x0a 角色号
        return 0x3748;

    // 频道门：GetChannelType()==7 或 IsIntegratedPvPBaseChannel() 时跳过发包
    if (G_GameWorld()->GetChannelType() == 7 ||
        G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;

    reinterpret_cast<WongWork::CMailBox*>(user->GetMailBox())
        ->IncNotLoadedMailCount();                   // ORIG W 0x084ed330

    PacketGuard packet;
    packet.put_header(0, 0xa9);
    packet.put_byte(                                 // msg+0x17（movzbl）
        static_cast<int>(pPkt->m_17));
    packet.put_byte(                                 // msg+0x12（movzbl）
        static_cast<int>(pPkt->m_12));
    packet.put_int(pPkt->m_13);
    // ORIG 0x084d56e2：call 后 `cwtl` → 实参以 short 形态再提升 int（探针 s1 实证）。
    packet.put_short(                                // 未读邮件计数
        (int)(short)reinterpret_cast<WongWork::CMailBox*>(user->GetMailBox())
            ->GetNotLoadedMailCount());
    // ORIG 0x084d56f5：经 -0xc(%ebp) 包基址槽取随机选项区
    reinterpret_cast<const RandomOption*>(
        // [b28d-solo-InterDispatcher] RandomOption::put_packet_random_option<PacketGuard>
        // verdict=DIFF ndiff=27：N107 成员常量偏移取址 add-vs-lea（4.4 恒 add；六形探针全败）⇒ N 封顶。
        reinterpret_cast<char*>(pPkt) + 0x18)
        ->put_packet_random_option(packet);          // msg+0x18 随机选项
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_AuctionResultAskPrice::~Inter_AuctionResultAskPrice() {}
// ORIG 0x084d5740 T dispatch_sig —— 拍卖询价应答：GM 延迟追踪后组 0xb9 包回发。
// 报告 docs/class_func_reports/Inter_AuctionResultAskPrice/dispatch_sig.md 全文转写。
// ORIG d _ZL6gmList @0x093701d8（延迟追踪 GM 角色号表；定义先移至首个使用点前）
static const int g_auctionResultGmList[5] =
    { 0x4c2dc8, 0x5cf723, 0xa9c742, 0xa2ad85, 0x9ae1ae };

#pragma pack(push, 1)
// Inter_AuctionResultAskPricePktView —— Inter_AuctionResultAskPrice::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultAskPricePktView
{
    int m_0;                  // +0x00
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=106（ORIG 0x084d5740）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=70（ORIG 0x84d5740）：P30/N4 add-vs-lea。
int Inter_AuctionResultAskPrice::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionResultAskPricePktView* pPkt = reinterpret_cast<Inter_AuctionResultAskPricePktView*>(data);
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp); jne
        return 0;

    const int* msg =&pPkt->m_0;
    if (msg[1] != user->getCurCharacNo())            // msg+0x04 vs 当前角色号
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle；ORIG d _ZL6gmList 同 TU g_auctionResultGmList）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x377c, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultAskPrice",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;                              // ORIG 0x0858dd4c C1
    packet.put_header(1, 0xb9);                      // put_header(1,0xb9)
    packet.put_byte(1);
    packet.put_byte(msg[0]);                         // msg+0x00（movzbl 截断）
    packet.put_int(msg[2]);                          // msg+0x08
    for (int i = 0; i <= 2; ++i)                     // msg+0x0c..+0x14（cmpl $0x2, setle）
        packet.put_int(msg[i + 3]);
    for (int i = 0; i <= 2; ++i)                     // msg+0x18..+0x20（同界）
        packet.put_int(msg[i + 6]);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_AuctionResultAskRegistedItemNum::~Inter_AuctionResultAskRegistedItemNum() {}
// ============================================================================
// ORIG 0x084d5930 T dispatch_sig —— 拍卖行"已登记物品数查询应答（登记执行）"。
// 报告 docs/class_func_reports/Inter_AuctionResultAskRegistedItemNum/dispatch_sig.md
// 全文转写（1073 insn，Gold 与 Cera 双通道）。包布局 PCK_AUCTION_REGIST_ITEM_GA
// 依 ORIG ctor @0x84e9a12 / DnfItemInfo::reset @0x84e9094 逐条 store 还原；
// ROI_Category::_sort @0x0822ac30。gmList ORIG d @0x093701d8 实测五值。
// ============================================================================

// ---- ORIG 辅助类型（同源弱符号；Game 侧唯一声明点在本 TU）----
#pragma pack(push, 1)
namespace {

struct RegPckGuid  // 10B：ORIG pck+0x8b..+0x95（=auction itemGloballyUniqueIdentifier_t 10B 版；AuctionItem.h 与 Inven_Item.h 冲突故本地定义）
{
    char data_[10];
};

}  // namespace

struct PCK_AUCTION_REGIST_ITEM_GA : public nsl::PACKET_HEADER
{
    int char_idx;                             // @18
    int owner_id;                             // @22
    char owner_name[13];                      // @26
    unsigned char user_type;                  // @39
    int price;                                // @40
    int instant_price;                        // @44
    DnfItemInfo item_info;                    // @48 (0x35)：权威类型（ORIG ctor call `_ZN11DnfItemInfoC1Ev` T @0x84e910a 实证；DnfItemInfo.h 同布局）
    stAvatarEmblemInfo_t jewel_info;          // @101
    stAvatarExpansionInfo_t expansion;        // @131
    unsigned int black_point;                 // @135
    char guid[10];                            // @139（10B；ORIG dispatch_sig +0x8b..+0x95）
    int unit_price;                           // @149
    ROI_Category _reg_roi_category_key;       // @153（12B，ORIG ROI_Category：m_keys[3]+pad+m_marks[3]）
    char owner_nexon_id[nexon::cash::MAX_NEXON_ID_LENTH];  // @165

    PCK_AUCTION_REGIST_ITEM_GA()
    {
        memset(this, 0, 0xc5);   // ORIG ctor memset(this,0,0xc5)
        setCategory(0);
        setPacketID(3);
        setSize(0xc5);
    }
};
#pragma pack(pop)

// 布局自检（ORIG 实证：guid @0x8b..+0x95、roi @0x99、sizeof 0xc5；字段已按 ORIG 补齐）



#pragma pack(push, 1)
// Inter_AuctionResultAskRegistedItemNumPktView —— Inter_AuctionResultAskRegistedItemNum::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultAskRegistedItemNumPktView
{
    unsigned int m_0;         // +0x00
    int m_4;                  // +0x04
    short m_8;                // +0x08
    char m_pada[0x1];  // 未消费
    unsigned short m_b;       // +0x0b
    unsigned int m_d;         // +0x0d
    int m_11;                 // +0x11
    int m_15;                 // +0x15
    int m_19;                 // +0x19
    int m_1d;                 // +0x1d
};
#pragma pack(pop)

// §6.2 巨件核销：P167 111/111 [0缺0多]；gap 24→24；残差归因 N82（4.4 槽复用全帧错位，
// 详见 docs/asm_translation_patterns.md 负例表）。batch-20「Inter 巨兽」已核销，本次维持。
// [b28d-lane2] verdict=DIFF ndiff=1035（ORIG 0x084d5930）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+21。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=802（ORIG 0x84d5930）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N18a 单段 regmove。
int Inter_AuctionResultAskRegistedItemNum::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionResultAskRegistedItemNumPktView* pPkt = reinterpret_cast<Inter_AuctionResultAskRegistedItemNumPktView*>(data);
    if (user == 0)                                   // asm cmpl $0,user
        return 0;

    const int reqCharacNo = pPkt->m_4;       // msg+0x04
    if (pPkt->m_4 != user->getCurCharacNo()) // 差值即 ORIG setne 判定
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace("virtual int Inter_AuctionResultAskRegistedItemNum::dispatch_sig(CUser*, char*, int)",
                0x37a3, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultAskRegistedItemNum",
                  user->getCurCharacNo());
            break;
        }
    }

    const unsigned int payType =
        pPkt->m_0;                      // msg+0x00
    const short nowRegisted =
        pPkt->m_8;                         // msg+0x08
    if (nowRegisted == -1)
    {   // 上限查询失败应答 err=0x9e
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x9e, (PAY_TYPE)payType);
        return 0;
    }
    if (nowRegisted == -2)
    {   // 非法区/黑名单 err=0x91
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
        return 0;
    }

    // 已登记数量上限校验：premium/gold 两套上限表（err=0x97）
    const bool premiumAffected = user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x1d);
    if (!premiumAffected)
    {
        const int maxCount = (pPkt->m_0 == 1) ? 3 : 10;
        if (maxCount <= nowRegisted)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x97, (PAY_TYPE)payType);
            return 0;
        }
    }
    else
    {
        const int maxCount = (pPkt->m_0 == 1) ? 10 : 30;
        if (maxCount <= nowRegisted)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x97, (PAY_TYPE)payType);
            return 0;
        }
    }

    // 手续费门槛（gold 登记 10000 / cera 0），不足 err=0x90
    int registMoney = (pPkt->m_0 == 1) ? 0 : 10000;
    if (user->getCurCharacMoney() < registMoney)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x90, (PAY_TYPE)payType);
        return 0;
    }

    const unsigned int itemId =                                       // msg+0x0d
        pPkt->m_d;
    // cera 礼包手续费基数（ORIG 仅当 pay==1 且 itemId∈礼包区间时换算登记费）
    long bonusSec = 0;
    if ((0x28d287 < itemId && itemId < 0x28d29a) &&
        pPkt->m_0 == 1)
    {
        if (itemId < 0x28d292)
            bonusSec = static_cast<long>(itemId) * 1000000 + 0x199b9040;
        else
            bonusSec = static_cast<long>(itemId) * 10000000 - 0x549a800;
    }

    const char invenKind = *(data + 0xa);                             // msg+0x0a
    const unsigned short slotNo =                                     // msg+0x0b
        pPkt->m_b;
    const int price = pPkt->m_15;     // msg+0x15
    const int instantPrice = pPkt->m_19;  // msg+0x19
    const int restrictAmount = pPkt->m_11;  // msg+0x11

    PCK_AUCTION_REGIST_ITEM_GA pck;

    // ---- 公共头域填写（asm +0x2f6..+0x42b 区）----
    pck.char_idx = static_cast<int>(user->GetUID());                  // GetUID
    pck.owner_id = user->getCurCharacNo();                            // getCurCharacNo
    strncpy(pck.owner_name,
            const_cast<char*>(reinterpret_cast<const char*>(user->getCurCharacR())) + 4, 0xd);
    pck.price = price;
    pck.instant_price = instantPrice;
    pck.black_point = user->getCurrCharacBlackCount();
    pck.unit_price = pPkt->m_1d;      // msg+0x1d
    pck.user_type = static_cast<unsigned char>(
        user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x1d));             // ORIG 第二次 isAffectedPremium → local_158
    {   // ORIG 三次 movl 装载 msg+0x21/0x25/0x29（12B ROI_Category），非 memcpy（_sort 延至块尾，ORIG +0x1577）
        unsigned int* roiw = reinterpret_cast<unsigned int*>(&pck._reg_roi_category_key);
        const unsigned int* roir = reinterpret_cast<const unsigned int*>(data + 0x21);
        roiw[0] = roir[0];
        roiw[1] = roir[1];
        roiw[2] = roir[2];
    }

    CInventory* inven = user->getCurCharacInvenW();
    Inven_Item item;                                                  // 局部空 Inven_Item

    if (itemId != 0)
    {
        // 槽位域 → INVEN_TYPE 映射（0→INVEN、1→AVATAR、7→CREATURE）
        int invenType = 1;
        if (invenKind == '\x01')
            invenType = 2;
        else if (invenKind == '\a')
            invenType = 3;
        else if (invenKind != '\0')
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
            return 0;
        }

        // cera 礼包专区（itemId∈礼包区间 & pay==1）：直接扣登记费 + 转发cera代理
        if ((0x28d287 < itemId && itemId < 0x28d29a))
        {
            if (pPkt->m_0 != 1)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x93, (PAY_TYPE)payType);
                return 0;
            }
            if (!inven->use_money(static_cast<int>(bonusSec), (eMoneySubReason)0x26, true))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x93, (PAY_TYPE)payType);
                return 0;
            }
            user->SaveInventory();
            user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
            pck.setCategory(0x12);
            pck.item_info.m_addInfo = restrictAmount;                  // ORIG local_149 = msg+0x11
            pck.jewel_info.init();                                    // ORIG stAvatarEmblemInfo_t::init(pck+0x65)
            pck.item_info.m_seal = false;                               // ORIG local_14f = 0
            pck.item_info.m_itemId = itemId;                            // ORIG local_14e = msg+0x0d
            pck.item_info.m_attr = 0;                             // ORIG local_145 = 0
            strncpy(pck.owner_nexon_id, user->get_acc_name(),
                    strlen(user->get_acc_name()));                     // ORIG 形：strncpy(dst,acc,strlen)（不补 \0）
            GlobalData::s_cera_auction_proxy->SendPacket(&pck);
            return 0;
        }

        // 实体槽位副本与合法链
        item = inven->GetInvenSlot(invenType, slotNo);
        if (item.m_addInfo != static_cast<int>(itemId) ||
            item.m_addInfo == 0x10ec)                                  // 禁拍 ID
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
            return 0;
        }
        if (IsTradeLimitAttachTypeItemByUser(*user, invenType, slotNo))
        {
            bool movable = true;
            CheckItemTradeLimitCountToMoveByUser(*user, invenType, slotNo, movable);
            if (!movable)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x72);
                return 0;
            }
        }
        if (!item.isEquipableItemType() &&
            item.m_addInfo2 < static_cast<unsigned int>(restrictAmount))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x92, (PAY_TYPE)payType);
            return 0;
        }

        CDataManager* dm = G_CDataManager();
        CItem* ci = dm->find_item(static_cast<int>(itemId));
        if (ci == 0 || !ci->isPackagable())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
            return 0;
        }

        // [b28d-solo-InterDispatcher] P282 槽序还原：ORIG 0x084d617e/0x084d64a1/0x084d65ab
        // 三次 `vptr+0x10` 间接调用 = _ZTV5CItem word6 = CItem::IsAvatarItem()（每次重调，
        // 非缓存 bool）；旧 avatarFlow 缓存使三处虚调用坍缩为一次。
        if (ci->IsAvatarItem())
        {
            const WongWork::CAvatarItemMgr* mgr =
                user->getCurCharacInvenR()->GetAvatarItemMgrR();      // ORIG getCurCharacInvenR→GetAvatarItemMgrR→GetExpireDate
            const int expireDate = mgr->GetExpireDate(item.m_addInfo2);
            if (expireDate != 0 &&
                GlobalData::s_systemTime_.getCurSec() > expireDate)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                return 0;
            }
        }
        else
        {
            const int usablePeriod = ci->getUsablePeriod();
            const int expirationDate = ci->getExpirationDate();
            if (usablePeriod != 0 || expirationDate != 0)
            {   // ORIG 双重判定（重复 getter 调用——不得合并为局部缓存）：
                //   第二次重读 usablePeriod/expirationDate 同时为 0 → err=0x91（ORIG +0x0a27）；
                //   否则到期时间 local_28 <= getCurSec → err=0x91（ORIG +0x0a31）。
                if (ci->getUsablePeriod() == 0 && ci->getExpirationDate() == 0)
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                    return 0;
                }
                const long endTime =
                    static_cast<long>(item.m_fieldd & 0xff) * 0x15180 +
                    0x44a54a80;
                if (endTime <= GlobalData::s_systemTime_.getCurSec())
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                    return 0;
                }
            }
        }

        if (inven->GetCreatureMgrR()->FindCreatureItem(item.m_addInfo2) == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
            return 0;
        }

        if (pck.user_type == 0 && ci->GetCashPrice() != 0 &&
            !G_CDataManager()->is_AuctionRegFreeCeraItem(itemId))     // ORIG 第二次 G_CDataManager（+0x0ae4）
        {   // 非白名单付费道具禁止金币拍卖（err=0x95）
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x95, (PAY_TYPE)payType);
            return 0;
        }

        switch (ci->GetAttachType())
        {
        case 1:
        case 2:
        case 5:
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
            return 0;
        case 3:                                                        // 绑定型须非空
            if (item.m_addInfo == 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                return 0;
            }
            break;
        default:
            break;
        }

        if (item.m_upgradeSep.IsTradeRestriction())                    // 分离升级位限制
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91);
            return 0;
        }
        if (ci->IsAvatarItem())                                   // ORIG +0xb71 第二次 vptr+0x10
        {
            const WongWork::CAvatarItemMgr* mgrT = inven->GetAvatarItemMgrR();   // ORIG 第 5 处 GetAvatarItemMgrR（+0x0bee 前置 mgr 装载）
            if (mgrT->IsTempKey(item.m_addInfo2))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                return 0;
            }
            if (inven->GetAvatarItemMgrR()->GetExpireDate(item.m_addInfo2) != 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x91, (PAY_TYPE)payType);
                return 0;
            }
        }

        // ==== 包主体详细字段化（报告反编译 local_14f..local_126 全链）====
        pck.item_info.m_seal = false;
        pck.item_info.m_itemId = itemId;
        pck.item_info.m_attr = item.GetItemAttr();                // 提升档位/seal 位
        pck.item_info.m_endurance = item.m_fieldb;
        pck.item_info.m_extendInfo = item.m_fieldd;
        pck.item_info.m_addInfo =
            item.isEquipableItemType() ? item.m_addInfo2 : restrictAmount;
        item.m_amp.getValues(pck.item_info.m_abilityType, pck.item_info.m_abilityValue);
        pck.jewel_info.init();                                         // ORIG 无条件 stAvatarEmblemInfo_t::init(local_11a)
        if (ci->IsAvatarItem())                                   // ORIG +0xc7b 第三次 vptr+0x10
        {
            const WongWork::CAvatarItemMgr* mgrJ = inven->GetAvatarItemMgrR();   // ORIG +0x0cad
            memcpy(&pck.jewel_info,
                   mgrJ->getJewelSocketData(item.m_addInfo2), 0x1e);             // ORIG memcpy(local_11a,src,0x1e)
            const WongWork::CAvatarItemMgr* mgrE = inven->GetAvatarItemMgrR();   // ORIG +0x0ce2
            const void* exp = mgrE->GetExpansionInfo(item.m_addInfo2);           // ORIG +0x0ce7
            if (exp == 0)
                pck.expansion.init();                                            // ORIG stAvatarExpansionInfo_t::init
            else
                memcpy(&pck.expansion, exp, 4);                                  // ORIG memcpy(asStack_fc,src,4)
            if (pck.jewel_info.stEmblemSocket[0].emblemSocketType_ != 0)
                pck.item_info.m_attr =
                    (unsigned char)((pck.item_info.m_attr & 0xe0) | 0x1);
        }
        {   // ORIG 逐字段（CONCAT13/CONCAT33）写 random_option，非 memcpy；随后 SetUpgradeSeparate
            const unsigned char* src = reinterpret_cast<const unsigned char*>(&item) + 0x25;
            unsigned char* dst = reinterpret_cast<unsigned char*>(&pck.item_info.m_opt);
            for (int i = 0; i < (int)sizeof(RandomOption); ++i)
                dst[i] = src[i];
            pck.item_info.m_up.SetUpgradeSeparate(
                item.m_upgradeSep.GetUpgradeSeparate());
        }

        // [b28d-solo-InterDispatcher] P282 槽序还原：ORIG 0x084d675f `vptr+0x4c` ⇒
        // _ZTV5CItem word21 = CItem::IsRandomOption()（0x08514412）；循环体在其 true 分支内。
        if (ci->IsRandomOption())
        {
            for (int k = 0; k <= 2; ++k)
            {
                if (pck._reg_roi_category_key.m_keys[k] == 0)
                    pck._reg_roi_category_key.m_keys[k] = 30000;
            }
        }
        pck._reg_roi_category_key._sort();                            // ORIG ROI_Category::_sort（+0x1577）
    }

    // ---- END 收尾段（LA_084d67be 起）----
    const int fee = (pPkt->m_0 == 1) ? 0 : 10000;
    if (!inven->use_money(fee, (eMoneySubReason)0x19, true))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x93, (PAY_TYPE)payType);
        return 0;
    }
    if (item.m_addInfo != 0)
    {
        if (item.m_field1 == '\b')                                     // 化身穿戴位删除
        {
            if (!inven->delete_item((INVEN_TYPE)2, slotNo, 1, (eItemDelReason)5, true))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x94, (PAY_TYPE)payType);
                const int gained = inven->gain_money(10000, (eMoneyAddReason)0x13, true, 0);
                if (gained > 9999)
                    return 0;
                user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, 10000,
                                          static_cast<unsigned long>(gained));
                return 0;
            }
            inven->SendAvatarEvent(0x36, item.m_addInfo2, 4, slotNo + 10, 0);
            inven->GetAvatarItemMgrW()->UnRegistItem(item.m_addInfo2);
        }
        else
        {
            if (!inven->delete_item((INVEN_TYPE)item.m_field0, slotNo, static_cast<int>(pck.item_info.m_addInfo),
                                    (eItemDelReason)0x19, true))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xba, 0x94, (PAY_TYPE)payType);
                const int gained = inven->gain_money(10000, (eMoneyAddReason)0x13, true, 0);
                if (gained > 9999)
                    return 0;
                user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)0, 10000,
                                          static_cast<unsigned long>(gained));
                return 0;
            }
        }
        if (item.IsCreatureItemType())
        {
            inven->GetCreatureMgrW()->AuctionRegist(&item, slotNo, 0x19);
        }
    }
    user->SaveInventory();
    strncpy(pck.owner_nexon_id, user->get_acc_name(), strlen(user->get_acc_name()));  // ORIG strncpy（END）
    GlobalData::s_auction_proxy->SendPacket(&pck);
    return 0;
}

Inter_AuctionResultRegist::~Inter_AuctionResultRegist() {}
// ORIG 0x084d6ad6 T dispatch_sig —— 拍卖登记结果应答：GM 延迟追踪 + 0xba 包 + AP 清理。
// 报告 docs/class_func_reports/Inter_AuctionResultRegist/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AuctionResultRegistPktView —— Inter_AuctionResultRegist::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultRegistPktView
{
    int m_0;                  // +0x00
    char m_pad4[0x4];  // 未消费
    unsigned char m_8;        // +0x08
    unsigned char m_9;        // +0x09
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=106（ORIG 0x084d6ad6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=69（ORIG 0x84d6ad6）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_AuctionResultRegist::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionResultRegistPktView* pPkt = reinterpret_cast<Inter_AuctionResultRegistPktView*>(data);
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    const int* msg =&pPkt->m_0;
    if (msg[1] != user->getCurCharacNo())            // msg+0x04 角色号校验
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3ab4, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultRegist",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xba);
    const unsigned char result =                     // msg+0x08 结果码
        pPkt->m_8;
    packet.put_byte(static_cast<int>(result));
    if (static_cast<char>(msg[2]) == '\0')           // 低字节为 0 时补错误码
        packet.put_byte(
            static_cast<int>(pPkt->m_9));
    packet.put_byte(msg[0]);                         // msg+0x00 pay 类型
    packet.finalize(true);
    user->Send(packet);

    if (static_cast<char>(msg[2]) == '\x01')         // 成功：清理 AP 9 号动作组
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)9, 0, (ENUM_PACKETCLASS)0);
    }
    return 0;
}

Inter_AuctionResultBidding::~Inter_AuctionResultBidding() {}
// ORIG 0x084d6ca4 T dispatch_sig —— 拍卖竞标结果转发台湾平台实现。
// 报告 docs/class_func_reports/Inter_AuctionResultBidding/dispatch_sig.md 全文转写。
int Inter_AuctionResultBidding::dispatch_sig(CUser* user, char* data, int len)
{
    if (user == 0)
        return 0;
    return dispatch_sig_taiwan(user, data, len);
}

// ORIG 0x084d6cda T _ZN26Inter_AuctionResultBidding19dispatch_sig_taiwanEP5CUserPci
// 报告 docs/class_func_reports/Inter_AuctionResultBidding/dispatch_sig_taiwan.md：
// 角色号校验→GM 延迟追踪→组 0xbc 包→按 pay 结果写确认/退款/金返还→
// IPG cancel/confirm 双段计费→发包→IPGQuery。
#pragma pack(push, 1)
// Inter_AuctionResultBiddingPktView —— Inter_AuctionResultBidding::dispatch_sig_taiwan 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultBiddingPktView
{
    int m_0;                  // +0x00
    char m_pad4[0x8];  // 未消费
    unsigned char m_c;        // +0x0c
    unsigned char m_d;        // +0x0d
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=449（ORIG 0x084d6cda）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-26。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=560（ORIG 0x84d6cda）：N18a 单段 regmove+N7 间接调用寄存器+N82/P112 帧槽平移。
int Inter_AuctionResultBidding::dispatch_sig_taiwan(CUser* user, char* data, int len)
{
    Inter_AuctionResultBiddingPktView* pPkt = reinterpret_cast<Inter_AuctionResultBiddingPktView*>(data);
    (void)len;                                         // ORIG 未用 param_3 长度
    const int* msg =&pPkt->m_0;
    if (msg[1] != static_cast<int>(user->getCurCharacNo()))   // msg+0x04 角色号
        return 0;

    {   // GM 延迟追踪（cmpl $0x4 setle；g_auctionResultGmList = ORIG _ZL6gmList）
        for (int i = 0; i <= 4; ++i)
        {
            if (g_auctionResultGmList[i] == static_cast<int>(user->getCurCharacNo()))
            {
                cMyTrace(__PRETTY_FUNCTION__,
                    0x3c12, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultBidding",
                      static_cast<int>(user->getCurCharacNo()));
                break;
            }
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xbc);
    packet.put_byte(static_cast<int>(
        pPkt->m_c));      // msg byte @0xc
    if (static_cast<char>(msg[3]) == '\0')                    // 低字节为 0 补错误码
        packet.put_byte(static_cast<int>(
            pPkt->m_d));  // byte @0xd
    packet.put_byte(msg[0]);                                  // msg+0x00 pay 类型

    PrePayInfo* prePay = 0;                            // local_2c（FindPrePayInfo 返回 &PrePayInfo）
    Taiwan::BillTransaction trans1;                    // -0x60..-0x54 段（+0x10/+0x14/+0x18）
    Taiwan::BillTransaction trans2;                    // -0x64..-0x58 段（+0x1c/+0x20/+0x24）
    unsigned int amount1 = 0;                          // local_24（PrePayInfo+0x08）
    unsigned int amount2 = 0;                          // local_20（PrePayInfo+0x0c）
    WongWork::IPG::CIPGMgr* mgr = 0;                   // local_28

    if (msg[0] == 1)
    {
        user->SetBuyingGold(false);
        prePay = G_CPrePayInfoMgr()->FindPrePayInfo(
            static_cast<int>(user->getCurCharacNo()));
        if (prePay == 0)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x3c28, "PrePayInfo is null. character:%u",
                static_cast<unsigned int>(user->getCurCharacNo()));
            return 0;                                  // LAB_084d7407（guard 析构后返回）
        }
        // [b28d-solo-InterDispatcher] P282 槽序还原：ORIG 0x084d6ebe `add $0x3c`（vptr）⇒
        // _ZTVN8WongWork3IPG10CIPGHelperE @0x08b420a0 word17 = GetIPGMgr()（无参、仅压 this）。
        // 旧桥多传 characNo（p167 EXTRA getCurCharacNo）且绕过 vtable 派发。
        mgr = GlobalData::s_pIPGHelper->GetIPGMgr();

        // PrePayInfo +0x10/+0x14/+0x18 → trans1；+0x1c/+0x20/+0x24 → trans2
        trans1.m_tranNo = *reinterpret_cast<unsigned long long*>(
            reinterpret_cast<char*>(prePay) + 0x10);
        trans1.m_field0 = *reinterpret_cast<int*>(
            reinterpret_cast<char*>(prePay) + 0x18);
        trans2.m_tranNo = *reinterpret_cast<unsigned long long*>(
            reinterpret_cast<char*>(prePay) + 0x1c);
        trans2.m_field0 = *reinterpret_cast<int*>(
            reinterpret_cast<char*>(prePay) + 0x24);
        amount1 = *reinterpret_cast<unsigned int*>(
            reinterpret_cast<char*>(prePay) + 0x08);
        amount2 = *reinterpret_cast<unsigned int*>(
            reinterpret_cast<char*>(prePay) + 0x0c);

        G_CPrePayInfoMgr()->DeletePrePayInfo(
            static_cast<int>(user->getCurCharacNo()));
    }

    if (static_cast<char>(msg[3]) == '\0')
    {
        if (msg[0] == 1)
        {   // 失败：双段回滚（cancel）
            Taiwan::TaiwanCash* cash = mgr->getBillingCash();
            int rc = cash->cancel(trans1);
            if (CheckBillingErrorCode(rc))
                user->m_historyLog.BillingSuccess(
                    static_cast<ENUM_BILLING_TYPE>(2), trans1.m_tranNo,
                    static_cast<int>(amount1));
            else
            {
                LogManager::logFormat(
                    1, "InterDispatcher.cpp",
                    __PRETTY_FUNCTION__,
                    0x3c3c, "IPG_CANCEL user: %d, charac: %s reason: %d",
                    static_cast<unsigned int>(user->getCurCharacNo()),
                    user->getCurCharacName(), rc);
                user->m_historyLog.BillingFail(
                    static_cast<ENUM_BILLING_TYPE>(2), trans1.m_tranNo,
                    static_cast<int>(amount1), rc, const_cast<char*>("rollback"));
            }
            cash = mgr->getBillingCash();
            rc = cash->cancel(trans2);
            if (CheckBillingErrorCode(rc))
                user->m_historyLog.BillingSuccess(
                    static_cast<ENUM_BILLING_TYPE>(2), trans2.m_tranNo,
                    static_cast<int>(amount2));
            else
            {
                LogManager::logFormat(
                    1, "InterDispatcher.cpp",
                    __PRETTY_FUNCTION__,
                    0x3c47, "IPG_CANCEL user: %d, charac: %s reason: %d",
                    static_cast<unsigned int>(user->getCurCharacNo()),
                    user->getCurCharacName(), rc);
                user->m_historyLog.BillingFail(
                    static_cast<ENUM_BILLING_TYPE>(2), trans2.m_tranNo,
                    static_cast<int>(amount2), rc, const_cast<char*>("rollback"));
            }
        }
        else
        {   // 成功退款：金币返还（msg+0x08 金额，reason 0x14）
            const int money = msg[2];
            const int gained = user->getCurCharacInvenW()->gain_money(
                money, static_cast<eMoneyAddReason>(0x14), true, 0);
            if (gained < money)
                user->SendMoneyFullReason(
                    static_cast<ENUM_MONEY_FULL_REASON>(0),
                    static_cast<unsigned long>(money),
                    static_cast<unsigned long>(gained));
        }
    }
    else if (msg[0] == 1)
    {   // 成功：双段确认（confirm）
        Taiwan::TaiwanCash* cash = mgr->getBillingCash();
        int rc = cash->confirm(trans1);
        if (CheckBillingErrorCode(rc))
            user->m_historyLog.BillingSuccess(
                static_cast<ENUM_BILLING_TYPE>(2), trans1.m_tranNo,
                static_cast<int>(amount1));
        else
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x3c5f, "IPG_CONFIRM user: %d, charac: %s reason: %d",
                static_cast<unsigned int>(user->getCurCharacNo()),
                user->getCurCharacName(), rc);
            user->m_historyLog.BillingFail(
                static_cast<ENUM_BILLING_TYPE>(2), trans1.m_tranNo,
                static_cast<int>(amount1), rc, const_cast<char*>("confirm"));
        }
        cash = mgr->getBillingCash();
        rc = cash->confirm(trans2);
        if (CheckBillingErrorCode(rc))
            user->m_historyLog.BillingSuccess(
                static_cast<ENUM_BILLING_TYPE>(2), trans2.m_tranNo,
                static_cast<int>(amount2));
        else
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x3c6a, "IPG_CONFIRM user: %d, charac: %s reason: %d",
                static_cast<unsigned int>(user->getCurCharacNo()),
                user->getCurCharacName(), rc);
            user->m_historyLog.BillingFail(
                static_cast<ENUM_BILLING_TYPE>(2), trans2.m_tranNo,
                static_cast<int>(amount2), rc, const_cast<char*>("confirm"));
        }
    }

    packet.finalize(true);
    user->Send(packet);
    if (msg[0] == 1)
    {   // vtable+0x2c（slot12 IPGQuery(CUser*)）：类未建模——asm 桥残留保留，按 §8 协议待建模清理
        typedef void (*QueryFn)(void*, CUser*);
        void* helper = GlobalData::s_pIPGHelper;
        QueryFn query = *(QueryFn*)((*(void***)helper)[0x2c / 4]);
        query(helper, user);
    }
    return 0;
}
Inter_AuctionResultRegistCancel::~Inter_AuctionResultRegistCancel() {}
// ORIG 0x084d741c T dispatch_sig —— 拍卖登记取消结果：GM 延迟追踪后组 0xbb 包回发。
// 报告 docs/class_func_reports/Inter_AuctionResultRegistCancel/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AuctionResultRegistCancelPktView —— Inter_AuctionResultRegistCancel::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultRegistCancelPktView
{
    int m_0;                  // +0x00
    char m_pad4[0x4];  // 未消费
    unsigned char m_8;        // +0x08
    unsigned char m_9;        // +0x09
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=95（ORIG 0x084d741c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=64（ORIG 0x84d741c）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_AuctionResultRegistCancel::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionResultRegistCancelPktView* pPkt = reinterpret_cast<Inter_AuctionResultRegistCancelPktView*>(data);
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    const int* msg =&pPkt->m_0;
    if (msg[1] != user->getCurCharacNo())            // msg+0x04 角色号校验
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3c88, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultRegistCancel",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xbb);
    const unsigned char result =                     // msg+0x08 结果码
        pPkt->m_8;
    packet.put_byte(static_cast<int>(result));
    if (static_cast<char>(msg[2]) == '\0')           // 低字节为 0 时补错误码
        packet.put_byte(
            static_cast<int>(pPkt->m_9));
    packet.put_byte(msg[0]);                         // msg+0x00 pay 类型
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_AuctionResultItemList::~Inter_AuctionResultItemList() {}
// ORIG 0x084d75bc T dispatch_sig —— 拍卖物品列表应答：GM 延迟追踪 + 0xbd 包转发余包。
// 报告 docs/class_func_reports/Inter_AuctionResultItemList/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=69（ORIG 0x084d75bc）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=69（ORIG 0x84d75bc）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_AuctionResultItemList::dispatch_sig(CUser* user, char* data, int len)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;
    if (static_cast<unsigned int>(len) <= 3)         // asm cmp $0x3; jbe
        return 0;

    int characNoMsg = 0;
    memcpy(&characNoMsg, data, 4);                   // msg+0x00（本地 4 字节拷贝）
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3cab, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultItemList",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xbd);
    packet.put_byte(1);
    packet.put_binary(data + 4, len - 4);            // 余包原样转发
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_AuctionResultMyRegistedItems::~Inter_AuctionResultMyRegistedItems() {}
// ORIG 0x084d7758 T dispatch_sig —— 我的登记物品列表应答：GM 追踪 + 0xbf 包转发余包。
// 报告 docs/class_func_reports/Inter_AuctionResultMyRegistedItems/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=69（ORIG 0x084d7758）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=69（ORIG 0x84d7758）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_AuctionResultMyRegistedItems::dispatch_sig(CUser* user, char* data, int len)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;
    if (static_cast<unsigned int>(len) <= 3)         // asm cmp $0x3; jbe
        return 0;

    int characNoMsg = 0;
    memcpy(&characNoMsg, data, 4);                   // msg+0x00（本地 4 字节拷贝）
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3ced, 0)("Trace Auction Delay, %s(%d)",
                  "ACK AuctionResultMyRegistedItems", user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xbf);
    packet.put_byte(1);
    packet.put_binary(data + 4, len - 4);            // 余包原样转发
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_AuctionResultMyBidding::~Inter_AuctionResultMyBidding() {}
// ORIG 0x084d78f4 T dispatch_sig —— 我的竞拍列表应答：GM 追踪 + 0xc0 包转发余包。
// 报告 docs/class_func_reports/Inter_AuctionResultMyBidding/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=69（ORIG 0x084d78f4）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=69（ORIG 0x84d78f4）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_AuctionResultMyBidding::dispatch_sig(CUser* user, char* data, int len)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;
    if (static_cast<unsigned int>(len) <= 3)         // asm cmp $0x3; jbe
        return 0;

    int characNoMsg = 0;
    memcpy(&characNoMsg, data, 4);                   // msg+0x00（本地 4 字节拷贝）
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
    for (int i = 0; i <= 4; ++i)
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3d31, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultMyBidding",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xc0);
    packet.put_byte(1);
    packet.put_binary(data + 4, len - 4);            // 余包原样转发
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_AuctionLogMessage::~Inter_AuctionLogMessage() {}
int Inter_AuctionLogMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
    {
        return 0;
    }
    SIG_AUCTION_LOG_MESSAGE_AG* message =
        reinterpret_cast<SIG_AUCTION_LOG_MESSAGE_AG*>(data);
    user->m_historyLog.Auction(message);
    return 0;
}

Inter_AuctionNotifyAuctionService::~Inter_AuctionNotifyAuctionService() {}
// ORIG 0x084d7ac8 T dispatch_sig —— 拍卖服务开关：置 proxy 运行态并全员广播。
// 报告 docs/class_func_reports/Inter_AuctionNotifyAuctionService/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=90（ORIG 0x084d7ac8）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=95（ORIG 0x84d7ac8）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_AuctionNotifyAuctionService::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
        reinterpret_cast<SIG_AUCTION_NOTIFY_AUCTION_SERVICE*>(data);

    PacketGuard packet;
    packet.put_header(0, 0xb7);
    packet.put_byte(sig->m_result);
    if (sig->m_result == 1)
    {   // 点数拍卖服
        GlobalData::s_cera_auction_proxy->SetRunning((char)sig->m_flag);
    }
    else
    {   // 金币拍卖服
        GlobalData::s_auction_proxy->SetRunning((char)sig->m_flag);
    }

    if (sig->m_flag == 0)
        packet.put_byte(0);
    else
        packet.put_byte(1);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);

    const char* state;
    if (sig->m_flag != 0)                              // ORIG 真分支直落 "Opened"
        state = "Opened";
    else
        state = "Closed";
    const char* serverName;
    if (sig->m_result == 0)                            // ORIG data==0 直落 "Auction"
        serverName = "Auction";
    else
        serverName = "Point";

    cMyTrace(__PRETTY_FUNCTION__,
        0x3dea, 0)("%s server %s! SEND SIG_AUCTION_NOTIFY_AUCTION_SERVICE TO ALL USERS",
          serverName, state);
    return 0;
}

Inter_PointAuctionLogMessage::~Inter_PointAuctionLogMessage() {}
int Inter_PointAuctionLogMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
    {
        return 0;
    }
    SIG_AUCTION_LOG_MESSAGE_PG* message =
        reinterpret_cast<SIG_AUCTION_LOG_MESSAGE_PG*>(data);
    user->m_historyLog.AuctionPoint(message);
    return 0;
}

Inter_PointAuctionChargeCeraPoint::~Inter_PointAuctionChargeCeraPoint() {}
// ORIG 0x084d7c84 T dispatch_sig —— 纯 ret 空函数。
// 报告 docs/class_func_reports/Inter_PointAuctionChargeCeraPoint/dispatch_sig.md 全文转写（汇编 9 字节，mov $0x0,%eax; ret）。
int Inter_PointAuctionChargeCeraPoint::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    return 0;
}

Inter_AuctionResultBuyItemApiece::~Inter_AuctionResultBuyItemApiece() {}
// ORIG 0x084d7c8e T dispatch_sig —— 拍卖逐件购买结果（金币返还 + 0x14f 包）。
// 报告 docs/class_func_reports/Inter_AuctionResultBuyItemApiece/dispatch_sig.md 全文转写。
// g_auctionResultGmList（ORIG _ZL6gmList d @0x093701d8）定义已存在于
// InterDispatcher.cpp:6527（五值 GM characNo 延迟追踪表），本函数直接引用。
#pragma pack(push, 1)
// Inter_AuctionResultBuyItemApiecePktView —— Inter_AuctionResultBuyItemApiece::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AuctionResultBuyItemApiecePktView
{
    int m_0;                  // +0x00
    int m_4;                  // +0x04
    unsigned char m_8;        // +0x08
    unsigned char m_9;        // +0x09
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=130（ORIG 0x084d7c8e）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=77（ORIG 0x84d7c8e）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_AuctionResultBuyItemApiece::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionResultBuyItemApiecePktView* pPkt = reinterpret_cast<Inter_AuctionResultBuyItemApiecePktView*>(data);
    if (user == 0)
        return 0;

    if (pPkt->m_0 !=               // msg+0x0 characNo
        user->getCurCharacNo())
        return 0;

    for (int i = 0; i <= 4; ++i)                       // cmpl $4; setle
    {
        if (g_auctionResultGmList[i] == user->getCurCharacNo())
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3e12, 0)("Trace Auction Delay, %s(%d)",
                  "ACK AuctionResultBuyItemApiece",
                  user->getCurCharacNo());
            break;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0x14f);
    packet.put_byte(
        (int)pPkt->m_8);
    if (pPkt->m_8 == 0)
    {
        const int money = pPkt->m_4;  // msg+0x4
        CInventory* inven =
            static_cast<CInventory*>(user->getCurCharacInvenW());
        const int gained =
            inven->gain_money(money, (eMoneyAddReason)0x14, true, 0);
        if (gained < money)
        {
            user->SendMoneyFullReason(
                (ENUM_MONEY_FULL_REASON)0,
                (unsigned long)money, (unsigned long)gained);
        }
        packet.put_byte(
            (int)pPkt->m_9);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_LoadGameOption::~Inter_LoadGameOption() {}
// ORIG 0x084d7e84 T dispatch_sig —— 游戏选项载入（ETC/快捷聊天/表情配置 + 0xad 回包）。
// 报告 docs/class_func_reports/Inter_LoadGameOption/dispatch_sig.md 全文转写。
int Inter_LoadGameOption::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 1)                        // cmp $1; setle
        return 0;

    // ORIG 0x084d7ea6 起 4 处 `mov -0x10(%ebp),%eax` 先于参数 `movl $…,(,4|8)(%esp)`
    // —— this 装载提前 = 「struct 成员指针作调用对象」形（probe this1 e25：裸指针局部
    // 在 4.4/4.1 均产常量先、load 后形；struct 成员形产 ORIG 序）。
    struct { CETCOption* etcOpt; } etcWrap;
    etcWrap.etcOpt = reinterpret_cast<CETCOption*>(data);
    etcWrap.etcOpt->setETCOption((ENUM_OPTION_ETC)1, 1);

    CGameOption* opt = user->GetGameOptionRef();
    opt->SetEtcOption(*etcWrap.etcOpt);
    opt->SetQuickChatOption(
        *reinterpret_cast<CQuickChattingOption*>(
            reinterpret_cast<char*>(etcWrap.etcOpt) + 0x48), 0);   // ORIG 经 -0x10 槽
    opt->SetQuickChatOption(
        *reinterpret_cast<CQuickChattingOption*>(
            reinterpret_cast<char*>(etcWrap.etcOpt) + 0x174), 1);  // ORIG 经 -0x10 槽
    opt->SetEmoticonInfo(
        *reinterpret_cast<CChattingEmoticonConfig*>(
            reinterpret_cast<char*>(etcWrap.etcOpt) + 0x2a0));     // ORIG 经 -0x10 槽

    PacketGuard packet;
    packet.put_header(0, 0xad);
    packet.put_int(0x48);
    if (G_GameWorld()->IsSchoolPvPChannel())
    {
        etcWrap.etcOpt->setETCOption((ENUM_OPTION_ETC)0xa, 1);
        etcWrap.etcOpt->setETCOption((ENUM_OPTION_ETC)0xb, 0);
    }
    // ORIG 0x084d7f4x：基址取 etcOpt 槽 -0x10(%ebp)（非 data 形参 0x10）。
    packet.put_binary(reinterpret_cast<char*>(etcWrap.etcOpt), 0x48);
    packet.put_int(300);
    packet.put_binary(reinterpret_cast<char*>(etcWrap.etcOpt) + 0x48, 300);
    packet.put_int(300);
    packet.put_binary(reinterpret_cast<char*>(etcWrap.etcOpt) + 0x174, 300);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_PvPChannelInfo::~Inter_PvPChannelInfo() {}
// ORIG 0x084d8096 T dispatch_sig —— PvP 频道信息同步：组 0xc6 包逐频道回发。
// 报告 docs/class_func_reports/Inter_PvPChannelInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PvPChannelInfoPktView —— Inter_PvPChannelInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_PvPChannelInfoPktView
{
    int m_0;                  // +0x00
    char m_pad4[0x6];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    int m_12;                 // +0x12
    char m_pad16[0x1];  // 未消费
    unsigned char m_17;       // +0x17
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=132（ORIG 0x084d8096）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-6。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=94（ORIG 0x84d8096）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_PvPChannelInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PvPChannelInfoPktView* pPkt = reinterpret_cast<Inter_PvPChannelInfoPktView*>(data);
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;

    int* msg =&pPkt->m_0;   // ORIG 写回 msg+(i+1)*0x10+0x10，须非 const
    if (user->getCurCharacR() == 0 ||                // asm 双失败归并 true
        user->getCurCharacNo() !=
            pPkt->m_a)     // msg+0x0a 角色号
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0xc6);
    packet.put_byte(1);
    packet.put_int(pPkt->m_12);  // msg+0x12
    const unsigned int channelCnt =                  // msg+0x17 频道数
        static_cast<unsigned int>(pPkt->m_17);
    packet.put_byte(static_cast<int>(channelCnt));
    for (int i = 0; i < static_cast<int>(channelCnt); ++i)
    {
        packet.put_int(*(msg + (i + 1) * 4 + 2));    // msg+(i+1)*0x10+0x08
        packet.put_int(*(msg + (i + 1) * 4 + 3));    // msg+(i+1)*0x10+0x0c
        if (msg[(i + 1) * 4 + 5] == 2)               // msg+(i+1)*0x10+0x14
            msg[(i + 1) * 4 + 4] = 4;                // msg+(i+1)*0x10+0x10 写回 4
        packet.put_int(msg[(i + 1) * 4 + 4]);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_PvPChannelUserCount::~Inter_PvPChannelUserCount() {}
// ORIG 0x084d8262 T dispatch_sig —— PvP 频道用户数统计回填并转发监控服。
// 报告 docs/class_func_reports/Inter_PvPChannelUserCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PvPChannelUserCountPktView —— Inter_PvPChannelUserCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_PvPChannelUserCountPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    unsigned int m_12;              // +0x12
    unsigned int m_16;              // +0x16
    unsigned int m_1a;              // +0x1a
    unsigned int m_1e;              // +0x1e
    unsigned int m_22;              // +0x22
    unsigned int m_26;              // +0x26
};
#pragma pack(pop)

int Inter_PvPChannelUserCount::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_PvPChannelUserCountPktView* pPkt = reinterpret_cast<Inter_PvPChannelUserCountPktView*>(data);
    pPkt->m_1a =
        g_schoolMgr.GetUserCount(
            pPkt->m_12);  // ORIG mov 0x12(%eax)→GetUserCount
    pPkt->m_16 =
        G_CEnvironment()->get_channel_no();
    pPkt->m_22 =
        (unsigned int)CLeagueMatch::GetPlayCountPerOneTeam() & 0xff;
    pPkt->m_26 =
        (unsigned int)CLeagueMatch::GetBattleMode() & 0xff;
    // ORIG 0x084d82bc `mov -0xc(%ebp),%ebx`：ebx 从 pPkt 局部槽装载后跨 call
    // 传参（0x084d82de `mov %ebx,0x4(%esp)`），帧 push %ebx/sub 0x24。
    // P52：pPkt 实参值先物化并跨 GetServerProxy 调用存活（callee-saved 独占）；
    // 原 register asm("ebx") 绑定违反 P0/N96 已删，源侧以槽局部 pPkt 直传复现。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)pPkt->m_1e
        )->SendTcpPacket(reinterpret_cast<char*>(pPkt), 0x2a);
    return 0;
}

Inter_LoadBloodDungeonBestRecord::~Inter_LoadBloodDungeonBestRecord() {}
// ORIG 0x084d82f6 T dispatch_sig —— 血轮副本个人最佳记录 DB 加载。
// 报告 docs/class_func_reports/Inter_LoadBloodDungeonBestRecord/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadBloodDungeonBestRecordPktView —— Inter_LoadBloodDungeonBestRecord::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadBloodDungeonBestRecordPktView
{
    unsigned int m_0;         // +0x00
    unsigned int m_4;         // +0x04
    int m_8;                  // +0x08
    int m_c;                  // +0x0c
};
#pragma pack(pop)

int Inter_LoadBloodDungeonBestRecord::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0x3edd;
    // ORIG：包基址槽在 get_state 守卫之后物化。
    Inter_LoadBloodDungeonBestRecordPktView* pPkt = reinterpret_cast<Inter_LoadBloodDungeonBestRecordPktView*>(data);
    const int idx = user->get_charac_idx(pPkt->m_0);
    if (idx < 0)
        return 0x3ee3;
    // ORIG +0x3e 一步 `mov (%eax),%edx`（同 signedness 折叠）⇒ 不加显式转换。
    if (user->m_characList[idx].m_characNo !=
        pPkt->m_0)
        return 0x3ee5;
    user->m_characList[idx].m_characNo =
        pPkt->m_0;
    user->setBloodBestRecord(
        pPkt->m_4,
        pPkt->m_8,
        pPkt->m_c, true);
    return 0;
}

Inter_LoadBreakAwayPreventSystem::~Inter_LoadBreakAwayPreventSystem() {}
int Inter_LoadBreakAwayPreventSystem::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* packet =
        reinterpret_cast<SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER*>(data);
    GlobalData::s_BreakAwaySys->loadDbData(packet);
    return 0;
}

Inter_LoadBreakAwayPreventMessage::~Inter_LoadBreakAwayPreventMessage() {}
// ORIG 0x084d92b4 T dispatch_sig —— 防流失活动状态通知（0x111 状态门 + 0x136 活动开启包）。
// 报告 docs/class_func_reports/Inter_LoadBreakAwayPreventMessage/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadBreakAwayPreventMessagePktView —— Inter_LoadBreakAwayPreventMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadBreakAwayPreventMessagePktView
{
    unsigned int m_0;         // +0x00
    int m_4;                  // +0x04
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=67（ORIG 0x084d92b4）：寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=7（ORIG 0x84d92b4）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_LoadBreakAwayPreventMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0;

    // ORIG：包基址在 user 判空后物化入 -0x14(%ebp)
    Inter_LoadBreakAwayPreventMessagePktView* pPkt = reinterpret_cast<Inter_LoadBreakAwayPreventMessagePktView*>(data);
    const int state = pPkt->m_4;      // msg+0x4
    const unsigned int questTime =                             // msg+0x0
        pPkt->m_0;
    if (state == 0 || state == 2)
    {   // 活动开启
        PacketGuard packet;
        packet.put_header(0, 0x111);
        packet.put_byte(state);
        packet.finalize(true);
        user->Send(packet);

        PacketGuard openPacket;
        openPacket.put_header(0, 0x136);
        openPacket.put_int(0x15180);
        openPacket.put_byte(0);
        openPacket.finalize(true);
        user->Send(openPacket);

        user->setBreakAwayDungeonClearCnt(3);
        user->setBreakAwayQuestTime(questTime);
    }
    else
    {   // 仅状态通知
        PacketGuard packet;
        packet.put_header(0, 0x111);
        packet.put_byte(state);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_LoadDimensionInout::~Inter_LoadDimensionInout() {}
// ORIG 0x084d83d2 T dispatch_sig —— 维度进出标志载入（更新角色数组并回传差异）。
// 报告 docs/class_func_reports/Inter_LoadDimensionInout/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadDimensionInoutPktView —— Inter_LoadDimensionInout::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadDimensionInoutPktView
{
    unsigned int m_0;         // +0x00
    char m_pad4[0x8];  // 未消费
    unsigned char m_c;        // +0x0c
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=133（ORIG 0x084d83d2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+24。
// [BigParty] 判 **N89**（本 TU 唯一剩余物化缺口件，已定性别动）：ORIG 对
//   `std::vector<_Charac_info>::operator[](unsigned)` 发 **8 处 out-of-line call**（含
//   `mov 0xc(%ebp),%edx; add $0x796e8,%edx` 的逐次物化 8 处），本方仅 2 处 —— 但 4.4 的
//   头文件里 operator[] 恒内联，**无论怎么展开 recBase 都发不出该 out-of-line call**，
//   故非可攻缺口（Main 已把此形态并入 N89 条目）。TOT call 数 ORIG 18 / 本方 12。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=146（ORIG 0x84d83d2）：N22 cmp 操作数方向+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_LoadDimensionInout::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadDimensionInoutPktView* pPkt = reinterpret_cast<Inter_LoadDimensionInoutPktView*>(data);
    bool changed = false;
    bool ultimateChanged = false;                      // ORIG -0x15 具名 bool（movb $0 初值 + 条件置 1 + 末尾 movzbl）

    if (user->get_state() == 0)                        // test eax; sete
        return 0x3ef3;

    // §P0：ORIG 对 pPkt->m_0 全程寄存器直传（无槽往返）⇒ 原码无 characNo 具名局部，三处内联。
    const int idx = user->get_charac_idx(pPkt->m_0);
    if (idx < 0)                                       // jns
        return 0x3ef9;

    if (user->m_characList[idx].m_characNo != pPkt->m_0)
        return 0x3efb;

    // §P183：ORIG 每处字段访问重发 vector<_Charac_info>::operator[]（8 处 out-of-line call，
    //   我方 recBase 缓存只发 2 处）⇒ 逐点重物化 `&user->m_characList[idx]`，不缓存基址。
    user->m_characList[idx].m_characNo = pPkt->m_0;
#define REC_BYTE(off) \
    (*(reinterpret_cast<unsigned char*>(&user->m_characList[idx]) + (off)))
    for (int i = 0; i <= 5; ++i)                       // cmpl $5; setle
    {
        REC_BYTE(0xeb9 + i) =                          // +0xeb9 m_demensionInout[i]
            *reinterpret_cast<unsigned char*>(data + 4 + i);
        // §P0：ORIG got 值留 %ebx 跨 G_CDataManager call 保活（无槽）⇒ 内联比较表达式。
        if (REC_BYTE(0xeb9 + i) !=
            (char)G_CDataManager()->get_dimensionInout(i))
            changed = true;
    }

    REC_BYTE(0xec2) = pPkt->m_c;                       // +0xec2 m_ultimateInout[2]
    if (REC_BYTE(0xec2) !=
        (char)G_CDataManager()->get_limit_inout_count(ENUM_BLOOD_TYPE_2))
        ultimateChanged = true;
    REC_BYTE(0xeb8) = 0;                               // +0xeb8 m_saveDemensionInout
    REC_BYTE(0xebf) = 0;                               // +0xebf m_saveBloodInout
#undef REC_BYTE

    if (user->getCurCharacR() != 0 && user->IsFirstTimeLogin() &&
        user->get_charac_level() > 0x3b)
    {
        user->DimensionInoutUpdate(changed, ultimateChanged);
    }
    return 0;
}

Inter_BlackIPMonitorReq::~Inter_BlackIPMonitorReq() {}
// ORIG 0x084d8608 T dispatch_sig —— 黑名单 IP 监控请求：触发 DB 加载并打日志。
// 报告 docs/class_func_reports/Inter_BlackIPMonitorReq/dispatch_sig.md 全文转写。
int Inter_BlackIPMonitorReq::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    DB_BlackIPMonitor::makeRequest();
    cMyTrace(__PRETTY_FUNCTION__, 0x3f28, 0)("Get BlackIPMonitor Request");
    return 0;
}

Inter_BlackIPMonitor::~Inter_BlackIPMonitor() {}
// ORIG 0x084d8650 T dispatch_sig —— 黑名单 IP 惩罚表全量装载。
// 报告 docs/class_func_reports/Inter_BlackIPMonitor/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_BlackIPMonitorPktView —— Inter_BlackIPMonitor::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_BlackIPMonitorPktView
{
    int m_0;                  // +0x00
    char m_pad4[0xc80];  // 未消费
    char m_c84;               // +0xc84
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_BlackIPMonitorRecWinView —— Inter_BlackIPMonitor::dispatch_sig 窗口 rec 元素视图
//   rec 基址 = data + (i*2+1)*0x10（ORIG 0x84d8693 寻址链 `add %edx,%edx; add $0x1,%edx;
//   shl $0x4,%edx; add %edx,%eax; add $0xK,%eax` 直证；记录 stride 0x20，本视图取后半单元，
//   故字段偏移 = 记录偏移 - 0x10）。字段宽度=ORIG 访问指令。
struct Inter_BlackIPMonitorRecWinView
{
    char m_pad0[0x4];  // 未消费（记录 +0x10..+0x13）
    unsigned int m_4;         // +0x04 ← 记录 +0x14
    unsigned int m_8;         // +0x08 ← 记录 +0x18
    long m_c;                 // +0x0c ← 记录 +0x1c
    long m_10;                // +0x10 ← 记录 +0x20
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=100（ORIG 0x084d8650）：槽序/帧差(N82/P212/P221)+混合残差；gap+22。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=126（ORIG 0x84d8650）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [b28d-solo2] P0 重判：ORIG 4 个字段值仅 endTime 落 spill 槽(-0x3c)、startTime/idCnt/punishType
//   走 %edi/%esi/%ebx callee-saved ⇒ 原码无 4 个具名局部（全内联实参，右到左求值序
//   m_20→m_1c→m_18→m_14 与 ORIG 逐槽一致）；循环后取值用循环变量 `i-1`（ORIG 0x84d8739
//   `lea -0x1(%eax),%edx` 直证，非重读 m_0-1）；trace 用全表达式临时链使全局值跨 ctor 保活
//   （ORIG 0x84d8753 `mov &g_...,%ebx` 在 ctor 之前，P328⑩/P227）。
int Inter_BlackIPMonitor::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_BlackIPMonitorPktView* pPkt = reinterpret_cast<Inter_BlackIPMonitorPktView*>(data);
    // ORIG 0x84d865f/0x84d8681 两次 `movl $0x0,-0x1c` ⇒ 原码 `int i = 0;` + `for (i = 0; …)`
    // 双初始化形（P191 配套）。
    int i = 0;
    if (pPkt->m_c84 != '\0')
        GlobalData::g_blackIPMonitor->reset();

    for (i = 0; i < pPkt->m_0; ++i)
    {
#define pData (reinterpret_cast<char*>(pPkt))
#define pRec ((Inter_BlackIPMonitorRecWinView*)(pData + (i * 2 + 1) * 0x10))
        GlobalData::g_blackIPMonitor->insertNewIP(
            inet_addr(pData + (i << 5) + 4),
            (WongWork::ENUM_BLACKIPMONITOR_PUNISH_TYPE)pRec->m_4,
            pRec->m_8, pRec->m_c, pRec->m_10);
#undef pRec
#undef pData
    }

    GlobalData::g_IPMonitorLastDBAccessTime =
        ((Inter_BlackIPMonitorRecWinView*)(reinterpret_cast<char*>(pPkt) +
                                           ((i - 1) * 2 + 1) * 0x10))->m_c;
    cMyTrace(__PRETTY_FUNCTION__, 0x3f5b, 0)(
        "IPMonitorPunish LAST LOAD TIME : %d",
        GlobalData::g_IPMonitorLastDBAccessTime);
    return 0;
}

Inter_BlackIPMonitorPartLoadReq::~Inter_BlackIPMonitorPartLoadReq() {}
// ORIG 0x084d87a0 T dispatch_sig —— 黑名单 IP 分批加载请求。
// 报告 docs/class_func_reports/Inter_BlackIPMonitorPartLoadReq/dispatch_sig.md 全文转写。
int Inter_BlackIPMonitorPartLoadReq::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    DB_BlackIPMonitorPartLoad::makeRequest();
    cMyTrace(__PRETTY_FUNCTION__,
        0x3f63, 0)("Get BlackIPMonitor Request Part Load");
    return 0;
}

Inter_BlackIPMonitorDeleteIP::~Inter_BlackIPMonitorDeleteIP() {}
// ORIG 0x084d87e8 T dispatch_sig —— 黑名单 IP 表批量删除。
// 报告 docs/class_func_reports/Inter_BlackIPMonitorDeleteIP/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_BlackIPMonitorDeleteIPPktView —— Inter_BlackIPMonitorDeleteIP::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_BlackIPMonitorDeleteIPPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_BlackIPMonitorDeleteIPRecWinView —— Inter_BlackIPMonitorDeleteIP::dispatch_sig 窗口 rec 元素视图
//   （rec 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_BlackIPMonitorDeleteIPRecWinView
{
    char m_pad0[0x1e];  // 未消费
    unsigned int m_1e;        // +0x1e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=84（ORIG 0x084d87e8）：混合残差；gap+18。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=90（ORIG 0x84d87e8）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→-2（87 vs 89，TU 121/1331/5/133 不回退）：真修三处——
//   ① 循环变量 `unsigned int i = 0;` 前置预清 + `for (i=0;...)` 双初始化（复现 ORIG `movl $0,-0xc`×2，P191）；
//   ② 两个 cMyTrace 改全表达式临时链（P221/P227，m_a 在 ctor 前落 %ebx）；
//   ③ 保留 RecWinView 具名视图（P94/P235）。残余=ORIG 常量拆分 `add $0x10; mov 0xe(%eax)` vs 4.4 单 disp（P30/N4）。
int Inter_BlackIPMonitorDeleteIP::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_BlackIPMonitorDeleteIPPktView* pPkt = reinterpret_cast<Inter_BlackIPMonitorDeleteIPPktView*>(data);
    unsigned int i = 0;                                // ORIG 0x084d87f6 预清 -0xc + 循环前再清零（P191 双初始化）
    cMyTrace(__PRETTY_FUNCTION__, 0x3f6d, 0)("Delete IP Cnt : %d", pPkt->m_a);

    for (i = 0; i < pPkt->m_a; ++i)
    {
        if (GlobalData::g_blackIPMonitor->deleteIP(data + i * 0x14 + 0xe,
                                                   ((Inter_BlackIPMonitorDeleteIPRecWinView*)(data + i * 0x14))->m_1e) != '\0')
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x3f76, 0)(
                "Delete IP: %s, Type: %d", data + i * 0x14 + 0xe,
                ((Inter_BlackIPMonitorDeleteIPRecWinView*)(data + i * 0x14))->m_1e);
        }
    }
    return 0;
}

Inter_LoadAutoMarketConditionsControl::~Inter_LoadAutoMarketConditionsControl() {}
int Inter_LoadAutoMarketConditionsControl::dispatch_sig(
    CUser* /*user*/, char* data, int /*len*/)
{
    SIG_AUTO_MARKET_CONDITIONS_CONTROL* packet =
        reinterpret_cast<SIG_AUTO_MARKET_CONDITIONS_CONTROL*>(data);
    G_CDataManager()->GetAutoMarketContitionsControl()->LoadDatabase(packet);
    return 0;
}

Inter_UpdateOneDayLethe::~Inter_UpdateOneDayLethe() {}
// ORIG 0x084d894c T dispatch_sig —— 一日忘却（OneDayLethe）状态更新：按
// SIG+0x4 的状态机字节切换 USE/SKILL_CONFIRM/INIT/NOT_USE 四分支。
// 报告 docs/class_func_reports/Inter_UpdateOneDayLethe/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_UpdateOneDayLethePktView —— Inter_UpdateOneDayLethe::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_UpdateOneDayLethePktView
{
    int m_0;                  // +0x00
    unsigned char m_4;        // +0x04
    char m_pad5[0x198];  // 未消费
    unsigned char m_19d;      // +0x19d（ORIG movzbl 装载+moval 存储：无符号字节）
    signed char m_19e;        // +0x19e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=289（ORIG 0x084d894c）：寄存器中转(N18a/N7)+混合残差；gap-5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=101（ORIG 0x84d894c）：N22 cmp 操作数方向+N18a 单段 regmove。
int Inter_UpdateOneDayLethe::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_UpdateOneDayLethePktView* pPkt = reinterpret_cast<Inter_UpdateOneDayLethePktView*>(data);
    if (pPkt->m_4 ==
        CPremiumLetheManager::USE_LETHE_STATE)
    {
        if (pPkt->m_0 != 0)        // sig+0x0 characNo
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3f8f, 0)("ONE_DAY_LETHE : INTER_USE_LETHE, char(%s)",
                  user->getCurCharacName());

            if (pPkt->m_19e == -1 ||
                pPkt->m_19e == 0)
            {
                user->SetOneDayLetheFlag(
                    CPremiumLetheManager::USE_LETHE_STATE);
                user->SaveBackupSkillInfo(
                    reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                        reinterpret_cast<char*>(pPkt) + 5),
                    (ENUM_SKILL_TREE_KIND)0);
            }
            else
            {
                user->SetOneDayLetheFlag2ND(
                    CPremiumLetheManager::USE_LETHE_STATE);
                user->SaveBackupSkillInfo(
                    reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                        reinterpret_cast<char*>(pPkt) + 5),
                    (ENUM_SKILL_TREE_KIND)1);
            }

            PacketGuard packet;                        // -0x3c
            if (user->GetCurCharacSkillTreeIndex() == -1)
            {
                if (pPkt->m_19e == 0)
                {
                    packet.put_header(0, 0xb4);
                    packet.put_byte(2);
                    packet.put_byte(1);
                    packet.finalize(true);
                    user->Send(packet);
                }
            }
            else
            {
                char state = 0;                        // -0x9（ORIG 084d8aeb movb $0x0）
                if (pPkt->m_19e == -1 ||
                    pPkt->m_19e == 0)
                    state = 1;
                else
                    state = 2;
                packet.put_header(0, 0xb4);
                packet.put_byte(2);
                packet.put_byte((int)state);           // movsbl 有符号扩展
                packet.finalize(true);
                user->Send(packet);
            }
        }
    }
    else if (pPkt->m_4 ==
             CPremiumLetheManager::SKILL_CONFIRM_STATE)
    {
        if (pPkt->m_0 != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__,
                0x3fbd, 0)("ONE_DAY_LETHE : INTER_CONFIRM_SKILL_STATE, char(%s)",
                  user->getCurCharacName());

            WongWork::CHandlePremium::handleRemoveOneDayLethePremium(user);

            if (pPkt->m_19e == -1 ||
                pPkt->m_19e == 0)
            {
                user->SetOneDayLetheFlag(
                    CPremiumLetheManager::NOT_USE_LETHE_STATE);
                pPkt->m_19d =
                    CPremiumLetheManager::SKILL_CONFIRM_STATE;
                user->SaveBackupSkillInfo(
                    reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                        reinterpret_cast<char*>(pPkt) + 5),
                    (ENUM_SKILL_TREE_KIND)0);
            }
            else
            {
                user->SetOneDayLetheFlag2ND(
                    CPremiumLetheManager::NOT_USE_LETHE_STATE);
                pPkt->m_19d =
                    CPremiumLetheManager::SKILL_CONFIRM_STATE;
                user->SaveBackupSkillInfo(
                    reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                        reinterpret_cast<char*>(pPkt) + 5),
                    (ENUM_SKILL_TREE_KIND)1);
            }
        }
    }
    else if (pPkt->m_4 ==
             CPremiumLetheManager::INIT_LETHE_SKILL)
    {
        if (pPkt->m_19e == -1 ||
            pPkt->m_19e == 0)
        {
            user->SetOneDayLetheFlag(
                CPremiumLetheManager::USE_LETHE_STATE);
            pPkt->m_19d =
                CPremiumLetheManager::USE_LETHE_STATE;
            user->SaveBackupSkillInfo(
                reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                    reinterpret_cast<char*>(pPkt) + 5),
                (ENUM_SKILL_TREE_KIND)0);
        }
        else
        {
            user->SetOneDayLetheFlag2ND(
                CPremiumLetheManager::USE_LETHE_STATE);
            pPkt->m_19d =
                CPremiumLetheManager::USE_LETHE_STATE;
            user->SaveBackupSkillInfo(
                reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                    reinterpret_cast<char*>(pPkt) + 5),
                (ENUM_SKILL_TREE_KIND)1);
        }
    }
    else if (pPkt->m_4 ==
             CPremiumLetheManager::NOT_USE_LETHE_STATE)
    {
        if (pPkt->m_19e == -1 ||
            pPkt->m_19e == 0)
        {
            user->SetOneDayLetheFlag(
                CPremiumLetheManager::NOT_USE_LETHE_STATE);
            pPkt->m_19d =
                CPremiumLetheManager::NOT_USE_LETHE_STATE;
            user->SaveBackupSkillInfo(
                reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                    reinterpret_cast<char*>(pPkt) + 5),
                (ENUM_SKILL_TREE_KIND)0);
        }
        else
        {
            user->SetOneDayLetheFlag2ND(
                CPremiumLetheManager::NOT_USE_LETHE_STATE);
            pPkt->m_19d =
                CPremiumLetheManager::NOT_USE_LETHE_STATE;
            user->SaveBackupSkillInfo(
                reinterpret_cast<SIG_LOAD_PREMIUM_LETHE*>(
                    reinterpret_cast<char*>(pPkt) + 5),
                (ENUM_SKILL_TREE_KIND)1);
        }
    }
    return 0;
}

Inter_DisconnectMonitorServer::~Inter_DisconnectMonitorServer() {}
#pragma pack(push, 1)
// Inter_DisconnectMonitorServerPktView —— Inter_DisconnectMonitorServer::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DisconnectMonitorServerPktView
{
    int m_0;                  // +0x00
};
#pragma pack(pop)

int Inter_DisconnectMonitorServer::dispatch_sig(
    CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG +0x06..+0x11：仅一个 int* 局部槽（-0xc），`*packet` 直接作实参
    // （再具名 int 局部会多两级落栈）。
    int* packet = reinterpret_cast<int*>(data);
    TimerConnectMonitorServer::registNextTimer(30L, 0, *packet);
    return 0;
}

Inter_ResposncePCRoomPlyerList::~Inter_ResposncePCRoomPlyerList() {}
// ORIG 0x084d9cbe T dispatch_sig —— 网吧在线玩家列表下发：角色号校验 + 0x25 字节
// stride 条目（名字/短值/属性字节）打包。
// 报告 docs/class_func_reports/Inter_ResposncePCRoomPlyerList/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ResposncePCRoomPlyerListPktView —— Inter_ResposncePCRoomPlyerList::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ResposncePCRoomPlyerListPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_ResposncePCRoomPlyerListEntryWinView —— Inter_ResposncePCRoomPlyerList::dispatch_sig 窗口 entry 元素视图
//   （entry 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_ResposncePCRoomPlyerListEntryWinView
{
    char m_pad0[0x25];  // 未消费
    unsigned char m_25;       // +0x25
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=270（ORIG 0x084d9cbe）：寄存器中转(N18a/N7)+混合残差；gap+47。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=163（ORIG 0x84d9cbe）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_ResposncePCRoomPlyerList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    PacketGuard packet;
    if (user->GetPCRoomNo() == 0)                      // 非网吧用户回执拒绝
    {
        packet.put_header(1, 0xd8);
        packet.put_byte(0);
        packet.put_byte(3);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    Inter_ResposncePCRoomPlyerListPktView* pPkt = reinterpret_cast<Inter_ResposncePCRoomPlyerListPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x4192,
            "Inter_ResposncePCRoomPlyerList::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    if (pPkt->m_12 > 0x28)   // 条目数越界
    {
        packet.put_header(1, 0xd8);
        packet.put_byte(0);
        packet.put_byte(3);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    packet.put_header(1, 0xd8);
    packet.put_byte(1);
    packet.put_byte((int)pPkt->m_12);

    // ORIG 0x84d9ed4/0x84d9f09 等 7 处 `add -0x14(%ebp),%eax`：循环体基址走 pPkt 具名槽。
#define pData (reinterpret_cast<char*>(pPkt))
    for (int i = 0; i < (int)pPkt->m_12; ++i)
    {
        const int nameLen = strlen(pData + i * 0x25 + 0x10 + 0x7);
        packet.put_int(nameLen);
        packet.put_str(pData + i * 0x25 + 0x10 + 0x7, nameLen);
        packet.put_short((int)*(short*)(pData + i * 0x25 + 0x10 + 0x5));
        packet.put_byte((int)*(signed char*)(pData + i * 0x25 + 0x10 + 0x3));
        packet.put_byte((int)*(signed char*)(pData + i * 0x25 + 0x10 + 0x4));
        packet.put_byte((int)((Inter_ResposncePCRoomPlyerListEntryWinView*)(pData + i * 0x25 + 0x10))->m_25);
        packet.put_byte((int)*(signed char*)(pData + i * 0x25 + 0x10 + 0x26));
        packet.put_byte((int)*(signed char*)(pData + i * 0x25 + 0x10 + 0x27));
    }
#undef pData
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_ResposncePCRoomPlyerCount::~Inter_ResposncePCRoomPlyerCount() {}
// ORIG 0x084da0aa T dispatch_sig —— 网吧玩家数应答转发。
// 报告 docs/class_func_reports/Inter_ResposncePCRoomPlyerCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ResposncePCRoomPlyerCountPktView —— Inter_ResposncePCRoomPlyerCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ResposncePCRoomPlyerCountPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e
    int m_12;                       // +0x12
};
#pragma pack(pop)

int Inter_ResposncePCRoomPlyerCount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_ResposncePCRoomPlyerCountPktView* pPkt = reinterpret_cast<Inter_ResposncePCRoomPlyerCountPktView*>(data);

    // ORIG：同 ReplyGuildLetter。
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x41cc,
            "Inter_ResponsePCRoomPlayerCount::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);   // ORIG 一步装载，同上
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0xd9);
    packet.put_byte(1);
    packet.put_byte(pPkt->m_12);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_NoticePCRoomPlayerLoginOut::~Inter_NoticePCRoomPlayerLoginOut() {}
// ORIG 0x084da1fa T dispatch_sig —— PC 房玩家登出通知：校验后组 0xba 包回发。
// 报告 docs/class_func_reports/Inter_NoticePCRoomPlayerLoginOut/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePCRoomPlayerLoginOutPktView —— Inter_NoticePCRoomPlayerLoginOut::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticePCRoomPlayerLoginOutPktView
{
    char m_pad0[0xa];  // 未消费
    signed char m_a;          // +0x0a
    char m_padb[0x4];  // 未消费
    unsigned int m_f;         // +0x0f（同上 unsigned→int 失配两步 spill 形态）
    unsigned char m_13;       // +0x13
    char m_14[1];             // +0x14 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_NoticePCRoomPlayerLoginOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x10(%ebp)
    Inter_NoticePCRoomPlayerLoginOutPktView* pPkt = reinterpret_cast<Inter_NoticePCRoomPlayerLoginOutPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_f)         // msg+0x0f 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x41eb,
            "PCRoom : Inter_NoticePCRoomPlayerLoginOut::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_f);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0xba);
    packet.put_byte(                                 // msg+0x0a（movsbl 符号扩展）
        static_cast<int>(pPkt->m_a));
    packet.put_byte(                                 // msg+0x13（movzbl 零扩展）
        static_cast<int>(pPkt->m_13));
    const int playerNameLen =                        // msg+0x14 玩家名
        static_cast<int>(strlen(pPkt->m_14));
    packet.put_int(playerNameLen);
    packet.put_str(pPkt->m_14, playerNameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_NoticePowerWarRank::~Inter_NoticePowerWarRank() {}
// ORIG 0x084d4f46 T dispatch_sig —— 力量战争前三名排行加载。
// 报告 docs/class_func_reports/Inter_NoticePowerWarRank/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePowerWarRankPktView —— Inter_NoticePowerWarRank::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarRankPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    unsigned int m_e;               // +0x0e
    unsigned int m_12;              // +0x12
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=38（ORIG 0x084d4f46）：混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：P227 全表达式临时链 + 删 (int)
// 冗余 cast + SetPowerWarUserRankingInfo 实参改 `&pPkt->m_a`（走槽读））。
int Inter_NoticePowerWarRank::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarRankPktView* pPkt = reinterpret_cast<Inter_NoticePowerWarRankPktView*>(data);
    // P227：ORIG 在 ctor 前把三字段载入 %edi/%esi/%ebx 跨 ctor 保活 ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x368e, 0)(
        "RECV Packet_Notice_Power_War_Rank ranker 1:%d, 2:%d, 3:%d",
        pPkt->m_a,
        pPkt->m_e,
        pPkt->m_12);
    GlobalData::s_power_manager->SetPowerWarUserRankingInfo(
        reinterpret_cast<unsigned int*>(&pPkt->m_a));
    return 0;
}

Inter_LoadPowerWarStatueRanker::~Inter_LoadPowerWarStatueRanker() {}
#pragma pack(push, 1)
// Inter_LoadPowerWarStatueRankerPktView —— Inter_LoadPowerWarStatueRanker::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadPowerWarStatueRankerPktView
{
    // ORIG +0x0f/+0x11 两步装载且求值先于 this：signedness 失配的数组 decay 实参
    // （`int[N]` → `unsigned int*` 形参，偏移 0）形态，探针 g1 实证。
    int m_charNos[1];       // +0x00 排行角色号表
};
#pragma pack(pop)

int Inter_LoadPowerWarStatueRanker::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadPowerWarStatueRankerPktView* pPkt =
        reinterpret_cast<Inter_LoadPowerWarStatueRankerPktView*>(data);
    GlobalData::s_power_manager->SetPowerWarUserRankingInfo(
        reinterpret_cast<unsigned int*>(pPkt->m_charNos));
    return 0;
}

Inter_LoadPowerWarStatueInfo::~Inter_LoadPowerWarStatueInfo() {}
int Inter_LoadPowerWarStatueInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    SIG_LOAD_POWER_WAR_STATUE_INFO* packet =
        reinterpret_cast<SIG_LOAD_POWER_WAR_STATUE_INFO*>(data);
    GlobalData::s_power_manager->SetPowerWarUserStatueInfo(packet);
    return 0;
}

Inter_UpdateServerMessage::~Inter_UpdateServerMessage() {}
// ORIG 0x084da3e6 T dispatch_sig —— 全服公告转发：type 0 → 0xc0 包全频道广播；
// type 1 → Packet_Monitor_Server_Message_Info 上报 monitor 服务器。
// 报告 docs/class_func_reports/Inter_UpdateServerMessage/dispatch_sig.md 全文转写。
// msg 布局：+4 sourceChannel(int) +8 kind(u8) +9 index(u8) +0xc msgLen(int) +0x10 msg
//           +0x44 nameLen(int) +0x48 name +0x5c time(int)。
#pragma pack(push, 1)
// Inter_UpdateServerMessagePktView —— Inter_UpdateServerMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_UpdateServerMessagePktView
{
    char m_pad0[0x4];  // 未消费
    int m_4;                  // +0x04
    unsigned char m_8;        // +0x08
    unsigned char m_9;        // +0x09
    char m_pada[0x2];  // 未消费
    int m_msgLen;             // +0x0c
    char m_10[1];             // +0x10 串/字节区首元素（容量未建模）
    char m_pad11[0x33];  // 未消费
    int m_nameLen;            // +0x44
    char m_48[1];             // +0x48 串/字节区首元素（容量未建模）
    char m_pad49[0x13];  // 未消费
    int m_time;               // +0x5c
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=160（ORIG 0x084da3e6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=164（ORIG 0x84da3e6）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_UpdateServerMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_UpdateServerMessagePktView* pPkt = reinterpret_cast<Inter_UpdateServerMessagePktView*>(data);
    PacketGuard packet;
    if (pPkt->m_8 == 0)
    {   // 本服广播：0xc0 包
        packet.put_header(0, 0xc0);
        packet.put_byte(1);
        packet.put_byte(1);
        packet.put_byte((int)pPkt->m_9);
        packet.put_int(pPkt->m_msgLen);
        packet.put_str(pPkt->m_10, pPkt->m_msgLen);
        packet.put_int(pPkt->m_nameLen);
        packet.put_str(pPkt->m_48, pPkt->m_nameLen);
        packet.finalize(true);
        G_GameWorld()->send_all_with_state(packet, 3);
    }
    else if (pPkt->m_8 == 1)
    {   // 跨服转发：组 monitor 包
        Packet_Monitor_Server_Message_Info info;
        info.m_channel = (char)G_CEnvironment()->get_channel_no();
        info.m_kind = (char)pPkt->m_8;
        info.m_index = (char)pPkt->m_9;
        info.m_msgLen = pPkt->m_msgLen;
        info.m_nameLen = pPkt->m_nameLen;
        info.m_time = pPkt->m_time;
        strncpy(info.m_msg, pPkt->m_10, 0x32);
        strncpy(info.m_name, pPkt->m_48, 0x14);

        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendTcpPacket((char*)&info, 0x5f);

        // P328⑩/P227：ORIG 0x84da5eb..0x84da65f 先求 8 个实参（time→%edi、name→-0xbc、
        // nameLen→-0xb8、msg→-0xb4、msgLen→-0xb0、index→-0xac、kind→%esi、ch→%ebx）
        // 再 ctor ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x42a0, 0)(
            "SERVER_MESSAGE : Send ServerMessage to monitor server(source_channel:%d, kind:%d, index:%d, msg_len:%d, msg:%s, name_len:%d, name:%s, time:%d",
            pPkt->m_4,
            (int)pPkt->m_8,
            (int)pPkt->m_9,
            pPkt->m_msgLen, data + 0x10,
            pPkt->m_nameLen, data + 0x48,
            pPkt->m_time);
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x42a6, 0)("SERVER_MESSAGE : NOT EXIST SHARE TYPE");
    }
    return 0;
}

Inter_LoadServerMessage::~Inter_LoadServerMessage() {}
// ORIG 0x084daeee T dispatch_sig —— 事件服公告写入共享服消息管理器。
// 报告 docs/class_func_reports/Inter_LoadServerMessage/dispatch_sig.md 全文转写。
int Inter_LoadServerMessage::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG +0x06：包基址先落 -0x10 具名局部（在 G_CGameManager 调用之前）。
    SIG_EVENT_SERVER_MESSAGE* pPkt = reinterpret_cast<SIG_EVENT_SERVER_MESSAGE*>(data);
    CSharedServerMessageManager* mgr = G_CGameManager()->GetSharedServerMessageManager();
    // ORIG +0x15：`cmpl $0x0,-0xc(%ebp)`+`jne body`+早退在 fall-through ⇒ == 0 写法。
    if (mgr == 0)
        return 0;
    mgr->SetServerMessage(pPkt);
    return 0;
}

Inter_NoticeServerMessageInfo::~Inter_NoticeServerMessageInfo() {}
// ORIG 0x084da726 T dispatch_sig —— 共享服务器公告下发（0xc0 包，全网 state3 广播）。
// 报告 docs/class_func_reports/Inter_NoticeServerMessageInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeServerMessageInfoPktView —— Inter_NoticeServerMessageInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticeServerMessageInfoPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_a;        // +0x0a
    unsigned char m_b;        // +0x0b
    unsigned char m_c;        // +0x0c
    int m_d;                  // +0x0d
    char m_11[1];             // +0x11 串/字节区首元素（容量未建模）
    char m_pad12[0x31];  // 未消费
    unsigned int m_43;        // +0x43
    char m_47[1];             // +0x47 串/字节区首元素（容量未建模）
    char m_pad48[0x13];  // 未消费
    int m_5b;                 // +0x5b
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=156（ORIG 0x084da726）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=157（ORIG 0x84da726）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_NoticeServerMessageInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticeServerMessageInfoPktView* pPkt = reinterpret_cast<Inter_NoticeServerMessageInfoPktView*>(data);
    const char* const local_24 = data;
    CSharedServerMessageManager* local_20 =
        G_CGameManager()->GetSharedServerMessageManager();
    if (local_20 != 0)
    {
        const unsigned char sourceChannel =
            pPkt->m_a;
        if (sourceChannel != G_CEnvironment()->get_channel_no())
        {
            local_20->UpdateServerMessageByBroadcast(
                (int)pPkt->m_c,
                const_cast<char*>(data + 0x11),
                const_cast<char*>(data + 0x47),
                pPkt->m_5b);
        }
        cMyTrace(__PRETTY_FUNCTION__,
            0x42be, 0)("SERVER_MESSAGE : Recv ServerMessage from monitor server(source_channel:%d, kind:%d, index:%d, msg_len:%d, msg:%s, name_len:%d, name:%s, time:%d",
              (unsigned int)pPkt->m_a,
              (unsigned int)pPkt->m_b,
              (unsigned int)pPkt->m_c,
              pPkt->m_d,
              data + 0x11,
              pPkt->m_43,
              data + 0x47,
              pPkt->m_5b);
    }

    PacketGuard packet;
    packet.put_header(0, 0xc0);
    packet.put_byte(1);
    packet.put_byte(1);
    packet.put_byte(
        (unsigned int)pPkt->m_c);
    packet.put_int(pPkt->m_d);
    packet.put_str(pPkt->m_11, pPkt->m_d);
    packet.put_int(pPkt->m_43);
    packet.put_str(pPkt->m_47, pPkt->m_43);
    packet.finalize(true);
    G_GameWorld()->send_all_with_state(packet, 3);
    return 0;
}

Inter_NoticeGuildMemo::~Inter_NoticeGuildMemo() {}
// ORIG 0x084dad96 T dispatch_sig —— 公会 memo 通知双字符串转发。
// 报告 docs/class_func_reports/Inter_NoticeGuildMemo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeGuildMemoPktView —— Inter_NoticeGuildMemo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticeGuildMemoPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    char m_30[1];             // +0x30 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=32（ORIG 0x084dad96）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=60（ORIG 0x84dad96）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
// [b28d-PoolInterDisp] verdict=NEAR ndiff=2（ORIG 0x84dad96）：memoLen 单变量复用（ORIG
// 84dae22/84dae61 同槽 -0xc 两次写实证）消 4B 多占槽 ⇒ 全族槽号对齐，60→2；残 = N22
// get_charac_no(int) cmp 方向（六形封顶，同 PowerWarProcessInfo）。
int Inter_NoticeGuildMemo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_NoticeGuildMemoPktView* pPkt = reinterpret_cast<Inter_NoticeGuildMemoPktView*>(data);
    // [b28d-lane2] N22 判词推翻（探针 /tmp/l2p/p2.cpp fC）：ORIG `mov 0xe(%eax),%ebx;
    // cmp %eax,%ebx` 靠「字段读内联进比较」复现；具名（register）局部必产反向 dst。
    if (pPkt->m_e != user->get_charac_no(-1))
        return 0;

    PacketGuard packet;
    packet.put_header(0, 0xc5);
    // ORIG 两段 memoLen 共用槽 -0xc（84dae22/84dae61 同址写）⇒ 单变量跨两段复用；
    // 旧双声明多占一槽，使 pPkt/packet 整体下移 4（N82 假性帧差主项）。
    int memoLen = (int)strlen(pPkt->m_12);              // memo 串 1
    packet.put_int(memoLen);
    packet.put_str(pPkt->m_12, memoLen);
    memoLen = (int)strlen(pPkt->m_30);                  // memo 串 2（重入同槽）
    packet.put_int(memoLen);
    packet.put_str(pPkt->m_30, memoLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_PowerWarProcessInfo::~Inter_PowerWarProcessInfo() {}
// ORIG 0x084daf30 T dispatch_sig —— 权力战进行信息下发。
// 报告 docs/class_func_reports/Inter_PowerWarProcessInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PowerWarProcessInfoPktView —— Inter_PowerWarProcessInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_PowerWarProcessInfoPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
    int m_16;                       // +0x16
    int m_1a;                       // +0x1a
    int m_1e;                       // +0x1e
    int m_22;                       // +0x22
};
#pragma pack(pop)

// [b28d-lane2] verdict=NEAR ndiff=1（ORIG 0x084daf30）：寄存器中转(N18a/N7)；gap0。
// [b28d-solo-InterDispatcher] verdict=NEAR ndiff=2（ORIG 0x84daf30）：N22——有参 callee
// get_charac_no(int) 下 4.4 恒 dst=call 结果；int 同型化/书写序交换/语句表达式/具名中转
// 六形实测均不翻（无参 callee 件同旋钮已归零，见 ReqAddPvPBuddy）⇒ 封顶。
int Inter_PowerWarProcessInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PowerWarProcessInfoPktView* pPkt = reinterpret_cast<Inter_PowerWarProcessInfoPktView*>(data);
    // [b28d-lane2] N22 判词推翻（探针 /tmp/l2p/p2.cpp fA..fF）：ORIG `mov 0xe(%eax),%ebx;
    // cmp %eax,%ebx` 只在「字段读**内联进比较**、无具名局部」时复现；具名（含 register）
    // 局部必产 `mov mem,%eax; mov %eax,%ebx`。属 P0（无槽往返 ⇒ 原码无该局部）。
    if (pPkt->m_e != user->get_charac_no(-1))
        return 0;

    user->SetPowerWarProcessInfoLastUpdateTime(
        GlobalData::s_systemTime_.getCurSec());

    PacketGuard packet;
    packet.put_header(0, 0xc9);
    packet.put_int(pPkt->m_12);
    packet.put_int(pPkt->m_16);
    packet.put_int(pPkt->m_1a);
    packet.put_int(pPkt->m_1e);
    packet.put_int(pPkt->m_22);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_PowerWarEndInfo::~Inter_PowerWarEndInfo() {}
// ORIG 0x084db070 T dispatch_sig —— 权力战结束信息：胜方判定后转发战果统计。
// 报告 docs/class_func_reports/Inter_PowerWarEndInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PowerWarEndInfoPktView —— Inter_PowerWarEndInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_PowerWarEndInfoPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    unsigned char m_12;             // +0x12
    int m_13;                       // +0x13
    int m_17;                       // +0x17
    int m_1b;                       // +0x1b
    int m_1f;                       // +0x1f
    int m_23;                       // +0x23
};
#pragma pack(pop)

int Inter_PowerWarEndInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PowerWarEndInfoPktView* pPkt = reinterpret_cast<Inter_PowerWarEndInfoPktView*>(data);

    if (pPkt->m_e != user->get_charac_no(-1))
        return 0;

    PacketGuard packet;
    packet.put_header(0, 0xdf);
    packet.put_byte((unsigned int)pPkt->m_12);
    packet.put_int(pPkt->m_13);
    packet.put_int(pPkt->m_17);
    packet.put_int(pPkt->m_1b);
    packet.put_int(pPkt->m_1f);
    packet.put_int(pPkt->m_23);

    if (GlobalData::s_power_manager->GetWinnerSide() == user->getPowerSide())
    {   // 与 ORIG +0xa6a4 对应（CDataManager 胜方附加字段，暂无具名成员）
        packet.put_int(G_CDataManager()->m_powerParameter.m_basicPowerWarPoint);
    }
    else
    {
        packet.put_int(0);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_NoticeCreateGuildAgit::~Inter_NoticeCreateGuildAgit() {}
// ORIG 0x084da98e T dispatch_sig —— 公会聚点创建通知：置标记后按匹配关系回执/同步 proxy。
// 报告 docs/class_func_reports/Inter_NoticeCreateGuildAgit/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeCreateGuildAgitPktView —— Inter_NoticeCreateGuildAgit::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeCreateGuildAgitPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

int Inter_NoticeCreateGuildAgit::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticeCreateGuildAgitPktView* pPkt = reinterpret_cast<Inter_NoticeCreateGuildAgitPktView*>(data);
    user->SetGuildAgitFlag(true);

    // ORIG 0x084daa2a：pPkt 槽 -0xc 入口即物化；m_12 比较。
    if (pPkt->m_12 == user->get_charac_no(-1))
    {   // 目标即本人 → 成功回执
        PacketGuard packet;
        packet.put_header(1, 0xe7);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }

    PacketGuard notify;
    notify.put_header(0, 0xbd);
    notify.finalize(true);
    user->Send(notify);

    // b28c-l23 P0 清绑：ORIG 0x084daac1.. guildkey 两段 `mov %eax,%ebx` 跨 call 钉形，
    // proxy 结果 %eax 免槽直落 (%esp)。绑定形违 P0 已删，裸 register 保留（N96/N18a）。
    register unsigned int guildKey = user->get_charac_guildkey();
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        user->GetServerGroup())->ChangeGuildAgitFlagInProxy(guildKey, 1);
    return 0;
}

Inter_NoticeDeleteGuildAgit::~Inter_NoticeDeleteGuildAgit() {}
// ORIG 0x084dab18 T dispatch_sig —— 公会驻地删除通知：清标记并发 0xe8/0xbe 包。
// 报告 docs/class_func_reports/Inter_NoticeDeleteGuildAgit/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeDeleteGuildAgitPktView —— Inter_NoticeDeleteGuildAgit::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeDeleteGuildAgitPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    int m_12;                       // +0x12
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

int Inter_NoticeDeleteGuildAgit::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticeDeleteGuildAgitPktView* pPkt = reinterpret_cast<Inter_NoticeDeleteGuildAgitPktView*>(data);
    if (pPkt->m_16 != 0)   // msg+0x16 错误码
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xe8,
                                 pPkt->m_16);
        return 0;
    }

    user->SetGuildAgitFlag(false);

    if (pPkt->m_12 ==               // msg+0x12 == 本人
        user->get_charac_no(-1))
    {
        PacketGuard packet;                                   // asm local_28
        packet.put_header(1, 0xe8);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }

    PacketGuard notice;                                       // asm local_1c
    notice.put_header(0, 0xbe);
    notice.finalize(true);
    user->Send(notice);

    // b28c-l23 P0 清绑：ORIG 0x084dac90.. guildkey 跨 call 直落 %ebx；具名 proxy 槽
    // 会顶深 pPkt 至 -0x10（ORIG pPkt=-0xc）。绑定违 P0 已删，裸 register 保留。
    register unsigned int guildKey =
        static_cast<unsigned int>(user->get_charac_guildkey());
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        user->GetServerGroup())->ChangeGuildAgitFlagInProxy(guildKey, 0);
    return 0;
}

Inter_NoticeLoadGuildAgit::~Inter_NoticeLoadGuildAgit() {}
// ORIG 0x084dacda T dispatch_sig —— 公会行会据点加载通知。
// 报告 docs/class_func_reports/Inter_NoticeLoadGuildAgit/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeLoadGuildAgitPktView —— Inter_NoticeLoadGuildAgit::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeLoadGuildAgitPktView
{
    char m_pad0[0x16];  // +0x0..未消费
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

int Inter_NoticeLoadGuildAgit::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticeLoadGuildAgitPktView* pPkt = reinterpret_cast<Inter_NoticeLoadGuildAgitPktView*>(data);
    user->SetGuildAgitDBInfo(
        *reinterpret_cast<STGuildAgitDBInfo*>((char*)pPkt + 0x16));
    PacketGuard packet;
    packet.put_header(0, 0xbf);
    packet.put_byte(pPkt->m_16);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_LoadExpertJob::~Inter_LoadExpertJob() {}
// ORIG 0x084db200 T dispatch_sig —— 达人职业 DB 加载（扩展数据 loadData + 信息包）。
// 报告 docs/class_func_reports/Inter_LoadExpertJob/dispatch_sig.md 全文转写。
int Inter_LoadExpertJob::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 1)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    // GetCharacExpandData(0) 实际对象为 expert_job::CCharacExpertJob（CData 派生，vptr+4
    // loadData 虚槽，ORIG 0x084db250 mov (%eax),%edx; add $0x4 两参虚调）；第二次独立
    // call（P48，ORIG 0x084db26a），SendExpertJobInfo 为非虚直调（ORIG 0x0832b454）——
    // 结果免槽直用（技巧37），cast 无代码。
    reinterpret_cast<expert_job::CCharacExpertJob*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))->loadData(user, data);
    reinterpret_cast<expert_job::CCharacExpertJob*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))->SendExpertJobInfo(user);
    return 0;
}

Inter_MonitorPunishUserReq::~Inter_MonitorPunishUserReq() {}
// ORIG 0x084db294 T dispatch_sig —— 监控踢人：按账号表逐一断线。
// 报告 docs/class_func_reports/Inter_MonitorPunishUserReq/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorPunishUserReqPktView —— Inter_MonitorPunishUserReq::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorPunishUserReqPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    signed char m_a;                // +0x0a
    unsigned short m_b;             // +0x0b
    unsigned int m_accIds[1];       // +0x0d（ORIG 084db31c `mov 0xd(%eax,%edx,4)` scaled 直证）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=81（ORIG 0x084db294）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=111（ORIG 0x84db294）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [lane2 2026-09-10] verdict=DIFF→AE：① m_accIds[1] 视图数组（ORIG 084db31c `mov 0xd(%eax,%edx,4)` scaled）；
//   ② P227 两处 cMyTrace 全表达式临时链；③ P191 双初始化（ORIG 084db303/084db30a 两条 count 清零）；
//   ④ P223 空 then 占位岛（ORIG 084db323 `jbe <nop>`）用负条件 goto 复现。
int Inter_MonitorPunishUserReq::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorPunishUserReqPktView* pPkt = reinterpret_cast<Inter_MonitorPunishUserReqPktView*>(data);

    // P227：ORIG 084db2a5/084db2af 在 ctor 前求出 m_b→%esi / m_a→%ebx 并跨 ctor 保活
    //   ⇒ 全表达式临时链（禁具名 trace 局部 + 二次语句；值不进栈槽）。
    cMyTrace(__PRETTY_FUNCTION__, 0x43bc, 0)(
        "Monitor Punish User / m_flag = %d / user_count", (int)pPkt->m_a,
        (unsigned int)pPkt->m_b);

    if (pPkt->m_a == 0)
    {
        int i = 0;
        for (i = 0; i < (int)pPkt->m_b; i = i + 1)     // P191 双初始化：ORIG 084db303/084db30a 两条清零
        {
            // P223：ORIG 084db323 `jbe <nop 岛>` ⇒ 空 then 占位岛用负条件 goto 复现。
            if (pPkt->m_accIds[i] <= 0x63)
                goto next_acc;
            if (G_CGameManager()->isConnectUserByAccId(pPkt->m_accIds[i]) != '\0')
            {
                CUser* target = G_CGameManager()->GetUserByAccId(pPkt->m_accIds[i]);
                target->DisConnSig((DISCONN_SIG)0x2c, true, 0);
                // P227：ORIG 084db3a7 把 NumberToString 结果留 %ebx 跨 ctor ⇒ 临时链内联。
                cMyTrace(__PRETTY_FUNCTION__, 0x43d4, 0)(
                    "System Kick User : %s", NumberToString(pPkt->m_accIds[i], 0));
            }
        next_acc:;
        }
    }
    return 0;
}

Inter_PcroomResponse::~Inter_PcroomResponse() {}
// ORIG 0x084db40e T IsShutdonTimeOverLogin —— 停机时段判定：当前小时 < 6。
bool Inter_PcroomResponse::IsShutdonTimeOverLogin()
{
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    struct tm localTm;
    struct tm* pTm = localtime_r(&curSec, &localTm);
    if (pTm->tm_hour < 6)
        return true;
    return false;
}
// ORIG 0x084db452 T dispatch_sig —— PC 房响应：鉴权/封禁/公告与 PC 房计费链。
// 报告 docs/class_func_reports/Inter_PcroomResponse/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PcroomResponsePktView —— Inter_PcroomResponse::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_PcroomResponsePktView
{
    char m_pad0[0x12];  // 未消费
    unsigned int m_12;        // +0x12
    long m_16;                // +0x16（ORIG 084db82c `mov 0x16(%eax),%eax` 4 字节读；
                              //   原 char 槽为转写宽度误，已按 handleSetUserPCRoom 形参 long 修正）
    // ORIG 0x84db571/0x84db597/0x84db5ac/0x84db5d2 全 `movzbl` 直证 ⇒ 四字段均为 unsigned char
    // （旧 signed char 生 movsbl，P11/T1 宽度扩展判据）。
    unsigned char m_1a;       // +0x1a
    unsigned char m_1b;       // +0x1b
    unsigned char m_1c;       // +0x1c
    unsigned char m_1d;       // +0x1d
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=241（ORIG 0x084db452）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=173（ORIG 0x84db452）：N18a 单段 regmove。
int Inter_PcroomResponse::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PcroomResponsePktView* pPkt = reinterpret_cast<Inter_PcroomResponsePktView*>(data);
    if (user->get_state() == 0)
        return 0x4404;

    if (user->getCurCharacR() != 0)
        user->setPcRoomAuth(true);
    user->SetPCRoomNo(pPkt->m_12);

    // ORIG 0x84db4be `movb $0x0,-0x2b(%ebp)`：写后全函数未读的死局部（P171 照抄原则）。
    char deadFlag = 0;
    (void)deadFlag;

    if (pPkt->m_1c == 7 &&
        pPkt->m_1a != 3 &&
        pPkt->m_1d == 1)
    {   // 封禁登录
        user->DisConnSig((DISCONN_SIG)0x3c, true, 0);
        // P328⑩/P227：ORIG 0x84db50e..0x84db546 先求 acc_id→%esi、acc_name→%ebx，
        // 其后才 cMyTrace ctor ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x4479, 0)(
            "SecuritySystem user ban ip login(%s,%u)",
            user->get_acc_name(), user->get_acc_id());
    }

    bool forbidden = false;                            // -0x2a
    bool showMsg = true;                               // -0x29
    // ORIG 084db578..084db592 二分决策树（cmp $1 / jg / cmp $2 / cmp $3）⇒ 原码是 switch（P17/P46）。
    // ★真语义修正：case 3 无条件 forbidden=true（084db5a5 movb $1,-0x2a），
    //   仅 showMsg=false 受 m_1b==1 约束（084db5b3..084db5b8）；
    //   旧写法把 forbidden 也绑在 m_1b==1 上 ⇒ kind==3 且 m_1b!=1 时漏封禁。
    switch (pPkt->m_1a)
    {
    case 0:
        if (pPkt->m_1b == 2)
            forbidden = true;
        break;
    case 1:                                            // ORIG 084db5be/084db5c1 空叶（nop;jmp）
    case 2:
        break;
    case 3:
        forbidden = true;
        if (pPkt->m_1b == 1)
            showMsg = false;
        break;
    }

    if (forbidden)
    {
        // P328⑩/P227：ORIG 0x84db5cf..0x84db601 先求 m_1b→%esi、m_1a→%ebx 再 ctor。
        cMyTrace(__PRETTY_FUNCTION__, 0x44ec, 0)(
            "Inter_PcroomResponse::dispatch_sig() forbidden ip (%d), %d)",
            (int)pPkt->m_1a,
            (int)pPkt->m_1b);

        PacketGuard packet;
        packet.put_header(0, 0xe9);
        packet.put_byte(1);
        packet.put_byte(5);
        const char* msgStr = g_nexonMsgTable.GetMessageString(
            (int)pPkt->m_1b);
        char msg[0x401] = { '\0' };                    // -0x45d（ORIG 内联清零）
        const char* str = g_scriptStringManager_.findString(4, msgStr, 0);
        strncpy(msg, str, 0x400);
        const int msgLen = (int)strlen(msg);
        packet.put_int(msgLen);
        packet.put_str(msg, msgLen);
        packet.put_byte((int)showMsg);
        packet.finalize(true);
        user->Send(packet);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                    (TIMER_MESSAGE)0x84, 0x78,
                                    (int)user->getCurCharacNo(), 0);
    }

    if (pPkt->m_12 != 0)
    {
        // ORIG 0x84db826/0x84db82f 两值各落槽（-0x20/-0x1c）后压参 ⇒ 具名局部（P0）。
        long curSec = GlobalData::s_systemTime_.getCurSec();
        long expireTime = pPkt->m_16;
        WongWork::CHandlePremium::handleSetUserPCRoom(user, curSec, expireTime);
    }
    // ORIG 0x84db84b..0x84db862 get_event_script_mng 返回值 eax 直压 (%esp)，无具名槽 ⇒ 单语句链。
    reinterpret_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())
        ->send_pc_room_play_init_time(user);
    return 0;
}

Inter_ItemLimitEdition_Sell_Start::~Inter_ItemLimitEdition_Sell_Start() {}
// ORIG 0x084db878 T dispatch_sig —— 限购版道具开卖：清旧登记（广播移除包）、逐条目
// 注册、广播信息包。msg 布局：+0xa flag(u8) +0xb count(u32) +0xf 起条目 stride 0x48。
// 报告 docs/class_func_reports/Inter_ItemLimitEdition_Sell_Start/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ItemLimitEdition_Sell_StartPktView —— Inter_ItemLimitEdition_Sell_Start::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ItemLimitEdition_Sell_StartPktView
{
    char m_pad0[0xa];  // 未消费
    unsigned char m_flag;     // +0x0a
    unsigned int m_b;         // +0x0b
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=179（ORIG 0x084db878）：槽序/帧差(N82/P212/P221)+混合残差；gap+4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=226（ORIG 0x84db878）：N82/P112 帧槽平移+P30/N4 add-vs-lea+P264 .o 重定位/立即数。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→-1（236 vs 237，TU 121/1331/5/133 不回退）：真修四处——
//   ① do-while+return 改 for + 循环后 `if(m_b!=0)` 广播（复现 ORIG 底部条件 `seta;jne` 与 fall-through 广播块）；
//   ② 删 itemIdx 具名局部改实参内联两次（P0）；③ cMyTrace 全表达式临时链（P221/P227）；
//   ④ IsAvatarItem 极性翻正 `if(IsAvatarItem()!=0) goto next_item;`（消 ORIG 无的 `xor $1`）。
//   残余=N4/P30 add-vs-lea（ORIG `mov 0x13(%eax,%ecx,1)` scaled disp vs 4.4 常量先加链）。
int Inter_ItemLimitEdition_Sell_Start::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ItemLimitEdition_Sell_StartPktView* pPkt = reinterpret_cast<Inter_ItemLimitEdition_Sell_StartPktView*>(data);
    if (pPkt->m_flag != 0)
    {
        if (!G_CDataManager()->getItemLimitEditionMgr()->empty())
        {   // 广播移除旧限购信息
            PacketGuard packet;
            packet.put_header(0, 0xe1);
            packet.put_byte(1);
            G_CDataManager()->getItemLimitEditionMgr()->makeItemLimitEditionRemoveInfo(packet);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }
        G_CDataManager()->getItemLimitEditionMgr()->clear();
    }

    for (unsigned int i = 0; i < pPkt->m_b; ++i)
    {
        CItem* found = G_CDataManager()->find_item(
            *reinterpret_cast<int*>(data + i * 0x48 + 0x13));
        if (found == 0)
        {
            if (*reinterpret_cast<int*>(data + i * 0x48 + 0x13) == 1)  // cmp $1 je（P15）
            {
                goto regist;
            }
        }
        else
        {
            if (found->IsAvatarItem() != 0)            // ORIG vptr+0x10 槽（P5/P62）正极性 `test; jne`
                goto next_item;
regist:
            G_CDataManager()->getItemLimitEditionMgr()->registItem(
                *reinterpret_cast<stItemLimitEditionItemInfo_t*>(data + i * 0x48 + 0xf));
            cMyTrace(__PRETTY_FUNCTION__, 0x4638, 0)(
                "ipg: %d, itemIdx: %d, totalCnt: %d/%d, sellEndTime: %d",
                *reinterpret_cast<int*>(data + i * 0x48 + 0xf),
                *reinterpret_cast<int*>(data + i * 0x48 + 0x13),
                *reinterpret_cast<unsigned int*>(data + i * 0x48 + 0x27),
                *reinterpret_cast<unsigned int*>(data + i * 0x48 + 0x2b),
                *reinterpret_cast<unsigned int*>(data + i * 0x48 + 0x37));
        }
next_item:;
    }

    if (pPkt->m_b != 0)
    {   // 广播新限购信息
        PacketGuard packet;
        packet.put_header(0, 0xe1);
        packet.put_byte(0);
        G_CDataManager()->getItemLimitEditionMgr()->makeItemLimitEditionInfo(packet);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
    }
    return 0;
}

Inter_ItemLimitEdition_Sell_End::~Inter_ItemLimitEdition_Sell_End() {}
// ORIG 0x084dbbb2 T dispatch_sig —— 限量版道具销售结束：逐项移除并广播 IPG 列表。
// 报告 docs/class_func_reports/Inter_ItemLimitEdition_Sell_End/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ItemLimitEdition_Sell_EndPktView —— Inter_ItemLimitEdition_Sell_End::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ItemLimitEdition_Sell_EndPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=90（ORIG 0x084dbbb2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=103（ORIG 0x84dbbb2）：P30/N4 add-vs-lea。
int Inter_ItemLimitEdition_Sell_End::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ItemLimitEdition_Sell_EndPktView* pPkt = reinterpret_cast<Inter_ItemLimitEdition_Sell_EndPktView*>(data);
    // ORIG 0x084dbbc2 仅一枚具名包指针（-0x10）；`char* recv`/`const unsigned int count`
    // 两个具名局部在 ORIG 无槽往返（循环界与下标基址均直读）⇒ P0/P329 内联删除。
    PacketGuard packet;
    packet.put_header(0, 0xe1);
    packet.put_byte(1);
    // ORIG 084dbbfc：0xe 按 32 位读（mov 0xe(%eax),%eax）直传 put_byte 形参。
    packet.put_byte(pPkt->m_e);

    // 残（已留痕）：ORIG pPkt 槽 -0x10（浅）/packet -0x2c；4.4 先声明深 ⇒ 我方 -0x14/-0x30
    //（把 i 提前到 packet 前实测 -0x10 被 i 占用、pPkt 仍 -0x14）⇒ N82/P212 跨版本槽方向封顶。
    unsigned int i = 0;
    while (i < (unsigned int)pPkt->m_e)                 // cmp/seta 无符号（界直读无具名槽）
    {
        const unsigned int ipg =
            *reinterpret_cast<unsigned int*>(data + 0x12 + i * 4);
        G_CDataManager()->getItemLimitEditionMgr()->removeItem(ipg);
        cMyTrace(__PRETTY_FUNCTION__,
            0x465a, 0)("ipg: %d", ipg);
        packet.put_int(*reinterpret_cast<int*>(data + 0x12 + i * 4));
        i = i + 1;                                     // addl $0x1,mem 形态
    }
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_ItemLimitEdition_Update::~Inter_ItemLimitEdition_Update() {}
// ORIG 0x084dbd16 T dispatch_sig —— 限售物品数量更新：逐条 updateItem 后广播 0xe1 包。
// 报告 docs/class_func_reports/Inter_ItemLimitEdition_Update/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ItemLimitEdition_UpdatePktView —— Inter_ItemLimitEdition_Update::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ItemLimitEdition_UpdatePktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=130（ORIG 0x084dbd16）：槽序/帧差(N82/P212/P221)+混合残差；gap-6。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=140（ORIG 0x84dbd16）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_ItemLimitEdition_Update::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ItemLimitEdition_UpdatePktView* pPkt = reinterpret_cast<Inter_ItemLimitEdition_UpdatePktView*>(data);
    const unsigned int entryCount =                  // msg+0x0e 条目数
        pPkt->m_e;

    PacketGuard packet;
    packet.put_header(0, 0xe1);
    packet.put_byte(0);
    int countIdx = packet.get_index();         // 计数回填位
    int putCount = 0;                                // local_18/asm -0x14(%ebp)
    packet.put_byte(0);

    for (unsigned int i = 0; i < entryCount; ++i)    // asm seta 无符号比较
    {
        CDataManager* mgrData = G_CDataManager();
        CItemLimitEditionMgr* mgr = mgrData->getItemLimitEditionMgr();
        CItemLimitEdition* info =
            mgr->getItemInfo(*reinterpret_cast<unsigned int*>(data + i * 9 + 0x12));
        if (info == 0 || info->isSellComplete())
            continue;                                // ORIG mov $1/mov $0 归并 true

        const unsigned int count =
            *reinterpret_cast<unsigned int*>(data + i * 9 + 0x16);
        const unsigned int itemIdx =
            *reinterpret_cast<unsigned int*>(data + i * 9 + 0x12);
        CItemLimitEditionMgr* mgr2 = G_CDataManager()->getItemLimitEditionMgr();
        // ORIG xor $0x1,%eax; test %al（bool 取反语义）
        if (mgr2->updateItem(itemIdx, count))
        {
            info->makeInfoPacket(packet);
            ++putCount;
        }
    }

    if (putCount != 0)
    {
        packet.put_byte(countIdx, putCount);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
    }
    return 0;
}

Inter_ItemLimitEdition_Reply::~Inter_ItemLimitEdition_Reply() {}
// ORIG 0x084dbef0 T —— 限时道具购买应答。
// 报告 docs/class_func_reports/Inter_ItemLimitEdition_Reply/dispatch_sig.md 全文转写
// （## 汇编 + ## 反编译 C）。
#pragma pack(push, 1)
// Inter_ItemLimitEdition_ReplyPktView —— Inter_ItemLimitEdition_Reply::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ItemLimitEdition_ReplyPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    unsigned int m_12;        // +0x12
    unsigned char m_16;       // +0x16
    unsigned char m_17;       // +0x17
    unsigned char m_18;       // +0x18
    char m_pad19[0xaf];  // 未消费
    unsigned char m_c8;       // +0xc8
};
#pragma pack(pop)

// §6.2 巨件核销：P167 106/106 [0缺0多]；gap -39→-39；残差归因 N82（4.4 槽复用全帧错位，
// 详见 docs/asm_translation_patterns.md 负例表）。batch-20「Inter 巨兽」已核销，本次维持。
// [b28d-lane2] verdict=DIFF ndiff=716（ORIG 0x084dbef0）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-41。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=527（ORIG 0x84dbef0）：N22 cmp 操作数方向+P226 具名 bool/char 落槽+N18a 单段 regmove。
int Inter_ItemLimitEdition_Reply::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ItemLimitEdition_ReplyPktView* pPkt = reinterpret_cast<Inter_ItemLimitEdition_ReplyPktView*>(data);
    if (user->get_state() < (ch_state)3)              // setle 界：state<3 → return 0
        return 0;

    const unsigned int reqItemId =                    // msg+0x0e
        pPkt->m_e;
    const unsigned int reqCount =                     // msg+0x12
        pPkt->m_12;

    int specialRestrictCode = 0;
    CItemLimitEdition* edition = 0;                    // 前后半段共享（try 内赋值）
    CItem* ci = 0;
    try
    {
        // ORIG 四处独立 G_CDataManager：updateItem / getItemInfo / find_item /
        // GetRestrictCode。
        if (!G_CDataManager()->getItemLimitEditionMgr()->updateItem(
                reqItemId, reqCount))
            throw static_cast<ENUM_ERROR>(0x16);
        const unsigned char buyType = pPkt->m_16;
        if (buyType == '\x01')
            throw static_cast<ENUM_ERROR>(0x16);
        if (buyType == '\x02')
            throw static_cast<ENUM_ERROR>(0x5f);
        if (buyType != '\0')
            throw static_cast<ENUM_ERROR>(1);

        edition = G_CDataManager()->getItemLimitEditionMgr()->getItemInfo(reqItemId);
        if (edition == 0)
            throw static_cast<ENUM_ERROR>(0x11);

        // 售罄后广播信息包（头 0xe1）
        if (edition->isSellComplete())
        {
            PacketGuard packet;
            packet.put_header(0, 0xe1);
            packet.put_byte(0);
            packet.put_byte(1);
            edition->makeInfoPacket(packet);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
        }

        if ((unsigned int)user->GetCera() <
            (unsigned int)edition->getPriceCera())
            throw static_cast<ENUM_ERROR>(0xb);
        if (user->getCurCharacInvenR()->get_money() < edition->getPriceGold())
            throw static_cast<ENUM_ERROR>(0xa);
        if ((unsigned int)user->GetWinPoint() <
            (unsigned int)edition->getPriceWinPoint())
            throw static_cast<ENUM_ERROR>(0xc);

        // ORIG 6 处独立 `_ZNK17CItemLimitEdition14getRelateditemEv`（无缓存）。
        if (edition->getRelateditem() != 1)
        {
            ci = G_CDataManager()->find_item(edition->getRelateditem());
            if (ci == 0)
                throw static_cast<ENUM_ERROR>(0x11);
        }
    }
    catch (const ENUM_ERROR& e)   // ORIG try1 @ 084dbf3b→084dc2c4，catch @ 084dc2c7
    {   // 前半段所有 ENUM_ERROR 抛出点收敛于此（SendCmdErrorPacket 0x100）。
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x100, (unsigned char)e & 0xff);
        return 0;
    }
    {   // 后半段（ORIG try3 @ 084dc3da，不 throw；guard/packet 异常走清理 resume）：

        bool specialFlag = false;
        int specialValue = 0;
        int specialType = 0;
        int featuredIdx = edition->getFeaturedIdx();
        bool isSpecial = false;
        if (featuredIdx == 0)
        {
            isSpecial = GlobalData::s_pCeraShop->IsSpecialItem(
                edition->getRelateditem(), specialValue, specialType, specialFlag);
        }
        if (isSpecial)
        {   // 特别物品借用 restrict code 作为 featured 字段。ORIG 直读
            // CDataManager+0xc（m_itemList）成员再 GetRestrictCode（无 getItemList 调用）。
            specialRestrictCode =
                G_CDataManager()->m_itemList->GetRestrictCode(
                    edition->getRelateditem());
        }

        user->resetUsedCera();
        Stream* stream = GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x46d9);
        CStreamGuard guard(stream, false);
        stCeraShopIPGRequest_t* reqBuf = guard->GetInBuffer<stCeraShopIPGRequest_t>();
        stCeraShopIPGRequest_t& rq = *reqBuf;

        rq.m_productNo = static_cast<unsigned int>(pPkt->m_e);
        rq.m_groupIdx = 10;                                            // IPG 请求类型
        rq.m_itemIdx = edition->getRelateditem();
        rq.m_stackCount = edition->getRelatedItemCnt();
        rq.m_ceraPrice = static_cast<unsigned int>(edition->getPriceCera());
        rq.m_winPoint = static_cast<unsigned int>(edition->getPriceWinPoint());
        rq.m_goldPrice = static_cast<unsigned int>(edition->getPriceGold());
        rq.m_avatarField = 0;
        rq.m_itemCount = 1;
        rq.m_seq = 1;
        rq.m_featuredIdx = specialRestrictCode;
        rq.m_avatarType0 = pPkt->m_17;
        rq.m_avatarType1 = edition->getAvatarTypeSelect();
        rq.m_curSec = static_cast<unsigned int>(GlobalData::s_systemTime_.getCurSec());
        rq.m_buyFlag = 0;
        rq.m_characNo = user->getCurCharacNo();
        rq.m_accId = static_cast<unsigned int>(user->get_acc_id());
        const unsigned int optionCnt =                                 // msg+0x18
            pPkt->m_18;
        rq.m_subCount = static_cast<unsigned char>(optionCnt);
        for (unsigned int i = 0; i < optionCnt; ++i)                   // stride 8 子项@0x48
        {
            // [Lane7GlobalData · Main 批准跨 TU 迁移 2026-09-10 · 「视图 cast 反模式」]
            // 目的侧改具名数组视角（SigTypes_E.h 匿名 union，sizeof/偏移/mangled 不变）：
            // 旧 `*reinterpret_cast<T*>(&rq.m_subEntries[i][0])=` 目的侧 cast 使 4.4 折
            // add+shl 普通 store；ORIG 为 stride-8 scaled 直存。源侧仍是 5 字节步长
            // 线格式缓冲读，保留 cast（P156 线格式豁免）。
            rq.m_subItems[i].m_itemIdx =
                *reinterpret_cast<int*>(data + 0x19 + i * 5);   // idx 槽 +0x00
            rq.m_subItems[i].m_value = *(data + 0x1d + i * 5);  // val 槽 +0x04
        }
        const unsigned int avatarCnt =                                 // msg+200(0xc8)
            pPkt->m_c8;
        rq.m_optCount = static_cast<unsigned char>(avatarCnt);
        for (unsigned int j = 0; j < avatarCnt; ++j)                   // stride 8 选项@0x164
        {
            // 同上（ORIG stride-8 scaled 直存）：目的侧改具名视角 m_optItems。
            rq.m_optItems[j].m_itemUid =
                static_cast<unsigned int>(*reinterpret_cast<int*>(data + 9 + j * 8));
            rq.m_optItems[j].m_a =
                *reinterpret_cast<unsigned short*>(data + 0xd + j * 8);
            rq.m_optItems[j].m_b =
                *reinterpret_cast<unsigned short*>(data + 0xf + j * 8);
        }

        char priceText[36] = { '\0' };                                 // ORIG memset(0x24)
        char uidText[21]   = { '\0' };                                 // -0x75..-0x59 区清零
        char itemNameText[0x62];
        OS_API::snprintf(priceText, sizeof(priceText), "%d",
                         edition->getPriceCera());
        OS_API::snprintf(uidText, 8, "%d", reqItemId);

        const unsigned short relatedCnt = rq.m_stackCount;   // +0x0c
        if (edition->getRelateditem() == 1)
        {
            OS_API::snprintf(itemNameText, 0x62, "코인 %d개", (unsigned int)relatedCnt);
        }
        else if (ci->is_stackable() && relatedCnt >= 2)
        {
            OS_API::snprintf(itemNameText, 0x62, "%s %d개",
                             ci->GetItemName(), (unsigned int)relatedCnt);
        }
        else
        {
            strcpy(itemNameText, ci->GetItemName());
        }

        char uniqueNo[0x15] = { '\0' };
        if (edition->getPriceCera() == 0)
        {   // vtable+0x18 = slot6 IPGOutputSimulation。ORIG：GenUniqueNo 静态
            // 直调（无构造），返回值作 IPG 实参。
            GlobalData::s_pIPGHelper->IPGOutputSimulation(
                user, uidText, priceText,
                WongWork::CGenUniqueNo::GenUniqueNo(uniqueNo), itemNameText,
                user->getCurCharacName(), guard.Get());
        }
        else
        {   // vtable+0x14 = slot5 IPGOutput
            GlobalData::s_pIPGHelper->IPGOutput(
                user, uidText, priceText,
                WongWork::CGenUniqueNo::GenUniqueNo(uniqueNo), itemNameText,
                user->getCurCharacName(), guard.Get());
        }

        PacketGuard packet;
        packet.put_header(1, 0x100);
        packet.put_byte(1);
        packet.put_int(edition->getIPGNO());
        packet.put_int(edition->getCurSellNum());
        packet.put_int(edition->getTotalSellNum());
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_FindFactoryHub::~Inter_FindFactoryHub() {}
// ORIG 0x084dc97a T dispatch_sig —— 工厂枢纽查找：计 hack 键并上报监视服。
// 报告 docs/class_func_reports/Inter_FindFactoryHub/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_FindFactoryHubPktView —— Inter_FindFactoryHub::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_FindFactoryHubPktView
{
    int m_0;                  // +0x00
    unsigned char m_4;        // +0x04
    char m_5[1];              // +0x5 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=78（ORIG 0x084dc97a）：槽序/帧差(N82/P212/P221)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=46（ORIG 0x84dc97a）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_FindFactoryHub::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_FindFactoryHubPktView* pPkt = reinterpret_cast<Inter_FindFactoryHubPktView*>(data);

    if (user != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x2c0, 1,
            (unsigned int)user->SetETC(0x2c0, pPkt->m_0), 0);
        if (pPkt->m_4 != 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x2bd, 1,
                (unsigned int)user->SetETC(0x2bd, pPkt->m_0), 0);
        }

        Packet_Monitor_Find_Factory_Hub_User packet;
        packet.m_charNo = user->getCurCharacNo();      // +0x0a
        packet.m_nameLen =
            (unsigned char)strlen(pPkt->m_5);           // +0x0e 截断到字节
        strncpy(packet.m_name, pPkt->m_5, 0x1e);        // +0x0f
        packet.m_field2e = 0x2c1;                      // +0x2d（ORIG 恒写 0x2c1）
        packet.m_accId = user->get_acc_id();           // +0x2f
        packet.packetSize = 0x33;                      // ORIG 发送长度恒 0x33

        // [b28d-lane2] ORIG 084dc9f4 段：长度 0x33 先落具名 int 槽（-0xc，
        // `movl $0x33,-0xc(%ebp)` + `mov -0xc(%ebp),%edx`），GetServerProxy 结果直落
        // (%esp)（无具名 proxy 槽）。
        // P198 孪生判据：初始化式会被 4.4 折成常量（丢槽）⇒ 声明与赋值分两句。
        int sendLen;
        sendLen = 0x33;
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&packet), sendLen);
    }
    return 0;
}

Inter_GuildExpLimit::~Inter_GuildExpLimit() {}
int Inter_GuildExpLimit::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))->SendGuildExpLimit();
    return 0;
}

Inter_SetIPCount::~Inter_SetIPCount() {}
// ORIG 0x084dcb18 T dispatch_sig —— 同 IP 连接计数表同步。
// 报告 docs/class_func_reports/Inter_SetIPCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetIPCountPktView —— Inter_SetIPCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
// 元素内 +0x4 的子结构（相对包体 +0x10）必须具名：ORIG 0x084dcb5e 段对子结构成员发
//   `lea (%ecx,%eax,1); add $0x10,%eax; mov 0xc(%eax)`（子结构基址 + 成员 disp 两步），
//   而元素内标量成员（+0xc/+0xe）走 `movzwl 0xc(%eax,%ecx,1)` 折叠 disp——两者共存
//   证明原码是「具名子结构成员」而非裸偏移（裸偏移 4.4 恒折 add $0x1c + mov (%eax)）。
struct Inter_SetIPCountSub
{
    char m_ip[0xc];             // 相对包体 +0x10（setIPCounterData 第 3 参 char* 原文）
    unsigned int m_counter;     // 相对包体 +0x1c（第 4 参）
};
// m_counter 的等址尾视图：ORIG 第 4 参走「子结构地址 +0x10」→「成员 disp +0xc」两步
//   （0x084dcb58 `lea (%ecx,%eax,1); add $0x10,%eax; mov 0xc(%eax)`），4.4 对直接成员访问
//   恒折单条 `mov 0x1c(%eax,%ecx,1)`。经「m_ip 数组名 + 尾视图成员」访问可复现两步形。
struct Inter_SetIPCountSubTail
{
    char m_pad[0xc];            // 与 m_ip 同址
    unsigned int m_counter;     // 相对包体 +0x1c
};
struct Inter_SetIPCountEntry
{
    unsigned short m_c;         // 相对包体 +0x0c + i*0x14
    unsigned short m_e;         // 相对包体 +0x0e + i*0x14
    Inter_SetIPCountSub m_sub;  // 相对包体 +0x10 + i*0x14
};
struct Inter_SetIPCountPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
    unsigned char m_b;              // +0x0b
    Inter_SetIPCountEntry m_entries[1];  // +0x0c 起，元素 stride 0x14
};
static_assert(sizeof(Inter_SetIPCountEntry) == 0x14, "SetIPCountEntry stride");
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=68（ORIG 0x084dcb18）：寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=60（ORIG 0x84dcb18）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_SetIPCount::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SetIPCountPktView* pPkt = reinterpret_cast<Inter_SetIPCountPktView*>(data);
    if (pPkt->m_a == 0)
        GlobalData::g_autoPunishRuleHackTypeMgr->ResetIpCounterData();
    // §P0：ORIG 084dcbc5 循环尾 `movzbl 0xb(%eax),%eax; movzbl %al,%eax; cmp -0x1c(%ebp),%eax`
    //   = 每轮重读 m_b（count 无栈槽）⇒ 不设具名 count，条件内联表达式。
    for (int i = 0; i < (int)(unsigned char)pPkt->m_b; ++i)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->setIPCounterData(
            (unsigned int)pPkt->m_entries[i].m_c,
            (unsigned int)pPkt->m_entries[i].m_e,
            pPkt->m_entries[i].m_sub.m_ip,
            reinterpret_cast<Inter_SetIPCountSubTail*>(pPkt->m_entries[i].m_sub.m_ip)->m_counter);
    }
    return 0;
}

Inter_SetFullIPCount::~Inter_SetFullIPCount() {}
// ORIG 0x084dcbea T dispatch_sig —— 全 IP 连接计数表同步。
// 报告 docs/class_func_reports/Inter_SetFullIPCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetFullIPCountPktView —— Inter_SetFullIPCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
// 与 SetIPCount 同族：元素 stride 0x18、数组基址 +0xc；子结构（元素内 +0x4 ⇒ 相对包体 +0x10）
//   必须具名（ORIG 0x084dcc2f `add $0x10,%eax; mov 0x10(%eax)` 两步），元素内标量走折叠 disp。
struct Inter_SetFullIPCountSub
{
    char m_ip[0x10];            // 相对包体 +0x10（setFullIPCounterData 第 3 参 char* 原文）
    unsigned int m_counter;     // 相对包体 +0x20（第 4 参）
};
// m_counter 等址尾视图（同上 SetIPCountSubTail 依据）。
struct Inter_SetFullIPCountSubTail
{
    char m_pad[0x10];           // 与 m_ip 同址
    unsigned int m_counter;     // 相对包体 +0x20
};
struct Inter_SetFullIPCountEntry
{
    unsigned short m_c;         // 相对包体 +0x0c + i*0x18
    unsigned short m_e;         // 相对包体 +0x0e + i*0x18
    Inter_SetFullIPCountSub m_sub;  // 相对包体 +0x10 + i*0x18
};
struct Inter_SetFullIPCountPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
    unsigned char m_b;              // +0x0b
    Inter_SetFullIPCountEntry m_entries[1];  // +0x0c 起，元素 stride 0x18
};
static_assert(sizeof(Inter_SetFullIPCountEntry) == 0x18, "SetFullIPCountEntry stride");
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=71（ORIG 0x084dcbea）：寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=60（ORIG 0x84dcbea）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_SetFullIPCount::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SetFullIPCountPktView* pPkt = reinterpret_cast<Inter_SetFullIPCountPktView*>(data);
    if (pPkt->m_a == 0)
        GlobalData::g_autoPunishRuleHackTypeMgr->ResetFullIpCounterData();
    // §P0：与 SetIPCount 同族，循环尾每轮重读 m_b（count 无栈槽）。
    for (int i = 0; i < (int)(unsigned char)pPkt->m_b; ++i)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->setFullIPCounterData(
            (unsigned int)pPkt->m_entries[i].m_c,
            (unsigned int)pPkt->m_entries[i].m_e,
            pPkt->m_entries[i].m_sub.m_ip,
            reinterpret_cast<Inter_SetFullIPCountSubTail*>(pPkt->m_entries[i].m_sub.m_ip)->m_counter);
    }
    return 0;
}

Inter_CancelRestingUser::~Inter_CancelRestingUser() {}
int Inter_CancelRestingUser::dispatch_sig(CUser* user, char* /*data*/, int /*len*/)
{
    if (user != 0)
    {
        user->setRestingUserRestrict(false);
    }
    return 0;
}

Inter_ApplyTempPowerSide::~Inter_ApplyTempPowerSide() {}
// ORIG 0x084dccde T dispatch_sig —— 临时势力侧应用（公会代理修改 + 全服广播 basic_info）。
// 报告 docs/class_func_reports/Inter_ApplyTempPowerSide/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ApplyTempPowerSidePktView —— Inter_ApplyTempPowerSide::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ApplyTempPowerSidePktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（同上）
    char m_pad12[0x4];  // +0x12..未消费
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=131（ORIG 0x084dccde）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84dccde）：本轮 GetGuildDBInfo 单次调用+具名缓存（p167 CLEAR）；余 N22/N18a/P30。
int Inter_ApplyTempPowerSide::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ApplyTempPowerSidePktView* pPkt = reinterpret_cast<Inter_ApplyTempPowerSidePktView*>(data);
    // [b28d-lane2] ORIG 084dccec 段为「字段先读」序（`mov -0x10(%ebp),%eax;
    // mov 0xe(%eax),%ebx; movl $0xffffffff,0x4(%esp); call get_charac_no; cmp %eax,%ebx`）
    // ⇒ 原码字段在比较左（`pPkt->m_e != user->get_charac_no(-1)`）；旧 call-first 序
    //   产 `mov %eax,%edx; mov 0xe(%eax),%eax; cmp %eax,%edx`（+3 条）。
    if (pPkt->m_e != user->get_charac_no(-1))
        return 0;

    {   // STGuildProxy(GetGuildDBInfo(), 名随写字节+0x1b, msg+0x16)
        // ORIG 单次调用 + 槽往返 ⇒ 具名局部（p167 E=GetGuildDBInfo×1）。
        const char* guildDbInfo = static_cast<const char*>(user->GetGuildDBInfo());
        STGuildProxy proxy(
            guildDbInfo,
            *(reinterpret_cast<const unsigned char*>(guildDbInfo) + 0x1b),
            pPkt->m_16);
        CGuildServerProxy* proxySrv =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxySrv->ModifyGuildInfoProxy(
            (unsigned int)user->get_charac_guildkey(), proxy);
    }

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), 0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);

    PacketGuard reply;
    reply.put_header(1, 0xc3);
    reply.put_byte(1);
    reply.put_byte(1);
    reply.finalize(true);
    user->Send(reply);
    return 0;
}

Inter_ReloadPowerWarRanker::~Inter_ReloadPowerWarRanker() {}
// ORIG 0x084dceec T dispatch_sig —— 力量战争排行器重载。
// 报告 docs/class_func_reports/Inter_ReloadPowerWarRanker/dispatch_sig.md 全文转写。
int Inter_ReloadPowerWarRanker::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    cMyTrace(__PRETTY_FUNCTION__,
        0x482b, 0)("Receive Packet_Request_Reload_Power_War_Ranker");
    GlobalData::s_power_manager->ClearMVPInfo();
    GlobalData::s_power_manager->LoadRankerInfo();
    return 0;
}

Inter_LoadPowerWarInfo::~Inter_LoadPowerWarInfo() {}
// ORIG 0x084dcf4a T dispatch_sig —— 力量战争胜负信息加载。
// 报告 docs/class_func_reports/Inter_LoadPowerWarInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadPowerWarInfoPktView —— Inter_LoadPowerWarInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadPowerWarInfoPktView
{
    int m_0;                  // +0x00（P211：与 SetPowerInfo 形参同型 ⇒ 4.4 单段直落 callee-saved）
    int m_4;                  // +0x04
    unsigned char m_8;        // +0x08
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=41（ORIG 0x084dcf4a）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：P227 全表达式临时链 + 字段 int
// 同型化（m_0/m_4 与 SetPowerInfo 形参同型 ⇒ 单段直落 callee-saved））。
int Inter_LoadPowerWarInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadPowerWarInfoPktView* pPkt = reinterpret_cast<Inter_LoadPowerWarInfoPktView*>(data);
    // P227：ORIG 在 ctor 前把三字段载入 %edi/%esi/%ebx 跨 ctor 保活 ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x483a, 0)(
        "Receive SIG_LOAD_POWER_WAR_INFO(Winner:%d, A:%d, B:%d)",
        (signed char)pPkt->m_8,
        pPkt->m_0,
        pPkt->m_4);
    GlobalData::s_power_manager->SetPowerInfo(
        (char)pPkt->m_8,
        pPkt->m_0,
        pPkt->m_4);
    return 0;
}

Inter_SetFactoryHubUser::~Inter_SetFactoryHubUser() {}
// ORIG 0x084dcfec T dispatch_sig —— 工厂中枢用户登记/查询。
// 报告 docs/class_func_reports/Inter_SetFactoryHubUser/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetFactoryHubUserPktView —— Inter_SetFactoryHubUser::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SetFactoryHubUserPktView
{
    char m_pad0[0xf];  // 未消费
    char m_f[1];              // +0xf 串/字节区首元素（容量未建模）
    char m_pad10[0x1d];  // 未消费
    unsigned char m_2d;       // +0x2d
    unsigned short m_2e;      // +0x2e
    ENUM_VIEW_FIELD m_30;     // +0x30（P254 枚型：ORIG 0x084dd012 保留 eax→edx 中转）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=57（ORIG 0x084dcfec）：槽序/帧差(N82/P212/P221)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=63（ORIG 0x84dcfec）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_SetFactoryHubUser::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 084dcff4..084dd070：单出口（尾 `mov $0x0,%eax` 共享）⇒ 源形为
    // `if (user != 0) { ... } return 0;`（非 early-return：后者产 jne+内联 return 两段）。
    if (user != 0)
    {
        // ORIG 084dcffe/084dd001：pPkt 槽在 user 非空门之后物化（-0xc）。
        Inter_SetFactoryHubUserPktView* pPkt = reinterpret_cast<Inter_SetFactoryHubUserPktView*>(data);
        // ORIG 084dd00d `je +0x86`：je 目标=DB 支（084dd072 strncpy/makeRequest），
        // 落空侧=hack 支（084dd00f SetETC/addServerHackCnt）⇒ 源形 then=hack、else=DB。
        if (pPkt->m_2d != 0)
        {
            // ORIG 084dd032 `mov %eax,%esi`（SetETC 结果跨 getHackAnalyzer 调用保活，
            // 无栈槽 ⇒ 具名局部须 register，否则占 -0xc 并把 pPkt 挤到 -0x10）。
            // ORIG 0x084dd032 `mov %eax,%esi` 单段直落（SetETC 返回 int → unsigned 形参无转换）；
            // 声明 int 时 4.4 恒多 `mov %eax,%ebx; mov %ebx,%esi`（P41 变体：实参冗余 cast）。
            register unsigned int etc = user->SetETC((short)pPkt->m_2e,
                                                      (int)pPkt->m_30);
            user->getHackAnalyzer()->addServerHackCnt(
                user,
                (WongWork::ENUM_HACKTYPE)(unsigned int)pPkt->m_2e,
                1, etc, 0);
        }
        else
        {
            char name[29];
            strncpy(name, pPkt->m_f, 0x1d);
            DB_Find_Factory_Hub_User::makeRequest(
                user->GetUID(), name,
                pPkt->m_2e);
        }
    }
    return 0;
}

Inter_SetFactoryHubUserNotConn::~Inter_SetFactoryHubUserNotConn() {}
// ORIG 0x084dd0c4 T dispatch_sig —— 工厂中枢用户未连接记录 DB 保存。
// 报告 docs/class_func_reports/Inter_SetFactoryHubUserNotConn/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetFactoryHubUserNotConnPktView —— Inter_SetFactoryHubUserNotConn::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SetFactoryHubUserNotConnPktView
{
    unsigned int m_0;               // +0x00
    int m_4;                        // +0x04（ORIG +0x3f 两步 spill＝signed 字段→unsigned 形参失配）
    unsigned short m_8;             // +0x08
};
#pragma pack(pop)

int Inter_SetFactoryHubUserNotConn::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user != 0)
    {
    // ORIG：包基址槽在 user 非空门之后物化。
    Inter_SetFactoryHubUserNotConnPktView* pPkt = reinterpret_cast<Inter_SetFactoryHubUserNotConnPktView*>(data);
        user->getHackAnalyzer()->ReqDBSaveHackInfo(
            pPkt->m_0,
            pPkt->m_4,
            pPkt->m_8,
            (int)user->get_acc_id(), 1);
    }
    return 0;
}

Inter_Set_CleanPad_Point::~Inter_Set_CleanPad_Point() {}
// ORIG 0x084dd12e T dispatch_sig —— CleanPad 点数设置（CMCAPManager）。
// 报告 docs/class_func_reports/Inter_Set_CleanPad_Point/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Set_CleanPad_PointPktView —— Inter_Set_CleanPad_Point::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_Set_CleanPad_PointPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned short m_e;             // +0x0e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=52（ORIG 0x084dd12e）：寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=45（ORIG 0x84dd12e）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_Set_CleanPad_Point::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 084dd137/084dd13b：`cmpl; je +0xb3`（⇒ 单出口 `if (user != 0) { ... } return 0;`）。
    if (user != 0)
    {
        // ORIG 084dd141：pPkt 槽在 user 非空门之后物化（084dd144 mov %eax,-0x1c）。
        Inter_Set_CleanPad_PointPktView* pPkt = reinterpret_cast<Inter_Set_CleanPad_PointPktView*>(data);
        user->m_mcap.setPoint(pPkt->m_e);
        // ORIG 084dd171/084dd17b：getPoint 与 pPkt->m_e 无栈槽，分别以 %esi/%edi 跨
        // NumberToString 保活（P227 全表达式临时链）⇒ 实参直接内联，勿建具名局部。
        cMyTrace(__PRETTY_FUNCTION__,
            0x4886, 0)("CLEANPAD_POINT_CONTROL / m_id : %s , Point : %d (getPorint : %d)",
              NumberToString(user->get_acc_id(), 0),
              (unsigned int)pPkt->m_e,
              (unsigned int)user->m_mcap.getPoint());
    }
    return 0;
}

Inter_LoadNoCacheData::~Inter_LoadNoCacheData() {}
// ORIG 0x084dd1ee T dispatch_sig —— 无缓存角色数据加载（扩展数据 loadData 虚调用）。
// 报告 docs/class_func_reports/Inter_LoadNoCacheData/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadNoCacheDataPktView —— Inter_LoadNoCacheData::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadNoCacheDataPktView
{
    int m_0;                        // +0x00
};
#pragma pack(pop)

    // [b28d-solo-InterDispatcher] P225 while+break 复现 4.1 bool 物化（ORIG 0x084dd1ee
    //   `test %eax,%eax; setne %al; test %al,%al; je`；具名 bool 局部 4.4 恒落槽 +3）。
int Inter_LoadNoCacheData::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user != 0)                                   // ORIG +0x06 cmpl $0x0,0xc; je tail
    {
        // [b28d-solo-InterDispatcher] P226 语句表达式：复现 4.1 的 bool 物化 + 直测
        // （`test %eax,%eax; setne %al; test %al,%al; je`，无栈槽）；具名 bool 局部恒落槽。
        if (({ user->getCurCharacR() != 0; }))       // ORIG +0x17 test %eax; setne %al
        {
            Inter_LoadNoCacheDataPktView* pPkt =    // ORIG +0x23 物化 -0xc
                reinterpret_cast<Inter_LoadNoCacheDataPktView*>(data);
            if (({ user->get_charac_no(-1) == pPkt->m_0; }))  // ORIG +0x39 cmp %edx,%eax; sete %al
            {   // ORIG +0x5a~：结果免槽直链，vptr+0x4 = CExpandEquipslot::loadData(user, recv+4)
                user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)1)->loadData(
                    user, reinterpret_cast<char*>(pPkt) + 4);
            }
        }
    }
    return 0;
}

Inter_GuildCargo::~Inter_GuildCargo() {}
// ORIG 0x084dd26c T dispatch_sig —— 公会仓库结果下发（状态门→历史日志→错误包/回填计数包）。
// 报告 docs/class_func_reports/Inter_GuildCargo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildCargoPktView —— Inter_GuildCargo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令核实）。
// entry 区视图：ORIG 0x084dd3dc `imul $0x35 → lea (base,i) → add $0x10 → mov 0x3(%eax)`
// 与 0x084dd405 `imul → add $0x10 → add base → add $0x2` 的两段拆分布局实证
// （flag 绝对 +0x13、blob 绝对 +0x12，entry stride 0x35）。
struct GuildCargoPayloadV
{
    char m_pad0[2];               // +0x00..+0x01（ORIG blob 起点在 payload+0x02）
    unsigned char m_blob0;        // +0x02（put_binary 起点，绝对 +0x12；
                                  //   ORIG 0x084dd405 `add $0x10 → add base → add $0x2`）
    int m_flag;                   // +0x03（绝对 +0x13，ORIG `mov 0x3(%eax)` 实证）
    unsigned char m_rest[0x1e];   // +0x07..0x24（payload 0x25 = 0x35−0x10）
};
struct GuildCargoEntryV
{
    char m_head[0x10];            // +0x00 entry 头
    GuildCargoPayloadV m_payload; // +0x10
};
static_assert(sizeof(GuildCargoEntryV) == 0x35,
              "GuildCargoEntryV ORIG stride 0x35（imul $0x35 实证）");
struct Inter_GuildCargoPktView
{
    char m_pad0[0xe];  // +0x0..未消费（entry 区经 GuildCargoEntryV cast 视图消费）
    unsigned int m_e;         // +0x0e（同上）
    char m_pad12[0x18d8];  // 未消费
    unsigned int m_18ea;      // +0x18ea
    unsigned char m_18ee;     // +0x18ee
};
#pragma pack(pop)

// [b28c-27] N94/N82附：entry 区两处地址形残差——①`add $0x10; mov 0x3(%eax)`
// 被 4.4 折成 `mov 0x13(%eax)`（0x10 与 int 字段 disp 3 合并；N94 判定 C%8 分支
// 不适用非 scaled 形）；②循环体首值装载序 `mov -0xc,%eax; mov -0x14,%edx` 跨版本
// 对调（N82附）。blob 起点偏移已按 ORIG 0x084dd405 `add $0x10 → add base → add $0x2`
// 修正为 payload+0x02（真语义 bug 修复，原 +0x00 少 2 字节）。
// [b28d-lane2] verdict=DIFF ndiff=62（ORIG 0x084dd26c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=11（ORIG 0x84dd26c）：P30/N4 add-vs-lea。
int Inter_GuildCargo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoPktView* pPkt = reinterpret_cast<Inter_GuildCargoPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // ORIG：同 ApplyTempPowerSide。
    if (user->get_charac_no(-1) != pPkt->m_e)
        return 0;

    {   // ORIG 0x084dd2df：GuildCargoResult(reason=1, a=msg+0x18ee, b=capacity=msg+0x18ea)
        //   （esp+4=1、esp+8=0x18ee、esp+0xc=0x18ea——reason 为常量 1，勿与字段错位）
        // ORIG 0x084dd2df 前 `mov 0xc,%ecx; add $0x79700,%ecx; mov %ecx,(%esp)`：
        // hist 免槽即算直落（具名 hist 槽会顶深 pPkt 至 -0x18，ORIG -0x14）。
        user->m_historyLog.GuildCargoResult(
            (ENUM_GUILD_CARGO_HISTORY_REASON)1,
            (int)pPkt->m_18ee,
            (int)pPkt->m_18ea);
    }

    // ORIG 0x084dd30a `movzbl 0x18ee; cmp $0xc1,%al; je +0xd0`：字段 unsigned char
    // 与 0xc1 直接字节比较（原 (char)0xc1 恒真不等致 4.4 折叠整个成功路径 -106 行，
    // 技巧46 同款语义 bug 已纠正）。
    if (pPkt->m_18ee != 0xc1)
    {   // 失败：SendCmdErrorPacket(0x109, errCode)
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x109,
            pPkt->m_18ee);
        return 0;
    }

    user->SetGuildCargoCapacity(
        pPkt->m_18ea);

    int index = 0;                                     // local_1c
    int count = 0;                                     // local_14
    PacketGuard packet;
    packet.put_header(1, 0x109);
    packet.put_byte(1);
    packet.put_int((int)pPkt->m_18ea);
    index = packet.get_index();
    packet.put_short(count);
    for (int i = 0; i <= 0x77; ++i)                    // cmpl $0x77; setle
    {
        // ORIG 0x084dd3dc..：entry 数组（stride 0x35）+ payload 嵌套成员 ⇒
        // 两段地址 `add $0x10` + disp；put_binary 地址独立重算（P48 无 CSE）。
        // entries 具名指针占槽顶深 pPkt（技巧36），改 rvalue cast 免槽。
        if (reinterpret_cast<GuildCargoEntryV*>(pPkt)[i].m_payload.m_flag != 0)
        {
            packet.put_short((int)(short)i);
            packet.put_binary(
                reinterpret_cast<char*>(
                    &reinterpret_cast<GuildCargoEntryV*>(pPkt)[i].m_payload.m_blob0),
                0x35);
            count = count + 1;
        }
    }
    packet.put_short(index, count);                    // put_shortERii 回填总数
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_NoticeGuildCargo::~Inter_NoticeGuildCargo() {}
// ORIG 0x084dd4b2 T dispatch_sig —— 公会仓库容量应答：记历史、设容量、组 0xed 包回发。
// 报告 docs/class_func_reports/Inter_NoticeGuildCargo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticeGuildCargoPktView —— Inter_NoticeGuildCargo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticeGuildCargoPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（ORIG 比较点 `mov %eax,%edx` 中转，P211/P265 档③）
    char m_pad12[0x18d8];  // 未消费
    unsigned int m_18ea;      // +0x18ea
};
// 载荷元素 stride 0x35（ORIG 0x084dd4xx 循环 `imul $0x35,%eax,%eax; lea (%edx,%eax,1)`）；
// 元素内 +0x10 起的载荷子结构必须具名，否则 4.4 把 +0x10/+0x3 折成单条 `add $0x13`
//（ORIG 保留 `add $0x10,%eax; mov 0x3(%eax)` 两步 —— 与 SetIPCount 同族判据）。
struct Inter_NoticeGuildCargoBody
{
    char m_bytes[0x25];   // +0x10..+0x34 载荷字节区（put_binary 源 = 本区 +0x2）
};
struct Inter_NoticeGuildCargoEntry
{
    char m_pad10[0x10];              // 元素内 +0x00..+0x0f
    Inter_NoticeGuildCargoBody m_body;   // 元素内 +0x10
};
static_assert(sizeof(Inter_NoticeGuildCargoEntry) == 0x35, "NoticeGuildCargoEntry stride");
// body 内 +0x3 的 4 字节字段等址视图（ORIG `add $0x10,%eax; mov 0x3(%eax),%eax`）
struct Inter_NoticeGuildCargoTail
{
    char m_pad3[3];
    int m_3;
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=115（ORIG 0x084dd4b2）：寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=49（ORIG 0x84dd4b2）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→2（137 vs 135，TU 121/1331/5/133 不回退）：真修三处——
//   ① countIdx/putCount 提到 PacketGuard 之前且 countIdx 先声明（复现 ORIG `movl $0,-0x18; movl $0,-0x10` 序）；
//   ② `put_short((short)i)` 复现 ORIG `cwtl` 截断；③ m_e int→unsigned int 触发比较点中转（P211/P265 档③）。
//   残余=data 形参 vs ORIG -0x14 槽基址 + 守卫块序（P30/N4）。
int Inter_NoticeGuildCargo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticeGuildCargoPktView* pPkt = reinterpret_cast<Inter_NoticeGuildCargoPktView*>(data);
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle; je 返回
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
        return 0;

    user->m_historyLog.GuildCargoResult(
        (ENUM_GUILD_CARGO_HISTORY_REASON)2, 0,
                           pPkt->m_18ea);
    user->SetGuildCargoCapacity(                     // msg+0x18ea 容量
        pPkt->m_18ea);

    int countIdx = 0;                                // local_1c/asm -0x18(%ebp)
    int putCount = 0;                                // local_14/asm -0x10(%ebp)
    PacketGuard packet;
    packet.put_header(0, 0xed);
    packet.put_int(pPkt->m_18ea);
    countIdx = packet.get_index();                   // 回填位
    packet.put_short(putCount);
    for (int i = 0; i <= 0x77; ++i)                  // asm cmpl $0x77, setle
    {
        // ORIG 循环 `mov -0x14(%ebp),%edx` 基址取自具名槽（非 0x10(%ebp) 形参槽）⇒
        // 元素访问经同一具名 base（pPkt）而非形参，避免第二指针槽（P89/P0）。
        // 残：ORIG `mov -0xc(%ebp),%eax; mov -0x14(%ebp),%edx; imul; lea`（先 i 后 base）
        // vs 4.4 恒 base-first（`i + ptr` 形实测同 base-first）⇒ P218/P30 装载序封顶。
        if (reinterpret_cast<const Inter_NoticeGuildCargoTail*>(
                (reinterpret_cast<const Inter_NoticeGuildCargoEntry*>(pPkt) + i)
                    ->m_body.m_bytes)->m_3 != 0)
        {
            packet.put_short((short)i);              // ORIG `cwtl`（源码 (short) 截断）
            packet.put_binary(
                (reinterpret_cast<const Inter_NoticeGuildCargoEntry*>(pPkt) + i)
                    ->m_body.m_bytes + 2, 0x35);
            ++putCount;
        }
    }
    packet.put_short(countIdx, putCount);            // put_short(int&,int) 回填
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_GuildCargoHistory::~Inter_GuildCargoHistory() {}
// ORIG 0x084dd69e T dispatch_sig —— 公会仓库历史查询：组 0x10a 包回发并记历史。
// 报告 docs/class_func_reports/Inter_GuildCargoHistory/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildCargoHistoryPktView —— Inter_GuildCargoHistory::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GuildCargoHistoryPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=100（ORIG 0x084dd69e）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=49（ORIG 0x84dd69e）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_GuildCargoHistory::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoHistoryPktView* pPkt = reinterpret_cast<Inter_GuildCargoHistoryPktView*>(data);
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
        return 0;

    int putCount = 0;                                // local_14/asm -0x10(%ebp)
    PacketGuard packet;
    packet.put_header(1, 0x10a);
    packet.put_byte(1);
    // （asm("ebx") 钉形被 put_int(int&,int) 取址阻断，保持具名槽。）
    int countIdx = packet.get_index();         // 计数回填位
    packet.put_int(putCount);
    for (int i = 0; i <= 0x31; ++i)                  // asm cmpl $0x31, setle
    {
        if (*reinterpret_cast<int*>(
                data + (i * 3 + 1) * 0x10 + 2) != 0)
        {
            packet.put_binary(
                data + (i * 3 + 1) * 0x10 + 2, 0x30);
            ++putCount;
        }
    }
    packet.put_int(countIdx, putCount);              // put_int(int&,int) 回填
    packet.finalize(true);
    user->Send(packet);
    user->m_historyLog.GuildCargoHistoryResult(
        (ENUM_GUILD_CARGO_HISTORY_REASON)4, putCount);
    return 0;
}

Inter_GuildCargoCheckPushItem::~Inter_GuildCargoCheckPushItem() {}
// ORIG 0x084dd856 T dispatch_sig —— 公会仓库上架申请：校验角色/物品/期限后删除
// 背包物品并把物品内容打包转发给公会服务器代理。
// 报告 docs/class_func_reports/Inter_GuildCargoCheckPushItem/dispatch_sig.md 全文转写。
// 依赖 Packet_Guild_Guild_Cargo_Push_Item（NEEDS_DECL，见同目录 meta）。
class DnfItemInfoView
{
public:
    unsigned char m_seal;
    unsigned int m_itemId;
    unsigned char m_attr;
    unsigned int m_addInfo;
    unsigned short m_endurance;
    unsigned int m_extendInfo;
    unsigned char m_abilityType;
    unsigned short m_abilityValue;
    RandomOption m_opt;
    UpgradeSeparateInfo m_up;
};

// ORIG ctor 0x084e9434：PacketHeader(0x70d,0x53) → 字段清零 → DnfItemInfo(+0x1b) 构造
// → m_e/m_f 清零。m_item 为字节区（Guild 域同名类冲突），placement-new 显式构造
// （多一次 operator new 调用，形态残留；gcc 4.4.7 拒绝显式构造调用，语义与 ORIG 一致）。
Packet_Guild_Guild_Cargo_Push_Item::Packet_Guild_Guild_Cargo_Push_Item()
    : PacketHeader(0x70d, 0x53),
      m_guildKey(0), m_charNo(0), m_b(0), m_d(0), m_itemType(0), m_item()
{
    m_e = 0;
    m_f = 0;
}

#pragma pack(push, 1)
// Inter_GuildCargoCheckPushItemPktView —— Inter_GuildCargoCheckPushItem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildCargoCheckPushItemPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    unsigned char m_12;       // +0x12
    int m_13;                 // +0x13
    int m_17;                 // +0x17
    int m_1b;                 // +0x1b
    unsigned int m_1f;        // +0x1f
    char m_pad23[0x1];  // 未消费
    unsigned char m_24;       // +0x24
    unsigned short m_25;      // +0x25
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=404（ORIG 0x084dd856）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-13。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84dd856）：本轮 memcpy→RandomOption 结构赋值（N84/P83，p167 CLEAR）；余 N22/N18a/P30。
int Inter_GuildCargoCheckPushItem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoCheckPushItemPktView* pPkt = reinterpret_cast<Inter_GuildCargoCheckPushItemPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $0x2,setle → state>2 才放行
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_e)          // msg+0x0e characNo
        return 0;

    if (pPkt->m_12 != 0xc1)   // 失败结果上报分支
    {
        user->m_historyLog.GuildCargoCheckPushItemResult(
            (ENUM_GUILD_CARGO_HISTORY_REASON)6,
            pPkt->m_12,
            pPkt->m_13,
            pPkt->m_17,
            pPkt->m_1b,
            pPkt->m_24,
            pPkt->m_25);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb,
                                 pPkt->m_12);
        return 0;
    }

    Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(   // -0x71 栈槽
        (int)GetInvenTypeFromItemSpace(
            (ENUM_ITEMSPACE)pPkt->m_24),  // msg+0x24 itemSpace
        (int)pPkt->m_25);    // msg+0x25 slot

    if (pPkt->m_17 == 0 ||
        item.m_addInfo != pPkt->m_17)   // 客户端槽位核对
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);      // -0x28
    if (ci == 0 || ci->isPackagable() != 1 || ci->GetCashPrice() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    if (ci->IsImpossibleCharacCargo())                 // ORIG vtable+0x40 槽直调
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    if (IsTradeLimitAttachTypeItem(item))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    if (ci->getUsablePeriod() != 0 || ci->getExpirationDate() != 0)
    {
        const int expireSec = (int)item.m_fieldb * 0x15180 + 0x44a54a80;
        if (expireSec <= GlobalData::s_systemTime_.getCurSec())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
            return 0;
        }
    }

    if (ci->is_stackable() != 0 &&
        pPkt->m_1b > item.m_addInfo2)   // 堆叠数不足
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc6);
        return 0;
    }

    const int attachType = ci->GetAttachType();
    if (attachType == 1 || attachType == 2 || attachType == 5 ||
        (attachType == 3 && item.m_field0 != 1) ||
        item.m_upgradeSep.IsTradeRestriction())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    if (user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)item.GetInventoryType(),
            pPkt->m_25,
            pPkt->m_1b,
            (eItemDelReason)0x22, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    user->SaveInventory();

    Packet_Guild_Guild_Cargo_Push_Item pck;            // ORIG ctor 0x084e9434
    DnfItemInfoView& pckItem = *reinterpret_cast<DnfItemInfoView*>(&pck.m_item);  // +0x1b DnfItemInfo 视图（头内 m_item 为字节区）
    pck.m_guildKey = (unsigned int)user->get_charac_guildkey();   // +0x0a
    pck.m_charNo   = (unsigned int)user->getCurCharacNo();        // +0x0e
    pck.m_b        = (int)pPkt->m_13;   // +0x12
    pck.m_d        = pPkt->m_1f;   // +0x16
    pckItem.m_itemId     = (unsigned int)item.m_addInfo;       // +0x1c
    pckItem.m_attr       = item.GetItemAttr();                 // +0x20
    pckItem.m_addInfo    = (unsigned int)item.m_addInfo2;      // +0x21
    pckItem.m_endurance  = item.m_fieldb;                      // +0x25
    pckItem.m_extendInfo = (unsigned int)item.m_fieldd;        // +0x27
    item.m_amp.getValues(pckItem.m_abilityType, pckItem.m_abilityValue);
    // p167：ORIG 无 memcpy call ⇒ 结构整体赋值形（N84/P83）。
    *reinterpret_cast<RandomOption*>(&pckItem.m_opt) =    // +0x1d ← item+0x25
        *reinterpret_cast<const RandomOption*>(
            reinterpret_cast<const unsigned char*>(&item) + 0x25);
    pckItem.m_up.SetUpgradeSeparate(item.m_upgradeSep.GetUpgradeSeparate());
    pck.m_e = pPkt->m_24;     // +0x50
    pck.m_f = pPkt->m_25;    // +0x51
    if (ci->is_stackable() != 0)                                  // 堆叠件记录申请数
        pck.m_d     = (unsigned int)pPkt->m_1b;  // +0x16 覆写
    else
        pck.m_d     = (unsigned int)item.m_addInfo2;

    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pck), (int)pck.m_f);

    user->m_historyLog.GuildCargoPushItem(
        (ENUM_GUILD_CARGO_HISTORY_REASON)7,
        (int)pck.m_b, (int)pckItem.m_itemId, (int)pck.m_d,
        (int)pckItem.m_endurance, (int)pckItem.m_extendInfo,
        (int)pckItem.m_attr, pckItem.m_up.GetUpgradeSeparate());
    return 0;
}

Inter_GuildCargoPushItem::~Inter_GuildCargoPushItem() {}
// ORIG 0x084ddea2 T dispatch_sig —— 公会仓库上架结果：成功回 0xfb 确认包；
// 失败则把物品从 msg 载荷重建入背包（失败再走系统邮件回收）。
// 报告 docs/class_func_reports/Inter_GuildCargoPushItem/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildCargoPushItemPktView —— Inter_GuildCargoPushItem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildCargoPushItemPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    unsigned char m_12;       // +0x12
    int m_13;                 // +0x13
    char m_pad17[0x1];  // 未消费
    int m_18;                 // +0x18
    unsigned char m_1c;       // +0x1c
    int m_1d;                 // +0x1d
    unsigned short m_21;      // +0x21
    int m_23;                 // +0x23
    unsigned char m_27;       // +0x27
    unsigned short m_28;      // +0x28
    char m_pad2a[0x18];       // +0x2a..未消费
    UpgradeSeparateInfo m_42; // +0x42 §4.3 字段化（1B class，ORIG 读 UpgradeSeparate）
    char m_pad43[0x9];        // +0x43..未消费
    unsigned char m_4c;       // +0x4c
    unsigned short m_4d;      // +0x4d
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=291（ORIG 0x084ddea2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+6。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84ddea2）：本轮 memcpy→RandomOption 结构赋值（N84/P83，p167 CLEAR）；余 N22/N18a/P30。
int Inter_GuildCargoPushItem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoPushItemPktView* pPkt = reinterpret_cast<Inter_GuildCargoPushItemPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $0x2,setle
        return 0;
    if (user->get_charac_no(-1) !=
        pPkt->m_e)
        return 0;

    user->m_historyLog.GuildCargoPushItemResult(
        (ENUM_GUILD_CARGO_HISTORY_REASON)8,
        pPkt->m_12,
        pPkt->m_13,
        pPkt->m_18,
        pPkt->m_1d,
        pPkt->m_21,
        pPkt->m_23);

    if (pPkt->m_12 == 0xc1)
    {   // 公会服务器确认成功
        PacketGuard packet;                            // -0x30
        packet.put_header(1, 0xfb);
        packet.put_byte(1);
        packet.put_byte((int)pPkt->m_4c);
        packet.put_short((int)pPkt->m_4d);
        packet.put_int(pPkt->m_1d);
        packet.put_short(pPkt->m_13);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb,
                             pPkt->m_12);

    CInventory* inven = user->getCurCharacInvenW();    // -0x20
    CItem* ci = G_CDataManager()->find_item(           // -0x1c
        pPkt->m_18);

    Inven_Item item;                                   // -0x6d（默认构造）
    ci->make_item(item);                               // ORIG vtable+0x08 槽直调

    item.m_addInfo = pPkt->m_18;
    item.m_addInfo2 = pPkt->m_1d;
    item.m_fieldb = pPkt->m_21;   // endurance
    item.m_fieldd = pPkt->m_23;
    item.SetItemAttr(pPkt->m_1c);
    item.m_amp.assign(pPkt->m_27,
                      pPkt->m_28);
    // RandomOption ← msg+0x34（+0x25 = m_random.m_random 连续区）
    // p167：ORIG 无 memcpy call ⇒ 结构整体赋值形（N84/P83）。
    item.m_random.m_random = *reinterpret_cast<const RandomOption*>(data + 0x34);
    item.m_upgradeSep.SetUpgradeSeparate(
        pPkt->m_42.GetUpgradeSeparate());

    const int slot = inven->insertItemIntoInventory(   // ORIG 0x08502d86
        item, (eItemAddReason)0x27, true, false);

    if (slot < 0)
    {   // 入包失败：系统邮件回收 + 恢复日志
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "game_server_msg_114", 0),
            item, 0,
            pPkt->m_e,
            reinterpret_cast<const char*>(0), 0, 0,
            (ENUM_SERVER_GROUP)user->GetServerGroup(), false, false);
        user->m_historyLog.GuildCargoItemRecovery(
            (ENUM_GUILD_CARGO_HISTORY_REASON)9,
            pPkt->m_18,
            pPkt->m_1d,
            (int)pPkt->m_21,
            pPkt->m_23,
            (int)pPkt->m_1c,
            pPkt->m_42.GetUpgradeSeparate());
    }
    return 0;
}

Inter_GuildCargoPopItem::~Inter_GuildCargoPopItem() {}
#pragma pack(push, 1)
// Inter_GuildCargoPopItemPktView —— Inter_GuildCargoPopItem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GuildCargoPopItemPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
    char m_pad16[0x1];
    int m_17;                       // +0x17
    unsigned char m_1b;             // +0x1b
    int m_1c;                       // +0x1c
    unsigned short m_20;            // +0x20
    int m_22;                       // +0x22
    unsigned char m_26;             // +0x26
    unsigned short m_27;            // +0x27
    char m_pad29[0xa];  // +0x29..未消费
    unsigned int m_33;              // +0x33
    unsigned int m_37;              // +0x37
    unsigned int m_3b;              // +0x3b
    unsigned short m_3f;            // +0x3f
    unsigned char m_41;             // +0x41
    char m_pad42[0x9];  // +0x42..未消费
    unsigned char m_4b;             // +0x4b
    int m_4c;                       // +0x4c
};
#pragma pack(pop)

// §6.2 巨件核销：P167 42/42 [0缺0多]；gap -7→-7；残差归因 N82（4.4 槽复用全帧错位，
// 详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-lane2] verdict=DIFF ndiff=544（ORIG 0x084de330）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=465（ORIG 0x84de330）：N22 cmp 操作数方向+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_GuildCargoPopItem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
Inter_GuildCargoPopItemPktView* pPkt = reinterpret_cast<Inter_GuildCargoPopItemPktView*>(data);
    // ORIG 0x084de330 报告 docs/class_func_reports/Inter_GuildCargoPopItem/dispatch_sig.md
    // 两段全读。msg 布局：+0x0e characNo / +0x12 resultByte / +0x17 itemIdx /
    // +0x1c addInfo2 / +0x20 endurance(u16) / +0x22 fieldd / +0x1b SetItemAttr /
    // +0x26..0x27 amp.assign / +0x33..0x41 RandomOption 区 15B 块 / +0x4b errByte /
    // +0x4c logIdx。
    if (user->get_state() <= 2)
        return 0;
    if (user->get_charac_no(-1) != pPkt->m_e)
        return 0;

    user->m_historyLog.GuildCargoPopItemResult(
        (ENUM_GUILD_CARGO_HISTORY_REASON)0xb,
        pPkt->m_4b,
        pPkt->m_4c,
        pPkt->m_17,
        pPkt->m_1c,
        pPkt->m_20,
        pPkt->m_22);

    if (pPkt->m_4b != 0xc1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfc,
                                 pPkt->m_4b);
        return 0;
    }

    CInventory* inven = user->getCurCharacInvenW();
    CDataManager* dm = G_CDataManager();
    Inven_Item item;                       // ORIG 局部 Inven_Item C1 @ebp-0x85
    CItem* ci = dm->find_item(pPkt->m_17);
    if (ci == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x4a6e, 0)("ERROR OCCURRED! GUILD CARGO POP ITEM - fail find item(%d), User(%d)",
              pPkt->m_17, user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfc, 0xc5);
        return 0;
    }

    ci->make_item(item);                   // vtable 槽 +0x08（make_item 出参）
    item.m_addInfo = pPkt->m_17;
    item.m_addInfo2 = pPkt->m_1c;
    item.m_fieldb = pPkt->m_20;
    item.m_fieldd = pPkt->m_22;
    item.SetItemAttr(pPkt->m_1b);
    item.m_amp.assign(pPkt->m_26,
                      pPkt->m_27);
    // RandomOption 连续块（item+0x25..0x32）与升级分离位（item+0x33）：
    // 网络消息到跨字段连续区的按块拷贝（ORIG 五段 disp 存储指令；
    // 同款按块视图先例 CMailBoxHelper.cpp:514 sig_at(&item,0x25)）。
    // +0x25..+0x32 = m_random.m_random（ChronicleRandomOptionInfo+0x06）连续区块写
    *(unsigned int*)&item.m_random.m_random.m_field0 =
        pPkt->m_33;
    *(unsigned int*)&item.m_random.m_random.m_field3 =
        pPkt->m_37;
    *(unsigned int*)&item.m_random.m_random.m_field6 =
        pPkt->m_3b;
    *(unsigned short*)&item.m_random.m_random.m_seed9 =
        pPkt->m_3f;
    item.m_upgradeSep.m_field = pPkt->m_41;

    int firstResult = 0;                   // 试占位结果（包内回发槽号）
    if (ci->IsCreatureItem())              // vtable 槽 +0x14
        firstResult = inven->tryInsertItemIntoCreature(item);
    else if (ci->IsAvatarItem())           // vtable 槽 +0x10
        firstResult = inven->tryInsertAvatarIntoInventory(item);
    else
        firstResult = inven->tryInsertItemIntoInventory(item);

    if (firstResult < 0)
    {
        const char* title =
            g_scriptStringManager_.findString(4, "game_server_msg_114",
                                              reinterpret_cast<bool*>(0));
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, item, 0u, static_cast<unsigned int>(pPkt->m_e),
            reinterpret_cast<const char*>(0), 0, 0u, user->GetServerGroup(),
            false, false);
        user->m_historyLog.GuildCargoItemRecovery(
            (ENUM_GUILD_CARGO_HISTORY_REASON)0xc,
            pPkt->m_17,
            pPkt->m_1c,
            pPkt->m_20,
            pPkt->m_22,
            pPkt->m_1b,
            (*reinterpret_cast<UpgradeSeparateInfo*>(&pPkt->m_41))   // §4.3 具名字段取址替代裸偏移 data+0x41
                .GetUpgradeSeparate());
    }
    else
    {
        int second = 0;
        if (ci->IsCreatureItem())
            second = inven->insertItemIntoCreature(
                item, (eItemAddReason)0x28, true, false);
        else if (ci->IsAvatarItem())
            second = inven->insertAvatarIntoInventory(
                item, (eAvatarItemAddReason)0x12, true);
        else
            // ORIG 0x084de1c7：普通物品实际插入（insertItemIntoInventory，
            // reason=0x28/bLog=true/b=false）；NEW 原 tryInsert* 成功后
            // 无实际插入分支 → 物品丢失，属转写缺失。
            second = inven->insertItemIntoInventory(
                item, (eItemAddReason)0x28, true, false);
        if (second < 0)
        {
            const char* title =
                g_scriptStringManager_.findString(4, "game_server_msg_114",
                                                  reinterpret_cast<bool*>(0));
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, item, 0u,
                static_cast<unsigned int>(pPkt->m_e),
                reinterpret_cast<const char*>(0), 0, 0u, user->GetServerGroup(),
                false, false);
            user->m_historyLog.GuildCargoItemRecovery(
                (ENUM_GUILD_CARGO_HISTORY_REASON)0xc,
                pPkt->m_17,
                pPkt->m_1c,
                pPkt->m_20,
                pPkt->m_22,
                pPkt->m_1b,
                (*reinterpret_cast<UpgradeSeparateInfo*>(&pPkt->m_41))   // §4.3 具名字段取址替代裸偏移 data+0x41
                    .GetUpgradeSeparate());
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xfc);
    packet.put_byte(1);
    packet.put_short(pPkt->m_4c);
    packet.put_byte(pPkt->m_12);
    packet.put_short(firstResult);
    packet.put_int(pPkt->m_1c);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_GuildCargoMoveItem::~Inter_GuildCargoMoveItem() {}
// ORIG 0x084debb4 T dispatch_sig —— 公会仓库移动结果：记历史日志并回执客户端。
// 报告 docs/class_func_reports/Inter_GuildCargoMoveItem/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildCargoMoveItemPktView —— Inter_GuildCargoMoveItem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildCargoMoveItemPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（[b28c-31] P211 字段侧旋钮：ORIG 0x084debd7
                              // `call get_charac_no; mov %eax,%edx; … cmp %eax,%edx`
                              // 为异型中转形 ⇒ 字段 unsigned 与 callee int 异型）
    unsigned char m_12;       // +0x12
    int m_13;                 // +0x13
    int m_17;                 // +0x17
};
#pragma pack(pop)

// [b28c-31] AE：P211 字段侧旋钮 —— 视图 m_e 改 unsigned int 与 callee
// get_charac_no(int) 异型，复现 ORIG 0x084debdd `mov %eax,%edx` 中转（113=113）。
int Inter_GuildCargoMoveItem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoMoveItemPktView* pPkt = reinterpret_cast<Inter_GuildCargoMoveItemPktView*>(data);
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    if (user->get_charac_no(-1) != pPkt->m_e)
        return 0;

    user->m_historyLog.GuildCargoMoveItemResult(       // +0x79700
        (ENUM_GUILD_CARGO_HISTORY_REASON)0xe,
        pPkt->m_12,
        pPkt->m_13,
        pPkt->m_17);

    PacketGuard packet;
    packet.put_header(1, 0xfd);
    // ORIG 0x084dec77 `cmp $0xc1,%al; je then-island`：false-path（else 体）在前
    // 作 fall-through（技巧17），then 为 taken 岛。
    if (pPkt->m_12 != 0xc1)
    {
        packet.put_byte(0);
        packet.put_byte((unsigned int)pPkt->m_12);
    }
    else
    {   // 结果码 0xc1 → 成功标记
        packet.put_byte(1);
    }
    packet.put_short(pPkt->m_13);
    packet.put_short(pPkt->m_17);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_GuildCargoUpgrade::~Inter_GuildCargoUpgrade() {}
// ORIG 0x084ded42 T dispatch_sig —— 公会仓库升级应答。
// 报告 docs/class_func_reports/Inter_GuildCargoUpgrade/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildCargoUpgradePktView —— Inter_GuildCargoUpgrade::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildCargoUpgradePktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e
    unsigned char m_12;       // +0x12
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=31（ORIG 0x084ded42）：槽序/帧差(N82/P212/P221)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：P272 分支极性+P196 字段 uchar 化+
// P0 删具名槽；末处删 `(int)` cast ⇒ 字段 unsigned 与 callee int 异型触发 edx 中转，与 ORIG 全等）。
int Inter_GuildCargoUpgrade::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoUpgradePktView* pPkt = reinterpret_cast<Inter_GuildCargoUpgradePktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    // P0：ORIG 无该局部槽（`mov %eax,%edx` 表达式直比）⇒ 内联，禁具名缓存。
    // ORIG `cmp %eax,%edx`（dst=call 结果）⇒ 书写序以 callee 调用为左操作数。
    if (user->get_charac_no(-1) != pPkt->m_e)
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x10d);
    // ORIG `je <then>`（then 块后置、else 直落）⇒ P272 反条件块序交换。
    if (pPkt->m_12 != 0xc1)
    {
        packet.put_byte(0);
        packet.put_byte((unsigned int)pPkt->m_12);
    }
    else
        packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_GuildBoardData::~Inter_GuildBoardData() {}
// ORIG 0x084dee6a T dispatch_sig —— 公会留言板数据下发：按请求码 0x232a/0x232e
// 组 0x155/0x156 回包，逐条 0xa5 记录转写。
// 报告 docs/class_func_reports/Inter_GuildBoardData/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildBoardDataPktView —— Inter_GuildBoardData::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GuildBoardDataPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned short m_a;             // +0x0a
    unsigned short m_c;             // +0x0c
    unsigned char m_e;              // +0x0e
    char m_padf[0x4];  // +0xf..未消费
    int m_13;                       // +0x13
    signed char m_17;               // +0x17
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=263（ORIG 0x084dee6a）：槽序/帧差(N82/P212/P221)+混合残差；gap+19。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=221（ORIG 0x84dee6a）：N22 cmp 操作数方向+N101 imul/lea 折叠+N18a 单段 regmove。
int Inter_GuildBoardData::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildBoardDataPktView* pPkt = reinterpret_cast<Inter_GuildBoardDataPktView*>(data);
    // ORIG 0x84dee8e/0x84deebc/0x84df274 三处 `mov $0x0,%ebx; jmp <epilogue>`（统一出口
    // `mov %ebx,%eax`）⇒ 原码为三个独立 `return 0`（P16 early-return 形，非单一出口）。
    if (user->get_state() <= 2)                        // cmp $0x2,setle
        return 0;
    if (user->get_charac_no(-1) !=
        pPkt->m_13)
        return 0;

    PacketGuard packet;                                // -0x44

    // §P0/P251：ORIG 对 msg+0xc 无槽往返（比较与 cMyTrace 实参各自重读 movzwl 双段）⇒ 不缓存 code。
    // ORIG 0x84deed4 为 `movzwl; movzwl %ax,%eax; cmp $0x232a,%eax`，4.4 恒折 `cmp $0x232a,%ax`
    // （显式 (unsigned int) 提升实测无效，已回退，P162/N7 封顶）。
    if (pPkt->m_c == 0x232a)
        packet.put_header(1, 0x155);
    else if (pPkt->m_c == 0x232e)
        packet.put_header(1, 0x156);

    if (pPkt->m_a == 0)
    {   // 正常数据分支
        packet.put_byte(1);
        packet.put_byte((int)pPkt->m_e);
        packet.put_byte((int)pPkt->m_17);
        // ORIG 全程以 -0x28 具名槽（pInfo）作基址（084df088 `add -0x28(%ebp),%edx` 等 9 处）
        // ⇒ 循环体基址走 pPkt 槽，不用形参 data。
#define pData (reinterpret_cast<char*>(pPkt))
        for (int i = 0;                                // -0x24
             i < (int)pPkt->m_17; ++i)
        {
            char flag = 0;                             // -0x1d
            if (*reinterpret_cast<unsigned char*>(pData + i * 0xa5 + 0x9e) == 1)
                flag = 1;
            packet.put_byte((int)(unsigned char)flag);
            packet.put_byte((int)*reinterpret_cast<signed char*>(pData + i * 0xa5 + 0x9c));
            if (user->get_charac_no(-1) ==
                *reinterpret_cast<int*>(pData + i * 0xa5 + 0x98))
                packet.put_byte(1);
            else
                packet.put_byte(0);

            packet.put_int((int)strlen(pData + i * 0xa5 + 0x9f));  // +0x9f 留言文本
            packet.put_str(pData + i * 0xa5 + 0x9f,
                           (int)strlen(pData + i * 0xa5 + 0x9f));
            packet.put_int(*reinterpret_cast<int*>(pData + i * 0xa5 + 0x94));

            int remainSec =                            // -0x1c
                GlobalData::s_systemTime_.getCurSec() -
                *reinterpret_cast<int*>(pData + i * 0xa5 + 0x90);
            if (remainSec < 0)
                remainSec = 0;
            packet.put_int(remainSec);

            packet.put_int((int)strlen(pData + i * 0xa5 + 0x18));  // +0x18 标题
            packet.put_str(pData + i * 0xa5 + 0x18,
                           (int)strlen(pData + i * 0xa5 + 0x18));
        }
#undef pData

        // P328⑩/P227：ORIG 0x84df1a2..0x84df20a 先求 4 个实参（m_17→-0x4c、m_e→%edi、
        // m_c→%esi、get_charac_no→%ebx），其后才 cMyTrace ctor ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x4b51, 0)(
            "GUILD_BOARD : CHARAC NO : %u, CODE : %u, TOTAL CNT : %u, CNT : %u : BOARD DATA SEND!!",
            user->get_charac_no(-1), (unsigned int)pPkt->m_c,
            (unsigned int)pPkt->m_e,
            (int)pPkt->m_17);
    }
    else
    {   // 错误码分支
        packet.put_byte(0);
        packet.put_short((int)(unsigned int)
                         pPkt->m_a);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_GuildBoardDelete::~Inter_GuildBoardDelete() {}
// ORIG 0x084df2ac T dispatch_sig —— 公会留言板删除应答。
// 报告 docs/class_func_reports/Inter_GuildBoardDelete/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildBoardDeletePktView —— Inter_GuildBoardDelete::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildBoardDeletePktView
{
    char m_pad0[0xa];  // 未消费
    short m_a;                // +0x0a
    char m_padc[0x7];  // 未消费
    unsigned int m_13;        // +0x13
};
#pragma pack(pop)

int Inter_GuildBoardDelete::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildBoardDeletePktView* pPkt = reinterpret_cast<Inter_GuildBoardDeletePktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    // ORIG +0x53：`mov %eax,%edx`+`mov -0xc(%ebp),%eax`+`mov 0x13(%eax),%eax` 两步 spill
    // ＝unsigned 字段与 get_charac_no(int) 失配；显式 `(int)` 会把失配折平 ⇒ 去掉转换。
    if (user->get_charac_no(-1) != pPkt->m_13)        // recv charId @ msg+0x13
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x157);
    if (pPkt->m_a == 0)
        packet.put_byte(1);
    else
    {
        packet.put_byte(0);
        packet.put_short(
            static_cast<int>(static_cast<unsigned short>(pPkt->m_a)));
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_TakeScreenShot::~Inter_TakeScreenShot() {}
// ORIG 0x084df3d6 T dispatch_sig —— 客户端截屏请求全网广播。
// 报告 docs/class_func_reports/Inter_TakeScreenShot/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_TakeScreenShotPktView —— Inter_TakeScreenShot::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_TakeScreenShotPktView
{
    char m_pad0[0xb];  // +0x0..未消费
    int m_b;                        // +0x0b
};
#pragma pack(pop)

int Inter_TakeScreenShot::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_TakeScreenShotPktView* pPkt = reinterpret_cast<Inter_TakeScreenShotPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0xf4);
    packet.put_int(pPkt->m_b);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_VillageAttackedStart::~Inter_VillageAttackedStart() {}
// ORIG 0x084df47a T dispatch_sig —— 村庄袭击开始：关闭极限地下城 + 全服广播 0xf1 包。
// 报告 docs/class_func_reports/Inter_VillageAttackedStart/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedStartPktView —— Inter_VillageAttackedStart::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedStartPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
};
#pragma pack(pop)

// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换（137=137）。
// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084df47a）：混合残差；gap0。
int Inter_VillageAttackedStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;

    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||                 // env+0x1a8（NEEDS_DECL 字段化）
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;

    if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff))
    {
        G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, false);
        G_GameWorld()->send_all_dungeon_inout_message(0x2aff, false);
        // [b28d-lane2] P221：ORIG 0x084df548 trace 占 -0x2c、else 支 -0x1c、packet -0x38
        // （深→浅）。具名 cMyTrace 使 4.4 槽向翻转（trace -0x38/packet -0x28）；
        // 改全表达式临时链复现 ORIG 槽序。
        cMyTrace(__PRETTY_FUNCTION__, 0x4b9a, 0)(
            "ULTIMATE_LOG : VILLAGE ATTACKTED START!! ULTIMATE DUNGEON CLOSE!!");
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x4b9d, 0)(
            "ULTIMATE_LOG : ALLEADY ULTIMATE DUNGEON CLOSE!!");
    }

    // ORIG 0x084df51f：pPkt 槽 -0xc 在 trace 区后才物化（技巧13）。
    Inter_VillageAttackedStartPktView* pPkt = reinterpret_cast<Inter_VillageAttackedStartPktView*>(data);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf1);
    packet.put_int(0);
    packet.put_int(pPkt->m_a);
    packet.put_int(pPkt->m_e);
    packet.put_int(pPkt->m_12);
    packet.put_int(0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    GlobalData::s_villageMonsterMgr->OnStartVillageAttacked();
    return 0;
}

Inter_VillageAttackedEnd::~Inter_VillageAttackedEnd() {}
// ORIG 0x084df6a4 T dispatch_sig —— 村庄攻防结束：开/关复仇副本、遍历在线用户
// 发 0x1ec 结束包（疲劳结算、复仇次数落库），最后广播奖励类型。
// 报告 docs/class_func_reports/Inter_VillageAttackedEnd/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedEndPktView —— Inter_VillageAttackedEnd::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedEndPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    unsigned int m_e;               // +0x0e
    unsigned int m_12;              // +0x12
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=267（ORIG 0x084df6a4）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+9。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84df6a4）：本轮 P28 后置 it++ + P157/P183 revenge 表逐字段重读（G_CDataManager×4/vector[]×3，p167 CLEAR）；余 P226/N18a/N82。
int Inter_VillageAttackedEnd::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_VillageAttackedEndPktView* pPkt = reinterpret_cast<Inter_VillageAttackedEndPktView*>(data);
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())   // test al → false 才继续
        return 0;

    const unsigned int curSec =                        // -0x38
        GlobalData::s_systemTime_.getCurSec();
    const unsigned int closeTime =                     // -0x34（msg+0x0a）
        pPkt->m_a;
    const int remainSec =                              // -0x34 复用（Ghidra local_34）
        (closeTime < curSec) ? 0 : (int)(closeTime - curSec);
    int state;                                         // -0x30

    if (pPkt->m_e <
        pPkt->m_12)
    {
        GlobalData::s_revengeDungeonMgr->CloseRevengeDungeon();
        GlobalData::s_revengeDungeonMgr->SetCloseTime(closeTime);
        state = 2;
    }
    else
    {
        GlobalData::s_revengeDungeonMgr->OpenRevengeDungeon();
        GlobalData::s_revengeDungeonMgr->SetCloseTime(closeTime);
        state = 1;
    }

    for (std::map<unsigned short, CUser*>::iterator it =   // GameWorld+0x134
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)   // P28：ORIG 增量段 ppEi ⇒ 后置++
    {
        CUser* target = it->second;                    // ORIG pair+4
        if (target == 0)
            continue;

        char buf[10];                                  // -0x5a
        memset(buf, 0, sizeof(buf));
        PacketGuard packet;                            // -0x68
        packet.clear();
        packet.put_header(0, 0x1ec);
        target->VillageAttack_NotDBUpdate();

        if (state == 1)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                target, (APSystem::_ActionGroupIndex)0x17, 0,
                (ENUM_PACKETCLASS)0);
            unsigned char inDungeon = 0;               // -0x22
            const unsigned char atkCnt =               // -0x21
                target->GetCurVillageAttackCount();
            buf[0] = 0;
            buf[1] = 0;
            *reinterpret_cast<int*>(buf + 2) = remainSec;
            const int st = target->get_state();        // -0x20
            if (st > 2)
            {   // 疲劳扣减（-5，下限 0）并回发
                const int fatigue =
                    std::max(0, (int)target->getCurCharacFatigue() - 5);
                target->setCurCharacFatigue(fatigue);
                target->SendFatigue();
            }
            if (st == 3)
            {
                inDungeon = 1;
                if (atkCnt > 2)
                    *reinterpret_cast<int*>(buf + 6) = 0x1e41;

                typedef std::vector<ConditionEnterRevenge> RevengeCondVec;
                // ORIG 0x84df8e4/905/924/944 四处独立 G_CDataManager + 三次 vector::operator[]
                //（p167 M=G_CDataManager×3 / vector[]×2）⇒ 逐字段重读（P157/P183），禁缓存引用。
#define S207_REVENGE_CONDS (*reinterpret_cast<RevengeCondVec*>(&G_CDataManager()->m_serverParameter.m_revenge498))
                const unsigned int condCnt = (unsigned int)S207_REVENGE_CONDS.size();
                for (unsigned int i = 0; i < condCnt; ++i)
                {
                    const unsigned int lo =            // 元素 +0x00
                        *reinterpret_cast<const unsigned int*>(&S207_REVENGE_CONDS[i]);
                    const unsigned int hi =            // 元素 +0x04
                        *reinterpret_cast<const unsigned int*>(
                            reinterpret_cast<const char*>(&S207_REVENGE_CONDS[i]) + 4);
                    const int cnt =                    // 元素 +0x08
                        *reinterpret_cast<const int*>(
                            reinterpret_cast<const char*>(&S207_REVENGE_CONDS[i]) + 8);
                    if (lo <= (unsigned int)atkCnt &&
                        (unsigned int)atkCnt <= hi)
                    {
                        target->SetCurRevengeDungeonCount((unsigned char)cnt);
                        target->VillageAttack_DBUpdate();
                        break;
                    }
                }
#undef S207_REVENGE_CONDS
            }
        }
        else
        {
            GlobalData::s_revengeDungeonMgr->CloseRevengeDungeon();
            GlobalData::s_revengeDungeonMgr->SetCloseTime(closeTime);
            buf[0] = 1;
            buf[1] = 0;
            *reinterpret_cast<int*>(buf + 2) = remainSec;
            *reinterpret_cast<int*>(buf + 6) = 0;
        }

        packet.put_binary(buf, 10);
        packet.finalize(true);
        target->SetVillageAttackedReward(false);
        target->Send(packet);
    }

    GlobalData::s_villageMonsterMgr->OnDestroyVillageMonster(
        (ENUM_VILLAGE_ATTACKED_REWARD)state);
    return 0;
}

Inter_VillageAttackedCountdown::~Inter_VillageAttackedCountdown() {}
// ORIG 0x084dfae0 T dispatch_sig —— 村庄攻击倒计时广播。
// 报告 docs/class_func_reports/Inter_VillageAttackedCountdown/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedCountdownPktView —— Inter_VillageAttackedCountdown::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedCountdownPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

int Inter_VillageAttackedCountdown::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // b28c-33：ORIG 0x84dfaf8/0x84dfb03 为 bool callee 直分支 `test %al; je 主体`
    //（无 xor 物化），且短路链两臂 `je L_true`/`jne L_false` ⇒ 早返形（正向包裹恒
    // 多一条 `xor $1` 且 ==1||==2 物化方向相反）。pPkt 局部赋值在守卫之后（ORIG +0x5d）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;
    Inter_VillageAttackedCountdownPktView* pPkt = reinterpret_cast<Inter_VillageAttackedCountdownPktView*>(data);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf1);
    packet.put_int(pPkt->m_a);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_VillageAttackedUpdate::~Inter_VillageAttackedUpdate() {}
// ORIG 0x084dfbe6 T dispatch_sig —— 村庄攻击状态全员更新广播。
// 报告 docs/class_func_reports/Inter_VillageAttackedUpdate/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedUpdatePktView —— Inter_VillageAttackedUpdate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedUpdatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
};
#pragma pack(pop)

int Inter_VillageAttackedUpdate::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [b28c-32] ORIG 0x084dfbfb/0x084dfbfd 为 `test %al; je 主体`（bool callee 直分支，
    // 无 xor 物化）⇒ 早返形；0x084dfc24 `jne false 块` = P45 单 return 短路链；pPkt 局部
    // 赋值位于两守卫之后（ORIG +0x5d 才 mov 0x10(%ebp)→-0xc）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    // ORIG 0x84dfc08/38：G_CEnvironment() 短路双调（O0 不 CSE）。==1||==2 → 不广播。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;
    Inter_VillageAttackedUpdatePktView* pPkt = reinterpret_cast<Inter_VillageAttackedUpdatePktView*>(data);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf7);
    packet.put_int(pPkt->m_a);
    packet.put_int(pPkt->m_e);
    packet.put_int(pPkt->m_12);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_VillageAttackedScore::~Inter_VillageAttackedScore() {}
// ORIG 0x084dfd16 T dispatch_sig —— 村庄袭击比分：多门校验后转发四方比分。
// 报告 docs/class_func_reports/Inter_VillageAttackedScore/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedScorePktView —— Inter_VillageAttackedScore::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedScorePktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（同上 unsigned→int 失配两步 spill 形态）
    int m_12;                       // +0x12
    int m_16;                       // +0x16
    int m_1a;                       // +0x1a
    int m_1e;                       // +0x1e
};
#pragma pack(pop)

int Inter_VillageAttackedScore::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() != '\0')   // 门1：整合频道拒绝
        return 0;

    // CEnvironment+0x1a8 环境槽未在权威头建模（沿 CUserOnlyOrig.cpp:9027 先例，
    // §4.3 例外保留地址形式；ORIG 两次独立 G_CEnvironment 调用，P48）
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;
    if (user->getCurCharacR() == 0)                            // 门3：当前角色必须存在
        return 0;
    // ORIG：包基址槽在门 3（getCurCharacR）之后才物化。
    Inter_VillageAttackedScorePktView* pPkt = reinterpret_cast<Inter_VillageAttackedScorePktView*>(data);

    if (user->get_charac_no(-1) != pPkt->m_e)
        return 0;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf8);
    packet.put_int(pPkt->m_12);
    packet.put_int(pPkt->m_16);
    packet.put_int(pPkt->m_1a);
    packet.put_int(pPkt->m_1e);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_VillageAttackedReward::~Inter_VillageAttackedReward() {}
// ORIG 0x084dfea6 T dispatch_sig —— 村庄攻防奖励发放：按奖励类型（win point /
// 排名 / 参战）检索 Arad_VillageAttackRewardItem 脚本并发奖。
// 报告 docs/class_func_reports/Inter_VillageAttackedReward/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_VillageAttackedRewardPktView —— Inter_VillageAttackedReward::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedRewardPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    int m_e;                        // +0x0e
    int m_12;                       // +0x12
    int m_16;                       // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=283（ORIG 0x084dfea6）：槽序/帧差(N82/P212/P221)+混合残差；gap-20。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=328（ORIG 0x84dfea6）：N22 cmp 操作数方向+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_VillageAttackedReward::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_VillageAttackedRewardPktView* pPkt = reinterpret_cast<Inter_VillageAttackedRewardPktView*>(data);
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())   // test al → false 才继续
        return 0;
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||   // +0x1a8
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;
    if (user == 0)                                     // cmp 0xc(%ebp),0
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_e)          // msg+0x0e characNo
        return 0;

    const int rewardType = pPkt->m_16;
    // ORIG 0x084dfea6：cmp 链 5→6→3（jne 尾），reward3 代码在比较链后 fallthrough，
    // reward5/6 代码跳转在后。保持 if 顺序 5→6→3 以匹配。
    if (rewardType == 5)
    {   // 胜利点数奖励
        Arad_VillageAttackRewardItem::Data rewardData(   // -0x64
            pPkt->m_12, 0, 0);
        const int winPoint = pPkt->m_12;
        ARAD::Arad_DataManager* adm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        Arad_VillageAttackRewardItem* reward =
            adm->getAradVillageAttackReward();
        if (reward->find_reward_item(winPoint, rewardData))
        {
            user->SetVillageAttackedReward(true);
            _send_reward_item(user, (unsigned long)rewardData.itemIndex,
                              rewardData.itemCount, rewardData.winPoint);
            cMyTrace(__PRETTY_FUNCTION__,
                0x4d7b, 0)("VillageAttacked Reward WinPoint Item Index. "
                  "[charac:%u][win:%d][item:%u][count:%d]",
                  user->get_charac_no(-1), rewardData.winPoint,
                  rewardData.itemIndex, rewardData.itemCount);
        }
    }
    else if (rewardType == 6)
    {   // 排名奖励
        Arad_VillageAttackRewardItem::RankData rankData;   // -0x74（memset 0x10）
        const short rank =
            (short)pPkt->m_12;   // movswl
        ARAD::Arad_DataManager* adm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        Arad_VillageAttackRewardItem* reward =
            adm->getAradVillageAttackReward();
        if (reward->find_ranking_reward_item(rank, rankData))
        {
            // ORIG dispatch 以两组并行 int 数组访问 out：
            // itemIdx = out[0..1]（-0x74+i*4）、count = out[2..3]（-0x6c+i*4）。
            int* fields = reinterpret_cast<int*>(&rankData);
            for (int i = 0; i <= 1; ++i)                   // cmpl $0x1,setle
            {
                if (fields[i] != 0)
                {
                    _send_reward_item(user, (unsigned long)fields[i],
                                      fields[2 + i],
                                      pPkt->m_12);
                    cMyTrace(__PRETTY_FUNCTION__,
                        0x4d8c, 0)("VillageAttacked Reward Ranker Item Index. "
                          "[charac:%u][rank:%d][item:%u][count:%d]",
                          user->get_charac_no(-1),
                          pPkt->m_12,
                          fields[i], fields[2 + i]);
                }
            }
        }
        else
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x4d92, "VillageAttackedReward Invalid Rank. [rank:%d]",
                pPkt->m_12);
            return 0;
        }
    }
    else if (rewardType == 3)
    {   // 参战奖励（固定物品）
        user->SetVillageAttackedReward(true);
        Inven_Item item;                               // -0xb1
        item.m_addInfo = 1;                            // +0x02 dword
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)3, item, 1);
        item.m_fieldb = 1;                             // +0x0b word（endurance）
        if (item.m_addInfo == -1)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x4d5f,
                "Inter_VillageAttackedReward::dispatch_sig() error item_id(%d)",
                -1);
        }
        else
        {
            const char* title =
                g_scriptStringManager_.findString(4, "game_server_msg_124", 0);
            char titleBuf[0x14];                       // -0x55
            strncpy(titleBuf, title, 0x14);
            const char* body =
                g_scriptStringManager_.findString(4, "game_server_msg_125", 0);
            char bodyBuf[0xff];                        // -0x1b1
            strncpy(bodyBuf, body, 0xff);
            const ENUM_SERVER_GROUP group =
                (ENUM_SERVER_GROUP)user->GetServerGroup();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                titleBuf, item, 0,
                (unsigned int)user->getCurCharacNo(), bodyBuf,
                (int)strlen(bodyBuf), 0x1e, group, false, false);
        }
    }
    return 0;
}

Inter_VillageAttackedRewardServer::~Inter_VillageAttackedRewardServer() {}
// ORIG 0x084e05c2 T dispatch_sig —— 村庄攻击奖励结束：组 0x1ec 包全网广播、清理复仇副本与角色状态。
// 报告 docs/class_func_reports/Inter_VillageAttackedRewardServer/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=119（ORIG 0x084e05c2）：槽序/帧差(N82/P212/P221)+混合残差；gap+7。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84e05c2）：本轮补 P253 memset(10) 显式形 + P28 后置 it++（p167 CLEAR）；余 N82/P112 帧槽平移。
int Inter_VillageAttackedRewardServer::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    // ORIG 读 env+0x1a8 = Server_Envir(+0x198)+0x10 = m_serverEnvir.m_serverType
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;

    // ORIG `call memset@plt`（Ghidra C `memset(local_26,0,10)`）⇒ 显式 memset 形（P253/P214③）。
    char local_26[10];
    memset(local_26, 0, 10);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1ec);

    const int local_14 =
        GlobalData::s_villageMonsterMgr->GetRewardType();   // ORIG _ZN16village_attacked18CVillageMonsterMgr13GetRewardTypeEv
    if (local_14 == 1)
        local_26[0] = '\0';
    else if (local_14 == 2)
        local_26[0] = '\x01';
    local_26[1] = 2;
    packet.put_binary(local_26, 10);
    packet.finalize(true);

    GlobalData::s_revengeDungeonMgr->CloseRevengeDungeon();
    GlobalData::s_revengeDungeonMgr->SetCloseTime(0);

    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)   // P28：ORIG 增量段 ppEi ⇒ 后置++
    {
        CUser* local_10 = it->second;
        if (local_10 != 0 && local_10->get_state() > 2)
        {
            local_10->SetCurCharacRevengeData('\0', '\0');
            local_10->VillageAttack_NotDBUpdate();
            local_10->Send(packet);
        }
    }

    GlobalData::s_villageMonsterMgr->OnEndRewardVillageMonster();
    return 0;
}

Inter_FullLevelBroadCast::~Inter_FullLevelBroadCast() {}
// ORIG 0x084e0816 T dispatch_sig —— 全服广播字符串（满级公告等）。
// 报告 docs/class_func_reports/Inter_FullLevelBroadCast/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_FullLevelBroadCastPktView —— Inter_FullLevelBroadCast::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_FullLevelBroadCastPktView
{
    char m_pad0[0xa];  // 未消费
    char m_a[1];              // +0xa 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_FullLevelBroadCast::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_FullLevelBroadCastPktView* pPkt = reinterpret_cast<Inter_FullLevelBroadCastPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x56);
    packet.put_byte(6);
    packet.put_int((int)strlen(pPkt->m_a));
    packet.put_str(pPkt->m_a, (int)strlen(pPkt->m_a));
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_Hack_CheckMyAccount::~Inter_Hack_CheckMyAccount() {}
// ORIG 0x084e08fc T dispatch_sig —— 账号自查异常登记 hack 计数。
// 报告 docs/class_func_reports/Inter_Hack_CheckMyAccount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Hack_CheckMyAccountPktView —— Inter_Hack_CheckMyAccount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_Hack_CheckMyAccountPktView
{
    int m_0;                        // +0x00
    char m_pad4[0x4];  // +0x4..未消费
    int m_8;                        // +0x08
};
#pragma pack(pop)

int Inter_Hack_CheckMyAccount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_Hack_CheckMyAccountPktView* pPkt = reinterpret_cast<Inter_Hack_CheckMyAccountPktView*>(data);
    if (pPkt->m_0 == 0)
    {
        // ORIG +0x3c 一步 `mov %eax,%ebx`（call 结果直落 callee-saved）＝register 变量形态；
        // const 普通局部会先落栈再取（多两行）。
        register int etc = user->SetETC(0x2c5, pPkt->m_8);
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x2c5, 1, (unsigned int)etc, 0);
    }
    return 0;
}

Inter_SetSecounUserData::~Inter_SetSecounUserData() {}
// ORIG 0x084e0972 T dispatch_sig —— 记录黑金没收用户的钱款入账。
// 报告 docs/class_func_reports/Inter_SetSecounUserData/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetSecounUserDataPktView —— Inter_SetSecounUserData::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SetSecounUserDataPktView
{
    unsigned long long m_0;   // +0x00
    unsigned long long m_8;   // +0x08
    unsigned int m_10;        // +0x10
    unsigned int m_14;        // +0x14
};
#pragma pack(pop)

int Inter_SetSecounUserData::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SetSecounUserDataPktView* pPkt = reinterpret_cast<Inter_SetSecounUserDataPktView*>(data);
    user->gainMoneyFromHackUser(
        pPkt->m_14,
        pPkt->m_0,
        pPkt->m_10,
        pPkt->m_8);
    return 0;
}

Inter_SetARSInfo::~Inter_SetARSInfo() {}
// ORIG 0x084e09ce T dispatch_sig —— 网吧自动惩罚规则(ARS)批量下发：reset 门 + 每条目
// setTotalTrade/setHackCnt 分发与 trace，条目 stride 0xc。
// 报告 docs/class_func_reports/Inter_SetARSInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SetARSInfoPktView —— Inter_SetARSInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SetARSInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    unsigned char m_e;              // +0x0e
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_SetARSInfoEntryWinView —— Inter_SetARSInfo::dispatch_sig 窗口 entry 元素视图
//   （entry 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_SetARSInfoEntryWinView
{
    char m_pad0[0xf];  // 未消费
    unsigned short m_f;       // +0x0f
    unsigned short m_11;      // +0x11
    unsigned short m_13;      // +0x13
    unsigned short m_15;      // +0x15
    unsigned char m_17;       // +0x17
    unsigned char m_18;       // +0x18
    unsigned short m_19;      // +0x19
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=259（ORIG 0x084e09ce）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+98。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=318（ORIG 0x84e09ce）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_SetARSInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SetARSInfoPktView* pPkt = reinterpret_cast<Inter_SetARSInfoPktView*>(data);
    if (pPkt->m_e == 0)
    {
        GlobalData::g_autoPunishRuleHackTypeMgr->reset();
        cMyTrace(__PRETTY_FUNCTION__, 0x4f5e, 0)("[ARS_INFO] Reset");
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x4f64, 0)("[ARS_INFO] Cnt : %d, State : %d",
          (int)pPkt->m_a, (int)(signed char)pPkt->m_e);

    if ((int)pPkt->m_a > 100)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x4f6b, 0)("[ARS_INFO] Cnt OverFlag(100)");
        return 0;
    }

    for (int i = 0; i < (int)pPkt->m_a; ++i)
    {
        if (((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_f == 0)
        {   // 交易总量条目：type=+0x11，total(u64)=+0x19
            GlobalData::g_autoPunishRuleHackTypeMgr->setTotalTrade(
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_11,
                (unsigned long long)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_19);
            cMyTrace(__PRETTY_FUNCTION__, 0x4f82, 0)("[ARS_INFO] TotalGold: %llu, TotalCount: %d",
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_19,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_11);
        }
        else
        {   // hack 计数条目：main=+0xf val=+0x11 sub=+0x13 val=+0x15 flag=+0x17 ipCnt=+0x18 arg=+0x19
            GlobalData::g_autoPunishRuleHackTypeMgr->setHackCnt(
                (WongWork::ENUM_HACKTYPE)(unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_f,
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_11,
                (WongWork::ENUM_HACKTYPE)(unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_13,
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_15,
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_17,
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_18,
                (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_19);
            cMyTrace(__PRETTY_FUNCTION__, 0x4f8e, 0)("[ARS_INFO] MainType : %d (%d), SubType : %d(%d), ApplyFlag %d, IpCnt : %d",
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_f,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_11,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_13,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_15,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_17,
              (unsigned int)((Inter_SetARSInfoEntryWinView*)((char*)pPkt + i * 0xc))->m_18);
        }
    }
    return 0;
}

Inter_OnTimeEventRewardStart::~Inter_OnTimeEventRewardStart() {}
// ORIG 0x084e0dc6 T dispatch_sig —— 定时活动奖励开始：0x33 号 repeat 事件广播 0x105 包。
// 报告 docs/class_func_reports/Inter_OnTimeEventRewardStart/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_OnTimeEventRewardStartPktView —— Inter_OnTimeEventRewardStart::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_OnTimeEventRewardStartPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    unsigned int m_e;         // +0x0e
    int m_12;                 // +0x12
    int m_16;                 // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=107（ORIG 0x084e0dc6）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=67（ORIG 0x84e0dc6）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→5（120 vs 115，TU 121/1331/5/133 不回退）：真修 cMyTrace 全表达式临时链
//   （P221/P227：`cMyTrace(f,l,v)(fmt, pPkt->m_e)` 使 m_e 在 ctor 前求值并 %ebx 跨 ctor 保活，trace 槽 -0x30/-0x20 与 ORIG 对齐）。
//   残余=N18a×2（process_set_reward_start 的 m_12/m_16 两段升格）+N7 虚调寄存器+else 分支 m_a 未升格。
int Inter_OnTimeEventRewardStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_OnTimeEventRewardStartPktView* pPkt = reinterpret_cast<Inter_OnTimeEventRewardStartPktView*>(data);
    cMyTrace(__PRETTY_FUNCTION__, 0x4fba, 0)(
        "[ON_TIME_EVENT] Reward Start(remain time : %u)",
        pPkt->m_e);

    COnTimeEvent* event =
        reinterpret_cast<COnTimeEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x33));
    // [b28d-solo-InterDispatcher] ORIG 0x084e0e2c..0x084e0e4e：两独立守卫同落 else
    // （`cmpl $0x0,-0xc; je` + `call *%edx; test %al,%al; je`）⇒ 短路 && 直判，无 bool 局部。
    // ORIG vtable+0x34 槽 = IsEventing(0)
    if (event != 0 && event->IsEventing(static_cast<CUser*>(0)))
    {
        event->process_set_reward_start(             // ORIG 0x081bbce0（见 NEEDS_DECL）
            pPkt->m_a,     // msg+0x0a ontimeId
            pPkt->m_12,    // msg+0x12 itemNo
            pPkt->m_16,    // msg+0x16 itemCount
            pPkt->m_e);    // msg+0x0e endSpan
        PacketGuard packet;
        packet.put_header(0, 0x105);
        packet.put_int(pPkt->m_e);
        packet.finalize(true);
        G_GameWorld()->send_all_with_state(packet, 3);
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x4fc5, 5)(
            "[ON_TIME_EVENT] isEventing = not, event_no(%u)",
            pPkt->m_a);
    }
    return 0;
}

Inter_OnTimeEventRewardEnd::~Inter_OnTimeEventRewardEnd() {}
// ORIG 0x084e1062 T dispatch_sig —— 准时事件奖励结束：置奖励态并全员广播结束包。
// 报告 docs/class_func_reports/Inter_OnTimeEventRewardEnd/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=96（ORIG 0x084e1062）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=53（ORIG 0x84e1062）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→3（102 vs 99，TU 121/1331/5/133 不回退）：真修两处——
//   ① 补 ORIG 0x084e106a 死写入 `char* p; p=data;`（P171/P198 两句式，ORIG `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)` 后从未读）；
//   ② 两个 cMyTrace 改全表达式临时链（P221/P227，消 else 分支具名对象槽）。残余=N7 虚调寄存器+块序（N104）。
int Inter_OnTimeEventRewardEnd::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG 0x084e106a 死写入照抄（P171/P198 两句式）：`mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`，之后从未读取。
    char* p;
    p = data;
    (void)p;

    cMyTrace(__PRETTY_FUNCTION__, 0x500c, 0)("[ON_TIME_EVENT] Reward End");

    COnTimeEvent* event = reinterpret_cast<COnTimeEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x33));

    // [b28d-solo-InterDispatcher] ORIG 0x084e10be..0x084e10e0：两独立守卫同落 else
    // （`cmpl $0x0,-0xc; je` + `call *%edx; test %al,%al; je`）⇒ 短路 && 直判，无 bool 局部。
    if (event != 0 && event->IsEventing(0))            // vtable+0x34 slot 13
    {
        event->set_reward_state(1);
        if (event->get_ontime_reward_user_count() != 0)
        {
            Timer_Send_Ontime_Reward::RegistNextTimer();
        }
        PacketGuard packet;
        packet.put_header(0, 0x106);
        packet.finalize(true);
        G_GameWorld()->send_all_with_state(packet, 3);
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x501b, 5)(
            "[ON_TIME_EVENT] Inter_OnTimeEventRewardEnd() error");
    }
    return 0;
}

Inter_LoadPvPBuddy::~Inter_LoadPvPBuddy() {}
// ORIG 0x084e16f4 T dispatch_sig —— 纯 ret 空函数。
// 报告 docs/class_func_reports/Inter_LoadPvPBuddy/dispatch_sig.md 全文转写（汇编 9 字节，mov $0x0,%eax; ret）。
int Inter_LoadPvPBuddy::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    return 0;
}

Inter_PvPBuddyConnList::~Inter_PvPBuddyConnList() {}
// ORIG 0x084e16fe T dispatch_sig —— PvP 伙伴连接列表下发。
// 报告 docs/class_func_reports/Inter_PvPBuddyConnList/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PvPBuddyConnListPktView —— Inter_PvPBuddyConnList::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_PvPBuddyConnListPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=45（ORIG 0x084e16fe）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=NEAR ndiff=6（ORIG 0x84e16fe）：cmp 方向经 int 同型化
// 已翻正（-1）；余 entry 地址形 N4/P30（ORIG 三段 add 0x10/base/0x6 vs 4.4 add 0x16+lea）。
int Inter_PvPBuddyConnList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [b28c-30] N22 封顶：ORIG 0x084e1709 `cmp %eax,%ebx`（ebx = 局部字段值）vs
    // 4.4 `cmp %ebx,%eax`（call 结果在 %eax）；cmp 两向不可翻（dst=局部 vs
    // dst=call 结果）。
    Inter_PvPBuddyConnListPktView* pPkt = reinterpret_cast<Inter_PvPBuddyConnListPktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    // ORIG 一步 `mov 0xa(%eax),%ebx` 直落 callee-saved（具名槽会顶深 pPkt，技巧2）。
    register int charIdMsg =                           // recv charId @ msg+0xa
        pPkt->m_a;
    if (charIdMsg != user->getCurCharacNo())
        return 0;

    // ORIG：m_12 每处独立 reload（无 count 槽；put_int + 循环界各一次）。
    PacketGuard packet;
    packet.put_header(0, 0x10f);
    packet.put_int((int)pPkt->m_12);
    for (int i = 0; i < (int)pPkt->m_12; ++i)
        // 求值序 = ORIG：i 项先算（eax），再 add $0x10 / add pPkt 槽 / add $0x6
        // （pPkt 置于两常量之间阻折叠，4.4 恒发三段 add；旧形 pPkt 先置产 ecx 提前载+lea）。
        // 形探针实测：指针形/收窄括号形 ndiff 反劣（8→12/>12）⇒ 保留原「pPkt 置两常量之间」形
        //（ndiff 8，残 = 4.4 lea 合成 vs ORIG 三段 add，P30/N4 封顶）。
        packet.put_binary(reinterpret_cast<char*>(i * 0x29 + 0x10 +
                                                  reinterpret_cast<int>(pPkt) + 6),
                          0x29);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_ReqAddPvPBuddy::~Inter_ReqAddPvPBuddy() {}
// ORIG 0x084e1832 T dispatch_sig —— PvP 伙伴添加请求应答。
// 报告 docs/class_func_reports/Inter_ReqAddPvPBuddy/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReqAddPvPBuddyPktView —— Inter_ReqAddPvPBuddy::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReqAddPvPBuddyPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    char m_12;                // +0x12
    char m_13[1];             // +0x13 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28 判词 N22] 唯一残差 `cmp %eax,%ebx`↔`cmp %ebx,%eax`（dst 方向），负例在册。
// [b28d-lane2] verdict=NEAR ndiff=1（ORIG 0x084e1832）：寄存器中转(N18a/N7)；gap0。
// [b28d-solo-InterDispatcher] verdict=AE（收口：字段/局部 int 同型化翻转 cmp 方向 → 归零）。
int Inter_ReqAddPvPBuddy::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG +0x10：包基址槽在首条 call 之前即物化（-0x10）。
    Inter_ReqAddPvPBuddyPktView* pPkt = reinterpret_cast<Inter_ReqAddPvPBuddyPktView*>(data);
    if (user->get_state() <= 2 || user->get_state() == 7)
        return 0;
    // ORIG +0x27 一步 `mov 0xa(%eax),%ebx` ⇒ register 局部。
    register int charIdMsg =                           // recv charId @ msg+0xa
        pPkt->m_a;
    if (charIdMsg != user->getCurCharacNo())   // ORIG `cmp %eax,%ebx` 同上
        return 0;

    PacketGuard packet;
    packet.put_header(0, 0x10c);
    packet.put_byte((int)pPkt->m_12);   // movsbl
    // ORIG：strlen 单次调用，结果落 -0xc 复用。
    const int nameLen = strlen(pPkt->m_13);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_13, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_AddPvPBuddyResult::~Inter_AddPvPBuddyResult() {}
// ORIG 0x084e1978 T dispatch_sig —— PvP 好友添加结果通知（0x121/0x122 两包 + 错误码分支）。
// 报告 docs/class_func_reports/Inter_AddPvPBuddyResult/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AddPvPBuddyResultPktView —— Inter_AddPvPBuddyResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_AddPvPBuddyResultPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    char m_12;                // +0x12
    signed char m_13;         // +0x13
    char m_14[1];             // +0x14 串/字节区首元素（容量未建模）
    char m_pad15[0x1d];  // 未消费
    char m_32;                // +0x32
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=130（ORIG 0x084e1978）：寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=41（ORIG 0x84e1978）：P231 register int 局部
// 未能翻单段（4.4 仍两段）；余 m_32 分支 je/jne 极性 + N4 add-vs-lea。
int Inter_AddPvPBuddyResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AddPvPBuddyResultPktView* pPkt = reinterpret_cast<Inter_AddPvPBuddyResultPktView*>(data);
    if (user->get_state() <= 2)
        return 0;
    register int characNoMsg = pPkt->m_a;
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    if (pPkt->m_12 == '\0')
    {   // 拒绝分支（0x121）
        if (pPkt->m_32 == '\0')
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(1, 0x121);
            packet.put_byte(1);
            packet.put_byte(pPkt->m_13);
            const int local_14 = (int)strlen(pPkt->m_14);
            packet.put_int(local_14);
            packet.put_str(pPkt->m_14, local_14);
            packet.finalize(true);
            user->Send(packet);
        }
        else
        {
            user->SendCmdErrorPacket(
                (ENUM_CMDPACKET)0x121,
                pPkt->m_32);
        }
    }
    else if (pPkt->m_32 == '\0')
    {   // 同意分支（0x122）
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x122);
        packet.put_byte(1);
        packet.put_byte(pPkt->m_13);
        const size_t local_10 = strlen(pPkt->m_14);
        packet.put_int(local_10);
        packet.put_str(pPkt->m_14, local_10);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x122,
            pPkt->m_32);
    }
    return 0;
}

Inter_RemovePvPBuddyResult::~Inter_RemovePvPBuddyResult() {}
// ORIG 0x084e1c1e T dispatch_sig —— PVP 好友删除结果：成功转发名字或错误码。
// 报告 docs/class_func_reports/Inter_RemovePvPBuddyResult/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_RemovePvPBuddyResultPktView —— Inter_RemovePvPBuddyResult::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_RemovePvPBuddyResultPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    signed char m_12;         // +0x12
    char m_13[1];             // +0x13 串/字节区首元素（容量未建模）
    char m_pad14[0x1d];  // 未消费
    signed char m_31;         // +0x31
};
#pragma pack(pop)

// [b28d-solo-InterDispatcher 收口] verdict=AE（P231 register int 局部直落 %ebx → 归零）。
int Inter_RemovePvPBuddyResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_RemovePvPBuddyResultPktView* pPkt = reinterpret_cast<Inter_RemovePvPBuddyResultPktView*>(data);
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    register int characNoMsg = pPkt->m_a;
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    if (pPkt->m_31 != '\0')
    {   // 结果码非零 → 错误包
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x123,
            (unsigned int)pPkt->m_31);
    }
    // ORIG：PacketGuard 只在 else 支，两支汇合后统一 `mov $0x0,%eax`。
    else
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x123);
        packet.put_byte(1);
        packet.put_byte((int)pPkt->m_12);
        const int nameLen = strlen(pPkt->m_13);
        packet.put_int(nameLen);
        packet.put_str(pPkt->m_13, nameLen);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_NoticePvPBuddyInOut::~Inter_NoticePvPBuddyInOut() {}
// ORIG 0x084e1d96 T dispatch_sig —— PVP 好友进出通知：登录/登出两种包头转发名字。
// 报告 docs/class_func_reports/Inter_NoticePvPBuddyInOut/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NoticePvPBuddyInOutPktView —— Inter_NoticePvPBuddyInOut::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_NoticePvPBuddyInOutPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    unsigned char m_12;       // +0x12
    unsigned char m_13;       // +0x13
    signed char m_14;         // +0x14
    char m_15[1];             // +0x15 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-solo-InterDispatcher 收口] verdict=AE（P231 register int 局部直落 %ebx → 归零）。
int Inter_NoticePvPBuddyInOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticePvPBuddyInOutPktView* pPkt = reinterpret_cast<Inter_NoticePvPBuddyInOutPktView*>(data);
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    register int characNoMsg = pPkt->m_a;
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    PacketGuard packet;
    if (pPkt->m_12 == 0)
    {   // 登出（0x10d）附结果码
        packet.put_header(0, 0x10d);
        packet.put_byte((unsigned int)pPkt->m_13);
    }
    else
    {   // 登入（0x10e）
        packet.put_header(0, 0x10e);
    }
    packet.put_byte((int)pPkt->m_14);
    const int nameLen = strlen(pPkt->m_15);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_15, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_LoadPvPLiveEventData::~Inter_LoadPvPLiveEventData() {}
// ORIG 0x084e25cc T dispatch_sig —— PvP 直播活动数据 DB 加载（扩展数据槽 3）。
// 报告 docs/class_func_reports/Inter_LoadPvPLiveEventData/dispatch_sig.md 全文转写。
// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（37=37）。
int Inter_LoadPvPLiveEventData::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG vfn→%ecx vs 4.4 恒 %eax（N7 探针
    // 实证）；ndiff 12 封顶。
    if (user->get_state() <= 2)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    reinterpret_cast<charac_expand::CData*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)3))->loadData(user, data);
    return 0;
}

Inter_Antibot_SendBufferClient::~Inter_Antibot_SendBufferClient() {}
// ORIG 0x084e43bc T dispatch_sig —— ANTIBOT/DPROTO 缓冲透传客户端：getUser 状态门、
// 异常计数日志、0x230/0x231 包发送与历史记录。
// 报告 docs/class_func_reports/Inter_Antibot_SendBufferClient/dispatch_sig.md 全文转写。
// msg 布局：+0 kind(int,0=ANTIBOT/1=DPROTO) +4 qqID(uint) +8 size(u16) +0xc payload。
#pragma pack(push, 1)
// Inter_Antibot_SendBufferClientPktView —— Inter_Antibot_SendBufferClient::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Antibot_SendBufferClientPktView
{
    int m_0;                  // +0x00
    unsigned int m_4;         // +0x04
    unsigned short m_8;       // +0x08
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=214（ORIG 0x084e43bc）：槽序/帧差(N82/P212/P221)+混合残差；gap-12。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84e43bc）：本轮删死 NumberToString 调用（p167 CLEAR）；余 P226/N18a/P30。
int Inter_Antibot_SendBufferClient::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Antibot_SendBufferClientPktView* pPkt = reinterpret_cast<Inter_Antibot_SendBufferClientPktView*>(data);
    char buffer[95000];                                // ORIG 栈缓冲 0x17338
    memset(buffer, 0, 95000);
    memcpy(buffer, data + 0xc, (int)(short)pPkt->m_8);

    CUser* target = getUser(pPkt->m_4);
    if (target == 0 || target->get_state() == 0)
    {   // 用户缺失/未登录：记录忽略日志并累计异常计数
        // ORIG 0x84e445a 单次 NumberToString（结果 %ecx 保活 → logFormat 末实参），
        // 无独立死调用（p167 EXTRA NumberToString×1 实证）。
        const char* reason;
        if (target == 0)
            reason = "NULL User";
        else
            reason = "NONE state";
        const char* kind;
        if (pPkt->m_0 == 0)
            kind = "ANTIBOT";
        else
            kind = "DPROTO";
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x5e56,
            "Ignore SendBuffertoClient [%s][size:%d]; reason:%s, QQID:%s",
            kind, (int)(short)pPkt->m_8,
            reason, NumberToString(pPkt->m_4, 0));
        if (target != 0 && (unsigned char)target->m_field4 < 5)
        {
            const unsigned char old = (unsigned char)target->m_field4;
            target->m_field4 = (char)(old + 1);
            target->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)3, 0x252,
                                              (int)old, 0);
        }
        return 0;
    }

    int packetType;                                    // 0x230=ANTIBOT / 0x231=DPROTO
    if (pPkt->m_0 == 0)
        packetType = 0x230;
    else
        packetType = 0x231;

    if (packetType == 0x230)
    {
        if (target->m_field8cf00 == 2)
        {
            target->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)2);
            target->m_field8cf00 = 3;
        }
        else if (target->m_field8cf00 != 4)
        {
            target->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b,
                (int)(short)pPkt->m_8,
                target->m_field8cf00);
        }
    }

    PacketGuard packet;
    packet.put_header(0, packetType);
    packet.put_binary(
        buffer, (int)(short)pPkt->m_8);
    packet.finalize(false);
    char sendRet = (char)target->Send(packet);

    if (packetType == 0x230)
    {
        if (target->m_field8cf00 == 3)
        {
            target->m_historyLog.AntibotPacketLog((AntibotPacketLogType::T)3);
            target->m_field8cf00 = 4;
        }
        else if (target->m_field8cf00 != 4)
        {
            target->m_historyLog.AbnormalUser(
                (ENUM_ABNORMAL_USER_LOG_TYPE)4, 0x25b,
                (int)(short)pPkt->m_8,
                target->m_field8cf00);
        }
    }
    else if (packetType == 0x231 &&
             (unsigned char)target->m_field4 < 5)
    {
        const unsigned char old = (unsigned char)target->m_field4;
        target->m_field4 = (char)(old + 1);
        target->m_historyLog.SendBuffertoClientLog(
            pPkt->m_0,
            (short)pPkt->m_8,
            old, sendRet != 0);
    }
    return 0;
}

Inter_Antibot_PunishClient::~Inter_Antibot_PunishClient() {}
// ORIG 0x084e4774 T dispatch_sig —— antibot 惩罚客户端指令。
// 报告 docs/class_func_reports/Inter_Antibot_PunishClient/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Antibot_PunishClientPktView —— Inter_Antibot_PunishClient::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Antibot_PunishClientPktView
{
    unsigned int m_0;         // +0x00
    unsigned char m_4;        // +0x04
    char m_pad5[0x7];  // 未消费
    int m_c;                  // +0x0c
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=85（ORIG 0x084e4774）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+9。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=87（ORIG 0x84e4774）：P226 具名 bool/char 落槽+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_Antibot_PunishClient::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Antibot_PunishClientPktView* pPkt = reinterpret_cast<Inter_Antibot_PunishClientPktView*>(data);
    // §P0：ORIG 084e4782/084e479c 两次独立重读 msg+0x0（uid 无槽），
    //   084e4792/084e47f8 两次独立重读 msg+0x4（punishMode 留 %esi / 重载）⇒ 不设具名缓存。
    CUser* target = getUser(pPkt->m_0);

    // ORIG 0x84e4792..0x84e47b1：punish_mode→%esi、NumberToString→%ebx（均无槽往返）
    // ⇒ 全表达式临时链，无 uidStr 具名局部（P0/P328⑩/P227）。
    cMyTrace(__PRETTY_FUNCTION__, 0x5eb3, 0)(
        "requested uid(%s), punish_mode(%d), user(%x)",
        NumberToString(pPkt->m_0, 0), (int)pPkt->m_4, target);

    // ORIG 0x84e4802..0x84e480f 决策链 cmp $2/cmp $3/cmp $1 ⇒ 原码 switch（P17/P251）。
    switch (pPkt->m_4)
    {
    case 1:
        break;
    case 2:
        if (target != 0 && target->get_state() != 0)
            target->DisConnSig((DISCONN_SIG)0x23, true, 0);
        break;
    case 3:
        if (target != 0 && target->get_state() != 0 &&
            0 < pPkt->m_c)
        {
            target->DisConnSig((DISCONN_SIG)0x23, true, 0);
        }
        break;
    }
    return 0;
}

Inter_Antibot_ProcessProtectMsg::~Inter_Antibot_ProcessProtectMsg() {}
// ORIG 0x084e48c2 T dispatch_sig —— 反外挂保护消息：按事件类型回 SAFEMODE 通知包。
// 报告 docs/class_func_reports/Inter_Antibot_ProcessProtectMsg/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Antibot_ProcessProtectMsgPktView —— Inter_Antibot_ProcessProtectMsg::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Antibot_ProcessProtectMsgPktView
{
    short m_0;                // +0x00
    unsigned int m_2;         // +0x02
    short m_6;                // +0x06
    unsigned char m_8;        // +0x08
    char m_pad9[0x1];  // 未消费
    short m_a;                // +0x0a
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=152（ORIG 0x084e48c2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=162（ORIG 0x84e48c2）：P226 具名 bool/char 落槽+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_Antibot_ProcessProtectMsg::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Antibot_ProcessProtectMsgPktView* pPkt = reinterpret_cast<Inter_Antibot_ProcessProtectMsgPktView*>(data);
    CUser* target =
        getUser(pPkt->m_2);   // msg+0x2 uid
    if (target == 0 || target->get_state() == 0)
        return 0;

    short eventType = pPkt->m_0;           // msg+0x0
    // ORIG 0x84e4911/0x84e4915 `movb $0x0,-0x1b; movw $0x0,-0x1a` 前置清零 ⇒ 声明=0
    // + 分支内独立赋值（P197③）。
    char protstatLevel = 0;
    short extra = 0;
    if (eventType == 0x13)
    {
        protstatLevel = 0;
        extra = pPkt->m_6;           // short@0x6
    }
    else if (eventType == 0x14)
    {
        protstatLevel = (char)pPkt->m_8;
        extra = pPkt->m_a;
    }
    else if (eventType == 0x12)
    {
        protstatLevel = (char)pPkt->m_8;
        extra = pPkt->m_a;
    }
    else
    {
        protstatLevel = 0;
        extra = 0;
    }

    // P328⑩/P227：ORIG 0x84e497e..0x84e49aa 先求 extra→%edi、protstatLevel→%esi、uid→%ebx
    // 再 ctor ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x5f3b, 0)(
        "[SAFEMODE]uid : %d    protstatLevel : %d    eventType : %d\n",
        (int)pPkt->m_2,
        (int)protstatLevel, (int)(unsigned short)extra);

    if (protstatLevel == 1 || protstatLevel == 8)
    {   // 开启保护通知
        PacketGuard packet;
        packet.put_header(0, 0x1ee);
        packet.put_byte(1);
        packet.finalize(true);
        target->Send(packet);
    }
    if (protstatLevel == 0)
    {   // 关闭保护通知
        PacketGuard packet;
        packet.put_header(0, 0x1ee);
        packet.put_byte(0);
        packet.finalize(true);
        target->Send(packet);
    }
    return 0;
}

Inter_LoadOnlinePreliminaryMember::~Inter_LoadOnlinePreliminaryMember() {}
// ORIG 0x084e263e T dispatch_sig —— 线上预选成员数据 DB 加载（扩展数据槽 4）。
// 报告 docs/class_func_reports/Inter_LoadOnlinePreliminaryMember/dispatch_sig.md 全文转写。
int Inter_LoadOnlinePreliminaryMember::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG vfn→%ecx vs 4.4 恒 %eax（N7 探针
    // 实证）；ndiff 12 封顶。
    if (user->get_state() <= 2)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    reinterpret_cast<charac_expand::CData*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))->loadData(user, data);
    return 0;
}

Inter_LoadOnlinePreliminaryTeam::~Inter_LoadOnlinePreliminaryTeam() {}
// ORIG 0x084e26b0 T dispatch_sig —— 线上预选队伍成员进入。
// 报告 docs/class_func_reports/Inter_LoadOnlinePreliminaryTeam/dispatch_sig.md 全文转写。
int Inter_LoadOnlinePreliminaryTeam::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    if (!GlobalData::s_onlinePreliminaryTeamMgr->OnEnterUser(user, data))
        return 0x5219;
    return 0;
}

Inter_LoadOnlinePreliminaryMemberMatchList::~Inter_LoadOnlinePreliminaryMemberMatchList() {}
// ORIG 0x084e271e T dispatch_sig —— 线上预选对战表数据 DB 加载（扩展数据槽 5）。
// 报告 docs/class_func_reports/Inter_LoadOnlinePreliminaryMemberMatchList/dispatch_sig.md 全文转写。
int Inter_LoadOnlinePreliminaryMemberMatchList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG vfn→%ecx vs 4.4 恒 %eax（N7 探针
    // 实证）；ndiff 12 封顶。
    if (user->get_state() <= 2)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    reinterpret_cast<charac_expand::CData*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)5))->loadData(user, data);
    return 0;
}

Inter_LoadOnlinePreliminaryTeamMatchList::~Inter_LoadOnlinePreliminaryTeamMatchList() {}
int Inter_LoadOnlinePreliminaryTeamMatchList::dispatch_sig(
    CUser* user, char* data, int /*len*/)
{
    GlobalData::s_onlinePreliminaryTeamMgr->OnLoadTeamInfo(user, data);
    return 0;
}

Inter_LoadCharacLinkBonus::~Inter_LoadCharacLinkBonus() {}
// ORIG 0x084e1f02 T dispatch_sig —— 角色联动奖励领取：经验直加、金币走系统邮件，
// 更新联动状态并广播 0x123 通知包。
// 报告 docs/class_func_reports/Inter_LoadCharacLinkBonus/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadCharacLinkBonusPktView —— Inter_LoadCharacLinkBonus::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadCharacLinkBonusPktView
{
    unsigned int m_0;         // +0x00
    int m_4;                  // +0x04
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=299（ORIG 0x084e1f02）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=113（ORIG 0x84e1f02）：P30/N4 add-vs-lea。
int Inter_LoadCharacLinkBonus::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x84e1f0e/0x84e1f15 两 int 前置清零（`movl $0x0,-0x28/-0x24`）后再由
    // msg 字段赋值（084e2068 起）⇒ 声明=0 + 独立赋值语句（P197③），pPkt 槽在 Inven_Item
    // 构造之后才物化（084e205f）。
    unsigned int bonusExp = 0;                         // -0x28（msg+0x0）
    int bonusGold = 0;                                 // -0x24（msg+0x4）

    char title[0x15] = { '\0' };                       // -0x3d（gcc 内联清零展开）
    char body[0x100] = { '\0' };                       // -0x1a5

    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_149", 0),
            0x14);
    strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_150", 0),
            0xff);

    Inven_Item item;                                   // -0xa5（默认构造）

    Inter_LoadCharacLinkBonusPktView* pPkt = reinterpret_cast<Inter_LoadCharacLinkBonusPktView*>(data);
    bonusExp = pPkt->m_0;
    bonusGold = pPkt->m_4;

    if (bonusExp != 0 || bonusGold != 0)
    {
        if (bonusGold != 0)
        {
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, item, (unsigned int)bonusGold,
                (unsigned int)user->getCurCharacNo(), body,
                (int)strlen(body), 0,
                (ENUM_SERVER_GROUP)user->GetServerGroup(), false, false);

            char name[0x1e] = { '\0' };                // -0x5b
            const int slotIdx = (int)user->getLinkCharacSlotIndex();   // -0x1c（movsbl）
            if (slotIdx >= 0 &&
                (int)user->m_characList.size() > slotIdx)
            {
                strcpy(name, user->m_characList[slotIdx].m_name);  // 元素 +0x04
            }

            PacketGuard packet;                        // -0x68
            packet.clear();
            packet.put_header(0, 0x123);
            packet.put_int((int)strlen(name));
            packet.put_str(name, (int)strlen(name));
            packet.put_int(bonusGold);
            packet.put_int(0);
            packet.put_short(0);
            packet.put_short(0);
            packet.finalize(true);
            user->Send(packet);
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x11, user,
                (unsigned int)bonusGold);
        }

        if (bonusExp != 0 && user->get_charac_level() != 0x46)
        {
            user->giveCharacLinkBonusExp(bonusExp);
        }

        user->UpdateCharacLinkBonus(0, 0,
                                    (unsigned int)user->getCurCharacNo(),
                                    (ENUM_CHARAC_LINK_BONUS_STATE)1);
        user->setCharacLinkBonusSetting(
            user->get_charac_idx((unsigned int)user->getCurCharacNo()), false);
    }
    return 0;
}

Inter_CreateAccountCargo::~Inter_CreateAccountCargo() {}
// ORIG 0x084e23a2 T dispatch_sig —— 账号仓库创建应答。
// 报告 docs/class_func_reports/Inter_CreateAccountCargo/dispatch_sig.md 全文转写。
int Inter_CreateAccountCargo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
#pragma pack(push, 1)
    struct CreateAccountCargoView
    {
        unsigned char m_errCode;    // +0x0
        char m_pad[3];
        unsigned int m_money;       // +0x4
    };
#pragma pack(pop)
    CreateAccountCargoView* p = reinterpret_cast<CreateAccountCargoView*>(data);
    if (user->get_state() <= 2)
        return 0;
    if (p->m_errCode != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x132, p->m_errCode);
        return 0;
    }
    user->SetAccountCargoDBData((Inven_Item*)0, 0, p->m_money);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x132);
    return 0;
}

Inter_UpgradeAccountCargo::~Inter_UpgradeAccountCargo() {}
// ORIG 0x084e243a T dispatch_sig —— 账号仓库扩容应答。
// 报告 docs/class_func_reports/Inter_UpgradeAccountCargo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_UpgradeAccountCargoPktView —— Inter_UpgradeAccountCargo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_UpgradeAccountCargoPktView
{
    unsigned char m_0;              // +0x00
    char m_pad1[0x3];  // +0x1..未消费
    unsigned int m_4;               // +0x04
};
#pragma pack(pop)

int Inter_UpgradeAccountCargo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_UpgradeAccountCargoPktView* pPkt = reinterpret_cast<Inter_UpgradeAccountCargoPktView*>(data);
    if (user->get_state() <= 2)
        return 0;
    if (pPkt->m_0 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x133,
                                 pPkt->m_0);
        return 0;
    }
    if ((unsigned char)user->IsExistAccountCargo() != 1)
        return 0;
    // ORIG：GetAccountCargo 结果物化槽 -0xc(%ebp)（pPkt 因此被挤到 -0x10），
    // 之后 pPkt->m_4 装载 → cargo 槽回读 → SetCapacity。链式调用形反产
    // 4.4 %ebx 缓存 + push ebx 帧。
    CAccountCargo* pCargo = user->GetAccountCargo();
    pCargo->SetCapacity(pPkt->m_4);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x133);
    return 0;
}

Inter_LoadAccountCargo::~Inter_LoadAccountCargo() {}
// ORIG 0x084e24ec T dispatch_sig —— 账号仓库 DB 加载。
// 报告 docs/class_func_reports/Inter_LoadAccountCargo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadAccountCargoPktView —— Inter_LoadAccountCargo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadAccountCargoPktView
{
    unsigned int m_0;               // +0x00
    unsigned int m_4;               // +0x04
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=61（ORIG 0x084e24ec）：寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（ORIG 0x084e24ec）：P227 全表达式临时链
// （cMyTrace(...)(...)，vararg 跨 ctor 保活）+ pPkt 槽取址 ⇒ 本波归零。
int Inter_LoadAccountCargo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadAccountCargoPktView* pPkt = reinterpret_cast<Inter_LoadAccountCargoPktView*>(data);
    if (user->get_state() <= 1)
        return 0;
    if (user->IsExistAccountCargo())
    {
        // [b28d-solo-InterDispatcher] P227 全表达式临时链：ORIG 0x084e24ec 段把两个
        // vararg 在 cMyTrace ctor 前求值并以 %esi/%ebx 跨 ctor 保活（`mov %eax,%esi;
        // mov %eax,%ebx; …ctor…; mov %esi,0xc(%esp); mov %ebx,0x8(%esp)`）。
        cMyTrace(__PRETTY_FUNCTION__, 0x51e9, 5)(
            "AccountCargo duplicated!(%s(%s))",
            user->get_acc_name(),
            NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    user->SetAccountCargoDBData(
        reinterpret_cast<Inven_Item*>(reinterpret_cast<char*>(pPkt) + 0x8),  // ORIG 从 pPkt 槽 -0xc 取址
        pPkt->m_4,
        pPkt->m_0);
    return 0;
}

Inter_MultiMailBoxReqSend::~Inter_MultiMailBoxReqSend() {}
// ORIG 0x084e27b8 T dispatch_sig —— 多邮件批量发送：逐收件人构造 MSG_MAILBOX_SEND 请求 DB 发信。
// 报告 docs/class_func_reports/Inter_MultiMailBoxReqSend/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MultiMailBoxReqSendItemView —— 包内条目区（+0x60 起，stride 0xb）视图；
//   ORIG 084e2896/084e28b8/084e28d8 = `add $0x60` 后 disp 0xa/0xc/0x10
//   （条目基址为动态表达式 ⇒ ORIG 保留 add 0x60 与成员 disp 两段）。
struct Inter_MultiMailBoxReqSendItemView
{
    char m_pad0[0xa];
    unsigned short m_slot;          // +0x0a
    unsigned int m_itemId;          // +0x0c
    unsigned int m_extra;           // +0x10
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_MultiMailBoxReqSendPktView —— Inter_MultiMailBoxReqSend::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MultiMailBoxReqSendPktView
{
    char m_pad0[0x1d];  // 未消费
    unsigned int m_1d;        // +0x1d
    char m_pad21[0x8];  // 未消费
    unsigned int m_29;        // +0x29
    char m_pad2d[0x15];  // 未消费
    unsigned int m_42;        // +0x42
    char m_46[1];             // +0x46 串/字节区首元素（容量未建模）
    char m_pad47[0x1d];  // 未消费
    unsigned int m_64;        // +0x64
    unsigned char m_68;       // +0x68
    char m_pad69[0x3f54];  // 未消费
    int m_3fbd;               // +0x3fbd
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=138（ORIG 0x084e27b8）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=135（ORIG 0x84e27b8）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
// [Lane30-InterDispatcher] 权威头 MSG_MAILBOX_SEND 具名成员直访（原 TU 局部 MailBufSendView
//   cast 反模式）：ORIG 084e27b8 的 mail 缓冲槽 -0x165 的写点即 -0x165+0x0d/0x11/0x2e/0x36/
//   0x37/0x39/0x3d = m_nameLenD/m_name11/m_gold2e/m_space36/m_slot37/m_itemId39/m_extra3d
//   （无中间视图指针槽：原 pMail 局部使帧 +0x10 并多出 lea/mov 与 lea 折址）。
int Inter_MultiMailBoxReqSend::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;

    Inter_MultiMailBoxReqSendPktView* pPkt =
        reinterpret_cast<Inter_MultiMailBoxReqSendPktView*>(data);
    unsigned int local_14 = 0;
    for (int local_10 = 0;
         local_10 < (int)(unsigned int)pPkt->m_68;
         ++local_10)
    {
        MSG_MAILBOX_SEND mailBuf;
        memset(&mailBuf, 0, 0x145);
        mailBuf.m_nameLenD = pPkt->m_42;
        strncpy(mailBuf.m_name11, reinterpret_cast<char*>(pPkt) + 0x46,
                mailBuf.m_nameLenD);
        mailBuf.m_gold2e = pPkt->m_64;
        pPkt->m_64 = 0;
        mailBuf.m_space36 = *reinterpret_cast<unsigned char*>(
            reinterpret_cast<char*>(pPkt) + local_10 * 0xb + 0x69);
        mailBuf.m_slot37 = reinterpret_cast<Inter_MultiMailBoxReqSendItemView*>(
            reinterpret_cast<char*>(pPkt) + local_10 * 0xb + 0x60)->m_slot;
        mailBuf.m_itemId39 = reinterpret_cast<Inter_MultiMailBoxReqSendItemView*>(
            reinterpret_cast<char*>(pPkt) + local_10 * 0xb + 0x60)->m_itemId;
        mailBuf.m_extra3d = reinterpret_cast<Inter_MultiMailBoxReqSendItemView*>(
            reinterpret_cast<char*>(pPkt) + local_10 * 0xb + 0x60)->m_extra;

        local_14 =
            WongWork::CMailBoxHelper::ReqDBSendNewMail(
                user,
                &mailBuf,
                pPkt->m_1d,
                false,
                pPkt->m_29,
                pPkt->m_3fbd);
        if (local_14 != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x13c,
                                     (unsigned char)(local_14 & 0xff));
            return 0;
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0x13c);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_ReqQueryCharacInfoMailbox::~Inter_ReqQueryCharacInfoMailbox() {}
// ORIG 0x084e2a06 T dispatch_sig —— 邮箱查询角色信息：转发角色名与等级/职业等摘要。
// 报告 docs/class_func_reports/Inter_ReqQueryCharacInfoMailbox/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReqQueryCharacInfoMailboxPktView —— Inter_ReqQueryCharacInfoMailbox::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_ReqQueryCharacInfoMailboxPktView
{
    char m_pad0[0x20];  // 未消费
    int m_20;                 // +0x20
    char m_pad24[0x4];  // 未消费
    signed char m_28;         // +0x28
    char m_pad29[0x1];  // 未消费
    short m_2a;               // +0x2a
    signed char m_2c;         // +0x2c
    signed char m_2d;         // +0x2d
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=107（ORIG 0x084e2a06）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=57（ORIG 0x84e2a06）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_ReqQueryCharacInfoMailbox::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    char* recv = data;                                 // -0xc(%ebp) 缓存消息指针

    if (reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_20 != 0)
    {   // 结果码非零 → 错误包
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x145,
            (unsigned char)reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_20);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x145);
    packet.put_byte(1);
    packet.put_int(strlen(recv));
    packet.put_str(recv, strlen(recv));                // ORIG 两次独立 strlen（P48）
    packet.put_short((int)reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_2a);  // movzwl+cwtl
    packet.put_byte((int)reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_28);
    packet.put_byte((int)reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_2c);
    packet.put_byte((int)reinterpret_cast<Inter_ReqQueryCharacInfoMailboxPktView*>(recv)->m_2d);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_ReqGameServerInfo::~Inter_ReqGameServerInfo() {}
// ORIG 0x084e2b94 T dispatch_sig —— 应答服务器信息查询（monitor/guild 代理 SendGameServerInfo）。
// 报告 docs/class_func_reports/Inter_ReqGameServerInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ReqGameServerInfoPktView —— Inter_ReqGameServerInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ReqGameServerInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    char m_a;                       // +0x0a
    unsigned char m_b;              // +0x0b
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=133（ORIG 0x084e2b94）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+6。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=194（ORIG 0x84e2b94）：P226 具名 bool/char 落槽+N18a 单段 regmove+P264 .o 重定位/立即数。
int Inter_ReqGameServerInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ReqGameServerInfoPktView* pPkt = reinterpret_cast<Inter_ReqGameServerInfoPktView*>(data);
    if (G_GameWorld()->GetChannelType() == 7)
        return 0;

    // §P0：ORIG 对 m_a/m_b 全程重读（084e2bd6/084e2ce9 两次 m_a；084e2c32 内联 m_b），无槽往返；
    //   proxy 亦无具名局部（GetServerProxy 结果直接在 %eax 作 this）⇒ 链式调用。
    // ORIG 084e2bcb/084e2bcd 为正向 `if (IsIntegratedPvPBaseChannel())` 双分支（真分支在前）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        if (pPkt->m_a == '\0')
        {
            GlobalData::s_monitor_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)pPkt->m_b)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
        }
        else if (pPkt->m_a == '\x01')
        {
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)pPkt->m_b)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
        }
    }
    else
    {
        if (pPkt->m_a == '\0')
        {
            GlobalData::s_monitor_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
        }
        else if (pPkt->m_a == '\x01')
        {
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGameServerInfo(
                    G_CEnvironment()->get_server_ip(),
                    (unsigned short)G_CEnvironment()->get_udp_port(),
                    (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
                    (unsigned char)G_CEnvironment()->get_channel_no());
        }
    }
    return 0;
}

Inter_MonitorRegistGameServerAck::~Inter_MonitorRegistGameServerAck() {}
// ORIG 0x084e2dee T dispatch_sig —— 监控服注册应答处理。
// 报告 docs/class_func_reports/Inter_MonitorRegistGameServerAck/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorRegistGameServerAckPktView —— Inter_MonitorRegistGameServerAck::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorRegistGameServerAckPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    char m_a;                       // +0x0a
    char m_b;                       // +0x0b
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=65（ORIG 0x084e2dee）：槽序/帧差(N82/P212/P221)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=53（ORIG 0x84e2dee）：P226 具名 bool/char 落槽+N18a 单段 regmove+N82/P112 帧槽平移。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→0（71=71，TU 121/1331/5/133 不回退）：真修——
//   删 serverKind/channelNo 具名局部（P0 表达式直用），三个 cMyTrace 改全表达式临时链（P221/P227：
//   `cMyTrace(f,l,5)(fmt, G_CEnvironment()->..., (unsigned char)pPkt->m_a)` 使两实参在 ctor 前分别落 %esi/%ebx）。
//   残余=比较宽度/块布局（ORIG 提升 int `test %eax,%eax; cmp $1,%eax` + je 落 then vs 4.4 8 位 test/jne，P47/P222）。
int Inter_MonitorRegistGameServerAck::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorRegistGameServerAckPktView* pPkt = reinterpret_cast<Inter_MonitorRegistGameServerAckPktView*>(data);

    if (pPkt->m_b == '\x01')
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x5308, 5)(
            "Channel NO %d is already registerd(server type : %d). use another channel NO",
            G_CEnvironment()->m_serverEnvir.m_channelNo,
            (unsigned int)(unsigned char)pPkt->m_a);
        GetInstanceShutdowManager()->SendShutdownSignal();
    }
    else
    {
        // [b28d-r5] ORIG 0x084e2e40 段 `movzbl 0xa(%eax),%eax; movzbl %al,%eax;
        //   test %eax,%eax; je <then0>; cmp $0x1,%eax; je <then1>` = 值只读一次、
        //   int 提升后比较链（P251/P261：无栈槽 + cmp 链 ⇒ 源形是 switch，if-else 链
        //   恒退化为逐次重读 + 8 位 test/jne）。
        switch ((int)(unsigned char)pPkt->m_a)
        {
        case 0:
            cMyTrace(__PRETTY_FUNCTION__, 0x5312, 9, true, true)(
                "[!] Monitor Server Connected");
            break;
        case 1:
            cMyTrace(__PRETTY_FUNCTION__, 0x5317, 9, true, true)(
                "[!] Guild Server Connected");
            break;
        }
    }
    return 0;
}

Inter_LoadMileage::~Inter_LoadMileage() {}
#pragma pack(push, 1)
// Inter_LoadMileagePktView —— Inter_LoadMileage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadMileagePktView
{
    char m_pad0[0x8];  // 未消费
    int m_8;                  // +0x08
};
#pragma pack(pop)

int Inter_LoadMileage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadMileagePktView* pPkt = reinterpret_cast<Inter_LoadMileagePktView*>(data);
    // ORIG +0x0b：`mov -0x10(%ebp),%eax`+`mov 0x8(%eax),%eax` 单级带偏移读 ⇒ 无中间 int* 槽。
    int mileage = pPkt->m_8;
    user->SetMileage(mileage);
    user->SendCashData(false);
    return 0;
}

Inter_GuildExpBookDelete::~Inter_GuildExpBookDelete() {}
// ORIG 0x084e2f1e T dispatch_sig —— 公会经验书删除：目标在线现场删除，离线走 DB+NoCache 包。
// 报告 docs/class_func_reports/Inter_GuildExpBookDelete/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildExpBookDeletePktView —— Inter_GuildExpBookDelete::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GuildExpBookDeletePktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    unsigned int m_e;         // +0x0e
    unsigned int m_12;        // +0x12
    int m_16;                 // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=99（ORIG 0x084e2f1e）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=93（ORIG 0x84e2f1e）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_GuildExpBookDelete::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_GuildExpBookDeletePktView* pPkt = reinterpret_cast<Inter_GuildExpBookDeletePktView*>(data);

    CUser* target = G_CGameManager()->getUser(
        pPkt->m_a, 2);
    if (target == 0)
    {   // 目标离线 → DB 删除 + monitor NoCache 广播
        DB_GuildExpBookDelete::makeRequest(
            pPkt->m_e, 1,
            (ENUM_SERVER_GROUP)pPkt->m_16);
        Packet_No_Cache packet;
        packet.m_dbid = pPkt->m_16;        // +0x0a
        packet.m_serverGroup = pPkt->m_12; // +0x0e
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                (ENUM_SERVER_GROUP)pPkt->m_16);
        proxy->SendPacket(reinterpret_cast<char*>(&packet), packet.packetSize);
    }
    else if (target->getCurCharacR() == 0)
    {   // 在线但未选角色 → 同上
        DB_GuildExpBookDelete::makeRequest(
            pPkt->m_e, 1,
            (ENUM_SERVER_GROUP)pPkt->m_16);
        Packet_No_Cache packet;
        packet.m_dbid = pPkt->m_16;        // +0x0a
        packet.m_serverGroup = pPkt->m_12; // +0x0e
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                (ENUM_SERVER_GROUP)pPkt->m_16);
        proxy->SendPacket(reinterpret_cast<char*>(&packet), packet.packetSize);
    }
    else
    {   // 目标可用 → DB 记录后现场删除
        DB_GuildExpBookDelete::makeRequest(
            pPkt->m_e, 0,
            (ENUM_SERVER_GROUP)pPkt->m_16);
        target->deleteGuildExpBook();
    }
    return 0;
}

Inter_NoCache::~Inter_NoCache() {}
int Inter_NoCache::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
#pragma pack(push, 1)
    struct NoCacheMsgView
    {
        char m_pad0[2];
        unsigned short m_len;       // +0x2
        char m_pad1[6];
        unsigned int m_characId;    // +0xa
        unsigned int m_serverGroup; // +0xe
        int m_cacheType;            // +0x12
    };
#pragma pack(pop)
    NoCacheMsgView* msg = reinterpret_cast<NoCacheMsgView*>(data);
    if (msg->m_cacheType == 0 || msg->m_cacheType == 1)
    {
        if (msg->m_characId == 0)
            G_GameWorld()->ResetCacheCharactorMemory();
        else
            G_GameWorld()->ResetCacheCharactorMemory(msg->m_characId);
    }
    if (msg->m_cacheType == 0 || msg->m_cacheType == 2)
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)msg->m_serverGroup
            )->SendPacket(reinterpret_cast<char*>(msg), (unsigned int)msg->m_len);
    return 0;
}

Inter_Send_Charac_no_from_monitor::~Inter_Send_Charac_no_from_monitor() {}
// ORIG 0x084e312a T dispatch_sig —— 监控侧角色号下发。
// 报告 docs/class_func_reports/Inter_Send_Charac_no_from_monitor/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Send_Charac_no_from_monitorPktView —— Inter_Send_Charac_no_from_monitor::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Send_Charac_no_from_monitorPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    int m_12;                 // +0x12
    char m_16[1];             // +0x16 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

int Inter_Send_Charac_no_from_monitor::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_Send_Charac_no_from_monitorPktView* pPkt = reinterpret_cast<Inter_Send_Charac_no_from_monitorPktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 0x14b);
    if (pPkt->m_12 == 0)
        packet.put_byte(0);
    else
    {
        packet.put_byte(1);
        packet.put_int(pPkt->m_e);
        packet.put_int(pPkt->m_12);
        packet.put_str(pPkt->m_16, pPkt->m_12);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_SendPeriodicMessage::~Inter_SendPeriodicMessage() {}
// ORIG 0x084e3232 T dispatch_sig —— 周期性公告按状态播发。
// 报告 docs/class_func_reports/Inter_SendPeriodicMessage/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SendPeriodicMessagePktView —— Inter_SendPeriodicMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SendPeriodicMessagePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    char m_a;                       // +0x0a
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=12（ORIG 0x084e3232）：块序重排(N104/P223)+寄存器中转(N18a/N7)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：`ret = 0;` 移入 PacketGuard
// 作用域内，复现 ORIG「ret 物化 → dtor → jmp 汇合」顺序）。
int Inter_SendPeriodicMessage::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG 0x084e3232：recv 槽 -0xc 处处 reload（(unsigned long) 往返破 4.4 SSA 传播，技巧26）；
    // 返回经 %ebx 物化（EH 函数 4.1 尾置 mov$0,%ebx + end mov%ebx,%eax）。
    // 正常路径 dtor：ORIG 4.1 inline epilogue（send 后 mov$0,%ebx + dtor + jmp），
    // 4.4 恒合并尾岛共享 dtor call（N 类：4.1 EH 双份 dtor epilogue vs 4.4 merge）。
    // [b28c-32] P0：register asm("ebx") 绑定清除，改 P231 纯 register（合法，无 asm）。
    register int ret;
    char* recv = reinterpret_cast<char*>((unsigned long)data);   // ORIG +0x0a 物化 -0xc
    if (*(recv + 0xa) == '\0')   // ORIG +0x10 add $0xa; movzbl 两步装载；false-branch 先写（技巧17：ORIG fall=island，body 为 jne taken）
    {
        ret = 0;
    }
    else
    {
        {
            PacketGuard packet;
            packet.put_header(0, 299);
            packet.put_int(strlen(recv + 0xa));
            packet.put_str(recv + 0xa, strlen(recv + 0xa));
            packet.finalize(true);
            G_GameWorld()->send_all_with_state(packet, 3);
            ret = 0;
        }
    }
    return ret;
}

Inter_SendTimeSync::~Inter_SendTimeSync() {}
// ORIG 0x084e3326 T dispatch_sig —— 时间同步按状态广播。
// 报告 docs/class_func_reports/Inter_SendTimeSync/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SendTimeSyncPktView —— Inter_SendTimeSync::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SendTimeSyncPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned short m_a;             // +0x0a
    unsigned short m_c;             // +0x0c
};
#pragma pack(pop)

int Inter_SendTimeSync::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SendTimeSyncPktView* pPkt = reinterpret_cast<Inter_SendTimeSyncPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x12c);
    packet.put_short((short)pPkt->m_a);
    packet.put_short((short)pPkt->m_c);
    packet.finalize(true);
    G_GameWorld()->send_all_with_state(packet, 3);
    return 0;
}

Inter_SendTimeSyncLogin::~Inter_SendTimeSyncLogin() {}
// ORIG 0x084e33ec T dispatch_sig —— 登录时时间同步下发。
// 报告 docs/class_func_reports/Inter_SendTimeSyncLogin/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SendTimeSyncLoginPktView —— Inter_SendTimeSyncLogin::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SendTimeSyncLoginPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    unsigned short m_12;            // +0x12
    unsigned short m_14;            // +0x14
};
#pragma pack(pop)

int Inter_SendTimeSyncLogin::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0x53d4;
    Inter_SendTimeSyncLoginPktView* pPkt = reinterpret_cast<Inter_SendTimeSyncLoginPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x12c);
    packet.put_short((short)pPkt->m_12);
    packet.put_short((short)pPkt->m_14);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_AddGuildFundResult::~Inter_AddGuildFundResult() {}
// ORIG 0x084e34ba T dispatch_sig —— 公会基金捐献结果处理。
// 报告 docs/class_func_reports/Inter_AddGuildFundResult/dispatch_sig.md 全文转写。
// [b28d-solo-InterDispatcher 收口] verdict=AE（P231 register int 局部直落 %ebx → 归零）。
int Inter_AddGuildFundResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
#pragma pack(push, 1)
    struct AddGuildFundResultView
    {
        char m_pad[0xa];
        unsigned char m_bSuccess;   // +0xa
        int m_characNo;             // +0xb
        char m_pad2[0x4];
        // ORIG +0x58 两步装载（`mov 0x13(%eax),%eax`+`mov %eax,%ebx`）＝unsigned 字段
        // → gain_money(int) 形参的 signedness 失配形态（Ghidra uVar1；探针 t1 实证）。
        unsigned int m_money;       // +0x13
    };
#pragma pack(pop)
    AddGuildFundResultView* p = reinterpret_cast<AddGuildFundResultView*>(data);
    if (user->get_state() <= 2)
        return 0;
    register int characNoMsg = p->m_characNo;
    if (characNoMsg != user->getCurCharacNo())
        return 0;
    if (p->m_bSuccess != 0)
    {
        user->getCurCharacInvenW()->gain_money(
            p->m_money, (eMoneyAddReason)0x1d, true, 0);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15b, p->m_bSuccess);
    }
    else
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x15b);
    }
    return 0;
}

Inter_TodayGuildMember::~Inter_TodayGuildMember() {}
// ORIG 0x084e358a T dispatch_sig —— 今日公会成员登记（SetTodayGuildMember + 0x130 包）。
// 报告 docs/class_func_reports/Inter_TodayGuildMember/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_TodayGuildMemberPktView —— Inter_TodayGuildMember::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_TodayGuildMemberPktView
{
    char m_pad0[0xe];  // 未消费
    // [b28d-lane2] P248 档②：ORIG 0x084e37d0 段两处 `mov 0xe/0x16(%eax),%ebx` 均单段
    // 直落 callee-saved ⇒ 字段与 getCurCharacNo() 返回值同型（unsigned）；int 字段会使
    // 4.4 插 `mov %eax,%ebx` 中转（2 处 × 1 条）。
    unsigned int m_e;         // +0x0e
    char m_pad12[0x4];  // 未消费
    unsigned int m_16;        // +0x16
    char m_1a[1];             // +0x1a 串/字节区首元素（容量未建模）
    char m_pad1b[0x1d];  // 未消费
    signed char m_38;         // +0x38
    signed char m_39;         // +0x39
    signed char m_3a;         // +0x3a
    signed char m_3b;         // +0x3b
    signed char m_3c;         // +0x3c
};
#pragma pack(pop)

// [b28d-solo-InterDispatcher 收口] verdict=NEAR ndiff=3（ORIG 0x84e358a）：首处比较已由
// register int 局部归位；第二处 `&&` 右操作数 4.4 恒两段 mov（N18a）⇒ 封顶。
int Inter_TodayGuildMember::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_TodayGuildMemberPktView* pPkt = reinterpret_cast<Inter_TodayGuildMemberPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $2; setle
        return 0;

    // [b28d-lane2] N22 判词推翻（探针 /tmp/l2p/p2.cpp fC）：字段读内联进比较即复现
    // ORIG `mov 0xe(%eax),%ebx; cmp %eax,%ebx`；具名（register）局部必产反向 dst。
    if (pPkt->m_e !=
        user->getCurCharacNo())
        return 0;

    if (pPkt->m_16 != 0 &&
        pPkt->m_16 == user->getCurCharacNo())
    {
        user->SetTodayGuildMember(true);
    }

    PacketGuard packet;
    packet.put_header(0, 0x130);
    if (pPkt->m_16 != 0)
    {
        packet.put_byte(1);
        // len 槽 int 精确匹配 put_str(char*,int)（size_t 会致 decay 参数降级 add+反序）
        const int msgLen = (int)strlen(pPkt->m_1a);
        packet.put_int(msgLen);
        packet.put_str(pPkt->m_1a, msgLen);
        packet.put_byte((int)pPkt->m_38);
        packet.put_byte((int)pPkt->m_39);
        packet.put_byte((int)pPkt->m_3a);
        packet.put_byte((int)pPkt->m_3c);
        packet.put_byte((int)pPkt->m_3b);
    }
    else
    {
        packet.put_byte(0);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_CheckJoinGuild::~Inter_CheckJoinGuild() {}
// ORIG 0x084e3790 T dispatch_sig —— 公会加入检查：错误码转发或公会信息包。
// 报告 docs/class_func_reports/Inter_CheckJoinGuild/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CheckJoinGuildPktView —— Inter_CheckJoinGuild::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_CheckJoinGuildPktView
{
    int m_0;                  // +0x00
    char m_4[1];              // +0x4 串/字节区首元素（容量未建模）
    char m_pad5[0x1d];  // 未消费
    char m_22[1];             // +0x22 串/字节区首元素（容量未建模）
    char m_pad23[0x19];  // 未消费
    int m_3c;                 // +0x3c
    int m_40;                 // +0x40
};
#pragma pack(pop)

int Inter_CheckJoinGuild::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_CheckJoinGuildPktView* pPkt = reinterpret_cast<Inter_CheckJoinGuildPktView*>(data);


    if (pPkt->m_0 != 0)
    {   // 结果码非零 → 错误包
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x15c,
            (unsigned char)pPkt->m_0);
    }
    // ORIG：PacketGuard 只在 else 支（两支汇合后 `mov $0x0,%eax`）。
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x15c);
        packet.put_byte(1);
        int strLen = strlen(pPkt->m_22);
        packet.put_int(strLen);
        packet.put_str(pPkt->m_22, strLen);
        strLen = strlen(pPkt->m_4);
        packet.put_int(strLen);
        packet.put_str(pPkt->m_4, strLen);
        packet.put_int(pPkt->m_3c);
        packet.put_int(pPkt->m_40);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_RequestJoinGuild::~Inter_RequestJoinGuild() {}
// ORIG 0x084e3924 T dispatch_sig —— 入会申请结果通知：成功组 0x15d 信息包，失败发错误包。
// 报告 docs/class_func_reports/Inter_RequestJoinGuild/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_RequestJoinGuildPktView —— Inter_RequestJoinGuild::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_RequestJoinGuildPktView
{
    int m_0;                  // +0x00
    int m_4;                  // +0x04
    char m_8[1];              // +0x8 串/字节区首元素（容量未建模）
    char m_pad9[0x1d];  // 未消费
    char m_26[1];             // +0x26 串/字节区首元素（容量未建模）
    char m_pad27[0x19];  // 未消费
    int m_40;                 // +0x40
    int m_44;                 // +0x44
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=110（ORIG 0x084e3924）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=66（ORIG 0x84e3924）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_RequestJoinGuild::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    Inter_RequestJoinGuildPktView* pPkt = reinterpret_cast<Inter_RequestJoinGuildPktView*>(data);

    if (pPkt->m_0 != 0)          // msg+0x00 错误码
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x15d,
            static_cast<unsigned char>(
                pPkt->m_0 & 0xff));
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x15d);
    packet.put_byte(1);
    packet.put_int(pPkt->m_4);
    const int guildNameLen =                         // msg+0x26 公会名
        static_cast<int>(strlen(pPkt->m_26));
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_26, guildNameLen);
    const int requesterLen =                         // msg+0x08 申请者名
        static_cast<int>(strlen(pPkt->m_8));
    packet.put_int(requesterLen);
    packet.put_str(pPkt->m_8, requesterLen);
    packet.put_int(pPkt->m_40);
    packet.put_int(pPkt->m_44);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_JoinGuildInfo::~Inter_JoinGuildInfo() {}
// ORIG 0x084e3ace T dispatch_sig —— 公会加入信息：按结果码选择字段转发。
// 报告 docs/class_func_reports/Inter_JoinGuildInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_JoinGuildInfoPktView —— Inter_JoinGuildInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_JoinGuildInfoPktView
{
    signed char m_0;          // +0x00
    char m_pad1[0x3];  // 未消费
    int m_4;                  // +0x04
    char m_8[1];              // +0x8 串/字节区首元素（容量未建模）
    char m_pad9[0x1d];  // 未消费
    char m_26[1];             // +0x26 串/字节区首元素（容量未建模）
    char m_pad27[0x19];  // 未消费
    int m_40;                 // +0x40
    int m_44;                 // +0x44
};
#pragma pack(pop)

int Inter_JoinGuildInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    // ORIG 0x084e3af5：null 岛后单槽 -0x10=recv（pPkt 经 cast 内联复用同槽，技巧36）。
    char* recv = data;                                 // -0x10(%ebp) 缓存消息指针

    PacketGuard packet;
    packet.put_header(0, 0x131);
    packet.put_byte(
        (int)reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_0);   // movzbl+movsbl
    if (*recv != '\0')
    {
        packet.put_int(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_4);
        int strLen = strlen(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_26);
        packet.put_int(strLen);
        packet.put_str(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_26, strLen);
        strLen = strlen(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_8);
        packet.put_int(strLen);
        packet.put_str(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_8, strLen);
        packet.put_int(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_40);
        packet.put_int(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_44);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_ApproveJoinGuild::~Inter_ApproveJoinGuild() {}
// ORIG 0x084e3f0e T dispatch_sig —— 公会入会审批应答。
// 报告 docs/class_func_reports/Inter_ApproveJoinGuild/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_ApproveJoinGuildPktView —— Inter_ApproveJoinGuild::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_ApproveJoinGuildPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
    int m_e;                        // +0x0e
    char m_pad12[0x4];  // +0x12..未消费
    int m_16;                       // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=82（ORIG 0x084e3f0e）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：删具名 errCode 局部，条件与
// put_byte 实参内联 pPkt->m_a ⇒ ORIG 的逐次重读形 + 帧归位）。
int Inter_ApproveJoinGuild::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ApproveJoinGuildPktView* pPkt = reinterpret_cast<Inter_ApproveJoinGuildPktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    register int charIdMsg =                           // recv charId @ msg+0xe
        pPkt->m_e;
    if (charIdMsg != user->getCurCharacNo())
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x15f);
    if (pPkt->m_a != 0)
    {
        packet.put_byte(0);
        packet.put_byte((int)pPkt->m_a);
        packet.put_int(pPkt->m_16);
    }
    else
    {
        packet.put_byte(1);
        packet.put_int(pPkt->m_16);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_GuildJoinList::~Inter_GuildJoinList() {}
// ORIG 0x084e3c60 T dispatch_sig —— 公会加入申请列表下发（0x161 包，逐申请项序列化）。
// 报告 docs/class_func_reports/Inter_GuildJoinList/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildJoinListEntryWinView —— Inter_GuildJoinList::dispatch_sig 窗口 entry 元素视图
//   （entry 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_GuildJoinListEntryWinView
{
    char m_pad0[0x4];  // 未消费
    int m_4;                  // +0x04
    char m_pad8[0x1e];  // 未消费
    signed char m_26;         // +0x26
    signed char m_27;         // +0x27
    signed char m_28;         // +0x28
    signed char m_29;         // +0x29
    char m_pad2a[0x1a];  // 未消费
    int m_44;                 // +0x44
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=196（ORIG 0x084e3c60）：槽序/帧差(N82/P212/P221)+混合残差；gap+54。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=184（ORIG 0x84e3c60）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_GuildJoinList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;

    char* const local_1c = data;

    PacketGuard packet;
    packet.put_header(1, 0x161);
    packet.put_byte(1);
    packet.put_int(*reinterpret_cast<int*>(local_1c));

    for (int local_18 = 0; local_18 < *reinterpret_cast<int*>(local_1c); ++local_18)
    {
        packet.put_int(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_4);
        const int local_14 = (int)strlen(local_1c + local_18 * 0x44 + 0x08);
        packet.put_int(local_14);
        packet.put_str(local_1c + local_18 * 0x44 + 0x08, local_14);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_26);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_27);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_29);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_28);
        const int local_10 = (int)strlen(local_1c + local_18 * 0x44 + 0x2a);
        packet.put_int(local_10);
        packet.put_str(local_1c + local_18 * 0x44 + 0x2a, local_10);
        packet.put_int(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_44);
    }

    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_GuildAttendanceInfo::~Inter_GuildAttendanceInfo() {}
// ORIG 0x084e4052 T dispatch_sig —— 公会签到信息：校验当前角色号后转发签到统计字段。
// 报告 docs/class_func_reports/Inter_GuildAttendanceInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GuildAttendanceInfoPktView —— Inter_GuildAttendanceInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GuildAttendanceInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    signed char m_12;               // +0x12
    int m_13;                       // +0x13
    int m_17;                       // +0x17
    int m_1b;                       // +0x1b
    int m_1f;                       // +0x1f
    int m_23;                       // +0x23
    int m_27;                       // +0x27
    int m_2b;                       // +0x2b
};
#pragma pack(pop)

// [b28d-solo-InterDispatcher 收口] verdict=AE（P231 register int 局部直落 %ebx → 归零）。
int Inter_GuildAttendanceInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildAttendanceInfoPktView* pPkt = reinterpret_cast<Inter_GuildAttendanceInfoPktView*>(data);
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    register int characNoMsg = pPkt->m_a;
    if (characNoMsg != user->getCurCharacNo())
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x164);
    packet.put_byte(1);
    packet.put_byte((int)pPkt->m_12);  // movzbl+movsbl
    packet.put_int(pPkt->m_13);
    packet.put_int(pPkt->m_17);
    packet.put_int(pPkt->m_2b);
    packet.put_int(pPkt->m_23);
    packet.put_int(pPkt->m_27);
    packet.put_int(pPkt->m_1b);
    packet.put_int(pPkt->m_1f);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_AchieveGuildAttendance::~Inter_AchieveGuildAttendance() {}
// ORIG 0x084e41e4 T dispatch_sig —— 公会签到达成应答。
// 报告 docs/class_func_reports/Inter_AchieveGuildAttendance/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AchieveGuildAttendancePktView —— Inter_AchieveGuildAttendance::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_AchieveGuildAttendancePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    char m_pade[0x4];  // +0xe..未消费
    int m_12;                       // +0x12
    int m_16;                       // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=NEAR ndiff=1（ORIG 0x084e41e4）：寄存器中转(N18a/N7)；gap0。
// [b28d-solo-InterDispatcher] verdict=AE（收口：字段/局部 int 同型化 + 删 (unsigned) cast → 归零）。
int Inter_AchieveGuildAttendance::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AchieveGuildAttendancePktView* pPkt = reinterpret_cast<Inter_AchieveGuildAttendancePktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    // ORIG 0x084e4214 `mov 0xa(%eax),%ebx`：register 直落 callee-saved（cmp 方向 N22）。
    register int charIdMsg =                           // recv charId @ msg+0xa
        pPkt->m_a;
    if (charIdMsg != user->getCurCharacNo())
        return 0;

    // ORIG 0x084e425b/0x084e4270：m_12/m_16 各自从 -0xc reload（PG 生命周期内无缓存）。
    PacketGuard packet;
    packet.put_header(0, 0x134);
    packet.put_int(pPkt->m_12);
    packet.put_int(pPkt->m_16);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_UpdatePurchaseCashItemBonus::~Inter_UpdatePurchaseCashItemBonus() {}
// ORIG 0x084e42e0 T dispatch_sig —— 商城消费奖励邮件触发。
// 报告 docs/class_func_reports/Inter_UpdatePurchaseCashItemBonus/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_UpdatePurchaseCashItemBonusPktView —— Inter_UpdatePurchaseCashItemBonus::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_UpdatePurchaseCashItemBonusPktView
{
    unsigned int m_0;               // +0x00
};
#pragma pack(pop)

// [b28d-solo-InterDispatcher] P226 语句表达式复现 ORIG 0x084e42e0 的 setle 物化直测
//   （`cmp $0x2,%eax; setle %al; test %al,%al; je`；具名 bool 局部 4.4 恒落槽 +2）。
int Inter_UpdatePurchaseCashItemBonus::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x084e42e0：state 检查 setle 布尔物化；pPkt 检查后才落槽 -0xc；
    // *recv 实参先于链式 call（R2L，ORIG +0x35 mov (%eax),%ebx 跨双 call）。
    if (({ user->get_state() <= 2; }))
        return 0;                                // ORIG +0x1c mov $0; jmp end
    Inter_UpdatePurchaseCashItemBonusPktView* pPkt =   // ORIG +0x26 物化 -0xc
        reinterpret_cast<Inter_UpdatePurchaseCashItemBonusPktView*>(data);
    if (pPkt->m_0 != 0)                          // ORIG +0x30 test %eax,%eax; je 尾岛（body fall-through 在前，技巧17）
    {
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->send_purchase_bonus_mail(user, pPkt->m_0);
    }
    return 0;
}

Inter_HadesDisConnect::~Inter_HadesDisConnect() {}
int Inter_HadesDisConnect::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    Timer_HadesHeartBeat::registNextTimer();
    return 0;
}

Inter_GoblinPadChagePwdReward::~Inter_GoblinPadChagePwdReward() {}
int Inter_GoblinPadChagePwdReward::dispatch_sig(
    CUser* user, char* data, int /*len*/)
{
    char* packet = data;
    if (user != 0)
    {
        user->getPad()->SendChagePwdReward(user);
    }
    return 0;
}

Inter_GoblinPadSamePwdCheck::~Inter_GoblinPadSamePwdCheck() {}
int Inter_GoblinPadSamePwdCheck::dispatch_sig(
    CUser* user, char* data, int /*len*/)
{
    char* packet = data;
    if (user != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x74),
                                 static_cast<unsigned char>(0x8b));
    }
    return 0;
}

Inter_PunishCancel::~Inter_PunishCancel() {}
// ORIG 0x084e4b02 T dispatch_sig —— 交易惩罚撤销。
// 报告 docs/class_func_reports/Inter_PunishCancel/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_PunishCancelPktView —— Inter_PunishCancel::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_PunishCancelPktView
{
    char m_pad0[0xe];  // 未消费
    short m_e;                // +0x0e
    short m_10;               // +0x10
};
#pragma pack(pop)

// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084e4b02）：混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本波已归零；AE 归一化后逐条相等）
int Inter_PunishCancel::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PunishCancelPktView* pPkt = reinterpret_cast<Inter_PunishCancelPktView*>(data);
    if (user != 0 &&
        pPkt->m_e == 2 &&
        pPkt->m_10 == 0x20 &&
        user->isTradePunishType(0x20))
    {
        user->DelTradePunishType(
            (int)(unsigned short)pPkt->m_10);

        PacketGuard packet;
        packet.put_header(0, 0x13f);
        packet.put_short((unsigned short)pPkt->m_e);
        packet.put_short((unsigned short)pPkt->m_10);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_BroadcastMsg::~Inter_BroadcastMsg() {}
// ORIG 0x084e4c2a T dispatch_sig —— 少年频道公告组播。
// 报告 docs/class_func_reports/Inter_BroadcastMsg/dispatch_sig.md 全文转写。
// CBelong 唯一声明点 = CSchoolMgr.h（本 TU 已 include；NL4 合并铁律迁移）。

// ORIG CUserTeen（CBelong 派生，C1 W 0x084f07d0 / IsRight W；BroadcastMsg 消费；
// 定义属主未落地，U 引用合规——InterWave1 批）
class CUserTeen : public CBelong
{
public:
    CUserTeen();
    bool IsRight(CUser* user);   // bool 化随 CBelong::IsRight 权威签名（mangled 不编码返回类型）
};

// ORIG CUserTeen::CUserTeen() W 0x084f07d0：调 CBelong() 后重置 vptr（正常 C++ 生成）。
CUserTeen::CUserTeen() {}

// ORIG CUserTeen::IsRight(CUser*) W 0x084f078c：
//   CheckLimitMiniumAge(0xf, local) 结果取反（xor $1）后 if/else 物化 0/1：
//   ORIG 0x084f07ac `xor $0x1,%eax`; 07af `test`; 07b1 `je`; 07b3 fall
//   `mov $0x0,%eax`+`jmp`; je 目标 07ba `mov $0x1,%eax`。直接 return !call()
//   缺这四行物化（技巧10 形）。
bool CUserTeen::IsRight(CUser* user)
{
    // ORIG +0x06：无 `movl $0x0,-0xc(%ebp)` ⇒ 局部不初始化（仅取址传引用）；
    int dummy;
    if (!user->CheckLimitMiniumAge(0xf, dummy))
        return 0;
    return 1;
}

#pragma pack(push, 1)
// Inter_BroadcastMsgPktView —— Inter_BroadcastMsg::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_BroadcastMsgPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    unsigned char m_e;        // +0x0e
    char m_f[1];              // +0xf 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=71（ORIG 0x084e4c2a）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=45（ORIG 0x84e4c2a）：N18a 单段 regmove。
int Inter_BroadcastMsg::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_BroadcastMsgPktView* pPkt = reinterpret_cast<Inter_BroadcastMsgPktView*>(data);
    if (pPkt->m_a == 0)
    {
        // [b28d-lane2] ORIG 0x084e4c5c 段无 msgLen 槽（`movzbl 0xe(%eax)` 直入 put_int/
        // put_str 实参，逐次重读）⇒ 删具名局部（P0：无槽往返 ⇒ 原码无该局部）。
        PacketGuard packet;
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int(pPkt->m_e);
        packet.put_str(pPkt->m_f, pPkt->m_e);
        packet.finalize(true);

        CUserTeen teen;
        G_GameWorld()->send_group(packet, &teen);

    }
    return 0;
}

Inter_TOD_ReturnToMonitor_APCSettingComplete::~Inter_TOD_ReturnToMonitor_APCSettingComplete() {}
// ORIG 0x084e4d90 T dispatch_sig —— 绝望之塔随机选择完成上报监控服。
// 报告 docs/class_func_reports/Inter_TOD_ReturnToMonitor_APCSettingComplete/dispatch_sig.md 全文转写。
int Inter_TOD_ReturnToMonitor_APCSettingComplete::dispatch_sig(
    CUser* /*user*/, char* /*data*/, int /*len*/)
{
    // ORIG +0x11 装参序 `mov <mgr>,%eax` 先于 `mov -0xc(%ebp),%edx` ⇒ 实参为 enum 同型 lvalue
    // （call 处 (ENUM)x cast 会反转成 edx 先载；probe use1/use2 实证）。
    const ENUM_SERVER_GROUP group =
        (ENUM_SERVER_GROUP)G_CEnvironment()->get_server_group();
    // ORIG +0x11：值先落 -0xc(%ebp)（普通 const 局部），再 `mov -0xc(%ebp),%ebx` 取别名
    // 跨 cMyTrace ctor 缓存 ⇒ 两级写法（register 别名；后续 GetServerProxy 仍读 -0xc 原槽）。
    register int tracedGroup = group;
    cMyTrace(__PRETTY_FUNCTION__,
        0x61f7, 5)("TOD Random Select Complete. send->monitor group(%d)", tracedGroup);
    Packet_TOD_DoRandomSelect packet;
    // ORIG 0x084e4df2/df7：this 先载 eax、group 后载 edx（enum 同型直传）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        group)->SendTcpPacket(reinterpret_cast<char*>(&packet), 10);
    return 0;
}

Inter_TOD_Reload_APC::~Inter_TOD_Reload_APC() {}
// ORIG 0x084e4e26 T dispatch_sig —— 绝望之塔 APC 信息重载。
// 报告 docs/class_func_reports/Inter_TOD_Reload_APC/dispatch_sig.md 全文转写。
int Inter_TOD_Reload_APC::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    _APCInfo* infos = reinterpret_cast<_APCInfo*>(data);
    static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr)->ResetAPCInfo();
    for (int i = 0; i <= 9; ++i)
    {
        static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr)->SetAPCInfo(
            (unsigned int)i,
            infos[i]);
    }
    return 0;
}

Inter_UpdateSecuServiceState::~Inter_UpdateSecuServiceState() {}
// ORIG 0x084e4e84 T dispatch_sig —— 安全服务状态更新：msg[0x14]=0 后转 Secu_DataControl。
// 报告 docs/class_func_reports/Inter_UpdateSecuServiceState/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_UpdateSecuServiceStateMsgWinView —— Inter_UpdateSecuServiceState::dispatch_sig 窗口 msg 元素视图
//   （msg 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_UpdateSecuServiceStateMsgWinView
{
    char m_pad0[0xe];  // 未消费
    unsigned char m_e;        // +0x0e
    unsigned char m_f;        // +0x0f
    char m_pad10[0x4];  // 未消费
    unsigned char m_14;       // +0x14
};
#pragma pack(pop)

int Inter_UpdateSecuServiceState::dispatch_sig(CUser* user, char* data, int len)
{
    // ORIG +0x06：包基址先无条件落 -0xc，其后才是 user 判空门。
    Inter_UpdateSecuServiceStateMsgWinView* pWin_msg =
        reinterpret_cast<Inter_UpdateSecuServiceStateMsgWinView*>(data);
    // ORIG +0x0c 判的是 0xc(%ebp)＝user 形参（非 data）。
    if (user != 0)
    {
        pWin_msg->m_14 = 0;
        GlobalData::s_pSecuDataControl->UpdateSecurityServiceState(
            user,                                      // 0xc(%ebp)
            (unsigned char)pWin_msg->m_e,
            (unsigned char)pWin_msg->m_f,
            pWin_msg->m_pad10);   // §4.3 具名数组基址替代裸偏移 pWin_msg+0x10
    }
    return 0;
}

Inter_LoadKillMonsterInfo::~Inter_LoadKillMonsterInfo() {}
// ORIG 0x084e4ee4 T dispatch_sig —— 击杀怪物统计 DB 加载（扩展数据槽 0xc）。
// 报告 docs/class_func_reports/Inter_LoadKillMonsterInfo/dispatch_sig.md 全文转写。
// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（35=35，助记符全等）。
int Inter_LoadKillMonsterInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0x6248;
    charac_expand::CData* p = reinterpret_cast<charac_expand::CData*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xc));
    p->loadData(user, data);
    user->EnableCharacInfo((CACHE_SECTION)9);
    return 0;
}

Inter_LevelUpReward::~Inter_LevelUpReward() {}
// ORIG 0x084e4f56 T dispatch_sig —— 升级奖励信号数据暂存（局部拷贝后丢弃）。
// 报告 docs/class_func_reports/Inter_LevelUpReward/dispatch_sig.md 全文转写。
int Inter_LevelUpReward::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0x62a6;
    unsigned int field0;
    unsigned int field4;
    memcpy(&field0, data, 4);
    memcpy(&field4, data + 4, 4);
    (void)field0; (void)field4;
    return 0;
}

Inter_GetUserRegdate::~Inter_GetUserRegdate() {}
#pragma pack(push, 1)
// Inter_GetUserRegdatePktView —— Inter_GetUserRegdate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GetUserRegdatePktView
{
    unsigned int m_0;         // +0x00
    unsigned int m_4;         // +0x04
    unsigned int m_8;         // +0x08
};
#pragma pack(pop)

int Inter_GetUserRegdate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GetUserRegdatePktView* pPkt = reinterpret_cast<Inter_GetUserRegdatePktView*>(data);
    // ORIG +0x11/+0x17：`mov 0x8(%eax),%edx`/`mov 0x4(%eax),%eax` 单级带偏移读 ⇒ 字段直取
    // （int* 中间槽会退化成 add+mov 两级）。
    checkNewMemberHacklog(pPkt->m_4, user, pPkt->m_8);
    return 0;
}

Inter_GoldControlMoneyActionCancel::~Inter_GoldControlMoneyActionCancel() {}
// ORIG 0x084e4fe8 T dispatch_sig —— 金币控制邮件操作取消/完成应答。
// 报告 docs/class_func_reports/Inter_GoldControlMoneyActionCancel/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GoldControlMoneyActionCancelPktView —— Inter_GoldControlMoneyActionCancel::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GoldControlMoneyActionCancelPktView
{
    unsigned char m_0;              // +0x00
    bool m_1;                       // +0x01
};
#pragma pack(pop)

int Inter_GoldControlMoneyActionCancel::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GoldControlMoneyActionCancelPktView* pPkt = reinterpret_cast<Inter_GoldControlMoneyActionCancelPktView*>(data);
    if (user != 0)
    {
        // ORIG 直接 user+0x8e980 成员访问（无 GetGoldControl() 调用）。
        if (pPkt->m_0 != 0)
            user->m_goldControl.MailSameMid();
        else
            user->m_goldControl.MailComplete(
                pPkt->m_1);
    }
    return 0;
}

Inter_MonitorNoticeGuildChatMsgHyperLink::~Inter_MonitorNoticeGuildChatMsgHyperLink() {}
// ORIG 0x084e503c T dispatch_sig —— 监控公会聊天超链接广播（字符串+超链接数组）。
// 报告 docs/class_func_reports/Inter_MonitorNoticeGuildChatMsgHyperLink/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeGuildChatMsgHyperLinkPktView —— Inter_MonitorNoticeGuildChatMsgHyperLink::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeGuildChatMsgHyperLinkPktView
{
    char m_pad0[0xe];  // 未消费
    int m_e;                  // +0x0e
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    unsigned char m_30;       // +0x30
    // ORIG 0x084e51xx 循环体走数组寻址 `imul $0x68 → add $0x30 → add 基址槽 → add $0x1`
    //（结构位移 0x30 + 元素首字段 0x1 两步；Member 版同配方 probe addr4 t18 实证）。
    struct ItemView { char m_0[1]; char m_pad[0x67]; };  // stride 0x68
    ItemView m_items[1];      // +0x31 链接项区（实际个数=m_30；容量未建模）
    char m_pad31[0xd0];  // 未消费
    unsigned char m_169;      // +0x169
    char m_16a[1];            // +0x16a 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=146（ORIG 0x084e503c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=96（ORIG 0x84e503c）：N22 cmp 操作数方向+N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_MonitorNoticeGuildChatMsgHyperLink::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    Inter_MonitorNoticeGuildChatMsgHyperLinkPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildChatMsgHyperLinkPktView*>(data);
    // §P0：ORIG 084e5080/084e508f 两次独立重读 msg+0xe（比较用 eax、logFormat 用 ebx 跨 call 保活，
    //   均无槽往返）⇒ 不设具名 characNo。
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x63e4,
            "Inter_MonitorNoticeGuildChatMsgHyperLink::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x173);
    packet.put_byte(6);
    packet.put_byte(0);
    const int msgLen = (int)strlen(pPkt->m_12);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_12, msgLen);
    packet.put_byte(0);
    // §P0：ORIG 084e5184/084e51a0 两次独立重读 msg+0x169（put_int 与 put_str 各自 movzbl 双段，无槽）。
    packet.put_int((int)(unsigned int)pPkt->m_169);
    packet.put_str(pPkt->m_16a, (int)(unsigned int)pPkt->m_169);
    packet.put_byte(
        (int)pPkt->m_30);
    for (int i = 0;
         i < (int)(unsigned int)pPkt->m_30;
         ++i)
    {
        packet.put_binary(pPkt->m_items[i].m_0, 0x68);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeMemberChatMsgHyperLink::~Inter_MonitorNoticeMemberChatMsgHyperLink() {}
// ORIG 0x084e5282 T dispatch_sig —— 公会员聊天超链接公告包（0x173）下发。
// 报告 docs/class_func_reports/Inter_MonitorNoticeMemberChatMsgHyperLink/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorNoticeMemberChatMsgHyperLinkPktView —— Inter_MonitorNoticeMemberChatMsgHyperLink::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeMemberChatMsgHyperLinkPktView
{
    char m_pad0[0xe];  // 未消费
    unsigned int m_e;         // +0x0e（同上 unsigned→int 失配两步 spill 形态）
    char m_12[1];             // +0x12 串/字节区首元素（容量未建模）
    char m_pad13[0x1d];  // 未消费
    unsigned char m_30;       // +0x30
    // ORIG 0x084e56xx 循环体走数组寻址 `imul $0x68 → add $0x30 → add 基址槽 → add $0x1`；
    // gcc 把数组基 0x31 折成 0x30(结构位移)+0x1(元素首字段)两步（probe addr4 t18 实证）。
    struct ItemView { char m_0[1]; char m_pad[0x67]; };  // stride 0x68
    ItemView m_items[1];        // +0x31 链接项区（实际个数=m_30；容量未建模）
    char m_pad99[0xd0];    // 未消费
    unsigned char m_169;      // +0x169
};
#pragma pack(pop)

int Inter_MonitorNoticeMemberChatMsgHyperLink::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x640a,
            "MEMBER : Inter_MonitorNoticeMemberChatMsgHyperLink::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    // ORIG：包基址在两道 guard 后物化入 -0x14(%ebp)
    Inter_MonitorNoticeMemberChatMsgHyperLinkPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberChatMsgHyperLinkPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x6411,
            "MEMBER : Inter_MonitorNoticeMemberChatMsgHyperLink::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x173);
    packet.put_byte(8);
    packet.put_byte(0);
    // len 槽 int 精确匹配（size_t 致 decay 降级 add+反序）
    const int local_14 = (int)strlen(pPkt->m_12);
    packet.put_int(local_14);
    packet.put_str(pPkt->m_12, local_14);
    packet.put_byte(0);
    packet.put_int(
        (unsigned int)pPkt->m_169);
    packet.put_str(
        reinterpret_cast<char*>(pPkt) + 0x16a,        // ORIG 经 -0x14 槽
        (unsigned int)pPkt->m_169);
    packet.put_byte(
        (unsigned int)pPkt->m_30);
    for (int local_10 = 0;
         local_10 < (int)(unsigned int)pPkt->m_30;
         ++local_10)
    {
        // ORIG 0x084e5xxx：数组寻址两步位移形态（见 PktView ItemView 注释）。
        packet.put_binary(pPkt->m_items[local_10].m_0, 0x68);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_MonitorNoticeOtherChannelChatHyperLink::~Inter_MonitorNoticeOtherChannelChatHyperLink() {}
#pragma pack(push, 1)
// Inter_MonitorNoticeOtherChannelChatHyperLinkPktView —— Inter_MonitorNoticeOtherChannelChatHyperLink::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorNoticeOtherChannelChatHyperLinkPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x5];  // 未消费
    int m_13;                 // +0x13
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
    char m_pad18[0x1d];  // 未消费
    unsigned char m_35;       // +0x35
    unsigned char m_36;       // +0x36
    char m_pad37[0x138];  // 未消费
    unsigned char m_16f;      // +0x16f
    char m_170[1];            // +0x170 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=423（ORIG 0x084e5528）：槽序/帧差(N82/P212/P221)+混合残差；gap+20。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84e5528）：本轮每分支单次 strlen 缓存+get_charac_no 两处独立调用（p167 CLEAR）；余 N22/P226/N18a。
int Inter_MonitorNoticeOtherChannelChatHyperLink::dispatch_sig(
    CUser* user, char* data, int /*len*/)
{
    Inter_MonitorNoticeOtherChannelChatHyperLinkPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeOtherChannelChatHyperLinkPktView*>(data);
    // ORIG 0x084e5528 报告 Inter_MonitorNoticeOtherChannelChatHyperLink/dispatch_sig.md
    // 两段全读。四分支按 msg[0x35]（1/2/3/其余）各组成 0x173 通知包。
    if (user->get_state() == 0)
        return 0;

    // §P0：ORIG 对 m_35/m_36 全程重读（无具名局部槽往返）⇒ 不缓存 linkType/itemCount/count。
    if (pPkt->m_35 == 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0x173);
        packet.put_byte(pPkt->m_a);
        packet.put_byte(0x15);
        // ORIG 每分支单次 strlen（p167 EXTRA strlen×4 实证）⇒ 具名缓存。
        const int strLen = static_cast<int>(strlen(pPkt->m_17));
        packet.put_int(strLen);
        packet.put_str(pPkt->m_17, strLen);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.put_byte(pPkt->m_36);
        for (int i = 0; i < (int)pPkt->m_36; ++i)
            packet.put_binary(data + i * 0x68 + 0x37, 0x68);
        packet.finalize(true);
        user->Send(packet);
    }
    else if (pPkt->m_35 == 2)
    {
        PacketGuard packet;
        packet.put_header(0, 0x173);
        packet.put_byte(pPkt->m_a);
        packet.put_byte(0x4d);
        const int strLen = static_cast<int>(strlen(pPkt->m_17));
        packet.put_int(strLen);
        packet.put_str(pPkt->m_17, strLen);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.put_byte(pPkt->m_36);
        for (int i = 0; i < (int)pPkt->m_36; ++i)
            packet.put_binary(data + i * 0x68 + 0x37, 0x68);
        packet.finalize(true);
        user->Send(packet);
    }
    else if (pPkt->m_35 == 3)
    {
        PacketGuard packet;
        packet.put_header(0, 0x173);
        packet.put_byte(pPkt->m_a);
        packet.put_byte(0x5a);
        const int strLen = static_cast<int>(strlen(pPkt->m_17));
        packet.put_int(strLen);
        packet.put_str(pPkt->m_17, strLen);
        packet.put_byte(0);
        packet.put_int(0);
        packet.put_str("", 0);
        packet.put_byte(pPkt->m_36);
        for (int i = 0; i < (int)pPkt->m_36; ++i)
            packet.put_binary(data + i * 0x68 + 0x37, 0x68);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        // ORIG 0x84e5a18/0x84e5a42：两处独立 get_charac_no 调用（P48，无具名缓存）。
        if (user->get_charac_no(-1) != pPkt->m_13)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                "virtual int Inter_MonitorNoticeOtherChannelChatHyperLink::"
                "dispatch_sig(CUser*, char*, int)", 0x6497,
                "Inter_MonitorNoticeOtherChannelChatHyperLink::dispatch_sig() : "
                "pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
                user->get_charac_no(-1), pPkt->m_13);
            return 0;
        }

        PacketGuard packet;
        packet.put_header(0, 0x173);
        packet.put_byte(pPkt->m_a);
        packet.put_byte(0);
        const int strLen = static_cast<int>(strlen(pPkt->m_17));
        packet.put_int(strLen);
        packet.put_str(pPkt->m_17, strLen);
        packet.put_byte(0);
        // ORIG 084e5b28/084e5b44：0x16f 按 1 字节读（movzbl），零扩展传值。
        packet.put_int(pPkt->m_16f);
        packet.put_str(pPkt->m_170,
                       pPkt->m_16f);
        packet.put_byte(pPkt->m_36);
        for (int i = 0; i < (int)pPkt->m_36; ++i)
            packet.put_binary(data + i * 0x68 + 0x37, 0x68);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}

Inter_Monitor_MegaphoneMsgHyperLink::~Inter_Monitor_MegaphoneMsgHyperLink() {}
// ORIG 0x084e5c26 T dispatch_sig —— 大喇叭超链接广播（0x174 包，按频道整合判定分发）。
// 报告 docs/class_func_reports/Inter_Monitor_MegaphoneMsgHyperLink/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_Monitor_MegaphoneMsgHyperLinkPktView —— Inter_Monitor_MegaphoneMsgHyperLink::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_Monitor_MegaphoneMsgHyperLinkPktView
{
    char m_pad0[0xa];  // 未消费
    signed char m_a;          // +0x0a
    unsigned char m_b;        // +0x0b
    unsigned short m_c;       // +0x0c
    unsigned char m_e;        // +0x0e
    char m_f[1];              // +0xf 串/字节区首元素（容量未建模）
    char m_pad10[0x1d];  // 未消费
    unsigned char m_2d;       // +0x2d
    char m_pad2e[0x138];  // 未消费
    unsigned char m_166;      // +0x166
    char m_167[1];            // +0x167 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=84（ORIG 0x084e5c26）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=83（ORIG 0x84e5c26）：N18a 单段 regmove+P30/N4 add-vs-lea。
int Inter_Monitor_MegaphoneMsgHyperLink::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Monitor_MegaphoneMsgHyperLinkPktView* pPkt = reinterpret_cast<Inter_Monitor_MegaphoneMsgHyperLinkPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x174);
    packet.put_byte(
        (int)pPkt->m_e);
    packet.put_byte(
        (int)pPkt->m_b);
    packet.put_short(
        (int)(unsigned short)pPkt->m_c);
    // ORIG 0x84e5cae/0x84e5ccb：strlen 双调（put_int/put_str 各算一次，O0 不 CSE）
    packet.put_int((int)strlen(pPkt->m_f));
    packet.put_str(pPkt->m_f, (int)strlen(pPkt->m_f));
    const unsigned int linkLen =
        (unsigned int)pPkt->m_166;
    packet.put_int((int)linkLen);
    packet.put_str(pPkt->m_167, (int)linkLen);
    packet.put_byte(
        (int)pPkt->m_2d);
    for (int i = 0;
         i < (int)(unsigned int)pPkt->m_2d;
         ++i)
    {
        packet.put_binary(data + i * 0x68 + 0x2e, 0x68);
    }
    packet.finalize(true);

    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        G_GameWorld()->send_all(packet);
    }
    else
    {
        const char group =
            pPkt->m_a;
        G_GameWorld()->send_server_group(packet, group);
    }
    return 0;
}

Inter_SendSecuReward::~Inter_SendSecuReward() {}
int Inter_SendSecuReward::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    int* packet = reinterpret_cast<int*>(data);      // 整包首槽指针视图（非成员访问）
    if (*packet == 0)
    {
        user->AddSecuRewardItem(static_cast<unsigned char>(8));
    }
    else
    {
        user->AddSecuRewardItem(static_cast<unsigned char>(0x10));
    }
    return 0;
}

// ORIG 0x084e5e62 W _ZN17Inter_HadesAction10send_alertEP5CUserj
// 报告 docs/class_func_reports/Inter_HadesAction/send_alert.md：PacketGuard→
// clear→put_header(0,0x1bf)→put_int(code)→finalize(true)→user->Send。
void Inter_HadesAction::send_alert(CUser* user, unsigned int code)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(0, 0x1bf);
    guard.put_int(static_cast<int>(code));
    guard.finalize(true);
    user->Send(guard);
}

Inter_HadesAction::~Inter_HadesAction() {}
// ORIG 0x084e5f02 T dispatch_sig —— Hades 行为处理：登录用户现场执行，未登录用户落惩罚 DB。
// 报告 docs/class_func_reports/Inter_HadesAction/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_HadesActionPktView —— Inter_HadesAction::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_HadesActionPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    unsigned int m_12;        // +0x12
    unsigned int m_16;        // +0x16
    unsigned int m_1a;        // +0x1a
    unsigned int m_1e;        // +0x1e
};
#pragma pack(pop)

int Inter_HadesAction::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_HadesActionPktView* pPkt = reinterpret_cast<Inter_HadesActionPktView*>(data);

    if (user != 0 && user->get_acc_id() == pPkt->m_a)
    {
        char ipBuf[16] = { '\0' };                     // 16 字节内联清零
        static_cast<XNuclear::CHades*>(user->getHades())
            ->GetIP(ipBuf);                            // 两次 getHades 各取一次（P48）
        static_cast<XNuclear::CHades*>(user->getHades())
            ->RunHadesAction(pPkt->m_12,
                             pPkt->m_16,
                             pPkt->m_1a,
                             pPkt->m_1e,
                             ipBuf);
        send_alert(user, pPkt->m_12);
    }
    else if (user == 0 && pPkt->m_a != 0)
    {
        if ((pPkt->m_12 & 2) != 0)
        {   // 双端惩罚（type=2）
            DB_HadesPunish::makeRequest(
                pPkt->m_a, 2,
                pPkt->m_16,
                pPkt->m_1a,
                pPkt->m_1e,
                reinterpret_cast<char*>(0));
        }
        if ((pPkt->m_12 & 1) != 0)
        {   // 单端惩罚（type=1）
            DB_HadesPunish::makeRequest(
                pPkt->m_a, 1,
                pPkt->m_16,
                pPkt->m_1a,
                pPkt->m_1e,
                reinterpret_cast<char*>(0));
        }
    }
    return 0;
}

Inter_Check3rdPartyConcent::~Inter_Check3rdPartyConcent() {}
// ORIG 0x084e6096 T dispatch_sig —— 第三方聚合协议同意结果下发。
// 报告 docs/class_func_reports/Inter_Check3rdPartyConcent/dispatch_sig.md 全文转写。
int Inter_Check3rdPartyConcent::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;
    // ORIG +0x26：包基址落 -0xc 具名局部（PacketGuard 因此被推到 -0x18）。
    char* pPkt = data;
    PacketGuard packet;
    packet.put_header(1, 0x1c3);
    packet.put_byte(1);
    packet.put_byte((int)(signed char)*pPkt);   // ORIG movzbl+movsbl 两级
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_SelectCharacMid::~Inter_SelectCharacMid() {}
// ORIG 0x084e6170 T dispatch_sig —— 角色选择中间校验/hack 计数与金币追踪。
// 报告 docs/class_func_reports/Inter_SelectCharacMid/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_SelectCharacMidPktView —— Inter_SelectCharacMid::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SelectCharacMidPktView
{
    int m_0;                  // +0x00
    unsigned int m_4;         // +0x04
    unsigned int m_8;         // +0x08
    char m_padc[0x4];  // 未消费
    unsigned int m_10;        // +0x10
};
#pragma pack(pop)

// [b28d-solo-InterDispatcher] P226 语句表达式复现 ORIG 0x084e6170 的短路 al 物化
//   （mov$1/mov$0/test al je tail；具名 bool 局部 4.4 恒落槽）。
int Inter_SelectCharacMid::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SelectCharacMidPktView* pPkt = reinterpret_cast<Inter_SelectCharacMidPktView*>(data);
    // ORIG 0x084e6170：user&&characR 以 al 物化（mov$1/mov$0/test al je tail，技巧22）；
    // m_0 检查独立正向（jne tail）；fatigue 实参免槽（call→movzwl %ax,%edi 跨双 call，R2L）。
    if (({ user != 0 && user->getCurCharacR() != 0; }))
    {
        if (pPkt->m_0 == 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user,
                (WongWork::ENUM_HACKTYPE)pPkt->m_4, 1,
                pPkt->m_10,
                pPkt->m_8);
            if (pPkt->m_4 == 0x2ce)
            {
                reinterpret_cast<XNuclear::CHades*>(user->getHades())->GoldTracking(
                    2,
                    pPkt->m_10, "",
                    pPkt->m_8,
                    user->getCurCharacUsedFatigue() & 0xffff);
            }
        }
    }
    return 0;
}

Inter_NpcLimitBuyItemInfoAll::~Inter_NpcLimitBuyItemInfoAll() {}
// ORIG 0x084e6258 T dispatch_sig —— NPC 限购全部条目信息下发（0x193 包）。
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemInfoAll/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NpcLimitBuyItemInfoAllPktView —— Inter_NpcLimitBuyItemInfoAll::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NpcLimitBuyItemInfoAllPktView
{
    char m_pad0[0x16];  // +0x0..未消费
    int m_16;                       // +0x16
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=135（ORIG 0x084e6258）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=87（ORIG 0x84e6258）：P30/N4 add-vs-lea+P264 .o 重定位/立即数。
// [b28d-SweepInterDispatcher2] verdict=DIFF gap 5→6（156 vs 150，TU 121/1331/5/133 不回退）：真修两处——
//   ① `user==0 || get_state()==0` 拆两条独立守卫（P133，复现 ORIG 两段 `jne`/`sete;je` 早退）；
//   ② pPkt 声明移到 get_state 检查后（P0，复现 ORIG `mov 0x10(%ebp),%eax; mov %eax,-0x10` 位置）。
//   残余=data 形参 vs ORIG -0x10 槽基址（`lea (%ecx,%eax,1)` vs `add $0x1a; add 0x10(%ebp)`，P30/N4 族）。
int Inter_NpcLimitBuyItemInfoAll::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)                                     // ORIG 两独立守卫（P133）
        return 0;
    if (user->get_state() == 0)
        return 0;

    Inter_NpcLimitBuyItemInfoAllPktView* pPkt = reinterpret_cast<Inter_NpcLimitBuyItemInfoAllPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x193);
    packet.put_int(pPkt->m_16);   // msg+0x16 count
    // ORIG 0x84e62de..0x84e63a9 每处 `lea (%ecx,%eax,1)` 的 ecx = -0x10 具名槽（pPkt），
    // 非形参 0x10(%ebp)（P30/N4 族可消）。
#define pData (reinterpret_cast<char*>(pPkt))
    for (int i = 0;
         i < pPkt->m_16 && i < 0x1e; ++i)
    {
        packet.put_int(
            *reinterpret_cast<int*>(pData + i * 0xc + 0x1a));
        if (*reinterpret_cast<int*>(pData + i * 0xc + 0x1e) == 0)
        {
            packet.put_int(0);
        }
        else if ((unsigned int)*reinterpret_cast<unsigned int*>(pData + i * 0xc + 0x22) <
                 (unsigned int)*reinterpret_cast<unsigned int*>(pData + i * 0xc + 0x1e))
        {
            packet.put_int(
                *reinterpret_cast<int*>(pData + i * 0xc + 0x1e) -
                *reinterpret_cast<int*>(pData + i * 0xc + 0x22));
        }
        else
        {
            packet.put_int(0);
        }
    }
#undef pData
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_NpcLimitBuyItemChangeInfo::~Inter_NpcLimitBuyItemChangeInfo() {}
// ORIG 0x084e6448 T dispatch_sig —— NPC 限购道具变更全网广播。
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemChangeInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NpcLimitBuyItemChangeInfoPktView —— Inter_NpcLimitBuyItemChangeInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NpcLimitBuyItemChangeInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
    int m_e;                        // +0x0e
};
#pragma pack(pop)

int Inter_NpcLimitBuyItemChangeInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NpcLimitBuyItemChangeInfoPktView* pPkt = reinterpret_cast<Inter_NpcLimitBuyItemChangeInfoPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x191);
    packet.put_int(pPkt->m_a);
    packet.put_int(pPkt->m_e);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_NpcLimitBuyItemInfoRenew::~Inter_NpcLimitBuyItemInfoRenew() {}
// ORIG 0x084e6e82 T dispatch_sig —— NPC 限购物品余量刷新：组 0x193 包全服广播。
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemInfoRenew/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_NpcLimitBuyItemInfoRenewPktView —— Inter_NpcLimitBuyItemInfoRenew::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NpcLimitBuyItemInfoRenewPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=127（ORIG 0x084e6e82）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=77（ORIG 0x84e6e82）：N22 cmp 操作数方向+P30/N4 add-vs-lea+P264 .o 重定位/立即数。
int Inter_NpcLimitBuyItemInfoRenew::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NpcLimitBuyItemInfoRenewPktView* pPkt = reinterpret_cast<Inter_NpcLimitBuyItemInfoRenewPktView*>(data);
    const int itemCount = pPkt->m_a;   // msg+0x0a 条目数

    PacketGuard packet;
    packet.put_header(0, 0x193);
    packet.put_int(itemCount);
    for (int i = 0; i < itemCount && i < 0x1e; ++i)   // asm 双条件（jle/jg 归并）
    {
        packet.put_int(*reinterpret_cast<int*>(data + i * 0xc + 0xe));
        if (*reinterpret_cast<int*>(data + i * 0xc + 0x12) == 0)
        {
            packet.put_int(0);
        }
        else if (*reinterpret_cast<unsigned int*>(data + i * 0xc + 0x16) <
                 *reinterpret_cast<unsigned int*>(data + i * 0xc + 0x12))   // jb 无符号比较
        {
            packet.put_int(
                *reinterpret_cast<int*>(data + i * 0xc + 0x12) -
                *reinterpret_cast<int*>(data + i * 0xc + 0x16));
        }
        else
        {
            packet.put_int(0);
        }
    }
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_MonitorReplyCharacInfo::~Inter_MonitorReplyCharacInfo() {}
// ORIG 0x084e7024 T dispatch_sig —— 监控服角色信息查询应答：组 0x1d6 包回发。
// 报告 docs/class_func_reports/Inter_MonitorReplyCharacInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_MonitorReplyCharacInfoPktView —— Inter_MonitorReplyCharacInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_MonitorReplyCharacInfoPktView
{
    char m_pad0[0xa];  // 未消费
    int m_a;                  // +0x0a
    char m_pade[0x4];  // 未消费
    int m_12;                 // +0x12
    unsigned char m_16;       // +0x16
    char m_17[1];             // +0x17 串/字节区首元素（容量未建模）
    char m_pad18[0x1d];  // 未消费
    short m_35;               // +0x35
    signed char m_37;         // +0x37
    signed char m_38;         // +0x38
};
#pragma pack(pop)

int Inter_MonitorReplyCharacInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_MonitorReplyCharacInfoPktView* pPkt = reinterpret_cast<Inter_MonitorReplyCharacInfoPktView*>(data);
    unsigned short uniqueId = 0;                     // asm -0x12(%ebp) movw $0
    CUser* target = G_CGameManager()->getUser(
        pPkt->m_12, 0xa);  // msg+0x12 characNo
    if (target != 0)
        uniqueId = target->get_unique_id();          // 返回值取 ax

    PacketGuard packet;
    packet.put_header(1, 0x1d6);
    if (pPkt->m_a == 0)    // msg+0x0a 成功分支
    {
        packet.put_byte(1);
        packet.put_byte(                             // msg+0x16（movzbl 零扩展）
            static_cast<int>(pPkt->m_16));
        packet.put_int(static_cast<unsigned int>(uniqueId));
        const int nameLen =                          // msg+0x17 角色名
            static_cast<int>(strlen(pPkt->m_17));
        packet.put_int(nameLen);
        packet.put_str(pPkt->m_17, nameLen);
        packet.put_byte(                             // msg+0x35 short（cwtl 符号扩展）
            static_cast<int>(pPkt->m_35));
        packet.put_byte(                             // msg+0x37 char（movsbl）
            static_cast<int>(pPkt->m_37));
        packet.put_byte(                             // msg+0x38 char（movsbl）
            static_cast<int>(pPkt->m_38));
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_a);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_CeraShopPurchaseCount::~Inter_CeraShopPurchaseCount() {}
// ORIG 0x084e7bfe T dispatch_sig —— 塞拉商城购买加成道具处理。
// 报告 docs/class_func_reports/Inter_CeraShopPurchaseCount/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CeraShopPurchaseCountPktView —— Inter_CeraShopPurchaseCount::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_CeraShopPurchaseCountPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned long m_4;              // +0x04
    unsigned short m_8;             // +0x08
    char m_pada[0x2];  // +0xa..未消费
    int m_c;                        // +0x0c
};
#pragma pack(pop)

int Inter_CeraShopPurchaseCount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_CeraShopPurchaseCountPktView* pPkt = reinterpret_cast<Inter_CeraShopPurchaseCountPktView*>(data);
    if (pPkt == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6a96, 5)("[PURCAHSE BONUS] internal data is null.");
        return 0x6a97;
    }
    GlobalData::s_pItemVendingMachine->ProcessPurcahseBonusItem(
        user,
        pPkt->m_4,
        pPkt->m_c,
        pPkt->m_8);
    return 0;
}

Inter_OwnerisVIP::~Inter_OwnerisVIP() {}
// ORIG 0x084e7c90 T dispatch_sig —— 点数拍卖 VIP 购买：计费/组包/预付登记。
// 报告 docs/class_func_reports/Inter_OwnerisVIP/dispatch_sig.md 全文转写。
// 实参口径（Ghidra 同证）：第二槽=CUser*，第三槽=int 位置实际传 msg 指针。
// ORIG PCK_AUCTION_BIDDING_GP（nsl::PACKET_HEADER 派生 pack(1)，
// C1/C2 W 0x084e9a98 sz0x67）。ctor 反汇编：nsl::PACKET_HEADER(this)；
// memset(this, 0, 0x33)；setCategory(0x12)；setPacketID(5)；setSize(0x33)
// —— 与 PACKET_CTOR_BODY(0x12, 5, 0x33) 宏语义逐指令一致。
struct PCK_AUCTION_BIDDING_GP : public nsl::PACKET_HEADER
{
    PCK_AUCTION_BIDDING_GP() { PACKET_CTOR_BODY(0x12, 5, 0x33); }
};

#pragma pack(push, 1)
// Inter_OwnerisVIPPktView —— Inter_OwnerisVIP::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_OwnerisVIPPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned int m_4;               // +0x04
    unsigned int m_8;               // +0x08
    bool m_c;                       // +0x0c
    int m_d;                        // +0x0d
    char m_pad11[0x11];  // +0x11..未消费
    unsigned int m_22;              // +0x22
    int m_26;                       // +0x26
};
#pragma pack(pop)

#pragma pack(push, 1)
// Inter_OwnerisVIPMsgWinView —— Inter_OwnerisVIP::dispatch_sig 窗口 msg 元素视图
//   （msg 基址表达式见声明行注释；字段宽度=本 TU 原 cast 类型，按 ORIG 报告汇编核实）。
struct Inter_OwnerisVIPMsgWinView
{
    char m_pad0[0x4];  // 未消费
    unsigned long long m_4;   // +0x04
};
#pragma pack(pop)

// Inter_OwnerisVIPTailView —— needInfo+0xc..+0x27（0x1c）尾部块视图：ORIG 用 7 条 movl
//   逐 dword 赋值（无 memcpy call、无 rep movsl，p167 实证）⇒ 结构整体赋值形（N84/P83）。
struct Inter_OwnerisVIPTailView
{
    unsigned int m_0, m_4, m_8, m_c, m_10, m_14, m_18;
};

// [b28d-lane2] verdict=DIFF ndiff=136（ORIG 0x084e7c90）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84e7c90）：本轮 memcpy→0x1c 结构赋值（N84/P83，p167 CLEAR）；余 N82/P112 帧槽平移+P30/N4。
int Inter_OwnerisVIP::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [b28d-lane2] ORIG 084e7ca4 段只有一个 data 派生槽（-0x14，`cmp $0,-0x14(%ebp)` 判空
    // + 每处访问 `mov -0x14(%ebp),%eax` 重载）⇒ 原码单具名 msg，其余视图逐点内联
    // （P183/P0：旧 pPkt/pWin_msg 双缓存各占 4B 并多发 8 条 lea/6 条 mov）。
    char* msg = data;                                      // -0x14（ORIG cmpl $0,-0x14 检查 data）
    if (msg == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x6aa6, 5)("[Check point auction error] internal data is null.");
        return 0x6aa7;
    }

    CUser* target = user;                                  // -0x10
    const int price =                                      // -0xc（ORIG 先物化 price 再填 needInfo）
        reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_22;
    ItemVendingMachine::AuctionNeedInfo needInfo;          // -0x4c（0x28）
    needInfo.m_price = static_cast<unsigned int>(price);
    needInfo.m_itemUid = reinterpret_cast<Inter_OwnerisVIPMsgWinView*>(msg)->m_4;   // lo/hi

    if (!GlobalData::s_pItemVendingMachine->BuyAuctionItem(
            target, needInfo,
            reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_26,           // +0x26
            reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_c,          // +0x0c
            reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_d))          // +0x0d
    {
        target->SetBuyingGold(false);
        target->SendCmdErrorPacket((ENUM_CMDPACKET)0xbc, 0x93, (PAY_TYPE)1);
        return 0;
    }

    PCK_AUCTION_BIDDING_GP pck;                            // -0xa7（0x33，ctor W 0x084e9a98）
    // ORIG：GetUID/getCurCharacNo 各调用一次，结果仅存栈（-0x95/-0x91）未使用——
    // 保留死调用以对齐 ORIG 指令序列（pck 除 characName 外保持 ctor 清零）。
    (void)target->GetUID();
    (void)target->getCurCharacNo();
    strncpy(reinterpret_cast<char*>(&pck) + 0x1a,
            reinterpret_cast<const char*>(target->getCurCharacR()) + 4, 0xd);

    PrePayInfo prePay;                                     // -0x74（0x28 按值）
    *reinterpret_cast<unsigned int*>(prePay.m_data + 0x00) =
        reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_4;
    *reinterpret_cast<unsigned int*>(prePay.m_data + 0x04) =
        reinterpret_cast<Inter_OwnerisVIPPktView*>(msg)->m_8;
    *reinterpret_cast<unsigned int*>(prePay.m_data + 0x08) =
        static_cast<unsigned int>(price);
    *reinterpret_cast<Inter_OwnerisVIPTailView*>(prePay.m_data + 0x0c) =
        *reinterpret_cast<const Inter_OwnerisVIPTailView*>(
            reinterpret_cast<const char*>(&needInfo) + 0x0c);      // needInfo+0xc..+0x27

    const unsigned int characNo = target->getCurCharacNo();
    G_CPrePayInfoMgr()->InsertPrePayInfo(static_cast<int>(characNo), prePay);

    GlobalData::s_cera_auction_proxy->SendPacket(
        reinterpret_cast<nsl::PACKET_HEADER*>(&pck));
    return 0;
}

Inter_LoadPuUserCheck::~Inter_LoadPuUserCheck() {}
// [b28d-lane2] verdict=IDENTICAL ndiff=0（ORIG 0x084e7fe2）：混合残差；gap0。
int Inter_LoadPuUserCheck::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    char* packet = data;
    // [b28d-lane2] 源侧 uchar 读旋钮（探针 /tmp/l2probe/p9.cpp v3/v4/v5 实证）：
    // ORIG 0x084e7ff1 单步 `movzbl (%eax),%edx` 只在「源表达式为 unsigned char 左值」
    // 时复现；`*packet`（char 源）4.4 恒产两段 `movzbl (%eax),%eax; mov %eax,%edx`。
    // 字段仍为 uchar（CUser.h 冻结），零 ABI/布局影响。
    user->m_field796d8 = *reinterpret_cast<unsigned char*>(packet);
    return 0;
}

Inter_CollectItemsResult::~Inter_CollectItemsResult() {}
// ORIG 0x084e770c T dispatch_sig —— 收集物品进度→时间门等级。
// 报告 docs/class_func_reports/Inter_CollectItemsResult/dispatch_sig.md 全文转写。
int Inter_CollectItemsResult::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
#pragma pack(push, 1)
    struct CollectItemsResultView
    {
        char m_pad[0xa];
        int m_total;        // +0xa
        int m_fieldE;       // +0xe
    };
#pragma pack(pop)
    CollectItemsResultView* p = reinterpret_cast<CollectItemsResultView*>(data);
    if (p->m_total != 0)
    {
        int level = (p->m_fieldE * 100) / p->m_total;
        if (100 < level)
            level = 100;

        GlobalData::s_timeGate_->SetLevel(level);

        PacketGuard packet;
        packet.put_header(0, 0x1a6);
        packet.put_byte(level);
        packet.finalize(true);

        G_GameWorld()->send_to_area(
            GlobalData::s_timeGate_->getVillageIndex(),
            GlobalData::s_timeGate_->getAreaIndex(),
            packet);
    }
    return 0;
}

Inter_CollectItemsReward::~Inter_CollectItemsReward() {}
// ORIG 0x084e71ee T dispatch_sig —— 收集品奖励邮件：result 分支选道具 + 脚本文案 +
// CItem::make_item 生成物品经系统邮件发放。
// 报告 docs/class_func_reports/Inter_CollectItemsReward/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CollectItemsRewardPktView —— Inter_CollectItemsReward::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_CollectItemsRewardPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=150（ORIG 0x084e71ee）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+2。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=66（ORIG 0x84e71ee）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
int Inter_CollectItemsReward::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_CollectItemsRewardPktView* pPkt = reinterpret_cast<Inter_CollectItemsRewardPktView*>(data);
    if (user->getCurCharacR() == 0)
        return 0;

    Inven_Item item;                                   // ORIG Inven_Item::Inven_Item()
    // ORIG 0x084e7212 `movl $0x0,-0x1c(%ebp)` 落于 msgBuf 清零（lea -0x18b/mov $0x100,%ebx）
    // **之前** ⇒ 原码在缓冲前有一枚「初始化式 = 0」的具名 int 局部（P198① 预清必写；4.4 对
    // 未读但带初始化式的局部仍发 store）。语义未被 ORIG 读取，照抄 P171。
    // 残：槽位（ORIG itemCount -0x1c 浅于 pPkt -0x20；4.4 先声明深 ⇒ 本序给我方 -0x24/-0x20）
    //   = N82/P212 槽族；两次声明位置实验（前置于 pPkt：pPkt -0x20 达阵但 store 提前到函数首）
    //   已留痕，取 store 位置正确形。
    int itemCount = 0;
    char msgBuf[0x100] = { '\0' };                     // P4 内联清零
    char titleBuf[0x1e] = { '\0' };

    switch (pPkt->m_12)
    {
    case 1:                                            // 收集完成
        item.m_addInfo = 0x21d7;
        item.m_addInfo2 = 1;
        OS_API::snprintf(
            msgBuf, 0xff,
            g_scriptStringManager_.findString(4, "game_server_msg_257", 0),
            user->getCurCharacName());
        break;
    case 2:                                            // 兑换完成
        item.m_addInfo = 0x68c8;
        item.m_addInfo2 = 1;
        OS_API::snprintf(
            msgBuf, 0xff,
            g_scriptStringManager_.findString(4, "game_server_msg_259", 0),
            user->getCurCharacName());
        break;
    case 0:                                            // 收集进行中
        item.m_addInfo = 0x21d7;
        item.m_addInfo2 = 1;
        OS_API::snprintf(
            msgBuf, 0xff,
            g_scriptStringManager_.findString(4, "game_server_msg_258", 0),
            user->getCurCharacName());
        break;
    default:                                           // ORIG 保持 item 字段未设置
        break;
    }

    strncpy(titleBuf, g_scriptStringManager_.findString(4, "game_server_msg_256", 0),
            0x1d);

    CItem* found = G_CDataManager()->find_item(item.m_addInfo);
    if (found != 0)
    {   // ORIG vptr+0x08 槽 = CItem::make_item(Inven_Item&)（P5/P62）
        found->make_item(item);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            titleBuf, item, 0, (unsigned int)user->getCurCharacNo(), msgBuf,
            strlen(msgBuf), 0, (ENUM_SERVER_GROUP)user->GetServerGroup(), true, false);
        cMyTrace(__PRETTY_FUNCTION__,
            0x69c0, 0)("[collect items]reward: m_id(), charac_no(), reward()",
              (int)user->get_acc_id(), (int)user->getCurCharacNo(),
              (int)item.m_addInfo);
    }
    return 0;
}

Inter_CollectItemsRewardBroadcast::~Inter_CollectItemsRewardBroadcast() {}
// ORIG 0x084e7598 T dispatch_sig —— 收集活动奖励广播：时间之门等级联动全员通知。
// 报告 docs/class_func_reports/Inter_CollectItemsRewardBroadcast/dispatch_sig.md 全文转写。
// ORIG CUserModifier 最小镜像（vtable 槽0=Modify(CUser*)，无虚析构，同 ORIG 口径）
class CUserModifier
{
public:
    virtual void Modify(CUser* user) = 0;
};

// ORIG CUserResetTimeGateQuest : CUserModifier（C1 W 0x084f0886 / Modify W 0x084f07ec；
// CollectItemsRewardBroadcast 消费；定义属主未落地，U 引用合规——InterWave1 批）
class CUserResetTimeGateQuest : public CUserModifier
{
public:
    CUserResetTimeGateQuest();
    void Modify(CUser* user);
};

// ORIG CUserResetTimeGateQuest::CUserResetTimeGateQuest() W 0x084f0886：
//   CUserModifier() 后重置 vptr（正常 C++ 生成）。
CUserResetTimeGateQuest::CUserResetTimeGateQuest() {}

// ORIG CUserResetTimeGateQuest::Modify(CUser*) W 0x084f07ec：
//   get_state()>2 && getCurCharacR() 时，取 GlobalData::s_timeGate_->getQuestIndex()，
//   若 getCurCharacQuestR()->IsDoingQuest(qi) 则 getCurCharacQuestW()->giveup_quest(qi)。
void CUserResetTimeGateQuest::Modify(CUser* user)
{
    if (user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        register int questIndex = GlobalData::s_timeGate_->getQuestIndex();  // ORIG 0x084f082f: mov %eax,%ebx（callee-saved 跨调用缓存）
        if (user->getCurCharacQuestR()->IsDoingQuest(questIndex))
        {
            register int questIndex2 = GlobalData::s_timeGate_->getQuestIndex();  // ORIG 0x084f0859: 第二次独立 call，同落 %ebx
            user->getCurCharacQuestW()->giveup_quest(questIndex2);
        }
    }
}

#pragma pack(push, 1)
// Inter_CollectItemsRewardBroadcastPktView —— Inter_CollectItemsRewardBroadcast::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_CollectItemsRewardBroadcastPktView
{
    char m_pad0[0x12];  // 未消费
    unsigned char m_12;       // +0x12
    char m_pad13[0x1];  // 未消费
    char m_14[1];             // +0x14 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=79（ORIG 0x084e7598）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=101（ORIG 0x84e7598）：P30/N4 add-vs-lea+P264 .o 重定位/立即数。
int Inter_CollectItemsRewardBroadcast::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_CollectItemsRewardBroadcastPktView* pPkt = reinterpret_cast<Inter_CollectItemsRewardBroadcastPktView*>(data);

    PacketGuard packet;
    packet.put_header(0, 0x56);
    packet.put_byte(0xb);
    packet.put_byte((unsigned int)pPkt->m_12);
    // ORIG +0x60..+0x67：`movzbl 0x12(%eax),%eax`+`movzbl %al,%eax`+`cmp $0x2,%eax`
    // —— 块作用域 register 绑定 %eax 保留二次零扩展（探针 r7 实证；
    //    非 register 或 asm("ebx") 均退化为单条 movzbl/cmp %al）。
    {
    // b28c-l23 P0 清绑：仅去 asm("eax") 绑定，裸 register 保留（值直落、不占槽）。
    int rewardType = pPkt->m_12;
    if (rewardType == 2)
    {
        GlobalData::s_timeGate_->SetLevel(100);
        CUserResetTimeGateQuest modifier;              // ORIG C1 0x084f0886，仅 vptr
        G_GameWorld()->modify_user(&modifier);
        packet.put_short(GlobalData::s_timeGate_->getNpcIndex());
    }
    else
    {
        packet.put_short(0);
    }
    }
    packet.put_int(strlen(pPkt->m_14));
    packet.put_str(pPkt->m_14, strlen(pPkt->m_14));  // ORIG 两次独立 strlen（P48）
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}

Inter_RequestCharacSkillInfo::~Inter_RequestCharacSkillInfo() {}
// ORIG 0x084e7884 T dispatch_sig —— 请求 tag 角色技能：写入技能树编号并重发技能信息。
// 报告 docs/class_func_reports/Inter_RequestCharacSkillInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_RequestCharacSkillInfoPktView —— Inter_RequestCharacSkillInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_RequestCharacSkillInfoPktView
{
    unsigned int m_0;         // +0x00
    char m_pad4[0x198];  // 未消费
    unsigned char m_19c;      // +0x19c
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=94（ORIG 0x084e7884）：槽序/帧差(N82/P212/P221)+混合残差；gap+10。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=40（ORIG 0x084e7884）：本波已消 p167
// MISSING vector<_Charac_info>::operator[] ×2（成员引用绑定使写+读共用一次 op[]、
// 3 处 slot 使用逐点重读）⇒ P167 CLEAR 11=11；残余 N18a/N82 结构差。
int Inter_RequestCharacSkillInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    char* recv = data;                                 // -0x14(%ebp) 缓存消息指针
    if (user == 0)
        return 0;
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    const int tagIdx =
        user->get_charac_idx(reinterpret_cast<Inter_RequestCharacSkillInfoPktView*>(recv)->m_0);
    if (tagIdx == -1)                                  // cmpl $0xffffffff
        return 0;

    // ORIG 0x084e78fa：写 m_field1239 与随后的读共用**同一次** operator[]
    // （`mov %dl,0x1239(%eax); movzbl 0x1239(%eax),%eax`）⇒ 原码是成员引用绑定。
    unsigned char& field1239 = user->m_characList[tagIdx].m_field1239;
    field1239 = reinterpret_cast<Inter_RequestCharacSkillInfoPktView*>(recv)->m_19c;                           // ORIG movzbl 0x19c; mov %dl
    const char skillTreeKind = (char)field1239;        // ORIG movzbl 0x1239(%eax)

    // [b28d-solo-InterDispatcher] P157 逐点重读：ORIG 0x084e78fa/0x084e792c/0x084e7959
    // （+1）每个使用点重新 operator[] 取 _Charac_info（无 slot 指针缓存）⇒ 消除 p167
    // MISSING vector<_Charac_info>::operator[] ×2。SkillSlot 子对象 @ +0x892。
    reinterpret_cast<SkillSlot*>(
        reinterpret_cast<char*>(&user->m_characList[tagIdx]) + 0x892)->set_parent(user);

    _PutSkillInfoToCharac(skillTreeKind,
                          reinterpret_cast<_Mastered_skill*>(recv + 4),
                          reinterpret_cast<SkillSlot*>(
                              reinterpret_cast<char*>(&user->m_characList[tagIdx]) + 0x892));

    reinterpret_cast<SkillSlot*>(
        reinterpret_cast<char*>(&user->m_characList[tagIdx]) + 0x892)
        ->SendSkillInfo(tagIdx,
                        (ENUM_SKILL_TREE_KIND)(int)skillTreeKind);  // movsbl 符号扩展
    reinterpret_cast<SkillSlot*>(
        reinterpret_cast<char*>(&user->m_characList[tagIdx]) + 0x892)
        ->SetLoadStrikerSkill(true);
    return 0;
}

Inter_RequestTagCharacInfo::~Inter_RequestTagCharacInfo() {}
// ORIG 0x084e79f2 T dispatch_sig —— 切换 tag 角色：设置 tag 状态与全部子对象 parent 后广播。
// 报告 docs/class_func_reports/Inter_RequestTagCharacInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_RequestTagCharacInfoPktView —— Inter_RequestTagCharacInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_RequestTagCharacInfoPktView
{
    unsigned int m_0;         // +0x00
    unsigned char m_4;        // +0x04
};
#pragma pack(pop)

int Inter_RequestTagCharacInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x084e79f8 入口仅单槽 -0x14=recv（pPkt 经 cast 内联复用同槽，技巧36）。
    char* recv = data;                                 // -0x14(%ebp) 缓存消息指针
    if (user == 0)
        return 0;
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    const int tagIdx =
        user->get_charac_idx(
            reinterpret_cast<Inter_RequestTagCharacInfoPktView*>(recv)->m_0);
    if (tagIdx == -1)                                  // cmpl $0xffffffff
        return 0;

    // ORIG 0x084e7a67/0x084e7a76：operator[] 结果免槽直传（技巧37）
    user->setTagCharac(&user->m_characList[tagIdx]);   // vector @ +0x796e8

    user->getTagCharacInvenR()->set_parent(user);      // CInventory::set_parent
    user->getTagCharacSkillR()->set_parent(user);      // SkillSlot::set_parent

    user->setTagCharacInfo(tagIdx,
                           reinterpret_cast<Inter_RequestTagCharacInfoPktView*>(recv)->m_0,
                           reinterpret_cast<TAG_CHARAC_DATA*>(recv + 5));
    user->setTagCharacInven(reinterpret_cast<TAG_CHARAC_INVEN_DATA*>(recv + 0x7d));

    CQuestShop* questShop = reinterpret_cast<CQuestShop*>(
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb));   // 0xb 槽即 QuestShop（ORIG 指针透传）
    questShop->SetTagCharacQuestShopData(
        reinterpret_cast<CHARAC_QUEST_SHOP_DATA*>(recv + 0x40a1));

    user->setCurCharacTagCharacNo(
        reinterpret_cast<Inter_RequestTagCharacInfoPktView*>(recv)->m_0);
    user->setCurCharacStrikerSkillIndex(
        reinterpret_cast<Inter_RequestTagCharacInfoPktView*>(recv)->m_4);

    user->m_characterView.enableSaveCharacView();      // +0x796f4

    // ORIG 0x084e7b6b `movb $0x1,0x14bc(%eax)`：operator[] 重调结果具名字段直折 disp。
    user->m_characList[tagIdx].m_field14BC = 1;

    user->SendTagCharacInfo();
    return 0;
}

Inter_EmergencyPatchMessage::~Inter_EmergencyPatchMessage() {}
// ORIG 0x084e7b84 T dispatch_sig —— 紧急补丁公告（禁止新连接+广播 10 分钟关服）。
// 报告 docs/class_func_reports/Inter_EmergencyPatchMessage/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_EmergencyPatchMessagePktView —— Inter_EmergencyPatchMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_EmergencyPatchMessagePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
};
#pragma pack(pop)

int Inter_EmergencyPatchMessage::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_EmergencyPatchMessagePktView* pPkt = reinterpret_cast<Inter_EmergencyPatchMessagePktView*>(data);
    // ORIG +0x1b：`test %al,%al`+`je` 无 setne 物化 ⇒ 直接真值测试（`!= 0` 会插入物化两行）。
    if (GameserverDemonManagerInstance()->IsProhibitConnectState())
        return 0;
    GameserverDemonManagerInstance()->SetUserProhibitFlag(true);
    // ORIG +0x3b：串基址取包基址槽 -0xc(%ebp)（经具名 pPkt，不回落参数槽 0x10）。
    GameserverDemonManagerInstance()->SetNoticeMessage(
        pPkt->m_a, reinterpret_cast<char*>(pPkt) + 0xb);
    GameserverDemonManagerInstance()->Notice10MinShutDownMessage();
    return 0;
}

Inter_AuctionSubMoney::~Inter_AuctionSubMoney() {}
// ORIG 0x084e8004 T dispatch_sig —— 拍卖扣款异常登记服务器 hack 计数。
// 报告 docs/class_func_reports/Inter_AuctionSubMoney/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_AuctionSubMoneyPktView —— Inter_AuctionSubMoney::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_AuctionSubMoneyPktView
{
    unsigned int m_0;               // +0x00
    // ORIG +0x14/+0x17 两步装载（`mov 0x4(%eax),%eax`+`mov %eax,%esi`）而 +0x1f 的 m_0
    // 一步直落 %ebx ⇒ 本字段为 signed int（→addServerHackCnt(unsigned) 形参失配）。
    int m_4;                        // +0x04
};
#pragma pack(pop)

int Inter_AuctionSubMoney::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AuctionSubMoneyPktView* pPkt = reinterpret_cast<Inter_AuctionSubMoneyPktView*>(data);
    if (user != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x2d3, 1,
            pPkt->m_0,
            pPkt->m_4);
    }
    return 0;
}

Inter_UpdateServerSnapShot::~Inter_UpdateServerSnapShot() {}
int Inter_UpdateServerSnapShot::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    G_GameWorld()->UpdateServerSnapShot();
    return 0;
}

Inter_LoadCharacTypeHotKey::~Inter_LoadCharacTypeHotKey() {}
// ORIG 0x084e87de T dispatch_sig —— DB 热键配置装载。
// 报告 docs/class_func_reports/Inter_LoadCharacTypeHotKey/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadCharacTypeHotKeyPktView —— Inter_LoadCharacTypeHotKey::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadCharacTypeHotKeyPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned char m_4;              // +0x04
};
#pragma pack(pop)

int Inter_LoadCharacTypeHotKey::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 1)
        return 0;

    // ORIG 0x084e87f9：pPkt 在 state 岛后物化 -0x10（技巧13）。
    Inter_LoadCharacTypeHotKeyPktView* pPkt = reinterpret_cast<Inter_LoadCharacTypeHotKeyPktView*>(data);
    // ORIG 0x084e8814 `mov %eax,-0xc`：gopt 具名槽；m_4 两处独立 reload（无 slot 槽）。
    CGameOption* gopt = user->GetGameOptionRef();
    gopt->SetHotKeyOption(
        (unsigned int)pPkt->m_4,
        *reinterpret_cast<CHotkeyOption*>(reinterpret_cast<char*>(pPkt) + 5));

    PacketGuard packet;
    packet.put_header(0, 0x1c4);
    packet.put_byte((unsigned int)pPkt->m_4);
    packet.put_int(0xbc);
    packet.put_binary(reinterpret_cast<char*>(pPkt) + 5, 0xbc);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_UpdateMiniCraneSeed::~Inter_UpdateMiniCraneSeed() {}
#pragma pack(push, 1)
// Inter_UpdateMiniCraneSeedPktView —— Inter_UpdateMiniCraneSeed::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_UpdateMiniCraneSeedPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

int Inter_UpdateMiniCraneSeed::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_UpdateMiniCraneSeedPktView* pPkt = reinterpret_cast<Inter_UpdateMiniCraneSeedPktView*>(data);
    // ORIG +0x11 一步 `mov 0xa(%eax),%ebx`（同 signedness 直落 callee-saved）⇒ register 局部；
    // 且无多余 char* 局部（否则 pPkt 槽被挤到 -0x14/-0x10 两级）。
    register int seed = pPkt->m_a;
    G_GameWorld()->ResetCraneItemNeedMaterial(seed);
    return 0;
}

Inter_GetPremiumSelectedCubeInfo::~Inter_GetPremiumSelectedCubeInfo() {}
// ORIG 0x084e8936 T dispatch_sig —— 高级方块合约信息初始化。
// 报告 docs/class_func_reports/Inter_GetPremiumSelectedCubeInfo/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_GetPremiumSelectedCubeInfoPktView —— Inter_GetPremiumSelectedCubeInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_GetPremiumSelectedCubeInfoPktView
{
    char m_pad0[0x4];  // 未消费
    char m_4;                 // +0x04
    unsigned char m_5;        // +0x05
};
#pragma pack(pop)

int Inter_GetPremiumSelectedCubeInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GetPremiumSelectedCubeInfoPktView* pPkt = reinterpret_cast<Inter_GetPremiumSelectedCubeInfoPktView*>(data);
    if (user->getCurCharacR() == 0)
        return 0;

    user->initContractOfCubePremiumInfo(
        pPkt->m_4 != '\0',
        pPkt->m_5);

    PacketGuard packet;
    packet.put_header(0, 0x217);
    packet.put_byte((unsigned int)(unsigned char)pPkt->m_4);
    packet.put_byte((unsigned int)pPkt->m_5);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}

Inter_CheckAddFriend::~Inter_CheckAddFriend() {}
// ORIG 0x084e8a42 T dispatch_sig —— 添加好友检查：空指针告警或转发监视服添加请求。
// 报告 docs/class_func_reports/Inter_CheckAddFriend/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_CheckAddFriendPktView —— Inter_CheckAddFriend::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_CheckAddFriendPktView
{
    int m_0;                  // +0x00
    char m_pad4[0x1];  // 未消费
    char m_5[1];              // +0x5 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=40（ORIG 0x084e8a42）：槽序/帧差(N82/P212/P221)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=49（ORIG 0x84e8a42）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_CheckAddFriend::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_CheckAddFriendPktView* pPkt = reinterpret_cast<Inter_CheckAddFriendPktView*>(data);
    if (user == 0 || data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6d7b, 5)("[%s] pUser or p is null.",
              __PRETTY_FUNCTION__);
        return 0x6d7c;
    }

    char* recv = data;                                 // -0x10(%ebp) 缓存消息指针
    if (recv == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6d82, 5)("[%s] internal data is null.",
              __PRETTY_FUNCTION__);
        return 0x6d83;
    }

    if (pPkt->m_0 == 0)
    {
        Packet_Monitor_Add_Buddy packet;
        packet.m_charNo = user->getCurCharacNo();      // +0x0a
        packet.m_idByChannel = user->GetUID();         // +0x0e
        memcpy(packet.m_name, pPkt->m_5, strlen(pPkt->m_5));
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&packet),
                          packet.packetSize);          // +0x02 长度字段
    }
    else
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x3f,
            (unsigned char)pPkt->m_0);
    }
    return 0;
}

Inter_LoadCerashopAddRestrictType::~Inter_LoadCerashopAddRestrictType() {}
// ORIG 0x080e2104 T dispatch_sig —— DB 限购类型数据装载。
// 报告 docs/class_func_reports/Inter_LoadCerashopAddRestrictType/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadCerashopAddRestrictTypePktView —— Inter_LoadCerashopAddRestrictType::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadCerashopAddRestrictTypePktView
{
    char m_pad0[0x4];  // +0x0..未消费
    int m_4;                        // +0x04
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=43（ORIG 0x080e2104）：槽序/帧差(N82/P212/P221)+混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=44（ORIG 0x80e2104）：P30/N4 add-vs-lea。
int Inter_LoadCerashopAddRestrictType::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x200, 5)("[%s] pUser or p is null.",
              __PRETTY_FUNCTION__);
        return 0x201;
    }

    Inter_LoadCerashopAddRestrictTypePktView* pPkt = reinterpret_cast<Inter_LoadCerashopAddRestrictTypePktView*>(data);

    if (data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x207, 5)("[%s] internal data is null.",
              __PRETTY_FUNCTION__);
        return 0x208;
    }

    CerashopAddRestrict::UserInfo& info = user->m_ceraUserInfo;
    const int type = pPkt->m_4;
    if (type == 0)
    {
        CerashopAddRestrict::Manager::LoadProcess(
            info.m_field30, info.m_accountDaily,
            reinterpret_cast<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE*>(data));
    }
    else if (type == 1)
    {
        CerashopAddRestrict::Manager::LoadProcess(
            info.m_field48, info.m_characDaily,
            reinterpret_cast<SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE*>(data));
    }
    CerashopAddRestrict::Manager::SendSyncPacket(user);
    return 0;
}

// ORIG 0x084e8ce4 T dispatch_sig —— 从 monitor 装载角色升级支援事件状态（m_levelupEvents 同步 + DB 上报）。
// 报告 docs/class_func_reports/Inter_LoadEventLevelupSupport/dispatch_sig.md 全文转写。
#pragma pack(push, 1)
// Inter_LoadEventLevelupSupportPktView —— Inter_LoadEventLevelupSupport::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadEventLevelupSupportPktView
{
    int m_0;                  // +0x00
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=167（ORIG 0x084e8ce4）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-7。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=93（ORIG 0x84e8ce4）：N22 cmp 操作数方向+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_LoadEventLevelupSupport::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_LoadEventLevelupSupportPktView* pPkt = reinterpret_cast<Inter_LoadEventLevelupSupportPktView*>(data);
    if (user == 0 || data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6dee, 5)("[%s] pUser or p is null.",
              __PRETTY_FUNCTION__);
        return 0x6def;
    }

    int* const local_2c =&pPkt->m_0;

    user->m_levelupEvents.clear();
    user->m_levelupEvents =
        ARAD::Singleton<LevelupSupportEventManger>::Get()->GetDefaultEventInfo();

    for (unsigned int local_28 = 0;
         local_28 < user->m_levelupEvents.size();
         ++local_28)
    {
        if (user->m_levelupEvents[local_28].m_level <=
            (unsigned int)user->get_charac_level())
        {
            user->m_levelupEvents[local_28].m_state = 1;
        }
    }

    for (int local_24 = 0; local_24 < *local_2c; ++local_24)
    {
        LevelupSupportEventManger::EventInfo& info =
            user->m_levelupEvents[local_24];
        if (info.m_level == (short)local_2c[local_24 + 1])
        {
            info.m_state =
                *reinterpret_cast<unsigned short*>(
                    reinterpret_cast<char*>(local_2c) + local_24 * 4 + 6);
        }
        else
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x6e06,
                "Inter_LoadEventLevelupSupport diff state charac_no:%d",
                user->getCurCharacNo());
        }
    }

    if (*local_2c == 0)
    {
        for (unsigned int local_20 = 0;
             local_20 < user->m_levelupEvents.size();
             ++local_20)
        {
            const unsigned short state = user->m_levelupEvents[local_20].m_state;
            const unsigned short level = user->m_levelupEvents[local_20].m_level;
            DB_SaveEventLevelupSupport::makeRequest(
                static_cast<int>(user->GetUID()),
                static_cast<int>(user->getCurCharacNo()),
                static_cast<int>(level), static_cast<int>(state));
        }
    }

    LevelupSupportEventManger::SendCharacState(user, 0);
    return 0;
}

// ORIG 0x084e8f68 T dispatch_sig —— 允许代理用户选择失败断开。
// 报告 docs/class_func_reports/Inter_SelectAllowProxyUser/dispatch_sig.md 全文转写。
int Inter_SelectAllowProxyUser::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || data == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6e1e, 5)("[%s] pUser or p is null.",
              __PRETTY_FUNCTION__);
        return 0x6e1f;
    }
    user->SendCmdErrorPacket((ENUM_CMDPACKET)2, 7);
    user->DisConnSig((DISCONN_SIG)0x1e, true, 0);
    return 0;
}


// ======== Inter_NpcLimitBuyItemInfo（B1：生命周期/reset；业务方法待类型域补齐） ========
// [b22判词·N-EH] Inter_NpcLimitBuyItemInfo::C1Ev（ORIG 0x084e6502，报告汇编+Ghidra C 全读）
// 主体逐指令同构 ORIG：ISigDispatcher::C1 → 置 vptr(&vtable+0x8) → pair<int,int>@+8::C1
// → Inven_Item@+0x1d::C1 → Inven_Item@+0x5a::C1 → this->reset()。orig25/our40 全差在
// 4.4.7 对 4 个可能抛的 sub-ctor 生成 EH cleanup 块（push %esi/%ebx + `jmp <landing>` +
// ~C1 pair + _Unwind_Resume，15 条），ORIG 编译器同位置无任何 landing pad。函数体不可
// 复现（编译器版本决策），语义等价。
// [b28c-32] verdict=AE：P216 reset() throw() 消 ~ISigDispatcher cleanup（40→25=ORIG）。
Inter_NpcLimitBuyItemInfo::Inter_NpcLimitBuyItemInfo()
{
    reset();
}

Inter_NpcLimitBuyItemInfo::~Inter_NpcLimitBuyItemInfo()
{
}

void Inter_NpcLimitBuyItemInfo::reset() throw()
{
    m_error = 0;
    m_needItem.first = 0;
    m_needItem.second = 0;
    m_param1 = -1;
    m_param2 = -1;
    m_itemRef = 0;
    m_data1c = 0;
    m_invenItem1.reset();
    m_invenItem2.reset();
}

// ORIG 0x084e6acc T _ZN25Inter_NpcLimitBuyItemInfo10checkErrorEP5CUserP19LimitNpcBuyItemInfo
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemInfo/checkError.md：
// 用户/状态/角色号校验→find_item→GetNeedMaterial 填 m_needItem→
// GetInvenData(this+0x5a)→add_info/计数比较，返回错误码。
// [b28d-lane2] verdict=DIFF ndiff=96（ORIG 0x084e6acc）：寄存器中转(N18a/N7)+混合残差；gap-4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=64（ORIG 0x84e6acc）：N22 cmp 操作数方向+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_NpcLimitBuyItemInfo::checkError(CUser* user, LimitNpcBuyItemInfo* info)
{
    if (user == 0)
        return 3;
    if (user->get_state() <= 2)
        return 3;
    if (user->get_charac_no(-1) !=
        *reinterpret_cast<int*>(reinterpret_cast<char*>(info) + 0x12))
        return 3;

    m_error = *reinterpret_cast<int*>(reinterpret_cast<char*>(info) + 0x16);
    if (m_error == 0)
        return 0x11;

    m_itemRef =
        reinterpret_cast<int>(G_CDataManager()->find_item(m_error));
    if (m_itemRef == 0)
        return 0x11;

    const CItem* mat = reinterpret_cast<CItem*>(m_itemRef)->GetNeedMaterial();
    m_needItem.first  = mat->m_index;    // GetNeedMaterial().first（CItem* 首 int）
    m_needItem.second = mat->m_grade;    // GetNeedMaterial().second
    if (m_needItem.first == 0 || m_needItem.first == -1)
        return 0x17;

    const CInventory* invenR = user->getCurCharacInvenRefR();
    m_param2 = invenR->GetInvenData(m_needItem.first, m_invenItem2);
    if (m_param2 < 0)
        return 0x15;
    if (m_needItem.second < 1 ||
        m_invenItem2.get_add_info() < m_needItem.second)
        return 0x16;
    return 0;
}

// ORIG 0x084e65a2 T dispatch_sig —— NPC 限量购买下单：定位用户、校验后生成物品
// 入背包（失败走系统邮件兜底），扣减源物品并回执 monitor。
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemInfo/dispatch_sig.md 全文转写。
// LimitNpcBuyItemUpdate NEEDS_DECL（PacketHeader(0x27dd,0x1a) 派生，见 meta）。
#pragma pack(push, 1)
// Inter_NpcLimitBuyItemInfoPktView —— Inter_NpcLimitBuyItemInfo::dispatch_sig 包视图（偏移/命名与
//   Monitor/LimitNpcBuyItem.h 权威类 LimitNpcBuyItemInfo 逐字段一致；该头不在本 TU 的 -I 路径内，
//   故本 TU 以同布局镜像视图承载，字段表见下方逐项 ORIG 证据）。
//   ORIG 084e65a2 逐访问：+0x0a `mov 0xa(%eax),%ebx`（getUser 第1参）/+0x12 `mov 0x12(%eax)`/03
//   +0x16 `mov 0x16(%eax)`/+0x1a `mov 0x1a(%eax)`/+0x22 `mov 0x22(%eax); test`（全字判 + movzbl %al 传参）。
struct Inter_NpcLimitBuyItemInfoPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_itemIdReq;       // +0x0a（权类同名）
    unsigned int m_dbid;            // +0x0e（本函数未消费）
    unsigned int m_charNo;          // +0x12
    unsigned int m_itemId;          // +0x16
    unsigned int m_count;           // +0x1a
    unsigned int m_pad1e;           // +0x1e（本函数未消费）
    int m_errorNo;                  // +0x22
};
#pragma pack(pop)

// [b28d-lane2] verdict=DIFF ndiff=302（ORIG 0x084e65a2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-9。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=189（ORIG 0x84e65a2）：N82/P112 帧槽平移+P30/N4 add-vs-lea+P264 .o 重定位/立即数。
int Inter_NpcLimitBuyItemInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    reset();                                           // ORIG 0x084e6e1a（已实现）

    // ORIG 单指针局部（-0xc）：包视图与 checkError 实参共用同一槽；声明在 reset() 之后
    // （084e65b8 才 `mov %eax,-0xc(%ebp)`）。
    Inter_NpcLimitBuyItemInfoPktView* msg =
        reinterpret_cast<Inter_NpcLimitBuyItemInfoPktView*>(data);
    LimitNpcBuyItemUpdate update;                      // -0x2e（0x1a 包）

    CUser* target =                                    // +0xc
        G_CGameManager()->getUser(
            msg->m_itemIdReq, 10);

    if (target == 0)
    {   // 用户不在线：按本类缓存字段回 monitor（ORIG GetServerGroup(NULL) 原样保留）
        update.m_errorNo     = 3;             // +0x16（084e65f1 movl $0x3,-0x18）
        update.m_itemId      = msg->m_itemId; // +0x0a（084e6604）
        update.m_charNo      = msg->m_charNo; // +0x0e（084e6601）
        update.m_cancelCount = msg->m_count;  // +0x12（084e660a）
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr
            ->GetServerProxy(user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&update), 0x1a);
        return 0;
    }

    // ORIG 084e6654/084e665e：0x22 按 32 位读（mov 0x22(%eax)），
    // test 判全字；实参仅取低字节（movzbl %al）传 SendCmdErrorPacket。
    if (msg->m_errorNo != 0)   // 请求自带错误码
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1d1,
                                 (unsigned char)msg->m_errorNo);
        return 0;
    }

    const int err = checkError(
        user, reinterpret_cast<LimitNpcBuyItemInfo*>(msg));   // -0x18（ORIG 返回 int）
    if (err != 0)
    {
        update.m_itemId      = m_error;                // +0x0a（084e66af this+0x4）
        update.m_charNo      = (unsigned int)user->get_charac_no(-1);  // +0x0e
        update.m_cancelCount = msg->m_count;           // +0x12（084e66ce）
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr
            ->GetServerProxy(user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&update), 0x1a);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1d1, (unsigned char)err);
        return 0;
    }

    m_invenItem1.m_addInfo = m_error;                  // +0x1f = +0x04（item idx）
    G_CDataManager()->m_itemList->create_item(
        (ENUM_ITEM_CREATE_TYPE)0, m_invenItem1, 1);

    m_param1 = user->getCurCharacInvenW()->insertItemIntoInventory(   // +0x10
        m_invenItem1, (eItemAddReason)0, true, false);
    m_data1c = 0;                                      // +0x1c

    if (m_param1 < 0)
    {   // 入包失败：系统邮件兜底
        std::string strTitle =                         // -0x14
            g_ServerString_.GetServerString(0x436, 0);
        char title[0x14];                              // -0x43
        strncpy(title, strTitle.c_str(), 0x14);
        std::string strBody =                          // -0x10
            g_ServerString_.GetServerString(0x437, 0);
        char body[0xff];                               // -0x143
        strncpy(body, strBody.c_str(), 0xff);

        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, m_invenItem1, 0,
                (unsigned int)user->get_charac_no(-1), body,
                (int)strlen(body), 0x1e,
                user->GetServerGroup(), false, false) == 0)
        {
            // ORIG 084e6989..084e69d9：0x145 清零后写 -0x288+0x11(m_name11, strncpy 0x14)、
            // -0x288+0x39(m_itemId39=m_addInfo)、-0x288+0x3d(m_extra3d=m_addInfo2)
            // ⇒ 即权威 MSG_MAILBOX_SEND 具名成员（原 NpcLimitMailView 槽位 +0x01/+0x35 为误读）。
            MSG_MAILBOX_SEND mail;
            memset(&mail, 0, 0x145);
            strncpy(mail.m_name11, title, 0x14);
            mail.m_itemId39 = m_invenItem1.m_addInfo;
            mail.m_extra3d = m_invenItem1.m_addInfo2;
            user->m_historyLog.SendMail(               // ORIG (user+0x79700)
                &mail, 0);
        }
        m_data1c = 1;
    }

    user->getCurCharacInvenW()->delete_item(
        (INVEN_TYPE)1, m_param2, m_needItem.first,
        (eItemDelReason)0x1a, true);

    update.m_errorNo     = 0;                          // +0x16（084e6a48）
    update.m_itemId      = m_error;                    // +0x0a（084e6a52 this+0x4）
    update.m_charNo      = (unsigned int)user->get_charac_no(-1);  // +0x0e（084e6a6b）
    update.m_cancelCount = msg->m_count;               // +0x12（084e6a71）
    {
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr
            ->GetServerProxy(user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&update), 0x1a);
    }
    sendPacket(user);
    return 0;
}

// 基类定义故意置于所有派生类实现之后（B1 ABI 形状修复）。
ISigDispatcher::~ISigDispatcher() {}
