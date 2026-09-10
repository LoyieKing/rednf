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
};
#pragma pack(pop)
// [H5DisPatcher 2026-09-11] 删 `char m_tail[0x1]`：ORIG 视图口径为
// 0x081ede34 `lea -0x2f(%ebp),%eax; add $0xd,%eax` ⇒ 基址 -0x2f + 成员 0xd，
// 视图末端 -0x21 ⇒ sizeof **0xf**（我方旧形 0x10 使基址落 -0x30，
// `movzwl -0x22` 变 `-0x23`）。无消费点引用 m_tail。
static_assert(sizeof(IncreStatusSigView) == 0xf, "IncreStatusSigView size");

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
#include "DisPatcherCore/read_1.inc"
#include "DisPatcherCore/process.inc"

#include "DisPatcherCore/DisPatcher_Login.inc"
#include "DisPatcherCore/dtor_DisPatcher_Login.inc"
// §6.2 巨件核销：P167 168/168 [0缺0多]；gap 986→986/990；残差归因 N82/P112
// （槽复用策略 + 块排布，diff 形态计数对称无缺类，详见
// [b28d-29] N82：帧 0x1c0 vs 0x1d0 + 局部对象槽平移（-0xc4 vs -0x90），调用点全等  (ORIG 0x081e8c78 insn 986->990)
// docs/asm_translation_patterns.md 负例表）。无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 704（ORIG 986 / ours 990），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_1.inc"
#include "DisPatcherCore/read_2.inc"
#include "DisPatcherCore/process_1.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetIP.inc"
#include "DisPatcherCore/read_3.inc"
#include "DisPatcherCore/process_2.inc"

// [b28d-29] N78/N82：ORIG 4 个 dword 清零 vs 4.4 逐字节 movb 展开 + 帧 0x60 vs 0xa0；语义等价  (ORIG 0x081ed948 insn 316->373)
#include "DisPatcherCore/dtor_DisPatcher_Exit.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 344（ORIG 316 / ours 372），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_2.inc"
#include "DisPatcherCore/read_4.inc"
#include "DisPatcherCore/process_3.inc"

// [b28d-29] N82/setcc-fold：帧 0x40 vs 0x60 + 槽平移与 sete 折痕，调用点全等  (ORIG 0x081c686e insn 267->285)
#include "DisPatcherCore/dtor_DisPatcher_SelectCharac.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 225（ORIG 267 / ours 284），残差=P168a④ bool 槽化/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error.inc"

// ORIG 0x081c6c22 T _ZN23DisPatcher_SelectCharac22checkPvpEnterCharacterEP5CUserR17MSG_CHARAC_SELECT
// （报告 DisPatcher_SelectCharac/checkPvpEnterCharacter.md 全文转写，wave2）：
// user==0 → false；PvP 频道且槽位职业为 9/10（ restricting job ）→ false；否则 true。
#include "DisPatcherCore/checkPvpEnterCharacter.inc"

// ORIG 0x081cab3a T _ZN18DisPatcher_UseCoin20usedCoinDungeonCheckEP5CUserP6CParty
// （报告 DisPatcher_UseCoin/usedCoinDungeonCheck.md 汇编+C 全文转写，wave2）。
#include "DisPatcherCore/usedCoinDungeonCheck.inc"

// ORIG 0x081ca63a T _ZN18DisPatcher_UseCoin11Dungeon_check_errorEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] N82/P183：ORIG 每次重调 GetParty 并落独立槽（不复用），4.4 复用同槽 + 帧 -0x14；语义集合全等  (ORIG 0x081ca63a insn 87->87)
// （报告 DisPatcher_UseCoin/Dungeon_check_error.md 汇编+C 全文转写，wave2）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 57（ORIG 87 / ours 86），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/Dungeon_check_error.inc"

// ORIG 0x081ca75c T _ZN18DisPatcher_UseCoin11WarRoom_check_errorEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseCoin/WarRoom_check_error.md 汇编+C 全文转写，wave2）。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 39 / ours 39 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/WarRoom_check_error.inc"
// ORIG 0x0821b098 W _ZN28DisPatcher_UdpCharacteristic13partyPingStatEP5CUserRtS2_
// （报告 DisPatcher_UdpCharacteristic/partyPingStat.md 汇编+C 全文转写，wave2）：
// Party_Ping 统计包（0xfa7,0xe）：+0xa=bad、+0xc=bad+total，经统计代理 SendPacket。
#include "DisPatcherCore/partyPingStat.inc"

// ORIG 0x0821afbc W _ZN28DisPatcher_UdpCharacteristic11pvpPingStatEP5CUserRtS2_
// （报告 pvpPingStat.md 全文转写，wave2）：Pvp_Ping 包（0xfa8,0xe）同型。
#include "DisPatcherCore/pvpPingStat.inc"

// ORIG 0x0821b02a W _ZN28DisPatcher_UdpCharacteristic15fairPvpPingStatEP5CUserRtS2_
// （报告 fairPvpPingStat.md 全文转写，wave2）：Fair_Pvp_Ping 包（0x27ed,0xe）同型。
#include "DisPatcherCore/fairPvpPingStat.inc"
// ORIG 0x081c632a T _ZN23DisPatcher_SelectCharac4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_SelectCharac/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x156f, PRETTY, 0, 0)。
#include "DisPatcherCore/read_5.inc"
#include "DisPatcherCore/send.inc"

// ORIG 0x081c6380 T _ZN23DisPatcher_SelectCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0x1587,pretty,err,acc_id)。
// resetSaveFlags → 视图状态/上次登录判定 → 综合PvP频道抑制 → exchange 命中计数 →
// 回填 MSG+0xe/+0x12（_Charac_info.m_field14b8/m_field14b7）→ 九路 DB/ES 请求 →
// [b28d-29] N82：帧 0x1a0 vs 0x1b4 + 槽平移，调用点全等  (ORIG 0x081c6380 insn 320->323)
// 清上次登录 → 死塔最佳记录/注册日期 DB 请求 → LimitNpcBuyItemInfoAll 监视包。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 283（ORIG 320 / ours 323），残差=N7/N82 帧与 callee-saved 角色/P168a④ bool 槽化/P211/N18a 中转/store 序/P30/N4 槽；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_4.inc"

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
// ORIG 0x081c727c T _ZN23DisPatcher_SelectCharac15RequestDBTicketEP5CUserR17MSG_CHARAC_SELECT
// （报告 RequestDBTicket.md）：opcode 0xb / uid → DisableCharacInfo(4) →
// 有效最后登录角色时追加 channelNo/characNo → SIG_LOAD_TICKET(0x19c 清零，
// +0=get_charac_no(msg.m_slot)) → 有效登录 put Q5 否则 Q2。
// SIG 缓冲首字 = charac_no（SigTypes_C.h SIG_LOAD_TICKET::m_characNo 权威字段；
// 视图 cast 已按 P0「无具名局部」律删除：ORIG 该 store 为右值内联直写）。
// [b28d-29] N93附/P222：同 RequestDBSkill 尾块布局差（4.4 就地 epilogue vs 4.1 jmp 尾岛）  (ORIG 0x081c727c insn 120->119)
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 55（ORIG 120 / ours 123），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBTicket.inc"

// ORIG 0x081c7bac T _ZN23DisPatcher_SelectCharac20RegistTimerSchedulesEP5CUser
// （报告 RegistTimerSchedules.md）：G_TimerQueue()->InsertTimer(OBJ_TYPE_0, uid,
// [b28d-29] NEAR=N7/N82附：两 callee-saved 角色对调（ORIG getCurCharacNo->%esi / get_unique_id->%edi，4.4.7 恒反向）；源码已为内联实参形，P248 三档试尽不翻（4.4.6 后端可翻但本波禁改路由）  (ORIG 0x081c7bac insn 34->35)
// TIMER_MESSAGE_0x51, 0, unique_id, characNo) 后恒返 1。
// [b28d-solo-DisPatcherCore] 复核 NEAR：ndiff 8（ORIG 34 / ours 34），残差=N82 槽/块序；N 封顶（详见上方 b28d-29 归因）。
#include "DisPatcherCore/RegistTimerSchedules.inc"

// ORIG T 0x081c79f2（RequestBlackCount.md）：SIG 0xb3 → CACHE_BLACK_COUNT；memset 0x10。
#include "DisPatcherCore/RequestBlackCount.inc"

// ORIG T 0x081c6ee6：直接尾调 RequestDBInventory_forchina。
#include "DisPatcherCore/RequestDBInventory.inc"

// ORIG T 0x081c7c1c（RequestDBInventory_forchina.md）：SIG 8 → CACHE_INVENTORY；
// BigStreamPool；memset 0x1544c 后 acc/charac/tagInven 清理/+0x8←MSG.m_0e。
// [b28d-29] setcc-fold：ORIG 尾 bool 物化岛（mov $1,%esi/mov $0,%ebx + test），4.4 恒折单条 mov/直分支  (ORIG 0x081c7c1c insn 137->134)
// 尾部两分支均返回 1（ORIG esi 未初始化路径经 test ebx 归一为恒真）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 8（ORIG 137 / ours 133），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBInventory_forchina.inc"

