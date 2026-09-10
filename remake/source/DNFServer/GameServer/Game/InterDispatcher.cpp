// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 InterDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "InterDispatcher/checkNewMemberHacklog.inc"
#include "CAradRyosikaEvent.h"
// CheckDailyScheduleTime —— 全局自由函数（ORIG T _Z22CheckDailyScheduleTimeill；
// 各调用 TU 按仓库先例以 TU 内 bool 声明引用，定义见某 Booster/Deps TU）。
#include "InterDispatcher/CheckDailyScheduleTime.inc"
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
#include "InterDispatcher/InterDispatcher.inc"
}

// ORIG 0x84be89e：非空槽经虚表[2]（D0）删除后置 0。
#include "InterDispatcher/dtor_InterDispatcher.inc"
}

// ORIG 0x84be906 逐指令转写。
#include "InterDispatcher/get_dispatcher.inc"
}

// ORIG 0x84be95e 逐指令转写。
#include "InterDispatcher/register_dispatcher.inc"
}

// ORIG 0x84be970 逐指令转写（getUser 参数序、DisConnSig 与双路径日志均按汇编）。
// [b28d-lane2] verdict=DIFF ndiff=42（ORIG 0x084be970）：混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=37（ORIG 0x84be970）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
#include "InterDispatcher/dispatch.inc"
}

// ======== 具体分发器生命周期 + dispatch_sig stub ========
// stub 体待后续批次按 docs/class_func_reports 还原；地址=该函数 ORIG 起点。

#include "InterDispatcher/dtor_Inter_Disconnect.inc"
#include "InterDispatcher/dispatch_sig.inc"
}

#include "InterDispatcher/dtor_Inter_SessionClose.inc"
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
#include "InterDispatcher/dispatch_sig_1.inc"
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

#include "InterDispatcher/dispatch_sig_2.inc"
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
                // ORIG 0x084c7464 `call getPoint; cmp $0x251b,%eax; setbe %al`
                // = 无符号 `<= 0x251b`（轴 B 反向：旧 `< 0x251c` 产有符号 setge）。
                if ((unsigned int)user->m_mcap.getPoint() <= 0x251bu)
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

#include "InterDispatcher/dtor_Inter_LoginInfo.inc"

#include "InterDispatcher/dtor_Inter_CreateCharac.inc"
#include "InterDispatcher/dispatch_sig_3.inc"
}

// ORIG T _ZN18Inter_CreateCharac13process_eventEP5CUserP17SIG_CREATE_CHARAC
// @0x084bf4ac（报告 process_event.md 两段全读；薄转发）。
#include "InterDispatcher/process_event.inc"
}

// ORIG T _ZN18Inter_CreateCharac18process_createGiftEP5CUserP17SIG_CREATE_CHARAC
// @0x084bf4ce（报告 process_createGift.md 两段全读）：取角色创建礼包脚本，
// 按 m_startTime/m_endTime 过滤后逐道具 find_item → make_item → 系统邮件投递。
// [b28d-lane2] verdict=DIFF ndiff=204（ORIG 0x084bf4ce）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84bf4ce）：本轮 P28 后置 it++（ppEi，p167 CLEAR）；余 N18a 单段 regmove+N82/P112 帧槽平移。
#include "InterDispatcher/process_createGift.inc"
}

#include "InterDispatcher/dtor_Inter_DeleteCharac.inc"
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
#include "InterDispatcher/dispatch_sig_4.inc"
}

#include "InterDispatcher/dtor_Inter_LoadCharac.inc"
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
#include "InterDispatcher/dispatch_sig_5.inc"
}

// ============================================================================
// Inter_LoadInven::dispatch_sig ORIG 0x084c3f92 T（402 insn）——登录加载背包。
#include "InterDispatcher/_verifyInventory.inc"
#include "InterDispatcher/GetItemIdByAddInfo.inc"
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
#include "InterDispatcher/dispatch_sig_6.inc"
}
// ---- ORIG 私有辅助实现（本 TU 唯一 owner；其余两个后续批转写）----

// ORIG T _ZN15Inter_LoadInven19_CheckOverEquipItemEP5CUser @0x084c4d38
// （报告 docs/class_func_reports/Inter_LoadInven/_CheckOverEquipItem.md 两段全读）
// [b28d-lane2] verdict=DIFF ndiff=216（ORIG 0x084c4d38）：槽序/帧差(N82/P212/P221)+混合残差；gap-13。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=157（ORIG 0x84c4d38）：N18a 单段 regmove+N82/P112 帧槽平移。
#include "InterDispatcher/_CheckOverEquipItem.inc"
}

// ORIG T _ZN15Inter_LoadInven30_CheckUsableExpertJobEquipmentEP5CUser @0x084c4b38
// （报告 docs/class_func_reports/Inter_LoadInven/_CheckUsableExpertJobEquipment.md
// 两段全读）：槽 10..0x15 装备经 isExpertJobUsable 校验，不可用则清槽回插背包，
// 插入失败走 ReqDBSendNewMail 兜底。
#include "InterDispatcher/_CheckUsableExpertJobEquipment.inc"
}

// ORIG T _ZN15Inter_LoadInven18_UpdateRentalItemsEP5CUserP18SIG_LOAD_INVENTORY
// @0x084c455c（报告 docs/class_func_reports/Inter_LoadInven/_UpdateRentalItems.md
// 两段全读）：PC 房/维度频道直接清租借；否则机器键 1 按 exchange 标志取消息表或
// 当前角色租借清单装配 RentInput，空表删除记录，非空走 Rent 成功后回写。
// [b28d-lane2] verdict=DIFF ndiff=201（ORIG 0x084c455c）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-8。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84c455c）：本轮 P167/P0 具名引用 r=*it 单次 de+字段回读（p167 CLEAR，pt×4→de×1）；余 N18a/N82/P112 帧槽。
#include "InterDispatcher/_UpdateRentalItems.inc"
}

// ORIG T _ZN15Inter_LoadInven24_UpdateDimensionRentalItemsEP5CUserP18SIG_LOAD_INVENTORY
// @0x084c4858（报告 _UpdateDimensionRentalItems.md 两段全读）：维度租借机器键 2、
// exchange 表 0xb 条 stride 8（msg+(i+0x2a7e)*8）、elem.type 取自表第 7 字节或
// RentalInfo+0xb；其余与 PC 房租借同构。m_rentItemId 表路径无数据，按 ORIG
// RentInElem ctor 0x0822f02e 置 -1 [推断：movl $0xffffffff 实证]。
// [b28d-lane2] verdict=DIFF ndiff=194（ORIG 0x084c4858）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-18。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84c4858）：本轮 P167/P0 具名引用 r=*it 单次 de+字段回读（p167 CLEAR，pt×5→de×1）；余 N18a/N82/P112 帧槽。
#include "InterDispatcher/_UpdateDimensionRentalItems.inc"
}

#include "InterDispatcher/dtor_Inter_LoadInven.inc"

#include "InterDispatcher/dtor_Inter_LoadSkill.inc"
#include "InterDispatcher/dispatch_sig_7.inc"
}

#include "InterDispatcher/dtor_Inter_LoadQuest.inc"
#pragma pack(push, 1)
// Inter_LoadQuestPktView —— Inter_LoadQuest::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadQuestPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    int m_4;                        // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_8.inc"
}

#include "InterDispatcher/dtor_Inter_LoadTicket.inc"
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

#include "InterDispatcher/dispatch_sig_9.inc"
}

#include "InterDispatcher/dtor_Inter_LoadDungeonClear.inc"
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

#include "InterDispatcher/dispatch_sig_10.inc"
}

#include "InterDispatcher/dtor_Inter_LoadActionPoint.inc"
#include "InterDispatcher/dispatch_sig_11.inc"
}

#include "InterDispatcher/dtor_Inter_EmptyActionPoint.inc"
#include "InterDispatcher/dispatch_sig_12.inc"
}

