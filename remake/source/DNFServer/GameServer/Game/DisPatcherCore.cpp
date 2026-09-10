// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DisPatcherCore/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

class DB_GetDeathTowerBestRecord   // ORIG T 0x0842a048 _ZN26DB_GetDeathTowerBestRecord11makeRequestEij
{                                  // 定义由 DB_U3.cpp 提供（Prod_DB_Misc 批次）；
public:                            // 参数类型按 ORIG mangled Eij=(int, unsigned int)，
    static void makeRequest(int uid, unsigned int characNo);   // uid 按位不变传参
};

#include "DisPatcherCore/getAvatarRentPrice.inc"
#include "DisPatcherCore/RentAvatarErrorProc.inc"
#include "DisPatcherCore/EnterPVPRoom.inc"

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
}
#include "DisPatcherCore/checkSpecialChar.inc"
#include "DisPatcherCore/checkSpecialCharName.inc"
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
}
#include "DisPatcherCore/read_1.inc"
}
#include "DisPatcherCore/process.inc"

#include "DisPatcherCore/DisPatcher_Login.inc"
#include "DisPatcherCore/dtor_DisPatcher_Login.inc"
#include "DisPatcherCore/dispatch_sig_1.inc"
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
}
#include "DisPatcherCore/process_1.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetIP.inc"
#include "DisPatcherCore/read_3.inc"
}
#include "DisPatcherCore/process_2.inc"
#include "DisPatcherCore/dtor_DisPatcher_Exit.inc"
#include "DisPatcherCore/dispatch_sig_2.inc"
}
#include "DisPatcherCore/read_4.inc"
}
#include "DisPatcherCore/process_3.inc"
#include "DisPatcherCore/dtor_DisPatcher_SelectCharac.inc"
#include "DisPatcherCore/check_error.inc"
}

// ORIG 0x081c6c22 T _ZN23DisPatcher_SelectCharac22checkPvpEnterCharacterEP5CUserR17MSG_CHARAC_SELECT
// （报告 DisPatcher_SelectCharac/checkPvpEnterCharacter.md 全文转写，wave2）：
// user==0 → false；PvP 频道且槽位职业为 9/10（ restricting job ）→ false；否则 true。
#include "DisPatcherCore/checkPvpEnterCharacter.inc"
}

// ORIG 0x081cab3a T _ZN18DisPatcher_UseCoin20usedCoinDungeonCheckEP5CUserP6CParty
// （报告 DisPatcher_UseCoin/usedCoinDungeonCheck.md 汇编+C 全文转写，wave2）。
#include "DisPatcherCore/usedCoinDungeonCheck.inc"
}

// ORIG 0x081ca63a T _ZN18DisPatcher_UseCoin11Dungeon_check_errorEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] N82/P183：ORIG 每次重调 GetParty 并落独立槽（不复用），4.4 复用同槽 + 帧 -0x14；语义集合全等  (ORIG 0x081ca63a insn 87->87)
// （报告 DisPatcher_UseCoin/Dungeon_check_error.md 汇编+C 全文转写，wave2）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 57（ORIG 87 / ours 86），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/Dungeon_check_error.inc"
}

// ORIG 0x081ca75c T _ZN18DisPatcher_UseCoin11WarRoom_check_errorEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseCoin/WarRoom_check_error.md 汇编+C 全文转写，wave2）。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 39 / ours 39 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/WarRoom_check_error.inc"
}
// ORIG 0x0821b098 W _ZN28DisPatcher_UdpCharacteristic13partyPingStatEP5CUserRtS2_
// （报告 DisPatcher_UdpCharacteristic/partyPingStat.md 汇编+C 全文转写，wave2）：
// Party_Ping 统计包（0xfa7,0xe）：+0xa=bad、+0xc=bad+total，经统计代理 SendPacket。
#include "DisPatcherCore/partyPingStat.inc"
}

// ORIG 0x0821afbc W _ZN28DisPatcher_UdpCharacteristic11pvpPingStatEP5CUserRtS2_
// （报告 pvpPingStat.md 全文转写，wave2）：Pvp_Ping 包（0xfa8,0xe）同型。
#include "DisPatcherCore/pvpPingStat.inc"
}

// ORIG 0x0821b02a W _ZN28DisPatcher_UdpCharacteristic15fairPvpPingStatEP5CUserRtS2_
// （报告 fairPvpPingStat.md 全文转写，wave2）：Fair_Pvp_Ping 包（0x27ed,0xe）同型。
#include "DisPatcherCore/fairPvpPingStat.inc"
}
// ORIG 0x081c632a T _ZN23DisPatcher_SelectCharac4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_SelectCharac/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x156f, PRETTY, 0, 0)。
#include "DisPatcherCore/read_5.inc"
}
#include "DisPatcherCore/send.inc"
}

// ORIG 0x081c6380 T _ZN23DisPatcher_SelectCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0x1587,pretty,err,acc_id)。
// resetSaveFlags → 视图状态/上次登录判定 → 综合PvP频道抑制 → exchange 命中计数 →
// 回填 MSG+0xe/+0x12（_Charac_info.m_field14b8/m_field14b7）→ 九路 DB/ES 请求 →
// [b28d-29] N82：帧 0x1a0 vs 0x1b4 + 槽平移，调用点全等  (ORIG 0x081c6380 insn 320->323)
// 清上次登录 → 死塔最佳记录/注册日期 DB 请求 → LimitNpcBuyItemInfoAll 监视包。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 283（ORIG 320 / ours 323），残差=N7/N82 帧与 callee-saved 角色/P168a④ bool 槽化/P211/N18a 中转/store 序/P30/N4 槽；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_4.inc"
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
}

// ORIG 0x081c7bac T _ZN23DisPatcher_SelectCharac20RegistTimerSchedulesEP5CUser
// （报告 RegistTimerSchedules.md）：G_TimerQueue()->InsertTimer(OBJ_TYPE_0, uid,
// [b28d-29] NEAR=N7/N82附：两 callee-saved 角色对调（ORIG getCurCharacNo->%esi / get_unique_id->%edi，4.4.7 恒反向）；源码已为内联实参形，P248 三档试尽不翻（4.4.6 后端可翻但本波禁改路由）  (ORIG 0x081c7bac insn 34->35)
// TIMER_MESSAGE_0x51, 0, unique_id, characNo) 后恒返 1。
// [b28d-solo-DisPatcherCore] 复核 NEAR：ndiff 8（ORIG 34 / ours 34），残差=N82 槽/块序；N 封顶（详见上方 b28d-29 归因）。
#include "DisPatcherCore/RegistTimerSchedules.inc"
}

// ORIG T 0x081c79f2（RequestBlackCount.md）：SIG 0xb3 → CACHE_BLACK_COUNT；memset 0x10。
#include "DisPatcherCore/RequestBlackCount.inc"
}

// ORIG T 0x081c6ee6：直接尾调 RequestDBInventory_forchina。
#include "DisPatcherCore/RequestDBInventory.inc"
}

// ORIG T 0x081c7c1c（RequestDBInventory_forchina.md）：SIG 8 → CACHE_INVENTORY；
// BigStreamPool；memset 0x1544c 后 acc/charac/tagInven 清理/+0x8←MSG.m_0e。
// [b28d-29] setcc-fold：ORIG 尾 bool 物化岛（mov $1,%esi/mov $0,%ebx + test），4.4 恒折单条 mov/直分支  (ORIG 0x081c7c1c insn 137->134)
// 尾部两分支均返回 1（ORIG esi 未初始化路径经 test ebx 归一为恒真）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 8（ORIG 137 / ours 133），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBInventory_forchina.inc"
}

// ORIG T 0x081c70c2（RequestDBQuest.md）：SIG 10 → CACHE_QUEST；BigStreamPool；
// memset 0x7608 后仅写 characNo。
#include "DisPatcherCore/RequestDBQuest.inc"
}

// ORIG T 0x081c7436（RequestDBDungeonClear.md）：SIG 12 → CACHE_DUNGEON_CLEAR；
// StreamPool；memset 0x1810 后 characNo@0 / accId@4。
#include "DisPatcherCore/RequestDBDungeonClear.inc"
}