// ORIG T 0x081c70c2（RequestDBQuest.md）：SIG 10 → CACHE_QUEST；BigStreamPool；
// memset 0x7608 后仅写 characNo。
#include "DisPatcherCore/RequestDBQuest.inc"

// ORIG T 0x081c7436（RequestDBDungeonClear.md）：SIG 12 → CACHE_DUNGEON_CLEAR；
// StreamPool；memset 0x1810 后 characNo@0 / accId@4。
#include "DisPatcherCore/RequestDBDungeonClear.inc"

// ORIG T 0x081c7836（RequestDBKillMonsterInfo.md）：SIG 0x282 → CACHE_KILL_MONSTER；
// BigStreamPool；memset 0xafcc 后 characNo 写于尾部 +0xafc8。
#include "DisPatcherCore/RequestDBKillMonsterInfo.inc"

// ORIG T 0x081c7602（RequestDBETC.md）：SIG 0x20 → CACHE_ETC；BigStreamPool；
// memset 0x14b60；serverGroup/channelNo ← CEnvironment；tagEtc 清理；
// [b28d-29] P254/N18a：字段->实参两段中转（ORIG mov mem,%eax; mov %eax,%edx）与块序差；语义集合全等  (ORIG 0x081c7602 insn 152->154)
// job 预写 combo 视图 m_job（SIG 内绝对偏移 +0x14ab0）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 41（ORIG 152 / ours 153），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBETC.inc"


// ORIG T 0x081c6f08（RequestDBSkill.md）：SIG 9 → CACHE_SKILL；StreamPool；
// [b28d-29] N93附/P222：尾块布局（ORIG 先 jmp 尾岛再统一 epilogue，4.4 就地内联 epilogue），槽集合与调用点全等  (ORIG 0x081c6f08 insn 120->119)
// memset 0x414 后仅写 characNo@0；队列 5(有效上次登录)/2(DB_Q)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 55（ORIG 120 / ours 123），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBSkill.inc"

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

// [b28d-29] setcc-fold + N82：ORIG 0x081c08cf 短路 bool 物化岛（mov $1/$0 + 二次 test）4.4 恒折 setne；cMyTrace 槽 -0x34 vs -0x24（4.4 复用，帧 0x44 vs 0x34）  (ORIG 0x081c0830 insn 140->136)
#include "DisPatcherCore/dtor_DisPatcher_CreateCharac.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 15（ORIG 140 / ours 135），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error_1.inc"
#include "DisPatcherCore/check_state.inc"
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
#include "DisPatcherCore/send_1.inc"

// ORIG T 0x081c0a1a（makeGift.md）：按创建槽位（=职业索引）发放初始武器/衣服与
// 职业赠礼道具。find_item 失败时仅置标志 0；命中则 Inven_Item 预填 +0x02 索引后
// [b28d-29] P205/P30：ORIG 单条 scaled `mov <A>(,%eax,8)`，4.4 拆 add+scaled；字段直读 vs 缓存差，语义集合全等  (ORIG 0x081c0a1a insn 227->223)
// 经 CItem::make_item（vtable+0x08）填充并整体拷贝入 SIG 槽位。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 158（ORIG 225 / ours 221），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/makeGift.inc"

#include "DisPatcherCore/dtor_DisPatcher_DeleteCharac.inc"
// ORIG 0x081c14f0 T _ZN23DisPatcher_DeleteCharac11check_errorEP5CUserR8MSG_BASE
// [b28d-29] N82/P0：msg 形参副本落 -0x18（ORIG -0x14 直读形），帧 0x44 vs 0x40，调用点全等  (ORIG 0x081c14f0 insn 199->198)
// 报告：docs/class_func_reports/DisPatcher_DeleteCharac/check_error.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 76（ORIG 199 / ours 197），残差=N7/N82 帧与 callee-saved 角色/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error_2.inc"
#include "DisPatcherCore/check_state_1.inc"
#include "DisPatcherCore/read_7.inc"
#include "DisPatcherCore/send_2.inc"

// ORIG 0x081c1182 T _ZN23DisPatcher_DeleteCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0xa41,pretty,err,acc_id)。
// RemoveCharacMercenaryInfo → StreamPool 取流构造 SIG_DELETE_CHARAC（memset 0x14）→
// characNo==0 则 m_errNo=0x15 并 Free 回流 → 填公会 id → lockDeleteCharac 成功投递
// [b28d-29] N82/P211：帧 0x5c vs 0x50 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081c1182 insn 216->210)
// DB_Q 并打 cMyTrace（accid 十进制串 / m_characNo / m_name），失败 Free + LineFunc(0xa68)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 134（ORIG 216 / ours 210），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_6.inc"

#include "DisPatcherCore/dtor_DisPatcher_ReturnToSelectCharacter.inc"
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 77 / ours 77 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/dispatch_sig_3.inc"
#include "DisPatcherCore/read_8.inc"

// ORIG 0x0823349a W _ZN34DisPatcher_ReturnToSelectCharacter7processEP5CUserR8MSG_BASER9ParamBase
// push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，
// 全部逻辑位于 dispatch_sig(0x081fd25c，见 docs/class_func_reports/
// DisPatcher_ReturnToSelectCharacter/dispatch_sig.md；按分工 dispatch_sig 保持 stub)。
#include "DisPatcherCore/process_7.inc"

#include "DisPatcherCore/dtor_DisPatcher_GetUserInfo.inc"
#include "DisPatcherCore/check_error_3.inc"
// ORIG 0x081c3d46 T _ZN22DisPatcher_GetUserInfo4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_GetUserInfo/read.md 逐块映射）：get_short→msg+0xd、
// get_byte→msg+0xf；失败各走 LineFunc(0xf5b/0xf5d, PRETTY, 0, 0)。
#include "DisPatcherCore/read_9.inc"

// ORIG 0x081c3dd8 T _ZN22DisPatcher_GetUserInfo7processEP5CUserR8MSG_BASER9ParamBase
// err=check_error；>0 返 -1；<0 LineFunc(0xf6d,pretty,0,acc_id)（第三参恒 0）；
// 否则 G_GameWorld()->get_user_info(user, (u16)msg+0xd, (s8)msg+0xf)，返回 err。
#include "DisPatcherCore/process_8.inc"

#include "DisPatcherCore/dtor_DisPatcher_RecoverStamina.inc"
// ORIG 0x081c3f4a T _ZN25DisPatcher_RecoverStamina11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_RecoverStamina/check_error.md 逐块映射）：state!=3 → 99；
// SecuProtectionField Check(user,0x2f) 非 0 直返；否则 getCurCharacR 为空 → -1。
#include "DisPatcherCore/check_error_4.inc"
#include "DisPatcherCore/read_10.inc"
#include "DisPatcherCore/send_3.inc"

// ORIG 0x081c3eae T _ZN25DisPatcher_RecoverStamina7processEP5CUserR8MSG_BASER9ParamBase
// err>0：SendCmdErrorPacket(cmd=9, err&0xff) 后返回 -1；err<0：LineFunc(0xf9d,pretty,0,acc_id)
// 并返回其值；err==0：param+0x4 = user->serviceOfShusia()，返回 err。
#include "DisPatcherCore/process_9.inc"


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
#include "DisPatcherCore/read_11.inc"
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
#include "DisPatcherCore/read_12.inc"
#include "DisPatcherCore/process_11.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPartyInfo.inc"
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮：返回 unsigned（与 int 字段异型，
// 复现 process 调用点 RHS-first+edx 中转）；返回类型不入 mangled/ABI。
#include "DisPatcherCore/check_error_5.inc"
// ORIG 0x081c4396 T _ZN23DisPatcher_SetPartyInfo4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_SetPartyInfo/read.md 汇编+反编译 C 全文转写）：
// get_byte(titleIndex) → titleIndex==0 时 get_int(+0x0e) 并 get_str(title,0x20,+0x0e)
// → get_byte(userMax)/get_short(dungIndex)/get_byte(dungDiffi)；任一失败 LineFunc
// 0x10d9/0x10dd/0x10e0/0x10e2/0x10e5/0x10e6，全成功返回 0。
#include "DisPatcherCore/read_13.inc"
#include "DisPatcherCore/send_4.inc"

// ORIG 0x081c4562 T _ZN23DisPatcher_SetPartyInfo7processEP5CUserR8MSG_BASER9ParamBase
// param+0x4 = check_error；>0 返回 0；<0 LineFunc(0x10f6,pretty,err,acc_id) 并返回其值；
// 无队：G_CGameManager()->GetParty() 空则 param=4 返回 0，否则 create_party +
// SetPartyInfoUI + send_party_info_to_all(party,0)；有队：非队长 param=8，
// 队长 SetPartyInfoUI + send_party_info_to_all(party,1)；均返回 0。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 96 / ours 96 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_12.inc"

