#include <string.h>               // memset/strncpy/strlen
#include "DisPatcherCore.h"
#include "CUser.h"
#include "CParty.h"
#include "GameWorld.h"
#include "CGameManager.h"
#include "CDataManager.h"         // G_CDataManager()/m_serverParameter（serviceOfShusia 计价）
#include "ServiceRestrictManager.h"
#include "Arad_DataManager.h"     // ARAD::Singleton<ServiceRestrictManager>::Get
#include "CInventory.h"           // use_money/GetInstanceValueStatistic/VALUE_STATISTIC_FIELD
#include "Statistics.h"           // Statistics::UseShusiaService
#include "CAssaultMgr.h"          // pvp_assault::GetInstanceAssaultMgr
#include "CompoundEmblemDeps.h"   // LineFunc 权威声明
#include "CUserProc.h"            // APSystem::CUserProc::ClearActionAndSendtoUser（UseCoin）
#include "CStreamGuard.h"         // CStreamGuard（G2 角色域 process 使用）
#include "StreamPool.h"           // StreamPool::Free(CStreamGuard&)/BigStreamPool
#include "MsgQueueMgr.h"          // MsgQueueMgr::put(DB_Q, guard)
#include "GlobalData.h"           // s_pGMAccounts_/s_big_stream_pool/s_stream_pool/
                                  // s_msgq_mgr/s_monitor_proxy_mgr/s_systemTime_
#include "PacketGuard.h"          // PacketGuard/InterfacePacketBuf（回滚/通知包）
#include "CDungeon.h"             // CDungeon 完整类型（find_dungeon 字段访问）
#include "CWorldMap.h"            // CWorldMap::hasDeathTower
#include "CCubeStatistic.h"       // GetInstanceCubeStatistic/collectCubeStatistics
#include "HeroMissionEvent.h"     // ARAD::Singleton<HeroMissionEvent>::Get/processMission
#include "CSystemTime.h"          // CSystemTime::getCurTickCount（exercise_mode）
#include "WarField.h"             // stMapMonsterKillChecker_t（死亡塔 +0xb58 视图）
#include "PvP_Room.h"             // PvP_Room 完整类型（PvPTimeout::process 调 time_out）
#include "SigTypes.h"             // SIG_LOAD_*/SIG_CREATE_CHARAC 权威布局（RequestDB*/makeGift）
#include "CItem.h"                // CItem::make_item（makeGift 虚调用需完整类型）
#include "HistoryLog.h"          // HistoryLog::WriteBuyItem（BuyItem::process）
#include "CGuildServerProxy.h"   // CGuildServerProxy::OnBuyItem（BuyItem::process）
#include "CStatisticServerProxy.h"  // statistc_proxy::add 发送代理
#include "../Statics/StatisticProxy.h"  // StatisticsPacket 权威布局
#include <stdarg.h>              // va_list（statistc_proxy::add 可变参）
#include "Inven_Item.h"          // randomOptionReason/RandomOption（随机选项三 process）
#include "CSkill.h"              // CSkill::m_fitGrowType/IsStealable（UseSkill 黑客校验）
char IsLightServer();   // _Z13IsLightServerv 0x0822ad44 W（定义 GlobalData.cpp）


// ============================================================================
// Global::CBossTowerFunc / advancealtar::Manager 权威声明已落地：
//   BossTowerFunc.h（UseItem/SelectDungeon 定义于 BossTowerFunc.cpp）
//   advancealtar.h（经 CUser.h 引入；selectStage/procErrorSelectStage 定义于
//   advancealtar_Manager.cpp）。原局部镜像块删除（含【DisP_GroupG 增】两行，
//   已迁入 BossTowerFunc.h）。
// ============================================================================
#include "CSpecialItemRoutingManager.h"  // ProcessFailEtc（MoveMap::process 路由失败转移）
#include "CBattle_Field.h"          // check_start_point（CheckOneMapPlayTimeHackCnt
                                    //  经 party+0xb24 视图调用，CParty.cpp 同口径）
#include "BossTowerFunc.h"        // Global::CBossTowerFunc 权威头

#include "SkillSlot.h"            // addSkillOnCreateCharacter/_Mastered_skill
#include "APSystemTypes.h"        // APSystem::CDBAndExchangeServerProc
#include "CMonitorServerProxy.h"  // SendTcpPacket（SelectCharac 监视包）
#include "CServerProxyMgr.h"      // CServerProxyMgr<CMonitorServerProxy>::GetServerProxy
#include "../Monitor/LimitNpcBuyItem.h"  // LimitNpcBuyItemInfoAll（监视包布局）
#include "CDisconnectDetecter.h"   // disconnect_detecter（PvpHeartBeat）
#include "CPremiumLetheManager.h"  // ResetSkillReq（OneDayLetheTicket）
#include "Packet_Notice_Guild_War_Info.h"            // shared/packet/include
#include "Packet_Disable_User_OneToOneChat_Police.h" // shared/packet/include
#include "Packet_Monitor_Del_Buddy.h" // shared/packet/include（RemoveFriend 监视包）

// ============================================================================
// G2 局部依赖声明（ORIG 符号已核实；权威头/定义未落地 → 链接期 blocked，
// 完整清单见 /tmp/df_batches/Dispatcher_G2.md 第三节）
// ============================================================================
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime);
//   ^ 定义见 GameUtils.cpp:88（_Z22CheckDailyScheduleTimeill；本头无权威声明，
//     与 CBoosterGage.cpp:122 同款局部声明口径）
char* NumberToString(unsigned int value, int radix);
//   ^ _Z14NumberToStringji（与 CInventory.h:403 声明同签名；定义待链接）

class DB_GetDeathTowerBestRecord   // ORIG T 0x0842a048 _ZN26DB_GetDeathTowerBestRecord11makeRequestEij
{                                  // 定义由 DB_U3.cpp 提供（Prod_DB_Misc 批次）；
public:                            // 参数类型按 ORIG mangled Eij=(int, unsigned int)，
    static void makeRequest(int uid, unsigned int characNo);   // uid 按位不变传参
};

class DB_ReqSaveHackUserPunish  // ORIG T 0x08425494 _ZN24DB_ReqSaveHackUserPunish11makeRequestEjjPKc
{                              // （blocked：定义待补；CheckTrapBitsHackCnt 对
public:                        //  队伍内其余成员逐个落库惩罚）
    static void makeRequest(unsigned int uid, unsigned int type, const char* ip);
};

class DB_GetUserRegdate            // ORIG T 0x08442fae _ZN17DB_GetUserRegdate11makeRequestEij
{                                  // 定义由 DB_U3.cpp 提供（Prod_DB_Misc 批次）
public:
    static void makeRequest(int uid, unsigned int accId);
};


DisPatcher_CheckConn::DisPatcher_CheckConn() {}
DisPatcher_CheckConn::~DisPatcher_CheckConn() {}
int DisPatcher_CheckConn::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_CheckConn::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_CheckConn::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232a88 W _ZN20DisPatcher_CheckConn7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ea900)

DisPatcher_Login::DisPatcher_Login() : m_field4(0) {}   // ORIG C1：基类 C1 后写 *(this+0x4)=0
DisPatcher_Login::~DisPatcher_Login() {}
int DisPatcher_Login::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_Login::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_Login::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232a74 W _ZN16DisPatcher_Login7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081e8c78)

DisPatcher_SetIP::DisPatcher_SetIP() {}
DisPatcher_SetIP::~DisPatcher_SetIP() {}
int DisPatcher_SetIP::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetIP::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetIP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232fd2 W _ZN16DisPatcher_SetIP7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f468c)

DisPatcher_Exit::DisPatcher_Exit() {}
DisPatcher_Exit::~DisPatcher_Exit() {}
int DisPatcher_Exit::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_Exit::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_Exit::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232bea W _ZN15DisPatcher_Exit7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ed948)

DisPatcher_SelectCharac::DisPatcher_SelectCharac() {}
DisPatcher_SelectCharac::~DisPatcher_SelectCharac() {}
int DisPatcher_SelectCharac::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SelectCharac::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_SelectCharac::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG 0x081c6380 T _ZN23DisPatcher_SelectCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0x1587,pretty,err,acc_id)。
// resetSaveFlags → 视图状态/上次登录判定 → 综合PvP频道抑制 → exchange 命中计数 →
// 回填 MSG+0xe/+0x12（_Charac_info.m_field14b8/m_field14b7）→ 九路 DB/ES 请求 →
// 清上次登录 → 死塔最佳记录/注册日期 DB 请求 → LimitNpcBuyItemInfoAll 监视包。
int DisPatcher_SelectCharac::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stSelectCharac& out = static_cast<stSelectCharac&>(param);
    MSG_CHARAC_SELECT& req = *reinterpret_cast<MSG_CHARAC_SELECT*>(&msg);

    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x1587,
                        "virtual int DisPatcher_SelectCharac::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }

    user->resetSaveFlags();
    if (user->GetCharacViewState(static_cast<char>(req.m_slot)) == 1)
    {
        user->SetValidLastLoginCharac(false);
        user->SetCharacViewState(static_cast<char>(req.m_slot), 0);
        user->SetLoadRentalInfoFromExchange(false);
    }
    else
    {
        if (user->get_charac_no(static_cast<char>(req.m_slot)) ==
            user->GetLastLoginCharacNo())
        {
            const bool valid = (user->GetLastLoginCharacNo() != 0 &&
                                user->GetLastLoginChannelNo() != 0);
            if (valid)
            {
                user->SetValidLastLoginCharac(true);
                user->SetLoadRentalInfoFromExchange(true);
            }
        }
    }

    if (G_GameWorld()->GetChannelType() == 7 ||
        G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        user->SetValidLastLoginCharac(false);
    }

    if (user->IsValidLastLoginCharac())
    {
        exchange_server::GetInstanceExchangeServerHitRate()->IncrementCacheLoadCount();
    }
    else
    {
        exchange_server::GetInstanceExchangeServerHitRate()->IncrementDBLoadCount();
    }

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        req.m_0e = 0;
        req.m_12 = 0;
    }
    else
    {
        const _Charac_info& info =
            user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))];
        req.m_0e = info.m_field14b8;
        req.m_12 = info.m_field14b7;
    }

    RequestDBCharac(user, req);
    RequestBlackCount(user, req);
    RequestDBInventory(user, req);
    RequestDBQuest(user, req);
    RequestDBDungeonClear(user, req);
    APSystem::CDBAndExchangeServerProc::RequestLoadActionPointFromDBAndES(*user, req);
    RequestDBKillMonsterInfo(user, req);
    advancealtar::DBAndExchangeServerProc::requestLoadFromDBAndES(*user, req);
    RequestDBETC(user, req);

    user->SetValidLastLoginCharac(false);
    user->SetLastLoginCharacNo(0);
    user->SetLastLoginChannelNo(0);

    DB_GetDeathTowerBestRecord::makeRequest(
        user->GetUID(), user->get_charac_no(static_cast<char>(req.m_slot)));

    const int curSec = GlobalData::s_systemTime_.getCurSec();
    const int lastPlay = user->GetAccountLastPlayTime();
    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               lastPlay, curSec))
    {
        DB_GetUserRegdate::makeRequest(user->GetUID(), user->get_acc_id());
    }

    // 监视包（ORIG 栈上构造，uid(+0xa)/acc_id(+0xe)/charac_no(+0x12) 后发 0x182 字节）
    LimitNpcBuyItemInfoAll monitorInfo;
    monitorInfo.m_padA = user->GetUID();
    monitorInfo.m_dbid = user->get_acc_id();
    monitorInfo.m_charNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<char>(req.m_slot)));
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&monitorInfo), 0x182);
    return 0;
}

// ============================================================================
// DisPatcher_SelectCharac DB/ES 请求生产者（ORIG PacketDispatcher_Impl_1.cpp 域；
// 报告 docs/class_func_reports/DisPatcher_SelectCharac/Request*.md，Prod_DispatcherHelpers）
// ============================================================================

// ORIG _ZL20CREATECHARACTEREQUIP（LOCAL d 0x09370180，88 字节=22 int；readelf 实测
// st_size=88。makeGift @0x081c0a1a 以 [job*2]=初始武器 / [job*2+1]=初始衣服 取索引，
// 11 职业全部为 {0x28a0, 0x3070}）。
static const int CREATECHARACTEREQUIP[22] =
{
    0x28a0, 0x3070, 0x28a0, 0x3070, 0x28a0, 0x3070, 0x28a0, 0x3070,
    0x28a0, 0x3070, 0x28a0, 0x3070, 0x28a0, 0x3070, 0x28a0, 0x3070,
    0x28a0, 0x3070, 0x28a0, 0x3070, 0x28a0, 0x3070
};

// ORIG T 0x081c6ca0（RequestDBCharac.md）：SIG 7 → CACHE_CHARAC；
// Reset 后填 acc/charac/school/tag 区；双职业热键装载；队列 5(有效上次登录)/2(DB_Q)。
int DisPatcher_SelectCharac::RequestDBCharac(CUser* user, MSG_CHARAC_SELECT const& msg)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x173d);
    CStreamGuard guard(stream, true);
    guard << 7;
    guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_CHARAC);
    if (user->IsValidLastLoginCharac())
    {
        guard << user->GetLastLoginChannelNo();
        guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_CHARAC* sig = guard.GetInBuffer<SIG_LOAD_CHARAC>();
    sig->m_characData.Reset();                    // CHARAC_DATA::Reset W 0x822ec9e
    sig->m_accId = user->get_acc_id();
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_characData.m_schoolNo = user->GetSchoolNo();   // ORIG 写 SIG+0x682
    sig->m_tagCharacData.clear();                 // TAG_CHARAC_DATA::clear W 0x822ed84
    sig->m_tagCharacNo = msg.m_0e;
    sig->m_strikerSkillIndex = msg.m_12;
    const int jobSelected = user->get_character_job(sig->m_characNo);
    const int jobLastLogin = user->get_character_job(
        static_cast<unsigned int>(user->GetLastLoginCharacNo()));
    user->LoadCharacTypeHotKey(jobLastLogin, jobSelected);
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 1;
}

// ORIG T 0x081c79f2（RequestBlackCount.md）：SIG 0xb3 → CACHE_BLACK_COUNT；memset 0x10。
int DisPatcher_SelectCharac::RequestBlackCount(CUser* user, MSG_CHARAC_SELECT& msg)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x1862);
    CStreamGuard guard(stream, true);
    guard << 0xb3;
    guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_BLACK_COUNT);
    if (user->IsValidLastLoginCharac())
    {
        guard << user->GetLastLoginChannelNo();
        guard << user->GetLastLoginCharacNo();
    }
    SIG_REQ_BLACK_COUNT* sig = guard.GetInBuffer<SIG_REQ_BLACK_COUNT>();
    memset(sig, 0, sizeof(SIG_REQ_BLACK_COUNT));
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 1;
}

// ORIG T 0x081c6ee6：直接尾调 RequestDBInventory_forchina。
void DisPatcher_SelectCharac::RequestDBInventory(CUser* user, MSG_CHARAC_SELECT& msg)
{
    RequestDBInventory_forchina(user, msg);
}

// ORIG T 0x081c7c1c（RequestDBInventory_forchina.md）：SIG 8 → CACHE_INVENTORY；
// BigStreamPool；memset 0x1544c 后 acc/charac/tagInven 清理/+0x8←MSG.m_0e。
// 尾部两分支均返回 1（ORIG esi 未初始化路径经 test ebx 归一为恒真）。
void DisPatcher_SelectCharac::RequestDBInventory_forchina(CUser* user,
                                                          MSG_CHARAC_SELECT& msg)
{
    Stream* stream =
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x18af);
    CStreamGuard guard(stream, true);
    guard << 8;
    guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_INVENTORY);
    if (user->IsValidLastLoginCharac())
    {
        guard << user->GetLastLoginChannelNo();
        guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_INVENTORY* sig = guard.GetInBuffer<SIG_LOAD_INVENTORY>();
    memset(sig, 0, sizeof(SIG_LOAD_INVENTORY));
    sig->m_accId = user->get_acc_id();
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_tagInven.clear();                      // TAG_CHARAC_INVEN_DATA::clear W 0x822edd0
    sig->m_field8 = msg.m_0e;
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ORIG T 0x081c70c2（RequestDBQuest.md）：SIG 10 → CACHE_QUEST；BigStreamPool；
// memset 0x7608 后仅写 characNo。
int DisPatcher_SelectCharac::RequestDBQuest(CUser* user, MSG_CHARAC_SELECT& msg)
{
    Stream* stream =
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x17bd);
    CStreamGuard guard(stream, true);
    guard << 10;
    guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_QUEST);
    if (user->IsValidLastLoginCharac())
    {
        guard << user->GetLastLoginChannelNo();
        guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_QUEST* sig = guard.GetInBuffer<SIG_LOAD_QUEST>();
    memset(sig, 0, sizeof(SIG_LOAD_QUEST));
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 1;
}

// ORIG T 0x081c7436（RequestDBDungeonClear.md）：SIG 12 → CACHE_DUNGEON_CLEAR；
// StreamPool；memset 0x1810 后 characNo@0 / accId@4。
int DisPatcher_SelectCharac::RequestDBDungeonClear(CUser* user, MSG_CHARAC_SELECT& msg)
{
    Stream* stream =
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x17f8);
    CStreamGuard guard(stream, true);
    guard << 12;
    guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_DUNGEON_CLEAR);
    if (user->IsValidLastLoginCharac())
    {
        guard << user->GetLastLoginChannelNo();
        guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_DUNGEON_CLEAR* sig = guard.GetInBuffer<SIG_LOAD_DUNGEON_CLEAR>();
    memset(sig, 0, sizeof(SIG_LOAD_DUNGEON_CLEAR));
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_accId = user->get_acc_id();
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 1;
}

// ORIG T 0x081c7836（RequestDBKillMonsterInfo.md）：SIG 0x282 → CACHE_KILL_MONSTER；
// BigStreamPool；memset 0xafcc 后 characNo 写于尾部 +0xafc8。
int DisPatcher_SelectCharac::RequestDBKillMonsterInfo(CUser* user,
                                                      MSG_CHARAC_SELECT& msg)
{
    Stream* stream =
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x1843);
    CStreamGuard guard(stream, true);
    guard << 0x282;
    guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_KILL_MONSTER);
    if (user->IsValidLastLoginCharac())
    {
        guard << user->GetLastLoginChannelNo();
        guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_KILL_MONSTER_INFO* sig =
        guard.GetInBuffer<SIG_LOAD_KILL_MONSTER_INFO>();
    memset(sig, 0, sizeof(SIG_LOAD_KILL_MONSTER_INFO));
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 1;
}

// ORIG T 0x081c7602（RequestDBETC.md）：SIG 0x20 → CACHE_ETC；BigStreamPool；
// memset 0x14b60；serverGroup/channelNo ← CEnvironment；tagEtc 清理；
// job 预写 combo 视图 m_job（SIG 内绝对偏移 +0x14ab0）。
int DisPatcher_SelectCharac::RequestDBETC(CUser* user, MSG_CHARAC_SELECT& msg)
{
    Stream* stream =
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x1814);
    CStreamGuard guard(stream, true);
    guard << 0x20;
    guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_ETC);
    if (user->IsValidLastLoginCharac())
    {
        guard << user->GetLastLoginChannelNo();
        guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_ETC* sig = guard.GetInBuffer<SIG_LOAD_ETC>();
    memset(sig, 0, sizeof(SIG_LOAD_ETC));
    sig->m_accId = user->get_acc_id();
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_serverGroup =
        G_CEnvironment()->m_serverEnvir.m_serverGroup;      // ORIG 读 ENV+0x378
    sig->m_channelNo = G_CEnvironment()->get_channel_no();
    sig->m_fieldC = msg.m_0e;
    reinterpret_cast<TAG_CHARAC_ETC_DATA*>(sig->m_tagSkillSlot)->clear();
    reinterpret_cast<SIG_COMBO_SKILL*>(sig->m_comboSkill)->m_job =
        user->GetCharacJob(static_cast<int>(static_cast<signed char>(msg.m_slot)));
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 1;
}