// ORIG T 0x081c7836（RequestDBKillMonsterInfo.md）：SIG 0x282 → CACHE_KILL_MONSTER；
// BigStreamPool；memset 0xafcc 后 characNo 写于尾部 +0xafc8。
#include "DisPatcherCore/RequestDBKillMonsterInfo.inc"
}

// ORIG T 0x081c7602（RequestDBETC.md）：SIG 0x20 → CACHE_ETC；BigStreamPool；
// memset 0x14b60；serverGroup/channelNo ← CEnvironment；tagEtc 清理；
// [b28d-29] P254/N18a：字段->实参两段中转（ORIG mov mem,%eax; mov %eax,%edx）与块序差；语义集合全等  (ORIG 0x081c7602 insn 152->154)
// job 预写 combo 视图 m_job（SIG 内绝对偏移 +0x14ab0）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 41（ORIG 152 / ours 153），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBETC.inc"
}


// ORIG T 0x081c6f08（RequestDBSkill.md）：SIG 9 → CACHE_SKILL；StreamPool；
// [b28d-29] N93附/P222：尾块布局（ORIG 先 jmp 尾岛再统一 epilogue，4.4 就地内联 epilogue），槽集合与调用点全等  (ORIG 0x081c6f08 insn 120->119)
// memset 0x414 后仅写 characNo@0；队列 5(有效上次登录)/2(DB_Q)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 55（ORIG 120 / ours 123），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RequestDBSkill.inc"
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
}

// [b28d-29] setcc-fold + N82：ORIG 0x081c08cf 短路 bool 物化岛（mov $1/$0 + 二次 test）4.4 恒折 setne；cMyTrace 槽 -0x34 vs -0x24（4.4 复用，帧 0x44 vs 0x34）  (ORIG 0x081c0830 insn 140->136)
#include "DisPatcherCore/dtor_DisPatcher_CreateCharac.inc"
#include "DisPatcherCore/check_error_1.inc"
}
#include "DisPatcherCore/check_state.inc"
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
}
#include "DisPatcherCore/send_1.inc"
}

// ORIG T 0x081c0a1a（makeGift.md）：按创建槽位（=职业索引）发放初始武器/衣服与
// 职业赠礼道具。find_item 失败时仅置标志 0；命中则 Inven_Item 预填 +0x02 索引后
// [b28d-29] P205/P30：ORIG 单条 scaled `mov <A>(,%eax,8)`，4.4 拆 add+scaled；字段直读 vs 缓存差，语义集合全等  (ORIG 0x081c0a1a insn 227->223)
// 经 CItem::make_item（vtable+0x08）填充并整体拷贝入 SIG 槽位。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 158（ORIG 225 / ours 221），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/makeGift.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_DeleteCharac.inc"
#include "DisPatcherCore/check_error_2.inc"
}
#include "DisPatcherCore/check_state_1.inc"
}
#include "DisPatcherCore/read_7.inc"
}
#include "DisPatcherCore/send_2.inc"
}

// ORIG 0x081c1182 T _ZN23DisPatcher_DeleteCharac7processEP5CUserR8MSG_BASER9ParamBase
// param.m_errNo=check_error；>0 返 0；<0 LineFunc(0xa41,pretty,err,acc_id)。
// RemoveCharacMercenaryInfo → StreamPool 取流构造 SIG_DELETE_CHARAC（memset 0x14）→
// characNo==0 则 m_errNo=0x15 并 Free 回流 → 填公会 id → lockDeleteCharac 成功投递
// [b28d-29] N82/P211：帧 0x5c vs 0x50 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081c1182 insn 216->210)
// DB_Q 并打 cMyTrace（accid 十进制串 / m_characNo / m_name），失败 Free + LineFunc(0xa68)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 134（ORIG 216 / ours 210），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_6.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_ReturnToSelectCharacter.inc"
#include "DisPatcherCore/dispatch_sig_3.inc"
}
#include "DisPatcherCore/read_8.inc"
}

// ORIG 0x0823349a W _ZN34DisPatcher_ReturnToSelectCharacter7processEP5CUserR8MSG_BASER9ParamBase
// push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，
// 全部逻辑位于 dispatch_sig(0x081fd25c，见 docs/class_func_reports/
// DisPatcher_ReturnToSelectCharacter/dispatch_sig.md；按分工 dispatch_sig 保持 stub)。
#include "DisPatcherCore/process_7.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_GetUserInfo.inc"
#include "DisPatcherCore/check_error_3.inc"
}
// ORIG 0x081c3d46 T _ZN22DisPatcher_GetUserInfo4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_GetUserInfo/read.md 逐块映射）：get_short→msg+0xd、
// get_byte→msg+0xf；失败各走 LineFunc(0xf5b/0xf5d, PRETTY, 0, 0)。
#include "DisPatcherCore/read_9.inc"
}

// ORIG 0x081c3dd8 T _ZN22DisPatcher_GetUserInfo7processEP5CUserR8MSG_BASER9ParamBase
// err=check_error；>0 返 -1；<0 LineFunc(0xf6d,pretty,0,acc_id)（第三参恒 0）；
// 否则 G_GameWorld()->get_user_info(user, (u16)msg+0xd, (s8)msg+0xf)，返回 err。
#include "DisPatcherCore/process_8.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_RecoverStamina.inc"
#include "DisPatcherCore/check_error_4.inc"
}
#include "DisPatcherCore/read_10.inc"
}
#include "DisPatcherCore/send_3.inc"
}

// ORIG 0x081c3eae T _ZN25DisPatcher_RecoverStamina7processEP5CUserR8MSG_BASER9ParamBase
// err>0：SendCmdErrorPacket(cmd=9, err&0xff) 后返回 -1；err<0：LineFunc(0xf9d,pretty,0,acc_id)
// 并返回其值；err==0：param+0x4 = user->serviceOfShusia()，返回 err。
#include "DisPatcherCore/process_9.inc"
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
#include "DisPatcherCore/dispatch_sig_4.inc"
}
#include "DisPatcherCore/read_11.inc"
}
#include "DisPatcherCore/process_10.inc"

#include "DisPatcherCore/dtor_DisPatcher_ResPeer.inc"
#include "DisPatcherCore/dispatch_sig_5.inc"
}
#include "DisPatcherCore/read_12.inc"
}
#include "DisPatcherCore/process_11.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPartyInfo.inc"
#include "DisPatcherCore/check_error_5.inc"
}
// ORIG 0x081c4396 T _ZN23DisPatcher_SetPartyInfo4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_SetPartyInfo/read.md 汇编+反编译 C 全文转写）：
// get_byte(titleIndex) → titleIndex==0 时 get_int(+0x0e) 并 get_str(title,0x20,+0x0e)
// → get_byte(userMax)/get_short(dungIndex)/get_byte(dungDiffi)；任一失败 LineFunc
// 0x10d9/0x10dd/0x10e0/0x10e2/0x10e5/0x10e6，全成功返回 0。
#include "DisPatcherCore/read_13.inc"
}
#include "DisPatcherCore/send_4.inc"
}

// ORIG 0x081c4562 T _ZN23DisPatcher_SetPartyInfo7processEP5CUserR8MSG_BASER9ParamBase
// param+0x4 = check_error；>0 返回 0；<0 LineFunc(0x10f6,pretty,err,acc_id) 并返回其值；
// 无队：G_CGameManager()->GetParty() 空则 param=4 返回 0，否则 create_party +
// SetPartyInfoUI + send_party_info_to_all(party,0)；有队：非队长 param=8，
// 队长 SetPartyInfoUI + send_party_info_to_all(party,1)；均返回 0。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 96 / ours 96 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_12.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_LeaveParty.inc"
#include "DisPatcherCore/check_error_6.inc"
}
#include "DisPatcherCore/check_state_2.inc"
}
#include "DisPatcherCore/read_14.inc"
}
#include "DisPatcherCore/send_5.inc"
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
}

#include "DisPatcherCore/dtor_DisPatcher_WalkoutParty.inc"
#include "DisPatcherCore/check_error_7.inc"
}
// ORIG 0x081caf42 T _ZN23DisPatcher_WalkoutParty4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_WalkoutParty/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x202b, PRETTY, 0, 0)。
#include "DisPatcherCore/read_15.inc"
}
#include "DisPatcherCore/send_6.inc"
}

