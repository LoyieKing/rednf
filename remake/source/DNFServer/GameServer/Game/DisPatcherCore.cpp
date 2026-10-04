// ============================================================================
#include <stdio.h>
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

#include "DisPatcherCore/EnterPVPRoom_1.inc"
#include "DisPatcherCore/checkSpecialChar.inc"
#include "DisPatcherCore/checkSpecialCharName.inc"
//   ^ ORIG T 0x0829f01f _Z20checkSpecialCharPc（同上）

// MSG_RES_PVP_RANK（ORIG C1 0x08235378/D1 0x0823538e：仅构造/析构 +0xd PvpResultType，
// sizeof 0x91 实证）。wave1 最小建模（当前全仓无定义；待迁 PacketTypes.h 权威区）。
#pragma pack(push, 1)
struct MSG_RES_PVP_RANK : MSG_BASE
{
    MSG_RES_PVP_RANK() {}
    ~MSG_RES_PVP_RANK() {}
    PvpResultType m_result;  // +0x0d..0x90
};                           // sizeof = 0x91
#pragma pack(pop)


// MSG_MAKE_PVP_ROOM（ORIG 真实类型 _ZTV/_ZTI 域存在；dispatch_sig memset 0x3d 实证）。
// wave1 最小建模（当前全仓无定义；待迁 PacketTypes.h 权威区）：
// +0x0d char 门类型（<0 失败）、+0x0e char 房名[0x1e]、+0x2c short、+0x2e byte 模式门、
// +0x2f int 密码长（<=8）、+0x33 char 密码/校验缓冲、+0x3c byte battleMode。
// （int m_field2f 需 pack(1) 才落在 +0x2f；ORIG ExtractPacket 0x081f81ba 实证。）

// CParty +0xb24 内嵌 CBattle_Field 子对象局部视图（CParty.h 以
// m_battleField[0x158] 等 pad 占位、未引入 CBattle_Field 类型；Main 裁定
// 本组 TU 局部视图，与 CParty.cpp 同口径）。布局：pad 0xb24 + BF 0xcd0。