#include "InterDispatcher/dtor_Inter_LoadRewardMedal.inc"
#include "InterDispatcher/dispatch_sig_13.inc"
}

#include "InterDispatcher/dtor_Inter_Load.inc"
#include "InterDispatcher/dispatch_sig_14.inc"
}

#include "InterDispatcher/dtor_Inter_DeleteChannel.inc"
#pragma pack(push, 1)
// Inter_DeleteChannelPktView —— Inter_DeleteChannel::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DeleteChannelPktView
{
    char m_pad0[0x8];  // 未消费
    int m_8;                  // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_15.inc"
}

#include "InterDispatcher/dispatch_sig_16.inc"
}
// Inter_Notice ctor/dtor：ORIG C1/D1 W（对象尺寸 4，仅 vptr；ISigDispatcher 基类
// vptr 覆写），与同文件 Inter_* 家族同构。
#include "InterDispatcher/dtor_Inter_Notice.inc"

#include "InterDispatcher/dtor_Inter_Terminate.inc"
#include "InterDispatcher/dispatch_sig_17.inc"
}

#include "InterDispatcher/dtor_Inter_ReadyToTerminate.inc"
#include "InterDispatcher/dispatch_sig_18.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorBuddyInOut.inc"
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
#include "InterDispatcher/dispatch_sig_19.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorLoginOK.inc"
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
#include "InterDispatcher/_NS_NonClient_Request.inc"
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

#include "InterDispatcher/makeRequest.inc"
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
#include "InterDispatcher/dispatch_sig_20.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorCallUserInfo.inc"
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
#include "InterDispatcher/dispatch_sig_21.inc"
}

#include "InterDispatcher/dtor_Inter_CutOffCallUserInfo.inc"
#include "InterDispatcher/dispatch_sig_22.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorUserGetout.inc"
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
#include "InterDispatcher/dispatch_sig_23.inc"
}

#include "InterDispatcher/dtor_Inter_WebNoticeIngameAdvertisement.inc"
#include "InterDispatcher/dispatch_sig_24.inc"
}

#include "InterDispatcher/dtor_Inter_ReloadIngameAdvertisement.inc"
#include "InterDispatcher/dispatch_sig_25.inc"
}

#include "InterDispatcher/dtor_Inter_TcpServerConnect.inc"
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
#include "InterDispatcher/dispatch_sig_26.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorReplyBuddyList.inc"
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

#include "InterDispatcher/dispatch_sig_27.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorCeraUpdate.inc"
#pragma pack(push, 1)
// Inter_MonitorCeraUpdatePktView —— Inter_MonitorCeraUpdate::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorCeraUpdatePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_28.inc"
}

#include "InterDispatcher/dtor_Inter_IPGResult.inc"
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
#include "InterDispatcher/dispatch_sig_29.inc"
}

#include "InterDispatcher/dtor_Inter_GenCeraTicket.inc"
#include "InterDispatcher/dispatch_sig_30.inc"
}

#include "InterDispatcher/dtor_Inter_LoadEventItem.inc"
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
#include "InterDispatcher/dispatch_sig_31.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorEventItemUpdate.inc"
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
#include "InterDispatcher/dispatch_sig_32.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorServerEventStart.inc"
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
#include "InterDispatcher/dispatch_sig_33.inc"
}

#include "InterDispatcher/dtor_Inter_FatigueResetDBReply.inc"
#include "InterDispatcher/dispatch_sig_34.inc"
}

#include "InterDispatcher/dtor_Inter_SuicideWhenQueueIsEmpty.inc"
#include "InterDispatcher/dispatch_sig_35.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildEnter.inc"
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
#include "InterDispatcher/dispatch_sig_36.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildSecede.inc"
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

#include "InterDispatcher/dispatch_sig_37.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildMarkChange.inc"
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
#include "InterDispatcher/dispatch_sig_38.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildDismiss.inc"
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
#include "InterDispatcher/dispatch_sig_39.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildChatMsg.inc"
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
#include "InterDispatcher/dispatch_sig_40.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorCallGuildMembers.inc"
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

#include "InterDispatcher/dispatch_sig_41.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorCallGuildMembersNext.inc"
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

#include "InterDispatcher/dispatch_sig_42.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorCallGuildAllMembers.inc"
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

#include "InterDispatcher/dispatch_sig_43.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorCallGuildAllMembersNext.inc"
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

#include "InterDispatcher/dispatch_sig_44.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildNames.inc"
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

#include "InterDispatcher/dispatch_sig_45.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildLoginOut.inc"
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

#include "InterDispatcher/dispatch_sig_46.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeOtherChannelChat.inc"
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
#include "InterDispatcher/dispatch_sig_47.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorCallGuildInfo.inc"
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
#include "InterDispatcher/dispatch_sig_48.inc"
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

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildInfo.inc"
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
#include "InterDispatcher/dispatch_sig_49.inc"
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

#include "InterDispatcher/dtor_Inter_MonitorSetGuildKey.inc"
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
#include "InterDispatcher/dispatch_sig_50.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildCreate.inc"
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

#include "InterDispatcher/dispatch_sig_51.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorGuildMemberNamingReply.inc"
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
#include "InterDispatcher/dispatch_sig_52.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorSetSubGuildMasterReply.inc"
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
#include "InterDispatcher/dispatch_sig_53.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildMemberInfo.inc"
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

#include "InterDispatcher/dispatch_sig_54.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeChangeGuildMemberGrade.inc"
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

#include "InterDispatcher/dispatch_sig_55.inc"
}

#include "InterDispatcher/dtor_Inter_GuildNoticeGuildMessage.inc"
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
#include "InterDispatcher/dispatch_sig_56.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberLoginOut.inc"
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

#include "InterDispatcher/dispatch_sig_57.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeRequestMemberEnterToRequester.inc"
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

#include "InterDispatcher/dispatch_sig_58.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeRequestMemberEnterToResponser.inc"
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
#include "InterDispatcher/dispatch_sig_59.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberEnterOk.inc"
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

#include "InterDispatcher/dispatch_sig_60.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberEnterReply.inc"
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
#include "InterDispatcher/dispatch_sig_61.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberSecede.inc"
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
#include "InterDispatcher/dispatch_sig_62.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberSecedeToSeceder.inc"
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

#include "InterDispatcher/dispatch_sig_63.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeDeleteMemberId.inc"
#pragma pack(push, 1)
// Inter_MonitorNoticeDeleteMemberIdPktView —— Inter_MonitorNoticeDeleteMemberId::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_MonitorNoticeDeleteMemberIdPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e（同上 unsigned→int 失配两步 spill 形态）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_64.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberChatMsg.inc"
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
#include "InterDispatcher/dispatch_sig_65.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorCallMemberList.inc"
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
#include "InterDispatcher/dispatch_sig_66.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorPayTaxToUpper.inc"
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
#include "InterDispatcher/dispatch_sig_67.inc"
}

#include "InterDispatcher/dtor_Inter_LoadPrivateStore.inc"
#include "InterDispatcher/dispatch_sig_68.inc"
}

#include "InterDispatcher/dtor_Inter_UpdateLogout.inc"
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
#include "InterDispatcher/dispatch_sig_69.inc"
}

#include "InterDispatcher/dtor_Inter_UserRepel.inc"
#include "InterDispatcher/dispatch_sig_70.inc"
}

#include "InterDispatcher/dtor_Inter_AvatarInsert.inc"
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
#include "InterDispatcher/dispatch_sig_71.inc"
}

#include "InterDispatcher/dtor_Inter_AvatarChangeStat.inc"
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
#include "InterDispatcher/dispatch_sig_72.inc"
}

#include "InterDispatcher/dtor_Inter_TriggerEventStart.inc"
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
#include "InterDispatcher/dispatch_sig_73.inc"
}