// ORIG 0x081caf98 T _ZN23DisPatcher_WalkoutParty7processEP5CUserR8MSG_BASER9ParamBase
// err>0 原样返回；err<0 返回 -1；否则 param+0x4=0，无队 param=0x13；
// 有队先 SendWalkout(msg+0xd 槽位, WALKOUT_TYPE_0) 再 param = walkout_member(user, 槽位)，返回 0。
#include "DisPatcherCore/process_14.inc"
}

// [b28d-29] N82：帧 0x34 vs 0x58 + 条件极性折痕（je/jne），槽集合与调用点全等  (ORIG 0x081c9f6c insn 228->246)
#include "DisPatcherCore/dtor_DisPatcher_StartGame.inc"
#include "DisPatcherCore/check_error_8.inc"
}
#include "DisPatcherCore/read_16.inc"
}
#include "DisPatcherCore/send_7.inc"
}

// ORIG T 0x081ca2a8 _ZN20DisPatcher_StartGame19excludeAddHackCountEP5CUser
// （excludeAddHackCount.md 全文转写）：空用户/副本入口区/moveSpace==1 免记，否则记账。
#include "DisPatcherCore/excludeAddHackCount.inc"
}

// ORIG 0x081ca2fe T process（process.md 全文转写）。
// err>0 返 0；err<0 LineFunc(0x1e5b)；err==0：世界图 hasDeathTower 死调用后，
// 未组队走 GameManager 单人 party（hack 记账 0x1f6 + set_single_play + game_start）；
// [b28d-29] N82：全帧槽号平移 4（ORIG 帧更浅），助记符序列与调用点全等  (ORIG 0x081ca2fe insn 129->133)
// 已组队且 state==3 时对所在 party game_start；均返回 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 55（ORIG 129 / ours 132），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_15.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_SelectDungeon.inc"
#include "DisPatcherCore/check_error_9.inc"
}
#include "DisPatcherCore/read_17.inc"
}
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 35（ORIG 201 / ours 198），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/send_8.inc"
}

// ORIG T 0x081c9172 _ZN24DisPatcher_SelectDungeon15checkBlueMarbleEP6CParty
// [b28d-29] AE：38=38 全等（同上重定位归一）  (ORIG 0x081c9172 insn 38->39)
// （checkBlueMarble.md 全文转写）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 38 / ours 38 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/checkBlueMarble.inc"
}

// ORIG 0x081c8102 T process（process.md 全文转写，含汇编分支次序）。
// §6.2 巨件核销：P167 95/95 [0缺0多]；gap 810→810/830；残差归因 N82/P112。
// 真修 1 条：死块 [HackUser] logFormat(0x198a) 误传 member 指针 → 按 ORIG
// [b28d-29] N82/P211：帧与参数副本槽（-0x90 vs -0x98）+ store 装载序，调用点全等  (ORIG 0x081c8102 insn 810->831)
// +0x13e 改 member->get_acc_id()（补齐缺失的 _ZNK5CUser10get_acc_idEv 调用点）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 840（ORIG 808 / ours 826），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_16.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_SendMess.inc"
#include "DisPatcherCore/read_18.inc"
}

// ORIG 0x08232f70 W _ZN19DisPatcher_SendMess7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部聊天业务（type 分派/表情改写/敏感词 Filter/消耗品扣减/community+monitor 监视包/
// DB_InsertPoliceSaveChatting/GameWorld::send_chat_msg）内嵌于
// dispatch_sig(0x081f3540，报告 docs/class_func_reports/DisPatcher_SendMess/dispatch_sig.md)。
#include "DisPatcherCore/process_17.inc"

#include "DisPatcherCore/dtor_DisPatcher_DeleteItem.inc"
#include "DisPatcherCore/check_error_10.inc"
}
// ORIG 0x081c17c0 T _ZN21DisPatcher_DeleteItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_DeleteItem/read.md（汇编+反编译 C 全读）
#include "DisPatcherCore/read_19.inc"
}
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 7（ORIG 154 / ours 157），残差=P211/N18a 中转/store 序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/exercise_mode.inc"
}
#include "DisPatcherCore/restricted_check.inc"
}
#include "DisPatcherCore/condition_check.inc"
}
// ORIG 0x081c2792 T _ZN21DisPatcher_DeleteItem4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_DeleteItem/send.md 汇编+反编译 C 全文转写）：
// m_4!=0x7fffffff 时发包。m_4==0：put_header(1,0x12)/1/m_8/m_9，i<(char)m_9 循环
// put_short(m_slotsD2[i])/put_int(m_counts19c[i])/put_short(m_slotsA[i])，
// finalize(true)+Send；否则 0/errNo/m_8 三字节包。
#include "DisPatcherCore/send_9.inc"
}


// ORIG 0x081c1dda T process（process.md 全文转写）。
// §6.2 巨件核销：P167 61/61 [0缺0多]；gap 31→1；残差归因 N82/P112。
// 真修 1 条：GetInvenSlot 返回 4.4 直构缺 ORIG +0x25e~+0x327 的 17 条逐字段
// [b28d-29] N82/P211：帧 0x100 vs 0x110 + 参数副本槽 + store 装载序；bool 条件 4.4 test 直判 vs 4.1 cmpb，语义集合全等  (ORIG 0x081c1dda insn 651->653)
// 拷贝（sret 匿名槽→具名局部）→ 改两段式（slotItem 收 sret + 拷贝入 item）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 400（ORIG 651 / ours 653），残差=P168a④ bool 槽化/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_18.inc"
}
#include "DisPatcherCore/dtor_DisPatcher_MoveItem.inc"
#include "DisPatcherCore/check_error_11.inc"
}
// ORIG 0x081c54e0 T _ZN19DisPatcher_MoveItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_MoveItem/read.md（汇编+反编译 C 全读）
// 串行 10 次 get_*；任一失败即 LineFunc(行号, pretty, 0, 0)。msgPtr 局部落 -0xc(%ebp)。
#include "DisPatcherCore/read_20.inc"
}
// ORIG 0x081c5b76 T _ZN19DisPatcher_MoveItem4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_MoveItem/send.md 汇编+反编译 C 全文转写）：
// result(m_4)!=0x7fffffff 时发包；失败分支 put_byte(0)/err/idx/spot 四字节；
// 成功分支 put_byte(1)/idx/count/itemIdx/spot/newCount 后 Send，并经
// ARAD::Singleton<AvatarRechargeServer>::Get()->SendDurability 上报耐久。
#include "DisPatcherCore/send_10.inc"
}


// ORIG 0x081c5904 T process（process.md 全文转写）。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 136（ORIG 192 / ours 196），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_19.inc"
}
#include "DisPatcherCore/dtor_DisPatcher_SortItem.inc"
#include "DisPatcherCore/check_error_12.inc"
}
// ORIG 0x081c9bfe T _ZN19DisPatcher_SortItem4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_SortItem/read.md 逐块映射）：get_byte → msg+0xd；
// 失败 LineFunc(0x1d16, PRETTY, 0, 0)。
#include "DisPatcherCore/read_21.inc"
}
// ORIG 0x081c9dba T _ZN19DisPatcher_SortItem4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_SortItem/send.md 汇编+反编译 C 全文转写）：
// put_header(1,0x14)；result==0 → put_byte(1)/(char)invenType，否则 0/(char)result/
// (char)invenType；finalize+Send。result==0 时 send_itemspace((char)invenType)；
// [b28d-29] 已修 DIFF->AE：if-else 决策链改 switch（P17/P251/P261，ORIG 0x081c9ec6 稀疏 switch 树）  (ORIG 0x081c9dba insn 128->129)
// result==0 且 invenType 1/0/2/7 分别 SendItemLockListAvatar/Inven/Cargo/Creature。
#include "DisPatcherCore/send_11.inc"
}


// ORIG 0x081c9c54 T process（process.md 全文转写）。
#include "DisPatcherCore/process_20.inc"
}
#include "DisPatcherCore/dtor_DisPatcher_BuyItem.inc"
#include "DisPatcherCore/check_error_13.inc"
}
// ORIG 0x081be20a T _ZN18DisPatcher_BuyItem4sendEP5CUserR9ParamBase
// 报告：docs/class_func_reports/DisPatcher_BuyItem/send.md（汇编+反编译 C 全读）
// [b28c-L19 N18a] 唯一正文差（165/164）：`UseFinishPoint(out.m_8c)` 处 ORIG
// [b28d-29] N18a/P254：ORIG `mov 0x8c(%eax),%eax; mov %eax,%edx` 冗余 regmove（int 字段->int 形参），4.4 单段直载 edx 不可反向造  (ORIG 0x081be20a insn 165->165)
// `mov 0x8c(%eax),%eax; mov %eax,%edx` 冗余 regmove vs 4.4 单步直载 edx（反向不可造）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 165 / ours 165 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/send_12.inc"
}

