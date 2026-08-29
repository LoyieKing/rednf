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
#include "TimerQueue.h"          // G_TimerQueue/InsertTimer/TIMER_MESSAGE_0x51（RegistTimerSchedules）
#include "CItem.h"                // CItem::make_item（makeGift 虚调用需完整类型）
#include "HistoryLog.h"          // HistoryLog::WriteBuyItem（BuyItem::process）
#include "CGuildServerProxy.h"   // CGuildServerProxy::OnBuyItem（BuyItem::process）
#include "CRestrictCharacCreationEvent.h"  // getLimitCreateCharacter（check_error 消费）
#include "CStatisticServerProxy.h"  // statistc_proxy::add 发送代理
#include "../Statics/StatisticProxy.h"  // StatisticsPacket 权威布局
#include <stdarg.h>              // va_list（statistc_proxy::add 可变参）
#include "Inven_Item.h"          // randomOptionReason/RandomOption（随机选项三 process）
#include "CSkill.h"              // CSkill::m_fitGrowType/IsStealable（UseSkill 黑客校验）
char IsLightServer();   // _Z13IsLightServerv 0x0822ad44 W（定义 GlobalData.cpp）
#include "CSwitchLog.h"           // CSwitchLog（PvP 加入日志）
#include "Secu_HackLogCheckByParty.h"  // MSG_MONSTER_DIE 完整布局 + Secu_HackLogCheck
#include "CHackLog_OverDamage.h"  // 过度伤害限值表（DieMob 反作弊链）
#include "CDeathTower.h"          // CDeathTower::handleKillMonster（DieMob state10）
#include "DisJoint.h"            // DisJoint/DisJointResult（DisJointItem::_disjoint）
#include "CExpertJob.h"          // expert_job::CExpertJob/CDisjointer（_disjoint 依赖）


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
#include "ST_Guild_War_Rank_Info.h"                   // Packet_Notice ctor 依赖（StubS3C）
#include "Packet_Monitor_Del_Buddy.h" // shared/packet/include（RemoveFriend 监视包）
#include "CSyncSlangFilter.h"                    // 敏感词过滤（SetPartyInfo check_error）
#include "TaiwanQueries.h"                       // Login::dispatch_sig 查询族
#include "TaiwanCash.h"                          // Cera/RentAvatar IPG helper
#include "AvatarRechargeServer.h"                // MoveItem::send 充值服
#include "CHackAnalyzer.h"                       // SellItem::dispatch_sig 反作弊链
#include "Packet_Party_Result_Statistic.h"       // shared/packet/include（UdpCharacteristic）
#include "Packet_Party_Ping_Statistic.h"         // shared/packet/include（UdpCharacteristic 三 ping，wave2）
#include "Packet_Pvp_Ping_Statistic.h"           // shared/packet/include（wave2）
#include "Packet_Fair_Pvp_Ping_Statistic.h"      // shared/packet/include（wave2）
#include "Packet_Abnormal_Exit_Statistic.h"      // shared/packet/include（UdpCharacteristic）
#include "RestrictNameDeps.h"                     // restrict_inputting_name::isUtf8StrInUnicodeRange（CreateCharac）
#include "AuctionProxy.h"                        // CAuctionServerProxy/CCeraAuctionServerProxy（Login auction 状态包）
#include "DB_InterDeps.h"                        // Exit::dispatch_sig DB makeRequest 声明
#include "Arad_EventPeriodDataManager.h"         // Exit::dispatch_sig delUserRewardData
#include "CPrivateStoreMgr.h"                    // private_store::GetInstancePrivateStoreMgr（Exit）
#include "CTimeGate.h"                            // s_timeGate_ 完整类型（QuestAction）

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

int getAvatarRentPrice(int a, int b);
//   ^ ORIG T 0x0898c999 _Z18getAvatarRentPriceii（wave1：DisPatcher_RentAvatar::dispatch_sig
//     消费；全仓无声明定义，定义由 Main 分派属主批 → U 引用合规）
void RentAvatarErrorProc(CUser* user, int err);
int EnterPVPRoom(CUser* user, PvP_Room* pvp, ENUM_CMDPACKET cmd, char const* pass);

// ============================================================================
// ORIG T _Z12EnterPVPRoomP5CUserP8PvP_Room14ENUM_CMDPACKETPKc @ 0x081ed09a
// (size 0x62e)  <global>::EnterPVPRoom(CUser*, PvP_Room*, ENUM_CMDPACKET, char const*)
// 报告：docs/class_func_reports/_lt_global_gt_/EnterPVPRoom.md（## 汇编 + ## 反编译 C 已完整阅读）
// 语义：进入 PvP 房间前置校验链 → join_room → 成功时广播席位并传送；返回 1 成功 / 0 失败。
// 依赖声明核验：
//   CUser.h:2610 int Send(PacketGuard&)；:2572 int get_state()；:2825 bool CheckInTrade()；
//            :2827 bool CheckInParty()；:2820 void* GetParty()；:2813 int get_unique_id() const；
//            :2745 const char* get_acc_name() const；:2623 SendCmdErrorPacket(cmd,err,PacketGuard&)
//   CUserCharacInfo.h:845 unsigned char IsCurCharacGhost()；:797 signed char getCurCharacVill() const；
//            :1048 bool IsBusyDisjointStore()；:673 const char* getCurCharacName() const
//   PvP_Room.h:87 int check_joinable(CUser*)；:46 int get_pvp_battle_mode()；:102 unsigned char
//            IsExistPassword()；:101 bool CheckPassword(const char*, int)；:77 int join_room(CUser*,
//            int&, bool)；:43 int get_index()；:153 int make_seat_info(char*, int)
//   CPrivateStoreMgr.h:347/361 IsBusyPrivateStore / GetInstancePrivateStoreMgr
//   CParty.h:438 void send_invite_pvp(int)    CGameManager.h:470/626 CheckOutParty / G_CGameManager
//   GameWorld.h:160/197/242/340 IsSameTeamChannelJoin / send_all / goto_pvp / G_GameWorld
//   PacketGuard.h(InterfacePacketBuf.h:22/34/36) put_header(int,int)/put_byte(int)/finalize(bool)
//   CSwitchLog.h ctor(const char*, int, int, int) / operator()(const char*, ...)
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "PvP_Room.h"
#include "CParty.h"
#include "CGameManager.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "CSwitchLog.h"
#include "CPrivateStoreMgr.h"
#include <cstring>

// ORIG T 0x081ed09a _Z12EnterPVPRoomP5CUserP8PvP_Room14ENUM_CMDPACKETPKc
int EnterPVPRoom(CUser* user, PvP_Room* room, ENUM_CMDPACKET cmd,
                 char const* password)
{
    PacketGuard pg;                                     // -0x40(%ebp) local_44
    if (user->IsCurCharacGhost())                       // ORIG 080da2da
    {
        // put_header(1,cmd)/put_byte(0)/put_byte(0x13)/finalize/send
        pg.put_header(1, cmd);
        pg.put_byte(0);
        pg.put_byte(0x13);
        pg.finalize(true);
        user->Send(pg);
        return 0;
    }
    if (user->get_state() != 3)                         // ORIG 080da38c cmp $3
    {
        pg.put_header(1, cmd);
        pg.put_byte(0);
        pg.put_byte(0x13);
        pg.finalize(true);
        user->Send(pg);
        return 0;
    }
    if (user->getCurCharacVill() == 7)                  // ORIG 08645564 cmp $7,%al（PvP 区内）
    {
        user->SendCmdErrorPacket(cmd, 0x13, pg);        // ORIG 0867bfae
        return 0;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {                                                   // ORIG 085c4be8/085cad5e
        user->SendCmdErrorPacket(cmd, 0x3c, pg);
        return 0;
    }
    if (user->IsBusyDisjointStore())                    // ORIG 0864781e
    {
        user->SendCmdErrorPacket(cmd, 0x3c, pg);
        return 0;
    }
    if (room->get_pvp_battle_mode() == 6)               // ORIG 0822d9cc cmp $6
    {
        user->SendCmdErrorPacket(cmd, 0x17, pg);
        return 0;
    }
    if (user->CheckInTrade())                           // ORIG 080da2fe
    {
        user->SendCmdErrorPacket(cmd, 0x13, pg);
        return 0;
    }

    int joinable = room->check_joinable(user);          // -0x24 local_28（ORIG 先清 0）
    if (joinable != 0)
    {
        pg.put_header(1, cmd);
        pg.put_byte(0);
        pg.put_byte(joinable);
        pg.finalize(true);
        user->Send(pg);
        return 0;
    }

    if (room->IsExistPassword())                        // ORIG 0822d974
    {
        if (!room->CheckPassword(password,              // ORIG 085ddeae（xor $1 后 test）
                                 static_cast<int>(strlen(password))))
        {
            pg.put_header(1, cmd);
            pg.put_byte(0);
            pg.put_byte(6);                             // 密码错误码
            pg.finalize(true);
            user->Send(pg);
            return 0;
        }
    }

    if (user->CheckInParty())                           // ORIG 080da314
    {
        CParty* party = static_cast<CParty*>(user->GetParty());   // -0x1c（ORIG 0865514c）
        if (party != 0)
        {
            party->send_invite_pvp(user->get_unique_id() & 0xffff);  // ORIG movzwl 低 16 位
            G_CGameManager()->CheckOutParty(user, false);             // ORIG 080cc18e/08297c76
        }
    }

    int seat = 0;                                       // -0x44 local_48
    bool sameTeamJoin =
        G_GameWorld()->IsSameTeamChannelJoin();         // ORIG movzbl → bool 参
    int joinRes = room->join_room(user, seat, sameTeamJoin);   // -0x20 local_24（ORIG 085d6cf2）

    int battleMode = room->get_pvp_battle_mode();       // -0x5c（join_room 后再取，ORIG 0822d9cc）
    int roomIdx = room->get_index();                    // edi（ORIG 0822d994）
    char const* characName = user->getCurCharacName();  // esi（ORIG 08101028）
    char const* accName = user->get_acc_name();         // ebx（ORIG 08101072）

    CSwitchLog pvpLog(
        "bool EnterPVPRoom(CUser*, PvP_Room*, ENUM_CMDPACKET, const char*)",
        0x6fa2, 0, 0);                                  // ORIG CSwitchLog::CSwitchLog 0854fd3c
    pvpLog("pvp@log %s,%s,room(%d),%d,%d,%d",
           accName, characName, roomIdx, battleMode, joinRes, cmd);

    if (joinRes < 1)                                    // ORIG cmpl $0,-0x20; jle 成功路径
    {
        if (cmd == 0x65)                                // ORIG cmpl $0x65,0x10(%ebp)
        {
            PacketGuard pg65;                           // -0x50 local_54（带 try 域）
            pg65.put_header(1, 0x65);
            pg65.put_byte(1);
            pg65.finalize(true);
            user->Send(pg65);
        }
        room->make_seat_info(reinterpret_cast<char*>(&pg), seat);   // ORIG 085db9e2
        G_GameWorld()->send_all(pg);                    // ORIG 086c8c14
        G_GameWorld()->goto_pvp(user);                  // ORIG 086c6cee
        return 1;
    }

    pg.put_header(1, cmd);                              // joinRes > 0：加入失败错误码回包
    pg.put_byte(0);
    pg.put_byte(joinRes);
    pg.finalize(true);
    user->Send(pg);
    return 0;
}
int checkSpecialChar(char* name);
//   ^ ORIG T 0x0829efae _Z16checkSpecialCharPc（wave1：CreateCharac::check_error 消费）
int checkSpecialCharName(char* name);
//   ^ ORIG T 0x0829f01f _Z20checkSpecialCharNamePc（同上）

// MSG_RES_PVP_RANK（ORIG C1 0x08235378/D1 0x0823538e：仅构造/析构 +0xd PvpResultType，
// sizeof 0x91 实证）。wave1 最小建模（当前全仓无定义；待迁 PacketTypes.h 权威区）。
#pragma pack(push, 1)
struct MSG_RES_PVP_RANK
{
    MSG_RES_PVP_RANK() {}
    ~MSG_RES_PVP_RANK() {}
    char m_pad0[0xd];        // +0x00..0x0c（MSG_BASE 头区）
    PvpResultType m_result;  // +0x0d..0x90
};                           // sizeof = 0x91
#pragma pack(pop)


// MSG_MAKE_PVP_ROOM（ORIG 真实类型 _ZTV/_ZTI 域存在；dispatch_sig memset 0x3d 实证）。
// wave1 最小建模（当前全仓无定义；待迁 PacketTypes.h 权威区）：
// +0x0d char 门类型（<0 失败）、+0x0e char 房名[0x1e]、+0x2c short、+0x2e byte 模式门、
// +0x2f int 密码长（<=8）、+0x33 char 密码/校验缓冲、+0x3c byte battleMode。
// （int m_field2f 需 pack(1) 才落在 +0x2f；ORIG ExtractPacket 0x081f81ba 实证。）
#pragma pack(push, 1)
struct MSG_MAKE_PVP_ROOM
{
    char m_pad0[0xd];        // +0x00..0x0c
    char m_field0d;          // +0x0d
    char m_name[0x1e];       // +0x0e..0x2b
    short m_field2c;         // +0x2c
    char m_field2e;          // +0x2e
    int m_field2f;           // +0x2f
    char m_field33[9];       // +0x33..0x3b
    char m_battleMode;       // +0x3c
};                          // sizeof = 0x3d
#pragma pack(pop)

// CParty +0xb24 内嵌 CBattle_Field 子对象局部视图（CParty.h 以
// m_padBattleField1[0x158] 等 pad 占位、未引入 CBattle_Field 类型；Main 裁定
// 本组 TU 局部视图，与 CParty.cpp 口径一致）。布局：pad 0xb24 + BF 0xcd0。
struct CPartyBattleFieldView
{
    char m_pad[0xb24];
    CBattle_Field m_battleField;   // +0xb24
};

bool MakePVPRoom(CUser* user, MSG_MAKE_PVP_ROOM& msg, ENUM_CMDPACKET cmd, PVP_BATTLE_MODE mode);
//   ^ ORIG T 0x081f793c _Z11MakePVPRoomP5CUserR17MSG_MAKE_PVP_ROOM14ENUM_CMDPACKET15PVP_BATTLE_MODE
//     （LinkE2：尾参改 ORIG 枚举 PVP_BATTLE_MODE 对齐 mangled；定义 MakePVPRoom.cpp）

//   ^ ORIG T 0x081ed09a _Z12EnterPVPRoomP5CUserP8PvP_Room14ENUM_CMDPACKETPKc（wave1：
//     DisPatcher_EnterPVP::dispatch_sig 消费；全仓无声明定义，定义由 Main 分派属主批）
//   ^ ORIG W 0x082352c5 _Z18RentAvatarErrorProcP5CUseri（同上）
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
int DisPatcher_CheckConn::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ea900 T（报告 DisPatcher_CheckConn/dispatch_sig.md 全文转写）：
    // decre_check_count → invenCk/skillCk 校验（hack 计数 0x25b/0x25c）→ CHades Send_ChangeGold。
    user->decre_check_count();
    int invenCk;
    if (!packet.get_int(invenCk))
    {
        return LineFunc(0x6bf8, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacInvenCheckSum() != invenCk)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
    }
    int skillCk;
    if (!packet.get_int(skillCk))
    {
        return LineFunc(0x6bff, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacSkillCheckSum() != skillCk)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x25c, 1, 0, 0);
    }
    static_cast<XNuclear::CHades*>(user->getHades())->Send_ChangeGold();
    return 0;
}
int DisPatcher_CheckConn::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x08232a7e：空实现，恒返 0（mov $0x0,%eax; pop %ebp; ret）。
    return 0;
}
int DisPatcher_CheckConn::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232a88 W _ZN20DisPatcher_CheckConn7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ea900)

DisPatcher_Login::DisPatcher_Login() : m_field4(0) {}   // ORIG C1：基类 C1 后写 *(this+0x4)=0
DisPatcher_Login::~DisPatcher_Login() {}
int DisPatcher_Login::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081e8c78 静态局部 garenaToken（801B，BSS 无初始化；每次调用 memset 清零）
    static char garenaToken[0x801];

    char authBuf[0xbf];                  // -0x1a7..-0xe9（ORIG memset 0xbf 清零）
    unsigned int patchTime;              // -0x84
    unsigned int tokenLen;               // -0x88
    unsigned char loginType;             // -0xf6（authBuf+0xb1）
    int versionMajor;                    // -0x8c
    char decPwd[8];                      // -0x94
    unsigned int version;                // -0x98
    int nowSec;                          // -0x10
    unsigned int rentField;              // -0xa0

    if (user->get_state() != 1)
    {
        PacketGuard packetGuard;
        packetGuard.put_header(1, 3);
        packetGuard.put_byte(1);
        packetGuard.finalize(true);
        user->Send(packetGuard);
        return 0;
    }

    user->OnRecvEvent();
    if (G_GameWorld()->GetChannelType() == 6)
    {
        CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0x21);
        if (!repeatEvent->IsEventing((CUser*)0))
        {
            PacketGuard packetGuard;
            packetGuard.put_header(1, 1);
            packetGuard.put_byte(0);
            packetGuard.put_byte(0x42);
            packetGuard.finalize(true);
            user->Send(packetGuard);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x65f4, 0);
            trace("Can't Enter Into Guild War Channel ");
            user->DisConnSig((DISCONN_SIG)0x16, true, 0);
            return 0;
        }
    }

    memset(authBuf, 0, 0xbf);
    patchTime = 0;
    if (packet.get_int(patchTime) != 1)
        return LineFunc(0x6601, __PRETTY_FUNCTION__, 0, 0);

    if (0xe10 < patchTime)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x6605, 5);
        trace("[Taiwan, Auth] Invalid patch time. (time:%d,limit:%d)",
              patchTime, 0xe10);
        return LineFunc(0x6607, __PRETTY_FUNCTION__, 0, 0);
    }

    tokenLen = 0;
    if (packet.get_int(tokenLen) != 1)
        return LineFunc(0x660d, __PRETTY_FUNCTION__, 0, 0);

    memset(garenaToken, 0, 0x801);
    if (packet.get_str(garenaToken, 0x801, (int)tokenLen) != 1)
        return LineFunc(0x6612, __PRETTY_FUNCTION__, 0, 0);

    Taiwan::GarenaAuthData* authData =
        reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData());
    if (Taiwan::AuthGarenaToken::decrypt(garenaToken, (int)tokenLen,
                                         *authData) != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x6617, 5);
        trace("[Taiwan, Auth] Fail decrypt:%s", garenaToken);
        return LineFunc(0x6618, __PRETTY_FUNCTION__, 0, 0);
    }

    authData->setPatchTime(patchTime);
    nowSec = GlobalData::s_systemTime_.getCurSec();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x661e, 0);
    trace("[Taiwan, Auth] Token timestamp info. (token_time:%u, current_time:%u, patch_time:%d)",
          authData->getTimeStamp(), nowSec, authData->getPatchTime());

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x21))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 0xd1);
        return 0;
    }

    if (packet.get_int(*(unsigned int*)(authBuf + 0xd)) != 1)
        return LineFunc(0x665f, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_str(authBuf + 0x11, 0x18,
                       (int)*(unsigned int*)(authBuf + 0xd)) != 1)
        return LineFunc(0x667d, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_int(*(unsigned int*)(authBuf + 0x29)) != 1)
        return LineFunc(0x6692, __PRETTY_FUNCTION__, 0, 0);

    if (0x83 < *(unsigned int*)(authBuf + 0x29))
        return LineFunc(0x6698, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_str(authBuf + 0x2d, 0x84,
                       (int)*(unsigned int*)(authBuf + 0x29)) != 1)
        return LineFunc(0x66a8, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_byte(*(unsigned char*)(authBuf + 0xb1)) != 1)
        return LineFunc(0x6734, __PRETTY_FUNCTION__, 0, 0);

    loginType = *(unsigned char*)(authBuf + 0xb1);
    if (loginType != 0 && loginType != 1 && loginType != 2)
        return LineFunc(0x6738, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());

    versionMajor = 0;
    if (packet.get_int(versionMajor) != 1)
        return LineFunc(0x673f, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());

    if (versionMajor != 8)
        return LineFunc(0x6741, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());

    if (packet.get_binary(authBuf + 0xb6, 8) != 1)
        return LineFunc(0x6742, __PRETTY_FUNCTION__, 0, 0);

    if (!Dispatcher_MouseRegister::decryptPassword(
            authBuf + 0xb6, decPwd, (unsigned int)versionMajor))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 1);
        return LineFunc(0x674e, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());
    }

    if (!Dispatcher_MouseRegister::checkValiedPassword(decPwd))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 1);
        return LineFunc(0x6754, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());
    }

    memcpy(authBuf + 0xb6, decPwd, 7);

    version = 0;
    if (packet.get_int(version) != 1)
        return LineFunc(0x675c, __PRETTY_FUNCTION__, 0, 0);

    // CDataManager +0x3c0 = m_serverParameter.m_358（服务端版本字段）
    if (G_CDataManager()->m_serverParameter.m_358 != version)
    {
        PacketGuard versionGuard;                 // -0xdc
        versionGuard.put_header(1, 1);
        versionGuard.put_byte(0);
        versionGuard.put_byte(0x17);
        versionGuard.finalize(true);
        user->Send(versionGuard);
        m_field4 = m_field4 + 1;
        cMyTrace versionTrace(__PRETTY_FUNCTION__, 0x6813, 0);
        versionTrace("version dismatched: count(%d)", m_field4);
        user->DisConnSig((DISCONN_SIG)0x19, false, 0);
        return 0;
    }

    rentField = 0;
    if (packet.get_int(rentField) != 1)
        return LineFunc(0x681b, __PRETTY_FUNCTION__, 0, 0);

    CStreamGuard streamGuard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp",
                                               0x6972),
        true);                                    // -0xa4
    // 【ABI 镜像】SIG_LOGIN_DATA 偏移字段（SigTypes_D.h 待具名化区：+0x18/+0x84/
    // +0x9c/+0xa0/+0x3905/+0x3a30；+0xa1=m_blackIpCheckBuf、+0x3a34=m_clientVersion、
    // +0x3a38=m_channelRestrictField 已具名可用）
    SIG_LOGIN_DATA* login = streamGuard.GetInBuffer<SIG_LOGIN_DATA>();
    memset(login, 0, 0x4358);
    if (packet.get_short(login->m_channelRestrictField) != 1)
        return LineFunc(0x6980, __PRETTY_FUNCTION__, 0, 0);

    if (login->m_channelRestrictField < 0 || 0x11 < login->m_channelRestrictField)
        login->m_channelRestrictField = 0;

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        cMyTrace groupTrace(__PRETTY_FUNCTION__, 0x699a, 0);
        groupTrace("CheckLoginServerGroup user_id(%s), server_group(%d)",
                   authBuf + 0x11, (int)login->m_channelRestrictField);
        if ((unsigned short)login->m_channelRestrictField <= 0xa &&
            ((1 << login->m_channelRestrictField) & 0x580) != 0)
            return LineFunc(0x69a2, __PRETTY_FUNCTION__, 0, 0);
    }
    else
    {
        login->m_channelRestrictField = 0;
    }

    user->SetServerGroup((ENUM_SERVER_GROUP)login->m_channelRestrictField);

    int localIp;
    if (packet.get_int(localIp) != 1)
        return LineFunc(0x69cf, __PRETTY_FUNCTION__, 0, 0);
    user->set_local_ip_address(localIp);

    unsigned char logInOutState = 0;
    packet.get_byte(logInOutState);
    user->SetLogInOutState(logInOutState);

    authData = reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData());
    char* userIdStr = NumberToString(authData->getUid(), 0);
    strncpy(login->m_userId, userIdStr, 0x18);          // +0x00
    memset(login->m_pad18, 0, 0x84);                    // +0x18 清零区
    login->m_timeStamp = authData->getTimeStamp();      // +0x3a30

    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        login->m_gcNo =
            static_cast<unsigned int>(G_CEnvironment()->m_serverEnvir.m_gcNo);  // +0x9c ← env+0x198
    else
        login->m_gcNo =
            (unsigned int)G_CEnvironment()->get_gc_no_hardcode(
                (ENUM_SERVER_GROUP)login->m_channelRestrictField);

    user->m_network.GetPeerIP2(login->m_blackIpCheckBuf, 0x14);
    login->m_loginType = loginType;                     // +0xa0
    // +0x3905 落于 m_passPadPassword(+0x38e1) 内 +0x24 区（ORIG 网吧密码串写入）
    strncpy(login->m_passPadPassword + 0x24, authBuf + 0xb6, 7);
    user->SetLoginTime(GlobalData::s_systemTime_.getCurSec());
    // CUser m_mcap 区（+0x8e3f0；字段偏移 = mcap 内偏移）
    user->m_mcap.m_field24 =                           // +0x24（0x8e414）
        (unsigned int)GlobalData::s_systemTime_.getCurSec();
    user->m_network.GetPeerIP2(user->m_mcap.m_pcRoomIp, 0x14);   // +0x28（0x8e418）
    user->m_mcap.m_pad20[0] = 0;                       // +0x20（0x8e410）
    user->m_mcap.m_sellTimeHitCount = 0;               // +0x40（0x8e430）
    user->m_mcap.m_sellTimeChkBaseSec = 0;             // +0x3c（0x8e42c）
    user->m_mcap.m_timeChkCount = 0;                   // +0x50（0x8e440）

    PacketGuard auctionGuard;                     // -0xb8
    auctionGuard.put_header(0, 0xb7);
    auctionGuard.put_byte(0);
    if (GlobalData::s_auction_proxy->IsRunning())
        auctionGuard.put_byte(1);
    else
        auctionGuard.put_byte(0);
    auctionGuard.finalize(true);
    user->Send(auctionGuard);

    PacketGuard ceraAuctionGuard;                 // -0xe8
    ceraAuctionGuard.put_header(0, 0xb7);
    ceraAuctionGuard.put_byte(1);
    if (GlobalData::s_cera_auction_proxy->IsRunning())
    {
        ceraAuctionGuard.put_byte(1);
        puts("\n\nInter_AuctionNotifyAuctionService::dispatch_sig : point opened\n");
    }
    else
    {
        ceraAuctionGuard.put_byte(0);
        puts("\n\nInter_AuctionNotifyAuctionService::dispatch_sig : point closed\n");
    }
    ceraAuctionGuard.finalize(true);
    user->Send(ceraAuctionGuard);

    cMyTrace loginTrace(__PRETTY_FUNCTION__, 0x6a7b, 0);
    loginTrace("USER LOGINED! SEND ENUM_NOTIPACKET_AUCTION_NOTIFY_AUCTION_SERVICE");

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, streamGuard);
    return 0;
}
int DisPatcher_Login::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x08232a6a：空实现，恒返 0（mov $0x0,%eax; pop %ebp; ret）。
    return 0;
}
int DisPatcher_Login::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232a74 W _ZN16DisPatcher_Login7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081e8c78)

DisPatcher_SetIP::DisPatcher_SetIP() {}
DisPatcher_SetIP::~DisPatcher_SetIP() {}
int DisPatcher_SetIP::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x08232fc8：空实现，恒返 0（mov $0x0,%eax; pop %ebp; ret）。
    return 0;
}
int DisPatcher_SetIP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232fd2 W _ZN16DisPatcher_SetIP7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f468c)

DisPatcher_Exit::DisPatcher_Exit() {}
DisPatcher_Exit::~DisPatcher_Exit() {}
int DisPatcher_Exit::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char local_48[16];
    local_48[0] = '\0';
    local_48[1] = '\0';
    local_48[2] = '\0';
    local_48[3] = '\0';
    local_48[4] = '\0';
    local_48[5] = '\0';
    local_48[6] = '\0';
    local_48[7] = '\0';
    local_48[8] = '\0';
    local_48[9] = '\0';
    local_48[10] = '\0';
    local_48[0xb] = '\0';
    local_48[0xc] = '\0';
    local_48[0xd] = '\0';
    local_48[0xe] = '\0';
    local_48[0xf] = '\0';
    user->m_network.GetPeerIP2(local_48, 0x10);
    unsigned char local_49 = 0;
    if (packet.get_byte(local_49) != true)
    {
        local_49 = 0;
    }
    if (user != 0)
    {
        user->SetLogInOutState(local_49);
    }
    if (user->m_mcap.isSaveCleanPadPoint() != false)
    {
        unsigned int uVar5 = user->m_mcap.getPoint();
        DB_SaveCleanpadPoint::makeRequest(uVar5, static_cast<int>(user->get_acc_id()));
        user->m_mcap.disableCleanPadPoint();
    }
    if (user->get_state() > 2)
    {
        if (G_GameWorld()->IsEquipSlotSwitchChannel() != false)
        {
            if (user->getCurChannelEquipslotSwitch() == 1)
            {
                CExpandEquipslot* local_20 = user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(9));
                char cVar1 = static_cast<char>(user->getCurExpandEquipslotSwitch());
                char cVar2 = static_cast<char>(user->getCurChannelEquipslotSwitch());
                local_20->EquipslotSwitch(user, '\0', cVar2, cVar1);
            }
        }
    }
    {
        std::string local_38("Arad_EventPeriodDataManager");
        ARAD::Arad_DataManager* this_01 = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        ARAD::Arad_EventPeriodDataManager* this_02 =
            static_cast<ARAD::Arad_EventPeriodDataManager*>(this_01->findGameScript(local_38));
        this_02->delUserRewardData(user);
    }
    if (user->get_state() != 0)
    {
        user->WorkPerFiveMin();
        if (user->get_charac_memberkey() != 0)
        {
            int local_14 = user->get_member_pay_tex_money_to_upper();
            int local_10 = user->get_member_pay_tex_fatigue_to_upper();
            unsigned int uVar5 = static_cast<unsigned int>(user->getCurCharacNo());
            bool bVar3 = user->is_connect_upper_member();
            ENUM_SERVER_GROUP uVar6 = user->GetServerGroup();
            CMonitorServerProxy* this_03 =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(uVar6);
            this_03->SendMemberPayTax(bVar3, uVar5, local_14, local_10);
            if (0 < local_14)
            {
                user->set_member_pay_tex_money_to_upper(0);
            }
            if (0 < local_10)
            {
                user->set_member_pay_tex_fatigue_to_upper(0);
            }
        }
        unsigned int local_1c = user->get_acc_id();
        unsigned int local_18 = user->GetUID();
        char* uVar6 = NumberToString(local_1c, 0);
        cMyTrace local_30(__PRETTY_FUNCTION__,
                          0x706d, 0);
        local_30("[USER LOGOUT (Normal)] DB ID : %s\n", uVar6);
        private_store::CPrivateStoreMgr* puVar7 = private_store::GetInstancePrivateStoreMgr();
        (*reinterpret_cast<void (**)(private_store::CPrivateStoreMgr*, CUser*)>(
            *reinterpret_cast<void**>(puVar7)))(puVar7, user);
        pvp_assault::CAssaultMgr* this_04 = pvp_assault::GetInstanceAssaultMgr();
        this_04->OnLeaveAssaultPlace(user, true);
        GlobalData::s_ExpertJobMgr->OnLeaveUser(user);
        GlobalData::s_onlinePreliminaryTeamMgr->OnLeaveUser(user);
        user->checkLogOutCorrectly();
        long lVar8 = user->GetCharacPlayTick();
        int iVar4 = user->get_charac_no(-1);
        DB_UpdateCharacPlayTime::makeRequest(iVar4, lVar8);
        CGameManager* this_05 = G_CGameManager();
        this_05->user_exit(user);
        if (local_49 == '\0')
        {
            DB_ArrangeUserLoginInfo::makeRequest(local_1c, true, local_18);
        }
        else
        {
            WongWork::CSecurityCard* this_06 = user->getSecurityCard();
            if (this_06->isCertified() != false)
            {
                DB_SecuServiceUpdateValidityTime::makeRequest(local_1c, 0xb4, 8, local_48);
            }
            Sanicova::CPad* this_07 = user->getPad();
            if (this_07->isCertified() != false)
            {
                DB_SecuServiceUpdateValidityTime::makeRequest(local_1c, 0xb4, 0x10, local_48);
            }
        }
    }
    return 0;
}
int DisPatcher_Exit::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08232be0：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
int DisPatcher_Exit::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232bea W _ZN15DisPatcher_Exit7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ed948)

DisPatcher_SelectCharac::DisPatcher_SelectCharac() {}
DisPatcher_SelectCharac::~DisPatcher_SelectCharac() {}
int DisPatcher_SelectCharac::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() != 2)
    {
        return 0x7fffffff;
    }
    if (user->isLockedCreateCharac() != false)
    {
        return 0x13;
    }
    if (user->isHumanCertified() != true)
    {
        return 0x7fffffff;
    }
    MSG_CHARAC_SELECT& sel = static_cast<MSG_CHARAC_SELECT&>(msg);
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        unsigned int uVar6 = user->getOnlinePreliminaryCharacNo();
        sel.m_slot = static_cast<signed char>(user->get_charac_idx(uVar6));
        if (static_cast<unsigned char>(sel.m_slot) == 0xff)
        {
            return 0xffffffff;
        }
    }
    if (checkPvpEnterCharacter(user, sel) != true)
    {
        return 0xffffffff;
    }
    if (('#' < static_cast<char>(sel.m_slot)) || (static_cast<char>(sel.m_slot) < '\0'))
    {
        signed char MVar1 = sel.m_slot;
        cMyTrace local_34(__PRETTY_FUNCTION__, 0x16c4, 5);
        local_34("pUser->SelectCharac() fail, %d out of range", static_cast<int>(MVar1));
        return 4;
    }
    if (user->m_characList.size() <= static_cast<unsigned char>(sel.m_slot))
    {
        return 0xfffffffe;
    }
    std::string local_38(user->m_characList[static_cast<int>(sel.m_slot)].m_name);
    bool bVar4;
    if (CSyncSlangFilter::GetInstance()->HasSlangName(local_38) == false &&
        CSyncSlangFilter::GetInstance()->HasSlang(local_38) == false)
    {
        bVar4 = false;
    }
    else
    {
        bVar4 = true;
    }
    int uVar10;
    if (bVar4)
    {
        uVar10 = 0x9f;
    }
    else
    {
        unsigned int uVar6 = user->get_acc_id();
        bool bVar4b = GlobalData::s_pGMAccounts_->isGM(uVar6);
        G_CDataManager()->setCharacNameFilterPass(bVar4b);
        if (G_CDataManager()->hasPreventPrefixCharacName(
                user->m_characList[static_cast<int>(sel.m_slot)].m_name,
                static_cast<int>(strlen(user->m_characList[static_cast<int>(sel.m_slot)].m_name))) != false)
        {
            uVar10 = 8;
        }
        else if (user->checkSelectCharac(static_cast<int>(sel.m_slot)) == true)
        {
            if (user->isLockedDeleteCharac() == false)
            {
                if (user->lock4DataLoad() == true)
                {
                    uVar10 = 0;
                }
                else
                {
                    uVar10 = 0xfffffffc;
                }
            }
            else
            {
                int uVar9 = user->m_characList[static_cast<int>(sel.m_slot)].m_characNo;
                cMyTrace local_20(__PRETTY_FUNCTION__,
                                  0x171c, 0);
                local_20("select_charac error during delete charac_no(%d)", uVar9);
                uVar10 = 0x7fffffff;
            }
        }
        else
        {
            uVar10 = 0xfffffffd;
        }
    }
    return uVar10;
}

// ORIG 0x081c6c22 T _ZN23DisPatcher_SelectCharac22checkPvpEnterCharacterEP5CUserR17MSG_CHARAC_SELECT
// （报告 DisPatcher_SelectCharac/checkPvpEnterCharacter.md 全文转写，wave2）：
// user==0 → false；PvP 频道且槽位职业为 9/10（ restricting job ）→ false；否则 true。
bool DisPatcher_SelectCharac::checkPvpEnterCharacter(CUser* user, MSG_CHARAC_SELECT& msg)
{
    if (user == 0)
    {
        return false;
    }
    if (G_GameWorld()->IsPVPChannel())
    {
        int job = user->GetCharacJob(static_cast<int>(msg.m_slot));
        if (job == 9)
        {
            return false;
        }
        if (job == 10)
        {
            return false;
        }
    }
    return true;
}

// ORIG 0x081cab3a T _ZN18DisPatcher_UseCoin20usedCoinDungeonCheckEP5CUserP6CParty
// （报告 DisPatcher_UseCoin/usedCoinDungeonCheck.md 汇编+C 全文转写，wave2）。
int DisPatcher_UseCoin::usedCoinDungeonCheck(CUser* user, CParty* party)
{
    if (user == 0)
    {
        return -1;
    }
    if (party == 0)
    {
        return -1;
    }
    int dungeonIndex = user->getPlayingDungeonIndex();
    if (dungeonIndex == -1)
    {
        // CParty+0xb24 内嵌 CBattle_Field（局部视图 CPartyBattleFieldView）
        CBattle_Field* battleField =
            &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField;
        dungeonIndex = battleField->get_dungeon_index();
    }
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIndex);
    if (dungeon == 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__, 0x1fa9,
                              "DisPatcher_UseCoin G_CDataManager()->find_dungeon( %d) fail", 0);
        return 0x7fffffff;
    }
    if (party->checkFreeRevivalCondition(dungeon))
    {
        return 0;
    }
    if (!user->getCurCharacInvenR()->IsCoinUsable())
    {
        return 0x11;
    }
    if (party->checkUnLimitUsedCoinCondition(dungeon))
    {
        return 0;
    }
    return party->checkLimitUsedCoinCondition(dungeon) ? 1 : 0;   // ORIG +0x108..0x11d：返 bool 升 int
}