// ORIG 0x081c041e T _ZN23DisPatcher_CreateCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0x79c,pretty,err,acc_id)。
// GM 透传 setCharacNameFilterPass → BigStreamPool 取流构造 SIG_CREATE_CHARAC
// （memset 0x5354）→ 填 accid/slot/name/+0x23 → 活动角色占用则 m_errNo=0x14 →
// 角色创建表（CDataManager::m_characList + job*0x7dc）回填 → 双技能树初始技能 →
// SP/makeGift/SSN 检查 → 战士(slot9) 连技 → lockCreateCharac 成功投递 DB_Q。
int DisPatcher_CreateCharac::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stCreateCharac& out = static_cast<stCreateCharac&>(param);
    MSG_CREATE_CHARACTER& req = *reinterpret_cast<MSG_CREATE_CHARACTER*>(&msg);

    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x79c,
                        "virtual int DisPatcher_CreateCharac::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }

    G_CDataManager()->setCharacNameFilterPass(
        GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()) ? true : false);

    Stream* stream =
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x7ab);
    CStreamGuard guard(stream, true);
    guard << 4;                                   // SIG 类型 4
    guard << static_cast<int>(user->GetUID());
    SIG_CREATE_CHARAC* sig = guard.GetInBuffer<SIG_CREATE_CHARAC>();
    memset(sig, 0, 0x5354);
    sig->m_accId = user->get_acc_id();
    sig->m_slot = req.m_slot;
    strncpy(sig->m_name, req.m_name, 0x1e);
    sig->m_field23 = static_cast<unsigned char>(req.m_name[0x1d]);
    if (sig->m_field23 != 0 && user->getUserEventCharacterFlag() != 0)
    {
        out.m_errNo = 0x14;                       // 活动角色创建位占用
        return 0;
    }

    sig->m_eventCharacFlag = user->getUserEventCharacterFlag();
    // 角色创建表：G_CDataManager()->m_characList 基址 + job*0x7dc
    // （与 SkillSlot.cpp:43 / CUser.cpp:4820 同口径；CCharacList 权威布局未建模）
    const unsigned char* jobRec =
        reinterpret_cast<const unsigned char*>(G_CDataManager()->m_characList) +
        static_cast<int>(sig->m_slot) * 0x7dc;
    sig->m_field26 =
        *reinterpret_cast<const unsigned short*>(jobRec);
    memcpy(sig->m_jobData, jobRec + 2, sizeof(sig->m_jobData));
    sig->m_field78 = jobRec[0x7d4];
    addSkillOnCreateCharacter(
        reinterpret_cast<_Mastered_skill*>(sig->m_masteredSkillA),
        static_cast<int>(sig->m_slot));
    addSkillOnCreateCharacter(
        reinterpret_cast<_Mastered_skill*>(sig->m_masteredSkillB),
        static_cast<int>(sig->m_slot));
    sig->m_spAtLevelUp = G_CDataManager()->GetSpAtLevelUp(1);
    makeGift(user, sig);

    const char* ssn = user->GetSsnString();
    sig->m_ssnCheckFail =
        (strlen(ssn) < 0xd || (ssn[6] != '2' && ssn[6] != '4')) ? 1 : 0;

    if (sig->m_slot == 9)
    {
        user->setComboSkillSet(true);
    }
    if (!user->lockCreateCharac())
    {
        return LineFunc(0x81f,
                        "virtual int DisPatcher_CreateCharac::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, user->get_acc_id());
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

DisPatcher_CreateCharac::DisPatcher_CreateCharac() {}
DisPatcher_CreateCharac::~DisPatcher_CreateCharac() {}
int DisPatcher_CreateCharac::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_CreateCharac::check_state(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_CreateCharac::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_CreateCharac::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG T 0x081c0a1a（makeGift.md）：按创建槽位（=职业索引）发放初始武器/衣服与
// 职业赠礼道具。find_item 失败时仅置标志 0；命中则 Inven_Item 预填 +0x02 索引后
// 经 CItem::make_item（vtable+0x08）填充并整体拷贝入 SIG 槽位。
void DisPatcher_CreateCharac::makeGift(CUser* /*user*/, SIG_CREATE_CHARAC* sig)
{
    const int job = static_cast<int>(static_cast<signed char>(sig->m_slot));

    CItem* weapon = G_CDataManager()->find_item(CREATECHARACTEREQUIP[job * 2]);
    CItem* clothes =
        G_CDataManager()->find_item(CREATECHARACTEREQUIP[job * 2 + 1]);
    if (weapon == 0 || clothes == 0)
    {
        sig->m_equipFindOk = 0;                       // ORIG SIG+0x5340；落入赠礼段
    }
    else
    {
        sig->m_equipFindOk = 1;
        Inven_Item tmpWeapon;
        tmpWeapon.m_addInfo = CREATECHARACTEREQUIP[job * 2];   // ORIG 预填 tmp+2 dword
        weapon->make_item(tmpWeapon);
        sig->m_weaponItem = tmpWeapon;                        // 0x3d 字节 → SIG+0x4e7b

        Inven_Item tmpClothes;
        tmpClothes.m_addInfo = CREATECHARACTEREQUIP[job * 2 + 1];
        clothes->make_item(tmpClothes);
        sig->m_clothesItem = tmpClothes;                      // 0x3d 字节 → SIG+0x4ef5
    }

    // ORIG +0x233 起的赠礼段：无论装备是否命中都会执行。
    int giftId;
    switch (job)
    {
    case 0: giftId = 0x6bd0; break;
    case 1: case 7: giftId = 0x74cc; break;
    case 2: case 5: giftId = 0x7a44; break;
    case 3: giftId = 0x8340; break;
    case 4: giftId = 0x88b9; break;
    case 6: giftId = 0x908a; break;
    case 8: giftId = 0x8340; break;
    case 9: giftId = 0x6bd0; break;
    case 10: giftId = 0x6520aea; break;
    default:
        return;                                       // ORIG ja +0x3d5：越界不处理
    }
    CItem* gift = G_CDataManager()->find_item(giftId);
    if (gift == 0)
    {
        sig->m_giftFindOk = 0;                        // ORIG SIG+0x533f
        return;
    }
    sig->m_giftFindOk = 1;
    {
        Inven_Item tmp;
        tmp.m_addInfo = giftId;
        gift->make_item(tmp);
        sig->m_giftItem = tmp;                        // 0x3d 字节 → SIG+0x4e01
    }
}

DisPatcher_DeleteCharac::DisPatcher_DeleteCharac() {}
DisPatcher_DeleteCharac::~DisPatcher_DeleteCharac() {}
int DisPatcher_DeleteCharac::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DeleteCharac::check_state(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DeleteCharac::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_DeleteCharac::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG 0x081c1182 T _ZN23DisPatcher_DeleteCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0xa41,pretty,err,acc_id)。
// RemoveCharacMercenaryInfo → StreamPool 取流构造 SIG_DELETE_CHARAC（memset 0x14）→
// characNo==0 则 m_errNo=0x15 并 Free 回流 → 填公会 id → lockDeleteCharac 成功投递
// DB_Q 并打 cMyTrace（accid 十进制串 / m_characNo / m_name），失败 Free + LineFunc(0xa68)。
int DisPatcher_DeleteCharac::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stDeleteCharac& out = static_cast<stDeleteCharac&>(param);
    MSG_DELETE_CHARACTER& req = *reinterpret_cast<MSG_DELETE_CHARACTER*>(&msg);

    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0xa41,
                        "virtual int DisPatcher_DeleteCharac::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }

    const char slot = static_cast<char>(req.m_slot);
    user->RemoveCharacMercenaryInfo(
        static_cast<unsigned int>(user->get_charac_no(slot)));

    Stream* stream =
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xa4d);
    CStreamGuard guard(stream, true);
    guard << 5;                                   // SIG 类型 5
    guard << static_cast<int>(user->GetUID());
    SIG_DELETE_CHARAC* sig = guard.GetInBuffer<SIG_DELETE_CHARAC>();
    memset(sig, 0, 0x14);
    sig->m_accId = user->get_acc_id();
    sig->m_characNo = static_cast<unsigned int>(user->get_charac_no(slot));
    if (sig->m_characNo == 0)
    {
        out.m_errNo = 0x15;                       // 槽位无角色
        GlobalData::s_stream_pool->Free(guard);
        return 0;
    }
    sig->m_guildId = static_cast<unsigned int>(
        user->GetGuildIdBeforeDeleteCharacter(slot));

    if (!user->lockDeleteCharac())
    {
        GlobalData::s_stream_pool->Free(guard);
        return LineFunc(0xa68,
                        "virtual int DisPatcher_DeleteCharac::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, user->get_acc_id());
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    const _Charac_info& info = user->m_characList[static_cast<int>(slot)];
    cMyTrace trace(
        "virtual int DisPatcher_DeleteCharac::process(CUser*, MSG_BASE&, ParamBase&)",
        0xa6e, 0);
    trace("[%s][%d] accid(%s) delete charac(%d) %s",
          "virtual int DisPatcher_DeleteCharac::process(CUser*, MSG_BASE&, ParamBase&)",
          0xa6e,
          NumberToString(user->get_acc_id(), 0),
          static_cast<int>(info.m_characNo),
          reinterpret_cast<const char*>(info.m_name));
    return 0;
}

DisPatcher_ReturnToSelectCharacter::DisPatcher_ReturnToSelectCharacter() {}
DisPatcher_ReturnToSelectCharacter::~DisPatcher_ReturnToSelectCharacter() {}
int DisPatcher_ReturnToSelectCharacter::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ReturnToSelectCharacter::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x0823349a W _ZN34DisPatcher_ReturnToSelectCharacter7processEP5CUserR8MSG_BASER9ParamBase
// push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，
// 全部逻辑位于 dispatch_sig(0x081fd25c，见 docs/class_func_reports/
// DisPatcher_ReturnToSelectCharacter/dispatch_sig.md；按分工 dispatch_sig 保持 stub)。
int DisPatcher_ReturnToSelectCharacter::process(CUser* /*user*/, MSG_BASE& /*msg*/,
                                                ParamBase& /*param*/)
{
    return 0;
}

DisPatcher_GetUserInfo::DisPatcher_GetUserInfo() {}
DisPatcher_GetUserInfo::~DisPatcher_GetUserInfo() {}
int DisPatcher_GetUserInfo::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_GetUserInfo::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x081c3dd8 T _ZN22DisPatcher_GetUserInfo7processEP5CUserR8MSG_BASER9ParamBase
// err=check_error；>0 返 -1；<0 LineFunc(0xf6d,pretty,0,acc_id)（第三参恒 0）；
// 否则 G_GameWorld()->get_user_info(user, (u16)msg+0xd, (s8)msg+0xf)，返回 err。
int DisPatcher_GetUserInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;   // ORIG 不写 param
    const int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0xf6d,
                        "virtual int DisPatcher_GetUserInfo::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, user->get_acc_id());
    }
    MSG_USERINFO_REQ& req = *reinterpret_cast<MSG_USERINFO_REQ*>(&msg);
    G_GameWorld()->get_user_info(user,
                                 static_cast<int>(req.m_targetId),
                                 static_cast<signed char>(req.m_slot));
    return errNo;
}

DisPatcher_RecoverStamina::DisPatcher_RecoverStamina() {}
DisPatcher_RecoverStamina::~DisPatcher_RecoverStamina() {}
int DisPatcher_RecoverStamina::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_RecoverStamina::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_RecoverStamina::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG 0x081c3eae T _ZN25DisPatcher_RecoverStamina7processEP5CUserR8MSG_BASER9ParamBase
// err>0：SendCmdErrorPacket(cmd=9, err&0xff) 后返回 -1；err<0：LineFunc(0xf9d,pretty,0,acc_id)
// 并返回其值；err==0：param+0x4 = user->serviceOfShusia()，返回 err。
int DisPatcher_RecoverStamina::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)9, (unsigned char)(errNo & 0xff));
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0xf9d,
                        "virtual int DisPatcher_RecoverStamina::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, user->get_acc_id());
    }
    static_cast<stOnRecoverStaminaParam&>(param).m_errNo = user->serviceOfShusia();
    return errNo;
}

DisPatcher_ReqPeer::DisPatcher_ReqPeer() {}
DisPatcher_ReqPeer::~DisPatcher_ReqPeer() {}
int DisPatcher_ReqPeer::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ReqPeer::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ReqPeer::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232de8 W _ZN18DisPatcher_ReqPeer7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081eed08)

DisPatcher_ResPeer::DisPatcher_ResPeer() {}
DisPatcher_ResPeer::~DisPatcher_ResPeer() {}
int DisPatcher_ResPeer::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ResPeer::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ResPeer::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232eac W _ZN18DisPatcher_ResPeer7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f088e)

DisPatcher_SetPartyInfo::DisPatcher_SetPartyInfo() {}
DisPatcher_SetPartyInfo::~DisPatcher_SetPartyInfo() {}
int DisPatcher_SetPartyInfo::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPartyInfo::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_SetPartyInfo::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG 0x081c4562 T _ZN23DisPatcher_SetPartyInfo7processEP5CUserR8MSG_BASER9ParamBase
// param+0x4 = check_error；>0 返回 0；<0 LineFunc(0x10f6,pretty,err,acc_id) 并返回其值；
// 无队：G_CGameManager()->GetParty() 空则 param=4 返回 0，否则 create_party +
// SetPartyInfoUI + send_party_info_to_all(party,0)；有队：非队长 param=8，
// 队长 SetPartyInfoUI + send_party_info_to_all(party,1)；均返回 0。
int DisPatcher_SetPartyInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stSetPartyInfo& out = static_cast<stSetPartyInfo&>(param);
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x10f6,
                        "virtual int DisPatcher_SetPartyInfo::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }

    const MSG_PARTY_SET_INFO& info = *reinterpret_cast<const MSG_PARTY_SET_INFO*>(&msg);
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        party = G_CGameManager()->GetParty();
        if (party == 0)
        {
            out.m_errNo = 4;
            return 0;
        }
        party->create_party(user);
        party->SetPartyInfoUI(info);
        G_GameWorld()->send_party_info_to_all(party, 0);
    }
    else
    {
        if (party->getManager() != user)
        {
            out.m_errNo = 8;
            return 0;
        }
        party->SetPartyInfoUI(info);
        G_GameWorld()->send_party_info_to_all(party, 1);
    }
    return 0;
}

DisPatcher_LeaveParty::DisPatcher_LeaveParty() {}
DisPatcher_LeaveParty::~DisPatcher_LeaveParty() {}
int DisPatcher_LeaveParty::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_LeaveParty::check_state(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_LeaveParty::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_LeaveParty::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG 0x081c4256 T _ZN21DisPatcher_LeaveParty7processEP5CUserR8MSG_BASER9ParamBase
// param+0x4 = check_error；>0 返回 0；<0 LineFunc(0x109f,pretty,err,acc_id) 并返回其值；
// 否则 leave_user(user,2)==1 时 param=1，随后 pvp_assault 强制移动，返回 0。
// BLOCKED（缺依赖，ORIG 符号无还原实现）：
//   pvp_assault::GetInstanceAssaultMgr()->ForcedMoveAtPowerWarPlace(user);
//   - _ZN11pvp_assault11CAssaultMgr25ForcedMoveAtPowerWarPlaceEP5CUser @0x082ed672
//     （try/catch 薄包装，转调 CAssaultPlace 同名函数）
//   - _ZN11pvp_assault13CAssaultPlace25ForcedMoveAtPowerWarPlaceEP5CUser @0x082ed6a6
//     （依赖未建模类 CForcedMoveCharacter）
//   - _ZN11pvp_assault20CForcedMoveCharacter10ForcedMoveEv @0x082e567c
int DisPatcher_LeaveParty::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stLeavePartyParam& out = static_cast<stLeavePartyParam&>(param);
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x109f,
                        "virtual int DisPatcher_LeaveParty::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2) == 1)  // ORIG 不判空直接调用
    {
        out.m_errNo = 1;
    }
    // pvp_assault::GetInstanceAssaultMgr()->ForcedMoveAtPowerWarPlace(user);  // 见上 BLOCKED
    return 0;
}

DisPatcher_WalkoutParty::DisPatcher_WalkoutParty() {}
DisPatcher_WalkoutParty::~DisPatcher_WalkoutParty() {}
int DisPatcher_WalkoutParty::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_WalkoutParty::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_WalkoutParty::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG 0x081caf98 T _ZN23DisPatcher_WalkoutParty7processEP5CUserR8MSG_BASER9ParamBase
// err>0 原样返回；err<0 返回 -1；否则 param+0x4=0，无队 param=0x13；
// 有队先 SendWalkout(msg+0xd 槽位, WALKOUT_TYPE_0) 再 param = walkout_member(user, 槽位)，返回 0。
int DisPatcher_WalkoutParty::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        return errNo;
    }
    if (errNo < 0)
    {
        return -1;
    }

    stWalkoutPartyParam& out = static_cast<stWalkoutPartyParam&>(param);
    out.m_errNo = 0;
    const MSG_WALKOUT_PARTY& req = *reinterpret_cast<const MSG_WALKOUT_PARTY*>(&msg);
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        out.m_errNo = 0x13;
        return 0;
    }
    const int slot = (int)(char)req.m_walkoutSlot;
    party->SendWalkout(slot, ENUM_WALKOUT_TYPE_0);
    out.m_errNo = party->walkout_member(user, slot);
    return 0;
}

DisPatcher_StartGame::DisPatcher_StartGame() {}
DisPatcher_StartGame::~DisPatcher_StartGame() {}
int DisPatcher_StartGame::check_error(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // TODO(ORIG)（签名按 ORIG mangled 三参化；体保持 stub）
int DisPatcher_StartGame::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_StartGame::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG T 0x081ca2a8 _ZN20DisPatcher_StartGame19excludeAddHackCountEP5CUser
// （excludeAddHackCount.md 全文转写）：空用户/副本入口区/moveSpace==1 免记，否则记账。
int DisPatcher_StartGame::excludeAddHackCount(CUser* user)
{
    if (user == 0)
    {
        return 0;
    }
    if (G_GameWorld()->isDungeonEntranceArea(user))
    {
        return 0;
    }
    if (user->getMoveSpace() == (UserSpace::T)1)
    {
        return 0;
    }
    return 1;
}

// ORIG 0x081ca2fe T process（process.md 全文转写）。
// err>0 返 0；err<0 LineFunc(0x1e5b)；err==0：世界图 hasDeathTower 死调用后，
// 未组队走 GameManager 单人 party（hack 记账 0x1f6 + set_single_play + game_start）；
// 已组队且 state==3 时对所在 party game_start；均返回 0。
int DisPatcher_StartGame::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    static const char* const kPretty =
        "virtual int DisPatcher_StartGame::process(CUser*, MSG_BASE&, ParamBase&)";
    stStartGameParam& out = static_cast<stStartGameParam&>(param);
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x1e5b, kPretty, out.m_errNo, user->get_acc_id());
    }

    int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
    if (worldIdx > 0)
    {
        CWorldMap* worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
        if (worldMap != 0)
        {
            worldMap->hasDeathTower();   // ORIG 调用后丢弃结果
        }
    }

    if (!user->CheckInParty())
    {
        CParty* party = G_CGameManager()->GetParty();
        if (party == 0)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", kPretty, 0x1e99,
                                  "cant party memory : you must free party memory!!!");
        }
        else
        {
            if (excludeAddHackCount(user))
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x1f6, 1u, 0u, 0u);
            }
            party->set_single_play(user);
            party->game_start(user);
        }
    }
    else if (user->get_state() == 3)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            party->game_start(user);
        }
    }
    return 0;
}

DisPatcher_SelectDungeon::DisPatcher_SelectDungeon() {}
DisPatcher_SelectDungeon::~DisPatcher_SelectDungeon() {}
int DisPatcher_SelectDungeon::check_error(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // TODO(ORIG)（签名按 ORIG mangled 三参化；体保持 stub）
int DisPatcher_SelectDungeon::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_SelectDungeon::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG T 0x081c9172 _ZN24DisPatcher_SelectDungeon15checkBlueMarbleEP6CParty
// （checkBlueMarble.md 全文转写）。
int DisPatcher_SelectDungeon::checkBlueMarble(CParty* party)
{
    if (party == 0)
    {
        return 0;
    }
    CUser* manager = party->getManager();
    if (manager == 0)
    {
        return 0;
    }
    if (!party->IsSinglePlay())
    {
        return 0;
    }
    return ((int)manager->getMoveSpace() == 1) ? 1 : 0;
}

// ORIG 0x081c8102 T process（process.md 全文转写，含汇编分支次序）。
int DisPatcher_SelectDungeon::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    static const char* const kPretty =
        "virtual int DisPatcher_SelectDungeon::process(CUser*, MSG_BASE&, ParamBase&)";
    const MSG_SELECT_DUNGEON& req = *reinterpret_cast<const MSG_SELECT_DUNGEON*>(&msg);
    stSelectDungeonParam& out = static_cast<stSelectDungeonParam&>(param);

    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x1970, kPretty, out.m_4, user->get_acc_id());
    }

    IsLightServer();   // ORIG 调用后丢弃结果

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0 && party->getManager() != user)
    {
        out.m_4 = 0x7fffffff;   // 非队长选副本
        return 0;
    }

    if (party != 0)
    {
        // CParty::m_partyType(+0xcd8) 私有且无 setter（ORIG 直接寻址），按布局偏移写入
        *reinterpret_cast<int*>(reinterpret_cast<char*>(party) + 0xcd8) =
            (int)(char)req.m_field11;
    }

    if (user->isDisguiseCharac())
    {
        user->setDisguiseCharac(false, 0, 0);
    }

    if (party != 0)
    {
        const int memberCount = party->get_member_count();
        cMyTrace trace(kPretty, 0x19bf, 0);
        trace("[DisPatcher_SelectDungeon] partyCount : %d", memberCount);

        char hackFound = 0;
        const unsigned char selfVill = user->getCurCharacVill();
        const int selfArea = user->get_area(false);
        for (int i = 0; i < memberCount; ++i)
        {
            CUser* member = party->get_user(i);
            if (member == 0)
            {
                continue;
            }
            member->setDisguiseCharac(false, 0, 0);
            const unsigned char memberVill = member->getCurCharacVill();
            const int memberArea = member->get_area(false);
            if ((selfVill == memberVill && selfArea == memberArea) ||
                party->is_quick_party())
            {
                member->SetDeleteEventDungeonInvitation(false);
                continue;
            }
            LogManager::logFormat(
                1, "PacketDispatcher_Impl_1.cpp", kPretty, 0x19df,
                "[HACK_VILL_AREA] m_id : %s, Vill(%d,%d), Area(%d,%d)",
                NumberToString(member->get_acc_id(), 0),
                (int)selfVill, (int)memberVill, selfArea, memberArea);
            hackFound = 1;
            break;
        }
        if (hackFound)
        {
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = party->get_user(i);
                if (member == 0)
                {
                    continue;
                }
                LogManager::logFormat(
                    1, "PacketDispatcher_Impl_1.cpp", kPretty, 0x19fa,
                    "[HACK_VILL_AREA] m_id : %s, Vill(%d), Area(%d)",
                    NumberToString(member->get_acc_id(), 0), (int)selfVill, selfArea);
                member->DisConnSig((DISCONN_SIG)0x18, true, 0);
            }
            out.m_4 = 0x7fffffff;
            return 0;
        }
    }

    bool checkSpecial = ((short)req.m_dungeonIdx != (short)10000);
    CDungeon* dungeon = G_CDataManager()->find_dungeon((int)req.m_dungeonIdx);
    if (dungeon != 0 && dungeon->m_advanceAltarSurvivalFlag != 0)
    {
        checkSpecial = false;
    }
    if (checkSpecial && party != 0)
    {
        const int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
        if (worldIdx <= 0)
        {
            out.m_8 = 8;   // 不在有效世界图：错误 8，继续后续流程
        }
        else
        {
            CWorldMap* worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
            if (worldMap == 0)
            {
                out.m_4 = 0x7fffffff;
                return 0;
            }
            if (!worldMap->hasDungeon(
                    static_cast<unsigned int>(static_cast<int>(req.m_dungeonIdx))))
            {
                for (int i = 0; i < 4; ++i)
                {
                    if (!party->checkValidUser(i))
                    {
                        continue;
                    }
                    CUser* member = party->get_user(i);
                    member->getHackAnalyzer()->addServerHackCnt(
                        member, (WongWork::ENUM_HACKTYPE)0x1f8, 1u, 0u, 0u);
                }
                out.m_8 = 8;
                return 0;
            }
        }
    }

    char hasDeathFlag = 0;
    {
        const int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
        if (worldIdx > 0)
        {
            CWorldMap* worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
            if (worldMap != 0)
            {
                hasDeathFlag = worldMap->hasDeathTower() ? 1 : 0;
            }
        }
    }

    dungeon = G_CDataManager()->find_dungeon((int)req.m_dungeonIdx);
    if (dungeon == 0)
    {
        out.m_4 = 0x7fffffff;
        return 0;
    }

    const int kind = dungeon->getDungeonKind();
    if (kind == 1)
    {
        // ---- 死亡之塔 ----
        if (party == 0)
        {
            out.m_4 = 0x7fffffff;
            return 0;
        }
        WongWork::CDeathTower* tower = G_CGameManager()->getDeathTower();
        if (tower == 0)
        {
            out.m_4 = 0x7fffffff;
            return 0;
        }
        const int minLevel = dungeon->get_min_level();
        for (int i = 0; i < 4; ++i)
        {
            CUser* member = party->get_user(i);
            if (member != 0 && member->get_charac_level() < minLevel)
            {
                return 0;   // 有成员等级不足：静默忽略
            }
        }
        tower->attachParty(party);
        tower->handleStartGame((int)req.m_dungeonIdx, (char)req.m_dungeonType,
                               (ENUM_DUNGEON_TYPE)(char)req.m_field10);
        out.m_4 = 0x7fffffff;
        return 0;
    }
    if (kind == 3)
    {
        // ---- BossTower（ORIG 此处不判空直接取成员数，保持一致）----
        const int memberCount = party->get_member_count();
        if ((signed char)dungeon->get_limit_party_count() < memberCount)
        {
            PacketGuard guard;
            guard.clear();
            guard.put_header(1, 0x10);
            guard.put_byte(0);
            guard.put_int(0xf3);
            guard.finalize(true);
            party->send_to_party(guard);
            return 0;
        }
        Global::CBossTowerFunc::SelectDungeon(party, (short)req.m_dungeonIdx,
                                              (ENUM_DUNGEON_TYPE)(char)req.m_field10);  // BLOCKED 符号见文档
        return 0;
    }

    if (dungeon->m_advanceAltarSurvivalFlag != 0)
    {
        // ---- 祭坛副本 ----
        const int selErr = advancealtar::Manager::selectStage(
            user, (int)req.m_dungeonIdx,
            (advancealtar::StageDifficulty::T)(char)req.m_dungeonType);   // BLOCKED 符号见文档
        advancealtar::Manager::procErrorSelectStage(
            user, (ENUM_ERROR)selErr);   // BLOCKED 符号见文档
        return 0;
    }

    if (dungeon->m_entryRestrictFlag != 0)
    {
        // ---- 等级下限检查型（ORIG 不判空 party，保持一致）----
        for (int i = 0; i < 4; ++i)
        {
            if (!party->checkValidUser(i))
            {
                continue;
            }
            CUser* member = party->get_user(i);
            if (member != 0 && member->get_charac_level() < dungeon->get_min_level())
            {
                out.m_c = 0xe;
                return 0;
            }
        }
    }
    else if (dungeon->m_villageAttackEventFlag != 0)
    {
        // ---- 复仇地下城 ----
        if ((char)req.m_dungeonType != 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x1f8, 1u, 0u, 0u);
            out.m_4 = 0x7fffffff;
            return 0;
        }
        if (party != 0)
        {
            const int memberCount = party->get_member_count();
            if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
            {
                out.m_c = 0x15;
                return 0;
            }
            if ((signed char)dungeon->m_maxAttackPartyMember < memberCount ||
                memberCount < (signed char)dungeon->m_minAttackPartyMember)
            {
                out.m_c = 0xab;
                return 0;
            }
            for (int i = 0; i < 4; ++i)
            {
                if (!party->checkValidUser(i))
                {
                    continue;
                }
                CUser* member = party->get_user(i);
                const unsigned char revengeCnt = member->GetCurRevengeDungeonCount();
                if (revengeCnt == 0 || (unsigned char)dungeon->m_maxRevengeCount < revengeCnt)
                {
                    member->SetCurRevengeDungeonCount(0);
                    out.m_c = 0xad;
                    out.m_14 = i;
                    return 0;
                }
            }
        }
    }

    // ---- 公共开团路径（ORIG +0xc1f）----
    if (party != 0)
    {
        const bool skipFatigue = (hasDeathFlag != 0) ||
                                 (party->GetPartyType() == 1) ||
                                 (checkBlueMarble(party) != 0) ||
                                 (dungeon->isNoFatigueDungeon() != 0);
        if (!skipFatigue)
        {
            const int fatigue = party->CheckMemberFatigue();
            if (fatigue > 0)
            {
                out.m_c = 0x16;
                out.m_8 = 0x16;
                out.m_14 = fatigue - 1;
                return 0;
            }
        }
        const int started = party->dungeon_start(
            (int)req.m_dungeonIdx, (char)req.m_dungeonType,
            (ENUM_DUNGEON_TYPE)(char)req.m_field10);
        if (started > 0)
        {
            out.m_c = started;
            out.m_8 = started;
        }
        else if (user->m_network.get_nat_type() == 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x326, 1u, 0u, 0u);
        }
    }
    return 0;
}