// ORIG 0x081bed44 T _ZN18DisPatcher_BuyItem15_onBuyItemEventERK12BuyItemParamP5CUser
// 报告 _onBuyItemEvent.md：push %ebp; mov %esp,%ebp; pop %ebp; ret —— 原始二进制
// 即为纯空实现（购买事件挂钩在 ORIG 中未挂任何回调），忠实转写空函数。
#include "DisPatcherCore/_onBuyItemEvent.inc"
}

// ORIG 0x081beab0 T get_nMaxBuyableCount（报告 get_nMaxBuyableCount.md 全文映射）：
// 对六种货币价格（money/cera/winpoint/finishpoint/guildPowerWarPoint/userPowerWarPoint）
// [b28d-29] N18b/P197③：ORIG 三目 min 形（cmp mem,%eax + jge 双出口）+ 两 dead 局部预清；4.4 折 reg-reg cmp 且丢预清  (ORIG 0x081beab0 insn 130->123)
// 逐项取「持有量 / 单价」与 0x7fffffff 取小，返回可购上限。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 34（ORIG 130 / ours 122），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/get_nMaxBuyableCount.inc"
}

// ORIG 0x081be658 T get_data（报告 get_data.md ## 反编译 C 全文映射）：
// 解析商城物品 → 材料槽 → 六种单价（>1 件时乘数量并校验上限）→ 持有量校验。
// [b28d-29] N82/N18a：帧 0x20 vs 0x34 + 返回指针缓存槽（-0x1c）与两段中转差；语义集合全等  (ORIG 0x081be658 insn 310->328)
// 返回 0=成功；0x11=物品不存在；1=参数非法；10=余额不足；0x24=公会会长限制。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 265（ORIG 310 / ours 327），残差=N7/N82 帧与 callee-saved 角色/setcc-fold bool 物化岛/P168a④ bool 槽化/P211/N18a；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/get_data.inc"
}

// ORIG 0x081bec5e T statistic_process（报告 statistic_process.md ## 反编译 C 全文映射）：
// 卖价估值入 VALUE_STATISTIC_FIELD(6)、货值入 (0xd)，材料对入 CCubeStatistic(0x6a)。
#include "DisPatcherCore/statistic_process.inc"
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
}

// [b28d-29] N78/N82：ORIG rep stos 结构体清零 vs 4.4 call memset（0x28 字节）+ 帧 0x19c vs 0x190；语义等价  (ORIG 0x081ee59c insn 480->487)
#include "DisPatcherCore/dtor_DisPatcher_RentAvatar.inc"
#include "DisPatcherCore/dispatch_sig_6.inc"
}
#include "DisPatcherCore/read_22.inc"
}
#include "DisPatcherCore/process_22.inc"

#include "DisPatcherCore/dtor_DisPatcher_ExtendAvatar.inc"
#include "DisPatcherCore/dispatch_sig_7.inc"
}
#include "DisPatcherCore/read_23.inc"
}
#include "DisPatcherCore/process_23.inc"

#include "DisPatcherCore/dtor_DisPatcher_SellItem.inc"
#include "DisPatcherCore/dispatch_sig_8.inc"
}
#include "DisPatcherCore/read_24.inc"
}
#include "DisPatcherCore/process_24.inc"
#include "DisPatcherCore/dtor_DisPatcher_RepairEquip.inc"
#include "DisPatcherCore/check_error_14.inc"
}
#include "DisPatcherCore/read_25.inc"
}
// ORIG 0x081c61ea T _ZN22DisPatcher_RepairEquip4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_RepairEquip/send.md 汇编+反编译 C 全文转写）：
// put_header(1,0x19)；errNo==0 → put_byte(1)/getCurCharacInvenR()->get_money()
// put_int/(char)m_8/(ushort)m_a/(ushort)m_c；否则 0/errNo；finalize(true)+Send。
#include "DisPatcherCore/send_13.inc"
}
// ORIG 0x081c6082 T _ZN22DisPatcher_RepairEquip7processEP5CUserR8MSG_BASER9ParamBase
// err>0：SendCmdErrorPacket(cmd=0x19, err&0xff) 返回 -1；err<0：LineFunc(0x1500,pretty,0,acc_id)
// 并返回其值；err==0：G_Store()->repair_equip(user, msg[0xd], (s16)msg+0xe, (u16)msg+0x10,
// [b28d-29] N82：帧差（ORIG sub $0x4c+push edi/esi，4.4 sub $0x34）+ 槽号平移，调用点全等  (ORIG 0x081c6082 insn 83->80)
// 出参 param+0xc) 写 param.m_errNo，成功(==0)时回显 m_8=msg[0xd]、m_a=(u16)msg+0xe；返回 0。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 69（ORIG 83 / ours 80），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_25.inc"
}


#include "DisPatcherCore/dtor_DisPatcher_SetTradeState.inc"
#include "DisPatcherCore/dispatch_sig_9.inc"
}
#include "DisPatcherCore/read_26.inc"
}

// ORIG 0x0823315a W _ZN24DisPatcher_SetTradeState7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部交易状态逻辑（get_state==3 → GetTradeSpace → is_empty/check_trade_possibility →
// change_trade_state / cancel_trade+PutTradeSpace）内嵌于
// dispatch_sig(0x081f6232，报告 docs/class_func_reports/DisPatcher_SetTradeState/dispatch_sig.md)。
#include "DisPatcherCore/process_26.inc"

#include "DisPatcherCore/dtor_DisPatcher_Compound.inc"
#include "DisPatcherCore/dispatch_sig_10.inc"
}
#include "DisPatcherCore/read_27.inc"
}
#include "DisPatcherCore/process_27.inc"

#include "DisPatcherCore/dtor_DisPatcher_DisJointItem.inc"
#include "DisPatcherCore/dispatch_sig_11.inc"
}
#include "DisPatcherCore/read_28.inc"
}
#include "DisPatcherCore/process_28.inc"
#include "DisPatcherCore/_check_disjointable_user_state.inc"
#include "DisPatcherCore/_check_disjointable_item_state.inc"
#include "DisPatcherCore/_check_disjoint_need_slot.inc"
#include "DisPatcherCore/_disjoint.inc"
}


#include "DisPatcherCore/dtor_DisPatcher_Lottery.inc"
#include "DisPatcherCore/dispatch_sig_12.inc"
}
#include "DisPatcherCore/read_29.inc"
}

// ORIG 0x08233280 W _ZN18DisPatcher_Lottery7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部抽奖逻辑（CheckInTrade 拒绝包 0x1d/0xda → lottery_item → 应答包 0x1d →
// 失败时限 3 秒内 addServerHackCnt(0x334) → setLastLotteryTime）内嵌于
// dispatch_sig(0x081f8380，报告 docs/class_func_reports/DisPatcher_Lottery/dispatch_sig.md)。
#include "DisPatcherCore/process_29.inc"

#include "DisPatcherCore/dtor_DisPatcher_ChangeSkill.inc"
#include "DisPatcherCore/check_error_15.inc"
}
#include "DisPatcherCore/read_30.inc"
}
#include "DisPatcherCore/send_14.inc"
}

// ORIG 0x081bffbc T _ZN22DisPatcher_ChangeSkill7processEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_ChangeSkill/process.md ## 反编译 C 全文映射）：
// check_error（stub 恒 0）→ param.m_curSlot/m_newSlot ← msg[0xe]/msg[0xf] →
// [b28d-29] N82/P0：msg/param 形参副本落槽（ORIG 0x10/0x14(%ebp) 直用），4.4 帧 0x28 vs ORIG 0x2c+三 push  (ORIG 0x081bffbc insn 74->69)
// SkillSlot::skill_move(cur, new, get_charac_job)；>0 时 SendCmdErrorPacket(0x1e) 返 -1。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 62（ORIG 74 / ours 68），残差=N7/N82 帧与 callee-saved 角色/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/process_30.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_BuySkill.inc"
#include "DisPatcherCore/check_error_16.inc"
}
#include "DisPatcherCore/check_state_3.inc"
}
// ORIG 0x081bed4a T _ZN19DisPatcher_BuySkill4readER9PacketBufR8MSG_BASE
// [b28d-29] AE：169=169 全等（同上重定位归一）  (ORIG 0x081bed4a insn 169->170)
// 报告：docs/class_func_reports/DisPatcher_BuySkill/read.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 169 / ours 169 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/read_31.inc"
}
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 415（ORIG 311 / ours 302），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/send_15.inc"
}