#include "InterDispatcher/dtor_Inter_TriggerEventEnd.inc"
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
#include "InterDispatcher/dispatch_sig_74.inc"
}

#include "InterDispatcher/dtor_Inter_MailBox_Req_List.inc"
#include "InterDispatcher/dispatch_sig_75.inc"
}

#include "InterDispatcher/dtor_Inter_MailBox_Req_CheckReceiver.inc"
#include "InterDispatcher/dispatch_sig_76.inc"
}
// ORIG 0x084d008a T _ZN31Inter_MailBox_Req_CheckReceiver15SendErrorPacketEP5CUser14ENUM_CMDPACKETt
// 报告 docs/class_func_reports/Inter_MailBox_Req_CheckReceiver/SendErrorPacket.md：
// 记日志后按命令 0x43 分流 2 字节/1 字节错误包。
#include "InterDispatcher/SendErrorPacket.inc"
}

// ORIG 0x084cff18 T _ZN31Inter_MailBox_Req_CheckReceiver21SendDBThread_forchinaEP5CUserP26SIG_MAILBOX_CHECK_RECEIVER
// 报告 SendDBThread_forchina.md：CStreamGuard(BigStreamPool) →写 0x1f5→写 UID→
// GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER> memcpy 0x3fde→+0x3fc1 角色名 0x1d→
// +0x2d 角色号→MsgQueueMgr::put(2)→返回 true。
// b28c-l23 SIGFIX：int→bool（ORIG __PRETTY_FUNCTION__ 串，P174；体尾
// `mov $0x1,%ebx` 直返，bool 免归一）。
// [b28d-lane2] verdict=DIFF ndiff=28（ORIG 0x084cff18）：寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=5（ORIG 0x84cff18）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
#include "InterDispatcher/SendDBThread_forchina.inc"
}

#include "InterDispatcher/dtor_Inter_MailBox_Req_Send.inc"
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
#include "InterDispatcher/dispatch_sig_77.inc"
}

#include "InterDispatcher/dtor_Inter_Monitor_Notify_New_Mail.inc"
#pragma pack(push, 1)
// Inter_Monitor_Notify_New_MailPktView —— Inter_Monitor_Notify_New_Mail::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_Monitor_Notify_New_MailPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_78.inc"
}

#include "InterDispatcher/dtor_Inter_InsertCreatureItemResult.inc"

#include "InterDispatcher/dispatch_sig_79.inc"
}

#include "InterDispatcher/dtor_Inter_UpdateCreatureItemResult.inc"
#include "InterDispatcher/dispatch_sig_80.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeMessage.inc"
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

#include "InterDispatcher/dispatch_sig_81.inc"
}

#include "InterDispatcher/dtor_Inter_LastMsgResponse.inc"
#include "InterDispatcher/dispatch_sig_82.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorGuildPointDel.inc"
#include "InterDispatcher/_makeDefaultTime.inc"
}
// ORIG 0x084ce756 T dispatch_sig —— 监控服公会点删除物品扣减。
// 报告 docs/class_func_reports/Inter_MonitorGuildPointDel/dispatch_sig.md 全文转写。
#include "InterDispatcher/dispatch_sig_83.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorGuildWarInfo.inc"
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
#include "InterDispatcher/dispatch_sig_84.inc"
}

#include "InterDispatcher/dtor_Inter_CodeChecksumRange.inc"
#include "InterDispatcher/dispatch_sig_85.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorForbidChat.inc"
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
#include "InterDispatcher/dispatch_sig_86.inc"
}

#include "InterDispatcher/dtor_Inter_MouseRegister.inc"
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
#include "InterDispatcher/dispatch_sig_87.inc"
}

#include "InterDispatcher/dtor_Inter_Monitor_MegaphoneMsg.inc"
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
#include "InterDispatcher/dispatch_sig_88.inc"
}

#include "InterDispatcher/dtor_Inter_Monitor_RegisterBlackListResult.inc"
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
#include "InterDispatcher/dispatch_sig_89.inc"
}

#include "InterDispatcher/dtor_Inter_Monitor_DeleteBlackListResult.inc"
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
#include "InterDispatcher/dispatch_sig_90.inc"
}

#include "InterDispatcher/dtor_Inter_Monitor_RequestBlackListResult.inc"
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
#include "InterDispatcher/dispatch_sig_91.inc"
}

#include "InterDispatcher/dtor_Inter_DB_RequestBlackCount.inc"
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

#include "InterDispatcher/dispatch_sig_92.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeBlackList.inc"
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
#include "InterDispatcher/dispatch_sig_93.inc"
}

#include "InterDispatcher/dtor_Inter_WebNoticeSingle.inc"
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

#include "InterDispatcher/dispatch_sig_94.inc"
}

#include "InterDispatcher/dtor_Inter_MemberExpLevelUp.inc"
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

#include "InterDispatcher/dispatch_sig_95.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorAddBuddy.inc"
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

#include "InterDispatcher/dispatch_sig_96.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorDelBuddy.inc"
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

#include "InterDispatcher/dispatch_sig_97.inc"
}

#include "InterDispatcher/dtor_Inter_ChangeLetterStat.inc"
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
#include "InterDispatcher/dispatch_sig_98.inc"
}

#include "InterDispatcher/dtor_Inter_SaveClientHackLog.inc"
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
#include "InterDispatcher/dispatch_sig_99.inc"
}

#include "InterDispatcher/dtor_Inter_ReqChangeCharacName.inc"
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
#include "InterDispatcher/dispatch_sig_100.inc"
}

#include "InterDispatcher/dtor_Inter_ReqQueryCharacInfo.inc"
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
#include "InterDispatcher/dispatch_sig_101.inc"
}

#include "InterDispatcher/dtor_Inter_ReportMannerless.inc"
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
#include "InterDispatcher/dispatch_sig_102.inc"
}

#include "InterDispatcher/dtor_Inter_ChagneUserHandicap.inc"
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

#include "InterDispatcher/dispatch_sig_103.inc"
}

#include "InterDispatcher/dtor_Inter_GMRequestMid.inc"
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

#include "InterDispatcher/dispatch_sig_104.inc"
}

#include "InterDispatcher/dtor_Inter_UDP_Incoming_Capcha_Data.inc"
#include "InterDispatcher/dispatch_sig_105.inc"
}

#include "InterDispatcher/dtor_Inter_Add_Slang.inc"
#include "InterDispatcher/dispatch_sig_106.inc"
}

#include "InterDispatcher/dtor_Inter_ReplyGuildLetter.inc"
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

#include "InterDispatcher/dispatch_sig_107.inc"
}

#include "InterDispatcher/dtor_Inter_CallGuildInviteToCaller.inc"
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

#include "InterDispatcher/dispatch_sig_108.inc"
}

#include "InterDispatcher/dtor_Inter_CallGuildInviteToInvited.inc"
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
#include "InterDispatcher/dispatch_sig_109.inc"
}

#include "InterDispatcher/dtor_Inter_ReplyGuildInviteToInvited.inc"
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

#include "InterDispatcher/dispatch_sig_110.inc"
}

#include "InterDispatcher/dtor_Inter_ReplyGuildInviteToCaller.inc"
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
#include "InterDispatcher/dispatch_sig_111.inc"
}

#include "InterDispatcher/dtor_Inter_LoadDeathTowerBestRecord.inc"
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
#include "InterDispatcher/dispatch_sig_112.inc"
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

#include "InterDispatcher/dtor_Inter_DeathTowerLoadTopRanker.inc"
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
#include "InterDispatcher/dispatch_sig_113.inc"
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

#include "InterDispatcher/dtor_Inter_MonitorDeathTowerRanking.inc"
#pragma pack(push, 1)
// Inter_MonitorDeathTowerRankingPktView —— 排行表（ORIG 084d32c3 `add $0x4,%edx;
//   mov 0x2(%eax,%edx,4)` ⇒ base=msg、数组元素 @+0x02、索引 (i*4+j+4) 单条 scaled）。
struct Inter_MonitorDeathTowerRankingPktView
{
    char m_pad0[0x2];
    unsigned int m_ranks[20];
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_114.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorTowerReloadTopRanker.inc"
#include "InterDispatcher/dispatch_sig_115.inc"
}