#include "DisPatcherCore/dtor_DisPatcher_LeaveParty.inc"
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
#include "DisPatcherCore/check_error_6.inc"
#include "DisPatcherCore/check_state_2.inc"
#include "DisPatcherCore/read_14.inc"
#include "DisPatcherCore/send_5.inc"

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

#include "DisPatcherCore/dtor_DisPatcher_WalkoutParty.inc"
// ORIG 0x081cb062 T _ZN23DisPatcher_WalkoutParty11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_WalkoutParty/check_error.md 逐块映射）：user 空 →
// LineFunc(0x2053)；state==3 → 0；否则 -1。
#include "DisPatcherCore/check_error_7.inc"
// ORIG 0x081caf42 T _ZN23DisPatcher_WalkoutParty4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_WalkoutParty/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x202b, PRETTY, 0, 0)。
#include "DisPatcherCore/read_15.inc"
#include "DisPatcherCore/send_6.inc"

// ORIG 0x081caf98 T _ZN23DisPatcher_WalkoutParty7processEP5CUserR8MSG_BASER9ParamBase
// err>0 原样返回；err<0 返回 -1；否则 param+0x4=0，无队 param=0x13；
// 有队先 SendWalkout(msg+0xd 槽位, WALKOUT_TYPE_0) 再 param = walkout_member(user, 槽位)，返回 0。
#include "DisPatcherCore/process_14.inc"

// [b28d-29] N82：帧 0x34 vs 0x58 + 条件极性折痕（je/jne），槽集合与调用点全等  (ORIG 0x081c9f6c insn 228->246)
#include "DisPatcherCore/dtor_DisPatcher_StartGame.inc"
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 332（ORIG 228 / ours 246），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/check_error_8.inc"
#include "DisPatcherCore/read_16.inc"
#include "DisPatcherCore/send_7.inc"

// ORIG T 0x081ca2a8 _ZN20DisPatcher_StartGame19excludeAddHackCountEP5CUser
// （excludeAddHackCount.md 全文转写）：空用户/副本入口区/moveSpace==1 免记，否则记账。
#include "DisPatcherCore/excludeAddHackCount.inc"

// ORIG 0x081ca2fe T process（process.md 全文转写）。
// err>0 返 0；err<0 LineFunc(0x1e5b)；err==0：世界图 hasDeathTower 死调用后，
// 未组队走 GameManager 单人 party（hack 记账 0x1f6 + set_single_play + game_start）；
// [b28d-29] N82：全帧槽号平移 4（ORIG 帧更浅），助记符序列与调用点全等  (ORIG 0x081ca2fe insn 129->133)
// 已组队且 state==3 时对所在 party game_start；均返回 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 55（ORIG 129 / ours 132），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_15.inc"

#include "DisPatcherCore/dtor_DisPatcher_SelectDungeon.inc"
// ORIG 0x081c7f32 T _ZN24DisPatcher_SelectDungeon11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_SelectDungeon/check_error.md 汇编+反编译 C 全文转写）：
// state∉{5,0xd} 或 IsBusyDisjointStore → 0x7fffffff；m_field10∉[0,2] 或
// m_dungeonType<0 → -1；m_field11∉[0,1] → -2；m10>0 且 m11>0 → 0x7fffffff；
// m10!=2 时要求队长（getManager()==user）、party state==1、!(m11==1&&is_quick_party)；
// 两段重复 party 检查（ORIG 0x0d9 与 0x0a9 两块），全部通过 → 0。
#include "DisPatcherCore/check_error_9.inc"
#include "DisPatcherCore/read_17.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 35（ORIG 201 / ours 198），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/send_8.inc"

// ORIG T 0x081c9172 _ZN24DisPatcher_SelectDungeon15checkBlueMarbleEP6CParty
// [b28d-29] AE：38=38 全等（同上重定位归一）  (ORIG 0x081c9172 insn 38->39)
// （checkBlueMarble.md 全文转写）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 38 / ours 38 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/checkBlueMarble.inc"

// ORIG 0x081c8102 T process（process.md 全文转写，含汇编分支次序）。
// §6.2 巨件核销：P167 95/95 [0缺0多]；gap 810→810/830；残差归因 N82/P112。
// 真修 1 条：死块 [HackUser] logFormat(0x198a) 误传 member 指针 → 按 ORIG
// [b28d-29] N82/P211：帧与参数副本槽（-0x90 vs -0x98）+ store 装载序，调用点全等  (ORIG 0x081c8102 insn 810->831)
// +0x13e 改 member->get_acc_id()（补齐缺失的 _ZNK5CUser10get_acc_idEv 调用点）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 840（ORIG 808 / ours 826），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_16.inc"

#include "DisPatcherCore/dtor_DisPatcher_SendMess.inc"
// dispatch_sig 真实实现见文件尾第二批段（ORIG T 0x081f3540）。
#include "DisPatcherCore/read_18.inc"

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
// ORIG 0x081c17c0 T _ZN21DisPatcher_DeleteItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_DeleteItem/read.md（汇编+反编译 C 全读）
#include "DisPatcherCore/read_19.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 7（ORIG 154 / ours 157），残差=P211/N18a 中转/store 序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/exercise_mode.inc"
#include "DisPatcherCore/restricted_check.inc"
#include "DisPatcherCore/condition_check.inc"
// ORIG 0x081c2792 T _ZN21DisPatcher_DeleteItem4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_DeleteItem/send.md 汇编+反编译 C 全文转写）：
// m_4!=0x7fffffff 时发包。m_4==0：put_header(1,0x12)/1/m_8/m_9，i<(char)m_9 循环
// put_short(m_slotsD2[i])/put_int(m_counts19c[i])/put_short(m_slotsA[i])，
// finalize(true)+Send；否则 0/errNo/m_8 三字节包。
#include "DisPatcherCore/send_9.inc"


// ORIG 0x081c1dda T process（process.md 全文转写）。
// §6.2 巨件核销：P167 61/61 [0缺0多]；gap 31→1；残差归因 N82/P112。
// 真修 1 条：GetInvenSlot 返回 4.4 直构缺 ORIG +0x25e~+0x327 的 17 条逐字段
// [b28d-29] N82/P211：帧 0x100 vs 0x110 + 参数副本槽 + store 装载序；bool 条件 4.4 test 直判 vs 4.1 cmpb，语义集合全等  (ORIG 0x081c1dda insn 651->653)
// 拷贝（sret 匿名槽→具名局部）→ 改两段式（slotItem 收 sret + 拷贝入 item）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 400（ORIG 651 / ours 653），残差=P168a④ bool 槽化/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_18.inc"
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
// ORIG 0x081c54e0 T _ZN19DisPatcher_MoveItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_MoveItem/read.md（汇编+反编译 C 全读）
// 串行 10 次 get_*；任一失败即 LineFunc(行号, pretty, 0, 0)。msgPtr 局部落 -0xc(%ebp)。
#include "DisPatcherCore/read_20.inc"
// ORIG 0x081c5b76 T _ZN19DisPatcher_MoveItem4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_MoveItem/send.md 汇编+反编译 C 全文转写）：
// result(m_4)!=0x7fffffff 时发包；失败分支 put_byte(0)/err/idx/spot 四字节；
// 成功分支 put_byte(1)/idx/count/itemIdx/spot/newCount 后 Send，并经
// ARAD::Singleton<AvatarRechargeServer>::Get()->SendDurability 上报耐久。
#include "DisPatcherCore/send_10.inc"


// ORIG 0x081c5904 T process（process.md 全文转写）。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 136（ORIG 192 / ours 196），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_19.inc"
#include "DisPatcherCore/dtor_DisPatcher_SortItem.inc"
// ORIG 0x081c9d64 T _ZN19DisPatcher_SortItem11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_SortItem/check_error.md 逐块映射）：user 空 →
// LineFunc(0x1d56)；state<=2 → -1；否则 0。
#include "DisPatcherCore/check_error_12.inc"
// ORIG 0x081c9bfe T _ZN19DisPatcher_SortItem4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_SortItem/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x1d16, PRETTY, 0, 0)。
#include "DisPatcherCore/read_21.inc"
// ORIG 0x081c9dba T _ZN19DisPatcher_SortItem4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_SortItem/send.md 汇编+反编译 C 全文转写）：
// put_header(1,0x14)；result==0 → put_byte(1)/(char)invenType，否则 0/(char)result/
// (char)invenType；finalize+Send。result==0 时 send_itemspace((char)invenType)；
// [b28d-29] 已修 DIFF->AE：if-else 决策链改 switch（P17/P251/P261，ORIG 0x081c9ec6 稀疏 switch 树）  (ORIG 0x081c9dba insn 128->129)
// result==0 且 invenType 1/0/2/7 分别 SendItemLockListAvatar/Inven/Cargo/Creature。
#include "DisPatcherCore/send_11.inc"