DisPatcher_SendMess::DisPatcher_SendMess() {}
DisPatcher_SendMess::~DisPatcher_SendMess() {}
int DisPatcher_SendMess::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SendMess::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x08232f70 W _ZN19DisPatcher_SendMess7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部聊天业务（type 分派/表情改写/敏感词 Filter/消耗品扣减/community+monitor 监视包/
// DB_InsertPoliceSaveChatting/GameWorld::send_chat_msg）内嵌于
// dispatch_sig(0x081f3540，报告 docs/class_func_reports/DisPatcher_SendMess/dispatch_sig.md)。
int DisPatcher_SendMess::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_DeleteItem::DisPatcher_DeleteItem() {}
DisPatcher_DeleteItem::~DisPatcher_DeleteItem() {}
int DisPatcher_DeleteItem::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DeleteItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
bool DisPatcher_DeleteItem::exercise_mode(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const MSG_DELETE_ITEM& req = *reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);
    stDeleteItemParam& out = static_cast<stDeleteItemParam&>(param);
    // ORIG thunk 0x0822fe4e = CUser::GetPartyIndex()，结果作 GetParty(int) 实参
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return false;
    }

    WongWork::CDeathTower* tower = 0;
    if (user->get_state() == 10)
    {
        tower = user->getDeathTower();
    }

    bool limitNotOne = false;
    if (tower != 0)
    {
        CDungeon* dungeon = tower->getCDungeonMGr()->getDungeon();
        limitNotOne = (dungeon->limitOfStackableItemInTower() != 1);
        if (dungeon->limitOfStackableItemInTower() != 0)
        {
            for (int i = 0; i < (int)(signed char)req.m_count; ++i)
            {
                CItem* item = G_CDataManager()->find_item(req.m_itemIdx[i]);
                if (item != 0 && item->get_index() == 0x1973)
                {
                    // ORIG lea 0xb58(tower) == &m_timerKeys[7]（CGameManager.h 布局）
                    reinterpret_cast<stMapMonsterKillChecker_t*>(
                        reinterpret_cast<char*>(tower) + 0xb58)
                        ->setUseSkillMaterial(GlobalData::s_systemTime_.getCurTickCount());
                }
            }
        }
    }

    const bool skip = (party->GetPartyType() == 1) &&
                      (user->get_state() == 5 || limitNotOne);
    if (!skip)
    {
        return false;
    }

    out.m_9 = 0;
    out.m_8 = req.m_itemSpace;
    for (int i = 0; i < (int)(signed char)req.m_count; ++i)
    {
        out.m_slotsD2[i] = req.m_position[i];
        out.m_counts19c[i] = req.m_countArr[i];
        out.m_slotsA[i] = req.m_opType[i];
    }
    return true;
}
int DisPatcher_DeleteItem::restricted_check(CUser* user, MSG_BASE& /*msg*/)
{
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 1))
    {
        return 0xd1;
    }
    return (int)GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x1d);
}
int DisPatcher_DeleteItem::condition_check(CUser* user, MSG_BASE& msg, INVEN_TYPE& type, int idx)
{
    const MSG_DELETE_ITEM& req = *reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);
    if (type == INVEN_TYPE_INVEN && req.m_position[idx] <= 9)
    {
        return 0x17;   // 普通背包槽位 <=9 受保护
    }
    const int invenType = GetInvenTypeFromItemSpace((int)(char)req.m_itemSpace);
    if (user->CheckItemLock(invenType, (int)req.m_position[idx]))
    {
        return 0xd5;   // 物品锁定
    }
    return 0;
}
void DisPatcher_DeleteItem::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)


// ORIG 0x081c1dda T process（process.md 全文转写）。
int DisPatcher_DeleteItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    static const char* const kPretty =
        "virtual int DisPatcher_DeleteItem::process(CUser*, MSG_BASE&, ParamBase&)";
    const MSG_DELETE_ITEM& req = *reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);
    stDeleteItemParam& out = static_cast<stDeleteItemParam&>(param);

    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0xb8e, kPretty, out.m_4, user->get_acc_id());
    }

    if (exercise_mode(user, msg, param))
    {
        return 0;
    }

    INVEN_TYPE invenKind = (INVEN_TYPE)6;
    if ((signed char)req.m_itemSpace == 1)
    {
        invenKind = (INVEN_TYPE)2;
    }
    else if ((signed char)req.m_itemSpace == 3)
    {
        invenKind = (INVEN_TYPE)0;
    }
    if (invenKind == (INVEN_TYPE)2)
    {
        out.m_4 = 0x17;   // 装备栏禁止该删除类型
        out.m_8 = req.m_itemSpace;
        return 0;
    }

    bool needRestricted = false;
    for (int i = 0; i < (int)(signed char)req.m_count; ++i)
    {
        if (req.m_opType[i] <= 1)
        {
            needRestricted = true;
            break;
        }
    }
    if (needRestricted)
    {
        out.m_4 = restricted_check(user, msg);
        if (out.m_4 != 0)
        {
            out.m_8 = req.m_itemSpace;
            return 0;
        }
    }

    const int count = (int)(signed char)req.m_count;
    for (int i = 0; i < count; ++i)
    {
        out.m_4 = condition_check(user, msg, invenKind, i);
        if (out.m_4 != 0)
        {
            out.m_8 = req.m_itemSpace;
            return 0;
        }

        int delKind = 3;
        if (req.m_opType[i] == 1)
        {
            delKind = 5;
            if (user->get_state() != 3)
            {
                delKind = 6;
            }
        }

        const int invenType = GetInvenTypeFromItemSpace((int)(char)req.m_itemSpace);
        Inven_Item item = ((CInventory*)user->getCurCharacInvenR())
                              ->GetInvenSlot(invenType, (int)req.m_position[i]);

        if (delKind == 3)
        {
            Global::CBossTowerFunc::UseItem(user, item);   // BLOCKED 符号见文档
        }
        if (item.m_addInfo == 0xbdd)
        {
            ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
                user, (HeroMissionCondition::MissionNo::T)3,
                (unsigned int)req.m_countArr[i]);
        }

        if (user->get_state() == 3 && delKind == 5 && req.m_countArr[i] == 0)
        {
            const int secu = GlobalData::s_pSecuProtectionField->Check(
                user, (SECURITY_PROTCTION)0x46);
            if (secu != 0)
            {
                out.m_4 = secu;
                return 0;
            }
        }

        // ORIG ILP32 ABI（objdump 逐槽核对）：arg3(m)=itemIdx(@0x19f)、
        // arg4(int)=count(@0x32f)、arg5(enum)=delKind；callee 以 ebp+0x14=itemIdx、
        // ebp+0x18=count 消费。
        const int deleted =
            user->delete_item((char)req.m_itemSpace, (int)req.m_position[i],
                              (unsigned long)req.m_itemIdx[i], req.m_countArr[i],
                              (eItemDelReason)delKind);

        if (deleted > 0)
        {
            // 中途失败：先向客户端回滚已删条目，再发失败确认
            cMyTrace trace(kPretty, 0xc2a, 5);
            trace("delete_item failed in the middle of deleting items. %d/%d",
                  i, (int)(signed char)req.m_count);

            PacketGuard ack;
            if (i > 1)
            {
                PacketGuard rollback;
                rollback.put_header(1, 0x12);
                rollback.put_byte(1);
                rollback.put_byte((int)(char)req.m_itemSpace);
                rollback.put_byte(i - 1);
                for (int k = 0; k < i - 1; ++k)
                {
                    rollback.put_short((int)req.m_position[k]);
                    rollback.put_int(req.m_countArr[k]);
                    rollback.put_short((int)req.m_opType[k]);
                }
                rollback.finalize(true);
                user->Send(rollback);
            }
            ack.put_header(1, 0x12);
            ack.put_byte(0);
            ack.put_byte(deleted);
            ack.put_byte((int)(char)req.m_itemSpace);
            ack.finalize(true);
            user->Send(ack);
            return LineFunc(0xc48, kPretty, 0, user->get_acc_id());
        }

        if (!item.isEmpty() && delKind == 5)
        {
            CItem* script = G_CDataManager()->find_item(item.m_addInfo);
            if (script != 0)
            {
                if (script->is_stackable())
                {
                    item.m_addInfo2 = req.m_countArr[i];   // Inven_Item+0x07 计数字段
                }
                user->Add_RedeemInfo(item, 0, false);
            }
        }

        CItem* sellScript = G_CDataManager()->find_item(req.m_itemIdx[i]);
        if (sellScript != 0)
        {
            int price = 0;
            G_Store()->GetSellItemPrice(item, sellScript, (short)req.m_countArr[i],
                                        false, price);
            if (user->get_state() != 8)
            {
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)0x16, user, (unsigned int)price);
            }
            if (user->get_state() == 5)
            {
                if (req.m_opType[i] == 2)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        sellScript, req.m_countArr[i], user, (CUBE_STATISTIC_FIELD)0x67);
                }
                else if (req.m_opType[i] == 3)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        sellScript, req.m_countArr[i], user, (CUBE_STATISTIC_FIELD)0x68);
                }
                else if (req.m_opType[i] == 4)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        sellScript, req.m_countArr[i], user, (CUBE_STATISTIC_FIELD)0x69);
                }
            }
        }
    }

    out.m_8 = req.m_itemSpace;
    out.m_9 = (unsigned char)req.m_count;
    for (int i = 0; i < count; ++i)
    {
        out.m_slotsD2[i] = req.m_position[i];
        out.m_counts19c[i] = req.m_countArr[i];
        out.m_slotsA[i] = req.m_opType[i];
    }
    return 0;
}
DisPatcher_MoveItem::DisPatcher_MoveItem() {}
DisPatcher_MoveItem::~DisPatcher_MoveItem() {}
int DisPatcher_MoveItem::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_MoveItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_MoveItem::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)


// ORIG 0x081c5904 T process（process.md 全文转写）。
int DisPatcher_MoveItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    static const char* const kPretty =
        "virtual int DisPatcher_MoveItem::process(CUser*, MSG_BASE&, ParamBase&)";
    const MSG_MOVE_ITEMSPACE& req = *reinterpret_cast<const MSG_MOVE_ITEMSPACE*>(&msg);
    stMoveItemParam& out = static_cast<stMoveItemParam&>(param);

    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        if (out.m_4 == 0xd5)   // 仅锁定类错误回显源/目的 space
        {
            out.m_8 = req.m_fromSpace;
            out.m_10 = req.m_toSpace;
        }
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x141b, kPretty, out.m_4, user->get_acc_id());
    }

    int outSlot = 0;
    int ret = 0;
    if (((signed char)req.m_toSpace == 3 || (signed char)req.m_toSpace == 0x12) &&
        req.m_toPos > 9 && req.m_toPos < 0x16)
    {
        // 扩容区间移动（toSpace 3/0x12 且目的槽位 10..21）
        ret = user->exchange_interspace_expand(
            (int)(char)req.m_fromSpace, (int)req.m_fromPos,
            (unsigned long)req.m_count, (int)req.m_field14,
            (int)(char)req.m_toSpace, (int)req.m_toPos,
            (unsigned long)req.m_itemIdx,
            &outSlot, (char)req.m_expanda, (char)req.m_expandb);
    }
    else
    {
        ret = user->exchange_interspace(
            (int)(char)req.m_fromSpace, (int)req.m_fromPos,
            (unsigned long)req.m_count, (int)req.m_field14,
            (int)(char)req.m_toSpace, (int)req.m_toPos,
            (unsigned long)req.m_itemIdx,
            &outSlot);
    }

    if (ret == 0)
    {
        out.m_8 = req.m_fromSpace;
        out.m_a = req.m_fromPos;
        out.m_c = (int)req.m_field14;
        out.m_10 = req.m_toSpace;
        out.m_14 = outSlot;
    }
    else
    {
        out.m_4 = ret;
        out.m_8 = req.m_fromSpace;
        out.m_10 = req.m_toSpace;
    }
    return 0;
}
DisPatcher_SortItem::DisPatcher_SortItem() {}
DisPatcher_SortItem::~DisPatcher_SortItem() {}
int DisPatcher_SortItem::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SortItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_SortItem::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)


// ORIG 0x081c9c54 T process（process.md 全文转写）。
int DisPatcher_SortItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const MSG_SORT_ITEM& req = *reinterpret_cast<const MSG_SORT_ITEM*>(&msg);
    stOnSortItemParam& out = static_cast<stOnSortItemParam&>(param);
    const int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }

    out.m_invenType = req.m_invenType;
    out.m_result = 0;
    switch (out.m_invenType)
    {
    case 0:
        out.m_result = user->getCurCharacInvenW()->sort((INVEN_TYPE)1);
        break;
    case 1:
        out.m_result = user->getCurCharacInvenW()->sort((INVEN_TYPE)2);
        break;
    case 2:
        user->getCurCharacCargoW()->sort();
        break;
    case 7:
        out.m_result = user->getCurCharacInvenW()->sort((INVEN_TYPE)3);
        break;
    default:
        break;
    }
    return 0;
}
DisPatcher_BuyItem::DisPatcher_BuyItem() {}
DisPatcher_BuyItem::~DisPatcher_BuyItem() {}
int DisPatcher_BuyItem::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_BuyItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_BuyItem::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG 0x081bed44 T _ZN18DisPatcher_BuyItem15_onBuyItemEventERK12BuyItemParamP5CUser
// 报告 _onBuyItemEvent.md：push %ebp; mov %esp,%ebp; pop %ebp; ret —— 原始二进制
// 即为纯空实现（购买事件挂钩在 ORIG 中未挂任何回调），忠实转写空函数。
void DisPatcher_BuyItem::_onBuyItemEvent(const BuyItemParam& /*param*/, CUser* /*user*/)
{
}

// ORIG 0x081beab0 T get_nMaxBuyableCount（报告 get_nMaxBuyableCount.md 全文映射）：
// 对六种货币价格（money/cera/winpoint/finishpoint/guildPowerWarPoint/userPowerWarPoint）
// 逐项取「持有量 / 单价」与 0x7fffffff 取小，返回可购上限。
unsigned int DisPatcher_BuyItem::get_nMaxBuyableCount(CUser* user, BuyItemParam& param)
{
    unsigned int maxCount = 0x7fffffff;
    if (param.m_80 != 0)
    {
        unsigned int byMoney =
            (unsigned int)user->getCurCharacMoney() / (unsigned int)param.m_80;
        if ((int)byMoney < (int)maxCount)
        {
            maxCount = byMoney;
        }
    }
    if (param.m_84 != 0)
    {
        unsigned int byCera = (unsigned int)user->GetCera() / (unsigned int)param.m_84;
        if ((int)byCera < (int)maxCount)
        {
            maxCount = byCera;
        }
    }
    if (param.m_88 != 0)
    {
        unsigned int byWinPoint =
            (unsigned int)user->GetWinPoint() / (unsigned int)param.m_88;
        if ((int)byWinPoint < (int)maxCount)
        {
            maxCount = byWinPoint;
        }
    }
    if (param.m_8c != 0)
    {
        unsigned int byFinishPoint =
            (unsigned int)user->GetFinishPoint() / (unsigned int)param.m_8c;
        if ((int)byFinishPoint < (int)maxCount)
        {
            maxCount = byFinishPoint;
        }
    }
    if (param.m_90 != 0)
    {
        unsigned int byGuildPoint =
            (unsigned int)user->GetGuildPowerWarPoint() / (unsigned int)param.m_90;
        if ((int)byGuildPoint < (int)maxCount)
        {
            maxCount = byGuildPoint;
        }
    }
    if (param.m_94 != 0)
    {
        unsigned int byUserWarPoint =
            (unsigned int)(int)user->GetUserPowerWarPoint() / (unsigned int)param.m_94;
        if ((int)byUserWarPoint < (int)maxCount)
        {
            maxCount = byUserWarPoint;
        }
    }
    return maxCount;
}

// ORIG 0x081be658 T get_data（报告 get_data.md ## 反编译 C 全文映射）：
// 解析商城物品 → 材料槽 → 六种单价（>1 件时乘数量并校验上限）→ 持有量校验。
// 返回 0=成功；0x11=物品不存在；1=参数非法；10=余额不足；0x24=公会会长限制。
int DisPatcher_BuyItem::get_data(CUser* user, MSG_BUY_ITEM& msg, BuyItemParam& param)
{
    CDataManager* dataMgr = G_CDataManager();
    param.m_a8 = dataMgr->find_item(msg.m_itemIdx);
    if (param.m_a8 == 0)
    {
        return 0x11;
    }
    // GetNeedMaterial() 返回 +0x24 处 pair<int,int>*（item idx, 数量），ORIG 读两 int
    const int* needMaterial =
        reinterpret_cast<const int*>(param.m_a8->GetNeedMaterial());
    param.m_pair.first = needMaterial[0];
    param.m_pair.second = needMaterial[1];
    if (param.m_pair.first != -1)
    {
        const CInventory* invenR = user->getCurCharacInvenR();
        param.m_a4 = invenR->GetInvenData(param.m_pair.first, param.m_resultItem);
        CDataManager* dataMgr2 = G_CDataManager();
        param.m_ac = dataMgr2->find_item(param.m_pair.first);
        if (param.m_ac == 0 || param.m_ac->is_stackable() != true)
        {
            return 1;
        }
    }
    param.m_80 = param.m_a8->get_price();             // ORIG 0x0822c84a W
    param.m_84 = param.m_a8->GetCashPrice();          // ORIG 0x0822c856 W
    param.m_88 = param.m_a8->GetMedalPrice();
    param.m_8c = param.m_a8->GetFinishPointPrice();
    param.m_90 = param.m_a8->GetGuildPowerWarPoint();   // ORIG vtable slot +0x28
    param.m_94 = param.m_a8->GetUserPowerWarPoint();    // ORIG vtable slot +0x2c
    if (param.m_a8->is_stackable() != true)
    {   // 非堆叠：msg+0x11 须等于当日种子（防复制）
        if (msg.m_buyCount != user->GetSeedFromDate())
        {
            return 1;
        }
    }
    else
    {
        if (msg.m_buyCount == 0)
        {
            return 1;
        }
        if ((unsigned int)msg.m_buyCount > 1)
        {
            if (get_nMaxBuyableCount(user, param) < (unsigned int)msg.m_buyCount)
            {
                return 10;
            }
            param.m_80 *= msg.m_buyCount;
            param.m_84 *= msg.m_buyCount;
            param.m_88 *= msg.m_buyCount;
            param.m_8c *= msg.m_buyCount;
            param.m_90 *= msg.m_buyCount;
            param.m_94 *= msg.m_buyCount;
            param.m_pair.second = msg.m_buyCount * param.m_pair.second;
        }
    }
    // ---- 余额校验：任一货币不足 → 10 ----
    bool insufficient =
        ((unsigned int)user->getCurCharacInvenR()->get_money() < (unsigned int)param.m_80) ||
        ((unsigned int)user->GetCera() < (unsigned int)param.m_84) ||
        ((unsigned int)user->GetWinPoint() < (unsigned int)param.m_88) ||
        ((unsigned int)user->GetFinishPoint() < (unsigned int)param.m_8c) ||
        ((param.m_pair.first != -1 &&
          param.m_resultItem.m_addInfo2 < param.m_pair.second)) ||
        ((unsigned int)user->GetGuildPowerWarPoint() < (unsigned int)param.m_90) ||
        ((short)user->GetUserPowerWarPoint() < (short)param.m_94);
    if (insufficient)
    {
        return 10;
    }
    // 公会点数商品仅会长可购（vtable slot +0x28 复读）
    if (param.m_a8->GetGuildPowerWarPoint() != 0 && !user->IsGuildMaster())
    {
        return 0x24;
    }
    return 0;
}

// ORIG 0x081bec5e T statistic_process（报告 statistic_process.md ## 反编译 C 全文映射）：
// 卖价估值入 VALUE_STATISTIC_FIELD(6)、货值入 (0xd)，材料对入 CCubeStatistic(0x6a)。
void DisPatcher_BuyItem::statistic_process(CUser* user, BuyItemParam& param)
{
    int sellPrice = 0;
    G_Store()->GetSellItemPrice(param.m_item, param.m_a8,
                                (short)param.m_item.m_addInfo2, false, sellPrice);
    CValueStatistic* valueStat = GetInstanceValueStatistic();
    valueStat->AddValueStatistic((VALUE_STATISTIC_FIELD)6, user,
                                 (unsigned int)sellPrice);
    valueStat->AddValueStatistic((VALUE_STATISTIC_FIELD)0xd, user,
                                 (unsigned int)param.m_80);
    GetInstanceCubeStatistic()->collectCubeStatistics(
        param.m_pair.first, param.m_pair.second, user, (CUBE_STATISTIC_FIELD)0x6a);
}