// ORIG 0x081ca63a T _ZN18DisPatcher_UseCoin11Dungeon_check_errorEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseCoin/Dungeon_check_error.md 汇编+C 全文转写，wave2）。
int DisPatcher_UseCoin::Dungeon_check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;   // ORIG 第 3 参未消费（coin 读自 msg+0xd）
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return -1;
    }
    CDungeon* dungeon = party->m_dungeon;   // +0xcac
    if (party != 0 && dungeon != 0)
    {
        // CParty+0xb24 内嵌 CBattle_Field（局部视图 CPartyBattleFieldView）
        CBattle_Field* battleField =
            &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField;
        // ORIG：bloodDungeonType>=1 且 bloodState>=3 → 0x13
        if (dungeon->m_bloodDungeonType >= 1 && battleField->getBloodState() >= 3)
        {
            return 0x13;
        }
        if (dungeon->isTournamentDungeon() && party->GetTournamentDungeonClearState() >= 1)
        {
            return 0x13;
        }
    }
    int err = usedCoinDungeonCheck(user, party);
    if (err == 0)
    {
        MSG_USE_COIN& req = *reinterpret_cast<MSG_USE_COIN*>(&msg);
        err = party->ReviveUser(user, req.m_0d);   // ORIG movzwl msg+0xd
        if (err < 1)
        {
            err = 0;
        }
    }
    return err;
}

// ORIG 0x081ca75c T _ZN18DisPatcher_UseCoin11WarRoom_check_errorEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseCoin/WarRoom_check_error.md 汇编+C 全文转写，wave2）。
int DisPatcher_UseCoin::WarRoom_check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;   // ORIG 第 3 参未消费（coin 读自 msg+0xd）
    WarRoom* room = static_cast<WarRoom*>(user->GetWarRoom());
    if (room == 0)
    {
        return -2;
    }
    if (!user->getCurCharacInvenR()->IsCoinUsable())
    {
        return 0x11;
    }
    MSG_USE_COIN& req = *reinterpret_cast<MSG_USE_COIN*>(&msg);
 
    int ret = room->ReviveUserByCoin(user, req.m_0d);
    return ret > 0 ? ret : 0;
}
// ORIG 0x0821b098 W _ZN28DisPatcher_UdpCharacteristic13partyPingStatEP5CUserRtS2_
// （报告 DisPatcher_UdpCharacteristic/partyPingStat.md 汇编+C 全文转写，wave2）：
// Party_Ping 统计包（0xfa7,0xe）：+0xa=bad、+0xc=bad+total，经统计代理 SendPacket。
void DisPatcher_UdpCharacteristic::partyPingStat(CUser* user, unsigned short& bad, unsigned short& total)
{
    Packet_Party_Ping_Statistic pkt;
    pkt.m_dungeonBadPing = bad;                 // ORIG movzwl (%eax)
    pkt.m_dungeonTotal = (unsigned short)(bad + total);   // ORIG lea (%edx,%eax,1)
    ENUM_SERVER_GROUP group = user->GetServerGroup();
    CStatisticServerProxy* proxy = GlobalData::s_statistic_proxy_mgr->GetServerProxy(group);
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0xe);
}

// ORIG 0x0821afbc W _ZN28DisPatcher_UdpCharacteristic11pvpPingStatEP5CUserRtS2_
// （报告 pvpPingStat.md 全文转写，wave2）：Pvp_Ping 包（0xfa8,0xe）同型。
void DisPatcher_UdpCharacteristic::pvpPingStat(CUser* user, unsigned short& bad, unsigned short& total)
{
    Packet_Pvp_Ping_Statistic pkt;
    pkt.m_pvpBadPing = bad;
    pkt.m_pvpTotal = (unsigned short)(total + bad);
    ENUM_SERVER_GROUP group = user->GetServerGroup();
    CStatisticServerProxy* proxy = GlobalData::s_statistic_proxy_mgr->GetServerProxy(group);
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0xe);
}

// ORIG 0x0821b02a W _ZN28DisPatcher_UdpCharacteristic15fairPvpPingStatEP5CUserRtS2_
// （报告 fairPvpPingStat.md 全文转写，wave2）：Fair_Pvp_Ping 包（0x27ed,0xe）同型。
void DisPatcher_UdpCharacteristic::fairPvpPingStat(CUser* user, unsigned short& bad, unsigned short& total)
{
    Packet_Fair_Pvp_Ping_Statistic pkt;
    pkt.m_badPing = bad;
    pkt.m_total = (short)(total + bad);
    ENUM_SERVER_GROUP group = user->GetServerGroup();
    CStatisticServerProxy* proxy = GlobalData::s_statistic_proxy_mgr->GetServerProxy(group);
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0xe);
}
// ORIG 0x081c632a T _ZN23DisPatcher_SelectCharac4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_SelectCharac/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x156f, PRETTY, 0, 0)。
int DisPatcher_SelectCharac::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CHARAC_SELECT& m = *reinterpret_cast<MSG_CHARAC_SELECT*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_slot)))  // +0x0d（signed char→char& 适配）
    {
        return LineFunc(0x156f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_SelectCharac::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081c6826 T：param+4 != 0x7fffffff 且 != 0 时 SendCmdErrorPacket(4, (uchar)值)。
    stSelectCharac& out = static_cast<stSelectCharac&>(param);
    if (out.m_errNo != 0x7fffffff && out.m_errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)4, (unsigned char)out.m_errNo);
    }
}

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
                        __PRETTY_FUNCTION__,
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
// ORIG 0x081c727c T _ZN23DisPatcher_SelectCharac15RequestDBTicketEP5CUserR17MSG_CHARAC_SELECT
// （报告 RequestDBTicket.md）：opcode 0xb / uid → DisableCharacInfo(4) →
// 有效最后登录角色时追加 channelNo/characNo → SIG_LOAD_TICKET(0x19c 清零，
// +0=get_charac_no(msg.m_slot)) → 有效登录 put Q5 否则 Q2。
int DisPatcher_SelectCharac::RequestDBTicket(CUser* user, MSG_CHARAC_SELECT& msg)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "PacketDispatcher_Impl_1.cpp", 0x17db), true);
    **guard << 0xb;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo((CACHE_SECTION)4);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_TICKET* sig = guard->GetInBuffer<SIG_LOAD_TICKET>();
    memset(sig, 0, sizeof(SIG_LOAD_TICKET));
    *reinterpret_cast<unsigned int*>(sig) = static_cast<unsigned int>(
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

// ORIG 0x081c7bac T _ZN23DisPatcher_SelectCharac20RegistTimerSchedulesEP5CUser
// （报告 RegistTimerSchedules.md）：G_TimerQueue()->InsertTimer(OBJ_TYPE_0, uid,
// TIMER_MESSAGE_0x51, 0, unique_id, characNo) 后恒返 1。
int DisPatcher_SelectCharac::RegistTimerSchedules(CUser* user)
{
    // ORIG 0x081c7bac：实参从右到左求值缓存（getCurCharacNo→esi /
    // get_unique_id→edi / GetUID→ebx），单行嵌套调用复现栈帧（sub $0x2c）。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, static_cast<int>(user->GetUID()),
        TIMER_MESSAGE_0x51, 0, user->get_unique_id(),
        user->getCurCharacNo());
    return 1;
}

// ORIG T 0x081c79f2（RequestBlackCount.md）：SIG 0xb3 → CACHE_BLACK_COUNT；memset 0x10。
int DisPatcher_SelectCharac::RequestBlackCount(CUser* user, MSG_CHARAC_SELECT& msg)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x1862), true);
    **guard << 0xb3;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_BLACK_COUNT);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_REQ_BLACK_COUNT* sig = guard->GetInBuffer<SIG_REQ_BLACK_COUNT>();
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
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x18af), true);
    **guard << 8;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_INVENTORY);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_INVENTORY* sig = guard->GetInBuffer<SIG_LOAD_INVENTORY>();
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
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x17bd), true);
    **guard << 10;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_QUEST);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_QUEST* sig = guard->GetInBuffer<SIG_LOAD_QUEST>();
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
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x17f8), true);
    **guard << 12;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_DUNGEON_CLEAR);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_DUNGEON_CLEAR* sig = guard->GetInBuffer<SIG_LOAD_DUNGEON_CLEAR>();
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
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x1843), true);
    **guard << 0x282;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_KILL_MONSTER);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_KILL_MONSTER_INFO* sig =
        guard->GetInBuffer<SIG_LOAD_KILL_MONSTER_INFO>();
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
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x1814), true);
    **guard << 0x20;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_ETC);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_ETC* sig = guard->GetInBuffer<SIG_LOAD_ETC>();
    memset(sig, 0, sizeof(SIG_LOAD_ETC));
    sig->m_accId = user->get_acc_id();
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_serverGroup =
        G_CEnvironment()->m_serverEnvir.m_serverGroup;      // ORIG 读 ENV+0x378
    sig->m_channelNo = G_CEnvironment()->get_channel_no();
    sig->m_fieldC = msg.m_0e;
    sig->m_tagEtc.clear();
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


// ORIG T 0x081c6f08（RequestDBSkill.md）：SIG 9 → CACHE_SKILL；StreamPool；
// memset 0x414 后仅写 characNo@0；队列 5(有效上次登录)/2(DB_Q)。
int DisPatcher_SelectCharac::RequestDBSkill(CUser* user, MSG_CHARAC_SELECT& msg)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x17a1), true);
    **guard << 9;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_SKILL);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_SKILL* sig = guard->GetInBuffer<SIG_LOAD_SKILL>();
    memset(sig, 0, sizeof(SIG_LOAD_SKILL));
    *reinterpret_cast<unsigned int*>(sig) = static_cast<unsigned int>(
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
                        __PRETTY_FUNCTION__,
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
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

DisPatcher_CreateCharac::DisPatcher_CreateCharac() {}
DisPatcher_CreateCharac::~DisPatcher_CreateCharac() {}
int DisPatcher_CreateCharac::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c0830 T（报告 DisPatcher_CreateCharac/check_error.md 全文转写）。
    // ORIG +0x26：state != 2 → 0x7fffffff（非 0x13）。
    if (user->get_state() != 2)
    {
        return 0x7fffffff;
    }
    if (user->isLockedDeleteCharac())
    {
        return 0x13;
    }
    if (user->isLocked4DataLoad())
    {
        return 0x13;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x20))
    {
        return 0xd1;
    }
    CRestrictCharacCreationEvent* restrictEvt =
        reinterpret_cast<CRestrictCharacCreationEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x31));
    bool eventing = false;
    if (restrictEvt != 0)
    {
        // ORIG vtable+0x34 槽直调（Ghidra C 原样；事件域虚表槽位归属见 FixEventVtable 批）
        eventing = (*(int (**)(void*, int))(*(char**)restrictEvt + 0x34))(
                       (void*)restrictEvt, 0) != 0;
    }
    if (eventing)
    {
        int limit = restrictEvt->getLimitCreateCharacter(user->get_charac_count());
        if (limit != 0)
        {
            return limit;
        }
    }
    if (user->get_charac_count() >= (int)user->getCharacSlotLimit())
    {
        return 4;
    }
    if (user->get_charac_count() == 0x24)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x8a0, 5);
        trace("DisPatcher_CreateCharac pUser->get_charac_count() == MAX_CHARAC_SLOT false");
        return 4;
    }
    MSG_CREATE_CHARACTER& req = *reinterpret_cast<MSG_CREATE_CHARACTER*>(&msg);
    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(req.m_name))
    {
        return 0x9f;
    }
    if (checkSpecialChar(req.m_name) || checkSpecialCharName(req.m_name))
    {
        return 0x9f;
    }
    return 0;
}
int DisPatcher_CreateCharac::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (user->get_state() != 2)
    {
        return LineFunc(0x145,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// ORIG 0x081c01ec T _ZN23DisPatcher_CreateCharac4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_CreateCharac/read.md（汇编+反编译 C 全读）
int DisPatcher_CreateCharac::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CREATE_CHARACTER& m = *reinterpret_cast<MSG_CREATE_CHARACTER*>(&msg);
    memset(&m, 0, 0x30);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_slot)))  // +0x0d（signed char→char& 适配）
    {
        return LineFunc(0x749,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!((signed char)m.m_slot < 0xc && m.m_slot >= 0))
    {
        return LineFunc(0x74b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (m.m_slot == 0xb)
    {
        return LineFunc(0x763,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x0e 名字长度 int（m_padE[4] 起点，ORIG 逐字节读同一槽）
    int& nameLen = *reinterpret_cast<int*>(m.m_padE);
    if (!packet.get_int(nameLen))
    {
        return LineFunc(0x766,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (nameLen < 4 || 0x12 < nameLen)
    {
        return LineFunc(0x76a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(m.m_name, 0x1d, nameLen))         // +0x12
    {
        return LineFunc(0x76e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(m.m_name[0x1d]))                 // +0x2f 末字节标志
    {
        return LineFunc(0x780,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (m.m_name[0x1d] < 0)
    {
        return LineFunc(0x782,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_CreateCharac::send(CUser* user, ParamBase& param)
{
    stCreateCharac& p = *reinterpret_cast<stCreateCharac*>(&param);
    if (p.m_errNo != 0x7fffffff && p.m_errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)5, (unsigned char)(p.m_errNo & 0xff));
    }
}

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
// ORIG 0x081c14f0 T _ZN23DisPatcher_DeleteCharac11check_errorEP5CUserR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_DeleteCharac/check_error.md（汇编+反编译 C 全读）
int DisPatcher_DeleteCharac::check_error(CUser* user, MSG_BASE& msg)
{
    MSG_DELETE_CHARACTER& req = *reinterpret_cast<MSG_DELETE_CHARACTER*>(&msg);

    if (user->get_state() != 2)
    {
        return -1;
    }
    if (user->isLockedCreateCharac())
    {
        return 0x13;
    }
    if (!((signed char)req.m_slot < user->get_charac_count() && req.m_slot >= 0))
    {
        return -2;
    }
    if (!user->CheckDeleteCharacTime(req.m_slot))
    {
        return 0x16;
    }
    if (user->getCurCharacNo() != 0 &&
        user->getCurCharacNo() == user->get_charac_no(req.m_slot))
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0xaa4, 5);
        trace("pUser->getCurCharacNo() == recv_pk_del_charac.charac_index %d %d",
              req.m_slot, user->get_charac_no(req.m_slot));
        return 0x16;
    }
    if (user->isLocked4DataLoad())
    {
        return 0x13;
    }
    if (user->getCurCharacR() != 0)
    {
        return -3;
    }
    if (user->m_characList[(int)req.m_slot].m_characLinkType > 1)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0xab4, 5);
        trace("LINK_LOG : ABNOMAL CHARAC DELETEING!!");
        return -4;
    }
    const char* name = user->get_charac_name(req.m_slot);
    // +0x12 角色名字区（m_padE 内 +0xe..+0x11 为填充，名字起点 = m_padE+4）
    const char* reqName = req.m_padE + 4;
    if (name == 0 || *reqName == 0)
    {
        return -5;
    }
    if (strncmp(name, reqName, 0x1e) != 0)
    {
        return -6;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 0x17))
    {
        return 0xd1;
    }
    int chk = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x2e);
    if (chk == 0)
    {
        return 0;
    }
    return chk;
}
int DisPatcher_DeleteCharac::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (user->get_state() != 2)
    {
        return LineFunc(0x170,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int DisPatcher_DeleteCharac::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_DELETE_CHARACTER& req = *reinterpret_cast<MSG_DELETE_CHARACTER*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_slot)))
    {
        return LineFunc(0xa26,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x0e 名字长度 int（m_padE[4] 起点）
    int& nameLen = *reinterpret_cast<int*>(req.m_padE);
    if (!packet.get_int(nameLen))
    {
        return LineFunc(0xa29,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(req.m_padE + 4, 0x1e, nameLen))   // +0x12 名字区
    {
        return LineFunc(0xa30,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_DeleteCharac::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081c14b8：param+0x4 != 0 → SendCmdErrorPacket(cmd=6, err&0xff)。
    const int errNo = static_cast<stDeleteCharac&>(param).m_errNo;   // +0x04
    if (errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)6,
                                 (unsigned char)(errNo & 0xff));
    }
}

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
                        __PRETTY_FUNCTION__,
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
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    const _Charac_info& info = user->m_characList[static_cast<int>(slot)];
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xa6e, 0);
    trace("[%s][%d] accid(%s) delete charac(%d) %s",
          __PRETTY_FUNCTION__,
          0xa6e,
          NumberToString(user->get_acc_id(), 0),
          static_cast<int>(info.m_characNo),
          reinterpret_cast<const char*>(info.m_name));
    return 0;
}

DisPatcher_ReturnToSelectCharacter::DisPatcher_ReturnToSelectCharacter() {}
DisPatcher_ReturnToSelectCharacter::~DisPatcher_ReturnToSelectCharacter() {}
int DisPatcher_ReturnToSelectCharacter::dispatch_sig(CUser* user, PacketBuf& /*packet*/)
{
    unsigned char flag = 1;                     // ORIG -0x9(%ebp)
    if (user->get_state() <= 2)
    {
        flag = 0;
    }
    else if (user->getCurCharacVill() == 7)
    {
        flag = 0;
    }
    else
    {
        user->ReturnToSelectCharacList(false);
    }
    PacketGuard guard;
    guard.put_header(1, 7);
    guard.put_byte(flag);
    if (flag != 1)
    {
        guard.put_byte(0x13);
    }
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
int DisPatcher_ReturnToSelectCharacter::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08233490：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}

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
int DisPatcher_GetUserInfo::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c3e7c：get_state(user)==0 → -1，否则 0。
    if (user->get_state() == 0)
        return -1;
    return 0;
}
// ORIG 0x081c3d46 T _ZN22DisPatcher_GetUserInfo4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_GetUserInfo/read.md 逐块映射）：get_short→msg+0xd、
// get_byte→msg+0xf；失败各走 LineFunc(0xf5b/0xf5d, PRETTY, 0, 0)。
int DisPatcher_GetUserInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_USERINFO_REQ& m = *reinterpret_cast<MSG_USERINFO_REQ*>(&msg);
    unsigned short unusedShort = 0;   // ORIG get_short 读 msg+0xd 后未再消费（movzwl 丢弃形态）
    if (!packet.get_short(m.m_targetId))                  // +0x0d
    {
        return LineFunc(0xf5b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(m.m_slot))                       // +0x0f
    {
        return LineFunc(0xf5d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

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
                        __PRETTY_FUNCTION__,
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
// ORIG 0x081c3f4a T _ZN25DisPatcher_RecoverStamina11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_RecoverStamina/check_error.md 逐块映射）：state!=3 → 99；
// SecuProtectionField Check(user,0x2f) 非 0 直返；否则 getCurCharacR 为空 → -1。
int DisPatcher_RecoverStamina::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() != 3)
    {
        return 0x63;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x2f);
    if (secu != 0)
    {
        return secu;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0xffffffff;
    }
    return 0;
}
int DisPatcher_RecoverStamina::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x081c3ea4：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
void DisPatcher_RecoverStamina::send(CUser* user, ParamBase& param)
{
    stOnRecoverStaminaParam& out = static_cast<stOnRecoverStaminaParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 9);
    if (out.m_errNo == 0)
    {
        guard.put_byte(1);
        guard.put_int(user->getCurCharacInvenR()->get_money());
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(out.m_errNo);
    }
    guard.finalize(true);
    user->Send(guard);
}

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
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    static_cast<stOnRecoverStaminaParam&>(param).m_errNo = user->serviceOfShusia();
    return errNo;
}


// ReqPeer 错误包直呼宏（无 helper 符号，等价 ORIG 内联形态）
#define PEER_ERR3(u, r, c)                                                     \
    ((u)->SendCmdPeerErrorPacket(                                              \
         static_cast<ENUM_CMDPACKET>(0xa),                                     \
         static_cast<ENUM_PEER_REQUEST_TYPE>(static_cast<int>(r)),             \
         static_cast<unsigned char>(c)),                                       \
     0)
#define PEER_ERR4(u, r, c, pg)                                                 \
    ((u)->SendCmdPeerErrorPacket(                                              \
         static_cast<ENUM_CMDPACKET>(0xa),                                     \
         static_cast<ENUM_PEER_REQUEST_TYPE>(static_cast<int>(r)),             \
         static_cast<unsigned char>(c), pg),                                   \
     0)

#include "CPrivateStoreMgr.h"  // private_store::GetInstancePrivateStoreMgr
typedef int (*DispatchBig5_AssaultPreventFn)(CUser*, bool*);
static inline int DispatchBig5_callAssaultPrevent(CUser* user)
{
    DispatchBig5_AssaultPreventFn fn =
        reinterpret_cast<DispatchBig5_AssaultPreventFn>(
            &expert_job::CAlchemist::GetAssaultPrevent);
    return fn(user, 0);
}
DisPatcher_ReqPeer::DisPatcher_ReqPeer() {}
DisPatcher_ReqPeer::~DisPatcher_ReqPeer() {}
// ============================================================================
// ORIG T 0x081eed08 _ZN18DisPatcher_ReqPeer12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_ReqPeer/dispatch_sig.md，1379 insn）
// state∈{3,6,8} 门禁 → 三字段读取 → 发起方门禁（私摊/分解店/幽灵/整合 PvP）→
// target 查找与 solo-play/游戏选项拒绝 → check_party_teleport_user(reqType,
// user,target,true) → 按 reqType 分派：0=组队、1=交易、2=PvP 房间、
// 3=战争房间、4=袭击请求、5/7 与 6/8=expert_job。成功尾部统一
// SetReceivedRequestType(target, reqType)。MSG_REQ_TO_PEER 仅作 SoloPlay
// 拒绝包上下文（类型未建模，零缓冲视图【推断】）。
// ============================================================================
int DisPatcher_ReqPeer::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int st = user->get_state();
    if (st != 3 && st != 6 && st != 8)
    {
        return 0;
    }
    PacketGuard pgRes;
    unsigned short peerIdx = 0;                  // ORIG msg+0xd（local_47）
    char requestType = 0;                        // ORIG msg+0xf（local_45）
    unsigned int cookie = 0;                     // ORIG msg+0x10（local_44）
    if (!packet.get_short(peerIdx))
        return LineFunc(0x725a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(requestType))
        return LineFunc(0x725b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(cookie))
        return LineFunc(0x725c,
                        __PRETTY_FUNCTION__,
                        0, 0);

    static char soloCtx[0x14];                   // ORIG local_54 上下文【推断】
    memset(soloCtx, 0, sizeof(soloCtx));
    const MSG_REQ_TO_PEER& soloRef =
        *reinterpret_cast<const MSG_REQ_TO_PEER*>(soloCtx);

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        return PEER_ERR3(user,requestType,0x3c), 0;
    if (user->IsBusyDisjointStore())
        return PEER_ERR3(user,requestType,0xbf), 0;
    if (user->IsCurCharacGhost())
        return PEER_ERR3(user,requestType,0x13), 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() && requestType != 2)
        return PEER_ERR3(user,requestType,0x13), 0;
    CUser* target = G_GameWorld()->find_from_world(peerIdx);
    if (target == 0)
        return PEER_ERR3(user,requestType,3), 0;
    if (target->alonePlay_Mode(soloRef))
    {
        user->Send_SoloPlay_Reject_NotiPack(
            target->getCurCharacName(),
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(1),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(
                target->get_SoloPlay_msgType(soloRef)));
        return 0;
    }
    int optionMap = 0x24;                        // 默认：无对应禁项
    if (requestType == 1)
        optionMap = 0x1f;
    else if (requestType < 2)
    {
        if (requestType == 0)
            optionMap = 6;
    }
    else if (requestType == 2)
        optionMap = 7;
    else if (requestType == 3)
        optionMap = 8;
    if (optionMap != 0x24 &&
        target->GetGameOptionRef()->get_etc_option(
            static_cast<ENUM_OPTION_ETC>(optionMap)) != 0)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            target->getCurCharacName(),
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(0),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(
                target->get_SoloPlay_msgType(soloRef)));
        return 0;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        int tst = target->get_state();
        if (tst == 6 || tst == 7)
            return PEER_ERR3(user,requestType,0x13), 0;
    }
    if (target->IsBusyDisjointStore())
        return PEER_ERR3(user,requestType,0xc0), 0;
    int tpErr = pvp_assault::GetInstanceAssaultMgr()
                    ->check_party_teleport_user(requestType, user, target,
                                                true);
    if (tpErr > 0)
        return 0;                                // 错误包由 check 内部发送
    switch (requestType)
    {
    case 0:
    {
        if (user->get_state() == 5 ||
            user->m_network.get_nat_type() == 0)
            return PEER_ERR3(user,requestType,0x13), 0;
        if (target->isBlackUser(
                static_cast<unsigned int>(user->get_charac_no(-1))))
            return PEER_ERR3(user,requestType,0x4d), 0;
        if (user->isCompetitionMercenary())
            return PEER_ERR3(user,requestType,0xea), 0;
        if (target->isCompetitionMercenary())
            return PEER_ERR3(user,requestType,0xbe), 0;
        if (private_store::GetInstancePrivateStoreMgr()
                ->IsBusyPrivateStore(target))
            return PEER_ERR3(user,requestType,0x3d), 0;
        if (target->IsCurCharacGhost())
            return PEER_ERR3(user,requestType,0x13), 0;
        if (target->get_state() == 5 ||
            target->m_network.get_nat_type() == 0)
            return PEER_ERR3(user,requestType,3), 0;
        if (GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            int villSelf = user->getCurCharacVill();
            int villPeer = target->getCurCharacVill();
            if ((villSelf == 7 && villPeer == 7 &&
                 user->getPowerSide() != target->getPowerSide()) ||
                (villSelf == 7) != (villPeer == 7))
                return PEER_ERR3(user,requestType,0x13), 0;
        }
        CParty* selfParty = static_cast<CParty*>(user->GetParty());
        if (selfParty != 0 &&
            (selfParty->get_member_count() > 3 ||
             !selfParty->CanDoPartyActionAssalutState()))
            return PEER_ERR3(user,requestType,0x13), 0;
        if (user->CheckInParty() && target->CheckInParty())
            return PEER_ERR3(user,requestType,0x12), 0;
        if (!GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            int curSec = GlobalData::s_systemTime_.getCurSec();
            if (DispatchBig5_callAssaultPrevent(user))
            {
                if (curSec < target->GetCurCharacChaosKillTime() + 600)
                    return PEER_ERR3(user,requestType,0xfa), 0;
                CParty* peerParty =
                    static_cast<CParty*>(target->GetParty());
                if (peerParty != 0 &&
                    peerParty->CheckMemberLastTryAssaultTime())
                    return PEER_ERR3(user,requestType,0xfa), 0;
            }
            if (curSec < user->GetCurCharacChaosKillTime() + 600 &&
                DispatchBig5_callAssaultPrevent(target))
                return PEER_ERR3(user,requestType,0xfa), 0;
            if (selfParty != 0 &&
                selfParty->CheckMemberLastTryAssaultTime() &&
                DispatchBig5_callAssaultPrevent(target))
                return PEER_ERR3(user,requestType,0xfa), 0;
        }
        unsigned short spareShort = 0;           // ORIG local_62 覆盖式读
        packet.get_short(spareShort);            // ORIG 忽略返回值
        pgRes.clear();
        pgRes.put_header(0, 7);
        pgRes.put_short(user->get_unique_id() & 0xffff);
        pgRes.put_byte(0);
        pgRes.put_int(static_cast<int>(cookie));
        pgRes.put_short(
            static_cast<int>(user->getCurCharacTotalMaxFatigue()) -
            static_cast<int>(user->getCurCharacTotalFatigue()));
        pgRes.put_short(
            static_cast<int>(user->getCurCharacStamina()) & 0xff);
        pgRes.put_short((int)(unsigned short)spareShort);
        pgRes.finalize(true);
        target->Send(pgRes);
        break;
    }
    case 1:
    {
        bool restricted =
            ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                user, static_cast<RestrictCategory::Enum>(1), 6) ||
            ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                target, static_cast<RestrictCategory::Enum>(1), 6);
        if (restricted)
            return PEER_ERR3(user,requestType,0xd1), 0;
        int secuErr =
            GlobalData::s_pSecuProtectionField->CheckOpposite(
                user, static_cast<SECURITY_PROTCTION>(0x36), target);
        if (secuErr != 0)
            return PEER_ERR3(user,requestType,secuErr & 0xff), 0;
        if (private_store::GetInstancePrivateStoreMgr()
                ->IsBusyPrivateStore(target))
            return PEER_ERR3(user,requestType,0x3d), 0;
        if (target->IsCurCharacGhost())
            return PEER_ERR3(user,requestType,0x13), 0;
        if (target->isBlackUser(
                static_cast<unsigned int>(user->get_charac_no(-1))))
            return PEER_ERR3(user,requestType,0x4d), 0;
        if (target->CheckInTrade())
            PEER_ERR3(user,requestType,0x12);
        else if (user->get_state() == target->get_state())
        {
            pgRes.put_header(0, 7);
            pgRes.put_short(user->get_unique_id() & 0xffff);
            pgRes.put_byte(1);
            pgRes.put_int(static_cast<int>(cookie));
            pgRes.put_int(
                static_cast<int>(user->getCurCharacCreateTime()));
            pgRes.finalize(true);
            target->Send(pgRes);
            target->SetReceivedRequestType(requestType);
            return 0;
        }
        else
            PEER_ERR3(user,requestType,0x13);
        break;
    }
    case 2:
    {
        if (user->GetPVPRoom() == 0)
            return LineFunc(0x7494,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (private_store::GetInstancePrivateStoreMgr()
                ->IsBusyPrivateStore(target))
            return PEER_ERR3(user,requestType,0x3d), 0;
        if (target->IsCurCharacGhost())
            return PEER_ERR3(user,requestType,0x13), 0;
        if (target->isCompetitionMercenary())
            return PEER_ERR3(user,requestType,0xbe), 0;
        if (target->get_state() > 4)
            return 0;
        pgRes.put_header(0, 7);
        pgRes.put_short(user->get_unique_id() & 0xffff);
        pgRes.put_byte(2);
        pgRes.put_int((int)(unsigned short)peerIdx);
        pgRes.finalize(true);
        target->Send(pgRes);
        break;
    }
    case 3:
    {
        WarRoom* war = static_cast<WarRoom*>(user->GetWarRoom());
        if (war == 0)
            return LineFunc(0x74bf,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (target->get_state() > 4)
            return 0;
        if (war->GetState() == 2)
            return 0;
        pgRes.put_header(0, 7);
        pgRes.put_short(user->get_unique_id() & 0xffff);
        pgRes.put_byte(3);
        pgRes.put_int((int)(unsigned short)peerIdx);
        pgRes.finalize(true);
        target->Send(pgRes);
        break;
    }
    case 4:
    {
        int secuErr =
            GlobalData::s_pSecuProtectionField->Check(
                user, static_cast<SECURITY_PROTCTION>(0x37));
        if (secuErr != 0)
            return PEER_ERR3(user,requestType,secuErr & 0xff), 0;
        if (target->get_state() > 4)
            return 0;
        if (GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()) ||
            GlobalData::s_pGMAccounts_->isGM(target->get_acc_id()))
            return 0;
        if (!pvp_assault::GetInstanceAssaultMgr()->OnRequestAssault(
                user, target, 0))
            return 0x7590;
        break;
    }
    case 5:
    case 6:
    case 7:
    case 8:
    {
        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                user, static_cast<RestrictCategory::Enum>(1), 8))
            return PEER_ERR4(user,requestType,0xd1,pgRes), 0;
        int secuErr = 0;
        if (requestType == 5 || requestType == 7)
            secuErr =
                GlobalData::s_pSecuProtectionField->Check(
                    user, static_cast<SECURITY_PROTCTION>(0x21));
        else
            secuErr =
                GlobalData::s_pSecuProtectionField->CheckOpposite(
                    user, static_cast<SECURITY_PROTCTION>(0x20), target);
        if (secuErr != 0)
            return PEER_ERR4(user,requestType,secuErr & 0xff,pgRes), 0;
        int expertOwner =
            (requestType == 5 || requestType == 7)
                ? target->GetCurCharacExpertJobType()
                : user->GetCurCharacExpertJobType();
        if (expertOwner != 1)
            return PEER_ERR4(user,requestType,0x17,pgRes), 0;
        GlobalData::s_ExpertJobMgr->OnRequestPeer(
            user, target,
            static_cast<ENUM_PEER_REQUEST_TYPE>(
                static_cast<int>(requestType)),
            pgRes);
        break;
    }
    default:
        break;
    }
    target->SetReceivedRequestType(requestType);
    return 0;
}
int DisPatcher_ReqPeer::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08232dde：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
int DisPatcher_ReqPeer::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232de8 W _ZN18DisPatcher_ReqPeer7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081eed08)