// ORIG 0x081c9c54 T process（process.md 全文转写）。
#include "DisPatcherCore/process_20.inc"
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
// ORIG 0x081be20a T _ZN18DisPatcher_BuyItem4sendEP5CUserR9ParamBase
// 报告：docs/class_func_reports/DisPatcher_BuyItem/send.md（汇编+反编译 C 全读）
// [b28c-L19 N18a] 唯一正文差（165/164）：`UseFinishPoint(out.m_8c)` 处 ORIG
// [b28d-29] N18a/P254：ORIG `mov 0x8c(%eax),%eax; mov %eax,%edx` 冗余 regmove（int 字段->int 形参），4.4 单段直载 edx 不可反向造  (ORIG 0x081be20a insn 165->165)
// `mov 0x8c(%eax),%eax; mov %eax,%edx` 冗余 regmove vs 4.4 单步直载 edx（反向不可造）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 165 / ours 165 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/send_12.inc"

// ORIG 0x081bed44 T _ZN18DisPatcher_BuyItem15_onBuyItemEventERK12BuyItemParamP5CUser
// 报告 _onBuyItemEvent.md：push %ebp; mov %esp,%ebp; pop %ebp; ret —— 原始二进制
// 即为纯空实现（购买事件挂钩在 ORIG 中未挂任何回调），忠实转写空函数。
#include "DisPatcherCore/_onBuyItemEvent.inc"

// ORIG 0x081beab0 T get_nMaxBuyableCount（报告 get_nMaxBuyableCount.md 全文映射）：
// 对六种货币价格（money/cera/winpoint/finishpoint/guildPowerWarPoint/userPowerWarPoint）
// [b28d-29] N18b/P197③：ORIG 三目 min 形（cmp mem,%eax + jge 双出口）+ 两 dead 局部预清；4.4 折 reg-reg cmp 且丢预清  (ORIG 0x081beab0 insn 130->123)
// 逐项取「持有量 / 单价」与 0x7fffffff 取小，返回可购上限。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 34（ORIG 130 / ours 122），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/get_nMaxBuyableCount.inc"

// ORIG 0x081be658 T get_data（报告 get_data.md ## 反编译 C 全文映射）：
// 解析商城物品 → 材料槽 → 六种单价（>1 件时乘数量并校验上限）→ 持有量校验。
// [b28d-29] N82/N18a：帧 0x20 vs 0x34 + 返回指针缓存槽（-0x1c）与两段中转差；语义集合全等  (ORIG 0x081be658 insn 310->328)
// 返回 0=成功；0x11=物品不存在；1=参数非法；10=余额不足；0x24=公会会长限制。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 265（ORIG 310 / ours 327），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P168a④ bool 槽化/P211/N18a；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/get_data.inc"

// ORIG 0x081bec5e T statistic_process（报告 statistic_process.md ## 反编译 C 全文映射）：
// 卖价估值入 VALUE_STATISTIC_FIELD(6)、货值入 (0xd)，材料对入 CCubeStatistic(0x6a)。
#include "DisPatcherCore/statistic_process.inc"

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

// [b28d-29] N78/N82：ORIG rep stos 结构体清零 vs 4.4 call memset（0x28 字节）+ 帧 0x19c vs 0x190；语义等价  (ORIG 0x081ee59c insn 480->487)
#include "DisPatcherCore/dtor_DisPatcher_RentAvatar.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 491（ORIG 480 / ours 487），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] callee 重载修正：m_itemId 本即 unsigned long，多余的 (int) 强转错绑
// _ZN12CStreamGuardlsEi；ORIG 0x081eeb65 实为 lsEm。实测 ndiff 345→342，差集一缺一多清零。
// 残差 5 处 memset→rep stos 属 N78（4.1 内联展开、4.4 O0 恒 call memset，不可翻）。
#include "DisPatcherCore/dispatch_sig_6.inc"
#include "DisPatcherCore/read_22.inc"
#include "DisPatcherCore/process_22.inc"

#include "DisPatcherCore/dtor_DisPatcher_ExtendAvatar.inc"
#include "DisPatcherCore/dispatch_sig_7.inc"
#include "DisPatcherCore/read_23.inc"
#include "DisPatcherCore/process_23.inc"

#include "DisPatcherCore/dtor_DisPatcher_SellItem.inc"
// §6.2 巨件核销：P167 77/77 [0缺0多]；gap 515→515/522；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] P30/N4：ORIG 结构体基址 lea -0x52 + add $0xd/0xe/0x10 两步，4.4 折单 lea；语义集合全等  (ORIG 0x081f2d0c insn 515->522)
// 无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 287（ORIG 515 / ours 522），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_8.inc"
#include "DisPatcherCore/read_24.inc"
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
#include "DisPatcherCore/read_25.inc"
// ORIG 0x081c61ea T _ZN22DisPatcher_RepairEquip4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_RepairEquip/send.md 汇编+反编译 C 全文转写）：
// put_header(1,0x19)；errNo==0 → put_byte(1)/getCurCharacInvenR()->get_money()
// put_int/(char)m_8/(ushort)m_a/(ushort)m_c；否则 0/errNo；finalize(true)+Send。
#include "DisPatcherCore/send_13.inc"
// ORIG 0x081c6082 T _ZN22DisPatcher_RepairEquip7processEP5CUserR8MSG_BASER9ParamBase
// err>0：SendCmdErrorPacket(cmd=0x19, err&0xff) 返回 -1；err<0：LineFunc(0x1500,pretty,0,acc_id)
// 并返回其值；err==0：G_Store()->repair_equip(user, msg[0xd], (s16)msg+0xe, (u16)msg+0x10,
// [b28d-29] N82：帧差（ORIG sub $0x4c+push edi/esi，4.4 sub $0x34）+ 槽号平移，调用点全等  (ORIG 0x081c6082 insn 83->80)
// 出参 param+0xc) 写 param.m_errNo，成功(==0)时回显 m_8=msg[0xd]、m_a=(u16)msg+0xe；返回 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 69（ORIG 83 / ours 80），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_25.inc"


#include "DisPatcherCore/dtor_DisPatcher_SetTradeState.inc"
// ORIG 0x081f6232 T _ZN24DisPatcher_SetTradeState12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetTradeState/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state==3 且 get_byte(state) 成功 → GetTradeSpace(idx) 池取；空 → SendCmdErrorPacket
// (0x1a,1)；is_empty → logFormat(0x82f1) + PutTradeSpace；check_trade_possibility 假 →
// cancel_trade + PutTradeSpace；真 → change_trade_state(user, (char)state)；返回 0。
#include "DisPatcherCore/dispatch_sig_9.inc"
#include "DisPatcherCore/read_26.inc"

// ORIG 0x0823315a W _ZN24DisPatcher_SetTradeState7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部交易状态逻辑（get_state==3 → GetTradeSpace → is_empty/check_trade_possibility →
// change_trade_state / cancel_trade+PutTradeSpace）内嵌于
// dispatch_sig(0x081f6232，报告 docs/class_func_reports/DisPatcher_SetTradeState/dispatch_sig.md)。
#include "DisPatcherCore/process_26.inc"

#include "DisPatcherCore/dtor_DisPatcher_Compound.inc"
#include "DisPatcherCore/dispatch_sig_10.inc"
#include "DisPatcherCore/read_27.inc"
#include "DisPatcherCore/process_27.inc"

#include "DisPatcherCore/dtor_DisPatcher_DisJointItem.inc"
// ORIG 0x081f9fb8 T _ZN23DisPatcher_DisJointItem12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_DisJointItem/dispatch_sig.md 汇编+反编译 C 全文转写）：
// user 空或 CheckInTrade() 真 → return 0；依次 get_short(slot)/get_byte(type)/
// get_short(target,0xffff 初始)，失败分别 LineFunc 0x88d0/0x88d2/0x88d6；
// 全部成功 → _disjoint(user,(int)(short)slot,(int)(char)type,0x1c,0,(ushort)target)。
#include "DisPatcherCore/dispatch_sig_11.inc"
#include "DisPatcherCore/read_28.inc"
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
#include "DisPatcherCore/read_29.inc"

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
#include "DisPatcherCore/read_30.inc"
#include "DisPatcherCore/send_14.inc"

// ORIG 0x081bffbc T _ZN22DisPatcher_ChangeSkill7processEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_ChangeSkill/process.md ## 反编译 C 全文映射）：
// check_error（stub 恒 0）→ param.m_curSlot/m_newSlot ← msg[0xe]/msg[0xf] →
// [b28d-29] N82/P0：msg/param 形参副本落槽（ORIG 0x10/0x14(%ebp) 直用），4.4 帧 0x28 vs ORIG 0x2c+三 push  (ORIG 0x081bffbc insn 74->69)
// SkillSlot::skill_move(cur, new, get_charac_job)；>0 时 SendCmdErrorPacket(0x1e) 返 -1。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 62（ORIG 74 / ours 68），残差=N7/N82 帧与 callee-saved 角色/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_30.inc"