#include "InterDispatcher/dtor_Inter_ReplyGuildSecede.inc"
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
#include "InterDispatcher/dispatch_sig_116.inc"
}

#include "InterDispatcher/dtor_Inter_NoticeGuildMasterDelegate.inc"
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

#include "InterDispatcher/dispatch_sig_117.inc"
}

#include "InterDispatcher/dtor_Inter_GuildReplyGuildMasterDelegate.inc"
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

#include "InterDispatcher/dispatch_sig_118.inc"
}

#include "InterDispatcher/dtor_Inter_NoticeHasBeenGuildMaster.inc"
#pragma pack(push, 1)
// Inter_NoticeHasBeenGuildMasterPktView —— Inter_NoticeHasBeenGuildMaster::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeHasBeenGuildMasterPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned int m_a;               // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_119.inc"
}

#include "InterDispatcher/dtor_Inter_DBCheckDoubleGuildName.inc"
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
#include "InterDispatcher/dispatch_sig_120.inc"
}

#include "InterDispatcher/dtor_Inter_DBCheckDoubleGuildAddress.inc"
#pragma pack(push, 1)
// Inter_DBCheckDoubleGuildAddressPktView —— Inter_DBCheckDoubleGuildAddress::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DBCheckDoubleGuildAddressPktView
{
    char m_0;                 // +0x00
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_121.inc"
}

#include "InterDispatcher/dtor_Inter_ReplyGuildCreate.inc"
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

#include "InterDispatcher/dispatch_sig_122.inc"
}

#include "InterDispatcher/dtor_Inter_NoticeGuildNameOnGuildCreate.inc"
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

#include "InterDispatcher/dispatch_sig_123.inc"
}

#include "InterDispatcher/dtor_Inter_SecurityCardIssue.inc"
#pragma pack(push, 1)
// Inter_SecurityCardIssuePktView —— Inter_SecurityCardIssue::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SecurityCardIssuePktView
{
    char m_pad0[0x8c];  // 未消费（N：+0x04/+0x25/+0x37/+0x5e 串区基址——拆 pad 建具名槽使 setSecurityCardInfo 多指针参装载重排、破坏 AE，保留 reinterpret_cast）
    int m_8c;                 // +0x8c
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_124.inc"
}

#include "InterDispatcher/dtor_Inter_SecurityCardActivate.inc"
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
#include "InterDispatcher/dispatch_sig_125.inc"
}

#include "InterDispatcher/dtor_Inter_SecurityCardRetransfer.inc"
#pragma pack(push, 1)
// Inter_SecurityCardRetransferPktView —— Inter_SecurityCardRetransfer::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_SecurityCardRetransferPktView
{
    char m_pad0[0x88];  // 未消费
    int m_88;                 // +0x88
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_126.inc"
}

#include "InterDispatcher/dtor_Inter_LoadDnfHackLog.inc"
#pragma pack(push, 1)
// Inter_LoadDnfHackLogPktView —— Inter_LoadDnfHackLog::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadDnfHackLogPktView
{
    int m_0;                        // +0x00
    unsigned int m_4[1];            // +0x04 累计计数表（ORIG `mov 0x4(%eax,%edx,4)` scaled-index）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_127.inc"
}

#include "InterDispatcher/dtor_Inter_AutoPunishRuleBlackIP.inc"
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
#include "InterDispatcher/dispatch_sig_128.inc"
}

#include "InterDispatcher/dtor_Inter_AutoPunishRuleHackType.inc"
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
#include "InterDispatcher/dispatch_sig_129.inc"
}

#include "InterDispatcher/dtor_Inter_ReplyPowerWarScore.inc"
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

#include "InterDispatcher/dispatch_sig_130.inc"
}

#include "InterDispatcher/dtor_Inter_ReplyJoinPower.inc"
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
#include "InterDispatcher/dispatch_sig_131.inc"
}

#include "InterDispatcher/dtor_Inter_SecedePower.inc"
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
#include "InterDispatcher/dispatch_sig_132.inc"
}

#include "InterDispatcher/dtor_Inter_NoticePowerWarStartTime.inc"
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

#include "InterDispatcher/dispatch_sig_133.inc"
}

#include "InterDispatcher/dtor_Inter_NoticePowerWarEndTime.inc"
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

#include "InterDispatcher/dispatch_sig_134.inc"
}

#include "InterDispatcher/dtor_Inter_NoticePowerWarInfo.inc"
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
#include "InterDispatcher/dispatch_sig_135.inc"
}

#include "InterDispatcher/dtor_Inter_NoticePowerWarProlong.inc"
#pragma pack(push, 1)
// Inter_NoticePowerWarProlongPktView —— Inter_NoticePowerWarProlong::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticePowerWarProlongPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_136.inc"
}

#include "InterDispatcher/dtor_Inter_ChangeGuildName.inc"
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
#include "InterDispatcher/dispatch_sig_137.inc"
}

#include "InterDispatcher/dtor_Inter_NoticeChangeGuildNameToMembers.inc"
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

#include "InterDispatcher/dispatch_sig_138.inc"
}

#include "InterDispatcher/dtor_Inter_SendAllUserInfoMiniumForGuildSystem.inc"
#pragma pack(push, 1)
// Inter_SendAllUserInfoMiniumForGuildSystemPktView —— Inter_SendAllUserInfoMiniumForGuildSystem::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_SendAllUserInfoMiniumForGuildSystemPktView
{
    char m_pad0[0xe];  // +0x0..未消费
    unsigned int m_e;               // +0x0e
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_139.inc"
}

#include "InterDispatcher/dtor_Inter_LoadCleanPadPointReq.inc"
#include "InterDispatcher/dispatch_sig_140.inc"
}

#include "InterDispatcher/dtor_Inter_LoadCleanPadPoint.inc"
#include "InterDispatcher/dispatch_sig_141.inc"
}

#include "InterDispatcher/dtor_Inter_NotifyAuctionMail.inc"
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

#include "InterDispatcher/dispatch_sig_142.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionResultAskPrice.inc"
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
#include "InterDispatcher/dispatch_sig_143.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionResultAskRegistedItemNum.inc"
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
// [IG2PartyCera 2026-09-10] 语义轴真缺口修复（清单 our_only 0x3/0xd 复核）：
//   ① 0xd 来自旧 `for(i<sizeof(RandomOption))` 循环界，ORIG 0x084d66e5..0x084d66f7 起为**结构赋值展开**
//      ⇒ 改 `pck.item_info.m_opt = item.m_random.m_random;`（源 item+0x25 ✓ 逐条同构）。
//   ② 核 ORIG 该段（0x084d66d9..0x084d6730）直证：连续 24B 目标 item_info+0x13..+0x2b 分
//      两段写入 —— 前 0xa = item.m_amp.m_guid（源 item+0x15/0x19/0x1d = amp+0x04/08/0c，
//      4+4+2），后 0xe = RandomOption。旧实现只拷后者 ⇒ item_info+0x13..0x1c（m_pad）恒 0，
//      与 ORIG 发出的包内容不一致（**真缺口**）⇒ 补 `*(stAmplifyGuidView*)m_pad = item.m_amp.m_guid;`。
//   ③ 余 0x3（switch 决策树节点）为 lowering 差异（ORIG 用 case 0..5 跳表 0x8c8922c，
//      我方树；case 集合 1/2/5→err0x91、3→flag、0/4→default 逐点核对一致）⇒ 记 N。
//   读数：our 1074→1071（ORIG 1073，gap -1→+2，语义轴不受闸门）；TU 四数
//   135/1370/1/115/703 改前改后逐值不变。
#include "InterDispatcher/dispatch_sig_144.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionResultRegist.inc"
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
#include "InterDispatcher/dispatch_sig_145.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionResultBidding.inc"
#include "InterDispatcher/dispatch_sig_146.inc"
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
#include "InterDispatcher/dispatch_sig_taiwan.inc"
}
#include "InterDispatcher/dtor_Inter_AuctionResultRegistCancel.inc"
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
#include "InterDispatcher/dispatch_sig_147.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionResultItemList.inc"
#include "InterDispatcher/dispatch_sig_148.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionResultMyRegistedItems.inc"
#include "InterDispatcher/dispatch_sig_149.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionResultMyBidding.inc"
#include "InterDispatcher/dispatch_sig_150.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionLogMessage.inc"
#include "InterDispatcher/dispatch_sig_151.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionNotifyAuctionService.inc"
#include "InterDispatcher/dispatch_sig_152.inc"
}

