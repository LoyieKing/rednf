// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W8LT_DBTask/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "W8LT_DBTask.h"
#include "GameBasicsScripts.h" // DB_UpdateAuraAvatarOption（权威声明点）
#include "DBDispatcher.h"
#include "Stream.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"       // MsgQueueMgr::QUEUE_IDX（DB_LoadPowerWarStatueInfo::dispatch）
#include "CAchievement.h"     // stAchievement_DBSaveData（DB_SaveAchievement::dispatch 协议头）
#include "CPremiumLetheManager.h"  // 静态状态常量（DB_UpdateOneDayLetheSkill）
#include "CPremiumLetheManager.h"  // 静态状态常量（DB_UpdateOneDayLetheSkill）
#include "WindowsPackFiles.h" // CCompressedBlob（put_compressed_blob_data）
#include "SigTypes_A.h"      // SIG_CHECK_ADD_FRIEND（DB_CheckAddFriend::dispatch 协议头）
#include "SigTypes_C.h"      // SIG_LOAD_KILL_MONSTER_INFO（DB_SaveKillMonsterInfo::dispatch 载荷）
#include "SigTypes_D.h"    // SIG_QUEST_SHOP_DATA（DB_UpdateQuestShopBuyingStatusList::dispatch 载荷）
#include "SigTypes_E.h"     // SIG_SELECT_SERVER_MESSAGE（DB_LoadServerMessage::dispatch 出站头）
#include "DB_AvatarChangeOwner.h"  // DB_AvatarChangeOwner 权威声明（dispatch ORIG 0x084004bc）
#include "DB_InsertAccountFirstLoginFlag.h"
#include "DB_InsertAccountFirstLoginDailyFlag.h"
#include "DB_UpdatePurchaseCashItemBonus.h"
#include "SigTypes_B.h"      // SIG_ITEM_DICTIONARY_FLAG/SIG_IN_GAME_ADVERTISEMENT_LOG（本 TU dispatch 载荷）
#include "GamePODTypes.h"   // stAvatarEmblemInfo_t（DB_AvatarChangeStat::dispatch 值对象）
#include "GamePODTypes.h"   // stAvatarEmblemInfo_t（DB_AvatarChangeStat::dispatch 值对象）
#include "OS_API.h"          // OS_API::GetDateTimeTick（DB_UpdateCreatureItem::GetField）
#include "RefPvpGrade.h"     // RefPvpGrade（DB_UpdatePvPGrade::dispatch：getPvP* 族）
#include "MySQL.h"           // MySQL::{set_query,exec,getAffectedRowCount,blob_to_str,getDBError}
#include "GlobalData.h"      // GlobalData::{s_db_mgr,s_systemTime_}
#include "CSystemTime.h"     // CSystemTime::getCurSec（DB_UpdateBingoEvent::dispatch）
#include "CSimpleSSO.h"     // WongWork::CSimpleSSO::updateHumanCertify（DB_ForceSaveSimpleSSO::dispatch）
#include <stdarg.h>          // va_list（AddFieldString）
#include <algorithm>         // std::max<int>（DB_UpdatePvPGrade::dispatch 等级钳制）
#include "CClientCrashDownLogStatistic.h"  // GetInstanceClientCrashDownLogStatistic（DB_TimerStatisticsLog::dispatch）
#include "DB_AccountCargoSync.h"   // DB_CreateAccountCargo/DB_DeleteAccountCargo 权威声明（ORIG 单一类）
#include "W8LT_EventTail.h"        // DB_InsertEventStageLog 等权威声明（ORIG 单一类）
#include "DB_LogBuyItem.h"        // DB_LogBuyItem 权威声明（P104 跨 TU 统一：W8LT/CUser）
#include "CDataManager.h"        // G_CDataManager / get_event_script_mng（组 B dispatch）
#include "DB_LoadCharac.h"       // DB_LoadCharac::_GetTagCharacInfo / TAG_CHARAC_DATA
#include "DB_LoadInventory.h"    // DB_LoadInventory::_GetTagCharacInven / _GetUserAvatar
#include "GameTypes.h"           // cMyTrace
#include "CInGameAdvertisementManager.h"  // InGameADManagerInstance（DB_NoticeIngameAdvertisement::dispatch）
// ORIG T 0x081090b4（u32 版；权威声明见 CInventory.h:535，本 TU 同款局部声明）
char* NumberToString(unsigned int value, int radix);
// ORIG 各 TU 一份 _ZL23GUILD_NAME_DOUBLE_CHECK（r 0x2）：公会名重复检查应答值。
static const int GUILD_NAME_DOUBLE_CHECK = 2;
#include "W8LT_DBTask/GetCurrentResetBaseTime.inc"
// ORIG T _Z22getComboSkillTableName20ENUM_SKILL_TREE_KIND @0x0844a38b
// （定义 DB_LoadEtc.cpp:2029；DB_UpdateComboSkill::setComboSkill 消费）。
char const* getComboSkillTableName(ENUM_SKILL_TREE_KIND kind);
// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】类与本类 dispatch 定义置于 TU 最前，使
// `_GLOBAL__I_` 名等于 ORIG 的 `_ZN29CLocalChina_DB_BreakAwayReset8dispatchEiiP6Stream`（P327）。
// 语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
class CLocalChina_DB_BreakAwayReset : public DBDispatcher
{
public:
    CLocalChina_DB_BreakAwayReset() {}   // ORIG C1 0x844fb92 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08127e84
};

#include "W8LT_DBTask/dispatch.inc"


// WongWork::DBCommon::GetIdentity —— ORIG T 0x083f9ad4，定义归
// LinkClosureB_Misc.cpp（TU 局部声明同款，DB_MailBox_Req_Send.cpp 同法）。
namespace WongWork {

class DBCommon
{
public:
    static unsigned int GetIdentity(MySQL* db);  // ORIG T 0x083f9ad4
};

}  // namespace WongWork

#pragma pack(push, 1)
struct DB_PartyEntriesView
{
    char m_pad[0x160];
    unsigned char m_nat0; int m_ping0; int m_mtu0; char m_pad0[4];
    unsigned char m_nat1; int m_ping1; int m_mtu1; char m_pad1[4];
    unsigned char m_nat2; int m_ping2; int m_mtu2; char m_pad2[4];
    unsigned char m_nat3; int m_ping3; int m_mtu3; char m_pad3[4];
    unsigned char m_nat4; int m_ping4; int m_mtu4; char m_pad4[4];
    unsigned char m_nat5; int m_ping5; int m_mtu5; char m_pad5[4];
    unsigned char m_nat6; int m_ping6; int m_mtu6; char m_pad6[4];
    unsigned char m_nat7; int m_ping7; int m_mtu7; char m_pad7[4];
};
// m_tryEntries[idx] 行内 +6 的 int（0x2e = 0x20 + 0xe）：P161 组件视图
// 折叠为 ORIG 的 `lea (sig+idx*10)` → `add $0x20` → `mov 0xe(%eax)` 两步。
struct DB_TryNatSection
{
    char m_pad[0xe];
    int m_f;
};
struct DB_TryNatView
{
    char m_pad[0x20];
    DB_TryNatSection m_sec;
};
#pragma pack(pop)
// ---- DB_SaveExpandEquipslot 存档视图（ORIG 0x843febe/0x843ff48）----
// SIG_SAVE_EXPAND_CHARAC（0x89c，pack(1) 权威布局见 CExpandEquipslot.cpp）：
// charac_no(+0x00) + Inven_Item m_items[0x24](+0x04..+0x897) +
// 两 switch(+0x898/+0x899)。_SaveExpandEquipslot 压缩前 0x898 字节
// （=SIG_SAVE_EXPAND_EQUIPSLOT 段），_SaveCharacStat 读尾部两 switch 字节。
struct SIG_SAVE_EXPAND_CHARAC_STAT
{
    unsigned char m_channelEquipslotSwitch;  // +0x00（SQL channel_equipslot_switch）
    unsigned char m_expandEquipslotSwitch;   // +0x01（SQL expand_equipslot_switch）
};

// ---- DB_SavePvp::SavePvp 的 SQL 字段视图（SIG_SAVE_PVP m_result 前 0x68）----
// ORIG 0x84172c8 逐偏移读：win/lose/pvp_point/pvp_grade/15×avg/pvp_count/
// win_point/play_time/play_count（+0x0c 跳过），SQL 25 占位见下。
#pragma pack(push, 1)
struct DB_SavePvpResultView
{
    int m_win;            // +0x00 win
    int m_lose;           // +0x04 lose
    int m_pvpPoint;       // +0x08 pvp_point
    int m_pad0c;          // +0x0c（SQL 未用，ORIG 不读）
    int m_pad10;          // +0x10（SQL 未用，ORIG 不读）
    int m_pvpGrade;       // +0x14 pvp_grade
    int m_avg[15];        // +0x18..+0x50 avg_kill_count..avg_hit_penalty
    int m_pvpCount;       // +0x54 pvp_count
    int m_winPoint;       // +0x58 win_point
    int m_pad5c;          // +0x5c（SQL 未用，ORIG 不读）
    int m_playTime;       // +0x60 play_time（SQL 第 24 项）
    int m_playCount;      // +0x64 play_count（SQL 第 23 项）
};
#pragma pack(pop)

// ---- DB_UpdateCharacterDay 的 SIG_UPDATE_CHARACTER_DAY_EVENT 载荷视图 ----
// 共享头 SigTypes_E.h 仅为 m_pad[0x8] 占位；ORIG 0x8435d12 读 +0x00(uint m_id)、
// +0x04(ushort entries_count)；写入方 W7_UserOnlyC.cpp 写 get_acc_id/+4(day)。
#pragma pack(push, 1)
struct DB_UpdateCharacterDayView
{
    unsigned int m_mId;         // +0x00
    unsigned short m_entries;   // +0x04
};
#pragma pack(pop)
// ---- DB_SaveInformNotice::dispatch 载荷视图（InformNoticeFlagData 0x14）----
// ORIG 0x8446fcc：+0x00 m_id(uint，NumberToString(,0))、+0x04 起 inform_notice
// 字符串（escape_string 转义后入 SQL '%s'）。
#pragma pack(push, 1)
struct DB_InformNoticeView
{
    unsigned int m_mId;         // +0x00
    char m_informNotice[0x10];  // +0x04
};
#pragma pack(pop)
static_assert(sizeof(DB_InformNoticeView) == 0x14,
              "DB_InformNoticeView ABI drift");

// ---- DB_AvatarColorChange::dispatch 载荷视图（SIG_AVATAR_COLOR_CHANGE 0xc）----
// ORIG 0x8446944：+0x00 charac_no(int)、+0x04 ui_id(int)、+0x08 color1(ushort)、
// +0x0a color2(ushort)；SQL 实参序 color1/color2/ui_id/charac_no。
struct DB_AvatarColorChangeView
{
    int m_characNo;             // +0x00
    int m_uiId;                 // +0x04
    unsigned short m_color1;    // +0x08
    unsigned short m_color2;    // +0x0a
};
// ---- DB_AvatarOptionChange::dispatch 载荷视图（SIG_AVATAR_OPTION_CHANGE 0xc）----
// ORIG 0x844687e：+0x00 ui_id(uint)、+0x04 charac_no(byte，movzbl 零扩展)、
// +0x08 ability_no(uint)；SQL 实参序 charac_no/ui_id/ability_no
// （"upDate user_items set ability_no=%d where ui_id=%d and charac_no=%d"）。
struct DB_AvatarOptionChangeView
{
    unsigned int m_uiId;        // +0x00
    unsigned char m_characNo;   // +0x04
    unsigned int m_abilityNo;   // +0x08
};
static_assert(sizeof(DB_AvatarOptionChangeView) == 0xc,
              "DB_AvatarOptionChangeView ABI drift");

// ---- DB_CreateAccountCargo::dispatch 回包视图（SIG_CREATE_ACCOUNT_CARGO_RESULT 0x8）----
// ORIG 0x843ac52：memset 0x8 清零；成功写 +0x00=0x0、+0x04=capacity(uint)；
// 已存在写 +0x00=0x14（movb）；回包经 MsgQueueMgr::put(DISPATCH_Q) 出队。
struct DB_AccountCargoResultView
{
    unsigned char m_result;     // +0x00（0x0 创建成功 / 0x14 已存在）
    unsigned int m_capacity;    // +0x04
};

// ---- DB_SaveRedeemItemList::dispatch 载荷视图（SIG_REDEEMINFO_LIST_TO_DB 0x59c）----
// ORIG 0x840e074：仅读 +0x00 characNo(uint，SQL charac_no=%u)；压缩源为
// sig 全 0x59c 字节（characNo@0 / count@4 / entries[21]@8，见 StubS3A_deps.cpp）。
struct DB_RedeemInfoView
{
    unsigned int m_characNo;    // +0x00
};

// ---- DB_InsertEventStageLog::dispatch 载荷视图（SIG_EVENT_STAGE_LOG 0x78）----
// ORIG 0x8449144：+0x00 event_id(int)、+0x04 m_id(uint，NumberToString(,0))、
// +0x08 charac_no(int)、+0x0c server_info(int)、+0x10 stage(int)、
// +0x14 stage_result(char*，SQL '%s')。写入方 SeriaRoomAniDecoEvent.cpp fill_stage_sig
// 写前 0x14 字节 + 内嵌文本（<100，见 questClearLog/buyItemLog）。
#pragma pack(push, 1)
struct DB_EventStageLogView
{
    int m_eventId;              // +0x00
    unsigned int m_mId;         // +0x04
    int m_characNo;             // +0x08
    int m_serverInfo;           // +0x0c
    int m_stage;                // +0x10
    char m_stageResult[0x64];   // +0x14（内嵌文本区）
};
#pragma pack(pop)
static_assert(sizeof(DB_EventStageLogView) == 0x78,
              "DB_EventStageLogView ABI drift");

// ---- DB_AvatarExtendPeriod::ExtendPeriod 载荷（SIG_AVATAR_RENEW，7 字节）----
// ORIG 0x83ffbb4：dispatch 经 get_binary(7) 解包，+0x00 charac_no(uint)、
// +0x04 slot(byte)、+0x05 days(ushort)。类型名必须为全局 SIG_AVATAR_RENEW
// 以匹配 ORIG mangled _ZN21DB_AvatarExtendPeriod12ExtendPeriodER16SIG_AVATAR_RENEW。
#pragma pack(push, 1)
struct SIG_AVATAR_RENEW
{
    unsigned int m_characNo;    // +0x00
    unsigned char m_slot;       // +0x04
    unsigned short m_days;      // +0x05
};
#pragma pack(pop)
static_assert(sizeof(SIG_AVATAR_RENEW) == 7, "SIG_AVATAR_RENEW ABI drift");

// +0x28 mercenary_start_time(int)、+0x2c mercenary_finish_time(int)、
// +0x30 area(char 符号扩展)、+0x31 period(char)。ORIG 汇编对 finish/area/
// period 以「行基址 add $0x30」+ 位移 0/4/5 两步寻址（P8 逐指令累加），故
// 行内定义 @ +0x2c 的 area/period 子视图以复现该形态。表头 4 字节 + 0x24 行。
#pragma pack(push, 1)
struct DB_MercenaryAreaView   // 行内 @ +0x2c
{
    int m_finishTime;           // +0x2c
    char m_area;                // +0x30
    char m_period;              // +0x31
};
#pragma pack(pop)
#pragma pack(push, 1)
struct DB_MercenaryRowView
{
    unsigned int m_characNo;    // +0x00
    char m_pad04[0x24];         // +0x04..+0x27
    int m_startTime;            // +0x28
    DB_MercenaryAreaView m_areaPeriod;  // +0x2c..+0x31
    char m_pad32[0x22];         // +0x32..+0x53
};
#pragma pack(pop)
#pragma pack(push, 1)
struct DB_MercenaryDataView
{
    int m_count;                // +0x00
    DB_MercenaryRowView m_rows[0x24];  // +0x04..+0xbd3
};
#pragma pack(pop)
static_assert(sizeof(DB_MercenaryDataView) == 0xbd4,
              "DB_MercenaryDataView ABI drift");

// ---- DB_UpdateCreatureItemList 行视图（SIG_CREATURE_ITEM_LIST +0x21a4 起）----

// ---- DB_UpdateCreatureItemList 行视图（SIG_CREATURE_ITEM_LIST +0x21a4 起）----
// ORIG 0x841fd12 逐偏移读：行步长 0x38，行内 +0x00=ui_id、+0x14=stomach、
// +0x1c=exp（SQL 实参序 exp/stomach/ui_id 与占位序相反）。
#pragma pack(push, 1)
struct DB_CreatureItemListEntry
{
    int m_uiId;             // +0x00
    char m_pad[0x10];       // +0x04
    int m_stomach;          // +0x14
    // [SweepIntervalSet 真 bug] ORIG 0x0841fda1 `add $0x21c0,%eax`（= 行基 +0x1c）
    // 读 exp，而旧声明把 m_exp 排到 +0x18（stomach 紧邻）⇒ 读的是行内 +0x18
    // 的未用字节。补 4 字节 pad 使 m_exp 落 +0x1c（SQL 实参 exp/stomach/ui_id
    // 与 ORIG 的 +0x21c0/+0x21b8/+0x21a4 三偏移一一对应）。
    char m_pad1[4];         // +0x18
    int m_exp;              // +0x1c
    char m_pad2[0x18];      // +0x20（行尾，步长 0x38）
};
#pragma pack(pop)
static_assert(sizeof(DB_CreatureItemListEntry) == 0x38,
              "DB_CreatureItemListEntry ABI drift");
// SIG_CREATURE_ITEM_LIST 行区基址（ORIG 0x0841fd12 `add $0x21a4` 行指针
// 基址实证；行类型 sizeof 断言见上，基址属包级布局常量）。
enum { kCreatureItemRowsOffset = 0x21a4 };

// ---- DB_InsertCreatureItem/UpdateCreatureItem 的 SIG_CREATURE_ITEM 视图 ----
// ORIG 直接 disp 读 +0x08(it_id)/+0x14(endurance ushort)/+0x4c(exp)；
// SIG_CREATURE_ITEM 的 m_item 数组下标会产生 add 两步寻址，改用
// 具名视图成员折叠单 disp（P82 尾部视图；仅表达真实布局，不伪造生命周期）。
#pragma pack(push, 1)
struct DB_CreatureItemInserView
{
    char m_pad0[0x08];
    int m_itId;                 // +0x08
    char m_pad1[0x08];
    unsigned short m_endurance; // +0x14（零扩展 %d）
    char m_pad2[0x4c - 0x16];
    int m_exp;                  // +0x4c
};
#pragma pack(pop)
static_assert(sizeof(DB_CreatureItemInserView) == 0x50,
              "DB_CreatureItemInserView ABI drift");

// ---- DB_SaveRank::SaveRankAll 行视图（SIG_SAVE_RANK 行步长 0x50）----
// ORIG 0x8417510 逐偏移读：dungeon_no=+0x0c / difficulty=+0x10(byte)；
// style/technic/attacked/rank 经 +0x40 起的 stats 视图（+0x8/+0xa/+0xc/+0x10，
// 即绝对 +0x48/+0x4a/+0x4c/+0x50，Ghidra 同）。
#pragma pack(push, 1)
struct DB_SaveRankRowView
{
    char m_pad0[0x0c];          // +0x00
    int m_dungeonNo;            // +0x0c
    unsigned char m_difficulty; // +0x10
};
#pragma pack(pop)
#pragma pack(push, 1)
struct DB_SaveRankStatsView   // sig + i*0x50 + 0x40 起
{
    char m_pad0[0x8];          // +0x00
    unsigned short m_style;    // +0x08
    unsigned short m_technic;  // +0x0a
    int m_attacked;            // +0x0c
    unsigned short m_rank;     // +0x10
};
#pragma pack(pop)
// ---- DB_UpdateDimension 的 SIG_UPDATE_DIMENSION 载荷视图（SIG m_pad[0x10]）----
// 写侧 CUserOnlyOrig.cpp UpdateDimensionInout：memset 0x10 后 +0x00=characNo、
// +0x04..+0x09=6×dungeon byte、+0x0c=blood dungeon1、+0x0d/+0x0e=两保存标志。
// ORIG dispatch 0x843230a 逐字节 movsbl 符号扩展读 dungeon（SQL %d）。
#pragma pack(push, 1)
struct DB_UpdateDimensionView
{
    unsigned int m_characNo;      // +0x00
    signed char m_dungeon[6];     // +0x04..+0x09
    char m_pad0a;                 // +0x0a
    char m_pad0b;                 // +0x0b
    signed char m_bloodDungeon1;  // +0x0c
    char m_dimensionInoutSave;    // +0x0d（非 0 走 charac_dimension_inout）
    char m_bloodInoutSave;        // +0x0e（非 0 走 charac_blood_inout）
    char m_pad0f;                 // +0x0f
};
#pragma pack(pop)
static_assert(sizeof(DB_UpdateDimensionView) == 0x10,
              "DB_UpdateDimensionView ABI drift");

// ---- DB_AvatarChangeStat 的 SIG_AVATAR_CHANGE_STAT 载荷视图（SIG m_pad[0x51]）----
// 输入：+0x00=ui_id、+0x04=it_id（SQL stat 源）、+0x08=slot byte、
// +0x09 起 4 字节 charac_no（ORIG mov 0x9(%eax) 读 int）。
// 回包（ui_id==0 时 seLect 组包）：逐列回填 +0x00/+0x04/+0x08/+0x09/
// +0x0a(reg_date)/+0x0e(expire_date)/+0x12(ipg_agency_no[0x18])/
// +0x4d/+0x4f(hidden_option 组)/+0x2f(jewel_socket 0x1e)/+0x4d/+0x4f。
#pragma pack(push, 1)
struct DB_AvatarChangeStatView
{
    int m_uiId;               // +0x00
    int m_itId;               // +0x04
    unsigned char m_slot;     // +0x08
    unsigned char m_abilityNo; // +0x09（SQL charac_no 读 +0x09 起 4 字节）
    int m_regDate;            // +0x0a
    int m_expireDate;         // +0x0e
    char m_ipgAgencyNo[0x18]; // +0x12
    char m_pad2a;             // +0x2a（0x12+0x18=0x2a，ORIG 回包跳至 +0x2b）
    short m_hiddenOption;     // +0x2b
    short m_field2d;          // +0x2d
    char m_jewelSocket[0x1e]; // +0x2f
    short m_field4d;          // +0x4d
    short m_emblemEndurance;  // +0x4f
};
#pragma pack(pop)
static_assert(sizeof(DB_AvatarChangeStatView) == 0x51,
              "DB_AvatarChangeStatView ABI drift");

// ---- DB_AvatarChangeStat 输入视图（upDate/seLect SQL 参数区）----
// ORIG 0x840066e 读 +0x09 起 4 字节作 charac_no（mov 0x9(%eax) 单指令，
// 组件视图折叠，P1）；+0x04 即 SQL stat 源。
#pragma pack(push, 1)
struct DB_AvatarChangeStatIn
{
    int m_uiId;               // +0x00
    int m_itId;               // +0x04
    unsigned char m_slot;     // +0x08
    int m_characNo;           // +0x09
};
#pragma pack(pop)

// ---- DB_ForbidUserToPlay 的 SIG_FORBID_USER_TO_PLAY 载荷视图（SIG m_pad[0x9]）----
// 写侧 Dispatchers.cpp gm_pkt::SigForbidPlayBody：memset 9B 后 +0x00=characNo、
// +0x04=flag byte、+0x05=days int。ORIG dispatch 0x8421af4 逐字段读。
#pragma pack(push, 1)
struct DB_ForbidUserToPlayView
{
    int m_characNo;           // +0x00
    bool m_flag;              // +0x04（ORIG 0x8421b45 movzbl 单字节装载 +
                              // `mov %al,-0xd` 直存，bool 形无 setne 归一）
    int m_days;               // +0x05
};
#pragma pack(pop)
static_assert(sizeof(DB_ForbidUserToPlayView) == 9,
              "DB_ForbidUserToPlayView ABI drift");

// ---- DB_SaveRank::SaveRankAll 的 SIG_SAVE_RANK 头视图（+0x0 charac_no
// / +0x4 count）----
#pragma pack(push, 1)
struct DB_SaveRankSigView
{
    int m_characNo;   // +0x00
    int m_count;      // +0x04（ORIG cmp/setg 带符号）
};
#pragma pack(pop)

// ---- DB_UpdateChannel::UpdateChannel 的 SIG_UPDATE_CHANNEL 字段视图 ----
// ORIG 0x8418032 逐偏移读：+0x00 gc_no / +0x04(short 符号扩展) gc_now /
// +0x06 swordman / +0x0a fighter / +0x0e gunner / +0x12 mage / +0x16 priest /
// +0x1a at_gunner / +0x1e thief / +0x32 hangame / +0x36 nexon。
#pragma pack(push, 1)
struct DB_UpdateChannelView
{
    int m_gcNo;              // +0x00 gc_no
    short m_gcNow;           // +0x04 gc_now（cwtl 符号扩展）
    int m_swordmanCnt;       // +0x06
    int m_fighterCnt;        // +0x0a
    int m_gunnerCnt;         // +0x0e
    int m_mageCnt;           // +0x12
    int m_priestCnt;         // +0x16
    int m_atGunnerCnt;       // +0x1a
    int m_thiefCnt;          // +0x1e
    char m_pad22[0x10];      // +0x22..+0x31
    int m_hangame;           // +0x32
    int m_nexon;             // +0x36
};
#pragma pack(pop)

// ---- DB_UpdateGrowType::UpdateGrowType 的 SIG_UPDATE_GROWTYPE 字段视图 ----
// ORIG 0x84181ea：+0x00 charac_no / +0x04 grow_type（movsbl 符号扩展）。
#pragma pack(push, 1)
struct DB_UpdateGrowTypeView
{
    unsigned int m_characNo;    // +0x00
    signed char m_growType;     // +0x04（movsbl）
};
#pragma pack(pop)

// ---- DB_UpdateNatType::dispatch 的 SIG_NAT_TYPE 字段视图 ----
// ORIG 0x841c74e：+0x00 nat_type / +0x04 nat_type_other / +0x08 fail_cnt /
// +0x0c success_cnt / +0x10 time_t（StatisticsNatType.cpp 同布局）。
#pragma pack(push, 1)
struct DB_NatTypeView
{
    int m_natType;        // +0x00
    int m_natTypeOther;   // +0x04
    int m_failCnt;        // +0x08
    int m_successCnt;     // +0x0c
    int m_time;           // +0x10
};
#pragma pack(pop)

// ---- DB_AvatarItemInsert 的 SIG_AVATAR_INSERT 字段视图（ORIG 0x83ffe0e
// InsertAvatarItem 逐偏移读 / 0x84001fe _SendResult 写 +0x4b）----
#pragma pack(push, 1)
struct DB_AvatarInsertView
{
    int m_characNo;            // +0x00（SQL %u）
    unsigned int m_itId;       // +0x04
    unsigned char m_slot;      // +0x08（SQL %d）
    unsigned char m_abilityNo; // +0x09
    int m_period;              // +0x0a（0/7..30 分支）
    unsigned char m_obtainFrom;// +0x0e
    // [SemA08 2026-09-12 §6双视图/真语义差异修复] ORIG 0x0840003x 段 set_query 第 9
    // 实参为 `mov 0x8(%ebp),%eax; add $0xf,%eax; mov %eax,-0xc0a0(%ebp)` = **取
    // &sig->m_agency（内联串地址）**，旧建模 `char* m_agency` 使 4.4 发
    // `mov 0xf(%eax),%eax`（解引用读 4 字节）⇒ 传 %s 的内容不同（线格式包不含
    // 主机指针；同函数 m_jewel[0x1e] 亦内联数组）。0x0f+0x19=0x28 与下一字段
    // m_hiddenOption 精确衔接 ⇒ sizeof=0x53、全部偏移零变化。
    char m_agency[0x19];       // +0x0f..+0x27（内联串，%s）
    unsigned short m_hiddenOption;     // +0x28
    unsigned short m_emblemEndurance;  // +0x2a
    signed char m_stat;        // +0x2c（movsbl）
    char m_jewel[0x1e];        // +0x2d（blob_to_str 0x1e）
    int m_identity;            // +0x4b（_SendResult 写）
    short m_field4f;           // +0x4f（set_query 多余实参，符号扩展）
    short m_field51;           // +0x51
};                             // 0x53
#pragma pack(pop)

// ---- DB_SaveDungeonClear 的 SIG_SAVE_DUNGEON_CLEAR 字段视图（ORIG 0x841a0a6
#pragma pack(push, 1)
struct DB_SaveDungeonClearView
{
    char m_pad0[0x4];
    int m_mId;                 // +0x04
    char m_pad8[0x4];
    // ORIG 0x0841a0xx：调用点传 sig+0xc 地址（`mov 0xc(%ebp),%edx; add
    // $0xc,%edx` / `lea 0xc(%eax),%ebx` 两形并存），非解引用 ⇒ +0xc 为
    // 线格式内联串（char 数组），此前按 char* 解引用读值属语义错位。
    char m_dungeon[0x1800];    // +0x0c（串内容，总包 0x180c）
};
#pragma pack(pop)
// ---- DB_UpdateItemLock::dispatch 的 SIG_ITEM_LOCK_UPDATE 视图 ----
// ORIG 0x843993a：+0x00 charac_no（%u）、+0x04 起 800 字节压缩源（全长 0x324）。
#pragma pack(push, 1)
struct DB_UpdateItemLockView
{
    int m_characNo;          // +0x00
    char m_lockData[0x320];  // +0x04..+0x323（compress_zip 源，800 字节）
};
#pragma pack(pop)
// ---- DB_SaveAchievement::dispatch 的 stAchievement_DBSaveData 字段视图 ----
// ORIG 0x840deaa：+0x00 起 0x1800 字节压缩源（m_data 段），+0x1808 charac_no（%u）。
#pragma pack(push, 1)
struct DB_SaveAchievementView
{
    char m_data[0x1808];   // +0x00..+0x1807（compress_zip 源 0x1800 字节）
    int m_characNo;        // +0x1808（SQL %u）
};
#pragma pack(pop)

// ---- DB_UpdateCoolTimeItem / DB_UpdateEffectItem::dispatch 的
// SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO 字段视图 ----
// ORIG 0x8430570 / 0x8430676：+0x00 charac_no（%u）、+0x04 起 0x194 字节
// cooltime_item / effect_item blob（全长 0x198）。
#pragma pack(push, 1)
struct DB_ContinuousItemStatView
{
    unsigned int m_characNo;  // +0x00（SQL %u）
    char m_itemData[0x194];   // +0x04..+0x197（blob_to_str 0x194）
};
#pragma pack(pop)

// ---- DB_SaveAccountInfo::dispatch 的 SIG_SAVE_ACCOUNT_INFO 字段视图 ----
// ORIG 0x8432646：+0x00 m_id(uint) / +0x04 pcbang_flag(byte) / +0x08
// last_play_time(uint) / +0x1c ip / +0x28 end_ip / +0x2c mac_addr。
#pragma pack(push, 1)
struct DB_SaveAccountInfoView
{
    unsigned int m_mId;          // +0x00（NumberToString %s）
    unsigned char m_pcbangFlag;  // +0x04（%d movzbl）
    char m_pad05[3];             // +0x05..+0x07
    unsigned int m_lastPlayTime; // +0x08（%u）
    char m_pad0c[0x10];          // +0x0c..+0x1b
    char m_ip[12];               // +0x1c..+0x27
    char m_endIp[4];             // +0x28..+0x2b
    char m_macAddr[0x10];        // +0x2c..+0x3b
};
#pragma pack(pop)

// ---- DB_SaveTitleBook::dispatch 的 stTitleBook_Save 头视图 ----
// ORIG 0x840db00：charac_no 位于 +0x6b3a（pack(1)，0x6b3a % 4 == 2）；
// 四段压缩源 +0x00/0x10ae/0x4056/0x5cee，尺寸 0x10ae/0x2fa8/0x1c98/0xe4c。
#pragma pack(push, 1)
struct DB_TitleBookSaveView
{
    char m_data[0x6b3a];   // +0x00
    int m_characNo;        // +0x6b3a
};
#pragma pack(pop)
#include "DB_MercenaryLogInfo.h"  // DB_MercenaryLogInfo（权威声明；makeRequest/dispatch 本文件实现）
#include "SigTypes_D.h"       // SIG_REQUEST_JOIN_GUILD / SIG_REQUEST_JOIN_GUILD_DB_REPLY（DB_RequestJoinGuild::dispatch）
#include "CStreamGuard.h"     // CStreamGuard(Stream*,bool)/operator<</GetInBuffer
#include "CGameOption.h"     // CETCOption/CQuickChattingOption/CChattingEmoticonConfig::clear（DB_LoadGameOption::dispatch）
#include "TaiwanInternal.h"   // Taiwan::internal_stream（本批两个 select 类）
#include "CUser.h"            // CUser::{GetUID,get_acc_id}
#include "MoreTypes.h"        // DBSelectCreateDnfUserInfo/DBSelectPcroomDailyReward::MSG
#include "DBSelectCreateDnfUserInfo.h"   // MSG/makeRequest（MoreTypes.h 占位已迁入类）
#include "DBSelectPcroomDailyReward.h"   // MSG/makeRequest（同上）
#include "advancealtar.h"     // advancealtar::DB_Load/DB_Update/DB_StatisticAdvanceAltar 权威定义
#include "CGuildServerProxy.h"  // CEventManager 完整类型（GetRepeatEvent）+ CEventBase::IsEventing
#include "AdvanceAltarZip.h"   // compress_zip（DB_SaveExpandEquipslot::_SaveExpandEquipslot）
#include <string.h>          // memcpy（DB_UpdateAvatarJewelSlot::makeRequest）
#include "CRijndael.h"       // encryptAES（ORIG DBThread.cpp:532 T 0x083fbe2f：GetBlockSize/Pad/Encrypt 需完整类型）
#include "../../ServerCommon/DNFFunctionLib.h"  // DNFFLib::Binary2Hex（encryptAES 出参转十六进制）
#include "SIG_SAVE_EXPAND_EQUIPSLOT.h"  // SIG_SAVE_EXPAND_EQUIPSLOT（同上）

#include "DB_SaveInven.h"   // DB_SaveInven（权威定义移出；本文件仅 new 注册）
#include "GameWorld.h"      // G_GameWorld / StatisticsShusiaService（DB_UpdateLogout::dispatch/UpdateLogout）
#include "CLogGameChannel.h"   // SIG_SAVE_LOGGAMECHANNEL / WongWork::CLogGameChannel（DB_SaveGamechannel::dispatch）
#include "CQueryCounter.h"     // CQueryCounter::QUERY_INFO（DB_UpdateQueryCountTimer*::dispatch）
#include "OnlinePreliminary.h"  // CEloRating / PvPGlobalEnvironmentVariable（DB_UpdatePvPResult::dispatch）
#include "CEventScriptMng.h"  // EventClassify::CEventScriptMng/CEventEntity（DB_UpdateAddItemCountValue / DB_UpdateFatigueAttendance）
#include "SyncScriptDeps.h"   // toTString（同上）
#include "DB_UpdateInventoryCapacity.h"  // DB_UpdateInventoryCapacity 权威声明（RegistDispatcher 注册 slot 686；本地占位类已删除）
#include "DB_AradDatabaseDispatcher.h"   // RegistDispatcher slot 729（2026-08-31 补齐注册）
#include "DB_DispatcherTaiwan.h"         // RegistDispatcher slot 819（2026-08-31 补齐注册）
#include "DB_LoadCharac.h"               // RegistDispatcher slot 7（ORIG C1 0x844d8c6，权威头已存在）
#include "DB_LoadInventory.h"            // RegistDispatcher slot 8（ORIG C1 0x840a604）
#include "DB_LoadQuest.h"                // RegistDispatcher slot 10（ORIG C1 0x844d930）
#include "DB_SaveCharac.h"               // RegistDispatcher slot 34（ORIG C1 0x8415c2e）
#include "TowerOfDespairMgr.h"   // _APCInfo（DB_TOD_Reset_UserAPC::dispatch/GetAPCUserInfo 值对象）


#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>           // sprintf（DB_GatheringParyStatus save_* 组装 IP 串）
#include <time.h>          // time/localtime_r（DB_BillingRecord::InsertSaleEntry 季度后缀）
#include <sys/select.h>
#include <unistd.h>

// ---- SIG_CHECK_ADD_FRIEND（SigTypes_A.h m_pad[0x24] 占位）载荷视图 ----
// 请求端 DisPatcher_AddFriend::process_packet_jpn 写 +0x04 字节标志
// （!isGM(acc_id)）与 +0x05 名字（strncpy 0x1e）；dispatch（ORIG 0x844c6e0）
// 读 +0x04 标志、+0x05 名字，回包写 +0x00 结果（初始 1 / GM 0x5a / 其他 0）
// 并回拷名字。
#pragma pack(push, 1)
struct DB_CheckAddFriendView
{
    int m_result;           // +0x00（回包：outCode）
    unsigned char m_flag;   // +0x04（请求标志，movzbl 读）
    char m_name[0x1e];      // +0x05..+0x22（strncpy 0x1e）
};                          // 0x23
#pragma pack(pop)
static_assert(sizeof(DB_CheckAddFriendView) == 0x23,
              "DB_CheckAddFriendView ABI drift");

// ---- SIG_IP_MONITOR_PUNISH 行视图 ----
// [Lane31W8LTDBTaskView] 权威头（SigTypes_B.h）已把整包字段化（匿名 union 双
// 视角，sizeof 0xc88 不变）：+0x00 m_count、+0x04 m_items[100]（步长 0x20）、
// +0xc84 m_flag。行类型唯一定义点在头内，本处用别名（删本地重复定义）。
typedef SIG_IP_MONITOR_PUNISH_ITEM DB_IpMonitorPunishItem;

// stPvPTempTable_t —— ORIG 仅存 W 析构 _ZN16stPvPTempTable_tD1Ev/D2Ev @0x08458e92
// （push/mov/pop/ret 空体，5 字节）及 vector<stPvPTempTable_t> 的 allocator 实例化。
// DB_UpdatePvPGrade::dispatch（ORIG 0x8428b26）消费点：vector push_back 源为
// SQL charac_no 槽（&local_bc，4 字节），operator[] 后读元素首 4 字节作 charac_no
// → 最小真实布局为 int m_characNo。
struct stPvPTempTable_t
{
    // 2026-08-31：ORIG 元素 sizeof 16（new_allocator 16B；dispatch push_back
    // 自栈 characNo 起整拷 16B = SQL 行 4 字段）。原仅 m_characNo（4B）——补全。
    unsigned int m_characNo;   // +0x00（DB_UpdatePvPGrade::dispatch 唯一消费字段；
    //   ORIG get_uint(0,&base+0) ⇒ 无符号）
    int m_pvpGrade;        // +0x04（SQL 列 1）
    int m_pvpPoint;        // +0x08（SQL 列 2）
    unsigned int m_lastPlay;   // +0x0c（SQL 列 3；ORIG get_uint(3,&base+0xc)）
    // [P5·2026-09-11] 显式 `~stPvPTempTable_t();` 已删（P3 的 UsedFatigueRewardItem::Data
    //   同源修法）：ORIG `_ZSt22__copy_move_backward_a…` 走 **ILb1**（`movb $0x1,-0x9(%ebp)`
    //   + `call __copy_move_backwardILb1ELb1E`）⇒ ORIG 该类型是隐式 trivial 析构；全 ELF 内
    //   `call 0x08458e92 <~stPvPTempTable_t>` 唯一 1 处落在 `new_allocator<…>::destroy`
    //   （0x08458e98）体内 = `p->~T()` 的 odr-use ⇒ D1/D2 符号仍由该路径发射（复编后 nm 已验）。
    //   判据：`_Destroy_aux<ILb0/ILb1>` 分支选择 + 全 ELF `~T` 调用点分布（「D1 符号存在」
    //   推不出「用户定义析构」）。
};

namespace OS_API
{
#include "W8LT_DBTask/uSleep.inc"
}

// ---- DB_UpdateDetailChannel::dispatch 的 SIG_UPDATE_DETAIL_CHANNEL 视图 ----
// ORIG 0x841bcee 逐偏移读 +0x00/+0x04/+0x08/+0x0c 四个 int（SQL 实参序
// occ_time/player_status/dungeon_index/channel_no/occ_count；cMyTrace 同序）。
#pragma pack(push, 1)
struct DB_UpdateDetailChannelView
{
    int m_playerStatus;    // +0x00
    int m_dungeonIndex;    // +0x04
    int m_channelNo;       // +0x08
    int m_occCount;        // +0x0c
};
#pragma pack(pop)
static_assert(sizeof(DB_UpdateDetailChannelView) == 0x10,
              "DB_UpdateDetailChannelView ABI drift");

// ---- DB_UpdateServerMessage::dispatch 的 SIG_UPDATE_SERVER_MESSAGE 视图 ----
// ORIG 0x8435a02 逐偏移读写：+0x00 server_info(int) / +0x04 channel_no(int) /
// +0x08 kind(byte) / +0x09 message_index(byte) / +0x0c message 长度(int) /
// +0x10 message[0x34] / +0x44 charac_name 长度(int) / +0x48 charac_name[0x14] /
// +0x5c update_time(int)，sizeof 0x60（memset/GetInBuffer 尺寸实证）。
#pragma pack(push, 1)
struct DB_UpdateServerMessageView
{
    int m_serverInfo;          // +0x00
    int m_channelNo;           // +0x04
    unsigned char m_kind;      // +0x08
    unsigned char m_messageIndex;  // +0x09
    char m_pad0a[2];           // +0x0a..+0x0b
    int m_messageLen;          // +0x0c
    char m_message[0x34];      // +0x10
    int m_characNameLen;       // +0x44
    char m_characName[0x14];   // +0x48
    int m_updateTime;          // +0x5c
};
#pragma pack(pop)
static_assert(sizeof(DB_UpdateServerMessageView) == 0x60,
              "DB_UpdateServerMessageView ABI drift");

// ---------------------------------------------------------------------------
// DB_Q 消息 dispatcher 类型集（329 个派生自 DBDispatcher 的空类型）
// ---------------------------------------------------------------------------

// ---- 全局作用域 ----
class DB_Login : public DBDispatcher { public: DB_Login() {} };   // ORIG C1 0x840f48e (sz 0x4: base ctor+vptr only)
class DB_CreateCharac : public DBDispatcher { public: DB_CreateCharac() {} };   // ORIG C1 0x8400cc2 (sz 0x4: base ctor+vptr only)
class DB_DeleteCharac : public DBDispatcher { public: DB_DeleteCharac() {} };   // ORIG C1 0x8402bce (sz 0x4: base ctor+vptr only)
// DB_UpdateCharacView —— 权威声明见 W8LT_DBTask.h（GoalAuditDB7 迁移；
// dispatch ORIG 0x08408848，定义在本 TU 尾部）。
class DB_LoadEtc : public DBDispatcher
{
public:
    DB_LoadEtc() {}   // ORIG C1 0x8403b84 (sz 0x4: base ctor+vptr only)
    static bool _LoadQuestShop(unsigned int characNo, CHARAC_QUEST_SHOP_DATA* out);  // ORIG T 0x08404b72（DB_LoadEtc.cpp 定义）
};
class DB_SaveSkill : public DBDispatcher
{
public:
    DB_SaveSkill() {}   // ORIG C1 0x8417770 (sz 0x4: base ctor+vptr only)
    virtual ~DB_SaveSkill() {}   // ORIG D1 0x841778c / D0 0x84177bc
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x84177da
    bool SaveSkill(SIG_LOAD_SKILL* sig);    // ORIG 0x841782a
    bool SendResult(int userIdx, int slot, SIG_LOAD_SKILL* sig);   // ORIG 0x8417a08（恒返 1）
};
class DB_SavePvp : public DBDispatcher
{
public:
    DB_SavePvp() {}   // ORIG C1 0x841720e (sz 0x4: base ctor+vptr only)
    virtual ~DB_SavePvp() {}   // ORIG D1 0x841722a / D0 0x841725a
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8417278
    bool SavePvp(SIG_SAVE_PVP* sig);   // ORIG 0x84172c8
};
// ---- DB_UpdateQuest 的 SIG_UPDATE_QUEST 载荷视图（SIG m_pad[0x7601]，pack(1)）----
// ORIG 0x8418c1c 逐偏移读：+0x00 charac_no / +0x04..+0x50 play_1..20 /
// +0x54..+0xa0 play_N_trigger / +0xa4 clear_quest 压缩源(30000B) /
// +0x75d4 标志(byte) / +0x75d5 urgentQuestIndex / +0x75d9 quest_notify[0x28]
// （0x75d9+0x28 == sizeof(SIG_UPDATE_QUEST)）。
#pragma pack(push, 1)
struct DB_UpdateQuestView
{
    unsigned int m_characNo;       // +0x00（SQL where charac_no=%u）
    int m_play[20];                // +0x04..+0x50（play_1..play_20）
    int m_playTrigger[20];         // +0x54..+0xa0（play_1_trigger..play_20_trigger）
    char m_clearData[0x7530];      // +0xa4（compress_zip 源，30000 字节）
    unsigned char m_clearFlag;     // +0x75d4（0=无 clear_quest 分支）
    unsigned int m_urgentIndex;    // +0x75d5（SQL urgentQuestIndex）
    char m_questNotify[0x28];      // +0x75d9（SQL quest_notify blob）
};
#pragma pack(pop)
static_assert(sizeof(DB_UpdateQuestView) == 0x7601,
              "DB_UpdateQuestView ABI drift");
class DB_UpdateQuest : public DBDispatcher
{
public:
    DB_UpdateQuest() {}   // ORIG C1 0x8418b74 (sz 0x4: base ctor+vptr only)
    virtual ~DB_UpdateQuest() {}   // ORIG D1 0x8418b90 / D0 0x8418bc0
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8418bde
    bool _updateQuests(SIG_UPDATE_QUEST* sig);   // ORIG 0x8418c1c
};
class DB_UpdateGrowType : public DBDispatcher
{
public:
    DB_UpdateGrowType() {}   // ORIG C1 0x8418130 (sz 0x4: base ctor+vptr only)
    virtual ~DB_UpdateGrowType() {}   // ORIG D1 0x841814c / D0 0x841817c
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x841819a
    bool UpdateGrowType(SIG_UPDATE_GROWTYPE* sig);   // ORIG 0x84181ea
};
// ---- DB_UpdateLogin 的 SIG_UPDATE_LOGIN 载荷视图（SIG m_pad[0x20]，pack(1)）----
// +0x08=serverGroup（DB_Login.cpp 写 SGF<short>(sig,0x3a38) 转 int，集成 PvP
// 时作 login_account_%d 表号）；+0x0c 起 login_ip 字符串。
#pragma pack(push, 1)
struct DB_UpdateLoginView
{
    unsigned int m_accId;      // +0x00（NumberToString %s m_id）
    int m_channelNo;           // +0x04（SQL m_channel_no）
    int m_serverGroup;         // +0x08（集成 PvP 时 login_account_%d 表号）
    char m_loginIp[0x14];      // +0x0c（SQL login_ip）
};
#pragma pack(pop)
static_assert(sizeof(DB_UpdateLoginView) == 0x20,
              "DB_UpdateLoginView ABI drift");
class DB_UpdateLogin : public DBDispatcher
{
public:
    DB_UpdateLogin() {}   // ORIG C1 0x844da04 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x841826a
};
class DB_UpdateLogout : public DBDispatcher
{
public:
    DB_UpdateLogout() {}   // ORIG C1 0x84183d8 (sz 0x4: base ctor+vptr only)
    virtual ~DB_UpdateLogout() {}   // ORIG D1 0x84183f4 / D0 0x8418424
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8418612
    bool UpdateLogout(SIG_UPDATE_LOGOUT* sig);                     // ORIG 0x8418a96
    // ORIG 0x8418442 尾部 `xor $1,%eax` 直返 !exec2（mangled 不含返回类型）。
    bool UpdateAccountMemberBonusFatigue(unsigned int accId, short memberBonusFatigue);
    bool SendResult(int userIdx, int slot, SIG_UPDATE_LOGOUT* sig);  // ORIG 0x841852e
};
class DB_UpdateChannel : public DBDispatcher
{
public:
    DB_UpdateChannel() {}   // ORIG C1 0x8417f4a (sz 0x4: base ctor+vptr only)
    virtual ~DB_UpdateChannel() {}   // ORIG D1 0x8417f66 / D0 0x8417f96
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8417fb4
    bool UpdateChannel(SIG_UPDATE_CHANNEL* sig);   // ORIG 0x8418032
};
class DB_DeleteChannel : public DBDispatcher
{
public:
    DB_DeleteChannel() {}   // ORIG C1 0x8402892 (sz 0x4: base ctor+vptr only)
    virtual ~DB_DeleteChannel() {}   // ORIG D1 0x84028ae / D0 0x84028de
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x84028fc
    bool DeleteChannel(SIG_DELETE_CHANNEL* sig);     // ORIG 0x84029ae
    bool SendResult(int userIdx, int slot, SIG_DELETE_CHANNEL* sig);  // ORIG 0x8402aec
};
class DB_InsertChannel : public DBDispatcher
{
public:
    DB_InsertChannel() {}   // ORIG C1 0x840364c (sz 0x4: base ctor+vptr only)
    virtual ~DB_InsertChannel() {}   // ORIG D1 0x8403668 / D0 0x8403698
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x84036b6
    bool InsertChannel(SIG_INSERT_CHANNEL* sig);     // ORIG 0x8403734
};
class DB_AvatarItemInsert : public DBDispatcher
{
public:
    DB_AvatarItemInsert() {}   // ORIG C1 0x83ffd50 (sz 0x4: base ctor+vptr only)
    virtual ~DB_AvatarItemInsert() {}   // ORIG D1 0x83ffd9c / D0 0x83ffd6c
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x83ffdba
    static bool InsertAvatarItem(SIG_AVATAR_INSERT* sig);    // ORIG 0x83ffe0e（static：调用无 this）
    void _SendResult(SIG_AVATAR_INSERT* sig, int slot);  // ORIG 0x84001fe（返 void）
};
// SIG_AVATAR_DELETE（sizeof 0x70c）：ORIG dispatch 0x83ffcac get_binary(stream, 0x70c)
// 缓冲；DelAvatarItem 0x83ffd46 恒返 1 不读字段，无字段布局证据，仅尺寸占位。
struct SIG_AVATAR_DELETE
{
    char m_pad[0x70c];
};
class DB_AvatarItemDel : public DBDispatcher
{
public:
    DB_AvatarItemDel() {}   // ORIG C1 0x83ffc42 (sz 0x4: base ctor+vptr only)
    virtual ~DB_AvatarItemDel() {}   // ORIG D1 0x83ffc5e / D0 0x83ffc8e
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x83ffcac
    bool DelAvatarItem(SIG_AVATAR_DELETE& sig);   // ORIG 0x83ffd46（恒返 1）
};
// SIG_AVATAR_MOVE（sizeof 0x16，SigTypes_E.h m_pad 占位）字段视图：
// ORIG MoveAvatarItem 0x84003ca 逐偏移读 +0x04/+0x08(int ui_id)、
// +0x0c/+0x0d(byte slot)、+0x0e/+0x12(int clear_avatar_id)。
#pragma pack(push, 1)
struct DB_AvatarMoveView
{
    char m_pad0[4];             // +0x00
    int m_uiId0;                // +0x04
    int m_uiId1;                // +0x08
    unsigned char m_slot1;      // +0x0c
    unsigned char m_slot0;      // +0x0d
    int m_clearAvatarId0;       // +0x0e
    int m_clearAvatarId1;       // +0x12
};
#pragma pack(pop)
class DB_AvatarItemMove : public DBDispatcher
{
public:
    DB_AvatarItemMove() {}   // ORIG C1 0x840031e (sz 0x4: base ctor+vptr only)
    virtual ~DB_AvatarItemMove() {}   // ORIG D1 0x840033a / D0 0x840036a
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8400388
    bool MoveAvatarItem(SIG_AVATAR_MOVE* sig);   // ORIG 0x84003ca（恒返 1）
    bool ChangeSlot(int userIdx, int slot, int clearAvatarId);   // ORIG 0x8400432
};
class DB_AvatarExtendPeriod : public DBDispatcher
{
public:
    DB_AvatarExtendPeriod() {}   // ORIG C1 0x83ffab8 (sz 0x4: base ctor+vptr only)
    virtual ~DB_AvatarExtendPeriod() {}   // ORIG D1 0x83ffad4 / D0 0x83ffb04
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x083ffb22
    bool ExtendPeriod(SIG_AVATAR_RENEW& renew);   // ORIG 0x083ffbb4
};
// DB_AvatarChangeOwner —— 权威声明见 DB_AvatarChangeOwner.h（GoalAuditDB7 迁移；
// dispatch ORIG 0x084004bc，定义在本 TU 尾部）。
class DB_AvatarChangeStat : public DBDispatcher
{
public:
    DB_AvatarChangeStat() {}   // ORIG C1 0x844e0aa (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0840065e
};
class DB_SaveRank : public DBDispatcher
{
public:
    DB_SaveRank() {}   // ORIG C1 0x844d99a (sz 0x4: base ctor+vptr only)
    virtual ~DB_SaveRank() {}   // ORIG D1 0x844d9b6 / D0 0x844d9e6
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x84174c0
    bool SaveRankAll(SIG_SAVE_RANK* sig);   // ORIG 0x8417510
};
class DB_InsertTicket : public DBDispatcher
{
public:
    DB_InsertTicket() {}   // ORIG C1 0x8403a4e (sz 0x4: base ctor+vptr only)
    virtual ~DB_InsertTicket() {}   // ORIG D1 0x8403a6a / D0 0x8403a9a
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x8403ab8
    bool InsertTicket(SIG_INSERT_TICKET* sig);       // ORIG 0x8403b08
};
class DB_LoadTicket : public DBDispatcher
{
public:
    DB_LoadTicket() {}   // ORIG C1 0x8419572 (sz 0x4: base ctor+vptr only)
    virtual ~DB_LoadTicket() {}   // ORIG D1 0x841958e / D0 0x84195be
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x84195dc
    bool LoadTicket(SIG_LOAD_TICKET* sig);    // ORIG 0x841968a
    bool SendResult(int userIdx, int slot, SIG_LOAD_TICKET* sig);  // ORIG 0x8419770
};
class DB_LoadDungeonClear : public DBDispatcher
{
public:
    DB_LoadDungeonClear() {}   // ORIG C1 0x8419852 (sz 0x4: base ctor+vptr only)
    virtual ~DB_LoadDungeonClear() {}   // ORIG D1 0x841986e / D0 0x841989e
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x84198bc
    bool LoadDungeonClear(SIG_LOAD_DUNGEON_CLEAR* sig);             // ORIG 0x8419db4
    bool AradLoadMemberDungeonClear(SIG_LOAD_DUNGEON_CLEAR* sig);   // ORIG 0x841996e
    bool AradLoadDungeonClear(SIG_LOAD_DUNGEON_CLEAR* sig);         // ORIG 0x8419a76
    bool SendResult(int userIdx, int slot, SIG_LOAD_DUNGEON_CLEAR* sig);  // ORIG 0x8419f0a
};
class DB_SaveDungeonClear : public DBDispatcher
{
public:
    DB_SaveDungeonClear() {}   // ORIG C1 0x8419fec (sz 0x4: base ctor+vptr only)
    virtual ~DB_SaveDungeonClear() {}   // ORIG D1 0x841a008 / D0 0x841a038
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x841a056
    bool SaveDungeonClear(SIG_SAVE_DUNGEON_CLEAR* sig);   // ORIG 0x841a0a6
    bool SendResult(int userIdx, int slot, SIG_SAVE_DUNGEON_CLEAR* sig);  // ORIG 0x841a1a2
};
class DB_TerminateServer : public DBDispatcher { public: DB_TerminateServer() {}   // ORIG C1 0x841a284 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841a2ee（恒返 1）
};
class DB_StatBreakJar : public DBDispatcher
{
public:
    DB_StatBreakJar() {}   // ORIG C1 0x8417a12 (sz 0x4: base ctor+vptr only)
    virtual ~DB_StatBreakJar() {}   // ORIG D1 0x8417a2e / D0 0x8417a5e
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8417a7c
    bool LogWrite(MSG_STAT_BREAKJAR* msg);   // ORIG 0x8417afa（恒返 1）
};
class DB_StatComposition : public DBDispatcher
{
public:
    DB_StatComposition() {}   // ORIG C1 0x8417b82 (sz 0x4: base ctor+vptr only)
    virtual ~DB_StatComposition() {}   // ORIG D1 0x8417b9e / D0 0x8417bce
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8417bec
    bool LogWrite(MSG_STAT_COMPOSITON* msg);   // ORIG 0x8417c6a（恒返 1）
};
struct MSG_STAT_USECOIN;   // 前向声明（ORIG LogWrite 引用参数；无 GetOutBuffer 实例化）
class DB_StatCoinUse : public DBDispatcher
{
public:
    DB_StatCoinUse() {}   // ORIG C1 0x8417b04 (sz 0x4: base ctor+vptr only)
    virtual ~DB_StatCoinUse() {}   // ORIG D1 0x8417b20 / D0 0x8417b50
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8417b6e（恒返 1）
    bool LogWrite(MSG_STAT_USECOIN& msg);   // ORIG 0x8417b78（恒返 1）
};
class DB_StatPvpPlay : public DBDispatcher
{
public:
    DB_StatPvpPlay() {}   // ORIG C1 0x8417e58 (sz 0x4: base ctor+vptr only)
    virtual ~DB_StatPvpPlay() {}   // ORIG D1 0x8417e74 / D0 0x8417ea4
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8417ec2
    bool LogWrite(MSG_STAT_PVPPLAY* msg);   // ORIG 0x8417f40（恒返 1）
};
class DB_StatGameRank : public DBDispatcher
{
public:
    DB_StatGameRank() {}   // ORIG C1 0x8417d66 (sz 0x4: base ctor+vptr only)
    virtual ~DB_StatGameRank() {}   // ORIG D1 0x8417d82 / D0 0x8417db2
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8417dd0
    bool LogWrite(MSG_STAT_RANKRES* msg);   // ORIG 0x8417e4e（恒返 1）
};
class DB_StatDungeonClear : public DBDispatcher
{
public:
    DB_StatDungeonClear() {}   // ORIG C1 0x8417c74 (sz 0x4: base ctor+vptr only)
    virtual ~DB_StatDungeonClear() {}   // ORIG D1 0x8417c90 / D0 0x8417cc0
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8417cde
    bool LogWrite(MSG_STAT_DUNGEONCLEAR* msg);   // ORIG 0x8417d5c（恒返 1）
};
class DB_CrashDownReport : public DBDispatcher
{
public:
    DB_CrashDownReport() {}   // ORIG C1 0x844e0c6 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841abc6
};
// DB_SaveFeaturedGoodsIdx 权威声明移至 W8LT_DBTask.h（LinkClosureB-2）
class DB_RemoveFeaturedGoodsIdx : public DBDispatcher
{
public:
    DB_RemoveFeaturedGoodsIdx() {}   // ORIG C1 0x844e0fe (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0841b41a
};   // ORIG C1 0x844e0fe (sz 0x4: base ctor+vptr only)
class DB_GenCeraTicket : public DBDispatcher
{
public:
    DB_GenCeraTicket() {}   // ORIG C1 0x844da6e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x841a2f8
};
class DB_RemoveEventItem : public DBDispatcher { public: DB_RemoveEventItem() {}   // ORIG C1 0x844db42 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841ab30
};
class DB_FatigueResetTimer : public DBDispatcher { public: DB_FatigueResetTimer() {}   // ORIG C1 0x844dbac (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841abbc（恒返 1）
};
class DB_SuicideWhenQueueIsEmpty : public DBDispatcher { public: DB_SuicideWhenQueueIsEmpty() {}   // ORIG C1 0x844e11a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841b206（恒返 1）
};
class DB_SavePremiumInfo : public DBDispatcher { public: DB_SavePremiumInfo() {}   // ORIG C1 0x844e136 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841b4be
};
class DB_SaveCharacInfo4Guild : public DBDispatcher { public: DB_SaveCharacInfo4Guild() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0841b624
};   // ORIG C1 0x844e152 (sz 0x4: base ctor+vptr only)
class DB_StatDungeonFail : public DBDispatcher { public: DB_StatDungeonFail() {}   // ORIG C1 0x844e16e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841b6e0
};
class DB_StatPVPPlay2 : public DBDispatcher
{
public:
    DB_StatPVPPlay2() {}   // ORIG C1 0x844e18a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841ba96
};
class DB_StatUseShusiaService : public DBDispatcher { public: DB_StatUseShusiaService() {}   // ORIG C1 0x844e1a6 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841bce4（恒返 1）
};
class DB_UpdateDetailChannel : public DBDispatcher
{
public:
    DB_UpdateDetailChannel() {}   // ORIG C1 0x844e1c2 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x841bcee
};
class DB_SaveChannelUserLevel : public DBDispatcher
{
public:
    DB_SaveChannelUserLevel() {}   // ORIG C1 0x844e1de (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841be50
};
class DB_SavePrivateStore : public DBDispatcher
{
public:
    DB_SavePrivateStore() {}   // ORIG C1 0x844e1fa (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841bf02
};
class DB_LoadPrivateStore : public DBDispatcher
{
public:
    DB_LoadPrivateStore() {}   // ORIG C1 0x844e216 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841c08c
};
class DB_SaveGamechannel : public DBDispatcher { public: DB_SaveGamechannel() {}   // ORIG C1 0x844e232 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841c428
};
class DB_UpdateQueryCountTimerLogdb : public DBDispatcher { public: DB_UpdateQueryCountTimerLogdb() {}   // ORIG C1 0x844e24e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841c458
};
class DB_UpdateQueryCountTimerDb : public DBDispatcher { public: DB_UpdateQueryCountTimerDb() {}   // ORIG C1 0x844e26a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841c5d4
};
class DB_UpdateQueryCountDbLogdb : public DBDispatcher
{
public:
    DB_UpdateQueryCountDbLogdb() {}   // ORIG C1 0x844e286 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0841c61a
};
class DB_UpdateNatType : public DBDispatcher
{
public:
    DB_UpdateNatType() {}   // ORIG C1 0x844e2a2 (sz 0x4: base ctor+vptr only)
    virtual ~DB_UpdateNatType() {}   // ORIG D1 0x845f60a / D0 0x845f63a
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x841c74e
};
class DB_UpdateNatTypeUser : public DBDispatcher
{
public:
    DB_UpdateNatTypeUser() {}   // ORIG C1 0x844e2be (sz 0x4: base ctor+vptr only)
    virtual ~DB_UpdateNatTypeUser() {}   // ORIG D1 0x845f5bc / D0 0x845f5ec
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x841c7f6
};
class DB_MailBox_Req_CheckReceiver : public DBDispatcher { public: DB_MailBox_Req_CheckReceiver() {} };   // ORIG C1 0x844e2da (sz 0x4: base ctor+vptr only)
class DB_MailBox_Req_List : public DBDispatcher { public: DB_MailBox_Req_List() {} };   // ORIG C1 0x844e2f6 (sz 0x4: base ctor+vptr only)
class DB_MailBox_Req_Delete : public DBDispatcher { public: DB_MailBox_Req_Delete() {} };   // ORIG C1 0x844e312 (sz 0x4: base ctor+vptr only)
class DB_MailBox_Req_Send : public DBDispatcher
{
public:
    DB_MailBox_Req_Send() {}   // ORIG C1 0x844e32e (sz 0x4: base ctor+vptr only)
    static bool insertMailData(MySQL* db, const SIG_MAILBOX_SEND_NEW_MAIL* sigRaw);  // ORIG T 0x0841f2fa（DB_MailBox_Req_Send.cpp）
    // [Q5W8LT 2026-09-11 落地·setcc/类型轴] 原为 `static int`，但**定义侧**
    // `DB_MailBox_Req_Send/sendResult.inc:1` 实读为 `bool`（两侧原不一致）⇒ 本行补齐。
    // ORIG 直证：调用点 0x084275f7 `xor $0x1,%eax; test %al,%al; je`（bool 取反形，无 sete）
    // ＋定义侧 0x0841f75a 尾部 `mov $0x1,%ebx; call CStreamGuardD1Ev; mov %ebx,%eax; add/pop/ret`
    // （唯一成功路径写 1、无 test/setne 归一化）⇒ 返回类型为 1 字节 bool。
    static bool sendResult(int userIdx, int slot, int mailIdx,
                           ENUM_SERVER_GROUP group);  // ORIG T 0x0841f75a（DB_MailBox_Req_Send.cpp）
};
class DB_InsertCreatureItem : public DBDispatcher
{
public:
    DB_InsertCreatureItem() {}   // ORIG C1 0x844e34a (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0841fa28 _ZN21DB_InsertCreatureItem18insertCreatureItemEP5MySQLPK17SIG_CREATURE_ITEM
    static bool insertCreatureItem(MySQL* db, SIG_CREATURE_ITEM const* sig);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0841f8d4
};
class DB_UpdateCreatureItemList : public DBDispatcher
{
public:
    DB_UpdateCreatureItemList() {}   // ORIG C1 0x844e366 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0841fd12
};
class DB_UpdateCreatureItem : public DBDispatcher
{
public:
    DB_UpdateCreatureItem() {}   // ORIG C1 0x844e382 (sz 0x4: base ctor+vptr only)
    void GetField(MySQL* db, char* out, SIG_CREATURE_ITEM* sig);   // ORIG T 0x084200a8
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x084203e8
};
// ORIG C1 0x844e3ba（sz 0x4: base ctor+vptr only）。5 个强符号：
//   save_dnf_pvp_ping_history 0x8421066 / save_dnf_party_ping_history 0x84217b6
//   save_dnf_pvp_result_history 0x8420e1a / save_dnf_party_result_history 0x842156a
//   dispatch 0x8421aea（覆盖 DBDispatcher::dispatch，返回 1）。
// 表：dnf_party_ping_history_pvp / dnf_party_ping_history / dnf_party_result_history_pvp
//   / dnf_party_result_history（ORIG 拼串，inSert 大小写照抄）。
class DB_GatheringParyStatus : public DBDispatcher
{
public:
    DB_GatheringParyStatus() {}
    // b28c-L19 SIGFIX：ORIG __PRETTY_FUNCTION__ 串实证返回 bool。
    bool save_dnf_pvp_ping_history(SIG_PARTY_STATUS* sig);    // ORIG 0x8421066
    bool save_dnf_party_ping_history(SIG_PARTY_STATUS* sig);  // ORIG 0x84217b6
    // b28c-L19 SIGFIX：ORIG __PRETTY_FUNCTION__ 串实证返回 bool（原写 int）。
    bool save_dnf_pvp_result_history(SIG_PARTY_STATUS* sig, int idx);   // ORIG 0x8420e1a
    bool save_dnf_party_result_history(SIG_PARTY_STATUS* sig, int idx); // ORIG 0x842156a
    bool dispatch(int userIdx, int slot, Stream* stream);    // ORIG 0x8421aea
};

// ORIG T _Z20get_skill_table_nameb @0x083fbdc6（定义 DB_LoadEtc.cpp:2018）
char const* get_skill_table_name(bool use2nd);

#include "W8LT_DBTask/save_dnf_pvp_ping_history.inc"


#include "W8LT_DBTask/save_dnf_party_ping_history.inc"


#include "W8LT_DBTask/save_dnf_pvp_result_history.inc"


#include "W8LT_DBTask/save_dnf_party_result_history.inc"


#include "W8LT_DBTask/dispatch_1.inc"
{
    return true;
}

class DB_ForbidUserToPlay : public DBDispatcher
{
public:
    DB_ForbidUserToPlay() {}   // ORIG C1 0x844e3d6 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08421af4
};
class DB_LastMsgDb : public DBDispatcher
{
public:
    DB_LastMsgDb() {}   // ORIG C1 0x844e3f2 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08421bcc
};
class DB_LastMsgLogDb : public DBDispatcher
{
public:
    DB_LastMsgLogDb() {}   // ORIG C1 0x844e40e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08421c8e
};
class DB_SaveClientHackLog : public DBDispatcher
{
public:
    DB_SaveClientHackLog() {}   // ORIG C1 0x844e42a (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08421d50
    int _getTodaysHackCount(MySQL* db, unsigned int mid, unsigned short hackType);
    // ORIG T 0x08421e3a
    void _sendResult(unsigned int mid);
    // ORIG T 0x08422152
    void _sendHackTypeResult(unsigned int mid, int hackType, int cnt);
    // ORIG T 0x08421f16（虚覆盖，vtable 槽 0）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_InsertQueueSizeLog : public DBDispatcher
{
public:
    DB_InsertQueueSizeLog() {}   // ORIG C1 0x844e446 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x084225a8
};
class DB_SaveSameMIDOtherCharLoginHack : public DBDispatcher
{
public:
    DB_SaveSameMIDOtherCharLoginHack() {}   // ORIG C1 0x844e462 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08422680
};
class DB_SaveProxyLog : public DBDispatcher
{
public:
    DB_SaveProxyLog() {}   // ORIG C1 0x844e47e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08422742
};
class DB_SaveGoldZeroTradeLog : public DBDispatcher { public: DB_SaveGoldZeroTradeLog() {} 
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x084227f6
};   // ORIG C1 0x844e49a (sz 0x4: base ctor+vptr only)
#pragma pack(push, 1)
// ORIG 0x08422964（dispatch）的写入视图：+0x1a 单字节（置 1）、+0x1b/+0x1f 各 4 字节清零。
// 三处偏移都落在 SIG_MOUSE_REGISTER::m_pad16（0x16..0x22，共享头未建模填充）内，
// 本 TU 只按 ORIG 的写宽/写序复现，故用本地视图结构（不改共享头）。
struct DB_MouseRegisterWriteView
{
    char m_pad0[0x1a];
    char m_field1a;     // +0x1a ORIG 0x08422b50 `movb $0x1,0x1a(%eax)`
    int  m_field1b;     // +0x1b ORIG 0x08422b5e `movl $0x0,0x1b(%eax)`
    int  m_field1f;     // +0x1f ORIG 0x08422b57 `movl $0x0,0x1f(%eax)`
};
#pragma pack(pop)
static_assert(offsetof(DB_MouseRegisterWriteView, m_field1a) == 0x1a,
              "DB_MouseRegisterWriteView m_field1a offset drift");
static_assert(offsetof(DB_MouseRegisterWriteView, m_field1b) == 0x1b,
              "DB_MouseRegisterWriteView m_field1b offset drift");
static_assert(offsetof(DB_MouseRegisterWriteView, m_field1f) == 0x1f,
              "DB_MouseRegisterWriteView m_field1f offset drift");

char* encryptAES_(CRijndael* cipher, char* src, char* dst);  // ORIG T 0x0846c398（定义在 MouseRegisterEJ.cpp）

class DB_MouseRegister : public DBDispatcher { public: DB_MouseRegister() {}
    // 虚表槽：本类在本 TU 是"注册用镜像"，真实实现在 MouseRegisterEJ.cpp（强定义）。
    // 内联定义在此仅用于满足 GCC 的 key-function 规则（否则本 TU 不发 `_ZTV`），
    // 链接期该弱定义被真实强定义覆盖 ⇒ vtable 槽指向真实 dispatch（行为不变）。
    // [N8W8LT] 真实体在本 TU 复写（ORIG 0x08422964 逐条转写，与 MouseRegisterEJ.cpp 同源）：
    //   改前 our=5（`mov $0x0,%eax` 桩，p167 MISSING 一大批）；改后 our=208=ORIG 208、
    //   .o 符号 size 0x325=ORIG range 0x325，check_tu_routed 判 **AE**（该件离开 non-identical 集）。
    //   关键旋钮：+0x192 分派必须是 **switch**(case 1/2/3 书写序)，4.4 才产出 ORIG 的
    //   `cmp 2/je → cmp 3/je → cmp 1/jne` 决策树 + case1 直落叶块形；写成 if/else-if
    //   同指令数 208 但分支反转（jne over block）⇒ 判 DIFF（实测）。
    static MySQL* _getDBHandleForDevideAccountDB();                 // ORIG T 0x08422938
    int _registNewPassword(unsigned int accId, const char* pwd,
                           char* ip, int param4);                   // ORIG T 0x084232e0
    int _modifyPassword(unsigned int accId, const char* pwd,
                        char* ip, int param4, bool param5);         // ORIG T 0x08423528
    int _unregistPassword(unsigned int accId, char* ip);            // ORIG T 0x08423794
    void _sendResult(unsigned int accId, int param2, int param3,
                     int param4, SIG_MOUSE_REGISTER* out);          // ORIG T 0x08423abe
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        // ORIG 栈槽：0x8=this 0xc=userIdx（未用）0x10=slot 0x14=stream；
        // 局部：-0x78 密文缓冲 -0x54 明文缓冲 -0x30 CStreamGuard -0x28 webAddress 快照
        //       -0x18 DB 句柄 -0x14 SIG 出站 -0x10 返回码 -0xc 入站指针。
        (void)userIdx;
        char buf58[36];                     // -0x54 明文缓冲（strncpy 7 字节）
        char buf7c[36];                     // -0x78 encryptAES_ 密文缓冲
        MySQL* db;                          // -0x18（ORIG 仅赋值未消费，同）
        SIG_MOUSE_REGISTER* out;            // -0x14
        int ret;                            // -0x10
        SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD* p;   // -0xc

        db = _getDBHandleForDevideAccountDB();
        out = stream->GetOutBuffer<SIG_MOUSE_REGISTER>();
        ret = 0;
        char buf2c[16] = { '\0' };          // -0x28（清零指令序须在 ret=0 之后，ORIG +0x28）
        strncpy(buf2c, out->m_webAddress, 0x10);
        strncpy(buf58, out->m_password2, 7);
        encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, buf58, buf7c);
        if (strncmp(out->m_padPassword, buf7c, 0x24) == 0)
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5b59), true);
            guard.operator*()->operator<<((int)0x246);
            guard.operator*()->operator<<((int)slot);
            p = guard.operator->()->GetInBuffer<SIG_GOBLIN_PAD_ADVICE_CHANGE_PWD>();
            p->m_adviceChangePwd = 0;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 0;
        }
        // ORIG +0x192 为 switch 决策树：cmp 2/je → cmp 3/je → cmp 1/jne，
        // 叶块物理序 = case 书写序 1→2→3（case1 直落，case2/case3 在外）。
        switch (out->m_pkgType)
        {
        case 1:
            ret = _registNewPassword(out->m_accId, buf7c, buf2c, slot);
            if (ret == 0)
            {
                reinterpret_cast<DB_MouseRegisterWriteView*>(out)->m_field1a = 1;
                reinterpret_cast<DB_MouseRegisterWriteView*>(out)->m_field1f = 0;
                reinterpret_cast<DB_MouseRegisterWriteView*>(out)->m_field1b = 0;
                memcpy(out->m_padPassword, buf7c, 0x24);
            }
            break;
        case 2:
            ret = _modifyPassword(out->m_accId, buf7c, buf2c, slot,
                                  out->m_advideChangePwd);
            if (ret == 0)
            {
                memcpy(out->m_padPassword, buf7c, 0x24);
                reinterpret_cast<DB_MouseRegisterWriteView*>(out)->m_field1f = 0;
                reinterpret_cast<DB_MouseRegisterWriteView*>(out)->m_field1b = 0;
            }
            break;
        case 3:
            ret = _unregistPassword(out->m_accId, buf2c);
            if (ret == 0)
            {
                memset(out->m_padPassword, 0, 0x24);
            }
            break;
        }
        _sendResult(out->m_accId, ret, out->m_pkgType, slot, out);
        return 1;
    }
};   // ORIG C1 0x844e4b6 (sz 0x4: base ctor+vptr only)
class DB_UpdateDungeonDataStatisticLogdb : public DBDispatcher
{
public:
    DB_UpdateDungeonDataStatisticLogdb() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08435f00
};   // ORIG C1 0x844e50a (sz 0x4: base ctor+vptr only)
#pragma pack(push, 1)
struct DB_BillingRecordSigView
{
    char m_pad0[0x15];
    int m_sealFlag;             // +0x15
    int m_saleTime;             // +0x19
    int m_price;                // +0x1d
    int m_slotPartType;         // +0x21
    int m_slotPartCode;         // +0x25
    int m_ipgNo;                // +0x29
    int m_mId;                  // +0x2d
    int m_characNo;             // +0x31
    char m_userId[0x19];        // +0x35
    int m_job;                  // +0x4e
    int m_sex;                  // +0x52
    char m_birthday[0xb];       // +0x56
    int m_prizeFlag;            // +0x61
    int m_resultCode;           // +0x65
    char m_resultMessage[0x64]; // +0x69
    int m_serverId;             // +0xcd
    int m_growType;             // +0xd1
    int m_level;                // +0xd5
    int m_giftServerInfo;       // +0xd9
    int m_giftGetCharacNo;      // +0xdd
    int m_chargePrice;          // +0xe1
    int m_freePrice;            // +0xe5
    int m_countNo;              // +0xe9
    int m_eachPrice;            // +0xed
    int m_priceType;            // +0xf1
    char m_userIp[0x10];        // +0xf5
    int m_tranNoLow;            // +0x105
    int m_tranNoHigh;           // +0x109
    char m_buyType;             // +0x10d
};
#pragma pack(pop)
class DB_BillingRecord : public DBDispatcher
{
public:
    DB_BillingRecord() {}   // ORIG C1 0x844e526 (sz 0x4: base ctor+vptr only)
    virtual ~DB_BillingRecord() {}   // ORIG D1/D2 0x845ee6c / D0 0x845ee9c
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08424574
    void InsertBuyUser(SIG_BILLING_RECORD* sig, bool& result);     // ORIG T 0x08424602
    void InsertSaleEntry(SIG_BILLING_RECORD* sig, bool isNew);     // ORIG T 0x084247e0
};
class DB_ReqBlackCount : public DBDispatcher
{
public:
    DB_ReqBlackCount() {}   // ORIG C1 0x844e542 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08424ce6
};
class DB_ReqSaveCrazyLevelupEvent : public DBDispatcher
{
public:
    DB_ReqSaveCrazyLevelupEvent() {}   // ORIG C1 0x844e55e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0842527a
};
class DB_ReqSaveHackUserPunish : public DBDispatcher
{
public:
    DB_ReqSaveHackUserPunish() {}   // ORIG C1 0x844e57a (sz 0x4: base ctor+vptr only)
    // 弱定义发 vtable；真实强定义在 DB_U3.cpp。ORIG 0x08425374（T, sz 0x11f）真实体在本 TU
    // 复写（与 DB_U3.cpp 版逐字一致），链接期仍由 DB_U3.cpp 强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx; (void)slot;
        SIG_SAVE_HACK_USER_PUNISH* sig =
            stream->GetOutBuffer<SIG_SAVE_HACK_USER_PUNISH>();
        MySQL* db6 =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
        MySQL* db1 =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
        (void)db6;
        db1->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), "
            "end_time='9999-12-31 23:59:59', punish_value=0, apply_flag=2 where "
            "m_id=%s and punish_type=%d",
            NumberToString(sig->m_accId, 0), 2);
        if (!db1->exec(true) || db1->getAffectedRowCount() == 0)
        {
            db1->set_query(
                "inSert into member_punish_info(m_id, punish_type, occ_time, "
                "start_time, end_time, punish_value, apply_flag) values(%s, %d, "
                "now(), now(), '9999-12-31 23:59:59', 0, 2)",
                NumberToString(sig->m_accId, 0), 2);
            if (!db1->exec(true))
                return false;
        }
        return true;
    }
    // ORIG T 0x08425494（定义归 DB_U3.cpp）
    static void makeRequest(unsigned int uid, unsigned int type, const char* ip);
};
class DB_InsertUserPlayTime : public DBDispatcher
{
public:
    DB_InsertUserPlayTime() {}   // ORIG C1 0x844e596 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x084256b4
};
class DB_UpdateHappyBeanInfo : public DBDispatcher { public: DB_UpdateHappyBeanInfo() {}
    // 弱定义发 vtable；真实强定义在 DB_U3.cpp。ORIG 0x084258ee（T, sz 0x113）真实体在本 TU
    // 复写（与 DB_U3.cpp 版逐字一致），链接期仍由 DB_U3.cpp 强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx; (void)slot;
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
        unsigned int uid = 0;
        unsigned int characNo = 0;
        char name[0x1d];
        *stream >> uid;
        *stream >> characNo;
        *stream >> name;
        (void)characNo; (void)name;
        db->set_query(
            "upDate event_happybean_log set cnt=cnt+1 where occ_date=now() and m_id=%s",
            NumberToString(uid, 0));
        if (!db->exec(true) || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into event_happybean_log(occ_date,m_id,cnt) values(now(),%s,1)",
                NumberToString(uid, 0));
            db->exec(true);
        }
        return 1;
    }
};   // ORIG C1 0x844e5b2 (sz 0x4: base ctor+vptr only)
class DB_InsertQueryCounterDb : public DBDispatcher
{
public:
    DB_InsertQueryCounterDb() {}   // ORIG C1 0x844e5ce (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08425b0c
};
class DB_InsertQueryCounterLogdb : public DBDispatcher
{
public:
    DB_InsertQueryCounterLogdb() {}   // ORIG C1 0x844e5ea (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08425cde
};
class DB_ReqItemUpgradeLog : public DBDispatcher
{
public:
    DB_ReqItemUpgradeLog() {}   // ORIG C1 0x844e606 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08425dbe _ZN20DB_ReqItemUpgradeLog8dispatchEiiP6Stream（虚覆盖，本文件实现）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_ChangeLetterStat : public DBDispatcher
{
public:
    DB_ChangeLetterStat() {}   // ORIG C1 0x844e622 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0842602e
    // ORIG T 0x084261fc _ZN19DB_ChangeLetterStat11makeRequestEiiiii
    static void makeRequest(int userIdx, int slot, int characNo, int mailIdx,
                            int state);
};
class DB_ReqChangeCharacName : public DBDispatcher { public: DB_ReqChangeCharacName() {}
    // 弱定义发 vtable；真实强定义在 DB_LinkMisc4.cpp。ORIG 0x084262ea（T, sz 0x2a5）真实体
    // 在本 TU 复写（与 DB_LinkMisc4.cpp 版逐字一致），链接期仍由该强定义覆盖（行为不变）。
    // 两非虚成员与 DB_ReqChangeCharacName.h 权威头一致（ORIG T 0x084266a4 / 0x084267c2）。
    int _hasSameCharacName(MySQL* db, char const* table, char const* name);
    // [Lane31W8LTDBTaskView] 签名对齐权威（DB_ReqChangeCharacName.h:36 /
    // DB_LinkMisc4.cpp:301 与 ORIG nm `_ZN22DB_ReqChangeCharacName11_sendResultEPK22SIG_CHANGE_CHARAC_NAMEii`
    // 均为 `SIG_CHANGE_CHARAC_NAME const*`）——原非 const 声明产生未定义 mangled
    // `…P22SIG_CHANGE_CHARAC_NAMEii`（全量链接 undefined reference 根因）。
    void _sendResult(SIG_CHANGE_CHARAC_NAME const* sig, int slot, int ret);
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        if (!DBDispatcher::dispatch(userIdx, slot, stream))
            return false;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
        SIG_CHANGE_CHARAC_NAME* sig = stream->GetOutBuffer<SIG_CHANGE_CHARAC_NAME>();
        char name_buf[60] = { '\0' };   // 转义缓冲区（ORIG rep stos 清零 60 字节）
        db->escape_string(name_buf, sig->m_name);
        typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DbConnMap;
        ENUM_DB_HANDLE_IDX connIdx = DB_HANDLE_2;   // ORIG 栈上局部初始值 2
        int ret = _hasSameCharacName(
            db,
            G_CEnvironment()->m_dbConnInfo[connIdx].m_db,
            name_buf);
        if (ret != 0)
        {
            _sendResult(sig, slot, ret);
            return true;
        }
        db->set_query("upDate charac_info set charac_name='%s' where charac_no=%d and delete_flag=0",
                      name_buf, sig->m_characNo);
        if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        {
            _sendResult(sig, slot, 2);
            return true;
        }
        MySQL* guildDb = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
        struct ChangeCharacNameByteView
        {
            char m_pad[0x2c];
            unsigned char m_serverId;
        };
        guildDb->set_query("seLect guild_id from guild_member where charac_no=%d and member_flag=1 "
                           "and server_id=%d",
                           sig->m_characNo,
                           reinterpret_cast<ChangeCharacNameByteView*>(sig)->m_serverId);
        sig->m_arg28 = 0;
        if (!(guildDb->exec(true) && guildDb->fetch()))
            sig->m_arg28 = 0;
        if (guildDb->get_n_rows() == 0)
            sig->m_arg28 = 0;
        else if (!guildDb->get_uint(0, sig->m_arg28))
            sig->m_arg28 = 0;
        _sendResult(sig, slot, 0);
        return true;
    }
};   // ORIG C1 0x844e63e (sz 0x4: base ctor+vptr only)
class DB_ReqQueryCharacInfo : public DBDispatcher
{
public:
    DB_ReqQueryCharacInfo() {}
    // ORIG T 0x08426d18 _ZN20DB_ReqQueryCharacInfo11makeRequestEiPKc（定义本文件尾部）
    static void makeRequest(int uid, const char* name);
    // ORIG T 0x0842688c _ZN21DB_ReqQueryCharacInfo8dispatchEiiP6Stream（虚覆盖，本文件实现）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    // ORIG T 0x08426e04 _ZN21DB_ReqQueryCharacInfo11_sendResultEiiPKciiiiii
    //（定义 OrigDBInter19.cpp；本 TU 仅声明供 dispatch 调用）
    int _sendResult(int errCode, int slot, char const* name, int characNo,
                    int job, int lev, int growType, int sex, int field2e);
    // ORIG T 0x08426f46 _ZN21DB_ReqQueryCharacInfo17_getGiftRecvCountEj（本文件实现）
    int _getGiftRecvCount(unsigned int mId);
};   // ORIG C1 0x844e65a (sz 0x4: base ctor+vptr only)
class DB_MailBoxSendNewMailWithDBWork : public DBDispatcher
{
public:
    DB_MailBoxSendNewMailWithDBWork() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843f6a0
};   // ORIG C1 0x844e676 (sz 0x4: base ctor+vptr only)
class DB_ReportMannerlessUser_Upgrade : public DBDispatcher
{
public:
    DB_ReportMannerlessUser_Upgrade() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843e7b0
};   // ORIG C1 0x844e692 (sz 0x4: base ctor+vptr only)
class DB_ReportMannerlessUser : public DBDispatcher
{
public:
    DB_ReportMannerlessUser() {}   // ORIG C1 0x844e6ae (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0842796a
};
class DB_UpdateClientCrashDownLog : public DBDispatcher
{
public:
    DB_UpdateClientCrashDownLog() {}   // ORIG C1 0x844e6ca (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08427bce
};
class DB_TimerStatisticsLog : public DBDispatcher { public: DB_TimerStatisticsLog() {}   // ORIG C1 0x844e6e6 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08427c06
};
class DB_SaveUserHandicap : public DBDispatcher
{
public:
    DB_SaveUserHandicap() {}   // ORIG C1 0x844e702 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0842801a _ZN19DB_SaveUserHandicap11makeRequestEjiiiii
    static void makeRequest(unsigned int accId, int param2, int param3,
                            int param4, int param5, int param6);
    MySQL* _getDBHandle();                  // ORIG T 0x08427dfc
    MySQL* _getDBHandleForDevideAccountDB();  // ORIG T 0x08427e28
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08427e54
};
class DB_ForceSaveSimpleSSO : public DBDispatcher
{
public:
    DB_ForceSaveSimpleSSO() {}   // ORIG C1 0x844e71e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08428154
};
class DB_CountOfHumanCertify : public DBDispatcher { public: DB_CountOfHumanCertify() {}   // ORIG C1 0x844e73a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x084282a0
};
class DB_UpdateGiftCount : public DBDispatcher { public: DB_UpdateGiftCount() {}
    // 弱定义发 vtable；真实强定义在 DB_UpdateGiftCount.cpp。ORIG 0x08427112（T, sz 0xd9）
    // 真实体在本 TU 复写（与该 TU 版逐字一致），链接期仍由该强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx;
        (void)slot;
        unsigned int mId = 0;
        int giftCnt = 0;
        stream->operator>>(mId);
        stream->operator>>(giftCnt);
        MySQL* handle = GlobalData::s_db_mgr->GetDBHandle(
            static_cast<ENUM_DB_HANDLE_IDX>(6), static_cast<ENUM_SERVER_GROUP>(0));
        char midBuf[20];
        memset(midBuf, 0, sizeof(midBuf));
        memcpy(midBuf, "member_login", 13);
        handle->set_query(
            "upDate member_login set last_gift_time=unix_timestamp(now()),gift_cnt=gift_cnt+%d where m_id=%s",
            giftCnt, NumberToString(mId, 0));
        handle->exec(true);
        return true;
    }
};   // ORIG C1 0x844e756 (sz 0x4: base ctor+vptr only)
class DB_UpdateMemberDoubtTrade : public DBDispatcher
{
public:
    DB_UpdateMemberDoubtTrade() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842bd00
};   // ORIG C1 0x844e772 (sz 0x4: base ctor+vptr only)
// DB_UpdateItemDropStatistics —— 权威定义已迁至 W8LT_DBTask.h（ODR 修复，P104）。
class DB_UpdatePvPGrade : public DBDispatcher
{
public:
    DB_UpdatePvPGrade() {}   // ORIG C1 0x844e7aa (sz 0x4: base ctor+vptr only)
    // ORIG T 0x084293a0 _ZN17DB_UpdatePvPGrade11makeRequestEv
    static void makeRequest();
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08428b26
};
class DB_InsertValueStatistic : public DBDispatcher { public: DB_InsertValueStatistic() {} 
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842945e
};   // ORIG C1 0x844e7c6 (sz 0x4: base ctor+vptr only)
class DB_InsertDungeonEntrance : public DBDispatcher { public: DB_InsertDungeonEntrance() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08429696
};   // ORIG C1 0x844e7e2 (sz 0x4: base ctor+vptr only)
class DB_InsertDungeonEntrance_hour : public DBDispatcher
{
public:
    DB_InsertDungeonEntrance_hour() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x084298a2
};   // ORIG C1 0x844e7fe (sz 0x4: base ctor+vptr only)
class DB_GetDeathTowerBestRecord : public DBDispatcher
{
public:
    DB_GetDeathTowerBestRecord() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08429ad0
};   // ORIG C1 0x844e81a (sz 0x4: base ctor+vptr only)
struct stDeathTowerRecordMemberInfo_t;   // 前向（stride 0x17，见 makeRequest/dispatch 转写）

class DB_UpdateDeathTowerBestRecord : public DBDispatcher
{
public:
    DB_UpdateDeathTowerBestRecord() {}   // ORIG C1 0x844e836 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0842a35e makeRequest（static 成员）/ 0x0842a11e dispatch
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    // ORIG T 0x0842a35e：返回 void（Ghidra C 实证）。
    static void makeRequest(unsigned int characNo, unsigned int accId,
                            unsigned int field3, stDeathTowerRecordMemberInfo_t const* members,
                            unsigned int count, unsigned int field6);
};
class DB_DeathTowerLoadTopRanker : public DBDispatcher
{
public:
    DB_DeathTowerLoadTopRanker() {}   // ORIG C1 0x844e852 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842a910
    // ORIG T 0x0842a910 _ZN26DB_DeathTowerLoadTopRanker11makeRequestEb（定义文件尾）。
    static void makeRequest(bool reload);
};
class DB_CheckDoubleGuildName : public DBDispatcher { public: DB_CheckDoubleGuildName() {} 
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842a9fc
};   // ORIG C1 0x844e86e (sz 0x4: base ctor+vptr only)
class DB_CheckDoubleGuildAddress : public DBDispatcher { public: DB_CheckDoubleGuildAddress() {} 
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842ab78
};   // ORIG C1 0x844e88a (sz 0x4: base ctor+vptr only)
class DB_ArrangeUserLoginInfo : public DBDispatcher
{
public:
    DB_ArrangeUserLoginInfo() {}   // ORIG C1 0x844e8a6 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0842ad30 _ZN23DB_ArrangeUserLoginInfo11makeRequestEjbj
    static void makeRequest(unsigned int accId, bool flag, unsigned int uid);
    // ORIG T 0x0842acf4（虚覆盖；定义本文件）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_SecurityCardIssue : public DBDispatcher { public: DB_SecurityCardIssue() {} };   // ORIG C1 0x844e8c2 (sz 0x4: base ctor+vptr only)
class DB_SecurityCardUpdateValidityTime : public DBDispatcher
{
public:
    DB_SecurityCardUpdateValidityTime() {}   // ORIG C1 0x844e916 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0842bce8 _ZN33DB_SecurityCardUpdateValidityTime11makeRequestEjj
    static void makeRequest(unsigned int accId, unsigned int flag);
};
class DB_SecurityCardActivate : public DBDispatcher { public: DB_SecurityCardActivate() {} };   // ORIG C1 0x844e8fa (sz 0x4: base ctor+vptr only)
class DB_SecurityCardDisuse : public DBDispatcher
{
public:
    DB_SecurityCardDisuse() {}
    // ORIG T 0x0842b790 _ZN22DB_SecurityCardDisuse11makeRequestEj（定义 StubS3A_deps.cpp）
    static void makeRequest(unsigned int accId);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842b6e6
};   // ORIG C1 0x844e8de (sz 0x4: base ctor+vptr only)
class DB_SecurityCardUpdateFailCnt : public DBDispatcher
{
public:
    DB_SecurityCardUpdateFailCnt() {}   // ORIG C1 0x844e932 (sz 0x4: base ctor+vptr only)
    static void makeRequest(unsigned int accId, unsigned int failCnt);  // ORIG T 0x0842bfaa
};
class DB_SecurityCardRetransfer : public DBDispatcher
{
public:
    DB_SecurityCardRetransfer() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842b800
    void _sendResult(int param_1, char* param_2, char const* param_3,
                     char const* param_4, ENUM_ERROR error);  // ORIG T 0x0842c7be（OrigDBInter19.cpp 定义）
};   // ORIG C1 0x844e96a (sz 0x4: base ctor+vptr only)
class DB_InsertPartyType : public DBDispatcher
{
public:
    DB_InsertPartyType() {}   // ORIG C1 0x844e986 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0842c906
};   // ORIG C1 0x844e986 (sz 0x4: base ctor+vptr only)
class DB_InsertPVPPlayerRate : public DBDispatcher { public: DB_InsertPVPPlayerRate() {} 
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842ca06
};   // ORIG C1 0x844e9a2 (sz 0x4: base ctor+vptr only)
class DB_InsertPVPMap : public DBDispatcher
{
public:
    DB_InsertPVPMap() {}   // ORIG C1 0x844e9be (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0842cb12
};
class DB_LoadDnfHackLog : public DBDispatcher
{
public:
    DB_LoadDnfHackLog() {}   // ORIG C1 0x844e9da (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08422242
    // ORIG T 0x084224ce _ZN17DB_LoadDnfHackLog11makeRequestEij
    static void makeRequest(int userIdx, unsigned int mId);
};
class DB_InsertLevelDungeonPlay : public DBDispatcher { public: DB_InsertLevelDungeonPlay() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842cc98
};   // ORIG C1 0x844e9f6 (sz 0x4: base ctor+vptr only)
class DB_ReloadAutoPunishRuleBackIP : public DBDispatcher
{
public:
    DB_ReloadAutoPunishRuleBackIP() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842cdf6
};   // ORIG C1 0x844ea12 (sz 0x4: base ctor+vptr only)
class DB_ReloadAutoPunishRuleHackType : public DBDispatcher
{
public:
    DB_ReloadAutoPunishRuleHackType() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842d200
};   // ORIG C1 0x844ea2e (sz 0x4: base ctor+vptr only)
class DB_InsertAutoPunishFirstUser : public DBDispatcher
{
public:
    DB_InsertAutoPunishFirstUser() {}   // ORIG C1 0x844ea4a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0842d6ce
};
class DB_InsertAutoPunishSecondUser : public DBDispatcher
{
public:
    DB_InsertAutoPunishSecondUser() {}   // ORIG C1 0x844ea66 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0842deb4
};
class DB_BlackIPMonitor : public DBDispatcher
{
public:
    DB_BlackIPMonitor() {}   // ORIG C1 0x844ea82 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08432ad2
};
class DB_SaveCleanpadPoint : public DBDispatcher
{
public:
    DB_SaveCleanpadPoint() {}   // ORIG C1 0x844ea9e (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08432fe2 _ZN20DB_SaveCleanpadPoint11makeRequestEji
    static void makeRequest(unsigned int point, int accId);
    // ORIG T 0x08432f3e _ZN20DB_SaveCleanpadPoint8dispatchEiiP6Stream（虚覆盖，本文件实现）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_BlackIPMonitorPartLoad : public DBDispatcher
{
public:
    DB_BlackIPMonitorPartLoad() {}   // ORIG C1 0x844eaba (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0843351e _ZN25DB_BlackIPMonitorPartLoad11makeRequestEv
    static void makeRequest();
    // ORIG T 0x084330d4（虚覆盖；定义本文件）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
// ---- DB_U3.cpp 派发族载荷视图（本 TU 复写弱镜像真实体所需；与 DB_U3.cpp 定义逐字一致）----
struct DBU3UnlimitIn
{
    unsigned int m_accId;       // +0x00
    int m_serverId;             // +0x04
    int m_characNo;             // +0x08
    char m_name[0x1d];          // +0x0c
};
struct DBU3AchieveIn
{
    unsigned int m_accId;                                     // +0x00
    int m_characNo;                                           // +0x04
    SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE m_type;       // +0x08
};
struct DBU3WinterCouponIn
{
    unsigned int m_accId;       // +0x00
    unsigned int m_characNo;    // +0x04（dispatch 未用）
    int m_count;                // +0x08
};
struct DBU3AccountHackIn
{
#pragma pack(push, 1)
    unsigned int m_a;        // +0x00
    unsigned int m_accId;    // +0x04
    char m_name[0x1d];       // +0x08
    unsigned int m_money;    // +0x25
};
struct DBU3AccountHackOut
{
    unsigned int m_a;        // +0x00
    unsigned int m_money;    // +0x04
    unsigned int m_id;       // +0x08
};
#pragma pack(pop)
class DB_Insert0712WinterEventCoupon : public DBDispatcher { public: DB_Insert0712WinterEventCoupon() {}
    // 弱定义发 vtable；真实强定义在 DB_U3.cpp。ORIG 0x0842e45e（T, sz 0x1ea）真实体在本 TU
    // 复写（与 DB_U3.cpp 版逐字一致），链接期仍由 DB_U3.cpp 强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)slot;
        SIG_INSERT_0712_WINTER_EVENT_COUPON* sig =
            stream->GetOutBuffer<SIG_INSERT_0712_WINTER_EVENT_COUPON>();
        if (userIdx == 0x112)
        {
            MySQL* db =
                GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
            db->set_query(
                "upDate event_goldcard_info set coupon=coupon+%d where m_id=%s",
                reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count,
                NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0));
            if (!db->exec(true) || db->getAffectedRowCount() == 0)
            {
                db->set_query(
                    "inSert into event_goldcard_info(m_id,coupon) values(%s,%d)",
                    NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0),
                    reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count);
                db->exec(true);
            }
        }
        else
        {
            MySQL* db =
                GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
            db->set_query(
                "upDate event_goldcard_log set cnt=cnt+%d where m_id=%s and occ_date=now()",
                reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count,
                NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0));
            if (!db->exec(true) || db->getAffectedRowCount() == 0)
            {
                db->set_query(
                    "inSert into event_goldcard_log(m_id,cnt,occ_date) values(%s,%d,now())",
                    NumberToString(reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_accId, 0),
                    reinterpret_cast<DBU3WinterCouponIn*>(sig)->m_count);
                db->exec(true);
            }
        }
        return true;
    }
};   // ORIG C1 0x844ead6 (sz 0x4: base ctor+vptr only)
class DB_InsertArchieveEventLog : public DBDispatcher { public: DB_InsertArchieveEventLog() {}
    // 弱定义发 vtable；真实强定义在 DB_U3.cpp。ORIG 0x0842e7be（T, sz 0x97）真实体在本 TU
    // 复写（与 DB_U3.cpp 版逐字一致），链接期仍由 DB_U3.cpp 强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx; (void)slot;
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
        SIG_INSERT_ACHIEVE_EVENT* sig =
            stream->GetOutBuffer<SIG_INSERT_ACHIEVE_EVENT>();
        db->set_query(
            "inSert into log_charac_lev_log(occ_time,m_id,charac_no,achieve_type) "
            "values(now(),%s,%d,%d)",
            NumberToString(reinterpret_cast<DBU3AchieveIn*>(sig)->m_accId, 0),
            reinterpret_cast<DBU3AchieveIn*>(sig)->m_characNo,
            reinterpret_cast<DBU3AchieveIn*>(sig)->m_type);
        db->exec(true);
        return true;
    }
};   // ORIG C1 0x844eaf2 (sz 0x4: base ctor+vptr only)
class DB_InsertDungeonExp : public DBDispatcher
{
public:
    DB_InsertDungeonExp() {}   // ORIG C1 0x844eb0e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0842e944
};
class DB_WriteGeoReject : public DBDispatcher
{
public:
    DB_WriteGeoReject() {}   // ORIG C1 0x844eb2a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844bb70
};
class DB_InsertUnlimitSupportLog : public DBDispatcher { public: DB_InsertUnlimitSupportLog() {}
    // 弱定义发 vtable；真实强定义在 DB_U3.cpp。ORIG 0x0842eb2e（T, sz 0xa1）真实体在本 TU
    // 复写（与 DB_U3.cpp 版逐字一致），链接期仍由 DB_U3.cpp 强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx; (void)slot;
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
        SIG_INSERT_UNLIMIT_SUPPORT_LOG* sig =
            stream->GetOutBuffer<SIG_INSERT_UNLIMIT_SUPPORT_LOG>();
        db->set_query(
            "inSert into event_unlimitsupport_lev(m_id,occ_time,server_id,charac_no,"
            "charac_name) values(%s,now(),%d,%d,'%s')",
            NumberToString(reinterpret_cast<DBU3UnlimitIn*>(sig)->m_accId, 0),
            reinterpret_cast<DBU3UnlimitIn*>(sig)->m_serverId,
            reinterpret_cast<DBU3UnlimitIn*>(sig)->m_characNo,
            reinterpret_cast<DBU3UnlimitIn*>(sig)->m_name);
        db->exec(true);
        return true;
    }
};   // ORIG C1 0x844eb46 (sz 0x4: base ctor+vptr only)
class DB_LoadCleanPadPoint : public DBDispatcher
{
public:
    DB_LoadCleanPadPoint() {}   // ORIG C1 0x844eb62 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0842ecf4 _ZN20DB_LoadCleanPadPoint8dispatchEiiP6Stream（虚覆盖，本文件实现）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_GoblinPadUpdateValidityTime : public DBDispatcher
{
public:
    DB_GoblinPadUpdateValidityTime() {}   // ORIG C1 0x844ebd2 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0842f2ae _ZN30DB_GoblinPadUpdateValidityTime11makeRequestEjj
    static void makeRequest(unsigned int accId, unsigned int flag);
    // ORIG T 0x0842f22a（虚覆盖）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_LoadGameOption : public DBDispatcher
{
public:
    DB_LoadGameOption() {}   // ORIG C1 0x844ebee (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842f7f4
};
class DB_SaveGameOption_1 : public DBDispatcher
{
public:
    DB_SaveGameOption_1() {}   // ORIG C1 0x844ec0a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842ff4e
};
class DB_SaveGameOption_2 : public DBDispatcher
{
public:
    DB_SaveGameOption_2() {}   // ORIG C1 0x844ec26 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0842fd42
};
class DB_SaveCharacterOption : public DBDispatcher { public: DB_SaveCharacterOption() {} 
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08430144
};   // ORIG C1 0x844ec42 (sz 0x4: base ctor+vptr only)
class DB_SaveAccountInfo : public DBDispatcher
{
public:
    DB_SaveAccountInfo() {}   // ORIG C1 0x844eb9a (sz 0x4: base ctor+vptr only)
    virtual ~DB_SaveAccountInfo() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8432646
};
class DB_UpdateCoolTimeItem : public DBDispatcher
{
public:
    DB_UpdateCoolTimeItem() {}   // ORIG C1 0x844ec5e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08430570
};
class DB_UpdateEffectItem : public DBDispatcher
{
public:
    DB_UpdateEffectItem() {}   // ORIG C1 0x844ec7a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08430676
};
class DB_UpdateAvatarJewelSlot : public DBDispatcher
{
public:
    DB_UpdateAvatarJewelSlot() {}   // ORIG C1 0x844ec96 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0843077c（虚覆盖：ORIG vtable _ZTV24DB_UpdateAvatarJewelSlot@0x8c5eec0
    // 槽序 [dispatch][D1][D0]。Ghidra 标 void——实际函数尾无 movzbl/置值，
    // 即 bool 覆盖且函数体末尾无显式 return（eax 残留 exec 返回值，UB 与原版一致）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    // ORIG T 0x0843081c _ZN24DB_UpdateAvatarJewelSlot11makeRequestEjjPK20stAvatarEmblemInfo_t
    static void makeRequest(unsigned int characNo, unsigned int avatarKey,
                            const stAvatarEmblemInfo_t* emblem);
};
class DB_UpdatePvPResult : public DBDispatcher
{
public:
    DB_UpdatePvPResult() {}   // ORIG C1 0x844ecb2 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG 0x0843097e
};
class DB_UpdatePvPLastPlayTime : public DBDispatcher { public: DB_UpdatePvPLastPlayTime() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x084317b2
};   // ORIG C1 0x844ecce (sz 0x4: base ctor+vptr only)
class DB_LoadBloodBestRecord : public DBDispatcher
{
public:
    DB_LoadBloodBestRecord() {}   // ORIG C1 0x844ecea (sz 0x4: base ctor+vptr only)
    // ORIG T 0x084318c4 _ZN22DB_LoadBloodBestRecord11makeRequestEij
    static void makeRequest(int userIdx, unsigned int param2);
    // ORIG T 0x0843199a（虚覆盖；定义本文件）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_UpdateBloodBestRecord : public DBDispatcher { public: DB_UpdateBloodBestRecord() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08431c74
};   // ORIG C1 0x844ed06 (sz 0x4: base ctor+vptr only)
class DB_LoadBreakAwayPreventData : public DBDispatcher {
public:
    DB_LoadBreakAwayPreventData() {}   // ORIG C1 0x844ed22
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843a1c0
    static void makeRequest(bool flag);  // ORIG T 0x08438920
};
class DB_LoadBreakAwayRewardData : public DBDispatcher
{
public:
    DB_LoadBreakAwayRewardData() {}   // ORIG C1 0x844ed3e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843915c
    // ORIG T 0x0843915c _ZN26DB_LoadBreakAwayRewardData11makeRequestEijjR18STBreakAwayRewards
    static void makeRequest(int userIdx, unsigned int param2, unsigned int param3,
                            STBreakAwayRewards& rewards);
};
class DB_UpdateBreakAwayAccruedCera : public DBDispatcher
{
public:
    DB_UpdateBreakAwayAccruedCera() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08439700
};   // ORIG C1 0x844ed5a (sz 0x4: base ctor+vptr only)
class DB_UpdateBreakAwayUvList : public DBDispatcher
{
public:
    DB_UpdateBreakAwayUvList() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08439c00
};   // ORIG C1 0x844ed76 (sz 0x4: base ctor+vptr only)
class DB_UpdateBreakAwayRuckPoint : public DBDispatcher
{
public:
    DB_UpdateBreakAwayRuckPoint() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08439800
};   // ORIG C1 0x844ed92 (sz 0x4: base ctor+vptr only)
class DB_UpdateBreakAwayClearAddInfo : public DBDispatcher
{
public:
    DB_UpdateBreakAwayClearAddInfo() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08439900
};   // ORIG C1 0x844edae (sz 0x4: base ctor+vptr only)
class DB_UpdateBreakAwayDungeonClearCnt : public DBDispatcher
{
public:
    DB_UpdateBreakAwayDungeonClearCnt() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08439a00
};   // ORIG C1 0x844edca (sz 0x4: base ctor+vptr only)
class DB_LoadDimension : public DBDispatcher
{
public:
    DB_LoadDimension() {}   // ORIG C1 0x844ee02 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08431d8c _ZN16DB_LoadDimension11makeRequestEij
    static void makeRequest(int userIdx, unsigned int param2);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08431ece
};
class DB_UpdateDimension : public DBDispatcher
{
public:
    DB_UpdateDimension() {}   // ORIG C1 0x844ee1e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0843230a
};
class DB_InsertCirculationStatistic : public DBDispatcher { public: DB_InsertCirculationStatistic() {}   // ORIG C1 0x844ee3a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x084327ba
};
class DB_LoadAutoMarketConditionsControl : public DBDispatcher { public: DB_LoadAutoMarketConditionsControl() {}   // ORIG C1 0x844ee56 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x084335dc
};
class DB_UpdateAutoMarketConditionsControl : public DBDispatcher { public: DB_UpdateAutoMarketConditionsControl() {}   // ORIG C1 0x844ee72 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08433bb8
};

class DB_UpdateOneDayLetheSkill : public DBDispatcher
{
public:
    DB_UpdateOneDayLetheSkill() {}   // ORIG C1 0x844ee8e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);   // ORIG T 0x08433eca
private:
    bool ProcessInitLethe(SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig);       // 0x0843410c
    bool ProcessUseLethe(SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig);        // 0x084341fc
    bool ProcessSkillConfirm(SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig, int slot);  // 0x08434340
    bool ProcessInitLetheSkill(SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig);  // 0x084345a4
};
class DB_UpdateVendingMachineStatistic : public DBDispatcher { public: DB_UpdateVendingMachineStatistic() {} 
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08434718
};   // ORIG C1 0x844eeaa (sz 0x4: base ctor+vptr only)
class DB_LoadPowerWarStatueRanker : public DBDispatcher
{
public:
    DB_LoadPowerWarStatueRanker() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08438b50
};   // ORIG C1 0x844eec6 (sz 0x4: base ctor+vptr only)
class DB_LoadPowerWarStatueInfo : public DBDispatcher
{
public:
    DB_LoadPowerWarStatueInfo() {}   // ORIG C1 0x844eee2 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08434886 dispatch / 0x08434a8a GetCharacInfo / 0x08434d36
    // GetAvatarInfo（vtable _ZTV @0x8c5ed28：[offtop][_ZTI][dispatch][D1][D0]；
    // D1/D2 0x845d5be / D0 0x845d5ee 为编译器隐式弱符号，不写用户析构）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    bool GetCharacInfo(SIG_LOAD_POWER_WAR_STATUE_INFO* sig, unsigned int characNo);
    bool GetAvatarInfo(SIG_LOAD_POWER_WAR_STATUE_INFO* sig, unsigned int characNo);
};   // ORIG C1 0x844eee2 (sz 0x4: base ctor+vptr only)
class DB_BlackipPrivateSecurityValuation : public DBDispatcher
{
public:
    DB_BlackipPrivateSecurityValuation() {}
    // ORIG T 0x084355ca _ZN34DB_BlackipPrivateSecurityValuation11makeRequestEj
    static void makeRequest(unsigned int accId);
    // ORIG T 0x084354da（虚覆盖；定义本文件）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};   // ORIG C1 0x844eefe (sz 0x4: base ctor+vptr only)
class DB_UpdateServerMessage : public DBDispatcher
{
public:
    DB_UpdateServerMessage() {}   // ORIG C1 0x844ef1a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08435a02
};
class DB_LoadServerMessage : public DBDispatcher
{
public:
    DB_LoadServerMessage() {}   // ORIG C1 0x844ef36 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08435ecc _ZN20DB_LoadServerMessage8dispatchEiiP6Stream（虚覆盖，本文件实现）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_UpdateCharacterDay : public DBDispatcher
{
public:
    DB_UpdateCharacterDay() {}   // ORIG C1 0x844ef52 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08435d12
};
class DB_UpdateNPCRelationship : public DBDispatcher
{
public:
    DB_UpdateNPCRelationship() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08439f00
};   // ORIG C1 0x844ef6e (sz 0x4: base ctor+vptr only)
class DB_SecurityCardAddCertCnt : public DBDispatcher
{
public:
    DB_SecurityCardAddCertCnt() {}   // ORIG C1 0x844ef8a (sz 0x4: base ctor+vptr only)
    static void makeRequest(unsigned int accId);  // ORIG T 0x08436388
};
class DB_LoadExpertJob : public DBDispatcher
{
public:
    DB_LoadExpertJob() {}   // ORIG C1 0x844efa6 (sz 0x4: base ctor+vptr only)
    virtual ~DB_LoadExpertJob() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8436464
};
class DB_SaveExpertJob : public DBDispatcher
{
public:
    DB_SaveExpertJob() {}   // ORIG C1 0x844efc2 (sz 0x4: base ctor+vptr only)
    virtual ~DB_SaveExpertJob() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x84367f4
};
class DB_UpdateExpertJobStatistic : public DBDispatcher
{
public:
    DB_UpdateExpertJobStatistic() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08439e00
};   // ORIG C1 0x844efde (sz 0x4: base ctor+vptr only)
class DB_InsertServerLoadStatistic : public DBDispatcher { public: DB_InsertServerLoadStatistic() {} 
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08436ad4
};   // ORIG C1 0x844effa (sz 0x4: base ctor+vptr only)
class DB_FindFactoryHub : public DBDispatcher
{
public:
    DB_FindFactoryHub() {}   // ORIG C1 0x844f016 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08436ba0
};
class DB_HackIPSave : public DBDispatcher
{
public:
    DB_HackIPSave() {}   // ORIG C1 0x844f032 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08436dec
};
class DB_CancelRestingUserRestrict : public DBDispatcher
{
public:
    DB_CancelRestingUserRestrict() {}   // ORIG C1 0x844f04e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0843712e
};
class DB_SaveBloodDungeon : public DBDispatcher
{
public:
    DB_SaveBloodDungeon() {}   // ORIG C1 0x844f06a (sz 0x4: base ctor+vptr only)
    // ORIG T 0x084374f8
    int SaveBloodDungeonReward(SIG_BLOOD_DUNGEON* sig);
    // ORIG T 0x08437728（虚覆盖，vtable 槽 0）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_LoadPowerWarInfo : public DBDispatcher
{
public:
    DB_LoadPowerWarInfo() {}   // ORIG C1 0x844f086 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08437756
};
class DB_SecuServiceReward : public DBDispatcher
{
public:
    DB_SecuServiceReward() {}   // ORIG C1 0x844f0a2 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08437844 _ZN20DB_SecuServiceReward11makeRequestEjc
    static void makeRequest(unsigned int accId, char flag);
    // ORIG T 0x08437764（虚覆盖）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_Find_Factory_Hub_User : public DBDispatcher
{
public:
    DB_Find_Factory_Hub_User() {}   // ORIG C1 0x844f0be (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08437c50 _ZN24DB_Find_Factory_Hub_User11makeRequestEjPct
    static void makeRequest(unsigned int uid, char* name, unsigned short flag);
    // ORIG T 0x08437a32（虚覆盖；定义本文件）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_SaveConditionEventInfo : public DBDispatcher { public: DB_SaveConditionEventInfo() {} };   // ORIG C1 0x844f0da (sz 0x4: base ctor+vptr only)
class DB_DeletePowerWarStatueMessage : public DBDispatcher { public: DB_DeletePowerWarStatueMessage() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08437d4e
};   // ORIG C1 0x844f0f6 (sz 0x4: base ctor+vptr only)
class DB_LoadNoCacheData : public DBDispatcher
{
public:
    DB_LoadNoCacheData() {}   // ORIG C1 0x844f112 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08437df4
    int LoadBloodDungeonReward(SIG_LOAD_NO_CACHE_DATA* sig);       // ORIG T 0x08437f58
};
class DB_UpdateItemLock : public DBDispatcher
{
public:
    DB_UpdateItemLock() {}   // ORIG C1 0x844f12e (sz 0x4: base ctor+vptr only)
    virtual ~DB_UpdateItemLock() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x843993a
};
class DB_UpdateCreatureItemLock : public DBDispatcher
{
public:
    DB_UpdateCreatureItemLock() {}   // ORIG C1 0x844f14a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08439aba
};
class DB_UpdateAvatarItemLock : public DBDispatcher
{
public:
    DB_UpdateAvatarItemLock() {}   // ORIG C1 0x844f166 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08439b4e
};
class DB_UpdateMemberPlayInfoPcRoom : public DBDispatcher
{
public:
    DB_UpdateMemberPlayInfoPcRoom() {}   // ORIG C1 0x844f182 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08439be2
};
class DB_UpdateCompoundEmblemStatics : public DBDispatcher
{
public:
    DB_UpdateCompoundEmblemStatics() {}   // ORIG C1 0x844f19e (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08439dec _ZN30DB_UpdateCompoundEmblemStatics11makeRequestEijjjjjjj
    static void makeRequest(int userIdx, unsigned int param2, unsigned int param3,
                            unsigned int param4, unsigned int param5,
                            unsigned int param6, unsigned int param7,
                            unsigned int param8);
    // ORIG T 0x08439c6c（虚覆盖；定义本文件）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_AccountHack_CheckMyAccount : public DBDispatcher { public: DB_AccountHack_CheckMyAccount() {}
    // 弱定义发 vtable；真实强定义在 DB_U3.cpp。ORIG 0x08439f06（T, sz 0x237）真实体在本 TU
    // 复写（与 DB_U3.cpp 版逐字一致），链接期仍由 DB_U3.cpp 强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx;
        SIG_CHECK_MY_ACCOUNT_IN_DB* sig =
            stream->GetOutBuffer<SIG_CHECK_MY_ACCOUNT_IN_DB>();
        unsigned int m_id = 0;
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
        char name[0x3b] = { '\0' };
        db->escape_string(name, sig->m_name);   // +0x08
        db->set_query(
            "seLect m_id from charac_info where charac_name='%s' limit 1", name);
        if (!db->exec(true))
            return false;
        if (!db->fetch())
            return false;
        if (!db->get_int(0, m_id))
            return false;
        if (reinterpret_cast<DBU3AccountHackIn*>(sig)->m_accId != m_id)
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8833), true);
            guard.operator*()->operator<<(0x18c);          // SIG_CHECK_MY_ACCOUNT_OK
            guard.operator*()->operator<<(slot);
            DBU3AccountHackOut* out =
                reinterpret_cast<DBU3AccountHackOut*>(
                    guard.operator->()->GetInBuffer<SIG_CHECK_MY_ACCOUNT_OK>());
            out->m_a = reinterpret_cast<DBU3AccountHackIn*>(sig)->m_a;          // +0x00
            out->m_money = reinterpret_cast<DBU3AccountHackIn*>(sig)->m_money;  // +0x25
            out->m_id = m_id;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
        return true;
    }
};   // ORIG C1 0x844f1ba (sz 0x4: base ctor+vptr only)
class DB_SecuServiceUpdateValidityTime : public DBDispatcher
{
public:
    DB_SecuServiceUpdateValidityTime() {}   // ORIG C1 0x844f1d6 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0843a34e _ZN32DB_SecuServiceUpdateValidityTime11makeRequestEjjtPc
    static void makeRequest(unsigned int accId, unsigned int type,
                            unsigned short flag, char* uid);
    // ORIG T 0x0843a264（虚覆盖；定义本文件）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_AutoPunishedSecondUserData : public DBDispatcher
{
public:
    DB_AutoPunishedSecondUserData() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843a462
};   // ORIG C1 0x844f1f2 (sz 0x4: base ctor+vptr only)
class DB_SaveProperDungeonClearCount : public DBDispatcher
{
public:
    DB_SaveProperDungeonClearCount() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843a8b2
};   // ORIG C1 0x844f20e (sz 0x4: base ctor+vptr only)
class DB_LoadPvPBuddy : public DBDispatcher
{
public:
    DB_LoadPvPBuddy() {}   // ORIG C1 0x844f22a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0843a998
};
class DB_InsertPvPBuddy : public DBDispatcher
{
public:
    DB_InsertPvPBuddy() {}   // ORIG C1 0x844f246 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843a9a2
};
class DB_DeletePvPBuddy : public DBDispatcher
{
public:
    DB_DeletePvPBuddy() {}   // ORIG C1 0x844f262 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843a9ac
};
class DB_UpdatePvPBuddy : public DBDispatcher
{
public:
    DB_UpdatePvPBuddy() {}   // ORIG C1 0x844f27e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0843a9b6
};
class DB_DeleteGuild : public DBDispatcher
{
public:
    DB_DeleteGuild() {}   // ORIG C1 0x844f29a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0843a9c0
};
class DB_LoadPvPLiveEventData : public DBDispatcher
{
public:
    DB_LoadPvPLiveEventData() {}   // ORIG C1 0x844f2b6 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0843ac3e
};
class DB_UpdatePvPLiveEventData : public DBDispatcher
{
public:
    DB_UpdatePvPLiveEventData() {}   // ORIG C1 0x844f2d2 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0843ac48
};
class DB_LoadOnlinePreliminaryMember : public DBDispatcher
{
public:
    DB_LoadOnlinePreliminaryMember() {}   // ORIG C1 0x844f2ee (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843ba94
};
class DB_UpdateOnlinePreliminaryMember : public DBDispatcher
{
public:
    DB_UpdateOnlinePreliminaryMember() {}   // ORIG C1 0x844f30a (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0843ba9e（4 条：mov $1; ret）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_LoadOnlinePreliminaryTeam : public DBDispatcher
{
public:
    DB_LoadOnlinePreliminaryTeam() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844a1c0
};   // ORIG C1 0x844f326 (sz 0x4: base ctor+vptr only)
class DB_UpdateOnlinePreliminaryTeam : public DBDispatcher
{
public:
    DB_UpdateOnlinePreliminaryTeam() {}   // ORIG C1 0x844f342 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0843babc（4 条：mov $1; ret）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_UpdateOnlinePreliminaryMemberMatchList : public DBDispatcher
{
public:
    DB_UpdateOnlinePreliminaryMemberMatchList() {}   // ORIG C1 0x844f35e (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0843baa8（4 条：mov $1; ret）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_LoadOnlinePreliminaryTeamMatchList : public DBDispatcher
{
public:
    DB_LoadOnlinePreliminaryTeamMatchList() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844a3c0
};   // ORIG C1 0x844f37a (sz 0x4: base ctor+vptr only)
class DB_UpdateOnlinePreliminaryTeamMatchList : public DBDispatcher
{
public:
    DB_UpdateOnlinePreliminaryTeamMatchList() {}   // ORIG C1 0x844f396 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0843bad0（4 条：mov $1; ret）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_SaveRedeemItemList : public DBDispatcher
{
public:
    DB_SaveRedeemItemList() {}   // ORIG C1 0x844f3b2 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0840e074
};
class DB_LoadAccountCargo : public DBDispatcher
{
public:
    DB_LoadAccountCargo() {}
    // ORIG T 0x0843b6ec _ZN19DB_LoadAccountCargo11makeRequestEij
    static void makeRequest(int userIdx, unsigned int accId);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843b3b6
};   // ORIG C1 0x844f422 (sz 0x4: base ctor+vptr only)
// ---- DB_LoadAccountCargo / DB_SaveAccountCargo 的 SIG_ACCOUNT_CARGO_DATA ----
// [Lane31W8LTDBTaskView] 视图 cast 反模式清理：本地 DB_AccountCargoView 与
// SigTypes_A.h 的 SIG_ACCOUNT_CARGO_DATA 同布局（+0x00 capacity/+0x04 money/
// +0x08 cargo[0xd58]），删本地定义改权威类型直访问（字段名不变）。
// ---- DB_SaveMileage / DB_LoadMileage 的 SIG_MILEAGE_STRUCT 载荷视图 ----
// （SIG m_pad[0xc]，pack(1)；CUserOnlyOrig.cpp 写 +0=accId / +4=characNo /
// +8=mileage）。
#pragma pack(push, 1)
struct DB_MileageView
{
    unsigned int m_mId;       // +0x00（SQL m_id）
    unsigned int m_characNo;  // +0x04（日志 charac_no）
    int m_miles;              // +0x08（SQL miles）
};
#pragma pack(pop)
static_assert(sizeof(DB_MileageView) == 0xc, "DB_MileageView ABI drift");
class DB_SaveMileage : public DBDispatcher
{
public:
    DB_SaveMileage() {}   // ORIG C1 0x844f45a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8443c62
};
class DB_LoadMileage : public DBDispatcher
{
public:
    DB_LoadMileage() {}   // ORIG C1 0x844f476 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x8443d2c
};
class DB_Multi_MailBox_Req_Send : public DBDispatcher { public: DB_Multi_MailBox_Req_Send() {} };   // ORIG C1 0x844f492 (sz 0x4: base ctor+vptr only)
class DB_ReqQueryCharacInfoMailbox : public DBDispatcher
{
public:
    DB_ReqQueryCharacInfoMailbox() {}
    // ORIG T 0x0843c138 _ZN28DB_ReqQueryCharacInfoMailbox11makeRequestEiPKc
    //（报告 DB_ReqQueryCharacInfoMailbox/makeRequest.md；定义 StubS3A_deps.cpp）；
    // Dispatchers.cpp:16783 调用。
    static void makeRequest(int userIdx, const char* name);
};   // ORIG C1 0x844f4ae (sz 0x4: base ctor+vptr only)
class DB_UpdateChuseokPackagePurchase : public DBDispatcher { public: DB_UpdateChuseokPackagePurchase() {}
    // 弱定义发 vtable；真实强定义在 DB_UpdateChuseokPackagePurchase.cpp。ORIG 0x0843c7bc
    // （T, sz 0xfa）真实体在本 TU 复写（与该 TU 版逐字一致），链接期仍由强定义覆盖。
    virtual bool dispatch(int userIdx, int slot, Stream* s)
    {
        (void)userIdx;
        (void)slot;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)9,
                                                      (ENUM_SERVER_GROUP)0);
        unsigned int mId = 0;
        *s >> mId;
        db->set_query(
            "upDate event_1009_package_count set purchase_cnt = purchase_cnt+1 "
            "where m_id=%s",
            NumberToString(mId, 0));
        if (!db->exec(true))
            return false;
        if (db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into event_1009_package_count(m_id, purchase_cnt) "
                "values(%s, %u)",
                NumberToString(mId, 0), 1);
            if (!db->exec(true))
                return false;
        }
        return true;
    }
};   // ORIG C1 0x844f502 (sz 0x4: base ctor+vptr only)
class DB_CheckJoinGuild : public DBDispatcher { public: DB_CheckJoinGuild() {} };   // ORIG C1 0x844f51e (sz 0x4: base ctor+vptr only)
class DB_RequestJoinGuild : public DBDispatcher
{
public:
    DB_RequestJoinGuild() {}   // ORIG C1 0x844f53a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843ccf2
};
// ---- DB_RequestJoinGuild::dispatch 的 SIG_REQUEST_JOIN_GUILD 载荷视图 ----
// （SigTypes_D.h m_pad[0x44] 占位；ORIG dispatch 0x843ccf2 逐偏移读：
// +0x00 guild_name（guild_info WHERE）/+0x18 charac_no(uint)/+0x1c m_id(uint)
// /+0x20 server_group(uint)/+0x24 born_year('%s')/+0x27 memo(escape 源)）。
#pragma pack(push, 1)
struct DB_RequestJoinGuildInView
{
    char m_guildName[0x16];      // +0x00
    char m_pad[2];               // +0x16..+0x17
    unsigned int m_characNo;     // +0x18
    unsigned int m_mId;          // +0x1c
    unsigned int m_serverGroup;  // +0x20
    char m_bornYear[3];          // +0x24..+0x26
    char m_memo[0x1d];           // +0x27..+0x43
};
#pragma pack(pop)
static_assert(sizeof(DB_RequestJoinGuildInView) == 0x44, "DB_RequestJoinGuildInView ABI drift");
// SIG_REQUEST_JOIN_GUILD_DB_REPLY（0x48，自然对齐，不可 pack(1)，同
// DB_JoinGuildInfoView）：+0x00 flag(uint)/+0x04 guild_id(uint)/
// +0x08 master_name[0x1e]/+0x26 guild_name[0x17]/+0x40 lev(int)/
// +0x44 member_count(int)。
struct DB_RequestJoinGuildReplyView
{
    unsigned int m_flag;         // +0x00
    unsigned int m_guildId;      // +0x04
    char m_masterName[0x1e];     // +0x08
    char m_guildName[0x17];      // +0x26
    int m_lev;                   // +0x40
    int m_memberCount;           // +0x44
};
static_assert(sizeof(DB_RequestJoinGuildReplyView) == 0x48, "DB_RequestJoinGuildReplyView ABI drift");
// SIG_JOIN_GUILD_INFO（sizeof 0x64，SigTypes_B.h m_pad 占位）字段视图：
// ORIG dispatch 0x843d286 逐偏移（自然 4 字节对齐布局：flag 后 3 字节 pad）：
// +0x00 byte flag（get_n_rows==0 写 0，成功写 1）、+0x04 uint guild_id /
// +0x08 char master_name[0x1e] / +0x26 char guild_name[0x17] / +0x40 int lev /
// +0x44 int member_count / +0x48 char memo[0x1a]。不可 pack(1)（会偏移至 +0x01）。
struct DB_JoinGuildInfoView
{
    unsigned char m_flag;              // +0x00
    unsigned char m_pad1[3];           // +0x01..+0x03
    unsigned int m_guildId;            // +0x04
    char m_masterName[0x1e];           // +0x08
    char m_guildName[0x17];            // +0x26
    int m_lev;                         // +0x40
    int m_memberCount;                 // +0x44
    char m_memo[0x1a];                 // +0x48
};
class DB_JoinGuildInfo : public DBDispatcher
{
public:
    DB_JoinGuildInfo() {}   // ORIG C1 0x844f556 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x843d286
    void makeRequest(int param1, ENUM_SERVER_GROUP serverGroup,
                     unsigned int param3);   // ORIG 0x843d5fa（参数语义【推断】）
};
class DB_CancelJoinGuild : public DBDispatcher
{
public:
    DB_CancelJoinGuild() {}   // ORIG C1 0x844f572 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843d6ea
    // ORIG T 0x0843d78c _ZN18DB_CancelJoinGuild11makeRequestEijj
    static void makeRequest(int userIdx, unsigned int guildId,
                            unsigned int characNo);
};
class DB_GuildJoinList : public DBDispatcher { public: DB_GuildJoinList() {} };   // ORIG C1 0x844f58e (sz 0x4: base ctor+vptr only)
class DB_DenyJoinGuild : public DBDispatcher { public: DB_DenyJoinGuild() {} };   // ORIG C1 0x844f5aa (sz 0x4: base ctor+vptr only)
class DB_UpdateReturnUserFirstLogin : public DBDispatcher
{
public:
    DB_UpdateReturnUserFirstLogin() {}   // ORIG C1 0x844f5c6 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843dffe
};
class DB_SaveGameOption_QuickChatting : public DBDispatcher
{
public:
    DB_SaveGameOption_QuickChatting() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844a800
};   // ORIG C1 0x844f5fe (sz 0x4: base ctor+vptr only)
class DB_UpdateLimitItemUsage : public DBDispatcher
{
public:
    DB_UpdateLimitItemUsage() {}   // ORIG C1 0x844f652 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0843f022（恒返 1）
};
// DB_LogBuyItem —— 权威声明见 DB_LogBuyItem.h（P104 统一；C1 0x844f66e /
// dispatch 0x0812859a 定义见本 TU，makeRequest 0x0812887e 定义见 CUser.cpp）。
// DB_UpdateRevengeData 权威声明见 W8LT_DBTask.h（FixCond_D 统一）；本 TU 仅提供
// makeRequest/dispatch 定义。
class DB_UpdateGoblinPadAdvicePwdRewardTime : public DBDispatcher
{
public:
    DB_UpdateGoblinPadAdvicePwdRewardTime() {}   // ORIG C1 0x844f6a6 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0843f684 _ZN37DB_UpdateGoblinPadAdvicePwdRewardTime11makeRequestEij（static；定义本文件）
    static void makeRequest(int param1, unsigned int param2);
    // ORIG T 0x0843f5d2（虚覆盖）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_UpdateCharacPlayTime : public DBDispatcher
{
public:
    DB_UpdateCharacPlayTime() {}   // ORIG C1 0x844f6c2 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0843f7ee _ZN23DB_UpdateCharacPlayTime11makeRequestEil
    static void makeRequest(int characNo, long playTick);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0843f75a
};
class DB_SaveExpandEquipslot : public DBDispatcher
{
public:
    DB_SaveExpandEquipslot() {}   // ORIG C1 0x844f6de (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0840da96 _ZN22DB_SaveExpandEquipslot8dispatchEiiP6Stream
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    // ORIG T 0x0843ff48 _ZN22DB_SaveExpandEquipslot20_SaveExpandEquipslotEP25SIG_SAVE_EXPAND_EQUIPSLOT
    bool _SaveExpandEquipslot(SIG_SAVE_EXPAND_EQUIPSLOT* sig);
    // ORIG T 0x0843febe _ZN22DB_SaveExpandEquipslot15_SaveCharacStatEjP27SIG_SAVE_EXPAND_CHARAC_STAT
    bool _SaveCharacStat(unsigned int characNo, SIG_SAVE_EXPAND_CHARAC_STAT* sig);
};
class DB_UpdateFairPvPScore : public DBDispatcher { public: DB_UpdateFairPvPScore() {}   // ORIG C1 0x844f6fa (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843fa78
};
class DB_SavePvPMissionList : public DBDispatcher { public: DB_SavePvPMissionList() {}   // ORIG C1 0x844f716 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843f8fa
};
class DB_InsertResponseTime : public DBDispatcher
{
public:
    DB_InsertResponseTime() {}   // ORIG C1 0x844f732 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0843fc26
};
class DB_UpdateItemMakingSkill : public DBDispatcher { public: DB_UpdateItemMakingSkill() {} 
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08440f3e
};   // ORIG C1 0x844f74e (sz 0x4: base ctor+vptr only)
class DB_TOD_Random_Select_UserAPC : public DBDispatcher
{
public:
    DB_TOD_Random_Select_UserAPC() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844a000
};   // ORIG C1 0x844f76a (sz 0x4: base ctor+vptr only)
class DB_TOD_Reset_UserAPC : public DBDispatcher
{
public:
    DB_TOD_Reset_UserAPC() {}   // ORIG C1 0x844f786 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08440548 _ZN20DB_TOD_Reset_UserAPC14GetAPCUserInfoER8_APCInfo
    // SIGFIX:b28c-28 ORIG rodata _ZZN20DB_TOD_Reset_UserAPC14GetAPCUserInfoER8_APCInfoE19__PRETTY_FUNCTION__
    //   @0x08c5b960 = "bool DB_TOD_Reset_UserAPC::GetAPCUserInfo(_APCInfo&)"（P174）；
    //   唯一调用点 DB_TOD_Reset_UserAPC::dispatch 0x08440331 为语句调用（不消费返回值）。
    bool GetAPCUserInfo(_APCInfo& info);
    // ORIG T 0x084400c8（虚覆盖）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_TOD_Update_UserState : public DBDispatcher { public: DB_TOD_Update_UserState() {}   // ORIG C1 0x844f7a2 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x084403aa
};
class DB_UpdateQuestShopBuyingStatusList : public DBDispatcher
{
public:
    DB_UpdateQuestShopBuyingStatusList() {}   // ORIG C1 0x844f7be (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0840568a
};
class DB_SaveKillMonsterInfo : public DBDispatcher { public: DB_SaveKillMonsterInfo() {}   // ORIG C1 0x844f7f6 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08441200
};
class DB_LoadKillMonsterInfo : public DBDispatcher
{
public:
    DB_LoadKillMonsterInfo() {}   // ORIG C1 0x844f812 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x084414e0 _ZN22DB_LoadKillMonsterInfo8dispatchEiiP6Stream（虚覆盖，本文件实现）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    // ORIG T 0x0844159a _ZN22DB_LoadKillMonsterInfo15LoadMonsterInfoEP26SIG_LOAD_KILL_MONSTER_INFO
    bool LoadMonsterInfo(SIG_LOAD_KILL_MONSTER_INFO* sig);
    // ORIG T 0x084419ac _ZN22DB_LoadKillMonsterInfo10SendResultEiiP26SIG_LOAD_KILL_MONSTER_INFO
    bool SendResult(int userIdx, int slot, SIG_LOAD_KILL_MONSTER_INFO* sig);
};   // ORIG C1 0x844f812 (sz 0x4: base ctor+vptr only)
class DB_SaveTitleBook : public DBDispatcher
{
public:
    DB_SaveTitleBook() {}   // ORIG C1 0x844f82e (sz 0x4: base ctor+vptr only)
    virtual ~DB_SaveTitleBook() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x840db00
};
class DB_SaveAchievement : public DBDispatcher
{
public:
    DB_SaveAchievement() {}   // ORIG C1 0x844f84a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0840deaa
};
class DB_SaveCharacSlotExtend : public DBDispatcher
{
public:
    DB_SaveCharacSlotExtend() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08442e00
};   // ORIG C1 0x844f89e (sz 0x4: base ctor+vptr only)
class DB_SaveGiveAvengerTitleFlag : public DBDispatcher
{
public:
    DB_SaveGiveAvengerTitleFlag() {}   // ORIG C1 0x844f8ba (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08442226
};
class DB_SelectLevelUpReward : public DBDispatcher
{
public:
    DB_SelectLevelUpReward() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x084423cc
};   // ORIG C1 0x844f8d6 (sz 0x4: base ctor+vptr only)
class DB_StatisticCeraShopGiftLog : public DBDispatcher
{
public:
    DB_StatisticCeraShopGiftLog() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08442f80
};   // ORIG C1 0x844f90e (sz 0x4: base ctor+vptr only)
class DB_GetUserRegdate : public DBDispatcher
{
public:
    DB_GetUserRegdate() {}   // ORIG C1 0x844f92a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08442e68
    bool SendResult(int userIdx, int slot, SIG_GET_USER_REGDATE* sig);  // ORIG T 0x0844309c（定义 OrigDBInter19.cpp）
};
class DB_InsertOnTimeRewardUserLog : public DBDispatcher
{
public:
    DB_InsertOnTimeRewardUserLog() {}   // ORIG C1 0x844f962 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08442a56
};
class DB_CheckCharacName : public DBDispatcher
{
public:
    DB_CheckCharacName() {}   // ORIG C1 0x844f97e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0844317e
};   // ORIG C1 0x844f97e (sz 0x4: base ctor+vptr only)
class DB_MailBox_Req_System_Multi_Mail : public DBDispatcher { public: DB_MailBox_Req_System_Multi_Mail() {} };   // ORIG C1 0x844f99a (sz 0x4: base ctor+vptr only)
class DB_CheckSecuReward : public DBDispatcher
{
public:
    DB_CheckSecuReward() {}   // ORIG C1 0x844f9b6 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x084441e6
};   // ORIG C1 0x844f9b6 (sz 0x4: base ctor+vptr only)

class DB_UpdateInGameEventHistory : public DBDispatcher
{
public:
    DB_UpdateInGameEventHistory() {}   // ORIG C1 0x844f9ee (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08444972
};
// DB_UpdateInventoryCapacity 权威声明已迁入 DB_UpdateInventoryCapacity.h（ORIG C1 0x844fa0a，
// sz 0x4；makeRequest 0x08444d12 + dispatch 0x08444bea）。本地占位类删除避免 ODR 重复。
// DB_UpdateAuraAvatarOption 权威声明在 GameBasicsScripts.h（含 makeRequest/dispatch）。
class DB_InsertCeraItemHistory : public DBDispatcher { public: DB_InsertCeraItemHistory() {}
    // 弱定义发 vtable；真实强定义在 DB_U3.cpp。ORIG 0x08445032（T, sz 0xe7）真实体在本 TU
    // 复写（与 DB_U3.cpp 版逐字一致），链接期仍由 DB_U3.cpp 强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx; (void)slot;
        unsigned int senderCharacNo = 0;
        unsigned int receiverCharacNo = 0;
        int itemIdx = 0;
        char flag = 0;
        *stream >> itemIdx;
        *stream >> senderCharacNo;
        *stream >> receiverCharacNo;
        *stream >> flag;
        MySQL* db =
            GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
        db->set_query(
            "inSert into user_items_change_history(ui_id,from_charac_no,to_charac_no,"
            "occ_date,change_type) values(%u,%u,%u,now(),%u)",
            itemIdx, senderCharacNo, receiverCharacNo, (int)flag);
        if (!db->exec(true))
            return false;
        else
            return true;
    }
};   // ORIG C1 0x844fa42 (sz 0x4: base ctor+vptr only)
class DB_UpdateAvatarHistory : public DBDispatcher
{
public:
    DB_UpdateAvatarHistory() {}   // ORIG C1 0x844fa5e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08445246
};
class DB_SaveItemDictionaryFlag : public DBDispatcher
{
public:
    DB_SaveItemDictionaryFlag() {}   // ORIG C1 0x844fa7a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0844547e
};
class DB_SaveMercenaryData : public DBDispatcher
{
public:
    DB_SaveMercenaryData() {}   // ORIG C1 0x844fa96 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x084458c6
};
class DB_HadesPunish : public DBDispatcher
{
public:
    DB_HadesPunish() {}   // ORIG C1 0x844fab2 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08445aa6
    // ORIG T 0x08445dd4 _ZN14DB_HadesPunish11makeRequestEjjjjjPc
    static void makeRequest(unsigned int a, unsigned int b, unsigned int c,
                            unsigned int d, unsigned int e, char* msg);
};
class DB_Check3rdPartyConcent : public DBDispatcher
{
public:
    DB_Check3rdPartyConcent() {}   // ORIG C1 0x844face
    // ORIG T 0x084460c0 _ZN23DB_Check3rdPartyConcent11makeRequestEij（static，无 this）
    static void makeRequest(int uid, unsigned int accId);
    // ORIG T 0x08445eee（虚覆盖）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_SelectCharacMid : public DBDispatcher { public: DB_SelectCharacMid() {}   // ORIG C1 0x844fb06 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08446516
};
class DB_AvatarOptionChange : public DBDispatcher
{
public:
    DB_AvatarOptionChange() {}   // ORIG C1 0x844fb22 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0844687e
};
class DB_SaveObjectBringUp : public DBDispatcher
{
public:
    DB_SaveObjectBringUp() {}   // ORIG C1 0x844fb3e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08446a1e
};
class DB_SaveGameOption_ChattingEmoticon : public DBDispatcher
{
public:
    DB_SaveGameOption_ChattingEmoticon() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844b400
};   // ORIG C1 0x844fb5a (sz 0x4: base ctor+vptr only)
class DB_UpdateBoosterGageData : public DBDispatcher { public: DB_UpdateBoosterGageData() {}
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08446eec
};   // ORIG C1 0x844fb76 (sz 0x4: base ctor+vptr only)
class CLocalChina_DB_LastDay_BreakAway : public DBDispatcher
{
public:
    CLocalChina_DB_LastDay_BreakAway() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0812801a
};   // ORIG C1 0x844fbae (sz 0x4: base ctor+vptr only)
class CLocalGlobal_DB_STATICS_BOSSTOWER : public DBDispatcher
{
public:
    CLocalGlobal_DB_STATICS_BOSSTOWER() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0812825e
};   // ORIG C1 0x844fbca (sz 0x4: base ctor+vptr only)
class DB_UpdateCeraShopPurchaseCount : public DBDispatcher { public: DB_UpdateCeraShopPurchaseCount() {}
    // 弱定义发 vtable；真实强定义在 DB_UpdateCeraShopPurchaseCount.cpp。ORIG 0x0844b0cc
    // （T, sz 0x24f）真实体在本 TU 复写（与 DB_UpdateCeraShopPurchaseCount.cpp 版逐字一致），
    // 链接期仍由该强定义覆盖（行为不变）。helper 声明同该 TU 的类定义（ORIG T
    // 0x0844b450/EjRt、0x0844b680/Ejt、0x0844b808/Ej）。
    bool _selectPurchaseCount(unsigned int mId, unsigned short& out);
    void _updatePurchaseCount(unsigned int mId, unsigned short count);
    void _insertPurchaseCount(unsigned int mId);
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx;
        SIG_CERASHOP_PURCHASE_COUNT* sig =
            stream->GetOutBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
        if (sig == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbbac, 5)(
                "[PURCAHSE BONUS] ptrData is null.");
            return false;
        }
        if (!_selectPurchaseCount(sig->m_id, sig->m_purchaseCnt))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbbb2, 5)(
                "[PURCAHSE BONUS] select query fail.");
            return false;
        }
        if (sig->m_purchaseCnt == 0)
        {
            sig->m_purchaseCnt = 1;
            _insertPurchaseCount(sig->m_id);
        }
        else
        {
            sig->m_purchaseCnt = sig->m_purchaseCnt + 1;
            _updatePurchaseCount(sig->m_id, sig->m_purchaseCnt);
        }
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbbc1), true);
        guard.operator*()->operator<<((int)0x2d9);   // ORIG operator*() → operator<<(int)
        guard.operator*()->operator<<(slot);         // ORIG 0x10(%ebp) 第 3 形参
        SIG_CERASHOP_PURCHASE_COUNT* replySig =
            guard->GetInBuffer<SIG_CERASHOP_PURCHASE_COUNT>();
        if (replySig == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbbc9, 5)(
                "[PURCAHSE BONUS] ptrData is null");
            return false;
        }
        *replySig = *sig;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return true;
    }
};   // ORIG C1 0x844fbe6 (sz 0x4: base ctor+vptr only)
class DB_LoadPuUserCheck : public DBDispatcher
{
public:
    DB_LoadPuUserCheck() {}   // ORIG C1 0x844fc02 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0844bd5c
    // ORIG T 0x0844bee2 _ZN19DB_LoadPuUserCheck11makeRequestEji
    // （static；定义本文件）
    static void makeRequest(unsigned int param1, int param2);
};   // ORIG C1 0x844fc02 (sz 0x4: base ctor+vptr only)
class DB_SavePuUserCheck : public DBDispatcher { public: DB_SavePuUserCheck() {}   // ORIG C1 0x844fc1e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0844bfb8
};
class DB_CheckAddFriend : public DBDispatcher
{
public:
    DB_CheckAddFriend() {}   // ORIG C1 0x844fc56 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844c6e0
};
class DB_SaveInformNotice : public DBDispatcher
{
public:
    DB_SaveInformNotice() {}   // ORIG C1 0x844fc72 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08446fcc
};
class DB_NoticeIngameAdvertisement : public DBDispatcher
{
public:
    DB_NoticeIngameAdvertisement() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844ba80
};   // ORIG C1 0x844fc8e (sz 0x4: base ctor+vptr only)
class DB_RequestCharacSkillInfo : public DBDispatcher { public: DB_RequestCharacSkillInfo() {} };   // ORIG C1 0x844fcaa (sz 0x4: base ctor+vptr only)
class DB_RequestTagCharacInfo : public DBDispatcher
{
public:
    DB_RequestTagCharacInfo() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844bc00
    void sendResult(int param_1, SIG_REQUEST_TAG_CHARAC_INFO* sig);  // ORIG T 0x084478a6（OrigDBInter19.cpp 定义）
};   // ORIG C1 0x844fcc6 (sz 0x4: base ctor+vptr only)
class DB_SavePcRoomPlayTimeEvent : public DBDispatcher
{
public:
    DB_SavePcRoomPlayTimeEvent() {}   // ORIG C1 0x844fce2 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844bd00
    // ORIG T 0x08447bd0 _ZN26DB_SavePcRoomPlayTimeEvent11makeRequestEjjh
    static void makeRequest(unsigned int accId, unsigned int totalTime,
                            unsigned char rewardCnt);
};   // ORIG C1 0x844fce2 (sz 0x4: base ctor+vptr only)

#include "W8LT_DBTask/makeRequest.inc"

class DB_SaveBestClearTime : public DBDispatcher
{
public:
    DB_SaveBestClearTime() {}   // ORIG C1 0x844fcfe (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08447cc4 _ZN20DB_SaveBestClearTime8dispatchEiiP6Stream（虚覆盖，本文件实现）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_AuctionSubMoney : public DBDispatcher
{
public:
    DB_AuctionSubMoney() {}   // ORIG C1 0x844fd1a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08448530
};
class DB_UpdateBlueMarbleEnterCount : public DBDispatcher
{
public:
    DB_UpdateBlueMarbleEnterCount() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844c100
};   // ORIG C1 0x844fd36 (sz 0x4: base ctor+vptr only)
class DB_InsertAttendanceEvent : public DBDispatcher
{
public:
    DB_InsertAttendanceEvent() {}   // ORIG C1 0x844fd52 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x08448afe _ZN24DB_InsertAttendanceEvent11makeRequestEijj
    static void makeRequest(int userIdx, unsigned int param2, unsigned int param3);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08448a48
};
// DB_UpdateBingoEvent —— 权威声明见 W8LT_DBTask.h（GoalAuditDB7 迁移；
// makeRequest 0x8449032 / dispatch 0x08448eb8，定义见本 TU）。
class DB_EventSaveGrowthWeapon : public DBDispatcher
{
public:
    DB_EventSaveGrowthWeapon() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844c280
};   // ORIG C1 0x844fd8a (sz 0x4: base ctor+vptr only)
class DB_EventUpdateGrowthWeaponCharac : public DBDispatcher
{
public:
    DB_EventUpdateGrowthWeaponCharac() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844c3f0
};   // ORIG C1 0x844fda6 (sz 0x4: base ctor+vptr only)
class DB_EventUpdateGrowthWeaponTimepiece : public DBDispatcher { public: DB_EventUpdateGrowthWeaponTimepiece() {}
    // 弱定义发 vtable；真实强定义在 DB_LinkMisc4.cpp。ORIG 0x084496f4（T, sz 0xf5）真实体在本
    // TU 复写（与 DB_LinkMisc4.cpp 版逐字一致），链接期仍由该强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx; (void)slot;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
        unsigned int m_id = 0;
        short timepiece = 0;
        int infinityweapon_id = 0;
        *stream >> m_id;
        *stream >> timepiece;
        *stream >> infinityweapon_id;
        if (infinityweapon_id != 0)
        {
            db->set_query("upDate event_1207_growthweapon set  reward_occ_date = now(), "
                          "timepiece = %d, infinityweapon_id = %d  where m_id = %u",
                          (int)timepiece, infinityweapon_id, m_id);
        }
        else
        {
            db->set_query("upDate event_1207_growthweapon set  timepiece = %d, "
                          "infinityweapon_id = %d  where m_id = %u",
                          (int)timepiece, infinityweapon_id, m_id);
        }
        if (!db->exec(true))
            return false;
        return true;
    }
};   // ORIG C1 0x844fdc2 (sz 0x4: base ctor+vptr only)
class DB_EventGrowthWeaponReset : public DBDispatcher { public: DB_EventGrowthWeaponReset() {}
    // 弱定义发 vtable；真实强定义在 DB_LinkMisc4.cpp。ORIG 0x084498fe（T, sz 0x81）真实体在本
    // TU 复写（与 DB_LinkMisc4.cpp 版逐字一致），链接期仍由该强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx; (void)slot;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
        unsigned int m_id = 0;
        *stream >> m_id;
        db->set_query("deLete from event_1207_growthweapon where m_id = %u", m_id);
        if (!db->exec(true))
            return false;
        return true;
    }
};   // ORIG C1 0x844fdde (sz 0x4: base ctor+vptr only)
class DB_EventAccountCharacterLevelUpOnceGift : public DBDispatcher
{
public:
    DB_EventAccountCharacterLevelUpOnceGift() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844c5b0
};   // ORIG C1 0x844fe32 (sz 0x4: base ctor+vptr only)
class DB_AvatarColorChange : public DBDispatcher
{
public:
    DB_AvatarColorChange() {}   // ORIG C1 0x844fe4e (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x08446944
};
class DB_TestTradeRestrict : public DBDispatcher { public: DB_TestTradeRestrict() {}
    // 弱定义发 vtable；真实强定义在 DB_LinkMisc4.cpp。ORIG 0x0844a0be（T, sz 0x73）
    // 的真实体（同 DB_LinkMisc4.cpp 版）在此处复写，使本 TU 的 W 槽与 ORIG 逐条一致；
    // 链接期仍由 DB_LinkMisc4.cpp 的强定义覆盖（行为不变）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream)
    {
        (void)userIdx; (void)slot;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
        unsigned int charac_no = 0;
        *stream >> charac_no;
        db->set_query("upDate user_items set trade_restrict = 1 where charac_no = %u", charac_no);
        db->exec(true);
        return true;
    }
};   // ORIG C1 0x844fe6a (sz 0x4: base ctor+vptr only)
// DB_UpdateComboSkill —— 权威声明见 W8LT_DBTask.h（GoalAuditDB7 迁移；
// dispatch 0x0844a8ea / setComboSkill 0x0844aa4c，定义见本 TU）。
class DB_SaveCharacInformNotice : public DBDispatcher
{
public:
    DB_SaveCharacInformNotice() {}   // ORIG C1 0x844fea2 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0844c170
};
class DB_LoadCharacHotKeyOption : public DBDispatcher
{
public:
    DB_LoadCharacHotKeyOption() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844c560
};   // ORIG C1 0x844feda (sz 0x4: base ctor+vptr only)
class DB_LogIngameAdvertisement : public DBDispatcher
{
public:
    DB_LogIngameAdvertisement() {}   // ORIG C1 0x844fef6 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0844c304
};
class DB_LogCeraShopError : public DBDispatcher
{
public:
    DB_LogCeraShopError() {}   // ORIG C1 0x844ff12 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844c40a
};
// ---- DB_LogCeraShopError::dispatch 的 SIG_CERASHOP_ERROR_LOG 载荷视图 ----
// （SigTypes_E.h m_pad[0x1c] 占位；ORIG dispatch 0x844c40a 逐偏移读：
// +0x00 m_id(uint)/+0x04 charac_no(uint)/+0x08 server_id(uint)/+0x0c
// channel_no(uint)/+0x10 type(short,SQL %d movswl)/+0x14 err_code(uint)/
// +0x18 goods_id(int)）。
struct DB_CerashopErrorLogView
{
    unsigned int m_mId;        // +0x00
    unsigned int m_characNo;   // +0x04
    unsigned int m_serverId;   // +0x08
    unsigned int m_channelNo;  // +0x0c
    short m_type;              // +0x10
    unsigned int m_errCode;    // +0x14
    int m_goodsId;             // +0x18
};
static_assert(sizeof(DB_CerashopErrorLogView) == 0x1c, "DB_CerashopErrorLogView ABI drift");
class DB_GetPremiumSelectedCubeInfo : public DBDispatcher
{
public:
    DB_GetPremiumSelectedCubeInfo() {}   // ORIG C1 0x844ff2e (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0844c4e2（虚覆盖；定义本文件）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};
class DB_SaveCerashopAddRestrictType : public DBDispatcher
{
public:
    DB_SaveCerashopAddRestrictType() {}   // ORIG C1 0x844ff66 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x080e14f6 _ZN30DB_SaveCerashopAddRestrictType8dispatchEiiP6Stream
    // （虚槽 2 覆盖；定义在 DB_CerashopAddRestrict.cpp）
    virtual bool dispatch(int a, int b, Stream* stream);
};
class DB_LoadCerashopAddRestrictType : public DBDispatcher
{
public:
    DB_LoadCerashopAddRestrictType() {}   // ORIG C1 0x844ff82 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x080e1aca _ZN30DB_LoadCerashopAddRestrictType8dispatchEiiP6Stream
    // （虚槽 2 覆盖；定义在 DB_CerashopAddRestrict.cpp）
    virtual bool dispatch(int a, int b, Stream* stream);
};
class DB_SaveEventLevelupSupport : public DBDispatcher
{
public:
    DB_SaveEventLevelupSupport() {}   // ORIG C1 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844c8d0
};   // ORIG C1 0x844ffba (sz 0x4: base ctor+vptr only)
class DB_LoadEventLevelupSupport : public DBDispatcher
{
public:
    DB_LoadEventLevelupSupport() {}   // ORIG C1 0x844ffd6 (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844c9b0
    // ORIG T 0x0844d0a6 _ZN26DB_LoadEventLevelupSupport11makeRequestEii
    static void makeRequest(int param1, int param2);
};
class DB_SelectAllowProxyUser : public DBDispatcher
{
public:
    DB_SelectAllowProxyUser() {}   // ORIG C1 0x844fff2 (sz 0x4: base ctor+vptr only)
    // ORIG T 0x0844d2d4 _ZN23DB_SelectAllowProxyUser11makeRequestEij
    static void makeRequest(int userIdx, unsigned int accId);
    // ORIG T 0x0844d17c（虚覆盖；定义本文件）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
};





namespace APSystem
{
class DB_LoadActionPoint : public DBDispatcher { public: DB_LoadActionPoint() {} };   // ORIG C1 0x844dd7a (sz 0x4: base ctor+vptr only)
class DB_LoadRewardMedal : public DBDispatcher { public: DB_LoadRewardMedal() {} };   // ORIG C1 0x844ddb2 (sz 0x4: base ctor+vptr only)
class DB_UpdateRewardMedal : public DBDispatcher { public: DB_UpdateRewardMedal() {} };   // ORIG C1 0x844ddce (sz 0x4: base ctor+vptr only)

}  // namespace APSystem
// 注意：advancealtar::DB_Update 权威定义在 advancealtar.h（勿重复）；
// DB_Load/DB_StatisticAdvanceAltar 尚无权威头，保留最小空类型（ORIG C1 W：
// DB_Load @0x844ddea / DB_StatisticAdvanceAltar @0x844fc3a，基类构造+vptr）。
namespace advancealtar
{
class DB_Load : public DBDispatcher { public: DB_Load() {} };   // ORIG C1 0x844ddea
class DB_StatisticAdvanceAltar : public DBDispatcher { public: DB_StatisticAdvanceAltar() {} };   // ORIG C1 0x844fc3a
}  // namespace advancealtar


// ---------------------------------------------------------------------------
// 本批四个 makeRequest 真实定义（报告 docs/class_func_reports/…/makeRequest.md）
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/makeRequest_1.inc"


#include "W8LT_DBTask/makeRequest_2.inc"


#include "W8LT_DBTask/dispatch_2.inc"


// ---------------------------------------------------------------------------
// encryptAES —— ORIG T 0x083fbe2f（nm size 0x14d；DBThread.cpp:532，报告缺失 ⇒
// 按 §3.2 objdump 切片转写）。与 ARAD::encryptAES(0x0819e997) /
// ARAD_CRYPT::encryptAES(0x08184abc) / encryptAES_(0x0846c398) **四体逐 89 条
// 指令相同**（去掉地址/常量后 diff 零行），唯一差异 = logFormat 常量组：
//   file = "DBThread.cpp"（0x08c36714）、line = 0x214 = 532、
//   func = __PRETTY_FUNCTION__（ORIG 局部串符号
//   _ZZ10encryptAESP9CRijndaelPcS1_E19__PRETTY_FUNCTION__ @0x08c5db00 r size 0x2b
//   = "char* encryptAES(CRijndael*, char*, char*)"，变参首项再次传该串）、
//   末项 "Encrypt Error"（0x08c366f9）、fmt "[%s][%d][%s]"（0x08c36707）。
// 语义：strlen → GetBlockSize → 块对齐 paddedLen → Pad → 虚槽 +0xc Encrypt →
//   结果 != 0x6fffffff 即 logFormat 并返 NULL；否则 Binary2Hex(len) 且
//   outHex[2*len+1] = '\0'（ORIG 0x083fbf47 把 2*len+1 **写回 srcLen 槽 -0x18**）。
// ---------------------------------------------------------------------------
char* encryptAES(CRijndael* cipher, char* inData, char* outHex)
{
    char enc[132];
    int srcLen = strlen(inData);
    int paddedLen = 0;
    int blockSize = cipher->IMethod::GetBlockSize(NULL);
    if (blockSize >= 0)
    {
        if (srcLen % blockSize != 0)
        {
            paddedLen = (srcLen / blockSize + 1) * blockSize;
        }
        else
        {
            paddedLen = srcLen;
        }
    }
    else
    {
        paddedLen = srcLen;
    }
    if (blockSize >= 0)
    {
        cipher->IMethod::Pad(inData, srcLen, NULL);
    }
    int result = cipher->Encrypt(inData, enc, paddedLen);
    if (result != 0x6fffffff)
    {
        LogManager::logFormat(
            1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x214,
            "[%s][%d][%s]", __PRETTY_FUNCTION__, 0x214, "Encrypt Error");
        return NULL;
    }
    srcLen = paddedLen * 2 + 1;
    DNFFLib::Binary2Hex(reinterpret_cast<unsigned char*>(enc), paddedLen, outHex);
    outHex[srcLen] = '\0';
    return outHex;
}

#include "W8LT_DBTask/dispatch_3.inc"


#include "W8LT_DBTask/makeRequest_3.inc"


#include "W8LT_DBTask/makeRequest_4.inc"



namespace OS_API
{

#include "W8LT_DBTask/uSleep_1.inc"
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
}

}  // namespace OS_API

// ---------------------------------------------------------------------------
// DBTask 生命周期
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/DBTask.inc"
{
    RegistDispatcher();
}


#include "W8LT_DBTask/put_compressed_blob_data.inc"
                              char** outStr);

// +0x000 实为 charac_no（CUser.cpp:5738 出站包填 getCurCharacNo；本类 SaveSkill
// 用作 SQL where 键）。SIG_LOAD_SKILL 该段声明为 m_pad0[4]，此处视图具名。
#pragma pack(push, 1)
struct DB_SaveSkillCharacView
{
    unsigned int m_characNo;   // +0x000（skill.charac_no）
};
#pragma pack(pop)

#include "W8LT_DBTask/ChangeSlot.inc"


#include "W8LT_DBTask/dispatch_4.inc"


#include "W8LT_DBTask/MoveAvatarItem.inc"


#include "W8LT_DBTask/DelAvatarItem.inc"


#include "W8LT_DBTask/dispatch_5.inc"


#include "W8LT_DBTask/dispatch_6.inc"


#include "W8LT_DBTask/dispatch_7.inc"


#include "W8LT_DBTask/dispatch_8.inc"


#include "W8LT_DBTask/dispatch_9.inc"


#include "W8LT_DBTask/SaveSkill.inc"


#include "W8LT_DBTask/SendResult.inc"


// ---------------------------------------------------------------------------
// DB_LoadTicket —— ORIG 6 符号（报告 docs/class_func_reports/DB_LoadTicket/）
// C1 0x8419572 / D1 0x841958e / D0 0x84195be / dispatch 0x84195dc /
// LoadTicket 0x841968a / SendResult 0x8419770。
// ---------------------------------------------------------------------------

// [Lane31W8LTDBTaskView] +0x000 已由 Lane38 字段化至 SigTypes_C.h
// SIG_LOAD_TICKET（匿名 union `unsigned int m_characNo`@+0x000，static_assert
// offsetof==0），本地 DB_LoadTicketCharacView 视图删除，改用权威直访问。

#include "W8LT_DBTask/dispatch_10.inc"


#include "W8LT_DBTask/LoadTicket.inc"


#include "W8LT_DBTask/SendResult_1.inc"


// ---------------------------------------------------------------------------
// DB_DeleteChannel —— ORIG 5 符号：C1 0x8402892 / D1 0x84028ae / D0 0x84028de /
// dispatch 0x84028fc / DeleteChannel 0x84029ae / SendResult 0x8402aec。
// ---------------------------------------------------------------------------

// SIG_DELETE_CHANNEL 该段声明为 m_pad[0xc]，此处视图具名
// （game_channel.gc_no / 分组号）。
#pragma pack(push, 1)
struct DB_DeleteChannelView
{
    int m_gcNo;       // +0x000（game_channel.gc_no）
    // +0x004：ORIG 0x84029ae `mov 0x4(%eax),%edx` 单步直落 ⇒ 字段与形参
    // 同型（ENUM_SERVER_GROUP）；int 字段 + cast 恒产 `%eax` 中转双 mov（P254）。
    ENUM_SERVER_GROUP m_field4;
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_11.inc"


#include "W8LT_DBTask/DeleteChannel.inc"


#include "W8LT_DBTask/SendResult_2.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x1271), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator->()->put_binary(sig, 0xc);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// SIG_INSERT_CHANNEL 该段声明为 m_pad[0x5c]，此处视图具名
// （game_channel.gc_no/gc_ip/gc_port/gc_max/gc_game/gc_channel/gc_ch_group/
//  gc_channeltype/gc_type + 选库 serverGroup；偏移全照 ORIG 汇编 0x8403734）。
#pragma pack(push, 1)
struct DB_InsertChannelView
{
    int m_gcNo;                  // +0x00（values %d / where gc_no）
    char m_pad4[2];              // +0x04
    char m_gcIp[0x10];           // +0x06（'%s'）
    unsigned short m_gcPort;     // +0x16（movzwl 零扩展）
    short m_gcMax;               // +0x18（movswl 符号扩展）
    char m_gcGame;               // +0x1a（movsbl 符号扩展）
    char m_gcChannel[0x1e];      // +0x1b（'%s'）
    short m_gcChGroup;           // +0x39（cwtl 符号扩展）
    char m_gcChanneltype[0x19];  // +0x3b（'%s'）
    int m_gcType;                // +0x54（%d）
    ENUM_SERVER_GROUP m_serverGroup;  // +0x58（GetDBHandle 第3参）
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_12.inc"


#include "W8LT_DBTask/InsertChannel.inc"


// ---------------------------------------------------------------------------
// DB_InsertTicket —— ORIG 4 符号：C1 0x8403a4e / D1 0x8403a6a / D0 0x8403a9a /
// dispatch 0x8403ab8 / InsertTicket 0x8403b08。
// ---------------------------------------------------------------------------

// SIG_INSERT_TICKET 该段声明为 m_pad[0x8]，此处视图具名
// （village_ticket 的 charac_no / ticket 类型）。
#pragma pack(push, 1)
struct DB_InsertTicketView
{
    unsigned int m_village;   // +0x000（inSert village_ticket %u）
    int m_field4;             // +0x004（%d）
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_13.inc"


#include "W8LT_DBTask/InsertTicket.inc"


// ---------------------------------------------------------------------------
// DB_LoadDungeonClear —— ORIG 8 符号（报告 docs/class_func_reports/DB_LoadDungeonClear/）
// C1 0x8419852 / D1 0x841986e / D0 0x841989e / dispatch 0x84198bc /
// AradLoadMemberDungeonClear 0x841996e / AradLoadDungeonClear 0x8419a76 /
// LoadDungeonClear 0x8419db4 / SendResult 0x8419f0a。
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/dispatch_14.inc"


#include "W8LT_DBTask/LoadDungeonClear.inc"


#include "W8LT_DBTask/AradLoadMemberDungeonClear.inc"


#include "W8LT_DBTask/AradLoadDungeonClear.inc"


#include "W8LT_DBTask/SendResult_3.inc"


// DB_UpdateLogout —— ORIG 7 符号（本类实现 4 个非平凡方法 + C1/D1/D0/vtable）
// C1 0x84183d8 / D1 0x84183f4 / D0 0x8418424 / dispatch 0x8418612 /
// UpdateAccountMemberBonusFatigue 0x8418442 / SendResult 0x841852e /
// UpdateLogout 0x8418a96。
// SIG_UPDATE_LOGOUT 布局（CUser.cpp 实证 + 本类 objdump）：+0x00 accId(u32)、
// +0x08 gcNo、+0x0e serverId、+0x12 byte 状态>1 标志、+0x13 playTime、
// +0x17/+0x1b int、+0x1f u16 fatigue、+0x21 byte PuUser、+0x22 byte pcbang、
#pragma pack(push, 1)
struct DB_UpdateLogoutSigView
{
    unsigned int m_accId;        // +0x00
    char m_pad04[4];             // +0x04
    unsigned int m_gcNo;         // +0x08
    unsigned short m_fatigue;    // +0x0c
    int m_serverId;              // +0x0e
    unsigned char m_stateFlag;   // +0x12
    int m_playTime;              // +0x13
    int m_field17;               // +0x17
    int m_field1b;               // +0x1b
    unsigned short m_field1f;    // +0x1f
    unsigned char m_puUser;      // +0x21
    unsigned char m_pcbang;      // +0x22
    int m_field23;               // +0x23
    char m_field27[0x10];        // +0x27
    char m_field37[0xc];         // +0x37
    char m_field43[0x4];         // +0x43
    char m_mac[0x41];            // +0x47
    unsigned char m_field88;     // +0x88
    unsigned short m_field89;    // +0x89
};
#pragma pack(pop)

#include "W8LT_DBTask/UpdateAccountMemberBonusFatigue.inc"


#include "W8LT_DBTask/SendResult_4.inc"


#include "W8LT_DBTask/UpdateLogout.inc"


#include "W8LT_DBTask/dispatch_15.inc"


#include "W8LT_DBTask/RegistDispatcher.inc"


#include "W8LT_DBTask/UnregistDispatcher.inc"


#include "W8LT_DBTask/HandleEvents.inc"


#include "W8LT_DBTask/DBProcess.inc"


// [P5·2026-09-11] stPvPTempTable_t 的析构强定义（`#include "…/dtor_stPvPTempTable_t.inc"` +
//   空体块）已删 —— 与 P3 的 UsedFatigueRewardItem::Data 同源修法，理由见结构体处注释。

static_assert(sizeof(DBTask) == 4 + 852 * 4,     // = 0xd54
              "DBTask layout drift (m_curQueueIdx@+0x00 + dispatchers[852]@+0x04)");

#include "W8LT_DBTask/makeRequest_5.inc"


#include "W8LT_DBTask/makeRequest_6.inc"


// ---------------------------------------------------------------------------
// 本批八个 DB makeRequest 真实定义（报告 docs/class_func_reports/…/makeRequest.md
// 全文已读，逐指令对应；Acquire 行号常量即 ORIG DBThread.cpp 内嵌行号）
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/makeRequest_7.inc"


#include "W8LT_DBTask/makeRequest_8.inc"


#include "W8LT_DBTask/makeRequest_9.inc"


#include "W8LT_DBTask/makeRequest_10.inc"


#include "W8LT_DBTask/makeRequest_11.inc"


#include "W8LT_DBTask/makeRequest_12.inc"


#include "W8LT_DBTask/makeRequest_13.inc"


#include "W8LT_DBTask/makeRequest_14.inc"


#include "W8LT_DBTask/makeRequest_15.inc"


#include "W8LT_DBTask/makeRequest_16.inc"



#include "W8LT_DBTask/makeRequest_17.inc"


// ---------------------------------------------------------------------------
// LinkE1 批 C：DB makeRequest 真实定义（报告 docs/class_func_reports/<类>/makeRequest.md；
// DB_Find_Factory_Hub_User 无报告，objdump 0x08437c50 转写）。
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/makeRequest_18.inc"


#include "W8LT_DBTask/makeRequest_19.inc"


#include "W8LT_DBTask/makeRequest_20.inc"


#include "W8LT_DBTask/makeRequest_21.inc"


// ORIG 0x08445dd4 sz 0x11a（_ZN14DB_HadesPunish11makeRequestEjjjjjPc）：
// Acquire("DBThread.cpp",0xad59) → CStreamGuard(stream,true)
//   → <<0x2b6(int) → <<-1(int) → sig=GetInBuffer<SIG_HADES_PUNISH>()
//   → sig+0x00..+0x10(uint)=a..e；msg!=0 时 strcpy(sig+0x14, msg)
//   → put(DB_Q(2))。
// ---- SIG_HADES_PUNISH（0x24）字段视图（makeRequest 0x08445dd4 写入
// +0x00..+0x10 五 uint + strcpy +0x14；dispatch 0x08445aa6 读
// m_id/punish_type/days/apply_flag）----
#pragma pack(push, 1)
struct DB_HadesPunishView
{
    unsigned int m_mId;        // +0x00
    unsigned int m_punishType; // +0x04
    unsigned int m_days;       // +0x08
    unsigned int m_applyFlag;  // +0x0c
    unsigned int m_field10;    // +0x10（makeRequest 第五实参 e）
    char m_msg[0x10];          // +0x14（strcpy(msg)，0x14..0x23 尾区）
};
#pragma pack(pop)
#include "W8LT_DBTask/makeRequest_22.inc"


#include "W8LT_DBTask/makeRequest_23.inc"


#include "W8LT_DBTask/makeRequest_24.inc"


#include "W8LT_DBTask/makeRequest_25.inc"


#include "W8LT_DBTask/makeRequest_26.inc"

// ---- StubS3A_deps 批：makeRequest 定义（声明在本 TU 类定义内）----

#include "W8LT_DBTask/makeRequest_27.inc"


#include "W8LT_DBTask/makeRequest_28.inc"


#include "W8LT_DBTask/makeRequest_29.inc"



#include "W8LT_DBTask/makeRequest_30.inc"


#include "W8LT_DBTask/makeRequest_31.inc"

#include "W8LT_DBTask/makeRequest_32.inc"


#include "W8LT_DBTask/dispatch_16.inc"


#include "W8LT_DBTask/dispatch_17.inc"

// ---------------------------------------------------------------------------
// DB_UpdateOneDayLetheSkill —— ORIG 5 个 T 函数逐指令转写（OrigLethe 批）：
//   T _ZN25DB_UpdateOneDayLetheSkill8dispatchEiiP6Stream  @0x08433eca
//   T _ZN25DB_UpdateOneDayLetheSkill16ProcessInitLetheEP35SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD  @0x0843410c
//   T _ZN25DB_UpdateOneDayLetheSkill15ProcessUseLetheEP35SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD  @0x084341fc
//   T _ZN25DB_UpdateOneDayLetheSkill19ProcessSkillConfirmEP35SIG_RESULT_PREMIUM_LETHE_SKILL_LOADi  @0x08434340
//   T _ZN25DB_UpdateOneDayLetheSkill21ProcessInitLetheSkillEP35SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD  @0x084345a4
// 依赖 ORIG T _Z24put_compressed_blob_dataP5MySQLiPciPS1_ @0x083fbd06（本文件补定义；
// get 版定义于 DB_Login.cpp:1965）。字符串常量取自 .rodata（0x8c52a54 区）。
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/put_compressed_blob_data_1.inc"


#include "W8LT_DBTask/dispatch_18.inc"


#include "W8LT_DBTask/ProcessInitLethe.inc"


#include "W8LT_DBTask/ProcessUseLethe.inc"


#include "W8LT_DBTask/ProcessSkillConfirm.inc"


#include "W8LT_DBTask/ProcessInitLetheSkill.inc"

// ============================================================================
// DB_SaveEventItem —— 批 OrigDBEventItem
// ORIG T（nm 实证）：dispatch 0x0841a7e4 / _CheckAlreadyGetYegamItem 0x0841a51a /
//   _HardCodeEventItems 0x0841a662 / _UpdateEventYegamInfo 0x0841a5ea
// 报告：docs/class_func_reports/DB_SaveEventItem/*（逐指令转写）。
// 引用方：DBTask::RegistDispatcher slot 76；类权威声明在 W8LT_DBTask.h。
// ============================================================================

// WongWork 活动族 static 接口（本 TU 调用视图；实现归 LinkClosureB_Misc.cpp /
// WongWorkEvents_A.h 域批）。ORIG dispatch 调用侧对 Check* 用 test %al 消费、
// 对 Update* 用 mov %al 消费返回值——本 TU 声明为单字节/布尔返回以产生同形
// 调用（返回类型不参与 mangling，与其它 TU 的 int/void 定义链接兼容）。
namespace WongWork
{
struct EventATI
{
    static bool CheckAlreadyGetATIItem(unsigned int uid, int itemNo);
    static bool UpdateEventATIInfo(unsigned int uid, int itemNo);
};
struct EventSigong
{
    static bool CheckAlreadyGetSigongItem(unsigned int id, int itemNo);
    static bool UpdateEventSigongInfo(unsigned int id, int itemNo);
};
struct EventEverland
{
    static bool CheckAlreadyGetEverlandItem(unsigned int id, int itemNo);
    static bool UpdateEventEverlandInfo(unsigned int id, int itemNo);
};
struct EventMuz
{
    static bool CheckAlreadyGetMuzItem(unsigned int uid, int itemNo);
    static bool UpdateEventMuzInfo(unsigned int uid, int itemNo);
};
struct EventAlttle
{
    static bool CheckAlreadyGetAlttleItem(unsigned int uid, int itemNo);
    static bool UpdateEventAlttleInfo(unsigned int uid, int itemNo);
};
struct EventFCCombo
{
    static bool CheckAlreadyFCComboEventItem(unsigned int id, int itemNo);
    static bool UpdateFCComboEventInfo(unsigned int id, int itemNo);
};
struct EventCommon
{
    static unsigned int InsertEventItem2EventTable(unsigned int characNo,
                                                   unsigned int itemId,
                                                   unsigned int stackCount,
                                                   unsigned int eventCode);
    static void LoadCharacEventItem(unsigned int eventId,
                                    unsigned int characNo,
                                    unsigned int loadId);
};
}  // namespace WongWork

#include "W8LT_DBTask/_CheckAlreadyGetYegamItem.inc"


#include "W8LT_DBTask/_UpdateEventYegamInfo.inc"


#include "W8LT_DBTask/_HardCodeEventItems.inc"


#include "W8LT_DBTask/dispatch_19.inc"


// ============================================================================
// DB_BillingRecord —— ORIG dispatch 0x08424574 / InsertBuyUser 0x08424602 /
// InsertSaleEntry 0x084247e0（计费：prod_buy_user 首购登记 + prod_sale_entry_
// <季度> 交易流水；句柄 GetDBHandle(DB_HANDLE_7, E_SERVER_GROUP_NONE)）。
// SIG 字段经 DB_BillingRecordSigView 组件视图强转访问（ORIG 每字段访问均
// 重载 sig 参数再 disp，视图表达式的 mov/add 形态与之一致）。
// ============================================================================

#include "W8LT_DBTask/dispatch_20.inc"


#include "W8LT_DBTask/InsertBuyUser.inc"


#include "W8LT_DBTask/InsertSaleEntry.inc"
{
    int buyType;
    struct tm tmVal;
    time_t t;
    char quarter[5] = { 0 };
    MySQL* db;
    time(&t);
    localtime_r(&t, &tmVal);
    sprintf(quarter, "%02d%d", (tmVal.tm_year + 0x76c) % 2000, tmVal.tm_mon / 3 + 1);
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_7, E_SERVER_GROUP_NONE);
    if (isNew)
        buyType = 1;
    else
        buyType = 2;
    db->set_query(
        " insert into prod_sale_entry_%s  ( agency_no, seal_flag, sale_time, price, slot_part_type, slot_part_code, ipg_no, bonus_ipg_no, m_id, user_id  , server_id, charac_no, job, grow_type, level, prize_flag, new_flag, result_code, result_message, gift_server_info, gift_get_charac_no  , charge_price, free_price, count_no, each_price, price_type, user_ip, buy_type, tran_id )  values( '%s', %d, now(), %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, %d, '%s', %u, %u ) ",
        quarter,
        (char*)sig, ((DB_BillingRecordSigView*)sig)->m_sealFlag,
        ((DB_BillingRecordSigView*)sig)->m_saleTime, ((DB_BillingRecordSigView*)sig)->m_price,
        ((DB_BillingRecordSigView*)sig)->m_slotPartType,
        ((DB_BillingRecordSigView*)sig)->m_slotPartCode,
        ((DB_BillingRecordSigView*)sig)->m_ipgNo, ((DB_BillingRecordSigView*)sig)->m_mId,
        ((DB_BillingRecordSigView*)sig)->m_userId, ((DB_BillingRecordSigView*)sig)->m_serverId,
        ((DB_BillingRecordSigView*)sig)->m_characNo, ((DB_BillingRecordSigView*)sig)->m_job,
        ((DB_BillingRecordSigView*)sig)->m_growType, ((DB_BillingRecordSigView*)sig)->m_level,
        ((DB_BillingRecordSigView*)sig)->m_prizeFlag, buyType,
        ((DB_BillingRecordSigView*)sig)->m_resultCode,
        ((DB_BillingRecordSigView*)sig)->m_resultMessage,
        ((DB_BillingRecordSigView*)sig)->m_giftServerInfo,
        ((DB_BillingRecordSigView*)sig)->m_giftGetCharacNo,
        ((DB_BillingRecordSigView*)sig)->m_chargePrice,
        ((DB_BillingRecordSigView*)sig)->m_freePrice, ((DB_BillingRecordSigView*)sig)->m_countNo,
        ((DB_BillingRecordSigView*)sig)->m_eachPrice,
        ((DB_BillingRecordSigView*)sig)->m_priceType, ((DB_BillingRecordSigView*)sig)->m_userIp,
        ((DB_BillingRecordSigView*)sig)->m_buyType,
        ((DB_BillingRecordSigView*)sig)->m_tranNoLow,
        ((DB_BillingRecordSigView*)sig)->m_tranNoHigh);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x6191,
                              "db insert error prod_sale_entry_%s m_id: %d", quarter,
                              ((DB_BillingRecordSigView*)sig)->m_mId);
        // [J4S4-⑤ P0] ORIG 0x08424b92 段该实参**无槽**（`cmpb $0,-0x5c; je;
        // mov $0x1,%ebx; jmp; mov $0x2,%ebx`）⇒ 第 2 次 buy_type 用右值三元，
        // 不再写 buyType 具名局部（第 1 次 ORIG 仍有 -0xb4 槽，保留）。
        db->set_query(
            " insert into prod_sale_entry_073 ( agency_no, seal_flag, sale_time, price, slot_part_type, slot_part_code, ipg_no, bonus_ipg_no, m_id, user_id, server_id, charac_no, job, prize_flag, new_flag, result_code, result_message )  values( '%s', %d, now(), %d, %d, %d, %d, %d, %d, '%s', %d, %d, %d, %d, %d, %d, '%s' ) ",
            (char*)sig, ((DB_BillingRecordSigView*)sig)->m_sealFlag,
            ((DB_BillingRecordSigView*)sig)->m_saleTime,
            ((DB_BillingRecordSigView*)sig)->m_price,
            ((DB_BillingRecordSigView*)sig)->m_slotPartType,
            ((DB_BillingRecordSigView*)sig)->m_slotPartCode,
            ((DB_BillingRecordSigView*)sig)->m_ipgNo,
            ((DB_BillingRecordSigView*)sig)->m_mId,
            ((DB_BillingRecordSigView*)sig)->m_userId,
            ((DB_BillingRecordSigView*)sig)->m_serverId,
            ((DB_BillingRecordSigView*)sig)->m_characNo,
            ((DB_BillingRecordSigView*)sig)->m_job,
            ((DB_BillingRecordSigView*)sig)->m_prizeFlag, (isNew ? 1 : 2),
            ((DB_BillingRecordSigView*)sig)->m_resultCode,
            ((DB_BillingRecordSigView*)sig)->m_resultMessage);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x61ab,
                                  "db insert error prod_sale_entry_073 m_id: %d",
                                  ((DB_BillingRecordSigView*)sig)->m_mId);
        }
    }
}

#include "W8LT_DBTask/makeRequest_33.inc"


// ============================================================================
// DB_SavePvp —— ORIG dispatch 0x8417278 / SavePvp 0x84172c8
// （pvp_result 全字段保存；GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE)）。
// ============================================================================

#include "W8LT_DBTask/dispatch_21.inc"


#include "W8LT_DBTask/SavePvp.inc"


// ============================================================================
// DB_SaveRank —— ORIG dispatch 0x84174c0 / SaveRankAll 0x8417510
// （charac_best_record 每行 inSert，冲突则 upDate）。
// ============================================================================

#include "W8LT_DBTask/dispatch_22.inc"


#include "W8LT_DBTask/SaveRankAll.inc"


// ============================================================================
// DB_Stat* 统计族 —— 5 个同构类：dispatch（基类检查 → GetOutBuffer<MSG_*> →
// LogWrite）+ LogWrite 恒返 1。ORIG 地址：
//   DB_StatBreakJar    dispatch 0x8417a7c / LogWrite 0x8417afa
//   DB_StatComposition dispatch 0x8417bec / LogWrite 0x8417c6a
//   DB_StatDungeonClear dispatch 0x8417cde / LogWrite 0x8417d5c
//   DB_StatGameRank    dispatch 0x8417dd0 / LogWrite 0x8417e4e
//   DB_StatPvpPlay     dispatch 0x8417ec2 / LogWrite 0x8417f40
// ============================================================================

#include "W8LT_DBTask/dispatch_23.inc"


#include "W8LT_DBTask/LogWrite.inc"


#include "W8LT_DBTask/dispatch_24.inc"


#include "W8LT_DBTask/LogWrite_1.inc"


#include "W8LT_DBTask/dispatch_25.inc"


#include "W8LT_DBTask/LogWrite_2.inc"


#include "W8LT_DBTask/dispatch_26.inc"


#include "W8LT_DBTask/LogWrite_3.inc"


#include "W8LT_DBTask/dispatch_27.inc"


#include "W8LT_DBTask/LogWrite_4.inc"


#include "W8LT_DBTask/dispatch_28.inc"


#include "W8LT_DBTask/LogWrite_5.inc"


// ============================================================================
// DB_UpdateChannel —— ORIG dispatch 0x8417fb4 / UpdateChannel 0x8418032
// （game_channel 在线人数/职业计数刷新）。
// ============================================================================

#include "W8LT_DBTask/dispatch_29.inc"


#include "W8LT_DBTask/UpdateChannel.inc"


// ============================================================================
// DB_UpdateGrowType —— ORIG dispatch 0x841819a / UpdateGrowType 0x84181ea
// （charac_info.grow_type 更新）。
// ============================================================================

#include "W8LT_DBTask/dispatch_30.inc"


#include "W8LT_DBTask/UpdateGrowType.inc"


// ============================================================================
// DB_UpdateQuest —— ORIG dispatch 0x8418bde / _updateQuests 0x8418c1c
// （new_charac_quest 任务状态保存：40 个 play/trigger 字段 + clear_quest）。
// ============================================================================

#include "W8LT_DBTask/dispatch_31.inc"


#include "W8LT_DBTask/_updateQuests.inc"


// ============================================================================
// DB_UpdateLogin —— ORIG dispatch 0x841826a（login_account_%d 登录登记）。
// ============================================================================

#include "W8LT_DBTask/dispatch_32.inc"


// ============================================================================
// DB_LoadMileage —— ORIG dispatch 0x8443d2c / DB_SaveMileage dispatch 0x8443c62
// （member_miles 里程表读写）。
// ============================================================================

#include "W8LT_DBTask/dispatch_33.inc"


#include "W8LT_DBTask/dispatch_34.inc"


// ============================================================================
// DB_UpdateNatType / DB_UpdateNatTypeUser —— ORIG 0x841c74e / 0x841c7f6
// （NAT 打点统计：log_nat_daily 日表 / log_nat_stat 汇总表）。
// ============================================================================
// （NAT 打点统计：log_nat_daily 日表 / log_nat_stat 汇总表）。
// ============================================================================

#include "W8LT_DBTask/dispatch_35.inc"


#include "W8LT_DBTask/dispatch_36.inc"


#include "W8LT_DBTask/dispatch_37.inc"


#include "W8LT_DBTask/dispatch_38.inc"


#include "W8LT_DBTask/dispatch_39.inc"


#include "W8LT_DBTask/dispatch_40.inc"


#include "W8LT_DBTask/dispatch_41.inc"


#include "W8LT_DBTask/InsertAvatarItem.inc"
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    // [SweepIntervalSet] 旧判词「N78：4.1 内联 memcpy / 4.4 恒 call ⇒ 封顶」被推翻。
    // ORIG 0x083ffe54..083ffec0 五段 = memset(0xc000) → rep movsl 0x33 dwords+尾 word
    // → memset(0x5f32) → rep movsl 0x37 dwords+尾 byte → memset(0x5f23)，
    // 正是 4.4 对「双数组成员结构体聚合初始化」的展开（探针 /tmp/pbig2.o::t23 逐条同形）；
    // 成员声明序 = 槽址升序（m_expireDate 深槽、m_fromUnixtime +0x6000）。
    struct SqlTemplateBuf
    {
        char m_expireDate[0x6000];
        char m_fromUnixtime[0x6000];
    };
    SqlTemplateBuf sql = {
        "inSert into user_items (charac_no,slot,it_id,expire_date,obtain_from,reg_date,ability_no,ipg_agency_no,stat,jewel_socket,hidden_option,emblem_endurance) values(%u,%d,%u,'%s',%d,now(),%d,'%s',%d,'%s',%d,%d)",
        "inSert into user_items (charac_no,slot,it_id,expire_date,obtain_from,reg_date,ability_no,ipg_agency_no,stat,jewel_socket,hidden_option,emblem_endurance) values(%u,%d,%u,from_unixtime('%s'),%d,now(),%d,'%s',%d,'%s',%d,%d)"
    };
    char* q = 0;
    char date[0x40] = "9999-12-31 23:59:59";
    if (((DB_AvatarInsertView*)sig)->m_period == 0) {
        q = sql.m_expireDate;
    } else {
        q = sql.m_fromUnixtime;
        memset(date, 0, 0x40);
        cMyTrace(__PRETTY_FUNCTION__,
                 0xae1, 0)("[InsertAvatarItem] pContext->nExpireTime : %d",
                           ((DB_AvatarInsertView*)sig)->m_period);
        // [R16a21] ORIG 0x83ff... 判定序为先 `cmp $0x1e; jg` 再 `cmp $0x6; jle`
        // ⇒ 源码短路链两项顺序照 ORIG 写（语义等价，仅指令序对齐）。
        if (((DB_AvatarInsertView*)sig)->m_period <= 0x1e &&
            ((DB_AvatarInsertView*)sig)->m_period > 6) {
            sprintf(date, "%d",
                    GlobalData::s_systemTime_.getCurSec() +
                        ((DB_AvatarInsertView*)sig)->m_period * 0x15180);
            cMyTrace(__PRETTY_FUNCTION__,
                     0xae5, 0)("[InsertAvatarItem] period value : %s", date);
        } else {
            sprintf(date, "%d", ((DB_AvatarInsertView*)sig)->m_period);
            cMyTrace(__PRETTY_FUNCTION__,
                     0xaea, 0)("[InsertAvatarItem] period value : %s", date);
        }
    }
    // [R9form4 2026-09-12 形态对齐] 删具名 `char* jewel` 局部，blob_to_str 内联进 set_query
    //   实参位（ORIG 0x083ffe0e 段：实参求值槽组 -0xc0ac/-0xc0a8/-0xc0a4/-0xc0a0/-0xc09c，
    //   我方旧形多一个 -0x20 具名槽致全帧槽平移 4）。
    db->set_query(q, ((DB_AvatarInsertView*)sig)->m_characNo,
                  ((DB_AvatarInsertView*)sig)->m_slot,
                  ((DB_AvatarInsertView*)sig)->m_itId, date,
                  ((DB_AvatarInsertView*)sig)->m_obtainFrom,
                  ((DB_AvatarInsertView*)sig)->m_abilityNo,
                  ((DB_AvatarInsertView*)sig)->m_agency,
                  ((DB_AvatarInsertView*)sig)->m_stat,
                  db->blob_to_str(0, ((DB_AvatarInsertView*)sig)->m_jewel, 0x1e),
                  ((DB_AvatarInsertView*)sig)->m_hiddenOption,
                  ((DB_AvatarInsertView*)sig)->m_emblemEndurance,
                  ((DB_AvatarInsertView*)sig)->m_field4f,
                  ((DB_AvatarInsertView*)sig)->m_field51);
    bool r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}

#include "W8LT_DBTask/_SendResult.inc"


// ---------------------------------------------------------------------------
// DB_SaveDungeonClear —— ORIG 5 符号：C1 0x8419fec / D1 0x841a008 / D0 0x841a038 /
// dispatch 0x841a056 / SaveDungeonClear 0x841a0a6 / SendResult 0x841a1a2。
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/dispatch_42.inc"


#include "W8LT_DBTask/SaveDungeonClear.inc"


#include "W8LT_DBTask/SendResult_5.inc"


// ---------------------------------------------------------------------------
// DB_SaveUserHandicap —— ORIG 符号：C1 0x844e702 / D1 0x845e8f0 / D2 0x845e920 /
// _getDBHandle 0x08427dfc / _getDBHandleForDevideAccountDB 0x08427e28 /
// dispatch 0x08427e54 / makeRequest 0x0842801a。
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/_getDBHandle.inc"


#include "W8LT_DBTask/_getDBHandleForDevideAccountDB.inc"


#include "W8LT_DBTask/dispatch_43.inc"


// ---------------------------------------------------------------------------
// DB_SaveExpandEquipslot —— ORIG 3 符号
// （docs/class_func_reports/DB_SaveExpandEquipslot/：dispatch 0x0840da96 /
//  _SaveExpandEquipslot 0x0843ff48 / _SaveCharacStat 0x0843febe）
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/_SaveExpandEquipslot.inc"


#include "W8LT_DBTask/_SaveCharacStat.inc"


// ORIG 0x0840da96：GetOutBuffer<SIG_SAVE_EXPAND_CHARAC>；先 _SaveExpandEquipslot
// 全绿再 _SaveCharacStat（charac_no=+0，STAT=+0x898）；任一步失败返 0。
// 与基类 DBDispatcher::dispatch 不同，ORIG 无 slot 前置检查（GetDBHandle 直接）。
#pragma pack(push, 1)
// DB_SaveExpandEquipslot::dispatch 出站载荷视图（§4.3 裸偏移→具名字段）：
// ORIG 0x0840da96 第二段实参 = sig+0x898；0x898 = sizeof(SIG_SAVE_EXPAND_EQUIPSLOT)
// （SIG_SAVE_EXPAND_EQUIPSLOT.h static_assert），即 CExpandEquipslot.cpp:157 权威
// SIG_SAVE_EXPAND_CHARAC 布局里 m_switch1(+0x898) 的起点。pad 仅表达布局、不参与写入（P99）。
struct DB_SaveExpandCharacView
{
    char m_equip[0x898];                 // +0x000 SIG_SAVE_EXPAND_EQUIPSLOT 段
    SIG_SAVE_EXPAND_CHARAC_STAT m_stat;  // +0x898（channel/expand 两 switch）
    char m_pad89a[2];                    // +0x89a..0x89b（SIG_SAVE_EXPAND_CHARAC=0x89c）
};
#pragma pack(pop)
static_assert(sizeof(DB_SaveExpandCharacView) == 0x89c,
              "DB_SaveExpandCharacView ABI drift");

#include "W8LT_DBTask/dispatch_44.inc"


// ---------------------------------------------------------------------------
// DB_ReqQueryCharacInfo —— dispatch 0x0842688c / _getGiftRecvCount 0x08426f46
// （_sendResult 0x08426e04 定义在 OrigDBInter19.cpp；makeRequest 见本文件前部）
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/_getGiftRecvCount.inc"


#include "W8LT_DBTask/dispatch_45.inc"


// ---------------------------------------------------------------------------
// DB_LoadKillMonsterInfo —— dispatch 0x084414e0 / LoadMonsterInfo 0x0844159a /
// SendResult 0x084419ac
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/get_compressed_blob_data.inc"
                              int recordSize);

#include "W8LT_DBTask/LoadMonsterInfo.inc"


#include "W8LT_DBTask/SendResult_6.inc"


#include "W8LT_DBTask/dispatch_46.inc"


// ============================================================================
// DB_UpdatePvPResult —— ORIG dispatch 0x0843097e（sz 0xe33；校际赛 PvP 结果落库）
// 载荷：Stream::GetOutBuffer<Packet_PvPResult>()（ORIG W 0x08452e24 实例化）。
// 载荷字节视图（ORIG 逐偏移读）：+0x0a int m_eloK（CEloRating::calc 第 4 参，
// 非对齐 4 字节读）/ +0x0e byte m_eloSys（calc 第 3 参）/ +0x0f byte m_count；
// 每队槽 stride 0x38（p∈{0,1}，行 r∈[0,m_count)）：
//   +0x10+3r daily badge / +0x1c+3r badge（green/red/blue，行 stride 3）、
//   +0x28+4r school_id(int) / +0x38+4r m_id(uint)（行 stride 4）。
// 本地类 school_info（ORIG W C1 0x0843091e，0x24 全 9 字段清零）：
//   +0x00 school_id / +0x04 fight_cnt / +0x08 last_update_time /
//   +0x0c fight_cnt_cur / +0x10 school_point / +0x14 school_point_prev /
//   +0x18 fight_cnt_prev / +0x1c rating(float) / +0x20 rating_prev(float)。
// 常量：school_match_* 三成员 ORIG D 0x093abb60/64/68（.data 初值 1/2/5，
// 定义归 OnlinePreliminaryMatch.cpp）；时段 0x48f26550<curSec<=0x48f8fccf
// 为 ORIG 字面（周末加成历史区间）。
// [SweepW8LT b28d] gap 105→-21（our 1054 / ORIG 1033）；两处真缺口已修：
// ① P227 两处 cMyTrace 改全表达式临时链——ORIG op() 实参装载 69/181 条位于
//    ctor 之前（临时链形），具名对象时 4.4 改到 ctor 之后 ⇒ -59/-54；
// ② P183 badge 循环去具名 slotv 指针缓存、全表达式重物化——ORIG 每处字段
//    访问从 buf+两下标重算 11 条地址链，缓存形每处省 5 条 ⇒ -54。
// 余 -21 属 N82（r 槽 -0x2c vs ORIG -0x20；帧 0x18c vs 0x17c）+ N7/N18a
// 寄存器分配（4.4 产 -0x15c 临时槽往返）；p167 29/29 [0缺0多]（C1/C2 同址
// 别名）；语义逐块对照完整（set_query 实参序与 +0x10/+0x1c/+0x28/+0x38 偏移全等）。
// ============================================================================
#pragma pack(push, 1)
struct DB_UpdatePvPResultHeadView      // 载荷 +0x00 起
{
    char m_pad0[0x0a];
    int m_eloK;                        // +0x0a
    unsigned char m_eloSys;            // +0x0e
    unsigned char m_count;             // +0x0f
};
struct DB_PvPResultBadgeRow            // 单玩家徽章三元组（行 stride 3）
{
    unsigned char m_green;
    unsigned char m_red;
    unsigned char m_blue;
};
struct DB_UpdatePvPResultSlotView      // 每队槽（stride 0x38）
{
    char m_pad0[0x10];
    DB_PvPResultBadgeRow m_dailyBadges[1];  // +0x10，行 stride 3
    char m_pad13[0x9];
    DB_PvPResultBadgeRow m_badges[1];       // +0x1c，行 stride 3
    char m_pad1f[0x9];
    int m_schoolIds[1];                     // +0x28，行 stride 4
    char m_pad2c[0xc];
    unsigned int m_ids[1];                  // +0x38，行 stride 4
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_47.inc"
{
    // 局部类 school_info（ORIG W C1 0x0843091e；9 字段全清零）
    struct school_info
    {
        school_info()
        {
            m_schoolId = 0;
            m_fightCnt = 0;
            m_lastUpdateTime = 0;
            m_fightCntCur = 0;
            m_schoolPoint = 0;
            m_schoolPointPrev = 0;
            m_fightCntPrev = 0;
            m_rating = 0;
            m_ratingPrev = 0;
        }
        int m_schoolId;          // +0x00
        int m_fightCnt;          // +0x04
        int m_lastUpdateTime;    // +0x08
        int m_fightCntCur;       // +0x0c
        int m_schoolPoint;       // +0x10
        int m_schoolPointPrev;   // +0x14
        int m_fightCntPrev;      // +0x18
        float m_rating;          // +0x1c
        float m_ratingPrev;      // +0x20
    };

    school_info row;        // DB 查询主行（ORIG 槽 -0x94）
    school_info rows[2];    // 两队槽（ORIG 槽 -0x108..-0xc0）
    int curSec = GlobalData::s_systemTime_.getCurSec();
    struct tm tmval;
    struct tm* ptm = localtime_r((const time_t*)&curSec, &tmval);
    int wday = ptm->tm_wday;
    int mday = ptm->tm_mday;
    ptm->tm_hour = 6;
    ptm->tm_min = 0;
    ptm->tm_sec = 0;
    int today6 = mktime(ptm);
    if (today6 > curSec)
        today6 -= 0x15180;
    int yesterday6 = today6 - 0x15180;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10,
                                                  E_SERVER_GROUP_NONE);
    unsigned char* buf =
        (unsigned char*)stream->GetOutBuffer<Packet_PvPResult>();
    int outer = 0;
    while (outer < ((DB_UpdatePvPResultHeadView*)buf)->m_count)
    {
        db->set_query(
            "seLect school_id, rating, fight_cnt, unix_timestamp(last_update_time), school_point, school_point_prev, fight_cnt_prev from school_info where school_id in (%d,%d)",
            ((DB_UpdatePvPResultSlotView*)(buf + 0 * 0x38))->m_schoolIds[outer],
            ((DB_UpdatePvPResultSlotView*)(buf + 1 * 0x38))->m_schoolIds[outer]);
        if (!db->exec(true))
            return 0;
        int needRows = db->get_n_rows();
        if (needRows != 2)
            return 0;
        int nRow = 0;
        while (nRow < needRows)
        {
            // 4.1 循环布局：fetch 在循环体顶部（`call; xor$1; test; jne 出口`），
            // 条件 nRow<needRows 在底部；`A && B()` 合写在 4.4 会改为底部 fetch。
            if (!db->fetch())
                break;
            if (!db->get_int(0, row.m_schoolId))
                return 0;
            if (!db->get_float(1, row.m_rating))
                return 0;
            if (!db->get_int(2, row.m_fightCnt))
                return 0;
            if (!db->get_int(3, row.m_lastUpdateTime))
                return 0;
            if (!db->get_int(4, row.m_schoolPoint))
                return 0;
            if (!db->get_int(5, row.m_schoolPointPrev))
                return 0;
            if (!db->get_int(6, row.m_fightCntPrev))
                return 0;
            row.m_ratingPrev = row.m_rating;
            if (((DB_UpdatePvPResultSlotView*)(buf + 0 * 0x38))->m_schoolIds[outer] ==
                row.m_schoolId)
                rows[0] = row;
            else if (((DB_UpdatePvPResultSlotView*)(buf + 1 * 0x38))->m_schoolIds[outer] ==
                     row.m_schoolId)
                rows[1] = row;
            else
                return 0;
            ++nRow;
        }
        CEloRating::calc(rows[0].m_rating, rows[1].m_rating,
                         (ENUM_ELO_RATING_SYSTEM)((DB_UpdatePvPResultHeadView*)buf)->m_eloSys,
                         ((DB_UpdatePvPResultHeadView*)buf)->m_eloK);
        for (int i = 0; i <= 1; ++i)
        {
            rows[i].m_fightCntCur =
                (int)(rows[i].m_rating - rows[i].m_ratingPrev);
            if (rows[i].m_lastUpdateTime - today6 <= 0)
            {
                if (rows[i].m_lastUpdateTime - yesterday6 > 0)
                {
                    rows[i].m_fightCntPrev = rows[i].m_fightCntCur;
                    rows[i].m_schoolPointPrev = rows[i].m_schoolPoint;
                    rows[i].m_fightCntCur = 0;
                    rows[i].m_schoolPoint = 0;
                }
                else
                {
                    rows[i].m_fightCntPrev = 0;
                    rows[i].m_schoolPointPrev = 0;
                    rows[i].m_fightCntCur = 0;
                    rows[i].m_schoolPoint = 0;
                }
            }
            ++rows[i].m_fightCntCur;
            if (rows[i].m_fightCntCur ==
                PvPGlobalEnvironmentVariable::school_match_fight_count_for_gain_point_)
                rows[i].m_schoolPoint =
                    PvPGlobalEnvironmentVariable::school_match_daily_point_;
            else if (rows[i].m_fightCntCur == 15)
            {
                if (wday == 0 || wday == 6)
                    rows[i].m_schoolPoint =
                        PvPGlobalEnvironmentVariable::school_match_daily_point_weekends_;
                if (curSec > 0x48f26550 && curSec <= 0x48f8fccf)
                    rows[i].m_schoolPoint =
                        PvPGlobalEnvironmentVariable::school_match_daily_point_weekends_;
            }
            db->set_query(
                "upDate school_info set fight_cnt=%d,school_point=%d, fight_cnt_prev=%d, school_point_prev=%d, rating=%f, last_update_time=from_unixtime(%d) where school_id=%d",
                rows[i].m_fightCntCur,
                rows[i].m_schoolPoint,
                rows[i].m_fightCntPrev,
                rows[i].m_schoolPointPrev,
                (double)rows[i].m_rating,
                curSec,
                ((DB_UpdatePvPResultSlotView*)(buf + i * 0x38))->m_schoolIds[outer]);
            if (!db->exec(true))
                return 0;
            // P227：全表达式临时链 ⇒ 4.4 在 ctor 前装载 op() 实参（ORIG 69 条
            // 装载块位于 ctor 之前；具名对象时 4.4 改到 ctor 之后，缺口 -59）。
            cMyTrace(__PRETTY_FUNCTION__, 0x7867, 5)(
                "fight_cnt=%d,school_point=%d, fight_cnt_prev=%d, school_point_prev=%d, rating=%f, last_update_time=%d, school_id=%d",
                rows[i].m_fightCntCur,
                rows[i].m_schoolPoint,
                rows[i].m_fightCntPrev,
                rows[i].m_schoolPointPrev,
                (double)rows[i].m_rating,
                curSec,
                ((DB_UpdatePvPResultSlotView*)(buf + i * 0x38))->m_schoolIds[outer]);
        }
        ++outer;
    }
    int r = 0;
    while (r < ((DB_UpdatePvPResultHeadView*)buf)->m_count)
    {
        int p;
        for (p = 0; p <= 1; ++p)
        {
            // P183 地址重物化铁律：ORIG 每处 badge/dailyBadge 字段访问都从
            // buf + 两下标（r×3 / p×0x38）重算完整地址链（13 条/处）。
            // [M8DBA r5] 关键：ORIG 先把「3r + 56p」按整型求和再 + buf
            // （0x0843155b 区 `mov %ecx,%edx; shl $0x3,%edx; lea (,%edx,8),%ecx;
            //  sub %edx,%ecx; add %ecx,%eax; lea (%ebx,%eax,1),%eax`），
            // 故此处写 `buf + (r * 3 + p * 0x38)` 再取 `m_badges[0]`；
            // 旧形 `(buf + p * 0x38)->m_badges[r]` 每处多 2 条搬移
            // （`mov %ecx,%ebx; sub %eax,%ebx; mov %ebx,%eax`，extra 计数实证）。
            if (((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].m_red != 0 ||
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].m_blue != 0)
                ++((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].m_green;
            // P227：同上（ORIG NumberToString 在 ctor 之前 ⇒ 临时链形）。
            cMyTrace(__PRETTY_FUNCTION__, 0x7881, 0)(
                "BADGE_COUNT m_id(%s) badge green(%d), red(%d), blue(%d), daily badge green(%d), red(%d), blue(%d)",
                NumberToString(((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_ids[r], 0),
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].m_green,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].m_red,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].m_blue,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_dailyBadges[0].m_green,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_dailyBadges[0].m_red,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_dailyBadges[0].m_blue);
            db->set_query(
                "upDate school_member set contribute=contribute+%d, last_play_time=NOW(), green_badge=green_badge+%d, red_badge=red_badge+%d, blue_badge=blue_badge+%d, daily_green_badge=%d, daily_red_badge=%d, daily_blue_badge=%d  where m_id=%s and school_id=%d",
                rows[p].m_fightCntCur,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].m_green,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].m_red,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].m_blue,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_dailyBadges[0].m_green,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_dailyBadges[0].m_red,
                ((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_dailyBadges[0].m_blue,
                NumberToString(((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_ids[r], 0),
                ((DB_UpdatePvPResultSlotView*)(buf + p * 0x38))->m_schoolIds[r]);
            if (!db->exec(true))
                return 0;
        }
        ++r;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// DB_LoadPowerWarStatueInfo —— ORIG 3 符号：dispatch 0x08434886 /
// GetCharacInfo 0x08434a8a / GetAvatarInfo 0x08434d36（C1/D1/D0 见类定义）。
// 回包 SIG_LOAD_POWER_WAR_STATUE_INFO（0x80，pack(1)）字段视图，偏移依
// GetCharacInfo/GetAvatarInfo 逐指令实证：
//   +0x00 characNo（dispatch 末尾写入）
//   +0x04 charac_name[0x1d]（get_str(0)）
//   +0x22 lev / +0x23 job / +0x24 grow_type（SQL 列序 job,lev,grow_type →
//      get_byte(1)=job,+0x23 / get_byte(2)=lev,+0x22 / get_byte(3)=grow,+0x24）
//   +0x25 guild_name[0x28]（guild db get_str(0)）
//   +0x50 guild_id（get_uint(4)）
//   +0x54 equipment[0xa]（slot 0..9 的 it_id；ORIG *(int*)(sig+(slot+0x14)*4+4)）
//   +0x7c 附加 dword（inventory.equipslot 解压数据偏移 2 的 dword）。
// [Lane31W8LTDBTaskView] 视图 cast 反模式清理：本地 DB_LoadPowerWarStatueInfoView
// 已整体字段化至 SigTypes_C.h SIG_LOAD_POWER_WAR_STATUE_INFO（匿名 union 双视角，
// 字段名/偏移逐项相同，含 int m_characNo 同型化与 m_field7c@+0x7c），本地定义删除。
// ---- inventory.equipslot 解压数据区视图（DB_LoadPowerWarStatueInfo::
// GetAvatarInfo 0x08434886 / DB_LoadTODUserState 0x08440cb6 区域同款）----
// ORIG 将解压缓冲叠放于栈上 Inven_Item[22] 的 &items[10]（10*0x3d=0x262，
// get_binary/uncompress_zip 写基址实证），并读解压数据 +2 处 dword
// （`mov 0x264(%esp)`）填入 sig +0x7c / m_abilities[10]。解压头 2 字节为
// zip 头，无权威字段名，ORIG 即按 blob 读——视图具化该 dword。
#pragma pack(push, 1)
struct DB_EquipslotBlobView
{
    char m_zipHead[2];      // 解压区 +0x00（zip 头）
    int m_dword2;           // 解压区 +0x02（ORIG `mov 0x264` 单 disp 读）
};
#pragma pack(pop)
static_assert(sizeof(Inven_Item) == 0x3d,
              "DB_EquipslotBlobView base assumption: Inven_Item stride");

#include "W8LT_DBTask/dispatch_48.inc"


#include "W8LT_DBTask/GetCharacInfo.inc"


#include "W8LT_DBTask/GetAvatarInfo.inc"


// ============================================================================
// DB_LastMsgDb —— ORIG dispatch 0x08421bcc（仅此一符号）。
// ============================================================================

#include "W8LT_DBTask/dispatch_49.inc"


// ============================================================================
// DB_LastMsgLogDb —— ORIG dispatch 0x08421c8e（仅此一符号）。
// ============================================================================

#include "W8LT_DBTask/dispatch_50.inc"


// ============================================================================
// DB_InsertQueueSizeLog —— ORIG dispatch 0x084225a8（仅此一符号）。
// ============================================================================

#include "W8LT_DBTask/dispatch_51.inc"


// ============================================================================
// DB_LoadPowerWarInfo —— ORIG dispatch 0x08437756（仅此一符号）。
// ============================================================================

#include "W8LT_DBTask/dispatch_52.inc"


// ============================================================================
// DB_FindFactoryHub —— ORIG dispatch 0x08436ba0（仅此一符号）。
// ============================================================================

#include "W8LT_DBTask/dispatch_53.inc"


// ============================================================================
// DB_CancelRestingUserRestrict —— ORIG dispatch 0x0843712e（仅此一符号）。
// ============================================================================

#include "W8LT_DBTask/dispatch_54.inc"

// DB_HackIPSave —— ORIG dispatch 0x08436dec。
// ============================================================================

// SIG_HACK_IP_SAVE（0x28）字段视图。W7LinkDB.cpp 填充：strncpy(+0x00,startIp,
// 0xd) 作 c_class_ip；strncpy(+0x0d,rawIp,0x10) 作 full_ip；+0x20=hack_type、
// +0x24=hack_sub_type（ORIG 0x8436df2 逐偏移读）。
#pragma pack(push, 1)
struct DB_HackIPSaveView
{
    char m_classIp[0xd];   // +0x00（c_class_ip）
    char m_fullIp[0x13];   // +0x0d（full_ip）
    int m_hackType;        // +0x20
    int m_hackSubType;     // +0x24
};                         // 0x28
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_55.inc"


// ============================================================================
// DB_DeleteGuild —— ORIG dispatch 0x0843a9c0。
// ============================================================================

#include "W8LT_DBTask/dispatch_56.inc"


// ============================================================================
// DB_LogBuyItem —— ORIG dispatch 0x0812859a。
// ============================================================================

#include "W8LT_DBTask/DB_LogBuyItem.inc"

#include "W8LT_DBTask/dispatch_57.inc"


// ============================================================================
// DB_HadesPunish —— ORIG dispatch 0x08445aa6。
// ============================================================================

#include "W8LT_DBTask/dispatch_58.inc"
{
    // 声明序照 ORIG 栈槽（低→高）：reason(-0x1b7) endTime(-0xb8) tm(-0x54)
    // year(-0x22) curSec(-0x28)...（ORIG 清零在 +0x1ec、getCurSec 在 +0x207）；
    // curSec 与 year 均声明于函数中部使用点以复现槽序。
    char reason[0xff];
    char endTime[0x64];
    struct tm tmVal;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    DB_HadesPunishView* sig =
        (DB_HadesPunishView*)stream->GetOutBuffer<SIG_HADES_PUNISH>();
    if (sig->m_days == 0)
    {
        memcpy(endTime, "'9999-12-31 23:59:59'", 0x16);
    }
    else
    {
        sprintf(endTime, "adddate(now(),interval %d day)", sig->m_days);
    }
    int applyFlag = (sig->m_applyFlag != 0) ? 2 : 1;
    int punishValue = 0;
    if (sig->m_punishType == 1)
        punishValue = 3;
    else
        punishValue = 0xc;
    db->set_query(
        "upDate member_punish_info set occ_time=now(), start_time=now(), end_time=%s, apply_flag=%d where m_id=%s and punish_type=%d",
        endTime, applyFlag, NumberToString(sig->m_mId, 0), punishValue);
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into member_punish_info (m_id, punish_type, occ_time, start_time, end_time, punish_value, apply_flag) values(%s, %d, now(), now(), %s, 0, %d)",
            NumberToString(sig->m_mId, 0), punishValue, endTime, applyFlag);
        if (!db->exec(true))
            return 0;
    }
    if (sig->m_punishType == 1)
    {
        memcpy(reason,
               "|*|HADES \xC1\xA6\xC0\xE7 / \xBA\xD2\xB9\xFD\xC7\xC1\xB7\xCE\xB1\xD7\xB7\xA5 \xB9\xD7 \xC0\xAF\xC7\xD8 \xC7\xC1\xB7\xCE\xB1\xD7\xB7\xA5\xC0\xBB \xBB\xE7\xBF\xEB\xC7\xCF\xBF\xA9 \xC0\xCC\xB5\xE6\xC0\xBB \xC3\xEB\xC7\xCF\xB0\xC5\xB3\xAA, \xBA\xD2\xB9\xFD \xC0\xE7\xC8\xAD \xBB\xFD\xBC\xBA \xB9\xD7 \xC0\xAF\xC5\xEB\xC7\xD1 \xB0\xE6\xBF\xEC(\xC6\xDB\xBD\xBA\xC6\xAE\xBC\xAD\xB9\xF6 \xC6\xF7\xC7\xD4) / 1\xC2\xF7 : \xC7\xD8\xC1\xF6\xBA\xD2\xB0\xA1 / \xB0\xE8\xC1\xA4\xC7\xD8\xC5\xB7 \xC1\xD6\xC0\xE5 \xBD\xC3 \xC8\xAE\xC0\xCE\xBF\xE4\xC3\xBB",
               0xa6);
    }
    else
    {
        memcpy(reason,
               "|*|HADES \xC1\xA6\xC0\xE7 / \xBA\xD2\xB9\xFD\xC7\xC1\xB7\xCE\xB1\xD7\xB7\xA5\xBF\xA1 \xC0\xC7\xC7\xD1 \xC3\xB3\xB9\xFA / \xB0\xE8\xC1\xA4\xBA\xB8\xC8\xA3\xC1\xA6\xB5\xB5 \xC0\xFB\xBF\xEB",
               0x3d);
    }
    char year[10] = { '\0' };
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    localtime_r((const time_t*)&curSec, &tmVal);
    strftime(year, 10, "%Y", &tmVal);
    db->set_query(
        "inSert into member_punish_info_history_%s (m_id, punish_type, occ_time, punish_value, start_time, end_time, admin_id, apply_flag, reason) values(%s, %d, now(), 6, now(), %s, 'HADES', %d, '%s') ",
        year, NumberToString(sig->m_mId, 0), punishValue, endTime, applyFlag, reason);
    if (!db->exec(true))
        return 0;
    if (punishValue == 3 || (punishValue == 0xc && applyFlag == 1))
    {
        db->set_query(
            "inSert into member_punish_info_ars_mail (m_id, punish_type, apply_flag, occ_time) values(%s, %d, %d, now())",
            NumberToString(sig->m_mId, 0), punishValue, applyFlag);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}
// ============================================================================
// DB_SaveClientHackLog —— ORIG 4 符号（自 OrigHackLog.cpp 迁移整合，2026-08-29）：
// C1 0x844e42a（inline weak，同本 TU 声明）/ dispatch 0x08421f16 /
// _getTodaysHackCount 0x08421d50 / _sendResult 0x08421e3a /
// _sendHackTypeResult 0x08422152。报告：docs/class_func_reports/
// DB_SaveClientHackLog/{dispatch,_getTodaysHackCount,_sendResult,
// _sendHackTypeResult}.md。
// ============================================================================

#pragma pack(push, 1)
// SIG_SAVE_CLIENT_HACK_LOG 载荷视图（SigTypes_D.h 中为 m_pad[0x12] 占位）。
// ORIG 写序实证：+0x00 m_id(uint)、+0x08 hack_type(ushort)、+0x0e cnt(int)；
// dispatch 读序：+0x04 charac_no、+0x0a etc。
struct SIG_SAVE_CLIENT_HACK_LOG_VIEW
{
    unsigned int m_mid;        // +0x00
    int m_characNo;            // +0x04
    unsigned short m_hackType; // +0x08
    int m_etc;                 // +0x0a
    int m_cnt;                 // +0x0e
};
#pragma pack(pop)

#include "W8LT_DBTask/_getTodaysHackCount.inc"


#include "W8LT_DBTask/_sendResult.inc"


#include "W8LT_DBTask/_sendHackTypeResult.inc"


#include "W8LT_DBTask/dispatch_59.inc"


// ============================================================================
// DB_SaveBloodDungeon —— ORIG 2 符号：dispatch 0x08437728 /
// SaveBloodDungeonReward 0x084374f8。
// 报告：docs/class_func_reports/DB_SaveBloodDungeon/{dispatch,
// SaveBloodDungeonReward}.md。
// ============================================================================

#include "W8LT_DBTask/getDailyInitializeTime.inc"

#include "W8LT_DBTask/SaveBloodDungeonReward.inc"
{
    // 声明序照 ORIG 栈槽（低→高）：tm(-0x54) today(-0x28) db(-0x24)
    // localTime(-0x20) weeklyOffset(-0x1c)。
    struct tm localTm;
    int today;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    struct tm* localTime;
    int weeklyOffset;

    today = getDailyInitializeTime(6, 0, 0);
    localTime = localtime_r((const time_t*)&today, &localTm);
    weeklyOffset = 0;   // ORIG movl $0,-0x1c（localtime_r 后、tm_wday 读取前）
    if (localTime->tm_wday == 0)
        weeklyOffset = 0x7e900;
    else
        weeklyOffset = (localTime->tm_wday - 1) * 0x15180;

    if (sig->m_flag1c != 0)
    {
        db->set_query("upDate charac_blood_dungeon_reward set reward=1 where "
                      "charac_no=%u and week_occ_date = "
                      "cast(from_unixtime(%d) as date)",
                      sig->m_characNo, (today - weeklyOffset) - 0x93a80);
        if (db->exec(true) != 1)
            return 0;
    }
    if (sig->m_field14 != 0)
    {
        db->set_query("upDate charac_blood_dungeon_reward set "
                      "week_point=%u,week_enter_count=%u,week_use_gold=%u,"
                      "last_play_date=cast(from_unixtime(%d) as date),"
                      "enter_count=%u where charac_no=%u and week_occ_date = "
                      "cast(from_unixtime(%d) as date)",
                      sig->m_point, sig->m_weeklyEnterCount, sig->m_useGold,
                      sig->m_dailyInitTime, sig->m_field14, sig->m_characNo,
                      today - weeklyOffset);
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            db->set_query("inSert into charac_blood_dungeon_reward("
                          "charac_no,week_occ_date,week_point,week_enter_count,"
                          "week_use_gold,last_play_date,enter_count) "
                          "values(%u,cast(from_unixtime(%d) as date),%u,%u,%u,"
                          "cast(from_unixtime(%d) as date),%u)",
                          sig->m_characNo, today - weeklyOffset, sig->m_point,
                          sig->m_weeklyEnterCount, sig->m_useGold,
                          sig->m_dailyInitTime, sig->m_field14);
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
}

#include "W8LT_DBTask/dispatch_60.inc"


#include "W8LT_DBTask/dispatch_61.inc"


#include "W8LT_DBTask/dispatch_62.inc"


#include "W8LT_DBTask/dispatch_63.inc"


#include "W8LT_DBTask/dispatch_64.inc"


// ============================================================================
// get_member_game_option —— ORIG T 0x083fbe25：直接返回表名常量
// "member_game_option"（DB_LoadGameOption / DB_SaveGameOption_1/2 /
// ChattingEmoticon / QuickChatting 的 SQL 表名占位）。
// 报告：docs/class_func_reports/_lt_global_gt_/get_member_game_option.md。
// ============================================================================
char* get_member_game_option()
{
    return "member_game_option";
}

// ============================================================================
// DB_LoadNoCacheData —— ORIG 2 符号：dispatch 0x08437df4 /
// LoadBloodDungeonReward 0x08437f58。
// 报告：docs/class_func_reports/DB_LoadNoCacheData/{dispatch,
// LoadBloodDungeonReward}.md。
// ============================================================================

// ---- LoadBloodDungeonReward 的 SIG_LOAD_NO_CACHE_DATA 字段视图 ----
// ORIG 0x08437f58 逐偏移写：+0x08 week_point / +0x0c week_enter_count /
// +0x10 week_use_gold / +0x14 last_play_date(int) / +0x18 enter_count /
// +0x1c rank / +0x20 reward(byte) / +0x24 reward_item_id / +0x28 reward_gold。
#pragma pack(push, 1)
struct DB_LoadNoCacheDataView
{
    unsigned int m_characNo;       // +0x00
    char m_pad4[4];                // +0x04
    unsigned int m_weekPoint;      // +0x08
    unsigned int m_weekEnterCount; // +0x0c
    unsigned int m_weekUseGold;    // +0x10
    int m_lastPlayDate;            // +0x14
    unsigned int m_enterCount;     // +0x18
    unsigned int m_rank;           // +0x1c
    unsigned char m_reward;        // +0x20
    char m_pad21[3];               // +0x21
    unsigned long m_rewardItemId;  // +0x24
    unsigned int m_rewardGold;     // +0x28
};
#pragma pack(pop)

#include "W8LT_DBTask/LoadBloodDungeonReward.inc"
{
    // 声明序照 ORIG 栈槽（低→高）：tm(-0x5c) rewardByte(-0x29) today(-0x28)
    // db(-0x24) localTime(-0x20) weeklyOffset(-0x1c) nRows(-0x18)
    // colIdx(-0x14) mktimeVal(-0x10) colIdx2(-0xc)。
    struct tm localTm;
    int today;
    char rewardByte;
    MySQL* db;
    struct tm* localTime;
    int weeklyOffset;
    int nRows;
    int colIdx;
    time_t mktimeVal;
    int colIdx2;

    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    today = getDailyInitializeTime(6, 0, 0);
    localTime = localtime_r((const time_t*)&today, &localTm);
    weeklyOffset = 0;
    if (localTime->tm_wday == 0)
        weeklyOffset = 0x7e900;
    else
        weeklyOffset = (localTime->tm_wday - 1) * 0x15180;

    db->set_query(
        "seLect week_point,week_enter_count,week_use_gold,unix_timestamp(last_play_date),enter_count,rank,reward,reward_item_id,reward_gold from charac_blood_dungeon_reward where charac_no=%d and week_occ_date = cast(from_unixtime(%d) as date)",
        ((DB_LoadNoCacheDataView*)sig)->m_characNo, today - weeklyOffset);
    if (db->exec(true) != 1)
        return 0;
    nRows = db->get_n_rows();
    if (nRows == 1)
    {
        if (db->fetch() != 1)
            return 0;
        colIdx = 0;
        if (db->get_uint(colIdx++, ((DB_LoadNoCacheDataView*)sig)->m_weekPoint) != 1)
            return 0;
        if (db->get_uint(colIdx++, ((DB_LoadNoCacheDataView*)sig)->m_weekEnterCount) != 1)
            return 0;
        if (db->get_uint(colIdx++, ((DB_LoadNoCacheDataView*)sig)->m_weekUseGold) != 1)
            return 0;
        if (db->get_int(colIdx++, ((DB_LoadNoCacheDataView*)sig)->m_lastPlayDate) != 1)
            return 0;
        if (db->get_uint(colIdx++, ((DB_LoadNoCacheDataView*)sig)->m_enterCount) != 1)
            return 0;
        localTime->tm_hour = 0;
        localTime->tm_min = 0;
        localTime->tm_sec = 0;
        mktimeVal = mktime(localTime);
        if (((DB_LoadNoCacheDataView*)sig)->m_lastPlayDate != (int)mktimeVal)
        {
            ((DB_LoadNoCacheDataView*)sig)->m_enterCount = 0;
            ((DB_LoadNoCacheDataView*)sig)->m_lastPlayDate = (int)mktimeVal;
        }
    }
    db->set_query(
        "seLect rank,reward,reward_item_id,reward_gold from charac_blood_dungeon_reward where charac_no=%d and week_occ_date = cast(from_unixtime(%d) as date)",
        ((DB_LoadNoCacheDataView*)sig)->m_characNo,
        (today - weeklyOffset) - 0x93a80);
    if (db->exec(true) != 1)
        return 0;
    nRows = db->get_n_rows();
    if (nRows == 1)
    {
        if (db->fetch() != 1)
            return 0;
        colIdx2 = 0;
        if (db->get_uint(colIdx2++, ((DB_LoadNoCacheDataView*)sig)->m_rank) != 1)
            return 0;
        rewardByte = 0;
        if (db->get_byte(colIdx2++, rewardByte) != 1)
            return 0;
        // 字段区见 DB_LoadNoCacheDataView::m_reward（+0x20）。gcc 对
        // reinterpret_cast 视图 lvalue 的求值点与 ORIG（rhs 先行、sig 基址
        // 后取）不符，保留头 m_pad 下标形态以逐指令一致（§4.3 例外注记）。
        sig->m_pad[0x20] = (char)(rewardByte == 0);
        if (db->get_ulong(colIdx2++, ((DB_LoadNoCacheDataView*)sig)->m_rewardItemId) != 1)
            return 0;
        if (db->get_uint(colIdx2++, ((DB_LoadNoCacheDataView*)sig)->m_rewardGold) != 1)
            return 0;
    }
    return 1;
}

#include "W8LT_DBTask/dispatch_65.inc"


// ---- G_CDataManager 黑名单参数视图（ORIG 0x8424ce6 单指令 disp 读
// +0x3b4/+0x3b8；P1 组件视图折叠）----
struct DB_CDataManagerBlkView
{
    char m_pad[0x3b4];   // +0x00
    int m_3b4;           // +0x3b4 消减周期基数（*0x15180 = 秒）
    int m_3b8;           // +0x3b8 单周期消减点数
};
#include "W8LT_DBTask/dispatch_66.inc"


// ============================================================================
// DB_SaveGameOption_1 —— ORIG dispatch 0x0842ff4e。
// 报告：docs/class_func_reports/DB_SaveGameOption_1/dispatch.md。
// ============================================================================
// ============================================================================
// DB_LoadGameOption —— ORIG dispatch 0x0842f7f4。
// 报告：docs/class_func_reports/DB_LoadGameOption/dispatch.md。
// 流程：DB_HANDLE_6 查 member_game_option（option_1/option_3/shortcut_emoticon，
// get_member_game_option() 表名）；exec 失败返 0；n_rows==0 → 组空包（4 个
// clear）put 返 1；有行 fetch 失败 cMyTrace(0x74dd) 返 0；组包 + 4 clear 后
// get_binary(0,0x62)→uncompress_zip 0x48（失败静默返 0）→ 清 buf → get_binary
// (1,0x2dc)→uncompress_zip 600（get_binary 失败 cMyTrace(0x7515) 返 0）→
// get_compressed_blob_data(col2,0x210,0x16) → put 返 1。buf 两次 memset 语句
// （ORIG gcc4.1 内联 rep stos；本工具链 gcc4.4 常 call memset，P4 工具链差异）。
// ============================================================================
#pragma pack(push, 1)
// DB_LoadGameOption::dispatch 回包载荷视图（§4.3 裸偏移→具名字段）：
// ORIG 0x0842f7f4 对 out+0x00/0x48/0x174/0x2a0 四子对象各 clear 一次，随后
// uncompress_zip 目标 out+0x00(cap 0x48)/out+0x48(cap 600) 与
// get_compressed_blob_data(col2, out+0x2a0, 0x210) —— 尺寸与 CGameOption.h
// CETCOption(0x48)/CQuickChattingOption(0x12c)/CChattingEmoticonConfig(0x210)
// 逐项吻合，合计 0x4b0 = sizeof(SIG_LOAD_GAME_OPTION)（SigTypes_C.h:644）。
struct DB_GameOptionSigView
{
    CETCOption m_etc;                    // +0x000（0x48）
    CQuickChattingOption m_quick[2];     // +0x048 / +0x174（0x12c×2 → +0x2a0）
    CChattingEmoticonConfig m_emoticon;  // +0x2a0（0x210）
};
#pragma pack(pop)
static_assert(sizeof(DB_GameOptionSigView) == 0x4b0,
              "DB_GameOptionSigView ABI drift");

#include "W8LT_DBTask/dispatch_67.inc"


#include "W8LT_DBTask/dispatch_68.inc"


// ============================================================================
// DB_SaveGameOption_2 —— ORIG dispatch 0x0842fd42。
// 报告：docs/class_func_reports/DB_SaveGameOption_2/dispatch.md。
// ============================================================================

// ---- SIG_LOAD_CHARAC_HOTKEY_OPTION 头部视图（ORIG 0x842fd42 movzbl 0x4）----
#pragma pack(push, 1)
struct DB_SaveGameOption2View
{
    unsigned int m_id;         // +0x00
    unsigned char m_keyType;   // +0x04
    char m_pad5[0xbf];         // +0x05..+0xc3
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_69.inc"


#include "W8LT_DBTask/dispatch_70.inc"


#include "W8LT_DBTask/dispatch_71.inc"


#include "W8LT_DBTask/dispatch_72.inc"


#include "W8LT_DBTask/dispatch_73.inc"


#include "W8LT_DBTask/dispatch_74.inc"

#include "W8LT_DBTask/dispatch_75.inc"


#include "W8LT_DBTask/dispatch_76.inc"


#include "W8LT_DBTask/dispatch_77.inc"


#include "W8LT_DBTask/dispatch_78.inc"

#include "W8LT_DBTask/dispatch_79.inc"


#include "W8LT_DBTask/dispatch_80.inc"


// ============================================================================
// DB_UpdateMemberPlayInfoPcRoom::dispatch —— ORIG 0x08439be2（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6。*stream >> m_id(uint) → NumberToString(m_id,0) →
// upDate member_play_info set pcbang_flag=1 where occ_date=now() and m_id=%s；
// 返回 exec(true)==1。
char* NumberToString(unsigned int value, int radix);   // ORIG T 0x0810904b
#include "W8LT_DBTask/dispatch_81.inc"


#include "W8LT_DBTask/dispatch_82.inc"


#include "W8LT_DBTask/dispatch_83.inc"


#include "W8LT_DBTask/AddFieldString.inc"


#include "W8LT_DBTask/GetField.inc"


#include "W8LT_DBTask/dispatch_84.inc"


#include "W8LT_DBTask/dispatch_85.inc"


// ============================================================================
// get_item_count_table_name —— ORIG 0x083fbe07
// ============================================================================
// ORIG 0x83fbe07（_Z25get_item_count_table_namev，DB_UpdateItemCountValue
// 同域辅助）：mov $"event_item_count_value",%eax + ret —— 表名字面量。
char* get_item_count_table_name()
{
    return "event_item_count_value";
}

#include "W8LT_DBTask/dispatch_86.inc"


#include "W8LT_DBTask/dispatch_87.inc"


// ============================================================================
// getList2inQuery(std::vector<int> const&, char*) —— ORIG 0x083ffa02
// ============================================================================
// ORIG 0x83ffa02（_Z15getList2inQueryRKSt6vectorIiSaIiEEPc，DB_UpdatePvPGrade
// 同域辅助）：out[0]=0 → memcpy(out,"in (",5) → for(i=0; size()-1>i; ++i)
// sprintf "%s%d," → sprintf "%s%d)" → out。
char* getList2inQuery(const std::vector<int>& list, char* out)
{
    out[0] = '\0';
    memcpy(out, "in (", 5);
    unsigned int i = 0;
    while (list.size() - 1U > i)
    {
        sprintf(out, "%s%d,", out, list[i]);
        ++i;
    }
    sprintf(out, "%s%d)", out, list[i]);
    return out;
}

#include "W8LT_DBTask/dispatch_88.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect charac_no,pvp_grade,pvp_point,unix_timestamp(last_play_time) from pvp_grade_expand where pvp_grade>=%d order by pvp_point desc limit 1500",
        GlobalData::s_ref_pvp_grade->getPvPRankRateGrade());
    // P84：短路 bool 物化（ORIG setne/xor/test 合并守卫）。
    if (!(db->exec(true) && db->get_n_rows() != 0))
        return 0;

    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm tmv;
    localtime_r(&cur, &tmv);
    tmv.tm_hour = 1;
    tmv.tm_min = 0;
    tmv.tm_sec = 0;
    cur = mktime(&tmv);
    int limitTime = (int)cur - 0x15180;

    std::vector<stPvPTempTable_t> rows;
    // ORIG 以循环构造相邻两 vector（edx/edi 指针步 0xc）：源码形态为数组。
    std::vector<int> lists[2];    // [0] 扣减列表 @ -0x90 / [1] 置回列表 @ -0x84
    int refPoint = GlobalData::s_ref_pvp_grade->GetPvpPoint(
        GlobalData::s_ref_pvp_grade->getPvPExpLossGrade());
    // SQL 行对象：ORIG 是**单个 16B stPvPTempTable_t** —— 四处读取目标地址
    // 连续递增（get_uint(0)→base+0 @0x08428cd7、get_int(1)→+4 @0x08428d05、
    // get_int(2)→+8 @0x08428d36、get_uint(3)→+0xc @0x08428d67，base=-0xb8），
    // push_back 传 &base @0x08428e79。旧版拆成 4 个独立标量，4.4 在栈上递减分配
    // （-0x94/-0x98/-0x9c/-0xa0）⇒ `*(stPvPTempTable_t*)&characNo` 越界读 16B，
    // 元素后 12B 取自相邻槽（refPoint 等），与 ORIG 数据不符。
    stPvPTempTable_t row;
    unsigned int i = 0;
    while (i < (unsigned int)db->get_n_rows())
    {
        if (db->fetch() != 1)
            return 0;
        if (!db->get_uint(0, row.m_characNo))
            return 0;
        if (!db->get_int(1, row.m_pvpGrade))
            return 0;
        if (!db->get_int(2, row.m_pvpPoint))
            return 0;
        if (!db->get_uint(3, row.m_lastPlay))
            return 0;
        // [R17a06] P360：ORIG 0x08428db2/0x08428dc1/0x08428dd5 三个失败出口同落
        // 0x08428e79（rows.push_back 后的循环尾）；条件 1 在 if 条件位 => 4.1 发
        // `call; cmp; setle %al; test %al,%al; je`（0x08428dad/0x08428db2），条件 2/3
        // 内存比较直跳（jg/jle）。写 `&&` 链时 4.4 把三条件融合成 `mov $1/mov $0`
        // 岛（我方多 11 条）⇒ 拆嵌套 if 逐个复现（P360 推广）。
        if (GlobalData::s_ref_pvp_grade->getPvPExpLossGrade() <= row.m_pvpGrade)
        {
            if ((int)row.m_lastPlay <= limitTime)
            {
                if (refPoint < row.m_pvpPoint)
                {
                    // [R17a06] ORIG 0x08428ddb 先 `mov -0xb0(%ebp),%ebx`（pvpPoint 跨
                    // call 保活）再 `call getPvPExpLossExp; add; cmp %eax,%ebx; setg`
                    // ⇒ 左操作数写 pvpPoint（P243 读取序）。
                    if (row.m_pvpPoint >
                        GlobalData::s_ref_pvp_grade->getPvPExpLossExp() + refPoint)
                        // [b28d-solo] P270：ORIG 调 push_back(int&&)（EOi），
                        // 值传选中 const& 版本 ⇒ 显式右值 cast。
                        lists[0].push_back(static_cast<int&&>((int)row.m_characNo));
                    else
                        lists[1].push_back(static_cast<int&&>((int)row.m_characNo));
                    int tmp = row.m_pvpPoint -
                              GlobalData::s_ref_pvp_grade->getPvPExpLossExp();
                    row.m_pvpPoint = std::max(refPoint, tmp);
                }
            }
        }
        // ORIG push_back 直传整个 16B 行对象（&base @0x08428e79）。
        rows.push_back(row);
        ++i;
    }

    int rowCount = (int)rows.size();   // -0x48
    // [b28d-solo] P167：ORIG 用 `operator[](unsigned) const` ⇒ 绑定 const 引用。
    const std::vector<int>& rankRate =
        GlobalData::s_ref_pvp_grade->getPvPRankRate();   // -0x44
    int rankCount = 4;                 // -0x40
    int rate[5] = {0, 0, 0, 0, 0};     // -0xa8..-0x98
    for (int r = 0; r < 5; ++r)
        rate[r] = (rankRate[r] * rowCount + 500) / 1000;
    for (int r = 1; r < 5; ++r)
    {
        if (rate[r - 1] > 0)
            rate[r] = std::max(rate[r] - rate[r - 1], 1);
    }
    if (rowCount != 0 && rate[3] == 0)
        rate[4] = 1;

    char buf[0x6000];
    memset(buf, 0, 0x6000);
    if (!lists[0].empty())
    {
        db->set_query(
            "upDate pvp_result set pvp_point=pvp_point-%d where charac_no %s",
            GlobalData::s_ref_pvp_grade->getPvPExpLossExp(),
            getList2inQuery(lists[0], buf));
        if (db->exec(true) != 1)
            return 0;
    }
    if (!lists[1].empty())
    {
        db->set_query(
            "upDate pvp_result set pvp_point=%d where charac_no %s",
            refPoint, getList2inQuery(lists[1], buf));
        if (db->exec(true) != 1)
            return 0;
    }

    std::vector<int> gradeList[5];     // -0x120 起（5×vector<int>）
    int gradeIdx = 0;                  // -0x3c
    int gradeLimit = rate[0];          // -0x38
    unsigned int k = 0;                // -0x24
    while (k < (unsigned int)rows.size())
    {
        if (gradeLimit == 0)
        {
            char done = 0;             // -0x1d
            do
            {
                ++gradeIdx;
                if (gradeIdx == 5)
                {
                    done = 1;
                    break;
                }
                gradeLimit = rate[gradeIdx];
            } while (rate[gradeIdx] == 0);
            if (done != 0)
                break;
        }
        // [b28d-solo] P270：ORIG push_back(int&&)（p167 -1 EOi/+1 ERKi）。
        gradeList[gradeIdx].push_back(
            static_cast<int&&>((int)rows[k].m_characNo));
        --gradeLimit;
        ++k;
    }

    time_t now = GlobalData::s_systemTime_.getCurSec();   // -0x34
    for (int g = 0; g < 5; ++g)
    {
        if (!gradeList[g].empty())
        {
            db->set_query(
                "upDate pvp_result set pvp_grade_ext=%d, pvp_grade_ext_update_time=from_unixtime(%d) where charac_no %s",
                5 - g, (int)now, getList2inQuery(gradeList[g], buf));
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
}

#include "W8LT_DBTask/dispatch_89.inc"

#include "W8LT_DBTask/dispatch_90.inc"


#include "W8LT_DBTask/dispatch_91.inc"

#include "W8LT_DBTask/dispatch_92.inc"


#include "W8LT_DBTask/dispatch_93.inc"


#include "W8LT_DBTask/dispatch_94.inc"


#include "W8LT_DBTask/lock.inc"
#include "W8LT_DBTask/unlock.inc"
#include "W8LT_DBTask/lock_1.inc"
#include "W8LT_DBTask/unlock_1.inc"
#include "W8LT_DBTask/lock_2.inc"
#include "W8LT_DBTask/unlock_2.inc"

// ORIG 0x841ba96：G_GameWorld()->getStatisticsPvP() → lock →
// GetDBHandle(DB_HANDLE_4,0) → 遍历 m_roomResults（PvP_Key{a,b} →
// PvP_Room{count,total}）：
//   每项先 inSert pvp_status(occ_date,room_number,play_type,no_participants,
//   occ_count) values(cast(now() as date),key.a,key.b,room.total,room.count)；
//   exec!=1 → upDate set occ_count=occ_count+room.count,
//   no_participants=no_participants+room.total where occ_date=cast(now() as
//   date) and room_number=key.a and play_type=key.b；仍失败 →
//   cMyTrace(0x4ed8,5) "upDate pvp_status failed at %d"(key.a)；
//   成功（insert 或 update）→ cMyTrace(0x4edc,5) "pvp_status : %d ROOM %d
//   TYPE %d Players %d Count "(key.a,key.b,room.total,room.count)。
// 循环结束 unlock → return 1。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 0841ba96
// [L4-r1] 2 处 cMyTrace 改 P227 全表达式临时链 ⇒ **DIFF→NEAR**（ndiff 112→12）；
//   余 12 行 = 迭代器槽阶梯/寄存器角色（N7/N86 族）。
// [M8DBB 2026-09-11 语义轴·字段错位修正（父泳道 M8BigTURest 裁决：先落 TU-local 视图，
// 共享头 GameWorld.h 的 StatisticsPvP::PvP_Room 修正待 Main 统一调度）]
// ORIG 的 PvP_Room = **16 字节**，三条直证：
//  ① pair 拷贝构造 _ZNSt4pairIKN13StatisticsPvP7PvP_KeyENS0_8PvP_RoomEEC1IRS1_RS3_EEOT_OT0_
//     @0x08610d61-0x08610d75 从 second 逐 4 个 dword 拷到 pair+0x08/+0x0c/+0x10/+0x14；
//  ② StatisticsPvP::incPvPRoomResult @0x0860c6b4 `mov 0x8(%eax),%eax`（+1 自增）与
//     0x0860c6c3 `mov 0xc(%eax),%eax`（+= param）⇒ roomCount@+0x08、resultSum@+0x0c；
//  ③ 本函数 ORIG 0x0841bbaf `mov 0x8(%eax),%eax`（occ_count）、0x0841bba9
//     `mov 0xc(%eax),%edx`（no_participants）—— 真偏移 +0x8/+0xc。
// [Main·2026-09-11] `GameWorld.h` 的 `StatisticsPvP::PvP_Room` 原为 8 字节版
// （m_roomCount@+0x00/m_resultSum@+0x04）⇒ 实参取错 +8 字节。**该头已按 ORIG 修为
// 16 字节版**（`PvP_Key m_key` +0x00 / `m_roomCount` +0x08 / `m_resultSum` +0x0c，
// 附 `static_assert(sizeof(PvP_Room)==0x10)`；直证见 GameWorld.h 注释）。
// **本视图保留不撤**：使用点的 `&(*it).second` 形是 **P167 硬要求**（ORIG 用
// `operator*`(deEv)×2），视图让该表达式保持「取址 + 逐字段读」的 ORIG 码形；
// 改成 `(*it).second.m_x` 会使 4.4 改发 `ptEv`，破坏已达成的 AE。
// ⇒ 本视图现为**码形保真镜像**（偏移与修头后的真类型逐值相同），不再承担偏移修正职责。
#pragma pack(push, 1)
struct DB_StatPVPPlay2RoomView
{
    int m_keyA;        // +0x00（ORIG 头部 8 字节 = PvP_Key 整体）
    int m_keyB;        // +0x04
    int m_roomCount;   // +0x08（ORIG 0x0860c6b4 自增 +1）
    int m_resultSum;   // +0x0c（ORIG 0x0860c6c3 += param_3）
};
#pragma pack(pop)
#include "W8LT_DBTask/dispatch_95.inc"


#include "W8LT_DBTask/dispatch_96.inc"


#include "W8LT_DBTask/dispatch_97.inc"


#include "W8LT_DBTask/dispatch_98.inc"


#include "W8LT_DBTask/dispatch_99.inc"


#include "W8LT_DBTask/dispatch_100.inc"


#include "W8LT_DBTask/dispatch_101.inc"


#include "W8LT_DBTask/dispatch_102.inc"


#include "W8LT_DBTask/dispatch_103.inc"


#include "W8LT_DBTask/dispatch_104.inc"

// ============================================================================
// DB_LoadDnfHackLog —— ORIG dispatch 0x08422242 / makeRequest 0x084224ce。
// 报告：docs/class_func_reports/DB_LoadDnfHackLog/{dispatch,makeRequest}.md。
// dispatch：基类前置检查 → DB_HANDLE_4 → 读 m_id → 组包（0x106/slot，清
// 0x3cf4 写 m_id）→ 查 dnf_hack_log 今日行（hack_type 0x65..0xfa0）→ 循环
// fetch/get_uint 填 cnt 数组 → put(DISPATCH_Q) 返 1。任一 DB 失败返 0。
// ============================================================================

// ---- SIG_LOAD_DNF_HACK_LOG 载荷视图（头为 m_pad[0x3cf4] 占位）----
// ORIG 0x8422256 读 +0x00 m_id（uint）；0x842244b 写
// +0x04+(hack_type-0x65)*4（hack_type 范围 0x65..0xfa0，0xf3c 项）。
#pragma pack(push, 1)
struct DB_DnfHackLogView
{
    unsigned int m_mId;         // +0x00
    unsigned int m_cnt[0xf3c];  // +0x04（hack_type-0x65 索引）
};
#pragma pack(pop)
static_assert(sizeof(DB_DnfHackLogView) == 0x3cf4,
              "DB_DnfHackLogView ABI drift");

#include "W8LT_DBTask/dispatch_105.inc"



// ============================================================================
// DB_WriteGeoReject —— ORIG dispatch 0x0844bb70。
// 报告：docs/class_func_reports/DB_WriteGeoReject/dispatch.md。
// DB_HANDLE_1；upDate geo_reject（rej_ip 匹配，%s=sig 首字段）exec 失败或
// affected==0 时补 inSert（rej_ip/rej_c_code=sig+0x10）→ 恒返 1。
// ============================================================================
// ---- DB_WriteGeoReject::dispatch 的 SIG_SAVE_GEO_REJECT 字段视图 ----
#pragma pack(push, 1)
struct DB_GeoRejectView
{
    char m_rejIp[0x10];    // +0x00（upDate %s 直传 sig）
    char m_rejCCode[0x10]; // +0x10（inSert 第二 %s，ORIG decay lea 0x10）
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_106.inc"


#include "W8LT_DBTask/dispatch_107.inc"


#include "W8LT_DBTask/dispatch_108.inc"


#include "W8LT_DBTask/dispatch_109.inc"


#include "W8LT_DBTask/dispatch_110.inc"


#include "W8LT_DBTask/dispatch_111.inc"


#include "W8LT_DBTask/dispatch_112.inc"


#include "W8LT_DBTask/dispatch_113.inc"


// ============================================================================
// DB_CheckCharacName::dispatch —— ORIG 0x0844317e（报告 dispatch.md 全文）。
// GetOutBuffer<SIG_CHECK_CHARAC_NAME>（+0x00=accId、+0x04=char_name 串、
// +0x22=写入方附加字节）→ GetDBHandle(DB_HANDLE_2,0) → set_query(
// "seLect m_id from charac_info where charac_name='%s' limit 1", sig+4) →
// exec 失败返 0；get_n_rows()!=0 时 fetch 失败返 0、get_int(0,mId) 失败返 0、
// sig->m_id==mId 置 flag=1 → Acquire("DBThread.cpp",0xa4ec) → guard
// << 0x2a3 << slot → GetInBuffer<SIG_GOLD_CONTROL_EXCEPTION_MID> 写
// out[0]=flag、out[1]=sig[0x22] → put(DISPATCH_Q) → 返 1。
// ============================================================================
// SIG_CHECK_CHARAC_NAME 协议视图（+0x00=accId、+0x04=char_name 串、
// +0x22=写入方附加字节；ORIG 0x0844317e 逐偏移读）。
#pragma pack(push, 1)
struct DB_CheckCharacNameView
{
    unsigned int m_mId;        // +0x00（与查询 m_id 比较）
    char m_charName[0x1e];     // +0x04（SQL %s）
    unsigned char m_byte22;    // +0x22（回包 out[1]）
    unsigned char m_pad23;     // +0x23（对齐，未用）
};
#pragma pack(pop)
// SIG_GOLD_CONTROL_EXCEPTION_MID（2 字节）回包视图。
#pragma pack(push, 1)
struct DB_ExceptionMidView
{
    unsigned char m_flag;      // +0x00
    unsigned char m_char;      // +0x01
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_114.inc"


// ============================================================================
// DB_CheckSecuReward::dispatch —— ORIG 0x084441e6（报告 dispatch.md 全文）。
// GetOutBuffer<SIG_CHECK_USER_SECU_REWARD>（+0x00=m_id、+0x04=param3）→
// GetDBHandle(DB_HANDLE_1,0) → NumberToString(m_id,0) → set_query(
// "seLect unix_timestamp(last_check_time) from member_security_grade where
// m_id=%s", num) → exec==1 && get_n_rows()!=0 && fetch()==1 时 bVar1=false
// （否则 true）；!bVar1 时 get_uint(0,lastCheck) + time(&now)：
//   今晨 = now - now%86400 - 10800；now%86400 > 75599 时今晨 += 86400；
//   (int)lastCheck < 今晨 → Acquire("DBThread.cpp",0xab25) → guard
//   << userIdx << slot → GetInBuffer<SIG_SEND_USER_REWARD> 写 sig+4 →
//   put(DISPATCH_Q)。恒返 1。
// ============================================================================
// DB_CheckSecuReward 载荷视图（SIG_* 头为 pad 占位）。
// ORIG 0x84443xx：sig +0x00=m_id（NumberToString 实参）、+0x04=透传字段；
// out +0x00=写入字段。
#pragma pack(push, 1)
struct DB_SecuRewardSigView
{
    unsigned int m_mId;     // +0x00
    unsigned int m_param4;  // +0x04
};
struct DB_SecuRewardOutView
{
    unsigned int m_reward;  // +0x00
};
#pragma pack(pop)
#include "W8LT_DBTask/dispatch_115.inc"


#include "W8LT_DBTask/dispatch_116.inc"


#include "W8LT_DBTask/dispatch_117.inc"


#include "W8LT_DBTask/dispatch_118.inc"
{
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm tmVal;
    // ORIG 六个 map 迭代器在函数顶部声明（帧 -0xb8..-0xa4，Ghidra
    // local_bc..local_a8），循环内再赋值 begin/end。P50 初始化式改法实测
    // 劣化（383→488 行）⇒ 维持先声明后赋值（C1Ev 差集 +6 记 N）。
    // [H4 2026-09-11 复核·负结论] ORIG 0x0841abc6 槽 -0xa4/-0xa8（0x0841ad4a/
    // 0x0841ad6f）、-0xac/-0xb0（0x0841aee7/0x0841aea1）、-0xb4/-0xb8 确为顶部
    // 具名迭代器，但 ORIG 区间内**无** `_Rb_tree_iterator<int,int>::C1Ev` call
    // （4.1 把空 ctor 内联为 0 条）；4.4 对顶部声明必发 6×5=30 条
    // （clean 401 vs orig 371 ⇒ DIFF）。6 个迭代器一律改「各循环前原位初始化」。
    // [SemA10 2026-09-12 Main 批准] 原「仅 it 保留顶部默认构造以保住 ORIG 实有的
    //   W 符号 _ZNSt17_Rb_tree_iteratorISt4pairIKiiEEC1Ev @0x080fa898」的做法已删：
    //   该声明使本函数多 5 条（379 vs ORIG 374），且 ORIG 区间内并无该 ctor call
    //   （4.1 内联为 0 条）⇒ 删后 our_insns 回到 ORIG 条数；代价 = 本 TU 少发射该
    //   W 符号（Main 明确允许 TU missing −1，前提 identical 不降，A/B 见 slice10 报告）。
    // （此前 5 个迭代器已改原位初始化，消 5×5=25 条 4.4 空 ctor call 序列。）
    struct tm* ptm = localtime_r(&cur, &tmVal);
    ptm->tm_sec = 0;
    time_t dayStart = mktime(ptm);
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int count = G_GameWorld()->getStatisticsToObserveCrash()
                    ->getAbnormalDownUserCount();
    db->set_query(
        "upDate client_down set occ_count = occ_count+%d where occ_date = cast(now() as date)",
        count);
    bool r = db->exec(true);
    if (!r)
    {
        // [L4-r1] P227 全表达式临时链（ORIG ctor 前取 varargs + 跨 ctor 保活）。
        cMyTrace(__PRETTY_FUNCTION__, 0x4d5c, 5)("upDate client_down failed");
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x4d5f, 0)("abnormal down user : %d", count);

    G_GameWorld()->getStatisticsToObserveCrash()->lock();
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                   E_SERVER_GROUP_NONE);
    // P171/P328⑤：ORIG 无 `movl $0,槽` 预清（裸声明只占槽不占指令）⇒ 不写初始化。
    int dungeon;
    int down;
    std::map<int, int>::iterator it =
        G_GameWorld()->getStatisticsToObserveCrash()
            ->m_userLastCrashDungeonId.begin();
    std::map<int, int>::iterator end =
        G_GameWorld()->getStatisticsToObserveCrash()
            ->m_userLastCrashDungeonId.end();
    while (it != end)
    {
        // [b28d-solo] P167：ORIG 用 operator*（deEv）×6，我方 it-> 为 ptEv。
        dungeon = (*it).first;
        down = (*it).second;
        db2->set_query(
            "inSert into crash_down_by_dungeon(occ_time,dungeon_index,down_count) values(from_unixtime(%d),%d,%d)",
            (int)dayStart, dungeon, down);
        bool c1 = db2->exec(true);
        if (!c1)
        {
            db2->set_query(
                "upDate crash_down_by_dungeon set down_count = down_count + %d where occ_time = from_unixtime(%d) and dungeon_index = %d",
                (int)dayStart, down, dungeon);
            bool c2 = db2->exec(true);
            if (!c2)
                goto skip_log_dungeon;
        }
        {
            // [FL3W8LT 2026-09-10 A/B 回滚] 三处 trace3/4/5 试 P328⑩ 单语句临时链：
            // gap 仍 −22（orig 374 / our 396）、TU 四数零变（plus 314→311 /
            // minus 292→289 仅少 3 对语句行）⇒ 非本件残差来源，回滚保原两语句形。
            cMyTrace trace3(__PRETTY_FUNCTION__, 0x4d7d, 0);
            trace3("abnormal down user by dungeon : %d DUNGEON %d", dungeon,
                   down);
        }
    skip_log_dungeon:
        ++it;
    }

    MySQL* db3 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                   E_SERVER_GROUP_NONE);
    int packet;
    int down2;
    std::map<int, int>::iterator it2 =
        G_GameWorld()->getStatisticsToObserveCrash()
            ->m_userLastCrashPacketId.begin();
    std::map<int, int>::iterator end2 =
        G_GameWorld()->getStatisticsToObserveCrash()
            ->m_userLastCrashPacketId.end();
    while (it2 != end2)
    {
        packet = (*it2).first;
        down2 = (*it2).second;
        db3->set_query(
            "inSert into crash_down_by_packet(occ_date,packet_index,down_count) values(cast(now() as date),%d,%d)",
            packet, down2);
        bool c3 = db3->exec(true);
        if (!c3)
        {
            db3->set_query(
                "upDate crash_down_by_packet set down_count = down_count + %d where occ_date = cast(now() as date) and packet_index = %d",
                packet, down2);
            bool c4 = db3->exec(true);
            if (!c4)
                goto skip_log_packet;
        }
        {
            cMyTrace trace4(__PRETTY_FUNCTION__, 0x4d98, 0);
            trace4("abnormal down user by packet : %d PACKET %d", packet,
                   down2);
        }
    skip_log_packet:
        ++it2;
    }

    MySQL* db4 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                   E_SERVER_GROUP_NONE);
    int play;
    int down3;
    std::map<int, int>::iterator it3 =
        G_GameWorld()->getStatisticsToObserveCrash()
            ->m_userLastCrashPlayStatus.begin();
    std::map<int, int>::iterator end3 =
        G_GameWorld()->getStatisticsToObserveCrash()
            ->m_userLastCrashPlayStatus.end();
    // ORIG 0x0841b0b0：`jmp <底部条件>` 纯 while 形（无顶部 == 检查；
    // 我方旧 do{ if(it3==end3)break; } 多 1 处 operator== 调用点，P167 EXTRA）。
    while (it3 != end3)
    {
        play = (*it3).first;
        down3 = (*it3).second;
        db4->set_query(
            "inSert into crash_down_by_playstatus(occ_date,play_status,down_count) values(cast(now() as date),%d,%d)",
            play, down3);
        bool c5 = db4->exec(true);
        if (!c5)
        {
            db4->set_query(
                "upDate crash_down_by_playstatus set down_count = down_count + %d where occ_date = cast(now() as date) and play_status = %d",
                play, down3);
            bool c6 = db4->exec(true);
            if (!c6)
                goto skip_log_play;
        }
        {
            cMyTrace trace5(__PRETTY_FUNCTION__, 0x4db2, 0);
            trace5("abnormal down user by playerstatus : %d PLAYERSTATUS %d",
                   play, down3);
        }
    skip_log_play:
        ++it3;
    }
    G_GameWorld()->getStatisticsToObserveCrash()->unlock();
    G_GameWorld()->getStatisticsToObserveCrash()->reset();
    return 1;
}

#include "W8LT_DBTask/dispatch_119.inc"
{
    return 1;
}

// ---- DB_RemoveEventItem::dispatch 载荷视图（SIG_REMOVE_EVENTITEM 共享头
// 仅 m_pad[0x18] 占位；ORIG 0x841ab30 循环 `mov 0x4(%eax,%edx,4),%eax`
// 步长 4 读 +0 计数、+4 起 uint 数组）----
#pragma pack(push, 1)
struct DB_RemoveEventItemView
{
    int m_count;           // +0x00（cmp/setg 带符号循环界）
    unsigned int m_ids[1]; // +0x04 起（可变长）
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_120.inc"


// ---- DB_SavePremiumInfo::dispatch 载荷视图（SIG_SAVE_PREMIUM_INFO 共享头
// 仅 m_pad[0x18] 占位；ORIG 0x841b4be 逐偏移读六字段）----
#pragma pack(push, 1)
struct DB_SavePremiumInfoView
{
    unsigned int m_mId;          // +0x00（SQL %s 经 NumberToString(m_id,0)）
    unsigned int m_preType;      // +0x04（%d）
    unsigned int m_serviceStart; // +0x08（from_unixtime(%d)）
    unsigned int m_serviceEnd;   // +0x0c
    unsigned int m_eventId;      // +0x10
    unsigned int m_serverId;     // +0x14
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_121.inc"


#include "W8LT_DBTask/dispatch_122.inc"


// ---- DB_SelectCharacMid::dispatch 回包视图（SIG_CHECK_CHARAC_MID_RESULT
// 共享头仅 m_pad[0x14] 占位；ORIG 0x8416516 写 +0x10=m_id、+0/+4/+8/+0xc
// 复制输入）----
#pragma pack(push, 1)
struct DB_SelectCharacMidResultView
{
    unsigned int m_field00;  // +0x00
    unsigned int m_field04;  // +0x04
    unsigned int m_field08;  // +0x08
    unsigned int m_field0c;  // +0x0c
    unsigned int m_mId;      // +0x10（查询结果）
};
#pragma pack(pop)
static_assert(sizeof(DB_SelectCharacMidResultView) == 0x14,
              "DB_SelectCharacMidResultView ABI drift");

#include "W8LT_DBTask/dispatch_123.inc"


#include "W8LT_DBTask/dispatch_124.inc"


#include "W8LT_DBTask/dispatch_125.inc"


// ============================================================================
// DB_ChangeLetterStat::dispatch —— ORIG 0x0842602e（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。GetOutBuffer<SIG_LETTER_STAT>（m_characNo/m_mailIdx/m_state
// short）：state==0 → deLete from letter where letter_id=%d（m_mailIdx）；
// exec 失败 makeRequest(uidx,slot,characNo,0,0) 返 0。state<0 或
// (unsigned)(state-2)>1（即非 {0,1,2,3} 全集判定后的其余）→
// makeRequest(uidx,slot,characNo,0,0) 返 0。state∈{2,3}（ORIG sub $2/
// cmp $1/ja 判定，1 也落入失败）→ upDate letter set stat=%d where
// letter_id=%d；exec 失败或 affected==0 → makeRequest(uidx,slot,characNo,
// 0,0) 返 0。成功路径统一 makeRequest(uidx,slot,characNo,mailIdx,state)
// 返 1。
// ============================================================================
// ORIG 判定：state==0 → delete 体；state<0 或 (unsigned)(state-2)>1 → fail
// 体（js/ja 直跳）；其余（state∈{2,3}，1 也落入）→ update 体。state 无
// 局部暂存（每次 movzwl+cwtl 重读）。失败/成功均以 makeRequest 通知。
// ---- 载荷视图：m_mailIdx 建模 unsigned（ORIG 0x084261cd 尾块
// `mov 0x4(%eax),%eax; mov %eax,%edx` 双跳 = 4.4 仅对 unsigned→int cast
// 实参反产，probe 实证；update 体 0x084260f7 varargs 无 cast 单跳
// `mov 0x4(%eax),%edx` 同规则互补）。----
#pragma pack(push, 1)
struct DB_LetterStatView
{
    int m_characNo;          // +0x00
    unsigned int m_mailIdx;  // +0x04
    short m_state;           // +0x08
};
#pragma pack(pop)
#include "W8LT_DBTask/dispatch_126.inc"


#include "W8LT_DBTask/makeRequest_34.inc"


#include "W8LT_DBTask/dispatch_127.inc"


#include "W8LT_DBTask/dispatch_128.inc"

#include "W8LT_DBTask/dispatch_129.inc"


#include "W8LT_DBTask/dispatch_130.inc"


// ============================================================================
// DB_InsertCirculationStatistic::dispatch —— ORIG 0x084327ba（报告 dispatch.md）
// ============================================================================
// DB_HANDLE_4（E_LOG_DB）。GetOutBuffer<SIG_CIRCULATION_STATISTIC>（pad 0xc8，
// ORIG 逐偏移读 +0x00..+0xa8 共 35 个 int）→ set_query log_gold_stat 36 占位
// （channel_no=get_channel_no() 打头，occ_time 走 from_unixtime）→ exec(true)。
// 无基类 dispatch 前置检查（ORIG 开头直接 GetDBHandle）。
// ---- SIG_CIRCULATION_STATISTIC 载荷视图（ORIG 逐偏移读取的 35 字段）----
#pragma pack(push, 1)
struct DB_CirculationStatisticView
{
    int m_occTime;                // +0x00 occ_time
    int m_level;                  // +0x04 level
    int m_dungeonDrop;            // +0x08 dungeon_drop
    int m_resultCard;             // +0x0c result_card
    int m_sellStore;              // +0x10 sell_store
    int m_questReward;            // +0x14 quest_reward
    int m_deathTowerReward;       // +0x18 death_tower_reward
    int m_illusionTowerReward;    // +0x1c illusion_tower_reward
    int m_warAreaDrop;            // +0x20 war_area_drop
    int m_memberTax;              // +0x24 member_tax
    char m_pad28[0xc];            // +0x28..0x33（未读）
    int m_bloodDungeonReward;     // +0x34 blood_dungeon_reward
    char m_pad38[0x4];            // +0x38（未读）
    int m_bloodDungeonLotto;      // +0x3c blood_dungeon_lotto
    int m_powerDungeonDrop;       // +0x40 power_dungeon_drop
    int m_powerDungeonResultCard; // +0x44 power_dungeon_result_card
    char m_pad48[0x8];            // +0x48..0x4f（未读）
    int m_buyStore;               // +0x50 buy_store
    int m_staminaRecovery;        // +0x54 stamina_recovery
    int m_repairItem;             // +0x58 repair_item
    int m_privateStoreCommission; // +0x5c private_store_commission
    int m_goldCard;               // +0x60 gold_card
    char m_pad64[0x4];            // +0x64（未读）
    int m_goldDrop;               // +0x68 gold_drop
    int m_upgrade;                // +0x6c upgrade
    int m_questUse;               // +0x70 quest_use
    int m_mailCommission;         // +0x74 mail_commission
    int m_punishUser;             // +0x78 punish_user
    int m_restrictTrade;          // +0x7c restrict_trade
    int m_guildLevelUp;           // +0x80 guild_level_up
    int m_guildSkill;             // +0x84 guild_skill
    int m_guildMail;              // +0x88 guild_mail
    int m_itemCompound;         // +0x8c item_compound
    char m_pad90[0x4];            // +0x90（未读）
    // ORIG 实参写序（0x08432815-0x08432848 预读序 = 源码右→左评估逆推）：
    // [..., item_compound, +0x9c, +0xa0, +0x94, +0x98, +0xa4, +0xa8] ⇒
    // 列映射 blood_dungeon_enter←+0x9c、buy_cerashop←+0xa0、
    // war_area_enter←+0x94、assault_gold←+0x98（原开发者列序，照抄）。
    int m_warAreaEnter;           // +0x94 → war_area_enter 列
    int m_assaultGold;            // +0x98 → assault_gold 列
    int m_bloodDungeonEnter;      // +0x9c → blood_dungeon_enter 列
    int m_buyCerashop;            // +0xa0 → buy_cerashop 列
    int m_upgradeGuildAgit;       // +0xa4 upgrade_guild_agit
    int m_upgradeGuildCargo;      // +0xa8 upgrade_guild_cargo
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_131.inc"


// ============================================================================
// DB_LoadAutoMarketConditionsControl::dispatch —— ORIG 0x084335dc（报告全文）
// ============================================================================
// DB_HANDLE_2。Stream>>bool 读 flag → memset 0x6c 清零本地载荷 →
// 单行查 auto_market_condition_ctrl（optimum_gold_supply→+0x08 / over_gold→
// +0x00，写头部）→ daily 查 auto_market_condition_ctrl_daily limit 2 循环
// 填 m_rows[2]（行 stride 0x2c，+0x10 起 4×ull + 3×int）；n==1 时行1=行0 →
// Acquire("DBThread.cpp",0x7c30) → guard<<0x15a<<-1 → GetInBuffer 拷贝 0x6c +
// +0x68=flag → put(DISPATCH_Q=1)。行数非 1/2 → cMyTrace 错误日志。
// 任一 DB 失败返 0；无基类 dispatch 前置检查。
// ---- SIG_AUTO_MARKET_CONDITIONS_CONTROL 载荷视图（+0x00/+0x08 头部两字段
// + 两行 daily 记录 + +0x68 flag；写侧 CAutoMarketConditionsControl::
// LoadDatabase 按同布局读，见 LinkMiscSecuTimerSolo.cpp）----
#pragma pack(push, 1)
struct DB_AutoMarketDailyRow
{
    unsigned long long m_totalGold;   // +0x00 total_gold
    unsigned long long m_auctionGold; // +0x08 auction_gold
    unsigned long long m_overGold;    // +0x10 over_gold
    unsigned long long m_optimumGold; // +0x18 optimum_gold_supply
    int m_goldPhase;                  // +0x20 gold_phase
    int m_itemPhase;                  // +0x24 item_phase
    int m_durabilityPhase;            // +0x28 durability_phase
};
#pragma pack(pop)

#pragma pack(push, 1)
struct DB_AutoMarketConditionView
{
    unsigned long long m_overGold;    // +0x00（单行 over_gold）
    unsigned long long m_optimumGold; // +0x08（单行 optimum_gold_supply）
    DB_AutoMarketDailyRow m_rows[2];  // +0x10/+0x3c（daily 两行）
    unsigned char m_flag;             // +0x68
    char m_pad69[0x3];                // +0x69..0x6b
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_132.inc"


#include "W8LT_DBTask/dispatch_133.inc"


#include "W8LT_DBTask/dispatch_134.inc"


#include "W8LT_DBTask/dispatch_135.inc"


// ============================================================================
// DB_UpdateCharacView::dispatch —— ORIG 0x08408848（报告
// docs/class_func_reports/DB_UpdateCharacView/dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_UPDATE_CHARAC_VIEW> → GetDBHandle(DB_HANDLE_2,
// sig->m_serverGroup+0x14d4 零扩)。本地 0x1905 字节缓冲清零（ORIG 内联
// rep stos，P4 形态）→ compress_zip(buf, len=0x1905, sig+4, 0x14d0) 失败
// 返 0 → blob_to_str(0, buf, len) → upDate charac_view set info='%s'
// where m_id=%s（m_id=NumberToString(*(uint*)sig,0)）；exec 成功且
// getAffectedRowCount()!=0 → 直接返 1；否则 inSert charac_view(m_id,info)
// values(%s,'%s')（blob_to_str 二次调用），exec 失败返 0。全过返 1。
// ---- SIG_UPDATE_CHARAC_VIEW 头视图（E.h 记录区因 CHARAC_LOGIN_DATA 双向环
// 保留 blob；此处按 ORIG 0x08408848 实证只具名两个访问形态：+0x00 m_id
// （写侧 CUser::UpdateCharacView *(sig+0)=get_acc_id()，亦 rec[0] 头）与
// +0x04 起 0x14d0 压缩区（compress_zip(sig+4,0x14d0)））----
#pragma pack(push, 1)
struct DB_CharacViewHeaderView
{
    unsigned int m_mId;       // +0x00
    char m_info[0x14d0];      // +0x04（压缩源区）
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_136.inc"


#include "W8LT_DBTask/dispatch_137.inc"


#include "W8LT_DBTask/setComboSkill.inc"


#include "W8LT_DBTask/dispatch_138.inc"


#include "W8LT_DBTask/dispatch_139.inc"


#include "W8LT_DBTask/dispatch_140.inc"


// ---- DB_LoadCleanPadPoint::dispatch 的 SIG_LOAD_CLEANPAD_POINT 载荷视图 ----
// SIG_LOAD_CLEANPAD_POINT（0x3cf0，SigTypes_C.h m_pad 占位）；ORIG 0x842ecf4
// memset 0x3cf0 后按 (hack_type-0x65)*4 写 int value（hack_type ∈ (100,0xfa1)）。
struct DB_CleanpadRatioView
{
    int m_value[0xf3c];   // +0x00（0x3cf0 / 4）
};
static_assert(sizeof(DB_CleanpadRatioView) == 0x3cf0,
              "DB_CleanpadRatioView ABI drift");

#include "W8LT_DBTask/dispatch_141.inc"


// ---- DB_LoadServerMessage::dispatch 的 SIG_SELECT_SERVER_MESSAGE 载荷视图 ----
// SIG_SELECT_SERVER_MESSAGE（0x4，SigTypes_E.h m_pad 占位）；ORIG 0x8435ecc
// 读 +0x00 server(ushort)/+0x02 channel(ushort) 作 SQL where 参数。
struct DB_ServerMessageSelectView
{
    short m_server;    // +0x00（SQL server_info=%d）
    short m_channel;   // +0x02（SQL channel_no=%d）
};
static_assert(sizeof(DB_ServerMessageSelectView) == 4,
              "DB_ServerMessageSelectView ABI drift");

#include "W8LT_DBTask/dispatch_142.inc"


// ---- DB_SaveBestClearTime::dispatch 的 SIG_BEST_CLEAR_TIME 载荷视图 ----
// SIG_BEST_CLEAR_TIME（0x5dc8，SigTypes_A.h m_pad 占位）；ORIG 0x8447cc4
// 读 +0x00 charac_no(uint)、+0x08 起 0x5dc0 字节 blob（压缩前段，
// = characNo + count + entries[1000]×0x18 的总尺寸 0x5dc8 - 8）。
struct DB_BestClearTimeView
{
    unsigned int m_characNo;   // +0x00
    char m_pad04[4];           // +0x04
    char m_data[0x5dc0];       // +0x08（put_compressed_blob_data 源）
};
static_assert(sizeof(DB_BestClearTimeView) == 0x5dc8,
              "DB_BestClearTimeView ABI drift");

#include "W8LT_DBTask/dispatch_143.inc"


#include "W8LT_DBTask/dispatch_144.inc"


#include "W8LT_DBTask/dispatch_145.inc"


#include "W8LT_DBTask/dispatch_146.inc"


#include "W8LT_DBTask/dispatch_147.inc"


#include "W8LT_DBTask/dispatch_148.inc"


#include "W8LT_DBTask/dispatch_149.inc"


#include "W8LT_DBTask/insertCreatureItem.inc"


#include "W8LT_DBTask/dispatch_150.inc"


// ---- DB_InsertResponseTime 的 SIG_INSERT_RESPONSE_TIME（0x7d8）载荷行视图 ----
// 头部 +0x00 baseSec(time_t)、+0x04 count；行数组 @+8 步长 0x14：
// {+0x00 id; +0x04 count; +0x08 bytesLow; +0x0c bytesHigh; +0x10 avgRounded}
// （与 MessageCounter.cpp 写入视图同布局；dispatch 只读不写）。
#pragma pack(push, 1)
struct DB_ResponseTimeEntryView
{
    unsigned int m_id;          // +0x00 packet_id
    unsigned int m_count;       // +0x04 packet_count
    unsigned int m_bytesLow;    // +0x08 total_response_time 低 32 位
    unsigned int m_bytesHigh;   // +0x0c total_response_time 高 32 位
    unsigned int m_avg;         // +0x10 avg_response_time
};
struct DB_ResponseTimeSigView
{
    int m_baseSec;              // +0x00 occ_time
    int m_count;                // +0x04 行数
    DB_ResponseTimeEntryView m_entries[100];   // +0x08..+0x7d7
};
#pragma pack(pop)
static_assert(sizeof(DB_ResponseTimeSigView) == 0x7d8,
              "DB_ResponseTimeSigView ABI drift vs SIG_INSERT_RESPONSE_TIME");

#include "W8LT_DBTask/dispatch_151.inc"


#include "W8LT_DBTask/dispatch_152.inc"

#include "W8LT_DBTask/dispatch_153.inc"


// ============================================================================
// DB_UpdateFairPvPScore::dispatch —— ORIG 0x0843fa78（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x843fa78：GetOutBuffer<SIG_FAIR_PVP_SCORE> 空 → 返 0；
// GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,0) → put_compressed_blob_data(db,0,sig+0x2c,
// 0x650,&outStr) 失败返 0；set_query(upDate fair_pvp_score set
// private_win..max_successive_win,job_score='%s',daily_play_count =%u,
// last_play_time=from_unixtime(%d),give_item=%d where charac_no=%u，
// 11 个 uint 字段 + blob + 0x67c/0x680 + (uint)0x684 字节 + 0x00) →
// exec 失败返 0，成功返 1。
// ============================================================================
// 普通对齐：0x67c/0x680/0x684 均 4 对齐，尾部 3 字节自然 pad → sizeof 恰 0x688。
struct DB_FairPvPScoreView
{
    unsigned int m_characNo;         // +0x00（SQL charac_no）
    unsigned int m_privateWin;       // +0x04
    unsigned int m_privateLose;      // +0x08
    unsigned int m_privateDraw;      // +0x0c
    unsigned int m_relayBattleWin;   // +0x10
    unsigned int m_relayBattleLose;  // +0x14
    unsigned int m_relayBattleDraw;  // +0x18
    unsigned int m_relayBattle2Kill; // +0x1c
    unsigned int m_relayBattle3Kill; // +0x20
    unsigned int m_successiveWin;    // +0x24
    unsigned int m_maxSuccessiveWin; // +0x28
    char m_jobScore[0x650];          // +0x2c（compress 源，0x650 字节）
    unsigned int m_dailyPlayCount;   // +0x67c
    unsigned int m_lastPlayTime;     // +0x680
    unsigned char m_giveItem;        // +0x684
};
static_assert(sizeof(DB_FairPvPScoreView) == 0x688,
              "DB_FairPvPScoreView ABI drift");
#include "W8LT_DBTask/dispatch_154.inc"


// ============================================================================
// DB_SecurityCardDisuse::dispatch —— ORIG 0x0842b6e6（报告 dispatch.md 全文）
// ============================================================================
// SIG_SECURITY_CARD_DISUSE 载荷视图（+0x00 m_id；SigTypes_E.h m_pad 段具名化）。
struct SecuDisuseView
{
    unsigned int m_id;   // +0x00
};
static_assert(sizeof(SecuDisuseView) == 0x4, "SecuDisuseView ABI drift");
// saveSecurityCardHistory 形参域（与 DBSecuLink.cpp 同名单值枚举；本 TU 需
// DISUSE=2 值，mangled 名仅含类型名故链接一致，值 2 为 ORIG 0x842b6e6 实证）。
enum ENUM_SECURITYCARD_MOD_TYPE
{
    ENUM_SECURITYCARD_MOD_TYPE_NONE = 0,
    ENUM_SECURITYCARD_MOD_TYPE_ACTIVATE = 1,
    ENUM_SECURITYCARD_MOD_TYPE_DISUSE = 2,
};
#include "W8LT_DBTask/saveSecurityCardHistory.inc"
                             ENUM_SECURITYCARD_MOD_TYPE mod);  // ORIG T 0x0842ae3f（DBSecuLink.cpp）
#include "W8LT_DBTask/turnOffSecurityFlag.inc"
#include "W8LT_DBTask/dispatch_155.inc"


#include "W8LT_DBTask/dispatch_156.inc"


#include "W8LT_DBTask/dispatch_157.inc"


// ============================================================================
// ---- DB_UpdateBloodBestRecord / DB_UpdateBoosterGageData 载荷视图 ----
// （SigTypes_E.h 为 m_pad 占位；ORIG dispatch 逐偏移读取，字段名与
// CUserMissingA2.cpp/CBoosterGage.cpp 写入端一致。）
#pragma pack(push, 1)
struct DB_BloodBestRecordView
{
    unsigned int m_characNo;    // +0x00（SQL charac_no）
    unsigned int m_bestTime;    // +0x04（SQL best_time）
    int m_bestRound;            // +0x08（SQL best_round）
    int m_dungeonIndex;         // +0x0c（SQL dungeon_index）
};
#pragma pack(pop)
static_assert(sizeof(DB_BloodBestRecordView) == 0x10,
              "DB_BloodBestRecordView ABI drift");

#pragma pack(push, 1)
struct DB_BoosterGageView
{
    unsigned int m_dwAccountNo;   // +0x00（SQL m_id）
    int m_dwBoosterGage;          // +0x04（SQL gage）
};
#pragma pack(pop)
static_assert(sizeof(DB_BoosterGageView) == 8,
              "DB_BoosterGageView ABI drift");

#include "W8LT_DBTask/dispatch_158.inc"


#include "W8LT_DBTask/dispatch_159.inc"


#include "W8LT_DBTask/dispatch_160.inc"


#include "W8LT_DBTask/dispatch_161.inc"


#include "W8LT_DBTask/dispatch_162.inc"


#include "W8LT_DBTask/makeRequest_35.inc"


#include "W8LT_DBTask/dispatch_163.inc"


#include "W8LT_DBTask/dispatch_164.inc"


#include "W8LT_DBTask/makeRequest_36.inc"


#include "W8LT_DBTask/dispatch_165.inc"


#include "W8LT_DBTask/makeRequest_37.inc"


#include "W8LT_DBTask/dispatch_166.inc"
                                    Stream* stream)
{
    unsigned int characNo = 0;   // -0x3c
    unsigned char attack = 0;    // -0x3d
    unsigned char revenge = 0;   // -0x3e
    *stream >> characNo;
    *stream >> attack;
    *stream >> revenge;
    if (characNo == 0)
        return 0;
    time_t t = GlobalData::s_systemTime_.getCurSec();      // -0x44
    struct tm* p = localtime(&t);                          // -0x18
    // 技巧21：dead 前置声明占 -0x14（ORIG mktime 结果=-0x14、hourLimit
    // 后声明占更近 -0x10；0x0843f34c `mov %eax,-0x10(%ebp)`+`cmp
    // -0x10(%ebp),%eax` 实证）。
    time_t dead;
    int hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // -0x10
    if (p->tm_hour < hourLimit)
    {
        p = 0;
        t -= 0x15180;
        p = localtime(&t);
    }
    dead = mktime(p);                                      // -0x14（ORIG 弃用）
    // ORIG +0x0c0 显式 call memset(date,0,12)（P4 反例：`= {'\\0'}` 会内联
    // 展开 movl×3，与 ORIG 库调用形态不符）。
    char date[12];                                         // -0x50
    memset(date, 0, 12);
    sprintf(date, "%04d-%02d-%02d", p->tm_year + 0x76c, p->tm_mon + 1,
            p->tm_mday);
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    db->set_query(
        "upDate village_attack_dungeon set attack_count = %u, revenge_dungeon = %u where occ_date = '%s' and charac_no = %u",
        (unsigned int)attack, (unsigned int)revenge, date, characNo);
    if (db->exec(true) != 1)
    {
        // 技巧3：临时链式（具名 trace 块对象致 4.4 帧底外推，先例
        // DB_LoadBloodBestRecord 闭合 / DB_LoadPowerWarStatueInfo::dispatch）。
        cMyTrace(__PRETTY_FUNCTION__, 0x98fc, 5)(
            "village_attack_dungeon upDate exec fail");
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into village_attack_dungeon(occ_date, charac_no, attack_count, revenge_dungeon) values('%s', %u, %u, %u)",
            date, characNo, (unsigned int)attack, (unsigned int)revenge);
        if (db->exec(true) != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x9907, 5)(
                "event_integration_charac inSert exec fail");
            return 0;
        }
    }
    return 1;
}

#include "W8LT_DBTask/GetAPCUserInfo.inc"


#include "W8LT_DBTask/dispatch_167.inc"


#include "W8LT_DBTask/dispatch_168.inc"


#include "W8LT_DBTask/dispatch_169.inc"


#include "W8LT_DBTask/dispatch_170.inc"


#include "W8LT_DBTask/dispatch_171.inc"


#include "W8LT_DBTask/dispatch_172.inc"


#include "W8LT_DBTask/ExtendPeriod.inc"


#include "W8LT_DBTask/dispatch_173.inc"


#include "W8LT_DBTask/dispatch_174.inc"


#include "W8LT_DBTask/dispatch_175.inc"


#include "W8LT_DBTask/dispatch_176.inc"


#include "W8LT_DBTask/dispatch_177.inc"


// ============================================================================
// DBOnlyOrigWave6：10 个 DB_* dispatch/makeRequest 真实实现
// （报告 docs/class_func_reports/<类>/ 全文已读，逐指令对应；Acquire 行号常量
//   即 ORIG DBThread.cpp 内嵌行号）
// ============================================================================

#include "W8LT_DBTask/dispatch_178.inc"


#include "W8LT_DBTask/dispatch_179.inc"


#include "W8LT_DBTask/dispatch_180.inc"


#include "W8LT_DBTask/dispatch_181.inc"


#include "W8LT_DBTask/dispatch_182.inc"


#include "W8LT_DBTask/dispatch_183.inc"


#include "W8LT_DBTask/dispatch_184.inc"


#include "W8LT_DBTask/dispatch_185.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    struct tm* ptm = localtime((const time_t*)&GlobalData::g_IPMonitorLastDBAccessTime);
    // [L5 K5W8LTDBTask 负结论] P312 变体实测证伪：ORIG 0x0843310f `movzwl "",%eax`
    // + 定宽清零（.rodata 0x8c52777 十六字节全 0 ⇒ 原码疑为空串字面量初始化），
    // 但我方改 `= ""` 后 4.4 仍发同样 5 条 movl（our 270 未变，TU 四数 141/1750/12/57
    // 逐值相同）⇒ 4.4 不作该展开，旋钮无效，回滚保 `{ '\0' }`。
    char timeBuf[0x14] = { '\0' };
    sprintf(timeBuf, "%d-%d-%d %d:%d:%d", ptm->tm_year + 0x76c, ptm->tm_mon + 1,
            ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    db->set_query(
        "seLect ip, type, m_id_cnt, unix_timestamp(start_time), unix_timestamp(end_time) from ip_monitor_punish where start_time > '%s'",
        timeBuf);
    if (!db->exec(true) || db->get_n_rows() == 0)
        return 0;
    // P227 全表达式临时链：ORIG 0x084331xx 在 ctor 前 `call get_n_rows;
    // mov %eax,%ebx` 并把 %ebx 跨 ctor 保活到 op() 实参（具名对象时 4.4 在
    // ctor 之后重调 get_n_rows，多一次 DB 调用）。
    cMyTrace(__PRETTY_FUNCTION__, 0x7ac4, 0)(
        "BLACKIP GET DB ROWS COUNT : %d", db->get_n_rows());
    char buf[0xc80];
    // [A04-r5i · 轴 B/P196] ORIG 0x084333cc `cmpl $0x62,-0x24(%ebp); **ja**`、
    // 0x084333dc `cmp -0x28(%ebp),%eax; **seta**`、0x08433503 循环尾 `**seta** %al;
    // test; jne 0x08433255` ⇒ 三处**全为无符号序** ⇒ 按 ORIG 把 rowCount/i 声明为
    // unsigned（原 int ⇒ 4.4 产 `jg/jle/setg`，与 ORIG 相反）；P196 前置通过（向 ORIG 对齐）。
    unsigned int rowCount = 0;
    for (unsigned int i = 0; i < db->get_n_rows(); ++i)
    {
        if (!db->fetch())
            return 0;
        if (rowCount == 0)
            memset(buf, 0, 0xc80);
        // P183：ORIG 每处 `lea -0xcdc(%ebp),%eax; shl $0x5,%edx; add %edx,%eax`
        // 重算 buf+rowCount*0x20；具名 items 指针缓存会省掉该链。
        if (!db->get_str(0, ((DB_IpMonitorPunishItem*)buf)[rowCount].m_ip, 0x10))
            return 0;
        if (!db->get_int(1, ((DB_IpMonitorPunishItem*)buf)[rowCount].m_type))
            return 0;
        if (!db->get_uint(2, ((DB_IpMonitorPunishItem*)buf)[rowCount].m_idCnt))
            return 0;
        if (!db->get_uint(3, ((DB_IpMonitorPunishItem*)buf)[rowCount].m_startTime))
            return 0;
        if (!db->get_uint(4, ((DB_IpMonitorPunishItem*)buf)[rowCount].m_endTime))
            return 0;
        // [R18a16] ORIG 0x084333cc 段：`cmpl $0x62,-0x24; ja <Acquire>` 与第二条件
        // 各自独立跳同一 Acquire 靶；原 `&&` 写法使 4.4 在条件位多出 bool 物化岛
        // （我方 .o `mov $1,%eax;jmp;mov $0,%eax;test %al,%al;je`，ORIG 无）。
        // P225/P133 拆形（负条件 → 落入共享 Acquire 块）。
        if (rowCount <= 0x62)
        {
            if (i < db->get_n_rows() - 1)
            {
                ++rowCount;
                continue;
            }
        }
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7ae9), true);
            guard.operator*()->operator<<((int)0x156);
            guard.operator*()->operator<<((int)-1);
            // [Lane31W8LTDBTaskView] 同上（ORIG 0x084330d4 段 RHS-first）。
            SIG_IP_MONITOR_PUNISH* view =
                guard->GetInBuffer<SIG_IP_MONITOR_PUNISH>();
            view->m_count = rowCount + 1;
            memcpy(view->m_items, buf, 0xc80);
            view->m_flag = 0;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            rowCount = 0;
        }
    }
    return 1;
}

#include "W8LT_DBTask/makeRequest_38.inc"


#include "W8LT_DBTask/makeRequest_39.inc"


#include "W8LT_DBTask/makeRequest_40.inc"

// ---------------------------------------------------------------------------
// 本批 12 个 ORIG-only DB_* dispatch/makeRequest 真实定义（报告
// docs/class_func_reports/DB_*/{dispatch,makeRequest}.md 全文已读，逐指令对应；
// Acquire 行号常量即 ORIG DBThread.cpp 内嵌行号）
// ---------------------------------------------------------------------------

#include "W8LT_DBTask/dispatch_186.inc"


#include "W8LT_DBTask/dispatch_187.inc"


#include "W8LT_DBTask/dispatch_188.inc"


#include "W8LT_DBTask/dispatch_189.inc"


#include "W8LT_DBTask/dispatch_190.inc"


#include "W8LT_DBTask/dispatch_191.inc"


// ORIG 0x08440f3e（_ZN24DB_UpdateItemMakingSkill8dispatchEiiP6Stream）：
// GetDBHandle(2,0)；GetOutBuffer<SIG_ITEM_MAKING_SKILL>（+0 characNo(uint)，
// +4..+0x18 11×short weapon..magic_stone）；upDate ... where charac_no=%u；
// exec 失败 → trace(0x9fde,5) 返 0；affected==0 → inSert(...)；exec 失败 →
// trace(0x9ff5,5) 返 0；返 1。
// ---- SIG_ITEM_MAKING_SKILL 视图（ORIG 0x08440f3e 读点 + StubS3A_deps.cpp
// _saveData 0x08545276 写点实证：+0 uint characNo、+4 short×11 技能等级
// weapon/cloth/leather/light_armor/heavy_armor/plate/amulet/wrist/ring/
// support/magic_stone；头 m_data[0x16] 为 ctor memset 单元，不动）----
#pragma pack(push, 1)
struct DB_ItemMakingSkillView
{
    unsigned int m_characNo;      // +0x00
    short m_skillLevels[11];      // +0x04..+0x18
};
#pragma pack(pop)
static_assert(sizeof(DB_ItemMakingSkillView) == 0x1a,
              "DB_ItemMakingSkillView ABI drift");
#include "W8LT_DBTask/dispatch_192.inc"


#include "W8LT_DBTask/dispatch_193.inc"


// ORIG 0x08436ad4（_ZN28DB_InsertServerLoadStatistic8dispatchEiiP6Stream）：
// GetDBHandle(4,0)；GetOutBuffer<SIG_INSERT_SERVER_LOAD_STATISTIC>；字段：
// +0 server_id(byte)、+1 channel_no(byte)、+2/+4/+6/+8 4×short
// (dbq/logdbq/netq/packetq)；inSert log_gameserver_load_stat set ...；
// 返 exec(true)==1。
// ---- SIG_INSERT_SERVER_LOAD_STATISTIC 视图（写侧 CAppLoadChecker.cpp
// SigInsertServerLoadStatisticView 同布局：+0 serverGroup、+1 channelNo、
// +2/+4/+6/+8 short 计数；ORIG dispatch 0x08436ad4 前两参 movsbl 符号
// 扩展，故用 signed char）----
#pragma pack(push, 1)
struct DB_ServerLoadStatView
{
    signed char m_serverGroup;    // +0x00（SQL server_id=%d，(int)*(char*)）
    signed char m_channelNo;      // +0x01（SQL channel_no=%d）
    short m_dbqCnt;               // +0x02
    short m_logdbqCnt;            // +0x04
    short m_netqCnt;              // +0x06
    short m_packetqCnt;           // +0x08（ORIG +0x72 起 edi←movzwl 0x8）
};
#pragma pack(pop)
static_assert(sizeof(DB_ServerLoadStatView) == 0xa,
              "DB_ServerLoadStatView ABI drift");
#include "W8LT_DBTask/dispatch_194.inc"


#include "W8LT_DBTask/dispatch_195.inc"


#include "W8LT_DBTask/makeRequest_41.inc"


#include "W8LT_DBTask/makeRequest_42.inc"


#include "W8LT_DBTask/dispatch_196.inc"


// ============================================================================
// DB_PassPadUpdateFailCnt::dispatch —— ORIG 0x08423bc4（报告 dispatch.md 全文）
// ============================================================================
// 载荷 SIG_SECURITY_CARD_UPDATE_FAIL_CNT 视图：+0 m_id(uint)，+4 fail_cnt(uint)，
// +8 ip_info（%s 字符串）。GetDBHandle(6,0)：upDate member_mousepass set
// fail_cnt=%d where m_id=%s and enable_flag='1'（exec 返回丢弃）；fail_cnt!=0 时
// seLect mousepass ... 若 exec 失败/无行/fetch 失败 → 返 0；get_str(0, buf, 0x24)
// 失败返 0；strftime "%Y%m" → inSert member_mousepass_history_%s ...（exec 丢弃）。
// 再 GetDBHandle(1,0)：upDate member_security_grade set goblin_fail_cnt=%d
// where m_id=%s；exec 失败或 affectedRowCount==0 → inSert member_security_grade
// (m_id, goblin_fail_cnt)，insert 失败返 0；全绿返 1。
#pragma pack(push, 1)
struct DB_PassPadUpdateFailCntView
{
    unsigned int m_mId;       // +0x00
    unsigned int m_failCnt;   // +0x04
    char m_ipInfo[0x10];      // +0x08（%s 源，长度覆盖到 +0x18 载荷尾）
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_197.inc"


#include "W8LT_DBTask/dispatch_198.inc"


// ============================================================================
// DB_SaveCharacInfo4Guild::dispatch —— ORIG 0x0841b624（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_SAVE_CHARACINFO4GUILD>（视图 +0 guild_id,+8 charac_no,
// +0xc grow_type,+0x10 lev）→ GetDBHandle(8,0) 与 GetDBHandle(1,0)（后者丢弃）；
// upDate guild_member set grow_type=%d,lev=%d where guild_id=%d and charac_no=%d
// （grow_type, lev, guild_id, charac_no）→ return exec==1。
#pragma pack(push, 1)
struct DB_SaveCharacInfo4GuildView
{
    int m_guildId;    // +0x00
    int m_pad0;       // +0x04
    int m_characNo;   // +0x08
    int m_growType;   // +0x0c
    int m_lev;        // +0x10
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_199.inc"


// ============================================================================
// DB_UpdatePvPLastPlayTime::dispatch —— ORIG 0x084317b2（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(0x10,0) → GetOutBuffer<Packet_PvPResult>；外层 i 从 0 到
// count-1（count=载荷 +0xf 字节），内层 j 0..1：school_id=*(int*)(p+j*0x38+
// i*4+0x28)，m_id=*(uint*)(p+j*0x38+i*4+0x38)；upDate school_member set
// last_play_time=NOW() where m_id=%s and school_id=%d（NumberToString(m_id,0),
// school_id）；exec 失败返 0；循环完返 1。
#pragma pack(push, 1)
struct DB_PvPResultSchoolView
{
    char m_pad0[0xf];        // +0x00..+0x0e
    unsigned char m_count;   // +0x0f（外层循环上界）
    char m_pad1[0x18];       // +0x10..+0x27
    int m_schoolId[2][0xe];  // +0x28（行 j，列 i，stride 0x38=14*4）
};
// [M8DBB 2026-09-11 语义轴·布局修正（父泳道 M8BigTURest 批准）]
// ORIG 0x084317b2 直证：schoolId 行内 +0x00（0x08431817 `add $0x8,%eax` ⇒
// 4*(14j+i+8)+8 = sig+0x28+56j+4i），mId 行内 **+0x10**（0x08431835
// `add $0xc,%eax` ⇒ 4*(14j+i+12)+8 = sig+0x38+56j+4i）。原写法把 mId 声明为
// schoolId 之后的独立 `[2][0xe]` 数组 ⇒ 实际基址 0x98（4.4 译出 `add $0x24`）
// ⇒ **读错列**（语义 bug）。拆成两个同起点的 14-int 行视图，行 stride 0x38。
struct DB_PvPResultMIdView
{
    char m_pad[0x38];            // +0x00..+0x37
    unsigned int m_mId[2][0xe];  // +0x38（行 j，列 i，stride 0x38）
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_200.inc"


#include "W8LT_DBTask/dispatch_201.inc"


// ============================================================================
// DB_RemoveFeaturedGoodsIdx::dispatch —— ORIG 0x0841b41a（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_REMOVE_FEATURED_GOODSIDX>（视图 +0 m_id(uint),+8 event_id,
// +0xc server_id）→ GetDBHandle(6,0)；deLete from dnf_event_entry where
// m_id=%s and event_id=%d and server_id=%d（NumberToString(m_id,0), event_id,
// server_id）→ return exec==1。
#pragma pack(push, 1)
struct DB_RemoveFeaturedGoodsIdxView
{
    unsigned int m_mId;    // +0x00
    int m_pad0;            // +0x04
    int m_eventId;         // +0x08
    int m_serverId;        // +0x0c
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_202.inc"


#include "W8LT_DBTask/dispatch_203.inc"


// ============================================================================
// DB_Find_Factory_Hub_User::dispatch —— ORIG 0x08437a32（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_FIND_FACTORY_HUB_USER>（视图 +0 charac_name 字符串, +0x1e
// flag(ushort)）→ GetDBHandle(2,0)；seLect charac_no,m_id from charac_info
// where charac_name='%s'（sig 即名字串）；exec 失败返 0；循环 i < get_n_rows()：
// fetch/get_int(0,&characNo)/get_uint(1,&mId) 任一失败返 0；
// Acquire("DBThread.cpp",0x850f) → CStreamGuard(stream,true) → <<0x1a1(int) →
// <<slot(int) → GetInBuffer<SIG_FIND_FACTORY_HUB_USER_NO_CONN>（视图 +0 m_id,
// +4 charac_no, +8 flag(ushort)）写 mId/characNo/sig+0x1e → put(DISPATCH_Q(1))；
// 返 1。
// [Lane31W8LTDBTaskView] 视图 cast 反模式→权威头具名直访问：两处局部视图
// （DB_FindFactoryHubUserView / DB_FindFactoryHubUserNoConnView）与 SigTypes_B.h
// 的 SIG_FIND_FACTORY_HUB_USER / SIG_FIND_FACTORY_HUB_USER_NO_CONN 完全同布局，
// 删本地重复定义改用权威类型（字段名一致：m_characName/m_flag/m_mId/m_characNo）。

#include "W8LT_DBTask/dispatch_204.inc"


#include "W8LT_DBTask/makeRequest_43.inc"


#include "W8LT_DBTask/makeRequest_44.inc"


// ============================================================================
// DB_SaveFeaturedGoodsIdx::dispatch —— ORIG 0x0841b210（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_SAVE_FEATURED_GOODSIDX>（视图 +0 m_id(uint),+4 charac_no,
// +8 event_id,+0xc server_id,+0x10 obtain_date）→ GetDBHandle(6,0)；
// inSert into dnf_event_entry(event_id,m_id,occ_date,server_id,charac_no,
// obtain_date) values(%d,%s,from_unixtime(%d),%d,%d,from_unixtime(%d))（event_id,
// NumberToString(m_id,0), obtain_date, server_id, charac_no, obtain_date）→
// return exec==1。
#pragma pack(push, 1)
struct DB_SaveFeaturedGoodsIdxView
{
    unsigned int m_mId;        // +0x00
    int m_characNo;            // +0x04
    int m_eventId;             // +0x08
    int m_serverId;            // +0x0c
    int m_obtainDate;          // +0x10
};
#pragma pack(pop)

#include "W8LT_DBTask/dispatch_205.inc"

#include "W8LT_DBTask/dispatch_206.inc"


// ---- SIG_SAVE_CRAZY_LEVELUP_EVENT（SigTypes_D.h m_pad[0xd] 占位）视图 ----
// ORIG 0x842527a 逐偏移读 +0x00(event_id 字符串 5 字节) / +0x05(uint m_id 非
// 对齐) / +0x09(int charac_no)，共 0xd 字节。非对齐 → pack(1)。
#pragma pack(push, 1)
struct DB_SaveCrazyLevelupEventView
{
    char m_eventId[5];      // +0x00（SQL '%s' 直接传 sig+0x0）
    unsigned int m_memberId;  // +0x05（NumberToString(...,0) → m_id）
    int m_characNo;         // +0x09
};
#pragma pack(pop)
static_assert(sizeof(DB_SaveCrazyLevelupEventView) == 0xd,
              "DB_SaveCrazyLevelupEventView ABI drift");

#include "W8LT_DBTask/dispatch_207.inc"


#include "W8LT_DBTask/dispatch_208.inc"


// ---- SIG_INSERT_AUTO_PUNISH_FIRST_USER（SigTypes_B.h m_pad[0x2c] 占位）视图 ----
// ORIG 0x842d6ce 逐偏移读：+0x00 m_id / +0x04 ip[0x10]（SQL '%s'）/+0x14
// hack_type / +0x18 cnt / +0x1c hack_sub_type / +0x20 hack_sub_cnt /
// +0x24 punish_flag(ushort) / +0x28 etc_value。共 0x2c。
struct DB_InsertAutoPunishFirstUserView
{
    unsigned int m_memberId;     // +0x00
    char m_ip[0x10];             // +0x04
    unsigned int m_hackType;     // +0x14
    unsigned int m_cnt;          // +0x18
    unsigned int m_hackSubType;  // +0x1c
    unsigned int m_hackSubCnt;   // +0x20
    unsigned short m_punishFlag; // +0x24
    unsigned int m_etcValue;     // +0x28
};
static_assert(sizeof(DB_InsertAutoPunishFirstUserView) == 0x2c,
              "DB_InsertAutoPunishFirstUserView ABI drift");

#include "W8LT_DBTask/dispatch_209.inc"


// ---- SIG_INSERT_AUTO_PUNISH_SECOND_USER（SigTypes_B.h m_pad[0x14] 占位）视图 ----
// ORIG 0x842deb4 逐偏移读：+0x00 m_id / +0x04 hack_m_id / +0x08
// total_trade_gold / +0x0c trade_gold / +0x10 punish_flag(bool)。共 0x14。
struct DB_InsertAutoPunishSecondUserView
{
    unsigned int m_memberId;       // +0x00
    unsigned int m_hackMemberId;   // +0x04
    unsigned int m_totalTradeGold; // +0x08
    unsigned int m_tradeGold;      // +0x0c
    bool m_punishFlag;             // +0x10
    char m_pad[3];                 // +0x11..+0x13
};
static_assert(sizeof(DB_InsertAutoPunishSecondUserView) == 0x14,
              "DB_InsertAutoPunishSecondUserView ABI drift");

#include "W8LT_DBTask/dispatch_210.inc"


// ---- SIG_MANNERLESS_USER（SigTypes_E.h m_pad[0x13a] 占位）视图 ----
// ORIG 0x842796a 拼接三段字符串：+0x00（reporter m_id）/+0x1d（target
// m_id）/+0x3a（reason），共 0x13a。
struct DB_MannerlessUserView
{
    char m_reporterId[0x1d];  // +0x00
    char m_targetId[0x1d];    // +0x1d
    char m_reason[0x100];     // +0x3a
};
static_assert(sizeof(DB_MannerlessUserView) == 0x13a,
              "DB_MannerlessUserView ABI drift");

#include "W8LT_DBTask/dispatch_211.inc"


#include "W8LT_DBTask/dispatch_212.inc"


#include "W8LT_DBTask/dispatch_213.inc"


#include "W8LT_DBTask/dispatch_214.inc"


#include "W8LT_DBTask/dispatch_215.inc"


#include "W8LT_DBTask/dispatch_216.inc"


#include "W8LT_DBTask/dispatch_217.inc"


// ============================================================================
// 本批落地（OnlyOrigBatch3）：
//   DB_UpdateOnlinePreliminaryMember::dispatch        ORIG T 0x0843ba9e
//   DB_UpdateOnlinePreliminaryMemberMatchList::dispatch ORIG T 0x0843baa8
//   DBTask::~DBTask                                   ORIG T 0x083fbfda
//   DB_UpdateDeathTowerBestRecord（占位见下，行为按报告补齐）
// ============================================================================

#include "W8LT_DBTask/dispatch_218.inc"


#include "W8LT_DBTask/dispatch_219.inc"


#include "W8LT_DBTask/dtor_DBTask.inc"


// ============================================================================
// DB_UpdateDeathTowerBestRecord（ORIG T 0x0842a35e makeRequest / 0x0842a11e dispatch；
// 报告 DB_UpdateDeathTowerBestRecord/，语义按报告转写）
// ============================================================================

#include "W8LT_DBTask/makeRequest_45.inc"


#include "W8LT_DBTask/dispatch_220.inc"


// ============================================================================
// 本批落地（OnlyOrigBatch4）：
//   DB_UpdateOnlinePreliminaryTeam::dispatch        ORIG T 0x0843babc（mov $1; ret）
//   DB_UpdateOnlinePreliminaryTeamMatchList::dispatch ORIG T 0x0843bad0（mov $1; ret）
//   DB_UpdateUserFatigueQuantity::makeRequest       ORIG T 0x08441c2a
//   DB_UpdateUserFatigueQuantity::dispatch          ORIG T 0x08441a8e
//   DB_UpdateUserGiftFatigueQuantity::makeRequest   ORIG T 0x08441fde
//   DB_UpdateUserGiftFatigueQuantity::dispatch      ORIG T 0x08441d1c
// 报告：docs/class_func_reports/DB_UpdateUserFatigueQuantity/、
//       docs/class_func_reports/DB_UpdateUserGiftFatigueQuantity/。
// ============================================================================

#include "W8LT_DBTask/dispatch_221.inc"


#include "W8LT_DBTask/dispatch_222.inc"


#include "W8LT_DBTask/makeRequest_46.inc"


#include "W8LT_DBTask/makeRequest_47.inc"


#include "W8LT_DBTask/dispatch_223.inc"


#include "W8LT_DBTask/dispatch_224.inc"

#include "W8LT_DBTask/dispatch_225.inc"


// ============================================================================
// OnlyOrigBatch13 —— DB dispatch 实现（ORIG 逐函数对照）
// ============================================================================

// ---- SIG_SECOND_USER_DATA（0x14）/ SIG_SECOND_USER_DATA_SET（0x18）载荷视图 ----
// ORIG DB_AutoPunishedSecondUserData::dispatch 0x0843a462 逐偏移读/写实证：
// req：+0x00 m_id、+0x04/+0x08 trade_gold(ull)、+0x0c trade_cnt、+0x10 punished。
// set：+0x00/+0x04 trade_gold(ull)、+0x08 trade_cnt、+0x0c 0、+0x10 punished、
//      +0x14 m_id。
#pragma pack(push, 1)
struct DB_AutoPunishedSecondUserReqView
{
    unsigned int m_mId;        // +0x00
    unsigned int m_goldLo;     // +0x04
    unsigned int m_goldHi;     // +0x08
    // [b28d-solo] P167：ORIG 两分支均调 `CStreamGuard::lsEi`（int），我方
    // unsigned 字段选中 lsEj ⇒ 目标符号差集 +2/-2；改 int 同宽同偏移
    // （ORIG 0x0843a4xx 两处 `mov 0xc(%eax),%eax` 无零扩展，宽度证据一致）。
    int m_tradeCnt;            // +0x0c
    unsigned int m_punished;   // +0x10
};
struct DB_AutoPunishedSecondUserSetView
{
    unsigned int m_goldLo;     // +0x00
    unsigned int m_goldHi;     // +0x04
    unsigned int m_tradeCnt;   // +0x08
    unsigned int m_pad0c;      // +0x0c
    unsigned int m_punished;   // +0x10
    unsigned int m_mId;        // +0x14
};
#pragma pack(pop)
static_assert(sizeof(DB_AutoPunishedSecondUserReqView) == 0x14,
              "SIG_SECOND_USER_DATA ABI drift");
static_assert(sizeof(DB_AutoPunishedSecondUserSetView) == 0x18,
              "SIG_SECOND_USER_DATA_SET ABI drift");

#include "W8LT_DBTask/dispatch_226.inc"


#include "W8LT_DBTask/dispatch_227.inc"


#include "W8LT_DBTask/dispatch_228.inc"


// ============================================================================
// OnlyOrigBatch13 组 B —— 25 个 DB dispatch/makeRequest 实现（ORIG 逐函数对照）
// ============================================================================
namespace {

// ---- SIG 载荷视图（共享头均为 m_pad 占位；ORIG 反汇编读/写偏移实证）----

// SIG_EXTEND_CHARAC_SLOT（0x8）：+0x00 m_id(uint) / +0x04 byte / +0x05 byte。
#pragma pack(push, 1)
struct SaveCharacSlotView
{
    unsigned int m_mId;          // +0x00
    unsigned char m_slotEffect;  // +0x04（slot_effect_count）
    unsigned char m_slotLimit;   // +0x05（charac_slot_limit）
};
#pragma pack(pop)

// SIG_SAVE_CHATTING_EMOTICON（0x214）：+0x00 m_id / +0x04 起 0x210 字节压缩源。
#pragma pack(push, 1)
struct ChattingEmoticonView
{
    unsigned int m_mId;          // +0x00
    char m_data[0x210];          // +0x04
};
#pragma pack(pop)

// SIG_SAVE_GAME_OPTION_QUICKCHAT（0x25c）：+0x00 m_id / +0x04 起 0x258 字节压缩源。
#pragma pack(push, 1)
struct QuickChattingView
{
    unsigned int m_mId;          // +0x00
    char m_data[0x258];          // +0x04
};
#pragma pack(pop)

// SIG_SECURITY_CARD_RETRANSFER（0x8c）载荷视图（dispatch @0x842c362 读偏移实证）：
// +0x00 m_id / +0x04 cert_key / +0x25 card / +0x2b server_key / +0x52 serial /
// +0x5e phone[0x29]。
#pragma pack(push, 1)
struct SecuRetransferReqView
{
    unsigned int m_mId;          // +0x00
    char m_certKey[0x21];        // +0x04
    char m_card[0x6];            // +0x25
    char m_serverKey[0x27];      // +0x2b
    char m_serial[0xc];          // +0x52
    char m_phone[0x29];          // +0x5e
};
#pragma pack(pop)

// SIG_STATISTIC_CERASHOP_GIFT_LOG（0x18）：6 个 uint（from_mid/from_charac_no/
// to_mid/to_charac_no/item_idx/item_cnt）。
#pragma pack(push, 1)
struct CeraShopGiftLogView
{
    unsigned int m_fromMid;      // +0x00
    unsigned int m_fromCharacNo; // +0x04
    unsigned int m_toMid;        // +0x08
    unsigned int m_toCharacNo;   // +0x0c
    unsigned int m_itemIdx;      // +0x10
    unsigned int m_itemCnt;      // +0x14
};
#pragma pack(pop)

// SIG_UPDATE_NPC_RELATIONSHIP（0x260）：+0x00 characNo(int) / +0x04 npcCnt(int)
// / +0x08 npc_data blob（blob 长 = npcCnt*12）。
#pragma pack(push, 1)
struct UpdateNpcRelationView
{
    int m_characNo;              // +0x00
    int m_npcCnt;                // +0x04
    char m_npcData[0x258];       // +0x08
};
#pragma pack(pop)

}  // namespace

// ============================================================================
// DB_ReportMannerlessUser_Upgrade::dispatch —— ORIG 0x084276b0
// GetOutBuffer<Packet_Mannerless_User_Accusation>（NULL 返 0）→ DB_HANDLE_4 →
// escape_string(targetName/chatMsg)；isNeedTypeForChattingMsg(serverGroup) 决定
// values 串尾（'%s' / 'NULL'）→ sprintf 拼 SQL → set_query → 返 exec==1。
// ============================================================================

#include "W8LT_DBTask/dispatch_229.inc"


// ============================================================================
// DB_RequestTagCharacInfo::dispatch —— ORIG 0x0844768c
// 依次调 DB_LoadCharac::_GetTagCharacInfo(+0x05)、DB_LoadInventory::
// _GetTagCharacInven(+0x7d)、_GetUserAvatar(+0x1ec4/+0x5bb/+0x7d/+0x1ec0)、
// DB_LoadEtc::_LoadQuestShop(+0x40a1)；任一失败 cMyTrace 返 0；全成
// sendResult(slot, sig)（定义 OrigDBInter19.cpp）返 1。
// ============================================================================
// ---- SIG_REQUEST_TAG_CHARAC_INFO 分区视图（ORIG 0x0844768c 调用实参基址
// 实证；分区尺寸由权威类型 sizeof 推导：+0x05 TAG_CHARAC_DATA(0x78) →
// +0x7d（inven 区 TAG_CHARAC_INVEN_DATA(0x4024) → +0x40a1）、
// +0x40a1 CHARAC_QUEST_SHOP_DATA(0xaa) → +0x414b。_GetUserAvatar 的
// equip(+0x7d, 22×0x3d)/avatar(+0x5bb, 0x69×0x3d)/cnt(+0x1ec0)/
// detail(+0x1ec4) 叠放于 inven 区同一存储（ORIG 双视图复用；union 因
// Inven_Item/TAG_* 带 ctor 不可用，按叠放视图分别 reinterpret）----
#pragma pack(push, 1)
struct DB_TagCharacInvenView    // inven 区整块视图（基址 = sig）
{
    char m_pad0[0x7d];          // +0x00..0x7c（characNo/tagCharac 段）
    TAG_CHARAC_INVEN_DATA m_tagInven;  // +0x7d（0x4024）
};
#pragma pack(pop)
#pragma pack(push, 1)
struct DB_TagCharacAvatarView   // _GetUserAvatar 实参叠放视图
{
    char m_pad0[0x7d];          // +0x00..0x7c
    Inven_Item m_equipItems[0x16];     // +0x007d（22×0x3d → +0x05bb）
    Inven_Item m_avatarItems[0x69];    // +0x05bb（0x69×0x3d → +0x1ec0）
    int m_avatarItemCnt;        // +0x1ec0
    AvatarDetailInfo m_avatarDetail;   // +0x1ec4（0x4b）
};
#pragma pack(pop)
#pragma pack(push, 1)
struct DB_TagCharacSigView
{
    unsigned int m_characNo;                 // +0x0000（SQL/日志 characNo）
    char m_pad04[1];                         // +0x0004
    TAG_CHARAC_DATA m_tagCharac;             // +0x0005（0x78）
    char m_inven[0x4024];                    // +0x007d（叠放区，见上视图）
    CHARAC_QUEST_SHOP_DATA m_questShop;      // +0x40a1（0xaa → +0x414b）
    char m_pad414b[1];                       // +0x414b
};
#pragma pack(pop)
static_assert(sizeof(DB_TagCharacSigView) == 0x414c,
              "DB_TagCharacSigView ABI drift");
static_assert(offsetof(DB_TagCharacSigView, m_questShop) == 0x40a1,
              "DB_TagCharacSigView questShop offset drift");

#include "W8LT_DBTask/dispatch_230.inc"


// ============================================================================
// DB_SaveCharacSlotExtend::dispatch —— ORIG 0x084420ea
// upDate charac_view（slot_effect_count/slot_charac_limit/m_id）；成功条件 =
// exec && affectedRowCount!=0，否则 cMyTrace 失败日志返 0。
// ============================================================================

#include "W8LT_DBTask/dispatch_231.inc"


// ============================================================================
// DB_SaveEventLevelupSupport::dispatch —— ORIG 0x0844cafe
// stream >> 3×int；DB_HANDLE_2（NULL 返 0）→ upDate event_levelup_support；
// exec 失败返 0；affectedRowCount==0 补 inSert，exec 失败返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_232.inc"


// ============================================================================
// DB_SaveGameOption_ChattingEmoticon::dispatch —— ORIG 0x08430412
// DB_HANDLE_6 → put_compressed_blob_data(sig+4, 0x210) 失败返 0 → upDate %s
// set shortcut_emoticon（get_member_game_option 表名）；exec 失败或
// affectedRowCount==0 补 inSert，exec 失败返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_233.inc"


// ============================================================================
// DB_SaveGameOption_QuickChatting::dispatch —— ORIG 0x08430250
// DB_HANDLE_6 → compress_zip(buf,&len,sig+4,0x258) 失败返 0 → blob_to_str(1) →
// upDate %s set option_3；exec 失败或 affectedRowCount==0 补 inSert
// （blob_to_str(0)），exec 失败返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_234.inc"


// ============================================================================
// DB_SavePcRoomPlayTimeEvent::dispatch —— ORIG 0x08447992
// DB_HANDLE_9 → 表名 get_db_table_name(0x5c)（std::string 视图经 toTString）→
// upDate %s set play_time/reward_cnt；exec 失败 logFormat(0xb25b) 返 0；
// affectedRowCount==0 补 inSert，exec 失败 logFormat(0xb266) 返 0；成功返 1。
// （makeRequest 已定义于 W8LT_DBTask.cpp。）
// ============================================================================

#include "W8LT_DBTask/dispatch_235.inc"


// ============================================================================
// DB_SecurityCardAddCertCnt::makeRequest —— ORIG 0x08436388（static）
// Acquire("DBThread.cpp",0x8199) → CStreamGuard(stream,true) → <<0x17c → <<-1
// → GetInBuffer<SIG_SECURITY_CARD_ADD_CERT_CNT>() → sig[0]=accId →
// put(DB_Q)。
// ============================================================================

#include "W8LT_DBTask/makeRequest_48.inc"


// ============================================================================
// DB_SecurityCardRetransfer::dispatch —— ORIG 0x0842c362
// 基类 DBDispatcher::dispatch 失败返 0 → DB_HANDLE_6 查 member_security_card
// （re_issue_cnt/last_issue_time/phone）；exec/fetch 失败 throw 1；phone 与
// sig+0x5e 不符 throw 0x7d；与上次同日 → reIssueCnt+1 且 >3 throw 0x78，
// 否则清零 → upDate member_security_card（cert_key/server_key/card/re_issue_cnt
// /m_id；exec 结果忽略）→ _sendResult(slot, serial, certKey, serverKey, 0)
// （定义 OrigDBInter19.cpp）；catch(int) → _sendResult(...,0,0,0,err) → DB_HANDLE_1
// upDate member_security_grade（失败/affected==0 补 inSert，失败返 0）→ 返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_236.inc"


// ============================================================================
// DB_SecurityCardUpdateFailCnt::makeRequest —— ORIG 0x0842bfaa（static）
// Acquire("DBThread.cpp",0x6f0b) → CStreamGuard(stream,true) → <<0x101 → <<-1
// → GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>() → sig[0]=accId、
// sig[4]=failCnt → put(DB_Q)。
// ============================================================================

#include "W8LT_DBTask/makeRequest_49.inc"


// ============================================================================
// DB_SelectLevelUpReward::dispatch —— ORIG 0x084423cc
// DB_HANDLE_2 → >>characNo(uint) >>rewardIdx(int) → 表名 get_db_table_name
// (0x194) → seLect recv_level；exec 失败返 0；rows==0 → inSert；有行 fetch/
// get_ubyte 失败返 0；recvLevel==rewardIdx → logFormat(0xa2a5) 返 0；否则
// upDate recv_level（exec 失败返 0）→ 组包（0xa2ba）：<<userIdx <<slot
// <<rewardIdx <<recvLevel → put(DISPATCH_Q) 返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_237.inc"


// ============================================================================
// DB_StatisticCeraShopGiftLog::dispatch —— ORIG 0x08444066
// DB_HANDLE_4（NULL 返 0）→ inSert log_cerashop_gift（6 个 uint 直传）；exec
// 失败 cMyTrace(0xa9c1/0xa9c3) 返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_238.inc"


// ============================================================================
// DB_TOD_Random_Select_UserAPC::dispatch —— ORIG 0x084400b8
// 空实现，恒返 1（ORIG 仅 push/mov $1/leave/ret 四指令）。
// ============================================================================

#include "W8LT_DBTask/dispatch_239.inc"


#include "W8LT_DBTask/makeRequest_50.inc"


// ============================================================================
// DB_UpdateAuraAvatarOption::dispatch —— ORIG 0x08444e1e
// DB_HANDLE_2 → >>characNo(uint) >>optionType(int) >>value(int) → upDate
// aura_avatar_option；exec 失败或 affectedRowCount==0 补 inSert（exec 结果
// 忽略）→ 恒返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_240.inc"


// ============================================================================
// DB_UpdateBlueMarbleEnterCount::dispatch —— ORIG 0x0844874e
// DB_HANDLE_2 → >>characNo(uint) >>enterCnt(uchar) → upDate charac_option；
// exec 失败返 0；affectedRowCount==0 补 inSert，exec 失败返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_241.inc"


// ============================================================================
// DB_UpdateBreakAwayAccruedCera::dispatch —— ORIG 0x0843865c
// DB_HANDLE_6 → >>m_id(uint) >>cera(uint) → upDate churn_member_info
// accrue_cera 累加；exec 失败 logFormat(0x8619) 返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_242.inc"


// ============================================================================
// DB_UpdateBreakAwayClearAddInfo::dispatch —— ORIG 0x0843848c
// DB_HANDLE_6 → >>m_id(uint) → upDate churn_member_info set add_info=-1；
// exec 失败或 affectedRowCount==0 → logFormat(0x85f4) 返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_243.inc"


// ============================================================================
// DB_UpdateBreakAwayDungeonClearCnt::dispatch —— ORIG 0x084383ba
// DB_HANDLE_6 → >>m_id(uint) >>clearCnt(int) → upDate churn_member_info
// set add_info=%d；exec 失败或 affectedRowCount==0 返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_244.inc"


// ============================================================================
// DB_UpdateBreakAwayRuckPoint::dispatch —— ORIG 0x08438300
// DB_HANDLE_6 → >>m_id(uint) → upDate churn_member_info set luck_point=0；
// exec 失败或 affectedRowCount==0 返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_245.inc"


// ============================================================================
// DB_UpdateBreakAwayUvList::dispatch —— ORIG 0x08438842
// DB_HANDLE_6 → curSec=getCurSec() → >>m_id(uint) → GetOutBuffer<
// SIG_BREAK_AWAY_UV_LIST>（play_info 字符串）→ upDate churn_member_info
// play_info/last_update_time；exec 失败或 affectedRowCount==0 返 0；成功返 1。
// （makeRequest 定义于 Dispatchers.cpp 域，本 TU 不重复。）
// ============================================================================

#include "W8LT_DBTask/dispatch_246.inc"


// ============================================================================
// DB_UpdateDungeonDataStatisticLogdb::dispatch —— ORIG 0x0842456a
// 空实现，恒返 1（ORIG 仅 push/mov $1/pop/ret 四指令）。
// ============================================================================

#include "W8LT_DBTask/dispatch_247.inc"


// ============================================================================
// DB_UpdateExpertJobStatistic::dispatch —— ORIG 0x08436964
// DB_HANDLE_4 → >>count(uint)（0 则返 1）→ 循环 count 次：>>type(int)
// >>itemCnt(ulong) >>itemId(uint)；itemCnt==0 break；upDate
// expert_job_product_cnt（item_cnt 累加）；exec 失败或 affectedRowCount==0 补
// inSert；循环结束返 1（恒）。
// ============================================================================

#include "W8LT_DBTask/dispatch_248.inc"


// ============================================================================
// DB_UpdateItemDropStatistics::dispatch —— ORIG 0x084285ae
// DB_HANDLE_4 → >>channelNo(int) >>dropType(int) → 4×6 个 uint 数组
// （stackable/equip/recipe/artifact）从 stream 读 → inSert log_item_drop
// （26 个 %d 直传）→ 返 exec(true) 结果（ORIG 无失败分支）。
// ============================================================================

#include "W8LT_DBTask/dispatch_249.inc"


// ============================================================================
// DB_UpdateMemberDoubtTrade::dispatch —— ORIG 0x084284a2
// >>m_id(uint) → DB_HANDLE_6 → curSec=getCurSec() → upDate member_doubt_trade
// （over_count+1）；exec 失败或 affectedRowCount==0 补 inSert（over_count=1），
// exec 失败返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_250.inc"


// ============================================================================
// DB_UpdateNPCRelationship::dispatch —— ORIG 0x08435e1e
// DB_HANDLE_2 → GetOutBuffer<SIG_UPDATE_NPC_RELATIONSHIP> → blob_to_str(0,
// sig+8, npcCnt*12) → upDate charac_npc（npc_cnt/npc_data/charac_no）→
// exec 结果忽略，恒返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_251.inc"



// ============================================================================
// OnlyOrigBatch13 组 A —— 24 个 DB dispatch 实现（ORIG 逐函数对照）
// ============================================================================
// ============================================================================
// groupA.cpp —— OnlyOrigBatch13.DBGroupA：25 个 DB dispatch/makeRequest 符号
// 从 ORIG ELF 反汇编逐函数翻译（语义与 ORIG 相等）。
//
// 符号清单（对应 /tmp/df_batches/dis/ 下同名 .dis）：
//   CLocalGlobal_DB_STATICS_BOSSTOWER::dispatch          ORIG 0x0812825e
//   DB_DeathTowerLoadTopRanker::dispatch                 ORIG 0x0842a484
//   DB_EventAccountCharacterLevelUpOnceGift::dispatch    ORIG 0x08449e3e
//   DB_EventSaveGiveGrowCreature::dispatch               ORIG 0x08449a58
//   DB_EventSaveGrowthWeapon::dispatch                   ORIG 0x084492fc
//   DB_EventUpdateGiveGrowCreature::dispatch             ORIG 0x08449c54
//   DB_EventUpdateGrowthWeaponCharac::dispatch           ORIG 0x084494e2
//   DB_GetDeathTowerBestRecord::dispatch                 ORIG 0x08429ad0
//   DB_GoblinPadUpdateReward::dispatch                   ORIG 0x0842413c
//   DB_InsertDungeonEntrance_hour::dispatch              ORIG 0x084298a2
//   DB_InsertPoliceSaveChatting::dispatch                ORIG 0x0843c514
//   DB_InsertRandomOptionItemInform::dispatch            ORIG 0x08442c6a
//   DB_LoadBreakAwayPreventData::dispatch                ORIG 0x084389fe
//   DB_LoadBreakAwayRewardData::dispatch                 ORIG 0x084392dc
//   DB_LoadCharacHotKeyOption::dispatch                  ORIG 0x0842f3a0
//   DB_LoadEventLevelupSupport::dispatch                 ORIG 0x0844cd38
//   DB_LoadOnlinePreliminaryTeam::dispatch               ORIG 0x0843bab2
//   DB_LoadOnlinePreliminaryTeamMatchList::dispatch      ORIG 0x0843bac6
//   DB_LoadPowerWarStatueRanker::dispatch                ORIG 0x08434878
//   DB_MailBoxSendNewMailWithDBWork::dispatch            ORIG 0x084272de
//   DB_NoticeIngameAdvertisement::dispatch               ORIG 0x0844710e
//   DB_PassPadUpdateCancelCnt::dispatch                  ORIG 0x08423fb6
//   DB_ReloadAutoPunishRuleBackIP::dispatch              ORIG 0x0842cdf6
//   DB_ReloadAutoPunishRuleHackType::dispatch            ORIG 0x0842d16c
//
// 注：DB_PassPadUpdateCancelCnt::makeRequest（ORIG 0x0842403a）已由
// DB_AccountCargoSync.cpp 完整定义（opcode 0xa5/-1 + 3 实参），本文件不重复
// 定义，仅实现其 dispatch。
// ============================================================================
                                  // DB_GoblinPadUpdateReward / DB_InsertRandomOptionItemInform（权威声明）

#include "W8LT_DBTask/__GetCurrentDNFDate.inc"

   // ORIG C1 0x844fe32
   // ORIG C1 0x844fd8a
   // ORIG C1 0x844fda6

   // ORIG C1 0x844feda

   // ORIG C1 0x844f35e（同址）
   // ORIG C1 0x844f37a

   // ORIG C1 0x844ea12
   // ORIG C1 0x844ea2e
// MailBoxSendNewMailWithDBWork 依赖的既有 DB 类（实现归 W8LT_DBTask.cpp / DB_AccountCargoSync.cpp）

// ============================================================================
// 载荷视图（SIG_* 多为 m_pad 占位；ORIG 逐偏移读/写实证，禁止裸偏移）
// ============================================================================

struct DB_TowerRankerRankView   // +0x06
{
    unsigned short m_ranks[0x800];
};
// +0xfa0 行视图（步长 0x6c）：+0x0c towerIndex(ushort)、+0x0e type(ushort)、
// +0x10 memberInfo[0x17]（ORIG DB_DeathTowerLoadTopRanker 0x0842a866..0x0842a87b
// 实测：get_ushort(1)→+0xc、写 type→+0xe、get_binary(2)→+0x10）。
struct DB_TowerRankerRowView
{
    char m_pad0[0xc];              // +0x00..+0x0b（ORIG 未写）
    unsigned short m_towerIndex;   // +0x0c
    unsigned short m_type;         // +0x0e
    char m_memberInfo[0x17];       // +0x10
};
// +0x1000 行视图（步长 0x6c）：+0x00 stage(uint)、+0x04 playTime(uint)。
struct DB_TowerRankerPlayView
{
    unsigned int m_stage;          // +0x00
    unsigned int m_playTime;       // +0x04
};
// [Lane31W8LTDBTaskView] 包头（count/flag）已字段化至 SigTypes_B.h
// SIG_DEATH_TOWER_LOAD_TOP_RANKER（匿名 union 双视角）；行区继续用显式偏移
// 视图（P82），本地包头视图 DB_DeathTowerRankerView 删除。
#pragma pack(pop)

// ---- SIG_DEATH_TOWER_GET_BEST_RECORD（0x878）行视图 ----
// ORIG 0x08429ad0：+0x00 count(uint)、+0x04 characNo(uint)；行步长 0x1b0
// （行内偏移 +0x0c..+0x1b4，ORIG 末两字段越入下行的原布局）：
//   +0x0c tower_index(ushort)、+0x0e type=1、+0x10 member_info1[0x17]、
//   +0x6c stage1、+0x70 play_time1、+0x7a type=2、+0x7c member_info2[0x2e]、
//   +0xd8 stage2、+0xdc play_time2、+0xe6 type=3、+0xe8 member_info3[0x45]、
//   +0x144 stage3、+0x148 play_time3、+0x152 type=4、+0x154 member_info4[0x5c]、
//   +0x1b0 stage4、+0x1b4 play_time4。
#pragma pack(push, 1)
// [b28d-BigW8LT 复原 ORIG 两级寻址（0x08429c60..0x08429fd2 逐站点实测）]
// ORIG 每站点都发 `mov i; imul $0x1b0; add $CONST; add row_base; add $FIELD`，
// 且 CONST 恰分两族、步长各 0x6c：
//   entry 族 @行+0x00 → tower_index(+0x0c)/type(+0x0e)/member(+0x10)，
//     k=0/1/2/3 ⇒ CONST = 0x00/0x6c/0xd8/0x144（member_info1..4、type=1..4）
//   play  族 @行+0x60 → stage(+0x0c)/playTime(+0x10)，
//     k=0/1/2/3 ⇒ CONST = 0x60/0xcc/0x138/0x1a4（stage_1..4、play_time_1..4）
// ⇒ 行内其实是两个 0x6c 步长数组（entry[4] 叠 play[4]，两者字段互相覆盖，
//   故 ORIG 的 stage1 = entry[1]+0x0c、play1 = entry[1]+0x10 同一 0x6c 槽）。
// 我方按此建模并**每站点内联重算**（ORIG 无行指针局部；提升会整体挪帧）。
struct DB_TowerBestRecordEntryView
{
    char m_pad0[0xc];              // +0x00
    unsigned short m_towerIndex;   // +0x0c
    unsigned short m_type;         // +0x0e
    unsigned char m_member[4];     // +0x10（blob 起点，实际长 0x17/0x2e/0x45/0x5c）
    char m_pad14[0x6c - 0x14];     // 组步长 0x6c
};
struct DB_TowerBestRecordPlayView
{
    char m_pad0[0xc];              // +0x00
    unsigned int m_stage;          // +0x0c
    unsigned int m_playTime;       // +0x10
    char m_pad14[0x6c - 0x14];     // 组步长 0x6c
};
struct DB_TowerBestRecordRowView
{
    char m_pad0[0xc];              // +0x00（行 0 的前 12B 即包头 count/characNo/pad）
    unsigned short m_towerIndex;   // +0x0c
    unsigned short m_type;         // +0x0e
    char m_memberInfo1[0x17];      // +0x10
    char m_pad27[0x45];            // +0x27..+0x6b
    unsigned int m_stage1;         // +0x6c
    unsigned int m_playTime1;      // +0x70
    char m_pad74[0x6];             // +0x74..+0x79
    unsigned short m_type2;        // +0x7a
    char m_memberInfo2[0x2e];      // +0x7c
    char m_padaa[0x2e];            // +0xaa..+0xd7
    unsigned int m_stage2;         // +0xd8
    unsigned int m_playTime2;      // +0xdc
    char m_pade0[0x6];             // +0xe0..+0xe5
    unsigned short m_type3;        // +0xe6
    char m_memberInfo3[0x45];      // +0xe8
    char m_pad12d[0x17];           // +0x12d..+0x143
    unsigned int m_stage3;         // +0x144
    unsigned int m_playTime3;      // +0x148
    char m_pad14c[0x6];            // +0x14c..+0x151
    unsigned short m_type4;        // +0x152
    char m_memberInfo4[0x5c];      // +0x154
    unsigned int m_stage4;         // +0x1b0（ORIG 越入下行）
    unsigned int m_playTime4;      // +0x1b4（ORIG 越入下行）
};
// [Lane31W8LTDBTaskView] 包头（count/characNo）已字段化至 SigTypes_B.h
// SIG_DEATH_TOWER_GET_BEST_RECORD；行区继续显式偏移视图（P82）。
#pragma pack(pop)
// ---- SIG_GOBLIN_UPDATE_REWARD（0x18）视图 ----
#pragma pack(push, 1)
struct DB_GoblinPadUpdateRewardView
{
    unsigned int m_mId;        // +0x00
    unsigned char m_field4;    // +0x04
    unsigned char m_field5;    // +0x05
    char m_ip[0x12];           // +0x06（strncpy 0x10 源）
};
#pragma pack(pop)

// [Lane31W8LTDBTaskView] 视图 cast 反模式批量清理：以下三类载荷视图已字段化
// 至权威头（SigTypes_C.h SIG_LOAD_CHARAC_HOTKEY_OPTION；
// SigTypes_D.h SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION；SigTypes_B.h
// SIG_EVENT_LEVEUP_SUPPORT_INFO；SigTypes_A.h SIG_BREAK_AWAY_PREVENT_MESSAGE），
// 本地重复定义删除，写点改权威类型直访问（字段名不变）。

// ---- SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK（0x1a4）视图 ----
#pragma pack(push, 1)
struct DB_MailBoxSendNewMailView
{
    int m_type;                // +0x00（0=avatar 附件 / 1=creature 附件）
    char m_sender[0x1d];       // +0x04
    char m_pad21[3];           // +0x21
    int m_field24;             // +0x24
    int m_field28;             // +0x28
    char m_attach[0x4];        // +0x2c（SIG_AVATAR_INSERT / SIG_CREATURE_ITEM 起始）
    int m_field30;             // +0x30
    // [Main·2026-09-10 round5 真语义 bug 修复 · G4 上报]
    // ORIG 0x0842738x 段实证：creature 分支 `mov 0x34(%eax),%eax`（**4 字节**@0x34）、
    // avatar 分支 `movzbl 0x35(%eax),%eax`（**1 字节**@0x35）—— 两个视图**共享** 0x34 槽，
    // 字节视图落在 4B 视图的第 2 字节。原先写成顺序两个字段（pack(1) 不重叠）⇒
    // `m_field35` 实际落到 +0x38，其后全部字段整体 +3：
    //   m_titleLen 读 +0xa3（应 +0xa0）⇒ 标题长度错、影响 strncpy 与 SQL 载荷；
    //   m_field1a2 读 +0x1a5（应 +0x1a2）。
    // 现用匿名 union 让 4B 视图（m_field34）与字节视图（m_field35 @ +0x35）正确重叠，
    // 后续字段偏移随之回到 ORIG 值（m_pad38 起 +0x38，m_field40 @ +0x40 …）。
    union                       // +0x34
    {
        int m_field34;         //   +0x34（4B，creature 分支用）
        struct                 //   +0x34 起的字节视图（1 + 1 + 2 = 4B）
        {
            char m_pad34;              // +0x34
            unsigned char m_field35;   // +0x35（avatar 分支用）
            char m_pad36b[2];          // +0x36..+0x37
        };
    };
    char m_pad38[0x8];         // +0x38..+0x3f
    unsigned short m_field40;  // +0x40（creature 用）
    char m_pad42[0x12];        // +0x42..+0x53
    int m_field54;             // +0x54
    char m_pad58[0x48];        // +0x58..+0x9f
    unsigned short m_titleLen; // +0xa0
    char m_title[0x100];       // +0xa2
    unsigned char m_field1a2;  // +0x1a2
};
struct DB_MailSendDataView      // SIG_MAILBOX_SEND_NEW_MAIL（0x168）
{
    char m_sender[0x1d];        // +0x00
    char m_pad1d[1];            // +0x1d
    int m_field1e;              // +0x1e
    int m_field22;              // +0x22
    int m_field26;              // +0x26
    unsigned char m_field2a;    // +0x2a
    int m_field2b;              // +0x2b
    unsigned int m_identity;    // +0x2f
    unsigned char m_field33;    // +0x33
    unsigned short m_field34;   // +0x34
    int m_field36;              // +0x36
    char m_pad3a[0x1c];         // +0x3a..+0x55
    unsigned char m_field56;    // +0x56
    unsigned char m_field57;    // +0x57
    unsigned char m_field58;    // +0x58
    unsigned short m_field59;   // +0x59
    char m_title[0x10d];        // +0x5b
};
#pragma pack(pop)

// [Main·2026-09-10 round5] 布局锁定（R2 精神）：上面两个载荷视图的**关键偏移**
// 必须与 ORIG 的字段读取地址逐一对应，否则运行期读错字段（曾出过 m_field35
// 顺序化导致其后全部字段 +3 的真 bug）。改动该结构体必须让这些断言继续成立。
static_assert(offsetof(DB_MailBoxSendNewMailView, m_field34) == 0x34,
              "m_field34 须在 +0x34（creature 分支 4B 读）");
static_assert(offsetof(DB_MailBoxSendNewMailView, m_field35) == 0x35,
              "m_field35 须在 +0x35（avatar 分支 1B 读，与 m_field34 共享槽）");
static_assert(offsetof(DB_MailBoxSendNewMailView, m_field40) == 0x40, "m_field40 @ +0x40");
static_assert(offsetof(DB_MailBoxSendNewMailView, m_titleLen) == 0xa0, "m_titleLen @ +0xa0");
static_assert(offsetof(DB_MailBoxSendNewMailView, m_title) == 0xa2, "m_title @ +0xa2");
static_assert(offsetof(DB_MailBoxSendNewMailView, m_field1a2) == 0x1a2, "m_field1a2 @ +0x1a2");
// 末字段偏移 0x1a2（1B）⇒ sizeof 0x1a3。**不要**把它断言成信号长 0x1a4：
// 视图只需覆盖被代码访问到的字段，尾部填充字节不影响字段地址。
static_assert(sizeof(DB_MailBoxSendNewMailView) == 0x1a3, "视图 sizeof 须为 0x1a3");
static_assert(offsetof(DB_MailSendDataView, m_title) == 0x5b, "DB_MailSendDataView::m_title @ +0x5b");

// ---- SIG_SECURITY_CARD_UPDATE_FAIL_CNT（0x18）视图 ----
#pragma pack(push, 1)
struct DB_PassPadUpdateCancelCntView
{
    unsigned int m_mId;        // +0x00
    unsigned int m_cancelCnt;  // +0x04
    char m_ip[0x10];           // +0x08
};
#pragma pack(pop)

// ---- SIG_AUTO_PUNISH_RULE_BLACK_IP（0x3e84）视图 ----
#pragma pack(push, 1)
struct DB_AutoPunishBlackIpView
{
    unsigned short m_count;    // +0x00
    char m_pad02[2];           // +0x02
    struct Entry                // 步长 8，自 +0x04 起
    {
        unsigned int m_ip;     // +0x04（inet_addr 结果）
        unsigned char m_start; // +0x08
        unsigned char m_end;   // +0x09
        char m_pad0a[2];       // +0x0a..0x0b [b23S3C] ORIG 0x0842d027 `mov %eax,0x4(%edx,%ebx,8)`
                               //  scaled 寻址实证步长 8（原缺尾 pad ⇒ sizeof 6，4.4 出
                               //  `add/add/add` i*6 链，索引偏移全错 = 真 bug）
    } m_entries[0x7d0];
};
#pragma pack(pop)

// ---- SIG_AUTO_PUNISH_RULE_HACK_TYPE（0x2720）----
// [Lane31W8LTDBTaskView] 视图 cast 反模式清理：本地 DB_AutoPunishHackTypeView
// 已整体字段化至 SigTypes_A.h SIG_AUTO_PUNISH_RULE_HACK_TYPE（含行类型
// SIG_AUTO_PUNISH_RULE_HACK_TYPE_ENTRY，步长 0x14），本地定义删除。

#include "W8LT_DBTask/dispatch_252.inc"
                                                 Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int keyCount = 0;
    *stream >> keyCount;
    if (keyCount <= 0)
        return 0;
    struct tm tmNow;
    __GetCurrentDNFDate(tmNow);
    char date[12];
    memset(date, 0, sizeof(date));
    sprintf(date, "%04d-%02d-%02d", tmNow.tm_year + 0x76c,
            tmNow.tm_mon + 1, tmNow.tm_mday);
    for (int i = 0; i < keyCount; ++i)
    {
        // [A04-r5] 判据②：ORIG 对 key 走**字节视图**（0x0812825e 段：
        // `movzbl -0x47(%ebp),%eax; movzbl %al,%eax` = byte1、`movzbl -0x46` = byte2、
        // `movzbl -0x45; and $0xf` = byte3&0xf、`movzbl -0x45; shr $0x4,%al` = byte3>>4、
        // `movzbl -0x48` = byte0），共 18 条 movzbl；我方 `(key>>N)&0xff` 被 4.4 折成
        // `mov;shr;and`（widthscan ext_movzbl orig=20 / our=2）⇒ 改局部匿名 union 双视角。
        union
        {
            unsigned int m_key;
            unsigned char m_byte[4];
        } key;
        key.m_key = 0;
        *stream >> key.m_key;
        STBossDungeonTowerLog* sig = stream->GetOutBuffer<STBossDungeonTowerLog>();
        // ORIG 列映射：play_cnt=+0x0 / success_cnt=+0x4 / play_time=+0xc /
        // uniq_cnt=+0x8（UPDATE 与 INSERT 同口径）。
        // ORIG 0x0812825e：每个 key 先打一条 trace（行号 0x4f9）再 UPDATE。
        // [J4S4-⑱a] ORIG 0x081283a4 `movl $0x5,0xc(%esp)`（level 实参）⇒ 原码为 5。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x4f9, 5);
        trace("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
              key.m_key, (int)key.m_byte[0], (int)(key.m_byte[3] >> 4),
              (int)(key.m_byte[3] & 0xf),
              (int)key.m_byte[2], (int)key.m_byte[1], sig->stageStartCount,
              sig->stageEndCount, sig->playTimeSum, sig->pickUpItemCount);
        db->set_query(
            "upDate log_monstertower_layer_stat SET play_cnt = play_cnt + %u, success_cnt = success_cnt + %u, play_time = play_time + %u, uniq_cnt = uniq_cnt + %u WHERE occ_date = '%s' and key_id = %u",
            sig->stageStartCount, sig->stageEndCount, sig->playTimeSum,
            sig->pickUpItemCount, date, key.m_key);
        // [J4S3-23 P0] ORIG 0x081284ab `test %al,%al` 直判（无 -0x19 具名 bool 槽）
        // ⇒ 删具名 needInsert，条件内联。
        if (!db->exec(true) || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into log_monstertower_layer_stat(occ_date, key_id, layer, monster_diff, charac_job, charac_grow, level, play_cnt, success_cnt, play_time, uniq_cnt) values('%s', %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)",
                date, key.m_key, (int)key.m_byte[0], (int)(key.m_byte[3] >> 4),
                (int)(key.m_byte[3] & 0xf),
                (int)key.m_byte[2], (int)key.m_byte[1], sig->stageStartCount,
                sig->stageEndCount, sig->playTimeSum, sig->pickUpItemCount);
            db->exec(true);
        }
    }
    return 1;
}

#include "W8LT_DBTask/dispatch_253.inc"


#include "W8LT_DBTask/dispatch_254.inc"


#include "W8LT_DBTask/dispatch_255.inc"


#include "W8LT_DBTask/dispatch_256.inc"


#include "W8LT_DBTask/dispatch_257.inc"


#include "W8LT_DBTask/dispatch_258.inc"


#include "W8LT_DBTask/dispatch_259.inc"


#include "W8LT_DBTask/dispatch_260.inc"


#include "W8LT_DBTask/dispatch_261.inc"


#include "W8LT_DBTask/dispatch_262.inc"


#include "W8LT_DBTask/dispatch_263.inc"


#include "W8LT_DBTask/dispatch_264.inc"


#include "W8LT_DBTask/dispatch_265.inc"


#include "W8LT_DBTask/dispatch_266.inc"


#include "W8LT_DBTask/dispatch_267.inc"


#include "W8LT_DBTask/dispatch_268.inc"


#include "W8LT_DBTask/dispatch_269.inc"


#include "W8LT_DBTask/dispatch_270.inc"


#include "W8LT_DBTask/dispatch_271.inc"


#include "W8LT_DBTask/dispatch_272.inc"


#include "W8LT_DBTask/dispatch_273.inc"


#include "W8LT_DBTask/dispatch_274.inc"


#include "W8LT_DBTask/dispatch_275.inc"