DisPatcher_ResPeer::DisPatcher_ResPeer() {}
DisPatcher_ResPeer::~DisPatcher_ResPeer() {}
// ============================================================================
// ORIG T 0x081f088e _ZN18DisPatcher_ResPeer12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_ResPeer/dispatch_sig.md，2061 insn）
// state==3 门禁 → 三字段（peerIdx/msg+0xd、requestType/msg+0xf、cookie/msg+0x10）
// → GetReceivedRequestType 匹配校验（5/7 与 6/8 成对）→ 按 requestType 分派：
//   0=组队邀请应答  1=交易邀请应答  2=PvP 房间加入  3=战争房间加入
//   5/7 与 6/8=expert_job 应答（两侧参数互换）。错误包形态：
//   put_header(1,0xb)/put_byte(0)/put_byte(code)/put_byte(tail)/finalize(true)。
// 汇编中 GetAssaultPrevent 以两参栈式调用（this 经 ecx），沿用
// W6_AssaultMgrDeps.cpp 同款成员函数指针 ABI 转接。
// ============================================================================
int DisPatcher_ResPeer::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3)
    {
        return 0;
    }

    PacketGuard pgRes;                       // ORIG -0xac（全程复用）
    unsigned short peerIdx = 0;              // ORIG msg+0xd（local_97）
    char requestType = 0;                    // ORIG msg+0xf（local_95）
    unsigned int cookie = 0;                 // ORIG msg+0x10（local_94）
    if (!packet.get_short(peerIdx))
    {
        return LineFunc(0x77d5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(requestType))
    {
        return LineFunc(0x77d6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(cookie))
    {
        return LineFunc(0x77d7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    unsigned char recvType = user->GetReceivedRequestType();
    if (recvType == 5 || recvType == 7)
    {
        if (requestType != 5 && requestType != 7)
            return 0;
    }
    else if (recvType == 6 || recvType == 8)
    {
        if (requestType != 6 && requestType != 8)
            return 0;
    }
    else if ((char)recvType != requestType)
    {
        return 0;
    }
    user->InitReceivedRequestType();

    switch (requestType)
    {
    case 0:                                  // ---- 组队邀请应答 ----
    {
        PacketGuard pgParty;                 // ORIG -0xb8
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        if (target == 0)
        {
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            pgParty.put_byte(3);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        bool sameUser = (target == user) ||
                        (target->getCurCharacNo() ==
                         user->getCurCharacNo());
        if (sameUser)
        {
            return 0x7828;
        }
        if (user->get_state() == 5 || target->get_state() == 5)
        {
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            pgParty.put_byte((char)tpErr);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        if (target->get_state() != 3)
        {
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            pgParty.put_byte(0x13);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        bool bothInParty = user->CheckInParty() && target->CheckInParty();
        if (bothInParty)
        {
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            pgParty.put_byte(0x12);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        if (user->isCompetitionMercenary())
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(0xea));
            break;
        }
        if (target->isCompetitionMercenary())
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(0xea));
            break;
        }

        int villSelf = user->getCurCharacVill();
        int villPeer = target->getCurCharacVill();
        if (GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            if (villSelf == 7 && villPeer == 7)
            {
                if (user->getPowerSide() != target->getPowerSide())
                {
                    pgParty.put_header(1, 0xb);
                    pgParty.put_byte(0);
                    pgParty.put_byte(0x13);
                    pgParty.put_byte(0);
                    pgParty.finalize(true);
                    user->Send(pgParty);
                    break;
                }
            }
            else if ((villSelf == 7) != (villPeer == 7))
            {
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(0x13);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
        }
        else
        {
            // ---- 混沌袭击时间窗四连检查（任一命中 → 0xfa 错误包）----
            int curSec = GlobalData::s_systemTime_.getCurSec();
            bool blocked =
                (curSec < target->GetCurCharacChaosKillTime() + 600 &&
                 DispatchBig5_callAssaultPrevent(user)) ||
                (curSec < user->GetCurCharacChaosKillTime() + 600 &&
                 DispatchBig5_callAssaultPrevent(target));
            CParty* selfParty = static_cast<CParty*>(user->GetParty());
            if (!blocked && selfParty != 0 &&
                selfParty->CheckMemberLastTryAssaultTime() &&
                DispatchBig5_callAssaultPrevent(target))
            {
                blocked = true;
            }
            CParty* peerParty = static_cast<CParty*>(target->GetParty());
            if (!blocked && peerParty != 0 &&
                peerParty->CheckMemberLastTryAssaultTime() &&
                DispatchBig5_callAssaultPrevent(user))
            {
                blocked = true;
            }
            if (blocked)
            {
                user->SendCmdPeerErrorPacket(
                    static_cast<ENUM_CMDPACKET>(0xb),
                    static_cast<ENUM_PEER_REQUEST_TYPE>(
                        static_cast<int>(requestType)),
                    static_cast<unsigned char>(0xfa), pgRes);
                break;
            }
        }

        // ---- 组队建立/加入主流程（LAB_081f12d8）----
        bool neitherInParty =
            !user->CheckInParty() && !target->CheckInParty();
        if (neitherInParty)
        {
            CParty* newParty = G_CGameManager()->GetParty();
            if (newParty == 0)
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(4);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
            newParty->create_party(target);          // ORIG 忽略返回值
            newParty->setDungIndex((unsigned short)0);
            newParty->setDungDiffi(static_cast<unsigned char>(5));
            newParty->join_user(user);
            newParty->send_party_realtime_info();
            pgParty.clear();
            pgParty.put_header(0, 8);
            pgParty.put_short(user->get_unique_id() & 0xffff);
            pgParty.put_byte(0);
            pgParty.put_int(static_cast<int>(peerIdx) & 0xffff);
            pgParty.finalize(true);
            target->Send(pgParty);
            newParty->send_party_ipinfo();
            G_GameWorld()->send_party_info_to_all(newParty, 0);
            break;
        }
        bool inviterHasNoParty =
            user->CheckInParty() && !target->CheckInParty();
        if (inviterHasNoParty)
        {
            CParty* party = static_cast<CParty*>(target->GetParty());
            if (party == 0)
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(4);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
            if (!party->CanDoPartyActionAssalutState())
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(0x13);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
            int joinErr = party->join_user(target);
            if (joinErr > 0)
            {
                party->send_quick_party_matching_result(false, 0);
                party->set_quick_party(false);
                party->send_party_realtime_info();
                pgParty.clear();
                pgParty.put_header(0, 8);
                pgParty.put_short(user->get_unique_id() & 0xffff);
                pgParty.put_byte(0);
                pgParty.put_int(static_cast<int>(peerIdx) & 0xffff);
                pgParty.finalize(true);
                target->Send(pgParty);
                party->send_party_ipinfo();
                G_GameWorld()->send_party_info_to_all(party, 2);
                break;
            }
            pgParty.clear();
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            if (joinErr == -2)
                pgParty.put_byte(0x13);
            else if (joinErr == -1)
                pgParty.put_byte(4);
            else
                pgParty.put_byte(1);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        bool inviteeLoner = !user->CheckInParty() && target->CheckInParty();
        if (inviteeLoner)
        {
            target->GetPartyIndex();                 // ORIG 直呼丢弃返回值
            CParty* party = G_CGameManager()->GetParty();
            if (party == 0)
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(4);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
            if (!party->CanDoPartyActionAssalutState())
            {
                pgParty.clear();
                pgParty.put_header(1, 0xb);
                pgParty.put_byte(0);
                pgParty.put_byte(0x13);
                pgParty.put_byte(0);
                pgParty.finalize(true);
                user->Send(pgParty);
                break;
            }
            int joinErr = party->join_user(user);
            if (joinErr > 0)
            {
                party->send_quick_party_matching_result(false, 0);
                party->set_quick_party(false);
                party->send_party_realtime_info();
                pgParty.clear();
                pgParty.put_header(0, 8);
                pgParty.put_short(user->get_unique_id() & 0xffff);
                pgParty.put_byte(0);
                pgParty.put_int(static_cast<int>(peerIdx) & 0xffff);
                pgParty.finalize(true);
                target->Send(pgParty);
                party->send_party_ipinfo();
                G_GameWorld()->send_party_info_to_all(party, 2);
                break;
            }
            pgParty.clear();
            pgParty.put_header(1, 0xb);
            pgParty.put_byte(0);
            if (joinErr == -2)
                pgParty.put_byte(0x13);
            else if (joinErr == -1)
                pgParty.put_byte(4);
            else
                pgParty.put_byte(1);
            pgParty.put_byte(0);
            pgParty.finalize(true);
            user->Send(pgParty);
            break;
        }
        break;
    }
    case 1:                                      // ---- 交易邀请应答 ----
    {
        if (user->CheckInTrade())
        {
            break;
        }
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        int err = -1;                            // ORIG -0x54
        if (target == 0)
        {
            err = 3;
        }
        else if (!target->CheckInTrade())
        {
            if (user->get_state() != target->get_state())
            {
                err = 0x13;
            }
        }
        else
        {
            err = 0x12;
        }
        if (err != -1)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(err);
            pgRes.put_byte(1);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte((char)tpErr);
            pgRes.put_byte(1);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        CTradeSpace* ts = G_CGameManager()->GetTradeSpace();
        if (ts == 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(1);
            pgRes.put_byte(1);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        ts->set_traders(target, user);           // 参数序照 asm（target 先）
        pgRes.put_header(0, 8);
        pgRes.put_short(user->get_unique_id() & 0xffff);
        pgRes.put_byte(1);
        pgRes.put_int(static_cast<int>(cookie));
        pgRes.put_byte(
            static_cast<int>(user->GetReliablePerson()) & 0xff);
        pgRes.put_int(
            static_cast<int>(user->getCurCharacCreateTime()));
        pgRes.finalize(true);
        target->Send(pgRes);
        pgRes.clear();
        pgRes.put_header(1, 0xb);
        pgRes.put_byte(1);
        pgRes.put_short(static_cast<int>(peerIdx) & 0xffff);
        pgRes.put_byte(1);
        pgRes.put_byte(
            static_cast<int>(target->GetReliablePerson()) & 0xff);
        pgRes.finalize(true);
        user->Send(pgRes);
        break;
    }
    case 2:                                      // ---- PvP 房间加入 ----
    {
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        if (target == 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(3);
            pgRes.put_byte(2);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte((char)tpErr);
            pgRes.put_byte(2);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        short pvpIdx = target->GetPvpIndex();
        PvP_Room* room =
            G_CGameManager()->GetPvp(static_cast<int>(pvpIdx), user, 0);
        if (room == 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(0x13);
            pgRes.put_byte(2);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        int seatOut = 0;                         // ORIG -0xbc
        bool sameTeamJoin = G_GameWorld()->IsSameTeamChannelJoin();
        int joinRes = room->join_room(user, seatOut, sameTeamJoin);
        int battleMode = room->get_pvp_battle_mode();   // ORIG -0xcc
        int roomIdx = room->get_index();         // edi
        const char* accName = user->get_acc_name();
        const char* charName = user->getCurCharacName();
        CSwitchLog trace(
            __PRETTY_FUNCTION__,
            0x7a82, 0, 0);
        trace("pvp@log %s,%s,room(%d),%d,%d,%d",
              accName, charName, roomIdx, battleMode, joinRes, 0xb);
        if (joinRes > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(joinRes);
            pgRes.put_byte(2);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        G_CGameManager()->CheckOutParty(user, false);
        room->make_seat_info(reinterpret_cast<char*>(&pgRes), seatOut);
        G_GameWorld()->send_all(pgRes);
        G_GameWorld()->goto_pvp(user);
        break;
    }
    case 3:                                      // ---- 战争房间加入 ----
    {
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        if (target == 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(3);
            pgRes.put_byte(3);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte((char)tpErr);
            pgRes.put_byte(3);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        short warIdx = target->GetWarRoomIndex();
        WarRoom* war = G_CGameManager()->GetWarRoom(warIdx);
        if (war == 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(0x13);
            pgRes.put_byte(3);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        int seatOut = 0;                         // ORIG -0xc0
        int res = war->IsJoinable(user);
        if (res > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(res);
            pgRes.put_byte(3);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        res = war->Join(user, seatOut);
        if (res > 0)
        {
            pgRes.put_header(1, 0xb);
            pgRes.put_byte(0);
            pgRes.put_byte(res);
            pgRes.put_byte(3);
            pgRes.finalize(true);
            user->Send(pgRes);
            break;
        }
        G_CGameManager()->CheckOutParty(user, false);
        war->MakeSlotInfo(reinterpret_cast<char*>(&pgRes), seatOut);
        G_GameWorld()->send_all(pgRes);
        G_GameWorld()->goto_warroom(user);
        if (war->GetWaiterCount() > 2 && war->GetState() == 0)
        {
            war->Start();
        }
        break;
    }
    case 5:
    case 7:                                      // ---- expert_job 应答（A 组）----
    {
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        if (target == 0)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(3), pgRes);
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(tpErr & 0xff), pgRes);
            break;
        }
        if (target->GetCurCharacExpertJobType() != 1)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(0x17), pgRes);
            break;
        }
        GlobalData::s_ExpertJobMgr->OnResponsePeer(
            user, target, user, target,
            static_cast<int>(cookie),
            static_cast<ENUM_PEER_REQUEST_TYPE>(
                static_cast<int>(requestType)),
            pgRes);
        break;
    }
    case 6:
    case 8:                                      // ---- expert_job 应答（B 组，owner 互换）----
    {
        CUser* target =
            G_GameWorld()->find_from_world(peerIdx);
        if (target == 0)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(3), pgRes);
            break;
        }
        int tpErr = pvp_assault::GetInstanceAssaultMgr()
                        ->check_party_teleport_user(0, user, target, false);
        if (tpErr > 0)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(tpErr & 0xff), pgRes);
            break;
        }
        if (target->GetCurCharacExpertJobType() != 1)
        {
            user->SendCmdPeerErrorPacket(
                static_cast<ENUM_CMDPACKET>(0xb),
                static_cast<ENUM_PEER_REQUEST_TYPE>(
                    static_cast<int>(requestType)),
                static_cast<unsigned char>(0x17), pgRes);
            break;
        }
        GlobalData::s_ExpertJobMgr->OnResponsePeer(
            user, target, target, user,
            static_cast<int>(cookie),
            static_cast<ENUM_PEER_REQUEST_TYPE>(
                static_cast<int>(requestType)),
            pgRes);
        break;
    }
    default:
        break;
    }
    return 0;
}
int DisPatcher_ResPeer::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08232ea2：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
int DisPatcher_ResPeer::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232eac W _ZN18DisPatcher_ResPeer7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f088e)

DisPatcher_SetPartyInfo::DisPatcher_SetPartyInfo() {}
DisPatcher_SetPartyInfo::~DisPatcher_SetPartyInfo() {}
int DisPatcher_SetPartyInfo::check_error(CUser* /*user*/, MSG_BASE& msg)
{
    MSG_PARTY_SET_INFO& req = *reinterpret_cast<MSG_PARTY_SET_INFO*>(&msg);
    std::string title(reinterpret_cast<char*>(&req.m_title), std::allocator<char>());
    if (CSyncSlangFilter::GetInstance()->HasSlang(title))
    {
        return 0x9f;
    }
    return 0;
}
// ORIG 0x081c4396 T _ZN23DisPatcher_SetPartyInfo4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_SetPartyInfo/read.md 汇编+反编译 C 全文转写）：
// get_byte(titleIndex) → titleIndex==0 时 get_int(+0x0e) 并 get_str(title,0x20,+0x0e)
// → get_byte(userMax)/get_short(dungIndex)/get_byte(dungDiffi)；任一失败 LineFunc
// 0x10d9/0x10dd/0x10e0/0x10e2/0x10e5/0x10e6，全成功返回 0。
int DisPatcher_SetPartyInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PARTY_SET_INFO& req = *reinterpret_cast<MSG_PARTY_SET_INFO*>(&msg);
    if (!packet.get_byte(req.m_titleIndex))
    {
        return LineFunc(0x10d9, __PRETTY_FUNCTION__, 0, 0);
    }
    if (req.m_titleIndex == 0)
    {
        if (!packet.get_int(*reinterpret_cast<int*>(&req.m_pad0e[0])))
        {
            return LineFunc(0x10dd, __PRETTY_FUNCTION__, 0, 0);
        }
        if (!packet.get_str(req.m_title, 0x20,
                            *reinterpret_cast<int*>(&req.m_pad0e[0])))
        {
            return LineFunc(0x10e0, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    if (!packet.get_byte(req.m_userMax))
    {
        return LineFunc(0x10e2, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_dungIndex))
    {
        return LineFunc(0x10e5, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(req.m_dungDiffi))
    {
        return LineFunc(0x10e6, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void DisPatcher_SetPartyInfo::send(CUser* user, ParamBase& param)
{
    stSetPartyInfo& p = *reinterpret_cast<stSetPartyInfo*>(&param);
    if (p.m_errNo != 0x7fffffff && p.m_errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc, (unsigned char)(p.m_errNo & 0xff));
    }
}

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
                        __PRETTY_FUNCTION__,
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
int DisPatcher_LeaveParty::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c436e：user->GetParty()==0 → 0x12，否则 0。
    if (user->GetParty() == 0)
        return 0x12;
    return 0;
}
int DisPatcher_LeaveParty::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (user->get_state() != 3)
    {
        return LineFunc(0x208,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int DisPatcher_LeaveParty::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x081c424c：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
void DisPatcher_LeaveParty::send(CUser* user, ParamBase& param)
{
    stLeavePartyParam& p = *reinterpret_cast<stLeavePartyParam*>(&param);
    if (p.m_errNo != 0x7fffffff && p.m_errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd, (unsigned char)(p.m_errNo & 0xff));
    }
}

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
                        __PRETTY_FUNCTION__,
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
// ORIG 0x081cb062 T _ZN23DisPatcher_WalkoutParty11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_WalkoutParty/check_error.md 逐块映射）：user 空 →
// LineFunc(0x2053)；state==3 → 0；否则 -1。
int DisPatcher_WalkoutParty::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return LineFunc(0x2053,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() == 3)
    {
        return 0;
    }
    return 0xffffffff;
}
// ORIG 0x081caf42 T _ZN23DisPatcher_WalkoutParty4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_WalkoutParty/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x202b, PRETTY, 0, 0)。
int DisPatcher_WalkoutParty::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_WALKOUT_PARTY& m = *reinterpret_cast<MSG_WALKOUT_PARTY*>(&msg);
    if (!packet.get_byte(m.m_walkoutSlot))                // +0x0d
    {
        return LineFunc(0x202b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_WalkoutParty::send(CUser* user, ParamBase& param)
{
    stWalkoutPartyParam& out = static_cast<stWalkoutPartyParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0xe);
    if (out.m_errNo == 0x13)
    {
        guard.put_byte(0);
        guard.put_byte(0x13);
        guard.finalize(true);
        user->Send(guard);
    }
    else if (0 < out.m_errNo)
    {
        guard.put_byte(0);
        guard.put_byte(out.m_errNo);
        guard.finalize(true);
        user->Send(guard);
    }
}

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
int DisPatcher_StartGame::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() == 3 || user->get_state() == 0xb)
    {
        // state 合法，继续
    }
    else
    {
        return 0x7fffffff;
    }
    stStartGameParam& paramRef = static_cast<stStartGameParam&>(param);
    int* local_18 = &paramRef.m_8;
    if (G_GameWorld()->IsPVPChannel() != false)
    {
        *local_18 = 0xffffffff;
        return 8;
    }
    if (user->IsBusyDisjointStore() != false)
    {
        *local_18 = 0xffffffff;
        return 0x13;
    }
    if (user->isCompetitionMercenary() != false)
    {
        *local_18 = 0xffffffff;
        return 0xea;
    }
    CParty* pCVar5 = static_cast<CParty*>(user->GetParty());
    if (pCVar5 != 0)
    {
        CPartyTelePort* pCVar6 = static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort());
        char cVar2 = pCVar6->get_teleport_state();
        bool bVar1;
        if (cVar2 < '\0')
        {
            bVar1 = false;
        }
        else
        {
            pCVar6 = static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort());
            cVar2 = pCVar6->get_teleport_state();
            if ('\x03' < cVar2)
            {
                bVar1 = false;
            }
            else
            {
                bVar1 = true;
            }
        }
        if (bVar1)
        {
            pCVar6 = static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort());
            pCVar6->reset_teleport_data();
            pCVar6 = static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort());
            pCVar6->send_teleport_status('\0');
        }
    }
    const char* pcVar7 = user->getCurCharacName();
    if (G_GameWorld()->IsForbiddenMove(pcVar7) != false)
    {
        pcVar7 = user->getCurCharacName();
        int iVar3 = G_GameWorld()->GetLeftTimeOfForbiddenMove(pcVar7);
        if (iVar3 != 0)
        {
            *local_18 = 0;
            return 0x44;
        }
        pcVar7 = user->getCurCharacName();
        G_GameWorld()->EnableMove(pcVar7);
    }
    if (user->CheckInParty() != false && user->get_state() == 3)
    {
        pCVar5 = static_cast<CParty*>(user->GetParty());
        if (pCVar5 != 0)
        {
            if (pCVar5->CheckMemberArea() == '\x01' || pCVar5->is_quick_party() == '\x01')
            {
                // 成员同区或快速队伍，继续
            }
            else
            {
                *local_18 = 0xffffffff;
                return 0x15;
            }
            if (excludeAddHackCount(user) != false)
            {
                WongWork::CHackAnalyzer* pCVar8 = user->getHackAnalyzer();
                pCVar8->addServerHackCnt(user, static_cast<WongWork::ENUM_HACKTYPE>(0x1f6), 1, 0, 0);
            }
            int iVar3b = G_GameWorld()->GetWorldMapIndex(user);
            if (0 < iVar3b)
            {
                CWorldMap* this_00 = G_CDataManager()->m_worldMapList.find_world_map(iVar3b);
                if (this_00 == 0)
                {
                    *local_18 = 0xffffffff;
                    return 0x15;
                }
                if (GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon() != '\x01')
                {
                    int iVar3c = this_00->getDungeonMinLevel();
                    int iVar9 = pCVar5->getMemberMinLevel();
                    if (iVar9 + 6 <= iVar3c)
                    {
                        *local_18 = 0xffffffff;
                        return 0xe;
                    }
                }
            }
            return 0;
        }
    }
    return 0;
}
int DisPatcher_StartGame::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x081c9f62：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
void DisPatcher_StartGame::send(CUser* user, ParamBase& param)
{
    stStartGameParam& out = static_cast<stStartGameParam&>(param);
    if (out.m_errNo != 0x7fffffff && out.m_errNo != 0)
    {
        PacketGuard guard;
        guard.put_header(1, 0xf);
        guard.put_byte(0);
        guard.put_byte(out.m_errNo);
        if (out.m_8 >= 0)
        {
            guard.put_byte(out.m_8);
        }
        guard.finalize(true);
        user->Send(guard);
    }
}

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
// ORIG 0x081c7f32 T _ZN24DisPatcher_SelectDungeon11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_SelectDungeon/check_error.md 汇编+反编译 C 全文转写）：
// state∉{5,0xd} 或 IsBusyDisjointStore → 0x7fffffff；m_field10∉[0,2] 或
// m_dungeonType<0 → -1；m_field11∉[0,1] → -2；m10>0 且 m11>0 → 0x7fffffff；
// m10!=2 时要求队长（getManager()==user）、party state==1、!(m11==1&&is_quick_party)；
// 两段重复 party 检查（ORIG 0x0d9 与 0x0a9 两块），全部通过 → 0。
int DisPatcher_SelectDungeon::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const MSG_SELECT_DUNGEON& req =
        *reinterpret_cast<const MSG_SELECT_DUNGEON*>(&msg);
    if (user->get_state() != 5 && user->get_state() != 0xd)
    {
        return 0x7fffffff;
    }
    if (user->IsBusyDisjointStore())
    {
        return 0x7fffffff;
    }
    if ((char)(unsigned char)req.m_field10 < 0 ||
        (char)(unsigned char)req.m_field10 > 2 ||
        (char)(unsigned char)req.m_dungeonType < 0)
    {
        return -1;
    }
    if ((char)(unsigned char)req.m_field11 < 0 ||
        (char)(unsigned char)req.m_field11 > 1)
    {
        return -2;
    }
    if ((char)(unsigned char)req.m_field10 > 0 &&
        (char)(unsigned char)req.m_field11 > 0)
    {
        return 0x7fffffff;
    }
    if ((unsigned char)req.m_field10 == 2)
    {
        // 第一段（ORIG +0x0d3/+0x165 两处 m10==2 检查形状保留）
    }
    else
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party == 0)
        {
            return 0x7fffffff;
        }
        if (party->getManager() != user)
        {
            return 0x7fffffff;
        }
        if (party->get_state() != 1)
        {
            return 0x7fffffff;
        }
        if (req.m_field11 == 1 && party->is_quick_party())
        {
            return 0x7fffffff;
        }
    }
    if ((unsigned char)req.m_field10 == 2)
    {
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return 0x7fffffff;
    }
    if (party->getManager() != user)
    {
        return 0x7fffffff;
    }
    if (party->get_state() != 1)
    {
        return 0x7fffffff;
    }
    return 0;
}
int DisPatcher_SelectDungeon::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SELECT_DUNGEON& req = *reinterpret_cast<MSG_SELECT_DUNGEON*>(&msg);
    if (!packet.get_short(req.m_dungeonIdx))
    {
        return LineFunc(0x18da,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_dungeonType)))
    {
        return LineFunc(0x18db,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_field10)))
    {
        return LineFunc(0x18e2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_field11)))
    {
        return LineFunc(0x18e4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_SelectDungeon::send(CUser* user, ParamBase& param)
{
    stSelectDungeonParam* local_14 = &static_cast<stSelectDungeonParam&>(param);
    if (local_14->m_4 != 0x7fffffff)
    {
        if (local_14->m_4 != 0)
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x10),
                                     static_cast<unsigned char>(local_14->m_4));
        }
        if (local_14->m_c != 0 || local_14->m_8 != 0)
        {
            CParty* local_10 = static_cast<CParty*>(user->GetParty());
            if (local_10 != 0)
            {
                if (local_14->m_8 == local_14->m_c)
                {
                    PacketGuard local_20;
                    local_20.put_header(1, 0x10);
                    local_20.put_byte(0);
                    local_20.put_byte(local_14->m_8);
                    if (local_14->m_8 == 0x16)
                    {
                        local_20.put_byte(local_14->m_14);
                    }
                    local_20.finalize(true);
                    local_10->send_to_party(local_20);
                }
                else if (local_14->m_8 == 0)
                {
                    if (local_14->m_c != 0)
                    {
                        PacketGuard local_38;
                        local_38.put_header(1, 0x10);
                        local_38.put_byte(0);
                        local_38.put_byte(local_14->m_8);
                        if (local_14->m_c == 0xad)
                        {
                            local_38.put_byte(local_14->m_14);
                        }
                        local_38.finalize(true);
                        local_10->send_to_party(local_38);
                    }
                }
                else
                {
                    PacketGuard local_2c;
                    local_2c.put_header(1, 0x10);
                    local_2c.put_byte(0);
                    local_2c.put_byte(local_14->m_8);
                    local_2c.finalize(true);
                    user->Send(local_2c);
                }
            }
        }
    }
}

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
        // CParty::m_partyType(+0xcd8)（CParty.h public 区，具名访问）
        party->m_partyType = (int)(char)req.m_field11;
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
// dispatch_sig 真实实现见文件尾第二批段（ORIG T 0x081f3540）。
int DisPatcher_SendMess::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08232f66：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}

// ORIG 0x08232f70 W _ZN19DisPatcher_SendMess7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部聊天业务（type 分派/表情改写/敏感词 Filter/消耗品扣减/community+monitor 监视包/
// DB_InsertPoliceSaveChatting/GameWorld::send_chat_msg）内嵌于
// dispatch_sig(0x081f3540，报告 docs/class_func_reports/DisPatcher_SendMess/dispatch_sig.md)。
int DisPatcher_SendMess::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_DeleteItem::DisPatcher_DeleteItem() {}
DisPatcher_DeleteItem::~DisPatcher_DeleteItem() {}
// ORIG 0x081c1a6c T _ZN21DisPatcher_DeleteItem11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_DeleteItem/check_error.md 逐块映射）：背包校验和
// getCurCharacInvenCheckSum 与 msg+0x4bf 不一致 → addServerHackCnt(0x25b,1,0,0)；恒返 0。
int DisPatcher_DeleteItem::check_error(CUser* user, MSG_BASE& msg)
{
    // msg+0x4bf 为线格式尾部校验和（超 MSG_DELETE_ITEM sizeof 0x4bf 的包尾字段，
    // 无具名字段可表达，保留视图）
    if (user->getCurCharacInvenCheckSum() !=
        *reinterpret_cast<const int*>(reinterpret_cast<const char*>(&msg) + 0x4bf))
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25b, 1u, 0u, 0u);
    }
    return 0;
}
// ORIG 0x081c17c0 T _ZN21DisPatcher_DeleteItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_DeleteItem/read.md（汇编+反编译 C 全读）
int DisPatcher_DeleteItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_DELETE_ITEM& m = *reinterpret_cast<MSG_DELETE_ITEM*>(&msg);
    if (!packet.get_byte(m.m_itemSpace))                   // +0x0d
    {
        return LineFunc(0xae6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_count)))  // +0x0e（signed char→char& 适配）
    {
        return LineFunc(0xae7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!(m.m_count < 0x65))
    {
        return LineFunc(0xaeb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < (char)m.m_count; ++i)
    {
        if (!packet.get_short(m.m_opType[i]))              // +0x0f+2i
        {
            return LineFunc(0xaf0,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_short(m.m_position[i]))            // +0xd7+2i
        {
            return LineFunc(0xaf1,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(m.m_itemIdx[i]))               // +0x19f+4i
        {
            return LineFunc(0xaf2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_int(m.m_countArr[i]))              // +0x32f+4i
        {
            return LineFunc(0xaf3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (m.m_countArr[i] < 1)
        {
            return LineFunc(0xaf5,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    // 线格式尾部校验和（超对象 sizeof，无具名字段）
    if (!packet.get_int(*reinterpret_cast<int*>(reinterpret_cast<char*>(&m) + 0x4bf)))
    {
        return LineFunc(0xaf8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
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

    const bool skip = (party->m_partyType == 1) &&
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
    const int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_itemSpace);
    if (user->CheckItemLock(invenType, (int)req.m_position[idx]))
    {
        return 0xd5;   // 物品锁定
    }
    return 0;
}
// ORIG 0x081c2792 T _ZN21DisPatcher_DeleteItem4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_DeleteItem/send.md 汇编+反编译 C 全文转写）：
// m_4!=0x7fffffff 时发包。m_4==0：put_header(1,0x12)/1/m_8/m_9，i<(char)m_9 循环
// put_short(m_slotsD2[i])/put_int(m_counts19c[i])/put_short(m_slotsA[i])，
// finalize(true)+Send；否则 0/errNo/m_8 三字节包。
void DisPatcher_DeleteItem::send(CUser* user, ParamBase& param)
{
    stDeleteItemParam& p = static_cast<stDeleteItemParam&>(param);
    PacketGuard guard;
    if (p.m_4 != 0x7fffffff)
    {
        if (p.m_4 == 0)
        {
            guard.put_header(1, 0x12);
            guard.put_byte(1);
            guard.put_byte((char)p.m_8);
            guard.put_byte((char)p.m_9);
            for (int i = 0; i < (int)(char)p.m_9; ++i)
            {
                guard.put_short((int)p.m_slotsD2[i]);
                guard.put_int(p.m_counts19c[i]);
                guard.put_short((int)p.m_slotsA[i]);
            }
            guard.finalize(true);
            user->Send(guard);
        }
        else
        {
            guard.put_header(1, 0x12);
            guard.put_byte(0);
            guard.put_byte(p.m_4);
            guard.put_byte((char)p.m_8);
            guard.finalize(true);
            user->Send(guard);
        }
    }
}


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

        const int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_itemSpace);
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
// ORIG 0x081c57a0 T _ZN19DisPatcher_MoveItem11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_MoveItem/check_error.md 汇编+反编译 C 全文转写）：
// m_field14<0 → -1。toSpace(space)>5 时仅 ==0xe、<=5 时 ==4/5 或 ==3 走限制/锁定检查；
// ==4/5 或 0xe：CheckItemTradeLimitCountToMoveByUser 假 → 0x72；==3 且 fromSpace==7：
// GetInvenSlot(3,fromPos) 非空且 fromPos>0x8b → 需锁定检查；需检查者 CheckItemLock
// 命中 → 0xd5；否则 0。
int DisPatcher_MoveItem::check_error(CUser* user, MSG_BASE& msg)
{
    const MSG_MOVE_ITEMSPACE& req =
        *reinterpret_cast<const MSG_MOVE_ITEMSPACE*>(&msg);
    if ((int)req.m_field14 < 0)
    {
        return -1;
    }
    char needLock = 0;
    int space = (int)(char)req.m_toSpace;
    if (space > 5)
    {
        if (space != 0xe)
        {
            space = -1;          // 仅作跳转标记（ORIG jmp 0x109 路径）
        }
    }
    if (space == 0xe || (space >= 4 && space <= 5))
    {
        needLock = 1;
        bool movable = true;
        CheckItemTradeLimitCountToMoveByUser(
            *user, GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_fromSpace),
            (int)req.m_fromPos, movable);
        if (!movable)
        {
            return 0x72;
        }
    }
    else if (space == 3)
    {
        if ((unsigned char)req.m_fromSpace == 7)
        {
            Inven_Item item = ((CInventory*)user->getCurCharacInvenR())
                                  ->GetInvenSlot(3, (int)req.m_fromPos);
            if (item.m_field0 != 0 && req.m_fromPos > 0x8b)
            {
                needLock = 1;
            }
        }
    }
    if (needLock != 0)
    {
        if (user->CheckItemLock(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_fromSpace),
                (int)req.m_fromPos))
        {
            return 0xd5;
        }
    }
    return 0;
}
// ORIG 0x081c54e0 T _ZN19DisPatcher_MoveItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_MoveItem/read.md（汇编+反编译 C 全读）
// 串行 10 次 get_*；任一失败即 LineFunc(行号, pretty, 0, 0)。msgPtr 局部落 -0xc(%ebp)。
int DisPatcher_MoveItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_MOVE_ITEMSPACE& m = *reinterpret_cast<MSG_MOVE_ITEMSPACE*>(&msg);
    if (!packet.get_byte(m.m_fromSpace))                 // +0x0d
    {
        return LineFunc(0x1373,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(m.m_fromPos))                  // +0x0e
    {
        return LineFunc(0x1374,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_count))                      // +0x10
    {
        return LineFunc(0x1375,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_field14))                    // +0x14
    {
        return LineFunc(0x1376,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(m.m_toSpace))                   // +0x18
    {
        return LineFunc(0x1377,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(m.m_toPos))                    // +0x19
    {
        return LineFunc(0x1378,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_itemIdx))                    // +0x1b
    {
        return LineFunc(0x1379,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x1f 4 字节槽（m_pad1f 起点 int 视图；ORIG read 实证读取）
    if (!packet.get_int(*reinterpret_cast<int*>(m.m_pad1f)))
    {
        return LineFunc(0x137b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(m.m_expanda))                   // +0x23
    {
        return LineFunc(0x137e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(m.m_expandb))                   // +0x24
    {
        return LineFunc(0x137f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// ORIG 0x081c5b76 T _ZN19DisPatcher_MoveItem4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_MoveItem/send.md 汇编+反编译 C 全文转写）：
// result(m_4)!=0x7fffffff 时发包；失败分支 put_byte(0)/err/idx/spot 四字节；
// 成功分支 put_byte(1)/idx/count/itemIdx/spot/newCount 后 Send，并经
// ARAD::Singleton<AvatarRechargeServer>::Get()->SendDurability 上报耐久。
void DisPatcher_MoveItem::send(CUser* user, ParamBase& param)
{
    stMoveItemParam& p = static_cast<stMoveItemParam&>(param);
    PacketGuard guard;
    if (p.m_4 != 0x7fffffff)
    {
        guard.put_header(1, 0x13);
        if (p.m_4 == 0)
        {
            guard.put_byte(1);
            guard.put_byte((char)p.m_8);
            guard.put_short((int)p.m_a);
            guard.put_int(p.m_c);
            guard.put_byte((char)p.m_10);
            guard.put_short(p.m_14);
            guard.finalize(true);
            user->Send(guard);
            ARAD::Singleton<AvatarRechargeServer>::Get()->SendDurability(
                user, (char)p.m_8, (short)p.m_a, (char)p.m_10, p.m_14);
        }
        else
        {
            guard.put_byte(0);
            guard.put_byte(p.m_4);
            guard.put_byte((char)p.m_8);
            guard.put_byte((char)p.m_10);
            guard.finalize(true);
            user->Send(guard);
        }
    }
}


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
// ORIG 0x081c9d64 T _ZN19DisPatcher_SortItem11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_SortItem/check_error.md 逐块映射）：user 空 →
// LineFunc(0x1d56)；state<=2 → -1；否则 0。
int DisPatcher_SortItem::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return LineFunc(0x1d56,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() <= 2)
    {
        return 0xffffffff;
    }
    return 0;
}
// ORIG 0x081c9bfe T _ZN19DisPatcher_SortItem4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_SortItem/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x1d16, PRETTY, 0, 0)。
int DisPatcher_SortItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SORT_ITEM& m = *reinterpret_cast<MSG_SORT_ITEM*>(&msg);
    if (!packet.get_byte(m.m_invenType))                  // +0x0d
    {
        return LineFunc(0x1d16,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// ORIG 0x081c9dba T _ZN19DisPatcher_SortItem4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_SortItem/send.md 汇编+反编译 C 全文转写）：
// put_header(1,0x14)；result==0 → put_byte(1)/(char)invenType，否则 0/(char)result/
// (char)invenType；finalize+Send。result==0 时 send_itemspace((char)invenType)；
// result==0 且 invenType 1/0/2/7 分别 SendItemLockListAvatar/Inven/Cargo/Creature。
void DisPatcher_SortItem::send(CUser* user, ParamBase& param)
{
    stOnSortItemParam& p = static_cast<stOnSortItemParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x14);
    if (p.m_result == 0)
    {
        guard.put_byte(1);
        guard.put_byte((char)p.m_invenType);
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte((char)p.m_result);
        guard.put_byte((char)p.m_invenType);
    }
    guard.finalize(true);
    user->Send(guard);
    if (p.m_result == 0)
    {
        user->send_itemspace((int)(char)p.m_invenType);
    }
    if (p.m_result == 0)
    {
        if ((int)(char)p.m_invenType == 1)
        {
            user->getCurCharacInvenR()->SendItemLockListAvatar();
        }
        else if ((int)(char)p.m_invenType > 1)
        {
            if ((int)(char)p.m_invenType == 2)
            {
                user->getCurCharacCargoR()->SendItemLockList();
            }
            else if ((int)(char)p.m_invenType == 7)
            {
                user->getCurCharacInvenR()->SendItemLockListCreature();
            }
        }
        else if ((int)(char)p.m_invenType == 0)
        {
            user->getCurCharacInvenR()->SendItemLockListInven();
        }
    }
}


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
// ORIG 0x081be46a T _ZN18DisPatcher_BuyItem11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_BuyItem/check_error.md 汇编+反编译 C 全文转写）：
// checkValidShopItem(0x15,(ulong)0x0d) 失败 → OneADayShop 判定（是→0x11，否→
// addServerHackCnt 0x3c9+(-1)）；随后 CheckInTrade/getCurCharacR/isHackUser → 0x13、
// isRestricted(1,3) → 0xd1、state!=8 时 secu 0x22 非0返码、IsBusyPrivateStore → 0x3c、
// OneADayShop 时 GetRepeatEvent(0x6d)->IsEventing(0) 为假 → 0x11；否则 0。
int DisPatcher_BuyItem::check_error(CUser* user, MSG_BASE& msg)
{
    const MSG_BUY_ITEM& req = *reinterpret_cast<const MSG_BUY_ITEM*>(&msg);
    if (!G_CDataManager()->checkValidShopItem((unsigned int)req.m_deliveryType,
                                              (unsigned long)req.m_itemIdx))
    {
        if (G_CDataManager()->m_itemShop->IsOneADayShop(req.m_deliveryType) == 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x3c9, 1, 0, 0);
            return -1;
        }
        return 0x11;
    }
    if (user->CheckInTrade())
    {
        return 0x13;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    if (user->isHackUser())
    {
        return 0x13;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 3))
    {
        return 0xd1;
    }
    if (user->get_state() == 8 ||
        GlobalData::s_pSecuProtectionField->Check(
            user, static_cast<SECURITY_PROTCTION>(0x22)) == 0)
    {
        if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        {
            return 0x3c;
        }
        if (G_CDataManager()->m_itemShop->IsOneADayShop(req.m_deliveryType) != 0)
        {
            // ORIG vtable+0x34 槽直调 = IsEventing(CUser*)；对指针无空检。
            if (!GlobalData::s_event_manager->GetRepeatEvent(0x6d)->IsEventing(
                    (CUser*)0))
            {
                return 0x11;
            }
        }
        return 0;
    }
    return GlobalData::s_pSecuProtectionField->Check(
        user, static_cast<SECURITY_PROTCTION>(0x22));
}
int DisPatcher_BuyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BUY_ITEM& m = *reinterpret_cast<MSG_BUY_ITEM*>(&msg);
    if (!packet.get_int(m.m_itemIdx))                    // +0x0d
    {
        return LineFunc(0x188,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_buyCount))                   // +0x11
    {
        return LineFunc(0x18b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(m.m_deliveryType))               // +0x15
    {
        return LineFunc(0x18e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// ORIG 0x081be20a T _ZN18DisPatcher_BuyItem4sendEP5CUserR9ParamBase
// 报告：docs/class_func_reports/DisPatcher_BuyItem/send.md（汇编+反编译 C 全读）
void DisPatcher_BuyItem::send(CUser* user, ParamBase& param)
{
    BuyItemParam& out = static_cast<BuyItemParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x15);
    guard.put_byte(1);
    guard.put_int(user->getCurCharacInvenR()->get_money());
    guard.put_int(user->UseWinPoint(out.m_88, (eWPSubReason)0));
    guard.put_int(user->UseFinishPoint(out.m_8c));
    guard.put_int(user->UseCera(out.m_84));
    guard.put_short(out.m_a0);
    guard.put_int(out.m_item.m_addInfo);        // param+0x06（m_item+0x02）
    guard.put_int(out.m_item.m_addInfo2);       // param+0x0b（m_item+0x07）
    guard.put_short(out.m_item.m_fieldb);       // param+0x0f（m_item+0x0b）
    guard.put_byte((int)out.m_item.m_amp.getAbilityType() & 0xff);   // param+0x15
    guard.put_short((int)out.m_item.m_amp.getAbilityValue() & 0xffff);
    guard.put_packet(out.m_item);
    guard.put_int(out.m_pair.first);            // param+0x98
    const Inven_Item* ref =
        user->getCurCharacInvenR()->GetInvenRef(1, out.m_a4);
    if (ref == 0)
    {
        guard.put_int(0);
    }
    else
    {
        guard.put_int(ref->m_addInfo2);         // ref+0x07
    }
    guard.finalize(true);
    user->Send(guard);
}

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
                        __PRETTY_FUNCTION__,
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
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (G_CGameManager()->getBossTower(user->getBossTowerIndex()) == 0)
        {
            return LineFunc(0x1b8,
                            __PRETTY_FUNCTION__,
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
int DisPatcher_RentAvatar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int unused0[10];                    // -0xa8（ORIG rep stos 0xa 清零，后未使用）
    char rentEntries[0x3f];             // -0x188..-0x14a（逐 entry stride 5：
                                        //   byte type@+0xd+5i / u32 itemId@+0xf+5i）
    int itemPtrs[10];                   // -0xd0（find_item 结果）
    int cashPrices[10];                 // -0xf8（GetCashPrice 租价）
    int goldPrices[10];                 // -0x148（get_price 租价）
    int winPrices[10];                  // -0x120（GetMedalPrice 租价）
    int totalCash;                      // -0x24
    int totalGold;                      // -0x20
    int totalWin;                       // -0x1c
    int i;                              // -0x30（循环游标）
    int rentDays;                       // -0x2c
    int rentSeconds;                    // -0x28
    unsigned char count;                // -0x17b（rentEntries[0xd] 视图）
    char itemIdStr[0x14];               // -0x54
    char priceStr[0x24];                // -0x80

    memset(unused0, 0, sizeof(unused0));
    totalCash = 0;
    rentDays = 0;
    rentSeconds = 0;

    PacketGuard packetGuard;            // -0x40
    if (packet.get_byte(*(unsigned char*)(rentEntries + 0xd)) != 1)
        return LineFunc(0x71ee, __PRETTY_FUNCTION__, 0, 0);

    count = rentEntries[0xd];
    if (count == 0 || 9 < count)
        return LineFunc(0x71f0, __PRETTY_FUNCTION__, 0, 0);

    for (i = 0; i < (int)(unsigned int)count; ++i)
    {
        if (packet.get_byte(*(unsigned char*)(rentEntries + 0xe + i * 5)) != 1)
            return LineFunc(0x71f4, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(*(unsigned long*)(rentEntries + 0xf + i * 5)) != 1)
            return LineFunc(0x71f5, __PRETTY_FUNCTION__, 0, 0);
    }

    if (user->getCurCharacR() == 0)
        return LineFunc(0x71fa, __PRETTY_FUNCTION__, 0, 0);

    if (user->getCurCharacInvenR()->GetRemainCapacity(
            (INVEN_TYPE)2, Inven_Item::ITEM_TYPE_0) <
        (int)(unsigned int)count)
    {
        RentAvatarErrorProc(user, 4);
        return 0;
    }

    memset(itemPtrs, 0, sizeof(itemPtrs));
    memset(cashPrices, 0, sizeof(cashPrices));
    memset(goldPrices, 0, sizeof(goldPrices));
    memset(winPrices, 0, sizeof(winPrices));
    totalCash = 0;
    totalGold = 0;
    totalWin = 0;
    for (i = 0; i < (int)(unsigned int)count; ++i)
    {
        itemPtrs[i] = (int)(long)G_CDataManager()->find_item(
            *(int*)(rentEntries + 0xf + i * 5));
        if (itemPtrs[i] == 0)
        {
            RentAvatarErrorProc(user, 0x11);
            return 0;
        }
        cashPrices[i] = getAvatarRentPrice(
            ((CItem*)itemPtrs[i])->GetCashPrice(),
            (unsigned int)rentEntries[i * 5 + 1]);
        goldPrices[i] = getAvatarRentPrice(
            ((CItem*)itemPtrs[i])->get_price(),
            (unsigned int)rentEntries[i * 5 + 1]);
        winPrices[i] = getAvatarRentPrice(
            ((CItem*)itemPtrs[i])->GetMedalPrice(),
            (unsigned int)rentEntries[i * 5 + 1]);
        totalCash += cashPrices[i];
        totalGold += goldPrices[i];
        totalWin += winPrices[i];
    }

    if ((unsigned int)user->GetCera() < (unsigned int)totalCash ||
        user->getCurCharacInvenR()->get_money() < totalGold ||
        (unsigned int)user->GetWinPoint() < (unsigned int)totalWin)
    {
        RentAvatarErrorProc(user, 10);
        return 0;
    }

    for (i = 0; i < (int)(unsigned int)count; ++i)
    {
        if (rentEntries[i * 5 + 1] == 1)
            rentDays = 0x1e;
        else if (rentEntries[i * 5 + 1] == 2)
            rentDays = 0x16d;
        else if (rentEntries[i * 5 + 1] != 0)
        {
            RentAvatarErrorProc(user, 1);
            return 0;
        }
        else
            rentDays = 7;
        rentSeconds = rentDays * 0x15180;

        CStreamGuard streamGuard(
            GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp",
                                               0x7239),
            false);                     // -0x5c
        streamGuard.operator*()->operator<<(cashPrices[i]);
        streamGuard.operator*()->operator<<(winPrices[i]);
        streamGuard.operator*()->operator<<(goldPrices[i]);
        streamGuard.operator*()->operator<<(
            *(int*)(rentEntries + 0xf + i * 5));
        streamGuard.operator*()->operator<<(rentDays);
        streamGuard.operator*()->operator<<(rentSeconds);
        streamGuard.operator*()->operator<<((int)(unsigned int)count);
        streamGuard.operator*()->operator<<(i + 1);
        OS_API::snprintf(itemIdStr, 0x14, "%d",
                         *(int*)(rentEntries + 0xf + i * 5));
        OS_API::snprintf(priceStr, 0x24, "%d", cashPrices[i]);
        GlobalData::s_pIPGHelper->IPGOutput(
            user, "", priceStr, itemIdStr, (char const*)0, (char const*)0,
            streamGuard.Get());
    }

    return 0;
}
int DisPatcher_RentAvatar::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08232d7c：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
int DisPatcher_RentAvatar::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232d86 W _ZN21DisPatcher_RentAvatar7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ee59c)（租期/价格计算与 IPG 记录）

DisPatcher_ExtendAvatar::DisPatcher_ExtendAvatar() {}
DisPatcher_ExtendAvatar::~DisPatcher_ExtendAvatar() {}
int DisPatcher_ExtendAvatar::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)
{
    return 0; // ORIG 0x081eddec：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
int DisPatcher_ExtendAvatar::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08232c42：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
int DisPatcher_ExtendAvatar::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232c4c W _ZN23DisPatcher_ExtendAvatar7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现；ORIG dispatch_sig(0x081eddec) 同样为纯 ret（报告 docs/class_func_reports/DisPatcher_ExtendAvatar/dispatch_sig.md），该分发器整体为空壳

DisPatcher_SellItem::DisPatcher_SellItem() {}
DisPatcher_SellItem::~DisPatcher_SellItem() {}
int DisPatcher_SellItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char invenType;                     // -0x45
    short slot;                         // -0x48
    unsigned short count;               // -0x46
    unsigned long money;                // -0x64
    int sum;                            // -0x3c
    int unusedGuard;                    // -0x38（ORIG 死存储 movl $0x6）
    int sellResult;                     // -0x34
    int sellTimeRet;                    // -0x30
    unsigned int nowTick;               // GetCurTickCount 返回（局部）
    void* party;                        // -0x2c
    CMap* map;                          // -0x24
    Village* village;                   // -0x20
    Area* area;                         // -0x1c
    Inven_Item* invenItem;              // GetInvenItemPtr（寄存器传递）
    CItem* item;                        // GetItemPtr（寄存器传递）
    int rarity;                         // -0x40

    if (user->get_state() == 3 || user->get_state() == 8 ||
        user->get_state() == 5 || user->get_state() == 10 ||
        user->get_state() == 0xc)
        return 0;

    if (packet.get_byte(invenType) != 1)
        return LineFunc(0x7e15, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_short(slot) != 1)
        return LineFunc(0x7e16, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_short(count) != 1)
        return LineFunc(0x7e17, __PRETTY_FUNCTION__, 0, 0);

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 3))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0xd1);
        return 0;
    }

    // P48：GetInvenItemPtr / GetItemPtr 为两次独立调用（各自求值实参）
    invenItem = GlobalData::s_pSecuProtectionField->GetInvenItemPtr(
        user, invenType, slot);
    item = GlobalData::s_pSecuProtectionField->GetItemPtr(user, invenType, slot);
    rarity = GlobalData::s_pSecuProtectionField->CheckItemRarity(
        user, (SECURITY_PROTCTION)0x23, item, invenItem);
    if (rarity != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, (unsigned char)rarity);
        return 0;
    }

    if (user->CheckItemLockSlotSpace(invenType, slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0xd5);
        return 0;
    }

    if (user->get_state() == 5 &&
        user->getInDungeonItemSellCount(invenType, slot) == 7)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x139,
                                       1, 0, 0);
    }

    PacketGuard packetGuard;            // -0x60
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        packetGuard.clear();
        packetGuard.put_header(1, 0x18);
        packetGuard.put_byte(0);
        packetGuard.put_byte(0x3c);
        packetGuard.finalize(true);
        user->Send(packetGuard);
        return 0;
    }

    money = 0;
    if (packet.get_int(money) != 1)
        return LineFunc(0x7e71, __PRETTY_FUNCTION__, 0, 0);

    sum = 0;
    if ((int)(signed char)invenType + (int)slot + (unsigned short)count + sum !=
        (int)money)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x335,
                                       1, 0, 0);
    }

    unusedGuard = 6;                    // ORIG -0x38 死存储
    if (invenType == 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0x17);
        return 0;
    }
    if (invenType == 3 && slot <= 9)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0x17);
        return 0;
    }

    if (user->get_state() == 5)
    {
        party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            // CParty+0xb24 内嵌 CBattle_Field（局部视图 CPartyBattleFieldView）
            CBattle_Field* battleField =
                &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField;
            map = (CMap*)G_CDataManager()->find_map(battleField->get_cur_map());
            if (map != 0 && map->getAssignedNPCCount() == 0 &&
                !battleField->IsAssignNPCByPotion())
            {
                WongWork::CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
                hackAnalyzer->addServerHackCnt(user,
                                               (WongWork::ENUM_HACKTYPE)0x323,
                                               1, 0, 0);
            }
        }
    }

    if (user->get_state() == 3)
    {
        if (G_GameWorld()->check_valid_area((int)user->getCurCharacVill(),
                                            user->get_area(false)))
        {
            village = G_GameWorld()->getUserVillage(user);
            if (village != 0)
            {
                area = village->getArea(user->get_area(false));
                if (area != 0 && !area->IsExistItemShopNPC())
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0x324, 1, 0, 0);
                }
            }
        }
    }

    sellResult = G_Store()->user_sell_item(user, invenType, slot, count);
    if (sellResult != 0)
    {
        packetGuard.put_header(1, 0x18);
        packetGuard.put_byte(0);
        packetGuard.put_byte(sellResult);
        packetGuard.finalize(true);
        user->Send(packetGuard);
        if (sellResult == 0x16)
            user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)3, 0, 0);
    }

    sellTimeRet = 0;
    nowTick = GlobalData::s_systemTime_.getCurTickCount();
    sellTimeRet = user->SellItemTimeChecker(nowTick, 1000);
    if (sellTimeRet == 2)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x138,
                                       1, 0, 0);
    }
    else if (sellTimeRet == 4)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x321,
                                       1, 0, 0);
    }

    return 0;
}
int DisPatcher_SellItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08232f04：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
int DisPatcher_SellItem::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232f0e W _ZN19DisPatcher_SellItem7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f2d0c)（状态检查/Secu 保护/Store::user_sell_item）