#include "DisPatcherCore/dtor_DisPatcher_BuySkill.inc"
// ORIG 0x081bf8be T _ZN19DisPatcher_BuySkill11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_BuySkill/check_error.md 逐块映射）：user 空 → -1；
// 遍历 msg 技能项（count@+0xe signed char，entries@+0xf stride 3），
// 技能 id ∈ (0xc7,0xd0) 且 ServiceRestrictManager::isRestricted(user,1,0x1e) → 0xd1。
#include "DisPatcherCore/check_error_16.inc"
#include "DisPatcherCore/check_state_3.inc"
// ORIG 0x081bed4a T _ZN19DisPatcher_BuySkill4readER9PacketBufR8MSG_BASE
// [b28d-29] AE：169=169 全等（同上重定位归一）  (ORIG 0x081bed4a insn 169->170)
// 报告：docs/class_func_reports/DisPatcher_BuySkill/read.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 169 / ours 169 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/read_31.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 415（ORIG 311 / ours 302），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/send_15.inc"

// ORIG 0x081befe8 T _ZN19DisPatcher_BuySkill7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_BuySkill/process.md 全文转写）：
//   check_error>0 → SendCmdErrorPacket(0x1f, err) 返 -1；<0 → LineFunc(0x3ef, pretty, err, acc_id)。
//   count==1 单条模式：entry0 → master/refund_skill_process，回写 m_354/m_350。
//   count!=1 批量模式：backupSkill 失败 → m_350=-1、m_354=infos[0].m_0（ORIG 读未初始化，忠实保留）
//   返 0；成功则两遍循环（try_* 试算 → 正式 master/refund），info.m_8<0 短路返 0。
//   四个 helper（T 符号）当前无 TU 定义，见 DisPatcherCore.h 声明。
#include "DisPatcherCore/process_31.inc"

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

// ORIG 0x081bfc5c T _ZN19DisPatcher_BuySkill20refund_skill_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/refund_skill_process.md 全文转写）：
// 无树门/无权限门（treeKind 存 -0x2c 后未再读取）；refundCount 局部清零后取址；
// SkillSlot::refund_skill(idx, level, job, first, second, (s8)m_1, &refundCount)
// [b28d-29] N82：帧/槽重排（ORIG sub $0x4c+三 push，4.4 sub $0x58），调用点全等  (ORIG 0x081bfc5c insn 73->72)
// 写 m_4，≥0 时 m_8←当前等级。ORIG 不读回 refundCount 值。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 66（ORIG 73 / ours 71），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/refund_skill_process.inc"

// ORIG 0x081bfd3c T _ZN19DisPatcher_BuySkill25try_refund_skills_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/try_refund_skills_process.md 全文转写）：
// [b28d-29] N82：帧/槽重排（ORIG sub $0x4c+push edi/esi/ebx，4.4 sub $0x58），助记符序列与调用点全等  (ORIG 0x081bfd3c insn 59->55)
// 与 refund 同参形态调 try_refund_skill；仅写 m_4，无等级回读、无任何门。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 58（ORIG 59 / ours 55），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/try_refund_skills_process.inc"

// ORIG 0x081bfdf0 T _ZN19DisPatcher_BuySkill29try_master_new_skills_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/try_master_new_skills_process.md 全文转写）：
// 公会技能 [200,208] 直接返回（cmp $0xd0 jbe → 尾声；与正式 master 相反）；
// [b28d-29] N82/N18b：帧/槽重排 + 比较方向折痕，调用点全等  (ORIG 0x081bfdf0 insn 84->79)
// 非公会走同一树门后调 try_master_new_skills，仅写 m_4，无等级回写、无日志尾部。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 51（ORIG 81 / ours 76），残差=N7/N82 帧与 callee-saved 角色/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/try_master_new_skills_process.inc"

#include "DisPatcherCore/dtor_DisPatcher_IncreStatus.inc"
// ORIG 0x081ede34 T _ZN22DisPatcher_IncreStatus12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_IncreStatus/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=2 → 0；get_short 失败 → LineFunc 0x70fc；GetItemPtr(user,0,slot)==0 →
// LineFunc 0x7101；CheckItem 0x2a 非0 → SendCmdErrorPacket(0x20,err)+0；
// [b28d-29] N82：帧 0x50 vs 0x40 + 槽平移（ORIG 结构体基址 -0x2f+add 两步，4.4 折 lea），语义集合全等  (ORIG 0x081ede34 insn 123->121)
// slot<0x99 → increase_status(slot)，否则 UseQuestItem(slot)+cMyTrace(0x710f,5)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 52（ORIG 123 / ours 121），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_13.inc"
#include "DisPatcherCore/read_32.inc"

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
#include "DisPatcherCore/read_33.inc"

// ORIG 0x08232d24 W _ZN22DisPatcher_QuestAction7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部任务动作逻辑（PvPSkilTree 频道拒绝/at(1) 命令码 0x21..0x24 分派 → find_quest/
// setBlindTimeGateStateQuest/quest_action 四参/GM quest flag）内嵌于
// dispatch_sig(0x081ee010，报告 docs/class_func_reports/DisPatcher_QuestAction/dispatch_sig.md)。
#include "DisPatcherCore/process_33.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPos.inc"
#include "DisPatcherCore/check_error_17.inc"
#include "DisPatcherCore/read_34.inc"
// ORIG 0x081c98c8 T _ZN17DisPatcher_SetPos7processEP5CUserR8MSG_BASER9ParamBase
#include "DisPatcherCore/process_34.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetArea.inc"
// ORIG 0x081c93e8 T _ZN18DisPatcher_SetArea11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_SetArea/check_error.md 汇编+反编译 C 全文转写）：
// state!=3 → 0x7fffffff；IsBusyDisjointStore → param.m_8=req.m_0d, 0xbf；
// IsBusyPrivateStore → param.m_8=req.m_0d, 0x3c；IsCurCharacGhost → param.m_8=req.m_0d,
// 0x13；IsForbiddenMove(name) 且剩余时间!=0 → param.m_9=req.m_0e, 0x44；时间==0 则
// EnableMove(name)；否则 0。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
#include "DisPatcherCore/check_error_18.inc"
// ORIG 0x081c91e6 T _ZN18DisPatcher_SetArea4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_SetArea/read.md（汇编+反编译 C 全读）
#include "DisPatcherCore/read_35.inc"
// ORIG 0x081c9656 T _ZN18DisPatcher_SetArea4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_SetArea/send.md 汇编+反编译 C 全文转写）：
// m_errNo!=0x7fffffff 且 !=0 时发包：put_header(1,0x26)/0/errNo；errNo==0x44 →
// put_byte((char)m_9)+GetLeftTimeOfForbiddenMove(getCurCharacName())/60000 put_int；
// 否则 put_byte((char)m_8)；finalize(true)+Send。
#include "DisPatcherCore/send_16.inc"
// ORIG 0x081c951c T _ZN18DisPatcher_SetArea7processEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 94 / ours 94 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_35.inc"

#include "DisPatcherCore/dtor_DisPatcher_Fish.inc"
#include "DisPatcherCore/read_36.inc"

// ORIG 0x08232cae W _ZN15DisPatcher_Fish7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
// —— 原始二进制即为空实现（无 process 报告，W 内联弱符号）。
#include "DisPatcherCore/process_36.inc"
// ORIG 0x081eddf6 T _ZN15DisPatcher_Fish12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_Fish/dispatch_sig.md 全文转写）：
//   状态非 4（钓鱼中）直接返 0；为 4 时调 GameWorld::fishing，恒返 0。
#include "DisPatcherCore/dispatch_sig_15.inc"

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
// ORIG 0x0826503c T _ZN21DisPatcher_FinishLoad7processEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 113 / ours 113 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_37.inc"
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
// ORIG T 0x08264b66 _ZN21DisPatcher_FinishLoad4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_FinishLoad/read.md ## 汇编 + ## 反编译 C
// 全文转写）。两次 PacketBuf::get_int 分别写 m_0d/m_11（msg+0x0d/+0x11 具名
// 字段）；任一失败即以 LineFunc(0x5f7/0x5f8) 返回（第三、四参恒 0），全成返回 0。
#include "DisPatcherCore/read_37.inc"

#include "DisPatcherCore/dtor_DisPatcher_UseSkill.inc"
// ORIG 0x081cad80 T _ZN19DisPatcher_UseSkill11check_errorEP5CUserR8MSG_BASER9ParamBase
// get_state(user)==5 → 0；否则 0x7fffffff（报告 DisPatcher_UseSkill/check_error.md 全文）。
// [b28d-solo-DisPatcherCore] P211 callee 侧旋钮（同 SetPartyInfo）。
#include "DisPatcherCore/check_error_20.inc"
#include "DisPatcherCore/read_38.inc"
#include "DisPatcherCore/send_17.inc"

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

// ORIG 0x08286612：get_second_growtype_max_level(first, second) > 0
#include "DisPatcherCore/IsSkillMatchSecondGrowType.inc"

// ORIG 0x08286656：恒返 0x339
#include "DisPatcherCore/GetHackType.inc"

// ORIG 0x082864fc：find_skill(job, slot)==0 → false；
// 首一转类型命中 / 二转上限>0 / IsStealable(first, second) 任一成立 → true。
#include "DisPatcherCore/IsValidUsingSkill.inc"