// ORIG 0x081bdc7e T process（报告 process.md ## 反编译 C 全文映射）：
// check_error → get_data → 死亡塔特殊商品(0x29f2a7)状态/塔位校验 → make_item 填充
// Inven_Item → 入包 insertItemIntoInventory(reason 0,log=1,b=0) → 生物物品特判
// （IsCreatureItem && GetItemType()==0x16 → InsertCreatureItem）→ 公会 OnBuyItem →
// use_money(m_80, reason 0, log) → 有材料则 delete_item(1, m_a4, m_9c, 0x1a, 1) →
// _onBuyItemEvent/statistic_process → WriteBuyItem/SendAradEventItemLog/
// eventDeliveryBuyItem(msg+0x15)。失败路径 SendCmdErrorPacket(0x15, 码) 返 -1。
int DisPatcher_BuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    BuyItemParam& data = static_cast<BuyItemParam&>(param);
    MSG_BUY_ITEM& req = *reinterpret_cast<MSG_BUY_ITEM*>(&msg);

    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, (unsigned char)errNo);
        return -1;
    }
    if (errNo < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, 0x15);
        return LineFunc(0x1a1,
                        "virtual int DisPatcher_BuyItem::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, user->get_acc_id());
    }
    errNo = get_data(user, req, data);
    if (errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, (unsigned char)errNo);
        return -1;
    }
    if (data.m_pair.first == 0x29f2a7)
    {   // 死亡塔专属商品：须非交易中且已开通对应死亡塔
        if (user->get_state() == 3)
        {
            return LineFunc(0x1b4,
                            "virtual int DisPatcher_BuyItem::process(CUser*, MSG_BASE&, ParamBase&)",
                            0, 0);
        }
        if (G_CGameManager()->getBossTower(user->getBossTowerIndex()) == 0)
        {
            return LineFunc(0x1b8,
                            "virtual int DisPatcher_BuyItem::process(CUser*, MSG_BASE&, ParamBase&)",
                            0, 0);
        }
    }
    data.m_item.m_addInfo = req.m_itemIdx;      // ORIG *(int*)(param+6)=*(int*)(msg+0xd)
    data.m_a8->make_item(data.m_item);          // vtable slot +0x08
    if (!data.m_a8->IsCreatureItem() && !data.m_a8->IsAvatarItem())
    {   // 非生物/非时装：数量取自 msg+0x11（写 m_item.m_addInfo2 @param+0xb）
        data.m_item.m_addInfo2 = req.m_buyCount;
    }
    data.m_a0 = user->getCurCharacInvenW()->insertItemIntoInventory(
        data.m_item, (eItemAddReason)0, true, false);
    if (data.m_a0 < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, 4);
        return -1;
    }
    CDataManager* dataMgr = G_CDataManager();
    CItem* shopItem = dataMgr->find_item(data.m_item.m_addInfo);
    if (shopItem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15, 0x13);
        return -1;
    }
    if (shopItem->IsCreatureItem() && shopItem->GetItemType() == 0x16)
    {   // 生物物品：按有效期限写入生物装备栏
        int expirationDate = shopItem->getExpirationDate();
        int usablePeriod = shopItem->getUsablePeriod();
        user_creature::CCreatureMgr* creatureMgr =
            user->getCurCharacInvenW()->GetCreatureMgrW();
        creatureMgr->InsertCreatureItem(&data.m_item, data.m_a0, 0,
                                        usablePeriod, expirationDate);
    }
    CGuildServerProxy* guildProxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    guildProxy->OnBuyItem(user, data.m_a8,
                          (unsigned int)data.m_90, (unsigned int)data.m_94);
    user->getCurCharacInvenW()->use_money(data.m_80, (eMoneySubReason)0, true);
    if (data.m_pair.first != -1)
    {   // 消耗材料（如代币/兑换券）：delete_item(INVEN_TYPE 1, 槽, 数量, 0x1a, log)
        user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, data.m_a4,
                                                data.m_pair.second,
                                                (eItemDelReason)0x1a, true);
    }
    _onBuyItemEvent(data, user);
    statistic_process(user, data);
    HistoryLog::WriteBuyItem((FILE*)user->m_field796f8,
                             data.m_item.m_addInfo, data.m_item.m_addInfo2);
    user->SendAradEventItemLog((unsigned long)data.m_item.m_addInfo,
                               data.m_item.m_addInfo2);
    user->eventDeliveryBuyItem(req.m_deliveryType, data.m_item);
    return 0;
}

DisPatcher_RentAvatar::DisPatcher_RentAvatar() {}
DisPatcher_RentAvatar::~DisPatcher_RentAvatar() {}
int DisPatcher_RentAvatar::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_RentAvatar::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_RentAvatar::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232d86 W _ZN21DisPatcher_RentAvatar7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ee59c)（租期/价格计算与 IPG 记录）

DisPatcher_ExtendAvatar::DisPatcher_ExtendAvatar() {}
DisPatcher_ExtendAvatar::~DisPatcher_ExtendAvatar() {}
int DisPatcher_ExtendAvatar::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ExtendAvatar::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ExtendAvatar::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232c4c W _ZN23DisPatcher_ExtendAvatar7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现；ORIG dispatch_sig(0x081eddec) 同样为纯 ret（报告 docs/class_func_reports/DisPatcher_ExtendAvatar/dispatch_sig.md），该分发器整体为空壳

DisPatcher_SellItem::DisPatcher_SellItem() {}
DisPatcher_SellItem::~DisPatcher_SellItem() {}
int DisPatcher_SellItem::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SellItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SellItem::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232f0e W _ZN19DisPatcher_SellItem7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f2d0c)（状态检查/Secu 保护/Store::user_sell_item）

DisPatcher_RepairEquip::DisPatcher_RepairEquip() {}
DisPatcher_RepairEquip::~DisPatcher_RepairEquip() {}
int DisPatcher_RepairEquip::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_RepairEquip::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_RepairEquip::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x081c6082 T _ZN22DisPatcher_RepairEquip7processEP5CUserR8MSG_BASER9ParamBase
// err>0：SendCmdErrorPacket(cmd=0x19, err&0xff) 返回 -1；err<0：LineFunc(0x1500,pretty,0,acc_id)
// 并返回其值；err==0：G_Store()->repair_equip(user, msg[0xd], (s16)msg+0xe, (u16)msg+0x10,
// 出参 param+0xc) 写 param.m_errNo，成功(==0)时回显 m_8=msg[0xd]、m_a=(u16)msg+0xe；返回 0。
int DisPatcher_RepairEquip::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x19, (unsigned char)(errNo & 0xff));
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0x1500,
                        "virtual int DisPatcher_RepairEquip::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, user->get_acc_id());
    }
    MSG_REPAIR_EQUIP& req = *reinterpret_cast<MSG_REPAIR_EQUIP*>(&msg);
    stRepairEquipParam& out = static_cast<stRepairEquipParam&>(param);
    Store* store = G_Store();  // ORIG thunk 0x8151add = _Z7G_Storev
    out.m_errNo = store->repair_equip(user, (char)req.m_0d, req.m_0e, req.m_10,
                                      out.m_c);  // 出参 ushort& @ param+0xc（ORIG asm -0x2c(%ebp)）
    if (out.m_errNo == 0)
    {
        out.m_8 = req.m_0d;                    // movzbl 0xd(%eax) -> mov %dl,0x8
        out.m_a = (unsigned short)req.m_0e;    // movzwl 0xe(%eax) -> mov %dx,0xa（原位型回显）
    }
    return 0;
}


DisPatcher_SetTradeState::DisPatcher_SetTradeState() {}
DisPatcher_SetTradeState::~DisPatcher_SetTradeState() {}
int DisPatcher_SetTradeState::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetTradeState::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x0823315a W _ZN24DisPatcher_SetTradeState7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部交易状态逻辑（get_state==3 → GetTradeSpace → is_empty/check_trade_possibility →
// change_trade_state / cancel_trade+PutTradeSpace）内嵌于
// dispatch_sig(0x081f6232，报告 docs/class_func_reports/DisPatcher_SetTradeState/dispatch_sig.md)。
int DisPatcher_SetTradeState::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_Compound::DisPatcher_Compound() {}
DisPatcher_Compound::~DisPatcher_Compound() {}
int DisPatcher_Compound::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_Compound::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_Compound::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232a9c W _ZN19DisPatcher_Compound7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081eaa5c)（compound_item 与结果回包）

DisPatcher_DisJointItem::DisPatcher_DisJointItem() {}
DisPatcher_DisJointItem::~DisPatcher_DisJointItem() {}
int DisPatcher_DisJointItem::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DisJointItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DisJointItem::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082333a6 W _ZN23DisPatcher_DisJointItem7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f9fb8)->_disjoint(0x081f92ca)

DisPatcher_Lottery::DisPatcher_Lottery() {}
DisPatcher_Lottery::~DisPatcher_Lottery() {}
int DisPatcher_Lottery::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_Lottery::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x08233280 W _ZN18DisPatcher_Lottery7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部抽奖逻辑（CheckInTrade 拒绝包 0x1d/0xda → lottery_item → 应答包 0x1d →
// 失败时限 3 秒内 addServerHackCnt(0x334) → setLastLotteryTime）内嵌于
// dispatch_sig(0x081f8380，报告 docs/class_func_reports/DisPatcher_Lottery/dispatch_sig.md)。
int DisPatcher_Lottery::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_ChangeSkill::DisPatcher_ChangeSkill() {}
DisPatcher_ChangeSkill::~DisPatcher_ChangeSkill() {}
int DisPatcher_ChangeSkill::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ChangeSkill::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_ChangeSkill::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG 0x081bffbc T _ZN22DisPatcher_ChangeSkill7processEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_ChangeSkill/process.md ## 反编译 C 全文映射）：
// check_error（stub 恒 0）→ param.m_curSlot/m_newSlot ← msg[0xe]/msg[0xf] →
// SkillSlot::skill_move(cur, new, get_charac_job)；>0 时 SendCmdErrorPacket(0x1e) 返 -1。
int DisPatcher_ChangeSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stOnChangeSkillParam& out = static_cast<stOnChangeSkillParam&>(param);
    MSG_CHANGE_SKILLSLOT& req = *reinterpret_cast<MSG_CHANGE_SKILLSLOT*>(&msg);

    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e, (unsigned char)errNo);
        return -1;
    }
    out.m_curSlot = req.m_curSlot;    // ORIG *(u8*)(param+4) = *(u8*)(msg+0xe)
    out.m_newSlot = req.m_newSlot;    // ORIG *(u8*)(param+5) = *(u8*)(msg+0xf)

    SkillSlot* skillSlot = (SkillSlot*)user->getCurCharacSkillW();
    errNo = skillSlot->skill_move(req.m_curSlot, req.m_newSlot,
                                  user->get_charac_job());
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e, (unsigned char)errNo);
        return -1;
    }
    return errNo;
}

DisPatcher_BuySkill::DisPatcher_BuySkill() {}
DisPatcher_BuySkill::~DisPatcher_BuySkill() {}
int DisPatcher_BuySkill::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_BuySkill::check_state(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_BuySkill::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_BuySkill::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG 0x081befe8 T _ZN19DisPatcher_BuySkill7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_BuySkill/process.md 全文转写）：
//   check_error>0 → SendCmdErrorPacket(0x1f, err) 返 -1；<0 → LineFunc(0x3ef, pretty, err, acc_id)。
//   count==1 单条模式：entry0 → master/refund_skill_process，回写 m_354/m_350。
//   count!=1 批量模式：backupSkill 失败 → m_350=-1、m_354=infos[0].m_0（ORIG 读未初始化，忠实保留）
//   返 0；成功则两遍循环（try_* 试算 → 正式 master/refund），info.m_8<0 短路返 0。
//   四个 helper（T 符号）当前无 TU 定义，见 DisPatcherCore.h 声明。
int DisPatcher_BuySkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int errNo = check_error(user, msg);            // ORIG -0x1c(%ebp)
    if (errNo >= 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1f, (unsigned char)errNo);
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0x3ef,
                        "virtual int DisPatcher_BuySkill::process(CUser*, MSG_BASE&, ParamBase&)",
                        errNo, user->get_acc_id());
    }

    // ORIG 在错误分支之后才把 param/msg 存入局部（-0x18/-0x14），声明位置保持一致
    stOnBuySkillParam& out = static_cast<stOnBuySkillParam&>(param);
    MSG_BUY_SKILL& req = *reinterpret_cast<MSG_BUY_SKILL*>(&msg);
    out.m_355 = req.m_count;
    if (req.m_count == 1)
    {   // 单条模式（只读 entry0；ORIG 不经 backupSkill）
        out.m_infos[0].m_0 = req.m_entries[0].m_skillIdx;
        out.m_infos[0].m_1 = req.m_entries[0].m_level;
        if (req.m_entries[0].m_refundFlag == 0)
        {
            master_new_skill_process(user, (char)req.m_treeKind, out.m_infos[0]);
        }
        else
        {
            refund_skill_process(user, (char)req.m_treeKind, out.m_infos[0]);
        }
        out.m_354 = out.m_infos[0].m_0;
        out.m_350 = out.m_infos[0].m_4;
    }
    else
    {   // 批量模式：先备份当前技能树
        ((SkillSlot*)user->getCurCharacSkillW())->set_parent(user);
        if (!((SkillSlot*)user->getCurCharacSkillW())
                 ->backupSkill((ENUM_SKILL_TREE_KIND)(signed char)req.m_treeKind))
        {   // backupSkill 失败：ORIG 读未初始化的 infos[0].m_0 写 m_354 —— 忠实转写
            out.m_350 = -1;
            out.m_354 = out.m_infos[0].m_0;
            return 0;
        }
        int i;
        for (i = 0; (signed char)req.m_count > i; ++i)
        {   // 第一遍：try_* 试算
            out.m_infos[i].m_0 = req.m_entries[i].m_skillIdx;
            out.m_infos[i].m_1 = req.m_entries[i].m_level;
            if (req.m_entries[i].m_refundFlag == 0)
            {
                try_master_new_skills_process(user, (char)req.m_treeKind, out.m_infos[i]);
            }
            else
            {
                try_refund_skills_process(user, (char)req.m_treeKind, out.m_infos[i]);
            }
            out.m_354 = out.m_infos[i].m_0;
            out.m_350 = out.m_infos[i].m_4;
            if (out.m_infos[i].m_4 < 0)
            {
                return 0;
            }
        }
        for (i = 0; (signed char)req.m_count > i; ++i)
        {   // 第二遍：正式执行
            out.m_infos[i].m_0 = req.m_entries[i].m_skillIdx;
            out.m_infos[i].m_1 = req.m_entries[i].m_level;
            if (req.m_entries[i].m_refundFlag == 0)
            {
                master_new_skill_process(user, (char)req.m_treeKind, out.m_infos[i]);
            }
            else
            {
                refund_skill_process(user, (char)req.m_treeKind, out.m_infos[i]);
            }
            out.m_354 = out.m_infos[i].m_0;
            out.m_350 = out.m_infos[i].m_4;
            if (out.m_infos[i].m_4 < 0)
            {
                return 0;
            }
        }
    }
    return errNo;   // 该路径 errNo==0，与 ORIG `mov -0x1c(%ebp),%eax` 一致
}

// ---------------------------------------------------------------------------
// DisPatcher_BuySkill 四个 helper（ORIG T 符号；各报告 ## 汇编+## 反编译 C 全文转写）。
// buySkillInfo 布局（PacketTypes.h）：m_0@+0(u8 技能号)/m_1@+1(u8)/m_4@+4(int 结果)/
// m_8@+8(int 等级出参)。公会技能区间 [200,208]（cmp $0xc7 jbe / cmp $0xd0 ja 门）。
// ---------------------------------------------------------------------------

// ORIG 0x081bf952 T _ZN19DisPatcher_BuySkill24master_new_skill_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/master_new_skill_process.md 全文转写）：
// 非公会分支（+0x1bf..+0x2ad）：树门 treeIdx∈{-1,0} 时 treeKind==1 返回 / 其余
// treeKind!=1 返回 → SkillSlot::master_new_skill(idx, level, job, first, second,
// (s8)m_1)，结果写 m_4，≥0 时 m_8←getCurCharacSkillR().get_skill_level(m_4)。
// 公会分支（+0x02b..+0x1ba）：IsGuildMaster/IsSubGuildMaster 皆假直落尾部（沿用
// 调用前遗留 m_4 判定）；否则 master_new_guild_skill(...,needGold,&info.m_8,needGSP,
// *(u16*)(GetGuildDBInfo()+0x42))，成功经 GlobalData::s_guild_proxy_mgr 发送
// SendBuyGuildSkill(get_charac_no(-1), get_charac_guildkey(), m_0, info.m_8,
// needGSP, needGold)。
// 尾部（+0x2b3）：m_4>=0 时 HistoryLog::WriteLearnSkill(m_field796f8, m_0, 当前等级)。
void DisPatcher_BuySkill::master_new_skill_process(CUser* user, char treeKind,
                                                   buySkillInfo& info)
{
    if ((unsigned int)info.m_0 >= 200 && (unsigned int)info.m_0 <= 208)
    {   // ---- 公会技能分支（ORIG +0x02b..+0x1ba；正区间判定使公会块内联、
        //      ja→非公会，与 ORIG 块排布一致）----
        if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
        {
            // 权限不足：ORIG 直落 +0x2af→尾部（不写 m_4，沿用调用前遗留值）
        }
        else
        {
            unsigned short guildLevel = *reinterpret_cast<unsigned short*>(
                static_cast<char*>(user->GetGuildDBInfo()) + 0x42);           // ebx
            int money = user->getCurCharacMoney();                      // -0x54
            STGuildDBInfoOnly* guildDB =
                static_cast<STGuildDBInfoOnly*>(user->GetGuildDBInfo());// -0x50
            int secondGrow =
                static_cast<int>(user->getCurCharSecondGrowType());           // -0x4c
            int firstGrow =
                static_cast<int>(user->getCurCharFirstGrowType());            // -0x48
            int job = user->get_charac_job();       // ORIG edi
            int needGold;   // ORIG -0x20：调用前未初始化，由出参引用写出
            short needGSP;  // ORIG -0x22：同上（&别名传参）
            SkillSlot* skillRG = (SkillSlot*)(user->getCurCharacSkillR());
            info.m_4 = skillRG->master_new_guild_skill(
                static_cast<unsigned char>(info.m_0), job, firstGrow, secondGrow,
                *guildDB, money, needGold,
                /*learnCnt=*/info.m_8,  // ORIG &info+8 别名 int& 出参
                needGSP, static_cast<int>(guildLevel));
            if (info.m_4 >= 0)
            {   // ORIG +0x132..+0x1ba：读回出参并发公会服务器包
                CGuildServerProxy* proxy =
                    GlobalData::s_guild_proxy_mgr->GetServerProxy(
                        user->GetServerGroup());
                proxy->SendBuyGuildSkill(
                    static_cast<unsigned int>(user->get_charac_no(-1)),
                    static_cast<unsigned int>(user->get_charac_guildkey()),
                    static_cast<unsigned char>(info.m_0),
                    /*level(=出参 learnCnt 回读)=*/info.m_8,
                    /*needGSP=*/needGSP, static_cast<unsigned int>(needGold));
            }
        }
    }
    else
    {   // ---- 非公会技能分支（ORIG +0x1bf..+0x2ad）----
        int treeIdx = user->GetCurCharacSkillTreeIndex();               // -0x1c
        if (treeIdx == -1 || treeIdx == 0)
        {
            if (treeKind == 1)
            {
                return;
            }
        }
        else if (treeKind != 1)
        {
            return;
        }
        int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));       // -0x38
        int secondGrow =
            static_cast<int>(user->getCurCharSecondGrowType());                       // -0x34
        int firstGrow =
            static_cast<int>(user->getCurCharFirstGrowType());                        // -0x30
        int job = user->get_charac_job();             // ORIG esi
        int level = user->get_charac_level();         // ORIG ebx
        SkillSlot* skillW = (SkillSlot*)(user->getCurCharacSkillW());
        info.m_4 = skillW->master_new_skill(
            static_cast<unsigned char>(info.m_0), level, job, firstGrow,
            secondGrow, addLevel);
        if (info.m_4 >= 0)
        {
            SkillSlot* skillR = (SkillSlot*)(user->getCurCharacSkillR());
            info.m_8 = skillR->get_skill_level(info.m_4);
        }
    }
    if (info.m_4 >= 0)
    {   // ---- 尾部（ORIG +0x2b3）----
        SkillSlot* skillRT = (SkillSlot*)(user->getCurCharacSkillR());
        int curLevel = skillRT->get_skill_level(info.m_4);
        HistoryLog::WriteLearnSkill(reinterpret_cast<FILE*>(user->m_field796f8),
                                    static_cast<unsigned char>(info.m_0),
                                    curLevel);
    }
}

// ORIG 0x081bfc5c T _ZN19DisPatcher_BuySkill20refund_skill_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/refund_skill_process.md 全文转写）：
// 无树门/无权限门（treeKind 存 -0x2c 后未再读取）；refundCount 局部清零后取址；
// SkillSlot::refund_skill(idx, level, job, first, second, (s8)m_1, &refundCount)
// 写 m_4，≥0 时 m_8←当前等级。ORIG 不读回 refundCount 值。
void DisPatcher_BuySkill::refund_skill_process(CUser* user, char /*treeKind*/,
                                               buySkillInfo& info)
{
    int refundCount = 0;                                                              // -0x1c
    int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));        // -0x38
    int secondGrow = static_cast<int>(user->getCurCharSecondGrowType());        // -0x34
    int firstGrow = static_cast<int>(user->getCurCharFirstGrowType());          // -0x30
    int job = user->get_charac_job();               // ORIG esi
    int level = user->get_charac_level();           // ORIG ebx
    SkillSlot* skillW = (SkillSlot*)(user->getCurCharacSkillW());
    info.m_4 =
        skillW->refund_skill(static_cast<unsigned char>(info.m_0), level, job,
                             firstGrow, secondGrow, addLevel, refundCount);
    if (info.m_4 >= 0)
    {
        SkillSlot* skillR = (SkillSlot*)(user->getCurCharacSkillR());
        info.m_8 = skillR->get_skill_level(info.m_4);
    }
}

// ORIG 0x081bfd3c T _ZN19DisPatcher_BuySkill25try_refund_skills_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/try_refund_skills_process.md 全文转写）：
// 与 refund 同参形态调 try_refund_skill；仅写 m_4，无等级回读、无任何门。
void DisPatcher_BuySkill::try_refund_skills_process(CUser* user, char /*treeKind*/,
                                                    buySkillInfo& info)
{
    int refundCount = 0;                                                              // -0x1c
    int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));        // -0x38
    int secondGrow = static_cast<int>(user->getCurCharSecondGrowType());        // -0x34
    int firstGrow = static_cast<int>(user->getCurCharFirstGrowType());          // -0x30
    int job = user->get_charac_job();               // ORIG esi
    int level = user->get_charac_level();           // ORIG ebx
    SkillSlot* skillW = (SkillSlot*)(user->getCurCharacSkillW());
    info.m_4 =
        skillW->try_refund_skill(static_cast<unsigned char>(info.m_0), level,
                                 job, firstGrow, secondGrow, addLevel,
                                 refundCount);
}