// ORIG 0x081befe8 T _ZN19DisPatcher_BuySkill7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_BuySkill/process.md 全文转写）：
//   check_error>0 → SendCmdErrorPacket(0x1f, err) 返 -1；<0 → LineFunc(0x3ef, pretty, err, acc_id)。
//   count==1 单条模式：entry0 → master/refund_skill_process，回写 m_354/m_350。
//   count!=1 批量模式：backupSkill 失败 → m_350=-1、m_354=infos[0].m_0（ORIG 读未初始化，忠实保留）
//   返 0；成功则两遍循环（try_* 试算 → 正式 master/refund），info.m_8<0 短路返 0。
//   四个 helper（T 符号）当前无 TU 定义，见 DisPatcherCore.h 声明。
#include "DisPatcherCore/process_31.inc"
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
}

// ORIG 0x081bfc5c T _ZN19DisPatcher_BuySkill20refund_skill_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/refund_skill_process.md 全文转写）：
// 无树门/无权限门（treeKind 存 -0x2c 后未再读取）；refundCount 局部清零后取址；
// SkillSlot::refund_skill(idx, level, job, first, second, (s8)m_1, &refundCount)
// [b28d-29] N82：帧/槽重排（ORIG sub $0x4c+三 push，4.4 sub $0x58），调用点全等  (ORIG 0x081bfc5c insn 73->72)
// 写 m_4，≥0 时 m_8←当前等级。ORIG 不读回 refundCount 值。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 66（ORIG 73 / ours 71），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/refund_skill_process.inc"
}

// ORIG 0x081bfd3c T _ZN19DisPatcher_BuySkill25try_refund_skills_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/try_refund_skills_process.md 全文转写）：
// [b28d-29] N82：帧/槽重排（ORIG sub $0x4c+push edi/esi/ebx，4.4 sub $0x58），助记符序列与调用点全等  (ORIG 0x081bfd3c insn 59->55)
// 与 refund 同参形态调 try_refund_skill；仅写 m_4，无等级回读、无任何门。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 58（ORIG 59 / ours 55），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/try_refund_skills_process.inc"
}

// ORIG 0x081bfdf0 T _ZN19DisPatcher_BuySkill29try_master_new_skills_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/try_master_new_skills_process.md 全文转写）：
// 公会技能 [200,208] 直接返回（cmp $0xd0 jbe → 尾声；与正式 master 相反）；
// [b28d-29] N82/N18b：帧/槽重排 + 比较方向折痕，调用点全等  (ORIG 0x081bfdf0 insn 84->79)
// 非公会走同一树门后调 try_master_new_skills，仅写 m_4，无等级回写、无日志尾部。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 51（ORIG 81 / ours 76），残差=N7/N82 帧与 callee-saved 角色/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/try_master_new_skills_process.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_IncreStatus.inc"
#include "DisPatcherCore/dispatch_sig_13.inc"
}
#include "DisPatcherCore/read_32.inc"
}

// ORIG 0x08232cc2 W _ZN22DisPatcher_IncreStatus7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部加点逻辑（get_short 属性号 → SecuProtectionField GetItemPtr/CheckItem 守卫 →
// <0x99 increase_status / >=0x99 UseQuestItem+cMyTrace）内嵌于
// dispatch_sig(0x081ede34，报告 docs/class_func_reports/DisPatcher_IncreStatus/dispatch_sig.md)。
#include "DisPatcherCore/process_32.inc"
#include "DisPatcherCore/dtor_DisPatcher_QuestAction.inc"
#include "DisPatcherCore/dispatch_sig_14.inc"
}
#include "DisPatcherCore/read_33.inc"
}

// ORIG 0x08232d24 W _ZN22DisPatcher_QuestAction7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部任务动作逻辑（PvPSkilTree 频道拒绝/at(1) 命令码 0x21..0x24 分派 → find_quest/
// setBlindTimeGateStateQuest/quest_action 四参/GM quest flag）内嵌于
// dispatch_sig(0x081ee010，报告 docs/class_func_reports/DisPatcher_QuestAction/dispatch_sig.md)。
#include "DisPatcherCore/process_33.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPos.inc"
#include "DisPatcherCore/check_error_17.inc"
}
#include "DisPatcherCore/read_34.inc"
}
// ORIG 0x081c98c8 T _ZN17DisPatcher_SetPos7processEP5CUserR8MSG_BASER9ParamBase
#include "DisPatcherCore/process_34.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_SetArea.inc"
#include "DisPatcherCore/check_error_18.inc"
}
// ORIG 0x081c91e6 T _ZN18DisPatcher_SetArea4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_SetArea/read.md（汇编+反编译 C 全读）
#include "DisPatcherCore/read_35.inc"
}
// ORIG 0x081c9656 T _ZN18DisPatcher_SetArea4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_SetArea/send.md 汇编+反编译 C 全文转写）：
// m_errNo!=0x7fffffff 且 !=0 时发包：put_header(1,0x26)/0/errNo；errNo==0x44 →
// put_byte((char)m_9)+GetLeftTimeOfForbiddenMove(getCurCharacName())/60000 put_int；
// 否则 put_byte((char)m_8)；finalize(true)+Send。
#include "DisPatcherCore/send_16.inc"
}
// ORIG 0x081c951c T _ZN18DisPatcher_SetArea7processEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 94 / ours 94 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_35.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_Fish.inc"
#include "DisPatcherCore/read_36.inc"
}

// ORIG 0x08232cae W _ZN15DisPatcher_Fish7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
// —— 原始二进制即为空实现（无 process 报告，W 内联弱符号）。
#include "DisPatcherCore/process_36.inc"
#include "DisPatcherCore/dispatch_sig_15.inc"
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
}
// ORIG 0x0826503c T _ZN21DisPatcher_FinishLoad7processEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 113 / ours 113 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_37.inc"
}
// ---------------------------------------------------------------------------
// ORIG W 0x08265540 D1 / 0x08265570 D0 _ZN21DisPatcher_FinishLoadD{1,0}Ev
// —— 无独立报告；objdump：写最终虚表指针后调模板基类实例 D1（0x08266d9e），
// 删除版追加 operator delete。空体由编译器按同形态生成，与同类
// DisPatcher_Fish/DisPatcher_UseSkill 写法一致。
// ---------------------------------------------------------------------------
#include "DisPatcherCore/dtor_DisPatcher_FinishLoad.inc"
#include "DisPatcherCore/check_state_4.inc"
}
// ORIG T 0x08264b66 _ZN21DisPatcher_FinishLoad4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_FinishLoad/read.md ## 汇编 + ## 反编译 C
// 全文转写）。两次 PacketBuf::get_int 分别写 m_0d/m_11（msg+0x0d/+0x11 具名
// 字段）；任一失败即以 LineFunc(0x5f7/0x5f8) 返回（第三、四参恒 0），全成返回 0。
#include "DisPatcherCore/read_37.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_UseSkill.inc"
#include "DisPatcherCore/check_error_20.inc"
}
#include "DisPatcherCore/read_38.inc"
}
#include "DisPatcherCore/send_17.inc"
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
}

// ORIG 0x08286612：get_second_growtype_max_level(first, second) > 0
#include "DisPatcherCore/IsSkillMatchSecondGrowType.inc"
}

// ORIG 0x08286656：恒返 0x339
#include "DisPatcherCore/GetHackType.inc"
}

// ORIG 0x082864fc：find_skill(job, slot)==0 → false；
// 首一转类型命中 / 二转上限>0 / IsStealable(first, second) 任一成立 → true。
#include "DisPatcherCore/IsValidUsingSkill.inc"
}