// ORIG 0x081cadaa T _ZN19DisPatcher_UseSkill7processEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseSkill/process.md ## 反编译 C 全文映射）：
// param.m_errNo=check_error；>=1 直接返回 0；<0 LineFunc(0x1ff2,err,acc_id)；
// ==0 时构造校验信息 → 非法使用记黑客计数（addServerHackCnt(type,1,slot,0)），
// 有队伍则 CParty::use_skill(user, slot, target)，恒返 0。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 112 / ours 112 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_38.inc"

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

#include "DisPatcherCore/read_39.inc"
#include "DisPatcherCore/process_39.inc"

#include "DisPatcherCore/dtor_DisPatcher_DieCharacInDungeon.inc"
#include "DisPatcherCore/check_error_21.inc"
#include "DisPatcherCore/check_state_5.inc"
#include "DisPatcherCore/read_40.inc"
#include "DisPatcherCore/send_18.inc"

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

#include "DisPatcherCore/dtor_DisPatcher_UseCoin.inc"
// ORIG 0x081ca7d6 T _ZN18DisPatcher_UseCoin11check_errorEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseCoin/check_error.md 逐块映射）：user->get_state()==5 →
// Dungeon_check_error；==8 → WarRoom_check_error；其余 → 0x13。
#include "DisPatcherCore/check_error_22.inc"
// ORIG 0x081ca5e4 T _ZN18DisPatcher_UseCoin4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_UseCoin/read.md 逐块映射）：get_short → msg+0xd；
// 失败 LineFunc(0x1ebb, PRETTY, 0, 0)。
#include "DisPatcherCore/read_41.inc"
#include "DisPatcherCore/send_19.inc"
// ORIG 0x081ca852 T _ZN18DisPatcher_UseCoin7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo = check_error(user,msg,param)（三参）；>=1 返回 0；<0 LineFunc(0x1f4d,pretty,err,acc_id)
// 并返回其值；==0：m_8(u16)=msg.m_0d；state==5：GetParty 判空 LineFunc(0x1f5a,...,0,acc_id)，
// checkFreeRevivalCondition(0) 为假时 UseCoin(eCoinSubReason 0) + set_authen_data(3,1,0) +
// party->IncreaseUsedCoinCount + APSystem ClearActionAndSendtoUser(0x25)；state==8：
// [b28d-29] N82/P211：帧 0x28 vs 0x38 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081ca852 insn 122->125)
// UseCoin(eCoinSubReason 4) + set_authen_data(3,1,0) + user->IncreaseUsedCoinCount；返回 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 47（ORIG 121 / ours 126），残差=N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_41.inc"


#include "DisPatcherCore/dtor_DisPatcher_GiveUpGame.inc"
// ORIG 0x081c41e8 T _ZN21DisPatcher_GiveUpGame11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_GiveUpGame/check_error.md 逐块映射）：state ∈
// {5,10,12,13} → 0；其余 → 0x13（ORIG 每次比较前调 get_state，勿缓存）。
#include "DisPatcherCore/check_error_23.inc"
#include "DisPatcherCore/read_42.inc"

// ORIG 0x081c40b4 T _ZN21DisPatcher_GiveUpGame7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_GiveUpGame/process.md 全文转写）。
// err>0 返 -1；<0 LineFunc(0xff7,pretty,0,acc_id) 返回其值；==0 按 state 分派：
// 10=getDeathTower 非空 handleLeaveUser；0xc=CBossTowerFunc::GiveUpGame；
// 0xd=advancealtar::Manager::giveUpGame；其它=GetParty 非空 giveup_game(user,true,false,false)。
// 尾部返回 check_error 原值（ORIG mov -0x14,%eax）。param 不写。
#include "DisPatcherCore/process_42.inc"

#include "DisPatcherCore/dtor_DisPatcher_GetItem.inc"
#include "DisPatcherCore/check_error_24.inc"
// ORIG 0x081c335c T _ZN18DisPatcher_GetItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_GetItem/read.md（汇编+反编译 C 全读）
#include "DisPatcherCore/read_43.inc"
// ORIG 0x081c3ce8 T _ZN18DisPatcher_GetItem4sendEP5CUserR9ParamBase
// （报告 DisPatcher_GetItem/send.md 逐块映射）：param.m_errNo(+0x4)
// !=0x7fffffff；==0 → SendCmdOkPacket(0x2e)；否则 SendCmdErrorPacket(0x2e, err&0xff)。
#include "DisPatcherCore/send_20.inc"
// ORIG T 0x0889209d（定义 OnlyOrigBatch11.cpp:141）：奇偶门，返回 1=通过 0=失败。
// [b28d-29] N82/P211：帧 0x50 vs 0x60 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081c35c0 insn 484->511)
#include "DisPatcherCore/checkGetItemPacket.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 355（ORIG 481 / ours 512），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_43.inc"


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
#include "DisPatcherCore/read_44.inc"
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
#include "DisPatcherCore/check_state_6.inc"
#include "DisPatcherCore/read_45.inc"
// ORIG 0x081c4b7a T _ZN18DisPatcher_MoveMap20CheckTrapBitsHackCntEP5CUserR12MSG_MOVE_MAP
// msg.m_18 ushort 非 7 的倍数 → 记 0x1f9；累计 >4 时对队伍其余成员记 0x1fa、
// [b28d-29] N-P223/N-JMP：唯一残差=cond 尾 4.4 多保留 jmp 岛（ORIG 0x081c4bb5 je 直落 epilogue，4.1 线程化折掉），ndiff 20->1  (ORIG 0x081c4b7a insn 108->111)
// 落库 DB_ReqSaveHackUserPunish 并 DisConnSig(0x18,1,0) 强制断线。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 1（ORIG 107 / ours 108），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/CheckTrapBitsHackCnt.inc"
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
// ORIG 0x081c5330 T _ZN18DisPatcher_MoveMap7processEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 120 / ours 120 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_45.inc"

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
#include "DisPatcherCore/read_46.inc"
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
// ORIG 0x081c2bee T _ZN19DisPatcher_DropItem4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_DropItem/read.md 汇编+反编译 C 全文转写）：
// 依次 get_short(+0x0d)/get_short(+0x0f)/get_byte(char&)(+0x11)/get_short(+0x12)/
// get_int(+0x14)/get_byte(char&)(+0x18)；失败分别 LineFunc 0xd0c..0xd12；成功返回 0。
#include "DisPatcherCore/read_47.inc"
#include "DisPatcherCore/send_21.inc"
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


#include "DisPatcherCore/dtor_DisPatcher_UseEquip.inc"
#include "DisPatcherCore/check_error_27.inc"
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 21 / ours 21 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/read_48.inc"
#include "DisPatcherCore/send_22.inc"
// ORIG 0x081c0e4a T _ZN19DisPatcher_UseEquip7processEP5CUserR8MSG_BASER9ParamBase
// err=check_error；>0：SendCmdErrorPacket(0x33, err&0xff) 返回 -1；==0：
// m_errNo=getCurCharacInvenW()->use_equip((int)(sbyte)msg.m_0d)，m_8=msg.m_0d，
// 返回 err。
#include "DisPatcherCore/process_48.inc"


#include "DisPatcherCore/dtor_DisPatcher_MakePVP.inc"
#include "DisPatcherCore/ExtractPacket.inc"
// [b28d-29] N82/P183：临时对象构造块（lea -0x10）位置前移 3 指令，difflib 块序伪差；语义集合全等  (ORIG 0x081f803e insn 103->104)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 40（ORIG 103 / ours 105），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_19.inc"
#include "DisPatcherCore/read_49.inc"

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
#include "DisPatcherCore/read_50.inc"
#include "DisPatcherCore/process_51.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPReady.inc"
#include "DisPatcherCore/check_error_28.inc"
#include "DisPatcherCore/read_51.inc"
#include "DisPatcherCore/send_23.inc"
// ORIG 0x081c9a14 T _ZN22DisPatcher_SetPVPReady7processEP5CUserR8MSG_BASER9ParamBase
#include "DisPatcherCore/process_52.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPTeam.inc"
// ORIG 0x081f6020 T _ZN21DisPatcher_SetPVPTeam12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] P30/N4：ORIG 结构体基址 lea -0x2c + add $0xd 两步，4.4 折单 lea -0x21；槽号平移随 N82  (ORIG 0x081f6020 insn 137->139)
// 报告：docs/class_func_reports/DisPatcher_SetPVPTeam/dispatch_sig.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 75（ORIG 137 / ours 138），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_22.inc"
#include "DisPatcherCore/read_52.inc"
#include "DisPatcherCore/process_53.inc"

#include "DisPatcherCore/dtor_DisPatcher_DiePvP.inc"
// ORIG 0x081ece64 T _ZN17DisPatcher_DiePvP12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] N82：帧 0x6c vs 0x60 + 槽平移（ushort 数组 -0x36 vs -0x26），调用点全等  (ORIG 0x081ece64 insn 146->147)
// 报告：docs/class_func_reports/DisPatcher_DiePvP/dispatch_sig.md（汇编+反编译 C 全读）
// [Lane5] P227 已落（gap 146→146=0）：getter 内联进 CSwitchLog 构造即调用单表达式，
// trace ctor 提到 getter 之前的问题消除。残余=整帧槽深 +4（-0x36 vs -0x3a 等）+
// 帧 0x6c vs 0x5c（N82/N115 族），ndiff ~60。
#include "DisPatcherCore/dispatch_sig_23.inc"
#include "DisPatcherCore/read_53.inc"
#include "DisPatcherCore/process_54.inc"