DisPatcher_RepairEquip::DisPatcher_RepairEquip() {}
DisPatcher_RepairEquip::~DisPatcher_RepairEquip() {}
int DisPatcher_RepairEquip::check_error(CUser* user, MSG_BASE& /*msg*/)
{
    if (user == 0)
    {
        return 0xffffffff;
    }
    int state = user->get_state();
    if ((unsigned int)state > 0xc || ((1 << state) & 0x15e8) == 0)
    {
        return 0x13;
    }
    return 0;
}
int DisPatcher_RepairEquip::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_REPAIR_EQUIP& req = *reinterpret_cast<MSG_REPAIR_EQUIP*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_0d)))
    {
        return LineFunc(0x14e6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&req.m_0e)))
    {
        return LineFunc(0x14e7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_10))
    {
        return LineFunc(0x14e9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// ORIG 0x081c61ea T _ZN22DisPatcher_RepairEquip4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_RepairEquip/send.md 汇编+反编译 C 全文转写）：
// put_header(1,0x19)；errNo==0 → put_byte(1)/getCurCharacInvenR()->get_money()
// put_int/(char)m_8/(ushort)m_a/(ushort)m_c；否则 0/errNo；finalize(true)+Send。
void DisPatcher_RepairEquip::send(CUser* user, ParamBase& param)
{
    stRepairEquipParam& p = static_cast<stRepairEquipParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x19);
    if (p.m_errNo == 0)
    {
        guard.put_byte(1);
        guard.put_int(user->getCurCharacInvenR()->get_money());
        guard.put_byte((char)p.m_8);
        guard.put_short((unsigned int)p.m_a);
        guard.put_short((unsigned int)p.m_c);
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(p.m_errNo);
    }
    guard.finalize(true);
    user->Send(guard);
}
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
                        __PRETTY_FUNCTION__,
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
// ORIG 0x081f6232 T _ZN24DisPatcher_SetTradeState12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetTradeState/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state==3 且 get_byte(state) 成功 → GetTradeSpace(idx) 池取；空 → SendCmdErrorPacket
// (0x1a,1)；is_empty → logFormat(0x82f1) + PutTradeSpace；check_trade_possibility 假 →
// cancel_trade + PutTradeSpace；真 → change_trade_state(user, (char)state)；返回 0。
int DisPatcher_SetTradeState::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3)
    {
        return 0;
    }
    char tradeState;
    if (!packet.get_byte(tradeState))
    {
        return LineFunc(0x82e6, __PRETTY_FUNCTION__, 0, 0);
    }
    CTradeSpace* trade = G_CGameManager()->GetTradeSpace(user->GetTradeSpace());
    if (trade == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 1);
        return 0;
    }
    if (trade->is_empty() != 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__, 0x82f1,
                              "Trade Space[%d] is Empty User[%d]",
                              trade->get_index(),
                              user->get_unique_id() & 0xffff);
        G_CGameManager()->PutTradeSpace(trade);
        return 0;
    }
    if (trade->check_trade_possibility() != 1)
    {
        trade->cancel_trade();
        G_CGameManager()->PutTradeSpace(trade);
        return 0;
    }
    trade->change_trade_state(user, (int)(char)tradeState);
    return 0;
}
int DisPatcher_SetTradeState::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08233150：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}

// ORIG 0x0823315a W _ZN24DisPatcher_SetTradeState7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部交易状态逻辑（get_state==3 → GetTradeSpace → is_empty/check_trade_possibility →
// change_trade_state / cancel_trade+PutTradeSpace）内嵌于
// dispatch_sig(0x081f6232，报告 docs/class_func_reports/DisPatcher_SetTradeState/dispatch_sig.md)。
int DisPatcher_SetTradeState::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_Compound::DisPatcher_Compound() {}
DisPatcher_Compound::~DisPatcher_Compound() {}
int DisPatcher_Compound::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(user, static_cast<RestrictCategory::Enum>(1), 9) != false)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1b), 0xd1);
        return 0;
    }
    unsigned long local_17;
    if (packet.get_int(local_17) != true)
    {
        return LineFunc(0x6c3a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned char local_13;
    if (packet.get_byte(local_13) != true)
    {
        return LineFunc(0x6c3b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    unsigned short local_12;
    if (packet.get_short(local_12) != true)
    {
        return LineFunc(0x6c3d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (local_12 == 0)
    {
        return LineFunc(0x6c40,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (local_13 != '\x01' &&
        user->CheckItemLock(1, static_cast<int>(local_17)) != false)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1b), 0xd5);
        return 0;
    }
    int local_10 = user->compound_item(static_cast<int>(local_17), local_13 == '\x01', local_12, 0x1b);
    if (local_10 < 1)
    {
        if (user != 0 && local_10 == 0)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, static_cast<APSystem::_ActionGroupIndex>(6), 0,
                static_cast<ENUM_PACKETCLASS>(0));
        }
        return 0;
    }
    PacketGuard local_30;
    local_30.put_header(1, 0x1b);
    local_30.put_byte(0);
    local_30.put_byte(local_10);
    local_30.finalize(true);
    user->Send(local_30);
    return 0;
}
int DisPatcher_Compound::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08232a92：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
int DisPatcher_Compound::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232a9c W _ZN19DisPatcher_Compound7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081eaa5c)（compound_item 与结果回包）

DisPatcher_DisJointItem::DisPatcher_DisJointItem() {}
DisPatcher_DisJointItem::~DisPatcher_DisJointItem() {}
// ORIG 0x081f9fb8 T _ZN23DisPatcher_DisJointItem12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_DisJointItem/dispatch_sig.md 汇编+反编译 C 全文转写）：
// user 空或 CheckInTrade() 真 → return 0；依次 get_short(slot)/get_byte(type)/
// get_short(target,0xffff 初始)，失败分别 LineFunc 0x88d0/0x88d2/0x88d6；
// 全部成功 → _disjoint(user,(int)(short)slot,(int)(char)type,0x1c,0,(ushort)target)。
int DisPatcher_DisJointItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->CheckInTrade())
    {
        return 0;
    }
    short slot;
    if (!packet.get_short(slot))
    {
        return LineFunc(0x88d0, __PRETTY_FUNCTION__, 0, 0);
    }
    char type = 0;
    if (!packet.get_byte(type))
    {
        return LineFunc(0x88d2, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned short target = 0xffff;
    if (!packet.get_short(target))
    {
        return LineFunc(0x88d6, __PRETTY_FUNCTION__, 0, 0);
    }
    DisPatcher_DisJointItem::_disjoint(user, (int)slot, (int)(char)type,
                                       (ENUM_CMDPACKET)0x1c, (CUser*)0,
                                       (unsigned short)target);
    return 0;
}
int DisPatcher_DisJointItem::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x0823339c：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
int DisPatcher_DisJointItem::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082333a6 W _ZN23DisPatcher_DisJointItem7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f9fb8)->_disjoint(0x081f92ca)

// ---------------------------------------------------------------------------
// LinkF1 批次（2026-08-28）：DisPatcher_DisJointItem::_disjoint —— ORIG T
// _ZN23DisPatcher_DisJointItem10_disjointEP5CUserii14ENUM_CMDPACKETS1_t @0x081f92ca
// （报告 docs/class_func_reports/DisPatcher_DisJointItem/_disjoint.md 全文转写，
// 草稿 /tmp/df_batches/linkf1/_disjoint.body.cpp + notes.md）。
// 依赖 _check_disjointable_user_state/_check_disjointable_item_state/
// _check_disjoint_need_slot：真实 C++ 名定义在 ExpertJobLink4.cpp（DispDebugJoint4.cpp
// 中的 _Z30 前缀名是带 asm 标签的桥接，勿混淆）。
// ---------------------------------------------------------------------------
bool _check_disjointable_user_state(CUser* user, ENUM_CMDPACKET cmd, char type, short slot);
void _check_disjointable_item_state(CUser* user, int slot, int invenType,
                                    Inven_Item& invenItem, CItem** item,
                                    CUser* targetUser, unsigned short region);
void _check_disjoint_need_slot(CUser* user, CItem* item, Inven_Item& invenItem,
                               expert_job::CDisjointer* disjointer);

int DisPatcher_DisJointItem::_disjoint(CUser* user, int slot, int type,
                                       ENUM_CMDPACKET cmd, CUser* targetUser,
                                       unsigned short region)
{
    // 081f9301 _check_disjointable_user_state(user, cmd, (char)type, (short)slot)
    if (!_check_disjointable_user_state(user, cmd,
                                        static_cast<char>(type),
                                        static_cast<short>(slot)))
    {
        return 0;
    }

    PacketGuard packet;                              // local_74（try 块外构造）
    try
    {
        // +0x337 put_header(1, cmd)
        packet.put_header(1, cmd);

        // +0x042 GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)type)（W 0x080f7845，定义 CTitleBook.cpp）
        const int invenType =
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)static_cast<ENUM_ITEMSPACE>(type));
        expert_job::CExpertJob* expertJob = 0;       // local_44
        if (targetUser != 0)
        {
            expertJob = targetUser->GetCurCharacExpertJob();   // 0x0822f8d4
            if (expertJob == 0)
            {
                LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                      __PRETTY_FUNCTION__, 0x87b7,
                                      "expert job null point");
                throw (ENUM_ERROR)0x13;
            }
            if (expertJob->GetType() != 3)            // W 0x08234796，非虚
            {
                LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                      __PRETTY_FUNCTION__, 0x87bd,
                                      "expert job permission error");
                throw (ENUM_ERROR)0x13;
            }
            expertJob->OnStartMission(targetUser, user);  // vptr slot0 虚调用
        }

        // +0x18b CUser::CheckItemLock(int,int) const
        if (user->CheckItemLock(invenType, slot))
        {
            throw (ENUM_ERROR)0xd5;
        }

        Inven_Item invenItem;                          // local_fd
        CItem* item = 0;                               // local_78
        if (region != 0xffff)
        {
            GameWorld* gw = G_GameWorld();
            if (gw->IsPVPChannel())
            {
                throw (ENUM_ERROR)0x60;
            }
        }
        // +0x263 _check_disjointable_item_state（C++ 名定义于 ExpertJobLink4.cpp）
        _check_disjointable_item_state(user, slot, invenType, invenItem, &item,
                                       targetUser, region);
        // +0x286 _check_disjoint_need_slot（C++ 名定义于 ExpertJobLink4.cpp）
        _check_disjoint_need_slot(
            user, item, invenItem,
            reinterpret_cast<expert_job::CDisjointer*>(expertJob));

        DisJointResult result;                         // local_c0，ctor 清零 10×int
        const int srcItemIdx = invenItem.m_addInfo;    // local_fb = item+0x02
        int ret = 0;                                   // local_40
        if (targetUser == 0)
        {
            DisJoint* dj = G_CDataManager()->m_disJoint;   // +0x1c 已具名
            ret = dj->GetResult(srcItemIdx, result);
        }
        else
        {
            ret = static_cast<expert_job::CDisjointer*>(expertJob)
                      ->get_disjoint_result(item, &invenItem, result, user);
        }
        if (0 < ret)
        {
            throw (ENUM_ERROR)ret;
        }
        if (region != 0xffff)
        {
            CInventory* invenW = user->getCurCharacInvenW();
            if (invenW->use_item((int)region, 1, 1) != 0)
            {
                throw (ENUM_ERROR)0x16;
            }
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 (int)region);
        }

        CItem* foundItems[6];                          // local_90[0..5]
        foundItems[0] = 0;                             // local_90[0]=0（+0x41a）
        int sellPrice0 = 0;                            // 源物品卖价出参（GetSellItemPrice 第 5 参）
        Inven_Item outItems[5];                        // local_26b..，步长 0x3d
        for (int i = 0; i < 5; ++i)
        {
            if (result.m_f[i + 1] == 0)                // -0xb8+(i+1)*4 = result+4*(i+1)
            {
                continue;
            }
            foundItems[i + 1] = G_CDataManager()->find_item(result.m_f[i + 1]);
            if (foundItems[i + 1] == 0)
            {
                continue;
            }
            outItems[i].m_addInfo = result.m_f[i + 1];   // +0x2 ← m_f[i+1]
            outItems[i].set_add_info(result.m_f[i]);     // +0x7 ← m_f[i]
            foundItems[i + 1]->make_item(outItems[i]);   // vptr+0x8 槽
            const int slotResult =
                ((const CInventory*)user->getCurCharacInvenR())
                    ->tryInsertItemIntoInventory(outItems[i]);
            if (slotResult == -1)
            {
                throw (ENUM_ERROR)4;
            }
        }
        // 删除源物品（INVEN_TYPE, slot, 1, eItemDelReason 9, 1）
        const bool deleted = user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)invenType, slot, 1, (eItemDelReason)9, true);
        if (!deleted)
        {
            throw (ENUM_ERROR)ret;
        }
        // 源物品卖价统计（sellPrice 用 foundItems[0] 槽位承接，ORIG local_90[0]）
        foundItems[0] = 0;
        G_Store()->GetSellItemPrice(invenItem, item, (short)invenItem.m_addInfo2,
                                    false, sellPrice0);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0x13, user, (unsigned int)sellPrice0);
        if (targetUser == 0)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, (APSystem::_ActionGroupIndex)7, 0, (ENUM_PACKETCLASS)0);
        }
        else if (user != 0)
        {
            if (user->get_charac_no(-1) == targetUser->get_charac_no(-1))
            {
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    user, (APSystem::_ActionGroupIndex)0x20, 0,
                    (ENUM_PACKETCLASS)0);
            }
            else
            {
                APSystem::CUserProc::ClearActionAndSendtoUser(
                    targetUser, (APSystem::_ActionGroupIndex)0x2d, 0,
                    (ENUM_PACKETCLASS)0);
            }
        }
        Inven_Item unusedPacketTmp;  // ORIG +0x765 构造 -0x136 处 Inven_Item，后续未使用
        packet.put_byte(1);
        packet.put_short(slot);
        packet.put_byte(type);
        int countIndex = packet.get_index();           // local_94
        packet.put_byte(0);
        int outCount = 0;                              // local_34
        for (int i = 0; i < 5; ++i)
        {
            if (foundItems[i + 1] == 0)
            {
                continue;
            }
            if (foundItems[i + 1]->is_stackable() &&
                999 < outItems[i].get_add_info())
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x8867, 5);
                trace("DisPatcher_DisJointItem::dispatch_sig, out_item , ITEM #%d, %d",
                      (int)outItems[i].m_addInfo, (int)outItems[i].get_add_info());
            }
            const int newSlot =
                user->getCurCharacInvenW()->insertItemIntoInventory(
                    outItems[i], (eItemAddReason)10, true, false);
            if (newSlot >= 0)
            {
                const Inven_Item* ref =
                    user->getCurCharacInvenR()->GetInvenRef(1, newSlot);
                if (ref != 0 && ref->m_addInfo != (int)outItems[i].m_addInfo)
                {
                    cMyTrace trace2(__PRETTY_FUNCTION__, 0x888a, 0);
                    trace2("ENUM_CMDPACKET_DISJOINT_ITEM error item_id mismatch %d, %d",
                           (int)ref->m_addInfo, (int)outItems[i].m_addInfo);
                }
                packet.put_short(newSlot);
                packet.put_int((int)outItems[i].m_addInfo);
                packet.put_int((int)outItems[i].m_addInfo2);
                if (targetUser == 0)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        (int)outItems[i].m_addInfo, (int)outItems[i].m_addInfo2,
                        user, (CUBE_STATISTIC_FIELD)0);
                }
                else if (targetUser->GetCurCharacExpertJob() != 0)
                {
                    GetInstanceCubeStatistic()->collectCubeStatistics(
                        (int)outItems[i].m_addInfo, (int)outItems[i].m_addInfo2,
                        user, (CUBE_STATISTIC_FIELD)2);
                }
                int itemPrice = 0;
                G_Store()->GetSellItemPrice(outItems[i], foundItems[i + 1],
                                            (short)outItems[i].m_addInfo2,
                                            false, itemPrice);
                GetInstanceValueStatistic()->AddValueStatistic(
                    (VALUE_STATISTIC_FIELD)8, user, (unsigned int)itemPrice);
                ++outCount;
            }
        }
        packet.put_byte(countIndex, outCount);
        if (targetUser != 0)
        {
            static_cast<expert_job::CDisjointer*>(expertJob)
                ->complete_disjoint(user, packet);
        }
        packet.finalize(true);
        user->Send(packet);
        HistoryLog::WriteDisJoint(
            reinterpret_cast<FILE*>(user->m_field796f8), invenItem.m_addInfo);
    }
    catch (ENUM_ERROR e)
    {
        packet.put_byte(0);
        packet.put_byte((int)e);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}


DisPatcher_Lottery::DisPatcher_Lottery() {}
DisPatcher_Lottery::~DisPatcher_Lottery() {}
// ORIG 0x081f8380 T _ZN18DisPatcher_Lottery12dispatch_sigEP5CUserR9PacketBuf
// 报告：docs/class_func_reports/DisPatcher_Lottery/dispatch_sig.md（汇编+反编译 C 全读）
int DisPatcher_Lottery::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3)
    {
        return 0;
    }
    if (user->CheckInTrade())
    {
        PacketGuard guard;
        guard.put_header(1, 0x1d);
        guard.put_byte(0);
        guard.put_byte(0xda);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    short idx;
    if (!packet.get_short(idx))
    {
        return LineFunc(0x861c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int result = user->lottery_item((int)idx);
    if (result < 1)
    {
        if (user->getLastLotteryTime() != 0)
        {
            int last = user->getLastLotteryTime();
            unsigned int diff = GlobalData::s_systemTime_.getCurSec() - last;
            if (diff <= 2)
            {
                WongWork::CHackAnalyzer* hack = user->getHackAnalyzer();
                hack->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x334, 1, 0, 0);
            }
        }
        user->setLastLotteryTime(GlobalData::s_systemTime_.getCurSec());
        return 0;
    }
    PacketGuard guard;
    guard.put_header(1, 0x1d);
    guard.put_byte(0);
    guard.put_byte(result);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
int DisPatcher_Lottery::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0; // ORIG 0x08233276：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}

// ORIG 0x08233280 W _ZN18DisPatcher_Lottery7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部抽奖逻辑（CheckInTrade 拒绝包 0x1d/0xda → lottery_item → 应答包 0x1d →
// 失败时限 3 秒内 addServerHackCnt(0x334) → setLastLotteryTime）内嵌于
// dispatch_sig(0x081f8380，报告 docs/class_func_reports/DisPatcher_Lottery/dispatch_sig.md)。
int DisPatcher_Lottery::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_ChangeSkill::DisPatcher_ChangeSkill() {}
DisPatcher_ChangeSkill::~DisPatcher_ChangeSkill() {}
// ORIG 0x081c0196 T _ZN22DisPatcher_ChangeSkill11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_ChangeSkill/check_error.md 逐块映射）：技能树索引
// ∈{-1,0} 时 msg+0xd==1 → 0x13；其它索引时 msg+0xd!=1 → 0x13；否则 0。
int DisPatcher_ChangeSkill::check_error(CUser* user, MSG_BASE& msg)
{
    const MSG_CHANGE_SKILLSLOT& req =
        *reinterpret_cast<const MSG_CHANGE_SKILLSLOT*>(&msg);
    const int treeIdx = user->GetCurCharacSkillTreeIndex();
    const unsigned char treeKind = req.m_head[0xd];       // +0x0d（m_head 末字节）
    if (treeIdx == -1 || treeIdx == 0)
    {
        if (treeKind == 1)
        {
            return 0x13;
        }
    }
    else if (treeKind != 1)
    {
        return 0x13;
    }
    return 0;
}
int DisPatcher_ChangeSkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CHANGE_SKILLSLOT* req = reinterpret_cast<MSG_CHANGE_SKILLSLOT*>(&msg);
    if (!packet.get_byte(req->m_head[0xd]))               // +0x0d（m_head 末字节）
    {
        return LineFunc(0x6f4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req->m_curSlot)))
    {
        return LineFunc(0x6f6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req->m_newSlot)))
    {
        return LineFunc(0x6f8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_ChangeSkill::send(CUser* user, ParamBase& param)
{
    stOnChangeSkillParam& out = static_cast<stOnChangeSkillParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x1e);
    guard.put_byte(1);
    guard.put_byte(user->GetCurCharacSkillTreeIndex());
    guard.put_byte(static_cast<char>(out.m_curSlot));
    guard.put_byte(static_cast<char>(out.m_newSlot));
    guard.finalize(true);
    user->Send(guard);
}

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
// ORIG 0x081bf8be T _ZN19DisPatcher_BuySkill11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_BuySkill/check_error.md 逐块映射）：user 空 → -1；
// 遍历 msg 技能项（count@+0xe signed char，entries@+0xf stride 3），
// 技能 id ∈ (0xc7,0xd0) 且 ServiceRestrictManager::isRestricted(user,1,0x1e) → 0xd1。
int DisPatcher_BuySkill::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return 0xffffffff;
    }
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&msg);
    for (int i = 0; i < (int)(signed char)raw[0xe]; ++i)
    {
        const unsigned char skillId = raw[0xf + i * 3];
        if (skillId > 0xc7 && skillId < 0xd1)
        {
            if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                    user, (RestrictCategory::Enum)1, 0x1e))
            {
                return 0xd1;
            }
        }
    }
    return 0;
}
int DisPatcher_BuySkill::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (user->get_state() != 3)
    {
        return LineFunc(0x109,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// ORIG 0x081bed4a T _ZN19DisPatcher_BuySkill4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_BuySkill/read.md（汇编+反编译 C 全读）
int DisPatcher_BuySkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BUY_SKILL& m = *reinterpret_cast<MSG_BUY_SKILL*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_treeKind)))  // +0x0d（signed char→char& 适配）
    {
        return LineFunc(0x37b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(m.m_count))                          // +0x0e
    {
        return LineFunc(0x37e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!(m.m_count > 0))
    {
        return LineFunc(0x381,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!(m.m_count < 0x47))
    {
        return LineFunc(0x383,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < (char)m.m_count; ++i)
    {
        if (!packet.get_byte(m.m_entries[i].m_skillIdx))      // +0x0f+3i
        {
            return LineFunc(0x388,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(m.m_entries[i].m_refundFlag))    // +0x11+3i
        {
            return LineFunc(0x38a,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (m.m_entries[i].m_refundFlag != 0 && m.m_entries[i].m_refundFlag != 1)
        {
            return LineFunc(0x38d,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(m.m_entries[i].m_level))         // +0x10+3i
        {
            return LineFunc(0x38f,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (m.m_entries[i].m_level < 1)
        {
            m.m_entries[i].m_level = 1;
        }
    }
    return 0;
}
void DisPatcher_BuySkill::send(CUser* user, ParamBase& param)
{
    stOnBuySkillParam* local_18 = &static_cast<stOnBuySkillParam&>(param);
    PacketGuard local_24;
    local_24.put_header(1, 0x1f);
    if (local_18->m_350 < 0)
    {
        local_24.put_byte(0);
        switch (local_18->m_350)
        {
        case -6:
            local_24.put_byte(0x5c);
            break;
        case -5:
            local_24.put_byte(0x5b);
            break;
        case -4:
            local_24.put_byte(1);
            break;
        case -3:
            local_24.put_byte(0x12);
            break;
        case -2:
            local_24.put_byte(0x16);
            break;
        case -1:
            local_24.put_byte(0x15);
            break;
        }
        local_24.put_byte(static_cast<int>(local_18->m_354));
    }
    else
    {
        local_24.put_byte(1);
        char cVar2 = static_cast<char>(user->GetCurCharacSkillTreeIndex());
        local_24.put_byte(static_cast<int>(cVar2));
        int uVar3 = user->GetCurCharacSkillTreeIndex();
        SkillSlot* pSVar4 = reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()));
        int iVar5 = pSVar4->get_remain_sp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(uVar3));
        local_24.put_short(iVar5);
        if (user->GetCurCharacSkillTreeIndex() == -1 ||
            user->GetCurCharacSkillTreeIndex() == 0)
        {
            pSVar4 = reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()));
            iVar5 = pSVar4->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(2));
            local_24.put_short(iVar5);
        }
        else
        {
            pSVar4 = reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()));
            iVar5 = pSVar4->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(3));
            local_24.put_short(iVar5);
        }
        local_24.put_byte(static_cast<int>(static_cast<char>(local_18->m_355)));
        for (int local_14 = 0; local_14 < static_cast<int>(static_cast<char>(local_18->m_355));
             local_14 = local_14 + 1)
        {
            buySkillInfo& entry = local_18->m_infos[local_14];
            if (static_cast<unsigned char>(entry.m_0) < 200 ||
                0xd0 < static_cast<unsigned char>(entry.m_0))
            {
                local_24.put_byte(static_cast<int>(static_cast<char>(entry.m_4)));
                local_24.put_byte(static_cast<unsigned int>(static_cast<unsigned char>(entry.m_0)));
                local_24.put_byte(entry.m_8);
            }
            else
            {
                local_24.put_byte(0xff);
                local_24.put_byte(static_cast<unsigned int>(static_cast<unsigned char>(entry.m_0)));
                local_24.put_byte(entry.m_8);
            }
            STSkillCommandData local_30;
            bool cVar2b = user->findSkillCommandData(static_cast<int>(static_cast<unsigned char>(entry.m_0)),
                                                     local_30, true);
            if (cVar2b == false)
            {
                local_24.put_byte(0);
            }
            else
            {
                local_24.put_byte(1);
                char local_2e = static_cast<char>(local_30.m_count);
                local_24.put_byte(static_cast<int>(local_2e));
                char acStack_2d[5];
                acStack_2d[0] = static_cast<char>(local_30.m_key0);
                acStack_2d[1] = static_cast<char>(local_30.m_key1);
                acStack_2d[2] = static_cast<char>(local_30.m_key2);
                acStack_2d[3] = static_cast<char>(local_30.m_key3);
                acStack_2d[4] = static_cast<char>(local_30.m_key4);
                for (int local_10 = 0; local_10 < static_cast<int>(local_2e);
                     local_10 = local_10 + 1)
                {
                    local_24.put_byte(static_cast<int>(acStack_2d[local_10]));
                }
            }
        }
    }
    local_24.finalize(true);
    user->Send(local_24);
}

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
                        __PRETTY_FUNCTION__,
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
// ORIG 0x081ede34 T _ZN22DisPatcher_IncreStatus12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_IncreStatus/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=2 → 0；get_short 失败 → LineFunc 0x70fc；GetItemPtr(user,0,slot)==0 →
// LineFunc 0x7101；CheckItem 0x2a 非0 → SendCmdErrorPacket(0x20,err)+0；
// slot<0x99 → increase_status(slot)，否则 UseQuestItem(slot)+cMyTrace(0x710f,5)。
int DisPatcher_IncreStatus::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    short val;
    if (!packet.get_short(val))
    {
        return LineFunc(0x70fc, __PRETTY_FUNCTION__, 0, 0);
    }
    CItem* pItem = GlobalData::s_pSecuProtectionField->GetItemPtr(
        user, (char)0, (short)val);
    if (pItem == 0)
    {
        return LineFunc(0x7101, __PRETTY_FUNCTION__, 0, 0);
    }
    int secu = GlobalData::s_pSecuProtectionField->CheckItem(
        user, static_cast<SECURITY_PROTCTION>(0x2a), pItem);
    if (secu != 0)
    {
        PacketGuard guard;
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x20, (unsigned char)secu, guard);
        return 0;
    }
    if (val < 0x99)
    {
        user->increase_status((short)val);
    }
    else
    {
        user->UseQuestItem((int)val);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x710f, 5);
        trace("__ USE QUEST ITEM __ %d", (int)val);
    }
    return 0;
}
int DisPatcher_IncreStatus::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08232cb8: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}

