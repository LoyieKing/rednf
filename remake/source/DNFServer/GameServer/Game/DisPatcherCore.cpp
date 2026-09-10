// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DisPatcherCore/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [BigDispatchCore 2026-09-09·大件专项] 本 TU 曾在 13 个函数里以
// `static const char* const kPretty = "virtual int X::m(...)";` 手抄函数名，
// 由此**发射 ORIG 不存在的导出符号** `_ZZN…E7kPretty`（nm 实测 13 个）并缺发
// ORIG 有的 `_ZZN…E19__PRETTY_FUNCTION__`。已逐字核对 ORIG .rodata（orig_str.py 读
// _ZZ…__PRETTY_FUNCTION__ 对象，12/12 文本一致）后全部改回 __PRETTY_FUNCTION__：
// 发明符号 13→0，本 TU 现发射 109 个 __PRETTY_FUNCTION__ 对象、全部命中 ORIG nm。
// 行为不变（同一字符串常量）。
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

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN18DisPatcher_BuyItem4readER9PacketBufR8MSG_BASE`（P327；
// plan_gi_swaps.py --arbitrate 唯一最高分 37/210）。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
#include "DisPatcherCore/read.inc"
{
    // ORIG 0x081...：无指针局部，实参内联 `mov 0x10(%ebp),%eax; add $0xd/11/15`（帧 0x18）。
    if (!packet.get_int(reinterpret_cast<MSG_BUY_ITEM*>(&msg)->m_itemIdx))   // +0x0d
    {
        return LineFunc(0x188,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(reinterpret_cast<MSG_BUY_ITEM*>(&msg)->m_buyCount))  // +0x11
    {
        return LineFunc(0x18b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(reinterpret_cast<MSG_BUY_ITEM*>(&msg)->m_deliveryType))  // +0x15
    {
        return LineFunc(0x18e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

#include "CSkill.h"              // CSkill::m_fitGrowType/IsStealable（UseSkill 黑客校验）
#include "DisPatcherCore/IsLightServer.inc"
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
#include "DisPatcherCore/CheckDailyScheduleTime.inc"
//   ^ 定义见 GameUtils.cpp:88（_Z22CheckDailyScheduleTimeill；本头无权威声明，
//     与 CBoosterGage.cpp:122 同款局部声明口径）
char* NumberToString(unsigned int value, int radix);
//   ^ _Z14NumberToStringji（与 CInventory.h:403 声明同签名；定义待链接）

class DB_GetDeathTowerBestRecord   // ORIG T 0x0842a048 _ZN26DB_GetDeathTowerBestRecord11makeRequestEij
{                                  // 定义由 DB_U3.cpp 提供（Prod_DB_Misc 批次）；
public:                            // 参数类型按 ORIG mangled Eij=(int, unsigned int)，
    static void makeRequest(int uid, unsigned int characNo);   // uid 按位不变传参
};

#include "DisPatcherCore/getAvatarRentPrice.inc"
//   ^ ORIG T 0x0898c999 _Z18getAvatarRentPriceii（wave1：DisPatcher_RentAvatar::dispatch_sig
//     消费；全仓无声明定义，定义由 Main 分派属主批 → U 引用合规）
#include "DisPatcherCore/RentAvatarErrorProc.inc"
#include "DisPatcherCore/EnterPVPRoom.inc"

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

// [b28d-29] N82：帧 0x7c vs 0x70 + 局部对象槽平移（-0x40 vs -0x34），调用点全等  (ORIG 0x081ed09a insn 409->408)
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 407（ORIG 409 / ours 408），残差=N7/N82 帧与 callee-saved 角色/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// ORIG T 0x081ed09a _Z12EnterPVPRoomP5CUserP8PvP_Room14ENUM_CMDPACKETPKc
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 407（ORIG 409 / ours 408，gap 1），
// p167 调用点差集 CLEAR；残差=N7（callee-saved 角色互换）/块序/P75 实参装载时刻，
// 语义集合全等 ⇒ N7 封顶（跨版本寄存器分配，无源码形可择）。
#include "DisPatcherCore/EnterPVPRoom_1.inc"
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
#include "DisPatcherCore/checkSpecialChar.inc"
//   ^ ORIG T 0x0829efae _Z16checkSpecialCharPc（wave1：CreateCharac::check_error 消费）
#include "DisPatcherCore/checkSpecialCharName.inc"
//   ^ ORIG T 0x0829f01f _Z20checkSpecialCharPc（同上）

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
// m_battleField[0x158] 等 pad 占位、未引入 CBattle_Field 类型；Main 裁定
// 本组 TU 局部视图，与 CParty.cpp 同口径）。布局：pad 0xb24 + BF 0xcd0。
struct CPartyBattleFieldView
{
    char m_pad[0xb24];
    CBattle_Field m_battleField;   // +0xb24
};

// WongWork::CDeathTower 私有区局部视图（m_timerKeys[10]@+0xb3c 为 private，
// CDeathTower.h:254；ORIG 跨类直访该区——本 TU 按 P1 组件视图具名取址，
// 与 CPartyBattleFieldView 同口径）。
struct CDeathTowerTimerKeyView
{
    char m_pad[0xb3c];
    int m_timerKeys[10];           // +0xb3c（TIMER_MESSAGE 0x48..）
};

// DisPatcher_Login::dispatch_sig 栈上登录载荷区（ORIG -0x1a7(%ebp) 起 0xbf 字节，
// ORIG 0x081e8c78；证据 docs/class_func_reports/DisPatcher_Login/dispatch_sig.md：
//   +0x0d get_int/读 `mov -0x19a(%ebp),%eax`（4B）= 账号 ID 串长
//   +0x11 get_str(authBuf+0x11,0x18) 0x081e9239（+0x11..0x28 账号 ID 串）
//   +0x29 get_int/读 `mov -0x17e(%ebp),%eax`（4B）= 后继串长（<=0x83）
//   +0x2d get_str(authBuf+0x2d,0x84) 0x081e9317（+0x2d..0xb0）
//   +0xb1 get_byte/读 `movzbl -0xf6(%ebp),%eax`（1B 无符号）= 登录类型
//   +0xb6 get_binary(authBuf+0xb6,8) 0x081e949d（+0xb6..0xbd 鼠标注册器密码密文）
// 全宽 pack(1)；sizeof 0xbf = ORIG memset 长度）。
#pragma pack(push, 1)
struct ST_LOGIN_AUTH_PAYLOAD
{
    char m_pad00[0xd];         // +0x00..0x0c（ORIG 未单独出线，仅 memset 覆盖）
    unsigned int m_userIdLen;  // +0x0d（get_int；get_str 长度实参）
    char m_userId[0x18];       // +0x11..0x28（get_str 0x18）
    unsigned int m_len2d;      // +0x29（get_int；<=0x83 门）
    char m_field2d[0x84];      // +0x2d..0xb0（get_str 0x84）
    unsigned char m_loginType; // +0xb1（get_byte）
    char m_padB2[4];           // +0xb2..0xb5（ORIG 未出线）
    char m_encPwd[8];          // +0xb6..0xbd（get_binary 8 / memcpy 7）
    char m_padBE[1];           // +0xbe（补齐 ORIG memset 长度 0xbf）
};
#pragma pack(pop)
static_assert(sizeof(ST_LOGIN_AUTH_PAYLOAD) == 0xbf,
              "ST_LOGIN_AUTH_PAYLOAD size (ORIG memset 0xbf)");

// DisPatcher_RentAvatar::dispatch_sig 租约载荷区（ORIG -0x188(%ebp) 起 0x3f 字节，
// ORIG 0x081ee59c；证据 docs/class_func_reports/DisPatcher_RentAvatar/dispatch_sig.md：
//   +0x0d get_byte(unsigned char&) 0x081ee5de / 读 `movzbl -0x17b(%ebp)` = 条目数(1..9)
//   条目 stride 5：0x081ee680 `mov %edx,%eax; shl $2; add %edx`（i*5）
//     entry+0 = 载荷 +0x0e（`movzbl 0xe(base+i*5)` 0x081ee8c2/0x081eea49：1→30 天、
//               2→365 天、0→7 天）；get_byte(unsigned char&) 装载同槽
//     entry+1 = 载荷 +0x0f（`mov 0xf(base+i*5)` 4B；get_int(unsigned long&)）
// sizeof 保持 ORIG 区extent 0x3f（尾 4 字节为相邻栈槽余量，ORIG 未出线）。
#pragma pack(push, 1)
struct STRentAvatarEntry
{
    unsigned char m_rentDaysType;  // entry+0x00（1=30d / 2=365d / 0=7d / 其它=错误）
    unsigned long m_itemId;        // entry+0x01（4B；get_int(unsigned long&)）
};
struct STRentAvatarReq
{
    char m_pad0[0xd];                   // +0x00..0x0c（ORIG 未出线）
    unsigned char m_count;              // +0x0d
    STRentAvatarEntry m_entries[9];     // +0x0e..0x3a（ORIG 上限 count<=9）
    char m_tailPad[4];                  // +0x3b..0x3e（保持 ORIG 栈区 0x3f 尺寸）
};
#pragma pack(pop)
static_assert(sizeof(STRentAvatarEntry) == 5, "STRentAvatarEntry stride 5");
static_assert(sizeof(STRentAvatarReq) == 0x3f, "STRentAvatarReq size");

// MSG_ADD_FRIEND / MSG_REMOVE_FRIEND 的 +0xd 起载荷区。权威头 PacketTypes.h
// （冻结，:63/:1168）该两型目前仍是 `char m_head[0xd]; char m_pad[0x37]`，
// 故本 TU 先按以下证据具名视图化；待 PacketTypes.h 落权威字段后删除本视图。
// 证据：DisPatcher_AddFriend/read.md ORIG 0x081cb1d9 `add $0xd` +
//   call _ZN9PacketBuf7get_intERi（int 名字长度）、0x081cb221 `add $0x11` +
//   get_str(char*,0x1e,int)（角色名）；RemoveFriend/read.md 同型
//   （0x081cb53d / 0x081cb585）；process_packet_jpn.md 0x081cb476 `lea 0x11(%eax)`
//   作 strncpy 源。
#pragma pack(push, 1)
struct STFriendReqView
{
    char m_head[0xd];         // +0x00..0x0c 包头
    int m_nameLen;            // +0x0d（get_int(int&)）
    char m_name[0x1e];        // +0x11..0x2e（get_str 0x1e / strncpy 0x1e）
    char m_pad2F[0x15];       // +0x2f..0x43（ORIG 未出线，保持 sizeof 0x44）
};
#pragma pack(pop)
static_assert(sizeof(STFriendReqView) == 0x44, "STFriendReqView size");

// [b28e-L5] P250：IncreStatus::dispatch_sig ORIG 载荷槽基 -0x2f（**非 4 对齐**）+
// `add $0xd` 取成员（0x081ede34 `lea -0x2f(%ebp),%eax; add $0xd,%eax`）⇒ 原码是
// pack(1) 结构体局部（val@+0x0d，sizeof 0x10），非裸 short。
#pragma pack(push, 1)
struct IncreStatusSigView
{
    char m_head[0xd];     // +0x00..0x0c 包头
    short m_val;          // +0x0d（get_short(short&)）
    char m_tail[0x1];     // +0x0f
};
#pragma pack(pop)
static_assert(sizeof(IncreStatusSigView) == 0x10, "IncreStatusSigView size");

// DisPatcher_MoveMap::read/process 消息视图（ORIG DWARF 语义修正 vs PacketTypes 冻结版）：
//  m_1a = bool：ORIG read `mov %al,0x1a(%edx)`（QI 直存）+ process `xor $0x1,%eax;
//  test %al,%al`（唯 bool 取反形，0x081...）；0x33..0x38 = get_short(0x33)+get_int(0x35)
//  两字段（ORIG `add $0x33` / `add $0x35` 单步）。sizeof 与 MSG_MOVE_MAP 一致 0x42。
#pragma pack(push, 1)
struct MoveMapMsgView
{
    char m_head[0xd];
    signed char m_x;
    signed char m_y;
    unsigned int m_fieldF;
    unsigned int m_field13;
    unsigned char m_field17;
    union { char m_pad18[2]; unsigned short m_18; };   // ORIG movzwl 0x18(%eax) 单级 ushort
    bool m_1a;
    unsigned short m_1b[4];
    unsigned int m_23[4];
    unsigned short m_33;
    int m_35;
    unsigned char m_39;
    char m_padTail[8];
};
#pragma pack(pop)
static_assert(sizeof(MoveMapMsgView) == 0x42, "MoveMapMsgView size");

// DisPatcher_MultiBoxLottery::dispatch_sig 栈上局部消息视图（ORIG -0x21(%ebp)，
// 0x11 字节；证据 docs/class_func_reports/DisPatcher_MultiBoxLottery/dispatch_sig.md
// ORIG 0x0820b8db `lea -0x21(%ebp),%eax; add $0xd` + get_shortERt、
// 0x0820b920 `add $0xf` + get_shortERt，0x0820b96c `movzwl -0x14(%ebp)` 读回）。
#pragma pack(push, 1)
struct STMultiBoxLotteryReq
{
    char m_pad0[0xd];              // +0x00..0x0c（ORIG 未出线）
    unsigned short m_field0d;      // +0x0d（multibox_lottery_item 第 1 实参；语义未证）
    unsigned short m_field0f;      // +0x0f（multibox_lottery_item 第 2 实参；语义未证）
};
#pragma pack(pop)
static_assert(sizeof(STMultiBoxLotteryReq) == 0x11, "STMultiBoxLotteryReq size");

#include "DisPatcherCore/MakePVPRoom.inc"
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


#include "DisPatcherCore/dtor_DisPatcher_CheckConn.inc"
#include "DisPatcherCore/dispatch_sig.inc"
{
    // ORIG 0x081ea900 T（报告 DisPatcher_CheckConn/dispatch_sig.md 全文转写）：
    // decre_check_count → invenCk/skillCk 校验（hack 计数 0x25b/0x25c）→ CHades Send_ChangeGold。
    user->decre_check_count();
    int invenCk = 0;  // ORIG 0x081ea900+0x?: decre_check_count 后 movl $0x0,-0xc(%ebp)
    if (!packet.get_int(invenCk))
    {
        return LineFunc(0x6bf8, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacInvenCheckSum() != invenCk)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
    }
    int skillCk = 0;  // ORIG addServerHackCnt(0x25b) call 后 movl $0x0,-0x10(%ebp)
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
#include "DisPatcherCore/read_1.inc"
{
    // ORIG 0x08232a7e：空实现，恒返 0（mov $0x0,%eax; pop %ebp; ret）。
    return 0;
}
#include "DisPatcherCore/process.inc"

#include "DisPatcherCore/DisPatcher_Login.inc"
#include "DisPatcherCore/dtor_DisPatcher_Login.inc"
// §6.2 巨件核销：P167 168/168 [0缺0多]；gap 986→986/990；残差归因 N82/P112
// （槽复用策略 + 块排布，diff 形态计数对称无缺类，详见
// [b28d-29] N82：帧 0x1c0 vs 0x1d0 + 局部对象槽平移（-0xc4 vs -0x90），调用点全等  (ORIG 0x081e8c78 insn 986->990)
// docs/asm_translation_patterns.md 负例表）。无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 704（ORIG 986 / ours 990），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_1.inc"
{
    // ORIG 0x081e8c78 静态局部 garenaToken（801B，BSS 无初始化；每次调用 memset 清零）
    static char garenaToken[0x801];

    ST_LOGIN_AUTH_PAYLOAD authBuf;       // -0x1a7..-0xe9（ORIG memset 0xbf 清零）
    unsigned int patchTime;              // -0x84
    unsigned int tokenLen;               // -0x88
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

    memset(&authBuf, 0, sizeof(authBuf));
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

    // P48/P0：ORIG 每用点独立求值（Ghidra 4 次 getGarenaAuthData：decrypt/setPatchTime/
    // getPatchTime/getTimeStamp 各一），禁缓存 authData。
#define GARENA_AUTH reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData())
    if (Taiwan::AuthGarenaToken::decrypt(garenaToken, (int)tokenLen,
                                         *GARENA_AUTH) != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x6617, 5);
        trace("[Taiwan, Auth] Fail decrypt:%s", garenaToken);
        return LineFunc(0x6618, __PRETTY_FUNCTION__, 0, 0);
    }

    GARENA_AUTH->setPatchTime(patchTime);
    nowSec = GlobalData::s_systemTime_.getCurSec();
    // ORIG 求值序：先 getPatchTime(+0x661e trace 前) 再 getTimeStamp；实参按槽装载。
    unsigned int patchTimeGot = GARENA_AUTH->getPatchTime();
    unsigned int tokenStamp = GARENA_AUTH->getTimeStamp();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x661e, 0);
    trace("[Taiwan, Auth] Token timestamp info. (token_time:%u, current_time:%u, patch_time:%d)",
          tokenStamp, nowSec, patchTimeGot);
#undef GARENA_AUTH

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x21))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 0xd1);
        return 0;
    }

    if (packet.get_int(authBuf.m_userIdLen) != 1)
        return LineFunc(0x665f, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_str(authBuf.m_userId, 0x18,
                       (int)authBuf.m_userIdLen) != 1)
        return LineFunc(0x667d, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_int(authBuf.m_len2d) != 1)
        return LineFunc(0x6692, __PRETTY_FUNCTION__, 0, 0);

    if (0x83 < authBuf.m_len2d)
        return LineFunc(0x6698, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_str(authBuf.m_field2d, 0x84,
                       (int)authBuf.m_len2d) != 1)
        return LineFunc(0x66a8, __PRETTY_FUNCTION__, 0, 0);

    // ORIG 0x081e9361/0x081e93ad：登录型别无独立局部，三处判定/末段写 SIG+0xa0
    // 均逐次重读载荷字节（movzbl -0xf6(%ebp)）——P86 逐次重读口径。
    if (packet.get_byte(authBuf.m_loginType) != 1)
        return LineFunc(0x6734, __PRETTY_FUNCTION__, 0, 0);

    if (authBuf.m_loginType != 0 && authBuf.m_loginType != 1 &&
        authBuf.m_loginType != 2)
        return LineFunc(0x6738, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());

    versionMajor = 0;
    if (packet.get_int(versionMajor) != 1)
        return LineFunc(0x673f, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());

    if (versionMajor != 8)
        return LineFunc(0x6741, __PRETTY_FUNCTION__, 0,
                        (unsigned int)user->get_acc_id());

    if (packet.get_binary(authBuf.m_encPwd, 8) != 1)
        return LineFunc(0x6742, __PRETTY_FUNCTION__, 0, 0);

    if (!Dispatcher_MouseRegister::decryptPassword(
            authBuf.m_encPwd, decPwd, (unsigned int)versionMajor))
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

    memcpy(authBuf.m_encPwd, decPwd, 7);

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
    // ORIG 0x81e9811..0x81e9843（Ghidra de→op<<→de→op<<）：operator* 显式链形。
    streamGuard.operator*()->operator<<(0x3);
    streamGuard.operator*()->operator<<(static_cast<int>(user->GetUID()));
    // 【ABI 镜像】SIG_LOGIN_DATA 偏移字段（SigTypes_D.h 待具名化区：+0x18/+0x84/
    // +0x9c/+0xa0/+0x3905/+0x3a30；+0xa1=m_blackIpCheckBuf、+0x3a34=m_clientVersion、
    // +0x3a38=m_channelRestrictField 已具名可用）
    SIG_LOGIN_DATA* login = streamGuard->GetInBuffer<SIG_LOGIN_DATA>();   // ORIG pt→GetInBuffer 链
    memset(login, 0, 0x4358);
    if (packet.get_short(login->m_channelRestrictField) != 1)
        return LineFunc(0x6980, __PRETTY_FUNCTION__, 0, 0);

    if (login->m_channelRestrictField < 0 || 0x11 < login->m_channelRestrictField)
        login->m_channelRestrictField = 0;

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        cMyTrace groupTrace(__PRETTY_FUNCTION__, 0x699a, 0);
        groupTrace("CheckLoginServerGroup user_id(%s), server_group(%d)",
                   authBuf.m_userId, (int)login->m_channelRestrictField);
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

    // P48/P0：ORIG 本区两个独立 getGarenaAuthData 求值点（getUid / getTimeStamp 各一）。
    char* userIdStr = NumberToString(
        reinterpret_cast<Taiwan::GarenaAuthData*>(
            user->getGarenaAuthData())->getUid(), 0);
    strncpy(login->m_userId, userIdStr, 0x18);          // +0x00
    memset(login->m_pad18, 0, 0x84);                    // +0x18 清零区
    login->m_timeStamp =
        reinterpret_cast<Taiwan::GarenaAuthData*>(
            user->getGarenaAuthData())->getTimeStamp(); // +0x3a30

    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        login->m_gcNo =
            static_cast<unsigned int>(G_CEnvironment()->m_serverEnvir.m_gcNo);  // +0x9c ← env+0x198
    else
        login->m_gcNo =
            (unsigned int)G_CEnvironment()->get_gc_no_hardcode(
                (ENUM_SERVER_GROUP)login->m_channelRestrictField);

    user->m_network.GetPeerIP2(login->m_blackIpCheckBuf, 0x14);
    // ORIG 0x081e9b64 `movzbl -0xf6(%ebp),%edx; mov %dl,0xa0(%eax)`：载荷字节直读
    login->m_loginType = authBuf.m_loginType;            // +0xa0
    // +0x3905 落于 m_passPadPassword(+0x38e1) 内 +0x24 区（ORIG 网吧密码串写入）
    strncpy(&login->m_passPadPassword[0x24], authBuf.m_encPwd, 7);
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
#include "DisPatcherCore/read_2.inc"
{
    // ORIG 0x08232a6a：空实现，恒返 0（mov $0x0,%eax; pop %ebp; ret）。
    return 0;
}
#include "DisPatcherCore/process_1.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetIP.inc"
#include "DisPatcherCore/read_3.inc"
{
    // ORIG 0x08232fc8：空实现，恒返 0（mov $0x0,%eax; pop %ebp; ret）。
    return 0;
}
#include "DisPatcherCore/process_2.inc"

// [b28d-29] N78/N82：ORIG 4 个 dword 清零 vs 4.4 逐字节 movb 展开 + 帧 0x60 vs 0xa0；语义等价  (ORIG 0x081ed948 insn 316->373)
#include "DisPatcherCore/dtor_DisPatcher_Exit.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 344（ORIG 316 / ours 372），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_2.inc"
{
    // [b28e-L5] P214③/N78：ORIG 0x081ed948 函数头为 `movl $0x0,-0x44/-0x40/-0x3c/-0x38`
   // 4×dword 常量清零（16 字节数组）⇒ 原码是 `char x[0x10] = {0};`；逐字节 16×movb
    // 是早期 hack 写法，多 12 条指令且与 ORIG 形态不符。
    char local_48[16] = {0};
    // [b28e-L5] P167 真缺口补修：ORIG 0x081ed96c `mov 0xc(%ebp),%eax; lea 0xe0(%eax),%edx`
    // + 0x081ed987 `call _ZN8CNetworkILi4096ELi450000EE10GetPeerIP2EPci`（len 0x10）
    // —— 原实现漏调用；该缓冲尾段（0x081edd84/0x081eddbd）作为
    // DB_SecuServiceUpdateValidityTime::makeRequest 的第 4 实参（len 8 / 0x10）。
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
    // [b28d-solo-DisPatcherCore] P167 真 bug 修正：ORIG 0x081... 调
    // `_ZN8WongWork12CMCAPManager19IsSaveCleanPadPointEv`（非 const，WongWorkMisc_A.cpp
    // 定义），原写 `isSaveCleanPadPoint()`（CMCAPManager.h:29 旧 const 变体）为错符号。
    if (user->m_mcap.IsSaveCleanPadPoint())
    {
        // [G5-DisPatcherCore] P329/P0：ORIG 0x081ed9a6 段实参右→左求值——
        // `call get_acc_id`（→%ebx 保活）先于 `call CMCAPManager::getPoint`（→%eax），
        // 即第二实参先算、第一实参后算；具名局部把 getPoint 提前到语句边界 ⇒ 序反 + 多 -0x5c 槽。
        DB_SaveCleanpadPoint::makeRequest(user->m_mcap.getPoint(),
                                          static_cast<int>(user->get_acc_id()));
        user->m_mcap.disableCleanPadPoint();
    }
    if (user->get_state() > 2)
    {
        if (G_GameWorld()->IsEquipSlotSwitchChannel() != false)
        {
            if (user->getCurChannelEquipslotSwitch() == 1)
            {
                CExpandEquipslot* local_20 = user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(9));
                // [G5-DisPatcherCore] P0：ORIG 0x081edbxx 段 movsbl 直入 %ebx/%eax（无槽往返）
                // ⇒ 原码无具名 char 局部，实参右→左（先第 4 实参 getCurExpand → %ebx，
                // 后第 3 实参 getCurChannel → %eax）；具名局部会各占一字节槽并 reload。
                local_20->EquipslotSwitch(user, '\0',
                                          static_cast<char>(user->getCurChannelEquipslotSwitch()),
                                          static_cast<char>(user->getCurExpandEquipslotSwitch()));
            }
        }
    }
    {
        // [Lane38 2026-09-10] P0/P219：ORIG 0x081edc9x 段无具名 string 局部——
        // 字面量直接喂按值形参（ORIG 全函数仅 1 次 `_ZNSsC2EPKcRKSaIcE` 构造、
        // **无** `_ZNSsC1ERKSs`；P167 实测我方 EXTRA `_ZNSsC1ERKSs` +1、
        // `_ZNSsC1EPKcRKSaIcE`/`_ZNSaIcEC1Ev` 为 C2 变体别名）。原「具名局部 +
        // 按值传参」多出 1 次拷贝构造与整段临时副本码。
        // P183/P0：ORIG 0x081edbxx 为单语句链式——`call Singleton::Get` 结果直接作
        // findGameScript 的 receiver，返回值直接作 delUserRewardData 的 receiver，
        // 无 this_01/this_02 两个具名指针槽（我方多发 2 组 store/load 且帧 +8）。
        static_cast<ARAD::Arad_EventPeriodDataManager*>(
            ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                "Arad_EventPeriodDataManager"))
            ->delUserRewardData(user);
    }
    if (user->get_state() != 0)
    {
        user->WorkPerFiveMin();
        if (user->get_charac_memberkey() != 0)
        {
            int local_14 = user->get_member_pay_tex_money_to_upper();
            int local_10 = user->get_member_pay_tex_fatigue_to_upper();
            // [G5-DisPatcherCore] P0/P52：ORIG 仅 money/fatigue 有具名槽（-0x10/-0xc）；
            // characNo→%ebx、is_connect→%esi、GetServerGroup 结果直作 receiver 实参、
            // GetServerProxy 结果直作 this（全无槽）。我方原 4 个具名量（uVar5/bVar3/uVar6/
            // this_03）正是本件帧 +0x10 的来源；实参序仍按 cdecl 右→左（characNo 先于
            // is_connect），与 ORIG 0x081edbxx 段一致。
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
                ->SendMemberPayTax(user->is_connect_upper_member(),
                                   static_cast<unsigned int>(user->getCurCharacNo()),
                                   local_14, local_10);
            if (0 < local_14)
            {
                user->set_member_pay_tex_money_to_upper(0);
            }
            if (0 < local_10)
            {
                user->set_member_pay_tex_fatigue_to_upper(0);
            }
        }
        // [G5-DisPatcherCore] P227/P329：ORIG 0x081edcb0 段是 cMyTrace **全表达式临时链**——
        // `call get_acc_id` 结果直入 NumberToString（无 -0x18 槽），NumberToString 结果
        // 升格 %ebx 跨 ctor 保活；trace 之后才 `call get_acc_id; mov %eax,-0x18`（第二次取值）。
        // 我方原「具名 local_1c/local_18/uVar6 + 具名 cMyTrace」多 3 个槽并改变求值序。
        unsigned int local_1c;    // ORIG -0x18（第二次 get_acc_id 落此）
        unsigned int local_18;    // ORIG -0x14（GetUID）
        cMyTrace(__PRETTY_FUNCTION__, 0x706d, 0)(
            "[USER LOGOUT (Normal)] DB ID : %s\n", NumberToString(user->get_acc_id(), 0));
        // 原为手写虚调（裸 vptr 解引用，违 §4.3）；ORIG 0x081edc7x 的
        // `call GetInstancePrivateStoreMgr; mov (%eax),%edx; mov (%edx),%ecx; call *%ecx`
        // 即 IUserPos 槽 0 = CPrivateStoreMgr::Exit（头文件 CPrivateStoreMgr.h:357 已声明）
        // 的真实虚成员调用，且无 puVar7 具名指针槽。
        private_store::GetInstancePrivateStoreMgr()->Exit(user);
        pvp_assault::GetInstanceAssaultMgr()->OnLeaveAssaultPlace(user, true);
        GlobalData::s_ExpertJobMgr->OnLeaveUser(user);
        GlobalData::s_onlinePreliminaryTeamMgr->OnLeaveUser(user);
        user->checkLogOutCorrectly();
        // ORIG 0x081edcd6 `call _ZNK5CUser10get_acc_idEv` → -0x18(%ebp)，紧接
        // `call GetUID` → -0x14（0x081edd68/0x081edda1 读 -0x18/-0x14）——此处的取值
        // 才是具名槽的来源；上一条 trace 处那次 get_acc_id 无槽（已内联）。
        local_1c = user->get_acc_id();
        local_18 = user->GetUID();
        // P0/P329：ORIG 该处 `call GetCharacPlayTick`→%ebx（保活）、`call get_charac_no`→%eax
        // 直传 makeRequest，无具名 lVar8/iVar4 槽。
        DB_UpdateCharacPlayTime::makeRequest(user->get_charac_no(-1),
                                            user->GetCharacPlayTick());
        G_CGameManager()->user_exit(user);
        if (local_49 == '\0')
        {
            DB_ArrangeUserLoginInfo::makeRequest(local_1c, true, local_18);
        }
        else
        {
            DB_ArrangeUserLoginInfo::makeRequest(local_1c, false, local_18);
            // P183：ORIG `call getSecurityCard; mov %eax,(%esp); call isCertified` 为单语句
            // 内联链（无 this_06/this_07 具名指针槽）。
            if (user->getSecurityCard()->isCertified() != false)
            {
                DB_SecuServiceUpdateValidityTime::makeRequest(local_1c, 0xb4, 8, local_48);
            }
            if (user->getPad()->isCertified() != false)
            {
                DB_SecuServiceUpdateValidityTime::makeRequest(local_1c, 0xb4, 0x10, local_48);
            }
        }
    }
    return 0;
}
#include "DisPatcherCore/read_4.inc"
{
    return 0; // ORIG 0x08232be0：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/process_3.inc"

// [b28d-29] N82/setcc-fold：帧 0x40 vs 0x60 + 槽平移与 sete 折痕，调用点全等  (ORIG 0x081c686e insn 267->285)
#include "DisPatcherCore/dtor_DisPatcher_SelectCharac.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 225（ORIG 267 / ours 284），残差=P168a④ bool 槽化/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error.inc"
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
    MSG_CHARAC_SELECT& sel = reinterpret_cast<MSG_CHARAC_SELECT&>(msg);
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        // [G5-MiscC] P0/P52：ORIG 0x081c68ed `call getOnlinePreliminaryCharacNo;
        // mov %eax,0x4(%esp); mov 0xc(%ebp),%eax; mov %eax,(%esp); call get_charac_idx`
        // —— 预选角色号作实参直传，无 uVar6 具名槽（我方 fresh 多 -0x1c 槽）⇒ 内联。
        sel.m_slot = static_cast<signed char>(
            user->get_charac_idx(user->getOnlinePreliminaryCharacNo()));
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
        // [G5-MiscC] P0/P52：ORIG 0x081c6963-0x081c696a 仅 `movzbl 0xd(%eax),%eax;
        // movsbl %al,%ebx`（值在 callee-saved %ebx 跨 cMyTrace ctor 保活），
        // 无 char 具名槽（我方 fresh 多 -0x15）⇒ 内联回 varargs 实参位。
        cMyTrace local_34(__PRETTY_FUNCTION__, 0x16c4, 5);
        local_34("pUser->SelectCharac() fail, %d out of range", static_cast<int>(sel.m_slot));
        return 4;
    }
    if (user->m_characList.size() <= static_cast<unsigned char>(sel.m_slot))
    {
        return 0xfffffffe;
    }
    std::string local_38(user->m_characList[static_cast<int>(sel.m_slot)].m_name);
    // [FL4] P0/P198②：ORIG 0x081c6a5d-0x081c6a87 该 bool 无栈槽（`mov $0x1,%eax; jmp;
    // mov $0x0,%eax; test %al,%al; je` 寄存器物化后直测）⇒ 原码是**单条 if（内联短路表达式）**，非
    // 「bool 具名 + 二次 if」；旧形多发 `movb $0/$1,-0x21; cmpb $0,-0x21` 四条槽往返。
    if (CSyncSlangFilter::GetInstance()->HasSlangName(local_38) == false &&
        CSyncSlangFilter::GetInstance()->HasSlang(local_38) == false)
    {
        return 0x9f;
    }
    // [G5-MiscC] P0/P52：ORIG 0x081c6a99-0x081c6abf 为
    // `call get_acc_id; mov %eax,0x4(%esp); ... call isGM; movzbl %al,%ebx;
    //  call G_CDataManager; mov %ebx,0x4(%esp); call setCharacNameFilterPass`
    // —— isGM 结果在 %ebx 跨 G_CDataManager() 保活，accId/bVar4b 均无具名槽
    // （我方 fresh 多 -0x1c/-0x15 两槽）⇒ 单语句链式（P52）。
    G_CDataManager()->setCharacNameFilterPass(
        GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()));
    if (G_CDataManager()->hasPreventPrefixCharacName(
            user->m_characList[static_cast<int>(sel.m_slot)].m_name,
            static_cast<int>(
                strlen(user->m_characList[static_cast<int>(sel.m_slot)].m_name))) != false)
    {
        return 8;
    }
    // [G5-MiscC] P292 档①：ORIG 0x081c6b50 `call checkSelectCharac; xor $1,%eax;
    // test %al,%al; je <继续>` ⇒ 原码是取反 `!`（`== true` 正向形不符）⇒ 反条件早退。
    if (!user->checkSelectCharac(static_cast<int>(sel.m_slot)))
    {
        return 0xfffffffd;
    }
    if (!user->isLockedDeleteCharac())
    {
        // [G5-MiscC] P292 档①：ORIG 0x081c6bdd `call lock4DataLoad; xor $1,%eax;
        // test %al,%al; je <+0x37d>` 后 0x081c6be4 `mov $0xfffffffc,%ebx` / 0x081c6beb
        // `mov $0x0,%ebx` ⇒ `!` 形 + 常量直返（无 uVar10 槽；ORIG 全函数返回值一律走
        // %ebx 寄存器直返，见 0x081c6c18 `mov %ebx,%eax`）。
        if (!user->lock4DataLoad())
        {
            return 0xfffffffc;
        }
        return 0;
    }
    // [G5-MiscC] P0/P52：ORIG 0x081c6b8a `call characList[]; mov (%eax),%ebx`
    // （m_characNo 直读 %ebx，跨 cMyTrace ctor 保活，无 uVar9 具名槽）⇒ 实参位内联。
    cMyTrace local_20(__PRETTY_FUNCTION__, 0x171c, 0);
    local_20("select_charac error during delete charac_no(%d)",
             static_cast<int>(user->m_characList[static_cast<int>(sel.m_slot)].m_characNo));
    return 0x7fffffff;
}

// ORIG 0x081c6c22 T _ZN23DisPatcher_SelectCharac22checkPvpEnterCharacterEP5CUserR17MSG_CHARAC_SELECT
// （报告 DisPatcher_SelectCharac/checkPvpEnterCharacter.md 全文转写，wave2）：
// user==0 → false；PvP 频道且槽位职业为 9/10（ restricting job ）→ false；否则 true。
#include "DisPatcherCore/checkPvpEnterCharacter.inc"
{
    if (user == 0)
    {
        return false;
    }
    // ORIG 0x081c6c22（帧 0x18 无槽）：GetCharacJob **调用两次**（各自重算实参
    // `movzbl 0xd; movsbl`），返回按 **%al 单字节** cmp（`cmp $0x9,%al; sete %al;
    // test; je` = 技9 bool 物化形）⇒ 原声明返回 char/uchar（已报 Main：CUser.h
    // 冻结头 _ZNK5CUser12GetCharacJobEi int→char）。
    if (G_GameWorld()->IsPVPChannel())
    {
        if (user->GetCharacJob(static_cast<int>(msg.m_slot)) == 9)
        {
            return false;
        }
        if (user->GetCharacJob(static_cast<int>(msg.m_slot)) == 10)
        {
            return false;
        }
    }
    return true;
}

// ORIG 0x081cab3a T _ZN18DisPatcher_UseCoin20usedCoinDungeonCheckEP5CUserP6CParty
// （报告 DisPatcher_UseCoin/usedCoinDungeonCheck.md 汇编+C 全文转写，wave2）。
#include "DisPatcherCore/usedCoinDungeonCheck.inc"
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
        // CParty+0xb24 内嵌 CBattle_Field。ORIG 0x81ca...：`mov 0x10(%ebp);add $0xb24;
        // mov %eax,(%esp);call` 无中间指针局部 → 表达式内联。
        dungeonIndex = reinterpret_cast<CPartyBattleFieldView*>(party)
                           ->m_battleField.get_dungeon_index();
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
    // ORIG 0x81cac0f `test %al,%al; je 远端 0x11 岛`（无 xor → 非 ! 写法）。
    // 0x11 块物理位于 result 块与 ret0 尾岛之间 = gcc else 侧布局；结构：
    if (user->getCurCharacInvenR()->IsCoinUsable())
    {
        if (party->checkUnLimitUsedCoinCondition(dungeon))
        {
            return 0;
        }
        // ORIG 块物理序：[result 块][0x11 else 岛][mov $0 fall 尾] ——
        // 非零早退 + else 显式 + 函数尾 return 0 共享 fall。
        int result = party->checkLimitUsedCoinCondition(dungeon);
        if (result != 0)
        {
            return result;
        }
    }
    else
    {
        return 0x11;
    }
    return 0;
}

// ORIG 0x081ca63a T _ZN18DisPatcher_UseCoin11Dungeon_check_errorEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] N82/P183：ORIG 每次重调 GetParty 并落独立槽（不复用），4.4 复用同槽 + 帧 -0x14；语义集合全等  (ORIG 0x081ca63a insn 87->87)
// （报告 DisPatcher_UseCoin/Dungeon_check_error.md 汇编+C 全文转写，wave2）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 57（ORIG 87 / ours 86），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/Dungeon_check_error.inc"
{
    // [b28d-solo-DisPatcherCore] P28/P212：ORIG +0x06 `mov 0x10(%ebp),%eax;
    // mov %eax,-0x1c(%ebp)` = msg 视图引用**首个声明**（-0x1c），其后 party0/-0x18、
    // party/-0x14、dungeon/-0xc 逐槽对齐 ⇒ 把 req 提到函数头。
    MSG_USE_COIN& req = *reinterpret_cast<MSG_USE_COIN*>(&msg);
    (void)param;   // ORIG 第 3 参未消费（coin 读自 msg+0xd）
    // ORIG（report Ghidra C）调 GetParty **两次**：this_00 用于早期 ==0 return -1
    // 且传给 usedCoinDungeonCheck；this_01 用于 dungeon 分支。源不得合并为一次缓存
    // （合并会少一次 call，P196 反例拓扑）。
    CParty* party0 = static_cast<CParty*>(user->GetParty());
    if (party0 == 0)
    {
        return -1;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    // [b28d-solo-DisPatcherCore] P157/P183：ORIG `cmpl $0,-0x14; je` 之后才发
    // `mov 0xcac(%eax),%eax`（dungeon 延迟到嵌套 if 内），且 battleField 地址每次
    // 重算 `add $0xb24`（无具名局部槽）⇒ 改嵌套 if + 视图内联表达式。
    if (party != 0)
    {
        CDungeon* dungeon = party->m_battleField.m_pDungeon;   // +0xcac
        if (dungeon != 0)
        {
            // ORIG：report Ghidra C `(char)this[0x89f] < 1` —— 对 +0x89f 字节做**有符号**
            // 比较（movzbl 载入后 test %al; jle）。字段声明 unsigned char，若直写
            // `m_bloodDungeonType >= 1` 4.4 产 `je`（==0 语义，对 ≥0x80 值分叉），故
            // 按 ORIG 有符号语义 `(int)(signed char)field > 0`（P196 同宽字节符号序）。
            if ((int)(signed char)dungeon->m_bloodDungeonType > 0 &&
                reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.getBloodState() >= 3)
            {
                return 0x13;
            }
            if (dungeon->isTournamentDungeon() && party->GetTournamentDungeonClearState() >= 1)
            {
                return 0x13;
            }
        }
    }
    int err = usedCoinDungeonCheck(user, party0);
    // [b28d-solo-DisPatcherCore] P16 早返形：ORIG `cmpl $0,-0x10; je L_body;
    // mov -0x10,%eax; jmp L_ret`（err!=0 直返）+ 内层 `jle L_skip; mov -0x10,%eax;
    // jmp; L_skip: mov $0,%eax`。
    if (err != 0)
    {
        return err;
    }
    // ORIG 0x081ca73c `mov -0x18(%ebp),%eax` ⇒ ReviveUser 的 this 是**第一次**
    // GetParty 的返回值（party0，与 usedCoinDungeonCheck 同源；Ghidra C 的 this_01
    // 映射有误，以 ORIG 汇编为准）。
    err = party0->ReviveUser(user, req.m_0d);   // ORIG movzwl msg+0xd
    if (err > 0)
    {
        return err;
    }
    return 0;
}

// ORIG 0x081ca75c T _ZN18DisPatcher_UseCoin11WarRoom_check_errorEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseCoin/WarRoom_check_error.md 汇编+C 全文转写，wave2）。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 39 / ours 39 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/WarRoom_check_error.inc"
{
    // ORIG +0x06：msg 先落远槽 -0x14（req 先声明，技21/13），room 后落 -0x10。
    MSG_USE_COIN& req = *reinterpret_cast<MSG_USE_COIN*>(&msg);
    (void)param;   // ORIG 第 3 参未消费（coin 读自 msg+0xd）
    WarRoom* room = static_cast<WarRoom*>(user->GetWarRoom());
    if (room == 0)
    {
        return -2;
    }
    // [b28d-solo-DisPatcherCore] P0：ORIG +0x3a `test %al,%al; je`（无槽无保活）
    // ⇒ 原码是内联表达式，无具名 bool 局部（register 形会引 push %ebx 反劣，实测）。
    if (user->getCurCharacInvenR()->IsCoinUsable())
    {
        int ret = room->ReviveUserByCoin(user, req.m_0d);   // ORIG -0xc(%ebp)
        // ORIG +0x61：`cmpl $0x0,-0xc; jle 0-岛; mov -0xc,%eax`（技6/7 尾置 if 岛）。
        if (ret > 0)
        {
            return ret;
        }
        return 0;
    }
    return 0x11;
}
// ORIG 0x0821b098 W _ZN28DisPatcher_UdpCharacteristic13partyPingStatEP5CUserRtS2_
// （报告 DisPatcher_UdpCharacteristic/partyPingStat.md 汇编+C 全文转写，wave2）：
// Party_Ping 统计包（0xfa7,0xe）：+0xa=bad、+0xc=bad+total，经统计代理 SendPacket。
#include "DisPatcherCore/partyPingStat.inc"
{
    Packet_Party_Ping_Statistic pkt;
    pkt.m_dungeonBadPing = bad;                 // ORIG movzwl (%eax)
    pkt.m_dungeonTotal = (unsigned short)(bad + total);   // ORIG `mov 0x10(bad),%edx; mov 0x14(total),%eax; lea` bad 先
    // 同 pvpPingStat：链式调用不留 proxy 槽（ORIG `lea -0x16(%ebp)` 帧 0x24）。
    register char* pPkt = reinterpret_cast<char*>(&pkt);
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy(user->GetServerGroup())->SendPacket(pPkt, 0xe);
    return 0;   // ORIG 0x0821b098 尾 `mov $0x0,%eax` ⇒ int 返回 0
}

// ORIG 0x0821afbc W _ZN28DisPatcher_UdpCharacteristic11pvpPingStatEP5CUserRtS2_
// （报告 pvpPingStat.md 全文转写，wave2）：Pvp_Ping 包（0xfa8,0xe）同型。
#include "DisPatcherCore/pvpPingStat.inc"
{
    Packet_Pvp_Ping_Statistic pkt;
    pkt.m_pvpBadPing = bad;
    pkt.m_pvpTotal = (unsigned short)(total + bad);
    // ORIG +0x2f：&pkt 物化一次直落 callee-saved %ebx（跨两 call 存活，
    // 无栈槽）→ register 局部实证（c6root 探针 register T* 形态）。
    // ORIG：proxy 结果 %eax 短程直落（无槽）→ register 声明。
    // 修正：ORIG `lea -0x16(%ebp),%ebx` + proxy 返回 %eax 3 参直落（无具名 proxy
    // 局部 → 返回值不被搬离 %eax）。
    register char* pPkt = reinterpret_cast<char*>(&pkt);
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy(user->GetServerGroup())->SendPacket(pPkt, 0xe);
    return 0;   // ORIG 0x0821afbc 尾 `mov $0x0,%eax` ⇒ int 返回 0
}

// ORIG 0x0821b02a W _ZN28DisPatcher_UdpCharacteristic15fairPvpPingStatEP5CUserRtS2_
// （报告 fairPvpPingStat.md 全文转写，wave2）：Fair_Pvp_Ping 包（0x27ed,0xe）同型。
#include "DisPatcherCore/fairPvpPingStat.inc"
{
    Packet_Fair_Pvp_Ping_Statistic pkt;
    pkt.m_badPing = bad;
    pkt.m_total = (short)(total + bad);
    // 同 pvpPingStat：链式调用不留 proxy 具名槽（否则挤低 pkt 基址 4B，ORIG
    // `lea -0x16(%ebp)` 帧 0x24）。
    register char* pPkt = reinterpret_cast<char*>(&pkt);
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy(user->GetServerGroup())->SendPacket(pPkt, 0xe);
    return 0;   // ORIG 0x0821b02a 尾 `mov $0x0,%eax` ⇒ int 返回 0
}
// ORIG 0x081c632a T _ZN23DisPatcher_SelectCharac4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_SelectCharac/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x156f, PRETTY, 0, 0)。
#include "DisPatcherCore/read_5.inc"
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
#include "DisPatcherCore/send.inc"
{
    // ORIG 0x081c6826 T：param+4 == 0x7fffffff → void early-return（call 尾
    // 显式 jmp + je→nop 空块岛实证）；!=0 时 SendCmdErrorPacket(4, (uchar)值)。
    stSelectCharac& out = static_cast<stSelectCharac&>(param);
    if (out.m_errNo == 0x7fffffff)
        return;
    if (out.m_errNo != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)4, (unsigned char)out.m_errNo);
}

// ORIG 0x081c6380 T _ZN23DisPatcher_SelectCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0x1587,pretty,err,acc_id)。
// resetSaveFlags → 视图状态/上次登录判定 → 综合PvP频道抑制 → exchange 命中计数 →
// 回填 MSG+0xe/+0x12（_Charac_info.m_field14b8/m_field14b7）→ 九路 DB/ES 请求 →
// [b28d-29] N82：帧 0x1a0 vs 0x1b4 + 槽平移，调用点全等  (ORIG 0x081c6380 insn 320->323)
// 清上次登录 → 死塔最佳记录/注册日期 DB 请求 → LimitNpcBuyItemInfoAll 监视包。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 283（ORIG 320 / ours 323），残差=N7/N82 帧与 callee-saved 角色/P168a④ bool 槽化/P211/N18a 中转/store 序/P30/N4 槽；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_4.inc"
{
    stSelectCharac& out = static_cast<stSelectCharac&>(param);

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

    // [G5-ProcB] ORIG 0x081c638e param 槽 -0x10（函数头）→ 0x081c6405
    // `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`：msg 视图具名局部在两级 errNo
    // 检查之后、resetSaveFlags() 之前初始化（函数头声明会多一条 store 且槽基下移）。
    MSG_CHARAC_SELECT& req = *reinterpret_cast<MSG_CHARAC_SELECT*>(&msg);

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
        // P48：O0 无 CSE，ORIG 两字段各触发一次 vector::operator[] 调用（禁缓存 info）。
        req.m_0e =
            user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))].m_field14b8;
        req.m_12 =
            user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))].m_field14b7;
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
// [b28d-29] N93附 + N18a：尾块布局差 + `mov 0xe(%eax),%eax; mov %eax,%edx` 单段直落不可造  (ORIG 0x081c6ca0 insn 160->159)
// Reset 后填 acc/charac/school/tag 区；双职业热键装载；队列 5(有效上次登录)/2(DB_Q)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 23（ORIG 160 / ours 159），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBCharac.inc"
{
    // [b28c-L9] fresh 复验 ndiff 24→19；p167 调用点差集 32=32（0 missing/0 extra）。
    // 全部残差 = CStreamGuard EH 尾块布局跨版本不可翻：ORIG 0x081c6dxx 正常路径
    // `mov $1,%ebx; jmp Ltail` 跳到**函数最末**的共享 D1+epilogue，unwind 块居中；
    // 4.4 把 D1+epilogue 就地内联在正常路径、unwind 块压到最末（少 1 jmp + 8:8 块序
    // rotate，difflib 放大为 18）。语义/gap/槽位集全等 ⇒ 按 §6.2 判语义完成、byte N 封顶。
    // [b23S3C] ORIG 帧 0x20 仅 guard -0x14/sig -0xc 两槽：Acquire 右值直落 ctor
    // 实参（无 stream 槽）；guard 写流 4 处均 `(*guard)->operator<<`（deEv call
    // 080c8c46 + lsEi call 080c8c56，P48 每次重取 operator*）；GetInBuffer 走
    // operator-> 链（ptEv 080c8c4e，P103）；双 job 无具名槽（ebx 跨 call 缓存，
    // +0x192）⇒ 嵌套实参直调。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x173d),
        true);
    (*guard)->operator<<(7);
    (*guard)->operator<<(static_cast<int>(user->GetUID()));
    user->DisableCharacInfo(CACHE_CHARAC);
    if (user->IsValidLastLoginCharac())
    {
        (*guard)->operator<<(user->GetLastLoginChannelNo());
        (*guard)->operator<<(user->GetLastLoginCharacNo());
    }
    SIG_LOAD_CHARAC* sig =
        guard.operator->()->GetInBuffer<SIG_LOAD_CHARAC>();
    sig->m_characData.Reset();                    // CHARAC_DATA::Reset W 0x822ec9e
    sig->m_accId = user->get_acc_id();
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_characData.m_schoolNo = user->GetSchoolNo();   // ORIG 写 SIG+0x682
    sig->m_tagCharacData.clear();                 // TAG_CHARAC_DATA::clear W 0x822ed84
    sig->m_tagCharacNo = msg.m_0e;
    sig->m_strikerSkillIndex = msg.m_12;
    user->LoadCharacTypeHotKey(
        user->get_character_job(
            static_cast<unsigned int>(user->GetLastLoginCharacNo())),
        user->get_character_job(sig->m_characNo));
    // [b28e-L5] C05-① 复查实测**不命中**（已回退）：套用 Ticket/Skill 的「两分支各自
    // `return 1;`」形后本件 our_insns 159→161（ORIG 160）且 TU 四数不变（仍 DIFF）
    // ⇒ 本件残差主体是 N18a（`mov 0xe(%eax),%eax; mov %eax,%edx` 单段直落）而非尾块布局，
    // 保持原形（单尾 return 1）不再试形。
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
// SIG 缓冲首字 = charac_no（SigTypes_C.h SIG_LOAD_TICKET::m_characNo 权威字段；
// 视图 cast 已按 P0「无具名局部」律删除：ORIG 该 store 为右值内联直写）。
// [b28d-29] N93附/P222：同 RequestDBSkill 尾块布局差（4.4 就地 epilogue vs 4.1 jmp 尾岛）  (ORIG 0x081c727c insn 120->119)
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 55（ORIG 120 / ours 123），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBTicket.inc"
{
    // [b28c-L9] fresh 复验 ndiff 21→18；p167 25=25 全清。残差同 RequestDBCharac：
    // guard D1 共享尾块与 unwind 块的相对发射序（4.1 `jmp tail` / 4.4 fall-through）
    // ⇒ 跨版本 EH 块序 N，语义完成。
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
    // ORIG 081c73a6/081c73ab（报告 RequestDBTicket.md）：`call get_charac_no;
    // mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`——无 vNo 局部、
    // 无视图指针，get_charac_no 右值经 %edx 直落 sig 首字（P0 双向对应律）。
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    // [C05-① 复查] ORIG 0x081c73dd/0x081c7400 两分支均为 `mov $0x1,%ebx; jmp <共享尾块>`
    // （尾块 0x081c7422 = guard D1Ev + `mov %ebx,%eax` + epilogue），且**全函数无 res 栈槽**
    // ⇒ 原码是两分支各自 `return 1;`（不引入具名 res 局部，P0）；4.1 用 %ebx 汇聚返回值。
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
        return 1;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// ORIG 0x081c7bac T _ZN23DisPatcher_SelectCharac20RegistTimerSchedulesEP5CUser
// （报告 RegistTimerSchedules.md）：G_TimerQueue()->InsertTimer(OBJ_TYPE_0, uid,
// [b28d-29] NEAR=N7/N82附：两 callee-saved 角色对调（ORIG getCurCharacNo->%esi / get_unique_id->%edi，4.4.7 恒反向）；源码已为内联实参形，P248 三档试尽不翻（4.4.6 后端可翻但本波禁改路由）  (ORIG 0x081c7bac insn 34->35)
// TIMER_MESSAGE_0x51, 0, unique_id, characNo) 后恒返 1。
// [b28d-solo-DisPatcherCore] 复核 NEAR：ndiff 8（ORIG 34 / ours 34），残差=N82 槽/块序；N 封顶（详见上方 b28d-29 归因）。
#include "DisPatcherCore/RegistTimerSchedules.inc"
{
    // [b28c-L9] fresh 复验 ndiff 8→4：唯一残差 = 两 callee-saved 角色对调
    // （ORIG getCurCharacNo→%esi / get_unique_id→%edi，4.4 恒 %edi/%esi 反向；
    // 实参装载 `%esi,0x18(%esp)`↔`%edi,0x14(%esp)` 随之互换）
    // ⇒ N82附「多实参 call 首值 callee-saved 寄存器对调」，源形不可择。
    // ORIG 0x081c7bac：实参从右到左求值缓存（getCurCharacNo→esi /
    // get_unique_id→edi / GetUID→ebx），单行嵌套调用复现栈帧（sub $0x2c）。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, static_cast<int>(user->GetUID()),
        TIMER_MESSAGE_0x51, 0, user->get_unique_id(),
        user->getCurCharacNo());
    return 1;
}

// ORIG T 0x081c79f2（RequestBlackCount.md）：SIG 0xb3 → CACHE_BLACK_COUNT；memset 0x10。
#include "DisPatcherCore/RequestBlackCount.inc"
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
        return 1;
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 1;
    }
}

// ORIG T 0x081c6ee6：直接尾调 RequestDBInventory_forchina。
#include "DisPatcherCore/RequestDBInventory.inc"
{
    // P31 帧差：ORIG sub $0x28 → dead 局部 0x10。
    volatile char dead_10[0x10]; (void)dead_10;
    RequestDBInventory_forchina(user, msg);
}

// ORIG T 0x081c7c1c（RequestDBInventory_forchina.md）：SIG 8 → CACHE_INVENTORY；
// BigStreamPool；memset 0x1544c 后 acc/charac/tagInven 清理/+0x8←MSG.m_0e。
// [b28d-29] setcc-fold：ORIG 尾 bool 物化岛（mov $1,%esi/mov $0,%ebx + test），4.4 恒折单条 mov/直分支  (ORIG 0x081c7c1c insn 137->134)
// 尾部两分支均返回 1（ORIG esi 未初始化路径经 test ebx 归一为恒真）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 8（ORIG 137 / ours 133），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBInventory_forchina.inc"
                                                          MSG_CHARAC_SELECT& msg)
{
    // [b28c-L9] fresh 复验 ndiff 14→6；p167 27=27 全清。残差 = EH 寄存器角色：
    // ORIG unwind 路用 %esi 承载返回值、%ebx 承载异常指针（`mov $1,%esi` ×2 +
    // `test %ebx,%ebx; je`），4.4 单寄存器 %ebx 复用 ⇒ N7/N18a 跨版本分配，语义完成。
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
        return 1;
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 1;
    }
}

// ORIG T 0x081c70c2（RequestDBQuest.md）：SIG 10 → CACHE_QUEST；BigStreamPool；
// memset 0x7608 后仅写 characNo。
#include "DisPatcherCore/RequestDBQuest.inc"
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
        return 1;
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 1;
    }
}

// ORIG T 0x081c7436（RequestDBDungeonClear.md）：SIG 12 → CACHE_DUNGEON_CLEAR；
// StreamPool；memset 0x1810 后 characNo@0 / accId@4。
#include "DisPatcherCore/RequestDBDungeonClear.inc"
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
        return 1;
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 1;
    }
}

// ORIG T 0x081c7836（RequestDBKillMonsterInfo.md）：SIG 0x282 → CACHE_KILL_MONSTER；
// BigStreamPool；memset 0xafcc 后 characNo 写于尾部 +0xafc8。
#include "DisPatcherCore/RequestDBKillMonsterInfo.inc"
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
        return 1;
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 1;
    }
}

// ORIG T 0x081c7602（RequestDBETC.md）：SIG 0x20 → CACHE_ETC；BigStreamPool；
// memset 0x14b60；serverGroup/channelNo ← CEnvironment；tagEtc 清理；
// [b28d-29] P254/N18a：字段->实参两段中转（ORIG mov mem,%eax; mov %eax,%edx）与块序差；语义集合全等  (ORIG 0x081c7602 insn 152->154)
// job 预写 combo 视图 m_job（SIG 内绝对偏移 +0x14ab0）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 41（ORIG 152 / ours 153），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBETC.inc"
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
    // [b28e-L5] C05-① 复查实测**不命中**（已回退）：本件 ORIG 尾部同样是两分支
    // `mov $0x1,%ebx; jmp 0x081c7822` 共享尾块，但套形后 our_insns 153→155（ORIG 152）
    // 且 TU 四数不变 ⇒ 残差主体为 P254/N18a 中转与帧差，保持原形。
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
// [b28d-29] N93附/P222：尾块布局（ORIG 先 jmp 尾岛再统一 epilogue，4.4 就地内联 epilogue），槽集合与调用点全等  (ORIG 0x081c6f08 insn 120->119)
// memset 0x414 后仅写 characNo@0；队列 5(有效上次登录)/2(DB_Q)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 55（ORIG 120 / ours 123），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBSkill.inc"
{
    // [b28c-L9] fresh 复验 ndiff 21→18；p167 25=25 全清。残差 = guard D1 共享尾
    // 块序（同 RequestDBCharac），跨版本 EH 布局 N，语义完成。
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
    // ORIG 081c7032/081c7037（报告 RequestDBSkill.md）：`call get_charac_no;
    // mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`——值经 %edx 中转、
    // sig 槽 reload 后直落首字（SigTypes_C.h m_characNo 权威字段，无局部视图）。
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    // [C05-① 命中形，同 RequestDBTicket] ORIG +0x161/+0x1a6 两分支均 `mov $0x1,%ebx;
    // jmp <共享尾块 0x081c7082：guard D1Ev + `mov %ebx,%eax` + epilogue>`，全函数无 res
    // 栈槽 ⇒ 原码两分支各自 `return 1;`（无 res 具名局部，P0）。实测 RequestDBTicket
    // 同法 120=120（TU ae +1 / diff −1）。
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
        return 1;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;   // guard dtor 后 `mov %ebx,%eax`（技39 EH-ebx）
}

// [b28d-TakeDispatchCore] 真缺口修复（P183 地址重物化 + 调用链）：① CStreamGuard 链
// `guard.operator*()->operator<<(x)`×2 + `guard.operator->()->GetInBuffer<T>()`
// （ORIG 0x081c0511/0x081c0539 de、0x081c054a pt；原直写 `guard <<`/`guard.GetInBuffer`
// 少 2 de + 1 pt）；② m_field78 处**重新** call G_CDataManager() 并重算 imul 0x7dc
// （ORIG 0x081c064c；原两处共用 jobRec ⇒ MISSING _Z14G_CDataManagerv ×1）。
// p167 5缺→0缺（CALLSITE CLEAR 38/38；余 C1↔C2/D1↔D2 同址别名对=Main 2026-09-09 裁定豁免、
// memcpy=N78）；ndiff 204→112（ORIG 267 / ours 273，gap −6 形态代价）；
// TU 三数 211/639/2/75/190 → 212/639/2/74/190。
// ORIG T 0x081c041e _ZN23DisPatcher_CreateCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0x79c,pretty,err,acc_id)。
// GM 透传 setCharacNameFilterPass → BigStreamPool 取流构造 SIG_CREATE_CHARAC
// （memset 0x5354）→ 填 accid/slot/name/+0x23 → 活动角色占用则 m_errNo=0x14 →
// 角色创建表（CDataManager::m_characList + job*0x7dc）回填 → 双技能树初始技能 →
// [b28d-29] N82/P211：帧 0x3c vs 0x30 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081c041e insn 267->258)
// SP/makeGift/SSN 检查 → 战士(slot9) 连技 → lockCreateCharac 成功投递 DB_Q。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 204（ORIG 267 / ours 259），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P211/N18a 中转/store 序/P30；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_5.inc"
{
    stCreateCharac& out = static_cast<stCreateCharac&>(param);

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

    // [G5-ProcB] ORIG 0x081c04d4 `mov 0x10(%ebp),%eax; mov %eax,-0x24(%ebp)`：
    // msg 视图具名局部在 setCharacNameFilterPass 之后初始化（函数头声明会多一条
    // store 并把槽基整体下移）。
    MSG_CREATE_CHARACTER& req = *reinterpret_cast<MSG_CREATE_CHARACTER*>(&msg);

    // [G5-ProcB] ORIG 0x081c04ef Acquire → 0x081c04f4 `movl $0x1,0x8(%esp)` /
    // 0x081c04fc `mov %eax,0x4(%esp)` / 0x081c0500 `lea -0x30(%ebp),%eax;(%esp)`：
    // Acquire 返回值经 %eax 直传 CStreamGuard 构造实参，ORIG 无 `stream` 具名槽。
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x7ab), true);
    // ORIG 0x081c0511/0x081c0539 每次 << 先 operator*()（de 0x080c8c46）再 operator<<
    guard.operator*()->operator<<(4);             // SIG 类型 4
    guard.operator*()->operator<<(static_cast<int>(user->GetUID()));
    SIG_CREATE_CHARAC* sig = guard.operator->()->GetInBuffer<SIG_CREATE_CHARAC>();
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
    // P183 地址重物化：ORIG 0x081c0614 取一次基址供 m_field26+memcpy（`add $0x2` 复用），
    // 0x081c064c 在 m_field78 处**重新** call G_CDataManager() 并重算 imul 0x7dc 全链
    // （我方原先两处共用 jobRec ⇒ p167 MISSING _Z14G_CDataManagerv ×1）。
    const unsigned char* jobRec =
        reinterpret_cast<const unsigned char*>(G_CDataManager()->m_characList) +
        // [G5-ProcB] ORIG 0x081c061f/0x081c0657 均 `mov -0x24(%ebp),%eax;
        // movzbl 0xd(%eax),%eax; movsbl %al,%edx` ⇒ 本处 slot 取自 **msg**+0xd
        // （req.m_slot，signed char 语义两段扩展），非 sig+0x22。
        static_cast<int>(static_cast<char>(req.m_slot)) * 0x7dc;
    sig->m_field26 = *reinterpret_cast<const unsigned short*>(jobRec);
    // N84 实测反例：结构体整体赋值在 4.4.7 下退化为逐字节序列（ours 273→310、ndiff 112→118）
    // ⇒ 保留 memcpy 调用（ORIG 0x081c0641 的 rep movsl 属 N78，不可翻）。
    memcpy(sig->m_jobData, jobRec + 2, sizeof(sig->m_jobData));
    sig->m_field78 =
        (reinterpret_cast<const unsigned char*>(G_CDataManager()->m_characList) +
         static_cast<int>(static_cast<char>(req.m_slot)) * 0x7dc)[0x7d4];
    addSkillOnCreateCharacter(
        reinterpret_cast<_Mastered_skill*>(sig->m_masteredSkillA),
        // [G5-ProcB] ORIG 0x081c0695 `mov -0x20(%ebp),%eax; movzbl 0x22(%eax),%eax;
        // movsbl %al,%eax` ⇒ 此处才是 sig+0x22（符号扩展两段）。
        static_cast<int>(static_cast<char>(sig->m_slot)));
    addSkillOnCreateCharacter(
        reinterpret_cast<_Mastered_skill*>(sig->m_masteredSkillB),
        static_cast<int>(static_cast<char>(sig->m_slot)));
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

// [b28d-29] setcc-fold + N82：ORIG 0x081c08cf 短路 bool 物化岛（mov $1/$0 + 二次 test）4.4 恒折 setne；cMyTrace 槽 -0x34 vs -0x24（4.4 复用，帧 0x44 vs 0x34）  (ORIG 0x081c0830 insn 140->136)
#include "DisPatcherCore/dtor_DisPatcher_CreateCharac.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 15（ORIG 140 / ours 135），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error_1.inc"
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
    // ORIG +0x0a2：嵌套 if（无 eventing 槽）。+0xb8..+0xd4 bool→int 归一化双
    // 常量岛 = `!= 0` 结果二次 test；4.1 形。残 call *%edx↔call *%eax = N7。
    if (restrictEvt != 0)
    {
        // [b28d-29] P226 语句表达式实测劣化（136→141），已回退。
        // ORIG 0x081c08cf 段为 4.1 短路 bool 物化岛（mov $1/$0 + 二次 test）；
        // 4.4 恒折 setne 直分支（setcc-fold 负例）⇒ N 封顶。call *%edx↔*%eax = N7。
        // [b28d-solo-DisPatcherCore] 复核：具名 bool 局部形实测劣化（ndiff 15→34，
        // 帧 0x34→0x34 且多 4 条槽往返）⇒ 已回退；维持 setcc-fold N 封顶。
        if ((*(bool (**)(void*, int))(*(char**)restrictEvt + 0x34))(
                (void*)restrictEvt, 0))
        {
            int limit = restrictEvt->getLimitCreateCharacter(user->get_charac_count());
            if (limit != 0)
            {
                return limit;
            }
        }
    }
    if (user->get_charac_count() >= (int)user->getCharacSlotLimit())
    {
        return 4;
    }
    if (user->get_charac_count() == 0x24)
    {
        // [b28e-L5] P227 链式实测**无收益**（本件 diff 63→63），恢复两语句形。
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
#include "DisPatcherCore/check_state.inc"
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
// 本 TU 局部视图（MoveMapMsgView 同法）：+0xe 的 4B nameLen 为真实 int 字段
// （ORIG 081c0297 `mov 0xe(%eax),%eax` 单步 disp 证据），冻结头 m_padE[4] 的
// 等偏移/等宽强类型化，sizeof(0x38) 不变；已同步列 PacketTypes 字段化请求给 Main。
#pragma pack(push, 1)
struct MSG_CREATE_CHARAC_VIEW
{
    char m_head[0xd];   // +0x0
    unsigned char m_slot;  // +0xd
    int m_nameLen;      // +0xe（ORIG read 081c02db get_intERi 目标）
    char m_name[0x1e];  // +0x12..+0x2f
    char m_padTail[8];  // +0x30..+0x37
};
#pragma pack(pop)
#include "DisPatcherCore/read_6.inc"
{
    // 入口具名视图引用：ORIG +0x06 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` —
    // msg 指针唯一槽 -0xc（所有读走该槽 reload，技13 声明位置=落栈时机）。
    MSG_CREATE_CHARAC_VIEW& v = *reinterpret_cast<MSG_CREATE_CHARAC_VIEW*>(&msg);
    memset(&v, 0, 0x30);
    if (!packet.get_byte(*reinterpret_cast<char*>(&v.m_slot)))  // +0x0d（uchar→char& 适配）
    {
        return LineFunc(0x749,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG +0x6c..+0x80：两独立装载两次判（先无符号上限 `cmp $0xb; jg` 岛、
    // 再重载 `test %al; jns` pass）= `a>11 || (char)a<0` 短路\|\|形。
    if ((signed char)v.m_slot > 0xb || (signed char)v.m_slot < 0)
    {
        return LineFunc(0x74b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (v.m_slot == 0xb)
    {
        return LineFunc(0x763,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x0e 名字长度 int（m_padE[4] 起点，ORIG 逐字节读同一槽）
    // ORIG 081c0297 `mov 0xe(%eax),%eax` 单步 disp ⇒ 该 4B 区为真实 int 字段。
    // 冻结头 PacketTypes.h 内 m_padE[4] 无法字段化（已报 Main），本 TU 用同地址视图。
    if (!packet.get_int(v.m_nameLen))
    {
        return LineFunc(0x766,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (v.m_nameLen < 4 || 0x12 < v.m_nameLen)
    {
        return LineFunc(0x76a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(v.m_name, 0x1d, v.m_nameLen))
    {
        return LineFunc(0x76e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(v.m_name[0x1d]))                 // +0x2f 末字节标志
    {
        return LineFunc(0x780,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (v.m_name[0x1d] < 0)
    {
        return LineFunc(0x782,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "DisPatcherCore/send_1.inc"
{
    stCreateCharac& p = *reinterpret_cast<stCreateCharac*>(&param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)5, (unsigned char)(p.m_errNo & 0xff));
}

// ORIG T 0x081c0a1a（makeGift.md）：按创建槽位（=职业索引）发放初始武器/衣服与
// 职业赠礼道具。find_item 失败时仅置标志 0；命中则 Inven_Item 预填 +0x02 索引后
// [b28d-29] P205/P30：ORIG 单条 scaled `mov <A>(,%eax,8)`，4.4 拆 add+scaled；字段直读 vs 缓存差，语义集合全等  (ORIG 0x081c0a1a insn 227->223)
// 经 CItem::make_item（vtable+0x08）填充并整体拷贝入 SIG 槽位。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 158（ORIG 225 / ours 221），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/makeGift.inc"
{
    // [b28d] P0/P183：ORIG 无 `job` 具名局部——0x081c0a24 / 0x081c0a49 / 0x081c0aab /
    // 0x081c0b75 / 0x081c0dd8 五处各自重发
    // `mov 0x10(%ebp),%eax; movzbl 0x22(%eax),%eax; movsbl %al,%eax`。
    CItem* weapon = G_CDataManager()->find_item(
        CREATECHARACTEREQUIP[static_cast<int>(static_cast<signed char>(sig->m_slot)) * 2]);
    CItem* clothes = G_CDataManager()->find_item(
        CREATECHARACTEREQUIP[static_cast<int>(static_cast<signed char>(sig->m_slot)) * 2 + 1]);
    if (weapon == 0 || clothes == 0)
    {
        sig->m_equipFindOk = 0;                       // ORIG SIG+0x5340；落入赠礼段
    }
    else
    {
        sig->m_equipFindOk = 1;
        Inven_Item tmpWeapon;
        tmpWeapon.m_addInfo =
            CREATECHARACTEREQUIP[static_cast<int>(static_cast<signed char>(sig->m_slot)) * 2];
        weapon->make_item(tmpWeapon);
        sig->m_weaponItem = tmpWeapon;                        // 0x3d 字节 → SIG+0x4e7b

        Inven_Item tmpClothes;
        tmpClothes.m_addInfo = CREATECHARACTEREQUIP[
            static_cast<int>(static_cast<signed char>(sig->m_slot)) * 2 + 1];
        clothes->make_item(tmpClothes);
        sig->m_clothesItem = tmpClothes;                      // 0x3d 字节 → SIG+0x4ef5
    }

    // ORIG +0x233 起的赠礼段：无论装备是否命中都会执行。
    int giftId = 0;   // ORIG 0x081c0dcd `movl $0x0,-0x10(%ebp)` 预清（P198①）
    switch (static_cast<int>(static_cast<signed char>(sig->m_slot)))
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

#include "DisPatcherCore/dtor_DisPatcher_DeleteCharac.inc"
// ORIG 0x081c14f0 T _ZN23DisPatcher_DeleteCharac11check_errorEP5CUserR8MSG_BASE
// [b28d-29] N82/P0：msg 形参副本落 -0x18（ORIG -0x14 直读形），帧 0x44 vs 0x40，调用点全等  (ORIG 0x081c14f0 insn 199->198)
// 报告：docs/class_func_reports/DisPatcher_DeleteCharac/check_error.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 76（ORIG 199 / ours 197），残差=N7/N82 帧与 callee-saved 角色/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error_2.inc"
{
    if (user->get_state() != 2)
    {
        return -1;
    }
    if (user->isLockedCreateCharac())
    {
        return 0x13;
    }
    // [G5-MiscC] P28/声明序：ORIG 0x081c14f8/0x081c1517 两次 state 检查（`call get_state`
    // 与 `call isLockedCreateCharac`）**之后**才在 0x081c1530 落 msg 视图具名槽
    // （`mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)`）⇒ msg 视图声明必须后置于前两条检查
    // （我方原把 req 声明放函数首，槽写出现在 0x081c150x 之前的 -0x18）。
    MSG_DELETE_CHARACTER& req = *reinterpret_cast<MSG_DELETE_CHARACTER*>(&msg);
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
        // [b28e-L5] P227 全表达式临时链**实测劣化**（本函数 114→177 diff 行）：
        // 本处 ORIG 的 varargs 是 ctor **后**求值（两语句形），链式反而改变装载序 ⇒
        // 恢复两语句形（与 ORIG 0x081c1a0x 段一致）。
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
    // [b28e-L5] P196/轴B 真语义分叉修正：ORIG 0x081c16aa `movzbl 0x1246(%eax),%eax;
    // cmp $0x1,%al; setg %al`（**有符号** setg），我方原写无符号比较产 `seta`
    // ⇒ 值域 ≥0x80 时两侧分叉（0x80 起 ORIG 视为负而跳过异常链）⇒ 按 ORIG 加
    // (signed char) 读法（字段本型不动）。
    if ((signed char)user->m_characList[(int)req.m_slot].m_characLinkType > 1)
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
#include "DisPatcherCore/check_state_1.inc"
{
    if (user->get_state() != 2)
    {
        return LineFunc(0x170,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "DisPatcherCore/read_7.inc"
{
    // 视图：+0x0e int 名长（ORIG 0x81c... `mov -0xc(%ebp),%eax; mov 0xe(%eax),%eax` 单级字段
    // 装载）+0x12 名字区（`add $0x12`）。单一指针局部 -0xc（原 nameLen 引用局部多余槽）。
    #pragma pack(push, 1)
    struct View
    {
        char m_head[0xd];
        signed char m_slot;
        int m_nameLen;
        char m_name[0x1e];
        char m_pad30[8];
    };
    #pragma pack(pop)
    static_assert(sizeof(View) == 0x38, "DeleteCharac read view");
    View* req = reinterpret_cast<View*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req->m_slot)))
    {
        return LineFunc(0xa26,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req->m_nameLen))
    {
        return LineFunc(0xa29,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(req->m_name, 0x1e, req->m_nameLen))   // +0x12 名字区
    {
        return LineFunc(0xa30,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "DisPatcherCore/send_2.inc"
{
    // ORIG 0x081c14b8：指针局部 -0xc；条件与实参各自独立读 m_errNo（两次 field load），
    // 非 0 → SendCmdErrorPacket(cmd=6, (uchar)err)。
    stDeleteCharac* pParam = static_cast<stDeleteCharac*>(&param);   // ORIG -0xc(%ebp)
    if (pParam->m_errNo != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)6,
                                 (unsigned char)pParam->m_errNo);
    }
}

// ORIG 0x081c1182 T _ZN23DisPatcher_DeleteCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0xa41,pretty,err,acc_id)。
// RemoveCharacMercenaryInfo → StreamPool 取流构造 SIG_DELETE_CHARAC（memset 0x14）→
// characNo==0 则 m_errNo=0x15 并 Free 回流 → 填公会 id → lockDeleteCharac 成功投递
// [b28d-29] N82/P211：帧 0x5c vs 0x50 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081c1182 insn 216->210)
// DB_Q 并打 cMyTrace（accid 十进制串 / m_characNo / m_name），失败 Free + LineFunc(0xa68)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 134（ORIG 216 / ours 210），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_6.inc"
{
    stDeleteCharac& out = static_cast<stDeleteCharac&>(param);
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

    // [b28d] P28：ORIG 0x081c1204 才 `mov 0x10(%ebp),%eax; mov %eax,-0x20(%ebp)`
    // （msg 引用槽在 check_error/两级 errNo 检查之后初始化），非函数头。
    MSG_DELETE_CHARACTER& req = *reinterpret_cast<MSG_DELETE_CHARACTER*>(&msg);
    // [b28d] P0/P183：ORIG 无 `slot` 具名局部——每个使用点重新发
    // `mov req,%eax; movzbl 0xd(%eax),%eax; movsbl %al,%eax`（4 处，共 +8 条）。
    user->RemoveCharacMercenaryInfo(
        static_cast<unsigned int>(user->get_charac_no(static_cast<char>(req.m_slot))));

    // [G5-ProcB] ORIG 0x081c124a Acquire → 0x081c124f `movl $0x1,0x8(%esp)` /
    // 0x081c1257 `mov %eax,0x4(%esp)` / 0x081c125b `lea -0x3c(%ebp),%eax;(%esp)`：
    // Acquire 返回值经 %eax 直传 CStreamGuard 构造实参，ORIG 无 `stream` 具名槽
    // （旧文 `Stream* stream = ...` 多 store/load 两条且槽基下移 4）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xa4d), true);
    // ORIG（Ghidra 0x81c13xx 序列 de→op<<→de→op<<→pt→GetInBuffer）：operator* 显式链形。
    guard.operator*()->operator<<(5);                  // SIG 类型 5
    guard.operator*()->operator<<(static_cast<int>(user->GetUID()));
    SIG_DELETE_CHARAC* sig = guard->GetInBuffer<SIG_DELETE_CHARAC>();
    memset(sig, 0, 0x14);
    sig->m_accId = user->get_acc_id();
    sig->m_characNo =
        static_cast<unsigned int>(user->get_charac_no(static_cast<char>(req.m_slot)));
    if (sig->m_characNo == 0)
    {
        out.m_errNo = 0x15;                       // 槽位无角色
        GlobalData::s_stream_pool->Free(guard);
        return 0;
    }
    sig->m_guildId = static_cast<unsigned int>(
        user->GetGuildIdBeforeDeleteCharacter(static_cast<char>(req.m_slot)));

    if (!user->lockDeleteCharac())
    {
        GlobalData::s_stream_pool->Free(guard);
        return LineFunc(0xa68,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0xa6e, 0);
    trace("[%s][%d] accid(%s) delete charac(%d) %s",
          __PRETTY_FUNCTION__,
          0xa6e,
          NumberToString(user->get_acc_id(), 0),
          // P48：m_characNo/m_name 两字段各触发一次 vector::operator[]（禁缓存 info）。
          static_cast<int>(
              user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))].m_characNo),
          reinterpret_cast<const char*>(
              user->m_characList[static_cast<int>(static_cast<char>(req.m_slot))].m_name));
    return 0;
}

#include "DisPatcherCore/dtor_DisPatcher_ReturnToSelectCharacter.inc"
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 77 / ours 77 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/dispatch_sig_3.inc"
{
    // [b28d-solo-DisPatcherCore] P292/P168a 档②：ORIG 尾
    // `movzbl -0x9(%ebp),%eax; xor $0x1,%eax; test %al,%al; je` ⇒ flag 是
    // **bool** 且原码为裸 `if (!flag)`（uchar 会产 test/sete/test 或 xorb 槽化）。
    bool flag = true;                           // ORIG -0x9(%ebp)
    if (user->get_state() <= 2)
    {
        flag = false;
    }
    else if (user->getCurCharacVill() == 7)
    {
        flag = false;
    }
    else
    {
        user->ReturnToSelectCharacList(false);
    }
    {   // 技12 显式块：dtor 正常收尾，免 4.4 EH-ebx 返回缓存（ORIG 尾 call; mov $0,%eax）。
        PacketGuard guard;
        guard.put_header(1, 7);
        guard.put_byte(flag);
        // [b28d-solo-DisPatcherCore] 裸 `if (!flag)`（bool 字段形，免 chk 局部槽化）。
        if (!flag)
        {
            guard.put_byte(0x13);
        }
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
}
#include "DisPatcherCore/read_8.inc"
{
    return 0; // ORIG 0x08233490：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}

// ORIG 0x0823349a W _ZN34DisPatcher_ReturnToSelectCharacter7processEP5CUserR8MSG_BASER9ParamBase
// push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，
// 全部逻辑位于 dispatch_sig(0x081fd25c，见 docs/class_func_reports/
// DisPatcher_ReturnToSelectCharacter/dispatch_sig.md；按分工 dispatch_sig 保持 stub)。
#include "DisPatcherCore/process_7.inc"
                                                ParamBase& /*param*/)
{
    return 0;
}

#include "DisPatcherCore/dtor_DisPatcher_GetUserInfo.inc"
#include "DisPatcherCore/check_error_3.inc"
{
    // ORIG 0x081c3e7c：get_state(user)==0 → -1，否则 0。
    if (user->get_state() == 0)
        return -1;
    return 0;
}
// ORIG 0x081c3d46 T _ZN22DisPatcher_GetUserInfo4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_GetUserInfo/read.md 逐块映射）：get_short→msg+0xd、
// get_byte→msg+0xf；失败各走 LineFunc(0xf5b/0xf5d, PRETTY, 0, 0)。
#include "DisPatcherCore/read_9.inc"
{
    // ORIG 0x081...：无指针局部，实参内联 `mov 0x10(%ebp),%eax; add $0xd`（帧 0x18）。
    if (!packet.get_short(reinterpret_cast<MSG_USERINFO_REQ*>(&msg)->m_targetId))   // +0x0d
    {
        return LineFunc(0xf5b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<MSG_USERINFO_REQ*>(&msg)->m_slot))        // +0x0f
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
#include "DisPatcherCore/process_8.inc"
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

#include "DisPatcherCore/dtor_DisPatcher_RecoverStamina.inc"
// ORIG 0x081c3f4a T _ZN25DisPatcher_RecoverStamina11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_RecoverStamina/check_error.md 逐块映射）：state!=3 → 99；
// SecuProtectionField Check(user,0x2f) 非 0 直返；否则 getCurCharacR 为空 → -1。
#include "DisPatcherCore/check_error_4.inc"
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
#include "DisPatcherCore/read_10.inc"
{
    return 0; // ORIG 0x081c3ea4：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/send_3.inc"
{
    stOnRecoverStaminaParam& out = static_cast<stOnRecoverStaminaParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 9);
    // ORIG 0x081c...：je 目标为 put 1 块、fall-through 为 put 0 块（O0 按
    // 源码分支布局）→ 首分支写 !=0 的 Error 路径。
    if (out.m_errNo != 0)
    {
        guard.put_byte(0);
        guard.put_byte(out.m_errNo);
    }
    else
    {
        guard.put_byte(1);
        guard.put_int(user->getCurCharacInvenR()->get_money());
    }
    guard.finalize(true);
    user->Send(guard);
}

// ORIG 0x081c3eae T _ZN25DisPatcher_RecoverStamina7processEP5CUserR8MSG_BASER9ParamBase
// err>0：SendCmdErrorPacket(cmd=9, err&0xff) 后返回 -1；err<0：LineFunc(0xf9d,pretty,0,acc_id)
// 并返回其值；err==0：param+0x4 = user->serviceOfShusia()，返回 err。
#include "DisPatcherCore/process_9.inc"
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
    // ORIG 0x081c3eae：`mov 0x14(%ebp),%eax; mov %eax,-0xc(%ebp)` = out 引用槽物化
    //（技13 声明位置=落槽时机；技21 errNo 先声明占远槽 -0x10、out 近槽 -0xc）；
    // 写形 `mov -0xc(%ebp),%edx; mov %eax,0x4(%edx)`（slot 载入非 %ebx）。
    stOnRecoverStaminaParam& out = static_cast<stOnRecoverStaminaParam&>(param);
    out.m_errNo = user->serviceOfShusia();
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
#include "DisPatcherCore/dtor_DisPatcher_ReqPeer.inc"
// ============================================================================
// ORIG T 0x081eed08 _ZN18DisPatcher_ReqPeer12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_ReqPeer/dispatch_sig.md，1379 insn）
// state∈{3,6,8} 门禁 → 三字段读取 → 发起方门禁（私摊/分解店/幽灵/整合 PvP）→
// target 查找与 solo-play/游戏选项拒绝 → check_party_teleport_user(reqType,
// user,target,true) → 按 reqType 分派：0=组队、1=交易、2=PvP 房间、
// 3=战争房间、4=袭击请求、5/7 与 6/8=expert_job。成功尾部统一
// SetReceivedRequestType(target, reqType)。MSG_REQ_TO_PEER 仅作 SoloPlay
// 拒绝包上下文（类型未建模，零缓冲视图【推断】）。
// §6.2 巨件核销：P167 194/194 [0缺0多]；gap 1379→1379/1387；残差归因 N82/P112
// （4.1 不复用槽 vs 4.4 复用 + 块排布，diff 形态计数对称无缺类，详见
// docs/asm_translation_patterns.md 负例表）。无真修条目。
// [b28d-29] P30/N4 + N82：ORIG 结构体基址 lea -0x50 + add $0xd/0xf/0x10 两步，4.4 折独立 lea；语义集合全等  (ORIG 0x081eed08 insn 1379->1387)
// ============================================================================
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 686（ORIG 1377 / ours 1385），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_4.inc"
{
    // ORIG top gate：三次独立 get_state reload（==3/==6/==8 各自 `call
    // _ZN5CUser9get_stateEv`），不缓存 `st`——调用点计数须为 3。
    if (user->get_state() != 3 &&
        user->get_state() != 6 &&
        user->get_state() != 8)
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
        // ORIG 两次独立 reload（==6 / ==7）。
        if (target->get_state() == 6 || target->get_state() == 7)
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
            // ORIG 6 次 `_ZNK getCurCharacVill` 内联（无缓存局部）；语义等价于
            // （双 7 且 power 侧异）∨（恰一 7）→ 拒 0x13。逐调用点展开匹配计数。
            // [G5-SigA] P0/P328：ORIG 0x081ef0xx 为 `call getCurCharacVill;
            // cmp $0x7,%al; jne`（直接 8 位比较、**无具名槽**）。原
            // `signed char selfVill` + `bool bothSeven` 两个具名量多出
            // `mov %al,-0x2e(%ebp); cmpb $0x7,-0x2e(%ebp)` 与
            // `mov %al,-0x2d(%ebp); cmpb $0x0,-0x2d(%ebp)` 共 4 条（fn_diff_tu 实测）。
            if (user->getCurCharacVill() == 7 &&
                target->getCurCharacVill() == 7)
            {
                if (user->getPowerSide() != target->getPowerSide())
                    return PEER_ERR3(user,requestType,0x13), 0;
            }
            else if ((user->getCurCharacVill() == 7 ||
                      target->getCurCharacVill() != 7) &&
                     (user->getCurCharacVill() != 7 ||
                      target->getCurCharacVill() == 7))
            {
                // 同 7 / 同非 7 → 通过
            }
            else
            {
                return PEER_ERR3(user,requestType,0x13), 0;
            }
        }
        CParty* selfParty = static_cast<CParty*>(user->GetParty());
        // ORIG 两个独立 0x13 错误调用点（count>3 与 !CanDo 各一）。
        if (selfParty != 0)
        {
            if (selfParty->get_member_count() > 3)
                return PEER_ERR3(user,requestType,0x13), 0;
            if (!selfParty->CanDoPartyActionAssalutState())
                return PEER_ERR3(user,requestType,0x13), 0;
        }
        if (user->CheckInParty() && target->CheckInParty())
            return PEER_ERR3(user,requestType,0x12), 0;
        if (!GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            int curSec = GlobalData::s_systemTime_.getCurSec();
            // ORIG 三处 `_ZN10expert_job10CAlchemist17GetAssaultPreventEP5CUserPb`
            // 第二参为 (bool*)0x0。
            if (expert_job::CAlchemist::GetAssaultPrevent(
                    user, static_cast<bool*>(0)))
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
                expert_job::CAlchemist::GetAssaultPrevent(
                    target, static_cast<bool*>(0)))
                return PEER_ERR3(user,requestType,0xfa), 0;
            if (selfParty != 0 &&
                selfParty->CheckMemberLastTryAssaultTime() &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    target, static_cast<bool*>(0)))
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
        // ORIG local_28 延迟错误码：非交易态先试同 state 直连成功包，失败累计
        // 码值后统一 fall-through 发包。两处 `Get()+isRestricted`、两处
        // `get_state`、两处 `CheckInTrade`。
        int errCode = 1;
        bool restricted =
            ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                user, static_cast<RestrictCategory::Enum>(1), 6);
        if (!restricted)
            restricted =
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
        if (!target->CheckInTrade())
        {
            if (user->get_state() == target->get_state())
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
            errCode = 0x13;
        }
        if (target == 0)
            errCode = 3;
        else if (target->CheckInTrade())
            errCode = 0x12;
        PEER_ERR3(user,requestType,errCode & 0xff);
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
    case 7:
    {
        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                user, static_cast<RestrictCategory::Enum>(1), 8))
            return PEER_ERR4(user,requestType,0xd1,pgRes), 0;
        int secuErr =
            GlobalData::s_pSecuProtectionField->Check(
                user, static_cast<SECURITY_PROTCTION>(0x21));
        if (secuErr != 0)
            return PEER_ERR4(user,requestType,secuErr & 0xff,pgRes), 0;
        if (target->GetCurCharacExpertJobType() != 1)
            return PEER_ERR4(user,requestType,0x17,pgRes), 0;
        GlobalData::s_ExpertJobMgr->OnRequestPeer(
            user, target,
            static_cast<ENUM_PEER_REQUEST_TYPE>(
                static_cast<int>(requestType)),
            pgRes);
        break;
    }
    case 6:
    case 8:
    {
        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                user, static_cast<RestrictCategory::Enum>(1), 8))
            return PEER_ERR4(user,requestType,0xd1,pgRes), 0;
        int secuErr =
            GlobalData::s_pSecuProtectionField->CheckOpposite(
                user, static_cast<SECURITY_PROTCTION>(0x20), target);
        if (secuErr != 0)
            return PEER_ERR4(user,requestType,secuErr & 0xff,pgRes), 0;
        if (user->GetCurCharacExpertJobType() != 1)
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
#include "DisPatcherCore/read_11.inc"
{
    return 0; // ORIG 0x08232dde：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/process_10.inc"

#include "DisPatcherCore/dtor_DisPatcher_ResPeer.inc"
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
// [b28d-29] P30/N4 + N82：帧 0xec vs 0xf0 + ORIG 两步 add 寻址 vs 4.4 独立局部槽；语义集合全等  (ORIG 0x081f088e insn 2061->2029)
// ============================================================================
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 2844（ORIG 2055 / ours 2023），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P168a④ bool 槽化/P211/N18a；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_5.inc"
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

    // [b28d] P17/P261：ORIG 0x081f09b3 起为 switch 二分决策树
    // （cmp $6/je → 09f0、cmp $6/jg → 09c4、cmp $5/je → 09d0、cmp $7/je → 09d0、
    // cmp $8/je → 09f0、jmp default 0a10），非 if-else 链。
    // [G5-SigA] P0：ORIG 0x081f09xx 为 `call GetReceivedRequestType; movsbl %al,%eax;
    // cmp $0x6,%eax`（**无具名槽**，直接 32 位提升后比较）。原具名
    // `unsigned char recvType` 多出 `mov %al,-0x99(%ebp); movzbl -0x99(%ebp),%eax`
    // 2 条，且该 1B 槽使帧 0xec→0xf0（fn_diff_tu 实测 sub $0xf0 vs ORIG $0xec）。
    switch ((char)user->GetReceivedRequestType())
    {
    case 5:
    case 7:
        if (requestType != 5 && requestType != 7)
            return 0;
        break;
    case 6:
    case 8:
        if (requestType != 6 && requestType != 8)
            return 0;
        break;
    default:
        if ((char)user->GetReceivedRequestType() != requestType)
            return 0;
        break;
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

        if (GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            // ORIG 6 次 `_ZNK getCurCharacVill` 内联（无缓存局部）；两个 0x13
            // 错误包各带一个 clear(local_bc)。语义 = （双 7 且 power 侧异）∨
            // （恰一 7）→ 拒；否则落到 LAB_081f12d8 组队逻辑。
            signed char selfVill = user->getCurCharacVill();
            bool bothSeven = selfVill == 7 && target->getCurCharacVill() == 7;
            if (bothSeven)
            {
                if (user->getPowerSide() != target->getPowerSide())
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
            }
            else if ((user->getCurCharacVill() == 7 ||
                      target->getCurCharacVill() != 7) &&
                     (user->getCurCharacVill() != 7 ||
                      target->getCurCharacVill() == 7))
            {
                // 同7 / 同非7 → 通过（落到组队逻辑）
            }
            else
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
        }
        else
        {
            // ---- 混沌袭击时间窗：4 个独立 0xfa 4-arg（PacketGuard&）错误点 ----
            int curSec = GlobalData::s_systemTime_.getCurSec();
            if (curSec < target->GetCurCharacChaosKillTime() + 600 &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    user, static_cast<bool*>(0)))
            {
                user->SendCmdPeerErrorPacket(
                    static_cast<ENUM_CMDPACKET>(0xb),
                    static_cast<ENUM_PEER_REQUEST_TYPE>(
                        static_cast<int>(requestType)),
                    static_cast<unsigned char>(0xfa), pgParty);
                break;
            }
            if (curSec < user->GetCurCharacChaosKillTime() + 600 &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    target, static_cast<bool*>(0)))
            {
                user->SendCmdPeerErrorPacket(
                    static_cast<ENUM_CMDPACKET>(0xb),
                    static_cast<ENUM_PEER_REQUEST_TYPE>(
                        static_cast<int>(requestType)),
                    static_cast<unsigned char>(0xfa), pgParty);
                break;
            }
            CParty* selfParty = static_cast<CParty*>(user->GetParty());
            if (selfParty != 0 &&
                selfParty->CheckMemberLastTryAssaultTime() &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    target, static_cast<bool*>(0)))
            {
                user->SendCmdPeerErrorPacket(
                    static_cast<ENUM_CMDPACKET>(0xb),
                    static_cast<ENUM_PEER_REQUEST_TYPE>(
                        static_cast<int>(requestType)),
                    static_cast<unsigned char>(0xfa), pgParty);
                break;
            }
            CParty* peerParty = static_cast<CParty*>(target->GetParty());
            if (peerParty != 0 &&
                peerParty->CheckMemberLastTryAssaultTime() &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    user, static_cast<bool*>(0)))
            {
                user->SendCmdPeerErrorPacket(
                    static_cast<ENUM_CMDPACKET>(0xb),
                    static_cast<ENUM_PEER_REQUEST_TYPE>(
                        static_cast<int>(requestType)),
                    static_cast<unsigned char>(0xfa), pgParty);
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
            // ORIG thunk：0x822fe4e=GetPartyIndex、0x80cc18e=G_CGameManager、
            // 0x8294f32=GetParty(int)（movswl %ax,%esi 实参）。
            CParty* party =
                G_CGameManager()->GetParty(target->GetPartyIndex());
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
#include "DisPatcherCore/read_12.inc"
{
    return 0; // ORIG 0x08232ea2：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/process_11.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPartyInfo.inc"
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮：返回 unsigned（与 int 字段异型，
// 复现 process 调用点 RHS-first+edx 中转）；返回类型不入 mangled/ABI。
#include "DisPatcherCore/check_error_5.inc"
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
#include "DisPatcherCore/read_13.inc"
{
    MSG_PARTY_SET_INFO& req = *reinterpret_cast<MSG_PARTY_SET_INFO*>(&msg);
    if (!packet.get_byte(req.m_titleIndex))
    {
        return LineFunc(0x10d9, __PRETTY_FUNCTION__, 0, 0);
    }
    if (req.m_titleIndex == 0)
    {
        if (!packet.get_int(req.m_titleLen))
        {
            return LineFunc(0x10dd, __PRETTY_FUNCTION__, 0, 0);
        }
        if (!packet.get_str(req.m_title, 0x20, req.m_titleLen))
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
#include "DisPatcherCore/send_4.inc"
{
    stSetPartyInfo& p = *reinterpret_cast<stSetPartyInfo*>(&param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc, (unsigned char)(p.m_errNo & 0xff));
}

// ORIG 0x081c4562 T _ZN23DisPatcher_SetPartyInfo7processEP5CUserR8MSG_BASER9ParamBase
// param+0x4 = check_error；>0 返回 0；<0 LineFunc(0x10f6,pretty,err,acc_id) 并返回其值；
// 无队：G_CGameManager()->GetParty() 空则 param=4 返回 0，否则 create_party +
// SetPartyInfoUI + send_party_info_to_all(party,0)；有队：非队长 param=8，
// 队长 SetPartyInfoUI + send_party_info_to_all(party,1)；均返回 0。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 96 / ours 96 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_12.inc"
{
    stSetPartyInfo& out = static_cast<stSetPartyInfo&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG `call; mov %eax,%edx; mov -0x18(%ebp),%eax;
    // mov %edx,0x4(%eax)`（RHS-first+edx 中转）由「unsigned 返回 + 无具名 err 局部」
    // 复现（探针 /tmp/p211probe.cpp b/c 形）；删 ec/dst 后 out 引用槽与 ORIG 一致。
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

    // ORIG 0x081c4562：槽链 out(-0x18)→info(-0x14)→party(-0x10)→nparty(-0xc)
    //（技21 声明序=远→近；技37 GetParty 结果独立具名槽 `mov %eax,-0xc; cmpl $0,-0xc`，
    // 非 party 复用——复用会让 4.4 合并槽号致全体错位 4B）。
    const MSG_PARTY_SET_INFO& info = *reinterpret_cast<const MSG_PARTY_SET_INFO*>(&msg);
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        CParty* nparty = G_CGameManager()->GetParty();
        if (nparty == 0)
        {
            out.m_errNo = 4;
            return 0;
        }
        nparty->create_party(user);
        nparty->SetPartyInfoUI(info);
        G_GameWorld()->send_party_info_to_all(nparty, 0);
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

#include "DisPatcherCore/dtor_DisPatcher_LeaveParty.inc"
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
#include "DisPatcherCore/check_error_6.inc"
{
    // ORIG 0x081c436e：party 落局部槽 -0xc 后 `cmpl $0x0,-0xc`（非寄存器 test）。
    CParty* pParty = (CParty*)user->GetParty();
    if (pParty == 0)
        return 0x12;
    return 0;
}
#include "DisPatcherCore/check_state_2.inc"
{
    if (user->get_state() != 3)
    {
        return LineFunc(0x208,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "DisPatcherCore/read_14.inc"
{
    return 0; // ORIG 0x081c424c：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/send_5.inc"
{
    stLeavePartyParam& p = *reinterpret_cast<stLeavePartyParam*>(&param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd, (unsigned char)(p.m_errNo & 0xff));
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
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 59 / ours 59 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_13.inc"
{
    stLeavePartyParam& out = static_cast<stLeavePartyParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG `mov %eax,%edx; mov -0x14(%ebp),%eax;
    // mov %edx,0x4(%eax)`（RHS-first+edx 中转）由「unsigned 返回 + 无具名 err/dst 局部」复现。
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

    // ORIG 0x081c4256：party 具名槽 -0x10（`mov %eax,-0x10; mov -0x10,%eax` 重载传参）
    // + leave_user 结果槽 -0xc（`mov %eax,-0xc; cmpl $1,-0xc; jne` 技9 具名槽融合形）。
    // 声明序 out→party→ret（技21 先声明占远槽）。
    CParty* party = static_cast<CParty*>(user->GetParty());
    int ret = party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);  // ORIG 不判空直接调用
    if (ret == 1)
    {
        out.m_errNo = 1;
    }
    pvp_assault::GetInstanceAssaultMgr()->ForcedMoveAtPowerWarPlace(user);  // ORIG 0x082350a0
    return 0;
}

#include "DisPatcherCore/dtor_DisPatcher_WalkoutParty.inc"
// ORIG 0x081cb062 T _ZN23DisPatcher_WalkoutParty11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_WalkoutParty/check_error.md 逐块映射）：user 空 →
// LineFunc(0x2053)；state==3 → 0；否则 -1。
#include "DisPatcherCore/check_error_7.inc"
{
    if (user == 0)
    {
        return LineFunc(0x2053,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x081cb062：`cmp $3; setne; test; je` = if(!=3) return -1; return 0。
    if (user->get_state() != 3)
        return 0xffffffff;
    return 0;
}
// ORIG 0x081caf42 T _ZN23DisPatcher_WalkoutParty4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_WalkoutParty/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x202b, PRETTY, 0, 0)。
#include "DisPatcherCore/read_15.inc"
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
#include "DisPatcherCore/send_6.inc"
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
#include "DisPatcherCore/process_14.inc"
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

    // ORIG +0x40：槽产生序为 msg 视图(-0x14) → param 视图(-0x10) → 立即
    // 写 param+4=0；两调用点各自内联重算 (int)(char)msg[0xd]（无缓存槽）。
    const MSG_WALKOUT_PARTY& req = *reinterpret_cast<const MSG_WALKOUT_PARTY*>(&msg);
    stWalkoutPartyParam& out = static_cast<stWalkoutPartyParam&>(param);
    out.m_errNo = 0;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        out.m_errNo = 0x13;
        return 0;
    }
    party->SendWalkout((int)(char)req.m_walkoutSlot, ENUM_WALKOUT_TYPE_0);
    out.m_errNo = party->walkout_member(user, (int)(char)req.m_walkoutSlot);
    return 0;
}

// [b28d-29] N82：帧 0x34 vs 0x58 + 条件极性折痕（je/jne），槽集合与调用点全等  (ORIG 0x081c9f6c insn 228->246)
#include "DisPatcherCore/dtor_DisPatcher_StartGame.inc"
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 332（ORIG 228 / ours 246），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error_8.inc"
{
    if (user->get_state() == 3 || user->get_state() == 0xb)
    {
        // state 合法，继续
    }
    else
    {
        return 0x7fffffff;
    }
    // [G5-MiscC] P0/P52：ORIG 0x081c9fad 只有**单**具名槽 -0x18
    // （`mov 0x14(%ebp),%eax; mov %eax,-0x18(%ebp)` 直存 param 本身，访问形
    //  `mov -0x18(%ebp),%eax; movl $0xffffffff,0x8(%eax)` = param+0x8）。
    // 我方原 `stStartGameParam& paramRef` + `int* local_18 = &paramRef.m_8` 是
    // 引用+指针两个具名（fresh 多 -0x38/-0x34 两条 mov）⇒ 合并为单个指针具名。
    stStartGameParam* local_18 = static_cast<stStartGameParam*>(&param);
    if (G_GameWorld()->IsPVPChannel() != false)
    {
        local_18->m_8 = 0xffffffff;
        return 8;
    }
    if (user->IsBusyDisjointStore() != false)
    {
        local_18->m_8 = 0xffffffff;
        return 0x13;
    }
    if (user->isCompetitionMercenary() != false)
    {
        local_18->m_8 = 0xffffffff;
        return 0xea;
    }
    CParty* pCVar5 = static_cast<CParty*>(user->GetParty());
    if (pCVar5 != 0)
    {
        // [G5-MiscC] P0/P183：ORIG 0x081ca032 与 0x081ca049 **各自重发**全链
        // `mov -0x14(%ebp),%eax; mov %eax,(%esp); call GetPartyTelePort;
        //  mov %eax,(%esp); call get_teleport_state`，teleport/state 均无具名槽
        // （我方 fresh 多 -0x28/-0x22/-0x21 三槽）⇒ 内联回链式右值。
        // 判据取 ORIG 极性：0x081ca047 `test %al,%al; js`（<0 为假）+
        // 0x081ca05c `cmp $0x3,%al; jg`（>3 为假）⇒ `>= 0 && <= 3`。
        if (static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort())
                    ->get_teleport_state() >= '\0' &&
            static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort())
                    ->get_teleport_state() <= '\x03')
        {
            // ORIG 0x081ca070 / 0x081ca083 同样各自重发全链（P183）。
            static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort())
                ->reset_teleport_data();
            static_cast<CPartyTelePort*>(pCVar5->GetPartyTelePort())
                ->send_teleport_status('\0');
        }
    }
    // [G5-MiscC] P0/P52：ORIG 0x081ca0a4 / 0x081ca0c0 / 0x081ca0fb 三处均为
    // `mov 0xc(%ebp),%eax; mov %eax,(%esp); call getCurCharacName; mov %eax,%ebx;
    //  call G_GameWorld; ...` —— pcVar7 无槽（我方 fresh 多 -0x2c），
    // GetLeftTimeOfForbiddenMove 结果亦无槽（0x081ca0de 直接
    // `test %eax,%eax; setne %al`，我方 fresh 多 -0x20）⇒ 全部内联回实参位
    // （P52：callee-saved 寄存器跨调用保活）。
    if (G_GameWorld()->IsForbiddenMove(user->getCurCharacName()) != false)
    {
        // [b28e-L5] C05④ 步骤2 实测**不命中**（已回退）：ORIG 0x081ca0de 为
        // `test %eax,%eax; setne %al; test %al,%al; je`（body 以 `return 0x44` 终止、
        // 表面符合「含 return」的 P225 命中前件），但整块换 `while (cond != 0)
        // { body; break; }` + 去具名 iVar3 后 our_insns **246→249**（ORIG 228）劣化
        // ⇒ 本件残差主体是 N82 帧差（0x34 vs 0x58）+ 条件极性折痕，保持原形。
        if (G_GameWorld()->GetLeftTimeOfForbiddenMove(user->getCurCharacName()) != 0)
        {
            local_18->m_8 = 0;
            return 0x44;
        }
        G_GameWorld()->EnableMove(user->getCurCharacName());
    }
    if (user->CheckInParty() != false && user->get_state() == 3)
    {
        pCVar5 = static_cast<CParty*>(user->GetParty());
        if (pCVar5 != 0)
        {
            // [G5-MiscC] P292 档①：ORIG 0x081ca16f 与 0x081ca181 均为
            // `call; xor $1,%eax; test %al,%al; je <假岛 0x081ca18f: mov $0,%eax>`
            // ⇒ 源码是 bool 取反 `!`（原 `== '\x01'` 变体不符，且我原先的
            // if/else 正条件形亦不符）。两跳同落假岛 ⇒ 判据 = `!A && !B`，
            // 0x081ca194 `test %al,%al; je 0x240` 即假（A 真或 B 真）跳过 0x15。
            if (!pCVar5->CheckMemberArea() && !pCVar5->is_quick_party())
            {
                local_18->m_8 = 0xffffffff;
                return 0x15;
            }
            // [G5-MiscC] P0/P52：ORIG 0x081ca1c2 的 getHackAnalyzer 结果只在
            // %eax 上跨实参布局（0x081ca1cd-0x081ca1f4 先直填 0x14/0x10/0xc/0x8(%esp)，
            // 最后才 `mov %eax,(%esp)` = this 最后求值），无具名槽 ⇒ 内联链式。
            if (excludeAddHackCount(user) != false)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, static_cast<WongWork::ENUM_HACKTYPE>(0x1f6), 1, 0, 0);
            }
            int iVar3b = G_GameWorld()->GetWorldMapIndex(user);
            if (0 < iVar3b)
            {
                CWorldMap* this_00 = G_CDataManager()->m_worldMapList.find_world_map(iVar3b);
                if (this_00 == 0)
                {
                    local_18->m_8 = 0xffffffff;
                    return 0x15;
                }
                // [G5-MiscC] P292 档①：ORIG 0x081ca25a `call; xor $1,%eax;
                // test %al,%al; je 0x296` ⇒ bool 取反 ⇒ `!IsOpenRevengeDungeon()`
                // （原 `!= '\x01'` 形不符）。
                if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
                {
                    // [G5-MiscC] P0/P52：ORIG 0x081ca261 `call getDungeonMinLevel;
                    // mov %eax,%ebx` + 0x081ca26e `call getMemberMinLevel;
                    // add $0x6,%eax; cmp %eax,%ebx; setge %al` —— 两结果均无槽
                    // ⇒ 内联回比较两侧（右值 getDungeonMinLevel 先求值，与 ORIG 序同）。
                    if (pCVar5->getMemberMinLevel() + 6 <= this_00->getDungeonMinLevel())
                    {
                        local_18->m_8 = 0xffffffff;
                        return 0xe;
                    }
                }
            }
            return 0;
        }
    }
    return 0;
}
#include "DisPatcherCore/read_16.inc"
{
    return 0; // ORIG 0x081c9f62：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/send_7.inc"
{
    stStartGameParam& out = static_cast<stStartGameParam&>(param);
    if (out.m_errNo == 0x7fffffff)
        return;
    if (out.m_errNo != 0)
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
#include "DisPatcherCore/excludeAddHackCount.inc"
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
// [b28d-29] N82：全帧槽号平移 4（ORIG 帧更浅），助记符序列与调用点全等  (ORIG 0x081ca2fe insn 129->133)
// 已组队且 state==3 时对所在 party game_start；均返回 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 55（ORIG 129 / ours 132），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_15.inc"
{
    stStartGameParam& out = static_cast<stStartGameParam&>(param);
    // ORIG +0x20：errNo store 双钉（技45）。
    // [b28d-solo-DisPatcherCore] P211：ORIG RHS-first+edx 中转（unsigned 返回 + 无具名局部）。
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x1e5b, __PRETTY_FUNCTION__, out.m_errNo, user->get_acc_id());
    }

    // ORIG +0x84 `movb $0x0,-0x15` dead bool 初始化（技8 变体）：hasDeathTower
    // 结果落 bool 槽；worldIdx 槽 -0x14。
    bool deathTower = false;
    int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
    if (worldIdx > 0)
    {
        CWorldMap* worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
        if (worldMap != 0)
        {
            deathTower = worldMap->hasDeathTower();   // ORIG +0xd3 `mov %al,-0x15`
        }
    }
    (void)deathTower;

    // [G5-ProcB] ORIG 0x081ca3d4 `movl $0x0,-0x10(%ebp)`（hasDeathTower 之后、
    // CheckInParty 之前）＋ 两个分支各 `mov %eax,-0x10(%ebp)`：原码是函数级
    // `CParty* party = 0;` 两个分支共用同一槽（旧文在两块内各声明一次 ⇒ 缺这条
    // 0 初始化且槽序/指令数偏移）。
    CParty* party = 0;

    // ORIG +0xe8 `test %al,%al; je +0x134`：inParty 体是 fall-through 块（技17/19），
    // `get_state()==3` 为 cmp+sete+test 物化（技9 直写 ==3 已实证产 sete）。
    if (user->CheckInParty())
    {
        if (user->get_state() == 3)
        {
            party = static_cast<CParty*>(user->GetParty());
            if (party != 0)
            {
                party->game_start(user);
            }
        }
        return 0;
    }
    {
        party = G_CGameManager()->GetParty();
        if (party == 0)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0x1e99,
                                  "cant party memory : you must free party memory!!!");
            return 0;
        }
        if (excludeAddHackCount(user))
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x1f6, 1u, 0u, 0u);
        }
        party->set_single_play(user);
        party->game_start(user);
    }
    return 0;
}

#include "DisPatcherCore/dtor_DisPatcher_SelectDungeon.inc"
// ORIG 0x081c7f32 T _ZN24DisPatcher_SelectDungeon11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_SelectDungeon/check_error.md 汇编+反编译 C 全文转写）：
// state∉{5,0xd} 或 IsBusyDisjointStore → 0x7fffffff；m_field10∉[0,2] 或
// m_dungeonType<0 → -1；m_field11∉[0,1] → -2；m10>0 且 m11>0 → 0x7fffffff；
// m10!=2 时要求队长（getManager()==user）、party state==1、!(m11==1&&is_quick_party)；
// 两段重复 party 检查（ORIG 0x0d9 与 0x0a9 两块），全部通过 → 0。
#include "DisPatcherCore/check_error_9.inc"
{
    if (user->get_state() != 5 && user->get_state() != 0xd)
    {
        return 0x7fffffff;
    }
    if (user->IsBusyDisjointStore())
    {
        return 0x7fffffff;
    }
    // ORIG 0x081...：msg→-0x14 spill 在 IsBusyDisjointStore 块 return 岛之后
    // （技巧13：具名局部落栈时机 = 声明位置）
    const MSG_SELECT_DUNGEON& req =
        *reinterpret_cast<const MSG_SELECT_DUNGEON*>(&msg);
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
#include "DisPatcherCore/read_17.inc"
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
// [b28d-29] N82/N93：槽号平移与块序（ORIG 无 -0x10 副本，4.4 多 mov 0xc(%eax) 重读），语义集合全等  (ORIG 0x081c8e9c insn 204->199)
}
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 35（ORIG 201 / ours 198），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/send_8.inc"
{
    stSelectDungeonParam* local_14 = &static_cast<stSelectDungeonParam&>(param);
    // [b28d] P223 多岛细则：ORIG 尾部有三个连续 nop 中继岛（0x081c9164/167/16a），
    // 三条守卫 je 各落一个 ⇒ 用三条不同源的负条件 goto 到连续尾空标签复现
    // （标签序=守卫序；4.4 单源裸 if 恒 jump-thread 成同一落点，gap=6）。
    if (local_14->m_4 == 0x7fffffff)
    {
        goto L_m4;
    }
    if (local_14->m_4 != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x10),
                                 static_cast<unsigned char>(local_14->m_4));
    }
    if (local_14->m_c == 0)
    {
        if (local_14->m_8 == 0)
        {
            goto L_party;
        }
    }
    {
        CParty* local_10 = static_cast<CParty*>(user->GetParty());
        if (local_10 == 0)
        {
            goto L_cparty;
        }
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
        // [b28d] ORIG 0x081c8fe9/0x081c9097 块布局：`m_8!=m_c` 时先判
        // `m_8!=0`（fall-through 到 user->Send 体），`m_8==0` 才跳去
        // `m_c!=0` 的 send_to_party 体 ⇒ 原码是 `else if(m_8!=0)` 在前、
        // `else if(m_c!=0)` 在后（P272 块序交换）。
        else if (local_14->m_8 != 0)
        {
            PacketGuard local_2c;
            local_2c.put_header(1, 0x10);
            local_2c.put_byte(0);
            local_2c.put_byte(local_14->m_8);
            local_2c.finalize(true);
            user->Send(local_2c);
        }
        else if (local_14->m_c != 0)
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
L_m4:;
L_party:;
L_cparty:;
}

// ORIG T 0x081c9172 _ZN24DisPatcher_SelectDungeon15checkBlueMarbleEP6CParty
// [b28d-29] AE：38=38 全等（同上重定位归一）  (ORIG 0x081c9172 insn 38->39)
// （checkBlueMarble.md 全文转写）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 38 / ours 38 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/checkBlueMarble.inc"
{
    if (party == 0)
    {
        return 0;
    }
    // ORIG +0x13：getManager()==0 → test/sete/test/je 物化（bool 判据，无
    // 具名槽）；IsSinglePlay(bool) 裸 test/je + `(... == 1) == true` 双层
    // 分支物化（c6root 探针 h14 逐条一致）。
    if (party->getManager() == 0)
    {
        return 0;
    }
    // [b28e-L5] 判据① 唯一残差：ORIG 0x081c….+0x5c 处 `je <0x5c>`（IsSinglePlay
    // false）与 `jne <0x5c>`（getMoveSpace!=1）**两跳同落** packed-false 岛，随后
    // `0x5c: mov $0,%eax; 0x61: test %al,%al; je <0x6c>; mov $1,%eax; jmp; 0x6c:
    // mov $0,%eax` 为 4.1 的 bool→int 归一岛；我方该 je 直落 0x6c（4.4 直跳折叠）。
    // 已试三目形（`return cond ? 1 : 0;`）实测 **ae -1 回退**（4.4 折成单层直跳，
    // 丢 getManager 二次调用路径）⇒ 恢复原双层 if 形，本处记 N（4.1 归一岛不可造）。
    if (party->IsSinglePlay())
    {
        if ((party->getManager()->getMoveSpace() == 1) == true)
        {
            return 1;
        }
    }
    return 0;
}

// ORIG 0x081c8102 T process（process.md 全文转写，含汇编分支次序）。
// §6.2 巨件核销：P167 95/95 [0缺0多]；gap 810→810/830；残差归因 N82/P112。
// 真修 1 条：死块 [HackUser] logFormat(0x198a) 误传 member 指针 → 按 ORIG
// [b28d-29] N82/P211：帧与参数副本槽（-0x90 vs -0x98）+ store 装载序，调用点全等  (ORIG 0x081c8102 insn 810->831)
// +0x13e 改 member->get_acc_id()（补齐缺失的 _ZNK5CUser10get_acc_idEv 调用点）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 840（ORIG 808 / ours 826），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_16.inc"
{
    stSelectDungeonParam& out = static_cast<stSelectDungeonParam&>(param);

    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x1970, __PRETTY_FUNCTION__, out.m_4, user->get_acc_id());
    }

    IsLightServer();   // ORIG 调用后丢弃结果

    // [G5-ProcB] ORIG 0x081c8110 param 槽 -0x90（函数头）→ 0x081c81a4
    // `mov 0x10(%ebp),%eax; mov %eax,-0x8c(%ebp)` msg 视图槽在 IsLightServer()
    // 之后才初始化。函数头先声明 req 会把槽基整体下移（我方 -0x9c/-0x98）。
    const MSG_SELECT_DUNGEON& req = *reinterpret_cast<const MSG_SELECT_DUNGEON*>(&msg);

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0 && party->getManager() != user)
    {
        out.m_4 = 0x7fffffff;   // 非队长选副本
        return 0;
    }

    // ORIG +0x0fc~+0x1be（asm 权威，Ghidra C 未渲染该支）：msg[0x11]==1 ⇒ 全队
    // get_user(i≤3)→logFormat(0x198a "[HackUser]...")→leave_user(3)→DisConnSig(0x18,1,0)。
    if (static_cast<unsigned char>(req.m_field11) == 1)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* member = party->get_user(i);
            if (member != 0)
            {
                LogManager::logFormat(
                    1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0x198a,
                    "[HackUser] dungeon hack user. (mid:%u)", member->get_acc_id());
                party->leave_user(member, (ENUM_PARTY_INFO_TYPE)3);
                member->DisConnSig((DISCONN_SIG)0x18, true, 0);
            }
        }
    }

    if (party != 0)
    {
        // CParty::m_battleField.m_bloodState(+0xcd8)（CParty.h public 区，具名访问）
        party->m_battleField.m_bloodState = (int)(char)req.m_field11;
    }

    if (user->isDisguiseCharac())
    {
        user->setDisguiseCharac(false, 0, 0);
    }

    if (party != 0)
    {
        const int memberCount = party->get_member_count();
        cMyTrace trace(__PRETTY_FUNCTION__, 0x19bf, 0);
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
                1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0x19df,
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
                    1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0x19fa,
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
                    // P48：ORIG 本区两次独立 get_user(i)（首实参一次、getHackAnalyzer 一次）。
                    CUser* hackArg = party->get_user(i);
                    party->get_user(i)->getHackAnalyzer()->addServerHackCnt(
                        hackArg, (WongWork::ENUM_HACKTYPE)0x1f8, 1u, 0u, 0u);
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

    // P48：ORIG 两处独立 getDungeonKind（==1 与 ==3 各一）。
    if (dungeon->getDungeonKind() == 1)
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
    if (dungeon->getDungeonKind() == 3)
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
                    party->get_user(i)->SetCurRevengeDungeonCount(0);   // P48：ORIG 第二次独立 get_user
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
        // ORIG 读 CParty::m_battleField.m_bloodState(+0xcd8) 字段与 1 比较（非 GetPartyType() 方法计算值）
        const bool skipFatigue = (hasDeathFlag != 0) ||
                                 (party->m_battleField.m_bloodState == 1) ||
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

#include "DisPatcherCore/dtor_DisPatcher_SendMess.inc"
// dispatch_sig 真实实现见文件尾第二批段（ORIG T 0x081f3540）。
#include "DisPatcherCore/read_18.inc"
{
    return 0; // ORIG 0x08232f66：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}

// ORIG 0x08232f70 W _ZN19DisPatcher_SendMess7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部聊天业务（type 分派/表情改写/敏感词 Filter/消耗品扣减/community+monitor 监视包/
// DB_InsertPoliceSaveChatting/GameWorld::send_chat_msg）内嵌于
// dispatch_sig(0x081f3540，报告 docs/class_func_reports/DisPatcher_SendMess/dispatch_sig.md)。
#include "DisPatcherCore/process_17.inc"

#include "DisPatcherCore/dtor_DisPatcher_DeleteItem.inc"
// [b28d-Main·检查点14] 原 TU 局部视图 DeleteItemMsgTailView 已删除：
//   m_checkSum 现为 MSG_DELETE_ITEM 本体字段（+0x4bf，PacketTypes.h）。
// ORIG 0x081c1a6c T _ZN21DisPatcher_DeleteItem11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_DeleteItem/check_error.md 逐块映射）：背包校验和
// getCurCharacInvenCheckSum 与 msg+0x4bf 不一致 → addServerHackCnt(0x25b,1,0,0)；恒返 0。
#include "DisPatcherCore/check_error_10.inc"
{
    // ORIG：msg 指针先落 -0xc，call 后 reload + `mov 0x4bf(%edx),%edx`。
    const MSG_DELETE_ITEM* pMsg =
        reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);   // ORIG -0xc(%ebp)
    if (user->getCurCharacInvenCheckSum() != (*(&pMsg))->m_checkSum)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25b, 1u, 0u, 0u);
    }
    return 0;
}
// ORIG 0x081c17c0 T _ZN21DisPatcher_DeleteItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_DeleteItem/read.md（汇编+反编译 C 全读）
#include "DisPatcherCore/read_19.inc"
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
// [b28d-29] setcc-fold + N18a：bool 物化（xor $1 vs cmpb/setne）与单段直落 callee-saved 不可造；语义集合全等  (ORIG 0x081c1ad6 insn 154->160)
}
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 7（ORIG 154 / ours 157），残差=P211/N18a 中转/store 序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/exercise_mode.inc"
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
        // [b23S3C] P48 去缓存：ORIG +0x077/+0x09d 两次独立求值
        // getCDungeonMGr()->getDungeon()->limitOfStackableItemInTower（无 dungeon 局部）；
        // !=1 用 `if(x) r=true` 形（P194）给出 ORIG xor $0x1 编码。
        // [b28d-solo-DisPatcherCore] P292 档①：CDungeon::limitOfStackableItemInTower
        // 已 bool 化（Main 代落）⇒ 原码为裸 `!f()`（ORIG 0x081c1b68 `call; xor $0x1,%eax;
        // test %al,%al; je`）与裸 `f()`（0x081c1b8e `call; test %al,%al; je`）。
        if (!tower->getCDungeonMGr()->getDungeon()->limitOfStackableItemInTower())
        {
            limitNotOne = true;
        }
        if (tower->getCDungeonMGr()->getDungeon()->limitOfStackableItemInTower())
        {
            for (int i = 0; i < (int)(signed char)req.m_count; ++i)
            {
                CItem* item = G_CDataManager()->find_item(req.m_itemIdx[i]);
                if (item != 0 && item->get_index() == 0x1973)
                {
                    // ORIG lea 0xb58(tower) == &tower->m_timerKeys[7]（CDeathTower.h:254
                    // m_timerKeys[10]@+0xb3c；0xb3c+7*4=0xb58；同 CDeathTower.cpp:807 口径）
                    // ORIG 0x81c1bf0 调嵌套版 CDeathTower::stMapMonsterKillChecker_t
                    // ::setUseSkillMaterial（W 0x0823464c）——勿用全局版（布局不同）。
                    reinterpret_cast<WongWork::CDeathTower::stMapMonsterKillChecker_t*>(
                        &reinterpret_cast<CDeathTowerTimerKeyView*>(tower)->m_timerKeys[7])
                        ->setUseSkillMaterial(GlobalData::s_systemTime_.getCurTickCount());
                }
            }
        }
    }

    // [b23S3C] ORIG +0x151..+0x183 用分支岛 `if(cond){body;return 1;}return 0;`
    // （P56/P198），无 skip 布尔物化——禁中间局部，直接 if 包裹 body 并 return。
    if ((party->m_battleField.m_bloodState == 1) &&
        (user->get_state() == 5 || limitNotOne))
    {
        out.m_9 = 0;
        // [Lane5] P266 cast 形实测劣化（add $0xd + movzbl (%eax) 两步）⇒ 维持直读；
        // 残余 `mov %eax,%edx` 中转需 PacketTypes.h m_itemSpace char→unsigned char（S2 待 Main）。
        out.m_8 = req.m_itemSpace;
        for (int i = 0; i < (int)(signed char)req.m_count; ++i)
        {
            out.m_slotsD2[i] = req.m_position[i];
            out.m_counts19c[i] = req.m_countArr[i];
            out.m_slotsA[i] = req.m_opType[i];
        }
        return true;
    }
    return false;
}
#include "DisPatcherCore/restricted_check.inc"
{
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 1))
    {
        return 0xd1;
    }
    // ORIG 0x081c1cf6：Check 结果落 -0xc 槽，`cmpl $0` 判后回读返回（帧 0x28）。
    int nRet = (int)GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x1d);
    if (nRet != 0)
        return nRet;
    return 0;
}
#include "DisPatcherCore/condition_check.inc"
{
    const MSG_DELETE_ITEM& req = *reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);
    if (type == INVEN_TYPE_INVEN && req.m_position[idx] <= 9)
    {
        return 0x17;   // 普通背包槽位 <=9 受保护
    }
    // ORIG +0x3f..+0x5c：position[idx] 短整表达式作普通实参直传（P200，
    // `movzwl field,%eax; movswl %ax,%ebx` 跨 GetInvenType 存活于 %ebx）。
    // 禁具名中间局部/register asm（退化 cwtl+mov 两步）；直接内联为实参。
    if (user->CheckItemLock(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_itemSpace),
            (int)req.m_position[idx]))
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
#include "DisPatcherCore/send_9.inc"
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
// §6.2 巨件核销：P167 61/61 [0缺0多]；gap 31→1；残差归因 N82/P112。
// 真修 1 条：GetInvenSlot 返回 4.4 直构缺 ORIG +0x25e~+0x327 的 17 条逐字段
// [b28d-29] N82/P211：帧 0x100 vs 0x110 + 参数副本槽 + store 装载序；bool 条件 4.4 test 直判 vs 4.1 cmpb，语义集合全等  (ORIG 0x081c1dda insn 651->653)
// 拷贝（sret 匿名槽→具名局部）→ 改两段式（slotItem 收 sret + 拷贝入 item）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 400（ORIG 651 / ours 653），残差=P168a④ bool 槽化/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_18.inc"
{
    stDeleteItemParam& out = static_cast<stDeleteItemParam&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0xb8e, __PRETTY_FUNCTION__, out.m_4, user->get_acc_id());
    }

    if (exercise_mode(user, msg, param))
    {
        return 0;
    }

    // [G5-ProcB] ORIG 0x081c1e94 `mov 0x10(%ebp),%eax; mov %eax,-0x34(%ebp)`：
    // msg 视图具名局部在 exercise_mode 之后才初始化（非函数头）。函数头声明会多
    // 一条 store 并把 -0x3c 槽基整体下移 4（我方帧 0x110 vs ORIG 0x100）。
    const MSG_DELETE_ITEM& req = *reinterpret_cast<const MSG_DELETE_ITEM*>(&msg);

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
        // ORIG +0x25e~+0x327：GetInvenSlot 先 sret 到匿名槽(-0xa9)，再逐字段
        // 拷入具名局部 item(-0xe6)；4.4 直构缺 17 条拷贝 ⇒ 两段式复现。
        Inven_Item slotItem = ((CInventory*)user->getCurCharacInvenR())
                                  ->GetInvenSlot(invenType, (int)req.m_position[i]);
        Inven_Item item = slotItem;

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
            cMyTrace trace(__PRETTY_FUNCTION__, 0xc2a, 5);
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
            return LineFunc(0xc48, __PRETTY_FUNCTION__, 0, user->get_acc_id());
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
#include "DisPatcherCore/dtor_DisPatcher_MoveItem.inc"
// ORIG 0x081c57a0 T _ZN19DisPatcher_MoveItem11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_MoveItem/check_error.md 汇编+反编译 C 全文转写）：
// m_field14<0 → -1。toSpace(space)>5 时仅 ==0xe、<=5 时 ==4/5 或 ==3 走限制/锁定检查；
// ==4/5 或 0xe：CheckItemTradeLimitCountToMoveByUser 假 → 0x72；==3 且 fromSpace==7：
// GetInvenSlot(3,fromPos) 非空且 fromPos>0x8b → 需锁定检查；需检查者 CheckItemLock
// [b28d-29] P251：ORIG 0x5/0xe/0x3 值域为稀疏 switch 决策树，源码为 if 链；槽往返随 P0 属 N82 族  (ORIG 0x081c57a0 insn 108->103)
// 命中 → 0xd5；否则 0。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 69（ORIG 107 / ours 102），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error_11.inc"
{
    const MSG_MOVE_ITEMSPACE& req =
        *reinterpret_cast<const MSG_MOVE_ITEMSPACE*>(&msg);
    if ((int)req.m_field14 < 0)
    {
        return -1;
    }
    char needLock = 0;
    // [b28e-L5] P212/P331 实测不可翻（已回退）：ORIG bool 局部占深槽 -0x4e
    // （0x081c57ec `movb $0x1,-0x4e`；0x081c5803 `lea -0x4e,%edx` 传址），而 4.4
    // 无论块内声明（-0x11）或提至函数头（仍 -0x11，且多一条 ORIG 无的
    // `movb $0x0,-0x11` 预清）都不可复现 ⇒ 归 4.1/4.4 块作用域槽分配差（P212 限定
    // 「混合尺寸区声明序不可控」），保持原形。
    // ORIG 0x081c57cf 决策树：cmp $0x5/jg、cmp $0x4/jge、cmp $0x3/je、cmp $0xe/jne
    // ⇒ 原码是 switch(寄存器直比，无槽，P251/P261)，叶块物理序 4/5/0xe 先、3 后。
    switch ((int)(char)req.m_toSpace)
    {
    case 4:
    case 5:
    case 0xe:
        needLock = 1;
        {
            bool movable = true;
            CheckItemTradeLimitCountToMoveByUser(
                *user, GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_fromSpace),
                (int)req.m_fromPos, movable);
            if (!movable)
            {
                return 0x72;
            }
        }
        break;
    case 3:
        if ((unsigned char)req.m_fromSpace == 7)
        {
            Inven_Item item = ((CInventory*)user->getCurCharacInvenR())
                                  ->GetInvenSlot(3, (int)req.m_fromPos);
            if (({ item.m_field0 != 0 && req.m_fromPos > 0x8b; }))
            {
                needLock = 1;
            }
        }
        break;
    default:
        break;
    }
    // ORIG 0x081c58a9：&& 物化岛（mov $1/jmp/mov $0/test/je）后再判，非直跳。
    if (({ needLock != 0 && user->CheckItemLock(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_fromSpace),
                (int)req.m_fromPos); }))
    {
        return 0xd5;
    }
    return 0;
}
// ORIG 0x081c54e0 T _ZN19DisPatcher_MoveItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_MoveItem/read.md（汇编+反编译 C 全读）
// 串行 10 次 get_*；任一失败即 LineFunc(行号, pretty, 0, 0)。msgPtr 局部落 -0xc(%ebp)。
#include "DisPatcherCore/read_20.inc"
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
#include "DisPatcherCore/send_10.inc"
{
    stMoveItemParam& p = static_cast<stMoveItemParam&>(param);
    PacketGuard guard;
    if (p.m_4 != 0x7fffffff)
    {
        guard.put_header(1, 0x13);
        // ORIG +0x4d `test; je` ⇒ 非零 4 字节回显块是 fall-through（技17 true 侧）。
        if (p.m_4 != 0)
        {
            guard.put_byte(0);
            guard.put_byte(p.m_4);
            guard.put_byte((char)p.m_8);
            guard.put_byte((char)p.m_10);
            guard.finalize(true);
            user->Send(guard);
        }
        else
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
    }
}


// ORIG 0x081c5904 T process（process.md 全文转写）。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 136（ORIG 192 / ours 196），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_19.inc"
{
    // 本 TU 同址视图（PvPTimeout::read ViewPacked 先例）：m_fromSpace/m_toSpace 以
    // unsigned char 读出 ORIG `movzbl 0xd/0x18(%eax),%edx` 单步 disp 折叠 + QI 直存
    // （冻结头 char 声明混号必两段）；偏移/宽度按 PacketTypes.h:908 布局注释。
    #pragma pack(push, 1)
    struct MoveItemSpaceView {
        char m_head[0xd];
        unsigned char m_fromSpace;   // +0x0d
        short m_fromPos;             // +0x0e
        unsigned int m_count;        // +0x10
        int m_field14;               // +0x14
        unsigned char m_toSpace;     // +0x18
        short m_toPos;               // +0x19
        unsigned int m_itemIdx;      // +0x1b
        char m_gap1f[4];             // +0x1f..0x22
        char m_expanda;              // +0x23
        char m_expandb;              // +0x24
    };
    #pragma pack(pop)
    const MoveItemSpaceView* req = reinterpret_cast<const MoveItemSpaceView*>(&msg);
    stMoveItemParam& out = static_cast<stMoveItemParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG value→%edx 先、槽地址→%eax 后
    // （RHS-first+edx 中转）=「unsigned 返回 + 无具名 err/dst 局部」形。
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        if (out.m_4 == 0xd5)   // 仅锁定类错误回显源/目的 space
        {
            out.m_8 = req->m_fromSpace;
            out.m_10 = req->m_toSpace;
        }
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x141b, __PRETTY_FUNCTION__, out.m_4, user->get_acc_id());
    }

    // ORIG 无 outSlot/ret 初始化写（ret 槽由 call 返回值直写 `mov %eax,-0x1c`）。
    int outSlot;
    int ret;
    // ORIG +0x0ad：uchar 装载 + 字节立即数 `cmp $0x3,%al` 直判（无 sext）。
    if ((req->m_toSpace == 3 || req->m_toSpace == 0x12) &&
        req->m_toPos > 9 && req->m_toPos < 0x16)
    {
        // 扩容区间移动（toSpace 3/0x12 且目的槽位 10..21）
        ret = user->exchange_interspace_expand(
            (int)(char)req->m_fromSpace, (int)req->m_fromPos,
            (unsigned long)req->m_count, (int)req->m_field14,
            (int)(char)req->m_toSpace, (int)req->m_toPos,
            (unsigned long)req->m_itemIdx,
            &outSlot, req->m_expanda, req->m_expandb);
    }
    else
    {
        ret = user->exchange_interspace(
            (int)(char)req->m_fromSpace, (int)req->m_fromPos,
            (unsigned long)req->m_count, (int)req->m_field14,
            (int)(char)req->m_toSpace, (int)req->m_toPos,
            (unsigned long)req->m_itemIdx,
            &outSlot);
    }

    if (ret == 0)
    {
        // 冻结头 m_fromSpace/m_toSpace 为 char；ORIG QI 直存 `movzbl mem,%edx`
        // 要求 RHS 与 m_8/m_10(uchar) 同型（probe /tmp/qi3 s4/s6），经同址
        // uchar 视图读取（偏移不变，§4.3 同型口径）。
        out.m_8 = req->m_fromSpace;
        out.m_a = req->m_fromPos;
        out.m_c = (int)req->m_field14;
        out.m_10 = req->m_toSpace;
        out.m_14 = outSlot;
    }
    else
    {
        // [G5-ProcB] ORIG 0x081c5b46 `mov -0x1c(%ebp),%edx; mov -0x20(%ebp),%eax;
        // mov %edx,0x4(%eax)` —— 单条 `out.m_4 = ret;` 即得「value 先载 edx、
        // 地址槽后载 eax」形。旧文用 `int rv = ret; stMoveItemParam* dst2 = &out;`
        // 造出 2 个具名槽（我方多 5 条 store/load、帧 0x7c vs ORIG 0x6c）。
        out.m_4 = ret;
        out.m_8 = req->m_fromSpace;
        out.m_10 = req->m_toSpace;
    }
    return 0;
}
#include "DisPatcherCore/dtor_DisPatcher_SortItem.inc"
// ORIG 0x081c9d64 T _ZN19DisPatcher_SortItem11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_SortItem/check_error.md 逐块映射）：user 空 →
// LineFunc(0x1d56)；state<=2 → -1；否则 0。
#include "DisPatcherCore/check_error_12.inc"
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
#include "DisPatcherCore/read_21.inc"
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
// [b28d-29] 已修 DIFF->AE：if-else 决策链改 switch（P17/P251/P261，ORIG 0x081c9ec6 稀疏 switch 树）  (ORIG 0x081c9dba insn 128->129)
// result==0 且 invenType 1/0/2/7 分别 SendItemLockListAvatar/Inven/Cargo/Creature。
#include "DisPatcherCore/send_11.inc"
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
        // [b28d-29] ORIG 0x081c9ec6 段为稀疏 switch 决策树（cmp$1/je case1 →
        // cmp$1/jg L2 → test/je case0 → jmp default；L2: cmp$2/je → cmp$7/je），
        // 非 if-else 链（P17/P251/P261：无状态槽 cmp 链 ⇒ 源形是 switch）。
        switch ((int)(char)p.m_invenType)
        {
        case 0:
            user->getCurCharacInvenR()->SendItemLockListInven();
            break;
        case 1:
            user->getCurCharacInvenR()->SendItemLockListAvatar();
            break;
        case 2:
            user->getCurCharacCargoR()->SendItemLockList();
            break;
        case 7:
            user->getCurCharacInvenR()->SendItemLockListCreature();
            break;
        default:
            break;
        }
    }
}


// ORIG 0x081c9c54 T process（process.md 全文转写）。
#include "DisPatcherCore/process_20.inc"
{
    // ORIG +0x06：check_error 最先（err→-0x14），msg(-0x10)/param(-0xc) 视图
    // 绑定发生在错误分支之后 —— 声明序决定 O0 槽深。
    const int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    const MSG_SORT_ITEM& req = *reinterpret_cast<const MSG_SORT_ITEM*>(&msg);
    stOnSortItemParam& out = static_cast<stOnSortItemParam&>(param);

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
#include "DisPatcherCore/dtor_DisPatcher_BuyItem.inc"
// ORIG 0x081be46a T _ZN18DisPatcher_BuyItem11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_BuyItem/check_error.md 汇编+反编译 C 全文转写）：
// checkValidShopItem(0x15,(ulong)0x0d) 失败 → OneADayShop 判定（是→0x11，否→
// addServerHackCnt 0x3c9+(-1)）；随后 CheckInTrade/getCurCharacR/isHackUser → 0x13、
// isRestricted(1,3) → 0xd1、state!=8 时 secu 0x22 非0返码、IsBusyPrivateStore → 0x3c、
// [b28d-29] setcc-fold + N82：ORIG 直分支常量岛（mov $0x11; jmp），4.4 折 sete/setne；槽号平移，语义集合全等  (ORIG 0x081be46a insn 136->149)
// OneADayShop 时 GetRepeatEvent(0x6d)->IsEventing(0) 为假 → 0x11；否则 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 39（ORIG 136 / ours 149），残差=setcc-fold bool 物化岛；沿用上方 b28d-29 归因，封顶。
// [b28d-TakeDispatchCore] 真缺口修复（双调用）：原「get_state()==8 || Check(0x22)==0 进主体 +
// 末尾重调 Check 返回」= Check 调 2 次（p167 EXTRA +1）；ORIG 0x081be5a8..0x081be5d0 是
// `if (get_state() != 8) { int r = Check(user,0x22); if (r != 0) return r; }` 单次调用
// （结果存 -0xc(%ebp)，cmpl $0 后回读 jmp 尾）。改写后 CALLSITE CLEAR 18/18、
// ndiff 39→10（ORIG 136 / ours 140）、our 149→140（gap −13→−4）。
#include "DisPatcherCore/check_error_13.inc"
{
    const MSG_BUY_ITEM& req = *reinterpret_cast<const MSG_BUY_ITEM*>(&msg);
    if (!G_CDataManager()->checkValidShopItem((unsigned int)req.m_deliveryType,
                                              (unsigned long)req.m_itemIdx))
    {
        // [Lane5] P47/P194 正极性：ORIG 0x081be4b6 `call; test %al,%al; je <hack块>`
        // + fall `mov $0x11` ⇒ 原码 `if (f()) return 0x11;`（负极性 ==0 生 xor $1）。
        if (G_CDataManager()->m_itemShop->IsOneADayShop(req.m_deliveryType))
        {
            return 0x11;
        }
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x3c9, 1, 0, 0);
        return -1;
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
    if (user->get_state() != 8)
    {
        // ORIG 0x081be5a8..0x081be5d0：Check 单次调用、结果存 -0xc(%ebp)，
        // 非 0 直接返回该码（我方原「==0 进主体 + 末尾重调」为双调用，p167 EXTRA +1）。
        int secuResult = GlobalData::s_pSecuProtectionField->Check(
            user, static_cast<SECURITY_PROTCTION>(0x22));
        if (secuResult != 0)
        {
            return secuResult;
        }
    }
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
// ORIG 0x081be20a T _ZN18DisPatcher_BuyItem4sendEP5CUserR9ParamBase
// 报告：docs/class_func_reports/DisPatcher_BuyItem/send.md（汇编+反编译 C 全读）
// [b28c-L19 N18a] 唯一正文差（165/164）：`UseFinishPoint(out.m_8c)` 处 ORIG
// [b28d-29] N18a/P254：ORIG `mov 0x8c(%eax),%eax; mov %eax,%edx` 冗余 regmove（int 字段->int 形参），4.4 单段直载 edx 不可反向造  (ORIG 0x081be20a insn 165->165)
// `mov 0x8c(%eax),%eax; mov %eax,%edx` 冗余 regmove vs 4.4 单步直载 edx（反向不可造）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 165 / ours 165 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/send_12.inc"
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
#include "DisPatcherCore/_onBuyItemEvent.inc"
{
}

// ORIG 0x081beab0 T get_nMaxBuyableCount（报告 get_nMaxBuyableCount.md 全文映射）：
// 对六种货币价格（money/cera/winpoint/finishpoint/guildPowerWarPoint/userPowerWarPoint）
// [b28d-29] N18b/P197③：ORIG 三目 min 形（cmp mem,%eax + jge 双出口）+ 两 dead 局部预清；4.4 折 reg-reg cmp 且丢预清  (ORIG 0x081beab0 insn 130->123)
// 逐项取「持有量 / 单价」与 0x7fffffff 取小，返回可购上限。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 34（ORIG 130 / ours 122），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/get_nMaxBuyableCount.inc"
{
    // [b28d] ORIG 0x081beab6..0x081beacb：六个结果局部全为函数头声明（槽序=声明序，
    // P28/P331）：maxCount -0x24、byMoney -0x20、byCera -0x1c、byWinPoint -0x18、
    // byFinishPoint -0x14（以上无预清）、byGuildPowerWarPoint -0x10 与
    // byUserPowerWarPoint -0xc（=0 预清两条，P198① 两句式）。
    // 每块用三目 min 形（ORIG 0x081beafa `mov maxCount,%eax; cmp byX,%eax; jge L;
    // mov maxCount,%eax; jmp; L: mov byX,%eax`），非 if 形。
    // [Lane5] P209/P310 试形：局部 int 化（ORIG 比较为 `mov maxCount,%eax; cmp byX,%eax`
    // 的 mem 直比形；unsigned 局部使 4.4 双载寄存器）。
    int maxCount = 0x7fffffff;
    int byMoney;
    int byCera;
    int byWinPoint;
    int byFinishPoint;
    int byGuildPowerWarPoint = 0;
    int byUserPowerWarPoint = 0;
    if (param.m_80 != 0)
    {
        byMoney = (int)((unsigned int)user->getCurCharacMoney() / (unsigned int)param.m_80);
        maxCount = (maxCount < byMoney) ? maxCount : byMoney;
    }
    if (param.m_84 != 0)
    {
        byCera = (int)((unsigned int)user->GetCera() / (unsigned int)param.m_84);
        maxCount = (maxCount < byCera) ? maxCount : byCera;
    }
    if (param.m_88 != 0)
    {
        byWinPoint = (int)((unsigned int)user->GetWinPoint() / (unsigned int)param.m_88);
        maxCount = (maxCount < byWinPoint) ? maxCount : byWinPoint;
    }
    if (param.m_8c != 0)
    {
        byFinishPoint = (int)((unsigned int)user->GetFinishPoint() / (unsigned int)param.m_8c);
        maxCount = (maxCount < byFinishPoint) ? maxCount : byFinishPoint;
    }
    if (param.m_90 != 0)
    {
        byGuildPowerWarPoint =
            (int)((unsigned int)user->GetGuildPowerWarPoint() / (unsigned int)param.m_90);
        maxCount = (maxCount < byGuildPowerWarPoint) ? maxCount : byGuildPowerWarPoint;
    }
    if (param.m_94 != 0)
    {
        byUserPowerWarPoint =
            (int)((unsigned int)(int)(short)user->GetUserPowerWarPoint() / (unsigned int)param.m_94);
        maxCount = (maxCount < byUserPowerWarPoint) ? maxCount : byUserPowerWarPoint;
    }
    return maxCount;
}

// ORIG 0x081be658 T get_data（报告 get_data.md ## 反编译 C 全文映射）：
// 解析商城物品 → 材料槽 → 六种单价（>1 件时乘数量并校验上限）→ 持有量校验。
// [b28d-29] N82/N18a：帧 0x20 vs 0x34 + 返回指针缓存槽（-0x1c）与两段中转差；语义集合全等  (ORIG 0x081be658 insn 310->328)
// 返回 0=成功；0x11=物品不存在；1=参数非法；10=余额不足；0x24=公会会长限制。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 265（ORIG 310 / ours 327），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P168a④ bool 槽化/P211/N18a；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/get_data.inc"
{
    // [G5-MiscC] P0/P52：ORIG 0x081be660 `mov 0x10(%ebp),%eax; mov 0xd(%eax),%eax;
    // mov %eax,%ebx` 先把 msg.m_itemIdx 存入 callee-saved %ebx，再
    // 0x081be668 `call G_CDataManager; mov %ebx,0x4(%esp); mov %eax,(%esp);
    // call find_item`（无 dataMgr 具名槽；我方 fresh 多 -0x1c 槽）⇒ 单语句链式。
    param.m_a8 = G_CDataManager()->find_item(msg.m_itemIdx);
    if (param.m_a8 == 0)
    {
        return 0x11;
    }
    // GetNeedMaterial() 返回 +0x24 处 pair<int,int>*（item idx, 数量），ORIG 读两 int
    // [G5-MiscC] ORIG 0x081be6a5 只调用一次、结果直接在 %eax 上被取走两 int：
    // `mov 0x4(%eax),%edx; mov (%eax),%eax`（单一 disp，无 add）⇒ 指针视图按
    // pair 访问（原 `const int*` + `[1]` 在 4.4 生成 `add $0x4,%eax` 两步寻址）。
    std::pair<int, int>* needMaterial =
        reinterpret_cast<std::pair<int, int>*>(param.m_a8->GetNeedMaterial());
    param.m_pair.first = needMaterial->first;
    param.m_pair.second = needMaterial->second;
    if (param.m_pair.first != -1)
    {
        // [G5-MiscC] P0/P183：ORIG 0x081be6d3 `lea 0x41(%eax),%esi`（&m_resultItem）/
        // 0x081be6d9 `mov 0x98(%eax),%ebx`（pair.first）/ 0x081be6e5
        // `mov 0xc(%ebp),%eax; mov %eax,(%esp); call getCurCharacInvenR`（对象最后求值）
        // —— 无 invenR 具名槽（我方 fresh 多 -0x10）⇒ 内联链式。
        param.m_a4 = user->getCurCharacInvenR()->GetInvenData(
            param.m_pair.first, param.m_resultItem);
        // [G5-MiscC] P0：ORIG 0x081be706 `mov 0x98(%eax),%ebx; call G_CDataManager;
        // mov %ebx,0x4(%esp); ... call find_item`（无 dataMgr2 具名槽，我方 fresh 多 -0xc）
        // ⇒ 内联链式（itemIdx 亦在 %ebx 保活）。
        param.m_ac = G_CDataManager()->find_item(param.m_pair.first);
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
            // [G5-MiscC] P0 反向：ORIG 0x081be87c `mov %eax,-0xc(%ebp)` 有槽往返
            // （0x081be87f `mov -0xc(%ebp),%edx; mov 0x10(%ebp),%eax;
            // mov 0x11(%eax),%eax; cmp %eax,%edx; jae`）⇒ 原码确有具名 unsigned int
            // 局部（我方原内联调用无槽）。
            unsigned int maxBuyableCount = get_nMaxBuyableCount(user, param);
            if (maxBuyableCount < (unsigned int)msg.m_buyCount)
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
#include "DisPatcherCore/statistic_process.inc"
{
    // ORIG +0x09：输出参 = `add $0xb0,%eax`（param.m_b0 字段，无局部 sellPrice、
    // 无初始化写）；+0x57 `mov 0xb0(%eax),%eax` 读回。两次 AddValueStatistic 各自
    // 现调 GetInstanceValueStatistic（+0x5f/+0x88，无 valueStat 槽，技37）。
    G_Store()->GetSellItemPrice(param.m_item, param.m_a8,
                                (short)param.m_item.m_addInfo2, false, param.m_b0);
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)6, user,
                                                   (unsigned int)param.m_b0);
    // ORIG +0x82 `mov 0x80(%eax),%ebx` 单步直落 %ebx；4.4 恒 `mov 0x80,%eax;
    // mov %eax,%ebx` 两段（N18a 反向不可造，register asm("ebx") 绑定反产
    // copy-back 4 段更差）→ 残 2 行 N 类。
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xd, user,
                                                   param.m_80);
    GetInstanceCubeStatistic()->collectCubeStatistics(
        param.m_pair.first, param.m_pair.second, user, (CUBE_STATISTIC_FIELD)0x6a);
}

// ORIG 0x081bdc7e T process（报告 process.md ## 反编译 C 全文映射）：
// check_error → get_data → 死亡塔特殊商品(0x29f2a7)状态/塔位校验 → make_item 填充
// Inven_Item → 入包 insertItemIntoInventory(reason 0,log=1,b=0) → 生物物品特判
// （IsCreatureItem && GetItemType()==0x16 → InsertCreatureItem）→ 公会 OnBuyItem →
// use_money(m_80, reason 0, log) → 有材料则 delete_item(1, m_a4, m_9c, 0x1a, 1) →
// _onBuyItemEvent/statistic_process → WriteBuyItem/SendAradEventItemLog/
// [b28d-29] N82/P211：帧 0x8c vs 0x98 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081bdc7e insn 373->376)
// eventDeliveryBuyItem(msg+0x15)。失败路径 SendCmdErrorPacket(0x15, 码) 返 -1。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 151（ORIG 373 / ours 376），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_21.inc"
{
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
    // [G5-ProcB] ORIG 0x081bdd27 `mov 0x14(%ebp),%eax; mov %eax,-0x2c(%ebp)` /
    // 0x081bdd2d `mov 0x10(%ebp),%eax; mov %eax,-0x28(%ebp)`：param/msg 两个视图
    // 具名局部都在两级 errNo 检查之后才初始化（函数头声明会多 2 条 store 并把
    // 槽基整体下移、丢 ORIG 的 push %edi）。
    BuyItemParam& data = static_cast<BuyItemParam&>(param);
    MSG_BUY_ITEM& req = *reinterpret_cast<MSG_BUY_ITEM*>(&msg);

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

// [b28d-29] N78/N82：ORIG rep stos 结构体清零 vs 4.4 call memset（0x28 字节）+ 帧 0x19c vs 0x190；语义等价  (ORIG 0x081ee59c insn 480->487)
#include "DisPatcherCore/dtor_DisPatcher_RentAvatar.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 491（ORIG 480 / ours 487），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] callee 重载修正：m_itemId 本即 unsigned long，多余的 (int) 强转错绑
// _ZN12CStreamGuardlsEi；ORIG 0x081eeb65 实为 lsEm。实测 ndiff 345→342，差集一缺一多清零。
// 残差 5 处 memset→rep stos 属 N78（4.1 内联展开、4.4 O0 恒 call memset，不可翻）。
#include "DisPatcherCore/dispatch_sig_6.inc"
{
    int unused0[10];                    // -0xa8（ORIG rep stos 0xa 清零，后未使用）
    STRentAvatarReq rentEntries;       // ORIG -0x188(%ebp) 起载荷区（逐 entry stride 5）
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
    char itemIdStr[0x14];               // -0x54
    char priceStr[0x24];                // -0x80

    memset(unused0, 0, sizeof(unused0));
    totalCash = 0;
    rentDays = 0;
    rentSeconds = 0;

    PacketGuard packetGuard;            // -0x40
    if (packet.get_byte(rentEntries.m_count) != 1)
        return LineFunc(0x71ee, __PRETTY_FUNCTION__, 0, 0);

    // ORIG 0x081ee628/0x081ee633：计数无独立局部，两处判定各自 movzbl -0x17b(%ebp)
    if (rentEntries.m_count == 0 || 9 < rentEntries.m_count)
        return LineFunc(0x71f0, __PRETTY_FUNCTION__, 0, 0);

    for (i = 0; i < (int)(unsigned int)rentEntries.m_count; ++i)
    {
        if (packet.get_byte(rentEntries.m_entries[i].m_rentDaysType) != 1)
            return LineFunc(0x71f4, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(rentEntries.m_entries[i].m_itemId) != 1)
            return LineFunc(0x71f5, __PRETTY_FUNCTION__, 0, 0);
    }

    if (user->getCurCharacR() == 0)
        return LineFunc(0x71fa, __PRETTY_FUNCTION__, 0, 0);

    if (user->getCurCharacInvenR()->GetRemainCapacity(
            (INVEN_TYPE)2, Inven_Item::ITEM_TYPE_0) <
        (int)(unsigned int)rentEntries.m_count)
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
    for (i = 0; i < (int)(unsigned int)rentEntries.m_count; ++i)
    {
        itemPtrs[i] = (int)(long)G_CDataManager()->find_item(
            (int)rentEntries.m_entries[i].m_itemId);
        if (itemPtrs[i] == 0)
        {
            RentAvatarErrorProc(user, 0x11);
            return 0;
        }
        cashPrices[i] = getAvatarRentPrice(
            ((CItem*)itemPtrs[i])->GetCashPrice(),
            (unsigned int)rentEntries.m_entries[i].m_rentDaysType);
        goldPrices[i] = getAvatarRentPrice(
            ((CItem*)itemPtrs[i])->get_price(),
            (unsigned int)rentEntries.m_entries[i].m_rentDaysType);
        winPrices[i] = getAvatarRentPrice(
            ((CItem*)itemPtrs[i])->GetMedalPrice(),
            (unsigned int)rentEntries.m_entries[i].m_rentDaysType);
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

    for (i = 0; i < (int)(unsigned int)rentEntries.m_count; ++i)
    {
        if (rentEntries.m_entries[i].m_rentDaysType == 1)
            rentDays = 0x1e;
        else if (rentEntries.m_entries[i].m_rentDaysType == 2)
            rentDays = 0x16d;
        else if (rentEntries.m_entries[i].m_rentDaysType != 0)
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
        // ORIG 0x081eeb65 call _ZN12CStreamGuardlsEm（unsigned long 重载）；
        // m_itemId 本即 unsigned long，多余的 (int) 强转会错选 lsEi。
        streamGuard.operator*()->operator<<(rentEntries.m_entries[i].m_itemId);
        streamGuard.operator*()->operator<<(rentDays);
        streamGuard.operator*()->operator<<(rentSeconds);
        streamGuard.operator*()->operator<<((int)(unsigned int)rentEntries.m_count);
        streamGuard.operator*()->operator<<(i + 1);
        OS_API::snprintf(itemIdStr, 0x14, "%d",
                         (int)rentEntries.m_entries[i].m_itemId);
        OS_API::snprintf(priceStr, 0x24, "%d", cashPrices[i]);
        GlobalData::s_pIPGHelper->IPGOutput(
            user, "", priceStr, itemIdStr, (char const*)0, (char const*)0,
            streamGuard.Get());
    }

    return 0;
}
#include "DisPatcherCore/read_22.inc"
{
    return 0; // ORIG 0x08232d7c：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/process_22.inc"

#include "DisPatcherCore/dtor_DisPatcher_ExtendAvatar.inc"
#include "DisPatcherCore/dispatch_sig_7.inc"
{
    return 0; // ORIG 0x081eddec：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/read_23.inc"
{
    return 0; // ORIG 0x08232c42：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/process_23.inc"

#include "DisPatcherCore/dtor_DisPatcher_SellItem.inc"
// §6.2 巨件核销：P167 77/77 [0缺0多]；gap 515→515/522；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] P30/N4：ORIG 结构体基址 lea -0x52 + add $0xd/0xe/0x10 两步，4.4 折单 lea；语义集合全等  (ORIG 0x081f2d0c insn 515->522)
// 无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 287（ORIG 515 / ours 522），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_8.inc"
{
    char invenType;                     // -0x45
    short slot;                         // -0x48
    unsigned short count;               // -0x46
    unsigned long money;                // -0x64
    int sum;                            // -0x3c
    int unusedGuard;                    // -0x38（ORIG 死存储 movl $0x6）
    int sellResult;                     // -0x34
    int sellTimeRet;                    // -0x30
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
            // [G5-SigA] P183：ORIG 0x081f31fd/0x081f323c 每次都重发完整寻址链
            // `mov -0x2c(%ebp),%eax; add $0xb24,%eax`（无 battleField 指针缓存槽）。
            // 原具名 CBattle_Field* battleField 多出槽写入（我方 -0x20）+ 后续沿用
            // 单条 load，改回逐次重算表达式。
            // CParty+0xb24 内嵌 CBattle_Field（局部视图 CPartyBattleFieldView）
            map = (CMap*)G_CDataManager()->find_map(
                reinterpret_cast<CPartyBattleFieldView*>(party)
                    ->m_battleField.get_cur_map());
            if (map != 0 && map->getAssignedNPCCount() == 0 &&
                !reinterpret_cast<CPartyBattleFieldView*>(party)
                     ->m_battleField.IsAssignNPCByPotion())
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
    // [G5-SigA] P52/P183：ORIG 0x081f345a `movl $0x0,-0x30(%ebp)` 之后
    // 直接把 getCurTickCount 的返回值放进实参位（无 -0x40 具名槽往返；
    // fn_diff_tu 实测我方多 `mov %eax,-0x40(%ebp)` + `mov -0x40(%ebp),%eax` 2 条）。
    sellTimeRet = user->SellItemTimeChecker(
        GlobalData::s_systemTime_.getCurTickCount(), 1000);
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
#include "DisPatcherCore/read_24.inc"
{
    return 0; // ORIG 0x08232f04：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/process_24.inc"

// [b28d-29] N6/N22：ORIG 29 条为 (1<<state)&0x15e8 无槽寄存器形；4.4 折叠成 (mask>>state)&1（N6）或槽化（P0），三形实测 33/34 均不达 29  (ORIG 0x081c619c insn 29->34)
#include "DisPatcherCore/dtor_DisPatcher_RepairEquip.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：正极性形实测 32→31 insn（ORIG 28）；
// 尾段已与 ORIG 同形（`test; jne <mov $0>` + fall `mov $0x13`），残余 3 条：
// ① 4.4 把 register state 落 callee-saved %esi（ORIG 留 %eax：call 返回值直判+
// `mov %eax,%ecx` 复用）⇒ push/pop esi + mov/cmp/mov 共 4 条；② ORIG `mov $1,%edx;
// mov %edx,%ebx` 立即数物化 2 条 vs 4.4 `mov $1,%ebx` 1 条；③ ja 落点块布局。
// 两句式/goto 形均劣 ⇒ N7+N18a 封顶。
#include "DisPatcherCore/check_error_14.inc"
{
    // [b28c-30] N96 封顶：原 asm("eax")/asm("edx") 绑定已按 P0 清除。
    // ORIG 0x081c61bb `call get_state; cmp $0xc,%eax; ja` 无槽直判 +
    // `mov $0x1,%edx; mov %edx,%ebx` 物化；清绑后纯 register 用 %esi/%ebx（35 insn）、
    // 普通局部落槽（34 insn）均不可复现（4.4 恒 callee-saved/槽化）。
    if (user == 0)
    {
        return 0xffffffff;
    }
    // ORIG 0x81c61b6..：get_state 结果无槽（`cmp $0xc,%eax; ja L13` 直判）；
    // `mov $0x1,%edx; mov %edx,%ebx; mov %eax,%ecx; shl %cl,%ebx; mov %ebx,%eax;
    // and $0x15e8,%eax; test; jne` —— 1 物化到 %edx（register asm 绑定）、移位累加器
    // 落 %ebx（callee-saved 已 push）；ja 与 !bit 共享 L13 fall 块。
    // ORIG 081c61bb `cmp $0xc,%eax; ja`→共享 0x13 块、+0xd4 `test; jne`→ret0 块。
    // 4.4 需 goto 形才能把「mov $0x13; jmp」排在 jne 之后（直落共享块），
    // OK 岛由 ja 落点承接（技23/17）。残留 mov %eax,%edx = N22 变体天花板。
    // [b28d-29] P0 清绑 + P231 纯 register（无 asm 绑定，合规）：ORIG 该值无槽
    // （`cmp $0xc,%eax` 直判后 `mov %eax,%ecx` 复用），字面量 1 无具名局部
    // （`mov $0x1,%edx` 直出）。
    register unsigned int state = (unsigned int)user->get_state();
    if (state > 0xc)
    {
        return 0x13;                  // ORIG ja → +0x3a 共享 0x13 块
    }
    // [Lane5] 试形（33→32 insn，ndiff 17→14）：bit 去具名 + `(1u<<state)&0x15e8`
    // 单表达式（ORIG 无 bit 槽、`mov $1,%edx` 直出）。残余=N6（4.4 折 shr/mask）+
    // N22（`mov %eax,%edx` 中转）+ 条件极性（jbe vs ja）+ push/pop esi。
    if (((1u << state) & 0x15e8) == 0)
    {
        return 0x13;
    }
    return 0;
}
#include "DisPatcherCore/read_25.inc"
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
#include "DisPatcherCore/send_13.inc"
{
    stRepairEquipParam& p = static_cast<stRepairEquipParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x19);
    // ORIG +0x3c：`test; je +0x6b` ⇒ 非零块是 fall-through 块（技17 写在 true 侧）。
    if (p.m_errNo != 0)
    {
        guard.put_byte(0);
        guard.put_byte(p.m_errNo);
    }
    else
    {
        guard.put_byte(1);
        guard.put_int(user->getCurCharacInvenR()->get_money());
        guard.put_byte((char)p.m_8);
        guard.put_short((unsigned int)p.m_a);
        guard.put_short((unsigned int)p.m_c);
    }
    guard.finalize(true);
    user->Send(guard);
}
// ORIG 0x081c6082 T _ZN22DisPatcher_RepairEquip7processEP5CUserR8MSG_BASER9ParamBase
// err>0：SendCmdErrorPacket(cmd=0x19, err&0xff) 返回 -1；err<0：LineFunc(0x1500,pretty,0,acc_id)
// 并返回其值；err==0：G_Store()->repair_equip(user, msg[0xd], (s16)msg+0xe, (u16)msg+0x10,
// [b28d-29] N82：帧差（ORIG sub $0x4c+push edi/esi，4.4 sub $0x34）+ 槽号平移，调用点全等  (ORIG 0x081c6082 insn 83->80)
// 出参 param+0xc) 写 param.m_errNo，成功(==0)时回显 m_8=msg[0xd]、m_a=(u16)msg+0xe；返回 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 69（ORIG 83 / ours 80），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_25.inc"
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
    // [G5-DisPatcherCore] P52/P183：ORIG 0x081c61xx 段先把 3 个 msg 字段读入
    // %edi/%esi/%ebx 跨 `call G_Store` 保活，receiver 表达式最后求值 ⇒ 原码无具名
    // `Store* store` 缓存（我方原形把 G_Store 提到语句前，实参退化为 caller-saved）。
    out.m_errNo = G_Store()->repair_equip(user, (char)req.m_0d, req.m_0e, req.m_10,
                                          out.m_c);  // 出参 ushort& @ param+0xc（ORIG asm -0x2c(%ebp)）
    if (out.m_errNo == 0)
    {
        out.m_8 = req.m_0d;                    // movzbl 0xd(%eax) -> mov %dl,0x8
        // [G5-DisPatcherCore] 尝试去掉 (unsigned short) cast：ORIG `movzwl 0xe(%eax),%edx`
        // 单步 vs 我方 `movzwl …,%eax; mov %eax,%edx` 两段；实测两种写法产物全同
        // ⇒ 属 N18a（单段 regmove 4.4 恒折/反向不可造），维持原 cast 形并封顶。
        out.m_a = (unsigned short)req.m_0e;    // movzwl 0xe(%eax) -> mov %dx,0xa（原位型回显）
    }
    return 0;
}


#include "DisPatcherCore/dtor_DisPatcher_SetTradeState.inc"
// ORIG 0x081f6232 T _ZN24DisPatcher_SetTradeState12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetTradeState/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state==3 且 get_byte(state) 成功 → GetTradeSpace(idx) 池取；空 → SendCmdErrorPacket
// (0x1a,1)；is_empty → logFormat(0x82f1) + PutTradeSpace；check_trade_possibility 假 →
// cancel_trade + PutTradeSpace；真 → change_trade_state(user, (char)state)；返回 0。
#include "DisPatcherCore/dispatch_sig_9.inc"
{
    if (user->get_state() != 3)
    {
        return 0;
    }
    // ORIG 0x81f6258：`lea -0x1a(%ebp),%eax; add $0xd,%eax` = 0xe 字节栈缓冲
    // （-0x1a..-0xd），目标字节 = buf[0xd]（-0xd，恰在 trade 槽 -0xc 之下）。
    char buf[0xe];
    if (!packet.get_byte(buf[0xd]))
    {
        return LineFunc(0x82e6, __PRETTY_FUNCTION__, 0, 0);
    }
    CTradeSpace* trade = G_CGameManager()->GetTradeSpace(user->GetTradeSpace());
    if (trade == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 1);
        return 0;
    }
    // ORIG `test %al,%al; je`：is_empty bool 返回直判（技巧19，勿加 !=0）
    if (trade->is_empty())
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__, 0x82f1,
                              "Trade Space[%d] is Empty User[%d]",
                              trade->get_index(),
                              user->get_unique_id() & 0xffff);
        G_CGameManager()->PutTradeSpace(trade);
        return 0;
    }
    // ORIG `xor $0x1,%eax; test %al,%al; je`：!boolcall 形（技巧1，ORIG 0x852994a
    // check_trade_possibility 体 xor$1 = bool 返回）
    if (!trade->check_trade_possibility())
    {
        trade->cancel_trade();
        G_CGameManager()->PutTradeSpace(trade);
        return 0;
    }
    trade->change_trade_state(user, (int)(char)buf[0xd]);
    return 0;
}
#include "DisPatcherCore/read_26.inc"
{
    return 0; // ORIG 0x08233150：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}

// ORIG 0x0823315a W _ZN24DisPatcher_SetTradeState7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部交易状态逻辑（get_state==3 → GetTradeSpace → is_empty/check_trade_possibility →
// change_trade_state / cancel_trade+PutTradeSpace）内嵌于
// dispatch_sig(0x081f6232，报告 docs/class_func_reports/DisPatcher_SetTradeState/dispatch_sig.md)。
#include "DisPatcherCore/process_26.inc"

#include "DisPatcherCore/dtor_DisPatcher_Compound.inc"
#include "DisPatcherCore/dispatch_sig_10.inc"
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
    // ORIG 0x081eaa5c：三 wire 字段是单 blob（base -0x20：get 传址 `lea -0x20,%eax;
    // add $0xd/11/12`，读 `mov -0x13`/`movzbl -0xf`/`movzwl -0xe` disp 直读）——
    // 单 pack(1) 视图复现（SetIP 同法）。
    #pragma pack(push, 1)
    struct CompoundMsgView {
        char m_head[13];          // +0x00..0x0c
        unsigned long m_itemId;   // +0x0d（ORIG +0x84 `get_intERm` ulong& 重载实证）
        unsigned char m_type;     // +0x11
        unsigned short m_count;   // +0x12
    };
    #pragma pack(pop)
    // ORIG blob 基 -0x20；4.4 pack(1) 基 -0x21（1B 对齐缝）、变量级 aligned(4) 反
    // 推远至 -0x24（4 对齐向远离 ebp 取整）——两向均留文本差，取奇基形（帧紧凑）。
    CompoundMsgView msg;
    if (packet.get_int(msg.m_itemId) != true)
    {
        return LineFunc(0x6c3a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_byte(msg.m_type) != true)
    {
        return LineFunc(0x6c3b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_short(msg.m_count) != true)
    {
        return LineFunc(0x6c3d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (msg.m_count == 0)
    {
        return LineFunc(0x6c40,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // P168a-①②：&& 合写在 4.4 产值物化岛（mov$1/$0;test;je 5 行）；ORIG +0x199..
    // 是两段直跳（`cmp $1,%al; je` + bool callee `test %al,%al; je` 裸形）→ 拆嵌套。
    if (msg.m_type != '\x01')
    {
        if (user->CheckItemLock(1, static_cast<int>(msg.m_itemId)))
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1b), 0xd5);
            return 0;
        }
    }
    // ORIG +0x1c8..+0x1f5：右→左求值 count(movzwl 两段→%ecx)、type==1（`cmp $1,%al;
    // sete %al; movzbl %al,%edx` bool 物化经具名量传参，技9）、itemId(→%eax)；
    // 分支 `cmpl $0,-0xc; jle fail岛` ⇒ 成功 guard 块是 fall-through（技17）。
    // ORIG 实参求值右→左：count(movzwl→ecx) 先、type==1(sete 岛→edx) 次、
    // itemId(→eax) 最后——实参内联表达式直产（register asm 强制提前反致
    // movzwl/sete 块序倒置）。
    int local_10 = user->compound_item(static_cast<int>(msg.m_itemId),
                                       msg.m_type == '\x01', msg.m_count, 0x1b);
    if (local_10 > 0)
    {
        PacketGuard local_30;
        local_30.put_header(1, 0x1b);
        local_30.put_byte(0);
        local_30.put_byte(local_10);
        local_30.finalize(true);
        user->Send(local_30);
        return 0;
    }
    // P168a-②直分支形：ORIG `cmpl $0,0xc; je; cmpl $0,-0xc; jne` 两段直跳，
    // && 合写在 4.4 反产值物化岛（mov$1/$0;test;je）→ 拆嵌套。
    if (user != 0)
    {
        if (local_10 == 0)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, static_cast<APSystem::_ActionGroupIndex>(6), 0,
                static_cast<ENUM_PACKETCLASS>(0));
        }
    }
    return 0;
}
#include "DisPatcherCore/read_27.inc"
{
    return 0; // ORIG 0x08232a92：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/process_27.inc"

#include "DisPatcherCore/dtor_DisPatcher_DisJointItem.inc"
// ORIG 0x081f9fb8 T _ZN23DisPatcher_DisJointItem12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_DisJointItem/dispatch_sig.md 汇编+反编译 C 全文转写）：
// user 空或 CheckInTrade() 真 → return 0；依次 get_short(slot)/get_byte(type)/
// get_short(target,0xffff 初始)，失败分别 LineFunc 0x88d0/0x88d2/0x88d6；
// 全部成功 → _disjoint(user,(int)(short)slot,(int)(char)type,0x1c,0,(ushort)target)。
#include "DisPatcherCore/dispatch_sig_11.inc"
{
    if (user == 0 || user->CheckInTrade())
    {
        return 0;
    }
    // ORIG 0x081f9fb8：首 get_short 实参两段 `lea -0x17(%ebp),%eax; add $0xd,%eax`
    // （技巧26 blob 覆写形：13B 前导的对象基址 -0x17，slot 成员 +0xd = -0xa）。
    // ORIG msg 基址 -0x17（奇数=align1，15B char 数组），实参两段
    // `lea -0x17(%ebp); add $0xd`（技巧26 blob 指针破 lea 折叠）；尾读
    // `movzwl -0xa(%ebp)` 单级（常量地址折叠）。
    struct JointMsg { char m_pad[0xd]; short m_slot; } __attribute__((packed)) msg;
    if (!packet.get_short(*reinterpret_cast<short*>(msg.m_pad + 0xd)))
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
    // ORIG 尾 `call; leave; ret`（结果直落 %eax 无 `mov $0x0,%eax`）= 尾 return。
    return DisPatcher_DisJointItem::_disjoint(user,
                                              (int)msg.m_slot,
                                              (int)(char)type,
                                              (ENUM_CMDPACKET)0x1c, (CUser*)0,
                                              (unsigned short)target);
}
#include "DisPatcherCore/read_28.inc"
{
    return 0; // ORIG 0x0823339c：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}
#include "DisPatcherCore/process_28.inc"

// ---------------------------------------------------------------------------
// LinkF1 批次（2026-08-28）：DisPatcher_DisJointItem::_disjoint —— ORIG T
// _ZN23DisPatcher_DisJointItem10_disjointEP5CUserii14ENUM_CMDPACKETS1_t @0x081f92ca
// （报告 docs/class_func_reports/DisPatcher_DisJointItem/_disjoint.md 全文转写，
// 草稿 /tmp/df_batches/linkf1/_disjoint.body.cpp + notes.md）。
// 依赖 _check_disjointable_user_state/_check_disjointable_item_state/
// _check_disjoint_need_slot：真实 C++ 名定义在 ExpertJobLink4.cpp（DispDebugJoint4.cpp
// 中的 _Z30 前缀名是带 asm 标签的桥接，勿混淆）。
// ---------------------------------------------------------------------------
#include "DisPatcherCore/_check_disjointable_user_state.inc"
#include "DisPatcherCore/_check_disjointable_item_state.inc"
                                    Inven_Item& invenItem, CItem** item,
                                    CUser* targetUser, unsigned short region);
#include "DisPatcherCore/_check_disjoint_need_slot.inc"
                               expert_job::CDisjointer* disjointer);

// §6.2 巨件核销：P167 98/98 [0缺0多]；gap 811→811/818；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] N82：帧 0x2bc vs 0x2dc + 槽平移（-0x26c vs -0x28c），调用点全等  (ORIG 0x081f92ca insn 811->818)
// 无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 544（ORIG 807 / ours 815），残差=setcc-fold bool 物化岛/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/_disjoint.inc"
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


#include "DisPatcherCore/dtor_DisPatcher_Lottery.inc"
// [Lane5] ORIG 0x081f845b `lea -0x1f(%ebp),%eax; add $0xd,%eax`（get_short 目标）
// + 0x081f84a2 `movzwl -0x12(%ebp)` 读回 + PacketGuard@-0x2c ⇒ 原码有 0xf 字节
// pack(1) 请求体局部（ORIG 未出线，仅 m_0d 被读）。**实测**：加 0xf 视图结构体后
// 4.4 帧 0x50→0x60、PacketGuard -0x2c→-0x40、ndiff 133（劣化）⇒ 回退为裸 `short idx`；
// 残余为 N93/P221 互斥块对象槽向（ORIG guard1 浅/guard2 深，4.4 反向）。
// ORIG 0x081f8380 T _ZN18DisPatcher_Lottery12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] N82：帧内对象槽整体平移 8（ORIG -0x2c，4.4 -0x34），助记符序列与调用点全等  (ORIG 0x081f8380 insn 166->167)
// 报告：docs/class_func_reports/DisPatcher_Lottery/dispatch_sig.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 128（ORIG 166 / ours 166），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_12.inc"
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
    // [G5-DisPatcherCore] 负结论（已回滚）：ORIG 0x081e1e0x 对 idx 取址为两步
    // `lea -0x1f(%ebp),%eax; add $0xd,%eax`（⇒ 地址 -0x12，即原码是「0xd 字节头 + 2 字节
    // 字段」的局部对象）。试形 `struct { char m_hdr[0xd]; char m_idx[2]; } view;
    // + *(short*)view.m_idx`（非 packed，避开 packed 成员引用限制）⇒ 读数 166→169
    // worse（多 3 条：对象落位与预期不符，且第二处读取未折叠回单条 movzwl）⇒ 回滚，
    // 保持独立 `short idx`（槽位 -0x12 与 ORIG 一致，仅差取址步数）。
    short idx;                                 // ORIG msg 视图未定（-0x1f 基址，见上）
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
            // [G5-DisPatcherCore] P0/P66：ORIG 0x081e... 段 `call getCurSec`→%ebx 先求（左操作数），
            // `call getLastLotteryTime`→%eax 后求，`sub %eax,%edx` 直算；无具名 last 槽往返
            // （我方原 `int last = ...` 反序 + 多占 4B 槽）。
            unsigned int diff =
                GlobalData::s_systemTime_.getCurSec() - user->getLastLotteryTime();
            if (diff <= 2)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x334, 1, 0, 0);
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
#include "DisPatcherCore/read_29.inc"
{
    return 0; // ORIG 0x08233276：空函数（恒返 0，10B：push ebp/mov esp,ebp/mov $0x0,eax/pop ebp/ret）
}

// ORIG 0x08233280 W _ZN18DisPatcher_Lottery7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部抽奖逻辑（CheckInTrade 拒绝包 0x1d/0xda → lottery_item → 应答包 0x1d →
// 失败时限 3 秒内 addServerHackCnt(0x334) → setLastLotteryTime）内嵌于
// dispatch_sig(0x081f8380，报告 docs/class_func_reports/DisPatcher_Lottery/dispatch_sig.md)。
#include "DisPatcherCore/process_29.inc"

#include "DisPatcherCore/dtor_DisPatcher_ChangeSkill.inc"
// ORIG 0x081c0196 T _ZN22DisPatcher_ChangeSkill11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_ChangeSkill/check_error.md 逐块映射）：技能树索引
// [b28d-29] N82/N18b：槽号平移 -0x10->-0x14 + 比较方向折痕（4.4 cmpl mem 直比 vs 4.1 两段），语义集合全等  (ORIG 0x081c0196 insn 31->39)
// ∈{-1,0} 时 msg+0xd==1 → 0x13；其它索引时 msg+0xd!=1 → 0x13；否则 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 18（ORIG 30 / ours 34），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error_15.inc"
{
    // [b28d-solo-DisPatcherCore] 删 av/goto 改自然 if/else-if（ORIG 源码形：
    // `cmp $-1; je L_eq` + `test; jne L_ne` 两独立分支、L_ne 侧 `je L_ret0`）。
    const MSG_CHANGE_SKILLSLOT* pReq =
        reinterpret_cast<const MSG_CHANGE_SKILLSLOT*>(&msg);   // ORIG -0x10(%ebp)
    int treeIdx = user->GetCurCharacSkillTreeIndex();          // ORIG -0xc(%ebp)
    // [b28d-solo-DisPatcherCore] 自然 if/else-if 实测 34→29 insn（ORIG 31）：
    // 唯一残差 = ORIG 两次独立 reload `mov -0xc(%ebp),%eax; cmp $0xffffffff,%eax` /
    // `mov -0xc(%ebp),%eax; test %eax,%eax` vs 4.4 折 `cmpl $imm,-0xc(%ebp)` 直比
    // （N18b：4.4 对具名 int 局部比较恒折 mem 直比，无源码写法产两段物化；
    // switch 形实测 30 条且产 add/cmp/ja 树，更劣）⇒ N18b 封顶。
    // 复核（本轮）：P310 形 `(unsigned)treeIdx == K` 与 volatile 形实测均劣化
    // （35 条 / 31 条，ndiff 17/23），原 if/else-if 形 28 条为最优 ⇒ N18b 封顶维持。
    if (treeIdx == -1 || treeIdx == 0)
    {
        if (pReq->m_head[0xd] == 1)
        {
            return 0x13;
        }
    }
    else if (pReq->m_head[0xd] != 1)
    {
        return 0x13;
    }
    return 0;
}
#include "DisPatcherCore/read_30.inc"
{
    // ORIG 0x081...：无指针局部，三处均 `mov 0x10(%ebp),%eax; add $0xd/e/f`（帧 0x18）。
    if (!packet.get_byte(reinterpret_cast<MSG_CHANGE_SKILLSLOT*>(&msg)->m_head[0xd]))   // +0x0d
    {
        return LineFunc(0x6f4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(
            &reinterpret_cast<MSG_CHANGE_SKILLSLOT*>(&msg)->m_curSlot)))              // +0x0e
    {
        return LineFunc(0x6f6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(
            &reinterpret_cast<MSG_CHANGE_SKILLSLOT*>(&msg)->m_newSlot)))               // +0x0f
    {
        return LineFunc(0x6f8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "DisPatcherCore/send_14.inc"
{
    stOnChangeSkillParam& out = static_cast<stOnChangeSkillParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x1e);
    guard.put_byte(1);
    // ORIG 0x81c00fe：`call 822f33c; movsbl %al,%eax; mov %eax,0x4(%esp)` —— int 返回
    // 调用点显式 (char) 截断再 sext（RefundSkill 0x81dadb4 无 cast 直存 = 基声明 int）
    guard.put_byte((int)(char)user->GetCurCharacSkillTreeIndex());
    guard.put_byte(static_cast<char>(out.m_curSlot));
    guard.put_byte(static_cast<char>(out.m_newSlot));
    guard.finalize(true);
    user->Send(guard);
}

// ORIG 0x081bffbc T _ZN22DisPatcher_ChangeSkill7processEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_ChangeSkill/process.md ## 反编译 C 全文映射）：
// check_error（stub 恒 0）→ param.m_curSlot/m_newSlot ← msg[0xe]/msg[0xf] →
// [b28d-29] N82/P0：msg/param 形参副本落槽（ORIG 0x10/0x14(%ebp) 直用），4.4 帧 0x28 vs ORIG 0x2c+三 push  (ORIG 0x081bffbc insn 74->69)
// SkillSlot::skill_move(cur, new, get_charac_job)；>0 时 SendCmdErrorPacket(0x1e) 返 -1。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 62（ORIG 74 / ours 68），残差=N7/N82 帧与 callee-saved 角色/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_30.inc"
{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e, (unsigned char)errNo);
        return -1;
    }
    // [b28d] ORIG 0x081bffde 起：out/req 引用槽在 check_error 之后才初始化
    // （P28 声明序=槽序：errNo -0x24 / out -0x20 / req -0x1c）。
    stOnChangeSkillParam& out = static_cast<stOnChangeSkillParam&>(param);
    MSG_CHANGE_SKILLSLOT& req = *reinterpret_cast<MSG_CHANGE_SKILLSLOT*>(&msg);
    out.m_curSlot = req.m_curSlot;    // ORIG *(u8*)(param+4) = *(u8*)(msg+0xe)
    out.m_newSlot = req.m_newSlot;    // ORIG *(u8*)(param+5) = *(u8*)(msg+0xf)

    // [b28d] P52/P23：ORIG 先 call get_charac_job→%ebx（跨 receiver call 保活）、
    // 再 m_newSlot→%edi / m_curSlot→%esi，最后 call getCurCharacSkillW→%eax
    // ⇒ 原码是单条链式表达式（实参右→左求值），无具名 skillSlot 局部。
    errNo = ((SkillSlot*)user->getCurCharacSkillW())->skill_move(
        req.m_curSlot, req.m_newSlot, user->get_charac_job());
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e, (unsigned char)errNo);
        return -1;
    }
    return errNo;
}

#include "DisPatcherCore/dtor_DisPatcher_BuySkill.inc"
// ORIG 0x081bf8be T _ZN19DisPatcher_BuySkill11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_BuySkill/check_error.md 逐块映射）：user 空 → -1；
// 遍历 msg 技能项（count@+0xe signed char，entries@+0xf stride 3），
// 技能 id ∈ (0xc7,0xd0) 且 ServiceRestrictManager::isRestricted(user,1,0x1e) → 0xd1。
#include "DisPatcherCore/check_error_16.inc"
{
    if (user == 0)
    {
        return 0xffffffff;
    }
    // 具名字段（MSG_BUY_SKILL pack(1)：m_count@+0x0e、m_entries[70]@+0x0f stride 3，
    //  条目首字节 m_skillIdx；ORIG 0x081bf93a `movzbl 0xe(%eax)`+`movsbl` 为计数
    //  有符号扩展，0x081bf8ec/0x081bf8ef `lea (%ecx,%eax,1),%eax; add $0xf` 为
    //  entries 基址 + 3i 两步寻址——与成员访问形态一致）
    const MSG_BUY_SKILL& req = *reinterpret_cast<const MSG_BUY_SKILL*>(&msg);
    for (int i = 0; i < (int)(signed char)req.m_count; ++i)
    {
        const unsigned char skillId = req.m_entries[i].m_skillIdx;
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
#include "DisPatcherCore/check_state_3.inc"
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
// [b28d-29] AE：169=169 全等（同上重定位归一）  (ORIG 0x081bed4a insn 169->170)
// 报告：docs/class_func_reports/DisPatcher_BuySkill/read.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 169 / ours 169 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/read_31.inc"
{
    MSG_BUY_SKILL& m = *reinterpret_cast<MSG_BUY_SKILL*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_treeKind)))  // +0x0d（signed char→char& 适配）
    {
        return LineFunc(0x37b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 调用序列 [c,c,h,c,c] ⇒ m_count /
    // m_refundFlag / m_level 均绑 `get_byte(char&)`（ERc），仅 m_skillIdx 为 ERh。
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_count)))   // +0x0e
    {
        return LineFunc(0x37e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG test/`jg`+cmp $0x46/`jle`（read.md +0x9d/+0xd1）：比较对象为
    // **signed char**（movzbl 后按 al 符号判定），必须显式 (char) 转换。
    if (!((char)m.m_count > 0))
    {
        return LineFunc(0x381,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!((char)m.m_count < 0x47))
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
        if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_entries[i].m_refundFlag)))  // +0x11+3i
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
        if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_entries[i].m_level)))  // +0x10+3i
        {
            return LineFunc(0x38f,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if ((char)m.m_entries[i].m_level < 1)
        {
            m.m_entries[i].m_level = 1;
        }
    }
    return 0;
// [b28d-29] N82：帧 0x40 vs 0x60 + 局部对象槽平移（-0x20 vs -0x38），调用点全等  (ORIG 0x081bf468 insn 311->302)
}
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 415（ORIG 311 / ours 302），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/send_15.inc"
{
    stOnBuySkillParam* local_18 = &static_cast<stOnBuySkillParam&>(param);
    PacketGuard local_24;
    local_24.put_header(1, 0x1f);
    if (local_18->m_350 < 0)
    {
        local_24.put_byte(0);
        // [b28d] P324：ORIG 叶块物理序 = 书写序 = 0x081bf4de(-1)/0x081bf4f3(-2)/
        // 0x081bf508(-3)/0x081bf51d(-4)/0x081bf532(-5)/0x081bf547(-6) ⇒ 降序书写。
        switch (local_18->m_350)
        {
        case -1:
            local_24.put_byte(0x15);
            break;
        case -2:
            local_24.put_byte(0x16);
            break;
        case -3:
            local_24.put_byte(0x12);
            break;
        case -4:
            local_24.put_byte(1);
            break;
        case -5:
            local_24.put_byte(0x5b);
            break;
        case -6:
            local_24.put_byte(0x5c);
            break;
        }
        local_24.put_byte(static_cast<int>(local_18->m_354));
    }
    else
    {
        local_24.put_byte(1);
        // [b28d] P0/P183/P329：ORIG 0x081bf58e/0x081bf5ab 无 cVar2/uVar3/pSVar4/iVar5
        // 具名局部——put_byte 实参直取 (char)GetCurCharacSkillTreeIndex()；uVar3 经
        // %ebx 跨 getCurCharacSkillR 保活（嵌套实参右→左求值），iVar5 直传 put_short。
        local_24.put_byte(static_cast<int>(static_cast<char>(user->GetCurCharacSkillTreeIndex())));
        local_24.put_short(
            reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                ->get_remain_sp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(
                    user->GetCurCharacSkillTreeIndex())));
        if (user->GetCurCharacSkillTreeIndex() == -1 ||
            user->GetCurCharacSkillTreeIndex() == 0)
        {
            local_24.put_short(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                    ->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(2)));
        }
        else
        {
            local_24.put_short(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                    ->get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(3)));
        }
        local_24.put_byte(static_cast<int>(static_cast<char>(local_18->m_355)));
        for (int local_14 = 0; local_14 < static_cast<int>(static_cast<char>(local_18->m_355));
             local_14 = local_14 + 1)
        {
            // [b28d] P183：ORIG 0x081bf68b 起每处字段访问重发完整
            // `mov i; mov base; add; add; shl; movzbl 0x8(%eax,%ecx,1)` 下标链，
            // 无 `buySkillInfo& entry` 引用缓存（缓存会少 6 条/处并撑槽）。
            // [b28d] P214①/P272：ORIG fall-through 分支是 0xff 分支
            // （`cmp $0xc7; jbe <m_4 分支>` + `cmp $0xd0; ja <m_4 分支>`），
            // 故源形取正条件 `m_0>0xc7 && m_0<=0xd0` 的 if 包裹。
            if (static_cast<unsigned char>(local_18->m_infos[local_14].m_0) > 0xc7 &&
                static_cast<unsigned char>(local_18->m_infos[local_14].m_0) <= 0xd0)
            {
                local_24.put_byte(0xff);
                local_24.put_byte(static_cast<unsigned int>(static_cast<unsigned char>(
                    local_18->m_infos[local_14].m_0)));
                local_24.put_byte(local_18->m_infos[local_14].m_8);
            }
            else
            {
                local_24.put_byte(
                    static_cast<int>(static_cast<char>(local_18->m_infos[local_14].m_4)));
                local_24.put_byte(static_cast<unsigned int>(static_cast<unsigned char>(
                    local_18->m_infos[local_14].m_0)));
                local_24.put_byte(local_18->m_infos[local_14].m_8);
            }
            STSkillCommandData local_30;
            // [b28d] ORIG 0x081bf7d1 直接 test %al（无 cVar2b 具名 bool 槽）。
            if (user->findSkillCommandData(
                    static_cast<int>(static_cast<unsigned char>(
                        local_18->m_infos[local_14].m_0)),
                    local_30, true))
            {
                local_24.put_byte(1);
                // ORIG 0x081bf7e8 单条 movzbl+movsbl 直传（无 local_2e 槽）；
                // 0x081bf80a `movzbl -0x29(%ebp,%eax,1)` = 对象 +3 起字节数组视图。
                local_24.put_byte(static_cast<int>(static_cast<char>(local_30.m_count)));
                for (int local_10 = 0;
                     local_10 < static_cast<int>(static_cast<char>(local_30.m_count));
                     local_10 = local_10 + 1)
                {
                    local_24.put_byte(
                        static_cast<int>(static_cast<char>(local_30.m_keyBytes[local_10])));
                }
            }
            else
            {
                local_24.put_byte(0);
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
#include "DisPatcherCore/process_31.inc"
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
        // ORIG 两遍循环各占独立槽（-0x10/-0xc，Ghidra local_14/local_10）：
        // O0 无槽共享，必须两个不同变量名。
        for (int i = 0; (signed char)req.m_count > i; ++i)
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
        for (int j = 0; (signed char)req.m_count > j; ++j)
        {   // 第二遍：正式执行
            out.m_infos[j].m_0 = req.m_entries[j].m_skillIdx;
            out.m_infos[j].m_1 = req.m_entries[j].m_level;
            if (req.m_entries[j].m_refundFlag == 0)
            {
                master_new_skill_process(user, (char)req.m_treeKind, out.m_infos[j]);
            }
            else
            {
                refund_skill_process(user, (char)req.m_treeKind, out.m_infos[j]);
            }
            out.m_354 = out.m_infos[j].m_0;
            out.m_350 = out.m_infos[j].m_4;
            if (out.m_infos[j].m_4 < 0)
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
// [b28d-29] N82 + P168a：帧 0x7c vs 0x9c + 取反物化方向（xor $1 位置），调用点全等  (ORIG 0x081bf952 insn 239->248)
// 尾部（+0x2b3）：m_4>=0 时 HistoryLog::WriteLearnSkill(m_field796f8, m_0, 当前等级)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 297（ORIG 234 / ours 243），残差=P168a④ bool 槽化/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/master_new_skill_process.inc"
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
            // STGuildDBInfoOnly::m_guildSkillPoint（CUser.h:1297 @+0x42 ushort，
            // 与 ORIG 0x081bf9ba `movzwl 0x42(%eax)` 同宽；ORIG 两次独立
            // GetGuildDBInfo 调用（0x081bf9b5 与 guildDB），故不合并成一次调用）
            // [G5-MiscC] P0/P52：ORIG 0x081bf9b5 `call GetGuildDBInfo;
            // movzwl 0x42(%eax),%eax; movzwl %ax,%ebx`（guildSkillPoint 值仅存
            // callee-saved %ebx 跨后续全部调用）、0x081bfa08 `call get_charac_job;
            // mov %eax,%edi`、0x081bfa1e `call getCurCharacSkillR; mov %eax,%edx`
            // —— 三者均**无槽**（我方原具名 guildLevel/job/skillRG 各多占一槽）⇒ 内联
            // 回实参位。内联后 4.4 右→左求值序 = ORIG：guildLevel → info.m_8 → money
            // → guildDB → secondGrow → firstGrow → job → info.m_0 → skillR。
            int money = user->getCurCharacMoney();                      // -0x54
            STGuildDBInfoOnly* guildDB =
                static_cast<STGuildDBInfoOnly*>(user->GetGuildDBInfo());// -0x50
            int secondGrow =
                static_cast<int>(user->getCurCharSecondGrowType());           // -0x4c
            int firstGrow =
                static_cast<int>(user->getCurCharFirstGrowType());            // -0x48
            int needGold;   // ORIG -0x20：调用前未初始化，由出参引用写出
            short needGSP;  // ORIG -0x22：同上（&别名传参）
            info.m_4 = ((SkillSlot*)(user->getCurCharacSkillR()))
                           ->master_new_guild_skill(
                static_cast<unsigned char>(info.m_0), user->get_charac_job(),
                firstGrow, secondGrow, *guildDB, money, needGold,
                /*learnCnt=*/info.m_8,  // ORIG &info+8 别名 int& 出参
                needGSP,
                static_cast<int>(static_cast<STGuildDBInfoOnly*>(
                    user->GetGuildDBInfo())->m_guildSkillPoint));
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
        // [G5-MiscC] P0/P52：ORIG 0x081bfb75 `call get_charac_job; mov %eax,%esi` /
        // 0x081bfb82 `call get_charac_level; mov %eax,%ebx` / 0x081bfb98
        // `call getCurCharacSkillW`（this 直入 (%esp)）—— 均无槽 ⇒ 内联回实参位；
        // 求值序右→左 = ORIG：addLevel → secondGrow → firstGrow → job → level
        // → info.m_0 → skillW。
        info.m_4 = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
                       ->master_new_skill(static_cast<unsigned char>(info.m_0),
                                          user->get_charac_level(),
                                          user->get_charac_job(), firstGrow,
                                          secondGrow, addLevel);
        if (info.m_4 >= 0)
        {
            // [G5-MiscC] P0/P52：ORIG 0x081bfbe2 `call getCurCharacSkillR;
            // mov %eax,(%esp)` 后直读 `mov 0x4(%eax),%ebx`(=info.m_4) 传参，
            // 结果直接 `mov %eax,0x8(%edx)` 回写 —— 无 skillR 具名槽 ⇒ 内联链式。
            info.m_8 = ((SkillSlot*)(user->getCurCharacSkillR()))->get_skill_level(info.m_4);
        }
    }
    if (info.m_4 >= 0)
    {   // ---- 尾部（ORIG +0x2b3）----
        // [G5-MiscC] P0/P52：ORIG 0x081bfc15 `call getCurCharacSkillR; mov %eax,(%esp)`
        // / 0x081bfc27 `call get_skill_level`（结果直接 `mov %eax,0x8(%esp)`）——
        // 无 skillRT/curLevel 具名槽 ⇒ 内联链式。
        HistoryLog::WriteLearnSkill(
            reinterpret_cast<FILE*>(user->m_field796f8),
            static_cast<unsigned char>(info.m_0),
            ((SkillSlot*)(user->getCurCharacSkillR()))->get_skill_level(info.m_4));
    }
}

// ORIG 0x081bfc5c T _ZN19DisPatcher_BuySkill20refund_skill_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/refund_skill_process.md 全文转写）：
// 无树门/无权限门（treeKind 存 -0x2c 后未再读取）；refundCount 局部清零后取址；
// SkillSlot::refund_skill(idx, level, job, first, second, (s8)m_1, &refundCount)
// [b28d-29] N82：帧/槽重排（ORIG sub $0x4c+三 push，4.4 sub $0x58），调用点全等  (ORIG 0x081bfc5c insn 73->72)
// 写 m_4，≥0 时 m_8←当前等级。ORIG 不读回 refundCount 值。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 66（ORIG 73 / ours 71），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/refund_skill_process.inc"
                                               buySkillInfo& info)
{
    int refundCount = 0;                                                              // -0x1c
    int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));        // -0x38
    int secondGrow = static_cast<int>(user->getCurCharSecondGrowType());        // -0x34
    int firstGrow = static_cast<int>(user->getCurCharFirstGrowType());         // -0x30
    // [b28d-solo-DisPatcherCore] P248 三条件（跨 call 保活 + 4.4 不自升格 +
    // ORIG 目标为 callee-saved %esi/%ebx）⇒ 保留裸 register（非 asm 绑定，P0 合规）。
    register int job = user->get_charac_job();               // ORIG esi
    register int level = user->get_charac_level();           // ORIG ebx
    // [G5-MiscC] P0/P89：ORIG 0x081bfcca `call getCurCharacSkillW; mov %eax,%edx`
    // （receiver 直落 %edx，**无**具名指针槽；我方原 `SkillSlot* skillW` 多占一槽
    // + 回读两条）⇒ 内联回 receiver 位。ORIG 实参布局同序：
    // &refundCount(0x1c) / addLevel / secondGrow / firstGrow / job(%esi) /
    // level(%ebx) / info.m_0(%edi) / this(%edx)，全部从右到左求值。
    info.m_4 = ((SkillSlot*)(user->getCurCharacSkillW()))
                   ->refund_skill(static_cast<unsigned char>(info.m_0), level, job,
                                  firstGrow, secondGrow, addLevel, refundCount);
    if (info.m_4 >= 0)
    {
        // [G5-MiscC] P0/P89：ORIG 0x081bfd1d `call getCurCharacSkillR;
        // mov %eax,(%esp)` 后直读 `mov 0x4(%eax),%ebx`(=info.m_4) 传参，结果
        // 直接 `mov %eax,0x8(%edx)` 回写 —— 无 skillR 具名槽 ⇒ 内联链式。
        info.m_8 = ((SkillSlot*)(user->getCurCharacSkillR()))->get_skill_level(info.m_4);
    }
}

// ORIG 0x081bfd3c T _ZN19DisPatcher_BuySkill25try_refund_skills_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/try_refund_skills_process.md 全文转写）：
// [b28d-29] N82：帧/槽重排（ORIG sub $0x4c+push edi/esi/ebx，4.4 sub $0x58），助记符序列与调用点全等  (ORIG 0x081bfd3c insn 59->55)
// 与 refund 同参形态调 try_refund_skill；仅写 m_4，无等级回读、无任何门。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 58（ORIG 59 / ours 55），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/try_refund_skills_process.inc"
                                                    buySkillInfo& info)
{
    int refundCount = 0;                                                              // -0x1c
    int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));        // -0x38
    int secondGrow = static_cast<int>(user->getCurCharSecondGrowType());        // -0x34
    int firstGrow = static_cast<int>(user->getCurCharFirstGrowType());          // -0x30
    // [b28d-solo-DisPatcherCore] P248 三条件（跨 call 保活 + 4.4 不自升格 +
    // ORIG 目标为 callee-saved %esi/%ebx）⇒ 保留裸 register（非 asm 绑定，P0 合规）。
    register int job = user->get_charac_job();               // ORIG esi
    register int level = user->get_charac_level();           // ORIG ebx
    // ORIG `mov 0x14(%ebp),%eax; movzbl (%eax),%eax; movzbl %al,%edi`：info.m_0
    // 在 skillW 的 call 之前取值并跨该 call 保活于 %edi ⇒ 具名 register 局部。
    // [FL4 负结论] P267 两段装载不可造：源侧改 `(unsigned char)(unsigned int)info.m_0`
    // 实测 4.4 仍发单段 `movzbl (%eax),%edi`（ORIG 为两段 cs 32/33），帧 0x5c 不变
    // ⇒ 归 N18a（4.1 冗余 regmove 折半不可造）族，维持单段 cast 原形。
    register unsigned char itemIdx = static_cast<unsigned char>(info.m_0);
    // [FL4] P0/P89：ORIG 0x...`call getCurCharacSkillW; mov %eax,%edx`（receiver 直落
    // %edx，**无**具名指针槽）；旧形 `SkillSlot* skillW = ...` 占 -0x1c 槽 + 回读 2 条。
    info.m_4 =
        ((SkillSlot*)(user->getCurCharacSkillW()))
            ->try_refund_skill(itemIdx, level, job, firstGrow, secondGrow, addLevel,
                               refundCount);
}

// ORIG 0x081bfdf0 T _ZN19DisPatcher_BuySkill29try_master_new_skills_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/try_master_new_skills_process.md 全文转写）：
// 公会技能 [200,208] 直接返回（cmp $0xd0 jbe → 尾声；与正式 master 相反）；
// [b28d-29] N82/N18b：帧/槽重排 + 比较方向折痕，调用点全等  (ORIG 0x081bfdf0 insn 84->79)
// 非公会走同一树门后调 try_master_new_skills，仅写 m_4，无等级回写、无日志尾部。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 51（ORIG 81 / ours 76），残差=N7/N82 帧与 callee-saved 角色/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/try_master_new_skills_process.inc"
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
        // [b28e-L5] P52/P290/P200：ORIG 0x081bfe86..0x081bfed2 把 (job→%esi,
        // level→%ebx, (byte)info.m_0→%edi) 物化入 callee-saved 后跨
        // getCurCharacSkillW call 保活（`push %edi/%esi/%ebx; sub $0x4c`）；拆具名
        // job/level/skillW 局部会使 4.4 三值各占栈槽（帧 0x4c→0x58、丢三 push）。
        // 链式单表达式的 receiver 置于末尾求值，实参位相应对齐 ORIG 装载序。
        info.m_4 = ((SkillSlot*)(user->getCurCharacSkillW()))->try_master_new_skills(
            static_cast<unsigned char>(info.m_0), user->get_charac_level(),
            user->get_charac_job(), firstGrow, secondGrow, addLevel);
}

#include "DisPatcherCore/dtor_DisPatcher_IncreStatus.inc"
// ORIG 0x081ede34 T _ZN22DisPatcher_IncreStatus12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_IncreStatus/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=2 → 0；get_short 失败 → LineFunc 0x70fc；GetItemPtr(user,0,slot)==0 →
// LineFunc 0x7101；CheckItem 0x2a 非0 → SendCmdErrorPacket(0x20,err)+0；
// [b28d-29] N82：帧 0x50 vs 0x40 + 槽平移（ORIG 结构体基址 -0x2f+add 两步，4.4 折 lea），语义集合全等  (ORIG 0x081ede34 insn 123->121)
// slot<0x99 → increase_status(slot)，否则 UseQuestItem(slot)+cMyTrace(0x710f,5)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 52（ORIG 123 / ours 121），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_13.inc"
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    // [b28e-L5] P250：ORIG 载荷为 pack(1) 结构体局部（槽基 -0x2f、val@+0x0d，
    // `lea -0x2f(%ebp),%eax; add $0xd,%eax` 两步取址；0x081ede5a/0x081ede7c 等多处
    // `movzwl -0x22(%ebp),%eax` 即 m_val 重读）。
    IncreStatusSigView sig;
    if (!packet.get_short(sig.m_val))
    {
        return LineFunc(0x70fc, __PRETTY_FUNCTION__, 0, 0);
    }
    CItem* pItem = GlobalData::s_pSecuProtectionField->GetItemPtr(
        user, (char)0, (short)sig.m_val);
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
    if (sig.m_val < 0x99)
    {
        user->increase_status((short)sig.m_val);
    }
    else
    {
        user->UseQuestItem((int)sig.m_val);
        // [b28e-L5] P227 全表达式临时链（ORIG 0x081ede9x 在 C1 call 前右起求 varargs
        // 并 `movswl %ax,%ebx` 跨 ctor 保活 ⇒ 单语句链式）。
        cMyTrace(__PRETTY_FUNCTION__, 0x710f, 5)("__ USE QUEST ITEM __ %d",
                                                 (int)sig.m_val);
    }
    return 0;
}
#include "DisPatcherCore/read_32.inc"
{
    // ORIG 0x08232cb8: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}

// ORIG 0x08232cc2 W _ZN22DisPatcher_IncreStatus7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部加点逻辑（get_short 属性号 → SecuProtectionField GetItemPtr/CheckItem 守卫 →
// <0x99 increase_status / >=0x99 UseQuestItem+cMyTrace）内嵌于
// dispatch_sig(0x081ede34，报告 docs/class_func_reports/DisPatcher_IncreStatus/dispatch_sig.md)。
#include "DisPatcherCore/process_32.inc"

// [b28d-29] N82/P168a：帧 0x6c vs 0x7c + 取反物化（ORIG test/je 直判 vs 4.4 xor/jne），语义集合全等  (ORIG 0x081ee010 insn 342->357)
#include "DisPatcherCore/dtor_DisPatcher_QuestAction.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 582（ORIG 341 / ours 357），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_14.inc"
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
    // [G5-DisPatcherCore] P196/T1：ORIG 0x081ee0xx `movzbl %al,%edx; movzwl -0x22(%ebp),%eax;
    // cwtl; cmp %eax,%edx` = **int 32 位比较**（at(1) 零扩展、cmd 符号扩展）；我方原写
    // `static_cast<unsigned short>(cmd)` 使其折成 `cmp %ax,%dx` 16 位比较（宽度/符号性错位，
    // 语义对 at1∈[0,255] 等价但机器形错）。去掉 cast 即恢复整型提升形。
    // 否定形 `== false` 产 `xor $1;test`（4 条），ORIG 为裸 `test %al;je`（P292 正形）⇒ 改 `!`。
    if (!packet.get_short(cmd) || (packet.at(1) & 0xff) == cmd)
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
                // [G5-DisPatcherCore] P0/P329：ORIG 0x081ee2xx `call at; movzbl %al,%eax;
                // cmp $0x21,%eax; jne` —— 判定值直用 %eax（无 char 具名槽），我方原写
                // `char cVar1 = static_cast<char>(packet.at(1));` 多一个 1 字节槽与往返。
                if (static_cast<char>(packet.at(1)) == '!' &&
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
                    if (local_20->m_battleField.m_bloodState == 1)   // CParty.h:764 int m_battleField.m_bloodState@+0xcd8
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
#include "DisPatcherCore/read_33.inc"
{
    // ORIG 0x08232d1a: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}

// ORIG 0x08232d24 W _ZN22DisPatcher_QuestAction7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部任务动作逻辑（PvPSkilTree 频道拒绝/at(1) 命令码 0x21..0x24 分派 → find_quest/
// setBlindTimeGateStateQuest/quest_action 四参/GM quest flag）内嵌于
// dispatch_sig(0x081ee010，报告 docs/class_func_reports/DisPatcher_QuestAction/dispatch_sig.md)。
#include "DisPatcherCore/process_33.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPos.inc"
#include "DisPatcherCore/check_error_17.inc"
{
    // ORIG 0x081c9994：get_state(user)==3 → 0，否则 0x13（setne 形式）。
    if (user->get_state() != 3)
        return 0x13;
    return 0;
}
#include "DisPatcherCore/read_34.inc"
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
#include "DisPatcherCore/process_34.inc"
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

#include "DisPatcherCore/dtor_DisPatcher_SetArea.inc"
// ORIG 0x081c93e8 T _ZN18DisPatcher_SetArea11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_SetArea/check_error.md 汇编+反编译 C 全文转写）：
// state!=3 → 0x7fffffff；IsBusyDisjointStore → param.m_8=req.m_0d, 0xbf；
// IsBusyPrivateStore → param.m_8=req.m_0d, 0x3c；IsCurCharacGhost → param.m_8=req.m_0d,
// 0x13；IsForbiddenMove(name) 且剩余时间!=0 → param.m_9=req.m_0e, 0x44；时间==0 则
// EnableMove(name)；否则 0。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
#include "DisPatcherCore/check_error_18.inc"
{
    // ORIG +0x07/+0x0d：param→-0x10（out 远槽）、msg→-0xc（req 近槽）——4.4 后声明
    // 占近槽（技21），out 先声明。三处 `out.m_8 = msg+0xd` 均 ORIG `movzbl mem,%edx`
    // 单步直落 %edx（技16/28 register asm 绑定破 4.4 累加器中转）。
    stSetAreaParam& out = static_cast<stSetAreaParam&>(param);
    const MSG_SET_USER_AREA& req =
        *reinterpret_cast<const MSG_SET_USER_AREA*>(&msg);
    if (user->get_state() != 3)
    {
        return 0x7fffffff;
    }
    if (user->IsBusyDisjointStore())
    {
        out.m_8 = req.m_0d;   // ORIG +0x44 `movzbl 0xd(%eax),%edx; ...; mov %dl,0x8`（同型 QI 直存，Main 已落 schar 字段）
        return 0xbf;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        out.m_8 = req.m_0d;   // ORIG +0x73
        return 0x3c;
    }
    if (user->IsCurCharacGhost())
    {
        out.m_8 = req.m_0d;   // ORIG +0x99
        return 0x13;
    }
    if (G_GameWorld()->IsForbiddenMove(user->getCurCharacName()))
    {
        if (G_GameWorld()->GetLeftTimeOfForbiddenMove(
                user->getCurCharacName()) != 0)
        {
            out.m_9 = req.m_0e;   // ORIG `movzbl 0xe,%edx; ...; mov %dl,0x9`
            return 0x44;
        }
        G_GameWorld()->EnableMove(user->getCurCharacName());
    }
    return 0;
}
// ORIG 0x081c91e6 T _ZN18DisPatcher_SetArea4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_SetArea/read.md（汇编+反编译 C 全读）
#include "DisPatcherCore/read_35.inc"
{
    // 载荷字段具名（MSG_SET_USER_AREA，PacketTypes.h:1499 pack(1)；宽度按 ORIG：
    //   get_byteERc @+0xd/+0xe/+0x13 → signed char 字段以 char& 适配；
    //   get_shortERt @+0xf/+0x11/+0x14/+0x16 → unsigned short 字段直引；
    //   081c934b movw $0x0,0x14(%eax) / 081c9354 movw $0x0,0x16(%eax) 两处清零）
    MSG_SET_USER_AREA& req = *reinterpret_cast<MSG_SET_USER_AREA*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_0d)))
    {
        return LineFunc(0x1c16,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_0e)))
    {
        return LineFunc(0x1c17,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_0f))
    {
        return LineFunc(0x1c18,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_11))
    {
        return LineFunc(0x1c19,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_13)))
    {
        return LineFunc(0x1c1a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    req.m_14 = 0;
    req.m_16 = 0;
    if (!packet.get_short(req.m_14))
    {
        return LineFunc(0x1c1f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_16))
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
#include "DisPatcherCore/send_16.inc"
{
    stSetAreaParam& p = static_cast<stSetAreaParam&>(param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
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
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 94 / ours 94 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_35.inc"
{
    // ORIG 081c9525/+0x0f：入口先存 msg→-0x20（req）、后存 param→-0x1c（out）；
    // 4.4 后声明占近槽（技21），故 req 先声明。
    MSG_SET_USER_AREA& req = *reinterpret_cast<MSG_SET_USER_AREA*>(&msg);
    stSetAreaParam& out = static_cast<stSetAreaParam&>(param);
    // ORIG +0x35：`mov %eax,%edx`（value 先落 edx）→ `mov -0x1c,%eax`（地址后载）→
    // `mov %edx,0x4(%eax)`（register asm 双钉，同 Dispatchers 16628 形）。
    // [b28d-solo-DisPatcherCore] P211：ORIG RHS-first+edx 中转（unsigned 返回 + 无具名局部）。
    out.m_errNo = check_error(user, msg, param);
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

#include "DisPatcherCore/dtor_DisPatcher_Fish.inc"
#include "DisPatcherCore/read_36.inc"
{
    // ORIG 0x08232ca4: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}

// ORIG 0x08232cae W _ZN15DisPatcher_Fish7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
// —— 原始二进制即为空实现（无 process 报告，W 内联弱符号）。
#include "DisPatcherCore/process_36.inc"
// ORIG 0x081eddf6 T _ZN15DisPatcher_Fish12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_Fish/dispatch_sig.md 全文转写）：
//   状态非 4（钓鱼中）直接返 0；为 4 时调 GameWorld::fishing，恒返 0。
#include "DisPatcherCore/dispatch_sig_15.inc"
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
// ORIG T 0x08264bfe _ZN21DisPatcher_FinishLoad11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_FinishLoad/check_error.md ## 汇编 + ## 反编译 C
// 全文转写）。ORIG mov 0xc(%ebp) 在隐式 this(+0x8) 之后即第一具名参数 user——
// Ghidra 伪 C 的 "GetDebugCommand((CUser*)param_2)" 为其省略 this 的编号假象，
// 实参就是 user。GetDebugCommand()==1 → 0x7fffffff，否则 0；
// if/else 形态对应 ORIG sete/test/je 分支骨架。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
#include "DisPatcherCore/check_error_19.inc"
{
    if (user->GetDebugCommand() == 1)
    {
        return 0x7fffffff;
    }
    return 0;
}
// 队伍 >2 人时逐成员核对：0 号记录 job/growType/IP 前缀/前三技能 index，
// 其余成员与之全等（job/growType 不等或 IP 网络前缀不等 → 0x330 多开计数清标；
// [b28d-29] N82/P197③：ORIG 4 dead 局部预清 + push ebx，4.4 丢预清且帧 +4；语义集合全等  (ORIG 0x08264c28 insn 251->240)
// 技能三槽不全等 → 仅清 0x32f 同 IP 标）。任一失败即短路置双标志为 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 277（ORIG 250 / ours 235），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [SweepDispatchCore 2026-09-09] 真缺口修复：ORIG 对三处 16B 栈缓冲预清零
// （basePrefix 0x08264c3a/41/48/4f、peerIpBuf 0x08264d43/4a/51/58、peerIp
// 0x08264e67/6e/75/7c 各 4×movl $0）⇒ 原码 char[16]={0}；我方原缺清零，strncpy
// 短写后残留栈垃圾（越界读隐患）。补 {0} 初始化并删多余显式终止 movb。
// gap 15→2（ours 235→249，ORIG 251），四数不回退。残余 p167 MISSING
// `_ZSt7strrchrPci`/EXTRA `strrchr` = glibc 头集差异：ORIG(4.1.2) 时
// __CORRECT_ISO_CPP_STRING_H_PROTO 未启用（__GNUC_PREREQ(4,4) 不成立）⇒ <cstring>
// 发射 inline 重载；4.4 头集启用该宏令重载被抑制。实测 undef 宏再引 <cstring>
// 触发 "conflicts with previous using declaration" 编译错误 ⇒ 合法源码不可复现
// （N130 同族工具链头集差异）。
// [SweepDispatchCore 2026-09-09b] 补块序对齐：ORIG 0x08264cf2 `cmpl $0,-0x28; je
// 08264e44` ⇒ idx!=0 分支物理在前、idx==0 分支在后 ⇒ 源码改写
// `if (idx != 0) {…} else {…}`，并把 idx==0 分支内 job/growType 提前到 peerIp 流程
// 之前（ORIG 0x08264e4a/58 → 0x08264e67/91）。指令数 251/249 不变，ndiff
// 332→282（-50），四数不回退（213/639/2/74）。
#include "DisPatcherCore/Party_hack_check.inc"
{   // ORIG 栈位映射：-0x40 sameIp / -0x3f multiBox / -0x3e job(sbyte) /
    // -0x3d growType / -0x50 baseIpPrefix[16] / -0x3c,-0x38,-0x34 = 技能三槽 index。
    // -0x50：0 号成员 IP 最后一个 '.' 前缀。ORIG 函数头 4×movl $0 预清零
    // （0x08264c3a/41/48/4f）⇒ 原码数组带 {0} 初始化；缺清零会令 strncpy
    // 短写后残留栈垃圾（越界读隐患）。
    // [b28e-L5] 判据⑩（声明序 = 槽由深到浅）：ORIG basePrefix@-0x50 是最深槽 ⇒
    // 原码**最先声明**（其 4×movl $0 落在 sameIp 之前）。故声明提到 sameIp 之前。
    char basePrefix[16] = {0};
    char sameIp = 1;
    char multiBox = 1;
    signed char jobByte = 0;
    signed char growType = 0;
    unsigned int skillIdx0 = 0;
    unsigned int skillIdx1 = 0;
    unsigned int skillIdx2 = 0;

    CParty* party = static_cast<CParty*>(user->GetParty());   // ORIG _ZN5CUser8GetPartyEv
    if (party == 0)
    {
        return;
    }
    if (party->get_member_count() <= 2)
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
        if (member == 0)
        {
            continue;
        }
        // ORIG 块序（P272/P59）：0x08264cf2 `cmpl $0,-0x28; je 08264e44` ⇒
        // idx!=0 分支物理在前（fall-through）、idx==0 分支在后（跳转目标）
        // ⇒ 源码写作 if (idx != 0) {…} else {…}。
        if (idx != 0)
        {
            if ((signed char)member->get_charac_job() != jobByte ||
                member->getCurCharFirstGrowType() != growType)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            char peerIp[16] = {0};      // ORIG 0x08264e67..7c 4×movl 预清零
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
        else
        {   // 记录基准：job/growType/IP 网络前缀/技能三槽。ORIG 0x21c 段顺序：
            // job(0x08264e4a)/growType(0x08264e58) 先于 peerIp 清零+GetPeerIP2(0x08264e67/91)
            jobByte = (signed char)member->get_charac_job();   // ORIG mov %al + movsbl 比较
            growType = member->getCurCharFirstGrowType();
            char peerIpBuf[16] = {0};   // ORIG 0x08264d43..58 4×movl 预清零
            char* peerIp = peerIpBuf;   // char* 静态类型命中 std::strrchr(char*,int) 非const重载
            member->m_network.GetPeerIP2(peerIp, 0x10);
            if (strlen(peerIp) <= 8)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            // peerIp 静态类型 char* → 命中 std::strrchr(char*,int) 非 const 重载
            // （mangled _ZSt7strrchrPci，匹配 ORIG）；指针间接 call 会被 p167 计为丢失，
            // 故直接调用（数组退化 const char* 会误选 ::strrchr plain）。
            char* dot = std::strrchr(peerIp, '.');
            if (dot == 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            size_t fullLen = strlen(peerIp);
            size_t dotLen = strlen(dot);
            size_t prefLen = fullLen - dotLen;
            strncpy(basePrefix, peerIp, prefLen);
            // 预清零已保证 null 终止（ORIG 无显式终止 store）
            // 现声明为 const char*（ORIG 实返回 SkillSlot*），按位转写
            SkillSlot* skill = reinterpret_cast<SkillSlot*>(
                const_cast<char*>(member->getCurCharacSkillR()));
            if (skill == 0)
            {
                sameIp = 0;
                multiBox = 0;
                break;
            }
            skillIdx0 = skill->get_skill_index(0);
            skillIdx1 = skill->get_skill_index(1);
            skillIdx2 = skill->get_skill_index(2);
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
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 113 / ours 113 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_37.inc"
{
    stFinishLoadParam& out = static_cast<stFinishLoadParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG `mov %eax,%edx; mov <out>,%eax; mov %edx,0x4(%eax)`。
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
#include "DisPatcherCore/dtor_DisPatcher_FinishLoad.inc"
// ORIG W 0x08265492 _ZN21DisPatcher_FinishLoad11check_stateEP5CUserR9PacketBuf
// （无独立报告；objdump 全文转写）。Get_State_Check(user->get_state()) 为真
// （state ∈ {5,6,12,13}）→ 0，否则 0x7fffffff——xor $1/test/je 分支骨架与
// 上方 check_error 注释一致。依赖 Global::CGlobalFunc::Get_State_Check
// （ORIG T 0x081466f0，声明 BossTowerFunc.h、定义 BossTowerFunc.cpp）。
#include "DisPatcherCore/check_state_4.inc"
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
#include "DisPatcherCore/read_37.inc"
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

#include "DisPatcherCore/dtor_DisPatcher_UseSkill.inc"
// ORIG 0x081cad80 T _ZN19DisPatcher_UseSkill11check_errorEP5CUserR8MSG_BASER9ParamBase
// get_state(user)==5 → 0；否则 0x7fffffff（报告 DisPatcher_UseSkill/check_error.md 全文）。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
#include "DisPatcherCore/check_error_20.inc"
{
    // ORIG setne+test+je 布局：4.4.7 对 `if (x != 5) return INF; return 0;` 物化 bool。
    if (user->get_state() != 5)
        return 0x7fffffff;
    return 0;
}
#include "DisPatcherCore/read_38.inc"
{
    MSG_USE_SKILL& req = *reinterpret_cast<MSG_USE_SKILL*>(&msg);
    // 具名字段（MSG_USE_SKILL pack(1)；ORIG 0x081cac6d add $0xd / ERc → 1B；
    //  0x081cad39 add $0x11 / ERt → 2B，落 m_padTail[2] 内，同宽度引用视图）
    if (!packet.get_byte(req.m_gapD))
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
            *reinterpret_cast<unsigned short*>(req.m_padTail)))
    {
        return LineFunc(0x1fd9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "DisPatcherCore/send_17.inc"
{
    // ORIG 0x081caf28：指针局部落栈 -0x4 → 取 +0x4 m_errNo → cmp $0x7fffffff
    // 后 `return`（then==merge，jcc 被 tail-merge 清理 → dead cmp 残留）。
    stUseSkillParam* pParam = static_cast<stUseSkillParam*>(&param);
    if (pParam->m_errNo == 0x7fffffff)
        return;
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
#include "DisPatcherCore/IsSkillMatchFirstGrowType.inc"
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
#include "DisPatcherCore/IsSkillMatchSecondGrowType.inc"
                                                            char a, char b)
{
    // ORIG 0x08286612：setg 后再 `test;je;mov 1;jmp;mov 0`（true/false 分支物化）。
    if (skill->get_second_growtype_max_level((int)a, (int)b) > 0)
        return true;
    return false;
}

// ORIG 0x08286656：恒返 0x339
#include "DisPatcherCore/GetHackType.inc"
{
    return 0x339;
}

// ORIG 0x082864fc：find_skill(job, slot)==0 → false；
// 首一转类型命中 / 二转上限>0 / IsStealable(first, second) 任一成立 → true。
#include "DisPatcherCore/IsValidUsingSkill.inc"
{
    CSkill* skill = G_CDataManager()->find_skill(info->m_job, (int)info->m_slot);
    // ORIG 0x081c... 头块 `cmpl $0x0,-0xc; je 远端 false 尾` = 正向嵌套 if（probe p8 fb 实证）
    if (skill != 0)
    {
    if (IsSkillMatchFirstGrowType(skill, info->m_firstGrowType))
    {
        return true;
    }
    if (IsSkillMatchSecondGrowType(skill, info->m_firstGrowType, info->m_secondGrowType))
    {
        return true;
    }
        // ORIG 尾块：`call; test %al,%al; je L_false; mov $1,%eax; jmp; L_false: mov $0,%eax`
    if (skill->IsStealable((int)info->m_firstGrowType, (int)info->m_secondGrowType))
    {
        return true;
    }
    }
    return false;
}

// ORIG 0x081cadaa T _ZN19DisPatcher_UseSkill7processEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseSkill/process.md ## 反编译 C 全文映射）：
// param.m_errNo=check_error；>=1 直接返回 0；<0 LineFunc(0x1ff2,err,acc_id)；
// ==0 时构造校验信息 → 非法使用记黑客计数（addServerHackCnt(type,1,slot,0)），
// 有队伍则 CParty::use_skill(user, slot, target)，恒返 0。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 112 / ours 112 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_38.inc"
{
    stUseSkillParam& out = static_cast<stUseSkillParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG 0x081cadaa+0x35
    // `mov %eax,%edx; mov -0x24(%ebp),%eax; mov %edx,0x4(%eax)`（RHS-first+edx 中转）
    // 由「unsigned 返回 + 无具名 err/dst 局部」复现。
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
    // ORIG 0x081c...：msg→-0x20 spill 位于 LineFunc(0x1ff2) 岛之后（技巧13）
    MSG_USE_SKILL& req = *reinterpret_cast<MSG_USE_SKILL*>(&msg);

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

#include "DisPatcherCore/dtor_DisPatcher_DieMob.inc"
// ============================================================================
// ORIG T 0x081eb0c4 _ZN17DisPatcher_DieMob12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_DieMob/dispatch_sig.md，1564 insn）
// setMonsterMoveHack 门禁（moveCheckFlag && state==5）后按 state 分派：
//   5=普通副本 kill_monster 全反作弊链 / 10=死亡塔 handleKillMonster /
//   0xc=BossTower Global::CBossTowerFunc::DieMob / 0xd=advancealtar dieMob /
// [b28d-29] N78/N82：ORIG 无 0xa3f 字节 memset 而 4.4 发 call + 帧 0xb10 vs 0xb40；调用点全等  (ORIG 0x081eb0c4 insn 1564->1585)
//   8=WarRoom HandleMonsterKill。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 1582（ORIG 1564 / ours 1578），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] 真语义缺口核销（实测 ndiff 824→759；P167 MISSING 8→2，仅剩变体）：
//  ① ORIG 0x081eb9b7/0x081ec396/0x081ec7a2/0x081ecac5/0x081ecb10 是逐层重新 call
//     get_state() 的 if-else 链（==5/0xa/0xc/0xd/8），原码折成 switch 少 4 次调用；
//  ② 4 处 `GetPartyIndex();`/`GetWarRoomIndex();` 丢弃直呼是我方凑数，ORIG
//     0x081eb635/0x081eb9d0/0x081ec629/0x081ecb29 各只 1 次（movswl 缓存），已删；
//  ③ ORIG 0x081ec355/0x081ec761 在 static ValidateFixMobDiePos 前确实 call
//     getHackAnalyzer 后丢弃返回值，已补。
// 残差封 N：1 处 memset(N78) + PacketGuard/std::pair/cMyTrace 的 C1↔C2、D1↔D2 变体。
#include "DisPatcherCore/dispatch_sig_16.inc"
{
    MSG_MONSTER_DIE die;

    unsigned char passiveFlag = 0;               // ORIG -0x75（独立局部）
    unsigned char moveCheckFlag = 0;             // ORIG -0x76
    int monsterIdxOut = 0;                       // ORIG -0x74（kill_monster 出参）

    // [G5-SigA] P0/P183：ORIG 每个字段都是「具名基址 + add $0xNN」两步把 packed
    // 成员地址直接交给 get_*（0x081eb0d6 `lea -0xadb(%ebp),%eax; add $0xd,%eax`
    // = &die.m_mobSeq；0x081eb0e9 call get_short(unsigned short&)），全程无 tmp 槽
    // 往返。原实现的 5 个 tmp 中转每点多出「写 tmp + 读 tmp 回填字段」≈3 条，
    // 28 个调用点累计即 1644 vs 1564 的主要来源。此处以 cast 解除 packed 成员
    // 不可绑非 const 引用的限制后直传。
    // 原 memset(&die,...) 亦属发明代码：ORIG 全函数（0x081eb0c4..0x081ecce5）
    // 无 memset 符号调用、无 rep stos，die 仅按包逐个字段写入。

    if (!packet.get_short(*(unsigned short*)&die.m_mobSeq))
        return LineFunc(0x6d09,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 调用序列 t×8 后 s×7 ⇒
    // 0x6d0a/0x6d25/0x6d26 三处绑 `get_short(unsigned short&)`（ERt），原写法为 short&。
    if (!packet.get_short(*(unsigned short*)&die.m_fieldF))
        return LineFunc(0x6d0a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(int*)&die.m_field11))
        return LineFunc(0x6d0c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(int*)&die.m_field15))
        return LineFunc(0x6d0d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(unsigned int*)&die.m_killValue2))
        return LineFunc(0x6d0e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(unsigned int*)&die.m_killValue1))
        return LineFunc(0x6d0f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*(unsigned char*)&die.m_mobCount))
        return LineFunc(0x6d11,
                        __PRETTY_FUNCTION__,
                        0, 0);
    for (int i = 0; i < (int)(unsigned char)die.m_mobCount; ++i)
    {
        if (!packet.get_short(*(unsigned short*)&die.m_skillList[i]))
            return LineFunc(0x6d16,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(*(unsigned short*)&die.m_attackerList[i]))
            return LineFunc(0x6d17,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_int(*(unsigned int*)&die.m_damageList[i]))
            return LineFunc(0x6d18,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(*(unsigned short*)&die.m_hitCountList[i]))
            return LineFunc(0x6d19,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }
    if (!packet.get_short(*(unsigned short*)&die.m_killPairA24))
        return LineFunc(0x6d25,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(unsigned short*)&die.m_killPairA26))
        return LineFunc(0x6d26,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*(unsigned char*)&die.m_fieldA22))
        return LineFunc(0x6d28,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*(unsigned char*)&die.m_fieldA23))
        return LineFunc(0x6d2b,
                        __PRETTY_FUNCTION__,
                        0, 0);
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
        CParty* party =
            G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party == 0)
            return LineFunc(0x6d38,
                            __PRETTY_FUNCTION__,
                            0, 0);
        party->setMonsterMoveHack();
    }
    // [G5-SigA] ORIG 槽 -0x78（0x081eb68b `lea -0x78(%ebp),%eax`），无预清写
    // （全函数无 `movw $0x0,-0x78(%ebp)`）⇒ 去掉 `= 0`（P328⑤）。
    unsigned short unusedShort;                  // ORIG -0x78（读而未用）
    if (!packet.get_short(unusedShort))
        return LineFunc(0x6d3e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*(char*)&die.m_fieldA28))
        return LineFunc(0x6d41,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(int*)&die.m_fieldA2d))
        return LineFunc(0x6d42,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(*(int*)&die.m_fieldA31))
        return LineFunc(0x6d43,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA29))
        return LineFunc(0x6d44,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA2b))
        return LineFunc(0x6d45,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA35))
        return LineFunc(0x6d46,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA37))
        return LineFunc(0x6d47,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA39))
        return LineFunc(0x6d48,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA3b))
        return LineFunc(0x6d49,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(*(short*)&die.m_fieldA3d))
        return LineFunc(0x6d4a,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // ORIG 不是 switch：0x081eb9b7(==5) / 0x081ec396(==0xa) / 0x081ec7a2(==0xc) /
    // 0x081ecac5(==0xd) / 0x081ecb10(==8) 是逐层重新 call get_state() 的 if-else 链
    // （每层 `cmp $k,%eax; sete %al; test; je next`）。原先折成 switch 少 4 次
    // get_state 调用，属真语义缺段。
    if (user->get_state() == 5)                 // ---- 普通副本击杀 ----
    {
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
            // [b28d-solo-DisPatcherCore] P167/P315：ORIG 为 `operator!=` +
            // `make_pair<unsigned short&,int>(m_mobSeq, 1)`（mangled IRtiE/OS3_OS6_），
            // 原写法 == / (int) cast 生成 eq + make_pair<int,int> 变体。
            if (it != cntMap.end())
                it->second += 1;
            else
            {
                unsigned short seq = die.m_mobSeq;   // packed 字段不可绑非 const 引用
                cntMap.insert(std::make_pair(seq, 1));
            }
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
                // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 为 **3 个独立调用点**
                // （Ghidra C：dungeon==0 → (1,0,0) / index==0x2afe → (1,2,0) /
                // else → (1,0,0)），原三元折叠成 1 个（MISSING 0xcd×2 + getHackAnalyzer×2）。
                if (party->m_battleField.m_pDungeon == 0)
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0xcd, 1, 0, 0);
                }
                else if (party->m_battleField.m_pDungeon->get_index() == 0x2afe)
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0xcd, 1, 2, 0);
                }
                else
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0xcd, 1, 0, 0);
                }
                if (!party->checkBossRoom())
                {
                    if (party->m_battleField.m_pDungeon == 0)
                    {
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xce, 1, 0, 0);
                    }
                    else if (party->m_battleField.m_pDungeon->get_index() == 0x2afe)
                    {
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xce, 1, 2, 0);
                    }
                    else
                    {
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xce, 1, 0, 0);
                    }
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
                    user, (WongWork::ENUM_HACKTYPE)0xd1, 1, 0, 0);
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
                    user, (WongWork::ENUM_HACKTYPE)0x6f, 1, 0, 0);
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
                    static_cast<CParty*>(attacker->GetParty())->m_battleField.m_pDungeon
                        ->get_index() == 0x2afe;
            }
            if (zeroHitBossDg)
            {
                attacker->getHackAnalyzer()->addServerHackCnt(
                    attacker, (WongWork::ENUM_HACKTYPE)0x7f, 1,
                    die.m_killValue1, 0);
            }
            if (die.m_killValue2 == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6d, 1, 0, 0);
                die.m_killValue2 = 1;
            }
            if (die.m_hitCountList[i] == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6e, 1, 0, 0);
                die.m_hitCountList[i] = 1;
            }
            unsigned int perHit =
                die.m_damageList[i] / (unsigned int)die.m_hitCountList[i];
            if (limit < perHit &&
                (party->m_battleField.m_pDungeon == 0 ||
                 party->m_battleField.m_pDungeon->get_index() != 0x2afe))
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
                user, (WongWork::ENUM_HACKTYPE)0xca, 1, 0, 0);
        }
        // ValidateFixMobDiePos 在 ORIG 是 static（0x080f908c，this 不入栈），但 ORIG
        // 源码仍写成 `user->getHackAnalyzer()->ValidateFixMobDiePos(...)`：0x081ec355
        // 处**先 call getHackAnalyzer 再丢弃返回值**。故此处保留直呼。
        user->getHackAnalyzer();
        WongWork::CHackAnalyzer::ValidateFixMobDiePos(
            user, (unsigned short)monsterIdxOut,
            die.m_killPairA24, die.m_killPairA26);
    }
    else if (user->get_state() == 10)            // ---- 死亡塔 ----
    {
        WongWork::CDeathTower* tower = user->getDeathTower();
        if (tower == 0)
            goto die_mob_end;                    // ORIG 0x081ec3bb je 公共出口
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
                    user, (WongWork::ENUM_HACKTYPE)0xd4, 1, 0, 0);
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
                    user, (WongWork::ENUM_HACKTYPE)0x6e, 1, 0, 0);
                die.m_hitCountList[i] = 1;
            }
            unsigned int perHit =
                die.m_damageList[i] / (unsigned int)die.m_hitCountList[i];
            if (perHit > 12999)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6c, 1, 0, 0);
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
                    user, (WongWork::ENUM_HACKTYPE)0xcd, 1, 1, 0);
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xce, 1, 1, 0);
            }
        }
        else
        {
            user->m_mcap.m_monsterDieHackCnt = 0;
        }
        tower->handleKillMonster(
            user, (unsigned short)die.m_mobSeq,
            (unsigned short)die.m_fieldF, die, &monsterIdxOut);
        user->getHackAnalyzer();                 // ORIG 0x081ec761（同上，直呼丢弃）
        WongWork::CHackAnalyzer::ValidateFixMobDiePos(
            user, (unsigned short)monsterIdxOut,
            die.m_killPairA24, die.m_killPairA26);
    }
    else if (user->get_state() == 12)            // ---- BossTower ----
    {
        if ((unsigned short)die.m_fieldF == 0xffff)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0xcd, 1, 0, 0);
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0xce, 1, 0, 0);
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
                    user, (WongWork::ENUM_HACKTYPE)0x6f, 1, 0, 0);
                die.m_killValue1 = 1;
            }
            if (die.m_hitCountList[i] == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6e, 1, 0, 0);
                die.m_hitCountList[i] = 1;
            }
            if (die.m_killValue2 == 0)
            {
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0x6d, 1, 0, 0);
                die.m_killValue2 = 1;
            }
        }
        Global::CBossTowerFunc::DieMob(
            user, (unsigned short)die.m_mobSeq,
            (unsigned short)die.m_fieldF, die, 0);
    }
    else if (user->get_state() == 13)            // ---- advancealtar ----
    {
        advancealtar::Manager::dieMob(
            user, (unsigned short)die.m_mobSeq,
            (unsigned short)die.m_fieldF, die);
    }
    else if (user->get_state() == 8)             // ---- 战争房间 ----
    {
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
    }
die_mob_end:
    return 0;
}

#include "DisPatcherCore/read_39.inc"
{
    // ORIG 0x08232af4: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
#include "DisPatcherCore/process_39.inc"

#include "DisPatcherCore/dtor_DisPatcher_DieCharacInDungeon.inc"
#include "DisPatcherCore/check_error_21.inc"
{
    // ORIG 0x081c2bc0：`xor $0x1,%eax; test; je` = if(!check) return -1; return 0。
    if (!Global::CGlobalFunc::Get_State_Check((ch_state)user->get_state()))
        return -1;
    return 0;
}
#include "DisPatcherCore/check_state_5.inc"
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
#include "DisPatcherCore/read_40.inc"
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
#include "DisPatcherCore/send_18.inc"
{
    // ORIG 0x081c2baa：指针局部落栈 -0x4 → 取 +0x4 m_errNo → test %eax,%eax
    // 后 `return`（tail-merge 后残留 dead test）。
    stDieCharacInDungeon* pParam = static_cast<stDieCharacInDungeon*>(&param);
    if (pParam->m_errNo)
        return;
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
// [b28d-29] P30/N4 + N82：ORIG 成员偏移两步寻址（movzwl 0xf(%eax) 折叠）vs 4.4 add 链；参数副本槽差，语义集合全等  (ORIG 0x081c2a12 insn 119->126)
// 否则 BloodHandleDieUser + m_errNo=0x7fffffff，返 0。CBossTowerFunc 为真直接返 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 51（ORIG 119 / ours 126），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_40.inc"
{
    // [G5-DisPatcherCore] P0/P183：ORIG 0x081c2a1d 唯一具名引用是 **param**（`mov %eax,-0x1c`），
    // msg 不具名（`mov 0x10(%ebp),%eax; mov %eax,0x8(%esp)` 直传 check_error）——
    // 我方原 `MSG_DIE_CHARACTER& req = ...` 多一个 -0x20 槽；改 msg 成员在使用点内联 cast。
    stDieCharacInDungeon& out = static_cast<stDieCharacInDungeon&>(param);
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0xcbb, __PRETTY_FUNCTION__, out.m_errNo, user->get_acc_id());
    }

    // ORIG 读包体 ushort：+0xf 先入 esi、+0xd 后入 ebx（callee-saved 保活，**无具名槽**），
    // 作 CharacDie 第 2/3 参（实参右→左 ⇒ +0xf 先求值）。我方原两个 const 具名局部
    // 各占 4B 槽并多出 store/load 对（P0/P329）。
    user->m_historyLog.CharacDie(
        user->getCurCharacName(),
        *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const MSG_DIE_CHARACTER*>(&msg)->m_pad),      // +0xd
        *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const MSG_DIE_CHARACTER*>(&msg)->m_pad + 2));  // +0xf
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

#include "DisPatcherCore/dtor_DisPatcher_UseCoin.inc"
// ORIG 0x081ca7d6 T _ZN18DisPatcher_UseCoin11check_errorEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseCoin/check_error.md 逐块映射）：user->get_state()==5 →
// Dungeon_check_error；==8 → WarRoom_check_error；其余 → 0x13。
#include "DisPatcherCore/check_error_22.inc"
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
#include "DisPatcherCore/read_41.inc"
{
    // 具名字段（MSG_USE_COIN pack(1) m_0d ushort；ORIG 0x081ca5f3 add $0xd +
    //  call _ZN9PacketBuf9get_shortERt → 2B 无符号，重载一致）
    MSG_USE_COIN& req = *reinterpret_cast<MSG_USE_COIN*>(&msg);
    if (!packet.get_short(req.m_0d))
    {
        return LineFunc(0x1ebb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "DisPatcherCore/send_19.inc"
{
    stUseCoinParam& out = static_cast<stUseCoinParam&>(param);
    if (out.m_errNo == 0x7fffffff)
        return;
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
// [b28d-29] N82/P211：帧 0x28 vs 0x38 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081ca852 insn 122->125)
// UseCoin(eCoinSubReason 4) + set_authen_data(3,1,0) + user->IncreaseUsedCoinCount；返回 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 47（ORIG 121 / ours 126），残差=N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_41.inc"
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
    // [Lane5] P251/P279：ORIG 0x081ca8a6 `call get_state; cmp $0x5,%eax; je;
    // cmp $0x8,%eax; je; jmp default` = 无槽 switch(call结果)，if 链会带状态槽。
    switch (user->get_state())
    {
    case 5:
    {
        CParty* party = (CParty*)user->GetParty();  // ORIG thunk 0x865514c = CUser::GetParty
        if (party == 0)
        {
            return LineFunc(0x1f5a,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }
        // ORIG 0x081ca929 `mov %al,-0x9(%ebp); movzbl -0x9(%ebp),%eax; xor $1`（具名 bool 槽）
        bool freeRevival = party->checkFreeRevivalCondition((CDungeon const*)0);
        if (!freeRevival)
        {
            // ORIG 无具名局部（`call getCurCharacInvenW; movl $0; mov %eax,(%esp); call UseCoin`）
            user->getCurCharacInvenW()->UseCoin(eCoinSubReason_0);   // asm push $0x0
            user->getCurCharacQuestW()->set_authen_data(QUEST_CONDITION_3, 1, 0);  // asm (3,1,0)
            party->IncreaseUsedCoinCount();   // thunk 0x822d8ce = CParty::IncreaseUsedCoinCount
            // ORIG 首参 0x25（_ActionGroupIndex 枚举，直压立即数）
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, (APSystem::_ActionGroupIndex)0x25, 0, (ENUM_PACKETCLASS)0);
        }
        break;
    }
    case 8:
        // ORIG 同样无具名局部（结果直传实参槽）
        user->getCurCharacInvenW()->UseCoin((eCoinSubReason)4);  // asm push $0x4
        user->getCurCharacQuestW()->set_authen_data(QUEST_CONDITION_3, 1, 0);
        user->IncreaseUsedCoinCount();      // thunk 0x823082a = CUser::IncreaseUsedCoinCount
        break;
    }
    return 0;
}


#include "DisPatcherCore/dtor_DisPatcher_GiveUpGame.inc"
// ORIG 0x081c41e8 T _ZN21DisPatcher_GiveUpGame11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_GiveUpGame/check_error.md 逐块映射）：state ∈
// {5,10,12,13} → 0；其余 → 0x13（ORIG 每次比较前调 get_state，勿缓存）。
#include "DisPatcherCore/check_error_23.inc"
{
    if (user->get_state() != 5 && user->get_state() != 10
        && user->get_state() != 0xc && user->get_state() != 0xd)
    {
        return 0x13;
    }
    return 0;
}
#include "DisPatcherCore/read_42.inc"
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
#include "DisPatcherCore/process_42.inc"
{
    (void)param;   // ORIG 不写 param

    const int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0xff7, __PRETTY_FUNCTION__, 0, user->get_acc_id());
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

#include "DisPatcherCore/dtor_DisPatcher_GetItem.inc"
#include "DisPatcherCore/check_error_24.inc"
{
    // ORIG 0x081c35ac：msg 存入局部（-0x4）未使用，恒返 0。
    MSG_BASE& req = msg;   // ORIG -0x4(%ebp)
    (void)req;
    return 0;
}
// ORIG 0x081c335c T _ZN18DisPatcher_GetItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_GetItem/read.md（汇编+反编译 C 全读）
#include "DisPatcherCore/read_43.inc"
{
    // 载荷字段具名（MSG_GET_ITEM，PacketTypes.h:582 pack(1)；宽度按 ORIG 081c335c：
    //   get_shortERt @+0xd/+0x11/+0x13/+0x15/+0x1a/+0x1c/+0x18 → ushort 字段；
    //   get_byteERc @+0xf/+0x10 → uchar 字段以 char& 适配；
    //   081c34d9 `mov %al,0x17(%edx)` → uchar 字段单字节直写）
    // ORIG 0x81c34d9 `mov %al,0x17(%edx)` = QI 直存：bool lvalue = bool 返回（probe 实证，
    // uchar/char 混搭 RHS 必产 %dl 中转）。以函数内同布局视图承载 m_17=bool。
    // pack(1) 对齐 MSG_GET_ITEM（PacketTypes.h 同为 pack(push,1)）。
    #pragma pack(push, 1)
    struct View
    {
        char m_head[0xd];
        unsigned short m_0d;
        unsigned char m_0f;
        unsigned char m_10;
        unsigned short m_11;
        unsigned short m_13;
        unsigned short m_15;
        bool m_17;
        union { unsigned char m_pad18[2]; unsigned short m_18; };
        unsigned short m_1a;
        unsigned short m_1c;
    };
    #pragma pack(pop)
    static_assert(sizeof(View) == 0x1e, "View layout");
    View& req = *reinterpret_cast<View*>(&msg);
    if (!packet.get_short(req.m_0d))
    {
        return LineFunc(0xe2d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_0f)))
    {
        return LineFunc(0xe2f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_10)))
    {
        return LineFunc(0xe30,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_11))
    {
        return LineFunc(0xe33,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_13))
    {
        return LineFunc(0xe34,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x15 get_short 不检失败；返回值（bool）低字节直写 +0x17
    // ORIG 0x081c34d9 `mov %al,0x17(%edx)`：bool lvalue = bool 返回（QI 直存，probe 实证）
    req.m_17 = packet.get_short(req.m_15);
    if (!packet.get_short(req.m_1a))
    {
        return LineFunc(0xe38,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_1c))
    {
        return LineFunc(0xe39,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x18 在权威头仍是 char m_pad18[2]（ORIG 此处 mov 宽度 2B = ushort）：
    // 已列「待 PacketTypes.h 权威字段 m_18(ushort) 落地」清单，暂以具名成员 +
    // 同宽度引用视图表达，不用裸偏移。
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req.m_pad18)))
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
#include "DisPatcherCore/send_20.inc"
{
    stGetItemParam& p = static_cast<stGetItemParam&>(param);
    // ORIG 0x081c3ce8：==0x7fffffff → void early-return（call;jmp;nop;merge 岛）；
    // 余下 ==0 走 Ok，else Error。
    if (p.m_errNo == 0x7fffffff)
        return;
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
// ORIG T 0x0889209d（定义 OnlyOrigBatch11.cpp:141）：奇偶门，返回 1=通过 0=失败。
// [b28d-29] N82/P211：帧 0x50 vs 0x60 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081c35c0 insn 484->511)
#include "DisPatcherCore/checkGetItemPacket.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 355（ORIG 481 / ours 512），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_43.inc"
{
    stGetItemParam& out = static_cast<stGetItemParam&>(param);
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
    // [G5-ProcB] ORIG 0x081c3642 `mov 0x10(%ebp),%eax; mov %eax,-0x20(%ebp)`：
    // msg 视图具名局部在两级 errNo 检查之后初始化（ORIG 参数槽 -0x24 先、msg 槽
    // -0x20 后）。函数头声明多 1 条 store 且槽基下移（我方帧 0x60 vs ORIG 0x50）。
    MSG_GET_ITEM& req = *reinterpret_cast<MSG_GET_ITEM*>(&msg);
    if (req.m_17 != 1)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x3bb, 1, 0, 0);
    }
    if (req.m_15 % 7 != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x12f, 1, req.m_11, req.m_13);
    }
    // [BLOCKED 依赖缺失] cUserHistoryLog::SetItemXYPos（ORIG W 0x0822f084
    // _ZN15cUserHistoryLog12SetItemXYPosEtt）无头文件声明且全仓无定义：
    // 待补齐后于 addServerHackCnt(0x12f) 之后调用
    // user->m_historyLog.SetItemXYPos(req.m_1a, req.m_1c)。
    user->m_historyLog.SetItemXYPos(req.m_1a, req.m_1c);   // ORIG 0x81c3731
    if (user->get_state() == 5)
    {
        CParty* party = (CParty*)user->GetParty();
        if (party != 0)
        {
            // ORIG 0x81c3782..0x81c3804：GetItemTimeChecker(now, m_1a, m_1c, 0x5dc)
            // 非 0 且双通道 diff（+0x8e444/+0x8e446）任一 >500 → 计 0x130
            const unsigned int nowTick =
                GlobalData::s_systemTime_.getCurTickCount();
            if (user->GetItemTimeChecker(nowTick, req.m_1a, req.m_1c, 0x5dc) != 0 &&
                // CUser+0x8e444/0x8e446 = 内嵌 m_mcap(+0x8e3f0) 的 +0x54/+0x56
                // （CMCAPManager.h m_timeChkDiffA/m_timeChkDiffB）；ORIG 0x081c37b2
                // `movzwl 0x8e444(%eax); cmp $0x1f4,%ax; jg`（0x081c37cd `jle`）=
                // 2B 比较 + **有符号序**，与头内 short 声明一致 ⇒ 取 short 视图
                // （轴 B 正向修复：旧 unsigned short 视图产 ja/jbe）。
                (*reinterpret_cast<const short*>(
                     &user->m_mcap.m_timeChkDiffA) > 0x1f4 ||
                 *reinterpret_cast<const short*>(
                     &user->m_mcap.m_timeChkDiffB) > 0x1f4))
            {
                user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x130, 1, 0, 0);
            }
            // ORIG 0x81c3809..0x81c387d：GetItemSerialChecker(m_1a, m_1c, m_0d)==2 → 计 0x131
            if (user->GetItemSerialChecker(req.m_1a, req.m_1c, req.m_0d) == 2)
            {
                user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x131, 1, 0, 0);
            }
            // ORIG 0x81c3882..0x81c38a0：IsRoutingState → m_errNo=0xd8
            if (party->IsRoutingState())
            {
                out.m_errNo = 0xd8;
                return 0;
            }
            int outIdx = 0;
            unsigned char outH = 0;
            bool outB = false;
            if (!party->get_item(user, req.m_0d, req.m_0f == 1, (char)req.m_10,
                                 outIdx, outH, outB))
            {
                out.m_errNo = 0x7fffffff;   // ORIG 0x81c3cd2
                return 0;
            }
            // ORIG 0x81c3912..0x81c39ac：IsAutoCreated || member_count==1 →
            // checkGetItemPacket(seed, m_11, m_13)==0 → 计 0x12e
            if (party->IsAutoCreated() || party->get_member_count() == 1)
            {
                if (checkGetItemPacket(user->getBackupSeed(),
                                       req.m_11, req.m_13) == 0)
                {
                    user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x12e, 1, 0, 0);
                }
            }
            // ORIG 0x81c39b1..0x81c3b2b：CBattle_Field(party+0xb24).get_dungeon_index()
            // → find_dungeon；null 或 outIdx==-1 → logFormat 0xf14；否则
            // m_entryRestrictFlag && m_10∈{2,3,4} && !outB → 0x56 包 send_all 广播
            CBattle_Field* bf =
                &party->m_battleField;
            const int dungeonIdx = bf->get_dungeon_index();
            CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
            if (dungeon == 0 || outIdx == -1)
            {
                LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                                      __PRETTY_FUNCTION__, 0xf14,
                                      "find_dungeon(%d) fail", dungeonIdx);
            }
            else if (dungeon->m_entryRestrictFlag != 0 &&
                     (req.m_10 == 2 || req.m_10 == 3 || req.m_10 == 4) &&
                     !outB)
            {
                PacketGuard packet;
                packet.put_header(0, 0x56);
                packet.put_byte(5);
                packet.put_short((int)(user->get_unique_id() & 0xffff));
                packet.put_int(outIdx);
                packet.finalize(true);
                G_GameWorld()->send_all(packet);
            }
            out.m_errNo = 0;
            return 0;
        }
        out.m_errNo = 0x7fffffff;
        return 0;
    }
    if (user->get_state() == 10)
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
    if (user->get_state() == 0xc)
    {
        // ORIG 0x81c3bdf..0x81c3c19：CBossTowerFunc::GetItem(user, m_0d, m_0f==1,
        // (char)m_10)，随后无条件置 m_errNo=0x7fffffff（asm +0x659）
        Global::CBossTowerFunc::GetItem(user, req.m_0d,
                                        req.m_0f == 1, (char)req.m_10);
        out.m_errNo = 0x7fffffff;
        return 0;
    }
    if (user->get_state() == 8)
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


#include "DisPatcherCore/dtor_DisPatcher_UseStack.inc"
// ============================================================================
// ORIG T 0x081f63c0 _ZN19DisPatcher_UseStack12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_UseStack/dispatch_sig.md，1320 insn）
// 读 slot(+0)/itemSpace?/itemId/slotIdx/checksum → state 白名单（5,7,8,3,6,
// 10,0xc）→ checksum 反作弊 0x25b → 封印限制 0xd1 → GetItemPtr/CheckItem(0x2a)
// → 可堆叠使用地点 → 组队/TOD/锦标赛限定（错误包 header(1,0x2f) 系列尾部）
// → use_stackable 落地。
// ============================================================================
// §6.2 巨件核销：P167 208/208 [0缺0多]；gap 1320→1320/1321；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] N82/P183：帧 0x120 vs 0x140 + ORIG 两步 add 寻址 vs 4.4 独立局部槽；语义集合全等  (ORIG 0x081f63c0 insn 1320->1321)
// 无真修条目（batch-20 半程核销语义未回退）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 1374（ORIG 1307 / ours 1321），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [IG3-semantic] 差一候选our `cmpl $0x1e6c` ↔ ORIG `cmp $0x1e6d`（ORIG 0x081f6c30）逐点核实为**等价，记 N**：
//   本文件 9654 行源码写 `itemId >= 0x1e6d`；ORIG 决策树 `cmp $0x1e6d,%eax; jb 0x81f6c4d(default)`，
//   4.4 对无符号 `>=` 发对偶 `cmpl $0x1e6c; jbe 下一比较`（Ghidra C `uVar1 < 0x1e6d ⇒ goto LAB_081f6c4d` 互证）；
//   v<=0x1e6c 路径经 `cmp $0x28978a`（恒不等）后仍落同一错误包块（0x81f6c4d ↔ 我方 183e5 段），无可见行为差。
//   同函数扫描：我方多出的比较常量仅此一处（非发明防御）；ORIG 多的 0x16/0x18/0x33 各 1 次
//   = 4.1 switch 树「je + 范围判定」重复比较，检查常量集合两侧一致（非缺失校验），故不改源码。
#include "DisPatcherCore/dispatch_sig_17.inc"
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
            user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
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
        // ORIG 4 处均调 const 重载 _ZNK10CInventory11GetInvenRefEii：
        // getCurCharacInvenR() 返 const CInventory*，const 命中重载。
        const CInventory* invR = reinterpret_cast<const CUserCharacInfo*>(user)
            ->getCurCharacInvenR();
        const Inven_Item* ref =
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
            if (party->m_battleField.m_bloodState == 1)
            {
                PacketGuard pgParty1;
                USESTACK_ERR(pgParty1, 0);
                return 0;
            }
            if (party->m_battleField.m_pDungeon != 0 &&
                party->m_battleField.m_pDungeon->isTowerOfDespairDungeon() &&
                party->m_battleField.m_pDungeon->limitOfStackableItemInTower())
            {
                const CInventory* invR2 = reinterpret_cast<const CUserCharacInfo*>(user)
                    ->getCurCharacInvenR();
                const Inven_Item* ref2 =
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
                            &party->m_battleField)
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
            if (party->m_battleField.m_pDungeon != 0 &&
                party->m_battleField.m_pDungeon->isTournamentDungeon())
            {
                const CInventory* invR3 = reinterpret_cast<const CUserCharacInfo*>(user)
                    ->getCurCharacInvenR();
                const Inven_Item* ref3 =
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
                else if (
                     /* ORIG 0x081f75ca `movzbl 0x364(%eax),%eax; test %al,%al`：
                        1B 零扩展读具名字段 m_char364（CStackableItem.h:476，
                        声明为 char，故此处按 ORIG 宽度取无符号视图；
                        原转写作 *(int*)(stack+0xd90)——偏移与宽度均与 ORIG 不符）*/
                     *reinterpret_cast<const unsigned char*>(&stack->m_char364) != 0 &&
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
                party->m_battleField.m_bloodState != 1)
            {
                PacketGuard pgTower;
                USESTACK_ERR(pgTower, 0);
                return 0;
            }
        }
    }

    {
        const CInventory* invRFin = reinterpret_cast<const CUserCharacInfo*>(user)
            ->getCurCharacInvenR();
        const Inven_Item* refFin =
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
#include "DisPatcherCore/read_44.inc"
{
    // ORIG 0x082331b2: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
// ORIG 0x082331bc W _ZN19DisPatcher_UseStack7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制
// 即为空实现（read 同为空）。UseStack 全部业务（读包/校验/应答 0x2f 系列）内嵌于
// dispatch_sig(0x081f63c0，见 docs/class_func_reports/DisPatcher_UseStack/dispatch_sig.md)。
#include "DisPatcherCore/process_44.inc"


#include "DisPatcherCore/dtor_DisPatcher_MoveMap.inc"
// [b28c-L19 属主修复] 参数名曾被注释但函数体用 &msg ⇒ TU 编译失败，恢复具名。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 71 / ours 71 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/check_error_25.inc"
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
    // ORIG +0x45/+0x63：每次 call 现场 `mov -0x10(%ebp),%eax; add $0xb24,%eax`，
    // BF 地址不入槽（技36 删中转）；ORIG +0x77/+0x7e 双 int 常量岛 + `test %al,%al`
    // = \|\| 短路物化绑 %al（技22/40），无 invalid 槽。
    // [b28d-solo-DisPatcherCore] P0：ORIG 无 invalid 槽（尾 `test %al,%al` 直判）
    // ⇒ 原码是内联 || 表达式；具名 bool 局部使 4.4 多占 4B 槽并整体平移（-0x14/-0x10
    // vs ORIG -0x10/-0xc）。
    if (((int)req.m_y >= party->m_battleField.getMapHeight()) ||
        ((int)req.m_x >= party->m_battleField.getMapWidth()))
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
#include "DisPatcherCore/check_state_6.inc"
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
#include "DisPatcherCore/read_45.inc"
{
    MoveMapMsgView& mv = *reinterpret_cast<MoveMapMsgView*>(&msg);
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
    mv.m_1a = packet.get_short(
        *reinterpret_cast<unsigned short*>(mv.m_pad18));   // ORIG `mov %al,0x1a(%edx)`
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
    if (packet.get_short(mv.m_33) != true)
    {
        return LineFunc(0x116e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_int(mv.m_35) != true)                  // ORIG add $0x35 单步
    {
        return LineFunc(0x1181,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
// ORIG 0x081c4b7a T _ZN18DisPatcher_MoveMap20CheckTrapBitsHackCntEP5CUserR12MSG_MOVE_MAP
// msg.m_18 ushort 非 7 的倍数 → 记 0x1f9；累计 >4 时对队伍其余成员记 0x1fa、
// [b28d-29] N-P223/N-JMP：唯一残差=cond 尾 4.4 多保留 jmp 岛（ORIG 0x081c4bb5 je 直落 epilogue，4.1 线程化折掉），ndiff 20->1  (ORIG 0x081c4b7a insn 108->111)
// 落库 DB_ReqSaveHackUserPunish 并 DisConnSig(0x18,1,0) 强制断线。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 1（ORIG 107 / ours 108），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/CheckTrapBitsHackCnt.inc"
{
    // ORIG 为 0x2493 魔数除法序列：q=v/7 后 r=v-q*7，r!=0 即非整除
    // PacketTypes.h MSG_MOVE_MAP::m_trapBits（ushort@+0x18，Main 已代落）真成员直读。
    // b28c-28（P0 清绑，P168a④ 裸形复测翻案）：原
    // `register unsigned short trapBits asm("ecx") = info.m_trapBits;` 绑定与 P0
    // 相悖，已删除并改为使用点直读成员表达式。旧判词称「4.4 恒产 movzwl+mov 两步、
    // 保留绑定使 divseq 保持 16 位形」系误判：实测清绑后 `info.m_trapBits % 7 == 0`
    // 直出 ORIG 的 `mov 0x10(%ebp),%eax; movzwl 0x18(%eax),%ecx; movzwl %cx,%eax;
    // imul $0x2493` 16 位 HImode 魔数除法链，ndiff 20 → 1。
    // [b28d-solo-DisPatcherCore] P214① 正向包裹：原 `if(cond){return;}` 早返形令 4.4
    // 生成空 return bb（`nop`）+ 循环出口 `jmp <epilogue>`（ORIG 0x081c4bb5 `je` 直落
    // epilogue 且无岛）⇒ 改 `if (bits%7 != 0) { body }` 后 4.4 发 `test; je L_after`，
    // 循环出口 fall-through 到 epilogue，残差 1→0。
    if (info.m_trapBits % 7 != 0)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x1f9, 1, 0, 0);
        // ORIG 0x081c4...：`cmp $0x4,%eax; setg %al; test; je` —— 正向 >4 大块（技巧17）
        if (user->getHackAnalyzer()->getServerHackAccumulatedCnt((WongWork::ENUM_HACKTYPE)0x1f9) > 4)
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0)
            {
                for (int idx = 0; idx <= 3; ++idx)   // ORIG cmpl $0x3; setle（尾块无 jmp）
                {
                    // ORIG `xor $0x1,%eax; test %al,%al; jne`：!boolcall continue 形（技巧1）
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
        }
    }
}
// ORIG 0x081c4d26 T _ZN18DisPatcher_MoveMap26CheckOneMapPlayTimeHackCntEP5CUserR12MSG_MOVE_MAP
// 以队伍地图游玩时长（死亡塔为 stage 通关时限/1000+5）为基准，逐成员修正
// msg.m_23[i] 上报 tick：>10000 部分按万位上报（0x2afe 副本记 0x82 否则 0x77）；
// job3 / job4 觉醒2 减半 ×0.5 否则 ×0.8；未开局且 >limit 时按 t<=1.5*limit
// [b28d-29] N82/P183：帧 0x64 vs 0x94 + 槽平移，ORIG 无缓存局部而 4.4 新增；语义集合全等  (ORIG 0x081c4d26 insn 412->366)
// 单次、超幅 ×10 次记 0x74/0x81。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 516（ORIG 411 / ours 363），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P211/N18a 中转/store 序/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] P167 差集 5 缺→0 缺（CALLSITE CLEAR，39/39）：ORIG 0x081c4e82/
// 0x081c4eda/0x081c50e3/0x081c5145/0x081c520a/0x081c526c 六处判定各自重新 call
// CUser::GetParty 并直读 +0xcac（不设 dungeon 局部）；0x081c4f83/0x081c4f93 两次
// get_charac_job；0x081c505a 的 check_start_point 用函数头 user party 缓存（-0x24）
// 而非 member 的 party。改后 ours 363→377（ORIG 411）。
// ⚠️ 本件 ndiff 370→398（+28）：ORIG `==0` 带 sete 物化，4.4 O0 恒折直分支（N78/
// setcc-fold 族）⇒ 形态代价，语义正确性优先故保留；回滚只需撤销本函数三处展开。
// [SweepDispatchCore 2026-09-09] P183 修复：删 `unsigned int& slot = info.m_23[idx]`
// 与 `CBattle_Field& battleField` 两处引用缓存，全部改 `info.m_23[idx]` / 内联取址
// 表达式（ORIG 13 处每次重算 `mov idx; add $8; mov 0x3(base,idx,4)`，实证 0x081c4e0a/
// 0x4e30/0x4f6d/0x4fbc/0x5075/0x50dd/0x50f7/0x5164/0x51a9/0x521e/0x528b/0x52d0）；
// halfTick 具名 bool 改 if/else 表达式（ORIG 无 bool 槽：0x4fac `mov $1` / 0x4fb3
// `mov $0` 岛直 test）。gap 32→-10（ours 380→422，ORIG 412），p167 CLEAR（39/39），
// 四数不回退（213/639/2/74）。
#include "DisPatcherCore/CheckOneMapPlayTimeHackCnt.inc"
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
        if (info.m_23[idx] > 10000)
        {
            // [b28e-L5] 判据⑫（clr_l orig=5 / our=3）：ORIG 0x081c4e22 与 0x081c4e29
            // 各有一条 `movl $0x0,-0x14(%ebp)` / `movl $0x0,-0x10(%ebp)`（两个具名
            // int 局部**预清**），随后 0x081c4e4f/0x081c4e79 才回写商与余数 ⇒ 原码是
            // 「声明=0 + 赋值语句」两句式（P198① 预清必写 / 判据⑫ 缺具名局部），
            // 初始化式会被 4.4 折成单条 store（我方案发 2 条预清）。
            unsigned int quotient = 0;
            unsigned int remainder = 0;
            quotient = info.m_23[idx] / 10000;
            remainder = info.m_23[idx] % 10000;
            // ORIG 不为 dungeon 设局部：0x081c4e82（==0 判定）与 0x081c4eda
            // （else-if 取 index）各自重新 call CUser::GetParty 并直读 +0xcac。
            if (static_cast<CParty*>(member->GetParty())->m_battleField.m_pDungeon == 0)
            {
                member->getHackAnalyzer()->addServerHackCnt(
                    member, (WongWork::ENUM_HACKTYPE)0x77, 1, quotient, 0);
            }
            else if (static_cast<CParty*>(member->GetParty())
                         ->m_battleField.m_pDungeon->get_index() == 0x2afe)
            {
                member->getHackAnalyzer()->addServerHackCnt(
                    member, (WongWork::ENUM_HACKTYPE)0x82, 1, quotient, 0);
            }
            else
            {
                member->getHackAnalyzer()->addServerHackCnt(
                    member, (WongWork::ENUM_HACKTYPE)0x77, 1, quotient, 0);
            }
            info.m_23[idx] = remainder;
        }
        // ORIG 0x081c4f83 / 0x081c4f93 两处各自 call get_charac_job，结果经
        // `mov $1 / $0` 岛物化后直接 test（无 bool 槽）⇒ 表达式直用于 if/else。
        // ORIG .rodata：0x8bd6320=0.5 / 0x8bd6328=0.8（fildll×const 后 fistpll 截断）
        if (member->get_charac_job() == 3 ||
            (member->get_charac_job() == 4 &&
             member->getCurCharFirstGrowType() == 2))
        {
            info.m_23[idx] = (unsigned int)((double)info.m_23[idx] * 0.5);
        }
        else
        {
            info.m_23[idx] = (unsigned int)((double)info.m_23[idx] * 0.8);
        }

        // ORIG 0x081c505a 用 -0x24(%ebp)（= 函数头 user->GetParty() 的缓存）+0xb24，
        // 不另取 member 的 party；P183：ORIG 单次取址，无引用缓存槽。
        if (!reinterpret_cast<CPartyBattleFieldView*>(party)
                 ->m_battleField.check_start_point() &&
            info.m_23[idx] > limit)
        {
            if ((double)info.m_23[idx] <= 1.5 * (double)limit)   // 0x8bd6330=1.5
            {   // 单次记录
                // ORIG 0x081c50e3 / 0x081c5145：两判定各自重新 call GetParty。
                if (static_cast<CParty*>(member->GetParty())
                        ->m_battleField.m_pDungeon == 0)
                {
                    member->getHackAnalyzer()->addServerHackCnt(
                        member, (WongWork::ENUM_HACKTYPE)0x74, 1, info.m_23[idx], 0);
                }
                else if (static_cast<CParty*>(member->GetParty())
                             ->m_battleField.m_pDungeon->get_index() == 0x2afe)
                {
                    member->getHackAnalyzer()->addServerHackCnt(
                        member, (WongWork::ENUM_HACKTYPE)0x81, 1, info.m_23[idx], 0);
                }
                else
                {
                    member->getHackAnalyzer()->addServerHackCnt(
                        member, (WongWork::ENUM_HACKTYPE)0x74, 1, info.m_23[idx], 0);
                }
            }
            else
            {   // 超幅：同一记录重复 ×10（ORIG 循环 10 次，无 break）
                for (int rep = 0; rep < 10; ++rep)
                {
                    // ORIG 0x081c520a / 0x081c526c：同上，逐次重新取 party。
                    if (static_cast<CParty*>(member->GetParty())
                            ->m_battleField.m_pDungeon == 0)
                    {
                        member->getHackAnalyzer()->addServerHackCnt(
                            member, (WongWork::ENUM_HACKTYPE)0x74, 1, info.m_23[idx], 0);
                    }
                    else if (static_cast<CParty*>(member->GetParty())
                                 ->m_battleField.m_pDungeon->get_index() == 0x2afe)
                    {
                        member->getHackAnalyzer()->addServerHackCnt(
                            member, (WongWork::ENUM_HACKTYPE)0x81, 1, info.m_23[idx], 0);
                    }
                    else
                    {
                        member->getHackAnalyzer()->addServerHackCnt(
                            member, (WongWork::ENUM_HACKTYPE)0x74, 1, info.m_23[idx], 0);
                    }
                }
            }
        }
    }
}
// ORIG 0x081c5330 T _ZN18DisPatcher_MoveMap7processEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 120 / ours 120 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_45.inc"
{
    stMoveMapParam& out = static_cast<stMoveMapParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG `mov %eax,%edx; mov <out>,%eax; mov %edx,0x4(%eax)`。
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
    MoveMapMsgView& req = *reinterpret_cast<MoveMapMsgView*>(&msg);
    // ORIG 0x081...：`movzbl 0x1a(%eax),%eax; xor $0x1,%eax; test %al,%al; je` =
    // bool 字段 ! 物化（唯 bool lvalue 产 xor$1；uchar 出 cmp$1）。视图字段判 + 恢复传参。
    if (!req.m_1a)
    {   // 客户端未置 0x1a 标志 → 记 0x25a
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25a, 1, 0, 0);
    }
    static_cast<Secu_HackLogCheck*>(user->GetHackCheck())->MoveMap();

    CheckTrapBitsHackCnt(user, *reinterpret_cast<MSG_MOVE_MAP*>(&req));
    CheckOneMapPlayTimeHackCnt(user, *reinterpret_cast<MSG_MOVE_MAP*>(&req));

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
    party->move_map(*reinterpret_cast<MSG_MOVE_MAP*>(&req));
    return 0;
}

#include "DisPatcherCore/dtor_DisPatcher_SetPlayResult.inc"
// ============================================================================
// ORIG T 0x081f4ce6 _ZN24DisPatcher_SetPlayResult12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetPlayResult/dispatch_sig.md，1083 insn）
// state==5 → party 校验（state==2 直接 0 / m_battleField.m_bloodState==1 跳过）→ GameResultType
// 读包（Clear 后逐字段直写 party 结果区【wire 布局按 ORIG +0x17f4..0x1846】）→
// 地图清除日志/金币/经验比率反作弊（0x1fb、0x78、0x75）/疲劳特化（job 3,4 且
// growtype 2 → ×0.5 否则 ×0.8）/清副本 timer 注册/角色信息核对（0xe2、0xe3）/
// 使用技能材料比率（0xe5、0xe4）/尾巴成员药丸检查（0x1a2）→
// CheckRecvFromAllMember → SetPlayResult。
// ============================================================================
// SetPlayResult::dispatch_sig（ORIG 0x081f4ce6，报告 dispatch_sig.md 全读）。
// 残 gap≈260 归 N78（Main 批准 2026-09-05）：ORIG +0x7e0..+0x906（081f54c6/
// 081f5528/081f559e）4.1 将 memset 0x703 / memcpy 0x676 / memcpy 0x78 inline 成
// rep stos/rep movsl 逐段展开；4.4 O0 恒 `call memset/memcpy`（GDataC8 探针实证
// 同族 N78），单 TU 源码写法不可翻。+0x99..+0xc4 blood-dungeon 守卫缺段已补
// [b28d-29] P0 清绑（register asm("al") 已删）+ P167 修正 5 处 get_int(int&) 重载
// （ORIG 0x081f4f57/0x081f59cc.. 实证 ERi；coinSlot 0x081f50dd 保持 ERj）。
// 残差：CHARAC_DATA/TAG_CHARAC_DATA 本仓建模为 POD（ORIG 有 C1 call，P104 建模差）
// + 帧 0x1d0c vs 0xf54（N82 4.1 不复用大缓冲槽）+ setcc-fold。  (ORIG 0x081f4ce6)
// （语义修复保留）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 943（ORIG 1083 / ours 816），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P168a④ bool 槽化/P211/N18a；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] **N129 登记（Main 2026-09-09 裁定）：本件残留的 2 个 MISSING ctor 属
// 「ORIG 死复制/死构造区：缺 callee 但净效果为零 ⇒ 不补」，后来泳道请勿再试图补。**
// 逐地址证据（ORIG T 0x081f4ce6 内 0x081f5489..0x081f5650，0xe2 检查块；
// 0x081f56e8.. 的 0xe3 块同构）：
//   0x081f5489 lea -0xe9e(%ebp),%eax ; 0x081f5492 call _ZN11CHARAC_DATAC1Ev  (W 0x0822ec4c)
//   0x081f5497 lea -0x828(%ebp),%eax ; 0x081f54a0 call _ZN15TAG_CHARAC_DATAC1Ev (W 0x0822edae)
//   0x081f54ab..  rep stos  memset(-0x15bb, 0, 0x703)
//   0x081f5500..  rep movs  memcpy(dst=-0x15af  ← src=-0xe9e, 0x676)   ← CHARAC_DATA 全量
//   0x081f55fc    call _ZNK5CUser13GetCharacInfoEP15SIG_LOAD_CHARAC (0x0864bc04) ← 整体覆写 dst
//   0x081f5635    movl $0xe2 … addServerHackCnt
// ⇒ 被复制的 CHARAC_DATA 在块内**没有任何填充调用**（上述区间总共只有 4 个 call），
//   拷进去的是刚 Reset 的零对象，且目标缓冲随后立刻被 GetCharacInfo 全量覆写 ⇒ 净效果为零。
// ⇒ 若为消 MISSING 而补这两个对象，必须连带补 2 次 rep movs（4.4 O0 只会发 call memcpy，
//   ORIG 侧无 memcpy 符号）⇒ 净增 2 项 EXTRA，形态与语义双亏 ⇒ **判不修**。
// 本件其余部分已按 P183（ORIG 逐处重调、我方局部缓存）展开 5 个无副作用 getter：
//   实测 ndiff 772→726，P167 计数 87→92 与 ORIG 相等，行为等价。
// [BigSetPlayResult] 2026-09-09 复核（ORIG 1083 / ours 874，gap 209，P167 92/92）：
// 本件真缺口 2 类 4 处，均已修（全部在 N129 带之外的 0xe2/0xe3 检查块）：
//  ① 0xe2 块原读 `(char*)&sigCharac + sizeof(SIG_LOAD_CHARAC)` = +0x703 **越界**；
//     ORIG 0x081f5605 实证读 -0x158a = 缓冲 -0x15bb + 0x31
//     = m_characData(+0xc)::m_lev(+0x25)（pack(1)）。
//  ② 0xe3 块原读 `(char*)&sigChk + 0`（m_accId 低半字）；ORIG 0x081f583f 读 -0x1c8d
//     = 缓冲 -0x1cbe + 0x31，同为 m_characData.m_lev。
//  ③ 两块的 GetCharacInfo 返回值门禁缺失（原为逗号表达式 `(call(), cmp)`，返回值被丢弃）；
//     ORIG 0x081f5601/0x081f583b 均为 `test %al,%al; je` 先于字段读取（即 && 短路）。
// 形态收敛（P0 局部建模，语义不变）：
//  - 金币余数区改回 ORIG 的两个具名局部（over=-0x34 / rest=-0x30，均在
//    addServerHackCnt 之前算好；原写法 %10000 在 call 之后重读内存）。
//  - 疲劳比率区改为 if/else 两分支各自内联（ORIG 0x081f5202/0x081f526b），
//    消除 bool 槽 + double ratioDelta 局部；常量实证 .rodata 0x8bd6320=0.5 /
//    0x8bd6328=0.8 / 0x8bd6354=100.0f / 0x8bd6358=99.98f。
// 残差 = N129/N78 带 223 条（0x081f5489..0x081f5650 / 0x081f56eb.. 两段
// CHARAC_DATA/TAG ctor + memset/memcpy 内联展开；我方为 SIG_LOAD_CHARAC C1 +
// call memset）+ 其余 233 条未匹配全为形态（帧 0x1d0c vs 0xf54、槽平移、
// fucompp vs fucomip、局部缓存指针）。
#include "DisPatcherCore/dispatch_sig_18.inc"
{
    if (user->get_state() != 5)
        return 0;

    // ORIG +0x31..+0x45：GetPartyIndex 单次调用（movswl %ax,%ebx 缓存），勿重复。
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
        return LineFunc(0x8155,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (party->m_battleField.m_bloodState == 1)
        return 0;
    // ORIG +0x99..+0xc4（前批缺段）：party 非空且 party+0xcac(dungeon) 非空且
    // dungeon+0x89f(m_bloodDungeonType) 有符号 >0 → 直接返 0。dungeon 独立具名槽
    // （ORIG -0x3c(%ebp)，技13）。
    if (party != 0)
    {
        CDungeon* dungeon = party->m_battleField.m_pDungeon;      // ORIG -0x3c(%ebp)
        if (dungeon != 0)
        {
            if ((char)dungeon->m_bloodDungeonType > 0)   // ORIG `test %al,%al; jle`
                return 0;
        }
    }

    // ---- 结果区：party+0x17f4 为 GameResultType（CParty.h 现为权威成员
    //      GameResultSet m_gameResultSet @+0x17f4，GameResultSet : GameResultType
    //      且零增成员 ⇒ 布局/偏移不变，直接取其址，无视图 cast）；
    //      字段偏移/宽度证据 docs/class_func_reports/DisPatcher_SetPlayResult/
    //      dispatch_sig.md：0x17fd→m_field9 / 0x17fe→m_fielda / 0x183e→m_field4a /
    //      0x1808+8i→m_shot[i].m_count / 0x180c+8i→m_shot[i].m_field4 /
    //      0x1840→m_field4c / 0x17f8→m_field4 / 0x17fc→m_field8）----
    GameResultType* gameResult = &party->m_gameResultSet;   // +0x17f4
    gameResult->Clear();

    if (!packet.get_byte(*reinterpret_cast<char*>(&gameResult->m_field9)))  // +0x17fd
        return LineFunc(0x816d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(&gameResult->m_fielda))) // +0x17fe
        return LineFunc(0x8172,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned short resultIdx = 0;
    // ORIG 0x081f4e5f `call _ZN9PacketBuf9get_shortERt`（unsigned short&）——
    // 原写法用 `*reinterpret_cast<short*>` 会绑到 ERs 重载，与 ORIG 符号不符，
    // 现按权威字段类型 m_field4a(ushort) 直传。
    if (!packet.get_short(gameResult->m_field4a))   // +0x183e
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
        unsigned short memberVal = 0;
        int memberCoin = 0;
        // 每成员结果二元组 = GameResultType::m_shot[i]{m_count(ushort),m_field4(int)}
        // （m_shot@+0x14、stride 8 → party+0x1808/0x180c；ORIG 0x081f4eec 以
        //  (i+0x300)*8+8 的「全局 8B 槽号」形态算址，此处按真实成员布局表达，
        //  P30 结构性寻址伪影差异已标注）
        if (!packet.get_short(gameResult->m_shot[i].m_count))
            return LineFunc(0x817e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_int(gameResult->m_shot[i].m_field4))
            return LineFunc(0x8181,
                            __PRETTY_FUNCTION__,
                            0, 0);
        (void)memberVal;
        (void)memberCoin;
    }
    user->m_historyLog.MapClear(
        gameResult->m_field4a,
        static_cast<int>(party->GetMapPlayingTime() / 1000), 0);

    int killCountRead = 0;              // ORIG local_ea8（P167：ORIG get_int(int&) ERi）
    if (!packet.get_int(killCountRead))
        return LineFunc(0x818b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // ORIG +0x375：party 外层重判（cmpl $0,-0x48; je 尾）；&& 条件物化双常量岛
    // （`!check()` xor 形先行、`kill!=0` 后行 → mov$1/mov$0 岛 + test al; je）。
    if (party != 0)
    {
        // [b28d-29] P0 清绑：原 `register bool bossCheat asm("al")` 违 P0/N96，已删；
        // ORIG 短路物化岛（mov $1/$0 + test）4.4 恒折直分支 ⇒ 形态差记 N（setcc-fold）。
        bool bossCheat = !party->checkBossRoom() && killCountRead != 0;
        if (bossCheat)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x1fb,
                static_cast<unsigned int>(killCountRead), 0, 0);
        }
    }
    // +0x1840 = GameResultType::m_field4c（头内声明 int；ORIG 0x081f50dd 以
    // get_int(unsigned int&) ERj 装载，故保留同宽度无符号引用视图）
    unsigned int* coinSlot =
        reinterpret_cast<unsigned int*>(&gameResult->m_field4c);
    if (!packet.get_int(*coinSlot))
        return LineFunc(0x8195,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (*coinSlot > 10000)
    {
        // ORIG 0x081f5134/0x081f515b 两次独立读 party+0x1840，分别算 over(-0x34)
        // 与 rest(-0x30)，二者均在 addServerHackCnt 之前算好（P0 具名局部）。
        unsigned int over = *coinSlot / 10000;
        unsigned int rest = *coinSlot % 10000;
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x78, 1, over, 0);
        *coinSlot = rest;
    }

    // ORIG 本件 _ZNK15CUserCharacInfo14get_charac_jobEv 计数 = 2（我方原存 `int job`
    // 局部复用只 1 次）：与 DisPatcher_MoveMap::CheckOneMapPlayTimeHackCnt
    // 0x081c4f83/0x081c4f93 同型 —— 原码两处判定各自重新 call（getter 无副作用，
    // 展开后可观察行为完全不变）。
    // ORIG 0x081f5202/0x081f526b：if/else 两分支各自完整内联
    // fildll/fldl/fmulp/fistpll（真分支 _DAT_08bd6320=0.5、假分支
    // _DAT_08bd6328=0.8）⇒ 原码无 ratioDelta 局部（P0）；疲劳特化
    // job==3 或 job==4 且 growtype==2 → ×0.5。
    if (reinterpret_cast<CUserCharacInfo*>(user)->get_charac_job() == 3 ||
        (reinterpret_cast<CUserCharacInfo*>(user)->get_charac_job() == 4 &&
         reinterpret_cast<CUserCharacInfo*>(user)
             ->getCurCharFirstGrowType() == 2))
    {
        *coinSlot = static_cast<unsigned int>(
            static_cast<long long>(0.5 * static_cast<double>(*coinSlot)));
    }
    else
    {
        *coinSlot = static_cast<unsigned int>(
            static_cast<long long>(0.8 * static_cast<double>(*coinSlot)));
    }
    if (party->GetMapPlayingTime() / 1000 < *coinSlot)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x75, 1, *coinSlot, 0);
    }

    CBattle_Field& field =
        *reinterpret_cast<CBattle_Field*>(&party->m_battleField);
    gameResult->m_field4 = field.get_dungeon_index();          // +0x17f8
    gameResult->m_field8 =
        static_cast<unsigned char>(field.get_dungeon_diff());  // +0x17fc
    party->IncreaseSetPlayResultCount(user);

    if (party->get_dungeon_clear_state() == 0)
    {
        if (!field.IsClearDungeon())
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x196, 1, 0, 0);
            return 0;
        }
        int key = party->gen_timer_key((TIMER_MESSAGE)0x29);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                    party->GetPartyIndex(),
                                    (TIMER_MESSAGE)0x29, 0xf, key, 0);
        party->set_dungeon_clear_state(1);
    }
    // ORIG _ZN13CBattle_Field17GetCurrentMapInfoEv 计数 = 2：第一个判定直呼，
    // 之后才取具名局部供后续两处复用（同族"ORIG 不缓存"形）。
    if (field.GetCurrentMapInfo()->m_killChecker1.m_useSkillMaterialTime == 0)  // +0x4c
    {
        SIG_LOAD_CHARAC sigCharac;               // ORIG local_15bf 清零缓冲
        memset(&sigCharac, 0, sizeof(sigCharac));
        // ORIG 0x081f5601 `test %al,%al; je`（返回值门禁）+ 0x081f5605
        // `movzwl -0x158a(%ebp),%eax`：-0x158a = 缓冲基址 -0x15bb + 0x31
        // = SIG_LOAD_CHARAC::m_characData(+0xc)::m_lev(+0x25)（pack(1)）。
        if (user->GetCharacInfo(&sigCharac) &&
            0x1d < sigCharac.m_characData.m_lev)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0xe2, 1, 0, 0);
        }
    }
    MapInfo* mapInfo = field.GetCurrentMapInfo();
    if (mapInfo->m_killChecker1.m_useSkillMaterialTime != 0)
    {
        int useSkillMaterialCount =
            mapInfo->m_killChecker1.getUseSkillMaterialCount();
        unsigned int playSec = party->GetMapPlayingTime() / 1000;
        unsigned int tickUnits = playSec / 10;
        if (playSec % 10 != 0)
            ++tickUnits;
        // ORIG 0x081f56df `mov -0x44(%ebp),%eax; cmp -0x28(%ebp),%eax; jae`：
        // int 与 unsigned tickUnits 的**无符号**比较（int→unsigned 隐式提升）。
        // 原 `static_cast<int>(tickUnits)` 会生成有符号 jle，count 为负时行为不同。
        if (static_cast<unsigned int>(useSkillMaterialCount) < tickUnits)
        {
            SIG_LOAD_CHARAC sigChk;
            memset(&sigChk, 0, sizeof(sigChk));
            // ORIG 0x081f583b `test %al,%al; je` + 0x081f583f
            // `movzwl -0x1c8d(%ebp),%eax`：-0x1c8d = 缓冲基址 -0x1cbe + 0x31
            // = 同上 m_characData.m_lev。
            if (user->GetCharacInfo(&sigChk) &&
                0x1d < sigChk.m_characData.m_lev)
                user->getHackAnalyzer()->addServerHackCnt(
                    user, (WongWork::ENUM_HACKTYPE)0xe3, 1, 0, 0);
        }
    }

    unsigned int playMs = party->GetMapPlayingTime();     // ORIG local_44
    if (playMs < 10000 &&
        10 < reinterpret_cast<CUserCharacInfo*>(user)->get_charac_level())
    {
        int fastCnt = 11 - playMs / 1000;                 // ORIG local_28
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0xe5,
            static_cast<unsigned int>(fastCnt), 0, 0);
    }
    if (playMs < 20000 &&
        10 < reinterpret_cast<CUserCharacInfo*>(user)->get_charac_level())
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0xe4, 1, 0, 0);
    }

    int memberKillSum = 0;               // local_ebc（P167：ORIG get_int(int&)）
    int coinUsedA = 0;                   // local_eac
    int coinDenomA = 0;                  // local_eb4
    int expGain = 0;                     // local_eb0
    int expDenomB = 0;                   // local_eb8
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
            // ORIG 0x8231a9/0x8232f6：GetInvenSlot(0, 0xc) 与 GetInvenSlot(0, 0xe)
            // （invenType=0，槽 0xc/0xe）；NEW 原为 (1,0xb)/(1,0x9) 属转写错误。
            // ORIG 里 _ZN6CParty9GetMemberEv / _ZNK15CUserCharacInfo18getCurCharacInvenREv
            // 计数各为 2 ⇒ 两个槽各自重新取 inventory（原码不缓存 memberInv）。
            Inven_Item slotItem =
                party->GetMember()[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 0xc);
            if (slotItem.m_addInfo == 0x2ad4)
            {
                pillFound = true;
                break;
            }
            Inven_Item slotItem2 =
                party->GetMember()[i].m_pUser->getCurCharacInvenR()->GetInvenSlot(0, 0xe);
            if (slotItem2.m_addInfo == 0x329e)
            {
                pillFound = true;
                break;
            }
        }
        if (!pillFound)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACKTYPE)0x1a2, 1, 0, 0);
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
#include "DisPatcherCore/read_46.inc"
{
    // ORIG 0x0823302a: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
#include "DisPatcherCore/process_46.inc"

#include "DisPatcherCore/dtor_DisPatcher_DropItem.inc"
// ORIG 0x081c2d9a T _ZN19DisPatcher_DropItem11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_DropItem/check_error.md 汇编+反编译 C 全文转写）：
// 角色空/数量<1 → -1；state==10 → 0x13；组队血副本 → 0x13；综合PVP频道 → 0x13；
// isRestricted(1,1) → param.m_c=1,0xd1；m_padTail!=1 时安全保护 0x1e（param.m_c=1,返码）；
// CheckItemLock → 0xd5；isHackUser → 0x13；space 1 → 0x17；space 3 且 pos<10 → 0x17；
// [b28d-29] N82/P211：形参副本槽 + store 装载序差，调用点与槽集合全等  (ORIG 0x081c2d9a insn 146->148)
// space 0 且 pos==0 且 count>1000 → 0x46；否则 0。（ORIG +0x18b 存在未读死局部 6）
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 56（ORIG 146 / ours 148），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error_26.inc"
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
    // [G5-MiscC] P0：ORIG 0x081c2e15 `mov -0x18(%ebp),%eax; mov 0xcac(%eax),%eax;
    // mov %eax,-0x10(%ebp)` 有具名槽 -0x10（随后 0x081c2e1e `cmpl $0x0,-0x10(%ebp)`、
    // 0x081c2e24 `mov -0x10(%ebp),%eax; movzbl 0x89f(%eax),%eax` 复用该槽）
    // ⇒ m_pDungeon 结果原码是具名局部，不是两次链式直读。
    CDungeon* dungeon = party->m_battleField.m_pDungeon;
    if (dungeon != 0 && dungeon->m_bloodDungeonType != 0)
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
    // [G5-MiscC] P251/P261：ORIG 0x081c2f2c-0x081c2f40 是**单次**载入
    // （`mov -0x20(%ebp),%eax; movzbl 0x11(%eax),%eax; movsbl %al,%eax`）后
    // `cmp $0x1,%eax; je <case1 0x081c2f42>` / `cmp $0x3,%eax; je <case3 0x081c2f49>` /
    // `jmp <default 0x081c2f5d>` = switch 直比；case3 内 0x081c2f4c
    // `movzwl 0x12(%eax),%eax; cmp $0x9,%ax; jg <default>`（16 位无符号）
    // ⇒ 原码是 switch（我方两次 if 会重复载入 m_11 并多一次 cmp）。
    switch ((int)(char)req.m_11)
    {
    case 1:
        return 0x17;
    case 3:
        if (req.m_12 <= 9)
        {
            return 0x17;
        }
        break;
    default:
        break;
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
#include "DisPatcherCore/read_47.inc"
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
#include "DisPatcherCore/send_21.inc"
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
// [b28d-29] N88/N82：3 处 ORIG 有 `mov $0x0,%eax` 死写入（写后未读），4.4 惰性求值位置错位；语义等价  (ORIG 0x081c2f94 insn 208->212)
// m_errNo/m_8；其它 m_errNo=0x13。
// [b28d-solo-DisPatcherCore] 复核 NEAR：ndiff 4（ORIG 208 / ours 208），残差=N82 槽/块序；N 封顶（详见上方 b28d-29 归因）。
// [b28d-PoolPatchCore] 改判+修复：原残差实为 2 处 jle/je 极性差（旧 unsigned 形封死）。drop_item/
// HandleItemDrop 返回 int，ORIG `cmpl $0,-0x28; jle` ⇒ 原码 `if (ret > 0)`（负返回不写 m_errNo，
// 旧 !=0 为真实语义偏差）已修（P321-T2 直证）。现残差 = 2 块 store 装载序（ORIG RHS-first vs
// 4.4 LHS-first）：字段 m_errNo(int) 与 RHS ret(int) 同型、ret 又受 jle 约束须 int，P211 双旋钮
// 均不可动 ⇒ N76 封。NEAR(2)，bad 总数不增。
#include "DisPatcherCore/process_47.inc"
{

    // ORIG +0x09/+0x0f：req(msg)→远槽 -0x34、out(param)→近槽 -0x30（技21 声明序）；
    // errNo store 双钉（技45）。state 无槽：三段各自 `call get_state; cmp $K,%eax;
    // sete %al; test %al,%al; je`（技9 (bool)==K 物化形）。
    MSG_DROP_ITEM& req = *reinterpret_cast<MSG_DROP_ITEM*>(&msg);
    stDropItemParam& out = static_cast<stDropItemParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG `mov %eax,%edx; mov <out>,%eax; mov %edx,0x4(%eax)`。
    out.m_errNo = check_error(user, msg, param);
    if (out.m_errNo >= 1)
    {
        out.m_8 = req.m_11;
        return 0;
    }
    if (out.m_errNo < 0)
    {
        // ORIG 81c2ffc：LineFunc(0xdcc, pretty, errNo, acc_id) 并返回其值。
        return LineFunc(0xdcc, __PRETTY_FUNCTION__, out.m_errNo,
                        user->get_acc_id());
    }
    if (user->get_state() == 5)
    {
        CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party != 0)
        {
            // [b28d-PoolPatchCore] P321-T2：ORIG `cmpl $0x0,-0x28; jle` ⇒ 原码带符号
            // `if (ret > 0)`（负值不写 errNo，旧 !=0 写法为真实语义偏差）。
            int ret = party->drop_item(
                user, req.m_0d, req.m_0f, (char)req.m_11, req.m_12, req.m_14);
            if (ret > 0)
            {
                // [b28d-solo-DisPatcherCore] P211 档③：ORIG RHS-first
                // （`mov -0x28,%edx; mov -0x30,%eax; mov %edx,0x4(%eax)`）⇒ RHS 局部异型化。
                // [b28e-L5] P211 档③ 镜像试验（RHS 侧 `(unsigned)` 转换节点）实测**无效**
                // ——4.4 仍出 LHS-address-first `mov -0x30(%ebp),%eax; mov -0x28(%ebp),%edx`
                // ⇒ 不改字段符号性该旋钮不生效；m_errNo 必须 signed（`<0`/`>0` 语义）
                // 故头侧不可动，本处按 N76 封顶（ORIG RHS-first 不可造）。
                out.m_errNo = ret;
                out.m_8 = req.m_11;
            }
        }
    }
    // [b28d-solo-DisPatcherCore] P169：ORIG 各 state 块尾单 `jmp +0x2b3`（跳共享尾
    // `mov $0,%eax`）⇒ 原码是 if/else-if 链 + 函数尾单一 return 0（goto 尾标签实测反劣）。
    else if (user->get_state() == 0xc)
    {
        // ORIG 81c35b0：user->getBossTower()（0x08655220：idx<0→0 否则
        // mgr->getBossTower(idx)）→ vtable+0x20 handleDropItem。
        WongWork::CBossTower* tower = user->getBossTower();
        if (tower != 0)
        {
            tower->handleDropItem(user, req.m_0d, req.m_0f, (char)req.m_11,
                                  req.m_12, req.m_14);
        }
    }
    else if (user->get_state() == 8)   // ORIG +0x1df `jmp +0x2b3`
    {
        WarRoom* room = G_CGameManager()->GetWarRoom(user->GetWarRoomIndex());
        if (room != 0)
        {
            // [b28d-PoolPatchCore] P321-T2：同上，ORIG `cmpl $0x0,-0x1c; jle`。
            int ret = room->HandleItemDrop(
                user, req.m_0d, req.m_0f, (char)req.m_11, req.m_12, req.m_14);
            if (ret > 0)
            {
                out.m_errNo = ret;   // P211 档③（同上）
                out.m_8 = req.m_11;
            }
        }
    }
    else
    {
        out.m_errNo = 0x13;
        out.m_8 = req.m_11;
    }
    return 0;
}


#include "DisPatcherCore/dtor_DisPatcher_UseEquip.inc"
#include "DisPatcherCore/check_error_27.inc"
{
    MSG_DECREASE_DURABILITY* req = reinterpret_cast<MSG_DECREASE_DURABILITY*>(&msg);  // ORIG 0x81c0ee8 入口 mov 0x10→-0x14，其后不读（dead）
    (void)req;
    // ORIG 0x81c0ef4..0x81c0f4c：各 clause `call get_state; cmp $k; je false_lbl`，
    // 全不匹配 fall→mov $1；merge `test %al,%al; je` 非零 → mov $0x13。&& 值物化形。
    if (user->get_state() != 5 && user->get_state() != 7 &&
        user->get_state() != 8 && user->get_state() != 0xa &&
        user->get_state() != 0xc)
    {
        return 0x13;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        if (party->m_battleField.m_bloodState == 1)                    // +0xcd8
        {
            return 0x13;
        }
        CDungeon* dungeon = party->m_battleField.m_pDungeon;           // +0xcac（ORIG -0xc）
        if (dungeon != 0 && dungeon->m_bloodDungeonType != 0)  // +0x89f
        {
            return 0x13;
        }
    }
    return 0;
// [b28d-29] AE：正文 21=21 全等，仅 .o 重定位/字符串地址显示差（跨二进制布局产物）  (ORIG 0x081c0dfa insn 21->22)
}
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 21 / ours 21 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/read_48.inc"
{
    // ORIG 0x081c0dfa：实参内联表达式（mov 0x10(%ebp);add $0xd），无指针局部 → 帧 0x18。
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 调 `get_byte(char&)`（_ZN9PacketBuf8get_byteERc），
    // 原写法经 uchar 字段绑到 ERh 重载。
    if (!packet.get_byte(*reinterpret_cast<char*>(
            &reinterpret_cast<MSG_DECREASE_DURABILITY&>(msg).m_0d)))
    {
        return LineFunc(0x9d0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "DisPatcherCore/send_22.inc"
{
    stOnUseEquipParam& out = static_cast<stOnUseEquipParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x33);
    if (out.m_errNo != 0)
    {
        guard.put_byte(0);
        guard.put_byte(out.m_errNo);
    }
    else
    {
        guard.put_byte(1);
        guard.put_byte(static_cast<char>(out.m_8));
    }
    guard.finalize(true);
    user->Send(guard);
}
// ORIG 0x081c0e4a T _ZN19DisPatcher_UseEquip7processEP5CUserR8MSG_BASER9ParamBase
// err=check_error；>0：SendCmdErrorPacket(0x33, err&0xff) 返回 -1；==0：
// m_errNo=getCurCharacInvenW()->use_equip((int)(sbyte)msg.m_0d)，m_8=msg.m_0d，
// 返回 err。
#include "DisPatcherCore/process_48.inc"
{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x33, (unsigned char)(errNo & 0xff));
        return -1;
    }
    // b28 清违规：register asm 绑定 P0 全域禁用（N96），恢复纯 C++ 单语句
    // 链形（P52：slot 表达式先物化跨 receiver 调用存活；P200：拓宽直传实参）。
    // 残差 `movsbl→eax 中转再落 ebx` = N18a 单段直落不可造，封顶。
    stOnUseEquipParam& out = static_cast<stOnUseEquipParam&>(param);
    MSG_DECREASE_DURABILITY& req = *reinterpret_cast<MSG_DECREASE_DURABILITY*>(&msg);
    out.m_errNo = user->getCurCharacInvenW()->use_equip((int)(signed char)req.m_0d);
    out.m_8 = req.m_0d;   // ORIG +0x85 `movzbl 0xd(%eax),%edx` 单步 QI 直存（同型）
    return errNo;
}


#include "DisPatcherCore/dtor_DisPatcher_MakePVP.inc"
#include "DisPatcherCore/ExtractPacket.inc"
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
    // ORIG +0x1a9：先无条件 movb $0，再 cmp $1/jne 分支补写 1（非 setne
    // 物化）→ 两步赋值形态。
    msg.m_battleMode = 0;
    if (tail == 1)
    {
        msg.m_battleMode = 1;
    }
    return true;
}
// [b28d-29] N82/P183：临时对象构造块（lea -0x10）位置前移 3 指令，difflib 块序伪差；语义集合全等  (ORIG 0x081f803e insn 103->104)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 40（ORIG 103 / ours 105），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_19.inc"
{
    // ORIG 0x081f803e T（报告 DisPatcher_MakePVP/dispatch_sig.md 全文转写）：
    // ExtractPacket 失败→LineFunc(0x85b6)；房名 slang→SendCmdErrorPacket(0x35,0x9f)；
    // 否则 MakePVPRoom(user,msg,0x35, mode==0?2:6)。
    MSG_MAKE_PVP_ROOM msg;
    memset(&msg, 0, 0x3d);
    if (!ExtractPacket(packet, msg))
    {
        // [b28d-solo-DisPatcherCore] P0：ORIG 无 res 槽（`mov %eax,%ebx` 直承载
        // 返回值跨 string dtor），原具名局部多一对 store/load 并使全帧深 4。
        return LineFunc(0x85b6, __PRETTY_FUNCTION__, 0, 0);
    }
    {
        std::string name(msg.m_name);
        if (CSyncSlangFilter::GetInstance()->HasSlang(name))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x35, 0x9f);
            return 0;               // [Lane5] ORIG +0x70：独立 `mov $0,%ebx; jmp dtor`
        }
        else
        {
            // ORIG +0xf7..+0x140：两独立调用块——`test; je` 远跳 2-块、6-块紧跟
            // fall（技17 fall 块在前，技34 双常数直落立即数 spill）。
            if (msg.m_battleMode != 0)
            {
                MakePVPRoom(user, msg, (ENUM_CMDPACKET)0x35, (PVP_BATTLE_MODE)6);
            }
            else
            {
                MakePVPRoom(user, msg, (ENUM_CMDPACKET)0x35, (PVP_BATTLE_MODE)2);
            }
        }
        return 0;                   // dtor 后 mov %ebx,%eax（ORIG +0x167/+0x172）
    }
}
#include "DisPatcherCore/read_49.inc"
{
    // ORIG 0x08233214: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}

// ORIG 0x0823321e W _ZN18DisPatcher_MakePVP7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部建房逻辑（ExtractPacket(0x081f81ba) 解包 MSG_MAKE_PVP_ROOM → CSyncSlangFilter::
// HasSlang 拒绝 0x35/0x9f → MakePVPRoom(cmd=0x35, mode=local_15?6:2)）内嵌于
// dispatch_sig(0x081f803e，报告 docs/class_func_reports/DisPatcher_MakePVP/dispatch_sig.md)。
#include "DisPatcherCore/process_49.inc"

// [b28d-29] P30/N4 + N82：ORIG 结构体基址 lea -0x1b + add $0xd 两步 vs 4.4 折 lea -0xe；槽平移 0xd，语义集合全等  (ORIG 0x081ed6c8 insn 152->151)
#include "DisPatcherCore/dtor_DisPatcher_EnterPVP.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 43（ORIG 152 / ours 151），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_20.inc"
{
    // ORIG 0x081ed6c8 T（报告 DisPatcher_EnterPVP/dispatch_sig.md 全文转写）：
    // get_short(pvpIdx) → 清 flag/seat/buf[9] → get_byte(flag)，flag==1 时
    // get_int(seat)（>0xa→LineFunc）+ get_binary(buf,seat)；
    // GetPvp(idx,user,0)==0 → 错误包(1,0x36)+0+0x16；否则 EnterPVPRoom(user,pvp,0x36,buf)。
    // [G5-DisPatcherCore] 负结论（已回滚）：ORIG 0x081ed6d0 对 pvpIdx 取址为两步
    // `lea -0x1b(%ebp),%eax; add $0xd,%eax`（基址 -0x1b + 0xd ⇒ 地址 -0xe），独立 `short`
    // 局部只产单条 `lea -0xe(%ebp)`。试形 `struct { char m_hdr[0xd]; short m_idx; }
    // __attribute__((packed));` ⇒ 4.4 报 `不能将紧实的字段 m_idx 绑定到 short&`
    // （packed 非对齐成员不可作 get_short(short&) 实参；4.1 同样非法）⇒ 原码不是 packed
    // 成员引用，`-0x1b` 基址的构造形未解出。本件残差仅此 1 条（ours=151 / ORIG=152）。
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
#include "DisPatcherCore/process_50.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPSeat.inc"
// ORIG 0x081f5eba T _ZN21DisPatcher_SetPVPSeat12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetPVPSeat/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=5 → 0；get_byte(seat) 失败或 seat∉[0,7] → LineFunc 0x82a4；get_byte(state)
// 失败 → 0x82a6；room==0 → 0x82a9；OnSetPVPSeatState(user,(int)(char)seat,state,2)!=1
// [b28d-29] AE：91=91 全等（同上重定位归一）  (ORIG 0x081f5eba insn 91->91)
// → room->reset() + G_CGameManager()->PutPvp(room)；返回 0。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 91 / ours 91 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/dispatch_sig_21.inc"
{
    if (user->get_state() <= 5)
    {
        return 0;
    }
    // ORIG 0x081f5eba：blob `char msg[15]`@-0x1b（帧 0x48），实参两段
    // `lea -0x1b(%ebp); add $0xd/0xe`（技26）；seat=msg[13]@-0xe、state=msg[14]@-0xd。
    // 尾读 seat `movzbl -0xe,%eax; movzbl %al,%edx`（技16 uchar→int 双零扩展）、
    // state `movzbl -0xd,%eax; movsbl %al,%eax`（(char) sext）。
    char msg[15];
    if (!packet.get_byte(*reinterpret_cast<char*>(msg + 0xd)) ||
        (char)msg[13] < 0 || (char)msg[13] > 7)
    {
        return LineFunc(0x82a4, __PRETTY_FUNCTION__, 0, 0);
    }
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 第二处调 `get_byte(unsigned char&)`
    // （_ZN9PacketBuf8get_byteERh），原写法为 char&（ERc）。
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(msg + 0xe)))
    {
        return LineFunc(0x82a6, __PRETTY_FUNCTION__, 0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x82a9, __PRETTY_FUNCTION__, 0, 0);
    }
    // seat=msg[13]@-0xe `(int)(char)` sext（`movsbl %al,%eax`→0x8(%esp)）；
    // state=msg[14]@-0xd `(int)(unsigned char)` 双零扩展（`movzbl %al,%edx`→0xc(%esp)）。
    // ORIG 0x081f5eba：call 后 `xor $0x1,%eax; test %al,%al; je`（技1 bool 取反）。
    if (!room->OnSetPVPSeatState(user, (int)(char)msg[13],
                                 (ENUM_SEAT_STATE)(int)(unsigned char)msg[14],
                                 (ENUM_WALKOUT_TYPE)2))
    {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
    return 0;
}
#include "DisPatcherCore/read_50.inc"
{
    // ORIG 0x0823308c: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
#include "DisPatcherCore/process_51.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPReady.inc"
#include "DisPatcherCore/check_error_28.inc"
{
    // ORIG 0x081c9b9c：`cmp $6; setne; test; je` = if(!=6) return 0x13; return 0。
    if (user->get_state() != 6)
        return 0x13;
    return 0;
}
#include "DisPatcherCore/read_51.inc"
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
#include "DisPatcherCore/send_23.inc"
{
    // ORIG 0x081c9bc6：param+0x4（stOnSetPvPReadyParam::m_errNo）> 0（jle 有符号）
    // → SendCmdErrorPacket(cmd=0x38, err&0xff)。
    stOnSetPvPReadyParam& out = static_cast<stOnSetPvPReadyParam&>(param);
    if (0 < out.m_errNo)
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x38,
            (unsigned char)(out.m_errNo & 0xff));
    }
}
// ORIG 0x081c9a14 T _ZN22DisPatcher_SetPVPReady7processEP5CUserR8MSG_BASER9ParamBase
#include "DisPatcherCore/process_52.inc"
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

#include "DisPatcherCore/dtor_DisPatcher_SetPVPTeam.inc"
// ORIG 0x081f6020 T _ZN21DisPatcher_SetPVPTeam12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] P30/N4：ORIG 结构体基址 lea -0x2c + add $0xd 两步，4.4 折单 lea -0x21；槽号平移随 N82  (ORIG 0x081f6020 insn 137->139)
// 报告：docs/class_func_reports/DisPatcher_SetPVPTeam/dispatch_sig.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 75（ORIG 137 / ours 138），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_22.inc"
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
        // [G5-SigA] P0/P329：ORIG 0x081f60eb `mov -0x10(%ebp),%eax; mov %eax,(%esp);
        // call GetRequestedMapNo` 之后结果**不经槽**，0x081f60f6 直接
        // `lea -0x34(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp);
        // call GetPvpMapInfo`（两次 call 之间 eax 保活）⇒ 原码是单语句
        // GetPvpMapInfo(room->GetRequestedMapNo(), info)，无 mapIdx 具名局部。
        // 原具名 int mapIdx 多出「store eax→槽 + load 槽→实参」2 条。
        CMapList::GetPvpMapInfo(room->GetRequestedMapNo(), info);
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
#include "DisPatcherCore/read_52.inc"
{
    // ORIG 0x082330ee: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
#include "DisPatcherCore/process_53.inc"

#include "DisPatcherCore/dtor_DisPatcher_DiePvP.inc"
// ORIG 0x081ece64 T _ZN17DisPatcher_DiePvP12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] N82：帧 0x6c vs 0x60 + 槽平移（ushort 数组 -0x36 vs -0x26），调用点全等  (ORIG 0x081ece64 insn 146->147)
// 报告：docs/class_func_reports/DisPatcher_DiePvP/dispatch_sig.md（汇编+反编译 C 全读）
// [Lane5] P227 已落（gap 146→146=0）：getter 内联进 CSwitchLog 构造即调用单表达式，
// trace ctor 提到 getter 之前的问题消除。残余=整帧槽深 +4（-0x36 vs -0x3a 等）+
// 帧 0x6c vs 0x5c（N82/N115 族），ndiff ~60。
#include "DisPatcherCore/dispatch_sig_23.inc"
{
    if (user->get_state() != 7)
    {
        return 0;
    }
    // [FL4 负结论] P232 撑帧法对本件无效：补 `char pad[0x10]` 后 `sub` 立即数虽对齐
    // 0x6c，但全部局部槽相对 ORIG 由「深 4」扩为「深 0x14」（ORIG PacketGuard -0x44 /
    // 我方 -0x58），ndiff 30→28 仅来自帧行本身 ⇒ 判 N82（4.1 按生命周期穿插 vs 4.4
    // 按首次使用分区的槽分配差），保持原帧。
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
    int battleMode = room->get_pvp_battle_mode();   // ORIG -0x4c 具名槽
    // [Lane5] P227/P37 全表达式临时链：ORIG 0x081ecea4..0x081eceb8 在 ctor **之前**
    // 求值 get_index/getCurCharacName/get_acc_name 到 %edi/%esi/%ebx 保活
    // （两语句形会把 ctor 提到 getter 之前）⇒ 构造即调用单表达式。
    CSwitchLog(__PRETTY_FUNCTION__, 0x6f22, 0, 0)(
        "pvp@log %s,%s,room(%d),%d,%d,%d",
        user->get_acc_name(), user->getCurCharacName(), room->get_index(),
        battleMode, result, 0x3a);
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
#include "DisPatcherCore/read_53.inc"
{
    // ORIG 0x08232b6a: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
#include "DisPatcherCore/process_54.inc"

#include "DisPatcherCore/dtor_DisPatcher_PvPTimeout.inc"
#include "DisPatcherCore/check_error_29.inc"
{
    // ORIG 0x081c5ebe：`cmp $7; setne; test; je` = if(!=7) return 0x13; return 0。
    if (user->get_state() != 7)
        return 0x13;
    return 0;
}
// ORIG 0x081c5d78 T _ZN21DisPatcher_PvPTimeout4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_PvPTimeout/read.md 逐块映射）：循环 i=0..7 共 8 次
// get_int → msg+0xd+i*4；失败 LineFunc(0x14a6, PRETTY, 0, 0)。
#include "DisPatcherCore/read_54.inc"
{
    // MSG_PVP_TIMEOUT（PacketTypes.h:1087 冻结）+0xd 起为 8 个 int 席位态。
    // ORIG 入口 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`；循环体
    // `mov -0xc,%eax; shl $0x2; add -0x10(%ebp),%eax; add $0xd,%eax` = 数组成员
    // 下标形 `req->seat[i]`（probe 实证：裸指针加法恒被 fold-const 折成 +0xd 先行）。
    #pragma pack(push, 1)
    struct ViewPacked { char head[13]; int seat[8]; };
    #pragma pack(pop)
    ViewPacked* req = reinterpret_cast<ViewPacked*>(&msg);
    for (int i = 0; i <= 7; ++i)
    {
        if (!packet.get_int(req->seat[i]))
        {
            return LineFunc(0x14a6,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
}
#include "DisPatcherCore/send_24.inc"
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
#include "DisPatcherCore/process_55.inc"
{
    const int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0x14b7, __PRETTY_FUNCTION__, 0, user->get_acc_id());
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());  // thunk 0x0865518e
    if (room == 0)
    {
        return LineFunc(0x14bc, __PRETTY_FUNCTION__, 0, user->get_acc_id());
    }
    // MSG_PVP_TIMEOUT 载荷自包头 +0xd 起（read 读 8 个 int），ORIG 直接把该地址
    // 作为 int* 传给 time_out；结果码写入 param+0x4（stOnPvPTimeOut::m_timeOutResult），
    // 由本类 send() 消费（>0 时 put_byte 发出）。局部引用声明次序镜像 ORIG 槽位
    // （param 先于 msg），保证逐指令一致。
    stOnPvPTimeOut& out = static_cast<stOnPvPTimeOut&>(param);
    MSG_PVP_TIMEOUT& rMsg = *reinterpret_cast<MSG_PVP_TIMEOUT*>(&msg);
    out.m_timeOutResult =
        room->time_out(reinterpret_cast<int*>(rMsg.m_pad));   // +0x0d 席位数组首元素
    return errNo;
}


#include "DisPatcherCore/dtor_DisPatcher_EndPvPResult.inc"
// ORIG 0x081f88ae T _ZN23DisPatcher_EndPvPResult12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_EndPvPResult/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state!=7 → 0；room==0 → LineFunc 0x8681；set_recv_pvp_end_result_flag 后
// recv_all 为真 → end_pvp_result + make_state_info/send_all + end_pvp + battle_reset；
// 否则 isRematchable&&recv_team 命中时对 m_matching(+0x6e4) vtable+0x80 槽直调
// [b28d-29] setcc-fold + N82：bool 物化方向（ORIG mov $0,%eax 岛，4.4 %ebx）+ 块位置，语义集合全等  (ORIG 0x081f88ae insn 130->136)
// （room==0 → logFormat 0x86b6），未命中 count==1 时插 end_result 定时器。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 35（ORIG 130 / ours 135），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_24.inc"
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

#include "DisPatcherCore/read_55.inc"
{
    // ORIG 0x0823333a: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
#include "DisPatcherCore/process_56.inc"

// [b28d-29] P183 反向 + N93：ORIG 缓存 &msg+0xd 到 -0xc 后 disp 访问，4.4 逐次 mov 0xc(%ebp)+add；两侧 store 偏移集合 {0x25..0x4d} 完全一致（语义等价）  (ORIG 0x081f039e insn 172->179)
#include "DisPatcherCore/dtor_DisPatcher_ResPvpRank.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 209（ORIG 172 / ours 179），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/ExtractPacket_1.inc"
{
    // ORIG 0x081f039e T（报告 DisPatcher_ResPvpRank/ExtractPacket.md 全文转写）：
    // PvpResultType::Clear → 11×get_short（短读后符号扩展写回 int 字段）→ 10×get_int。
    msg.m_result.Clear();
    // [G5-DisPatcherCore] P0/P161：ORIG 在 Clear 之后 `mov 0xc(%ebp),%eax; add $0xd,%eax;
    // mov %eax,-0xc(%ebp)` = **具名基址槽 -0xc**；其后每处读取均为
    // `mov -0xc(%ebp),%eax; add $0x44/$0x6c/…(%eax)`（基址+偏移两步）。我方原先写
    // `msg.m_result.xxx` 被折叠成单条 `add $0x51(%eax)`（0xd+0x44）⇒ 全函数形态相异。
    PvpResultType& result = msg.m_result;
    // ORIG 的 11 个 short 是**独立具名局部**（槽 -0xe/-0x10/…/-0x20，逐条 `lea -0xN(%ebp)`），
    // 不是数组 —— 数组形（我方原 `short s[11]`）恒产 `lea 基址; add $2k`。
    short s0;
    short s1;
    short s2;
    short s3;
    short s4;
    short s5;
    short s6;
    short s7;
    short s8;
    short s9;
    short s10;
    packet.get_short(s0);
    packet.get_short(s1);
    packet.get_short(s2);
    packet.get_short(s3);
    packet.get_short(s4);
    packet.get_short(s5);
    packet.get_short(s6);
    packet.get_short(s7);
    packet.get_short(s8);
    packet.get_short(s9);
    packet.get_short(s10);             // ORIG 无失败分支（恒返 1）
    packet.get_int(result.m_avg[11]);     // +0x44
    packet.get_int(result.m_field6c);     // +0x6c
    packet.get_int(result.m_field70);     // +0x70
    packet.get_int(result.m_field74);     // +0x74
    packet.get_int(result.m_field78);     // +0x78
    packet.get_int(result.m_avg[12]);     // +0x48
    // [G5-DisPatcherCore] 真 bug 修复：ORIG 该两处 `add $0x4c/%0x50,%eax` = **m_avg[13]/m_avg[14]**。
    // m_count 的权威偏移是 +0x54（ORIG PvpResultType::AddNewResult 0x085d5944 段 0x54(%eax) 出现 18 次，
    // 与 PvpResultType.h 布局 m_avg[15]@0x18 一致）；原写 m_count/m_winPoint 会错写 +0x54/+0x58。
    packet.get_int(result.m_avg[13]);     // +0x4c
    packet.get_int(result.m_avg[14]);     // +0x50
    packet.get_int(result.m_field7c);     // +0x7c
    packet.get_int(result.m_field80);     // +0x80
    result.m_avg[0] = s0;    // +0x18
    result.m_avg[1] = s1;    // +0x1c
    result.m_avg[2] = s2;    // +0x20
    result.m_avg[3] = s3;    // +0x24
    result.m_avg[8] = s4;    // +0x38
    result.m_avg[9] = s5;    // +0x3c
    result.m_avg[6] = s6;    // +0x30
    result.m_avg[7] = s7;    // +0x34
    result.m_avg[4] = s8;    // +0x28
    result.m_avg[5] = s9;    // +0x2c
    result.m_avg[10] = s10;  // +0x40
    return true;
}
// [b28d-29] N82/P197③：ORIG 两 dead 局部预清 + 槽序（-0x1c/-0x18/-0x10/-0xc），4.4 合并复用致全帧平移；语义集合全等  (ORIG 0x081f05ea insn 197->197)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 72（ORIG 197 / ours 197），残差=setcc-fold bool 物化岛/P168a④ bool 槽化；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/SavePvpRank.inc"
{
    // ORIG 0x081f05ea T（报告 DisPatcher_ResPvpRank/SavePvpRank.md 全文转写）：
    // 分数(0x89=视图+0x7c)/时间(0x8d=视图+0x80) 反作弊 → job 3/4-grow2 折算 → update_pvp_rank
    // → PVPRoom SetPVPResult/recv 链。
    // ORIG 0x081f05f2/0x081f05f8 双 `movl $0x0` 预清（P198③ 两句式声明）
    unsigned int score = 0;
    int playSec = 0;
    score = (unsigned int)msg.m_result.m_field7c;   // msg+0x89（视图 +0x7c）
    playSec = (int)msg.m_result.m_field80 / 1000;   // msg+0x8d（视图 +0x80），有符号除
    if (10000 < score)
    {
        // [G5-MiscC] P0/P198③：ORIG 0x081f063b/0x081f0642 双 `movl $0x0` 预清
        // （-0x10=商 / -0xc=余数 两个具名 int），随后 0x081f065e 写商、0x081f067e
        // 写余数，0x081f06b9 `mov -0xc(%ebp),%eax; mov %eax,-0x1c(%ebp)` 才回写 score
        // ⇒ 原码是「声明=0 + 赋值语句」两句式（我方原初始化式 + 直接回写无 -0xc 临时）。
        unsigned int over = 0;
        unsigned int rest = 0;
        over = score / 10000;
        rest = score % 10000;
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x76, 1, (int)over, (int)playSec);
        score = rest;
    }
    // ORIG _DAT_08bd6320=0.5（job3）/ _DAT_08bd6328=0.8（其余）双精度折算（(int) 截断语义）
    // [G5-MiscC] P0/P198②：ORIG 0x081f06ca-0x081f06fc 该 bool 无栈槽（三重条件各自
    // 重发 `call get_charac_job`/`call getCurCharFirstGrowType`，`mov $1/$0,%eax` 寄存器
    // 物化后 `test %al,%al; je <0.8 块>`）⇒ 原码是单条 if 内联短路表达式，非具名 bool。
    if (user->get_charac_job() == 3 ||
        (user->get_charac_job() == 4 && user->getCurCharFirstGrowType() == 2))
    {
        score = (unsigned int)(0.5 * (double)score);
    }
    else
    {
        score = (unsigned int)(0.8 * (double)score);
    }
    // ORIG 折算后直接写回 score（-0x1c），无 adjusted 具名局部（P0）
    if (playSec < score)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x73, 1, (int)score, (int)playSec);
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

#include "DisPatcherCore/dispatch_sig_25.inc"
{
    // ORIG 0x081f0282 T（报告 DisPatcher_ResPvpRank/dispatch_sig.md 全文转写）。
    if (user->get_state() != 7)
    {
        return 0;
    }
    MSG_RES_PVP_RANK msg;
    // ORIG 0x081f0282：bool 返回值具名槽 `mov %al,-0x9(%ebp); movzbl -0x9(%ebp),%eax`
    // 后 xor$1 取反（技巧1+8：槽复用于两处调用）。声明序 msg→ok ⇒ ok 更近 ebp。
    bool ok;
    ok = ExtractPacket(msg, packet);
    if (!ok)
    {
        return LineFunc(0x7736, __PRETTY_FUNCTION__, 0, 0);
    }
    ok = SavePvpRank(user, msg);
    if (!ok)
    {
        return LineFunc(0x7739, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
#include "DisPatcherCore/read_56.inc"
{
    // ORIG 0x08232e8e: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
// ORIG 0x08232e98 W _ZN21DisPatcher_ResPvpRank7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现（objdump 核实），全部逻辑位于 dispatch_sig(0x081f0282)：state==7 → MSG_RES_PVP_RANK ctor → ExtractPacket(0x081f039e)/SavePvpRank(0x081f05ea) + LineFunc 断言
#include "DisPatcherCore/process_57.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPMapIndex.inc"
// ORIG 0x081f8602 T _ZN25DisPatcher_SetPVPMapIndex12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] N82 + setcc-fold：帧 0x40 vs 0x30；`!flag` 已按 P168a 修正（xor 形），残余为槽平移与 4.4 sete 折痕  (ORIG 0x081f8602 insn 175->179)
// 报告：docs/class_func_reports/DisPatcher_SetPVPMapIndex/dispatch_sig.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 59（ORIG 175 / ours 178），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_26.inc"
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
    // [b28d-29] P168a：ORIG 0x081f87d6 `movzbl flag; xor $0x1,%eax; test %al,%al`
    // ⇒ 原码是裸 `!flag`（bool 取反物化），`flag != true` 使 4.4 反产 test/sete。
    if (!flag && room->get_pvp_battle_mode() == 4)
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
#include "DisPatcherCore/read_57.inc"
{
    // ORIG 0x082332d8: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
#include "DisPatcherCore/process_58.inc"

#include "DisPatcherCore/dtor_DisPatcher_DebugCommand.inc"
#include "DisPatcherCore/read_58.inc"
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

#include "DisPatcherCore/process_59.inc"

#include "DisPatcherCore/dtor_DisPatcher_AddFriend.inc"
// ORIG 0x081cb274 T _ZN20DisPatcher_AddFriend11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_AddFriend/check_error.md 逐块映射）：state<=2 → 0x7fffffff；
// (msg+0x11) 指针空或与自身角色名 strcmp 相同 → 1；否则 0。
// 注：ORIG 汇编对 (msg+0x11) 地址值做 test/jne（非解引用检查）。
#include "DisPatcherCore/check_error_30.inc"
{
    // ORIG 0x081... 入口 `mov 0x10(%ebp),%eax; mov %eax,-0xc(%ebp)`：view 指针局部在入口声明，
    // 后续 `mov -0xc(%ebp),%eax/%edx; add $0x11` 均从该槽重定位。
    STFriendReqView* req = reinterpret_cast<STFriendReqView*>(&msg);
    if (user->get_state() <= 2)
    {
        return 0x7fffffff;
    }
    if (req->m_name == 0)                                    // ORIG `add $0x11; test %eax,%eax; jne`（死检查保留）
    {
        return 1;
    }
    if (strcmp(req->m_name, user->getCurCharacName()) == 0)
    {
        return 1;
    }
    return 0;
}
#include "DisPatcherCore/read_59.inc"
{
    STFriendReqView* pMsg =
        reinterpret_cast<STFriendReqView*>(&msg);   // ORIG -0xc(%ebp)
    if (!packet.get_int(pMsg->m_nameLen))           // +0x0d
    {
        return LineFunc(0x207b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(pMsg->m_name, 0x1e, pMsg->m_nameLen))
    {
        return LineFunc(0x2082,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}

#include "DisPatcherCore/send_25.inc"
{
    stAddFriendParam& p = *reinterpret_cast<stAddFriendParam*>(&param);
    // ORIG 0x081cb4e6：call 后显式 jmp + je 目标含 nop 空块 → 首条件为
    // void early-return（探针 c6root gcc4.4.7 sD 形态实证）。
    if (p.m_field4 == 0x7fffffff)
        return;
    if (p.m_field4 != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x3f,
                                 (unsigned char)(p.m_field4 & 0xff));
}
// ORIG 0x081cb2e2 T _ZN20DisPatcher_AddFriend7processEP5CUserR8MSG_BASER9ParamBase：
// 薄包装，尾调 process_packet_jpn 并透传返回值（汇编 leave/ret 直传 eax）。
#include "DisPatcherCore/process_60.inc"
{
    // P31 帧差：ORIG sub $0x58 → dead 局部 0x40（薄包装仍保留深帧）。
    volatile char dead_40[0x40]; (void)dead_40;
    return process_packet_jpn(user, msg, param);
}

// ORIG 0x081cb30a T _ZN20DisPatcher_AddFriend18process_packet_jpnEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_AddFriend/process_packet_jpn.md 全文转写）。
// m_field4=check_error；>0 返 0；<0 logFormat(1,file,pretty,0x20d8,
// "Dispatcher_Impl Check_Error No(%d)",err) 返 0x20d9；==0：StreamPool 取流构造
// CStreamGuard → <<0x33e <<GetUID → GetInBuffer<SIG_CHECK_ADD_FRIEND> 判空置 m_field4=1；
// [b28d-29] N82/P211：store 装载序 + 临时对象块位置（P183 反向），调用点全等  (ORIG 0x081cb30a insn 126->125)
// 否则 sig[+4]=!isGM(acc_id)、strncpy(sig+5, msg+0x11, 0x1e)、MsgQueueMgr put(DB_Q)；返 0。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 126 / ours 126 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_packet_jpn.inc"
{
    stAddFriendParam& out = static_cast<stAddFriendParam&>(param);
    // [b23S3C] ORIG 槽集 {-0x10 req, -0x14 out, -0x1c guard, -0xc sig}；
    // kFile/kPretty 各用点均字符串立即数（无 static 局部指针槽）；
    // Acquire 结果直落 ctor 0x4(%esp)（无 stream 局部，P0 寄存器直传）；
    // 写流为 operator* + operator<< 两 call 链（+0x0c7/+0x0ef 各一次），
    // GetInBuffer 走 operator-> 链（+0x10c/+0x114，P103）——GmCmds_C
    // CDeleteGuild DONE 同族形。

    out.m_field4 = check_error(user, msg);
    if (out.m_field4 > 0)
    {
        return 0;
    }
    if (out.m_field4 < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp",
            "int DisPatcher_AddFriend::process_packet_jpn(CUser*, MSG_BASE&, ParamBase&)",
            0x20d8, "Dispatcher_Impl Check_Error No(%d)", out.m_field4);
        return 0x20d9;
    }

    MSG_ADD_FRIEND& req = *reinterpret_cast<MSG_ADD_FRIEND*>(&msg);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x20ed),
        true);
    (*guard)->operator<<(0x33e);                 // inter-server SIG 消息号
    (*guard)->operator<<(static_cast<int>(user->GetUID()));
    SIG_CHECK_ADD_FRIEND* sig =
        guard.operator->()->GetInBuffer<SIG_CHECK_ADD_FRIEND>();
    if (sig == 0)
    {
        out.m_field4 = 1;
        // [b28d-solo-DisPatcherCore] ORIG +0x12c `movl $0x1,0x4(%eax); mov $0x0,%ebx;
        // jmp <尾块>`（返回值物化到 %ebx 后跳共享尾析构块）⇒ if 块内是独立 return 0。
        return 0;
    }
    else
    {
        // ORIG +0x153 分支岛 mov$0/mov$1（非 setne 归一），直落 sig+0x04。
        sig->m_notGmFlag =
            GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()) ? 0 : 1;
        strncpy(sig->m_characName,                               // sig+0x05..0x22
                reinterpret_cast<STFriendReqView*>(&req)->m_name, // msg+0x11
                0x1e);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);    // QUEUE_IDX 2=DB_Q
    }
    return 0;
}

#include "DisPatcherCore/dtor_DisPatcher_RemoveFriend.inc"
#include "DisPatcherCore/check_error_31.inc"
{
    if (user->get_state() <= 2)
    {
        return 0x7fffffff;
    }
    // ORIG 0x081cb5d8：msg 指针先落 -0xc 再 reload（帧 0x28；m_name 数组地址判 0，恒 false）。
    STFriendReqView* pMsg = reinterpret_cast<STFriendReqView*>(&msg);   // ORIG -0xc(%ebp)
    if ((*(&pMsg))->m_name == 0)   // +0x11
    {
        return 1;
    }
    return 0;
}
#include "DisPatcherCore/read_60.inc"
{
    STFriendReqView* pMsg =
        reinterpret_cast<STFriendReqView*>(&msg);   // ORIG -0xc(%ebp)
    if (!packet.get_int(pMsg->m_nameLen))           // +0x0d
    {
        return LineFunc(0x2115,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(pMsg->m_name, 0x1e, pMsg->m_nameLen))
    {
        return LineFunc(0x211c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "DisPatcherCore/send_26.inc"
{
    stRemoveFriendParam& p = *reinterpret_cast<stRemoveFriendParam*>(&param);
    if (p.m_errNo == 0x7fffffff)
        return;
    if (p.m_errNo != 0)
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x40, (unsigned char)(p.m_errNo & 0xff));
}
// ORIG 0x081cb618 T _ZN23DisPatcher_RemoveFriend7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_RemoveFriend/process.md 全文转写）。
// m_errNo=check_error；>0 返 0；<0 LineFunc(0x2141,pretty,err,acc_id) 返回其值；
// ==0：组 Packet_Monitor_Del_Buddy（ctor 0x0822bcb2 W：PacketHeader(0x674,0x30)+memset(m_name)），
// 填 m_charNo=getCurCharacNo、m_idByChannel=GetUID、memcpy(m_name, msg+0x11, strlen)，
// [b28d-29] N82/P211：参数副本槽 + store 装载序（ORIG RHS-first mov %eax,%edx 中转，4.4 LHS-first）  (ORIG 0x081cb618 insn 85->81)
// 经 s_monitor_proxy_mgr GetServerProxy(GetServerGroup())->SendPacket(&pkt, packetSize)，返 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 44（ORIG 85 / ours 81），残差=N7/N82 帧与 callee-saved 角色/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_61.inc"
{
    stRemoveFriendParam& out = static_cast<stRemoveFriendParam&>(param);

    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x2141, __PRETTY_FUNCTION__, out.m_errNo, user->get_acc_id());
    }

    // ORIG 0x081cb6a4：msg 指针具名局部 -0x10（在本分支内声明，非函数头）+ 每处重算
    // `mov -0x10(%ebp),%eax; add $0x11,%eax`（strlen 与 memcpy 各一次，P183 禁缓存）；
    // 0x081cb6d2 `mov %eax,-0xc(%ebp)` + 0x081cb6d5 回读 = strlen 结果经具名 len 槽
    // （P197② 具名结果槽）。len 槽存在使 pkt/全部局部槽基下移 4（-0x40→-0x44）。
    MSG_BASE* pReq = &msg;                                              // -0x10（先声明，
                                                                        // ORIG +0x07f store
                                                                        // 早于 pkt ctor）
    Packet_Monitor_Del_Buddy pkt;
    pkt.m_charNo = static_cast<unsigned int>(user->getCurCharacNo());   // +0x0a
    pkt.m_idByChannel = user->GetUID();                                 // +0x0e
    // P319 试验（已回滚，实测 ae -1 回退）：整型加法 cast 形不能把这处 lea 折回转；
    // 该处 `mov -0x10,%edx; add $0x11,%edx`（ORIG）vs `lea 0x11(%eax),%edx`（4.4）
    // 属 P30/N4 寻址折叠族，记 N。
    size_t len = strlen(reinterpret_cast<STFriendReqView*>(pReq)->m_name);
    memcpy(pkt.m_name, reinterpret_cast<STFriendReqView*>(pReq)->m_name, len);
    // [b28e-L5] P52 应用例：ORIG 0x081cb6f4 先 `movzwl -0x42(%ebp),%eax; movzwl %ax,%esi`
    // 与 `lea -0x44(%ebp),%ebx` 把 (size, pktAddr) 物化入 callee-saved，再跨
    // GetServerGroup→GetServerProxy 两次 call 保活，最后三压参。单条链式表达式复现
    // （拆具名 proxy 局部会使 4.4 两语句坍缩、丢 push %esi/%ebx 与物化段）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);    // +0x02 长度字段(0x30)
    return 0;
}

#include "DisPatcherCore/dtor_DisPatcher_Cera.inc"
#include "DisPatcherCore/dispatch_sig_27.inc"
{
    // ORIG 0x081fd360：s_pIPGHelper 虚表 +0x2c（slot11 IPGQuery——vtable
    // 0x08b420a8 逐槽实证：+0x28=slot10 IPGOutputConfirm(SIPGData*,SIPGData*)
    // 0x08100e0a，+0x2c=slot11 IPGQuery 0x08100790）；随后 LoadMileage(user)。
    GlobalData::s_pIPGHelper->IPGQuery(user);
    user->LoadMileage();
    return 0;
}
#include "DisPatcherCore/read_61.inc"
{
    return 0;    // ORIG 0x082334a4：恒定返回 0
}
#include "DisPatcherCore/process_62.inc"

#include "DisPatcherCore/dtor_DisPatcher_EnterWarRoom.inc"
#include "DisPatcherCore/dispatch_sig_28.inc"
{
    // ORIG 0x0820229e：纯 LineFunc 占位（报告 DisPatcher_EnterWarRoom/dispatch_sig.md）。
    // P31 帧差：ORIG sub $0x48 vs 最小帧 0x18 → dead 局部 0x30（volatile 保槽零指令）。
    volatile char dead_30[0x30]; (void)dead_30;
    return LineFunc(0x97fb, __PRETTY_FUNCTION__, 0, 0);
}
#include "DisPatcherCore/read_62.inc"
{
    return 0;    // ORIG 0x0823360c：恒定返回 0
}
#include "DisPatcherCore/process_63.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetWarRoomSeatState.inc"
#include "DisPatcherCore/dispatch_sig_29.inc"
{
    // ORIG 0x082022ca：纯 LineFunc 占位（报告 DisPatcher_SetWarRoomSeatState/dispatch_sig.md）。
    // P31 帧差：ORIG sub $0x38 → dead 局部 0x20。
    volatile char dead_20[0x20]; (void)dead_20;
    return LineFunc(0x9848, __PRETTY_FUNCTION__, 0, 0);
}
#include "DisPatcherCore/read_63.inc"
{
    return 0;    // ORIG 0x08233620：恒定返回 0
}
#include "DisPatcherCore/process_64.inc"

#include "DisPatcherCore/dtor_DisPatcher_DieWarRoomCharacter.inc"
#include "DisPatcherCore/dispatch_sig_30.inc"
{
    // ORIG 0x082022f6：纯 LineFunc 占位（报告 DisPatcher_DieWarRoomCharacter/dispatch_sig.md）。
    // P31 帧差：ORIG sub $0x38 → dead 局部 0x20。
    volatile char dead_20[0x20]; (void)dead_20;
    return LineFunc(0x9867, __PRETTY_FUNCTION__, 0, 0);
}
#include "DisPatcherCore/read_64.inc"
{
    return 0;    // ORIG 0x08233634：恒定返回 0
}
#include "DisPatcherCore/process_65.inc"

#include "DisPatcherCore/dtor_DisPatcher_StartWarRoomTemp.inc"
#include "DisPatcherCore/dispatch_sig_31.inc"
{
    // ORIG 0x08201b06：纯 LineFunc 占位（报告 DisPatcher_StartWarRoomTemp/dispatch_sig.md）。
    // P31 帧差：ORIG sub $0x28 → dead 局部 0x10。
    volatile char dead_10[0x10]; (void)dead_10;
    return LineFunc(0x9737, __PRETTY_FUNCTION__, 0, 0);
}
#include "DisPatcherCore/read_65.inc"
{
    return 0;    // ORIG 0x082335e4：恒定返回 0
}
#include "DisPatcherCore/process_66.inc"

#include "DisPatcherCore/dtor_DisPatcher_ReportClient4Hack.inc"
// ORIG 0x082041a0 T _ZN28DisPatcher_ReportClient4Hack12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_ReportClient4Hack/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=1 或 getCurCharacR()==0 → return 0。state==6 → 仅 analyzeHackReport。
// state==5 组队校验（GetParty==0 → LineFunc 0x9bb7）；state==7 PVP 校验
// （GetPvp==0 → LineFunc 0x9bc1）；state==8 战场校验（GetWarRoom==0 → LineFunc 0x9bc5）；
// 校验通过同 6 → analyzeHackReport；其余 state → beginCollect/analyze/report/reset 全链。
#include "DisPatcherCore/dispatch_sig_32.inc"
{
    if (user->get_state() <= 1 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    // ORIG +0x40：重新 call get_state 取 %eax 后 4.1 switch 二分下探
    // （`cmp $6;je; cmp $6;jg; cmp $5;je; jmp d; cmp $7;je; cmp $8;je; jmp d`，
    // 技34 直写 switch 复现）；case5/7/8 校验失败走 LineFunc 岛，成功 break 共
    // 尾 analyze（+0x1c7 汇合）；default 走全链后直接 return 0。
    switch (user->get_state())
    {
    case 5:
        if (user->GetParty() == 0)
        {
            return LineFunc(0x9bb7, __PRETTY_FUNCTION__, 0, 0);
        }
        break;
    case 7:
        if (G_CGameManager()->GetPvp((int)user->GetPvpIndex(), user, 0) == 0)
        {
            return LineFunc(0x9bc1, __PRETTY_FUNCTION__, 0, 0);
        }
        break;
    case 8:
        if (user->GetWarRoom() == 0)
        {
            return LineFunc(0x9bc5, __PRETTY_FUNCTION__, 0, 0);
        }
        break;
    case 6:
        break;
    default:
        user->getHackAnalyzer()->beginCollectHackInfo(user);
        user->getHackAnalyzer()->analyzeHackReport(user, &packet);
        user->getHackAnalyzer()->reportHackInfo();
        user->getHackAnalyzer()->resetHackInfo();
        return 0;
    }
    user->getHackAnalyzer()->analyzeHackReport(user, &packet);
    return 0;
}
#include "DisPatcherCore/read_66.inc"
{
    return 0;    // ORIG 0x08233738：恒定返回 0
}
#include "DisPatcherCore/process_67.inc"

// ---------------------------------------------------------------------------
// ST_Guild_War_Rank_Info / Packet_Notice_Guild_War_Info 生命周期（StubS3C）。
// ST ctor：ORIG W 0x0822ba9a —— uint(+0)=0、uint(+4)=0、ushort(+8)=0、
// memset(+0xa,0,0x17)。Packet_Notice ctor：ORIG W 0x0822badc ——
// PacketHeader(0x414,0x15c) 后对 +0x12 起 9 个元素（stride 0x21）逐个构造。
// ---------------------------------------------------------------------------
#include "DisPatcherCore/ST_Guild_War_Rank_Info.inc"
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    memset(m_pad0a, 0, sizeof(m_pad0a));
}
// [b28d-solo-DisPatcherCore] 修复：m_rankInfo 由 char[0x14a] 改真实元素数组
// ST_Guild_War_Rank_Info[10]（头 R2 广播），ctor 删手写 placement-new 循环
// ⇒ 编译器隐式数组成员构造复现 ORIG P151 倒计数循环（esi=9→-1、ebx 步进 0x21，
// 无 _ZnwjPv/size 装载）；N83 的 placement-new 多 2 条消除。
#include "DisPatcherCore/Packet_Notice_Guild_War_Info.inc"
    : PacketHeader(0x414, 0x15c)
{
    // ORIG +0x2f 循环由编译器隐式发射（数组成员构造）；函数体为空。
}

#include "DisPatcherCore/dtor_DisPatcher_RequestGuildWarInfo.inc"
#include "DisPatcherCore/dispatch_sig_33.inc"
{
    // ORIG 0x0820439a（报告全文转写）：短路 `||` 反判（state<2 或 cur==0 早退）——
    // `cur==0` 检查为分支双 immediates bool 物化形（`jne→mov $0;fall mov $1;jmp`+
    // `test %al; je body`），&& 正向形在 4.4 恒产单 je 直跳不等形。
    if (user->get_state() < 2 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    Packet_Notice_Guild_War_Info pkt;
    pkt.m_uid = user->GetUID();
    pkt.m_characNo = user->get_charac_no(-1);
    // ORIG `mov %eax,-0x156; lea -0x164(%ebp),%ebx`：&pkt 在 proxy 链**之前**物化
    // 直落 callee-saved %ebx（技2+技13 声明位置=落点时机）；proxy 返回 %eax 短程直落。
    // 链式调用不留 proxy 槽（同 PingStat 定律：ORIG 返回 %eax 直落 this 位）。
    register char* pPkt = reinterpret_cast<char*>(&pkt);
    GlobalData::s_guild_proxy_mgr
        ->GetServerProxy(user->GetServerGroup())->SendPacket(pPkt, sizeof(pkt));
    return 0;
}
#include "DisPatcherCore/read_67.inc"
{
    return 0;    // ORIG 0x0823374c：恒定返回 0
}
#include "DisPatcherCore/process_68.inc"

#include "DisPatcherCore/dtor_DisPatcher_PvpHeartBeat.inc"
#include "DisPatcherCore/read_68.inc"
{
    return 0;    // ORIG 0x08233760：恒定返回 0
}
#include "DisPatcherCore/dispatch_sig_34.inc"
{
    // ORIG 0x08204458（报告全文转写）：短路 || 真值经分支直落 %eax（cmp $1;
    // jle→true 块；call R; test; jne→false 块），随后 test %al + fall 进
    // `return 0`（mov $0; jmp end）、je 达 refresh 体（体尾独立 mov $0）。
    if (user->get_state() <= 1 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    disconnect_detecter::GetInstanceDisconnectDetecter()->RefreshRecvTime(user);
    return 0;
}
#include "DisPatcherCore/process_69.inc"

#include "DisPatcherCore/dtor_DisPatcher_MultiBoxLottery.inc"
// ORIG 0x0820b802 T _ZN26DisPatcher_MultiBoxLottery12dispatch_sigEP5CUserR9PacketBuf
// 报告：docs/class_func_reports/DisPatcher_MultiBoxLottery/dispatch_sig.md（汇编+反编译 C 全读）
#include "DisPatcherCore/dispatch_sig_35.inc"
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
    STMultiBoxLotteryReq reqView;   // ORIG -0x21 局部消息视图（-0x14/-0x12 两 ushort）
    if (!packet.get_short(reqView.m_field0d))
    {
        return LineFunc(0xa838,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(reqView.m_field0f))
    {
        return LineFunc(0xa839,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    int result = user->multibox_lottery_item(
        (int)reqView.m_field0d,
        (int)reqView.m_field0f);
    if (result > 0)
    {
        // 技12：guard 置于显式 `{}` 块 → 早退 return 0 不携 guard dtor，
        // 各 `return 0` 恢复 ORIG `mov $0x0,%eax; jmp EPI` 形（4.4 无 %ebx phi）；
        // `LineFunc` 返回直落 %eax 无 `mov %eax,%ebx`（ORIG 0x0820b802）。
        PacketGuard guard;
        guard.put_header(1, 0xd3);
        guard.put_byte(0);
        guard.put_byte(result);
        guard.finalize(true);
        user->Send(guard);
    }
    return 0;
}
#include "DisPatcherCore/read_69.inc"
{
    return 0;    // ORIG 0x08233bd2：恒定返回 0
}
#include "DisPatcherCore/process_70.inc"

// [b28d-29] P251/P208：ORIG 0x0821ac7c 为 switch(local_e) 稀疏树（ushort 入口截断槽 -0xa 双段）。
// [Lane38 2026-09-10] 已按 switch 复现（case 0/1/2 物理序 = ORIG，fall-through = case 0）：
//   jcc 目标差由 −106/+134/+133 收敛为单常数 −3（结构对齐），228→233（残 N82 帧 0x58 vs 0x44）。
#include "DisPatcherCore/dtor_DisPatcher_UdpCharacteristic.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 345（ORIG 228 / ours 233），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_36.inc"
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
    // [Lane38 2026-09-10] 判据①/⑧ 块布局：ORIG 0x0821ac7c 分派为 **switch 比较链**
    // `movzwl -0xa(%ebp),%eax; movzwl %ax,%eax; cmp $1,%eax; je +0x193;
    //  cmp $2,%eax; je +0x2d3; test %eax,%eax; jne +0x3aa`（fall-through = case 0，
    // default 在 case2 之后）；if-else 链的 4.4 O0 形是就近 `jne` + 各块 fall-through
    // （我方旧形态 jcc 目标差 −106/+134/+133）。双 `movzwl` 提升亦为 switch 表达式
    // int 提升直证（if 比较为 `cmp $1,%ax` 16 位）。语义等价改写。
    switch (local_e)
    {
    case 0:
    {
        Packet_Party_Result_Statistic local_1d;
        // [G5-SigA] ORIG 0x0821aca1 `lea -0x19(%ebp),%eax; add $0xa,%eax` + 0x0821ace6
        // `...; add $0xc,%eax`：两个 get_short 直接写结构成员本身（+0xa/+0xc），
        // 随后 0x0821ad2b `movzwl -0xd(%ebp),%edx; movzwl -0xf(%ebp),%eax;
        // lea (%edx,%eax,1),%eax; mov %ax,-0xd(%ebp)` 才做 total += success。
        // 原写法用两个具名 2B 临时量中转（P0：ORIG 无该槽），改为直传成员地址。
        if (packet.get_short(*(unsigned short*)&local_1d.m_successPartyTry) != true)
        {
            return LineFunc(0xce1d,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(*(unsigned short*)&local_1d.m_totalPartyTry) != true)
        {
            return LineFunc(0xce1e,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        local_1d.m_totalPartyTry = static_cast<unsigned short>(
            local_1d.m_totalPartyTry + local_1d.m_successPartyTry);
        ENUM_SERVER_GROUP uVar2 = user->GetServerGroup();
        CStatisticServerProxy* pCVar3 = GlobalData::s_statistic_proxy_mgr->GetServerProxy(uVar2);
        pCVar3->SendPacket(reinterpret_cast<char*>(&local_1d), 0xe);
        break;
    }
    case 1:
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
        break;
    }
    case 2:
    {
        Packet_Abnormal_Exit_Statistic local_30;
        // [G5-SigA] 同 case0：ORIG 0x0821aebe `lea -0x2c(%ebp),%eax; add $0xa,%eax`
        // 与 0x0821af03 `...; add $0xc,%eax` 直写成员，加法在 0x0821af48 后进行。
        if (packet.get_short(*(unsigned short*)&local_30.m_successDungeonClear) != true)
        {
            return LineFunc(0xce68,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (packet.get_short(*(unsigned short*)&local_30.m_totalDungeonClear) != true)
        {
            return LineFunc(0xce69,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        local_30.m_totalDungeonClear = static_cast<unsigned short>(
            local_30.m_totalDungeonClear + local_30.m_successDungeonClear);
        ENUM_SERVER_GROUP uVar2 = user->GetServerGroup();
        CStatisticServerProxy* pCVar3 = GlobalData::s_statistic_proxy_mgr->GetServerProxy(uVar2);
        pCVar3->SendPacket(reinterpret_cast<char*>(&local_30), 0xe);
        break;
    }
    default:
        // ORIG 0x0821af8a（+0x3aa，case2 块之后）：非 0/1/2 值 → LineFunc(0xce76)。
        return LineFunc(0xce76,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
#include "DisPatcherCore/read_70.inc"
{
    return 0;    // ORIG 0x08233d62：恒定返回 0
}
#include "DisPatcherCore/process_71.inc"

#include "DisPatcherCore/dtor_DIspatcher_OneDayLetheTicket.inc"
#include "DisPatcherCore/dispatch_sig_37.inc"
{
    // ORIG 0x0821b106（报告 DIspatcher_OneDayLetheTicket/dispatch_sig.md 全文转写）。
    cMyTrace trace(__PRETTY_FUNCTION__, 0xceaa, 0);
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
    // ORIG（report Ghidra C）：blocked 判定为**早返**（AL 直测无槽，P198），
    // 随后 GetPremiumLetheManager 空检、ResetSkillReq 各为独立早返——禁布尔局部
    // 合并（合并致 blocked spill 到 -0xd 槽 + 条件合一，与 ORIG 三段独立早返不符）。
    if (G_GameWorld()->IsPVPChannel() && !G_GameWorld()->IsPvPSkilTreeChannel())
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xd5), 1);
        return 0;
    }
    CPremiumLetheManager* lethe = G_CGameManager()->GetPremiumLetheManager();
    if (lethe == 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xd5), 1);
        return 0;
    }
    if (!lethe->ResetSkillReq(user))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xd5), 1);
    }
    return 0;
}
#include "DisPatcherCore/read_71.inc"
{
    return 0;    // ORIG 0x08233d76：恒定返回 0
}
#include "DisPatcherCore/process_72.inc"

#include "DisPatcherCore/dtor_Dispatcer_polic_one_to_one_chat_disable.inc"
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 65 / ours 65 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/dispatch_sig_38.inc"
{
    // ORIG 0x082272ce（报告 Dispatcer_polic_one_to_one_chat_disable/dispatch_sig.md）：
    // state>2 且当前角色存在 → 读包组 Packet_Disable_User_OneToOneChat_Police；
    // GM 发起时回填 characNo 并经 Monitor 代理 SendTcpPacket(0x30)。
    // ORIG +0x12..+0x34：4.1 将 !(state>=3 && ptr) 物化为 %al（双常量岛 jle→mov$1/
    // jne→mov$0 + test;je），跳过路径 `mov $0,%eax; jmp end`。等价 skip-flag 写法。
    // [b28d-solo-DisPatcherCore] P0：ORIG `test %al,%al` 无槽 ⇒ 原码是内联 || 表达式；
    // 具名 bool skip 使 4.4 落 -0x9 槽（movb+cmpb）并把 pkt 对象槽整体顶浅 1B
    //（-0x39 vs ORIG -0x38）与帧 +0x10。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    {
        Packet_Disable_User_OneToOneChat_Police pkt;
        // ORIG +0x44/+0x5b：无 nameLen 临时槽——`lea -0x38,%eax; add $0xe` 直传
        // get_int(pkt.m_nameLength)，get_str 第 3 参 `mov -0x2a(%ebp),%eax`（= pkt+0xe）。
        packet.get_int(*reinterpret_cast<unsigned int*>(
            reinterpret_cast<char*>(&pkt) +
            offsetof(Packet_Disable_User_OneToOneChat_Police, m_nameLength)));
        packet.get_str(pkt.m_name, static_cast<int>(sizeof(pkt.m_name)),
                       static_cast<int>(pkt.m_nameLength));
        if (user->isGMUser())
        {
            pkt.m_gmId = user->getCurCharacNo();
            // ORIG：GetServerProxy 返回值寄存器直传 SendTcpPacket（无 proxy 具名槽）。
            // P52/P200：&pkt 作实参直传，跨 GetServerGroup/GetServerProxy 存活 → 4.4 落 %ebx。
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
                ->SendTcpPacket(reinterpret_cast<char*>(&pkt), sizeof(pkt));
        }
    }
    return 0;
}
#include "DisPatcherCore/read_72.inc"
{
    return 0;    // ORIG 0x0823429e：恒定返回 0
}
#include "DisPatcherCore/process_73.inc"

#include "DisPatcherCore/dtor_DisPatcher_CheckSecurityProtection.inc"
#include "DisPatcherCore/check_error_32.inc"
{
    return 0;    // ORIG 0x08264912：恒定返回 0
}
#include "DisPatcherCore/read_73.inc"
{
    return 0;    // ORIG 0x08264810：恒定返回 0
}
#include "DisPatcherCore/send_27.inc"
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
#include "DisPatcherCore/process_74.inc"
{
    CheckSecurityProtection& out = static_cast<CheckSecurityProtection&>(param);
    out.m_4 = static_cast<unsigned char>(
        GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x38)));
    return 0;
}

#include "DisPatcherCore/dtor_DisPatcher_EventDungeon_DestoryObject.inc"
#include "DisPatcherCore/check_error_33.inc"
{
    return 0;    // ORIG 0x0812bd82：恒定返回 0
}
#include "DisPatcherCore/read_74.inc"
{
    // ORIG 0x0812bd54：pBuf 局部落栈（-0xc）后 get_binary(pBuf, 0x18)，恒返 0。
    // *(&pBuf) 实参使 reload 紧跟 spill（ORIG +0x0c），复现 4.4.7 求值序。
    char* pBuf = reinterpret_cast<char*>(&msg);   // ORIG -0xc(%ebp)
    packet.get_binary(*(&pBuf), 0x18);
    return 0;
}
#include "DisPatcherCore/send_28.inc"
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
#include "DisPatcherCore/process_75.inc"
{
    // ORIG 0x0812bd8c：组件视图单指令 disp 访存（P1 范式），
    // 写入序 +0x4←+0xe（m_point ← m_roomIndex）。
    CMDPacketStruct::_STReqEventDungeonDestoryObject* rMsg =
        reinterpret_cast<CMDPacketStruct::_STReqEventDungeonDestoryObject*>(&msg);   // ORIG -0x8(%ebp)
    CMDPacketStruct::_STResEventDungeonDestoryObject* rParam =
        reinterpret_cast<CMDPacketStruct::_STResEventDungeonDestoryObject*>(&param);  // ORIG -0x4(%ebp)
    rParam->m_point = rMsg->m_roomIndex;
    check_error(user, *rMsg);   // ORIG：经局部槽 -0x8 reload 传参（非直读 0x10(%ebp)）
    return 0;
}

// ORIG 0x0812c1c6 W _ZN33DisPatcher_EventDungeon_ClearRoomD1Ev/D2Ev/D0Ev（三地址相同）
// objdump 实测 D1：写 vptr=0x8b66068 → 调 IPacketDispatcher<...>::D1 → 恒假 delete 守卫
// —— 即编译器生成的空析构，{} 与其逐指令等价。
#include "DisPatcherCore/dtor_DisPatcher_EventDungeon_ClearRoom.inc"

// ORIG 0x085957e8 W _ZN33DisPatcher_EventDungeon_ClearRoomC1Ev（无报告，objdump 转写）：
//   调 IPacketDispatcher<CMDPacketStruct::_STReqEventDungeonClearRoom,
//                        CMDPacketStruct::_STResEventDungeonClearRoom, ch_state3>::C1
//   后写 vptr=0x8b66068。基类实参已按 ORIG mangled 统一（OrigPktDisp 批），
//   空体 ctor 由编译器生成基类构造+vptr 写入，与 ORIG 逐指令等价。

// ORIG 0x0812beb0 T _ZN33DisPatcher_EventDungeon_ClearRoom11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/check_error.md 全文转写：
//   push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret）
#include "DisPatcherCore/check_error_34.inc"
{
    return 0;
}

// ORIG 0x0812beba T _ZN33DisPatcher_EventDungeon_ClearRoom7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/process.md 全文转写，
//   含汇编写入次序：param+0x4←msg+0xe、param+0x8←msg+0x12、param+0x6←msg+0x10）；check_error 恒返 0。
#include "DisPatcherCore/process_76.inc"
{
    // ORIG 0x0812beba：组件视图单指令 disp 访存（P1 范式），
    // 写入序 +0x4←+0xe、+0x8←+0x12、+0x6←+0x10（roomIndex/monsterCount/monsterIndex）。
    CMDPacketStruct::_STReqEventDungeonClearRoom* rMsg =
        reinterpret_cast<CMDPacketStruct::_STReqEventDungeonClearRoom*>(&msg);    // ORIG -0x8(%ebp)
    CMDPacketStruct::_STResEventDungeonClearRoom* rParam =
        reinterpret_cast<CMDPacketStruct::_STResEventDungeonClearRoom*>(&param);  // ORIG -0x4(%ebp)
    rParam->m_roomIndex = rMsg->m_roomIndex;
    rParam->m_monsterCount = rMsg->m_monsterCount;
    rParam->m_monsterIndex = rMsg->m_monsterIndex;
    check_error(user, *rMsg);   // ORIG：-0x8 槽 reload（非直读 0x10(%ebp)）
    return 0;
}

// ORIG 0x0812be82 T _ZN33DisPatcher_EventDungeon_ClearRoom4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/read.md 全文转写）：
// ORIG 汇编含 &msg 的局部落栈（-0x4c(%ebp) spill/reload 对），以具名局部复现。
#include "DisPatcherCore/read_75.inc"
{
    char* pBuf = reinterpret_cast<char*>(&msg);   // ORIG -0xc(%ebp)
    packet.get_binary(*(&pBuf), 0x14);            // 同上：reload 先于 len 入栈
    return 0;
}

// ORIG 0x0812bf16 T _ZN33DisPatcher_EventDungeon_ClearRoom4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/send.md 全文转写）：
// put_header(1,0x21c) → put_byte(1) → put_binary(pParam,0xc) → finalize(true) →
// CUser::Send；PacketGuard 栈对象析构（ORIG 含 EH 清理路径）。
// 反编译 C 首行 local_10=param_2：param 引用先落栈（-0xc），guard 位于 -0x18。
#include "DisPatcherCore/send_29.inc"
{
    ParamBase* pParam = &param;   // ORIG -0xc(%ebp)（local_10，先于 guard ctor 落栈）
    PacketGuard guard;
    guard.put_header(1, 0x21c);
    guard.put_byte(1);
    guard.put_binary(reinterpret_cast<char*>(pParam), 0xc);
    guard.finalize(true);
    user->Send(guard);
}

#include "DisPatcherCore/dtor_Dispather_RegenerationRandomOption.inc"
#include "DisPatcherCore/check_state_7.inc"
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
#include "DisPatcherCore/read_76.inc"
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
#include "DisPatcherCore/send_30.inc"
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
// [b28d-29] N78：ORIG 手写 0x400 字节对齐清零展开（and/test/字节+字+双字三段），4.4 恒 call memset；语义等价  (ORIG 0x0860fdd2 insn 106->74)
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 74（ORIG 106 / ours 74），残差=N7/N82 帧与 callee-saved 角色/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [b28d-TakeDispatchCore] **P312 翻案（N78 突破口，原「4.4 恒 call memset 不可翻」作废）**：
// `char key[0x400] = {0};`（原 `memset(key,0,sizeof(key))`）复现 ORIG 0x0860fddd 的
// 对齐剥离+rep stos 慢路 ⇒ ours 77→111、ndiff 58→19、gap 31→−2；
// p167 11/11 CLEAR（余 StatisticsPacket C1↔C2 同址别名对=豁免）。
#include "DisPatcherCore/add.inc"
         unsigned int value, const char* fmt, ...)
{
    char key[0x400] = {0};          // P312：ORIG 0x0860fddd 内联对齐剥离+rep stos 慢路
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
    // [Lane5] P36/P52：ORIG 无 proxy 具名局部（`call GetServerGroup; mov mgr,%edx;
    // call GetServerProxy; mov %eax,(%esp); call SendPacket`），且 `lea packet,%ebx`
    // 提前到 GetServerGroup 之前 ⇒ 链式单语句（&packet 跨调用由 ebx 承载）。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x89);
}
}  // namespace statistc_proxy



#include "DisPatcherCore/dtor_DispatcherResetRandomOption.inc"
#include "DisPatcherCore/read_77.inc"
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
// [b28d-29] N82/P183：ORIG 无逐字段临时缓存，4.4 新增 -0x50..-0x48 三槽（帧 0x9c vs 0xac）；语义集合全等  (ORIG 0x08682d3a insn 102->119)
//   item.m_random.m_random 的 14 字节（+0x25..+0x32）逐字节, reason)。无 m_user 空检查。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 124（ORIG 102 / ours 118），残差=N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RandomOption.inc"
{
    // [FL4] P183/P157：ORIG 0x0815f6xx 每处 `mov 0xc(%ebp),%eax; movzbl 0x32(%eax),%eax`
    // —— 14 个字节全部为「item 基址 + 常量 disp」具名成员直读（0x25..0x32 =
    // Inven_Item::m_random(+0x1f)::m_random(+0x06) 的 0x0e 字节），**无** opt 指针缓存槽
    // （旧形 `const unsigned char* opt=...` 占 -0x1c 槽 + 每处 add $0x25 ⇒ +16 insn）。
    // 成员展开 = m_fields[0..2]（3×3B）+ m_seed9 + m_fielda（3B）+ m_seedd = 1+1+1+1+1+1+1+1+1+1+1+1+1+1 字节。
    // 实参右→左求值 ⇒ ORIG 读序 0x32→0x25（m_seedd → m_fields[0].m_field0），故书写序即源序。
    m_user->LogHistory(
        "RandomOptionItem,%d,%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d",
        item.m_addInfo,
        item.m_random.m_random.m_fields[0].m_field0,
        item.m_random.m_random.m_fields[0].m_field1,
        item.m_random.m_random.m_fields[0].m_field2,
        item.m_random.m_random.m_fields[1].m_field0,
        item.m_random.m_random.m_fields[1].m_field1,
        item.m_random.m_random.m_fields[1].m_field2,
        item.m_random.m_random.m_fields[2].m_field0,
        item.m_random.m_random.m_fields[2].m_field1,
        item.m_random.m_random.m_fields[2].m_field2,
        item.m_random.m_random.m_seed9.m_field,
        item.m_random.m_random.m_fielda.m_field0,
        item.m_random.m_random.m_fielda.m_field1,
        item.m_random.m_random.m_fielda.m_field2,
        item.m_random.m_random.m_seedd.m_field,
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
// [b28d-29] N82：帧 0x19c vs 0x1ac + 槽平移与参数副本顺序差，调用点全等  (ORIG 0x081d6af4 insn 479->466)
// 错误码写 param.m_errNo；插入槽位写 param.m_insertedSlot。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 238（ORIG 479 / ours 465），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_77.inc"
                                                ParamBase& param)
{
    stRegenROIResult& out = static_cast<stRegenROIResult&>(param);
    MSG_REGENERATION_RANDOM_OPTION& req =
        *reinterpret_cast<MSG_REGENERATION_RANDOM_OPTION*>(&msg);

    out.m_errNo = 0;
    Inven_Item items[2];
    // [G5-ProcB] ORIG 0x081d6cec `mov %eax,-0x44(%ebp,%ebx,4)` 直接落数组槽，
    // 其前无 `movl $0x0` 清零 ⇒ 原码 `CItem* baseItems[2];` 无 `= {0,0}`
    // （旧文多 2 条 movl）。
    CItem* baseItems[2];
    // [G5-ProcB] ORIG 0x081d6b57 `mov -0x3c(%ebp),%eax; movzwl 0xd(%eax,%edx,2),%eax;
    // movswl %ax,%esi` ⇒ 两槽位不拷入本地数组，直接以 msg 为基址索引读
    // （0x0d + i*2）。旧文 `const unsigned short slotPos[2] = {...}` 多 1 条
    // movl 清零 + 2 条读存（数组落在 -0x48，索引变 `movzwl -0x48(%ebp,%eax,2)`）。
    #pragma pack(push, 1)
    struct RegenROView {
        char m_head[0xd];
        unsigned short m_slots[2];   // +0x0d/+0x0f
    };
    #pragma pack(pop)
    bool allSealable = true;          // ORIG local_36（任一槽 m_field0!=1 即 false）
    unsigned char reSealCount = 0;

    for (int i = 0; i < 2; ++i)
    {
        // [G5-ProcB] ORIG 0x081d6b62 `call getCurCharacInvenR` → 结果留在 %eax 跨
        // imul/lea 直作 0x081d6b8e `mov %eax,0x4(%esp)`（GetInvenSlot this 实参），
        // 无 invenR 具名槽。
        items[i] = user->getCurCharacInvenR()->GetInvenSlot(
            (INVEN_TYPE)1,
            (int)(short)reinterpret_cast<const RegenROView*>(&req)->m_slots[i]);
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
        if (user->CheckItemLock(
                1,
                (int)(short)reinterpret_cast<const RegenROView*>(&req)->m_slots[i]))
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
        // [G5-ProcB] ORIG 0x081d6e49 `mov %eax,-0x14c(%ebp)` / 0x081d6e4f
        // `fildl -0x14c(%ebp)`：ORIG 是 **signed int**→float（fildl）；旧文
        // `(unsigned int)` 使 4.4 走无符号修正路径（零扩展 64 位 + `fildll`，多
        // `mov $0x0,%edx; mov %edx,SLOT` 两条）。
        0.5f + (float)((int)items[0].GetReSealCount() +
                       (int)items[1].GetReSealCount()) / 2.0f);

    // [BigDispatchCore] **语义完整优先，+41 行形态代价，Main 2026-09-09 裁定** ——
    // 本件 ndiff 216 → 257 是**已裁定的正确结果**，后来泳道请勿以“数字变差”为由回滚。
    // 依据：ORIG local_4c 是原码真有的一个具名 std::string 局部（只构造、全函数不读取），
    // callee 证据 _ZNSsC1Ev@0x081d6e9d、_ZNSsD1Ev@0x081d724e/0x081d7265、
    // _Unwind_Resume@0x081d725a（landing pad 0x081d7244）；不补则 P167 恒有 4 项 MISSING。
    // 4.4 会把 dtor 集中到函数尾并改写 landing pad ⇒ 形态代价计入 **N1042**。
    std::string unusedSealName;
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
                (INVEN_TYPE)1,
                (int)(short)reinterpret_cast<const RegenROView*>(&req)->m_slots[i], 1,
                (eItemDelReason)0x2e, true) != 1)
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
// [b28d-29] setcc-fold + P208：ORIG sete 物化与 ushort 截断槽，4.4 直折 jne/jbe；语义集合全等  (ORIG 0x081dd2be insn 323->317)
// SendUpdateItem×2 + SendCmdOkPacket(0x1c8)。全程错误只发包不外抛，恒返 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 47（ORIG 323 / ours 316），残差=P211/N18a 中转/store 序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_78.inc"
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
    // [b28d] P196/P171：ORIG 0x081dd4e0 `movzbl 0x12(%eax); movzbl %al,%eax;
    // mov %eax,-0x20; cmpl $0,-0x20; js <err>; cmpl $2,-0x20; jle <ok>` ⇒
    // 原码有具名 int 局部 + 两个独立有符号比较（`<0` 为原码遗留死条件，照抄）。
    const int rarityChoice = static_cast<unsigned char>(req.m_rarityChoice);
    if (rarityChoice < 0 || rarityChoice > 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x13);
        return 0;
    }
    // 再铸卷轴校验：GetInvenSlot(invenR,1,scrollSlot)，idx 三选一且数量 > 0
    Inven_Item scroll = user->getCurCharacInvenR()->GetInvenSlot(
        (INVEN_TYPE)1, (int)req.m_scrollSlot);
    // ORIG 0x81dd557/0x81dd598：数量校验与 id 三选一校验各为独立 if+SendCmdErrorPacket
    //（两次 0x15 错误块，不合并）。
    if (scroll.m_addInfo2 <= 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x15);
        return 0;
    }
    // [b28d] P48/P0：ORIG 0x081dd5a8/0x081dd5bd/0x081dd5d2 每次独立重读
    // `mov -0x6b(%ebp),%eax; cmp $K,%eax`（无 scrollIdx 缓存局部）。
    if (!((scroll.m_addInfo == 0xf) || (scroll.m_addInfo == 0x381) ||
          (scroll.m_addInfo == (int)0x292090f2)))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1c8, 0x15);
        return 0;
    }
    // ORIG 函数级 static handle（__cxa_guard 保护），ctor 由 GlobalData.cpp 提供；
    // reset_option 定义链接期 blocked（见本 TU random_option 镜像注释）。
    static random_option::CRandomOptionItemHandle handle;
    int resetErr = (int)handle.reset_option(
        (unsigned long)target->m_addInfo, (ENUM_RARITY)shopItem->get_rarity(),
        rarityChoice, &target->m_random.m_random);   // ORIG 0x081dd68c 用 -0x20 槽值
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
// [G5-ProcB] 负结论（本件残留 4 条：ORIG 323 vs 我方 319，即 ORIG 多 4 条）：
// ①② ORIG 0x081dd398 `cmpl $0x0,-0x2c(%ebp)` 后 0x081dd39c `sete %al; test %al,%al; je`，
//    及 0x081dd5de / 0x081dd67e / 0x081dd74b 的 `test %eax,%eax; setne %al; test %al,%al; je`
//    ⇒ ORIG 把 `x==0`/`x!=0` 结果物化为**无槽临时 bool**（setcc+test 两连）。
//    4.4 对 `if (x==0)`/`if (!x)`/`if (x!=0)` 均直接出 cmp+jcc（不产 setcc 物化），
//    本轮无 4.4 可复现形，**回滚不动**（属 4.4/ORIG 编译器 codegen 族，勿再试）。
// ③ ORIG 0x081dd5bd `movl $0x0,-0x1c(%ebp)`：static-guard 段（`_ZGVZ...6handle`
//    0x081dd5c4 读）之前的一个 int=0 具名槽，我方无对应 C++ 变量证据 ⇒ 保留现状。
}

// ============================================================================
// ORIG 0x0865798a T _ZN5CUser15serviceOfShusiaEv —— 休沙疲劳恢复服务。
// 幽灵 0x13；疲劳>99 0x12；Restrict(1,0x16) 0xd1；余额<价 0x16；
// 扣费 use_money(price,0xa,true)、AddValueStatistic(0x10,user,price)、
// setCurCharacStamina(100)、UseShusiaService((short)m_level)，成功返 0。
// 定义宿主为本 TU（CUser.cpp 由 RestoreCUserStatMethods 独占，经广播协调）。
// ============================================================================
#include "DisPatcherCore/serviceOfShusia.inc"
{
    if (IsCurCharacGhost())
    {
        return 0x13;
    }
    if (getCurCharacR()->m_stamina > 99)  // ORIG getCurCharacR + movzbl 0x75
    {
        return 0x12;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            this, (RestrictCategory::Enum)1, 0x16))
    {
        return 0xd1;
    }

    const int price = G_CDataManager()->m_serverParameter.getRecoverStaminaPrice(
        get_charac_level(), getCurCharacR()->m_stamina);
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
#include "DisPatcherCore/SetLoadRentalInfoFromExchange.inc"
{
    // ORIG 0x0822fc06：movzbl 参数后直接按字节存 +0x6ef91（无 0/1 归一化）。
    m_rentalFlag2 = flag;                      // +0x6ef91（m_rentalFlag2）
}

// ORIG 0x0868e672 T _ZN5CUser25RemoveCharacMercenaryInfoEj：转调雇佣兵管理器
#include "DisPatcherCore/RemoveCharacMercenaryInfo.inc"
{
    m_mercenaryMgr.RemoveCharac(characNo);     // UserMercenaryInfoMgr @ +0x6ef6c
}

// ORIG 0x0864a956 T _ZN5CUser31GetGuildIdBeforeDeleteCharacterEi：
// 返回 m_characList[slot].m_guildId（_Charac_info +0xdbe）
#include "DisPatcherCore/GetGuildIdBeforeDeleteCharacter.inc"
{
    return m_characList[static_cast<unsigned int>(slot)].m_guildId;
}


// ORIG 0x08230844 W _ZN5CUser31SetDeleteEventDungeonInvitationEb：
// 写 m_deleteEventDungeonInvitation 字节（objdump 逐指令转写；SelectDungeon::process 对全队调用）。
#include "DisPatcherCore/SetDeleteEventDungeonInvitation.inc"
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
#include "DisPatcherCore/MSG_ADD_SOCKET_TO_AVATAR.inc"
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
// [b28d-29] N82：帧 0x140 vs 0x150 + 局部对象槽平移（-0x3d vs -0x45），调用点全等  (ORIG 0x0821a412 insn 488->478)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 452（ORIG 488 / ours 478），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_39.inc"
{
    MSG_ADD_SOCKET_TO_AVATAR msg;
    // [b28e-L5] P171/P232 死写入照抄：ORIG 0x0821a428 在 ctor 之后发
    // `movl $0x0,-0x28(%ebp)`（= msg 槽尾 -0x3d+0x15 之后的 4B 局部，全函数
    // **0 个读取点**，objdump 全区间仅 1 处 -0x28(%ebp) 写）⇒ 原码有未读标量局部。
    int unusedZero = 0;   // 死写入
    (void)unusedZero;
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
    // [Lane38 2026-09-10] 符号绑定（判据㉓）：ORIG 0x0821a8c6/0x0821a8d6 在
    // `mov -0x1c(%ebp),%eax; add $0x3a0,%eax; movl $0x0,0x4(%esp)` 后
    // **call `_ZNKSt6vectorIiSaIiEEixEj`**（const 重载）；我方非 const 指针绑到
    // `_ZNSt6vectorIiSaIiEEixEj`（P167 MISSING/EXTRA 各 1）。本函数对 stackable 的
    // 全部用法（verifyStackableAction 0x0822ca58 `_ZNK…`、get_grade、m_vec3a0）皆
    // const ⇒ 原码即 `const CStackableItem*`。
    const CStackableItem* stackable = static_cast<const CStackableItem*>(cube);
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

    // [b28d] P0/P183：ORIG 0x0821a8xx 直接 `mov stackable,%eax; add $0x3a0,%eax`
    // 取 vector 成员地址（无 socketTypes 引用槽），我方引用槽撑帧 4B。
    if (stackable->m_vec3a0.empty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x16);
        return 0;
    }
    const int socketType = stackable->m_vec3a0[0];
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
    // [b28d] P0：ORIG 无 avatarKey 具名局部——每次直读 `mov -0x9e(%ebp),%eax`
    // （= avatarItem(-0xa5) 的 m_addInfo2 @+7）。
    // ORIG 0x0821aa8x：IsTempKey 判定前有一次 GetAvatarItemMgrW（结果丢弃，call site 保留）。
    inven->GetAvatarItemMgrW();
    if (WongWork::CAvatarItemMgr::IsTempKey(avatarItem.m_addInfo2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x11);
        return 0;
    }
    if (inven->GetAvatarItemMgrW()->isSocketAvatar(avatarItem.m_addInfo2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd1, 0x13);
        return 0;
    }

    stAvatarEmblemInfo_t emblem;
    emblem.init();
    if (source != 0 && source->IsAvatarItem())
        equip->getAvatarSocket(-1, emblem);
    inven->GetAvatarItemMgrW()->setEmblemSocketData(avatarItem.m_addInfo2, emblem);

    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, (int)msg.m_slotB, 1,
            (eItemDelReason)0x1b, true))
        return 0x11;

    DB_UpdateAvatarJewelSlot::makeRequest(
        (unsigned int)user->getCurCharacNo(),
        (unsigned int)avatarItem.m_addInfo2, &emblem);
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
// [b28d-29] P30/N4：ORIG 结构体基址 lea -0x1a3 + add 偏移两步，4.4 折单 lea；帧 0x31c vs 0x2ec（N82），语义集合全等  (ORIG 0x081f3540 insn 896->897)
#include "DB_AccountCargoSync.h"                // DB_InsertPoliceSaveChatting::makeRequest
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 737（ORIG 892 / ours 893），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] 实测 ndiff 483→466；两处 REAL 修正：
//  ① **REAL-BUG**：`std::string(notify.m_msg, sizeof(notify.m_msg))` 会把缓冲区尾部
//     未初始化字节纳入字符串（ORIG 0x081f3b63 只传 (const char*, alloc)，按 NUL 截断），
//     使下游 filtered.size()/strncpy 长度与上送 total 全部失真；
//  ② msgType==0x22 里 ORIG 0x081f4249(find_item 判空) 与 0x081f427f(is_stackable) 是
//     两个独立判定、各发一次 SendCmdErrorPacket(0x11,0x11)（合并成 || 少一次调用）。
// 残差封 N：std::string/allocator 的 C1↔C2、D1↔D2 变体（三方库，协议豁免）。
#include "DisPatcherCore/dispatch_sig_40.inc"
{
    if (user->get_state() < 3)
        return 0;

    char msgType;                                        // ORIG msg+0xd（-0x196）
    unsigned short target;                               // ORIG msg+0xe（-0x195）
    unsigned int recvCharno;                             // ORIG msg+0x10（-0x193）
    int msgLen;                                          // ORIG msg+0x36（-0x16d）
    char msgBuf[0x100];                                  // ORIG acStack_16d
    if (!packet.get_byte(*(char*)&msgType))
        return LineFunc(0x7ef5, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(target))
        return LineFunc(0x7ef6, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(recvCharno))
        return LineFunc(0x7ef7, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(msgLen))
        return LineFunc(0x7ef8, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(msgBuf, 0x100, msgLen))
        return LineFunc(0x7ef9, __PRETTY_FUNCTION__, 0, 0);

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
            return LineFunc(0x7f4b, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_str(nickBuf, 0x1e, srcLen))
            return LineFunc(0x7f4d, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_byte(*(char*)&nickFlag))
            return LineFunc(0x7f4f, __PRETTY_FUNCTION__, 0, 0);

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
            return LineFunc(0x7f71, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_str(nickBuf, 0x1e, srcLen))
            return LineFunc(0x7f73, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_byte(channelFlag))
            return LineFunc(0x7f78, __PRETTY_FUNCTION__, 0, 0);

        if (msgType == 7)
        {
            Packet_Monitor_Other_Channel_Chat notify;
            notify.m_characNo = (unsigned int)user->getCurCharacNo();
            notify.m_buddyNameLen = (char)((unsigned int)srcLen & 0xff);
            memcpy(notify.buddy_n_user_id_what, nickBuf,
                   (size_t)srcLen & 0xffu);
            memcpy(notify.m_msg, msgBuf, (unsigned char)msgLen);
            notify.m_msgLen = (unsigned char)msgLen;

            // ORIG 0x081f3b63 call _ZNSsC1EPKcRKSaIcE —— 只有 (const char*, alloc) 两参，
            // 即按 NUL 截断构造。原先多传 sizeof(m_msg) 会把缓冲区尾部未初始化字节
            // 一并纳入字符串（REAL-BUG：filtered.size() 随之失真）。
            std::string filtered(CSyncSlangFilter::GetInstance()->Filter(
                std::string(notify.m_msg)));
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
            return LineFunc(0x8020, __PRETTY_FUNCTION__, 0, 0);
        if (!((item->m_addInfo == 0x25 && msgType == 0xd) ||
              (item->m_addInfo == 0x28992f && msgType == 0xf)))
            return LineFunc(0x8025, __PRETTY_FUNCTION__, 0, 0);
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
            return LineFunc(0x8053, __PRETTY_FUNCTION__, 0, 0);
        if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
            return LineFunc(0x8057, __PRETTY_FUNCTION__, 0, 0);
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
            return LineFunc(0x806a, __PRETTY_FUNCTION__, 0, 0);
        CItem* itemInfo = G_CDataManager()->find_item(item->m_addInfo);
        // ORIG 0x081f4249（find_item 判空）与 0x081f427f（is_stackable）是**两个**独立
        // 判定，各发一次 SendCmdErrorPacket(0x11,0x11) 后 return 0；合并成 || 会少一次调用。
        if (itemInfo == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
        if (!itemInfo->is_stackable())
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
// [b28d-29] N82：帧 0xcc vs 0xc0 + 槽平移（-0x65 vs -0x13），调用点全等  (ORIG 0x08205898 insn 418->417)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 378（ORIG 418 / ours 416），残差=N7/N82 帧与 callee-saved 角色/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_41.inc"
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

// §6.2 巨件核销：P167 117/117 [0缺0多]；gap 761→761/777；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] N82/setcc-fold：帧 0x8c vs 0x90 + bool 局部槽物化（movb -0x1b）vs ORIG 寄存器岛；语义集合全等  (ORIG 0x081fc4de insn 761->777)
// 无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 982（ORIG 756 / ours 772），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_42.inc"
{
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x8b6a, 5);
        trace("Illegal GM Command : %d", user->get_acc_id());
        return LineFunc(0x8b6b, __PRETTY_FUNCTION__, 0, 0);
    }

    cMyTrace trace1(__PRETTY_FUNCTION__, 0x8b65, 0);
    trace1("GM(%d) trying to command(%d)", user->get_acc_id(),
           (int)msg->m_command);

    if ((int)(unsigned int)(size_t)user->getCurCharacR() == 0)
        return LineFunc(0x8b70, __PRETTY_FUNCTION__, 0, 0);

    cMyTrace trace2(__PRETTY_FUNCTION__, 0x8b72, 0);
    trace2("GM Command : %s", NumberToString(user->get_acc_id(), 0));

    bool result = true;                                  // ORIG local_1e(-0x1a)
    bool passFlag = false;                               // ORIG local_1d(-0x19) 恒 0
    // ORIG 反汇编：`IsGm(accId)` 后 xor $1/test；非 GM 走 Illegal trace+
    // LineFunc(0x8b91)（不静默 return 0）；GM 再检 passFlag（movzbl -0x19/
    // xor $1/test，非折叠），假则 return 0、真则 GM Pass trace + switch。
    if ((unsigned char)GlobalData::s_GM_Manager->IsGm(
            user->get_acc_id()) != 1)
    {
        cMyTrace trace3(__PRETTY_FUNCTION__, 0x8b90, 5);
        trace3("Illegal GM Command : %d", user->get_acc_id());
        return LineFunc(0x8b91, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!passFlag)
    {
        return 0;
    }
    {
        cMyTrace trace4(__PRETTY_FUNCTION__, 0x8bb6, 0);
        trace4("GM Pass Complete! Command(%d)", (int)msg->m_command);
    }

    // ORIG 以 byte 口径取载荷首 int（DisPatcherCore.h:716 MSG_DEBUG_COMMAND
    // `int m_arg` @+0x0e）的第 0/1 字节（movzbl 0xe / 0xf）——取具名成员的
    // 同宽度字节视图，不用裸偏移。
    const unsigned char* argBytes =
        reinterpret_cast<const unsigned char*>(&msg->m_arg);   // +0x0e..0x11

    switch (msg->m_command)
    {
    case 0:
    {
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) < GM_LV1)
            return LineFunc(0x8bcb, __PRETTY_FUNCTION__, 0, 0);
        GlobalData::s_GM_Manager->TurnGmMode(user->get_acc_id());
        if ((unsigned char)GlobalData::s_GM_Manager->GetCurrentGmMode(
                user->get_acc_id()) == 1)
        {
            user->SetGameMasterMode(true);
            result = _debugCommandOn(user, *msg);
        }
        else
        {
            cMyTrace trace5(__PRETTY_FUNCTION__, 0x8bd0, 0);
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
        result = _debugCommandSetGrowType(user, argBytes[0], argBytes[1]);
        break;
    case 4:
        result = _debugCommandClearSkill(user, *msg);
        break;
    case 5:
        result = _debugCommandSetSkill(user, argBytes[0], argBytes[1]);
        break;
    case 6:
        result = _debugCommandAddItem(
            user,
            (unsigned long)*(const unsigned int*)reinterpret_cast<
                const unsigned char*>(&msg->m_arg),
            *reinterpret_cast<const int*>(&msg->payload));   // +0x12
        break;
    case 7:
        result = _debugCommandSetLevel(user, (int)argBytes[0]);
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
            static_cast<unsigned int>(user->getCurCharacNo()), msg->m_arg);
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
        return LineFunc(0x8da2, __PRETTY_FUNCTION__, 0, 0);
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
// [b28d-29] N82/N7：帧 0x12c vs 0x13c + 尾值寄存器角色（ORIG %eax，4.4 %ebx），调用点全等  (ORIG 0x081f468c insn 381->357)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 303（ORIG 381 / ours 356），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [b28d-TakeDispatchCore] **P312**：`char digestHex[0x41] = {0};`（原 memset 调用）复现
// ORIG 0x081f490c 对齐剥离+rep stos 慢路 ⇒ ours 359→387、ndiff 214→180（fresh 复测；
// 上方 303 为更早读数）、gap 22→−6。残差仍为 P211/N18a/P30/N82 帧差族。
#include "DisPatcherCore/dispatch_sig_43.inc"
{
    if (user->get_state() >= 4)
        return 0;

    // ORIG 0x081f468c：wire 字段全体是单个栈 blob（base -0xf6，`lea -0xf6,%eax;
    // add $0xd/e/12/16/18/1c/20` 传址、`movzbl -0xe9` 等 disp 读）——单一 pack(1)
    // 视图结构体本地量复现连续落栈（技47/50 同址视图法；独立局部分散落槽为 4.4 常态、
    // 与 ORIG 差 20+ 行）。
    #pragma pack(push, 1)
    struct SetIPMsgView {
        char m_head[13];                 // +0x00..0x0c
        unsigned char m_natType;         // +0x0d（ORIG `movzbl -0xe9` 单步零延装载 ⇒ uchar）
        unsigned int m_innerIp;          // +0x0e
        unsigned int m_natIp;            // +0x12
        unsigned short m_natPort;        // +0x16
        unsigned int m_mtu;              // +0x18
        unsigned int m_macLen;           // +0x1c（ORIG get_int(unsigned int&) 实证）
        char m_macBuf[0x41];             // +0x20..0x60
    };
    #pragma pack(pop)
    SetIPMsgView msg;                    // 技36：无具名引用中转（引用会多占 spill 槽）
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 调 get_byte(char&)（ERc）。
    if (!packet.get_byte(*reinterpret_cast<char*>(&msg.m_natType)))
        return LineFunc(0x80e4, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(msg.m_innerIp))
        return LineFunc(0x80e5, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(msg.m_natIp))
        return LineFunc(0x80e6, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(msg.m_natPort))
        return LineFunc(0x80e7, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(msg.m_mtu))
        return LineFunc(0x80e8, __PRETTY_FUNCTION__, 0, 0);
    user->m_network.set_nat(msg.m_natType, msg.m_innerIp, msg.m_natIp,
                            msg.m_natPort, msg.m_mtu);

    if (!packet.get_int(msg.m_macLen))
        return LineFunc(0x8100, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(msg.m_macBuf, 0x41, msg.m_macLen))
        return LineFunc(0x8102, __PRETTY_FUNCTION__, 0, 0);

    char digestHex[0x41] = {0};          // P312：ORIG 0x081f490c 内联对齐剥离+rep stos 慢路
    getSha256UsingSalt(reinterpret_cast<const unsigned char*>(msg.m_macBuf),
                       0x41, digestHex);

    cMyTrace trace(__PRETTY_FUNCTION__, 0x810b, 0);
    trace("(acc_id: %s) (nat type: %d) (inner_ip: %d) (nat_ip: %d) "
          "(nat_port: %d) (mac_addr: %s) (mtu: %d)",
          NumberToString(user->get_acc_id(), 0), (int)(signed char)msg.m_natType,
          msg.m_innerIp, msg.m_natIp, (int)msg.m_natPort, digestHex, msg.m_mtu);

    // 第二次 set_nat：以 mac 摘要串写入（ORIG set_nat(...,const char* macAddr)）
    user->m_network.set_nat(msg.m_natType, msg.m_innerIp, msg.m_natIp,
                            msg.m_natPort, digestHex);

    char peerIp[0x14] = {0};             // ORIG 0x081f4a94..0x081f4ab0 5×movl $0 清零
    // [G5-SigA] P52/P183：ORIG 0x081f4ae4 `call GetPeerIP2` 后返回值**不经槽**，
    // 由 callee-saved %ebx 跨 0x081f4ae4..0x081f4af0 的 get_str_ip 调用保活
    // （0x081f4ae9 `mov %ebx,0x4(%esp)`），即原码是单语句
    // `checkDenyIP(get_str_ip(), GetPeerIP2(peerIp, 0x14))`（实参右→左求值）。
    // 原具名 char* ip2 多出槽写入 + 该 4B 槽使帧 0x12c→0x13c（fn_diff_tu 实测）。
    if (!(bool)(unsigned char)checkDenyIP(
            user->m_network.get_str_ip(),
            user->m_network.GetPeerIP2(peerIp, 0x14)))
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
    // [b28d-solo-DisPatcherCore] P103：ORIG 先调 CStreamGuard::operator->() 再
    // GetInBuffer（p167 MISSING _ZN12CStreamGuardptEv）⇒ 保留 operator-> 链。
    SIG_SAVE_PROXY_LOG* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_PROXY_LOG>();
    sig->m_accId = user->get_acc_id();
    strncpy(sig->m_strIp, user->m_network.get_str_ip(), 0x10);
    strncpy(sig->m_peerIp, peerIp, 0x10);
    sig->m_natType = (unsigned char)msg.m_natType;
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

#include "DisPatcherCore/Packet_Frame_Lag_Statistic_Add.inc"
    : PacketHeader(0xc26, 0x186)
{
}
// [b28d-29] N82/P183：帧 0x1b4 vs 0x1b8 + ORIG 无 -0x20 指针缓存而 4.4 新增，槽平移；语义集合全等  (ORIG 0x08217046 insn 418->368)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 239（ORIG 418 / ours 367），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_44.inc"
{
#define FLLF(line) \
    return LineFunc((line), __PRETTY_FUNCTION__, 0, 0)

    Packet_Frame_Lag_Statistic_Add pkt;
    // 【P156 保留项：线格式序列化缓冲 + ORIG 别名算址】本包是纯 wire 缓冲区，
    // ORIG（0x082173cd/0x08217550/0x08217699/0x082176ec）以「包基址 + 常数/变数
    // 累加」的两步形态算址（行 stride 0x38、尾表 +0x170/+0xa 与行区重叠别名）。
    // 实测改写成 `pkt.m_body[i]` 具名成员索引后，GCC4.4 O0 会把算址折叠成单条
    // lea（419→375 insn），使本函数由 NEAR 退为 DIFF —— 按验收口径保留原形态。
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
        // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 为**4 个独立 get_short(short&)
        // 调用点**（Ghidra C：单层 r 循环体内 4 条连续调用，LineFunc 0xc914..0xc917），
        // 原内层 c 循环把 4 个调用点折成 1 个（p167 MISSING get_shortERs×3 + LineFunc×3）
        // ⇒ 按 ORIG 展开为 4 条，dst = base + r*0x38 + 0x24 + {0,2,4,6}。
        if (!packet.get_short(*reinterpret_cast<short*>(base + r * 0x38 + 0x24)))
            FLLF(0xc914);
        if (!packet.get_short(*reinterpret_cast<short*>(base + r * 0x38 + 0x24 + 2)))
            FLLF(0xc915);
        if (!packet.get_short(*reinterpret_cast<short*>(base + r * 0x38 + 0x24 + 4)))
            FLLF(0xc916);
        if (!packet.get_short(*reinterpret_cast<short*>(base + r * 0x38 + 0x24 + 6)))
            FLLF(0xc917);
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

    // ---- 尾表（ORIG -0xc）：读侧算址 base+0xa+(i+0xb8)*2，缺项清零 base+0xa+i*2 ----
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
//   [b28c-34] 判词（76/76 AE；TU compare 口径：符号不在 DIFF/NEAR 列表）：
//   两处根因已实证——
//   ①+0xb/+0x166 store 装载序：ORIG LHS-first（`mov 0x8(%ebp),%eax` 先载 this），
//     真实字段 char 与 uchar 形参异型 ⇒ 4.4 恒 RHS-first（P211）；源侧 cast 无效
//     （探针 s2/s4），经本 TU 局部 uchar 具名视图写入归位（P95/P235）。
//   ②循环 memcpy 两侧算址：ORIG 两侧均为 char 数组下标形（src `imul; add 0x28(%ebp),%eax`、
//     dst `imul; add $0x20; add 0x8(%ebp); add $0xe` 三段独立 add）；整型加法链/指针
//     算术形在 4.4 下恒折 lea+常量合并（探针 a1..a6/b1..b8/c1..c6），改用下标形逐条复现
//     （探针 w1）。语义：uchar/char 视角写同一字节，位模式不变；下标越界写与原码一致（P171）。
// ============================================================================
#pragma pack(push, 1)
struct MegaPhoneHyperLinkSetView
{
    char m_pad00[0xb];            // +0x00..0x0a（PacketHeader + m_sg）
    unsigned char m_channel;      // +0x0b（ORIG `mov %dl,0xb(%eax)`）
    unsigned short m_uniqueId;    // +0x0c
    unsigned char m_type;         // +0x0e
    char m_name[0x1e];            // +0x0f..0x2c
    unsigned char m_itemCount;    // +0x2d
    char m_items[0x68];           // +0x2e..0x95
    char m_pad96[0xd0];           // +0x96..0x165
    unsigned char m_msgLen;       // +0x166（ORIG `mov %dl,0x166(%eax)`）
    char m_msg[0x100];            // +0x167..0x266
};
#pragma pack(pop)
static_assert(sizeof(MegaPhoneHyperLinkSetView) == 0x267,
              "MegaPhoneHyperLinkSetView size");
static_assert(offsetof(MegaPhoneHyperLinkSetView, m_channel) == 0xb,
              "MegaPhoneHyperLinkSetView m_channel off");
static_assert(offsetof(MegaPhoneHyperLinkSetView, m_msgLen) == 0x166,
              "MegaPhoneHyperLinkSetView m_msgLen off");
#include "Packet_Monitor_MegaPhone_Hyper_Link.h"
#include "DisPatcherCore/set.inc"
    unsigned char channel, unsigned short uniqueId, char const* name,
    unsigned char msgLen, char const* msg, unsigned char type,
    unsigned char itemCount, hyperlink_item_info const* items)
{
    // [b28c-34] ORIG：`mov 0x8(%ebp),%eax; movzbl -0x1c(%ebp),%edx; mov %dl,0xb(%eax)`
    reinterpret_cast<MegaPhoneHyperLinkSetView*>(this)->m_channel = channel;
    m_uniqueId = uniqueId;
    m_type = type;
    strncpy(m_name, name, 0x1e);
    strncpy(m_msg, msg, 0x100);
    // [b28c-34] ORIG：`mov 0x8(%ebp),%eax; movzbl -0x24(%ebp),%edx; mov %dl,0x166(%eax)`
    reinterpret_cast<MegaPhoneHyperLinkSetView*>(this)->m_msgLen = msgLen;
    m_itemCount = itemCount;
    // ORIG 0x086d1b9a：src `mov -0xc,%eax; imul $0x68; add 0x28(%ebp),%eax`、
    // dst `mov -0xc,%edx; imul; add $0x20; add 0x8(%ebp); add $0xe`——两侧均为
    // char 数组下标形（各重算一遍 i*0x68、常量不合并）。
    for (int i = 0; i < itemCount; ++i)
    {
        memcpy(&m_items[i * 0x68],
               &reinterpret_cast<char*>(
                   const_cast<hyperlink_item_info*>(items))[i * 0x68], 0x68);
    }
    packetSize = 0x167 + msgLen;
}