// ORIG 0x081bfdf0 T _ZN19DisPatcher_BuySkill29try_master_new_skills_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/try_master_new_skills_process.md 全文转写）：
// 公会技能 [200,208] 直接返回（cmp $0xd0 jbe → 尾声；与正式 master 相反）；
// 非公会走同一树门后调 try_master_new_skills，仅写 m_4，无等级回写、无日志尾部。
void DisPatcher_BuySkill::try_master_new_skills_process(CUser* user, char treeKind,
                                                        buySkillInfo& info)
{
    if ((unsigned int)info.m_0 >= 200 && (unsigned int)info.m_0 <= 208)
    {   // 公会技能 [200,208]：直接返回（ORIG cmp $0xd0 jbe → 尾声）
        return;
    }
        int treeIdx = user->GetCurCharacSkillTreeIndex();
        if (treeIdx == -1 || treeIdx == 0)
        {
            if (treeKind == 1)
            {
                return;
            }
        }
        else if (treeKind != 1)
        {
            return;
        }
        int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));    // -0x38
        int secondGrow =
            static_cast<int>(user->getCurCharSecondGrowType());                       // -0x34
        int firstGrow =
            static_cast<int>(user->getCurCharFirstGrowType());                        // -0x30
        int job = user->get_charac_job();           // ORIG esi
        int level = user->get_charac_level();       // ORIG ebx
        SkillSlot* skillW = (SkillSlot*)(user->getCurCharacSkillW());
        info.m_4 = skillW->try_master_new_skills(
            static_cast<unsigned char>(info.m_0), level, job, firstGrow,
            secondGrow, addLevel);
}

DisPatcher_IncreStatus::DisPatcher_IncreStatus() {}
DisPatcher_IncreStatus::~DisPatcher_IncreStatus() {}
int DisPatcher_IncreStatus::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_IncreStatus::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x08232cc2 W _ZN22DisPatcher_IncreStatus7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部加点逻辑（get_short 属性号 → SecuProtectionField GetItemPtr/CheckItem 守卫 →
// <0x99 increase_status / >=0x99 UseQuestItem+cMyTrace）内嵌于
// dispatch_sig(0x081ede34，报告 docs/class_func_reports/DisPatcher_IncreStatus/dispatch_sig.md)。
int DisPatcher_IncreStatus::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_QuestAction::DisPatcher_QuestAction() {}
DisPatcher_QuestAction::~DisPatcher_QuestAction() {}
int DisPatcher_QuestAction::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_QuestAction::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x08232d24 W _ZN22DisPatcher_QuestAction7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部任务动作逻辑（PvPSkilTree 频道拒绝/at(1) 命令码 0x21..0x24 分派 → find_quest/
// setBlindTimeGateStateQuest/quest_action 四参/GM quest flag）内嵌于
// dispatch_sig(0x081ee010，报告 docs/class_func_reports/DisPatcher_QuestAction/dispatch_sig.md)。
int DisPatcher_QuestAction::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_SetPos::DisPatcher_SetPos() {}
DisPatcher_SetPos::~DisPatcher_SetPos() {}
int DisPatcher_SetPos::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPos::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
// ORIG 0x081c98c8 T _ZN17DisPatcher_SetPos7processEP5CUserR8MSG_BASER9ParamBase
int DisPatcher_SetPos::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    if (err >= 1)
    {
        return -1;
    }
    if (err < 0)
    {   // LineFunc(0x1cb8, pretty, 0, acc_id)——第三参恒 0、第四参 acc_id（ORIG 实证）
        return LineFunc(0x1cb8,
                        "virtual int DisPatcher_SetPos::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, user->get_acc_id());
    }
    MSG_USER_SET_POS& req = *reinterpret_cast<MSG_USER_SET_POS*>(&msg);
    G_GameWorld()->move_position(user,
                                 (int)req.m_0d,          // x（ushort 零扩展）
                                 (int)req.m_0f,          // y
                                 (int)req.m_11,          // z（sbyte 符号扩展）
                                 (unsigned short)req.m_12);  // dir
    return err;   // 该分支 err==0，与 ORIG `mov -0x20(%ebp),%eax` 一致
}

DisPatcher_SetArea::DisPatcher_SetArea() {}
DisPatcher_SetArea::~DisPatcher_SetArea() {}
int DisPatcher_SetArea::check_error(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // TODO(ORIG)（三参 ABI 见 _ZN18DisPatcher_SetArea11check_errorEP5CUserR8MSG_BASER9ParamBase）
int DisPatcher_SetArea::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_SetArea::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x081c951c T _ZN18DisPatcher_SetArea7processEP5CUserR8MSG_BASER9ParamBase
int DisPatcher_SetArea::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stSetAreaParam& out = static_cast<stSetAreaParam&>(param);
    out.m_errNo = check_error(user, msg, param);   // ORIG check_error 为三参
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {   // LineFunc(0x1c6d, pretty, m_errNo, acc_id)；ORIG 直接返回其值
        return LineFunc(0x1c6d,
                        "virtual int DisPatcher_SetArea::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }
    MSG_SET_USER_AREA& req = *reinterpret_cast<MSG_SET_USER_AREA*>(&msg);
    G_GameWorld()->move_area(user,
                             (int)req.m_0d,           // vill（sbyte）
                             (int)req.m_0e,           // area（sbyte）
                             (int)req.m_0f,           // x（ushort）
                             (int)req.m_11,           // y（ushort）
                             (int)req.m_13,           // z（sbyte）
                             true,                    // flag 恒 1
                             0,                       // param 恒 0
                             (int)req.m_14,           // prevVill（ushort）
                             (int)req.m_16);          // prevArea（ushort）
    return 0;
}

DisPatcher_Fish::DisPatcher_Fish() {}
DisPatcher_Fish::~DisPatcher_Fish() {}
int DisPatcher_Fish::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x08232cae W _ZN15DisPatcher_Fish7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
// —— 原始二进制即为空实现（无 process 报告，W 内联弱符号）。
int DisPatcher_Fish::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }
// ORIG 0x081eddf6 T _ZN15DisPatcher_Fish12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_Fish/dispatch_sig.md 全文转写）：
//   状态非 4（钓鱼中）直接返 0；为 4 时调 GameWorld::fishing，恒返 0。
int DisPatcher_Fish::dispatch_sig(CUser* user, PacketBuf& /*packet*/)
{
    if (user->get_state() != 4)
    {
        return 0;
    }
    G_GameWorld()->fishing(user);
    return 0;
}

// ---------------------------------------------------------------------------
// ORIG W 0x08595a6c _ZN21DisPatcher_FinishLoad{C1,C2}Ev —— 无独立报告（该类报告目录
// 仅 process/check_error/Party_hack_check/read），objdump 0x8595a6c 转写：
// call 模板基类实例 C1（IPacketDispatcher<MSG_FINISH_LOADING,stFinishLoadParam,
// ch_state0>::C1，ORIG 0x85984c6；PacketTypes.cpp 显式实例化）后写最终虚表指针。
// ---------------------------------------------------------------------------
DisPatcher_FinishLoad::DisPatcher_FinishLoad() {}
// ORIG T 0x08264bfe _ZN21DisPatcher_FinishLoad11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_FinishLoad/check_error.md ## 汇编 + ## 反编译 C
// 全文转写）。ORIG mov 0xc(%ebp) 在隐式 this(+0x8) 之后即第一具名参数 user——
// Ghidra 伪 C 的 "GetDebugCommand((CUser*)param_2)" 为其省略 this 的编号假象，
// 实参就是 user。GetDebugCommand()==1 → 0x7fffffff，否则 0；
// if/else 形态对应 ORIG sete/test/je 分支骨架。
int DisPatcher_FinishLoad::check_error(CUser* user, MSG_BASE& /*msg*/)
{
    if (user->GetDebugCommand() == 1)
    {
        return 0x7fffffff;
    }
    return 0;
}
// 队伍 >2 人时逐成员核对：0 号记录 job/growType/IP 前缀/前三技能 index，
// 其余成员与之全等（job/growType 不等或 IP 网络前缀不等 → 0x330 多开计数清标；
// 技能三槽不全等 → 仅清 0x32f 同 IP 标）。任一失败即短路置双标志为 0。
void DisPatcher_FinishLoad::Party_hack_check(CUser* user)
{   // ORIG 栈位映射：-0x40 sameIp / -0x3f multiBox / -0x3e job(sbyte) /
    // -0x3d growType / -0x50 baseIpPrefix[16] / -0x3c,-0x38,-0x34 = 技能三槽 index。
    char sameIp = 1;
    char multiBox = 1;
    signed char jobByte = 0;
    signed char growType = 0;
    unsigned int skillIdx0 = 0;
    unsigned int skillIdx1 = 0;
    unsigned int skillIdx2 = 0;
    char basePrefix[16];              // -0x50：0 号成员 IP 最后一个 '.' 前缀

    CParty* party = static_cast<CParty*>(user->GetParty());   // ORIG _ZN5CUser8GetPartyEv
    if (party == 0)
    {
        return;
    }
    if (party->get_member_count() <= 2)
    {
        return;
    }
    memset(basePrefix, 0, sizeof(basePrefix));
    for (int idx = 0; idx < 4; ++idx)
    {
        if (!party->checkValidUser(idx))
        {
            continue;
        }
        CUser* member = party->get_user(idx);
        if (member == 0)
        {
            continue;
        }
        if (idx == 0)
        {   // 记录基准：job/growType/IP 网络前缀/技能三槽
            char peerIp[16];
            memset(peerIp, 0, sizeof(peerIp));
            member->m_network.GetPeerIP2(peerIp, 0x10);
            if (strlen(peerIp) <= 8)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            char* dot = strrchr(peerIp, '.');
            if (dot == 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            size_t fullLen = strlen(peerIp);
            size_t dotLen = strlen(dot);
            strncpy(basePrefix, peerIp, fullLen - dotLen);
            // 现声明为 const char*（ORIG 实返回 SkillSlot*），按位转写
            SkillSlot* skill = reinterpret_cast<SkillSlot*>(
                const_cast<char*>(member->getCurCharacSkillR()));
            if (skill == 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            jobByte = (signed char)member->get_charac_job();   // ORIG mov %al + movsbl 比较
            growType = member->getCurCharFirstGrowType();
            skillIdx0 = skill->get_skill_index(0);
            skillIdx1 = skill->get_skill_index(1);
            skillIdx2 = skill->get_skill_index(2);
        }
        else
        {
            if ((signed char)member->get_charac_job() != jobByte ||
                member->getCurCharFirstGrowType() != growType)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            char peerIp[16];
            memset(peerIp, 0, sizeof(peerIp));
            member->m_network.GetPeerIP2(peerIp, 0x10);
            if (strncmp(basePrefix, peerIp, strlen(basePrefix)) != 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            SkillSlot* skill = reinterpret_cast<SkillSlot*>(
                const_cast<char*>(member->getCurCharacSkillR()));
            if (skill == 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            if (skill->get_skill_index(0) != skillIdx0 ||
                skill->get_skill_index(1) != skillIdx1 ||
                skill->get_skill_index(2) != skillIdx2)
            {
                sameIp = 0;   // 仅清同 IP 标志，循环继续（ORIG jmp +0x377）
                continue;
            }
        }
    }
    if (multiBox)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x330, 1, 0, 0);
        if (sameIp)
        {
            user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x32f, 1, 0, 0);
        }
    }
}
// ORIG 0x0826503c T _ZN21DisPatcher_FinishLoad7processEP5CUserR8MSG_BASER9ParamBase
int DisPatcher_FinishLoad::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stFinishLoadParam& out = static_cast<stFinishLoadParam&>(param);
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {   // LineFunc(0x6ca, pretty, m_errNo, 0)——第四参恒 0（ORIG 无 get_acc_id 调用）
        return LineFunc(0x6ca,
                        "virtual int DisPatcher_FinishLoad::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, 0);
    }
    if (Global::CBossTowerFunc::FinishLoad(user))   // 死亡塔(0xa)/Boss 塔(0xc) 自行收尾
    {
        return 0;
    }
    if (advancealtar::Manager::finishLoad(user))    // 祭坛进行中则接管
    {
        return 0;
    }
    Party_hack_check(user);
    MSG_FINISH_LOADING& req = *reinterpret_cast<MSG_FINISH_LOADING*>(&msg);
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        party->finish_loading(user);
        if (party->checkStartRoom())
        {   // 开房校验后核对客户端 checksum，不符记 0x25b/0x25c
            if (user->getCurCharacInvenCheckSum() != req.m_0d)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
            }
            if (user->getCurCharacSkillCheckSum() != req.m_11)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x25c, 1, 0, 0);
            }
        }
    }
    return 0;
}
// ---------------------------------------------------------------------------
// ORIG W 0x08265540 D1 / 0x08265570 D0 _ZN21DisPatcher_FinishLoadD{1,0}Ev
// —— 无独立报告；objdump：写最终虚表指针后调模板基类实例 D1（0x08266d9e），
// 删除版追加 operator delete。空体由编译器按同形态生成，与同类
// DisPatcher_Fish/DisPatcher_UseSkill 写法一致。
// ---------------------------------------------------------------------------
DisPatcher_FinishLoad::~DisPatcher_FinishLoad() {}
// ORIG W 0x08265492 _ZN21DisPatcher_FinishLoad11check_stateEP5CUserR9PacketBuf
// （无独立报告；objdump 全文转写）。Get_State_Check(user->get_state()) 为真
// （state ∈ {5,6,12,13}）→ 0，否则 0x7fffffff——xor $1/test/je 分支骨架与
// 上方 check_error 注释一致。依赖 Global::CGlobalFunc::Get_State_Check
// （ORIG T 0x081466f0，声明 BossTowerFunc.h、定义 BossTowerFunc.cpp）。
int DisPatcher_FinishLoad::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (!Global::CGlobalFunc::Get_State_Check(
            static_cast<ch_state>(user->get_state())))
    {
        return 0x7fffffff;
    }
    return 0;
}
// ORIG T 0x08264b66 _ZN21DisPatcher_FinishLoad4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_FinishLoad/read.md ## 汇编 + ## 反编译 C
// 全文转写）。两次 PacketBuf::get_int 分别写 m_0d/m_11（msg+0x0d/+0x11 具名
// 字段）；任一失败即以 LineFunc(0x5f7/0x5f8) 返回（第三、四参恒 0），全成返回 0。
int DisPatcher_FinishLoad::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_FINISH_LOADING& req = *reinterpret_cast<MSG_FINISH_LOADING*>(&msg);
    if (!packet.get_int(req.m_0d))
    {
        return LineFunc(0x5f7,
                        "virtual int DisPatcher_FinishLoad::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(req.m_11))
    {
        return LineFunc(0x5f8,
                        "virtual int DisPatcher_FinishLoad::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

DisPatcher_UseSkill::DisPatcher_UseSkill() {}
DisPatcher_UseSkill::~DisPatcher_UseSkill() {}
// ORIG 0x081cad80 T _ZN19DisPatcher_UseSkill11check_errorEP5CUserR8MSG_BASER9ParamBase
// get_state(user)==5 → 0；否则 0x7fffffff（报告 DisPatcher_UseSkill/check_error.md 全文）。
int DisPatcher_UseSkill::check_error(CUser* user, MSG_BASE& /*msg*/, ParamBase& /*param*/)
{
    return (user->get_state() == 5) ? 0 : 0x7fffffff;
}
int DisPatcher_UseSkill::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_UseSkill::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ---------------------------------------------------------------------------
// TU 局部镜像（ORIG 全局类型；权威头未落地，符号由本 TU 独家提供）：
//   SKILL_USING_VALIDATE_INFO —— ORIG W ctor 0x08234756 C1Eihcc（int,uchar,char,char）
//   布局按 ctor 逐字节：+0x00 int job / +0x04 u8 slot / +0x05 s8 firstGrowType /
//   +0x06 s8 secondGrowType（sizeof 8 含尾 pad）。
//   CHackLog_InvalidUsingSkill —— ORIG T 四方法（无 this 使用，按 static 成员声明，
//     mangled 与 ORIG 完全一致）；IsValidUsingSkill 调 find_skill + CSkill
//     growtype 校验 + IsStealable，GetHackType 恒 0x339。
// 权威头落地后整体迁出本 TU。
// ---------------------------------------------------------------------------
struct SKILL_USING_VALIDATE_INFO
{
    int m_job;               // +0x00
    unsigned char m_slot;    // +0x04
    char m_firstGrowType;    // +0x05
    char m_secondGrowType;   // +0x06

    SKILL_USING_VALIDATE_INFO(int job, unsigned char slot,
                              char firstGrowType, char secondGrowType)
    {   // ORIG 0x08234756：四成员顺序赋值
        m_job = job;
        m_slot = slot;
        m_firstGrowType = firstGrowType;
        m_secondGrowType = secondGrowType;
    }
};

class CHackLog_InvalidUsingSkill
{
public:
    static bool IsValidUsingSkill(const SKILL_USING_VALIDATE_INFO* info);   // 0x082864fc
    static bool IsSkillMatchFirstGrowType(const CSkill* skill, char type);  // 0x082865ca
    static bool IsSkillMatchSecondGrowType(const CSkill* skill, char a, char b);  // 0x08286612
    static int GetHackType();                                               // 0x08286656
};

// ORIG 0x082865ca：for i in 0..5: skill->m_fitGrowType[i](+0xcc+4i) == type → true
bool CHackLog_InvalidUsingSkill::IsSkillMatchFirstGrowType(const CSkill* skill, char type)
{
    for (int i = 0; i <= 5; ++i)
    {
        if (skill->m_fitGrowType[i] == (int)type)
        {
            return true;
        }
    }
    return false;
}

// ORIG 0x08286612：get_second_growtype_max_level(first, second) > 0
bool CHackLog_InvalidUsingSkill::IsSkillMatchSecondGrowType(const CSkill* skill,
                                                            char a, char b)
{
    return skill->get_second_growtype_max_level((int)a, (int)b) > 0;
}

// ORIG 0x08286656：恒返 0x339
int CHackLog_InvalidUsingSkill::GetHackType()
{
    return 0x339;
}

// ORIG 0x082864fc：find_skill(job, slot)==0 → false；
// 首一转类型命中 / 二转上限>0 / IsStealable(first, second) 任一成立 → true。
bool CHackLog_InvalidUsingSkill::IsValidUsingSkill(const SKILL_USING_VALIDATE_INFO* info)
{
    CSkill* skill = G_CDataManager()->find_skill(info->m_job, (int)info->m_slot);
    if (skill == 0)
    {
        return false;
    }
    if (IsSkillMatchFirstGrowType(skill, info->m_firstGrowType))
    {
        return true;
    }
    if (IsSkillMatchSecondGrowType(skill, info->m_firstGrowType, info->m_secondGrowType))
    {
        return true;
    }
    return skill->IsStealable((int)info->m_firstGrowType, (int)info->m_secondGrowType);
}

// ORIG 0x081cadaa T _ZN19DisPatcher_UseSkill7processEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseSkill/process.md ## 反编译 C 全文映射）：
// param.m_errNo=check_error；>=1 直接返回 0；<0 LineFunc(0x1ff2,err,acc_id)；
// ==0 时构造校验信息 → 非法使用记黑客计数（addServerHackCnt(type,1,slot,0)），
// 有队伍则 CParty::use_skill(user, slot, target)，恒返 0。
int DisPatcher_UseSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUseSkillParam& out = static_cast<stUseSkillParam&>(param);
    MSG_USE_SKILL& req = *reinterpret_cast<MSG_USE_SKILL*>(&msg);

    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x1ff2,
                        "virtual int DisPatcher_UseSkill::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }

    SKILL_USING_VALIDATE_INFO info(user->get_charac_job(), req.m_slot,
                                   user->getCurCharFirstGrowType(),
                                   user->getCurCharSecondGrowType());
    if (!CHackLog_InvalidUsingSkill::IsValidUsingSkill(&info))
    {
        user->getHackAnalyzer()->addServerHackCnt(user,
                                                  (WongWork::ENUM_HACKTYPE)CHackLog_InvalidUsingSkill::GetHackType(),
                                                  1, (unsigned int)req.m_slot, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party != 0)
    {
        party->use_skill(user, (int)req.m_slot, (int)req.m_targetId);
    }
    return 0;
}

DisPatcher_DieMob::DisPatcher_DieMob() {}
DisPatcher_DieMob::~DisPatcher_DieMob() {}
int DisPatcher_DieMob::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DieMob::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DieMob::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232afe W _ZN17DisPatcher_DieMob7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081eb0c4)（读 MSG_MONSTER_DIE 各字段并按 get_state 分派 CParty::kill_monster/CDeathTower::handleKillMonster/Global::CBossTowerFunc::DieMob/advancealtar::Manager::dieMob/WarRoom::HandleMonsterKill，报告 docs/class_func_reports/DisPatcher_DieMob/dispatch_sig.md）

DisPatcher_DieCharacInDungeon::DisPatcher_DieCharacInDungeon() {}
DisPatcher_DieCharacInDungeon::~DisPatcher_DieCharacInDungeon() {}
int DisPatcher_DieCharacInDungeon::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DieCharacInDungeon::check_state(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DieCharacInDungeon::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_DieCharacInDungeon::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// CParty::m_dungeon（+0xcac，CParty.h private）外部只读视图 —— 与 CTraceMobDieHack.cpp
// 同款口径：ORIG process 直读 party+0xcac（_ZN5CUser8GetPartyEv 后 mov 0xcac(%eax)）。
struct CPartyDungeonView
{
    char m_pad[0xcac];
    CDungeon* m_dungeon;
};

// ---------------------------------------------------------------------------
// ORIG 0x081c2a12 T _ZN29DisPatcher_DieCharacInDungeon7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_DieCharacInDungeon/process.md 全文转写，含汇编分支次序）。
// m_errNo=check_error；>0 返 0；<0 LineFunc(0xcbb,pretty,err,acc_id) 返回其值；
// ==0：CharacDie(getCurCharacName, msg+0xd, msg+0xf) → GetPartyIndex→G_CGameManager()->GetParty
// 备用队 → CBossTowerFunc::DieCharacInDungeon 为假时：state==0xd 先 advancealtar dieCharac；
// 无队/无 dungeon/dungeon->m_bloodDungeonType(signed)<1：备用队非空则 die_user，返 0；
// 否则 BloodHandleDieUser + m_errNo=0x7fffffff，返 0。CBossTowerFunc 为真直接返 0。
int DisPatcher_DieCharacInDungeon::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    static const char* const kPretty =
        "virtual int DisPatcher_DieCharacInDungeon::process(CUser*, MSG_BASE&, ParamBase&)";
    MSG_DIE_CHARACTER& req = *reinterpret_cast<MSG_DIE_CHARACTER*>(&msg);
    stDieCharacInDungeon& out = static_cast<stDieCharacInDungeon&>(param);

    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0xcbb, kPretty, out.m_errNo, user->get_acc_id());
    }

    // ORIG 读包体 ushort：+0xf 先入 esi、+0xd 后入 ebx，作 CharacDie 第 2/3 参
    const unsigned short fieldF =
        *reinterpret_cast<const unsigned short*>(req.m_pad + 2);   // +0xf
    const unsigned short fieldD =
        *reinterpret_cast<const unsigned short*>(req.m_pad);       // +0xd
    user->m_historyLog.CharacDie(user->getCurCharacName(), fieldD, fieldF);
    // ORIG thunk：0x822fe4e=GetPartyIndex、0x80cc18e=G_CGameManager、0x8294f32=GetParty(int)
    CParty* partyByIdx = G_CGameManager()->GetParty(user->GetPartyIndex());

    if (!Global::CBossTowerFunc::DieCharacInDungeon(user))
    {
        if (user->get_state() == 0xd)
        {
            advancealtar::Manager::dieCharac(user);
        }

        CParty* myParty = static_cast<CParty*>(user->GetParty());
        CPartyDungeonView* view =
            reinterpret_cast<CPartyDungeonView*>(myParty);
        if (myParty == 0 || view->m_dungeon == 0 ||
            static_cast<signed char>(view->m_dungeon->m_bloodDungeonType) < 1)
        {
            if (partyByIdx != 0)
            {
                partyByIdx->die_user(user);
            }
        }
        else
        {
            myParty->BloodHandleDieUser(user);
            out.m_errNo = 0x7fffffff;
        }
    }
    return 0;
}

DisPatcher_UseCoin::DisPatcher_UseCoin() {}
DisPatcher_UseCoin::~DisPatcher_UseCoin() {}
int DisPatcher_UseCoin::check_error(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // TODO(ORIG)（三参 ABI 见 _ZN18DisPatcher_UseCoin11check_errorEP5CUserR8MSG_BASER9ParamBase）
int DisPatcher_UseCoin::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_UseCoin::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x081ca852 T _ZN18DisPatcher_UseCoin7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo = check_error(user,msg,param)（三参）；>=1 返回 0；<0 LineFunc(0x1f4d,pretty,err,acc_id)
// 并返回其值；==0：m_8(u16)=msg.m_0d；state==5：GetParty 判空 LineFunc(0x1f5a,...,0,acc_id)，
// checkFreeRevivalCondition(0) 为假时 UseCoin(eCoinSubReason 0) + set_authen_data(3,1,0) +
// party->IncreaseUsedCoinCount + APSystem ClearActionAndSendtoUser(0x25)；state==8：
// UseCoin(eCoinSubReason 4) + set_authen_data(3,1,0) + user->IncreaseUsedCoinCount；返回 0。
int DisPatcher_UseCoin::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUseCoinParam& out = static_cast<stUseCoinParam&>(param);
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x1f4d,
                        "virtual int DisPatcher_UseCoin::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }
    MSG_USE_COIN& req = *reinterpret_cast<MSG_USE_COIN*>(&msg);
    out.m_8 = req.m_0d;  // movzwl 0xd(%eax) -> mov %dx,0x8(%eax)
    int state = user->get_state();
    if (state == 5)
    {
        CParty* party = (CParty*)user->GetParty();  // ORIG thunk 0x865514c = CUser::GetParty
        if (party == 0)
        {
            return LineFunc(0x1f5a,
                            "virtual int DisPatcher_UseCoin::process(CUser*, MSG_BASE&, ParamBase&)",
                            0, user->get_acc_id());
        }
        if (!party->checkFreeRevivalCondition((CDungeon const*)0))
        {
            CInventory* inven = user->getCurCharacInvenW();  // thunk 0x80da28e
            inven->UseCoin(eCoinSubReason_0);                // asm push $0x0
            UserQuest* quest = user->getCurCharacQuestW();   // thunk 0x814aa5e
            quest->set_authen_data(QUEST_CONDITION_3, 1, 0); // asm (3,1,0)
            party->IncreaseUsedCoinCount();                  // thunk 0x822d8ce = CParty::IncreaseUsedCoinCount
            APSystem::_ActionGroupIndex group;               // ORIG 首参 0x25（_ActionGroupIndex）
            group.m_value = 0x25;
            APSystem::CUserProc::ClearActionAndSendtoUser(user, group, 0,
                                                          (ENUM_PACKETCLASS)0);
        }
    }
    else if (state == 8)
    {
        CInventory* inven = user->getCurCharacInvenW();
        inven->UseCoin((eCoinSubReason)4);  // asm push $0x4（枚举仅建模了 0 值）
        UserQuest* quest = user->getCurCharacQuestW();
        quest->set_authen_data(QUEST_CONDITION_3, 1, 0);
        user->IncreaseUsedCoinCount();      // thunk 0x823082a = CUser::IncreaseUsedCoinCount
    }
    return 0;
}


DisPatcher_GiveUpGame::DisPatcher_GiveUpGame() {}
DisPatcher_GiveUpGame::~DisPatcher_GiveUpGame() {}
int DisPatcher_GiveUpGame::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_GiveUpGame::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x081c40b4 T _ZN21DisPatcher_GiveUpGame7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_GiveUpGame/process.md 全文转写）。
// err>0 返 -1；<0 LineFunc(0xff7,pretty,0,acc_id) 返回其值；==0 按 state 分派：
// 10=getDeathTower 非空 handleLeaveUser；0xc=CBossTowerFunc::GiveUpGame；
// 0xd=advancealtar::Manager::giveUpGame；其它=GetParty 非空 giveup_game(user,true,false,false)。
// 尾部返回 check_error 原值（ORIG mov -0x14,%eax）。param 不写。
int DisPatcher_GiveUpGame::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    static const char* const kPretty =
        "virtual int DisPatcher_GiveUpGame::process(CUser*, MSG_BASE&, ParamBase&)";
    (void)param;   // ORIG 不写 param

    const int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0xff7, kPretty, 0, user->get_acc_id());
    }

    const int state = user->get_state();
    if (state == 10)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower != 0)
        {
            tower->handleLeaveUser(user);
        }
    }
    else if (state == 0xc)
    {
        Global::CBossTowerFunc::GiveUpGame(user);
    }
    else if (state == 0xd)
    {
        advancealtar::Manager::giveUpGame(user);
    }
    else
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            party->giveup_game(user, true, false, false);
        }
    }
    return errNo;
}