// ORIG 0x08232cc2 W _ZN22DisPatcher_IncreStatus7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部加点逻辑（get_short 属性号 → SecuProtectionField GetItemPtr/CheckItem 守卫 →
// <0x99 increase_status / >=0x99 UseQuestItem+cMyTrace）内嵌于
// dispatch_sig(0x081ede34，报告 docs/class_func_reports/DisPatcher_IncreStatus/dispatch_sig.md)。
int DisPatcher_IncreStatus::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_QuestAction::DisPatcher_QuestAction() {}
DisPatcher_QuestAction::~DisPatcher_QuestAction() {}
int DisPatcher_QuestAction::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    if (G_GameWorld()->IsPvPSkilTreeChannel() != false)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(packet.at(1)), 1);
        return 0;
    }
    short cmd = 0;
    if (packet.get_short(cmd) == false || (packet.at(1) & 0xff) == static_cast<unsigned short>(cmd))
    {
        unsigned char bVar3 = packet.at(1);
        if (bVar3 > 0x20)
        {
            if (bVar3 < 0x23)
            {
                short local_28;
                if (packet.get_short(local_28) != true)
                {
                    LineFunc(0x7149,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                int local_24 = reinterpret_cast<int>(G_CDataManager()->find_quest(static_cast<int>(local_28)));
                if (local_24 == 0)
                {
                    return 0;
                }
                char cVar1 = static_cast<char>(packet.at(1));
                if (cVar1 == '!' &&
                    GlobalData::s_timeGate_->setBlindTimeGateStateQuest(static_cast<int>(local_28)) != false)
                {
                    user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x21), 8);
                }
                user->quest_action(static_cast<int>(packet.at(1) & 0xff), static_cast<int>(local_28), 0, 0);
            }
            else if (bVar3 == 0x23)
            {
                if (user->get_state() == 5)
                {
                    CParty* local_20 = static_cast<CParty*>(user->GetParty());
                    if (local_20 == 0)
                    {
                        short sVar5 = user->GetPartyIndex();
                        unsigned int uVar9 = user->get_acc_id();
                        char* uVar8 = NumberToString(uVar9, 0);
                        LogManager::logFormat
                                  (1, "PacketDispatcher_Impl_1.cpp",
                                   __PRETTY_FUNCTION__,
                                   0x71af, "<IN_DUNGEON_NULL_PARTY> error m_id(%s), PartyIndex(%d)", uVar8,
                                   static_cast<int>(sVar5));
                        return 0;
                    }
                    if (*(int*)(reinterpret_cast<char*>(local_20) + 0xcd8) == 1)
                    {
                        return 0;
                    }
                }
                short local_4c;
                if (packet.get_short(local_4c) != true)
                {
                    LineFunc(0x71b9,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                char local_4a;
                if (packet.get_byte(local_4a) != true)
                {
                    LineFunc(0x71ba,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                char local_49[14];
                local_49[0] = '\0';
                if ((((local_4a == '\x10') || (local_4a == ' ')) || (local_4a == '@')) &&
                    (packet.get_byte(local_49[0]) != true))
                {
                    LineFunc(0x71c4,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                user->quest_action(static_cast<int>(packet.at(1) & 0xff), static_cast<int>(local_4c),
                                   static_cast<int>(local_4a), static_cast<int>(local_49[0]));
            }
            else if (bVar3 == 0x24)
            {
                short local_3b;
                if (packet.get_short(local_3b) != true)
                {
                    LineFunc(0x719a,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                short local_39;
                if (packet.get_short(local_39) != true)
                {
                    LineFunc(0x719b,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                short local_37[7];
                if (packet.get_short(local_37[0]) != true)
                {
                    LineFunc(0x719c,
                             __PRETTY_FUNCTION__,
                             0, 0);
                    return 0;
                }
                if (0 < local_37[0])
                {
                    user->quest_action(static_cast<int>(packet.at(1) & 0xff), static_cast<int>(local_3b),
                                       static_cast<int>(local_39), static_cast<int>(local_37[0]));
                    user->setGmQuestFlag(false);
                }
            }
        }
    }
    else
    {
        // 0x81ee0b4..0x81ee130：get_short 成功但 cmd 与 at(1) 不一致 → 伪造命令，hack 记数 + 错误包
        WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
        analyzer->addServerHackCnt(user, static_cast<WongWork::ENUM_HACKTYPE>(0x333), 1, 0, 0);
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(packet.at(1)), 1);
    }
    return 0;
}
int DisPatcher_QuestAction::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08232d1a: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}

// ORIG 0x08232d24 W _ZN22DisPatcher_QuestAction7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部任务动作逻辑（PvPSkilTree 频道拒绝/at(1) 命令码 0x21..0x24 分派 → find_quest/
// setBlindTimeGateStateQuest/quest_action 四参/GM quest flag）内嵌于
// dispatch_sig(0x081ee010，报告 docs/class_func_reports/DisPatcher_QuestAction/dispatch_sig.md)。
int DisPatcher_QuestAction::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_SetPos::DisPatcher_SetPos() {}
DisPatcher_SetPos::~DisPatcher_SetPos() {}
int DisPatcher_SetPos::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c9994：get_state(user)==3 → 0，否则 0x13（setne 形式）。
    if (user->get_state() != 3)
        return 0x13;
    return 0;
}
int DisPatcher_SetPos::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_USER_SET_POS& req = *reinterpret_cast<MSG_USER_SET_POS*>(&msg);
    if (!packet.get_short(req.m_0d))
    {
        return LineFunc(0x1ca2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_0f))
    {
        return LineFunc(0x1ca4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_11)))
    {
        return LineFunc(0x1ca6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_12))
    {
        return LineFunc(0x1ca8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
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
                        __PRETTY_FUNCTION__,
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
// ORIG 0x081c93e8 T _ZN18DisPatcher_SetArea11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_SetArea/check_error.md 汇编+反编译 C 全文转写）：
// state!=3 → 0x7fffffff；IsBusyDisjointStore → param.m_8=req.m_0d, 0xbf；
// IsBusyPrivateStore → param.m_8=req.m_0d, 0x3c；IsCurCharacGhost → param.m_8=req.m_0d,
// 0x13；IsForbiddenMove(name) 且剩余时间!=0 → param.m_9=req.m_0e, 0x44；时间==0 则
// EnableMove(name)；否则 0。
int DisPatcher_SetArea::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const MSG_SET_USER_AREA& req =
        *reinterpret_cast<const MSG_SET_USER_AREA*>(&msg);
    stSetAreaParam& out = static_cast<stSetAreaParam&>(param);
    if (user->get_state() != 3)
    {
        return 0x7fffffff;
    }
    if (user->IsBusyDisjointStore())
    {
        out.m_8 = (unsigned char)req.m_0d;
        return 0xbf;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        out.m_8 = (unsigned char)req.m_0d;
        return 0x3c;
    }
    if (user->IsCurCharacGhost())
    {
        out.m_8 = (unsigned char)req.m_0d;
        return 0x13;
    }
    if (G_GameWorld()->IsForbiddenMove(user->getCurCharacName()))
    {
        if (G_GameWorld()->GetLeftTimeOfForbiddenMove(
                user->getCurCharacName()) != 0)
        {
            out.m_9 = (unsigned char)req.m_0e;
            return 0x44;
        }
        G_GameWorld()->EnableMove(user->getCurCharacName());
    }
    return 0;
}
// ORIG 0x081c91e6 T _ZN18DisPatcher_SetArea4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_SetArea/read.md（汇编+反编译 C 全读）
int DisPatcher_SetArea::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* msgPtr = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*(msgPtr + 0xd)))
    {
        return LineFunc(0x1c16,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*(msgPtr + 0xe)))
    {
        return LineFunc(0x1c17,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0xf)))
    {
        return LineFunc(0x1c18,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0x11)))
    {
        return LineFunc(0x1c19,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*(msgPtr + 0x13)))
    {
        return LineFunc(0x1c1a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    *reinterpret_cast<unsigned short*>(msgPtr + 0x14) = 0;
    *reinterpret_cast<unsigned short*>(msgPtr + 0x16) = 0;
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0x14)))
    {
        return LineFunc(0x1c1f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0x16)))
    {
        return LineFunc(0x1c20,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// ORIG 0x081c9656 T _ZN18DisPatcher_SetArea4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_SetArea/send.md 汇编+反编译 C 全文转写）：
// m_errNo!=0x7fffffff 且 !=0 时发包：put_header(1,0x26)/0/errNo；errNo==0x44 →
// put_byte((char)m_9)+GetLeftTimeOfForbiddenMove(getCurCharacName())/60000 put_int；
// 否则 put_byte((char)m_8)；finalize(true)+Send。
void DisPatcher_SetArea::send(CUser* user, ParamBase& param)
{
    stSetAreaParam& p = static_cast<stSetAreaParam&>(param);
    if (p.m_errNo != 0x7fffffff && p.m_errNo != 0)
    {
        PacketGuard guard;
        guard.put_header(1, 0x26);
        guard.put_byte(0);
        guard.put_byte(p.m_errNo);
        if (p.m_errNo == 0x44)
        {
            guard.put_byte((char)p.m_9);
            guard.put_int(
                G_GameWorld()->GetLeftTimeOfForbiddenMove(
                    user->getCurCharacName()) /
                60000);
        }
        else
        {
            guard.put_byte((char)p.m_8);
        }
        guard.finalize(true);
        user->Send(guard);
    }
}
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
                        __PRETTY_FUNCTION__,
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
int DisPatcher_Fish::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08232ca4: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}

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
                        __PRETTY_FUNCTION__,
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
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_11))
    {
        return LineFunc(0x5f8,
                        __PRETTY_FUNCTION__,
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
int DisPatcher_UseSkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_USE_SKILL& req = *reinterpret_cast<MSG_USE_SKILL*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(reinterpret_cast<char*>(&msg) + 0xd)))
    {
        return LineFunc(0x1fce,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_slot))
    {
        return LineFunc(0x1fd0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_targetId))
    {
        return LineFunc(0x1fd2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(
            *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&msg) + 0x11)))
    {
        return LineFunc(0x1fd9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_UseSkill::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081caf28：读 param+0x4 到局部后即返回（值残留 eax，void 签名不消费）。
    int errNo = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    (void)errNo;
}

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
                        __PRETTY_FUNCTION__,
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
// ============================================================================
// ORIG T 0x081eb0c4 _ZN17DisPatcher_DieMob12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_DieMob/dispatch_sig.md，1564 insn）
// setMonsterMoveHack 门禁（moveCheckFlag && state==5）后按 state 分派：
//   5=普通副本 kill_monster 全反作弊链 / 10=死亡塔 handleKillMonster /
//   0xc=BossTower Global::CBossTowerFunc::DieMob / 0xd=advancealtar dieMob /
//   8=WarRoom HandleMonsterKill。
int DisPatcher_DieMob::dispatch_sig(CUser* user, PacketBuf& packet)
{
    MSG_MONSTER_DIE die;
    memset(&die, 0, sizeof(die));

    unsigned char passiveFlag = 0;               // ORIG -0x75（独立局部）
    unsigned char moveCheckFlag = 0;             // ORIG -0x76
    int monsterIdxOut = 0;                       // ORIG -0x74（kill_monster 出参）

    // packed 字段不可绑定非 const 引用，按 ORIG get_* 重载逐一读入临时量回填。
    unsigned short tmpU16 = 0;
    short tmpS16 = 0;
    int tmpS32 = 0;
    unsigned int tmpU32 = 0;
    unsigned char tmpU8 = 0;

    if (!packet.get_short(tmpU16))
        return LineFunc(0x6d09,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_mobSeq = tmpU16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d0a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldF = (unsigned short)tmpS16;
    if (!packet.get_int(tmpS32))
        return LineFunc(0x6d0c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_field11 = tmpS32;
    if (!packet.get_int(tmpS32))
        return LineFunc(0x6d0d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_field15 = tmpS32;
    if (!packet.get_int(tmpU32))
        return LineFunc(0x6d0e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_killValue2 = tmpU32;
    if (!packet.get_int(tmpU32))
        return LineFunc(0x6d0f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_killValue1 = tmpU32;
    if (!packet.get_byte(tmpU8))
        return LineFunc(0x6d11,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_mobCount = tmpU8;
    for (int i = 0; i < (int)(unsigned char)die.m_mobCount; ++i)
    {
        if (!packet.get_short(tmpU16))
            return LineFunc(0x6d16,
                            __PRETTY_FUNCTION__,
                            0, 0);
        die.m_skillList[i] = tmpU16;
        if (!packet.get_short(tmpU16))
            return LineFunc(0x6d17,
                            __PRETTY_FUNCTION__,
                            0, 0);
        die.m_attackerList[i] = tmpU16;
        if (!packet.get_int(tmpU32))
            return LineFunc(0x6d18,
                            __PRETTY_FUNCTION__,
                            0, 0);
        die.m_damageList[i] = tmpU32;
        if (!packet.get_short(tmpU16))
            return LineFunc(0x6d19,
                            __PRETTY_FUNCTION__,
                            0, 0);
        die.m_hitCountList[i] = tmpU16;
    }
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d25,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_killPairA24 = (unsigned short)tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d26,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_killPairA26 = (unsigned short)tmpS16;
    if (!packet.get_byte(tmpU8))
        return LineFunc(0x6d28,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA22 = tmpU8;
    if (!packet.get_byte(tmpU8))
        return LineFunc(0x6d2b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA23 = tmpU8;
    if (!packet.get_byte(passiveFlag))
        return LineFunc(0x6d2f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(moveCheckFlag))
        return LineFunc(0x6d32,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (moveCheckFlag != 0 && user->get_state() == 5)
    {
        user->GetPartyIndex();                   // ORIG 直呼丢弃返回值
        CParty* party =
            G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party == 0)
            return LineFunc(0x6d38,
                            __PRETTY_FUNCTION__,
                            0, 0);
        party->setMonsterMoveHack();
    }
    unsigned short unusedShort = 0;              // ORIG local_7c（读而未用）
    if (!packet.get_short(unusedShort))
        return LineFunc(0x6d3e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(die.m_fieldA28))
        return LineFunc(0x6d41,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(tmpS32))
        return LineFunc(0x6d42,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA2d = tmpS32;
    if (!packet.get_int(tmpS32))
        return LineFunc(0x6d43,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA31 = tmpS32;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d44,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA29 = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d45,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA2b = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d46,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA35 = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d47,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA37 = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d48,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA39 = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d49,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA3b = tmpS16;
    if (!packet.get_short(tmpS16))
        return LineFunc(0x6d4a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    die.m_fieldA3d = tmpS16;

    switch (user->get_state())
    {
    case 5:                                      // ---- 普通副本击杀 ----
    {
        user->GetPartyIndex();
        CParty* party =
            G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party == 0)
            return LineFunc(0x6d54,
                            __PRETTY_FUNCTION__,
                            0, 0);
        std::map<int, int>& cntMap = party->m_killMobCounter;
        if (cntMap.size() <= 199)
        {
            std::map<int, int>::iterator it =
                cntMap.find((int)die.m_mobSeq);
            if (it == cntMap.end())
                cntMap.insert(std::make_pair((int)die.m_mobSeq, 1));
            else
                it->second += 1;
        }
        if (passiveFlag == 1)
        {
            party->DestroyPassiveObject((int)die.m_mobSeq);
            return 0;
        }
        int r = party->kill_monster(
            user, (int)die.m_mobSeq,
            static_cast<unsigned short>(die.m_fieldF), die, &monsterIdxOut);
        if (r == 0)
        {
            PacketGuard pgFail;
            pgFail.put_header(1, 0x2a);
            pgFail.put_byte(0);
            pgFail.put_byte(5);
            pgFail.put_short((int)die.m_mobSeq & 0xffff);
            pgFail.finalize(true);
            user->Send(pgFail);
            return 0;
        }

        // ---- kill_monster 成功后的过度伤害/位置反作弊链 ----
        static_cast<Secu_HackLogCheck*>(user->GetHackCheck())
            ->DieMob(&die);
        if ((unsigned short)die.m_fieldF == 0xffff)
        {
            user->m_mcap.m_monsterDieHackCnt += 1;
            if ((unsigned int)user->m_mcap.m_monsterDieHackCnt > 4)
            {
                user->m_mcap.m_monsterDieHackCnt = 0;
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0xcd*/, 1,
                    (party->m_dungeon == 0) ? 0 :
                    (party->m_dungeon->get_index() == 0x2afe ? 2 : 0), 0);
                if (!party->checkBossRoom())
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, WongWork::ENUM_HACKTYPE_0 /*0xce*/, 1,
                        (party->m_dungeon == 0) ? 0 :
                        (party->m_dungeon->get_index() == 0x2afe ? 2 : 0),
                        0);
                }
            }
        }
        else
        {
            unsigned int tick =
                GlobalData::s_systemTime_.getCurTickCount();
            CUser::st_monstorkill& killWin =
                *reinterpret_cast<CUser::st_monstorkill*>(
                    &user->m_mcap.m_monsterKillWindow[0]);
            if (killWin.Check(tick, die.m_killPairA24, die.m_killPairA26,
                              600, 600, 1000))
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0xd1*/, 1, 0, 0);
            }
            user->m_mcap.m_monsterDieHackCnt = 0;
        }
        for (int i = 0; i < (int)(unsigned char)die.m_mobCount; ++i)
        {
            if (die.m_skillList[i] != 0x111)
                continue;
            unsigned int limit = 0;
            WongWork::ENUM_HACKTYPE hackType =
                static_cast<WongWork::ENUM_HACKTYPE>(0x6b);
            CUser* attacker =
                G_GameWorld()->find_from_world(die.m_attackerList[i]);
            bool attackerOk =
                attacker != 0 &&
                party->isMyMember((unsigned int)die.m_attackerList[i]) != 0;
            if (!attackerOk)
                continue;
            CHackLog_OverDamage::GetDungeonLimitDamageAndHackType(
                attacker, limit, hackType);
            if (die.m_killValue1 > 0x5dc)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0x6f*/, 1, 0, 0);
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x6ddb, 0);
                trace("Position : PacketDispatcher.cpp ZeroHit or 1500up Hit    Hit Count : %d  :::::::::::: ENUM_HACKTYPE_KILL_MONSTER_ZERO_HIT",
                      die.m_killValue1);
                die.m_killValue1 = 1;
            }
            bool zeroHitBossDg = false;
            if (die.m_killValue1 == 1)
            {
                zeroHitBossDg =
                    static_cast<CParty*>(attacker->GetParty())->m_dungeon
                        ->get_index() == 0x2afe;
            }
            if (zeroHitBossDg)
            {
                attacker->getHackAnalyzer()->addServerHackCnt(
                    attacker, WongWork::ENUM_HACKTYPE_0 /*0x7f*/, 1,
                    die.m_killValue1, 0);
            }
            if (die.m_killValue2 == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0x6d*/, 1, 0, 0);
                die.m_killValue2 = 1;
            }
            if (die.m_hitCountList[i] == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0x6e*/, 1, 0, 0);
                die.m_hitCountList[i] = 1;
            }
            unsigned int perHit =
                die.m_damageList[i] / (unsigned int)die.m_hitCountList[i];
            if (limit < perHit &&
                (party->m_dungeon == 0 ||
                 party->m_dungeon->get_index() != 0x2afe))
            {
                attacker->getHackAnalyzer()->addServerHackCnt(
                    attacker, hackType, 1, 0, 0);
                if (CHackLog_OverDamage::GetDungeonLimitMaxDamage() <
                    perHit)
                {
                    attacker->getHackAnalyzer()->addServerHackCnt(
                        attacker,
                        CHackLog_OverDamage::GetHackTypeOverLimitMaxDamage(),
                        1, 0, 0);
                }
            }
        }
        if (die.m_killValue1 % 10 == 0 && die.m_killValue2 % 10 == 0)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, WongWork::ENUM_HACKTYPE_0 /*0xca*/, 1, 0, 0);
        }
        user->getHackAnalyzer()->ValidateFixMobDiePos(
            user, (unsigned short)monsterIdxOut,
            die.m_killPairA24, die.m_killPairA26);
        break;
    }
    case 10:                                     // ---- 死亡塔 ----
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower == 0)
            break;
        if ((unsigned short)die.m_fieldF != 0xffff)
        {
            unsigned int tick =
                GlobalData::s_systemTime_.getCurTickCount();
            CUser::st_monstorkill& killWin =
                *reinterpret_cast<CUser::st_monstorkill*>(
                    &user->m_mcap.m_monsterKillWindow[0]);
            if (killWin.Check(tick, die.m_killPairA24, die.m_killPairA26,
                              600, 600, 1000))
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0xd4*/, 1, 0, 0);
            }
        }
        for (int i = 0; i < (int)(unsigned char)die.m_mobCount; ++i)
        {
            if (die.m_skillList[i] != 0x111)
                continue;
            CUser* attacker =
                G_GameWorld()->find_from_world(die.m_attackerList[i]);
            if (attacker == 0)
                continue;
            if (die.m_hitCountList[i] == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0x6e*/, 1, 0, 0);
                die.m_hitCountList[i] = 1;
            }
            unsigned int perHit =
                die.m_damageList[i] / (unsigned int)die.m_hitCountList[i];
            if (perHit > 12999)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0x6c*/, 1, 0, 0);
            }
            if (CHackLog_OverDamage::GetDeathTowerLimitDamage() < perHit)
            {
                attacker->getHackAnalyzer()->addServerHackCnt(
                    attacker,
                    CHackLog_OverDamage::GetDeathTowerOverDamageHackType(),
                    1, 0, 0);
            }
        }
        if ((unsigned short)die.m_fieldF == 0xffff)
        {
            user->GetPartyIndex();               // ORIG 直呼丢弃返回值
            CParty* party =
                G_CGameManager()->GetParty(user->GetPartyIndex());
            if (party == 0)
                return LineFunc(0x6e7f,
                                __PRETTY_FUNCTION__,
                                0, 0);
            user->m_mcap.m_monsterDieHackCnt += 1;
            if ((unsigned int)user->m_mcap.m_monsterDieHackCnt > 4)
            {
                user->m_mcap.m_monsterDieHackCnt = 0;
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0xcd*/, 1, 1, 0);
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0xce*/, 1, 1, 0);
            }
        }
        else
        {
            user->m_mcap.m_monsterDieHackCnt = 0;
        }
        tower->handleKillMonster(
            user, (unsigned short)die.m_mobSeq,
            (unsigned short)die.m_fieldF, die, &monsterIdxOut);
        user->getHackAnalyzer()->ValidateFixMobDiePos(
            user, (unsigned short)monsterIdxOut,
            die.m_killPairA24, die.m_killPairA26);
        break;
    }
    case 12:                                     // ---- BossTower ----
    {
        if ((unsigned short)die.m_fieldF == 0xffff)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, WongWork::ENUM_HACKTYPE_0 /*0xcd*/, 1, 0, 0);
            user->getHackAnalyzer()->addServerHackCnt(
                user, WongWork::ENUM_HACKTYPE_0 /*0xce*/, 1, 0, 0);
        }
        for (int i = 0; i < (int)(unsigned char)die.m_mobCount; ++i)
        {
            if (die.m_skillList[i] != 0x111)
                continue;
            CUser* attacker =
                G_GameWorld()->find_from_world(die.m_attackerList[i]);
            if (attacker == 0)
                continue;
            unsigned int perHit =
                die.m_damageList[i] / (unsigned int)die.m_hitCountList[i];
            if (CHackLog_OverDamage::GetBossTowerLimitDamage() < perHit)
            {
                attacker->getHackAnalyzer()->addServerHackCnt(
                    attacker,
                    CHackLog_OverDamage::GetBossTowerOverDamageHackType(),
                    1, 0, 0);
                if (CHackLog_OverDamage::GetDungeonLimitMaxDamage() <
                    perHit)
                {
                    attacker->getHackAnalyzer()->addServerHackCnt(
                        attacker,
                        CHackLog_OverDamage::GetHackTypeOverLimitMaxDamage(),
                        1, 0, 0);
                }
            }
            if (die.m_killValue1 > 0x5dc)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0x6f*/, 1, 0, 0);
                die.m_killValue1 = 1;
            }
            if (die.m_hitCountList[i] == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0x6e*/, 1, 0, 0);
                die.m_hitCountList[i] = 1;
            }
            if (die.m_killValue2 == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0x6d*/, 1, 0, 0);
                die.m_killValue2 = 1;
            }
        }
        Global::CBossTowerFunc::DieMob(
            user, (unsigned short)die.m_mobSeq,
            (unsigned short)die.m_fieldF, die, 0);
        break;
    }
    case 13:                                     // ---- advancealtar ----
        advancealtar::Manager::dieMob(
            user, (unsigned short)die.m_mobSeq,
            (unsigned short)die.m_fieldF, die);
        break;
    case 8:                                      // ---- 战争房间 ----
    {
        user->GetWarRoomIndex();                 // ORIG 直呼丢弃返回值
        WarRoom* war =
            G_CGameManager()->GetWarRoom(user->GetWarRoomIndex());
        if (war == 0)
            return LineFunc(0x6ed0,
                            __PRETTY_FUNCTION__,
                            0, 0);
        CUser* killer = 0;
        if ((unsigned short)die.m_fieldF != 0xffff)
        {
            killer =
                G_GameWorld()->find_from_world(die.m_fieldF);
            if (killer == 0)
                return LineFunc(0x6edb,
                                __PRETTY_FUNCTION__,
                                0, 0);
        }
        if (!war->HandleMonsterKill(user, (int)die.m_mobSeq, killer))
        {
            PacketGuard pgFail;
            pgFail.put_header(1, 0x2a);
            pgFail.put_byte(0);
            pgFail.put_byte(5);
            pgFail.put_short((int)die.m_mobSeq & 0xffff);
            pgFail.finalize(true);
            user->Send(pgFail);
        }
        break;
    }
    default:
        break;
    }
    return 0;
}

int DisPatcher_DieMob::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08232af4: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
int DisPatcher_DieMob::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232afe W _ZN17DisPatcher_DieMob7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081eb0c4)（读 MSG_MONSTER_DIE 各字段并按 get_state 分派 CParty::kill_monster/CDeathTower::handleKillMonster/Global::CBossTowerFunc::DieMob/advancealtar::Manager::dieMob/WarRoom::HandleMonsterKill，报告 docs/class_func_reports/DisPatcher_DieMob/dispatch_sig.md）