#include "InterDispatcher/dtor_Inter_PointAuctionLogMessage.inc"
#include "InterDispatcher/dispatch_sig_153.inc"
}

#include "InterDispatcher/dtor_Inter_PointAuctionChargeCeraPoint.inc"
#include "InterDispatcher/dispatch_sig_154.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionResultBuyItemApiece.inc"
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
#include "InterDispatcher/dispatch_sig_155.inc"
}

#include "InterDispatcher/dtor_Inter_LoadGameOption.inc"
#include "InterDispatcher/dispatch_sig_156.inc"
}

#include "InterDispatcher/dtor_Inter_PvPChannelInfo.inc"
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
#include "InterDispatcher/dispatch_sig_157.inc"
}

#include "InterDispatcher/dtor_Inter_PvPChannelUserCount.inc"
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

#include "InterDispatcher/dispatch_sig_158.inc"
}

#include "InterDispatcher/dtor_Inter_LoadBloodDungeonBestRecord.inc"
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

#include "InterDispatcher/dispatch_sig_159.inc"
}

#include "InterDispatcher/dtor_Inter_LoadBreakAwayPreventSystem.inc"
#include "InterDispatcher/dispatch_sig_160.inc"
}

#include "InterDispatcher/dtor_Inter_LoadBreakAwayPreventMessage.inc"
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
#include "InterDispatcher/dispatch_sig_161.inc"
}

#include "InterDispatcher/dtor_Inter_LoadDimensionInout.inc"
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
#include "InterDispatcher/dispatch_sig_162.inc"
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

#include "InterDispatcher/dtor_Inter_BlackIPMonitorReq.inc"
#include "InterDispatcher/dispatch_sig_163.inc"
}

#include "InterDispatcher/dtor_Inter_BlackIPMonitor.inc"
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
#include "InterDispatcher/dispatch_sig_164.inc"
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

#include "InterDispatcher/dtor_Inter_BlackIPMonitorPartLoadReq.inc"
#include "InterDispatcher/dispatch_sig_165.inc"
}

#include "InterDispatcher/dtor_Inter_BlackIPMonitorDeleteIP.inc"
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
#include "InterDispatcher/dispatch_sig_166.inc"
}

#include "InterDispatcher/dtor_Inter_LoadAutoMarketConditionsControl.inc"
#include "InterDispatcher/dispatch_sig_167.inc"
}

#include "InterDispatcher/dtor_Inter_UpdateOneDayLethe.inc"
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
#include "InterDispatcher/dispatch_sig_168.inc"
}

#include "InterDispatcher/dtor_Inter_DisconnectMonitorServer.inc"
#pragma pack(push, 1)
// Inter_DisconnectMonitorServerPktView —— Inter_DisconnectMonitorServer::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_DisconnectMonitorServerPktView
{
    int m_0;                  // +0x00
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_169.inc"
}

#include "InterDispatcher/dtor_Inter_ResposncePCRoomPlyerList.inc"
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
#include "InterDispatcher/dispatch_sig_170.inc"
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

#include "InterDispatcher/dtor_Inter_ResposncePCRoomPlyerCount.inc"
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

#include "InterDispatcher/dispatch_sig_171.inc"
}

#include "InterDispatcher/dtor_Inter_NoticePCRoomPlayerLoginOut.inc"
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

#include "InterDispatcher/dispatch_sig_172.inc"
}

#include "InterDispatcher/dtor_Inter_NoticePowerWarRank.inc"
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
#include "InterDispatcher/dispatch_sig_173.inc"
}

#include "InterDispatcher/dtor_Inter_LoadPowerWarStatueRanker.inc"
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

#include "InterDispatcher/dispatch_sig_174.inc"
}

#include "InterDispatcher/dtor_Inter_LoadPowerWarStatueInfo.inc"
#include "InterDispatcher/dispatch_sig_175.inc"
}

#include "InterDispatcher/dtor_Inter_UpdateServerMessage.inc"
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
#include "InterDispatcher/dispatch_sig_176.inc"
}

#include "InterDispatcher/dtor_Inter_LoadServerMessage.inc"
#include "InterDispatcher/dispatch_sig_177.inc"
}

#include "InterDispatcher/dtor_Inter_NoticeServerMessageInfo.inc"
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
#include "InterDispatcher/dispatch_sig_178.inc"
}

#include "InterDispatcher/dtor_Inter_NoticeGuildMemo.inc"
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
#include "InterDispatcher/dispatch_sig_179.inc"
}

#include "InterDispatcher/dtor_Inter_PowerWarProcessInfo.inc"
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
#include "InterDispatcher/dispatch_sig_180.inc"
}

#include "InterDispatcher/dtor_Inter_PowerWarEndInfo.inc"
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

#include "InterDispatcher/dispatch_sig_181.inc"
}

#include "InterDispatcher/dtor_Inter_NoticeCreateGuildAgit.inc"
#pragma pack(push, 1)
// Inter_NoticeCreateGuildAgitPktView —— Inter_NoticeCreateGuildAgit::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeCreateGuildAgitPktView
{
    char m_pad0[0x12];  // +0x0..未消费
    int m_12;                       // +0x12
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_182.inc"
}

#include "InterDispatcher/dtor_Inter_NoticeDeleteGuildAgit.inc"
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

#include "InterDispatcher/dispatch_sig_183.inc"
}

#include "InterDispatcher/dtor_Inter_NoticeLoadGuildAgit.inc"
#pragma pack(push, 1)
// Inter_NoticeLoadGuildAgitPktView —— Inter_NoticeLoadGuildAgit::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_NoticeLoadGuildAgitPktView
{
    char m_pad0[0x16];  // +0x0..未消费
    unsigned char m_16;             // +0x16
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_184.inc"
}

#include "InterDispatcher/dtor_Inter_LoadExpertJob.inc"
#include "InterDispatcher/dispatch_sig_185.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorPunishUserReq.inc"
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
#include "InterDispatcher/dispatch_sig_186.inc"
}

#include "InterDispatcher/dtor_Inter_PcroomResponse.inc"
#include "InterDispatcher/IsShutdonTimeOverLogin.inc"
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
#include "InterDispatcher/dispatch_sig_187.inc"
}

#include "InterDispatcher/dtor_Inter_ItemLimitEdition_Sell_Start.inc"
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
#include "InterDispatcher/dispatch_sig_188.inc"
}

#include "InterDispatcher/dtor_Inter_ItemLimitEdition_Sell_End.inc"
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
#include "InterDispatcher/dispatch_sig_189.inc"
}

#include "InterDispatcher/dtor_Inter_ItemLimitEdition_Update.inc"
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
#include "InterDispatcher/dispatch_sig_190.inc"
}

#include "InterDispatcher/dtor_Inter_ItemLimitEdition_Reply.inc"
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
#include "InterDispatcher/dispatch_sig_191.inc"
}

#include "InterDispatcher/dtor_Inter_FindFactoryHub.inc"
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
#include "InterDispatcher/dispatch_sig_192.inc"
}