// WongWork::CDeathTower 私有区局部视图（m_timerKeys[10]@+0xb3c 为 private，
// CDeathTower.h:254；ORIG 跨类直访该区——本 TU 按 P1 组件视图具名取址，
// 与 CPartyBattleFieldView 同口径）。

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
struct STRentAvatarReq : MSG_BASE
{
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
struct STFriendReqView : MSG_BASE
{
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
struct IncreStatusSigView : MSG_BASE
{
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
struct MoveMapMsgView : MSG_BASE
{
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
struct STMultiBoxLotteryReq : MSG_BASE
{
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
#include "DisPatcherCore/dispatch_sig_1.inc"
#include "DisPatcherCore/read_2.inc"
#include "DisPatcherCore/process_1.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetIP.inc"
#include "DisPatcherCore/read_3.inc"
#include "DisPatcherCore/process_2.inc"

#include "DisPatcherCore/dtor_DisPatcher_Exit.inc"
#include "DisPatcherCore/dispatch_sig_2.inc"
#include "DisPatcherCore/read_4.inc"
#include "DisPatcherCore/process_3.inc"

#include "DisPatcherCore/dtor_DisPatcher_SelectCharac.inc"
#include "DisPatcherCore/check_error.inc"

#include "DisPatcherCore/checkPvpEnterCharacter.inc"

#include "DisPatcherCore/usedCoinDungeonCheck.inc"

#include "DisPatcherCore/Dungeon_check_error.inc"

#include "DisPatcherCore/WarRoom_check_error.inc"
#include "DisPatcherCore/partyPingStat.inc"

#include "DisPatcherCore/pvpPingStat.inc"

#include "DisPatcherCore/fairPvpPingStat.inc"
#include "DisPatcherCore/read_5.inc"
#include "DisPatcherCore/send.inc"

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

#include "DisPatcherCore/RequestDBCharac.inc"
#include "DisPatcherCore/RequestDBTicket.inc"

#include "DisPatcherCore/RegistTimerSchedules.inc"

#include "DisPatcherCore/RequestBlackCount.inc"

#include "DisPatcherCore/RequestDBInventory.inc"

#include "DisPatcherCore/RequestDBInventory_forchina.inc"

#include "DisPatcherCore/RequestDBQuest.inc"

#include "DisPatcherCore/RequestDBDungeonClear.inc"

#include "DisPatcherCore/RequestDBKillMonsterInfo.inc"

#include "DisPatcherCore/RequestDBETC.inc"


#include "DisPatcherCore/RequestDBSkill.inc"

#include "DisPatcherCore/process_5.inc"

#include "DisPatcherCore/dtor_DisPatcher_CreateCharac.inc"
#include "DisPatcherCore/check_error_1.inc"
#include "DisPatcherCore/check_state.inc"
// ORIG 0x081c01ec T _ZN23DisPatcher_CreateCharac4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_CreateCharac/read.md（汇编+反编译 C 全读）
// 本 TU 局部视图（MoveMapMsgView 同法）：+0xe 的 4B nameLen 为真实 int 字段
// （ORIG 081c0297 `mov 0xe(%eax),%eax` 单步 disp 证据），冻结头 m_padE[4] 的
// 等偏移/等宽强类型化，sizeof(0x38) 不变；已同步列 PacketTypes 字段化请求给 Main。
#pragma pack(push, 1)
struct MSG_CREATE_CHARAC_VIEW : MSG_BASE
{
    unsigned char m_slot;  // +0xd
    int m_nameLen;      // +0xe（ORIG read 081c02db get_intERi 目标）
    char m_name[0x1e];  // +0x12..+0x2f
    char m_padTail[8];  // +0x30..+0x37
};
#pragma pack(pop)
#include "DisPatcherCore/read_6.inc"
#include "DisPatcherCore/send_1.inc"

#include "DisPatcherCore/makeGift.inc"

#include "DisPatcherCore/dtor_DisPatcher_DeleteCharac.inc"
#include "DisPatcherCore/check_error_2.inc"
#include "DisPatcherCore/check_state_1.inc"
#include "DisPatcherCore/read_7.inc"
#include "DisPatcherCore/send_2.inc"

#include "DisPatcherCore/process_6.inc"

#include "DisPatcherCore/dtor_DisPatcher_ReturnToSelectCharacter.inc"
#include "DisPatcherCore/dispatch_sig_3.inc"
#include "DisPatcherCore/read_8.inc"

#include "DisPatcherCore/process_7.inc"

#include "DisPatcherCore/dtor_DisPatcher_GetUserInfo.inc"
#include "DisPatcherCore/check_error_3.inc"
#include "DisPatcherCore/read_9.inc"

#include "DisPatcherCore/process_8.inc"

#include "DisPatcherCore/dtor_DisPatcher_RecoverStamina.inc"
#include "DisPatcherCore/check_error_4.inc"
#include "DisPatcherCore/read_10.inc"
#include "DisPatcherCore/send_3.inc"

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
#include "DisPatcherCore/dispatch_sig_4.inc"
#include "DisPatcherCore/read_11.inc"
#include "DisPatcherCore/process_10.inc"

#include "DisPatcherCore/dtor_DisPatcher_ResPeer.inc"
#include "DisPatcherCore/dispatch_sig_5.inc"
#include "DisPatcherCore/read_12.inc"
#include "DisPatcherCore/process_11.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPartyInfo.inc"
#include "DisPatcherCore/check_error_5.inc"
#include "DisPatcherCore/read_13.inc"
#include "DisPatcherCore/send_4.inc"

#include "DisPatcherCore/process_12.inc"

#include "DisPatcherCore/dtor_DisPatcher_LeaveParty.inc"
#include "DisPatcherCore/check_error_6.inc"
#include "DisPatcherCore/check_state_2.inc"
#include "DisPatcherCore/read_14.inc"
#include "DisPatcherCore/send_5.inc"

#include "DisPatcherCore/process_13.inc"

#include "DisPatcherCore/dtor_DisPatcher_WalkoutParty.inc"
#include "DisPatcherCore/check_error_7.inc"
#include "DisPatcherCore/read_15.inc"
#include "DisPatcherCore/send_6.inc"

#include "DisPatcherCore/process_14.inc"

#include "DisPatcherCore/dtor_DisPatcher_StartGame.inc"
#include "DisPatcherCore/check_error_8.inc"
#include "DisPatcherCore/read_16.inc"
#include "DisPatcherCore/send_7.inc"

#include "DisPatcherCore/excludeAddHackCount.inc"

#include "DisPatcherCore/process_15.inc"

#include "DisPatcherCore/dtor_DisPatcher_SelectDungeon.inc"
#include "DisPatcherCore/check_error_9.inc"
#include "DisPatcherCore/read_17.inc"
#include "DisPatcherCore/send_8.inc"

#include "DisPatcherCore/checkBlueMarble.inc"

#include "DisPatcherCore/process_16.inc"

#include "DisPatcherCore/dtor_DisPatcher_SendMess.inc"
#include "DisPatcherCore/read_18.inc"

#include "DisPatcherCore/process_17.inc"

#include "DisPatcherCore/dtor_DisPatcher_DeleteItem.inc"
#include "DisPatcherCore/check_error_10.inc"
#include "DisPatcherCore/read_19.inc"
#include "DisPatcherCore/exercise_mode.inc"
#include "DisPatcherCore/restricted_check.inc"
#include "DisPatcherCore/condition_check.inc"
#include "DisPatcherCore/send_9.inc"


#include "DisPatcherCore/process_18.inc"
#include "DisPatcherCore/dtor_DisPatcher_MoveItem.inc"
#include "DisPatcherCore/check_error_11.inc"
#include "DisPatcherCore/read_20.inc"
#include "DisPatcherCore/send_10.inc"


#include "DisPatcherCore/process_19.inc"
#include "DisPatcherCore/dtor_DisPatcher_SortItem.inc"
#include "DisPatcherCore/check_error_12.inc"
#include "DisPatcherCore/read_21.inc"
#include "DisPatcherCore/send_11.inc"


#include "DisPatcherCore/process_20.inc"
#include "DisPatcherCore/dtor_DisPatcher_BuyItem.inc"
#include "DisPatcherCore/check_error_13.inc"
#include "DisPatcherCore/send_12.inc"

#include "DisPatcherCore/_onBuyItemEvent.inc"

#include "DisPatcherCore/get_nMaxBuyableCount.inc"

#include "DisPatcherCore/get_data.inc"

#include "DisPatcherCore/statistic_process.inc"

#include "DisPatcherCore/process_21.inc"

#include "DisPatcherCore/dtor_DisPatcher_RentAvatar.inc"
#include "DisPatcherCore/dispatch_sig_6.inc"
#include "DisPatcherCore/read_22.inc"
#include "DisPatcherCore/process_22.inc"

#include "DisPatcherCore/dtor_DisPatcher_ExtendAvatar.inc"
#include "DisPatcherCore/dispatch_sig_7.inc"
#include "DisPatcherCore/read_23.inc"
#include "DisPatcherCore/process_23.inc"

#include "DisPatcherCore/dtor_DisPatcher_SellItem.inc"
#include "DisPatcherCore/dispatch_sig_8.inc"
#include "DisPatcherCore/read_24.inc"
#include "DisPatcherCore/process_24.inc"

#include "DisPatcherCore/dtor_DisPatcher_RepairEquip.inc"
#include "DisPatcherCore/check_error_14.inc"
#include "DisPatcherCore/read_25.inc"
#include "DisPatcherCore/send_13.inc"
#include "DisPatcherCore/process_25.inc"


#include "DisPatcherCore/dtor_DisPatcher_SetTradeState.inc"
#include "DisPatcherCore/dispatch_sig_9.inc"
#include "DisPatcherCore/read_26.inc"

#include "DisPatcherCore/process_26.inc"

#include "DisPatcherCore/dtor_DisPatcher_Compound.inc"
#include "DisPatcherCore/dispatch_sig_10.inc"
#include "DisPatcherCore/read_27.inc"
#include "DisPatcherCore/process_27.inc"

#include "DisPatcherCore/dtor_DisPatcher_DisJointItem.inc"
#include "DisPatcherCore/dispatch_sig_11.inc"
#include "DisPatcherCore/read_28.inc"
#include "DisPatcherCore/process_28.inc"

#include "DisPatcherCore/_check_disjointable_user_state.inc"
#include "DisPatcherCore/_check_disjointable_item_state.inc"
                                    Inven_Item& invenItem, CItem** item,
                                    CUser* targetUser, unsigned short region);
#include "DisPatcherCore/_check_disjoint_need_slot.inc"
                               expert_job::CDisjointer* disjointer);

#include "DisPatcherCore/_disjoint.inc"


#include "DisPatcherCore/dtor_DisPatcher_Lottery.inc"
#include "DisPatcherCore/dispatch_sig_12.inc"
#include "DisPatcherCore/read_29.inc"

#include "DisPatcherCore/process_29.inc"

#include "DisPatcherCore/dtor_DisPatcher_ChangeSkill.inc"
#include "DisPatcherCore/check_error_15.inc"
#include "DisPatcherCore/read_30.inc"
#include "DisPatcherCore/send_14.inc"

#include "DisPatcherCore/process_30.inc"

#include "DisPatcherCore/dtor_DisPatcher_BuySkill.inc"
#include "DisPatcherCore/check_error_16.inc"
#include "DisPatcherCore/check_state_3.inc"
#include "DisPatcherCore/read_31.inc"
#include "DisPatcherCore/send_15.inc"

#include "DisPatcherCore/process_31.inc"

// ---------------------------------------------------------------------------
// DisPatcher_BuySkill 四个 helper（ORIG T 符号；各报告 ## 汇编+## 反编译 C 全文转写）。
// buySkillInfo 布局（PacketTypes.h）：m_0@+0(u8 技能号)/m_1@+1(u8)/m_4@+4(int 结果)/
// m_8@+8(int 等级出参)。公会技能区间 [200,208]（cmp $0xc7 jbe / cmp $0xd0 ja 门）。
// ---------------------------------------------------------------------------

#include "DisPatcherCore/master_new_skill_process.inc"

#include "DisPatcherCore/refund_skill_process.inc"

#include "DisPatcherCore/try_refund_skills_process.inc"

#include "DisPatcherCore/try_master_new_skills_process.inc"

#include "DisPatcherCore/dtor_DisPatcher_IncreStatus.inc"
#include "DisPatcherCore/dispatch_sig_13.inc"
#include "DisPatcherCore/read_32.inc"

#include "DisPatcherCore/process_32.inc"

#include "DisPatcherCore/dtor_DisPatcher_QuestAction.inc"
#include "DisPatcherCore/dispatch_sig_14.inc"
#include "DisPatcherCore/read_33.inc"

#include "DisPatcherCore/process_33.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPos.inc"
#include "DisPatcherCore/check_error_17.inc"
#include "DisPatcherCore/read_34.inc"
#include "DisPatcherCore/process_34.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetArea.inc"
#include "DisPatcherCore/check_error_18.inc"
#include "DisPatcherCore/read_35.inc"
#include "DisPatcherCore/send_16.inc"
#include "DisPatcherCore/process_35.inc"

#include "DisPatcherCore/dtor_DisPatcher_Fish.inc"
#include "DisPatcherCore/read_36.inc"

#include "DisPatcherCore/process_36.inc"
#include "DisPatcherCore/dispatch_sig_15.inc"

#include "DisPatcherCore/check_error_19.inc"
#include "DisPatcherCore/Party_hack_check.inc"
#include "DisPatcherCore/process_37.inc"
#include "DisPatcherCore/dtor_DisPatcher_FinishLoad.inc"
#include "DisPatcherCore/check_state_4.inc"
#include "DisPatcherCore/read_37.inc"

#include "DisPatcherCore/dtor_DisPatcher_UseSkill.inc"
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

#include "DisPatcherCore/IsSkillMatchFirstGrowType.inc"

#include "DisPatcherCore/IsSkillMatchSecondGrowType.inc"

#include "DisPatcherCore/GetHackType.inc"

// [SplitC 2026-09-11 batch4 · TU 迁移] IsValidUsingSkill 已迁出至 HackLog_InvalidUsingSkill.cpp（ORIG GI 键对齐）。


#include "DisPatcherCore/process_38.inc"

#include "DisPatcherCore/dtor_DisPatcher_DieMob.inc"
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

#include "DisPatcherCore/process_40.inc"

#include "DisPatcherCore/dtor_DisPatcher_UseCoin.inc"
#include "DisPatcherCore/check_error_22.inc"
#include "DisPatcherCore/read_41.inc"
#include "DisPatcherCore/send_19.inc"
#include "DisPatcherCore/process_41.inc"


#include "DisPatcherCore/dtor_DisPatcher_GiveUpGame.inc"
#include "DisPatcherCore/check_error_23.inc"
#include "DisPatcherCore/read_42.inc"

#include "DisPatcherCore/process_42.inc"

#include "DisPatcherCore/dtor_DisPatcher_GetItem.inc"
#include "DisPatcherCore/check_error_24.inc"
#include "DisPatcherCore/read_43.inc"
#include "DisPatcherCore/send_20.inc"
#include "DisPatcherCore/checkGetItemPacket.inc"
#include "DisPatcherCore/process_43.inc"


#include "DisPatcherCore/dtor_DisPatcher_UseStack.inc"
#include "DisPatcherCore/dispatch_sig_17.inc"
#include "DisPatcherCore/read_44.inc"
#include "DisPatcherCore/process_44.inc"


#include "DisPatcherCore/dtor_DisPatcher_MoveMap.inc"
#include "DisPatcherCore/check_error_25.inc"
#include "DisPatcherCore/check_state_6.inc"
#include "DisPatcherCore/read_45.inc"
#include "DisPatcherCore/CheckTrapBitsHackCnt.inc"
#include "DisPatcherCore/CheckOneMapPlayTimeHackCnt.inc"
#include "DisPatcherCore/process_45.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPlayResult.inc"
#include "DisPatcherCore/dispatch_sig_18.inc"
#include "DisPatcherCore/read_46.inc"
#include "DisPatcherCore/process_46.inc"

#include "DisPatcherCore/dtor_DisPatcher_DropItem.inc"
#include "DisPatcherCore/check_error_26.inc"
#include "DisPatcherCore/read_47.inc"
#include "DisPatcherCore/send_21.inc"
#include "DisPatcherCore/process_47.inc"


#include "DisPatcherCore/dtor_DisPatcher_UseEquip.inc"
#include "DisPatcherCore/check_error_27.inc"
#include "DisPatcherCore/read_48.inc"
#include "DisPatcherCore/send_22.inc"
#include "DisPatcherCore/process_48.inc"


#include "DisPatcherCore/dtor_DisPatcher_MakePVP.inc"
#include "DisPatcherCore/ExtractPacket.inc"
// [b28d-29] N82/P183：临时对象构造块（lea -0x10）位置前移 3 指令，difflib 块序伪差；语义集合全等  (ORIG 0x081f803e insn 103->104)

#include "DisPatcherCore/dispatch_sig_19.inc"
#include "DisPatcherCore/read_49.inc"

#include "DisPatcherCore/process_49.inc"

#include "DisPatcherCore/dtor_DisPatcher_EnterPVP.inc"
#include "DisPatcherCore/dispatch_sig_20.inc"
#include "DisPatcherCore/process_50.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPSeat.inc"
#include "DisPatcherCore/dispatch_sig_21.inc"
#include "DisPatcherCore/read_50.inc"
#include "DisPatcherCore/process_51.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPReady.inc"
#include "DisPatcherCore/check_error_28.inc"
#include "DisPatcherCore/read_51.inc"
#include "DisPatcherCore/send_23.inc"
#include "DisPatcherCore/process_52.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPTeam.inc"
#include "DisPatcherCore/dispatch_sig_22.inc"
#include "DisPatcherCore/read_52.inc"
#include "DisPatcherCore/process_53.inc"

#include "DisPatcherCore/dtor_DisPatcher_DiePvP.inc"
#include "DisPatcherCore/dispatch_sig_23.inc"
#include "DisPatcherCore/read_53.inc"
#include "DisPatcherCore/process_54.inc"

#include "DisPatcherCore/dtor_DisPatcher_PvPTimeout.inc"
#include "DisPatcherCore/check_error_29.inc"
#include "DisPatcherCore/read_54.inc"
#include "DisPatcherCore/send_24.inc"
#include "DisPatcherCore/process_55.inc"


#include "DisPatcherCore/dtor_DisPatcher_EndPvPResult.inc"
#include "DisPatcherCore/dispatch_sig_24.inc"

#include "DisPatcherCore/read_55.inc"
#include "DisPatcherCore/process_56.inc"

#include "DisPatcherCore/dtor_DisPatcher_ResPvpRank.inc"
#include "DisPatcherCore/ExtractPacket_1.inc"
// [b28d-29] N82/P197③：ORIG 两 dead 局部预清 + 槽序（-0x1c/-0x18/-0x10/-0xc），4.4 合并复用致全帧平移；语义集合全等  (ORIG 0x081f05ea insn 197->197)

#include "DisPatcherCore/SavePvpRank.inc"

#include "DisPatcherCore/dispatch_sig_25.inc"
#include "DisPatcherCore/read_56.inc"
#include "DisPatcherCore/process_57.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPMapIndex.inc"
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
#include "DisPatcherCore/check_error_30.inc"
#include "DisPatcherCore/read_59.inc"

#include "DisPatcherCore/send_25.inc"
#include "DisPatcherCore/process_60.inc"

#include "DisPatcherCore/process_packet_jpn.inc"

#include "DisPatcherCore/dtor_DisPatcher_RemoveFriend.inc"
#include "DisPatcherCore/check_error_31.inc"
#include "DisPatcherCore/read_60.inc"
#include "DisPatcherCore/send_26.inc"
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
#include "DisPatcherCore/dispatch_sig_32.inc"
#include "DisPatcherCore/read_66.inc"
#include "DisPatcherCore/process_67.inc"

#include "DisPatcherCore/ST_Guild_War_Rank_Info.inc"
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
#include "DisPatcherCore/dispatch_sig_35.inc"
#include "DisPatcherCore/read_69.inc"
#include "DisPatcherCore/process_70.inc"

#include "DisPatcherCore/dtor_DisPatcher_UdpCharacteristic.inc"
#include "DisPatcherCore/dispatch_sig_36.inc"
#include "DisPatcherCore/read_70.inc"
#include "DisPatcherCore/process_71.inc"

#include "DisPatcherCore/dtor_DIspatcher_OneDayLetheTicket.inc"
#include "DisPatcherCore/dispatch_sig_37.inc"
#include "DisPatcherCore/read_71.inc"
#include "DisPatcherCore/process_72.inc"

#include "DisPatcherCore/dtor_Dispatcer_polic_one_to_one_chat_disable.inc"
#include "DisPatcherCore/dispatch_sig_38.inc"
#include "DisPatcherCore/read_72.inc"
#include "DisPatcherCore/process_73.inc"

#include "DisPatcherCore/dtor_DisPatcher_CheckSecurityProtection.inc"
#include "DisPatcherCore/check_error_32.inc"
#include "DisPatcherCore/read_73.inc"
#include "DisPatcherCore/send_27.inc"

#include "DisPatcherCore/process_74.inc"

#include "DisPatcherCore/dtor_DisPatcher_EventDungeon_DestoryObject.inc"
#include "DisPatcherCore/check_error_33.inc"
// [SplitC 2026-09-11 batch4 · TU 迁移] read_74 已迁出至 localchina_PacketDispatcher.cpp（ORIG GI 键对齐）。

#include "DisPatcherCore/send_28.inc"
#include "DisPatcherCore/process_75.inc"

#include "DisPatcherCore/dtor_DisPatcher_EventDungeon_ClearRoom.inc"

// ORIG 0x085957e8 W _ZN33DisPatcher_EventDungeon_ClearRoomC1Ev（无报告，objdump 转写）：
//   调 IPacketDispatcher<CMDPacketStruct::_STReqEventDungeonClearRoom,
//                        CMDPacketStruct::_STResEventDungeonClearRoom, ch_state3>::C1
//   后写 vptr=0x8b66068。基类实参已按 ORIG mangled 统一（OrigPktDisp 批），
//   空体 ctor 由编译器生成基类构造+vptr 写入，与 ORIG 逐指令等价。

#include "DisPatcherCore/check_error_34.inc"

#include "DisPatcherCore/process_76.inc"

#include "DisPatcherCore/read_75.inc"

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
#include "DisPatcherCore/add.inc"
}  // namespace statistc_proxy



#include "DisPatcherCore/dtor_DispatcherResetRandomOption.inc"
#include "DisPatcherCore/read_77.inc"

#include "DisPatcherCore/RandomOption.inc"

#include "DisPatcherCore/process_77.inc"

#include "DisPatcherCore/process_78.inc"

#include "DisPatcherCore/serviceOfShusia.inc"

// ============================================================================
// G2 角色域三个 CUser 叶子方法（定义宿主为本 TU，沿用 serviceOfShusia 先例；
// CUser.cpp 由 RestoreCUserStatMethods 独占，声明已按广播加入 CUser.h）。
// ============================================================================

#include "DisPatcherCore/SetLoadRentalInfoFromExchange.inc"

#include "DisPatcherCore/RemoveCharacMercenaryInfo.inc"

#include "DisPatcherCore/GetGuildIdBeforeDeleteCharacter.inc"


#include "DisPatcherCore/SetDeleteEventDungeonInvitation.inc"


// ---- 本段局部声明（符号定义在别处；链接解析）----
// ORIG T 0x0843081c _ZN24DB_UpdateAvatarJewelSlot11makeRequestEjjPK20stAvatarEmblemInfo_t
class DB_UpdateAvatarJewelSlot
{
public:
    static void makeRequest(unsigned int characNo, unsigned int avatarKey,
                            const stAvatarEmblemInfo_t* emblem);
};

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

#include "DisPatcherCore/dispatch_sig_41.inc"


// ============================================================================
// 第二批：DisPatcher_DebugCommand::dispatch_sig（ORIG T 0x081fc4de，761 insn）
// 报告 docs/class_func_reports/DisPatcher_DebugCommand/dispatch_sig.md 全文已读
// （## 汇编 + ## 反编译 C）。GM 命令总入口：get_packet(0) → cmd/gm-mode 门禁 →
// IsGm 门 → 0x00..0x30 switch 分派（_debugCommand* 家族定义于
// DisPatcher_DebugCommand_cmds.cpp）→ SendCmdPacket(0x41) 回显。
// ============================================================================
#include "CGM_Manager.h"

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
    struct BodyTail {
        unsigned char m_bodyHead[0x14c];
        unsigned char m_tailFlag[6];
        short m_tailShort[6];
        unsigned char m_tailPad[4];
    };
    union {
        unsigned char m_body[0x162];
        BodyTail m_tail;
    };
};                               // sizeof = 0x186（静态断言随 ctor 定义）
#pragma pack(pop)
static_assert(sizeof(Packet_Frame_Lag_Statistic_Add) == 0x186,
              "Packet_Frame_Lag_Statistic_Add ABI drift (ctor arg 0x186)");

#include "DisPatcherCore/Packet_Frame_Lag_Statistic_Add.inc"
// [b28d-29] N82/P183：帧 0x1b4 vs 0x1b8 + ORIG 无 -0x20 指针缓存而 4.4 新增，槽平移；语义集合全等  (ORIG 0x08217046 insn 418->368)

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