DisPatcher_DieCharacInDungeon::DisPatcher_DieCharacInDungeon() {}
DisPatcher_DieCharacInDungeon::~DisPatcher_DieCharacInDungeon() {}
int DisPatcher_DieCharacInDungeon::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c2bc0：Get_State_Check(user->get_state()) 为真 → 0，否则 -1。
    if (Global::CGlobalFunc::Get_State_Check((ch_state)user->get_state()))
        return 0;
    return -1;
}
int DisPatcher_DieCharacInDungeon::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (!Global::CGlobalFunc::Get_State_Check(
            static_cast<ch_state>(user->get_state())))
    {
        return LineFunc(0x19a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int DisPatcher_DieCharacInDungeon::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_DIE_CHARACTER& req = *reinterpret_cast<MSG_DIE_CHARACTER*>(&msg);
    if (!packet.get_short(
            *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&req) + 0xd)))
    {
        return LineFunc(0xca9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(
            *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&req) + 0xf)))
    {
        return LineFunc(0xcaa,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_DieCharacInDungeon::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081c2baa：读 param+0x4 到局部后即返回（值残留 eax，void 签名不消费）。
    int errNo = *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4);
    (void)errNo;
}

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
// ORIG 0x081ca7d6 T _ZN18DisPatcher_UseCoin11check_errorEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseCoin/check_error.md 逐块映射）：user->get_state()==5 →
// Dungeon_check_error；==8 → WarRoom_check_error；其余 → 0x13。
int DisPatcher_UseCoin::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() == 5)
    {
        return Dungeon_check_error(user, msg, param);
    }
    if (user->get_state() == 8)
    {
        return WarRoom_check_error(user, msg, param);
    }
    return 0x13;
}
// ORIG 0x081ca5e4 T _ZN18DisPatcher_UseCoin4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_UseCoin/read.md 逐块映射）：get_short → msg+0xd；
// 失败 LineFunc(0x1ebb, PRETTY, 0, 0)。
int DisPatcher_UseCoin::read(PacketBuf& packet, MSG_BASE& msg)
{
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(
            reinterpret_cast<char*>(&msg) + 0xd)))
    {
        return LineFunc(0x1ebb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_UseCoin::send(CUser* user, ParamBase& param)
{
    stUseCoinParam& out = static_cast<stUseCoinParam&>(param);
    if (out.m_errNo != 0x7fffffff)
    {
        if (out.m_errNo == 0)
        {
            PacketGuard guard;
            guard.put_header(1, 0x2c);
            guard.put_byte(1);
            guard.put_short(out.m_8);
            guard.finalize(true);
            user->Send(guard);
        }
        else
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2c, out.m_errNo & 0xff);
        }
    }
}
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
                        __PRETTY_FUNCTION__,
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
                            __PRETTY_FUNCTION__,
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
// ORIG 0x081c41e8 T _ZN21DisPatcher_GiveUpGame11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_GiveUpGame/check_error.md 逐块映射）：state ∈
// {5,10,12,13} → 0；其余 → 0x13（ORIG 每次比较前调 get_state，勿缓存）。
int DisPatcher_GiveUpGame::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() != 5 && user->get_state() != 10
        && user->get_state() != 0xc && user->get_state() != 0xd)
    {
        return 0x13;
    }
    return 0;
}
int DisPatcher_GiveUpGame::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x081c40aa: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}

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

    // ORIG 每个 state 分支前独立调 get_state（勿缓存）。
    if (user->get_state() == 10)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower != 0)
        {
            tower->handleLeaveUser(user);
        }
    }
    else if (user->get_state() == 0xc)
    {
        Global::CBossTowerFunc::GiveUpGame(user);
    }
    else if (user->get_state() == 0xd)
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
int DisPatcher_GetItem::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c35ac：msg 存入局部（-0x4）未使用，恒返 0。
    MSG_BASE& req = msg;   // ORIG -0x4(%ebp)
    (void)req;
    return 0;
}
// ORIG 0x081c335c T _ZN18DisPatcher_GetItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_GetItem/read.md（汇编+反编译 C 全读）
int DisPatcher_GetItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* msgPtr = reinterpret_cast<char*>(&msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0xd)))
    {
        return LineFunc(0xe2d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*(msgPtr + 0xf)))
    {
        return LineFunc(0xe2f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*(msgPtr + 0x10)))
    {
        return LineFunc(0xe30,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0x11)))
    {
        return LineFunc(0xe33,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0x13)))
    {
        return LineFunc(0xe34,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x15 get_short 不检失败；返回值（bool）低字节直写 +0x17
    *(msgPtr + 0x17) = packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0x15));
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0x1a)))
    {
        return LineFunc(0xe38,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0x1c)))
    {
        return LineFunc(0xe39,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(msgPtr + 0x18)))
    {
        return LineFunc(0xe3d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// ORIG 0x081c3ce8 T _ZN18DisPatcher_GetItem4sendEP5CUserR9ParamBase
// （报告 DisPatcher_GetItem/send.md 逐块映射）：param.m_errNo(+0x4)
// !=0x7fffffff；==0 → SendCmdOkPacket(0x2e)；否则 SendCmdErrorPacket(0x2e, err&0xff)。
void DisPatcher_GetItem::send(CUser* user, ParamBase& param)
{
    stGetItemParam& p = static_cast<stGetItemParam&>(param);
    if (p.m_errNo != 0x7fffffff)
    {
        if (p.m_errNo == 0)
        {
            user->SendCmdOkPacket((ENUM_CMDPACKET)0x2e);
        }
        else
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2e,
                                     (unsigned char)(p.m_errNo & 0xff));
        }
    }
}
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
                        __PRETTY_FUNCTION__,
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
                            __PRETTY_FUNCTION__,
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
// ============================================================================
// ORIG T 0x081f63c0 _ZN19DisPatcher_UseStack12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_UseStack/dispatch_sig.md，1320 insn）
// 读 slot(+0)/itemSpace?/itemId/slotIdx/checksum → state 白名单（5,7,8,3,6,
// 10,0xc）→ checksum 反作弊 0x25b → 封印限制 0xd1 → GetItemPtr/CheckItem(0x2a)
// → 可堆叠使用地点 → 组队/TOD/锦标赛限定（错误包 header(1,0x2f) 系列尾部）
// → use_stackable 落地。
// ============================================================================
int DisPatcher_UseStack::dispatch_sig(CUser* user, PacketBuf& packet)
{
    short slotNo = 0;                            // ORIG local_6e（get_short(short&)/movswl 实证）
    if (!packet.get_short(slotNo))
        return LineFunc(0x8315,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned char itemSpace = 0;                 // ORIG local_7c[0]
    if (!packet.get_byte(itemSpace))
        return LineFunc(0x8318,
                        __PRETTY_FUNCTION__,
                        0, 0);
    int itemAddInfo = 0;                         // ORIG local_80
    int param2 = 0;                              // ORIG local_84
    if (!packet.get_int(itemAddInfo))
        return LineFunc(0x831d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(param2))
        return LineFunc(0x831e,
                        __PRETTY_FUNCTION__,
                        0, 0);

    bool stateOk =
        (user->get_state() == 5) || (user->get_state() == 7) ||
        (user->get_state() == 8) || (user->get_state() == 3) ||
        (user->get_state() == 6) || (user->get_state() == 10) ||
        (user->get_state() == 12);
    // ---- 状态拒绝包形态：header(1,0x2f)/byte0/byte(code)/byte(space)/
    //      int(itemAddInfo)/int(param2)/finalize(true) ----
#define USESTACK_ERR(pg_, code_)                                               \
    do                                                                         \
    {                                                                          \
        pg_.put_header(1, 0x2f);                                               \
        pg_.put_byte(0);                                                       \
        pg_.put_byte(static_cast<int>(code_));                                 \
        pg_.put_byte(static_cast<int>(itemSpace));                             \
        pg_.put_int(static_cast<int>(itemAddInfo));                            \
        pg_.put_int(static_cast<int>(param2));                                 \
        pg_.finalize(true);                                                    \
        user->Send(pg_);                                                       \
    } while (0)

    if (!stateOk)
    {
        PacketGuard pgState;
        USESTACK_ERR(pgState, 0x13);
        return 0;
    }
    int invenCheckSum = 0;                       // ORIG local_88
    if (!packet.get_int(invenCheckSum))
        return LineFunc(0x8338,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if ((unsigned int)user->getCurCharacInvenCheckSum() != invenCheckSum)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, WongWork::ENUM_HACKTYPE_0 /*0x25b*/, 1, 0, 0);
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, static_cast<RestrictCategory::Enum>(1), 0xc))
    {
        PacketGuard pgRest;
        USESTACK_ERR(pgRest, 0xd1);
        return 0;
    }
    CItem* pItem = GlobalData::s_pSecuProtectionField->GetItemPtr(
        user, static_cast<char>(itemSpace), slotNo);
    if (pItem == 0)
    {
        return LineFunc(0x8354,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int secuErr = GlobalData::s_pSecuProtectionField->CheckItem(
        user, static_cast<SECURITY_PROTCTION>(0x2a), pItem);
    if (secuErr != 0)
    {
        PacketGuard pgSecu;
        USESTACK_ERR(pgSecu, secuErr);
        return 0;
    }
    if (pItem->is_stackable() &&
        !static_cast<CStackableItem*>(pItem)->verifyUsablePlace(user))
    {
        PacketGuard pgPlace;
        USESTACK_ERR(pgPlace, 0x13);
        return 0;
    }

    if (user->GetAssaultPlace() == 0 &&
        (user->get_state() == 3 || user->get_state() == 6))
    {
        CInventory* invR = static_cast<CInventory*>(const_cast<void*>(static_cast<const void*>(
            reinterpret_cast<const CUserCharacInfo*>(user)
                ->getCurCharacInvenR())));
        Inven_Item* ref =
            invR->GetInvenRef(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace),
                              (int)slotNo);
        if (ref == 0)
        {
            return LineFunc(0x83b7,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        unsigned int itemId =
            static_cast<unsigned int>(ref->m_addInfo);   // ORIG ref+2
        CDataManager* dm = G_CDataManager();
        CItem* tmpl = dm->find_item(static_cast<int>(itemId));
        if (tmpl == 0)
        {
            return 0;
        }
        int itemType = tmpl->GetItemType();              // ORIG vtable +0xc
        bool usableFamily =
            (itemType == 3) || (itemType == 0x16) ||
            (itemType == 0x18) || (itemType == 0x23);
        if (!usableFamily)
        {
            usableFamily =
                (itemId == 10) || (itemId == 0x18) ||
                (itemId == 0x33) ||
                (itemId >= 0x1e6d && itemId <= 0x1e73) ||
                itemId == 0x28978a;
        }
        if (!usableFamily)
        {
            PacketGuard pgType;
            USESTACK_ERR(pgType, 0x13);
            return 0;
        }
        // ---- 需要消耗疲劳的特殊物品表（LAB_081f6df8 上方比较树）----
        unsigned short fatigueNeed = 0;
        bool needFatigue = false;
        switch (itemId)
        {
        case 0x1f8f:
        case 0x27ac5a:
        case 0x27addd:
        case 0x292090de:
            fatigueNeed = 0x1e;
            needFatigue = true;
            break;
        case 0x1d5e:
            fatigueNeed = 10;
            needFatigue = true;
            break;
        case 0x1d20:
            fatigueNeed = 0x28;
            needFatigue = true;
            break;
        case 0x1ddc:
        case 0x1ddd:
            fatigueNeed = 6;
            needFatigue = true;
            break;
        case 0x201d:
            fatigueNeed = 0x14;
            needFatigue = true;
            break;
        default:
            break;
        }
        if (needFatigue)
        {
            if (user->get_state() != 3)
            {
                PacketGuard pgFat1;
                USESTACK_ERR(pgFat1, 0x13);
                return 0;
            }
            if (reinterpret_cast<CUserCharacInfo*>(user)
                    ->getCurCharacFatigue() < fatigueNeed)
            {
                PacketGuard pgFat2;
                USESTACK_ERR(pgFat2, 0x5f);
                return 0;
            }
        }
    }

    if (user->get_state() == 10)
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower == 0)
        {
            return LineFunc(0x8456,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!tower->handleUseStackable(
                user, static_cast<ENUM_ITEMSPACE>(itemSpace), slotNo))
        {
            return LineFunc(0x8458,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        WongWork::CDeathTower* tower = 0;
        if (user->get_state() == 10)
            tower = user->getDeathTower();
        if (tower == 0)
        {
            if (party->m_partyType == 1)
            {
                PacketGuard pgParty1;
                USESTACK_ERR(pgParty1, 0);
                return 0;
            }
            if (party->m_dungeon != 0 &&
                party->m_dungeon->isTowerOfDespairDungeon() &&
                party->m_dungeon->limitOfStackableItemInTower())
            {
                CInventory* invR2 = static_cast<CInventory*>(const_cast<void*>(static_cast<const void*>(
                    reinterpret_cast<const CUserCharacInfo*>(user)
                        ->getCurCharacInvenR())));
                Inven_Item* ref2 =
                    invR2->GetInvenRef(
                        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace),
                        (int)slotNo);
                if (ref2 == 0)
                {
                    return 0;
                }
                unsigned int todItemId =
                    static_cast<unsigned int>(ref2->m_addInfo);
                if (!(todItemId == 0x18 || todItemId == 10 ||
                      todItemId == 0x33 || todItemId == 0x28978a))
                {
                    int dungeonIndex =
                        reinterpret_cast<CBattle_Field*>(
                            &party->m_padBattleField1)
                            ->get_dungeon_index();
                    cMyTrace trace(
                        __PRETTY_FUNCTION__,
                        0x84a2, 5);
                    trace("TOD : charac_no(%u),dungeonIndex(%d),item_id(%lu)",
                          reinterpret_cast<CUserCharacInfo*>(user)
                              ->getCurCharacNo(),
                          dungeonIndex, todItemId);
                    return 0;
                }
            }
            if (party->m_dungeon != 0 &&
                party->m_dungeon->isTournamentDungeon())
            {
                CInventory* invR3 = static_cast<CInventory*>(const_cast<void*>(static_cast<const void*>(
                    reinterpret_cast<const CUserCharacInfo*>(user)
                        ->getCurCharacInvenR())));
                Inven_Item* ref3 =
                    invR3->GetInvenRef(
                        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace),
                        (int)slotNo);
                if (ref3 == 0)
                {
                    return LineFunc(0x84b1,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                CItem* tItem =
                    G_CDataManager()->find_item(ref3->m_addInfo);
                if (tItem == 0)
                {
                    return 0;
                }
                if (!tItem->is_stackable())
                {
                    return 0;
                }
                CStackableItem* stack = static_cast<CStackableItem*>(tItem);
                int denyCode = 0;
                if (stack->get_sub_type() == 2)
                {
                    denyCode = 0xff;
                }
                else if (*reinterpret_cast<int*>(
                             reinterpret_cast<char*>(stack) + 0xd90) != 0 &&
                         stack->GetItemType() != 0x12)
                {
                    denyCode = 0xff;
                }
                if (denyCode != 0)
                {
                    PacketGuard pgTour;
                    USESTACK_ERR(pgTour, denyCode);
                    return 0;
                }
            }
        }
        else
        {
            CDungeon* dg = tower->getCDungeonMGr()->getDungeon();
            if (dg->limitOfStackableItemInTower() &&
                party->m_partyType != 1)
            {
                PacketGuard pgTower;
                USESTACK_ERR(pgTower, 0);
                return 0;
            }
        }
    }

    {
        CInventory* invRFin = static_cast<CInventory*>(const_cast<void*>(static_cast<const void*>(
            reinterpret_cast<const CUserCharacInfo*>(user)
                ->getCurCharacInvenR())));
        Inven_Item* refFin =
            invRFin->GetInvenRef(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)itemSpace),
                                 (int)slotNo);
        if (refFin == 0)
        {
            return 0;
        }
        if (refFin->m_addInfo == 0x27ac4a)
        {
            int lastKillTime =
                user->GetCurCharacChaosKillTime();
            CParty* finParty = static_cast<CParty*>(user->GetParty());
            char chaosErr = 0;                       // ORIG local_d
            int curSec = GlobalData::s_systemTime_.getCurSec();
            if (curSec < lastKillTime + 600)
            {
                chaosErr = -6;
            }
            else if (finParty != 0 &&
                     finParty->CheckMemberLastTryAssaultTime())
            {
                chaosErr = -3;
            }
            if (chaosErr != 0)
            {
                PacketGuard pgChaos;
                USESTACK_ERR(pgChaos, (int)chaosErr);
                return 0;
            }
        }
        if (!user->use_stackable(slotNo, (int)itemSpace,
                                 static_cast<int>(itemAddInfo),
                                 static_cast<int>(param2)))
        {
            return LineFunc(0x8508,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
#undef USESTACK_ERR
}
int DisPatcher_UseStack::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x082331b2: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
// ORIG 0x082331bc W _ZN19DisPatcher_UseStack7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制
// 即为空实现（read 同为空）。UseStack 全部业务（读包/校验/应答 0x2f 系列）内嵌于
// dispatch_sig(0x081f63c0，见 docs/class_func_reports/DisPatcher_UseStack/dispatch_sig.md)。
int DisPatcher_UseStack::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }


DisPatcher_MoveMap::DisPatcher_MoveMap() {}
DisPatcher_MoveMap::~DisPatcher_MoveMap() {}
int DisPatcher_MoveMap::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return 0xfffffffd;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        return 0xfffffffc;
    }
    MSG_MOVE_MAP& req = *reinterpret_cast<MSG_MOVE_MAP*>(&msg);   // ORIG -0xc(%ebp)
    // CParty::m_padBattleField1(+0xb24) 无访问器，按 ORIG 裸偏移取 BF 视图（同 TU 先例）
    CBattle_Field& battleField =
        *reinterpret_cast<CBattle_Field*>(reinterpret_cast<char*>(party) + 0xb24);
    bool invalid;
    if ((int)req.m_y >= battleField.getMapHeight() ||
        (int)req.m_x >= battleField.getMapWidth())
    {
        invalid = true;
    }
    else
    {
        invalid = false;
    }
    if (invalid)
    {
        return 0xfffffffb;
    }
    user->lock();
    if (user != 0)
    {
        static_cast<std::set<std::pair<unsigned char, unsigned short> >*>(
            user->GetCSHashSet())
            ->clear();
    }
    user->unlock();
    if (!Global::CGlobalFunc::Get_State_Check(
            static_cast<ch_state>(user->get_state())))
    {
        return 0xffffffff;
    }
    return 0;
}
int DisPatcher_MoveMap::check_state(CUser* user, PacketBuf& /*packet*/)
{
    if (!Global::CGlobalFunc::Get_State_Check(
            static_cast<ch_state>(user->get_state())))
    {
        return LineFunc(0x241,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int DisPatcher_MoveMap::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_MOVE_MAP& mv = *reinterpret_cast<MSG_MOVE_MAP*>(&msg);
    if (packet.get_byte(*reinterpret_cast<char*>(&mv.m_x)) != true)
    {
        return LineFunc(0x1152,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_byte(*reinterpret_cast<char*>(&mv.m_y)) != true)
    {
        return LineFunc(0x1155,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_int(mv.m_fieldF) != true)
    {
        return LineFunc(0x1158,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_int(mv.m_field13) != true)
    {
        return LineFunc(0x1159,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_byte(mv.m_field17) != true)
    {
        return LineFunc(0x115d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    mv.m_1a = static_cast<unsigned char>(
        packet.get_short(*reinterpret_cast<unsigned short*>(mv.m_pad18)));
    for (int local_14 = 0; local_14 < 4; local_14 = local_14 + 1)
    {
        if (packet.get_short(mv.m_1b[local_14]) != true)
        {
            return LineFunc(0x1164,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    for (int local_10 = 0; local_10 < 4; local_10 = local_10 + 1)
    {
        if (packet.get_int(mv.m_23[local_10]) != true)
        {
            return LineFunc(0x1169,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    if (packet.get_short(*reinterpret_cast<unsigned short*>(mv.m_pad33)) != true)
    {
        return LineFunc(0x116e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_int(*reinterpret_cast<int*>(mv.m_pad33 + 2)) != true)
    {
        return LineFunc(0x1181,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
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
                        __PRETTY_FUNCTION__,
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
// ============================================================================
// ORIG T 0x081f4ce6 _ZN24DisPatcher_SetPlayResult12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetPlayResult/dispatch_sig.md，1083 insn）
// state==5 → party 校验（state==2 直接 0 / m_partyType==1 跳过）→ GameResultType
// 读包（Clear 后逐字段直写 party 结果区【wire 布局按 ORIG +0x17f4..0x1846】）→
// 地图清除日志/金币/经验比率反作弊（0x1fb、0x78、0x75）/疲劳特化（job 3,4 且
// growtype 2 → ×0.5 否则 ×0.8）/清副本 timer 注册/角色信息核对（0xe2、0xe3）/
// 使用技能材料比率（0xe5、0xe4）/尾巴成员药丸检查（0x1a2）→
// CheckRecvFromAllMember → SetPlayResult。
// ============================================================================
int DisPatcher_SetPlayResult::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 5)
        return 0;

    user->GetPartyIndex();                       // ORIG 直呼丢弃返回值
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
        return LineFunc(0x8155,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (party->m_partyType == 1)
        return 0;

    // ---- 结果区视图（ORIG party+0x17f4 GameResultType，本仓库为
    //      char m_padResult[0x50] 视图；字节序 wire 布局逐字段写入）----
    unsigned char* resultBase =
        reinterpret_cast<unsigned char*>(&party->m_padResult);
    GameResultType* gameResult =
        reinterpret_cast<GameResultType*>(resultBase);   // +0x17f4 Clear 起点
    gameResult->Clear();

    if (!packet.get_byte(*reinterpret_cast<char*>(resultBase + 9)))   // +0x17fd
        return LineFunc(0x816d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(resultBase + 10)))  // +0x17fe
        return LineFunc(0x8172,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned short resultIdx = 0;
    if (!packet.get_short(*reinterpret_cast<short*>(resultBase + 0x4a))) // +0x183e
        return LineFunc(0x8174,
                        __PRETTY_FUNCTION__,
                        0, 0);

    char memberCountByte = 0;                    // ORIG local_ea3
    if (!packet.get_byte(memberCountByte))
        return LineFunc(0x8179,
                        __PRETTY_FUNCTION__,
                        0, 0);
    for (int i = 0; i < memberCountByte && i < 4; ++i)
    {
        // 每成员结果二元组：ushort@party+0x1808+i*8、int@party+0x180c+i*8
        unsigned short memberVal = 0;
        int memberCoin = 0;
        if (!packet.get_short(*reinterpret_cast<short*>(
                reinterpret_cast<char*>(party) + (i + 0x300) * 8 + 8)))
            return LineFunc(0x817e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_int(*reinterpret_cast<int*>(
                reinterpret_cast<char*>(party) + (i + 0x300) * 8 + 0xc)))
            return LineFunc(0x8181,
                            __PRETTY_FUNCTION__,
                            0, 0);
        (void)memberVal;
        (void)memberCoin;
    }
    user->m_historyLog.MapClear(
        *reinterpret_cast<unsigned short*>(resultBase + 0x4a),
        static_cast<int>(party->GetMapPlayingTime() / 1000), 0);

    unsigned int killCountRead = 0;              // ORIG local_ea8
    if (!packet.get_int(killCountRead))
        return LineFunc(0x818b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (killCountRead != 0 && !party->checkBossRoom())
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, WongWork::ENUM_HACKTYPE_0 /*0x1fb*/,
            static_cast<unsigned int>(killCountRead), 0, 0);
    }
    unsigned int* coinSlot =
        reinterpret_cast<unsigned int*>(
            const_cast<char*>(reinterpret_cast<const char*>(party)) +
            0x1840);                             // ORIG +0x1840 金币累计
    if (!packet.get_int(*coinSlot))
        return LineFunc(0x8195,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (*coinSlot > 10000)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, WongWork::ENUM_HACKTYPE_0 /*0x78*/, 1,
            *coinSlot / 10000, 0);
        *coinSlot = *coinSlot % 10000;
    }

    int job =
        reinterpret_cast<CUserCharacInfo*>(user)->get_charac_job();
    bool fatigueSpecJob =
        job == 3 ||
        (job == 4 &&
         reinterpret_cast<CUserCharacInfo*>(user)
             ->getCurCharFirstGrowType() == 2);
    double ratioDelta =
        fatigueSpecJob ? 0.5 : 0.8;              // _DAT_08bd6320 / _DAT_08bd6328
    *coinSlot = static_cast<unsigned int>(
        static_cast<long long>(ratioDelta *
                               static_cast<double>(*coinSlot)));
    if (party->GetMapPlayingTime() / 1000 < *coinSlot)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, WongWork::ENUM_HACKTYPE_0 /*0x75*/, 1, *coinSlot, 0);
    }

    CBattle_Field& field =
        *reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1);
    *reinterpret_cast<int*>(resultBase + 4) =        // +0x17f8 dungeon index
        field.get_dungeon_index();
    *reinterpret_cast<char*>(resultBase + 8) =       // +0x17fc diff
        static_cast<char>(field.get_dungeon_diff());
    party->IncreaseSetPlayResultCount(user);

    if (party->get_dungeon_clear_state() == 0)
    {
        if (!field.IsClearDungeon())
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, WongWork::ENUM_HACKTYPE_0 /*0x196*/, 1, 0, 0);
            return 0;
        }
        int key = party->gen_timer_key((TIMER_MESSAGE)0x29);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                    party->GetPartyIndex(),
                                    (TIMER_MESSAGE)0x29, 0xf, key, 0);
        party->set_dungeon_clear_state(1);
    }
    MapInfo* mapInfo = field.GetCurrentMapInfo();
    if (*reinterpret_cast<int*>(
            reinterpret_cast<char*>(mapInfo) + 0x4c) == 0)   // kill checker 区判空
    {
        SIG_LOAD_CHARAC sigCharac;               // ORIG local_15bf 清零缓冲
        memset(&sigCharac, 0, sizeof(sigCharac));
        if (user->GetCharacInfo(&sigCharac),
            0x1d < *reinterpret_cast<short*>(
                       reinterpret_cast<char*>(&sigCharac) + sizeof(SIG_LOAD_CHARAC)))
        {
            // ORIG local_158e 为 SIG_LOAD_CHARAC 尾部长度字段；
            // 此处结构建模以 GetCharacInfo 报告为准【推断】。
            user->getHackAnalyzer()->addServerHackCnt(
                user, WongWork::ENUM_HACKTYPE_0 /*0xe2*/, 1, 0, 0);
        }
    }
    if (*reinterpret_cast<int*>(
            reinterpret_cast<char*>(mapInfo) + 0x4c) != 0)
    {
        // stMapMonsterKillChecker_t 计数比较（WarField.h 权威）
        int useSkillMaterialCount =
            reinterpret_cast<stMapMonsterKillChecker_t*>(
                reinterpret_cast<char*>(mapInfo) + 0x4c)
                ->getUseSkillMaterialCount();
        unsigned int playSec = party->GetMapPlayingTime() / 1000;
        unsigned int tickUnits = playSec / 10;
        if (playSec % 10 != 0)
            ++tickUnits;
        if (useSkillMaterialCount < static_cast<int>(tickUnits))
        {
            SIG_LOAD_CHARAC sigChk;
            memset(&sigChk, 0, sizeof(sigChk));
            if ((user->GetCharacInfo(&sigChk),
                 0x1d < *reinterpret_cast<short*>(
                            reinterpret_cast<char*>(&sigChk))))
                user->getHackAnalyzer()->addServerHackCnt(
                    user, WongWork::ENUM_HACKTYPE_0 /*0xe3*/, 1, 0, 0);
        }
    }

    unsigned int playMs = party->GetMapPlayingTime();     // ORIG local_44
    int level =
        reinterpret_cast<CUserCharacInfo*>(user)->get_charac_level();
    if (playMs < 10000 && 10 < level)
    {
        int fastCnt = 11 - playMs / 1000;                 // ORIG local_28
        user->getHackAnalyzer()->addServerHackCnt(
            user, WongWork::ENUM_HACKTYPE_0 /*0xe5*/,
            static_cast<unsigned int>(fastCnt), 0, 0);
    }
    if (playMs < 20000 && 10 < level)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, WongWork::ENUM_HACKTYPE_0 /*0xe4*/, 1, 0, 0);
    }

    unsigned int memberKillSum = 0;               // local_ebc
    unsigned int coinUsedA = 0;                   // local_eac
    unsigned int coinDenomA = 0;                  // local_eb4
    unsigned int expGain = 0;                     // local_eb0
    unsigned int expDenomB = 0;                   // local_eb8
    if (!packet.get_int(memberKillSum))
        return LineFunc(0x820e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(coinUsedA))
        return LineFunc(0x820f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(expGain))
        return LineFunc(0x8210,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(coinDenomA))
        return LineFunc(0x8211,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(expDenomB))
        return LineFunc(0x8212,
                        __PRETTY_FUNCTION__,
                        0, 0);

    bool tailChecksOk =
        !(memberKillSum != 0 || party->GetUsedCoinCount() != 0 ||
          field.GetTotalSkillMaterial() != 0 ||
          coinDenomA == 0 ||
          100.0f * (static_cast<float>(coinUsedA) /
                    static_cast<float>(coinDenomA)) < 99.98f ||
          expDenomB == 0 ||
          100.0f * (static_cast<float>(expGain) /
                    static_cast<float>(expDenomB)) < 99.98f);
    if (tailChecksOk)
    {
        bool pillFound = false;                           // ORIG local_21
        for (int i = 0; i < 4; ++i)
        {
            if (!party->checkValidUser(i))
                continue;
            CInventory* memberInv = static_cast<CInventory*>(
                const_cast<void*>(static_cast<const void*>(
                    party->GetMember()[i].m_pUser->getCurCharacInvenR())));
            Inven_Item slotItem = memberInv->GetInvenSlot(1, 0xb);
            if (slotItem.m_addInfo == 0x2ad4)
            {
                pillFound = true;
                break;
            }
            Inven_Item slotItem2 = memberInv->GetInvenSlot(1, 0x89 - 0x80);
            if (slotItem2.m_addInfo == 0x329e)
            {
                pillFound = true;
                break;
            }
        }
        if (!pillFound)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, WongWork::ENUM_HACKTYPE_0 /*0x1a2*/, 1, 0, 0);
        }
    }
    if (!party->CheckRecvFromAllMember(user))
        return 0;
    if (!party->SetPlayResult(user))
        return LineFunc(0x8235,
                        __PRETTY_FUNCTION__,
                        0, 0);
    return 0;
}
int DisPatcher_SetPlayResult::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x0823302a: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
int DisPatcher_SetPlayResult::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233034 W _ZN24DisPatcher_SetPlayResult7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f4ce6)（GetParty → GameResultType 读包/反作弊计数/SetPlayResult，报告 docs/class_func_reports/DisPatcher_SetPlayResult/dispatch_sig.md）

DisPatcher_DropItem::DisPatcher_DropItem() {}
DisPatcher_DropItem::~DisPatcher_DropItem() {}
// ORIG 0x081c2d9a T _ZN19DisPatcher_DropItem11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_DropItem/check_error.md 汇编+反编译 C 全文转写）：
// 角色空/数量<1 → -1；state==10 → 0x13；组队血副本 → 0x13；综合PVP频道 → 0x13；
// isRestricted(1,1) → param.m_c=1,0xd1；m_padTail!=1 时安全保护 0x1e（param.m_c=1,返码）；
// CheckItemLock → 0xd5；isHackUser → 0x13；space 1 → 0x17；space 3 且 pos<10 → 0x17；
// space 0 且 pos==0 且 count>1000 → 0x46；否则 0。（ORIG +0x18b 存在未读死局部 6）
int DisPatcher_DropItem::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const MSG_DROP_ITEM& req = *reinterpret_cast<const MSG_DROP_ITEM*>(&msg);
    stDropItemParam& out = static_cast<stDropItemParam&>(param);
    int deadLocal;               // ORIG -0x14 槽（+0x18b 写 6 未再读，帧形状保留）
    if (user->getCurCharacR() == 0)
    {
        return -1;
    }
    if (req.m_14 < 1)
    {
        return -1;
    }
    if (user->get_state() == 10)
    {
        return 0x13;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0 && party->m_dungeon != 0 &&
        party->m_dungeon->m_bloodDungeonType != 0)
    {
        return 0x13;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return 0x13;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 1))
    {
        out.m_c = 1;
        return 0xd1;
    }
    if ((unsigned char)req.m_padTail[0] != 1)
    {
        int secu = GlobalData::s_pSecuProtectionField->Check(
            user, static_cast<SECURITY_PROTCTION>(0x1e));
        if (secu != 0)
        {
            out.m_c = 1;
            return secu;
        }
    }
    if (user->CheckItemLock(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_11), (int)req.m_12))
    {
        return 0xd5;
    }
    deadLocal = 6;               // ORIG +0x18b（未再读）
    if (user->isHackUser())
    {
        return 0x13;
    }
    if ((int)(char)req.m_11 == 1)
    {
        return 0x17;
    }
    if ((int)(char)req.m_11 == 3 && req.m_12 <= 9)
    {
        return 0x17;
    }
    if ((unsigned char)req.m_11 == 0 && (unsigned short)req.m_12 == 0 &&
        (unsigned int)req.m_14 > 0x3e8)
    {
        return 0x46;
    }
    return 0;
}
// ORIG 0x081c2bee T _ZN19DisPatcher_DropItem4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_DropItem/read.md 汇编+反编译 C 全文转写）：
// 依次 get_short(+0x0d)/get_short(+0x0f)/get_byte(char&)(+0x11)/get_short(+0x12)/
// get_int(+0x14)/get_byte(char&)(+0x18)；失败分别 LineFunc 0xd0c..0xd12；成功返回 0。
int DisPatcher_DropItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_DROP_ITEM& req = *reinterpret_cast<MSG_DROP_ITEM*>(&msg);
    if (!packet.get_short(req.m_0d))
    {
        return LineFunc(0xd0c, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_0f))
    {
        return LineFunc(0xd0d, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_11)))
    {
        return LineFunc(0xd0e, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_12))
    {
        return LineFunc(0xd0f, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(req.m_14))
    {
        return LineFunc(0xd10, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(req.m_padTail[0]))
    {
        return LineFunc(0xd12, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
void DisPatcher_DropItem::send(CUser* user, ParamBase& param)
{
    stDropItemParam& out = static_cast<stDropItemParam&>(param);
    if (out.m_errNo != 0)
    {
        if (out.m_c == 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x32, out.m_errNo & 0xff);
        }
        else
        {
            PacketGuard guard;
            guard.put_header(1, 0x32);
            guard.put_byte(0);
            guard.put_byte(out.m_errNo);
            guard.put_byte(static_cast<char>(out.m_8));
            guard.finalize(true);
            user->Send(guard);
        }
    }
}
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
    int state = user->get_state();
    if (state == 5)
    {
        CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party != 0)
        {
            int ret = party->drop_item(user, req.m_0d, req.m_0f, (char)req.m_11,
                                       req.m_12, req.m_14);
            if (ret > 0)
            {
                out.m_errNo = ret;
                out.m_8 = req.m_11;
            }
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
int DisPatcher_UseEquip::check_error(CUser* user, MSG_BASE& /*msg*/)
{
    bool valid = user->get_state() == 5 || user->get_state() == 7 ||
                 user->get_state() == 8 || user->get_state() == 0xa ||
                 user->get_state() == 0xc;
    if (!valid)
    {
        return 0x13;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        if (party->m_partyType == 1)                    // +0xcd8
        {
            return 0x13;
        }
        CDungeon* dungeon = party->m_dungeon;           // +0xcac（ORIG -0xc）
        if (dungeon != 0 && dungeon->m_bloodDungeonType != 0)  // +0x89f
        {
            return 0x13;
        }
    }
    return 0;
}
int DisPatcher_UseEquip::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_DECREASE_DURABILITY& req = *reinterpret_cast<MSG_DECREASE_DURABILITY*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))
    {
        return LineFunc(0x9d0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_UseEquip::send(CUser* user, ParamBase& param)
{
    stOnUseEquipParam& out = static_cast<stOnUseEquipParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x33);
    if (out.m_errNo == 0)
    {
        guard.put_byte(1);
        guard.put_byte(static_cast<char>(out.m_8));
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(out.m_errNo);
    }
    guard.finalize(true);
    user->Send(guard);
}
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
bool DisPatcher_MakePVP::ExtractPacket(PacketBuf& packet, MSG_MAKE_PVP_ROOM& msg)
{
    // ORIG 0x081f81ba T（报告 DisPatcher_MakePVP/ExtractPacket.md 全文转写）。
    char tail;
    if (!packet.get_byte(msg.m_field0d) || msg.m_field0d < 0)
    {
        return false;
    }
    if (msg.m_field0d == 0)
    {
        int len = 0;
        if (!packet.get_int(len) || len < 0 || len >= 0x1e)
        {
            return false;
        }
        if (!packet.get_str(msg.m_name, 0x1e, len))
        {
            return false;
        }
    }
    if (!packet.get_short(msg.m_field2c))
    {
        return false;
    }
    if (!packet.get_byte(msg.m_field2e))
    {
        return false;
    }
    if (msg.m_field2e == 1)
    {
        if (!packet.get_int(msg.m_field2f))
        {
            return false;
        }
        if (msg.m_field2f > 8)
        {
            return false;
        }
        if (!packet.get_binary(msg.m_field33, msg.m_field2f))
        {
            return false;
        }
    }
    if (!packet.get_byte(tail))
    {
        return false;
    }
    msg.m_battleMode = (tail == 1) ? 1 : 0;
    return true;
}

int DisPatcher_MakePVP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081f803e T（报告 DisPatcher_MakePVP/dispatch_sig.md 全文转写）：
    // ExtractPacket 失败→LineFunc(0x85b6)；房名 slang→SendCmdErrorPacket(0x35,0x9f)；
    // 否则 MakePVPRoom(user,msg,0x35, mode==0?2:6)。
    MSG_MAKE_PVP_ROOM msg;
    memset(&msg, 0, 0x3d);
    if (!ExtractPacket(packet, msg))
    {
        return LineFunc(0x85b6, __PRETTY_FUNCTION__, 0, 0);
    }
    std::string name(msg.m_name);
    if (CSyncSlangFilter::GetInstance()->HasSlang(name))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x35, 0x9f);
    }
    else
    {
        MakePVPRoom(user, msg, (ENUM_CMDPACKET)0x35, (PVP_BATTLE_MODE)(msg.m_battleMode == 0 ? 2 : 6));
    }
    return 0;
}
int DisPatcher_MakePVP::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08233214: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}