#include "InterDispatcher/dtor_Inter_GuildExpLimit.inc"
#include "InterDispatcher/dispatch_sig_193.inc"
}

#include "InterDispatcher/dtor_Inter_SetIPCount.inc"
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
#include "InterDispatcher/dispatch_sig_194.inc"
}

#include "InterDispatcher/dtor_Inter_SetFullIPCount.inc"
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
#include "InterDispatcher/dispatch_sig_195.inc"
}

#include "InterDispatcher/dtor_Inter_CancelRestingUser.inc"
#include "InterDispatcher/dispatch_sig_196.inc"
}

#include "InterDispatcher/dtor_Inter_ApplyTempPowerSide.inc"
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
#include "InterDispatcher/dispatch_sig_197.inc"
}

#include "InterDispatcher/dtor_Inter_ReloadPowerWarRanker.inc"
#include "InterDispatcher/dispatch_sig_198.inc"
}

#include "InterDispatcher/dtor_Inter_LoadPowerWarInfo.inc"
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
#include "InterDispatcher/dispatch_sig_199.inc"
}

#include "InterDispatcher/dtor_Inter_SetFactoryHubUser.inc"
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
#include "InterDispatcher/dispatch_sig_200.inc"
}

#include "InterDispatcher/dtor_Inter_SetFactoryHubUserNotConn.inc"
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

#include "InterDispatcher/dispatch_sig_201.inc"
}

#include "InterDispatcher/dtor_Inter_Set_CleanPad_Point.inc"
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
#include "InterDispatcher/dispatch_sig_202.inc"
}

#include "InterDispatcher/dtor_Inter_LoadNoCacheData.inc"
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
#include "InterDispatcher/dispatch_sig_203.inc"
}

#include "InterDispatcher/dtor_Inter_GuildCargo.inc"
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
#include "InterDispatcher/dispatch_sig_204.inc"
}

#include "InterDispatcher/dtor_Inter_NoticeGuildCargo.inc"
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
#include "InterDispatcher/dispatch_sig_205.inc"
}

#include "InterDispatcher/dtor_Inter_GuildCargoHistory.inc"
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
#include "InterDispatcher/dispatch_sig_206.inc"
}

#include "InterDispatcher/dtor_Inter_GuildCargoCheckPushItem.inc"
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
#include "InterDispatcher/Packet_Guild_Guild_Cargo_Push_Item.inc"
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
#include "InterDispatcher/dispatch_sig_207.inc"
}

#include "InterDispatcher/dtor_Inter_GuildCargoPushItem.inc"
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
#include "InterDispatcher/dispatch_sig_208.inc"
}

#include "InterDispatcher/dtor_Inter_GuildCargoPopItem.inc"
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
#include "InterDispatcher/dispatch_sig_209.inc"
}

#include "InterDispatcher/dtor_Inter_GuildCargoMoveItem.inc"
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
#include "InterDispatcher/dispatch_sig_210.inc"
}

#include "InterDispatcher/dtor_Inter_GuildCargoUpgrade.inc"
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
#include "InterDispatcher/dispatch_sig_211.inc"
}

#include "InterDispatcher/dtor_Inter_GuildBoardData.inc"
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
#include "InterDispatcher/dispatch_sig_212.inc"
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

#include "InterDispatcher/dtor_Inter_GuildBoardDelete.inc"
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

#include "InterDispatcher/dispatch_sig_213.inc"
}

#include "InterDispatcher/dtor_Inter_TakeScreenShot.inc"
#pragma pack(push, 1)
// Inter_TakeScreenShotPktView —— Inter_TakeScreenShot::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_TakeScreenShotPktView
{
    char m_pad0[0xb];  // +0x0..未消费
    int m_b;                        // +0x0b
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_214.inc"
}

#include "InterDispatcher/dtor_Inter_VillageAttackedStart.inc"
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
#include "InterDispatcher/dispatch_sig_215.inc"
}

#include "InterDispatcher/dtor_Inter_VillageAttackedEnd.inc"
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
#include "InterDispatcher/dispatch_sig_216.inc"
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

#include "InterDispatcher/dtor_Inter_VillageAttackedCountdown.inc"
#pragma pack(push, 1)
// Inter_VillageAttackedCountdownPktView —— Inter_VillageAttackedCountdown::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_VillageAttackedCountdownPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_217.inc"
}

#include "InterDispatcher/dtor_Inter_VillageAttackedUpdate.inc"
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

#include "InterDispatcher/dispatch_sig_218.inc"
}

#include "InterDispatcher/dtor_Inter_VillageAttackedScore.inc"
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

#include "InterDispatcher/dispatch_sig_219.inc"
}

#include "InterDispatcher/dtor_Inter_VillageAttackedReward.inc"
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
#include "InterDispatcher/dispatch_sig_220.inc"
}

#include "InterDispatcher/dtor_Inter_VillageAttackedRewardServer.inc"
#include "InterDispatcher/dispatch_sig_221.inc"
}

#include "InterDispatcher/dtor_Inter_FullLevelBroadCast.inc"
#pragma pack(push, 1)
// Inter_FullLevelBroadCastPktView —— Inter_FullLevelBroadCast::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_FullLevelBroadCastPktView
{
    char m_pad0[0xa];  // 未消费
    char m_a[1];              // +0xa 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_222.inc"
}

#include "InterDispatcher/dtor_Inter_Hack_CheckMyAccount.inc"
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

#include "InterDispatcher/dispatch_sig_223.inc"
}

#include "InterDispatcher/dtor_Inter_SetSecounUserData.inc"
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

#include "InterDispatcher/dispatch_sig_224.inc"
}

#include "InterDispatcher/dtor_Inter_SetARSInfo.inc"
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
#include "InterDispatcher/dispatch_sig_225.inc"
}

#include "InterDispatcher/dtor_Inter_OnTimeEventRewardStart.inc"
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
#include "InterDispatcher/dispatch_sig_226.inc"
}

#include "InterDispatcher/dtor_Inter_OnTimeEventRewardEnd.inc"
#include "InterDispatcher/dispatch_sig_227.inc"
}

#include "InterDispatcher/dtor_Inter_LoadPvPBuddy.inc"
#include "InterDispatcher/dispatch_sig_228.inc"
}

#include "InterDispatcher/dtor_Inter_PvPBuddyConnList.inc"
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
#include "InterDispatcher/dispatch_sig_229.inc"
}

#include "InterDispatcher/dtor_Inter_ReqAddPvPBuddy.inc"
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
#include "InterDispatcher/dispatch_sig_230.inc"
}

#include "InterDispatcher/dtor_Inter_AddPvPBuddyResult.inc"
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
#include "InterDispatcher/dispatch_sig_231.inc"
}

#include "InterDispatcher/dtor_Inter_RemovePvPBuddyResult.inc"
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
#include "InterDispatcher/dispatch_sig_232.inc"
}

#include "InterDispatcher/dtor_Inter_NoticePvPBuddyInOut.inc"
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
#include "InterDispatcher/dispatch_sig_233.inc"
}

#include "InterDispatcher/dtor_Inter_LoadPvPLiveEventData.inc"
#include "InterDispatcher/dispatch_sig_234.inc"
}

#include "InterDispatcher/dtor_Inter_Antibot_SendBufferClient.inc"
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
#include "InterDispatcher/dispatch_sig_235.inc"
}

#include "InterDispatcher/dtor_Inter_Antibot_PunishClient.inc"
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
#include "InterDispatcher/dispatch_sig_236.inc"
}

#include "InterDispatcher/dtor_Inter_Antibot_ProcessProtectMsg.inc"
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
#include "InterDispatcher/dispatch_sig_237.inc"
}

#include "InterDispatcher/dtor_Inter_LoadOnlinePreliminaryMember.inc"
#include "InterDispatcher/dispatch_sig_238.inc"
}