#include "DisPatcherCore/dtor_DisPatcher_PvPTimeout.inc"
#include "DisPatcherCore/check_error_29.inc"
// ORIG 0x081c5d78 T _ZN21DisPatcher_PvPTimeout4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_PvPTimeout/read.md 逐块映射）：循环 i=0..7 共 8 次
// get_int → msg+0xd+i*4；失败 LineFunc(0x14a6, PRETTY, 0, 0)。
#include "DisPatcherCore/read_54.inc"
#include "DisPatcherCore/send_24.inc"
// ORIG 0x081c5dec T _ZN21DisPatcher_PvPTimeout7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_PvPTimeout/process.md 全文转写）
// err=check_error（ORIG 0x081c5ebe：get_state!=7 → 0x13）；>0 返回 -1；<0
// LineFunc(0x14b7,pretty,0,acc_id)；==0：room=user->GetPVPRoom()（空则
// LineFunc(0x14bc,pretty,0,acc_id)），否则 ret=room->time_out((int*)(msg+0xd))
// —— msg+0xd 即 read() 读入的 8 个 seat int 数组首址 —— param+0x4=ret，返回 err。
#include "DisPatcherCore/process_55.inc"


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

#include "DisPatcherCore/read_55.inc"
#include "DisPatcherCore/process_56.inc"

// [b28d-29] P183 反向 + N93：ORIG 缓存 &msg+0xd 到 -0xc 后 disp 访问，4.4 逐次 mov 0xc(%ebp)+add；两侧 store 偏移集合 {0x25..0x4d} 完全一致（语义等价）  (ORIG 0x081f039e insn 172->179)
#include "DisPatcherCore/dtor_DisPatcher_ResPvpRank.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 209（ORIG 172 / ours 179），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/ExtractPacket_1.inc"
// [b28d-29] N82/P197③：ORIG 两 dead 局部预清 + 槽序（-0x1c/-0x18/-0x10/-0xc），4.4 合并复用致全帧平移；语义集合全等  (ORIG 0x081f05ea insn 197->197)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 72（ORIG 197 / ours 197），残差=setcc-fold bool 物化岛/P168a④ bool 槽化；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/SavePvpRank.inc"

#include "DisPatcherCore/dispatch_sig_25.inc"
#include "DisPatcherCore/read_56.inc"
// ORIG 0x08232e98 W _ZN21DisPatcher_ResPvpRank7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现（objdump 核实），全部逻辑位于 dispatch_sig(0x081f0282)：state==7 → MSG_RES_PVP_RANK ctor → ExtractPacket(0x081f039e)/SavePvpRank(0x081f05ea) + LineFunc 断言
#include "DisPatcherCore/process_57.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPMapIndex.inc"
// ORIG 0x081f8602 T _ZN25DisPatcher_SetPVPMapIndex12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] N82 + setcc-fold：帧 0x40 vs 0x30；`!flag` 已按 P168a 修正（xor 形），残余为槽平移与 4.4 sete 折痕  (ORIG 0x081f8602 insn 175->179)
// 报告：docs/class_func_reports/DisPatcher_SetPVPMapIndex/dispatch_sig.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 59（ORIG 175 / ours 178），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_26.inc"
#include "DisPatcherCore/read_57.inc"
#include "DisPatcherCore/process_58.inc"

#include "DisPatcherCore/dtor_DisPatcher_DebugCommand.inc"
#include "DisPatcherCore/read_58.inc"

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
#include "DisPatcherCore/read_59.inc"

#include "DisPatcherCore/send_25.inc"
// ORIG 0x081cb2e2 T _ZN20DisPatcher_AddFriend7processEP5CUserR8MSG_BASER9ParamBase：
// 薄包装，尾调 process_packet_jpn 并透传返回值（汇编 leave/ret 直传 eax）。
#include "DisPatcherCore/process_60.inc"

// ORIG 0x081cb30a T _ZN20DisPatcher_AddFriend18process_packet_jpnEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_AddFriend/process_packet_jpn.md 全文转写）。
// m_field4=check_error；>0 返 0；<0 logFormat(1,file,pretty,0x20d8,
// "Dispatcher_Impl Check_Error No(%d)",err) 返 0x20d9；==0：StreamPool 取流构造
// CStreamGuard → <<0x33e <<GetUID → GetInBuffer<SIG_CHECK_ADD_FRIEND> 判空置 m_field4=1；
// [b28d-29] N82/P211：store 装载序 + 临时对象块位置（P183 反向），调用点全等  (ORIG 0x081cb30a insn 126->125)
// 否则 sig[+4]=!isGM(acc_id)、strncpy(sig+5, msg+0x11, 0x1e)、MsgQueueMgr put(DB_Q)；返 0。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 126 / ours 126 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_packet_jpn.inc"

#include "DisPatcherCore/dtor_DisPatcher_RemoveFriend.inc"
#include "DisPatcherCore/check_error_31.inc"
#include "DisPatcherCore/read_60.inc"
#include "DisPatcherCore/send_26.inc"
// ORIG 0x081cb618 T _ZN23DisPatcher_RemoveFriend7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_RemoveFriend/process.md 全文转写）。
// m_errNo=check_error；>0 返 0；<0 LineFunc(0x2141,pretty,err,acc_id) 返回其值；
// ==0：组 Packet_Monitor_Del_Buddy（ctor 0x0822bcb2 W：PacketHeader(0x674,0x30)+memset(m_name)），
// 填 m_charNo=getCurCharacNo、m_idByChannel=GetUID、memcpy(m_name, msg+0x11, strlen)，
// [b28d-29] N82/P211：参数副本槽 + store 装载序（ORIG RHS-first mov %eax,%edx 中转，4.4 LHS-first）  (ORIG 0x081cb618 insn 85->81)
// 经 s_monitor_proxy_mgr GetServerProxy(GetServerGroup())->SendPacket(&pkt, packetSize)，返 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 44（ORIG 85 / ours 81），残差=N7/N82 帧与 callee-saved 角色/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_61.inc"

#include "DisPatcherCore/dtor_DisPatcher_Cera.inc"
#include "DisPatcherCore/dispatch_sig_27.inc"
#include "DisPatcherCore/read_61.inc"
#include "DisPatcherCore/process_62.inc"

#include "DisPatcherCore/dtor_DisPatcher_EnterWarRoom.inc"
#include "DisPatcherCore/dispatch_sig_28.inc"
#include "DisPatcherCore/read_62.inc"
#include "DisPatcherCore/process_63.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetWarRoomSeatState.inc"
#include "DisPatcherCore/dispatch_sig_29.inc"
#include "DisPatcherCore/read_63.inc"
#include "DisPatcherCore/process_64.inc"

#include "DisPatcherCore/dtor_DisPatcher_DieWarRoomCharacter.inc"
#include "DisPatcherCore/dispatch_sig_30.inc"
#include "DisPatcherCore/read_64.inc"
#include "DisPatcherCore/process_65.inc"

#include "DisPatcherCore/dtor_DisPatcher_StartWarRoomTemp.inc"
#include "DisPatcherCore/dispatch_sig_31.inc"
#include "DisPatcherCore/read_65.inc"
#include "DisPatcherCore/process_66.inc"

#include "DisPatcherCore/dtor_DisPatcher_ReportClient4Hack.inc"
// ORIG 0x082041a0 T _ZN28DisPatcher_ReportClient4Hack12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_ReportClient4Hack/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=1 或 getCurCharacR()==0 → return 0。state==6 → 仅 analyzeHackReport。
// state==5 组队校验（GetParty==0 → LineFunc 0x9bb7）；state==7 PVP 校验
// （GetPvp==0 → LineFunc 0x9bc1）；state==8 战场校验（GetWarRoom==0 → LineFunc 0x9bc5）；
// 校验通过同 6 → analyzeHackReport；其余 state → beginCollect/analyze/report/reset 全链。
#include "DisPatcherCore/dispatch_sig_32.inc"
#include "DisPatcherCore/read_66.inc"
#include "DisPatcherCore/process_67.inc"

// ---------------------------------------------------------------------------
// ST_Guild_War_Rank_Info / Packet_Notice_Guild_War_Info 生命周期（StubS3C）。
// ST ctor：ORIG W 0x0822ba9a —— uint(+0)=0、uint(+4)=0、ushort(+8)=0、
// memset(+0xa,0,0x17)。Packet_Notice ctor：ORIG W 0x0822badc ——
// PacketHeader(0x414,0x15c) 后对 +0x12 起 9 个元素（stride 0x21）逐个构造。
// ---------------------------------------------------------------------------
#include "DisPatcherCore/ST_Guild_War_Rank_Info.inc"
// [b28d-solo-DisPatcherCore] 修复：m_rankInfo 由 char[0x14a] 改真实元素数组
// ST_Guild_War_Rank_Info[10]（头 R2 广播），ctor 删手写 placement-new 循环
// ⇒ 编译器隐式数组成员构造复现 ORIG P151 倒计数循环（esi=9→-1、ebx 步进 0x21，
// 无 _ZnwjPv/size 装载）；N83 的 placement-new 多 2 条消除。
#include "DisPatcherCore/Packet_Notice_Guild_War_Info.inc"