// ORIG 0x0823321e W _ZN18DisPatcher_MakePVP7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部建房逻辑（ExtractPacket(0x081f81ba) 解包 MSG_MAKE_PVP_ROOM → CSyncSlangFilter::
// HasSlang 拒绝 0x35/0x9f → MakePVPRoom(cmd=0x35, mode=local_15?6:2)）内嵌于
// dispatch_sig(0x081f803e，报告 docs/class_func_reports/DisPatcher_MakePVP/dispatch_sig.md)。
int DisPatcher_MakePVP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_EnterPVP::DisPatcher_EnterPVP() {}
DisPatcher_EnterPVP::~DisPatcher_EnterPVP() {}
int DisPatcher_EnterPVP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ed6c8 T（报告 DisPatcher_EnterPVP/dispatch_sig.md 全文转写）：
    // get_short(pvpIdx) → 清 flag/seat/buf[9] → get_byte(flag)，flag==1 时
    // get_int(seat)（>0xa→LineFunc）+ get_binary(buf,seat)；
    // GetPvp(idx,user,0)==0 → 错误包(1,0x36)+0+0x16；否则 EnterPVPRoom(user,pvp,0x36,buf)。
    short pvpIdx;
    if (!packet.get_short(pvpIdx))
    {
        return LineFunc(0x6fc7, __PRETTY_FUNCTION__, 0, 0);
    }
    char flag = 0;              // ORIG movb $0x0,-0x1c
    int seat = 0;               // ORIG movl $0x0,-0x20
    char buf[9] = { 0 };        // ORIG movl/movl/movb 内联清零（-0x29..-0x21）
    if (!packet.get_byte(flag))
    {
        return LineFunc(0x6fce, __PRETTY_FUNCTION__, 0, 0);
    }
    if (flag == 1)
    {
        if (!packet.get_int(seat))
        {
            return LineFunc(0x6fd2, __PRETTY_FUNCTION__, 0, 0);
        }
        if (seat > 0xa)         // ORIG cmp $0xa; jle 通过
        {
            return LineFunc(0x6fd4, __PRETTY_FUNCTION__, 0, 0);
        }
        if (!packet.get_binary(buf, seat))
        {
            return LineFunc(0x6fd7, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    PvP_Room* pvp = G_CGameManager()->GetPvp((int)pvpIdx, user, 0);
    if (pvp == 0)
    {
        PacketGuard guard;
        guard.put_header(1, 0x36);
        guard.put_byte(0);
        guard.put_byte(0x16);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    EnterPVPRoom(user, pvp, (ENUM_CMDPACKET)0x36, buf);
    return 0;
}
   int DisPatcher_EnterPVP::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08232b7e: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
int DisPatcher_EnterPVP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232b88 W _ZN19DisPatcher_EnterPVP7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ed6c8)（读 pvp 索引/密码后 CGameManager::GetPvp → EnterPVPRoom，报告 docs/class_func_reports/DisPatcher_EnterPVP/dispatch_sig.md）

DisPatcher_SetPVPSeat::DisPatcher_SetPVPSeat() {}
DisPatcher_SetPVPSeat::~DisPatcher_SetPVPSeat() {}
// ORIG 0x081f5eba T _ZN21DisPatcher_SetPVPSeat12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetPVPSeat/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=5 → 0；get_byte(seat) 失败或 seat∉[0,7] → LineFunc 0x82a4；get_byte(state)
// 失败 → 0x82a6；room==0 → 0x82a9；OnSetPVPSeatState(user,(int)(char)seat,state,2)!=1
// → room->reset() + G_CGameManager()->PutPvp(room)；返回 0。
int DisPatcher_SetPVPSeat::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 5)
    {
        return 0;
    }
    char seat;
    if (!packet.get_byte(seat) || (char)seat < 0 || (char)seat > 7)
    {
        return LineFunc(0x82a4, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned char state;
    if (!packet.get_byte(state))
    {
        return LineFunc(0x82a6, __PRETTY_FUNCTION__, 0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x82a9, __PRETTY_FUNCTION__, 0, 0);
    }
    if (room->OnSetPVPSeatState(user, (int)(char)seat,
                                (ENUM_SEAT_STATE)(int)(unsigned char)state,
                                (ENUM_WALKOUT_TYPE)2) != 1)
    {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
    return 0;
}
int DisPatcher_SetPVPSeat::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x0823308c: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
int DisPatcher_SetPVPSeat::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233096 W _ZN21DisPatcher_SetPVPSeat7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f5eba)（seat/state 两 byte → PvP_Room::OnSetPVPSeatState(…,WALKOUT_TYPE_2)，失败 reset+PutPvp 回池，报告 docs/class_func_reports/DisPatcher_SetPVPSeat/dispatch_sig.md）

DisPatcher_SetPVPReady::DisPatcher_SetPVPReady() {}
DisPatcher_SetPVPReady::~DisPatcher_SetPVPReady() {}
int DisPatcher_SetPVPReady::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c9b9c：get_state(user)==6 → 0，否则 0x13。
    if (user->get_state() == 6)
        return 0;
    return 0x13;
}
int DisPatcher_SetPVPReady::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SET_READY_STATE& req = *reinterpret_cast<MSG_SET_READY_STATE*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))
    {
        return LineFunc(0x1cd3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_SetPVPReady::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081c9bc6：param+0x4 > 0（jle 有符号）→ SendCmdErrorPacket(cmd=0x38, err&0xff)。
    if (0 < *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4))
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x38,
            (unsigned char)(*reinterpret_cast<int*>(
                reinterpret_cast<char*>(&param) + 4) & 0xff));
    }
}
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
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {   // 无房间：LineFunc(0x1ce6, pretty, 0, acc_id)
        return LineFunc(0x1ce6,
                        __PRETTY_FUNCTION__,
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
// ORIG 0x081f6020 T _ZN21DisPatcher_SetPVPTeam12dispatch_sigEP5CUserR9PacketBuf
// 报告：docs/class_func_reports/DisPatcher_SetPVPTeam/dispatch_sig.md（汇编+反编译 C 全读）
int DisPatcher_SetPVPTeam::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 6)
    {
        return 0;
    }
    PacketGuard guard;
    char team;
    if (!packet.get_byte(team))
    {
        return LineFunc(0x82be,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x82c3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (team == 4)
    {
        PvPMapInfo info;
        int mapIdx = room->GetRequestedMapNo();
        CMapList::GetPvpMapInfo(mapIdx, info);
        if (!info.m_playable)
        {
            return LineFunc(0x82ca,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    int result = room->set_pvp_mode(user, (int)(signed char)team);
    if (result < 1)
    {
        room->make_seat_info(reinterpret_cast<char*>(&guard), 0xffff);
        G_GameWorld()->send_all(guard);
        return 0;
    }
    guard.put_header(1, 0x39);
    guard.put_byte(0);
    guard.put_byte(result);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
int DisPatcher_SetPVPTeam::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x082330ee: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
int DisPatcher_SetPVPTeam::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082330f8 W _ZN21DisPatcher_SetPVPTeam7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f6020)（get_byte 队伍号 → set_pvp_mode，失败 make_seat_info(0xffff) 广播，报告 docs/class_func_reports/DisPatcher_SetPVPTeam/dispatch_sig.md）

DisPatcher_DiePvP::DisPatcher_DiePvP() {}
DisPatcher_DiePvP::~DisPatcher_DiePvP() {}
// ORIG 0x081ece64 T _ZN17DisPatcher_DiePvP12dispatch_sigEP5CUserR9PacketBuf
// 报告：docs/class_func_reports/DisPatcher_DiePvP/dispatch_sig.md（汇编+反编译 C 全读）
int DisPatcher_DiePvP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 7)
    {
        return 0;
    }
    unsigned short targetIdx = 0xffff;
    if (!packet.get_short(targetIdx))
    {
        return LineFunc(0x6f12,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CUser* target = G_GameWorld()->find_from_world(targetIdx);
    if (target == 0)
    {
        targetIdx = 0xffff;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x6f1c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PacketGuard guard;
    int result = room->die_charac(user, target);
    int battleMode = room->get_pvp_battle_mode();
    int roomIdx = room->get_index();
    const char* accName = user->get_acc_name();
    const char* charName = user->getCurCharacName();
    CSwitchLog trace(
        __PRETTY_FUNCTION__,
        0x6f22, 0, 0);
    trace("pvp@log %s,%s,room(%d),%d,%d,%d",
          accName, charName, roomIdx, battleMode, result, 0x3a);
    if (0 < result)
    {
        guard.put_header(1, 0x3a);
        guard.put_byte(0);
        guard.put_byte(result);
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
}
int DisPatcher_DiePvP::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08232b6a: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
int DisPatcher_DiePvP::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08232b74 W _ZN17DisPatcher_DiePvP7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081ece64)（get_state==7 时读死亡角色 id → PvP_Room::die_charac + CSwitchLog pvp@log + 应答包，报告 docs/class_func_reports/DisPatcher_DiePvP/dispatch_sig.md）

DisPatcher_PvPTimeout::DisPatcher_PvPTimeout() {}
DisPatcher_PvPTimeout::~DisPatcher_PvPTimeout() {}
int DisPatcher_PvPTimeout::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c5ebe：get_state(user)==7 → 0，否则 0x13。
    if (user->get_state() == 7)
        return 0;
    return 0x13;
}
// ORIG 0x081c5d78 T _ZN21DisPatcher_PvPTimeout4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_PvPTimeout/read.md 逐块映射）：循环 i=0..7 共 8 次
// get_int → msg+0xd+i*4；失败 LineFunc(0x14a6, PRETTY, 0, 0)。
int DisPatcher_PvPTimeout::read(PacketBuf& packet, MSG_BASE& msg)
{
    for (int i = 0; i <= 7; ++i)
    {
        if (!packet.get_int(*reinterpret_cast<int*>(
                reinterpret_cast<char*>(&msg) + i * 4 + 0xd)))
        {
            return LineFunc(0x14a6,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
}
void DisPatcher_PvPTimeout::send(CUser* user, ParamBase& param)
{
    stOnPvPTimeOut& out = static_cast<stOnPvPTimeOut&>(param);
    PacketGuard guard;
    if (0 < out.m_timeOutResult)
    {
        guard.put_header(1, 0x3b);
        guard.put_byte(0);
        guard.put_byte(out.m_timeOutResult);
        guard.finalize(true);
        user->Send(guard);
    }
}
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
// ORIG 0x081f88ae T _ZN23DisPatcher_EndPvPResult12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_EndPvPResult/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state!=7 → 0；room==0 → LineFunc 0x8681；set_recv_pvp_end_result_flag 后
// recv_all 为真 → end_pvp_result + make_state_info/send_all + end_pvp + battle_reset；
// 否则 isRematchable&&recv_team 命中时对 m_matching(+0x6e4) vtable+0x80 槽直调
// （room==0 → logFormat 0x86b6），未命中 count==1 时插 end_result 定时器。
int DisPatcher_EndPvPResult::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 7)
    {
        return 0;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x8681, __PRETTY_FUNCTION__, 0, 0);
    }
    room->set_recv_pvp_end_result_flag(user);
    if (room->recv_pvp_end_result_all() != 0)
    {
        room->end_pvp_result();
        PacketGuard guard;
        room->make_state_info(reinterpret_cast<char*>(&guard));
        G_GameWorld()->send_all(guard);
        room->end_pvp();
        room->battle_reset();
        return 0;
    }
    if (room->isRematchable() != 0 &&
        room->recv_pvp_end_result_team(user) != 0)
    {
        if (room->m_matching == 0)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                  __PRETTY_FUNCTION__, 0x86b6,
                                  "error end pvp result team");
        }
        else
        {
            // ORIG vtable+0x80 槽直调（与 PvP_Room.cpp 同款 m_matching 槽直调口径）
            (*(int (**)(void*, void*, void*))(*(char**)room->m_matching + 0x80))(
                (void*)room->m_matching, room, user);
        }
        return 0;
    }
    if (room->get_recv_pvp_end_result_count() == 1)
    {
        room->insert_timer_recv_pvp_end_result();
    }
    return 0;
}

int DisPatcher_EndPvPResult::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x0823333a: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
int DisPatcher_EndPvPResult::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233344 W _ZN23DisPatcher_EndPvPResult7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f88ae)（set_recv_pvp_end_result_flag → recv_pvp_end_result_all 全齐则 end_pvp_result/make_state_info 广播/end_pvp/battle_reset，报告 docs/class_func_reports/DisPatcher_EndPvPResult/dispatch_sig.md）

DisPatcher_ResPvpRank::DisPatcher_ResPvpRank() {}
DisPatcher_ResPvpRank::~DisPatcher_ResPvpRank() {}
bool DisPatcher_ResPvpRank::ExtractPacket(MSG_RES_PVP_RANK& msg, PacketBuf& packet)
{
    // ORIG 0x081f039e T（报告 DisPatcher_ResPvpRank/ExtractPacket.md 全文转写）：
    // PvpResultType::Clear → 11×get_short（短读后符号扩展写回 int 字段）→ 10×get_int。
    msg.m_result.Clear();
    // 字段名按 PvpResultType.h 权威版（HdrFieldG ODR 收敛后；视图偏移 = PvpResultType 内偏移）。
    short s[11];
    packet.get_short(s[0]);
    packet.get_short(s[1]);
    packet.get_short(s[2]);
    packet.get_short(s[3]);
    packet.get_short(s[4]);
    packet.get_short(s[5]);
    packet.get_short(s[6]);
    packet.get_short(s[7]);
    packet.get_short(s[8]);
    packet.get_short(s[9]);
    packet.get_short(s[10]);             // ORIG 无失败分支（恒返 1）
    packet.get_int(msg.m_result.m_avg[11]);     // +0x44
    packet.get_int(msg.m_result.m_field6c);     // +0x6c
    packet.get_int(msg.m_result.m_field70);     // +0x70
    packet.get_int(msg.m_result.m_field74);     // +0x74
    packet.get_int(msg.m_result.m_field78);     // +0x78
    packet.get_int(msg.m_result.m_avg[12]);     // +0x48
    packet.get_int(msg.m_result.m_count);       // +0x4c
    packet.get_int(msg.m_result.m_winPoint);    // +0x50
    packet.get_int(msg.m_result.m_field7c);     // +0x7c
    packet.get_int(msg.m_result.m_field80);     // +0x80
    msg.m_result.m_avg[0] = s[0];    // +0x18
    msg.m_result.m_avg[1] = s[1];    // +0x1c
    msg.m_result.m_avg[2] = s[2];    // +0x20
    msg.m_result.m_avg[3] = s[3];    // +0x24
    msg.m_result.m_avg[8] = s[4];    // +0x38
    msg.m_result.m_avg[9] = s[5];    // +0x3c
    msg.m_result.m_avg[6] = s[6];    // +0x30
    msg.m_result.m_avg[7] = s[7];    // +0x34
    msg.m_result.m_avg[4] = s[8];    // +0x28
    msg.m_result.m_avg[5] = s[9];    // +0x2c
    msg.m_result.m_avg[10] = s[10];  // +0x40
    return true;
}

bool DisPatcher_ResPvpRank::SavePvpRank(CUser* user, MSG_RES_PVP_RANK& msg)
{
    // ORIG 0x081f05ea T（报告 DisPatcher_ResPvpRank/SavePvpRank.md 全文转写）：
    // 分数(0x89=视图+0x7c)/时间(0x8d=视图+0x80) 反作弊 → job 3/4-grow2 折算 → update_pvp_rank
    // → PVPRoom SetPVPResult/recv 链。
    unsigned int score = (unsigned int)msg.m_result.m_field7c;   // msg+0x89（视图 +0x7c）
    unsigned int playSec = (unsigned int)msg.m_result.m_field80 / 1000;  // msg+0x8d（视图 +0x80）
    if (10000 < score)
    {
        unsigned int over = score / 10000;
        score = score % 10000;
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x76, 1, (int)over, (int)playSec);
    }
    bool reduced;
    if (user->get_charac_job() == 3)
    {
        reduced = true;
    }
    else if (user->get_charac_job() == 4 && user->getCurCharFirstGrowType() == 2)
    {
        reduced = true;
    }
    else
    {
        reduced = false;
    }
    // ORIG _DAT_08bd6320=0.5（job3）/ _DAT_08bd6328=0.8（其余）双精度折算（ROUND 语义）
    const double coeff = reduced ? 0.5 : 0.8;
    unsigned int adjusted = (unsigned int)(coeff * (double)score + 0.5);
    if (playSec < adjusted)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x73, 1, (int)adjusted, (int)playSec);
    }
    if (!user->update_pvp_rank(msg.m_result))
    {
        return false;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return false;
    }
    int seat = room->get_user_seat(user);
    room->SetPVPResult(seat, msg.m_result);
    if (!room->set_recv_pvp_rank_flag(user))
    {
        return false;
    }
    if (!room->recv_pvp_rank_all())
    {
        if (room->get_recv_pvp_rank_count() == 1)
        {
            room->insert_timer_recv_pvp_rank();
        }
    }
    else
    {
        room->send_pvp_end();
    }
    return true;
}

int DisPatcher_ResPvpRank::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081f0282 T（报告 DisPatcher_ResPvpRank/dispatch_sig.md 全文转写）。
    if (user->get_state() != 7)
    {
        return 0;
    }
    MSG_RES_PVP_RANK msg;
    if (!ExtractPacket(msg, packet))
    {
        return LineFunc(0x7736, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!SavePvpRank(user, msg))
    {
        return LineFunc(0x7739, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
int DisPatcher_ResPvpRank::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08232e8e: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
// ORIG 0x08232e98 W _ZN21DisPatcher_ResPvpRank7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现（objdump 核实），全部逻辑位于 dispatch_sig(0x081f0282)：state==7 → MSG_RES_PVP_RANK ctor → ExtractPacket(0x081f039e)/SavePvpRank(0x081f05ea) + LineFunc 断言
int DisPatcher_ResPvpRank::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_SetPVPMapIndex::DisPatcher_SetPVPMapIndex() {}
DisPatcher_SetPVPMapIndex::~DisPatcher_SetPVPMapIndex() {}
// ORIG 0x081f8602 T _ZN25DisPatcher_SetPVPMapIndex12dispatch_sigEP5CUserR9PacketBuf
// 报告：docs/class_func_reports/DisPatcher_SetPVPMapIndex/dispatch_sig.md（汇编+反编译 C 全读）
int DisPatcher_SetPVPMapIndex::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 6)
    {
        return 0;
    }
    bool flag;
    PacketGuard guard;
    short mapIdx;
    if (!packet.get_short(mapIdx))
    {
        return LineFunc(0x8650,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x8653,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    flag = false;
    if (!room->CheckPvPMapMode(user, mapIdx, flag))
    {
        guard.put_header(1, 0x3e);
        guard.put_byte(0);
        guard.put_int(0x13);
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    room->SetRequestedMapNo(mapIdx);
    room->make_state_info(reinterpret_cast<char*>(&guard));
    G_GameWorld()->send_all(guard);
    if (flag != true && room->get_pvp_battle_mode() == 4)
    {
        int res = room->set_pvp_mode(user, 5);
        if (0 < res)
        {
            guard.put_header(1, 0x3e);
            guard.put_byte(0);
            guard.put_byte(0x13);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
        room->make_seat_info(reinterpret_cast<char*>(&guard), 0xffff);
        G_GameWorld()->send_all(guard);
    }
    return 0;
}
int DisPatcher_SetPVPMapIndex::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x082332d8: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
int DisPatcher_SetPVPMapIndex::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082332e2 W _ZN25DisPatcher_SetPVPMapIndex7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f8602)（get_short 地图号 → CheckPvPMapMode/SetRequestedMapNo/make_state_info 广播，报告 docs/class_func_reports/DisPatcher_SetPVPMapIndex/dispatch_sig.md）

DisPatcher_DebugCommand::DisPatcher_DebugCommand() {}
DisPatcher_DebugCommand::~DisPatcher_DebugCommand() {}
int DisPatcher_DebugCommand::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x082333fe: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}

// ORIG 0x08233408 W _ZN23DisPatcher_DebugCommand7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部 GM 命令逻辑（GetCurrentGmMode/IsGm 门禁 → get_packet(0) 取 MSG_DEBUG_COMMAND →
// 0x00..0x30 switch 分派 _debugCommand* 家族 → SendCmdPacket(0x41) 回显）内嵌于
// dispatch_sig(0x081fc4de，报告 docs/class_func_reports/DisPatcher_DebugCommand/dispatch_sig.md；
// 各 _debugCommand*.md 同目录)。

int DisPatcher_DebugCommand::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }

DisPatcher_AddFriend::DisPatcher_AddFriend() {}
DisPatcher_AddFriend::~DisPatcher_AddFriend() {}
// ORIG 0x081cb274 T _ZN20DisPatcher_AddFriend11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_AddFriend/check_error.md 逐块映射）：state<=2 → 0x7fffffff；
// (msg+0x11) 指针空或与自身角色名 strcmp 相同 → 1；否则 0。
// 注：ORIG 汇编对 (msg+0x11) 地址值做 test/jne（非解引用检查）。
int DisPatcher_AddFriend::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() <= 2)
    {
        return 0x7fffffff;
    }
    const char* reqName = reinterpret_cast<const char*>(&msg) + 0x11;
    if (reqName == 0)
    {
        return 1;
    }
    if (strcmp(reqName, user->getCurCharacName()) == 0)
    {
        return 1;
    }
    return 0;
}
int DisPatcher_AddFriend::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = reinterpret_cast<char*>(&msg);   // ORIG -0xc(%ebp)
    if (!packet.get_int(*reinterpret_cast<int*>(pMsg + 0xd)))
    {
        return LineFunc(0x207b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(pMsg + 0x11, 0x1e, *reinterpret_cast<int*>(pMsg + 0xd)))
    {
        return LineFunc(0x2082,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

void DisPatcher_AddFriend::send(CUser* user, ParamBase& param)
{
    stAddFriendParam& p = *reinterpret_cast<stAddFriendParam*>(&param);
    if (p.m_field4 != 0x7fffffff && p.m_field4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x3f, (unsigned char)(p.m_field4 & 0xff));
    }
}
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
int DisPatcher_RemoveFriend::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() <= 2)
    {
        return 0x7fffffff;
    }
    if ((char*)&msg + 0x11 == 0)
    {
        return 1;
    }
    return 0;
}
int DisPatcher_RemoveFriend::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = reinterpret_cast<char*>(&msg);   // ORIG -0xc(%ebp)
    if (!packet.get_int(*reinterpret_cast<int*>(pMsg + 0xd)))
    {
        return LineFunc(0x2115,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(pMsg + 0x11, 0x1e, *reinterpret_cast<int*>(pMsg + 0xd)))
    {
        return LineFunc(0x211c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void DisPatcher_RemoveFriend::send(CUser* user, ParamBase& param)
{
    stRemoveFriendParam& p = *reinterpret_cast<stRemoveFriendParam*>(&param);
    if (p.m_errNo != 0x7fffffff && p.m_errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x40, (unsigned char)(p.m_errNo & 0xff));
    }
}
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
int DisPatcher_Cera::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081fd360：s_pIPGHelper 虚表 +0x2c（slot10 IPGOutputConfirm(SIPGData*,SIPGData*)）
    // 第一实参=user，第二实参为栈上未初始化（ORIG 未写 0x8(%esp)）；随后 LoadMileage(user)。
    WongWork::IPG::SIPGData* res;   // ORIG 0x8(%esp) 未初始化
    GlobalData::s_pIPGHelper->IPGOutputConfirm(
        reinterpret_cast<WongWork::IPG::SIPGData*>(user), res);
    user->LoadMileage();
    return 0;
}
int DisPatcher_Cera::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x082334a4：恒定返回 0
}
int DisPatcher_Cera::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082334ae W _ZN15DisPatcher_Cera7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081fd360)（IPGHelper 校验 + LoadMileage）

DisPatcher_EnterWarRoom::DisPatcher_EnterWarRoom() {}
DisPatcher_EnterWarRoom::~DisPatcher_EnterWarRoom() {}
int DisPatcher_EnterWarRoom::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)
{
    // ORIG 0x0820229e：纯 LineFunc 占位（报告 DisPatcher_EnterWarRoom/dispatch_sig.md）。
    return LineFunc(0x97fb, __PRETTY_FUNCTION__, 0, 0);
}
int DisPatcher_EnterWarRoom::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x0823360c：恒定返回 0
}
int DisPatcher_EnterWarRoom::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233616 W _ZN23DisPatcher_EnterWarRoom7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现；ORIG dispatch_sig(0x0820229e) 亦仅为 LineFunc(0x97fb) 占位，无业务逻辑（报告 docs/class_func_reports/DisPatcher_EnterWarRoom/dispatch_sig.md）

DisPatcher_SetWarRoomSeatState::DisPatcher_SetWarRoomSeatState() {}
DisPatcher_SetWarRoomSeatState::~DisPatcher_SetWarRoomSeatState() {}
int DisPatcher_SetWarRoomSeatState::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)
{
    // ORIG 0x082022ca：纯 LineFunc 占位（报告 DisPatcher_SetWarRoomSeatState/dispatch_sig.md）。
    return LineFunc(0x9848, __PRETTY_FUNCTION__, 0, 0);
}
int DisPatcher_SetWarRoomSeatState::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08233620：恒定返回 0
}
int DisPatcher_SetWarRoomSeatState::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x0823362a W _ZN30DisPatcher_SetWarRoomSeatState7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现；ORIG dispatch_sig(0x082022ca) 亦仅为 LineFunc(0x9848) 占位，无业务逻辑（报告 docs/class_func_reports/DisPatcher_SetWarRoomSeatState/dispatch_sig.md）

DisPatcher_DieWarRoomCharacter::DisPatcher_DieWarRoomCharacter() {}
DisPatcher_DieWarRoomCharacter::~DisPatcher_DieWarRoomCharacter() {}
int DisPatcher_DieWarRoomCharacter::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)
{
    // ORIG 0x082022f6：纯 LineFunc 占位（报告 DisPatcher_DieWarRoomCharacter/dispatch_sig.md）。
    return LineFunc(0x9867, __PRETTY_FUNCTION__, 0, 0);
}
int DisPatcher_DieWarRoomCharacter::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08233634：恒定返回 0
}
int DisPatcher_DieWarRoomCharacter::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x0823363e W _ZN30DisPatcher_DieWarRoomCharacter7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现；ORIG dispatch_sig(0x082022f6) 亦仅为 LineFunc(0x9867) 占位，无业务逻辑（报告 docs/class_func_reports/DisPatcher_DieWarRoomCharacter/dispatch_sig.md）

DisPatcher_StartWarRoomTemp::DisPatcher_StartWarRoomTemp() {}
DisPatcher_StartWarRoomTemp::~DisPatcher_StartWarRoomTemp() {}
int DisPatcher_StartWarRoomTemp::dispatch_sig(CUser* /*user*/, PacketBuf& /*packet*/)
{
    // ORIG 0x08201b06：纯 LineFunc 占位（报告 DisPatcher_StartWarRoomTemp/dispatch_sig.md）。
    return LineFunc(0x9737, __PRETTY_FUNCTION__, 0, 0);
}
int DisPatcher_StartWarRoomTemp::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x082335e4：恒定返回 0
}
int DisPatcher_StartWarRoomTemp::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082335ee W _ZN27DisPatcher_StartWarRoomTemp7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现；ORIG dispatch_sig(0x08201b06) 亦仅为 LineFunc(0x9737) 占位，无业务逻辑（报告 docs/class_func_reports/DisPatcher_StartWarRoomTemp/dispatch_sig.md）

DisPatcher_ReportClient4Hack::DisPatcher_ReportClient4Hack() {}
DisPatcher_ReportClient4Hack::~DisPatcher_ReportClient4Hack() {}
// ORIG 0x082041a0 T _ZN28DisPatcher_ReportClient4Hack12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_ReportClient4Hack/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=1 或 getCurCharacR()==0 → return 0。state==6 → 仅 analyzeHackReport。
// state==5 组队校验（GetParty==0 → LineFunc 0x9bb7）；state==7 PVP 校验
// （GetPvp==0 → LineFunc 0x9bc1）；state==8 战场校验（GetWarRoom==0 → LineFunc 0x9bc5）；
// 校验通过同 6 → analyzeHackReport；其余 state → beginCollect/analyze/report/reset 全链。
int DisPatcher_ReportClient4Hack::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 1 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    int state = user->get_state();
    if (state != 6)
    {
        if (state < 7)
        {
            if (state == 5)
            {
                if (user->GetParty() == 0)
                {
                    return LineFunc(0x9bb7, __PRETTY_FUNCTION__, 0, 0);
                }
            }
        }
        else if (state == 7)
        {
            if (G_CGameManager()->GetPvp((int)user->GetPvpIndex(), user, 0) == 0)
            {
                return LineFunc(0x9bc1, __PRETTY_FUNCTION__, 0, 0);
            }
        }
        else if (state == 8)
        {
            if (user->GetWarRoom() == 0)
            {
                return LineFunc(0x9bc5, __PRETTY_FUNCTION__, 0, 0);
            }
        }
        else
        {
            user->getHackAnalyzer()->beginCollectHackInfo(user);
            user->getHackAnalyzer()->analyzeHackReport(user, &packet);
            user->getHackAnalyzer()->reportHackInfo();
            user->getHackAnalyzer()->resetHackInfo();
            return 0;
        }
    }
    user->getHackAnalyzer()->analyzeHackReport(user, &packet);
    return 0;
}
int DisPatcher_ReportClient4Hack::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08233738：恒定返回 0
}
int DisPatcher_ReportClient4Hack::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233742 W _ZN28DisPatcher_ReportClient4Hack7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x082041a0，见 docs/class_func_reports/DisPatcher_ReportClient4Hack/dispatch_sig.md)

// ---------------------------------------------------------------------------
// ST_Guild_War_Rank_Info / Packet_Notice_Guild_War_Info 生命周期（StubS3C）。
// ST ctor：ORIG W 0x0822ba9a —— uint(+0)=0、uint(+4)=0、ushort(+8)=0、
// memset(+0xa,0,0x17)。Packet_Notice ctor：ORIG W 0x0822badc ——
// PacketHeader(0x414,0x15c) 后对 +0x12 起 9 个元素（stride 0x21）逐个构造。
// ---------------------------------------------------------------------------
ST_Guild_War_Rank_Info::ST_Guild_War_Rank_Info()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    memset(m_pad0a, 0, sizeof(m_pad0a));
}

Packet_Notice_Guild_War_Info::Packet_Notice_Guild_War_Info()
    : PacketHeader(0x414, 0x15c)
{
    char* base = reinterpret_cast<char*>(this) + 0x12;
    for (int i = 0; i < 9; ++i)
    {
        new (base + i * 0x21) ST_Guild_War_Rank_Info();
    }
}

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
int DisPatcher_RequestGuildWarInfo::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x0823374c：恒定返回 0
}
int DisPatcher_RequestGuildWarInfo::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233756 W _ZN30DisPatcher_RequestGuildWarInfo7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x0820439a，见 docs/class_func_reports/DisPatcher_RequestGuildWarInfo/dispatch_sig.md)

DisPatcher_PvpHeartBeat::DisPatcher_PvpHeartBeat() {}
DisPatcher_PvpHeartBeat::~DisPatcher_PvpHeartBeat() {}
int DisPatcher_PvpHeartBeat::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08233760：恒定返回 0
}
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
// ORIG 0x0820b802 T _ZN26DisPatcher_MultiBoxLottery12dispatch_sigEP5CUserR9PacketBuf
// 报告：docs/class_func_reports/DisPatcher_MultiBoxLottery/dispatch_sig.md（汇编+反编译 C 全读）
int DisPatcher_MultiBoxLottery::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3)
    {
        return 0;
    }
    if (user->CheckInTrade())
    {
        return 0;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 0x12))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd3, (unsigned char)0xd1);
        return 0;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x18);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd3, (unsigned char)secu);
        return 0;
    }
    char req[0x11];   // ORIG -0x21 局部消息视图（字段 +0xd/+0xf 两步 lea+add 寻址）
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req + 0xd)))
    {
        return LineFunc(0xa838,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req + 0xf)))
    {
        return LineFunc(0xa839,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int result = user->multibox_lottery_item(
        (int)*reinterpret_cast<unsigned short*>(req + 0xd),
        (int)*reinterpret_cast<unsigned short*>(req + 0xf));
    if (result <= 0)
    {
        return 0;
    }
    PacketGuard guard;
    guard.put_header(1, 0xd3);
    guard.put_byte(0);
    guard.put_byte(result);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
int DisPatcher_MultiBoxLottery::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08233bd2：恒定返回 0
}
int DisPatcher_MultiBoxLottery::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x08233bdc W _ZN26DisPatcher_MultiBoxLottery7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x0820b802，见 docs/class_func_reports/DisPatcher_MultiBoxLottery/dispatch_sig.md)

DisPatcher_UdpCharacteristic::DisPatcher_UdpCharacteristic() {}
DisPatcher_UdpCharacteristic::~DisPatcher_UdpCharacteristic() {}
int DisPatcher_UdpCharacteristic::dispatch_sig(CUser* user, PacketBuf& packet)
{
    unsigned short local_e = 0;
    unsigned char local_f = 0;
    if (packet.get_short(local_e) != true)
    {
        return LineFunc(0xce12,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_byte(local_f) != true)
    {
        return LineFunc(0xce14,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (local_e == 1)
    {
        unsigned short local_26 = 0;
        unsigned short local_24 = 0;
        if (packet.get_short(local_26) != true)
        {
            return LineFunc(0xce30,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(local_24) != true)
        {
            return LineFunc(0xce31,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (local_f == '\0')
        {
            partyPingStat(user, local_26, local_24);
        }
        else if (local_f == '\x01')
        {
            pvpPingStat(user, local_26, local_24);
        }
        else
        {
            if (local_f != '\x02')
            {
                return LineFunc(0xce44,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            fairPvpPingStat(user, local_26, local_24);
        }
    }
    else if (local_e == 2)
    {
        Packet_Abnormal_Exit_Statistic local_30;
        unsigned short tmp_succClear; unsigned short tmp_totalClear;   // packed 字段不可绑引用，经临时读取
        if (packet.get_short(tmp_succClear) != true)
        {
            return LineFunc(0xce68,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(tmp_totalClear) != true)
        {
            return LineFunc(0xce69,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        local_30.m_successDungeonClear = tmp_succClear;
        local_30.m_totalDungeonClear = static_cast<unsigned short>(tmp_totalClear + tmp_succClear);
        ENUM_SERVER_GROUP uVar2 = user->GetServerGroup();
        CStatisticServerProxy* pCVar3 = GlobalData::s_statistic_proxy_mgr->GetServerProxy(uVar2);
        pCVar3->SendPacket(reinterpret_cast<char*>(&local_30), 0xe);
    }
    else
    {
        if (local_e != 0)
        {
            return LineFunc(0xce76,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        Packet_Party_Result_Statistic local_1d;
        unsigned short tmp_succTry; unsigned short tmp_totalTry;   // packed 字段不可绑引用，经临时读取
        if (packet.get_short(tmp_succTry) != true)
        {
            return LineFunc(0xce1d,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(tmp_totalTry) != true)
        {
            return LineFunc(0xce1e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        local_1d.m_successPartyTry = tmp_succTry;
        local_1d.m_totalPartyTry = static_cast<unsigned short>(tmp_totalTry + tmp_succTry);
        ENUM_SERVER_GROUP uVar2 = user->GetServerGroup();
        CStatisticServerProxy* pCVar3 = GlobalData::s_statistic_proxy_mgr->GetServerProxy(uVar2);
        pCVar3->SendPacket(reinterpret_cast<char*>(&local_1d), 0xe);
    }
    return 0;
}
int DisPatcher_UdpCharacteristic::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08233d62：恒定返回 0
}
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
int DIspatcher_OneDayLetheTicket::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08233d76：恒定返回 0
}
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
int Dispatcer_polic_one_to_one_chat_disable::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x0823429e：恒定返回 0
}
int Dispatcer_polic_one_to_one_chat_disable::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082342a8 W _ZN39Dispatcer_polic_one_to_one_chat_disable7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现（objdump 0x082342a8..0x082342bc 核实），全部逻辑位于 dispatch_sig(0x082272ce：state>2 且当前角色存在 → 组 Packet_Disable_User_OneToOneChat_Police(get_int+get_str) → isGMUser 时经 s_monitor_proxy_mgr SendTcpPacket(0x30)，报告 docs/class_func_reports/Dispatcer_polic_one_to_one_chat_disable/dispatch_sig.md)

DisPatcher_CheckSecurityProtection::DisPatcher_CheckSecurityProtection() {}
DisPatcher_CheckSecurityProtection::~DisPatcher_CheckSecurityProtection() {}
int DisPatcher_CheckSecurityProtection::check_error(CUser* /*user*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08264912：恒定返回 0
}
int DisPatcher_CheckSecurityProtection::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x08264810：恒定返回 0
}
void DisPatcher_CheckSecurityProtection::send(CUser* user, ParamBase& param)
{
    CheckSecurityProtection& out = static_cast<CheckSecurityProtection&>(param);
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x176);
    guard.put_byte(0);
    guard.put_byte(out.m_4);
    guard.finalize(true);
    user->Send(guard);
}

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
int DisPatcher_EventDungeon_DestoryObject::check_error(CUser* /*user*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x0812bd82：恒定返回 0
}
int DisPatcher_EventDungeon_DestoryObject::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x0812bd54：pBuf 局部落栈（-0xc）后 get_binary(pBuf, 0x18)，恒返 0。
    char* pBuf = reinterpret_cast<char*>(&msg);   // ORIG -0xc(%ebp)
    packet.get_binary(pBuf, 0x18);
    return 0;
}
void DisPatcher_EventDungeon_DestoryObject::send(CUser* user, ParamBase& param)
{
    ParamBase* pParam = &param;   // ORIG -0xc(%ebp)（local_10，先于 guard ctor 落栈）
    PacketGuard guard;
    guard.put_header(1, 0x21b);
    guard.put_byte(1);
    guard.put_binary(reinterpret_cast<char*>(pParam), 6);
    guard.finalize(true);
    user->Send(guard);
}
// ORIG 0x0812bd8c T _ZN37DisPatcher_EventDungeon_DestoryObject7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_DestoryObject/process.md 全文转写：
//   局部引用槽 -0x8=msg、-0x4=param；*(u16*)(param+0x4)=*(u16*)(msg+0xe)；check_error；返回 0。
//   模板实例化中 ParamBase 栈对象仅 4 字节 vptr（ORIG dispatch_template 0x824aa8c 实证：
//   param@-0x1d 与 msg@-0x19 相邻），param+0x4 写入与相邻 msg 存储重叠为 ORIG 原始行为，忠实转写。）
int DisPatcher_EventDungeon_DestoryObject::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x0812bd8c：组件视图单指令 disp 访存（P1 范式），
    // 写入序 +0x4←+0xe（m_point ← m_roomIndex）。
    CMDPacketStruct::_STReqEventDungeonDestoryObject& rMsg =
        static_cast<CMDPacketStruct::_STReqEventDungeonDestoryObject&>(msg);   // ORIG -0x8(%ebp)
    CMDPacketStruct::_STResEventDungeonDestoryObject& rParam =
        static_cast<CMDPacketStruct::_STResEventDungeonDestoryObject&>(param);  // ORIG -0x4(%ebp)
    rParam.m_point = rMsg.m_roomIndex;
    check_error(user, msg);
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
//   后写 vptr=0x8b66068。基类实参已按 ORIG mangled 统一（OrigPktDisp 批），
//   空体 ctor 由编译器生成基类构造+vptr 写入，与 ORIG 逐指令等价。

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
    // ORIG 0x0812beba：组件视图单指令 disp 访存（P1 范式），
    // 写入序 +0x4←+0xe、+0x8←+0x12、+0x6←+0x10（roomIndex/monsterCount/monsterIndex）。
    CMDPacketStruct::_STReqEventDungeonClearRoom& rMsg =
        static_cast<CMDPacketStruct::_STReqEventDungeonClearRoom&>(msg);    // ORIG -0x8(%ebp)
    CMDPacketStruct::_STResEventDungeonClearRoom& rParam =
        static_cast<CMDPacketStruct::_STResEventDungeonClearRoom&>(param);  // ORIG -0x4(%ebp)
    rParam.m_roomIndex = rMsg.m_roomIndex;
    rParam.m_monsterCount = rMsg.m_monsterCount;
    rParam.m_monsterIndex = rMsg.m_monsterIndex;
    check_error(user, msg);
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
int Dispather_RegenerationRandomOption::check_state(CUser* user, PacketBuf& /*packet*/)
{
    // 报告缺失/推断：ORIG 0x08232984 W（nm+objdump 全文转写，无独立 md 报告）。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x8a0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->CheckInTrade())
    {
        return LineFunc(0x8a4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
int Dispather_RegenerationRandomOption::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_REGENERATION_RANDOM_OPTION& req =
        *reinterpret_cast<MSG_REGENERATION_RANDOM_OPTION*>(&msg);
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_equipSlot0)))
    {
        return LineFunc(0x40d3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_equipSlot1)))
    {
        return LineFunc(0x40d6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_srcOptIndex0))
    {
        return LineFunc(0x40da,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_srcOptIndex1))
    {
        return LineFunc(0x40dd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
void Dispather_RegenerationRandomOption::send(CUser* user, ParamBase& param)
{
    stRegenROIResult& out = static_cast<stRegenROIResult&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x197);
    if (out.m_errNo == 0)
    {
        guard.put_byte(1);
        guard.put_short(out.m_insertedSlot);
    }
    else
    {
        guard.put_byte(0);
        guard.put_int(out.m_errNo);
    }
    guard.finalize(true);
    user->Send(guard);
}

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
int DispatcherResetRandomOption::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_RESET_RANDOM_OPTION& req = *reinterpret_cast<MSG_RESET_RANDOM_OPTION*>(&msg);
    if (!packet.get_short(req.m_equipSlot))
    {
        return LineFunc(0x4db2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_scrollSlot))
    {
        return LineFunc(0x4db5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_rarityChoice))
    {
        return LineFunc(0x4db8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

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


// ---- 本段局部声明（符号定义在别处；链接解析）----
// ORIG T 0x0843081c _ZN24DB_UpdateAvatarJewelSlot11makeRequestEjjPK20stAvatarEmblemInfo_t
class DB_UpdateAvatarJewelSlot
{
public:
    static void makeRequest(unsigned int characNo, unsigned int avatarKey,
                            const stAvatarEmblemInfo_t* emblem);
};

// ORIG W 0x0822dd2c _ZN24MSG_ADD_SOCKET_TO_AVATARC1Ev（21B）：三字段清零，
// 声明 PacketTypes.h:1350（本 TU 提供唯一定义）。
MSG_ADD_SOCKET_TO_AVATAR::MSG_ADD_SOCKET_TO_AVATAR()
{
    m_slotA = 0;
    m_addInfo = 0;
    m_slotB = 0;
}

// ============================================================================
// 第二批（DispatcherRest7）：Dispatcher_AddSocketToAvatar::dispatch_sig
// ORIG T 0x0821a412（报告 docs/class_func_reports/Dispatcher_AddSocketToAvatar/
// dispatch_sig.md 全文已读：## 汇编 + ## 反编译 C）。
// 三字段 wire 读取 → 双槽校验 → 宝石加孔主链。
// 需 Dispatchers.h 中类声明（与 *_impl.cpp 同口径 include）。
// ============================================================================
#include "Dispatchers.h"         // Dispatcher_AddSocketToAvatar 类声明
#include "Inven_Item.h"          // Inven_Item 完整类型
#include "CEquipItem.h"          // CEquipItem::{check_eType,getAvatarSocket}

int Dispatcher_AddSocketToAvatar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    MSG_ADD_SOCKET_TO_AVATAR msg;
    if (!packet.get_short(msg.m_slotA))
        return LineFunc(0xcd60,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(msg.m_addInfo))
        return LineFunc(0xcd62,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(msg.m_slotB))
        return LineFunc(0xcd64,
                        __PRETTY_FUNCTION__,
                        0, 0);

    CInventory* inven = user->getCurCharacInvenW();
    Inven_Item avatarItem = inven->GetInvenSlot(2, (int)msg.m_slotA);   // space=2 avatar
    if (avatarItem.isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 4);
        return 0;
    }
    if ((int)msg.m_addInfo != avatarItem.m_addInfo)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 4);
        return 0;
    }

    Inven_Item cubeItem;                                              // ORIG ctor + 内联逐字段拷贝
    cubeItem = inven->GetInvenSlot(1, (int)msg.m_slotB);              // space=1 inventory
    if (cubeItem.isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x16);
        return 0;
    }

    CItem* cube = G_CDataManager()->find_item(cubeItem.m_addInfo);
    if (cube == 0 || !cube->is_stackable())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }
    CStackableItem* stackable = static_cast<CStackableItem*>(cube);
    if (!stackable->verifyStackableAction((StackableAction::Type)0x20))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 4);
        return 0;
    }
    if (!avatarItem.isAvatarItemType())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }

    CItem* source = G_CDataManager()->find_item((int)msg.m_addInfo);
    bool okSource = false;                                            // ORIG bVar1
    if (source != 0)
    {
        okSource =
            (source->get_grade() == stackable->get_grade())
            && source->IsAvatarItem();
    }
    if (!okSource)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }
    CEquipItem* equip = static_cast<CEquipItem*>(source);
    if (equip->check_eType('\b'))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }

    std::vector<int>& socketTypes = stackable->m_vec3a0;
    if (socketTypes.empty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x16);
        return 0;
    }
    const int socketType = socketTypes[0];
    bool slotOk = false;                                              // ORIG local_d(-0x9)
    if (socketType == 0)
        slotOk = !equip->IsClearAvatar();
    else if (socketType == 1)
        slotOk = equip->IsClearAvatar();
    else if (socketType == 2)
        slotOk = true;
    if (!slotOk)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }

    // ---- 主路径：avatar key 门禁 → 徽章写槽 → 删宝石 → DB 同步 → 回包 ----
    const int avatarKey = avatarItem.m_addInfo2;                      // ORIG -0x9e 槽
    if (WongWork::CAvatarItemMgr::IsTempKey(avatarKey))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }
    WongWork::CAvatarItemMgr* mgr = inven->GetAvatarItemMgrW();
    if (mgr->isSocketAvatar(avatarKey))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x13);
        return 0;
    }

    stAvatarEmblemInfo_t emblem;
    emblem.init();
    if (source != 0 && source->IsAvatarItem())
        equip->getAvatarSocket(-1, emblem);
    mgr->setEmblemSocketData(avatarKey, emblem);

    if (!inven->delete_item((INVEN_TYPE)1, (int)msg.m_slotB, 1,
                            (eItemDelReason)0x1b, true))
        return 0x11;

    DB_UpdateAvatarJewelSlot::makeRequest(
        (unsigned int)user->getCurCharacNo(), (unsigned int)avatarKey, &emblem);
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1,
                             (int)(unsigned short)msg.m_slotA);

    PacketGuard packetGuard;
    packetGuard.put_header(1, 0xd1);
    packetGuard.put_byte(1);
    packetGuard.put_short((int)(unsigned short)msg.m_slotA);
    packetGuard.put_short((int)(unsigned short)msg.m_slotB);
    packetGuard.finalize(true);
    user->Send(packetGuard);
    return 0;
}