#include "InterDispatcher/dtor_Inter_LoadOnlinePreliminaryTeam.inc"
#include "InterDispatcher/dispatch_sig_239.inc"
}

#include "InterDispatcher/dtor_Inter_LoadOnlinePreliminaryMemberMatchList.inc"
#include "InterDispatcher/dispatch_sig_240.inc"
}

#include "InterDispatcher/dtor_Inter_LoadOnlinePreliminaryTeamMatchList.inc"
#include "InterDispatcher/dispatch_sig_241.inc"
}

#include "InterDispatcher/dtor_Inter_LoadCharacLinkBonus.inc"
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
#include "InterDispatcher/dispatch_sig_242.inc"
}

#include "InterDispatcher/dtor_Inter_CreateAccountCargo.inc"
#include "InterDispatcher/dispatch_sig_243.inc"
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

#include "InterDispatcher/dtor_Inter_UpgradeAccountCargo.inc"
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

#include "InterDispatcher/dispatch_sig_244.inc"
}

#include "InterDispatcher/dtor_Inter_LoadAccountCargo.inc"
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
#include "InterDispatcher/dispatch_sig_245.inc"
}

#include "InterDispatcher/dtor_Inter_MultiMailBoxReqSend.inc"
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
#include "InterDispatcher/dispatch_sig_246.inc"
}

#include "InterDispatcher/dtor_Inter_ReqQueryCharacInfoMailbox.inc"
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
#include "InterDispatcher/dispatch_sig_247.inc"
}

#include "InterDispatcher/dtor_Inter_ReqGameServerInfo.inc"
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
#include "InterDispatcher/dispatch_sig_248.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorRegistGameServerAck.inc"
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
#include "InterDispatcher/dispatch_sig_249.inc"
}

#include "InterDispatcher/dtor_Inter_LoadMileage.inc"
#pragma pack(push, 1)
// Inter_LoadMileagePktView —— Inter_LoadMileage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_LoadMileagePktView
{
    char m_pad0[0x8];  // 未消费
    int m_8;                  // +0x08
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_250.inc"
}

#include "InterDispatcher/dtor_Inter_GuildExpBookDelete.inc"
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
#include "InterDispatcher/dispatch_sig_251.inc"
}

#include "InterDispatcher/dtor_Inter_NoCache.inc"
#include "InterDispatcher/dispatch_sig_252.inc"
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

#include "InterDispatcher/dtor_Inter_Send_Charac_no_from_monitor.inc"
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

#include "InterDispatcher/dispatch_sig_253.inc"
}

#include "InterDispatcher/dtor_Inter_SendPeriodicMessage.inc"
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
#include "InterDispatcher/dispatch_sig_254.inc"
}

#include "InterDispatcher/dtor_Inter_SendTimeSync.inc"
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

#include "InterDispatcher/dispatch_sig_255.inc"
}

#include "InterDispatcher/dtor_Inter_SendTimeSyncLogin.inc"
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

#include "InterDispatcher/dispatch_sig_256.inc"
}

#include "InterDispatcher/dtor_Inter_AddGuildFundResult.inc"
#include "InterDispatcher/dispatch_sig_257.inc"
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

#include "InterDispatcher/dtor_Inter_TodayGuildMember.inc"
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
#include "InterDispatcher/dispatch_sig_258.inc"
}

#include "InterDispatcher/dtor_Inter_CheckJoinGuild.inc"
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

#include "InterDispatcher/dispatch_sig_259.inc"
}

#include "InterDispatcher/dtor_Inter_RequestJoinGuild.inc"
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
#include "InterDispatcher/dispatch_sig_260.inc"
}

#include "InterDispatcher/dtor_Inter_JoinGuildInfo.inc"
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

#include "InterDispatcher/dispatch_sig_261.inc"
}

#include "InterDispatcher/dtor_Inter_ApproveJoinGuild.inc"
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
#include "InterDispatcher/dispatch_sig_262.inc"
}

#include "InterDispatcher/dtor_Inter_GuildJoinList.inc"
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
#include "InterDispatcher/dispatch_sig_263.inc"
}

#include "InterDispatcher/dtor_Inter_GuildAttendanceInfo.inc"
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
#include "InterDispatcher/dispatch_sig_264.inc"
}

#include "InterDispatcher/dtor_Inter_AchieveGuildAttendance.inc"
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
#include "InterDispatcher/dispatch_sig_265.inc"
}

#include "InterDispatcher/dtor_Inter_UpdatePurchaseCashItemBonus.inc"
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
#include "InterDispatcher/dispatch_sig_266.inc"
}

#include "InterDispatcher/dtor_Inter_HadesDisConnect.inc"
#include "InterDispatcher/dispatch_sig_267.inc"
}

#include "InterDispatcher/dtor_Inter_GoblinPadChagePwdReward.inc"
#include "InterDispatcher/dispatch_sig_268.inc"
}

#include "InterDispatcher/dtor_Inter_GoblinPadSamePwdCheck.inc"
#include "InterDispatcher/dispatch_sig_269.inc"
}

#include "InterDispatcher/dtor_Inter_PunishCancel.inc"
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
#include "InterDispatcher/dispatch_sig_270.inc"
}

#include "InterDispatcher/dtor_Inter_BroadcastMsg.inc"
class CUserTeen : public CBelong
{
public:
    CUserTeen();
    bool IsRight(CUser* user);   // bool 化随 CBelong::IsRight 权威签名（mangled 不编码返回类型）
};

// ORIG CUserTeen::CUserTeen() W 0x084f07d0：调 CBelong() 后重置 vptr（正常 C++ 生成）。
#include "InterDispatcher/CUserTeen.inc"
#include "InterDispatcher/IsRight.inc"
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
#include "InterDispatcher/dispatch_sig_271.inc"
}

#include "InterDispatcher/dtor_Inter_TOD_ReturnToMonitor_APCSettingComplete.inc"
#include "InterDispatcher/dispatch_sig_272.inc"
}

#include "InterDispatcher/dtor_Inter_TOD_Reload_APC.inc"
#include "InterDispatcher/dispatch_sig_273.inc"
}

#include "InterDispatcher/dtor_Inter_UpdateSecuServiceState.inc"
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

#include "InterDispatcher/dispatch_sig_274.inc"
}

#include "InterDispatcher/dtor_Inter_LoadKillMonsterInfo.inc"
#include "InterDispatcher/dispatch_sig_275.inc"
}

#include "InterDispatcher/dtor_Inter_LevelUpReward.inc"
#include "InterDispatcher/dispatch_sig_276.inc"
}

#include "InterDispatcher/dtor_Inter_GetUserRegdate.inc"
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

#include "InterDispatcher/dispatch_sig_277.inc"
}

#include "InterDispatcher/dtor_Inter_GoldControlMoneyActionCancel.inc"
#pragma pack(push, 1)
// Inter_GoldControlMoneyActionCancelPktView —— Inter_GoldControlMoneyActionCancel::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_GoldControlMoneyActionCancelPktView
{
    unsigned char m_0;              // +0x00
    bool m_1;                       // +0x01
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_278.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeGuildChatMsgHyperLink.inc"
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
#include "InterDispatcher/dispatch_sig_279.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeMemberChatMsgHyperLink.inc"
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

#include "InterDispatcher/dispatch_sig_280.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorNoticeOtherChannelChatHyperLink.inc"
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
#include "InterDispatcher/dispatch_sig_281.inc"
}

#include "InterDispatcher/dtor_Inter_Monitor_MegaphoneMsgHyperLink.inc"
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
#include "InterDispatcher/dispatch_sig_282.inc"
}

#include "InterDispatcher/dtor_Inter_SendSecuReward.inc"
#include "InterDispatcher/dispatch_sig_283.inc"
}