DisPatcher_GetItem::DisPatcher_GetItem() {}
DisPatcher_GetItem::~DisPatcher_GetItem() {}
int DisPatcher_GetItem::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_GetItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_GetItem::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x081c35c0 T _ZN18DisPatcher_GetItem7processEP5CUserR8MSG_BASER9ParamBase
// m_errNo=check_error；>=1 返回 0；<0 LineFunc(0xe5c,pretty,err,acc_id) 返回其值；
// ==0：反外挂前置（msg.m_17!=1 计 0x3bb；m_15%7!=0 计 0x12f 带 m_11/m_13）→
// state==5 队伍拾取 / 10 死塔 handlePickupItem / 0xc CBossTowerFunc::GetItem /
// 8 WarRoom::HandleGetItem / 其它 m_errNo=0x13；未完成路径置 m_errNo=0x7fffffff。
int DisPatcher_GetItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stGetItemParam& out = static_cast<stGetItemParam&>(param);
    MSG_GET_ITEM& req = *reinterpret_cast<MSG_GET_ITEM*>(&msg);
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0xe5c,
                        "virtual int DisPatcher_GetItem::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }
    WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();  // thunk 0x80f995a
    if (req.m_17 != 1)
    {
        hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x3bb, 1, 0, 0);
    }
    if (req.m_15 % 7 != 0)
    {
        hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x12f, 1, req.m_11, req.m_13);
    }
    // [BLOCKED 依赖缺失] cUserHistoryLog::SetItemXYPos（ORIG W 0x0822f084
    // _ZN15cUserHistoryLog12SetItemXYPosEtt）无头文件声明且全仓无定义：
    // 待补齐后于 addServerHackCnt(0x12f) 之后调用
    // user->m_historyLog.SetItemXYPos(req.m_1a, req.m_1c)。
    int state = user->get_state();
    if (state == 5)
    {
        CParty* party = (CParty*)user->GetParty();
        if (party != 0)
        {
            if (party->IsAutoCreated())  // thunk 0x822d88c = CParty::IsAutoCreated（nm 实证，非 IsRoutingState）
            {
                out.m_errNo = 0xd8;
                return 0;
            }
            // [BLOCKED 依赖缺失] 反外挂三门前置（ORIG asm +0x18f..+0x2bd）：
            //  1. CUser::GetItemTimeChecker（_ZN5CUser18GetItemTimeCheckerEjttj W 0x082302ac）
            //     + CUser +0x8e444/+0x8e446 两 short 字段（>500 判定）→ 命中计 0x130；
            //  2. CUser::GetItemSerialChecker（_ZN5CUser20GetItemSerialCheckerEtti W 0x0823066a，
            //     参 (m_1a,m_1c,m_0d)==2 → 计 0x131；
            //  三者均无头文件声明/全仓定义，字段未建模。补齐后依次位于 get_item 之前。
            // [BLOCKED 依赖缺失] 核心分配链（ORIG asm +0x2e5..+0x56b）：
            //  CParty::get_item（_ZN6CParty8get_itemEP5CUseribcRiRhRb T 0x85a3b98，
            //  参 (user,(int)m_0d,m_0f==1,(char)m_10,int&,uchar&,bool&) 返回 bool）
            //  无任何 TU 定义。其后为：checkGetItemPacket 门（_Z18checkGetItemPacketiii
            //  T 0x889209d 亦无定义；命中计 0x12e）、CBattle_Field(party+0xb24)::
            //  get_dungeon_index → G_CDataManager()->find_dungeon(idx) 判空 logFormat
            //  ("...find_dungeon(%d) fail", 0xf14)、dungeon->m_entryRestrictFlag && pos∈{2,3,4} &&
            //  出参 bool 为假时 PacketGuard put_header(0,0x56)/put_byte(5)/put_short(
            //  get_unique_id&0xffff)/put_int(itemId)/finalize(true) 后 GameWorld::
            //  send_all 广播；结束 m_errNo=0。
            out.m_errNo = 0x7fffffff;  // get_item 失败路径同款状态码（asm +0x715）
            return 0;
        }
        out.m_errNo = 0x7fffffff;
        return 0;
    }
    if (state == 10)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();  // thunk 直调 0x86552a4
        if (tower != 0)
        {
            if (!tower->handlePickupItem(user, req.m_0d,
                                         req.m_0f == 1, (char)req.m_10))
            {
                out.m_errNo = 0x7fffffff;
                return 0;
            }
            out.m_errNo = 0;
            return 0;
        }
        out.m_errNo = 0x7fffffff;
        return 0;
    }
    if (state == 0xc)
    {
        // [BLOCKED 依赖缺失] Global::CBossTowerFunc::GetItem（ORIG T 0x08146994
        // _ZN6Global14CBossTowerFunc7GetItemEP5CUseribc，参 (user,m_0d,m_0f==1,(char)m_10)）
        // 类与函数均未声明、全仓无定义。补齐后调用并按 asm +0x659 置 m_errNo=0x7fffffff。
        out.m_errNo = 0x7fffffff;
        return 0;
    }
    if (state == 8)
    {
        WarRoom* room = G_CGameManager()->GetWarRoom(
            user->GetWarRoomIndex());  // thunk 0x822fe84/0x80cc18e/0x82a25d0
        if (room == 0)
        {
            return LineFunc(0xf32,
                            "virtual int DisPatcher_GetItem::process(CUser*, MSG_BASE&, ParamBase&)",
                            0, user->get_acc_id());
        }
        if (!room->HandleGetItem(user, req.m_0d))
        {
            out.m_errNo = 0x7fffffff;
            return 0;
        }
        out.m_errNo = 0;
        return 0;
    }
    out.m_errNo = 0x13;
    return 0;
}


DisPatcher_UseStack::DisPatcher_UseStack() {}
DisPatcher_UseStack::~DisPatcher_UseStack() {}
int DisPatcher_UseStack::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_UseStack::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
// ORIG 0x082331bc W _ZN19DisPatcher_UseStack7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制
// 即为空实现（read 同为空）。UseStack 全部业务（读包/校验/应答 0x2f 系列）内嵌于
// dispatch_sig(0x081f63c0，见 docs/class_func_reports/DisPatcher_UseStack/dispatch_sig.md)。
int DisPatcher_UseStack::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }


DisPatcher_MoveMap::DisPatcher_MoveMap() {}
DisPatcher_MoveMap::~DisPatcher_MoveMap() {}
int DisPatcher_MoveMap::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_MoveMap::check_state(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_MoveMap::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
// ORIG 0x081c4b7a T _ZN18DisPatcher_MoveMap20CheckTrapBitsHackCntEP5CUserR12MSG_MOVE_MAP
// msg.m_18 ushort 非 7 的倍数 → 记 0x1f9；累计 >4 时对队伍其余成员记 0x1fa、
// 落库 DB_ReqSaveHackUserPunish 并 DisConnSig(0x18,1,0) 强制断线。
void DisPatcher_MoveMap::CheckTrapBitsHackCnt(CUser* user, MSG_MOVE_MAP& info)
{
    // ORIG 为 0x2493 魔数除法序列：q=v/7 后 r=v-q*7，r!=0 即非整除
    unsigned short trapBits = *reinterpret_cast<unsigned short*>(
        reinterpret_cast<char*>(&info) + 0x18);
    if (trapBits % 7 == 0)
    {
        return;
    }
    user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x1f9, 1, 0, 0);
    if (user->getHackAnalyzer()->getServerHackAccumulatedCnt((WongWork::ENUM_HACKTYPE)0x1f9) <= 4)
    {
        return;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return;
    }
    for (int idx = 0; idx < 4; ++idx)
    {
        if (!party->checkValidUser(idx))
        {
            continue;
        }
        CUser* member = party->get_user(idx);
        if (member != user)
        {
            member->getHackAnalyzer()->addServerHackCnt(
                member, (WongWork::ENUM_HACKTYPE)0x1fa, 1, 0, 0);
        }
        DB_ReqSaveHackUserPunish::makeRequest(member->get_acc_id(), 0,
                                              member->m_network.get_str_ip());
        member->DisConnSig((DISCONN_SIG)0x18, true, 0);
    }
}
// ORIG 0x081c4d26 T _ZN18DisPatcher_MoveMap26CheckOneMapPlayTimeHackCntEP5CUserR12MSG_MOVE_MAP
// 以队伍地图游玩时长（死亡塔为 stage 通关时限/1000+5）为基准，逐成员修正
// msg.m_23[i] 上报 tick：>10000 部分按万位上报（0x2afe 副本记 0x82 否则 0x77）；
// job3 / job4 觉醒2 减半 ×0.5 否则 ×0.8；未开局且 >limit 时按 t<=1.5*limit
// 单次、超幅 ×10 次记 0x74/0x81。
void DisPatcher_MoveMap::CheckOneMapPlayTimeHackCnt(CUser* user, MSG_MOVE_MAP& info)
{
    unsigned int limit = 0;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return;
    }
    if (user->get_state() == 0xa)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower != 0)
        {
            unsigned int clearTime =
                (unsigned int)tower->getCStage()->getStageClearTime();
            limit = clearTime / 1000 + 5;
        }
    }
    else
    {
        limit = party->GetMapPlayingTime() / 1000;
    }
    for (int idx = 0; idx < 4; ++idx)
    {
        if (!party->checkValidUser(idx))
        {
            continue;
        }
        CUser* member = party->get_user(idx);
        if (member == 0)
        {
            continue;
        }
        unsigned int& slot = info.m_23[idx];
        if (slot > 10000)
        {
            unsigned int quotient = slot / 10000;
            unsigned int remainder = slot % 10000;
            // CParty.h 数据区已 public 化（Prod_CParty），恢复直接成员访问
            // （ORIG process 直读 party+0xcac m_dungeon，语义一致）。
            CDungeon* dungeon =
                static_cast<CParty*>(member->GetParty())->m_dungeon;
            if (dungeon == 0)
            {
                member->getHackAnalyzer()->addServerHackCnt(
                    member, (WongWork::ENUM_HACKTYPE)0x77, 1, quotient, 0);
            }
            else if (dungeon->get_index() == 0x2afe)
            {
                member->getHackAnalyzer()->addServerHackCnt(
                    member, (WongWork::ENUM_HACKTYPE)0x82, 1, quotient, 0);
            }
            else
            {
                member->getHackAnalyzer()->addServerHackCnt(
                    member, (WongWork::ENUM_HACKTYPE)0x77, 1, quotient, 0);
            }
            slot = remainder;
        }
        int job = member->get_charac_job();
        bool halfTick = (job == 3) ||
                        (job == 4 && member->getCurCharFirstGrowType() == 2);
        // ORIG .rodata：0x8bd6320=0.5 / 0x8bd6328=0.8（fildll×const 后 fistpll 截断）
        slot = (unsigned int)((double)slot * (halfTick ? 0.5 : 0.8));

        CParty* memberParty = static_cast<CParty*>(member->GetParty());
        // m_padBattleField1(+0xb24) 现为 private 且无访问器，按 ORIG 裸偏移取视图
        CBattle_Field& battleField =
            *reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(memberParty) + 0xb24);
        if (!battleField.check_start_point() && slot > limit)
        {
            if ((double)slot <= 1.5 * (double)limit)   // 0x8bd6330=1.5
            {   // 单次记录
                CDungeon* dungeon = *reinterpret_cast<CDungeon**>(
                    reinterpret_cast<char*>(memberParty) + 0xcac);   // +0xcac m_dungeon
                if (dungeon == 0)
                {
                    member->getHackAnalyzer()->addServerHackCnt(
                        member, (WongWork::ENUM_HACKTYPE)0x74, 1, slot, 0);
                }
                else if (dungeon->get_index() == 0x2afe)
                {
                    member->getHackAnalyzer()->addServerHackCnt(
                        member, (WongWork::ENUM_HACKTYPE)0x81, 1, slot, 0);
                }
                else
                {
                    member->getHackAnalyzer()->addServerHackCnt(
                        member, (WongWork::ENUM_HACKTYPE)0x74, 1, slot, 0);
                }
            }
            else
            {   // 超幅：同一记录重复 ×10（ORIG 循环 10 次，无 break）
                for (int rep = 0; rep < 10; ++rep)
                {
                    CDungeon* dungeon =
                        *reinterpret_cast<CDungeon**>(
                            reinterpret_cast<char*>(memberParty) + 0xcac);
                    if (dungeon == 0)
                    {
                        member->getHackAnalyzer()->addServerHackCnt(
                            member, (WongWork::ENUM_HACKTYPE)0x74, 1, slot, 0);
                    }
                    else if (dungeon->get_index() == 0x2afe)
                    {
                        member->getHackAnalyzer()->addServerHackCnt(
                            member, (WongWork::ENUM_HACKTYPE)0x81, 1, slot, 0);
                    }
                    else
                    {
                        member->getHackAnalyzer()->addServerHackCnt(
                            member, (WongWork::ENUM_HACKTYPE)0x74, 1, slot, 0);
                    }
                }
            }
        }
    }
}
// ORIG 0x081c5330 T _ZN18DisPatcher_MoveMap7processEP5CUserR8MSG_BASER9ParamBase
int DisPatcher_MoveMap::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stMoveMapParam& out = static_cast<stMoveMapParam&>(param);
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {   // LineFunc(0x1268, pretty, m_errNo, acc_id)；ORIG 直接返回其值
        return LineFunc(0x1268,
                        "virtual int DisPatcher_MoveMap::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }
    MSG_MOVE_MAP& req = *reinterpret_cast<MSG_MOVE_MAP*>(&msg);
    if (req.m_1a != 1)
    {   // 客户端未置 0x1a 标志 → 记 0x25a
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25a, 1, 0, 0);
    }
    static_cast<Secu_HackLogCheck*>(user->GetHackCheck())->MoveMap();

    CheckTrapBitsHackCnt(user, req);
    CheckOneMapPlayTimeHackCnt(user, req);

    if (Global::CBossTowerFunc::MoveMap(user, req.m_1b))
    {   // 死亡塔/Boss 塔自行处理换图
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party->IsRoutingState())
    {
        CSpecialItemRoutingManager* mgr =
            G_CGameManager()->GetSpecialItemRoutingManager();
        if (mgr != 0)
        {
            mgr->ProcessFailEtc(party, 0);
        }
    }
    req.m_39 = (unsigned char)party->get_party_seatno(user);
    party->move_map(req);
    return 0;
}

DisPatcher_SetPlayResult::DisPatcher_SetPlayResult() {}
DisPatcher_SetPlayResult::~DisPatcher_SetPlayResult() {}
int DisPatcher_SetPlayResult::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPlayResult::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPlayResult::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233034 W _ZN24DisPatcher_SetPlayResult7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f4ce6)（GetParty → GameResultType 读包/反作弊计数/SetPlayResult，报告 docs/class_func_reports/DisPatcher_SetPlayResult/dispatch_sig.md）

DisPatcher_DropItem::DisPatcher_DropItem() {}
DisPatcher_DropItem::~DisPatcher_DropItem() {}
int DisPatcher_DropItem::check_error(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // TODO(ORIG)（三参 ABI 见 _ZN19DisPatcher_DropItem11check_errorEP5CUserR8MSG_BASER9ParamBase）
int DisPatcher_DropItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_DropItem::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x081c2f94 T _ZN19DisPatcher_DropItem7processEP5CUserR8MSG_BASER9ParamBase
// m_errNo=check_error(user,msg,param)（三参）；>=1：m_8=msg.m_11 返回 0；<0
// LineFunc(0xdcc,pretty,err,acc_id) 返回其值；==0 按 state 分派：
// 5=CGameManager::GetParty(GetPartyIndex) 后 CParty::drop_item（[BLOCKED]）；
// 0xc=getBossTower 虚调 handleDropItem；8=WarRoom::HandleItemDrop>0 时写
// m_errNo/m_8；其它 m_errNo=0x13。
int DisPatcher_DropItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stDropItemParam& out = static_cast<stDropItemParam&>(param);
    MSG_DROP_ITEM& req = *reinterpret_cast<MSG_DROP_ITEM*>(&msg);
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo >= 1)
    {
        out.m_8 = req.m_11;
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0xdcc,
                        "virtual int DisPatcher_DropItem::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_errNo, user->get_acc_id());
    }
    int state = user->get_state();
    if (state == 5)
    {
        // ORIG thunk 0x822fe4e=GetPartyIndex、0x80cc18e=G_CGameManager、0x8294f32=GetParty(int)
        CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party != 0)
        {
            // [BLOCKED 依赖缺失] CParty::drop_item（ORIG T 0x085a63f4
            // _ZN6CParty9drop_itemEP5CUseriicii，参 (user,(int)m_0d,(int)m_0f,
            // (char)m_11,(int)m_12,m_14) 返回 int）无任何 TU 定义（头文件现声明为
            // void，需按 ORIG 改 int）。补齐后此处：
            //   int ret = party->drop_item(user, req.m_0d, req.m_0f, (char)req.m_11,
            //                              req.m_12, req.m_14);
            //   if (ret > 0) { out.m_errNo = ret; out.m_8 = req.m_11; }
        }
        return 0;
    }
    if (state == 0xc)
    {
        // ORIG 经 vtable+0x20（槽 8=handleDropItem，vtable for WongWork::CBossTower
        // @0x8b70260 实测）；getBossTower 等价于 getBossTowerIndex + mgr->getBossTower(idx)
        // （ORIG _ZN5CUser12getBossTowerEv 0x08655220 反汇编实证）。
        WongWork::CBossTower* tower =
            G_CGameManager()->getBossTower(user->getBossTowerIndex());
        if (tower != 0)
        {
            tower->handleDropItem(user, req.m_0d, req.m_0f, (char)req.m_11,
                                  req.m_12, req.m_14);
        }
        return 0;
    }
    if (state == 8)
    {
        WarRoom* room = G_CGameManager()->GetWarRoom(user->GetWarRoomIndex());
        if (room != 0)
        {
            int ret = room->HandleItemDrop(user, req.m_0d, req.m_0f,
                                           (char)req.m_11, req.m_12, req.m_14);
            if (ret > 0)
            {
                out.m_errNo = ret;
                out.m_8 = req.m_11;
            }
        }
        return 0;
    }
    out.m_errNo = 0x13;
    out.m_8 = req.m_11;
    return 0;
}


DisPatcher_UseEquip::DisPatcher_UseEquip() {}
DisPatcher_UseEquip::~DisPatcher_UseEquip() {}
int DisPatcher_UseEquip::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_UseEquip::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_UseEquip::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x081c0e4a T _ZN19DisPatcher_UseEquip7processEP5CUserR8MSG_BASER9ParamBase
// err=check_error；>0：SendCmdErrorPacket(0x33, err&0xff) 返回 -1；==0：
// m_errNo=getCurCharacInvenW()->use_equip((int)(sbyte)msg.m_0d)，m_8=msg.m_0d，
// 返回 err。
int DisPatcher_UseEquip::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x33, (unsigned char)(errNo & 0xff));
        return -1;
    }
    MSG_DECREASE_DURABILITY& req = *reinterpret_cast<MSG_DECREASE_DURABILITY*>(&msg);
    stOnUseEquipParam& out = static_cast<stOnUseEquipParam&>(param);
    CInventory* inven = user->getCurCharacInvenW();  // thunk 0x80da28e
    out.m_errNo = inven->use_equip((int)(signed char)req.m_0d);  // movzbl+movesbl
    out.m_8 = req.m_0d;                              // movzbl 0xd(%eax) -> mov %dl,0x8
    return errNo;
}