#include "DisPatcherCore/dtor_DisPatcher_RequestGuildWarInfo.inc"
#include "DisPatcherCore/dispatch_sig_33.inc"
#include "DisPatcherCore/read_67.inc"
#include "DisPatcherCore/process_68.inc"

#include "DisPatcherCore/dtor_DisPatcher_PvpHeartBeat.inc"
#include "DisPatcherCore/read_68.inc"
#include "DisPatcherCore/dispatch_sig_34.inc"
#include "DisPatcherCore/process_69.inc"

#include "DisPatcherCore/dtor_DisPatcher_MultiBoxLottery.inc"
// ORIG 0x0820b802 T _ZN26DisPatcher_MultiBoxLottery12dispatch_sigEP5CUserR9PacketBuf
// 报告：docs/class_func_reports/DisPatcher_MultiBoxLottery/dispatch_sig.md（汇编+反编译 C 全读）
#include "DisPatcherCore/dispatch_sig_35.inc"
#include "DisPatcherCore/read_69.inc"
#include "DisPatcherCore/process_70.inc"

// [b28d-29] P251/P208：ORIG 0x0821ac7c 为 switch(local_e) 稀疏树（ushort 入口截断槽 -0xa 双段）。
// [Lane38 2026-09-10] 已按 switch 复现（case 0/1/2 物理序 = ORIG，fall-through = case 0）：
//   jcc 目标差由 −106/+134/+133 收敛为单常数 −3（结构对齐），228→233（残 N82 帧 0x58 vs 0x44）。
#include "DisPatcherCore/dtor_DisPatcher_UdpCharacteristic.inc"
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 345（ORIG 228 / ours 233），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_36.inc"
#include "DisPatcherCore/read_70.inc"
#include "DisPatcherCore/process_71.inc"

#include "DisPatcherCore/dtor_DIspatcher_OneDayLetheTicket.inc"
#include "DisPatcherCore/dispatch_sig_37.inc"
#include "DisPatcherCore/read_71.inc"
#include "DisPatcherCore/process_72.inc"

#include "DisPatcherCore/dtor_Dispatcer_polic_one_to_one_chat_disable.inc"
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 65 / ours 65 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/dispatch_sig_38.inc"
#include "DisPatcherCore/read_72.inc"
#include "DisPatcherCore/process_73.inc"

#include "DisPatcherCore/dtor_DisPatcher_CheckSecurityProtection.inc"
#include "DisPatcherCore/check_error_32.inc"
#include "DisPatcherCore/read_73.inc"
#include "DisPatcherCore/send_27.inc"

// ORIG 0x0826481a T _ZN34DisPatcher_CheckSecurityProtection7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_CheckSecurityProtection/process.md 全文转写）：
// sub $0x28 帧；将 CSecu_ProtectionField::Check(GlobalData::s_pSecuProtectionField,user,0x38)
// 返回值低字节写入 param_3[4]（CheckSecurityProtection 首数据字节 @+0x4 = m_pad[0]），恒返回 0。
#include "DisPatcherCore/process_74.inc"

#include "DisPatcherCore/dtor_DisPatcher_EventDungeon_DestoryObject.inc"
#include "DisPatcherCore/check_error_33.inc"
#include "DisPatcherCore/read_74.inc"
#include "DisPatcherCore/send_28.inc"
// ORIG 0x0812bd8c T _ZN37DisPatcher_EventDungeon_DestoryObject7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_DestoryObject/process.md 全文转写：
//   局部引用槽 -0x8=msg、-0x4=param；*(u16*)(param+0x4)=*(u16*)(msg+0xe)；check_error；返回 0。
//   模板实例化中 ParamBase 栈对象仅 4 字节 vptr（ORIG dispatch_template 0x824aa8c 实证：
//   param@-0x1d 与 msg@-0x19 相邻），param+0x4 写入与相邻 msg 存储重叠为 ORIG 原始行为，忠实转写。）
#include "DisPatcherCore/process_75.inc"

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

// ORIG 0x0812beba T _ZN33DisPatcher_EventDungeon_ClearRoom7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/process.md 全文转写，
//   含汇编写入次序：param+0x4←msg+0xe、param+0x8←msg+0x12、param+0x6←msg+0x10）；check_error 恒返 0。
#include "DisPatcherCore/process_76.inc"

// ORIG 0x0812be82 T _ZN33DisPatcher_EventDungeon_ClearRoom4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/read.md 全文转写）：
// ORIG 汇编含 &msg 的局部落栈（-0x4c(%ebp) spill/reload 对），以具名局部复现。
#include "DisPatcherCore/read_75.inc"

// ORIG 0x0812bf16 T _ZN33DisPatcher_EventDungeon_ClearRoom4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/send.md 全文转写）：
// put_header(1,0x21c) → put_byte(1) → put_binary(pParam,0xc) → finalize(true) →
// CUser::Send；PacketGuard 栈对象析构（ORIG 含 EH 清理路径）。
// 反编译 C 首行 local_10=param_2：param 引用先落栈（-0xc），guard 位于 -0x18。
#include "DisPatcherCore/send_29.inc"

#include "DisPatcherCore/dtor_Dispather_RegenerationRandomOption.inc"
#include "DisPatcherCore/check_state_7.inc"
#include "DisPatcherCore/read_76.inc"
#include "DisPatcherCore/send_30.inc"

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
}  // namespace statistc_proxy



#include "DisPatcherCore/dtor_DispatcherResetRandomOption.inc"
#include "DisPatcherCore/read_77.inc"

// ORIG 0x08682d3a T _ZN15cUserHistoryLog12RandomOptionERK10Inven_ItemN18randomOptionReason1TE
// （docs/class_func_reports/cUserHistoryLog/RandomOption.md + objdump 全文转写）：
// m_user->LogHistory("RandomOptionItem,%d,%02X×14,%d", item.m_addInfo,
// [b28d-29] N82/P183：ORIG 无逐字段临时缓存，4.4 新增 -0x50..-0x48 三槽（帧 0x9c vs 0xac）；语义集合全等  (ORIG 0x08682d3a insn 102->119)
//   item.m_random.m_random 的 14 字节（+0x25..+0x32）逐字节, reason)。无 m_user 空检查。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 124（ORIG 102 / ours 118），残差=N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RandomOption.inc"

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

// ============================================================================
// ORIG 0x0865798a T _ZN5CUser15serviceOfShusiaEv —— 休沙疲劳恢复服务。
// 幽灵 0x13；疲劳>99 0x12；Restrict(1,0x16) 0xd1；余额<价 0x16；
// 扣费 use_money(price,0xa,true)、AddValueStatistic(0x10,user,price)、
// setCurCharacStamina(100)、UseShusiaService((short)m_level)，成功返 0。
// 定义宿主为本 TU（CUser.cpp 由 RestoreCUserStatMethods 独占，经广播协调）。
// ============================================================================
#include "DisPatcherCore/serviceOfShusia.inc"

// ============================================================================
// G2 角色域三个 CUser 叶子方法（定义宿主为本 TU，沿用 serviceOfShusia 先例；
// CUser.cpp 由 RestoreCUserStatMethods 独占，声明已按广播加入 CUser.h）。
// ============================================================================

// ORIG 0x0822fc06 W _ZN5CUser29SetLoadRentalInfoFromExchangeEb：写 +0x6ef91 字节
#include "DisPatcherCore/SetLoadRentalInfoFromExchange.inc"

// ORIG 0x0868e672 T _ZN5CUser25RemoveCharacMercenaryInfoEj：转调雇佣兵管理器
#include "DisPatcherCore/RemoveCharacMercenaryInfo.inc"

// ORIG 0x0864a956 T _ZN5CUser31GetGuildIdBeforeDeleteCharacterEi：
// 返回 m_characList[slot].m_guildId（_Charac_info +0xdbe）
#include "DisPatcherCore/GetGuildIdBeforeDeleteCharacter.inc"


// ORIG 0x08230844 W _ZN5CUser31SetDeleteEventDungeonInvitationEb：
// 写 m_deleteEventDungeonInvitation 字节（objdump 逐指令转写；SelectDungeon::process 对全队调用）。
#include "DisPatcherCore/SetDeleteEventDungeonInvitation.inc"


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
// [b28d-29] N82/P183：帧 0x1b4 vs 0x1b8 + ORIG 无 -0x20 指针缓存而 4.4 新增，槽平移；语义集合全等  (ORIG 0x08217046 insn 418->368)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 239（ORIG 418 / ours 367），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_44.inc"


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