// ORIG 0x084e5e62 W _ZN17Inter_HadesAction10send_alertEP5CUserj
// 报告 docs/class_func_reports/Inter_HadesAction/send_alert.md：PacketGuard→
// clear→put_header(0,0x1bf)→put_int(code)→finalize(true)→user->Send。
#include "InterDispatcher/send_alert.inc"
}

#include "InterDispatcher/dtor_Inter_HadesAction.inc"
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

#include "InterDispatcher/dispatch_sig_284.inc"
}

#include "InterDispatcher/dtor_Inter_Check3rdPartyConcent.inc"
#include "InterDispatcher/dispatch_sig_285.inc"
}

#include "InterDispatcher/dtor_Inter_SelectCharacMid.inc"
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
#include "InterDispatcher/dispatch_sig_286.inc"
}

#include "InterDispatcher/dtor_Inter_NpcLimitBuyItemInfoAll.inc"
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
#include "InterDispatcher/dispatch_sig_287.inc"
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

#include "InterDispatcher/dtor_Inter_NpcLimitBuyItemChangeInfo.inc"
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

#include "InterDispatcher/dispatch_sig_288.inc"
}

#include "InterDispatcher/dtor_Inter_NpcLimitBuyItemInfoRenew.inc"
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
#include "InterDispatcher/dispatch_sig_289.inc"
}

#include "InterDispatcher/dtor_Inter_MonitorReplyCharacInfo.inc"
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

#include "InterDispatcher/dispatch_sig_290.inc"
}

#include "InterDispatcher/dtor_Inter_CeraShopPurchaseCount.inc"
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

#include "InterDispatcher/dispatch_sig_291.inc"
}

#include "InterDispatcher/dtor_Inter_OwnerisVIP.inc"
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
#include "InterDispatcher/dispatch_sig_292.inc"
}

#include "InterDispatcher/dtor_Inter_LoadPuUserCheck.inc"
#include "InterDispatcher/dispatch_sig_293.inc"
}

#include "InterDispatcher/dtor_Inter_CollectItemsResult.inc"
#include "InterDispatcher/dispatch_sig_294.inc"
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

#include "InterDispatcher/dtor_Inter_CollectItemsReward.inc"
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
#include "InterDispatcher/dispatch_sig_295.inc"
}

#include "InterDispatcher/dtor_Inter_CollectItemsRewardBroadcast.inc"
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
#include "InterDispatcher/CUserResetTimeGateQuest.inc"
#include "InterDispatcher/Modify.inc"
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
#include "InterDispatcher/dispatch_sig_296.inc"
}

#include "InterDispatcher/dtor_Inter_RequestCharacSkillInfo.inc"
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
#include "InterDispatcher/dispatch_sig_297.inc"
}

#include "InterDispatcher/dtor_Inter_RequestTagCharacInfo.inc"
#pragma pack(push, 1)
// Inter_RequestTagCharacInfoPktView —— Inter_RequestTagCharacInfo::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   宽度=ORIG 访问指令，逐处对照 docs/class_func_reports 汇编核实）。
struct Inter_RequestTagCharacInfoPktView
{
    unsigned int m_0;         // +0x00
    unsigned char m_4;        // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_298.inc"
}

#include "InterDispatcher/dtor_Inter_EmergencyPatchMessage.inc"
#pragma pack(push, 1)
// Inter_EmergencyPatchMessagePktView —— Inter_EmergencyPatchMessage::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_EmergencyPatchMessagePktView
{
    char m_pad0[0xa];  // +0x0..未消费
    unsigned char m_a;              // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_299.inc"
}

#include "InterDispatcher/dtor_Inter_AuctionSubMoney.inc"
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

#include "InterDispatcher/dispatch_sig_300.inc"
}

#include "InterDispatcher/dtor_Inter_UpdateServerSnapShot.inc"
#include "InterDispatcher/dispatch_sig_301.inc"
}

#include "InterDispatcher/dtor_Inter_LoadCharacTypeHotKey.inc"
#pragma pack(push, 1)
// Inter_LoadCharacTypeHotKeyPktView —— Inter_LoadCharacTypeHotKey::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_LoadCharacTypeHotKeyPktView
{
    char m_pad0[0x4];  // +0x0..未消费
    unsigned char m_4;              // +0x04
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_302.inc"
}

#include "InterDispatcher/dtor_Inter_UpdateMiniCraneSeed.inc"
#pragma pack(push, 1)
// Inter_UpdateMiniCraneSeedPktView —— Inter_UpdateMiniCraneSeed::dispatch_sig 包视图（本 TU 消费偏移逐槽具名；
//   各字段宽度=转换前 reinterpret_cast 类型，逐处对照 ORIG 报告汇编指令核实）。
struct Inter_UpdateMiniCraneSeedPktView
{
    char m_pad0[0xa];  // +0x0..未消费
    int m_a;                        // +0x0a
};
#pragma pack(pop)

#include "InterDispatcher/dispatch_sig_303.inc"
}

#include "InterDispatcher/dtor_Inter_GetPremiumSelectedCubeInfo.inc"
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

#include "InterDispatcher/dispatch_sig_304.inc"
}

#include "InterDispatcher/dtor_Inter_CheckAddFriend.inc"
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
#include "InterDispatcher/dispatch_sig_305.inc"
}

#include "InterDispatcher/dtor_Inter_LoadCerashopAddRestrictType.inc"
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
#include "InterDispatcher/dispatch_sig_306.inc"
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
#include "InterDispatcher/dispatch_sig_307.inc"
}

// ORIG 0x084e8f68 T dispatch_sig —— 允许代理用户选择失败断开。
// 报告 docs/class_func_reports/Inter_SelectAllowProxyUser/dispatch_sig.md 全文转写。
#include "InterDispatcher/dispatch_sig_308.inc"
}


// ======== Inter_NpcLimitBuyItemInfo（B1：生命周期/reset；业务方法待类型域补齐） ========
// [b22判词·N-EH] Inter_NpcLimitBuyItemInfo::C1Ev（ORIG 0x084e6502，报告汇编+Ghidra C 全读）
// 主体逐指令同构 ORIG：ISigDispatcher::C1 → 置 vptr(&vtable+0x8) → pair<int,int>@+8::C1
// → Inven_Item@+0x1d::C1 → Inven_Item@+0x5a::C1 → this->reset()。orig25/our40 全差在
// 4.4.7 对 4 个可能抛的 sub-ctor 生成 EH cleanup 块（push %esi/%ebx + `jmp <landing>` +
// ~C1 pair + _Unwind_Resume，15 条），ORIG 编译器同位置无任何 landing pad。函数体不可
// 复现（编译器版本决策），语义等价。
// [b28c-32] verdict=AE：P216 reset() throw() 消 ~ISigDispatcher cleanup（40→25=ORIG）。
#include "InterDispatcher/Inter_NpcLimitBuyItemInfo.inc"
}

#include "InterDispatcher/dtor_Inter_NpcLimitBuyItemInfo.inc"
}

#include "InterDispatcher/reset.inc"
}

// ORIG 0x084e6acc T _ZN25Inter_NpcLimitBuyItemInfo10checkErrorEP5CUserP19LimitNpcBuyItemInfo
// 报告 docs/class_func_reports/Inter_NpcLimitBuyItemInfo/checkError.md：
// 用户/状态/角色号校验→find_item→GetNeedMaterial 填 m_needItem→
// GetInvenData(this+0x5a)→add_info/计数比较，返回错误码。
// [b28d-lane2] verdict=DIFF ndiff=96（ORIG 0x084e6acc）：寄存器中转(N18a/N7)+混合残差；gap-4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=64（ORIG 0x84e6acc）：N22 cmp 操作数方向+N82/P112 帧槽平移+P30/N4 add-vs-lea。
#include "InterDispatcher/checkError.inc"
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
#include "InterDispatcher/dispatch_sig_309.inc"
}

// 基类定义故意置于所有派生类实现之后（B1 ABI 形状修复）。
#include "InterDispatcher/dtor_ISigDispatcher.inc"