DisPatcher_MakePVP::DisPatcher_MakePVP() {}
DisPatcher_MakePVP::~DisPatcher_MakePVP() {}
int DisPatcher_MakePVP::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_MakePVP::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x0823321e W _ZN18DisPatcher_MakePVP7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部建房逻辑（ExtractPacket(0x081f81ba) 解包 MSG_MAKE_PVP_ROOM → CSyncSlangFilter::
// HasSlang 拒绝 0x35/0x9f → MakePVPRoom(cmd=0x35, mode=local_15?6:2)）内嵌于
// dispatch_sig(0x081f803e，报告 docs/class_func_reports/DisPatcher_MakePVP/dispatch_sig.md)。
int DisPatcher_MakePVP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_EnterPVP::DisPatcher_EnterPVP() {}
DisPatcher_EnterPVP::~DisPatcher_EnterPVP() {}
int DisPatcher_EnterPVP::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_EnterPVP::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_EnterPVP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232b88 W _ZN19DisPatcher_EnterPVP7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ed6c8)（读 pvp 索引/密码后 CGameManager::GetPvp → EnterPVPRoom，报告 docs/class_func_reports/DisPatcher_EnterPVP/dispatch_sig.md）

DisPatcher_SetPVPSeat::DisPatcher_SetPVPSeat() {}
DisPatcher_SetPVPSeat::~DisPatcher_SetPVPSeat() {}
int DisPatcher_SetPVPSeat::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPVPSeat::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPVPSeat::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233096 W _ZN21DisPatcher_SetPVPSeat7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f5eba)（seat/state 两 byte → PvP_Room::OnSetPVPSeatState(…,WALKOUT_TYPE_2)，失败 reset+PutPvp 回池，报告 docs/class_func_reports/DisPatcher_SetPVPSeat/dispatch_sig.md）

DisPatcher_SetPVPReady::DisPatcher_SetPVPReady() {}
DisPatcher_SetPVPReady::~DisPatcher_SetPVPReady() {}
int DisPatcher_SetPVPReady::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPVPReady::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_SetPVPReady::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x081c9a14 T _ZN22DisPatcher_SetPVPReady7processEP5CUserR8MSG_BASER9ParamBase
int DisPatcher_SetPVPReady::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    if (err >= 1)
    {
        return -1;
    }
    if (err < 0)
    {   // LineFunc(0x1ce3, pretty, 0, acc_id)
        return LineFunc(0x1ce3,
                        "virtual int DisPatcher_SetPVPReady::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, user->get_acc_id());
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {   // 无房间：LineFunc(0x1ce6, pretty, 0, acc_id)
        return LineFunc(0x1ce6,
                        "virtual int DisPatcher_SetPVPReady::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, user->get_acc_id());
    }
    if (room->get_pvp_battle_mode() == 6)   // 围观/观战模式拒绝
    {
        return -1;
    }
    stOnSetPvPReadyParam& out = static_cast<stOnSetPvPReadyParam&>(param);
    MSG_SET_READY_STATE& req = *reinterpret_cast<MSG_SET_READY_STATE*>(&msg);
    out.m_errNo = room->set_ready_state(user, req.m_0d != 0);
    if (out.m_errNo < 0)
    {   // 全员就绪 → 开赛并广播状态包
        room->start_pvp();
        PacketGuard guard;                       // ORIG local_28（栈上 48B）
        room->make_state_info(reinterpret_cast<char*>(&guard));
        G_GameWorld()->send_all(guard);
        return -1;
    }
    return err;   // 该分支 err==0，与 ORIG `mov -0x18(%ebp),%ebx` 一致
}

DisPatcher_SetPVPTeam::DisPatcher_SetPVPTeam() {}
DisPatcher_SetPVPTeam::~DisPatcher_SetPVPTeam() {}
int DisPatcher_SetPVPTeam::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPVPTeam::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPVPTeam::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082330f8 W _ZN21DisPatcher_SetPVPTeam7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f6020)（get_byte 队伍号 → set_pvp_mode，失败 make_seat_info(0xffff) 广播，报告 docs/class_func_reports/DisPatcher_SetPVPTeam/dispatch_sig.md）

DisPatcher_DiePvP::DisPatcher_DiePvP() {}
DisPatcher_DiePvP::~DisPatcher_DiePvP() {}
int DisPatcher_DiePvP::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DiePvP::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DiePvP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232b74 W _ZN17DisPatcher_DiePvP7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ece64)（get_state==7 时读死亡角色 id → PvP_Room::die_charac + CSwitchLog pvp@log + 应答包，报告 docs/class_func_reports/DisPatcher_DiePvP/dispatch_sig.md）

DisPatcher_PvPTimeout::DisPatcher_PvPTimeout() {}
DisPatcher_PvPTimeout::~DisPatcher_PvPTimeout() {}
int DisPatcher_PvPTimeout::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_PvPTimeout::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_PvPTimeout::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x081c5dec T _ZN21DisPatcher_PvPTimeout7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_PvPTimeout/process.md 全文转写）
// err=check_error（ORIG 0x081c5ebe：get_state!=7 → 0x13）；>0 返回 -1；<0
// LineFunc(0x14b7,pretty,0,acc_id)；==0：room=user->GetPVPRoom()（空则
// LineFunc(0x14bc,pretty,0,acc_id)），否则 ret=room->time_out((int*)(msg+0xd))
// —— msg+0xd 即 read() 读入的 8 个 seat int 数组首址 —— param+0x4=ret，返回 err。
int DisPatcher_PvPTimeout::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    static const char* const kPretty =
        "virtual int DisPatcher_PvPTimeout::process(CUser*, MSG_BASE&, ParamBase&)";
    const int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0x14b7, kPretty, 0, user->get_acc_id());
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());  // thunk 0x0865518e
    if (room == 0)
    {
        return LineFunc(0x14bc, kPretty, 0, user->get_acc_id());
    }
    // MSG_PVP_TIMEOUT 载荷自包头 +0xd 起（read 读 8 个 int），ORIG 直接把该地址
    // 作为 int* 传给 time_out；结果码写入 param+0x4（stOnPvPTimeOut::m_timeOutResult），
    // 由本类 send() 消费（>0 时 put_byte 发出）。局部引用声明次序镜像 ORIG 槽位
    // （param 先于 msg），保证逐指令一致。
    stOnPvPTimeOut& out = static_cast<stOnPvPTimeOut&>(param);
    MSG_BASE& rMsg = msg;
    out.m_timeOutResult =
        room->time_out(reinterpret_cast<int*>(reinterpret_cast<char*>(&rMsg) + 0xd));
    return errNo;
}


DisPatcher_EndPvPResult::DisPatcher_EndPvPResult() {}
DisPatcher_EndPvPResult::~DisPatcher_EndPvPResult() {}
int DisPatcher_EndPvPResult::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)

int DisPatcher_EndPvPResult::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_EndPvPResult::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233344 W _ZN23DisPatcher_EndPvPResult7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f88ae)（set_recv_pvp_end_result_flag → recv_pvp_end_result_all 全齐则 end_pvp_result/make_state_info 广播/end_pvp/battle_reset，报告 docs/class_func_reports/DisPatcher_EndPvPResult/dispatch_sig.md）

DisPatcher_ResPvpRank::DisPatcher_ResPvpRank() {}
DisPatcher_ResPvpRank::~DisPatcher_ResPvpRank() {}
int DisPatcher_ResPvpRank::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ResPvpRank::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
// ORIG 0x08232e98 W _ZN21DisPatcher_ResPvpRank7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现（objdump 核实），全部逻辑位于 dispatch_sig(0x081f0282)：state==7 → MSG_RES_PVP_RANK ctor → ExtractPacket(0x081f039e)/SavePvpRank(0x081f05ea) + LineFunc 断言
int DisPatcher_ResPvpRank::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_SetPVPMapIndex::DisPatcher_SetPVPMapIndex() {}
DisPatcher_SetPVPMapIndex::~DisPatcher_SetPVPMapIndex() {}
int DisPatcher_SetPVPMapIndex::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPVPMapIndex::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetPVPMapIndex::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082332e2 W _ZN25DisPatcher_SetPVPMapIndex7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f8602)（get_short 地图号 → CheckPvPMapMode/SetRequestedMapNo/make_state_info 广播，报告 docs/class_func_reports/DisPatcher_SetPVPMapIndex/dispatch_sig.md）

DisPatcher_DebugCommand::DisPatcher_DebugCommand() {}
DisPatcher_DebugCommand::~DisPatcher_DebugCommand() {}
int DisPatcher_DebugCommand::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DebugCommand::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x08233408 W _ZN23DisPatcher_DebugCommand7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部 GM 命令逻辑（GetCurrentGmMode/IsGm 门禁 → get_packet(0) 取 MSG_DEBUG_COMMAND →
// 0x00..0x30 switch 分派 _debugCommand* 家族 → SendCmdPacket(0x41) 回显）内嵌于
// dispatch_sig(0x081fc4de，报告 docs/class_func_reports/DisPatcher_DebugCommand/dispatch_sig.md；
// 各 _debugCommand*.md 同目录)。

int DisPatcher_DebugCommand::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_AddFriend::DisPatcher_AddFriend() {}
DisPatcher_AddFriend::~DisPatcher_AddFriend() {}
int DisPatcher_AddFriend::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_AddFriend::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

void DisPatcher_AddFriend::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x081cb2e2 T _ZN20DisPatcher_AddFriend7processEP5CUserR8MSG_BASER9ParamBase：
// 薄包装，尾调 process_packet_jpn 并透传返回值（汇编 leave/ret 直传 eax）。
int DisPatcher_AddFriend::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return process_packet_jpn(user, msg, param);
}

// ORIG 0x081cb30a T _ZN20DisPatcher_AddFriend18process_packet_jpnEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_AddFriend/process_packet_jpn.md 全文转写）。
// m_field4=check_error；>0 返 0；<0 logFormat(1,file,pretty,0x20d8,
// "Dispatcher_Impl Check_Error No(%d)",err) 返 0x20d9；==0：StreamPool 取流构造
// CStreamGuard → <<0x33e <<GetUID → GetInBuffer<SIG_CHECK_ADD_FRIEND> 判空置 m_field4=1；
// 否则 sig[+4]=!isGM(acc_id)、strncpy(sig+5, msg+0x11, 0x1e)、MsgQueueMgr put(DB_Q)；返 0。
int DisPatcher_AddFriend::process_packet_jpn(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    static const char* const kFile = "PacketDispatcher_Impl_1.cpp";
    static const char* const kPretty =
        "int DisPatcher_AddFriend::process_packet_jpn(CUser*, MSG_BASE&, ParamBase&)";
    MSG_ADD_FRIEND& req = *reinterpret_cast<MSG_ADD_FRIEND*>(&msg);
    stAddFriendParam& out = static_cast<stAddFriendParam&>(param);

    out.m_field4 = check_error(user, msg);
    if (out.m_field4 > 0)
    {
        return 0;
    }
    if (out.m_field4 < 0)
    {
        LogManager::logFormat(1, kFile, kPretty, 0x20d8,
                              "Dispatcher_Impl Check_Error No(%d)", out.m_field4);
        return 0x20d9;
    }

    Stream* stream = GlobalData::s_stream_pool->Acquire(kFile, 0x20ed);
    CStreamGuard guard(stream, true);
    guard << 0x33e;                              // inter-server SIG 消息号
    guard << static_cast<int>(user->GetUID());   // ORIG 与上一行同为 operator<<(int)
    SIG_CHECK_ADD_FRIEND* sig = guard.GetInBuffer<SIG_CHECK_ADD_FRIEND>();
    if (sig == 0)
    {
        out.m_field4 = 1;
    }
    else
    {
        const bool notGM = !GlobalData::s_pGMAccounts_->isGM(user->get_acc_id());
        sig->m_pad[4] = static_cast<char>(notGM ? 1 : 0);         // +0x04
        strncpy(&sig->m_pad[5],
                reinterpret_cast<const char*>(req.m_pad + 4),     // msg+0x11
                0x1e);                                            // +0x05..+0x22
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);    // QUEUE_IDX 2=DB_Q
    }
    return 0;
}

DisPatcher_RemoveFriend::DisPatcher_RemoveFriend() {}
DisPatcher_RemoveFriend::~DisPatcher_RemoveFriend() {}
int DisPatcher_RemoveFriend::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_RemoveFriend::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_RemoveFriend::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x081cb618 T _ZN23DisPatcher_RemoveFriend7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_RemoveFriend/process.md 全文转写）。
// m_errNo=check_error；>0 返 0；<0 LineFunc(0x2141,pretty,err,acc_id) 返回其值；
// ==0：组 Packet_Monitor_Del_Buddy（ctor 0x0822bcb2 W：PacketHeader(0x674,0x30)+memset(m_name)），
// 填 m_charNo=getCurCharacNo、m_idByChannel=GetUID、memcpy(m_name, msg+0x11, strlen)，
// 经 s_monitor_proxy_mgr GetServerProxy(GetServerGroup())->SendPacket(&pkt, packetSize)，返 0。
int DisPatcher_RemoveFriend::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    static const char* const kPretty =
        "virtual int DisPatcher_RemoveFriend::process(CUser*, MSG_BASE&, ParamBase&)";
    MSG_REMOVE_FRIEND& req = *reinterpret_cast<MSG_REMOVE_FRIEND*>(&msg);
    stRemoveFriendParam& out = static_cast<stRemoveFriendParam&>(param);

    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x2141, kPretty, out.m_errNo, user->get_acc_id());
    }

    Packet_Monitor_Del_Buddy pkt;
    pkt.m_charNo = static_cast<unsigned int>(user->getCurCharacNo());   // +0x0a
    pkt.m_idByChannel = user->GetUID();                                 // +0x0e
    memcpy(pkt.m_name, reinterpret_cast<const char*>(req.m_pad + 4),    // msg+0x11
           strlen(reinterpret_cast<const char*>(req.m_pad + 4)));
    CMonitorServerProxy* proxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);   // +0x02 长度字段(0x30)
    return 0;
}

DisPatcher_Cera::DisPatcher_Cera() {}
DisPatcher_Cera::~DisPatcher_Cera() {}
int DisPatcher_Cera::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_Cera::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_Cera::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082334ae W _ZN15DisPatcher_Cera7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081fd360)（IPGHelper 校验 + LoadMileage）

DisPatcher_EnterWarRoom::DisPatcher_EnterWarRoom() {}
DisPatcher_EnterWarRoom::~DisPatcher_EnterWarRoom() {}
int DisPatcher_EnterWarRoom::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)
{
    // ORIG 0x0820229e：纯 LineFunc 占位（报告 DisPatcher_EnterWarRoom/dispatch_sig.md）。
    return LineFunc(0x97fb, __PRETTY_FUNCTION__, 0, 0);
}
int DisPatcher_EnterWarRoom::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_EnterWarRoom::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233616 W _ZN23DisPatcher_EnterWarRoom7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现；ORIG dispatch_sig(0x0820229e) 亦仅为 LineFunc(0x97fb) 占位，无业务逻辑（报告 docs/class_func_reports/DisPatcher_EnterWarRoom/dispatch_sig.md）

DisPatcher_SetWarRoomSeatState::DisPatcher_SetWarRoomSeatState() {}
DisPatcher_SetWarRoomSeatState::~DisPatcher_SetWarRoomSeatState() {}
int DisPatcher_SetWarRoomSeatState::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)
{
    // ORIG 0x082022ca：纯 LineFunc 占位（报告 DisPatcher_SetWarRoomSeatState/dispatch_sig.md）。
    return LineFunc(0x9848, __PRETTY_FUNCTION__, 0, 0);
}
int DisPatcher_SetWarRoomSeatState::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_SetWarRoomSeatState::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x0823362a W _ZN30DisPatcher_SetWarRoomSeatState7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现；ORIG dispatch_sig(0x082022ca) 亦仅为 LineFunc(0x9848) 占位，无业务逻辑（报告 docs/class_func_reports/DisPatcher_SetWarRoomSeatState/dispatch_sig.md）

DisPatcher_DieWarRoomCharacter::DisPatcher_DieWarRoomCharacter() {}
DisPatcher_DieWarRoomCharacter::~DisPatcher_DieWarRoomCharacter() {}
int DisPatcher_DieWarRoomCharacter::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)
{
    // ORIG 0x082022f6：纯 LineFunc 占位（报告 DisPatcher_DieWarRoomCharacter/dispatch_sig.md）。
    return LineFunc(0x9867, __PRETTY_FUNCTION__, 0, 0);
}
int DisPatcher_DieWarRoomCharacter::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_DieWarRoomCharacter::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x0823363e W _ZN30DisPatcher_DieWarRoomCharacter7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现；ORIG dispatch_sig(0x082022f6) 亦仅为 LineFunc(0x9867) 占位，无业务逻辑（报告 docs/class_func_reports/DisPatcher_DieWarRoomCharacter/dispatch_sig.md）

DisPatcher_StartWarRoomTemp::DisPatcher_StartWarRoomTemp() {}
DisPatcher_StartWarRoomTemp::~DisPatcher_StartWarRoomTemp() {}
int DisPatcher_StartWarRoomTemp::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)
{
    // ORIG 0x08201b06：纯 LineFunc 占位（报告 DisPatcher_StartWarRoomTemp/dispatch_sig.md）。
    return LineFunc(0x9737, __PRETTY_FUNCTION__, 0, 0);
}
int DisPatcher_StartWarRoomTemp::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_StartWarRoomTemp::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082335ee W _ZN27DisPatcher_StartWarRoomTemp7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现；ORIG dispatch_sig(0x08201b06) 亦仅为 LineFunc(0x9737) 占位，无业务逻辑（报告 docs/class_func_reports/DisPatcher_StartWarRoomTemp/dispatch_sig.md）

DisPatcher_ReportClient4Hack::DisPatcher_ReportClient4Hack() {}
DisPatcher_ReportClient4Hack::~DisPatcher_ReportClient4Hack() {}
int DisPatcher_ReportClient4Hack::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ReportClient4Hack::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_ReportClient4Hack::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233742 W _ZN28DisPatcher_ReportClient4Hack7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x082041a0，见 docs/class_func_reports/DisPatcher_ReportClient4Hack/dispatch_sig.md)

DisPatcher_RequestGuildWarInfo::DisPatcher_RequestGuildWarInfo() {}
DisPatcher_RequestGuildWarInfo::~DisPatcher_RequestGuildWarInfo() {}
int DisPatcher_RequestGuildWarInfo::dispatch_sig(CUser* user, PacketBuf& /*packet*/)
{
    // ORIG 0x0820439a（报告 DisPatcher_RequestGuildWarInfo/dispatch_sig.md 全文转写）：
    // state>=2 且当前角色存在时组 Packet_Notice_Guild_War_Info 并经公会代理发送。
    if (user->get_state() >= 2 && user->getCurCharacR() != 0)
    {
        Packet_Notice_Guild_War_Info pkt;
        pkt.m_uid = user->GetUID();
        pkt.m_characNo = user->get_charac_no(-1);
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&pkt), sizeof(pkt));
    }
    return 0;
}
int DisPatcher_RequestGuildWarInfo::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_RequestGuildWarInfo::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233756 W _ZN30DisPatcher_RequestGuildWarInfo7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x0820439a，见 docs/class_func_reports/DisPatcher_RequestGuildWarInfo/dispatch_sig.md)

DisPatcher_PvpHeartBeat::DisPatcher_PvpHeartBeat() {}
DisPatcher_PvpHeartBeat::~DisPatcher_PvpHeartBeat() {}
int DisPatcher_PvpHeartBeat::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_PvpHeartBeat::dispatch_sig(CUser* user, PacketBuf& /*packet*/)
{
    // ORIG 0x08204458（报告 DisPatcher_PvpHeartBeat/dispatch_sig.md 全文转写）。
    if (user->get_state() >= 2 && user->getCurCharacR() != 0)
    {
        disconnect_detecter::GetInstanceDisconnectDetecter()->RefreshRecvTime(user);
    }
    return 0;
}
int DisPatcher_PvpHeartBeat::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x0823376a W _ZN23DisPatcher_PvpHeartBeat7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x08204458，见 docs/class_func_reports/DisPatcher_PvpHeartBeat/dispatch_sig.md)

DisPatcher_MultiBoxLottery::DisPatcher_MultiBoxLottery() {}
DisPatcher_MultiBoxLottery::~DisPatcher_MultiBoxLottery() {}
int DisPatcher_MultiBoxLottery::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_MultiBoxLottery::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_MultiBoxLottery::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233bdc W _ZN26DisPatcher_MultiBoxLottery7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x0820b802，见 docs/class_func_reports/DisPatcher_MultiBoxLottery/dispatch_sig.md)

DisPatcher_UdpCharacteristic::DisPatcher_UdpCharacteristic() {}
DisPatcher_UdpCharacteristic::~DisPatcher_UdpCharacteristic() {}
int DisPatcher_UdpCharacteristic::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int DisPatcher_UdpCharacteristic::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_UdpCharacteristic::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233d6c W _ZN28DisPatcher_UdpCharacteristic7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x0821abe0，见 docs/class_func_reports/DisPatcher_UdpCharacteristic/dispatch_sig.md)

DIspatcher_OneDayLetheTicket::DIspatcher_OneDayLetheTicket() {}
DIspatcher_OneDayLetheTicket::~DIspatcher_OneDayLetheTicket() {}
int DIspatcher_OneDayLetheTicket::dispatch_sig(CUser* user, PacketBuf& /*packet*/)
{
    // ORIG 0x0821b106（报告 DIspatcher_OneDayLetheTicket/dispatch_sig.md 全文转写）。
    static const char kPretty[] =
        "virtual int DIspatcher_OneDayLetheTicket::dispatch_sig(CUser*, PacketBuf&)";
    cMyTrace trace(kPretty, 0xceaa, 0);
    trace("ONE_DAY_LETHE : SKILL_INIT_PACKET");
    if (user == 0)
    {
        return 0;
    }
    if (user->get_state() != 3)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xd5), 1);
        return 0;
    }
    bool blocked;
    if (G_GameWorld()->IsPVPChannel())
    {
        blocked = !G_GameWorld()->IsPvPSkilTreeChannel();
    }
    else
    {
        blocked = false;
    }
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    if (blocked || lethe == 0 || lethe->ResetSkillReq(user) != true)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xd5), 1);
    }
    return 0;
}
int DIspatcher_OneDayLetheTicket::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DIspatcher_OneDayLetheTicket::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233d80 W _ZN28DIspatcher_OneDayLetheTicket7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现（objdump 0x08233d80..0x08233d94 核实），全部逻辑位于 dispatch_sig(0x0821b106：cMyTrace"ONE_DAY_LETHE : SKILL_INIT_PACKET"→state==3→非 PVP/PvPSkillTree 渠道校验→GetPremiumLetheManager().ResetSkillReq(user)，报告 docs/class_func_reports/DIspatcher_OneDayLetheTicket/dispatch_sig.md)