// ORIG 0x081cadaa T _ZN19DisPatcher_UseSkill7processEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseSkill/process.md ## 反编译 C 全文映射）：
// param.m_errNo=check_error；>=1 直接返回 0；<0 LineFunc(0x1ff2,err,acc_id)；
// ==0 时构造校验信息 → 非法使用记黑客计数（addServerHackCnt(type,1,slot,0)），
// 有队伍则 CParty::use_skill(user, slot, target)，恒返 0。
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 112 / ours 112 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_38.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_DieMob.inc"
#include "DisPatcherCore/dispatch_sig_16.inc"
}

#include "DisPatcherCore/read_39.inc"
}
#include "DisPatcherCore/process_39.inc"

#include "DisPatcherCore/dtor_DisPatcher_DieCharacInDungeon.inc"
#include "DisPatcherCore/check_error_21.inc"
}
#include "DisPatcherCore/check_state_5.inc"
}
#include "DisPatcherCore/read_40.inc"
}
#include "DisPatcherCore/send_18.inc"
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
}

#include "DisPatcherCore/dtor_DisPatcher_UseCoin.inc"
#include "DisPatcherCore/check_error_22.inc"
}
// ORIG 0x081ca5e4 T _ZN18DisPatcher_UseCoin4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_UseCoin/read.md 逐块映射）：get_short → msg+0xd；
// 失败 LineFunc(0x1ebb, PRETTY, 0, 0)。
#include "DisPatcherCore/read_41.inc"
}
#include "DisPatcherCore/send_19.inc"
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
}


#include "DisPatcherCore/dtor_DisPatcher_GiveUpGame.inc"
#include "DisPatcherCore/check_error_23.inc"
}
#include "DisPatcherCore/read_42.inc"
}

// ORIG 0x081c40b4 T _ZN21DisPatcher_GiveUpGame7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_GiveUpGame/process.md 全文转写）。
// err>0 返 -1；<0 LineFunc(0xff7,pretty,0,acc_id) 返回其值；==0 按 state 分派：
// 10=getDeathTower 非空 handleLeaveUser；0xc=CBossTowerFunc::GiveUpGame；
// 0xd=advancealtar::Manager::giveUpGame；其它=GetParty 非空 giveup_game(user,true,false,false)。
// 尾部返回 check_error 原值（ORIG mov -0x14,%eax）。param 不写。
#include "DisPatcherCore/process_42.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_GetItem.inc"
#include "DisPatcherCore/check_error_24.inc"
}
// ORIG 0x081c335c T _ZN18DisPatcher_GetItem4readER9PacketBufR8MSG_BASE
// 报告：docs/class_func_reports/DisPatcher_GetItem/read.md（汇编+反编译 C 全读）
#include "DisPatcherCore/read_43.inc"
}
// ORIG 0x081c3ce8 T _ZN18DisPatcher_GetItem4sendEP5CUserR9ParamBase
// （报告 DisPatcher_GetItem/send.md 逐块映射）：param.m_errNo(+0x4)
// !=0x7fffffff；==0 → SendCmdOkPacket(0x2e)；否则 SendCmdErrorPacket(0x2e, err&0xff)。
#include "DisPatcherCore/send_20.inc"
}
// ORIG T 0x0889209d（定义 OnlyOrigBatch11.cpp:141）：奇偶门，返回 1=通过 0=失败。
// [b28d-29] N82/P211：帧 0x50 vs 0x60 + 参数副本槽 + store 装载序，调用点全等  (ORIG 0x081c35c0 insn 484->511)
#include "DisPatcherCore/checkGetItemPacket.inc"
#include "DisPatcherCore/process_43.inc"
}


#include "DisPatcherCore/dtor_DisPatcher_UseStack.inc"
#include "DisPatcherCore/dispatch_sig_17.inc"
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
}
// ORIG 0x082331bc W _ZN19DisPatcher_UseStack7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制
// 即为空实现（read 同为空）。UseStack 全部业务（读包/校验/应答 0x2f 系列）内嵌于
// dispatch_sig(0x081f63c0，见 docs/class_func_reports/DisPatcher_UseStack/dispatch_sig.md)。
#include "DisPatcherCore/process_44.inc"


#include "DisPatcherCore/dtor_DisPatcher_MoveMap.inc"
#include "DisPatcherCore/check_error_25.inc"
}
#include "DisPatcherCore/check_state_6.inc"
}
#include "DisPatcherCore/read_45.inc"
}
// ORIG 0x081c4b7a T _ZN18DisPatcher_MoveMap20CheckTrapBitsHackCntEP5CUserR12MSG_MOVE_MAP
// msg.m_18 ushort 非 7 的倍数 → 记 0x1f9；累计 >4 时对队伍其余成员记 0x1fa、
// [b28d-29] N-P223/N-JMP：唯一残差=cond 尾 4.4 多保留 jmp 岛（ORIG 0x081c4bb5 je 直落 epilogue，4.1 线程化折掉），ndiff 20->1  (ORIG 0x081c4b7a insn 108->111)
// 落库 DB_ReqSaveHackUserPunish 并 DisConnSig(0x18,1,0) 强制断线。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 1（ORIG 107 / ours 108），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/CheckTrapBitsHackCnt.inc"
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
}
// ORIG 0x081c5330 T _ZN18DisPatcher_MoveMap7processEP5CUserR8MSG_BASER9ParamBase
// [b28d-29] P0 清绑（原 register asm 绑定已删，N96）：清绑后 4.4 恒槽化/单段直落 callee-saved 不可造 ⇒ N18a/N96 封顶（绑定形为唯一通路，P0 禁）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 120 / ours 120 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/process_45.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_SetPlayResult.inc"
#include "DisPatcherCore/dispatch_sig_18.inc"
}
#include "DisPatcherCore/read_46.inc"
}
#include "DisPatcherCore/process_46.inc"

#include "DisPatcherCore/dtor_DisPatcher_DropItem.inc"
#include "DisPatcherCore/check_error_26.inc"
}
// ORIG 0x081c2bee T _ZN19DisPatcher_DropItem4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_DropItem/read.md 汇编+反编译 C 全文转写）：
// 依次 get_short(+0x0d)/get_short(+0x0f)/get_byte(char&)(+0x11)/get_short(+0x12)/
// get_int(+0x14)/get_byte(char&)(+0x18)；失败分别 LineFunc 0xd0c..0xd12；成功返回 0。
#include "DisPatcherCore/read_47.inc"
}
#include "DisPatcherCore/send_21.inc"
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
}


#include "DisPatcherCore/dtor_DisPatcher_UseEquip.inc"
#include "DisPatcherCore/check_error_27.inc"
}
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 21 / ours 21 insn 全等（AE 口径，重定位归一）。
#include "DisPatcherCore/read_48.inc"
}
#include "DisPatcherCore/send_22.inc"
}
// ORIG 0x081c0e4a T _ZN19DisPatcher_UseEquip7processEP5CUserR8MSG_BASER9ParamBase
// err=check_error；>0：SendCmdErrorPacket(0x33, err&0xff) 返回 -1；==0：
// m_errNo=getCurCharacInvenW()->use_equip((int)(sbyte)msg.m_0d)，m_8=msg.m_0d，
// 返回 err。
#include "DisPatcherCore/process_48.inc"
}


#include "DisPatcherCore/dtor_DisPatcher_MakePVP.inc"
#include "DisPatcherCore/ExtractPacket.inc"
}
// [b28d-29] N82/P183：临时对象构造块（lea -0x10）位置前移 3 指令，difflib 块序伪差；语义集合全等  (ORIG 0x081f803e insn 103->104)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 40（ORIG 103 / ours 105），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_19.inc"
}
#include "DisPatcherCore/read_49.inc"
}