// ============================================================================
// 第二批：DisPatcher_SendMess::dispatch_sig（ORIG T 0x081f3540，896 insn）
// 报告 docs/class_func_reports/DisPatcher_SendMess/dispatch_sig.md 全文已读
// （## 汇编 + ## 反编译 C）。聊天消息总入口：公共头五段 wire 读 → 表情改写 →
// Ghost 门禁 → 按 msgType 分派（社区监视包 0x24/0x25/0x26、monitor 包 type==7、
// 消耗品 0xb/0xc/0x23·0xd/0xf·0xe·0x22）→ GM 存证 DB → send_chat_msg 广播。
// ============================================================================
#include "Packet_Monitor_Other_Channel_Chat.h"   // shared/packet/include 监视包权威头
#include "CCommunityServerProxy.h"              // SendTcpPacket（Community 监视包）
#include "DB_AccountCargoSync.h"                // DB_InsertPoliceSaveChatting::makeRequest
int DisPatcher_SendMess::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3)
        return 0;

    static const char* const kPretty =
        "virtual int DisPatcher_SendMess::dispatch_sig(CUser*, PacketBuf&)";
    char msgType;                                        // ORIG msg+0xd（-0x196）
    unsigned short target;                               // ORIG msg+0xe（-0x195）
    unsigned int recvCharno;                             // ORIG msg+0x10（-0x193）
    int msgLen;                                          // ORIG msg+0x36（-0x16d）
    char msgBuf[0x100];                                  // ORIG acStack_16d
    if (!packet.get_byte(*(char*)&msgType))
        return LineFunc(0x7ef5, kPretty, 0, 0);
    if (!packet.get_short(target))
        return LineFunc(0x7ef6, kPretty, 0, 0);
    if (!packet.get_int(recvCharno))
        return LineFunc(0x7ef7, kPretty, 0, 0);
    if (!packet.get_int(msgLen))
        return LineFunc(0x7ef8, kPretty, 0, 0);
    if (!packet.get_str(msgBuf, 0x100, msgLen))
        return LineFunc(0x7ef9, kPretty, 0, 0);

    G_CDataManager()->rewriteUsableEmoticon(*user, msgBuf);

    const bool isGhost = (bool)(unsigned char)user->IsCurCharacGhost();
    bool bypassGate = !isGhost ||
                      ((unsigned char)msgType < 9 &&
                       (((1u << (msgType & 0x1f)) & 0x144u) != 0));
    if (!bypassGate)
        return 0;

    int srcLen;                                          // ORIG -0x189/-0x193 别名区（昵称长）
    char nickBuf[0x1e];                                  // ORIG acStack_18f
    unsigned char channelFlag = 0;                       // ORIG local_62（-0x5e）
    char nickFlag = 0;                                   // ORIG local_6d（msg+0x13a 尾字节）

    if (msgType == 0x24 || msgType == 0x25 || msgType == 0x26)
    {
        // ---- 社区频道监视包（Community server） ----
        if (!packet.get_int(srcLen))
            return LineFunc(0x7f4b, kPretty, 0, 0);
        if (!packet.get_str(nickBuf, 0x1e, srcLen))
            return LineFunc(0x7f4d, kPretty, 0, 0);
        if (!packet.get_byte(*(char*)&nickFlag))
            return LineFunc(0x7f4f, kPretty, 0, 0);

        Packet_Monitor_Other_Channel_Chat notify;
        if (msgType == 0x24)
            notify.m_chatType = 7;
        else if (msgType == 0x25)
            notify.m_chatType = 0x21;
        else
            notify.m_chatType = 1;
        notify.m_id = user->get_acc_id();
        notify.server_id = nickFlag;
        notify.m_characNo = (unsigned int)user->getCurCharacNo();
        notify.m_recverCharId = (int)recvCharno;
        notify.m_buddyNameLen = (char)((unsigned int)srcLen & 0xff);
        memcpy(notify.buddy_n_user_id_what, nickBuf,
               (size_t)srcLen & 0xffu);
        memcpy(notify.m_msg, msgBuf, (unsigned char)msgLen);
        notify.m_msgLen = (unsigned char)msgLen;

        GlobalData::s_community_proxy->SendTcpPacket(
            reinterpret_cast<char*>(&notify),
            (int)(unsigned short)(notify.m_msgLen + 0x3b));
        return 0;
    }

    if (msgType == 7 || msgType == 1 || msgType == 0x21)
    {
        // ---- 本地频道监视包（monitor server）：type==7 才真正上送 ----
        if (!packet.get_int(srcLen))
            return LineFunc(0x7f71, kPretty, 0, 0);
        if (!packet.get_str(nickBuf, 0x1e, srcLen))
            return LineFunc(0x7f73, kPretty, 0, 0);
        if (!packet.get_byte(channelFlag))
            return LineFunc(0x7f78, kPretty, 0, 0);

        if (msgType == 7)
        {
            Packet_Monitor_Other_Channel_Chat notify;
            notify.m_characNo = (unsigned int)user->getCurCharacNo();
            notify.m_buddyNameLen = (char)((unsigned int)srcLen & 0xff);
            memcpy(notify.buddy_n_user_id_what, nickBuf,
                   (size_t)srcLen & 0xffu);
            memcpy(notify.m_msg, msgBuf, (unsigned char)msgLen);
            notify.m_msgLen = (unsigned char)msgLen;

            std::string filtered(CSyncSlangFilter::GetInstance()->Filter(
                std::string(notify.m_msg, sizeof(notify.m_msg))));
            if (filtered.size() != 0)
            {
                strncpy(notify.m_msg, filtered.c_str(), filtered.size());
                notify.m_msgLen = (unsigned char)(msgLen + 0);   // 长度保持原值口径
                const int total =
                    (int)(unsigned short)(notify.m_msgLen + 0x3b);
                CMonitorServerProxy* proxy =
                    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                        user->GetServerGroup());
                proxy->SendPacket(reinterpret_cast<char*>(&notify), total);
            }
            return 0;
        }
    }
    else if (msgType == 0xb || msgType == 0xc || msgType == 0x23)
    {
        // ---- 聊天消耗品 A：itemIdx∈{0x24,0x382,0x28d150,0x292090dd,0x29209138} ----
        const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target), (int)recvCharno);
        if (item == 0 ||
            (item->m_addInfo != 0x24 && item->m_addInfo != 0x382 &&
             item->m_addInfo != 0x28d150 && item->m_addInfo != 0x292090dd &&
             item->m_addInfo != 0x29209138))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
        int secu = (int)(unsigned char)
            GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x24);
        if (secu != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, (unsigned char)secu);
            return 0;
        }
        if (user->CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target),
                                (int)recvCharno))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0xd5);
            return 0;
        }
        if (!user->use_stackable((short)(unsigned short)recvCharno,
                                 (int)target, -1, -1))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
    }
    else if (msgType == 0xd || msgType == 0xf)
    {
        // ---- 聊天消耗品 B：itemIdx==0x25(d)/0x28992f(f) ----
        const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target), (int)recvCharno);
        if (item == 0)
            return LineFunc(0x8020, kPretty, 0, 0);
        if (!((item->m_addInfo == 0x25 && msgType == 0xd) ||
              (item->m_addInfo == 0x28992f && msgType == 0xf)))
            return LineFunc(0x8025, kPretty, 0, 0);
        int secu = (int)(unsigned char)
            GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x24);
        if (secu != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, (unsigned char)secu);
            return 0;
        }
        if (user->CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target),
                                (int)recvCharno))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0xd5);
            return 0;
        }
        if (!user->use_stackable((short)(unsigned short)recvCharno,
                                 (int)target, -1, -1))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
    }
    else if (msgType == 0xe)
    {
        // ---- 聊天消耗品 C：itemIdx==0x460 且 GM 才可用 ----
        const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target), (int)recvCharno);
        if (item == 0 || item->m_addInfo != 0x460)
            return LineFunc(0x8053, kPretty, 0, 0);
        if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
            return LineFunc(0x8057, kPretty, 0, 0);
        if (!user->use_stackable((short)(unsigned short)recvCharno,
                                 (int)target, -1, -1))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
    }
    else if (msgType == 0x22)
    {
        // ---- 聊天消耗品 D：itemIdx==0x1d9e（state==3 门禁，直接删） ----
        if (user->get_state() != 3)
            return 0;
        const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target), (int)recvCharno);
        if (item == 0 || item->m_addInfo != 0x1d9e)
            return LineFunc(0x806a, kPretty, 0, 0);
        CItem* itemInfo = G_CDataManager()->find_item(item->m_addInfo);
        if (itemInfo == 0 || !itemInfo->is_stackable())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
        CInventory* invW = user->getCurCharacInvenW();
        if (!invW->delete_item((INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target),
                               (int)recvCharno, 1, (eItemDelReason)3, true))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
    }

    // ---- GM 存证 + 全服广播 ----
    if (GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
    {
        DB_InsertPoliceSaveChatting::makeRequest(
            (unsigned int)user->getCurCharacNo(), (char)msgType, msgBuf);
    }
    G_GameWorld()->send_chat_msg(user, (char)msgType, target, recvCharno,
                                 msgBuf, msgLen, nickBuf, srcLen,
                                 (ENUM_NOTIPACKET)0xc);
    return 0;
}


// ============================================================================
// 第二批：Dispatcher_ChangeCharacName::dispatch_sig
// ORIG T 0x08205898（报告 docs/class_func_reports/Dispatcher_ChangeCharacName/
// dispatch_sig.md 全文已读：## 汇编 + ## 反编译 C）。依赖：
//   restrict_inputting_name::isUtf8StrInUnicodeRange / checkValidCharacName →
//   RestrictNameDeps.{h,cpp}（本批新建）；DB_ReqChangeCharacName 权威头。
// ============================================================================
#include "RestrictNameDeps.h"
#include "DB_ReqChangeCharacName.h"
#include "GameTypes.h"            // cMyTrace

int Dispatcher_ChangeCharacName::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 2)
        return LineFunc(0x9eb3,
                        __PRETTY_FUNCTION__,
                        0, 0);

    unsigned char slot = 0;                              // ORIG -0x65
    int newLen = 0;                                      // ORIG -0x6c
    int srcLen = 0;                                      // ORIG -0x70
    char srcName[0x1e] = { '\0' };                       // ORIG -0x8e（P4 零初始化展开）
    char newName[0x1e] = { '\0' };                       // ORIG -0xac
    if (!packet.get_byte(slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    bool srcOk = packet.get_int(srcLen);                 // ORIG local_25
    if (!srcOk)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x9ec7, 0);
        trace("ting : m_id(%s) src char name length(%d), ret(%d)",
              NumberToString(user->get_acc_id(), 0), srcLen, (int)srcOk);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    if (!packet.get_str(srcName, 0x1e, srcLen))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    bool newOk = packet.get_int(newLen);                 // ORIG local_25（第二次）
    if (!newOk)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x9ed4, 0);
        trace("ting : m_id(%s) char name length(%d), ret(%d)",
              NumberToString(user->get_acc_id(), 0), newLen, (int)newOk);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    if (!packet.get_str(newName, 0x1e, newLen))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }
    if (newLen < 4 || 0x12 < newLen)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 1);
        return 0;
    }

    const char* serverName = user->get_charac_name((int)slot);
    if (serverName == 0 || strncmp(srcName, serverName, 0x1d) != 0)
    {
        if (serverName == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x9ef6, 0);
            trace("ting : m_id(%s) srcCharacName2 is null",
                  NumberToString(user->get_acc_id(), 0));
        }
        else
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x9ef8, 0);
            trace("ting : src char name(%s), src server char name(%s)",
                  srcName, serverName);
        }
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
        return 0;
    }

    unsigned char typeFlag = 0;                          // ORIG -0xad
    if (!packet.get_byte(typeFlag))
        return LineFunc(0x9eff,
                        __PRETTY_FUNCTION__,
                        0, 0);

    int nameState;                                       // ORIG -0x1c
    if (typeFlag == 2)
    {
        // ORIG hasPreventPrefixCharacName(src, srcLen)（Ghidra 误抄参数，asm 实证）。
        if (!G_CDataManager()->hasPreventPrefixCharacName(srcName, srcLen))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
            return 0;
        }
        nameState = 2;
    }
    else
    {
        if (G_CDataManager()->hasSpecialCharacter(serverName))
            nameState = 0;
        else if (G_CDataManager()->hasPreventString(serverName) ||
                 G_CDataManager()->hasPreventStringName(serverName))
            nameState = 1;
        else
        {
            if (!user->CheckPossibleRequestRenameState((int)slot))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 7);
                return 0;
            }
            if (!checkValidCharacName(newName))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 0x9f);
                return 0;
            }
            user->SetCharacViewState((int)slot, 1);
            nameState = 3;
        }
    }

    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(newName))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 0x9f);
        return 0;
    }
    if (!checkValidCharacName(newName))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8a, 0x9f);
        return 0;
    }
    DB_ReqChangeCharacName::makeRequest((int)user->GetUID(),
                                        user->get_charac_no((int)slot),
                                        newName, nameState);
    return 0;
}


// ============================================================================
// 第二批：DisPatcher_DebugCommand::dispatch_sig（ORIG T 0x081fc4de，761 insn）
// 报告 docs/class_func_reports/DisPatcher_DebugCommand/dispatch_sig.md 全文已读
// （## 汇编 + ## 反编译 C）。GM 命令总入口：get_packet(0) → cmd/gm-mode 门禁 →
// IsGm 门 → 0x00..0x30 switch 分派（_debugCommand* 家族定义于
// DisPatcher_DebugCommand_cmds.cpp）→ SendCmdPacket(0x41) 回显。
// ============================================================================
#include "CGM_Manager.h"

int DisPatcher_DebugCommand::dispatch_sig(CUser* user, PacketBuf& packet)
{
    static const char* const kPretty =
        "virtual int DisPatcher_DebugCommand::dispatch_sig(CUser*, PacketBuf&)";
    MSG_DEBUG_COMMAND* msg =
        reinterpret_cast<MSG_DEBUG_COMMAND*>(packet.get_packet(0));
    if (msg == 0)
        return 0;

    bool allowed;                                        // ORIG bVar2
    if (msg->m_command != 0)
        allowed = (bool)(unsigned char)GlobalData::s_GM_Manager->GetCurrentGmMode(
            user->get_acc_id());
    else
        allowed = true;
    if (!allowed)
    {
        cMyTrace trace(kPretty, 0x8b6a, 5);
        trace("Illegal GM Command : %d", user->get_acc_id());
        return LineFunc(0x8b6b, kPretty, 0, 0);
    }

    cMyTrace trace1(kPretty, 0x8b65, 0);
    trace1("GM(%d) trying to command(%d)", user->get_acc_id(),
           (int)msg->m_command);

    if ((int)(unsigned int)(size_t)user->getCurCharacR() == 0)
        return LineFunc(0x8b70, kPretty, 0, 0);

    cMyTrace trace2(kPretty, 0x8b72, 0);
    trace2("GM Command : %s", NumberToString(user->get_acc_id(), 0));

    bool result = true;                                  // ORIG local_1e(-0x1a)
    const bool passFlag = false;                         // ORIG local_1d(-0x19) 恒 0
    if ((unsigned char)GlobalData::s_GM_Manager->IsGm(user->get_acc_id()) != 1 ||
        passFlag)
    {
        if (!passFlag)
            return 0;
        cMyTrace trace3(kPretty, 0x8b90, 5);
        trace3("Illegal GM Command : %d", user->get_acc_id());
        return LineFunc(0x8b91, kPretty, 0, 0);
    }
    else
    {
        cMyTrace trace4(kPretty, 0x8bb6, 0);
        trace4("GM Pass Complete! Command(%d)", (int)msg->m_command);
    }

    // raw 字节视图（ORIG 以 byte 口径取 +0xe/+0xf）
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(msg);

    switch (msg->m_command)
    {
    case 0:
    {
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) < GM_LV1)
            return LineFunc(0x8bcb, kPretty, 0, 0);
        GlobalData::s_GM_Manager->TurnGmMode(user->get_acc_id());
        if ((unsigned char)GlobalData::s_GM_Manager->GetCurrentGmMode(
                user->get_acc_id()) == 1)
        {
            user->SetGameMasterMode(true);
            result = _debugCommandOn(user, *msg);
        }
        else
        {
            cMyTrace trace5(kPretty, 0x8bd0, 0);
            trace5("%d:'%s' GM Mode OFF.", user->get_acc_id(),
                   (const char*)user->getCurCharacName());
            result = true;
            msg->m_command = 0x2a;
            result = _debugCommandOff(user, *msg);
        }
        break;
    }
    case 1:
    case 2:
        result = _debugCommandLevelUpDown(user, -1);
        break;
    case 3:
        result = _debugCommandSetGrowType(user, raw[0xe], raw[0xf]);
        break;
    case 4:
        result = _debugCommandClearSkill(user, *msg);
        break;
    case 5:
        result = _debugCommandSetSkill(user, raw[0xe], raw[0xf]);
        break;
    case 6:
        result = _debugCommandAddItem(
            user,
            (unsigned long)*(const unsigned int*)reinterpret_cast<
                const unsigned char*>(raw + 0xe),
            *(const int*)(raw + 0x12));
        break;
    case 7:
        result = _debugCommandSetLevel(user, (int)raw[0xe]);
        break;
    case 8:
        result = _debugCommandPVPRankSet(user, *msg);
        break;
    case 9:
        user->SetGMUpgradeMode((ENUM_GM_ITEM_UPGRADE)1);
        break;
    case 0xa:
        user->SetGMUpgradeMode((ENUM_GM_ITEM_UPGRADE)2);
        break;
    case 0xb:
        user->SetGMUpgradeMode((ENUM_GM_ITEM_UPGRADE)0);
        break;
    case 0xc:
        result = _debugCommandUserRepel(user, *msg);
        break;
    case 0xd:
        G_CGameManager()->WarRoomCountManageTest(msg->m_arg);
        break;
    case 0xe:
        if (!(bool)(unsigned char)
                user->getCurCharacInvenR()->GetCreatureMgrR()
                    ->IsGrowCreature_Equipped_Creature())
            user->getCurCharacInvenW()->GetCreatureMgrW()->GainExp(
                msg->m_arg);
        break;
    case 0xf:
        user->getCurCharacInvenW()->GetCreatureMgrW()->SetStomach(msg->m_arg);
        break;
    case 0x10:
        user->getCurCharacInvenW()->GetCreatureMgrW()->Evolute(0, 0, true);
        break;
    case 0x11:
        result = _debugCommandAddAvatar(user, *msg);
        break;
    case 0x12:
        CBattle_Field::SetEnableHellDungeon(true);
        break;
    case 0x13:
        result = (bool)(unsigned char)GlobalData::s_pGMAccounts_->appendGM(
            user->get_acc_id(), (unsigned int)msg->m_arg);
        break;
    case 0x14:
        GlobalData::s_pGMAccounts_->removeGM(user->get_acc_id(),
                                             (unsigned int)msg->m_arg);
        result = true;   // ORIG 定义为空实现（返回 0 前的 al 未定义口径，恒视作成功）
        break;
    case 0x15:
        result = _debugCommandInvisible(user, *msg);
        break;
    case 0x16:
        result = _debugCommandVisible(user, *msg);
        break;
    case 0x17:
        result = _debugCommandRequestMid(user, *msg);
        break;
    case 0x18:
        user->SetCurCharacLuckPoint(msg->m_arg);
        user->enableSaveCharacStat();
        break;
    case 0x19:
        user->SetAge((unsigned int)msg->m_arg);
        break;
    case 0x1a:
        result = _debugCommandSetEnchant(user, *msg);
        break;
    case 0x1b:
        if ((int)(unsigned int)(size_t)user->getCurCharacR() != 0)
            expert_job::CExpertJob::IncreaseExpertJobExp(user, msg->m_arg);
        break;
    case 0x1c:
        result = _debugCommandSetUserDefine(user, packet);
        break;
    case 0x1d:
        result = _debugCommandGuildLevelUp(user);
        break;
    case 0x1e:
        result = _debugCommandPowerWarStart(user);
        break;
    case 0x1f:
        result = _debugCommandPowerWarEnd(user);
        break;
    case 0x20:
        result = _debugCommandPowerUpOn(user, *msg);
        break;
    case 0x21:
        G_GameWorld()->CancelPowerUp();
        break;
    case 0x22:
        result = _debugCommandUserPowerwarPointUp(user, *msg);
        break;
    case 0x23:
        result = _debugCommandInitUserPowerwarPoint(user, *msg);
        break;
    case 0x24:
        result = _debugCommandPowerwarPointUp(user, *msg);
        break;
    case 0x25:
        result = _debugCommandSetBlackCount(user, *msg);
        break;
    case 0x26:
        result = _debugCommandCargoCapacity(user, *msg);
        break;
    case 0x27:
    {
        CGuildServerProxy* proxy =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(
                user->GetServerGroup());
        proxy->SendIncreaseDecreasePowerWarPoint(
            (unsigned int)(unsigned short)(size_t)user->get_charac_guildkey(),
            (unsigned int)user->get_charac_no(-1), msg->m_arg);
        break;
    }
    case 0x28:
        if (msg->m_arg < 1)
        {
            if (msg->m_arg == 0)
            {
                user->RecoverFatigue(0);
                user->SendFatigue();
            }
        }
        else
        {
            user->FatigueUp(msg->m_arg);
            user->SendFatigue();
        }
        break;
    case 0x29:
        if (0 < msg->m_arg)
        {
            user->getCurCharacInvenW()->UseUpCoin((unsigned int)msg->m_arg);
            user->SendCoin();
        }
        break;
    case 0x2a:
        result = _debugCommandOff(user, *msg);
        break;
    case 0x2b:
        result = _debugCommandClearAvtInven(user, *msg);
        break;
    case 0x2c:
        result = _debugCommandClearInven(user, *msg);
        break;
    case 0x2f:
        if (msg->m_arg != 0)
        {
            CInventory* invW = user->getCurCharacInvenW();
            invW->set_money((unsigned int)msg->m_arg);
            const int curMoney = user->getCurCharacInvenR()->get_money();
            user->m_historyLog.MoneyAdd(curMoney, msg->m_arg,
                                        (eMoneyAddReason)6);
            user->SendUpdateItemList(CUser::eSendTarget_1,
                                     (ENUM_ITEMSPACE)0, 0);
            result = true;
        }
        break;
    case 0x30:
        G_GameWorld()->UpdateMiniCraneSeed();
        break;
    default:
        return LineFunc(0x8da2, kPretty, 0, 0);
    }

    if (result)
    {
        user->SendCmdPacket(CUser::eSendTarget_1, (ENUM_CMDPACKET)0x41,
                            (int)msg->m_command);
        if (msg->m_command == 0)
        {
            user->ReCalcChattingEmoticon();
            user->SendChattingEmoticon();
        }
    }
    return 0;
}


// ============================================================================
// 第二批：DisPatcher_SetIP::dispatch_sig（ORIG T 0x081f468c，381 insn）
// 报告 docs/class_func_reports/DisPatcher_SetIP/dispatch_sig.md 全文已读
// （## 汇编 + ## 反编译 C）。五段 wire 读 → set_nat×2 → SHA 盐摘要 → deny 检查
// → 监视流投递/拒绝包 → DB_SelectAllowProxyUser。依赖：
//   checkDenyIP/cutIPdclass/getSha256UsingSalt → ProxySecurityDeps.{h,cpp}；
//   DB_SelectAllowProxyUser::makeRequest 定义 W8LT_DBTask.cpp。
// ============================================================================
#include "ProxySecurityDeps.h"

// ORIG T 0x0844d2d4 _ZN23DB_SelectAllowProxyUser11makeRequestEij（定义 W8LT_DBTask.cpp）
class DB_SelectAllowProxyUser
{
public:
    static void makeRequest(int userIdx, unsigned int accId);
};

int DisPatcher_SetIP::dispatch_sig(CUser* user, PacketBuf& packet)
{
    static const char* const kPretty =
        "virtual int DisPatcher_SetIP::dispatch_sig(CUser*, PacketBuf&)";
    if (user->get_state() >= 4)
        return 0;

    char natType;                        // ORIG msg+0xd
    unsigned int innerIp;                // msg+0xe
    unsigned int natIp;                  // msg+0x12
    unsigned short natPort;              // msg+0x16
    unsigned int mtu;                    // msg+0x18
    if (!packet.get_byte(natType))
        return LineFunc(0x80e4, kPretty, 0, 0);
    if (!packet.get_int(innerIp))
        return LineFunc(0x80e5, kPretty, 0, 0);
    if (!packet.get_int(natIp))
        return LineFunc(0x80e6, kPretty, 0, 0);
    if (!packet.get_short(natPort))
        return LineFunc(33000, kPretty, 0, 0);
    if (!packet.get_int(mtu))
        return LineFunc(0x80e8, kPretty, 0, 0);
    user->m_network.set_nat(natType, innerIp, natIp, natPort, mtu);

    int macLen;                          // ORIG msg+0x1c
    char macBuf[0x41];                   // ORIG msg+0x20 / local_da
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(&macLen)))
        return LineFunc(0x8100, kPretty, 0, 0);
    if (!packet.get_str(macBuf, 0x41, macLen))
        return LineFunc(0x8102, kPretty, 0, 0);

    char digestHex[0x41];                // ORIG -0x95 零初始化
    memset(digestHex, 0, sizeof(digestHex));
    getSha256UsingSalt(reinterpret_cast<const unsigned char*>(macBuf),
                       0x41, digestHex);

    cMyTrace trace(kPretty, 0x810b, 0);
    trace("(acc_id: %s) (nat type: %d) (inner_ip: %d) (nat_ip: %d) "
          "(nat_port: %d) (mac_addr: %s) (mtu: %d)",
          NumberToString(user->get_acc_id(), 0), (int)(signed char)natType,
          innerIp, natIp, (int)natPort, digestHex, mtu);

    // 第二次 set_nat：以 mac 摘要串写入（ORIG set_nat(...,const char* macAddr)）
    user->m_network.set_nat(natType, innerIp, natIp, natPort, digestHex);

    char peerIp[0x14];                   // ORIG local_44 零初始化
    memset(peerIp, 0, sizeof(peerIp));
    char* ip2 = user->m_network.GetPeerIP2(peerIp, 0x14);
    if (!(bool)(unsigned char)checkDenyIP(user->m_network.get_str_ip(), ip2))
    {
        PacketGuard packetGuard;
        packetGuard.put_header(1, 2);
        packetGuard.put_byte(1);
        packetGuard.finalize(true);
        user->Send(packetGuard);
        return 0;
    }

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "PacketDispatcher_Impl_1.cpp", 0x811d),
                       true);
    guard.operator*()->operator<<((int)0x9e);
    guard.operator*()->operator<<((int)-1);
    SIG_SAVE_PROXY_LOG* sig = guard.GetInBuffer<SIG_SAVE_PROXY_LOG>();
    sig->m_accId = user->get_acc_id();
    strncpy(sig->m_strIp, user->m_network.get_str_ip(), 0x10);
    strncpy(sig->m_peerIp, peerIp, 0x10);
    sig->m_natType = (unsigned char)natType;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);

    DB_SelectAllowProxyUser::makeRequest((int)user->GetUID(),
                                         user->get_acc_id());
    return 0;
}


// ============================================================================
// 第二批：Dispatcher_FrameLagStatistics::dispatch_sig（ORIG T 0x08217046，约 500 insn）
// 报告 docs/class_func_reports/Dispatcher_FrameLagStatistics/dispatch_sig.md 全文已读
// （## 汇编 + ## 反编译 C）。整包 wire 读入 Packet_Frame_Lag_Statistic_Add
// （sizeof 0x186）后按 GetServerGroup 送 statistic proxy。
// ============================================================================
#include "PacketHeader.h"

// Packet_Frame_Lag_Statistic_Add 权威声明落 shared/packet/include（本 TU 提供唯一
// ctor 定义，与 MSG_ADD_SOCKET_TO_AVATAR 同规约）；本体为纯 wire POD（无 getter
// 证据）：各标量域按 asm 取数偏移具名，rows/entries/tail 区为交叠别名寻址，
// 以平坦缓冲 + 显式偏移访问忠实复现（asm 寻址式逐条对照）。
#pragma pack(push, 1)
class Packet_Frame_Lag_Statistic_Add : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Add();
    int m_fieldA;                // +0x0a（首个 get_int）
    char m_field0e;              // +0x0e
    char m_field0f;              // +0x0f
    unsigned char m_pad10[2];    // +0x10 驻留（wire 段间空档）
    unsigned short m_field12;    // +0x12
    unsigned short m_field14;    // +0x14
    short m_field16;             // +0x16
    short m_field18;             // +0x18
    char m_field1a;              // +0x1a
    int m_field1b;               // +0x1b（奇偏移，pack(1)）
    char m_field1f;              // +0x1f
    short m_field20;             // +0x20
    short m_field22;             // +0x22
    unsigned char m_body[0x162]; // +0x24..+0x185 逐行帧统计/tail 区（别名寻址见 dispatch_sig）
};                               // sizeof = 0x186（静态断言随 ctor 定义）
#pragma pack(pop)
static_assert(sizeof(Packet_Frame_Lag_Statistic_Add) == 0x186,
              "Packet_Frame_Lag_Statistic_Add ABI drift (ctor arg 0x186)");

Packet_Frame_Lag_Statistic_Add::Packet_Frame_Lag_Statistic_Add()
    : PacketHeader(0xc26, 0x186)
{
}

int Dispatcher_FrameLagStatistics::dispatch_sig(CUser* user, PacketBuf& packet)
{
    static const char* const kPretty =
        "virtual int Dispatcher_FrameLagStatistics::dispatch_sig(CUser*, PacketBuf&)";
#define FLLF(line) \
    return LineFunc((line), kPretty, 0, 0)

    Packet_Frame_Lag_Statistic_Add pkt;
    char* base = reinterpret_cast<char*>(&pkt);

    if (!packet.get_int(pkt.m_fieldA))
        FLLF(0xc8fd);
    if (!packet.get_byte(pkt.m_field0e))
        FLLF(0xc8fe);
    if (!packet.get_byte(pkt.m_field0f))
        FLLF(0xc8ff);
    if (!packet.get_short(pkt.m_field12))
        FLLF(0xc901);
    if (!packet.get_short(pkt.m_field14))
        FLLF(0xc902);
    if (!packet.get_short(pkt.m_field16))
        FLLF(0xc903);
    if (!packet.get_short(pkt.m_field18))
        FLLF(0xc905);
    if (!packet.get_byte(pkt.m_field1a))
        FLLF(0xc906);
    if (!packet.get_int(pkt.m_field1b))
        FLLF(0xc907);
    if (!packet.get_byte(pkt.m_field1f))
        FLLF(0xc909);
    if (!packet.get_short(pkt.m_field20))
        FLLF(0xc90a);
    if (!packet.get_short(pkt.m_field22))
        FLLF(0xc90b);

    // ---- 行循环（ORIG -0x14）：行内四 short + 6×(int+bin[4]) ----
    for (int r = 0; r <= 5; ++r)
    {
        for (int c = 0; c < 4; ++c)          // asm dst = base + r*0x38 + 0x24 + c*2
        {
            if (!packet.get_short(
                    *reinterpret_cast<short*>(base + r * 0x38 + 0x24 + c * 2)))
                FLLF(0xc914 + c);
        }
        for (int j = 0; j <= 5; ++j)         // ORIG -0x10
        {
            if (!packet.get_int(*reinterpret_cast<int*>(
                    base + ((r * 7 + j + 4) << 3) + 0xc)))
                FLLF(0xc91f);
            if (!packet.get_binary(
                    base + ((r * 7 + j + 4) << 3) + 0x10, 4))
                FLLF(0xc920);
        }
    }

    signed char tailCount = 0;
    if (!packet.get_byte(*reinterpret_cast<char*>(&tailCount)))
        FLLF(0xc925);

    // ---- 尾表（ORIG -0xc）：读侧写 base+0xa+(i+0xb8)*2，缺项清零 base+0xa+i*2 ----
    for (int i = 0; i <= 5; ++i)
    {
        if (i < (int)tailCount)
        {
            if (!packet.get_byte(
                    *reinterpret_cast<char*>(base + 0x170 + i)))
                FLLF(0xc92a);
            if (!packet.get_short(*reinterpret_cast<short*>(
                    base + 0xa + ((i + 0xb8) << 1))))
                FLLF(0xc92b);
        }
        else
        {
            *(base + 0x170 + i) = 0;
            *reinterpret_cast<short*>(base + 0xa + (i << 1)) = 0;
        }
    }

    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(
            user->GetServerGroup());
    proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0x186);
    return 0;
#undef FLLF
}


// ============================================================================
// Packet_Monitor_MegaPhone_Hyper_Link::set（ORIG W 0x086d1b9a，
//   _ZN35Packet_Monitor_MegaPhone_Hyper_Link3setEhtPKchS1_hhPK19hyperlink_item_info，
//   objdump 逐指令转写；消费方 GameWorld.cpp send_chat_msg_hyper_link case 0xb/c/e/0x23）。
//   参数序：channel/uniqueId/name/msgLen/msg/type/itemCount/items。
//   写 m_channel(+b)/m_uniqueId(+c)/m_type(+e) → strncpy m_name 0x1e →
//   strncpy m_msg 0x100 → m_msgLen(+166) → m_itemCount(+2d) →
//   for(i<itemCount) memcpy(m_items+i*0x68, items+i*0x68, 0x68) →
//   packetSize(+2) = msgLen + 0x167。
// ============================================================================
#include "Packet_Monitor_MegaPhone_Hyper_Link.h"
void Packet_Monitor_MegaPhone_Hyper_Link::set(
    unsigned char channel, unsigned short uniqueId, char const* name,
    unsigned char msgLen, char const* msg, unsigned char type,
    unsigned char itemCount, hyperlink_item_info const* items)
{
    m_channel = channel;
    m_uniqueId = uniqueId;
    m_type = type;
    strncpy(m_name, name, 0x1e);
    strncpy(m_msg, msg, 0x100);
    m_msgLen = msgLen;
    m_itemCount = itemCount;
    for (int i = 0; i < itemCount; ++i)
        memcpy(m_items + i * 0x68, reinterpret_cast<const char*>(items) + i * 0x68, 0x68);
    packetSize = 0x167 + msgLen;
}