Dispatcer_polic_one_to_one_chat_disable::Dispatcer_polic_one_to_one_chat_disable() {}
Dispatcer_polic_one_to_one_chat_disable::~Dispatcer_polic_one_to_one_chat_disable() {}
int Dispatcer_polic_one_to_one_chat_disable::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082272ce（报告 Dispatcer_polic_one_to_one_chat_disable/dispatch_sig.md）：
    // state>2 且当前角色存在 → 读包组 Packet_Disable_User_OneToOneChat_Police；
    // GM 发起时回填 characNo 并经 Monitor 代理 SendTcpPacket(0x30)。
    if (user->get_state() >= 3 && user->getCurCharacR() != 0)
    {
        Packet_Disable_User_OneToOneChat_Police pkt;
        unsigned int nameLen = 0;
        char name[0x1e] = {0};
        packet.get_int(nameLen);
        packet.get_str(name, static_cast<int>(sizeof(name)), static_cast<int>(nameLen));
        pkt.m_nameLength = nameLen;
        memcpy(pkt.m_name, name, sizeof(pkt.m_name));
        if (user->isGMUser())
        {
            pkt.m_gmId = user->getCurCharacNo();
            CMonitorServerProxy* proxy =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
            proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), sizeof(pkt));
        }
    }
    return 0;
}
int Dispatcer_polic_one_to_one_chat_disable::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int Dispatcer_polic_one_to_one_chat_disable::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082342a8 W _ZN39Dispatcer_polic_one_to_one_chat_disable7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现（objdump 0x082342a8..0x082342bc 核实），全部逻辑位于 dispatch_sig(0x082272ce：state>2 且当前角色存在 → 组 Packet_Disable_User_OneToOneChat_Police(get_int+get_str) → isGMUser 时经 s_monitor_proxy_mgr SendTcpPacket(0x30)，报告 docs/class_func_reports/Dispatcer_polic_one_to_one_chat_disable/dispatch_sig.md)

DisPatcher_CheckSecurityProtection::DisPatcher_CheckSecurityProtection() {}
DisPatcher_CheckSecurityProtection::~DisPatcher_CheckSecurityProtection() {}
int DisPatcher_CheckSecurityProtection::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_CheckSecurityProtection::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_CheckSecurityProtection::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ORIG 0x0826481a T _ZN34DisPatcher_CheckSecurityProtection7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_CheckSecurityProtection/process.md 全文转写）：
// sub $0x28 帧；将 CSecu_ProtectionField::Check(GlobalData::s_pSecuProtectionField,user,0x38)
// 返回值低字节写入 param_3[4]（CheckSecurityProtection 首数据字节 @+0x4 = m_pad[0]），恒返回 0。
int DisPatcher_CheckSecurityProtection::process(CUser* user, MSG_BASE& /*msg*/, ParamBase& param)
{
    CheckSecurityProtection& out = static_cast<CheckSecurityProtection&>(param);
    out.m_4 = static_cast<unsigned char>(
        GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x38)));
    return 0;
}

DisPatcher_EventDungeon_DestoryObject::DisPatcher_EventDungeon_DestoryObject() {}
DisPatcher_EventDungeon_DestoryObject::~DisPatcher_EventDungeon_DestoryObject() {}
int DisPatcher_EventDungeon_DestoryObject::check_error(CUser* /*user*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
int DisPatcher_EventDungeon_DestoryObject::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void DisPatcher_EventDungeon_DestoryObject::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)
// ORIG 0x0812bd8c T _ZN37DisPatcher_EventDungeon_DestoryObject7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_DestoryObject/process.md 全文转写：
//   局部引用槽 -0x8=msg、-0x4=param；*(u16*)(param+0x4)=*(u16*)(msg+0xe)；check_error；返回 0。
//   模板实例化中 ParamBase 栈对象仅 4 字节 vptr（ORIG dispatch_template 0x824aa8c 实证：
//   param@-0x1d 与 msg@-0x19 相邻），param+0x4 写入与相邻 msg 存储重叠为 ORIG 原始行为，忠实转写。）
int DisPatcher_EventDungeon_DestoryObject::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_BASE& rMsg = msg;         // ORIG -0x8(%ebp)
    ParamBase& rParam = param;    // ORIG -0x4(%ebp)
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&rParam) + 0x4) =
        *reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&rMsg) + 0xe);
    check_error(user, rMsg);
    return 0;
}

DisPatcher_EventDungeon_ClearRoom::DisPatcher_EventDungeon_ClearRoom() {}
// ORIG 0x0812c1c6 W _ZN33DisPatcher_EventDungeon_ClearRoomD1Ev/D2Ev/D0Ev（三地址相同）
// objdump 实测 D1：写 vptr=0x8b66068 → 调 IPacketDispatcher<...>::D1 → 恒假 delete 守卫
// —— 即编译器生成的空析构，{} 与其逐指令等价。
DisPatcher_EventDungeon_ClearRoom::~DisPatcher_EventDungeon_ClearRoom() {}

// ORIG 0x085957e8 W _ZN33DisPatcher_EventDungeon_ClearRoomC1Ev（无报告，objdump 转写）：
//   调 IPacketDispatcher<CMDPacketStruct::_STReqEventDungeonClearRoom,
//                        CMDPacketStruct::_STResEventDungeonClearRoom, ch_state3>::C1
//   后写 vptr=0x8b66068。本仓库基类模板实参暂为 MSG_BASE/ParamBase 占位（与 ORIG
//   mangled 不一致；_STRes 需 STBaseResponse 多态建模，待专项批次统一），空体
//   ctor/dtor 由编译器生成基类构造+vptr 写入，结构等价。

// ORIG 0x0812beb0 T _ZN33DisPatcher_EventDungeon_ClearRoom11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/check_error.md 全文转写：
//   push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret）
int DisPatcher_EventDungeon_ClearRoom::check_error(CUser* /*user*/, MSG_BASE& /*msg*/)
{
    return 0;
}

// ORIG 0x0812beba T _ZN33DisPatcher_EventDungeon_ClearRoom7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/process.md 全文转写，
//   含汇编写入次序：param+0x4←msg+0xe、param+0x8←msg+0x12、param+0x6←msg+0x10）；check_error 恒返 0。
int DisPatcher_EventDungeon_ClearRoom::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_BASE& rMsg = msg;         // ORIG -0x8(%ebp)
    ParamBase& rParam = param;    // ORIG -0x4(%ebp)
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&rParam) + 0x4) =
        *reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&rMsg) + 0xe);
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&rParam) + 0x8) =
        *reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&rMsg) + 0x12);
    *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&rParam) + 0x6) =
        *reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&rMsg) + 0x10);
    check_error(user, rMsg);
    return 0;
}

// ORIG 0x0812be82 T _ZN33DisPatcher_EventDungeon_ClearRoom4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/read.md 全文转写）：
// ORIG 汇编含 &msg 的局部落栈（-0x4c(%ebp) spill/reload 对），以具名局部复现。
int DisPatcher_EventDungeon_ClearRoom::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pBuf = reinterpret_cast<char*>(&msg);   // ORIG -0xc(%ebp)
    packet.get_binary(pBuf, 0x14);
    return 0;
}

// ORIG 0x0812bf16 T _ZN33DisPatcher_EventDungeon_ClearRoom4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/send.md 全文转写）：
// put_header(1,0x21c) → put_byte(1) → put_binary(pParam,0xc) → finalize(true) →
// CUser::Send；PacketGuard 栈对象析构（ORIG 含 EH 清理路径）。
// 反编译 C 首行 local_10=param_2：param 引用先落栈（-0xc），guard 位于 -0x18。
void DisPatcher_EventDungeon_ClearRoom::send(CUser* user, ParamBase& param)
{
    ParamBase* pParam = &param;   // ORIG -0xc(%ebp)（local_10，先于 guard ctor 落栈）
    PacketGuard guard;
    guard.put_header(1, 0x21c);
    guard.put_byte(1);
    guard.put_binary(reinterpret_cast<char*>(pParam), 0xc);
    guard.finalize(true);
    user->Send(guard);
}

Dispather_RegenerationRandomOption::Dispather_RegenerationRandomOption() {}
Dispather_RegenerationRandomOption::~Dispather_RegenerationRandomOption() {}
int Dispather_RegenerationRandomOption::check_state(CUser* /*user*/, PacketBuf& /*packet*/) { return 0; }  // TODO(ORIG)
int Dispather_RegenerationRandomOption::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)
void Dispather_RegenerationRandomOption::send(CUser* /*user*/, ParamBase& /*param*/) {}  // TODO(ORIG)

// ---------------------------------------------------------------------------
// TU 局部支撑（随机选项重置/再铸两 process 共用；符号 ORIG 实证）：

// random_option::CRandomOptionItemHandle 权威类定义在 GlobalData.cpp（私有布局
// +0x04 CMTRand* 等，ctor/dtor 由其提供）。本 TU 仅镜像方法签名以引用 ORIG 符号：
//   reset_option  T 0x085f39d2 _ZN13random_option23CRandomOptionItemHandle12reset_optionEm11ENUM_RARITYiP12RandomOption
// 该方法当前无任何 TU 提供定义 → 链接期 blocked（表驱动选项生成链，待专项批次按
// objdump 还原）；此处声明使调用点类型正确。

// statistc_proxy::add —— ORIG T 0x0860fdd2 _ZN14statistc_proxy3addEP5CUserPKcS3_jS3_z
// （objdump 全文转写）：vsprintf 组 key → 三段长度校验(table<=0x20/field<=0x20/key<=0x38，
// 超长静默丢弃) → StatisticsPacket{table@+0xa, field@+0x2b, key@+0x4c, value@+0x85}
// → s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())->SendPacket(pkt,0x89)。
namespace statistc_proxy
{
void add(CUser* user, const char* tableName, const char* fieldName,
         unsigned int value, const char* fmt, ...)
{
    char key[0x400];
    memset(key, 0, sizeof(key));
    va_list args;
    va_start(args, fmt);
    vsprintf(key, fmt, args);   // ORIG 无上限包装，忠实转写
    va_end(args);
    if (strlen(tableName) > 0x20 || strlen(fieldName) > 0x20 || strlen(key) > 0x38)
    {
        return;
    }
    StatisticsPacket packet;
    strcpy(packet.m_tableName, tableName);
    strcpy(packet.m_fieldName, fieldName);
    strcpy(packet.m_key, key);
    packet.m_value = value;
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(&packet), 0x89);
}
}  // namespace statistc_proxy



DispatcherResetRandomOption::DispatcherResetRandomOption() {}
DispatcherResetRandomOption::~DispatcherResetRandomOption() {}
int DispatcherResetRandomOption::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/) { return 0; }  // TODO(ORIG)

// ORIG 0x08682d3a T _ZN15cUserHistoryLog12RandomOptionERK10Inven_ItemN18randomOptionReason1TE
// （docs/class_func_reports/cUserHistoryLog/RandomOption.md + objdump 全文转写）：
// m_user->LogHistory("RandomOptionItem,%d,%02X×14,%d", item.m_addInfo,
//   item.m_random.m_random 的 14 字节（+0x25..+0x32）逐字节, reason)。无 m_user 空检查。
void cUserHistoryLog::RandomOption(const Inven_Item& item, randomOptionReason::T reason)
{
    const unsigned char* opt =
        reinterpret_cast<const unsigned char*>(&item.m_random.m_random);
    m_user->LogHistory(
        "RandomOptionItem,%d,%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d",
        item.m_addInfo,
        opt[0], opt[1], opt[2], opt[3], opt[4], opt[5], opt[6],
        opt[7], opt[8], opt[9], opt[10], opt[11], opt[12], opt[13],
        (int)reason);
}

// ORIG 0x081d6af4 T _ZN34Dispather_RegenerationRandomOption7processEP5CUserR8MSG_BASER9ParamBase
// （报告 Dispather_RegenerationRandomOption/process.md ## 反编译 C 全文映射）：
// 两装备槽逐槽校验（非空/可穿装/已有选项/IsRandomOption/Restrict(1,0x10)/安全保护
// 0x1b/CheckItemLock）→ 全部 m_field0==1 时可再封印 → reSealCount=ROUND(0.5+(r0+r1)/2)
// → RegenerationROI::Regenerate(usableLv0,usableLv1,opt0,opt1,&cost,newItem,user)
// → 统计 regen_cnt/regen_gold → use_money(cost,0x2f,1) → delete_item(1,槽,1,0x2e,1)×2
// → newItem 置封印标志与次数 → insertItemIntoInventory(reason 0x36,1,1) →
// SendUpdateItemList ×3 + cUserHistoryLog::RandomOption(reason 1)。
// 错误码写 param.m_errNo；插入槽位写 param.m_insertedSlot。
int Dispather_RegenerationRandomOption::process(CUser* user, MSG_BASE& msg,
                                                ParamBase& param)
{
    stRegenROIResult& out = static_cast<stRegenROIResult&>(param);
    MSG_REGENERATION_RANDOM_OPTION& req =
        *reinterpret_cast<MSG_REGENERATION_RANDOM_OPTION*>(&msg);

    out.m_errNo = 0;
    Inven_Item items[2];
    CItem* baseItems[2] = { 0, 0 };
    const unsigned short slotPos[2] = { req.m_equipSlot0, req.m_equipSlot1 };
    bool allSealable = true;          // ORIG local_36（任一槽 m_field0!=1 即 false）
    unsigned char reSealCount = 0;

    for (int i = 0; i < 2; ++i)
    {
        const CInventory* invenR = user->getCurCharacInvenR();
        items[i] = invenR->GetInvenSlot((INVEN_TYPE)1, (int)slotPos[i]);
        if (items[i].isEmpty() || !items[i].isEquipableItemType() ||
            items[i].m_random.m_random.empty())
        {
            out.m_errNo = 0x13;
            return 0;
        }
        baseItems[i] = G_CDataManager()->find_item(items[i].m_addInfo);
        if (baseItems[i] == 0 || baseItems[i]->IsRandomOption() != true)
        {
            out.m_errNo = 0x13;
            return 0;
        }
        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                user, RestrictCategory::Enum_1, 0x10))
        {
            out.m_errNo = 0xd1;
            return 0;
        }
        int secuErr = GlobalData::s_pSecuProtectionField->Check(user,
                                                                (SECURITY_PROTCTION)0x1b);
        if (secuErr != 0)
        {
            out.m_errNo = secuErr;
            return 0;
        }
        if (user->CheckItemLock(1, (int)(short)slotPos[i]))
        {
            out.m_errNo = 0xd5;
            return 0;
        }
        if (items[i].m_field0 != 1)
        {
            allSealable = false;
        }
    }
    // ORIG：reSealCount=(uchar)(short)fistp(0.5f+(r0+r1)/2.0f)（浮点常量 0x3f000000/0x40000000）
    reSealCount = (unsigned char)(short)(
        0.5f + (float)((unsigned int)items[0].GetReSealCount() +
                       (unsigned int)items[1].GetReSealCount()) / 2.0f);

    Inven_Item newItem;
    int regenCost = 0;
    // ORIG RegenerationROI::Regenerate T 0x085f8646（声明 CDataManager.h，定义 blocked：
    // 依赖 random_option::CRandomOptionItemHandle::RegenVer_* 无任何 TU 提供）
    int regenErr = G_CDataManager()->m_regeneration.Regenerate(
        baseItems[0]->getUsableLevel(), baseItems[1]->getUsableLevel(),
        (int)req.m_srcOptIndex0, (int)req.m_srcOptIndex1, regenCost, newItem, user);
    if (regenErr != 0)
    {
        out.m_errNo = regenErr;
        return 0;
    }
    statistc_proxy::add(user, "log_random_option", "regen_cnt", 1, "%d,'%s'",
                        user->get_charac_10_level_section(),
                        GlobalData::s_systemTime_.getCurDate());
    if (regenCost > 0)
    {
        statistc_proxy::add(user, "log_random_option", "regen_gold",
                            (unsigned int)regenCost, "%d,'%s'",
                            user->get_charac_10_level_section(),
                            GlobalData::s_systemTime_.getCurDate());
    }
    if (!user->getCurCharacInvenW()->use_money(regenCost, (eMoneySubReason)0x2f, true))
    {
        out.m_errNo = 0xa;
        return 0;
    }
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, 0);
    for (int i = 0; i < 2; ++i)
    {
        if (user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, (int)slotPos[i], 1, (eItemDelReason)0x2e, true) != 1)
        {
            out.m_errNo = 0x13;
            return 0;
        }
    }
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)req.m_equipSlot0);
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)req.m_equipSlot1);
    newItem.m_field0 = allSealable ? 1 : 0;   // ORIG CONCAT31 低字节 ← local_36
    newItem.SetReSealCount(reSealCount);
    int insertedSlot = user->getCurCharacInvenW()->insertItemIntoInventory(
        newItem, (eItemAddReason)0x36, true, true);
    if (insertedSlot >= 0)
    {
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, insertedSlot);
        out.m_insertedSlot = (short)insertedSlot;
    }
    user->m_historyLog.RandomOption(newItem, randomOptionReason::T_REGENERATION);
    return 0;
}

// ORIG 0x081dd2be T _ZN27DispatcherResetRandomOption7processEP5CUserR8MSG_BASER9ParamBase
// （报告 DispatcherResetRandomOption/process.md ## 反编译 C 全文映射）：
// state==3 且当前角色存在且非交易中 → CheckItemLock(1,equipSlot) → GetInvenRef(1,槽)
// → find_item(idx) → Restrict(1,0xf)→0xd1 / 安全保护 0x26 / 选项空→0x13 /
// rarityChoice>2→0x13 / 再铸卷轴(m_addInfo∈{0xf,0x381,0x292090f2} 且数量>0，
// 否则 0x15) → 函数级 static CRandomOptionItemHandle → reset_option(idx,rarity,
// choice,&item.m_random.m_random) 非零即失败回显 → cUserHistoryLog.RandomOption(3)
// → statistc regen_cnt 同款 reset_cnt → use_item(scrollSlot,1,1)==0 失败 0x16 →
// SendUpdateItem×2 + SendCmdOkPacket(0x1c8)。全程错误只发包不外抛，恒返 0。
int DispatcherResetRandomOption::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_RESET_RANDOM_OPTION& req = *reinterpret_cast<MSG_RESET_RANDOM_OPTION*>(&msg);

    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return 0;
    }
    if (user->CheckInTrade())
    {
        return 0;
    }
    if (user->CheckItemLock(1, (int)req.m_equipSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0xd5);
        return 0;
    }
    Inven_Item* target =
        user->getCurCharacInvenW()->GetInvenRef((int)1, (int)req.m_equipSlot);
    if (target == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x11);
        return 0;
    }
    CItem* shopItem = G_CDataManager()->find_item(target->m_addInfo);
    if (shopItem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x11);
        return 0;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 0xf))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0xd1);
        return 0;
    }
    int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x26);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, (unsigned char)secuErr);
        return 0;
    }
    if (target->m_random.m_random.empty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x13);
        return 0;
    }
    if ((unsigned int)req.m_rarityChoice > 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x13);
        return 0;
    }
    // 再铸卷轴校验：GetInvenSlot(invenR,1,scrollSlot)，idx 三选一且数量 > 0
    Inven_Item scroll = user->getCurCharacInvenR()->GetInvenSlot(
        (INVEN_TYPE)1, (int)req.m_scrollSlot);
    const int scrollIdx = scroll.m_addInfo;
    if (!((scrollIdx == 0xf) || (scrollIdx == 0x381) || (scrollIdx == (int)0x292090f2)) ||
        scroll.m_addInfo2 <= 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x15);
        return 0;
    }
    // ORIG 函数级 static handle（__cxa_guard 保护），ctor 由 GlobalData.cpp 提供；
    // reset_option 定义链接期 blocked（见本 TU random_option 镜像注释）。
    static random_option::CRandomOptionItemHandle handle;
    int resetErr = (int)handle.reset_option(
        (unsigned long)target->m_addInfo, (ENUM_RARITY)shopItem->get_rarity(),
        (int)req.m_rarityChoice, &target->m_random.m_random);
    if (resetErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, (unsigned char)resetErr);
        return 0;
    }
    user->m_historyLog.RandomOption(*target, randomOptionReason::T_RESET);
    statistc_proxy::add(user, "log_random_option", "reset_cnt", 1, "%d,'%s'",
                        user->get_charac_10_level_section(),
                        GlobalData::s_systemTime_.getCurDate());
    if (user->getCurCharacInvenW()->use_item((int)req.m_scrollSlot, 1, 1) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x16);
        return 0;
    }
    user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)req.m_equipSlot);
    user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)req.m_scrollSlot);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x1c8);
    return 0;
}

// ============================================================================
// ORIG 0x0865798a T _ZN5CUser15serviceOfShusiaEv —— 休沙疲劳恢复服务。
// 幽灵 0x13；疲劳>99 0x12；Restrict(1,0x16) 0xd1；余额<价 0x16；
// 扣费 use_money(price,0xa,true)、AddValueStatistic(0x10,user,price)、
// setCurCharacStamina(100)、UseShusiaService((short)m_level)，成功返 0。
// 定义宿主为本 TU（CUser.cpp 由 RestoreCUserStatMethods 独占，经广播协调）。
// ============================================================================
int CUser::serviceOfShusia()
{
    if (IsCurCharacGhost())
    {
        return 0x13;
    }
    if (getCurCharacStamina() > 99)  // ORIG movzbl/cmp $0x63/seta（byte 无符号比较）
    {
        return 0x12;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            this, (RestrictCategory::Enum)1, 0x16))
    {
        return 0xd1;
    }

    const int price = G_CDataManager()->m_serverParameter.getRecoverStaminaPrice(
        get_charac_level(), getCurCharacStamina());
    if (getCurCharacMoney() < price)
    {
        return 0x16;
    }

    getCurCharacInvenW()->use_money(price, (eMoneySubReason)0xa, true);
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x10, this,
                                                   (unsigned int)price);
    setCurCharacStamina(100);
    Statistics::UseShusiaService((int)(short)getCurCharacR()->m_level);
    return 0;
}

// ============================================================================
// G2 角色域三个 CUser 叶子方法（定义宿主为本 TU，沿用 serviceOfShusia 先例；
// CUser.cpp 由 RestoreCUserStatMethods 独占，声明已按广播加入 CUser.h）。
// ============================================================================

// ORIG 0x0822fc06 W _ZN5CUser29SetLoadRentalInfoFromExchangeEb：写 +0x6ef91 字节
void CUser::SetLoadRentalInfoFromExchange(bool flag)
{
    // ORIG 0x0822fc06：movzbl 参数后直接按字节存 +0x6ef91（无 0/1 归一化）。
    m_rentalFlag2 = flag;                      // +0x6ef91（m_rentalFlag2）
}

// ORIG 0x0868e672 T _ZN5CUser25RemoveCharacMercenaryInfoEj：转调雇佣兵管理器
void CUser::RemoveCharacMercenaryInfo(unsigned int characNo)
{
    m_mercenaryMgr.RemoveCharac(characNo);     // UserMercenaryInfoMgr @ +0x6ef6c
}

// ORIG 0x0864a956 T _ZN5CUser31GetGuildIdBeforeDeleteCharacterEi：
// 返回 m_characList[slot].m_guildId（_Charac_info +0xdbe）
int CUser::GetGuildIdBeforeDeleteCharacter(int slot)
{
    return m_characList[static_cast<unsigned int>(slot)].m_guildId;
}


// ORIG 0x08230844 W _ZN5CUser31SetDeleteEventDungeonInvitationEb：
// 写 m_deleteEventDungeonInvitation 字节（objdump 逐指令转写；SelectDungeon::process 对全队调用）。
void CUser::SetDeleteEventDungeonInvitation(bool flag)
{
    m_deleteEventDungeonInvitation = flag ? 1 : 0;
}