// ORIG 0x0823321e W _ZN18DisPatcher_MakePVP7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部建房逻辑（ExtractPacket(0x081f81ba) 解包 MSG_MAKE_PVP_ROOM → CSyncSlangFilter::
// HasSlang 拒绝 0x35/0x9f → MakePVPRoom(cmd=0x35, mode=local_15?6:2)）内嵌于
// dispatch_sig(0x081f803e，报告 docs/class_func_reports/DisPatcher_MakePVP/dispatch_sig.md)。
#include "DisPatcherCore/process_49.inc"
#include "DisPatcherCore/dtor_DisPatcher_EnterPVP.inc"
#include "DisPatcherCore/dispatch_sig_20.inc"
}
   int DisPatcher_EnterPVP::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    // ORIG 0x08232b7e: push ebp; mov ebp,esp; mov eax,0x0; pop ebp; ret（恒返 0，10 字节，报告缺失/objdump 转写）
    return 0;
}
#include "DisPatcherCore/process_50.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPSeat.inc"
#include "DisPatcherCore/dispatch_sig_21.inc"
}
#include "DisPatcherCore/read_50.inc"
}
#include "DisPatcherCore/process_51.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPReady.inc"
#include "DisPatcherCore/check_error_28.inc"
}
#include "DisPatcherCore/read_51.inc"
}
#include "DisPatcherCore/send_23.inc"
}
// ORIG 0x081c9a14 T _ZN22DisPatcher_SetPVPReady7processEP5CUserR8MSG_BASER9ParamBase
#include "DisPatcherCore/process_52.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_SetPVPTeam.inc"
#include "DisPatcherCore/dispatch_sig_22.inc"
}
#include "DisPatcherCore/read_52.inc"
}
#include "DisPatcherCore/process_53.inc"

#include "DisPatcherCore/dtor_DisPatcher_DiePvP.inc"
#include "DisPatcherCore/dispatch_sig_23.inc"
}
#include "DisPatcherCore/read_53.inc"
}
#include "DisPatcherCore/process_54.inc"

#include "DisPatcherCore/dtor_DisPatcher_PvPTimeout.inc"
#include "DisPatcherCore/check_error_29.inc"
}
// ORIG 0x081c5d78 T _ZN21DisPatcher_PvPTimeout4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_PvPTimeout/read.md 逐块映射）：循环 i=0..7 共 8 次
// get_int → msg+0xd+i*4；失败 LineFunc(0x14a6, PRETTY, 0, 0)。
#include "DisPatcherCore/read_54.inc"
}
#include "DisPatcherCore/send_24.inc"
}
// ORIG 0x081c5dec T _ZN21DisPatcher_PvPTimeout7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_PvPTimeout/process.md 全文转写）
// err=check_error（ORIG 0x081c5ebe：get_state!=7 → 0x13）；>0 返回 -1；<0
// LineFunc(0x14b7,pretty,0,acc_id)；==0：room=user->GetPVPRoom()（空则
// LineFunc(0x14bc,pretty,0,acc_id)），否则 ret=room->time_out((int*)(msg+0xd))
// —— msg+0xd 即 read() 读入的 8 个 seat int 数组首址 —— param+0x4=ret，返回 err。
#include "DisPatcherCore/process_55.inc"
}


#include "DisPatcherCore/dtor_DisPatcher_EndPvPResult.inc"
#include "DisPatcherCore/dispatch_sig_24.inc"
}

#include "DisPatcherCore/read_55.inc"
}
#include "DisPatcherCore/process_56.inc"
#include "DisPatcherCore/dtor_DisPatcher_ResPvpRank.inc"
#include "DisPatcherCore/ExtractPacket_1.inc"
}
// [b28d-29] N82/P197③：ORIG 两 dead 局部预清 + 槽序（-0x1c/-0x18/-0x10/-0xc），4.4 合并复用致全帧平移；语义集合全等  (ORIG 0x081f05ea insn 197->197)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 72（ORIG 197 / ours 197），残差=setcc-fold bool 物化岛/P168a④ bool 槽化；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/SavePvpRank.inc"
}

#include "DisPatcherCore/dispatch_sig_25.inc"
}
#include "DisPatcherCore/read_56.inc"
}
// ORIG 0x08232e98 W _ZN21DisPatcher_ResPvpRank7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现（objdump 核实），全部逻辑位于 dispatch_sig(0x081f0282)：state==7 → MSG_RES_PVP_RANK ctor → ExtractPacket(0x081f039e)/SavePvpRank(0x081f05ea) + LineFunc 断言
#include "DisPatcherCore/process_57.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetPVPMapIndex.inc"
#include "DisPatcherCore/dispatch_sig_26.inc"
}
#include "DisPatcherCore/read_57.inc"
}
#include "DisPatcherCore/process_58.inc"

#include "DisPatcherCore/dtor_DisPatcher_DebugCommand.inc"
#include "DisPatcherCore/read_58.inc"
}

// ORIG 0x08233408 W _ZN23DisPatcher_DebugCommand7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现。
// 全部 GM 命令逻辑（GetCurrentGmMode/IsGm 门禁 → get_packet(0) 取 MSG_DEBUG_COMMAND →
// 0x00..0x30 switch 分派 _debugCommand* 家族 → SendCmdPacket(0x41) 回显）内嵌于
// dispatch_sig(0x081fc4de，报告 docs/class_func_reports/DisPatcher_DebugCommand/dispatch_sig.md；
// 各 _debugCommand*.md 同目录)。

#include "DisPatcherCore/process_59.inc"

#include "DisPatcherCore/dtor_DisPatcher_AddFriend.inc"
#include "DisPatcherCore/check_error_30.inc"
}
#include "DisPatcherCore/read_59.inc"
}

#include "DisPatcherCore/send_25.inc"
}
// ORIG 0x081cb2e2 T _ZN20DisPatcher_AddFriend7processEP5CUserR8MSG_BASER9ParamBase：
// 薄包装，尾调 process_packet_jpn 并透传返回值（汇编 leave/ret 直传 eax）。
#include "DisPatcherCore/process_60.inc"
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
}

#include "DisPatcherCore/dtor_DisPatcher_RemoveFriend.inc"
#include "DisPatcherCore/check_error_31.inc"
}
#include "DisPatcherCore/read_60.inc"
}
#include "DisPatcherCore/send_26.inc"
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
}

#include "DisPatcherCore/dtor_DisPatcher_Cera.inc"
#include "DisPatcherCore/dispatch_sig_27.inc"
}
#include "DisPatcherCore/read_61.inc"
}
#include "DisPatcherCore/process_62.inc"

#include "DisPatcherCore/dtor_DisPatcher_EnterWarRoom.inc"
#include "DisPatcherCore/dispatch_sig_28.inc"
}
#include "DisPatcherCore/read_62.inc"
}
#include "DisPatcherCore/process_63.inc"

#include "DisPatcherCore/dtor_DisPatcher_SetWarRoomSeatState.inc"
#include "DisPatcherCore/dispatch_sig_29.inc"
}
#include "DisPatcherCore/read_63.inc"
}
#include "DisPatcherCore/process_64.inc"

#include "DisPatcherCore/dtor_DisPatcher_DieWarRoomCharacter.inc"
#include "DisPatcherCore/dispatch_sig_30.inc"
}
#include "DisPatcherCore/read_64.inc"
}
#include "DisPatcherCore/process_65.inc"

#include "DisPatcherCore/dtor_DisPatcher_StartWarRoomTemp.inc"
#include "DisPatcherCore/dispatch_sig_31.inc"
}
#include "DisPatcherCore/read_65.inc"
}
#include "DisPatcherCore/process_66.inc"

#include "DisPatcherCore/dtor_DisPatcher_ReportClient4Hack.inc"
#include "DisPatcherCore/dispatch_sig_32.inc"
}
#include "DisPatcherCore/read_66.inc"
}
#include "DisPatcherCore/process_67.inc"
#include "DisPatcherCore/ST_Guild_War_Rank_Info.inc"
}
// [b28d-solo-DisPatcherCore] 修复：m_rankInfo 由 char[0x14a] 改真实元素数组
// ST_Guild_War_Rank_Info[10]（头 R2 广播），ctor 删手写 placement-new 循环
// ⇒ 编译器隐式数组成员构造复现 ORIG P151 倒计数循环（esi=9→-1、ebx 步进 0x21，
// 无 _ZnwjPv/size 装载）；N83 的 placement-new 多 2 条消除。
#include "DisPatcherCore/Packet_Notice_Guild_War_Info.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_RequestGuildWarInfo.inc"
#include "DisPatcherCore/dispatch_sig_33.inc"
}
#include "DisPatcherCore/read_67.inc"
}
#include "DisPatcherCore/process_68.inc"

#include "DisPatcherCore/dtor_DisPatcher_PvpHeartBeat.inc"
#include "DisPatcherCore/read_68.inc"
}
#include "DisPatcherCore/dispatch_sig_34.inc"
}
#include "DisPatcherCore/process_69.inc"

#include "DisPatcherCore/dtor_DisPatcher_MultiBoxLottery.inc"
#include "DisPatcherCore/dispatch_sig_35.inc"
}
#include "DisPatcherCore/read_69.inc"
}
#include "DisPatcherCore/process_70.inc"
#include "DisPatcherCore/dtor_DisPatcher_UdpCharacteristic.inc"
#include "DisPatcherCore/dispatch_sig_36.inc"
}
#include "DisPatcherCore/read_70.inc"
}
#include "DisPatcherCore/process_71.inc"

#include "DisPatcherCore/dtor_DIspatcher_OneDayLetheTicket.inc"
#include "DisPatcherCore/dispatch_sig_37.inc"
}
#include "DisPatcherCore/read_71.inc"
}
#include "DisPatcherCore/process_72.inc"

#include "DisPatcherCore/dtor_Dispatcer_polic_one_to_one_chat_disable.inc"
#include "DisPatcherCore/dispatch_sig_38.inc"
}
#include "DisPatcherCore/read_72.inc"
}
#include "DisPatcherCore/process_73.inc"

#include "DisPatcherCore/dtor_DisPatcher_CheckSecurityProtection.inc"
#include "DisPatcherCore/check_error_32.inc"
}
#include "DisPatcherCore/read_73.inc"
}
#include "DisPatcherCore/send_27.inc"
}

// ORIG 0x0826481a T _ZN34DisPatcher_CheckSecurityProtection7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_CheckSecurityProtection/process.md 全文转写）：
// sub $0x28 帧；将 CSecu_ProtectionField::Check(GlobalData::s_pSecuProtectionField,user,0x38)
// 返回值低字节写入 param_3[4]（CheckSecurityProtection 首数据字节 @+0x4 = m_pad[0]），恒返回 0。
#include "DisPatcherCore/process_74.inc"
}

#include "DisPatcherCore/dtor_DisPatcher_EventDungeon_DestoryObject.inc"
#include "DisPatcherCore/check_error_33.inc"
}
#include "DisPatcherCore/read_74.inc"
}
#include "DisPatcherCore/send_28.inc"
}
// ORIG 0x0812bd8c T _ZN37DisPatcher_EventDungeon_DestoryObject7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_DestoryObject/process.md 全文转写：
//   局部引用槽 -0x8=msg、-0x4=param；*(u16*)(param+0x4)=*(u16*)(msg+0xe)；check_error；返回 0。
//   模板实例化中 ParamBase 栈对象仅 4 字节 vptr（ORIG dispatch_template 0x824aa8c 实证：
//   param@-0x1d 与 msg@-0x19 相邻），param+0x4 写入与相邻 msg 存储重叠为 ORIG 原始行为，忠实转写。）
#include "DisPatcherCore/process_75.inc"
}

// ORIG 0x0812c1c6 W _ZN33DisPatcher_EventDungeon_ClearRoomD1Ev/D2Ev/D0Ev（三地址相同）
// objdump 实测 D1：写 vptr=0x8b66068 → 调 IPacketDispatcher<...>::D1 → 恒假 delete 守卫
// —— 即编译器生成的空析构，{} 与其逐指令等价。
#include "DisPatcherCore/dtor_DisPatcher_EventDungeon_ClearRoom.inc"
#include "DisPatcherCore/check_error_34.inc"
}

// ORIG 0x0812beba T _ZN33DisPatcher_EventDungeon_ClearRoom7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/process.md 全文转写，
//   含汇编写入次序：param+0x4←msg+0xe、param+0x8←msg+0x12、param+0x6←msg+0x10）；check_error 恒返 0。
#include "DisPatcherCore/process_76.inc"
}

// ORIG 0x0812be82 T _ZN33DisPatcher_EventDungeon_ClearRoom4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/read.md 全文转写）：
// ORIG 汇编含 &msg 的局部落栈（-0x4c(%ebp) spill/reload 对），以具名局部复现。
#include "DisPatcherCore/read_75.inc"
}

// ORIG 0x0812bf16 T _ZN33DisPatcher_EventDungeon_ClearRoom4sendEP5CUserR9ParamBase
// （docs/class_func_reports/DisPatcher_EventDungeon_ClearRoom/send.md 全文转写）：
// put_header(1,0x21c) → put_byte(1) → put_binary(pParam,0xc) → finalize(true) →
// CUser::Send；PacketGuard 栈对象析构（ORIG 含 EH 清理路径）。
// 反编译 C 首行 local_10=param_2：param 引用先落栈（-0xc），guard 位于 -0x18。
#include "DisPatcherCore/send_29.inc"
}

#include "DisPatcherCore/dtor_Dispather_RegenerationRandomOption.inc"
#include "DisPatcherCore/check_state_7.inc"
}
#include "DisPatcherCore/read_76.inc"
}
#include "DisPatcherCore/send_30.inc"
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
}
}  // namespace statistc_proxy



#include "DisPatcherCore/dtor_DispatcherResetRandomOption.inc"
#include "DisPatcherCore/read_77.inc"
}

// ORIG 0x08682d3a T _ZN15cUserHistoryLog12RandomOptionERK10Inven_ItemN18randomOptionReason1TE
// （docs/class_func_reports/cUserHistoryLog/RandomOption.md + objdump 全文转写）：
// m_user->LogHistory("RandomOptionItem,%d,%02X×14,%d", item.m_addInfo,
// [b28d-29] N82/P183：ORIG 无逐字段临时缓存，4.4 新增 -0x50..-0x48 三槽（帧 0x9c vs 0xac）；语义集合全等  (ORIG 0x08682d3a insn 102->119)
//   item.m_random.m_random 的 14 字节（+0x25..+0x32）逐字节, reason)。无 m_user 空检查。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 124（ORIG 102 / ours 118），残差=N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/RandomOption.inc"
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
}

// ============================================================================
// ORIG 0x0865798a T _ZN5CUser15serviceOfShusiaEv —— 休沙疲劳恢复服务。
// 幽灵 0x13；疲劳>99 0x12；Restrict(1,0x16) 0xd1；余额<价 0x16；
// 扣费 use_money(price,0xa,true)、AddValueStatistic(0x10,user,price)、
// setCurCharacStamina(100)、UseShusiaService((short)m_level)，成功返 0。
// 定义宿主为本 TU（CUser.cpp 由 RestoreCUserStatMethods 独占，经广播协调）。
// ============================================================================
#include "DisPatcherCore/serviceOfShusia.inc"
}

// ============================================================================
// G2 角色域三个 CUser 叶子方法（定义宿主为本 TU，沿用 serviceOfShusia 先例；
// CUser.cpp 由 RestoreCUserStatMethods 独占，声明已按广播加入 CUser.h）。
// ============================================================================

// ORIG 0x0822fc06 W _ZN5CUser29SetLoadRentalInfoFromExchangeEb：写 +0x6ef91 字节
#include "DisPatcherCore/SetLoadRentalInfoFromExchange.inc"
}

// ORIG 0x0868e672 T _ZN5CUser25RemoveCharacMercenaryInfoEj：转调雇佣兵管理器
#include "DisPatcherCore/RemoveCharacMercenaryInfo.inc"
}

// ORIG 0x0864a956 T _ZN5CUser31GetGuildIdBeforeDeleteCharacterEi：
// 返回 m_characList[slot].m_guildId（_Charac_info +0xdbe）
#include "DisPatcherCore/GetGuildIdBeforeDeleteCharacter.inc"
}


// ORIG 0x08230844 W _ZN5CUser31SetDeleteEventDungeonInvitationEb：
// 写 m_deleteEventDungeonInvitation 字节（objdump 逐指令转写；SelectDungeon::process 对全队调用）。
#include "DisPatcherCore/SetDeleteEventDungeonInvitation.inc"
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
}
// [b28d-29] N82/P183：帧 0x1b4 vs 0x1b8 + ORIG 无 -0x20 指针缓存而 4.4 新增，槽平移；语义集合全等  (ORIG 0x08217046 insn 418->368)

// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 239（ORIG 418 / ours 367），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
#include "DisPatcherCore/dispatch_sig_44.inc"
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
}
