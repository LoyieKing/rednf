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
//
// ORIG T GetCurrentResetBaseTime(long, int) @0x0810912a（LinkClosureA_Misc.cpp
// 定义；DB_Login.cpp/DB_LoadEtc.cpp 同款局部声明）。
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

// CLocalChina_DB_BreakAwayReset::dispatch —— ORIG 0x08127e84
// 读 accId；UPDATE churn_member_info 复位 break-away 数据。失败 log 返 0。
#include "W8LT_DBTask/dispatch.inc"
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    *stream >> m_id;
    db->set_query(
        "UPDATE churn_member_info \t\t\t\t   SET accrue_cera = 0, first_reward_time = 0, last_reward_time = 0, \t\t\t\t\t   add_info = 0, luck_point = 0, last_update_time = 0, second_reward_time = 0, quest_time = 0 \t\t\t\t   WHERE m_id=%s",
        NumberToString(m_id, 0));
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localchina/localchina_DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x3ff, "[GM]CLocalChina_DB_BreakAwayReset Fail!!");
        return 0;
    }
    return 1;
}

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
    char* m_agency;            // +0x0f（%s 指针）
    char m_pad13[0x15];        // +0x13..+0x27
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
    int m_characNo;        // +0x00（DB_UpdatePvPGrade::dispatch 唯一消费字段）
    int m_pvpGrade;        // +0x04（SQL 列 1）
    int m_pvpPoint;        // +0x08（SQL 列 2）
    int m_lastPlay;        // +0x0c（SQL 列 3）
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

// ORIG 0x8421066：8 名成员 ping 上报。每行 m_partyEntries[i][9..12] 拼
// "a.b.c.d" 存入 buf[i]（0x10 字节行），SQL 25 个占位 + now()。
// [b28c-L19 SIGFIX] 返回 int→bool（ORIG __PRETTY_FUNCTION__ 串实证）。
#include "W8LT_DBTask/save_dnf_pvp_ping_history.inc"
{
    char buf[8][0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    int i;
    for (i = 0; i <= 7; ++i)
        sprintf(buf[i], "%d.%d.%d.%d",
                (unsigned char)sig->m_partyEntries[i][9],
                (unsigned char)sig->m_partyEntries[i][10],
                (unsigned char)sig->m_partyEntries[i][11],
                (unsigned char)sig->m_partyEntries[i][12]);
    db->set_query(
        "inSert into dnf_party_ping_history_pvp(str_ip,m_id,nat_type_1,ping1,mtu_1,"
        "nat_type_2,ping2,mtu_2,nat_type_3,ping3,mtu_3,nat_type_4,ping4,mtu_4,"
        "nat_type_5,ping5,mtu_5,nat_type_6,ping6,mtu_6,nat_type_7,ping7,mtu_7,"
        "nat_type_8,ping8,mtu_8,occ_time,map_id,dungeon_id,ip_1,ip_2,ip_3,ip_4,"
        "ip_5,ip_6,ip_7,ip_8, end_reason) values('%s',%s,%d,%d,%d,%d,%d,%d,%d,%d,"
        "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now(),%d,%d,'%s','%s','%s',"
        "'%s','%s','%s','%s','%s',%d)",
        sig->m_ip,
        NumberToString(sig->m_accId, 0),
        ((DB_PartyEntriesView*)sig)->m_nat0,
        ((DB_PartyEntriesView*)sig)->m_ping0,
        ((DB_PartyEntriesView*)sig)->m_mtu0,
        ((DB_PartyEntriesView*)sig)->m_nat1,
        ((DB_PartyEntriesView*)sig)->m_ping1,
        ((DB_PartyEntriesView*)sig)->m_mtu1,
        ((DB_PartyEntriesView*)sig)->m_nat2,
        ((DB_PartyEntriesView*)sig)->m_ping2,
        ((DB_PartyEntriesView*)sig)->m_mtu2,
        ((DB_PartyEntriesView*)sig)->m_nat3,
        ((DB_PartyEntriesView*)sig)->m_ping3,
        ((DB_PartyEntriesView*)sig)->m_mtu3,
        ((DB_PartyEntriesView*)sig)->m_nat4,
        ((DB_PartyEntriesView*)sig)->m_ping4,
        ((DB_PartyEntriesView*)sig)->m_mtu4,
        ((DB_PartyEntriesView*)sig)->m_nat5,
        ((DB_PartyEntriesView*)sig)->m_ping5,
        ((DB_PartyEntriesView*)sig)->m_mtu5,
        ((DB_PartyEntriesView*)sig)->m_nat6,
        ((DB_PartyEntriesView*)sig)->m_ping6,
        ((DB_PartyEntriesView*)sig)->m_mtu6,
        ((DB_PartyEntriesView*)sig)->m_nat7,
        ((DB_PartyEntriesView*)sig)->m_ping7,
        ((DB_PartyEntriesView*)sig)->m_mtu7,
        sig->m_field18,
        sig->m_field1c,
        buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
        (unsigned char)sig->m_field20);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x58db, 5);
        trace("DB_GatheringParyStatus failed");
    }
    return 1;
}

// ORIG 0x84217b6：4 名成员 ping 上报（结构同 pvp 版，仅 4 组 + dnf_party_ping_history）。
// [b28c-L19 SIGFIX] 返回 int→bool（同上）。
#include "W8LT_DBTask/save_dnf_party_ping_history.inc"
{
    char buf[4][0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    int i;
    for (i = 0; i <= 3; ++i)
        sprintf(buf[i], "%d.%d.%d.%d",
                (unsigned char)sig->m_partyEntries[i][9],
                (unsigned char)sig->m_partyEntries[i][10],
                (unsigned char)sig->m_partyEntries[i][11],
                (unsigned char)sig->m_partyEntries[i][12]);
    db->set_query(
        "inSert into dnf_party_ping_history(str_ip,m_id,nat_type_1,ping1,mtu_1,"
        "nat_type_2,ping2,mtu_2,nat_type_3,ping3,mtu_3,nat_type_4,ping4,mtu_4,"
        "occ_time,map_id,dungeon_id,ip_1,ip_2,ip_3,ip_4, end_reason) values('%s',"
        "%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,now(),%d,%d,'%s','%s','%s','%s',%d)",
        sig->m_ip,
        NumberToString(sig->m_accId, 0),
        ((DB_PartyEntriesView*)sig)->m_nat0,
        ((DB_PartyEntriesView*)sig)->m_ping0,
        ((DB_PartyEntriesView*)sig)->m_mtu0,
        ((DB_PartyEntriesView*)sig)->m_nat1,
        ((DB_PartyEntriesView*)sig)->m_ping1,
        ((DB_PartyEntriesView*)sig)->m_mtu1,
        ((DB_PartyEntriesView*)sig)->m_nat2,
        ((DB_PartyEntriesView*)sig)->m_ping2,
        ((DB_PartyEntriesView*)sig)->m_mtu2,
        ((DB_PartyEntriesView*)sig)->m_nat3,
        ((DB_PartyEntriesView*)sig)->m_ping3,
        ((DB_PartyEntriesView*)sig)->m_mtu3,
        sig->m_field18,
        sig->m_field1c,
        buf[0], buf[1], buf[2], buf[3],
        (unsigned char)sig->m_field20);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x5964, 5);
        trace("DB_GatheringParyStatus failed");
    }
    return 1;
}

// ORIG 0x8420e1a：PVP 对战结果。idx 选择 m_tryEntries[idx]（10 字节行）：
// [0]/[1] byte（party_result/dungeon_id）、[2..5] byte 拼 peer_ip、
// [6..9] int（peer_nat_type）。relay_* 取 G_CEnvironment()->m_serverEnvir。
// [b28c-L19 SIGFIX] 返回 int→bool；残差 N76/N18a（装载序 ecx/edx + 常量 0x20 折叠位置）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08420e1a
#include "W8LT_DBTask/save_dnf_pvp_result_history.inc"
{
    char buf[0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    // ORIG 0x8421…：`lea (%ecx,%eax);add $0x20;mov 0xe(%eax)` 三段：
    // 4.4 对语句内联合表达式恒折叠单 add（两步拆分则指针落槽多 2 对），
    // 装载序（idx 先/sig 后）不可控 → N18a 残差 2 对，行为等价。
    sprintf(buf, "%d.%d.%d.%d",
            (unsigned char)sig->m_tryEntries[idx][2],
            (unsigned char)sig->m_tryEntries[idx][3],
            (unsigned char)sig->m_tryEntries[idx][4],
            (unsigned char)sig->m_tryEntries[idx][5]);
    db->set_query(
        "inSert into dnf_party_result_history_pvp (str_ip,nat_type,m_id,party_result,"
        "occ_time,map_id,dungeon_id,peer_nat_type,peer_ip, established_time, "
        "IsConnectedToRelaySrv, ConnectedTimeToRelaySrv, relay_ip, relay_udp_port, "
        "relay_tcp_port ) values('%s',%d,%s,%d,now(),%d,%d,%d,'%s',%d, %d, %d, "
        "'%s', %d, %d)",
        sig->m_ip,
        sig->m_natType,
        NumberToString(sig->m_accId, 0),
        (unsigned char)sig->m_tryEntries[idx][0],
        sig->m_field18,
        sig->m_field1c,
        (unsigned char)sig->m_tryEntries[idx][1],
        buf,
        // [r3-W8LT_DBTask] 已改：`(char*)sig + idx*10 + 0x20` 显式算术 +
        // DB_TryNatSection 单级成员 ⇒ 复现 ORIG 0x08420e1a `add $0x20,%eax;
        // mov 0xe(%eax)`（gap 1→0、ndiff 5→4、差集归零）。残余 = 装载序/常量
        // 结合顺序（N18a）。反例已记：改 `&sig->m_tryEntries[idx][0]` 反劣
        // （174→175、add:3/lea:1）。
        // [M8DBB 2026-09-11 负结论] 常量前置形 `sig + 0x20 + idx*10` 实测
        // **零变化**（174/175、diff blocks 17 全同，仅 `mov 0xc(%ebp),%ecx`
        // 与 `add $0x20` 的发射序不变）⇒ 回滚。残余归 N18a 结合序封顶。
        reinterpret_cast<const DB_TryNatSection*>(
            reinterpret_cast<const char*>(
                sig) + idx * 10 + 0x20)->m_f,
        (unsigned char)sig->m_field21,
        sig->m_field22,
        G_CEnvironment()->m_serverEnvir.m_relayIP,
        G_CEnvironment()->m_serverEnvir.m_relayUDPPort,
        G_CEnvironment()->m_serverEnvir.m_relayTCPPort);
    ok = db->exec(true);
    if (!ok)
    {
        // ORIG 0x8421…：buf 占远槽 -0x40、cMyTrace 临时槽 -0x30（近）。
        // 4.4 具名块对象把 buf 挤到 -0x30 ⇒ 改临时链式（技巧3）翻槽序。
        cMyTrace(__PRETTY_FUNCTION__, 0x5864, 5)(
            "DB_GatheringParyStatus failed");
    }
    return 1;
}

// ORIG 0x842156a：普通队伍对战结果（结构同 pvp 版，表 dnf_party_result_history）。
// [b28c-L19 SIGFIX] 返回 int→bool；残差同 pvp 版（N76/N18a）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0842156a
#include "W8LT_DBTask/save_dnf_party_result_history.inc"
{
    char buf[0x10];
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    bool ok;
    // [r3] 同 pvp 版：`+0x20` 显式算术（ORIG 0842156a `add $0x20,%eax;
    // mov 0xe(%eax)`）⇒ gap 1→0、ndiff 5→4；残余 = 装载序/常量结合序（N18a）。
    sprintf(buf, "%d.%d.%d.%d",
            (unsigned char)sig->m_tryEntries[idx][2],
            (unsigned char)sig->m_tryEntries[idx][3],
            (unsigned char)sig->m_tryEntries[idx][4],
            (unsigned char)sig->m_tryEntries[idx][5]);
    db->set_query(
        "inSert into dnf_party_result_history (str_ip,nat_type,m_id,party_result,"
        "occ_time,map_id,dungeon_id,peer_nat_type,peer_ip,established_time, "
        "IsConnectedToRelaySrv, ConnectedTimeToRelaySrv, relay_ip, relay_udp_port, "
        "relay_tcp_port) values('%s',%d,%s,%d,now(),%d,%d,%d,'%s',%d, %d, %d, "
        "'%s', %d, %d)",
        sig->m_ip,
        sig->m_natType,
        NumberToString(sig->m_accId, 0),
        (unsigned char)sig->m_tryEntries[idx][0],
        sig->m_field18,
        sig->m_field1c,
        (unsigned char)sig->m_tryEntries[idx][1],
        buf,
        reinterpret_cast<const DB_TryNatSection*>(
            reinterpret_cast<const char*>(
                sig) + idx * 10 + 0x20)->m_f,
        (unsigned char)sig->m_field21,
        sig->m_field22,
        G_CEnvironment()->m_serverEnvir.m_relayIP,
        G_CEnvironment()->m_serverEnvir.m_relayUDPPort,
        G_CEnvironment()->m_serverEnvir.m_relayTCPPort);
    ok = db->exec(true);
    if (!ok)
    {
        // ORIG：buf 远槽/trace 近槽 ⇒ 临时链式翻槽序（技巧3，同 pvp 版）。
        cMyTrace(__PRETTY_FUNCTION__, 0x590f, 5)(
            "DB_GatheringParyStatus failed");
    }
    return 1;
}

// ORIG 0x8421aea：空处理，恒返回 1（mov $1,%eax; ret）。
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

// makeRequest —— ORIG 0x08447bd0 逐指令转写：
// StreamPool::Acquire("DBThread.cpp", 0xb270) → CStreamGuard(stream,true)
//   → guard << 0x2ff → guard << -1
//   → sig = guard.GetInBuffer<SIG_SAVE_PC_ROOM_PLAY_TIME>()
//   → sig +0/+4/+8 = accId/totalTime/rewardCnt
//   → MsgQueueMgr::put((QUEUE_IDX)2, guard)（析构时入队）。
#include "W8LT_DBTask/makeRequest.inc"
    unsigned int accId, unsigned int totalTime, unsigned char rewardCnt)
{
    // ORIG 0x08447bd0：Acquire call 结果 %eax 直落 ctor 0x4(%esp) spill
    // （无 Stream* 中转槽）⇒ 嵌套实参一步（同 HadesPunish 单槽模式）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb270), true);
    // ORIG 0x08447bd0 每 << 前独立 `lea guard; call operator*`，返回值
    // %eax 直落 Stream::operator<< 的 this spill ⇒ 显式两步形。
    guard.operator*()->operator<<((int)0x2ff);
    guard.operator*()->operator<<((int)-1);
    SIG_SAVE_PC_ROOM_PLAY_TIME* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_PC_ROOM_PLAY_TIME>();
    sig->m_accId = accId;
    sig->m_totalTime = totalTime;
    sig->m_rewardCnt = rewardCnt;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}
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

// ORIG 0x084355ca sz 0xd7：Acquire("DBThread.cpp",0x7fa1) → CStreamGuard(stream,true)
//   → guard<<(int)0x164 → guard<<(int)-1 → guard<<(unsigned)accId
//   → MsgQueueMgr::put(DB_Q(2), guard)（析构时入队）。
#include "W8LT_DBTask/makeRequest_1.inc"
{
    // ORIG 无 stream 局部（Acquire 返回值直传 guard ctor），链式保持帧形态。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7fa1), true);
    // ORIG 每条 << 前均先 call CStreamGuard::operator*（0x080c8c46），逐指令对齐。
    guard.operator*()->operator<<((int)0x164);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0843b6ec sz 0xd6：Acquire("DBThread.cpp",0x8c75) → CStreamGuard(stream,true)
//   → guard<<(int)0x1f2 → guard<<(int)userIdx → guard<<(unsigned)accId
//   → MsgQueueMgr::put(DB_Q(2), guard)。
#include "W8LT_DBTask/makeRequest_2.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c75), true);
    guard.operator*()->operator<<((int)0x1f2);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0843b3b6 sz 0x335：*stream >> m_id(uint) → GetDBHandle(2,0) →
// NumberToString(m_id,0) → seLect capacity,money,cargo from account_cargo
// where m_id=%s → exec 失败返 0；get_n_rows()==0 返 1；fetch 失败返 0；
// Acquire("DBThread.cpp",0x8c53) → CStreamGuard(stream,true) → <<userIdx →
// <<slot → GetInBuffer<SIG_ACCOUNT_CARGO_DATA> → memset 0xd60 →
// get_uint(0,cap)/get_uint(1,money)/get_binary(2,buf,0x100f) 任一失败返 0 →
// get_binary_length(2) → outLen=0xd58 → uncompress_zip(sig+8,&outLen,buf,len)
// 失败返 0；outLen==0 || outLen%0xd58==0 → put(DISPATCH_Q,guard) 返 1，否则返 0。
// [b28d-4] DIFF：P222：块序/极性（ORIG je 落点 vs 4.4 jne）；ORIG 0843b3b6
#include "W8LT_DBTask/dispatch_2.inc"
{
    unsigned int mId = 0;
    MySQL* db;
    SIG_ACCOUNT_CARGO_DATA* sig;
    int binLen;
    unsigned int outLen;
    // [Q5W8LT 2026-09-11 负结论·回滚] 曾试删此初始化器（想消我方 41 条 4.4 inline
    // memset 展开），实测 our 189 / ORIG 225（gap +36 劣化）⇒ ORIG 0x0843b3c9 起
    // **确实**有清零（`lea -0x1043(%ebp),%edx; mov $0x100f,%ebx; ...; shr $0x2,%ecx;
    // rep stos %eax,%es:(%edi)` 共 49 条），只是 4.1 用 rep stos 而 4.4 用逐字节/字
    // 对齐循环（41 条，无 rep stos）⇒ 编译器 stringop 策略差，非源形可控。保留初始化器。
    char buf[0x100f] = { '\0' };
    static const int compressed_buff_size = 0x100f;  // ORIG _ZZ…DB_LoadAccountCargo…E20compressed_buff_size

    *stream >> mId;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect capacity, money, cargo from account_cargo where m_id = %s",
                  NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
        return 1;
    if (db->fetch() != 1)
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c53), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    sig = guard.operator->()->GetInBuffer<SIG_ACCOUNT_CARGO_DATA>();
    memset(sig, 0, 0xd60);
    if (db->get_uint(0, sig->m_capacity) != 1)
        return 0;
    if (db->get_uint(1, sig->m_money) != 1)
        return 0;
    if (db->get_binary(2, buf, compressed_buff_size) != 1)
        return 0;
    binLen = db->get_binary_length(2);
    outLen = 0xd58;
    if (uncompress_zip((char*)sig->m_cargo, outLen, buf,
                       static_cast<unsigned int>(binLen)) != 1)
        return 0;
    // [J4S1 P222] ORIG 0x0843b65c/0x0843b68c 双 `je <put 块>`、fallthrough = 失败块
    // ⇒ 原码是否定早返（put 块后置），写成正向包裹会产反极性 `jne`。
    if (outLen != 0 && outLen % 0xd58 != 0)
        return 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

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

// ORIG 0x0843b7c2 sz 0x183：*stream >> m_id(uint) → GetOutBuffer<
// SIG_ACCOUNT_CARGO_DATA> → buf[0x100f] 清零 → bufLen=0x100f →
// compress_zip(buf,&bufLen,sig+8,0xd58) 失败返 0 → GetDBHandle(2,0) →
// NumberToString(m_id,0) → blob_to_str(0,buf,bufLen) → upDate account_cargo
// set capacity=%u,money=%u,cargo='%s' where m_id=%s → 返回 exec(true)==1。
// [b28d-4] DIFF：P183/P171：ORIG 多 {'mov': 1, 'test': 1, 'je': 1, 'xor': 1, 'jmp': 1}（漏具名局部/去缓存不足）；ORIG 0843b7c2
#include "W8LT_DBTask/dispatch_3.inc"
{
    unsigned int mId = 0;
    SIG_ACCOUNT_CARGO_DATA* sig;
    MySQL* db;

    // ORIG 0x0843b7cd 语句序：mId=0 → >>mId → GetOutBuffer → buf 清零 →
    // bufLen=0x100f（声明序决定槽位；buf 清零在 stream 读之后）。
    *stream >> mId;
    sig = stream->GetOutBuffer<SIG_ACCOUNT_CARGO_DATA>();
    static const int compressed_buff_size = 0x100f;  // ORIG _ZZ…DB_SaveAccountCargo…E20compressed_buff_size
    char buf[0x100f] = { '\0' };
    unsigned int bufLen = compressed_buff_size;
    if (compress_zip(buf, bufLen, sig->m_cargo, 0xd58) != 1)
        return 0;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    // ORIG 0x0843b8e2：blob_to_str 无具名槽——实参内联进 set_query（P0/P89）。
    db->set_query("upDate account_cargo set capacity=%u, money=%u, cargo='%s' where m_id = %s",
                  sig->m_capacity, sig->m_money,
                  db->blob_to_str(0, buf, (int)bufLen), NumberToString(mId, 0));
    // ORIG 0x0843b928：`xor $1; test; je; mov $0/%1,%eax` ⇒ P101 早返形。
    if (!db->exec(true))
        return 0;
    return 1;
}

// ORIG 0x0815bee0 sz 0x153（_ZN25DBSelectCreateDnfUserInfo11makeRequestEij）：
// 先查 GlobalData::s_event_manager GetRepeatEvent(0xa0)，事件存在且其 vtable
// +0x34 虚槽（IsEventing 口径）返回真时跳过请求；Acquire(
// "localtaiwan/Dispatcher/Taiwan_DbThread.cpp",0x227) → CStreamGuard(stream,true)
// → Taiwan::internal_stream(guard,T(10),uid) → GetInBuffer<MSG>()，null 走
// cMyTrace("[Taiwan, Event] msg is null")，否则 *(uint*)msg(+0x00)=accId；
// MsgQueueMgr::put(DB_Q(2), guard)。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0815bee0
#include "W8LT_DBTask/makeRequest_3.inc"
{
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0xa0);
    // ORIG 0x0842a3??：enabled 无槽——%eax 物化（`mov $1/$0` 双常数岛 +
    // vcall 后 `xor $1,%eax; test`）。真虚调 IsEventing(vtable+0x34) 复现
    // `mov (%eax),%edx … call` 形（手写 SlotFn 三中转指针 = 多 4 槽 = 帧
    // 外推 16B；§4.3 裸偏移改具名虚函数，repo 先例 CConditionEventManager/
    // CBossPlay）。
    // ORIG 0x0815bee0：早退条件 = event==0 || !IsEventing()（`cmpl $0,-0x10; je
    // <mov $1岛>`；vcall 后 `xor $1; test; je <mov $0岛>`；`test %al; jne 尾`）
    // ⇒ P226/P316 语句表达式零槽短路物化 + 正极性早返（P59/P214①）。
    if (({ event == 0 || !event->IsEventing((CUser*)0); }))
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x227),
                       true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)10, userIdx);
    MSG* msg = guard->GetInBuffer<MSG>();
    if (msg == 0)
    {
        // 技巧3：临时链式（ORIG guard=-0x28/trace=-0x20，具名块对象把
        // trace 外推到 -0x28 顶移 guard）。
        cMyTrace(__PRETTY_FUNCTION__, 0x22c, 0)("[Taiwan, Event] msg is null");
    }
    else
    {
        // ORIG：*(uint*)msg = accId（MSG +0x00 account id 字段）
        msg->m_id = accId;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ORIG 0x0815c986 sz 0x10d（_ZN25DBSelectPcroomDailyReward11makeRequestEP5CUser）：
// Acquire("localtaiwan/Dispatcher/Taiwan_DbThread.cpp",0x2d9) → CStreamGuard
//   → Taiwan::internal_stream(guard, T(13), user->GetUID())
//     （实测 2026-08-30：ORIG 0x0815c9c8 实址 call _ZNK5CUser6GetUIDEv，非
//      get_acc_id；NEW 误用 get_acc_id 已修正为 GetUID。GetUID=slotIDX<<16|field704a6）
//   → GetInBuffer<MSG>()，null 走 cMyTrace("[Taiwan, Pcroom] msg is null")，
//   否则 *(uint*)msg(+0x00)=user->get_acc_id()；MsgQueueMgr::put(DB_Q(2), guard)。
#include "W8LT_DBTask/makeRequest_4.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x2d9),
                       true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)13,
                            (int)user->GetUID());
    MSG* msg = guard->GetInBuffer<MSG>();
    if (msg == 0)
    {
        // ORIG 0x0815c…：trace 临时对象落近槽 -0x1c、guard 保持 -0x24
        // （具名 trace 使 4.4 两槽互换，QueueSizeLog/CountOfHumanCertify
        // 同法 P37）。
        cMyTrace(__PRETTY_FUNCTION__, 0x2df, 0)(
            "[Taiwan, Pcroom] msg is null");
    }
    else
    {
        // ORIG：*(uint*)msg = accId（MSG +0x00 account id 字段）
        msg->m_id = user->get_acc_id();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}


namespace OS_API
{

// ORIG 0x0858c4fa：select(0,NULL,NULL,NULL,{sec=0,usec})。
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

// ORIG 0x083fbfc6（报告 docs/class_func_reports/DBTask/DBTask.md）
// [Q5W8LT 2026-09-11 符号名轴落地] 原为类外定义 `const int DBTask::DISPATCHER_COUNT;`
// （C++98 odr-use 保险）—— 实测该定义发射了**发明符号** `_ZN6DBTask16DISPATCHER_COUNTE`
// （R 4B），而 **ORIG 全 ELF 无任何含 `DISPATCHER_COUNT` 的符号**（`nm -S ORIG` 0 命中）。
// 该常量仅在两处作**值使用**（`for (i = 0; i <= DISPATCHER_COUNT - 1; ++i)`，`RegistDispatcher`
// 与 `DBTask::clear` 内），**无取址/引用绑定 ⇒ 非 odr-use**，故定义可删。
// 链接验证（Main 要求的前置）：扫 1429 个 `.o` 的 `nm -u` ⇒ **UND 引用 = 0**；
// 整库 `build/game/df_game_r` 中该符号仅 1 处 R（= 本 TU 定义被链接进去，非引用）。
// overlay 实测：删除后 OUR-only 1→0、四数 143/1771/9/48/705 逐项不变。
#include "W8LT_DBTask/DBTask.inc"
{
    RegistDispatcher();
}


// ORIG T _Z24put_compressed_blob_dataP5MySQLiPciPS1_ @0x083fbd06；定义在本文件
// DB_UpdateOneDayLetheSkill 块（0x2395 区），此处前向声明（与权威签名一致）。
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

// ORIG 0x8400432：ui_id<0 恒返 1；否则 UPDATE user_item 的 slot/clear_avatar_id。
// i386 cdecl 变参顺序：set_query(fmt, slot, clearAvatarId, ui_id)
// （ORIG 压栈 0x8=param3, 0xc=param4, 0x10=param2；SQL "%d,%d where ui_id=%d"）。
#include "W8LT_DBTask/ChangeSlot.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    bool ok;
    if (userIdx < 0)
        return 1;
    db->set_query("upDate user_items set slot=%d,clear_avatar_id=%d where ui_id=%d",
                  slot, clearAvatarId, userIdx);
    ok = db->exec(true);
    if (!ok)
        return 0;
    return 1;
}

// ORIG 0x8400388：GetOutBuffer<SIG_AVATAR_MOVE>（null 不检查，ORIG 直传）→
// MoveAvatarItem 恒返 1 → 显式 if/return 分支（ORIG xor/test/je 形态）。
#include "W8LT_DBTask/dispatch_4.inc"
{
    bool r;
    SIG_AVATAR_MOVE* sig;
    sig = stream->GetOutBuffer<SIG_AVATAR_MOVE>();
    r = MoveAvatarItem(sig);
    if (r != 1)
        return 0;
    return 1;
}

// ORIG 0x84003ca：两次 ChangeSlot —— (sig+0x04, sig[0xd], sig+0xe) 与
// (sig+0x08, sig[0xc], sig+0x12)；恒返 1（两次结果均忽略）。
// 视图字段访问（结构位移寻址 mov 0xe(%eax),%ecx）：char* 算术会让 gcc4.4
// 生成 add+mov，与 ORIG 单指令位移读不符；亦不可用中间指针变量（栈缓存）。
#include "W8LT_DBTask/MoveAvatarItem.inc"
{
    ChangeSlot(reinterpret_cast<DB_AvatarMoveView*>(sig)->m_uiId0,
               reinterpret_cast<DB_AvatarMoveView*>(sig)->m_slot0,
               reinterpret_cast<DB_AvatarMoveView*>(sig)->m_clearAvatarId0);
    ChangeSlot(reinterpret_cast<DB_AvatarMoveView*>(sig)->m_uiId1,
               reinterpret_cast<DB_AvatarMoveView*>(sig)->m_slot1,
               reinterpret_cast<DB_AvatarMoveView*>(sig)->m_clearAvatarId1);
    return 1;
}

// ORIG 0x83ffd46：恒返 1（mov $0x1,%eax; ret；参数未使用）。
#include "W8LT_DBTask/DelAvatarItem.inc"
{
    return 1;
}

// ORIG 0x83ffcac：基类 DBDispatcher::dispatch(userIdx, slot, stream) 前置检查
// 失败返 0；通过后 get_binary(buf, 0x70c) 失败返 0；DelAvatarItem 失败返 0，
// 成功返 1。
#include "W8LT_DBTask/dispatch_5.inc"
{
    bool r;
    char buf[0x70c];
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    r = stream->get_binary(buf, 0x70c);
    if (r != 1)
        return 0;
    r = DelAvatarItem(*reinterpret_cast<SIG_AVATAR_DELETE*>(buf));
    if (r != 1)
        return 0;
    return 1;
}

// ORIG 0x841a2f8：无基类前置。*stream >> m_id(uint) → >> ticketKey(int) →
// >> ticketId(char[32]) → GetDBHandle(3,0) → inSert user_cera_ticket
// （m_id 经 NumberToString(m_id,0)；insert exec 失败改 upDate，仍失败结果置 0）
// → Acquire("DBThread.cpp",0x4c6d) → CStreamGuard(stream,true) →
// <<userIdx → <<slot → <<结果(int) → <<ticketId(char*) → put(DISPATCH_Q) 返 1。
#include "W8LT_DBTask/dispatch_6.inc"
{
    static const int handleIdx = 3;  // ORIG _ZZN16DB_GenCeraTicket8dispatch…E9handleIdx
    int result = 0;
    MySQL* db;
    bool ok;
    unsigned int mId;
    char ticketId[32];

    *stream >> mId;
    *stream >> result;
    *stream >> ticketId;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into user_cera_ticket(m_id,ticket_id,ticketKey,expire_time,delete_flag,update_time) values(%s,'%s',%d,adddate(now(),interval %d hour), 0, now())",
        NumberToString(mId, 0), ticketId, result, 3);
    ok = db->exec(true);
    if (ok != 1)
    {
        db->set_query(
            "upDate user_cera_ticket set ticket_id='%s',ticketKey=%d,expire_time=adddate(now(),interval %d hour), delete_flag=%d, update_time=now() where m_id=%s",
            ticketId, result, 3, 0, NumberToString(mId, 0));
        ok = db->exec(true);
        if (ok != 1)
            result = 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4c6d), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator*()->operator<<(result);
    guard.operator*()->operator<<(ticketId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ORIG 0x843d286：GetDBHandle(8,0) → >> server_group(int) → >> charac_no(uint)
// → Acquire("DBThread.cpp",0x91bd) → CStreamGuard(stream,true) → <<0x219 →
// <<slot → sig=GetInBuffer<SIG_JOIN_GUILD_INFO>() → 查 guild_join_list：
// exec 失败返 0；get_n_rows()==0 → sig flag=0 → put(DISPATCH_Q) 返 1；
// 有行 → fetch + guild_id(0)/memo(1,0x1a) → 查 guild_info（guild_id）：
// exec 失败/无行返 1（不回包）；有行 → fetch + guild_name(0,0x17)/
// master_name(1,0x1e)/lev(2)/member_count(3) → sig flag=1 → put(DISPATCH_Q) 返 1。
#include "W8LT_DBTask/dispatch_7.inc"
{
    // ORIG 序：GetDBHandle 初始化语句在局部零初始化之前（gcc O0 按声明序）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    int serverGroup = 0;
    unsigned int characNo = 0;
    DB_JoinGuildInfoView* sig;

    *stream >> serverGroup;
    *stream >> characNo;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x91bd), true);
    guard.operator*()->operator<<((int)0x219);
    guard.operator*()->operator<<(slot);
    sig = reinterpret_cast<DB_JoinGuildInfoView*>(
        guard.operator->()->GetInBuffer<SIG_JOIN_GUILD_INFO>());
    db->set_query("seLect guild_id,memo from guild_join_list where charac_no=%d and server_group=%d",
                  characNo, serverGroup);
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
    {
        sig->m_flag = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    if (db->fetch() != 1)
        return 0;
    if (db->get_uint(0, sig->m_guildId) != 1)
        return 0;
    if (db->get_str(1, sig->m_memo, 0x1a) != 1)
        return 0;
    db->set_query("seLect guild_name,master_name,lev,member_count from guild_info where expire_flag=0 and guild_id=%d",
                  sig->m_guildId);
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
        return 1;
    if (db->fetch() != 1)
        return 0;
    if (db->get_str(0, sig->m_guildName, 0x17) != 1)
        return 0;
    if (db->get_str(1, sig->m_masterName, 0x1e) != 1)
        return 0;
    if (db->get_int(2, sig->m_lev) != 1)
        return 0;
    if (db->get_int(3, sig->m_memberCount) != 1)
        return 0;
        sig->m_flag = 1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ORIG 0x0843ccf2 sz 0x4a7：GetDBHandle(8,0) → Acquire("DBThread.cpp",0x9129)
// → CStreamGuard(stream,true) → <<0x217 → <<slot →
// GetInBuffer<SIG_REQUEST_JOIN_GUILD_DB_REPLY> → GetOutBuffer<
// SIG_REQUEST_JOIN_GUILD> → 查 guild_join_list(charac_no=+0x18, server_group=
// +0x20)：exec 失败返 0；有行 → reply flag=0x12 → put(DISPATCH_Q) 返 0；
// 无行 → 查 guild_info(server_id=+0x20, guild_name=+0x00)：exec 失败返 0；
// 无行 → reply flag=0x15 → put 返 1；有行 fetch 失败返 0 → guild_id(0)/
// guild_name(1,0x17)/master_name(2,0x1e)/lev(3)/member_count(4) 任一失败返 0
// → 查 guild_join_list(guild_id)：exec 失败返 0；行数>=0x32 → reply flag=0x5f
// → put 返 1；<0x32 → escape_string(memo 源 +0x27) → inSert(guild_id, m_id=
// NumberToString(+0x1c), server_group=+0x20, charac_no=+0x18, born_year=+0x24,
// memo) → exec 失败返 0；affected==0 → reply flag=2 → put 返 1；否则 reply
// flag=0 → put 返 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0843ccf2
#include "W8LT_DBTask/dispatch_8.inc"
{
    MySQL* db;
    // [Lane31W8LTDBTaskView] 语句序照 ORIG（0x0843ccf2）：GetDBHandle 先于
    // StreamPool::Acquire/CStreamGuard 构造（ORIG `call GetDBHandle; mov 0x940bd6c,%eax;
    // … call Acquire; call CStreamGuard::C1` 顺序实证）。
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9129), true);
    DB_RequestJoinGuildReplyView* reply;
    DB_RequestJoinGuildInView* sig;
    char escBuf[53];

    guard.operator*()->operator<<((int)0x217);
    guard.operator*()->operator<<(slot);
    reply = reinterpret_cast<DB_RequestJoinGuildReplyView*>(
        guard.operator->()->GetInBuffer<SIG_REQUEST_JOIN_GUILD_DB_REPLY>());
    sig = reinterpret_cast<DB_RequestJoinGuildInView*>(
        stream->GetOutBuffer<SIG_REQUEST_JOIN_GUILD>());
    // [Q5W8LT 2026-09-11 落地] ORIG 0x0843ccf2：死局部 `result`（全函数零引用）的
    // `movl $0x0,-0x1c(%ebp)` 发在 ORIG 第 45 条（GetOutBuffer 之后），而声明区版本
    // 落在第 22 条（guard 构造后）⇒ 把声明移到使用点（GetOutBuffer 之后）后
    // 本件 DIFF→**AE**（308=308 归一 + gap 0）。
    int result = 0;
    db->set_query("seLect * from guild_join_list where charac_no=%d and server_group=%d",
                  sig->m_characNo, sig->m_serverGroup);
    if (db->exec(true) != 1)
        return 0;
    // [P5S2W8LT r1] P196/P222 极性+块序：ORIG 0x0843ce05 `test %eax,%eax;
    //   setne %al; test %al,%al; je 0x0843ce3d`（正极性：有行 = fall-through，
    //   无行块在跳转目标 0x843ce3d），且 flag=0x12/return 0 块属**有行**分支
    //   （ORIG 0x843ce66 区 `movl $0x12,(%eax); …; mov $0x0,%ebx; jmp epilogue`）
    //   ⇒ 原 `if (get_n_rows() == 0) {无行}` 包裹形产 sete + 反向块序。
    if (db->get_n_rows() != 0)
    {
        reply->m_flag = 0x12;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 0;
    }
    {
        db->set_query("seLect guild_id,guild_name,master_name,lev,member_count from guild_info where server_id=%d and expire_flag=0 and guild_name='%s'",
                      sig->m_serverGroup, sig->m_guildName);
        if (db->exec(true) != 1)
            return 0;
        if (db->get_n_rows() == 0)
        {
            reply->m_flag = 0x15;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        if (db->fetch() != 1)
            return 0;
        if (db->get_uint(0, reply->m_guildId) != 1)
            return 0;
        if (db->get_str(1, reply->m_guildName, 0x17) != 1)
            return 0;
        if (db->get_str(2, reply->m_masterName, 0x1e) != 1)
            return 0;
        if (db->get_int(3, reply->m_lev) != 1)
            return 0;
        if (db->get_int(4, reply->m_memberCount) != 1)
            return 0;
        db->set_query("seLect * from guild_join_list where guild_id=%d",
                      reply->m_guildId);
        if (db->exec(true) != 1)
            return 0;
        // [A04-r5h · 轴 B/P196] ORIG 0x0843d026-0x0843d02e 为
        // `call get_n_rows; cmp $0x31,%eax; **seta** %al; test %al,%al; je 0x843d05f`
        // ⇒ 真形 = **正极性无符号** `get_n_rows() > 0x31`（then=0x5f 块落 fall-through、
        // body 在跳转目标 0x843d05f）。原 `get_n_rows() < 0x32`（负极性 + signed）使 4.4
        // 产 `setle` 且块序反 ⇒ 按 ORIG 改正极性比较（ORIG 为无符号序 ⇒ P196 前置通过）。
        // [J4S1 P2/P196] ORIG 0x0843d026 `cmp $0x31,%eax; seta %al`（无符号序）
        // ⇒ 需显式无符号视图，否则 4.4 产 setg。
        if ((unsigned int)db->get_n_rows() > 0x31u)
        {
            reply->m_flag = 0x5f;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        db->escape_string(escBuf, sig->m_memo);
        db->set_query("inSert into guild_join_list(guild_id,m_id,server_group,charac_no,born_year,memo,occ_time) values(%d,%s,%d,%d,'%s','%s',now())",
                      reply->m_guildId, NumberToString(sig->m_mId, 0),
                      sig->m_serverGroup, sig->m_characNo,
                      sig->m_bornYear, escBuf);
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            reply->m_flag = 2;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        reply->m_flag = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
}

// ORIG 0x84177da：GetOutBuffer<SIG_LOAD_SKILL> → SaveSkill 全绿返 1
// （无基类 dispatch 前置检查，与 DB_LoadTicket::dispatch 不同）。
#include "W8LT_DBTask/dispatch_9.inc"
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_LOAD_SKILL* sig;
    sig = stream->GetOutBuffer<SIG_LOAD_SKILL>();
    if (sig == 0)
        return 0;
    r = SaveSkill(sig);
    if (!r)
        return 0;
    return 1;
}

// NEAR 豁免：指令全同（125=125），仅 db/r 栈槽差 4 字节（ORIG gcc4.1 布局
// db@-0x1c/r@-0x1d；本工具链 gcc4.4 固定 db@-0x20/r@-0x19，6 种声明序穷举
// 均无法复现，属跨编译器栈布局差异，语义等价）。
#include "W8LT_DBTask/SaveSkill.inc"
{
    static const int handleIdx = 3;  // ORIG _ZZN12DB_SaveSkill9SaveSkill…E9handleIdx
    // 声明序 out1→out2→out3→r→db 对应 ORIG 栈槽 -0x24/-0x28/-0x2c/-0x1d/-0x1c
    // （4.4.7 -O0 同对齐组内首声明者靠高；证据 0x08417850/853/084179e6）。
    char* out1;
    char* out2;
    char* out3;
    bool r;
    MySQL* db;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    out1 = 0;
    if (!put_compressed_blob_data(db, 0, (char*)sig->m_skillSlotTree1, 0x198, &out1))
        return 0;
    out2 = 0;
    if (!put_compressed_blob_data(db, 1, (char*)sig->m_skillSlotTree2, 0x198, &out2))
        return 0;
    out3 = 0;
    if (!put_compressed_blob_data(db, 4, (char*)sig->m_commands, 0xc8, &out3))
        return 0;
    db->set_query(
        "upDate %s set remain_sp=%u,skill_slot='%s',remain_sp_2nd=%u, skill_slot_2nd='%s', remain_sfp_1st=%u, remain_sfp_2nd=%u, skill_command ='%s',script_version=%d where charac_no=%u",
        get_skill_table_name(sig->m_flag410),
        sig->m_remainSpTree1,
        out1,
        sig->m_remainSpTree2,
        out2,
        sig->m_remainSfpTree1,
        sig->m_remainSfpTree2,
        out3,
        sig->m_field411,
        ((DB_SaveSkillCharacView*)sig)->m_characNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8417a08：恒返 1（mov $0x1; ret，参数均未使用）。
#include "W8LT_DBTask/SendResult.inc"
{
    return 1;
}

// ---------------------------------------------------------------------------
// DB_LoadTicket —— ORIG 6 符号（报告 docs/class_func_reports/DB_LoadTicket/）
// C1 0x8419572 / D1 0x841958e / D0 0x84195be / dispatch 0x84195dc /
// LoadTicket 0x841968a / SendResult 0x8419770。
// ---------------------------------------------------------------------------

// [Lane31W8LTDBTaskView] +0x000 已由 Lane38 字段化至 SigTypes_C.h
// SIG_LOAD_TICKET（匿名 union `unsigned int m_characNo`@+0x000，static_assert
// offsetof==0），本地 DB_LoadTicketCharacView 视图删除，改用权威直访问。

// ORIG 0x84195dc：基类 dispatch 放行后 GetOutBuffer<SIG_LOAD_TICKET>；
// LoadTicket 失败置 m_field4=1（结果标志），再 SendResult，全绿返 1。
#include "W8LT_DBTask/dispatch_10.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_LOAD_TICKET* sig;
    sig = stream->GetOutBuffer<SIG_LOAD_TICKET>();
    if (sig == 0)
        return 0;
    if (!LoadTicket(sig))
        sig->m_field4 = 1;
    r = SendResult(userIdx, slot, sig);
    if (!r)
        return 0;
    return 1;
}

#include "W8LT_DBTask/LoadTicket.inc"
{
    static const int handleIdx = 3;  // ORIG _ZZN13DB_LoadTicket10LoadTicket…E9handleIdx
    // 声明序 i→ok→db 与 ORIG 栈槽对应（-0x14/-0xd/-0xc；gcc O0 按声明序分配）。
    int i;
    bool ok;
    MySQL* db;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query("seLect village from village_ticket where charac_no=%u",
                  sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
        return 0;
    i = 0;
    while (i <= 0x63)
    {
        ok = db->fetch();
        if (!ok)
            break;
        ok = db->get_short(0, sig->m_tickets[i]);
        if (!ok)
            return 0;
        ++i;
    }
    sig->m_count = i;
    return 1;
}

// ORIG 0x8419770：结果回包 —— Acquire("DBThread.cpp",0x4b17) →
// CStreamGuard(stream,true) → <<userIdx → <<slot → put_binary(sig,0x19c)
// → put(DISPATCH_Q,guard) 返 1。
#include "W8LT_DBTask/SendResult_1.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4b17), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator->()->put_binary(sig, 0x19c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

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

// ORIG 0x84028fc：基类 dispatch 放行后 GetOutBuffer<SIG_DELETE_CHANNEL>，
// 依次 DeleteChannel → SendResult，全绿返 1。
#include "W8LT_DBTask/dispatch_11.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_DELETE_CHANNEL* sig;
    sig = stream->GetOutBuffer<SIG_DELETE_CHANNEL>();
    if (sig == 0)
        return 0;
    r = DeleteChannel(sig);
    if (!r)
        return 0;
    r = SendResult(userIdx, slot, sig);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x84029ae：game_channel / channel_occ_info 两条删除；exec 成功路径
// cMyTrace(file,0x1262/0x126a,0) 记录，失败路径返 0。
// [b28c-27] N18a：GetDBHandle 第二实参 `(ENUM_SERVER_GROUP)view->m_field4` 的
// ORIG 单步 `mov 0x4(%eax),%edx` vs 4.4 恒 `mov 0x4(%eax),%eax; mov %eax,%edx`
// 中转；字段/形参异型化与 register 形均实测不可翻（batch-19 已封顶）。
// [b28d-4] AE：P254 enum 化翻 N18a 中转：视图 m_field4 int→ENUM_SERVER_GROUP + 删 cast ⇒ 复现 ORIG 0x84029ae mov 0x4(%eax),%edx 单段直落
#include "W8LT_DBTask/DeleteChannel.inc"
{
    // 技巧21 翻声明序：4.4 先声明占远槽；ORIG 0x84029ae r(-0xd) 比
    // db(-0xc) 远 ⇒ r 先声明。
    bool r;
    // 残留 1 处（[batch-19 复核 N18a 封顶，非 blocked]）：ORIG 0x84029ae
    // `mov 0x4(%eax),%edx` 单步直落 %edx；4.4 恒经 %eax 中转双 mov
    // （register asm("edx") 探针反产 4 步链，已回退）。语义等价：
    // 只是把同一个 uint 读经 %eax 再挪 %edx，无副作用差异。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_2, ((DB_DeleteChannelView*)sig)->m_field4);
    db->set_query("deLete from game_channel where gc_no=%d",
                  ((DB_DeleteChannelView*)sig)->m_gcNo);
    r = db->exec(true);
    if (!r)
        return 0;
    cMyTrace(__PRETTY_FUNCTION__, 0x1262, 0)(
        "deLete from game_channel where gc_no=%d", ((DB_DeleteChannelView*)sig)->m_gcNo);
    db->set_query("deLete from channel_occ_info where gc_no=%d",
                  ((DB_DeleteChannelView*)sig)->m_gcNo);
    r = db->exec(true);
    if (!r)
        return 0;
    cMyTrace(__PRETTY_FUNCTION__, 0x126a, 0)(
        "deLete from channel_occ_info where gc_no=%d", ((DB_DeleteChannelView*)sig)->m_gcNo);
    return 1;
}

// ORIG 0x8402aec：结果回包 —— Acquire("DBThread.cpp",0x1271) →
// CStreamGuard(stream,true) → <<userIdx → <<slot → put_binary(sig,0xc)
// → put(DISPATCH_Q,guard) 返 1。
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

// ORIG 0x84036b6：基类 dispatch 放行后 GetOutBuffer<SIG_INSERT_CHANNEL>，
// InsertChannel 通过后直接返 1（无 SendResult）。
#include "W8LT_DBTask/dispatch_12.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_INSERT_CHANNEL* sig;
    sig = stream->GetOutBuffer<SIG_INSERT_CHANNEL>();
    if (sig == 0)
        return 0;
    r = InsertChannel(sig);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8403734：按 (DB_HANDLE_2, sig->m_serverGroup) 取句柄；
// game_channel 插入失败转 update 同键行，仍失败返 0；随后向 channel_occ_info
// 批量灌 100 行 (gc_no, age 1..100, 0)，失败转 update occ_num 清零，仍失败返 0。
// [b28d-4 P0 清绑] 原 `register unsigned int v/w asm("eax")` 已删（N96 全域禁绑定）；
// ORIG 0x8403961/0x8403980 两段 `mov -0x1c,%eax; cmp $0x62/%0x63,%eax` 物化，
// 4.4 对 unsigned 槽比较恒融合 `cmpl $imm,-0x1c` ⇒ N18a/P209 封顶，语义等价。
#include "W8LT_DBTask/InsertChannel.inc"
{
    // 声明序 r→db 与 ORIG 栈槽对应（-0x21/-0x20；gcc O0 按声明序分配）。
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_2, ((DB_InsertChannelView*)sig)->m_serverGroup);
    db->set_query(
        "inSert into game_channel (gc_no,gc_now,gc_ip,gc_port,gc_max,gc_game,gc_channel,gc_ch_group,gc_channeltype,gc_up_time, gc_type) values(%d,0,'%s',%d,%d,%d,'%s',%d,'%s',now(),%d)",
        ((DB_InsertChannelView*)sig)->m_gcNo,
        ((DB_InsertChannelView*)sig)->m_gcIp,
        ((DB_InsertChannelView*)sig)->m_gcPort,
        ((DB_InsertChannelView*)sig)->m_gcMax,
        ((DB_InsertChannelView*)sig)->m_gcGame,
        ((DB_InsertChannelView*)sig)->m_gcChannel,
        ((DB_InsertChannelView*)sig)->m_gcChGroup,
        ((DB_InsertChannelView*)sig)->m_gcChanneltype,
        ((DB_InsertChannelView*)sig)->m_gcType);
    r = db->exec(true);
    if (!r)
    {
        db->set_query(
            "upDate game_channel set gc_now=0,gc_ip='%s',gc_port=%d,gc_max=%d,gc_game=%d,gc_channel='%s',gc_ch_group=%d,gc_channeltype='%s',gc_up_time=now(),gc_type=%d where gc_no=%d",
            ((DB_InsertChannelView*)sig)->m_gcIp,
            ((DB_InsertChannelView*)sig)->m_gcPort,
            ((DB_InsertChannelView*)sig)->m_gcMax,
            ((DB_InsertChannelView*)sig)->m_gcGame,
            ((DB_InsertChannelView*)sig)->m_gcChannel,
            ((DB_InsertChannelView*)sig)->m_gcChGroup,
            ((DB_InsertChannelView*)sig)->m_gcChanneltype,
            ((DB_InsertChannelView*)sig)->m_gcType,
            ((DB_InsertChannelView*)sig)->m_gcNo);
        r = db->exec(true);
        if (!r)
            return 0;
    }
    std::string str;
    str.clear();
    // ORIG 0x8403961-0x8403986 `mov -0x1c,%eax; cmp $0x62/%0x63,%eax; ja/setbe`
    // 两段物化；4.4 对 unsigned 槽比较恒融合 `cmpl $imm,-0x1c`。
    // [b28d-solo-2 P310] 局部改 int + 显式 (unsigned int) 比较即逐条复现两段形。
    int i;
    for (i = 0; (unsigned int)i <= 99u; ++i)
    {
        char buf[0x64] = { '\0' };
        sprintf(buf, "(%d, %d, 0)", ((DB_InsertChannelView*)sig)->m_gcNo, i + 1);
        str.append(buf);
        if ((unsigned int)i < 99u)
            str.append(", ");
    }
    db->set_query("inSert into channel_occ_info (gc_no, age, occ_num) values %s",
                  str.c_str());
    r = db->exec(true);
    if (!r)
    {
        db->set_query("upDate channel_occ_info set occ_num = 0 where gc_no = %d",
                      ((DB_InsertChannelView*)sig)->m_gcNo);
        r = db->exec(true);
        if (!r)
            return 0;
    }
    return 1;
}

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

// ORIG 0x8403ab8：无基类 dispatch 前置检查（同 DB_SaveSkill），
// GetOutBuffer<SIG_INSERT_TICKET> → InsertTicket 通过返 1。
#include "W8LT_DBTask/dispatch_13.inc"
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_INSERT_TICKET* sig;
    sig = stream->GetOutBuffer<SIG_INSERT_TICKET>();
    if (sig == 0)
        return 0;
    r = InsertTicket(sig);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8403b08：village_ticket 单条 inSert，失败返 0。
#include "W8LT_DBTask/InsertTicket.inc"
{
    // 声明序 r→db 对应 ORIG 栈槽 -0xd/-0xc（gcc O0 首局部占最低槽）。
    bool r;
    static const int handleIdx = 3;  // ORIG _ZZN15DB_InsertTicket12InsertTicket…E9handleIdx
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query("inSert village_ticket values(%u,%d)",
                  ((DB_InsertTicketView*)sig)->m_village,
                  ((DB_InsertTicketView*)sig)->m_field4);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// DB_LoadDungeonClear —— ORIG 8 符号（报告 docs/class_func_reports/DB_LoadDungeonClear/）
// C1 0x8419852 / D1 0x841986e / D0 0x841989e / dispatch 0x84198bc /
// AradLoadMemberDungeonClear 0x841996e / AradLoadDungeonClear 0x8419a76 /
// LoadDungeonClear 0x8419db4 / SendResult 0x8419f0a。
// ---------------------------------------------------------------------------

// ORIG 0x84198bc：基类 dispatch 放行后 GetOutBuffer<SIG_LOAD_DUNGEON_CLEAR>，
// 依次 LoadDungeonClear → SendResult，全绿返 1。
#include "W8LT_DBTask/dispatch_14.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→sig 对应 ORIG 栈槽 -0xd/-0xc（gcc O0 首局部占最低槽）。
    bool r;
    SIG_LOAD_DUNGEON_CLEAR* sig = stream->GetOutBuffer<SIG_LOAD_DUNGEON_CLEAR>();
    if (sig == 0)
        return 0;
    r = LoadDungeonClear(sig);
    if (!r)
        return 0;
    r = SendResult(userIdx, slot, sig);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8419db4：member_dungeon 表按 m_id 取单行 dungeon 串（%s+NumberToString），
// 成功行数必须恰为 1；取回后 m_selected 清零返 1。
#include "W8LT_DBTask/LoadDungeonClear.inc"
{
    sig->m_selected = 1;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect dungeon from member_dungeon where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() != 1)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x4bbc,
                              "Dungeon count error : m_id(%s) dungeon count(%d)",
                              NumberToString(sig->m_accId, 0), db->get_n_rows());
        return 0;
    }
    if (!db->fetch())
        return 0;
    if (!db->get_str(0, sig->m_clearData, 0x17ff))
        return 0;
    sig->m_selected = 0;
    return 1;
}

// ORIG 0x841996e：member_dungeon 表按 m_id 取单行（%u 直传），行数>0 且
// dungeon 串非空才置 m_pad8[0]=0、m_selected=0 返 1。
#include "W8LT_DBTask/AradLoadMemberDungeonClear.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect dungeon from member_dungeon where m_id=%u", sig->m_accId);
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() == 0)
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_str(0, sig->m_clearData, 0x17ff))
        return 0;
    size_t len = strlen(sig->m_clearData);
    if (len == 0)
        return 0;
    sig->m_pad8[0] = 0;
    sig->m_selected = 0;
    return 1;
}

// ORIG 0x8419a76：member 加载失败则回退全量：charac_info JOIN charac_dungeon
// 取全部 dungeon 串，逐行拼入 m_clearData（逗号分隔），总长超 0x1800 即错。
// N22 封顶（ndiff1）：`cmp %eax,%ebx`（ORIG，dst=局部）vs `cmp %ebx,%eax`（4.4
// 恒 dst=call 结果方）；其余为跳表 R_386_32 重定位归一项。NL6 2026-09-08。
// [b28 判词 N22] 尾环 `cmp %eax,%ebx`(dst=局部) vs 4.4 恒 `cmp %ebx,%eax`(dst=call
// 结果)——负例表在册，NEAR 2 封顶。
// [b28d-solo-2R] NEAR(1) 复核：`curI == rows-1` / `rows-1 == curI` / `curI != rows-1`
// 三形均产 `cmp %ebx,%eax`（4.4 恒以 call 结果作 dst）⇒ N22 封顶。
// [b28d-solo-4] NEAR(1)→AE：N22 翻案（P302/P319 族）——比较两侧 `(unsigned int)`
// 化后 4.4 产 ORIG 的 `cmp %eax,%ebx`（dst=curI），末条差异归零。ORIG 08419a76
#include "W8LT_DBTask/AradLoadDungeonClear.inc"
{
    if (AradLoadMemberDungeonClear(sig))
        return 1;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("select B.dungeon from charac_info A Inner Join charac_dungeon B where A.m_id = %d and B.charac_no = A.charac_no and A.delete_flag = 0",
                  sig->m_accId);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x4b72,
                              "Dungeon share query error : m_id(%d) dungeon count(%d)",
                              sig->m_accId, db->get_n_rows());
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x4b79,
                              "Dungeon count error : m_id(%d) dungeon count(%d)",
                              sig->m_accId, db->get_n_rows());
        return 1;
    }
    char buf[428] = { '\0' };
    char* dst = sig->m_clearData;
    int i = 0;
    while (i < db->get_n_rows())
    {
        if (!db->fetch())
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x4b89,
                                  "Dungeon fetch error : m_id(%d) dungeon count(%d)",
                                  sig->m_accId, db->get_n_rows());
            return 0;
        }
        if (!db->get_str(0, buf, 0x1ab))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x4b8f,
                                  "Dungeon Parsing error : m_id(%d) dungeon count(%d)",
                                  sig->m_accId, db->get_n_rows());
            return 0;
        }
        if (strlen(sig->m_clearData) + strlen(buf) > 0x1800)
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x4b95,
                                  "Dungeon Total Data Error : m_id(%d) dungeon count(%d)",
                                  sig->m_accId, db->get_n_rows());
            return 0;
        }
        memcpy(dst, buf, 0x1ac);
        dst += strlen(dst);
        {
            // ORIG 0x08419…164：`mov -0xc,%ebx` 前置缓存 i，跨 get_n_rows
            // 调用后 `cmp %eax,%ebx`——块作用域 register 局部直落 %ebx。
            register int curI = i;
            // [b28d-solo-4] N22 翻案（P302/P319 族）：比较两侧 unsigned 化后
            // 4.4 产 `cmp %eax,%ebx`（dst=curI，与 ORIG 0x08419d64 全等）；
            // 纯 int 形恒产 `cmp %ebx,%eax`（dst=call 结果）。!= 对符号不敏感，
            // curI>=0 且 rows==0 时两形均判不等 ⇒ 语义等价。
            if ((unsigned int)curI != (unsigned int)db->get_n_rows() - 1)
            {
                *dst = ',';
                ++dst;
            }
        }
        ++i;
    }
    sig->m_pad8[0] = 1;
    sig->m_selected = 0;
    return 1;
}

// ORIG 0x8419f0a：结果回包 —— Acquire("DBThread.cpp",0x4bca) →
// CStreamGuard(stream,true) → <<userIdx → <<slot → put_binary(sig,0x1810)
// → put(DISPATCH_Q,guard) 返 1。
#include "W8LT_DBTask/SendResult_3.inc"
                                     SIG_LOAD_DUNGEON_CLEAR* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4bca), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator->()->put_binary(sig, 0x1810);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

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

// ORIG 0x8418442：login_common 表按 m_id 更新会员奖励疲劳；无行受影响时
// 回退 inSert 新行（update_time=now()），返 !exec(inSert)；已更新路径返 0
//（ORIG +0x97 test %al/je +0xe5 汇合，eax=v=false 直落出口）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08418442
// [b28d-solo-2R] DIFF(6)：`!db->exec(true)` / `exec()==false` 两形 4.4 均产
// `xor$1; jmp; mov$0` 岛（+1 条），ORIG 4.1 为 `xor$1; test %al,%al` dead-test 直落
// epilogue ⇒ N122/跨版本 bool 返回封顶（P168a③）。ORIG 0x8418442
#include "W8LT_DBTask/UpdateAccountMemberBonusFatigue.inc"
                                                      short memberBonusFatigue)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query("upDate login_common set member_bonus_fatigue=%d where m_id=%s",
                  memberBonusFatigue, NumberToString(accId, 0));
    // ORIG +0x97 `test %al,%al; je +0xe5`：v=!exec||rows==0 choose 汇聚于
    // eax（mov$1/mov$0 无槽），false 路径（更新成功）先落 mov$0 块，
    // body 尾 `xor$1;test` 直落 epilogue。
    // [b28c-L18 spec，L19 代落并实测] P222 正向包裹形（if(exec==1&&rows!=0)return false）
    // 反劣：缺 ORIG 的 `xor $1` 段且仍产 mov$0 岛 ⇒ 维持原形，记 N3/N93附。
    // [b28d-solo-4] 早返形（if(ok)return false; body; return !exec）实测同 DIFF：
    // 4.4 恒把 return false 的 mov$0 块放 body 之后（body 尾多 jmp），ORIG 把它
    // 并入 choose 汇聚 +0x92（body 尾 xor$1;test 直落）⇒ N93附 尾块发射序封顶。
    // [r3 负结论] 尾部 `return false;` 改显式 `else { return false; }` 包裹（P222）
    // 实测**零变化**（62/63、ndiff 3、ORIG-only{test:1}/OURS-only{jmp:1,mov:1} 全同）
    // ⇒ 回滚，维持 N93附 封顶。
    // [P5S2W8LT r2 负结论·已回滚] v1 正极性正文形实测**劣化**：blocks 1→4
    // （新增两处 `jne/je` 镜像 + 尾块仍 `jmp/mov$0`）、ours 63→62 但差异块
    // 反增 ⇒ 维持原负极性形，N93附 封顶（ORIG 0x084184d9 `test %al,%al;
    // je +0xe5` 的 ver 值由 4.1 choose 汇聚，4.4 不可复现）。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query("inSert into login_common(m_id,member_bonus_fatigue,update_time) values(%s,%d,unix_timestamp(now()))",
                      NumberToString(accId, 0), memberBonusFatigue);
        // ORIG +0xe0-0xe3 `xor $1,%eax; test %al,%al`（dead test 直落 epilogue，
        // 0x08418522-0x08418525）⇒ P22/P168a③ `!exec` 直返（^true 多 jmp+mov$0）。
        return !db->exec(true);
    }
    return false;
}

// ORIG 0x841852e：结果回包 —— Acquire("DBThread.cpp",0x48e2) →
// CStreamGuard(stream,true) → <<userIdx → <<-1 → put_binary(sig,0x8b)
// → put(DISPATCH_Q,guard) 返 1（slot 实参未使用）。
#include "W8LT_DBTask/SendResult_4.inc"
                                 SIG_UPDATE_LOGOUT* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x48e2), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<((int)-1);
    guard.operator->()->put_binary(sig, 0x8b);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ORIG 0x8418a96：accId==0 直返 1；serverId 取 IsIntegratedPvPBaseChannel ?
// sig+0xe : Server_Envir::m_serverGroup；login_account_%d 表写
// channel_no=+0x8、login_status=0、last_login_date=now()；exec 失败返 0。
// [b28c-L18 判词 N7 寄存器轮换（0xc/0x8 esp 槽值集与 ORIG 全等），L19 代落注释]
// [b28d-solo-4] DIFF→NEAR：P316 volatile 声明 serverId 强制其读先于 m_gcNo
//   （ORIG 4.1 左→右：`mov -0x10,%edx; mov 0x8(%edx),%ecx`；4.4 右→左反序）。
//   现助记符序列与 ORIG 全等（m_gcNo→%ecx、serverId→%edx 角色也一致）；
//   余 serverId 槽 -0x14 vs ORIG -0x10（bool r -0xd vs -0x11）= N82/P112 槽号封顶。
// ORIG 08418a96
#include "W8LT_DBTask/UpdateLogout.inc"
{
    if (((DB_UpdateLogoutSigView*)sig)->m_accId == 0)
        return 1;
    // [b28d-solo-4] P316 volatile 旋钮：ORIG 4.1 先装载 serverId（-0x10→%edx）
    // 再算 m_gcNo（→%ecx）；4.4 右→左恒反序。volatile 声明强制该读先行。
    // [L4-r1 判据⑩] 槽序反转律：ORIG 声明序 = bool r → int serverId → ptr db
    //（槽 r@-0x11 / serverId@-0x10 / db@-0xc）。
    // [H4 2026-09-11 复核] 实测把 serverId 提到 r 之前**槽号完全不变**
    //（r 仍 -0xd / serverId 仍 -0x14）⇒ 4.4 按尺寸分组+首次使用序分配、
    // 声明序不可翻（N98/P331 混合尺寸区）⇒ 回滚保原序，判 N82 槽号封顶。
    // [M8DBA r5] 追加一试：`volatile bool r;`（补 volatile 促其提前分配）
    // 读数 58/58 NEAR 不变、槽仍 r@-0xd / serverId@-0x14 ⇒ 无效，回滚。
    bool r;
    // [W8LT-lane 2026-09-11 A/B 定论] 去 volatile 实测 **NEAR→DIFF 劣化**
    //（ndiff 5→2 但助记符序列变，verdict 降档）⇒ 回滚；volatile 是 P316
    // 求值序（ORIG 先 serverId 后 m_gcNo）的必要载体，槽号差（serverId
    // -0x14 vs ORIG -0x10）与 r(-0xd vs -0x11) 维持 N82 槽图封顶。
    volatile int serverId = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverId = ((DB_UpdateLogoutSigView*)sig)->m_serverId;
    else
        serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("upDate login_account_%d set m_channel_no=%d,login_status=%d,last_login_date=now() where m_id=%s",
                  serverId, ((DB_UpdateLogoutSigView*)sig)->m_gcNo, 0,
                  NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0));
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8418612：GetOutBuffer<SIG_UPDATE_LOGOUT> → 填 serverId（PvP 基频道
// 取 GetServerType(userIdx)，否则 Server_Envir::m_serverGroup）→
// removeUserDBInfo(userIdx) → UpdateLogout 失败记日志返 0；+0x12 置位时
// 按 +0x22(pcbang) 选 SQL 更新 member_play_info（当日行）并 exec；无行时
// inSert 新行（exec 失败返 0）；+0x88 置位补会员疲劳；最后 SendResult。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08418612
// [r3 负结论] 三处 `m_puUser ? 0 : 1`（ORIG 08418793/08418874/0841896c 形
//   `test %al,%al; je L1; mov $0x0,%ebx; jmp L2; L1: mov $0x1,%ebx`）改
//   `if/else` 赋函数级具名 `int puFlag` 实测**反劣**：our 317→326、ndiff
//   96→345（4.4 对具名 int 恒发栈槽往返，ORIG 用 %ebx 寄存器保活）⇒ 回滚。
//   该三处属 4.1 分支赋值 vs 4.4 setcc 常量三元，判 N 封顶。
#include "W8LT_DBTask/dispatch_15.inc"
{
    // [L4-r2 判据⑩ 一次性试验] ORIG ok(r)@-0x21 深于 sig@-0x20（4.1 声明序），
    // 我方 r@-0x1d 浅于 sig@-0x24（4.4 首次使用序）⇒ 试把 r 的声明前置。
    bool r;
    SIG_UPDATE_LOGOUT* sig = stream->GetOutBuffer<SIG_UPDATE_LOGOUT>();
    if (sig == 0)
        return 0;
    // [Lane31W8LTDBTaskView] 视图 cast 反模式：`((View*)sig)->f=v` 使 4.4 走
    // base-first（base 提前入 callee-saved 跨 call）；权威头 SIG_UPDATE_LOGOUT
    // 具名字段直访问复现 ORIG RHS-first（0x0841865c `mov 0x10(%ebp),%edx` …
    // 0x0841866e `mov -0x20(%ebp),%edx; mov %eax,0xe(%edx)`）。
    sig->m_serverId = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        sig->m_serverId = GlobalData::s_db_mgr->GetServerType(slot);
    else
        sig->m_serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    GlobalData::s_db_mgr->removeUserDBInfo(slot);
    r = UpdateLogout(sig);
    if (!r)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x4939,
                              "UpdateLogout() fail - m_id[%s]\n",
                              NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0));
        return 0;
    }
    if (((DB_UpdateLogoutSigView*)sig)->m_stateFlag)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
        if (((DB_UpdateLogoutSigView*)sig)->m_pcbang)
        {
            db->set_query("upDate member_play_info set play_time=play_time+%d,play_count=play_count+1,trade_cnt=trade_cnt+%d, exp=exp+%u, used_fatigue=used_fatigue+%u, ting_count=ting_count+%d, ip='%s', last_play_time=%u, pcbang_flag=%d, end_ip='%s', mac_addr='%s', server_id = %d where occ_date=cast(now() as date) and m_id=%s",
                          ((DB_UpdateLogoutSigView*)sig)->m_playTime,
                          ((DB_UpdateLogoutSigView*)sig)->m_field17,
                          ((DB_UpdateLogoutSigView*)sig)->m_field1b,
                          ((DB_UpdateLogoutSigView*)sig)->m_field1f,
                          ((DB_UpdateLogoutSigView*)sig)->m_puUser ? 0 : 1,
                          ((DB_UpdateLogoutSigView*)sig)->m_field37,
                          ((DB_UpdateLogoutSigView*)sig)->m_field23,
                          ((DB_UpdateLogoutSigView*)sig)->m_pcbang,
                          ((DB_UpdateLogoutSigView*)sig)->m_field43,
                          ((DB_UpdateLogoutSigView*)sig)->m_mac,
                          ((DB_UpdateLogoutSigView*)sig)->m_serverId,
                          NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0));
        }
        else
        {
            db->set_query("upDate member_play_info set play_time=play_time+%d,play_count=play_count+1,trade_cnt=trade_cnt+%d, exp=exp+%u, used_fatigue=used_fatigue+%u, ting_count=ting_count+%d, ip='%s', last_play_time=%u, end_ip='%s', mac_addr='%s', server_id = %d where occ_date=cast(now() as date) and m_id=%s",
                          ((DB_UpdateLogoutSigView*)sig)->m_playTime,
                          ((DB_UpdateLogoutSigView*)sig)->m_field17,
                          ((DB_UpdateLogoutSigView*)sig)->m_field1b,
                          ((DB_UpdateLogoutSigView*)sig)->m_field1f,
                          ((DB_UpdateLogoutSigView*)sig)->m_puUser ? 0 : 1,
                          ((DB_UpdateLogoutSigView*)sig)->m_field37,
                          ((DB_UpdateLogoutSigView*)sig)->m_field23,
                          ((DB_UpdateLogoutSigView*)sig)->m_field43,
                          ((DB_UpdateLogoutSigView*)sig)->m_mac,
                          ((DB_UpdateLogoutSigView*)sig)->m_serverId,
                          NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0));
        }
        if (((DB_UpdateLogoutSigView*)sig)->m_accId != 0)
        {
            if (!db->exec(true) || db->getAffectedRowCount() == 0)
            {
                db->set_query("inSert into member_play_info(occ_date,m_id,play_time,play_count,trade_cnt,exp,used_fatigue,ting_count, ip, last_play_time, pcbang_flag, end_ip, mac_addr, server_id) values(cast(now() as date),%s,%d,1,%d,%u,%u,%d,'%s', %u, %d, '%s', '%s',%d)",
                              NumberToString(((DB_UpdateLogoutSigView*)sig)->m_accId, 0),
                              ((DB_UpdateLogoutSigView*)sig)->m_playTime,
                              ((DB_UpdateLogoutSigView*)sig)->m_field17,
                              ((DB_UpdateLogoutSigView*)sig)->m_field1b,
                              ((DB_UpdateLogoutSigView*)sig)->m_field1f,
                              ((DB_UpdateLogoutSigView*)sig)->m_puUser ? 0 : 1,
                              ((DB_UpdateLogoutSigView*)sig)->m_field37,
                              ((DB_UpdateLogoutSigView*)sig)->m_field23,
                              ((DB_UpdateLogoutSigView*)sig)->m_pcbang,
                              ((DB_UpdateLogoutSigView*)sig)->m_field43,
                              ((DB_UpdateLogoutSigView*)sig)->m_mac,
                              ((DB_UpdateLogoutSigView*)sig)->m_serverId);
                if (!db->exec(true))
                    return 0;
            }
        }
    }
    if (((DB_UpdateLogoutSigView*)sig)->m_field88)
        UpdateAccountMemberBonusFatigue(((DB_UpdateLogoutSigView*)sig)->m_accId,
                                        ((DB_UpdateLogoutSigView*)sig)->m_field89);
    return SendResult(userIdx, slot, sig);
}

// ---------------------------------------------------------------------------
// RegistDispatcher —— ORIG 0x083fbfee（0x3168 字节逐指令转写：
// 先把 852 个槽清零，再按消息 id 顺序 new 各 dispatcher 并写入对应槽；
// 槽下标 = (ORIG store 偏移 - 4)/4，逐块对照报告 ## ORIG 汇编 store 指令；
// 函数体整体处于 try/catch(...)（Ghidra CatchHandler @0x083ff0dd，尾部
// mov $0x1,%ebx 正常返回、catch 内 cMyTrace(__PRETTY_FUNCTION__,0x8b2,5)
// ("[%s][%d]", __PRETTY_FUNCTION__, 0x8b2) 后返 0 → 返回值 int）。
// ---------------------------------------------------------------------------
#include "W8LT_DBTask/RegistDispatcher.inc"
{
    for (int i = 0; i <= DISPATCHER_COUNT - 1; ++i)     // ORIG cmpl $0x353/setle
    {
        m_dispatchers[i] = 0;
    }
    try {

        m_dispatchers[3] = new DB_Login();                                       // ORIG C1 0x840f48e
        m_dispatchers[4] = new DB_CreateCharac();                                // ORIG C1 0x8400cc2
        m_dispatchers[5] = new DB_DeleteCharac();                                // ORIG C1 0x8402bce
        m_dispatchers[6] = new DB_UpdateCharacView();                            // ORIG C1 0x844d85c
        m_dispatchers[7] = new DB_LoadCharac();                                  // ORIG C1 0x844d8c6
        m_dispatchers[8] = new DB_LoadInventory();                               // ORIG C1 0x840a604
        m_dispatchers[10] = new DB_LoadQuest();                                  // ORIG C1 0x844d930
        m_dispatchers[17] = new APSystem::DB_LoadActionPoint();                  // ORIG C1 0x844dd7a
        m_dispatchers[19] = new APSystem::DB_UpdateActionPoint();                // ORIG C1 0x844dd96
        m_dispatchers[20] = new APSystem::DB_LoadRewardMedal();                  // ORIG C1 0x844ddb2
        m_dispatchers[21] = new APSystem::DB_UpdateRewardMedal();                // ORIG C1 0x844ddce
        m_dispatchers[32] = new DB_LoadEtc();                                    // ORIG C1 0x8403b84
        m_dispatchers[34] = new DB_SaveCharac();                                 // ORIG C1 0x8415c2e
        m_dispatchers[35] = new DB_SaveInven();                                  // ORIG C1 0x8416a42
        m_dispatchers[36] = new DB_SaveSkill();                                  // ORIG C1 0x8417770
        m_dispatchers[37] = new DB_SavePvp();                                    // ORIG C1 0x841720e
        m_dispatchers[41] = new DB_UpdateQuest();                                // ORIG C1 0x8418b74
        m_dispatchers[42] = new DB_UpdateGrowType();                             // ORIG C1 0x8418130
        m_dispatchers[43] = new DB_UpdateLogin();                                // ORIG C1 0x844da04
        m_dispatchers[44] = new DB_UpdateLogout();                               // ORIG C1 0x84183d8
        m_dispatchers[46] = new DB_UpdateChannel();                              // ORIG C1 0x8417f4a
        m_dispatchers[47] = new DB_DeleteChannel();                              // ORIG C1 0x8402892
        m_dispatchers[45] = new DB_InsertChannel();                              // ORIG C1 0x840364c
        m_dispatchers[49] = new DB_AvatarItemInsert();                           // ORIG C1 0x83ffd50
        m_dispatchers[51] = new DB_AvatarItemDel();                              // ORIG C1 0x83ffc42
        m_dispatchers[50] = new DB_AvatarItemMove();                             // ORIG C1 0x840031e
        m_dispatchers[52] = new DB_AvatarExtendPeriod();                         // ORIG C1 0x83ffab8
        m_dispatchers[53] = new DB_AvatarChangeOwner();                          // ORIG C1 0x844d7f2
        m_dispatchers[54] = new DB_AvatarChangeStat();                           // ORIG C1 0x844e0aa
        m_dispatchers[38] = new DB_SaveRank();                                   // ORIG C1 0x844d99a
        m_dispatchers[40] = new DB_InsertTicket();                               // ORIG C1 0x8403a4e
        m_dispatchers[11] = new DB_LoadTicket();                                 // ORIG C1 0x8419572
        m_dispatchers[12] = new DB_LoadDungeonClear();                           // ORIG C1 0x8419852
        m_dispatchers[39] = new DB_SaveDungeonClear();                           // ORIG C1 0x8419fec
        m_dispatchers[63] = new DB_TerminateServer();                            // ORIG C1 0x841a284
        m_dispatchers[55] = new DB_StatBreakJar();                               // ORIG C1 0x8417a12
        m_dispatchers[56] = new DB_StatComposition();                            // ORIG C1 0x8417b82
        m_dispatchers[57] = new DB_StatCoinUse();                                // ORIG C1 0x8417b04
        m_dispatchers[58] = new DB_StatPvpPlay();                                // ORIG C1 0x8417e58
        m_dispatchers[59] = new DB_StatGameRank();                               // ORIG C1 0x8417d66
        m_dispatchers[60] = new DB_StatDungeonClear();                           // ORIG C1 0x8417c74
        m_dispatchers[80] = new DB_CrashDownReport();                            // ORIG C1 0x844e0c6
        m_dispatchers[82] = new DB_SaveFeaturedGoodsIdx();                       // ORIG C1 0x844e0e2
        m_dispatchers[83] = new DB_RemoveFeaturedGoodsIdx();                     // ORIG C1 0x844e0fe
        m_dispatchers[73] = new DB_GenCeraTicket();                              // ORIG C1 0x844da6e
        m_dispatchers[76] = new DB_SaveEventItem();                              // ORIG C1 0x844dad8
        m_dispatchers[77] = new DB_RemoveEventItem();                            // ORIG C1 0x844db42
        m_dispatchers[78] = new DB_FatigueResetTimer();                          // ORIG C1 0x844dbac
        m_dispatchers[81] = new DB_SuicideWhenQueueIsEmpty();                    // ORIG C1 0x844e11a
        m_dispatchers[84] = new DB_SavePremiumInfo();                            // ORIG C1 0x844e136
        m_dispatchers[99] = new DB_SaveCharacInfo4Guild();                       // ORIG C1 0x844e152
        m_dispatchers[103] = new DB_StatDungeonFail();                           // ORIG C1 0x844e16e
        m_dispatchers[104] = new DB_StatPVPPlay2();                              // ORIG C1 0x844e18a
        m_dispatchers[105] = new DB_StatUseShusiaService();                      // ORIG C1 0x844e1a6
        m_dispatchers[106] = new DB_UpdateDetailChannel();                       // ORIG C1 0x844e1c2
        m_dispatchers[107] = new DB_SaveChannelUserLevel();                      // ORIG C1 0x844e1de
        m_dispatchers[119] = new DB_SavePrivateStore();                          // ORIG C1 0x844e1fa
        m_dispatchers[120] = new DB_LoadPrivateStore();                          // ORIG C1 0x844e216
        m_dispatchers[121] = new DB_SaveGamechannel();                           // ORIG C1 0x844e232
        m_dispatchers[123] = new DB_UpdateQueryCountTimerLogdb();                // ORIG C1 0x844e24e
        m_dispatchers[124] = new DB_UpdateQueryCountTimerDb();                   // ORIG C1 0x844e26a
        m_dispatchers[125] = new DB_UpdateQueryCountDbLogdb();                   // ORIG C1 0x844e286
        m_dispatchers[131] = new DB_UpdateNatType();                             // ORIG C1 0x844e2a2
        m_dispatchers[132] = new DB_UpdateNatTypeUser();                         // ORIG C1 0x844e2be
        m_dispatchers[126] = new DB_MailBox_Req_CheckReceiver();                 // ORIG C1 0x844e2da
        m_dispatchers[127] = new DB_MailBox_Req_List();                          // ORIG C1 0x844e2f6
        m_dispatchers[129] = new DB_MailBox_Req_Delete();                        // ORIG C1 0x844e312
        m_dispatchers[128] = new DB_MailBox_Req_Send();                          // ORIG C1 0x844e32e
        m_dispatchers[135] = new DB_InsertCreatureItem();                        // ORIG C1 0x844e34a
        m_dispatchers[137] = new DB_UpdateCreatureItemList();                    // ORIG C1 0x844e366
        m_dispatchers[138] = new DB_UpdateCreatureItem();                        // ORIG C1 0x844e382
        m_dispatchers[140] = new DB_SwapCreatureItem();                          // ORIG C1 0x844e39e
        m_dispatchers[142] = new DB_GatheringParyStatus();                       // ORIG C1 0x844e3ba
        m_dispatchers[147] = new DB_ForbidUserToPlay();                          // ORIG C1 0x844e3d6
        m_dispatchers[149] = new DB_LastMsgDb();                                 // ORIG C1 0x844e3f2
        m_dispatchers[150] = new DB_LastMsgLogDb();                              // ORIG C1 0x844e40e
        m_dispatchers[154] = new DB_SaveClientHackLog();                         // ORIG C1 0x844e42a
        m_dispatchers[155] = new DB_InsertQueueSizeLog();                        // ORIG C1 0x844e446
        m_dispatchers[157] = new DB_SaveSameMIDOtherCharLoginHack();             // ORIG C1 0x844e462
        m_dispatchers[158] = new DB_SaveProxyLog();                              // ORIG C1 0x844e47e
        m_dispatchers[161] = new DB_SaveGoldZeroTradeLog();                      // ORIG C1 0x844e49a
        m_dispatchers[163] = new DB_MouseRegister();                             // ORIG C1 0x844e4b6
        m_dispatchers[164] = new DB_PassPadUpdateFailCnt();                      // ORIG C1 0x844e4d2
        m_dispatchers[165] = new DB_PassPadUpdateCancelCnt();                    // ORIG C1 0x844e4ee
        m_dispatchers[175] = new DB_UpdateDungeonDataStatisticLogdb();           // ORIG C1 0x844e50a
        m_dispatchers[176] = new DB_BillingRecord();                             // ORIG C1 0x844e526
        m_dispatchers[179] = new DB_ReqBlackCount();                             // ORIG C1 0x844e542
        m_dispatchers[180] = new DB_ReqSaveCrazyLevelupEvent();                  // ORIG C1 0x844e55e
        m_dispatchers[181] = new DB_ReqSaveHackUserPunish();                     // ORIG C1 0x844e57a
        m_dispatchers[182] = new DB_InsertUserPlayTime();                        // ORIG C1 0x844e596
        m_dispatchers[187] = new DB_UpdateHappyBeanInfo();                       // ORIG C1 0x844e5b2
        m_dispatchers[190] = new DB_InsertQueryCounterDb();                      // ORIG C1 0x844e5ce
        m_dispatchers[191] = new DB_InsertQueryCounterLogdb();                   // ORIG C1 0x844e5ea
        m_dispatchers[192] = new DB_ReqItemUpgradeLog();                         // ORIG C1 0x844e606
        m_dispatchers[194] = new DB_ChangeLetterStat();                          // ORIG C1 0x844e622
        m_dispatchers[195] = new DB_ReqChangeCharacName();                       // ORIG C1 0x844e63e
        m_dispatchers[196] = new DB_ReqQueryCharacInfo();                        // ORIG C1 0x844e65a
        m_dispatchers[197] = new DB_MailBoxSendNewMailWithDBWork();              // ORIG C1 0x844e676
        m_dispatchers[202] = new DB_ReportMannerlessUser_Upgrade();              // ORIG C1 0x844e692
        m_dispatchers[203] = new DB_ReportMannerlessUser();                      // ORIG C1 0x844e6ae
        m_dispatchers[204] = new DB_UpdateClientCrashDownLog();                  // ORIG C1 0x844e6ca
        m_dispatchers[205] = new DB_TimerStatisticsLog();                        // ORIG C1 0x844e6e6
        m_dispatchers[206] = new DB_SaveUserHandicap();                          // ORIG C1 0x844e702
        m_dispatchers[207] = new DB_ForceSaveSimpleSSO();                        // ORIG C1 0x844e71e
        m_dispatchers[273] = new DB_CountOfHumanCertify();                       // ORIG C1 0x844e73a
        m_dispatchers[209] = new DB_UpdateGiftCount();                           // ORIG C1 0x844e756
        m_dispatchers[208] = new DB_UpdateMemberDoubtTrade();                    // ORIG C1 0x844e772
        m_dispatchers[210] = new DB_UpdateItemDropStatistics();                  // ORIG C1 0x844e78e
        m_dispatchers[212] = new DB_UpdatePvPGrade();                            // ORIG C1 0x844e7aa
        m_dispatchers[213] = new DB_InsertValueStatistic();                      // ORIG C1 0x844e7c6
        m_dispatchers[224] = new DB_InsertDungeonEntrance();                     // ORIG C1 0x844e7e2
        m_dispatchers[225] = new DB_InsertDungeonEntrance_hour();                // ORIG C1 0x844e7fe
        m_dispatchers[226] = new DB_GetDeathTowerBestRecord();                   // ORIG C1 0x844e81a
        m_dispatchers[227] = new DB_UpdateDeathTowerBestRecord();                // ORIG C1 0x844e836
        m_dispatchers[228] = new DB_DeathTowerLoadTopRanker();                   // ORIG C1 0x844e852
        m_dispatchers[235] = new DB_CheckDoubleGuildName();                      // ORIG C1 0x844e86e
        m_dispatchers[236] = new DB_CheckDoubleGuildAddress();                   // ORIG C1 0x844e88a
        m_dispatchers[238] = new DB_ArrangeUserLoginInfo();                      // ORIG C1 0x844e8a6
        m_dispatchers[253] = new DB_SecurityCardIssue();                         // ORIG C1 0x844e8c2
        m_dispatchers[254] = new DB_SecurityCardDisuse();                        // ORIG C1 0x844e8de
        m_dispatchers[255] = new DB_SecurityCardActivate();                      // ORIG C1 0x844e8fa
        m_dispatchers[256] = new DB_SecurityCardUpdateValidityTime();            // ORIG C1 0x844e916
        m_dispatchers[257] = new DB_SecurityCardUpdateFailCnt();                 // ORIG C1 0x844e932
        m_dispatchers[258] = new DB_SecurityCardUpdateCancelCnt();               // ORIG C1 0x844e94e
        m_dispatchers[312] = new DB_SecurityCardRetransfer();                    // ORIG C1 0x844e96a
        m_dispatchers[259] = new DB_InsertPartyType();                           // ORIG C1 0x844e986
        m_dispatchers[260] = new DB_InsertPVPPlayerRate();                       // ORIG C1 0x844e9a2
        m_dispatchers[261] = new DB_InsertPVPMap();                              // ORIG C1 0x844e9be
        m_dispatchers[262] = new DB_LoadDnfHackLog();                            // ORIG C1 0x844e9da
        m_dispatchers[263] = new DB_InsertLevelDungeonPlay();                    // ORIG C1 0x844e9f6
        m_dispatchers[264] = new DB_ReloadAutoPunishRuleBackIP();                // ORIG C1 0x844ea12
        m_dispatchers[265] = new DB_ReloadAutoPunishRuleHackType();              // ORIG C1 0x844ea2e
        m_dispatchers[266] = new DB_InsertAutoPunishFirstUser();                 // ORIG C1 0x844ea4a
        m_dispatchers[267] = new DB_InsertAutoPunishSecondUser();                // ORIG C1 0x844ea66
        m_dispatchers[338] = new DB_BlackIPMonitor();                            // ORIG C1 0x844ea82
        m_dispatchers[340] = new DB_SaveCleanpadPoint();                         // ORIG C1 0x844ea9e
        m_dispatchers[341] = new DB_BlackIPMonitorPartLoad();                    // ORIG C1 0x844eaba
        m_dispatchers[274] = new DB_Insert0712WinterEventCoupon();               // ORIG C1 0x844ead6
        m_dispatchers[275] = new DB_Insert0712WinterEventCoupon();               // ORIG C1 0x844ead6
        m_dispatchers[279] = new DB_InsertArchieveEventLog();                    // ORIG C1 0x844eaf2
        m_dispatchers[278] = new DB_InsertDungeonExp();                          // ORIG C1 0x844eb0e
        m_dispatchers[283] = new DB_WriteGeoReject();                            // ORIG C1 0x844eb2a
        m_dispatchers[290] = new DB_InsertUnlimitSupportLog();                   // ORIG C1 0x844eb46
        m_dispatchers[295] = new DB_LoadCleanPadPoint();                         // ORIG C1 0x844eb62
        m_dispatchers[297] = new DB_UpdateSchoolPoint();                         // ORIG C1 0x844eb7e
        m_dispatchers[336] = new DB_SaveAccountInfo();                           // ORIG C1 0x844eb9a
        m_dispatchers[299] = new DB_GoblinPadUpdateReward();                     // ORIG C1 0x844ebb6
        m_dispatchers[300] = new DB_GoblinPadUpdateValidityTime();               // ORIG C1 0x844ebd2
        m_dispatchers[302] = new DB_LoadGameOption();                            // ORIG C1 0x844ebee
        m_dispatchers[304] = new DB_SaveGameOption_1();                          // ORIG C1 0x844ec0a
        m_dispatchers[305] = new DB_SaveGameOption_2();                          // ORIG C1 0x844ec26
        m_dispatchers[693] = new DB_SaveCharacterOption();                       // ORIG C1 0x844ec42
        m_dispatchers[306] = new DB_UpdateCoolTimeItem();                        // ORIG C1 0x844ec5e
        m_dispatchers[311] = new DB_UpdateEffectItem();                          // ORIG C1 0x844ec7a
        m_dispatchers[313] = new DB_UpdateAvatarJewelSlot();                     // ORIG C1 0x844ec96
        m_dispatchers[309] = new DB_UpdatePvPResult();                           // ORIG C1 0x844ecb2
        m_dispatchers[310] = new DB_UpdatePvPLastPlayTime();                     // ORIG C1 0x844ecce
        m_dispatchers[319] = new DB_LoadBloodBestRecord();                       // ORIG C1 0x844ecea
        m_dispatchers[320] = new DB_UpdateBloodBestRecord();                     // ORIG C1 0x844ed06
        m_dispatchers[321] = new DB_LoadBreakAwayPreventData();                  // ORIG C1 0x844ed22
        m_dispatchers[322] = new DB_LoadBreakAwayRewardData();                   // ORIG C1 0x844ed3e
        m_dispatchers[326] = new DB_UpdateBreakAwayAccruedCera();                // ORIG C1 0x844ed5a
        m_dispatchers[327] = new DB_UpdateBreakAwayUvList();                     // ORIG C1 0x844ed76
        m_dispatchers[329] = new DB_UpdateBreakAwayRuckPoint();                  // ORIG C1 0x844ed92
        m_dispatchers[324] = new DB_UpdateBreakAwayClearAddInfo();               // ORIG C1 0x844edae
        m_dispatchers[325] = new DB_UpdateBreakAwayDungeonClearCnt();            // ORIG C1 0x844edca
        m_dispatchers[330] = new DB_UpdateGMData();                              // ORIG C1 0x844ede6
        m_dispatchers[334] = new DB_LoadDimension();                             // ORIG C1 0x844ee02
        m_dispatchers[335] = new DB_UpdateDimension();                           // ORIG C1 0x844ee1e
        m_dispatchers[337] = new DB_InsertCirculationStatistic();                // ORIG C1 0x844ee3a
        m_dispatchers[346] = new DB_LoadAutoMarketConditionsControl();           // ORIG C1 0x844ee56
        m_dispatchers[347] = new DB_UpdateAutoMarketConditionsControl();         // ORIG C1 0x844ee72
        m_dispatchers[348] = new DB_UpdateOneDayLetheSkill();                    // ORIG C1 0x844ee8e
        m_dispatchers[355] = new DB_UpdateVendingMachineStatistic();             // ORIG C1 0x844eeaa
        m_dispatchers[357] = new DB_LoadPowerWarStatueRanker();                  // ORIG C1 0x844eec6
        m_dispatchers[358] = new DB_LoadPowerWarStatueInfo();                    // ORIG C1 0x844eee2
        m_dispatchers[356] = new DB_BlackipPrivateSecurityValuation();           // ORIG C1 0x844eefe
        m_dispatchers[364] = new DB_UpdateServerMessage();                       // ORIG C1 0x844ef1a
        m_dispatchers[365] = new DB_LoadServerMessage();                         // ORIG C1 0x844ef36
        m_dispatchers[366] = new DB_UpdateCharacterDay();                        // ORIG C1 0x844ef52
        m_dispatchers[367] = new DB_UpdateNPCRelationship();                     // ORIG C1 0x844ef6e
        m_dispatchers[380] = new DB_SecurityCardAddCertCnt();                    // ORIG C1 0x844ef8a
        m_dispatchers[384] = new DB_LoadExpertJob();                             // ORIG C1 0x844efa6
        m_dispatchers[385] = new DB_SaveExpertJob();                             // ORIG C1 0x844efc2
        m_dispatchers[386] = new DB_UpdateExpertJobStatistic();                  // ORIG C1 0x844efde
        m_dispatchers[387] = new DB_InsertServerLoadStatistic();                 // ORIG C1 0x844effa
        m_dispatchers[394] = new DB_FindFactoryHub();                            // ORIG C1 0x844f016
        m_dispatchers[400] = new DB_HackIPSave();                                // ORIG C1 0x844f032
        m_dispatchers[405] = new DB_CancelRestingUserRestrict();                 // ORIG C1 0x844f04e
        m_dispatchers[407] = new DB_SaveBloodDungeon();                          // ORIG C1 0x844f06a
        m_dispatchers[411] = new DB_LoadPowerWarInfo();                          // ORIG C1 0x844f086
        m_dispatchers[413] = new DB_SecuServiceReward();                         // ORIG C1 0x844f0a2
        m_dispatchers[416] = new DB_Find_Factory_Hub_User();                     // ORIG C1 0x844f0be
        m_dispatchers[415] = new DB_SaveConditionEventInfo();                    // ORIG C1 0x844f0da
        m_dispatchers[421] = new DB_DeletePowerWarStatueMessage();               // ORIG C1 0x844f0f6
        m_dispatchers[423] = new DB_LoadNoCacheData();                           // ORIG C1 0x844f112
        m_dispatchers[440] = new DB_UpdateItemLock();                            // ORIG C1 0x844f12e
        m_dispatchers[449] = new DB_UpdateCreatureItemLock();                    // ORIG C1 0x844f14a
        m_dispatchers[450] = new DB_UpdateAvatarItemLock();                      // ORIG C1 0x844f166
        m_dispatchers[451] = new DB_UpdateMemberPlayInfoPcRoom();                // ORIG C1 0x844f182
        m_dispatchers[452] = new DB_UpdateCompoundEmblemStatics();               // ORIG C1 0x844f19e
        m_dispatchers[395] = new DB_AccountHack_CheckMyAccount();                // ORIG C1 0x844f1ba
        m_dispatchers[457] = new DB_SecuServiceUpdateValidityTime();             // ORIG C1 0x844f1d6
        m_dispatchers[458] = new DB_AutoPunishedSecondUserData();                // ORIG C1 0x844f1f2
        m_dispatchers[468] = new DB_SaveProperDungeonClearCount();               // ORIG C1 0x844f20e
        m_dispatchers[469] = new DB_LoadPvPBuddy();                              // ORIG C1 0x844f22a
        m_dispatchers[475] = new DB_InsertPvPBuddy();                            // ORIG C1 0x844f246
        m_dispatchers[476] = new DB_DeletePvPBuddy();                            // ORIG C1 0x844f262
        m_dispatchers[477] = new DB_UpdatePvPBuddy();                            // ORIG C1 0x844f27e
        m_dispatchers[479] = new DB_DeleteGuild();                               // ORIG C1 0x844f29a
        m_dispatchers[480] = new DB_LoadPvPLiveEventData();                      // ORIG C1 0x844f2b6
        m_dispatchers[481] = new DB_UpdatePvPLiveEventData();                    // ORIG C1 0x844f2d2
        m_dispatchers[486] = new DB_LoadOnlinePreliminaryMember();               // ORIG C1 0x844f2ee
        m_dispatchers[487] = new DB_UpdateOnlinePreliminaryMember();             // ORIG C1 0x844f30a
        m_dispatchers[488] = new DB_LoadOnlinePreliminaryTeam();                 // ORIG C1 0x844f326
        m_dispatchers[489] = new DB_UpdateOnlinePreliminaryTeam();               // ORIG C1 0x844f342
        m_dispatchers[491] = new DB_UpdateOnlinePreliminaryMemberMatchList();    // ORIG C1 0x844f35e
        m_dispatchers[492] = new DB_LoadOnlinePreliminaryTeamMatchList();        // ORIG C1 0x844f37a
        m_dispatchers[493] = new DB_UpdateOnlinePreliminaryTeamMatchList();      // ORIG C1 0x844f396
        m_dispatchers[500] = new DB_SaveRedeemItemList();                        // ORIG C1 0x844f3b2
        m_dispatchers[494] = new DB_CreateAccountCargo();                        // ORIG C1 0x844f3ce
        m_dispatchers[495] = new DB_UpgradeAccountCargo();                       // ORIG C1 0x844f3ea
        m_dispatchers[496] = new DB_DeleteAccountCargo();                        // ORIG C1 0x844f406
        m_dispatchers[498] = new DB_LoadAccountCargo();                          // ORIG C1 0x844f422
        m_dispatchers[497] = new DB_SaveAccountCargo();                          // ORIG C1 0x844f43e
        m_dispatchers[508] = new DB_SaveMileage();                               // ORIG C1 0x844f45a
        m_dispatchers[509] = new DB_LoadMileage();                               // ORIG C1 0x844f476
        m_dispatchers[25] = new advancealtar::DB_Load();                         // ORIG C1 0x844ddea
        m_dispatchers[26] = new advancealtar::DB_Update();                       // ORIG C1 0x844de06
        m_dispatchers[501] = new DB_Multi_MailBox_Req_Send();                    // ORIG C1 0x844f492
        m_dispatchers[502] = new DB_ReqQueryCharacInfoMailbox();                 // ORIG C1 0x844f4ae
        m_dispatchers[521] = new DB_GuildExpBookDelete();                        // ORIG C1 0x844f4ca
        m_dispatchers[524] = new DB_InsertPoliceSaveChatting();                  // ORIG C1 0x844f4e6
        m_dispatchers[531] = new DB_UpdateChuseokPackagePurchase();              // ORIG C1 0x844f502
        m_dispatchers[534] = new DB_CheckJoinGuild();                            // ORIG C1 0x844f51e
        m_dispatchers[535] = new DB_RequestJoinGuild();                          // ORIG C1 0x844f53a
        m_dispatchers[537] = new DB_JoinGuildInfo();                             // ORIG C1 0x844f556
        m_dispatchers[536] = new DB_CancelJoinGuild();                           // ORIG C1 0x844f572
        m_dispatchers[538] = new DB_GuildJoinList();                             // ORIG C1 0x844f58e
        m_dispatchers[540] = new DB_DenyJoinGuild();                             // ORIG C1 0x844f5aa
        m_dispatchers[543] = new DB_UpdateReturnUserFirstLogin();                // ORIG C1 0x844f5c6
        m_dispatchers[606] = new DB_InsertAccountFirstLoginFlag();               // ORIG C1 0x844f5e2
        m_dispatchers[569] = new DB_SaveGameOption_QuickChatting();              // ORIG C1 0x844f5fe
        m_dispatchers[572] = new DB_UpdatePurchaseCashItemBonus();               // ORIG C1 0x844f61a
        m_dispatchers[576] = new DB_UpdateItemCountValue();                      // ORIG C1 0x844f636
        m_dispatchers[577] = new DB_UpdateLimitItemUsage();                      // ORIG C1 0x844f652
        m_dispatchers[593] = new DB_LogBuyItem();                                // ORIG C1 0x844f66e
        m_dispatchers[580] = new DB_UpdateRevengeData();                         // ORIG C1 0x844f68a
        m_dispatchers[583] = new DB_UpdateGoblinPadAdvicePwdRewardTime();        // ORIG C1 0x844f6a6
        m_dispatchers[590] = new DB_UpdateCharacPlayTime();                      // ORIG C1 0x844f6c2
        m_dispatchers[621] = new DB_SaveExpandEquipslot();                       // ORIG C1 0x844f6de
        m_dispatchers[609] = new DB_UpdateFairPvPScore();                        // ORIG C1 0x844f6fa
        m_dispatchers[611] = new DB_SavePvPMissionList();                        // ORIG C1 0x844f716
        m_dispatchers[620] = new DB_InsertResponseTime();                        // ORIG C1 0x844f732
        m_dispatchers[622] = new DB_UpdateItemMakingSkill();                     // ORIG C1 0x844f74e
        m_dispatchers[629] = new DB_TOD_Random_Select_UserAPC();                 // ORIG C1 0x844f76a
        m_dispatchers[631] = new DB_TOD_Reset_UserAPC();                         // ORIG C1 0x844f786
        m_dispatchers[632] = new DB_TOD_Update_UserState();                      // ORIG C1 0x844f7a2
        m_dispatchers[633] = new DB_UpdateQuestShopBuyingStatusList();           // ORIG C1 0x844f7be
        m_dispatchers[640] = new DB_UpdateAddItemCountValue();                   // ORIG C1 0x844f7da
        m_dispatchers[641] = new DB_SaveKillMonsterInfo();                       // ORIG C1 0x844f7f6
        m_dispatchers[642] = new DB_LoadKillMonsterInfo();                       // ORIG C1 0x844f812
        m_dispatchers[645] = new DB_SaveTitleBook();                             // ORIG C1 0x844f82e
        m_dispatchers[646] = new DB_SaveAchievement();                           // ORIG C1 0x844f84a
        m_dispatchers[647] = new DB_UpdateUserFatigueQuantity();                 // ORIG C1 0x844f866
        m_dispatchers[648] = new DB_UpdateUserGiftFatigueQuantity();             // ORIG C1 0x844f882
        m_dispatchers[650] = new DB_SaveCharacSlotExtend();                      // ORIG C1 0x844f89e
        m_dispatchers[653] = new DB_SaveGiveAvengerTitleFlag();                  // ORIG C1 0x844f8ba
        m_dispatchers[654] = new DB_SelectLevelUpReward();                       // ORIG C1 0x844f8d6
        m_dispatchers[543] = new DB_UpdateReturnUserFirstLogin();                // ORIG C1 0x844f5c6
        m_dispatchers[665] = new DB_InsertRandomOptionItemInform();              // ORIG C1 0x844f8f2
        m_dispatchers[668] = new DB_StatisticCeraShopGiftLog();                  // ORIG C1 0x844f90e
        m_dispatchers[673] = new DB_GetUserRegdate();                            // ORIG C1 0x844f92a
        m_dispatchers[663] = new DB_UpdateOntimeLastRecvIdx();                   // ORIG C1 0x844f946
        m_dispatchers[664] = new DB_InsertOnTimeRewardUserLog();                 // ORIG C1 0x844f962
        m_dispatchers[674] = new DB_CheckCharacName();                           // ORIG C1 0x844f97e
        m_dispatchers[676] = new DB_MailBox_Req_System_Multi_Mail();             // ORIG C1 0x844f99a
        m_dispatchers[683] = new DB_CheckSecuReward();                           // ORIG C1 0x844f9b6
        m_dispatchers[684] = new DB_UpdateFatigueAttendance();                   // ORIG C1 0x844f9d2
        m_dispatchers[685] = new DB_UpdateInGameEventHistory();                  // ORIG C1 0x844f9ee
        m_dispatchers[686] = new DB_UpdateInventoryCapacity();                   // ORIG C1 0x844fa0a
        m_dispatchers[687] = new DB_UpdateAuraAvatarOption();                    // ORIG C1 0x844fa26
        m_dispatchers[689] = new DB_InsertCeraItemHistory();                     // ORIG C1 0x844fa42
        m_dispatchers[688] = new DB_UpdateAvatarHistory();                       // ORIG C1 0x844fa5e
        m_dispatchers[690] = new DB_SaveItemDictionaryFlag();                    // ORIG C1 0x844fa7a
        m_dispatchers[691] = new DB_SaveMercenaryData();                         // ORIG C1 0x844fa96
        m_dispatchers[694] = new DB_HadesPunish();                               // ORIG C1 0x844fab2
        m_dispatchers[695] = new DB_Check3rdPartyConcent();                      // ORIG C1 0x844face
        m_dispatchers[696] = new DB_MercenaryLogInfo();                          // ORIG C1 0x844faea
        m_dispatchers[697] = new DB_SelectCharacMid();                           // ORIG C1 0x844fb06
        m_dispatchers[707] = new DB_AvatarOptionChange();                        // ORIG C1 0x844fb22
        m_dispatchers[714] = new DB_SaveObjectBringUp();                         // ORIG C1 0x844fb3e
        m_dispatchers[727] = new DB_SaveGameOption_ChattingEmoticon();           // ORIG C1 0x844fb5a
        m_dispatchers[728] = new DB_UpdateBoosterGageData();                     // ORIG C1 0x844fb76
        m_dispatchers[733] = new CLocalChina_DB_BreakAwayReset();                // ORIG C1 0x844fb92
        m_dispatchers[734] = new CLocalChina_DB_LastDay_BreakAway();             // ORIG C1 0x844fbae
        m_dispatchers[736] = new CLocalGlobal_DB_STATICS_BOSSTOWER();            // ORIG C1 0x844fbca
        m_dispatchers[729] = new DB_UpdateCeraShopPurchaseCount();               // ORIG C1 0x844fbe6
        m_dispatchers[732] = new DB_AradDatabaseDispatcher();                    // ORIG C1 0x87244f0
        m_dispatchers[737] = new DB_LoadPuUserCheck();                           // ORIG C1 0x844fc02
        m_dispatchers[738] = new DB_SavePuUserCheck();                           // ORIG C1 0x844fc1e
        m_dispatchers[827] = new advancealtar::DB_StatisticAdvanceAltar();       // ORIG C1 0x844fc3a
        m_dispatchers[830] = new DB_CheckAddFriend();                            // ORIG C1 0x844fc56
        m_dispatchers[757] = new DB_SaveInformNotice();                          // ORIG C1 0x844fc72
        m_dispatchers[760] = new DB_NoticeIngameAdvertisement();                 // ORIG C1 0x844fc8e
        m_dispatchers[765] = new DB_RequestCharacSkillInfo();                    // ORIG C1 0x844fcaa
        m_dispatchers[766] = new DB_RequestTagCharacInfo();                      // ORIG C1 0x844fcc6
        m_dispatchers[767] = new DB_SavePcRoomPlayTimeEvent();                   // ORIG C1 0x844fce2
        m_dispatchers[768] = new DB_SaveBestClearTime();                         // ORIG C1 0x844fcfe
        m_dispatchers[777] = new DB_AuctionSubMoney();                           // ORIG C1 0x844fd1a
        m_dispatchers[780] = new DB_UpdateBlueMarbleEnterCount();                // ORIG C1 0x844fd36
        m_dispatchers[782] = new DB_InsertAttendanceEvent();                     // ORIG C1 0x844fd52
        m_dispatchers[799] = new DB_UpdateBingoEvent();                          // ORIG C1 0x844fd6e
        m_dispatchers[787] = new DB_EventSaveGrowthWeapon();                     // ORIG C1 0x844fd8a
        m_dispatchers[788] = new DB_EventUpdateGrowthWeaponCharac();             // ORIG C1 0x844fda6
        m_dispatchers[789] = new DB_EventUpdateGrowthWeaponTimepiece();          // ORIG C1 0x844fdc2
        m_dispatchers[790] = new DB_EventGrowthWeaponReset();                    // ORIG C1 0x844fdde
        m_dispatchers[791] = new DB_EventSaveGiveGrowCreature();                 // ORIG C1 0x844fdfa
        m_dispatchers[792] = new DB_EventUpdateGiveGrowCreature();               // ORIG C1 0x844fe16
        m_dispatchers[793] = new DB_EventAccountCharacterLevelUpOnceGift();      // ORIG C1 0x844fe32
        m_dispatchers[794] = new DB_AvatarColorChange();                         // ORIG C1 0x844fe4e
        m_dispatchers[796] = new DB_TestTradeRestrict();                         // ORIG C1 0x844fe6a
        m_dispatchers[797] = new DB_UpdateComboSkill();                          // ORIG C1 0x844fe86
        m_dispatchers[785] = new DB_SaveCharacInformNotice();                    // ORIG C1 0x844fea2
        m_dispatchers[798] = new DB_UpdateSeriaRoomAnimationDeco();              // ORIG C1 0x844febe
        m_dispatchers[800] = new DB_LoadCharacHotKeyOption();                    // ORIG C1 0x844feda
        m_dispatchers[812] = new DB_LogIngameAdvertisement();                    // ORIG C1 0x844fef6
        m_dispatchers[814] = new DB_LogCeraShopError();                          // ORIG C1 0x844ff12
        m_dispatchers[818] = new DB_GetPremiumSelectedCubeInfo();                // ORIG C1 0x844ff2e
        m_dispatchers[819] = new DB_InsertEventStageLog();                       // ORIG C1 0x844ff4a
        m_dispatchers[846] = new DB_DispatcherTaiwan();                          // ORIG C1 0x87244f0
        m_dispatchers[832] = new DB_SaveCerashopAddRestrictType();               // ORIG C1 0x844ff66
        m_dispatchers[833] = new DB_LoadCerashopAddRestrictType();               // ORIG C1 0x844ff82
        m_dispatchers[834] = new DB_InsertAccountFirstLoginDailyFlag();          // ORIG C1 0x844ff9e
        m_dispatchers[847] = new DB_SaveEventLevelupSupport();                   // ORIG C1 0x844ffba
        m_dispatchers[848] = new DB_LoadEventLevelupSupport();                   // ORIG C1 0x844ffd6
        m_dispatchers[850] = new DB_SelectAllowProxyUser();                      // ORIG C1 0x844fff2
        return 1;                                   // ORIG mov $0x1,%ebx
    } catch (...) {                                 // ORIG CatchHandler 0x083ff0dd
        cMyTrace(__PRETTY_FUNCTION__, 0x8b2, 5)(
            "[%s][%d]", __PRETTY_FUNCTION__, 0x8b2);
        return 0;                                   // ORIG mov $0x0,%ebx
    }
}

// ---------------------------------------------------------------------------
// UnregistDispatcher —— ORIG 0x083ff156 sz 0x72
// 遍历 852 槽：槽非空则调用其 vtable +0x08 槽（ORIG
// `mov (%eax),%ecx` 其中 eax=vptr+0x8），随后清槽；等价于 delete p。
// ORIG 存在重复判空分支（0x83ff173 与 0x83ff17d 连续两次 test），忠实保留。
// 返回恒 1。
// ---------------------------------------------------------------------------
#include "W8LT_DBTask/UnregistDispatcher.inc"
{
    // [b26N7] 循环内虚调用 callee 寄存器 ORIG vfn→%ecx vs 4.4 恒 %eax（N7
    // 探针实证）；其余全等，ndiff 12 封顶。
    for (int i = 0; i <= DISPATCHER_COUNT - 1; ++i)
    {
        if (m_dispatchers[i] != 0)
        {
            delete m_dispatchers[i];            // ORIG 判空×2：if + delete 隐式判空
            m_dispatchers[i] = 0;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// HandleEvents —— ORIG 0x083ff1c8 sz 0x22b
// 取队列流：空队列 uSleep(100) 后返回 true；否则记录当前队列号并交给
// DBProcess。若处理结果是信号 0x3f（关服信号）打 Terminate trace 并返回
// false；否则按 stream 的池类型归还对应流池（size 超限时各打越界 trace）
// 并返回 true。
// 字符串证据：'bool DBTask::HandleEvents(...)' @0x8c5daa0、
// 'Terminate Server db queue: %s(%d)\n' @0x8c3672c、
// 'Small Stream Size over code(%d), size(%u)' @0x8c36750、
// 'Default Stream Size over code(%d), size(%u)' @0x8c3677c、
// 'Big Stream Size over code(%d), size(%u)' @0x8c367a8。
// ---------------------------------------------------------------------------
// [b28d-4] DIFF：P222：块序/极性（ORIG je 落点 vs 4.4 jne）；ORIG 083ff1c8
// [r3 负结论] 三处 cMyTrace 实参 `mov -0x10(%ebp),%eax; mov %eax,0xc(%esp)`
//   （ORIG 使用点重载）vs 我方 `mov -0x10(%ebp),%esi` 跨 call 保活 = 4.4
//   callee-saved 保活策略，纯寄存器分配差（ndiff 12、差集空）⇒ N 封顶。
#include "W8LT_DBTask/HandleEvents.inc"
{
    Stream* stream = GlobalData::s_msgq_mgr->get(queueIdx);
    if (stream == 0)
    {
        OS_API::uSleep(100u);
        return true;
    }

    m_curQueueIdx = static_cast<int>(queueIdx);
    const int length = stream->length();
    const int poolType = stream->m_poolType;      // Stream +0x20（Stream.h:73）

    DBProcess(stream, signal);

    if (signal == 0x3f)
    {
        // P227：ORIG 0x083ff24e 在 ctor 前 call get_queue_name 存 %ebx 保活。
        cMyTrace(__PRETTY_FUNCTION__, 0x8e6, 0)(
            "Terminate Server db queue: %s(%d)\n",
            MsgQueueMgr::get_queue_name(queueIdx), static_cast<int>(queueIdx));
        return false;
    }

    // [SweepIntervalSet] P251：ORIG 0x083ff29d `mov -0xc(%ebp),%eax; cmp $0x1,%eax;
    // je; cmp $0x2,%eax; je; test %eax,%eax; je; jmp`（值留寄存器、无槽直比）⇒
    // 源形为 switch(poolType)（if-else 链在 4.4 恒产 `cmpl $K,mem; jne`）。
    // 三处 trace 同时改 P227 全表达式临时链（ORIG 各在 ctor 前读 *signal/length）。
    switch (poolType)
    {
    case 1:                                       // SmallStreamPool
        if (length > 0xfff)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x8f2, 5)(
                "Small Stream Size over code(%d), size(%u)", signal,
                static_cast<unsigned int>(length));
        }
        GlobalData::s_small_stream_pool->Free(stream);        // ORIG 0x08290004
        break;
    case 0:                                       // StreamPool
        if (length > 0x3fff)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x8fd, 5)(
                "Default Stream Size over code(%d), size(%u)", signal,
                static_cast<unsigned int>(length));
        }
        GlobalData::s_stream_pool->Free(stream);              // ORIG 0x0828fb28
        break;
    case 2:                                       // BigStreamPool
        if (length > 0x3ffff)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x908, 5)(
                "Big Stream Size over code(%d), size(%u)", signal,
                static_cast<unsigned int>(length));
        }
        GlobalData::s_big_stream_pool->Free(stream);          // ORIG 0x08290304
        break;
    }
    return true;
}

// ---------------------------------------------------------------------------
// DBProcess —— ORIG 0x083ff3f4 sz 0x1fd
// 从流中顺序读 [signal][userIdx]，按 signal 索引 dispatcher 表执行虚
// dispatch；m_curQueueIdx==DB_Q(2) 时前后设置/复位 DBMgr 的用户分库索引。
// 非法 signal/userIdx 读取失败或表空均走 LogManager logFormat 错误路径。
// 字符串证据：函数名 'bool DBTask::DBProcess(Stream*, int&)' @0x8c5da60、
// '[%s][%d]' @0x8c36721、文件名 'DBThread.cpp' @0x8c36714、
// 'Invalid signal received' @0x8c367d0(line 0x967)、
// "Can't find the db dispatcher : signal(%d)" @0x8c367e8(line 0x970)。
// ---------------------------------------------------------------------------
// [b28d-4] AE：P171 死分支照抄：ORIG 0x83ff4f1 test;jns<body> / cmp$0x353;jle<body> 两跳同落主体 ⇒ 原码 && 失败链 if(msgId<0&&msgId>851){fail}（Invalid 分支实为死码）；探针 /tmp/l4_probe_dbp.cpp formB/formC
#include "W8LT_DBTask/DBProcess.inc"
{
    int msgId = 0;
    // 技巧18 反向：ORIG 两处共用单 bool 槽 -0xd（lifetime 合并）⇒
    // 单具名变量跨段复用（双 bool = 4.4 恒分两槽不可合并）。
    bool ok;
    ok = (*stream >> msgId);                      // ORIG Stream::rsERi
    signal = msgId;
    if (!ok)
    {
        // 技巧3：临时链式（ORIG 0x08443…? trace1=-0x30 介于 msgId(-0x34)
        // 与后续槽之间；具名块对象使 4.4 槽外推 20B+）。
        cMyTrace(__PRETTY_FUNCTION__, 0x94d, 5)("[%s][%d]",
                                                __PRETTY_FUNCTION__, 0x94d);
        return false;
    }

    int userIdx = 0;
    ok = (*stream >> userIdx);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x95f, 5)("[%s][%d]",
                                                __PRETTY_FUNCTION__, 0x95f);
        return false;
    }

    // ORIG 0x83ff4f1..0x83ff533：`test %eax,%eax; jns <body>` 与
    // `cmp $0x353,%eax; jle <body>` 两跳同落主体、失败块居中 ⇒ 原码是
    // **&& 失败链** `if (msgId < 0 && msgId > 851) { fail; return false; }`
    // （探针 /tmp/l4_probe_dbp.cpp formB/formC 逐条复现；|| 正极性包裹
    // formA 反产 `jns body; jg fail` 块序 DIFF）。负值经有符号 jle 亦落
    // 主体 ⇒ ORIG 的 "Invalid signal received" 实为死代码，P171 照抄。
    if (msgId < 0 && msgId > 851)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0x967,
                              "Invalid signal received");
        return false;
    }

    DBDispatcher* dispatcher = m_dispatchers[msgId];
    if (dispatcher == 0)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0x970,
                              "Can't find the db dispatcher : signal(%d)", msgId);
        return false;
    }

    if (m_curQueueIdx == MsgQueueMgr::DB_Q)
    {
        GlobalData::s_db_mgr->setUserIdx(userIdx);
    }
    dispatcher->dispatch(msgId, userIdx, stream); // ORIG vtable slot0 (this,msgId,userIdx,stream)
    if (m_curQueueIdx == MsgQueueMgr::DB_Q)
    {
        GlobalData::s_db_mgr->setUserIdx(-1);
    }
    return true;
}

// [P5·2026-09-11] stPvPTempTable_t 的析构强定义（`#include "…/dtor_stPvPTempTable_t.inc"` +
//   空体块）已删 —— 与 P3 的 UsedFatigueRewardItem::Data 同源修法，理由见结构体处注释。

static_assert(sizeof(DBTask) == 4 + 852 * 4,     // = 0xd54
              "DBTask layout drift (m_curQueueIdx@+0x00 + dispatchers[852]@+0x04)");

// ---------------------------------------------------------------------------
// DB_UpdateAvatarJewelSlot::makeRequest —— ORIG 0x0843081c 逐指令转写
// （报告 docs/class_func_reports/DB_UpdateAvatarJewelSlot/makeRequest.md）：
// StreamPool::Acquire("DBThread.cpp",0x7731) → CStreamGuard(stream,true)
//   → guard << 0x139 → guard << -1
//   → sig = guard.GetInBuffer<SIG_UPDATE_AVATAR_JEWEL_SOCKET>()
//   → sig->m_characNo/m_avatarKey = param_1/2；memcpy(&sig->m_emblem, emblem, 0x1e)
//   → MsgQueueMgr::put((QUEUE_IDX)2, guard)（析构时入队）。
#include "W8LT_DBTask/makeRequest_5.inc"
    unsigned int characNo, unsigned int avatarKey,
    const stAvatarEmblemInfo_t* emblem)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7731), true);
    guard.operator*()->operator<<((int)0x139);
    guard.operator*()->operator<<((int)-1);
    SIG_UPDATE_AVATAR_JEWEL_SOCKET* sig =

        guard->GetInBuffer<SIG_UPDATE_AVATAR_JEWEL_SOCKET>();
    sig->m_characNo = characNo;
    sig->m_avatarKey = avatarKey;
    memcpy(sig->m_emblem, emblem, 0x1e);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---------------------------------------------------------------------------
// DB_SelectAllowProxyUser::makeRequest —— ORIG 0x0844d2d4 逐指令转写
// （报告 docs/class_func_reports/DB_SelectAllowProxyUser/makeRequest.md 全文已读）：
// Acquire("DBThread.cpp",0xc2c6) → CStreamGuard(stream,true)
//   → <<0x352(int) → <<userIdx(int) → <<accId(uint) → MsgQueueMgr::put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_6.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc2c6), true);
    guard.operator*()->operator<<((int)0x352);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---------------------------------------------------------------------------
// 本批八个 DB makeRequest 真实定义（报告 docs/class_func_reports/…/makeRequest.md
// 全文已读，逐指令对应；Acquire 行号常量即 ORIG DBThread.cpp 内嵌行号）
// ---------------------------------------------------------------------------

// ORIG 0x08448afe sz 0xef（_ZN24DB_InsertAttendanceEvent11makeRequestEijj）：
// Acquire("DBThread.cpp",0xb651) → CStreamGuard(stream,true)
//   → <<0x30e(int) → <<param_1(int) → <<param_2(uint) → <<param_3(uint)
//   → MsgQueueMgr::put(DB_Q(2), guard)。
#include "W8LT_DBTask/makeRequest_7.inc"
                                           unsigned int param3)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb651), true);
    guard.operator*()->operator<<((int)0x30e);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)param2);
    guard.operator*()->operator<<((unsigned int)param3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x084318c4 sz 0xd5（_ZN22DB_LoadBloodBestRecord11makeRequestEij）：
// Acquire("DBThread.cpp",0x78c6) → CStreamGuard(stream,true)
//   → <<0x13f(int) → <<param_1(int) → <<param_2(uint) → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_8.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x78c6), true);
    guard.operator*()->operator<<((int)0x13f);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0843915c sz 0x17f（_ZN26DB_LoadBreakAwayRewardData11makeRequestEijjR18STBreakAwayRewards）：
// Acquire("DBThread.cpp",0x86d3) → CStreamGuard(stream,true)
//   → <<0x142(int) → <<param_1(int) → <<param_2(uint) → <<param_3(uint)
//   → <<rewards+0x00(uint) → <<rewards+0x04(int) → <<rewards+0x08(int)
//   → <<rewards+0x10(int) → <<rewards+0x0c(uint)（ORIG 字段写序 +0x10 先于 +0xc）
//   → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_9.inc"
    int userIdx, unsigned int param2, unsigned int param3,
    STBreakAwayRewards& rewards)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x86d3), true);
    guard.operator*()->operator<<((int)0x142);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)param2);
    guard.operator*()->operator<<((unsigned int)param3);
    // ORIG 0x08439204 `mov (%eax),%ebx` + call _ZN12CStreamGuardlsEj
    // （unsigned int 重载，080e2292）——字段必须走 uint 重载；单条装载要求
    // 字段类型为 unsigned int（CUser.h 现声明 int，已上报 Main 代落）。
    guard.operator*()->operator<<((unsigned int)rewards.m_rewardItemId);
    guard.operator*()->operator<<((int)rewards.m_rewardAItemCnt);
    guard.operator*()->operator<<((int)rewards.m_luckPoint);
    guard.operator*()->operator<<((int)rewards.m_itemOrder);
    guard.operator*()->operator<<((unsigned int)rewards.m_fieldc);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x08431d8c sz 0x141（_ZN16DB_LoadDimension11makeRequestEij）：
// Acquire("DBThread.cpp",0x7916) → CStreamGuard(stream,true)
//   → <<0x14e(int) → <<param_1(int) → sig=GetInBuffer<SIG_LOAD_DIMENSION>()
//   → sig+0x00(uint)=param_2；i=0..5：sig+0x04+i(byte)=(char)
//     G_CDataManager()->get_dimensionInout(i)；sig+0x0c(byte)=(char)
//     G_CDataManager()->get_limit_inout_count(2) → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_10.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7916), true);
    guard.operator*()->operator<<((int)0x14e);
    guard.operator*()->operator<<((int)userIdx);
    SIG_LOAD_DIMENSION* sig = guard->GetInBuffer<SIG_LOAD_DIMENSION>();
    // ORIG 0x08431e74(+0xe8) `movsbl %al,%eax; mov %eax,-0x14(%ebp);
    // mov -0x14(%ebp),%eax; mov %eax,%edx; mov -0x18(%ebp),%eax;
    // mov %dl,0xc(%eax)` ⇒ limit 值为跨循环边界的具名 int 槽（-0x14，
    // 声明位与 sig(-0x18)/guard(-0x20) 槽序一致，技巧21），store 走
    // 槽回读→%edx 隐式窄化（无显式 cast，避免折叠）。
    int limitVal;
    sig->m_field00 = param2;
    for (int i = 0; i <= 5; ++i)
    {
        // ORIG 0x08431d8c 体：`movsbl %al,%eax; mov %eax,-0x14(%ebp);
        // mov -0x14(%ebp),%eax; mov %eax,%edx; mov %dl,...` —— v int 槽
        // 物化（char 返回→int 扩展存槽，(char) 截断隐式）。4.4 显式
        // (char)v 会把槽折叠，⇒ 去掉显式 cast 走隐式窄化。
        // 残留（blocked N18a）：ORIG v 有 int 槽（`movsbl %al,%eax; mov
        // %eax,-0x14; mov -0x14,%eax; mov %eax,%edx` 四步），4.4 对单使用
        // v 恒 copy-forward 折槽；volatile 探针反产特殊远槽（-0x1c）全帧
        // 更乱，已回退。
        int v = G_CDataManager()->get_dimensionInout(i);
        sig->m_dimensionInout[i] = v;
    }
    limitVal = G_CDataManager()->get_limit_inout_count((ENUM_BLOOD_TYPE)2);
    sig->m_limitInoutCount = limitVal;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0844d0a6 sz 0xd5（_ZN26DB_LoadEventLevelupSupport11makeRequestEii）：
// Acquire("DBThread.cpp",0xc2a0) → CStreamGuard(stream,true)
//   → <<0x350(int) → <<param_1(int) → <<param_2(int) → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_11.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc2a0), true);
    guard.operator*()->operator<<((int)0x350);
    guard.operator*()->operator<<((int)param1);
    guard.operator*()->operator<<((int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0842801a sz 0x139（_ZN19DB_SaveUserHandicap11makeRequestEjiiiii）：
// Acquire("DBThread.cpp",0x698a) → CStreamGuard(stream,true)
//   → <<0xce(int) → <<-1(int) → sig=GetInBuffer<SIG_UPDATE_USER_HANDICAP>()
//   → sig+0x00(int)=param_2、+0x04(uint)=param_1(accId)、+0x08..+0x14(int)=param_3..6
//   → put(DB_Q(2))；随后 curSec=CSystemTime::getCurSec(s_systemTime_)，
//     DB_SaveFeaturedGoodsIdx::makeRequest(accId, 0, param_2, curSec, param_6)。
// [b28d-4] DIFF：N18a/P211：4.4 多 {'mov': 2} 中转 mov（单段直落/同型化待验）；ORIG 0842801a
#include "W8LT_DBTask/makeRequest_12.inc"
                                      int param4, int param5, int param6)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x698a), true);
    guard.operator*()->operator<<((int)0xce);
    guard.operator*()->operator<<((int)-1);
    // 残留（blocked N23 帧槽方向）：ORIG 0x08428xxx guard this=-0x14 比
    // sig(-0x18) 近；4.4 对含 dtor 对象恒置 guard 于更远槽，声明序
    // （sig 前置分离声明）探针无效已回退。
    SIG_UPDATE_USER_HANDICAP* sig = guard->GetInBuffer<SIG_UPDATE_USER_HANDICAP>();
    sig->m_param2 = param2;
    sig->m_accId = accId;
    sig->m_params[0] = param3;
    sig->m_params[1] = param4;
    sig->m_params[2] = param5;
    sig->m_params[3] = param6;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    // [J4S1 P329/P227] 删具名 curSec（ORIG 尾段 `call getCurSec; mov %eax,0xc(%esp)`
    // 无槽）⇒ 内联进实参位。
    DB_SaveFeaturedGoodsIdx::makeRequest(
        accId, 0, param2, GlobalData::s_systemTime_.getCurSec(), param6);
}

// ORIG 0x08439dec sz 0x119（_ZN30DB_UpdateCompoundEmblemStatics11makeRequestEijjjjjjj）：
// Acquire("DBThread.cpp",0x87fc) → CStreamGuard(stream,true)
//   → <<0x1c4(int) → <<-1(int) → sig=GetInBuffer<SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC>()
//   → sig+0x00(int)=param_1、+0x04..+0x1c(uint)=param_2..param_8
//   → put(LOGDB_Q(4), guard)。
#include "W8LT_DBTask/makeRequest_13.inc"
    int userIdx, unsigned int param2, unsigned int param3, unsigned int param4,
    unsigned int param5, unsigned int param6, unsigned int param7,
    unsigned int param8)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x87fc), true);
    guard.operator*()->operator<<((int)0x1c4);
    guard.operator*()->operator<<((int)-1);
    SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC* sig =
        guard.operator->()->GetInBuffer<SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC>();
    sig->m_userIdx = userIdx;
    sig->m_values[0] = param2;
    sig->m_values[1] = param3;
    sig->m_values[2] = param4;
    sig->m_values[3] = param5;
    sig->m_values[4] = param6;
    sig->m_values[5] = param7;
    sig->m_values[6] = param8;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ORIG 0x084293a0 sz 0xbd（_ZN17DB_UpdatePvPGrade11makeRequestEv）：
// Acquire("DBThread.cpp",0x6ac8) → CStreamGuard(stream,true)
//   → <<0xd4(int) → <<-1(int) → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_14.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6ac8), true);
    guard.operator*()->operator<<((int)0xd4);
    guard.operator*()->operator<<((int)-1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// DB_UpdateGMData::makeRequest —— ORIG T 0x0843982c sz 0xde 逐指令转写
// （报告缺失按 §3.2 兜底：objdump 区间 [0x0843982c,0x08439909]）：
// Acquire("DBThread.cpp",0x8761) → CStreamGuard(stream,true)
//   → <<0x14a(int) → <<-1(int) → <<flag(bool) → put(DB_Q(2))。
// 声明见 W8LT_DBTask.h（权威点）；CGmRegistEvent.cpp StartEvent 调用。
#include "W8LT_DBTask/makeRequest_15.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8761), true);
    guard.operator*()->operator<<((int)0x14a);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((bool)flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "W8LT_DBTask/makeRequest_16.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8642), true);
    guard.operator*()->operator<<((int)0x141);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((bool)flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}


// ORIG 0x0842a910 sz 0xeb（_ZN26DB_DeathTowerLoadTopRanker11makeRequestEb）：
// BigStreamPool::Acquire("DBThread.cpp",0x6c5f) → CStreamGuard(stream,true)
//   → <<0xe4(int) → <<-1(int) → <<param(bool) → put(DB_Q(2))。
// 报告：docs/class_func_reports/DB_DeathTowerLoadTopRanker/makeRequest.md。
// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
#include "W8LT_DBTask/makeRequest_17.inc"
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x6c5f), true);
    guard.operator*()->operator<<((int)0xe4);
    guard.operator*()->operator<<((int)-1);
    // ORIG +0x78 `cmpb $0,-0x1c; je; mov $1/0,%ebx`：分支即时直写 callee-saved
    // %ebx 跨 operator* 调用存活（register 局部实证 g2 同形；`v=c?1:0` 经
    // %eax 聚合多一条 mov %eax,%ebx；probe 验证）。
    // [b28d-solo] P167：ORIG 调 `CStreamGuard::lsEh`（unsigned char），
    // int 实参选中 lsEi（差集 -1/+1）⇒ 改 unsigned char。
    guard.operator*()->operator<<((unsigned char)(reload ? 1 : 0));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---------------------------------------------------------------------------
// LinkE1 批 C：DB makeRequest 真实定义（报告 docs/class_func_reports/<类>/makeRequest.md；
// DB_Find_Factory_Hub_User 无报告，objdump 0x08437c50 转写）。
// ---------------------------------------------------------------------------

// ORIG 0x0842ad30 sz 0x10f（_ZN23DB_ArrangeUserLoginInfo11makeRequestEjbj）：
// accId==0 直接返回；Acquire("DBThread.cpp",0x6ce9) → CStreamGuard(stream,true)
//   → <<0xee(int) → <<-1(int) → sig=GetInBuffer<SIG_ARRANGE_USER_LOGIN_INFO>()
//   → sig+0x00(uint)=accId、sig+0x04(byte)=flag、sig+0x08(uint)=uid
//   → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_18.inc"
                                          unsigned int uid)
{
    if (accId == 0)
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6ce9), true);
    guard.operator*()->operator<<((int)0xee);
    guard.operator*()->operator<<((int)-1);
    SIG_ARRANGE_USER_LOGIN_INFO* sig =
        guard->GetInBuffer<SIG_ARRANGE_USER_LOGIN_INFO>();
    sig->m_accId = accId;
    sig->m_flag = flag ? (char)1 : (char)0;
    sig->m_uid = uid;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0843351e sz 0xbd（_ZN25DB_BlackIPMonitorPartLoad11makeRequestEv）：
// Acquire("DBThread.cpp",0x7b3d) → CStreamGuard(stream,true)
//   → <<0x155(int) → <<-1(int) → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_19.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7b3d), true);
    guard.operator*()->operator<<((int)0x155);
    guard.operator*()->operator<<((int)-1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x08437c50 sz 0xfe（_ZN24DB_Find_Factory_Hub_User11makeRequestEjPct；
// objdump 转写，报告目录仅有 dispatch.md）：Acquire("DBThread.cpp",0x851e)
//   → CStreamGuard(stream,true) → <<0x1a0(int) → <<uid(int)
//   → sig=GetInBuffer<SIG_FIND_FACTORY_HUB_USER>()
//   → strncpy(sig+0x00, name, 0x1d) → *(ushort*)(sig+0x1e)=flag
//   → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_20.inc"
                                           unsigned short flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x851e), true);
    guard.operator*()->operator<<((int)0x1a0);
    guard.operator*()->operator<<((int)uid);
    SIG_FIND_FACTORY_HUB_USER* sig =
        guard->GetInBuffer<SIG_FIND_FACTORY_HUB_USER>();
    strncpy(sig->m_characName, name, 0x1d);
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0842f2ae sz 0xf1（_ZN30DB_GoblinPadUpdateValidityTime11makeRequestEjj）：
// accId==0 直接返回；Acquire("DBThread.cpp",0x7453) → CStreamGuard(stream,true)
//   → <<0x12c(int) → <<-1(int) → sig=GetInBuffer<SIG_GOBLIN_UPDATE_VALIDITYTIME>()
//   → sig+0x00(uint)=accId、sig+0x04(uint)=flag → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_21.inc"
                                                 unsigned int flag)
{
    if (accId == 0)
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7453), true);
    guard.operator*()->operator<<((int)0x12c);
    guard.operator*()->operator<<((int)-1);
    SIG_GOBLIN_UPDATE_VALIDITYTIME* sig =
        guard->GetInBuffer<SIG_GOBLIN_UPDATE_VALIDITYTIME>();
    sig->m_mId = accId;
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

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
                                 unsigned int c, unsigned int d,
                                 unsigned int e, char* msg)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xad59), true);
    guard.operator*()->operator<<((int)0x2b6);
    guard.operator*()->operator<<((int)-1);
    // ORIG 0x08432fe2 单槽：call GetInBuffer 后 `mov %eax,-0xc(%ebp)` 一次
    // 落栈（无 sig 中转槽双 spill 形）⇒ view cast 直落声明初始化（技巧14）。
    DB_HadesPunishView* v = reinterpret_cast<DB_HadesPunishView*>(
        guard->GetInBuffer<SIG_HADES_PUNISH>());
    v->m_mId = a;
    v->m_punishType = b;
    v->m_days = c;
    v->m_applyFlag = d;
    v->m_field10 = e;
    if (msg != 0)
        strcpy(v->m_msg, msg);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x08432fe2 sz 0xf1（_ZN20DB_SaveCleanpadPoint11makeRequestEji）：
// Acquire("DBThread.cpp",0x7aaa) → CStreamGuard(stream,true)
//   → <<0x154(int) → <<-1(int) → <<point(uint) → <<accId(int)
//   → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_23.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7aaa), true);
    guard.operator*()->operator<<((int)0x154);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)point);
    guard.operator*()->operator<<((int)accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0843a34e sz 0x113（_ZN32DB_SecuServiceUpdateValidityTime11makeRequestEjjtPc）：
// Acquire("DBThread.cpp",0x88d4) → CStreamGuard(stream,true)
//   → <<0x1c9(int) → <<-1(int)
//   → sig=GetInBuffer<SIG_SECURITY_CARD_SERVICE_VALIDITYTIME>()
//   → sig+0x00(uint)=accId、sig+0x04(uint)=type、sig+0x08(ushort)=flag
//   → strncpy(sig+0x0a, uid, 0x10) → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_24.inc"
                                                   unsigned int type,
                                                   unsigned short flag,
                                                   char* uid)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x88d4), true);
    guard.operator*()->operator<<((int)0x1c9);
    guard.operator*()->operator<<((int)-1);
    SIG_SECURITY_CARD_SERVICE_VALIDITYTIME* sig =
        guard.operator->()->GetInBuffer<SIG_SECURITY_CARD_SERVICE_VALIDITYTIME>();
    sig->m_mId = accId;
    sig->m_type = type;
    sig->m_flag = flag;
    strncpy(sig->m_uid, uid, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0842bce8 sz 0xf1（_ZN33DB_SecurityCardUpdateValidityTime11makeRequestEjj）：
// accId==0 直接返回；Acquire("DBThread.cpp",0x6ebb) → CStreamGuard(stream,true)
//   → <<0x100(int) → <<-1(int)
//   → sig=GetInBuffer<SIG_SECURITY_CARD_UPDATE_VALIDITYTIME>()
//   → sig+0x00(uint)=accId、sig+0x04(uint)=flag → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_25.inc"
                                                    unsigned int flag)
{
    if (accId == 0)
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6ebb), true);
    guard.operator*()->operator<<((int)0x100);
    guard.operator*()->operator<<((int)-1);
    SIG_SECURITY_CARD_UPDATE_VALIDITYTIME* sig =
        guard->GetInBuffer<SIG_SECURITY_CARD_UPDATE_VALIDITYTIME>();
    sig->m_mId = accId;
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0843f7ee sz 0x10b（_ZN23DB_UpdateCharacPlayTime11makeRequestEil）：
// characNo==0 或 playTick==0 直接返回；Acquire("DBThread.cpp",0x99a6)
//   → CStreamGuard(stream,true) → <<0x24e(int) → <<-1(int)
//   → <<characNo(int) → <<playTick(long) → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_26.inc"
{
    if (characNo == 0 || playTick == 0)
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x99a6), true);
    guard.operator*()->operator<<((int)0x24e);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((int)characNo);
    guard.operator*()->operator<<((long)playTick);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
// ---- StubS3A_deps 批：makeRequest 定义（声明在本 TU 类定义内）----

// ORIG 0x08426d18 sz 0xec（_ZN20DB_ReqQueryCharacInfo11makeRequestEiPKc，
// d_aa_body.cpp objdump 转写）：Acquire("DBThread.cpp",0x66eb)
//   → CStreamGuard(stream,true) → <<0xc4(int) → <<uid(int)
//   → GetInBuffer<SIG_QUERY_CHARAC_INFO> → strncpy(name,0x1d)
//   → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_27.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x66eb), true);
    guard.operator*()->operator<<((int)0xc4);
    guard.operator*()->operator<<((int)uid);
    SIG_QUERY_CHARAC_INFO* sig = guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>();
    strncpy(sig->m_pad, name, 0x1d);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0842b790 sz 0xdb（_ZN22DB_SecurityCardDisuse11makeRequestEj，
// d_aa_body.cpp objdump 转写）：Acquire("DBThread.cpp",0x6e2d)
//   → CStreamGuard(stream,true) → <<0xfe(int) → <<-1(int)
//   → GetInBuffer<SIG_SECURITY_CARD_DISUSE> → sig+0x00=accId
//   → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_28.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6e2d), true);
    guard.operator*()->operator<<((int)0xfe);
    guard.operator*()->operator<<((int)-1);
    SIG_SECURITY_CARD_DISUSE* sig =
        guard->GetInBuffer<SIG_SECURITY_CARD_DISUSE>();
    sig->m_mId = accId;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0843c138 sz 0xec（_ZN28DB_ReqQueryCharacInfoMailbox11makeRequestEiPKc，
// 报告 DB_ReqQueryCharacInfoMailbox/makeRequest.md 全文转写）：
// Acquire("DBThread.cpp",0x8ec5) → CStreamGuard(stream,true)
//   → <<0x1f6(int) → <<userIdx(int) → GetInBuffer<SIG_QUERY_CHARAC_INFO>
//   → strncpy(name,0x1d) → put(DB_Q(2))。Dispatchers.cpp:16783 调用。
#include "W8LT_DBTask/makeRequest_29.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8ec5), true);
    guard.operator*()->operator<<((int)0x1f6);
    guard.operator*()->operator<<((int)userIdx);
    SIG_QUERY_CHARAC_INFO* sig = guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>();
    strncpy(sig->m_pad, name, 0x1d);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}


// ---------------------------------------------------------------------------
// DB_SwapCreatureItem::makeRequest —— ORIG 0x0841fef2（报告
// docs/class_func_reports/DB_SwapCreatureItem/makeRequest.md 全文已读）：
// Acquire("DBThread.cpp",0x5662) → CStreamGuard(stream,true)
//   → guard << 0x8c(int) → guard << -1(int)
//   → sig = guard.GetInBuffer<SIG_SWAP_CREATURE_ITEM>()
//   → m_empty1=0、m_slot2=param_2、m_itemIdx1=param_1、m_empty2=1、
//     m_slot1=0、m_itemIdx2=0（逐字段独立重读 sig，O0 无 CSE）
//   → MsgQueueMgr::put(DB_Q(2), guard)（析构时入队）。
#include "W8LT_DBTask/makeRequest_30.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5662), true);
    guard.operator*()->operator<<((int)0x8c);
    guard.operator*()->operator<<((int)-1);
    SIG_SWAP_CREATURE_ITEM* sig =
        guard->GetInBuffer<SIG_SWAP_CREATURE_ITEM>();
    sig->m_empty1 = 0;
    sig->m_slot2 = slot2;
    sig->m_itemIdx1 = itemIdx1;
    sig->m_empty2 = 1;
    sig->m_slot1 = 0;
    sig->m_itemIdx2 = 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "W8LT_DBTask/makeRequest_31.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xada1), true);
    guard.operator*()->operator<<((int)0x2b7);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
// DB_MercenaryLogInfo::makeRequest —— ORIG T 0x08446340 逐指令转写
// （opcode 696(0x2b8) → -1 → accId/characNo/startSec/endSec(uint) →
//  status/periodIdx/areaIdx(char) → gold/itemIdx/rarity(int) → LOGDB_Q(4)）。
#include "W8LT_DBTask/makeRequest_32.inc"
                                      unsigned int startSec, unsigned int endSec,
                                      char status, char periodIdx, char areaIdx,
                                      int gold, int itemIdx, int rarity)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xadd0), true);
    guard.operator*()->operator<<((int)0x2b8);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(characNo);
    guard.operator*()->operator<<(startSec);
    guard.operator*()->operator<<(endSec);
    guard.operator*()->operator<<(status);
    guard.operator*()->operator<<(periodIdx);
    guard.operator*()->operator<<(areaIdx);
    guard.operator*()->operator<<(gold);
    guard.operator*()->operator<<(itemIdx);
    guard.operator*()->operator<<(rarity);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ORIG 0x08446196 sz 0x1a9：GetDBHandle(4,0) → 解包 m_id(uint)/charac_no(int)/
// start_time(int)/finish_time(int)/type(char)/period(char)/area(char)/gold(int)/
// item_id(int)/rarity(int) → NumberToString(m_id,0) → inSert into
// log_mercenary(m_id,charac_no,start_time,finish_time,type,period,area,gold,
// item_id,rarity) values(%s,%u,from_unixtime(%d),from_unixtime(%d),%d,%d,%d,
// %d,%d,%d) → 返回 exec(true)==1。
// [b28d-solo-2R] AE：P309 声明序翻案——char 组(type/period/area)由 int 组中间移到最后声明，
//   4.4 即复现 ORIG 的深槽共享布局（char 三字节落 -0x3b..-0x39，int 组 -0x20..-0x38 全等）；
//   原 NEAR(12) 槽号差全消。ORIG 0x8446196
#include "W8LT_DBTask/dispatch_16.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId;
    int characNo;
    int startTime;
    int finishTime;
    int gold;
    int itemId;
    int rarity;
    char type;
    char period;
    char area;

    *stream >> mId;
    *stream >> characNo;
    *stream >> startTime;
    *stream >> finishTime;
    *stream >> type;
    *stream >> period;
    *stream >> area;
    *stream >> gold;
    *stream >> itemId;
    *stream >> rarity;
    db->set_query("inSert into log_mercenary(m_id,charac_no,start_time,finish_time,type,period,area,gold,item_id,rarity) values(%s,%u,from_unixtime(%d),from_unixtime(%d),%d,%d,%d,%d,%d,%d)",
                  NumberToString(mId, 0), characNo, startTime, finishTime,
                  (int)type, (int)period, (int)area, gold, itemId, rarity);
    // ORIG 尾块 xor $1/test 物化（gcc4.1 `if(!call())return 0;return 1`）。
    if (!db->exec(true))
        return 0;
    return 1;
}

// ORIG 0x0844c40a sz 0xd7：GetDBHandle(13,0) 为空返 0；GetOutBuffer<
// SIG_CERASHOP_ERROR_LOG> 为空返 0；inSert into cerashop_err_log(m_id,
// charac_no,server_id,channel_no,type,err_code,goods_id,time) values(%u,%u,
// %u,%u,%d,%u,%d,now()) → 返回 exec(true)。
#include "W8LT_DBTask/dispatch_17.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_D,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    DB_CerashopErrorLogView* sig =
        reinterpret_cast<DB_CerashopErrorLogView*>(
            stream->GetOutBuffer<SIG_CERASHOP_ERROR_LOG>());
    if (sig == 0)
        return 0;
    db->set_query("inSert into cerashop_err_log(m_id, charac_no, server_id, channel_no, type, err_code, goods_id, time) values (%u, %u, %u, %u, %d, %u, %d, now())",
                  sig->m_mId, sig->m_characNo, sig->m_serverId,
                  sig->m_channelNo, (int)sig->m_type, sig->m_errCode,
                  sig->m_goodsId);
    return db->exec(true);
}
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

// ORIG T _Z24put_compressed_blob_dataP5MySQLiPciPS1_ @0x083fbd06：
// 函数局部 static CCompressedBlob（guard 0x943c1e8 / 实例 0x943d244）直传
// compress——与 ORIG guard/atexit 结构一致；gcc 额外生成 EH cleanup 保护
// （ORIG 该 TU 无异常处理路径，差异属结构性豁免）。
#include "W8LT_DBTask/put_compressed_blob_data_1.inc"
                              char** outStr)
{
    static CCompressedBlob compressed_blob;
    return compressed_blob.compress(db, fieldIdx, src, srcLen, outStr);
}

// ============================================================================
// DB_UpdateOneDayLetheSkill::dispatch @0x08433eca
// 状态机：NOT_USE_LETHE_STATE→ProcessInitLethe；USE_LETHE_STATE→ProcessUseLethe；
// SKILL_CONFIRM_STATE→ProcessSkillConfirm(槽位)；INIT_LETHE_SKILL→
// ProcessInitLetheSkill + 回写 m_state=INIT/m_state2=USE；无匹配直接返 0。
// 处理完成后 Acquire("DBThread.cpp",0x7cac) → <<userIdx → <<slot →
// SIG_PREMIUM_LETHE_FLAG{flag,state,skills(memcpy 0x199),treeKind} → put(DB_Q)。
// ============================================================================
#include "W8LT_DBTask/dispatch_18.inc"
{
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* out =
        stream->GetOutBuffer<SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD>();
    int flag = 1;
    if (out->m_state == CPremiumLetheManager::NOT_USE_LETHE_STATE)
    {
        if (!ProcessInitLethe(out))
        {
            flag = 0;
        }
    }
    else if (out->m_state == CPremiumLetheManager::USE_LETHE_STATE)
    {
        if (!ProcessUseLethe(out))
        {
            flag = 0;
        }
    }
    else if (out->m_state == CPremiumLetheManager::SKILL_CONFIRM_STATE)
    {
        if (!ProcessSkillConfirm(out, slot))
        {
            flag = 0;
        }
    }
    else if (out->m_state == CPremiumLetheManager::INIT_LETHE_SKILL)
    {
        if (!ProcessInitLetheSkill(out))
        {
            flag = 0;
        }
        out->m_state = CPremiumLetheManager::INIT_LETHE_SKILL;
        out->m_state2 = CPremiumLetheManager::USE_LETHE_STATE;
    }
    else
    {
        return 0;
    }

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7cac), true);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((int)slot);
    SIG_PREMIUM_LETHE_FLAG* p = guard->GetInBuffer<SIG_PREMIUM_LETHE_FLAG>();
    p->m_flag = flag;
    p->m_state = out->m_state;
    p->m_treeKind = out->m_treeKind;
    memcpy(p->m_skills, out->m_skills, 0x199);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_UpdateOneDayLetheSkill::ProcessInitLethe @0x0843410c
// GetDBHandle(3,0) → treeKind∈{0xff,0} 分支 lethe_flag/lethe_flag_2nd 更新
// （skill 表名 get_skill_table_name(false)）→ exec(true) 成败映射 bool。
// ============================================================================
#include "W8LT_DBTask/ProcessInitLethe.inc"
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    if (sig->m_treeKind == 0xff || sig->m_treeKind == 0)
    {
        db->set_query("upDate %s set lethe_flag=%u where charac_no=%u",
                      get_skill_table_name(false),
                      (int)(unsigned char)sig->m_state2, sig->m_characNo);
    }
    else
    {
        db->set_query("upDate %s set lethe_flag_2nd=%u where charac_no=%u",
                      get_skill_table_name(false),
                      (int)(unsigned char)sig->m_state2, sig->m_characNo);
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_UpdateOneDayLetheSkill::ProcessUseLethe @0x084341fc
// put_compressed_blob_data(sig+0xd, 0x198, &blob) → treeKind 分支
// skill_slot_lethe(_2nd)='blob', lethe_flag(_2nd)=m_state2 → exec。
// ============================================================================
#include "W8LT_DBTask/ProcessUseLethe.inc"
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    char* blob = 0;
    // ORIG 0x08434223 `add $0xd,%eax` 覆写基址 vs gcc4.4 decay `lea
    // 0xd(%eax),%edx` 新寄存器——add-vs-lea 固有差（N4/P30），保语义具名成员。
    if (!put_compressed_blob_data(
            db, 0, (char*)((unsigned long)sig + 0xdUL), 0x198, &blob))
    {
        return false;
    }
    if (sig->m_treeKind == 0xff || sig->m_treeKind == 0)
    {
        db->set_query("upDate %s set skill_slot_lethe='%s', lethe_flag=%u where charac_no=%u",
                      get_skill_table_name(false), blob,
                      (int)(unsigned char)sig->m_state2, sig->m_characNo);
    }
    else
    {
        db->set_query("upDate %s set skill_slot_lethe_2nd='%s', lethe_flag_2nd=%u where charac_no=%u",
                      get_skill_table_name(false), blob,
                      (int)(unsigned char)sig->m_state2, sig->m_characNo);
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_UpdateOneDayLetheSkill::ProcessSkillConfirm @0x08434340
// dbPremium=GetDBHandle(6,0) / dbSkill=GetDBHandle(3,0)；blob 写 skill 表
// （SKILL_CONFIRM_STATE 标志）→ cMyTrace(0x7d1a) → CDataManager+0xc
// GetRestrictCode(0xc5) → member_premium service_end=now() 更新
// （event_id=restrictCode, pre_type=0x21, m_id=NumberToString(accId), server_id
// =serverGroup）→ cMyTrace(0x7d29) → 1。
// ============================================================================
#include "W8LT_DBTask/ProcessSkillConfirm.inc"
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig, int /*slot*/)
{
    MySQL* dbPremium =
        GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                          (ENUM_SERVER_GROUP)0);
    MySQL* dbSkill =
        GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                          (ENUM_SERVER_GROUP)0);
    char* blob = 0;
    if (!put_compressed_blob_data(
            dbSkill, 0, (char*)((unsigned long)sig + 0xdUL), 0x198, &blob))
    {
        return false;
    }
    if (sig->m_treeKind == 0xff || sig->m_treeKind == 0)
    {
        dbSkill->set_query("upDate %s set skill_slot_lethe='%s', lethe_flag=%u  where charac_no=%u",
                           get_skill_table_name(false), blob,
                           (int)CPremiumLetheManager::SKILL_CONFIRM_STATE,
                           sig->m_characNo);
    }
    else
    {
        dbSkill->set_query("upDate %s set skill_slot_lethe_2nd='%s', lethe_flag_2nd=%u  where charac_no=%u",
                           get_skill_table_name(false), blob,
                           (int)CPremiumLetheManager::SKILL_CONFIRM_STATE,
                           sig->m_characNo);
    }
    if (!dbSkill->exec(true))
    {
        return false;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x7d1a, 0)(
        "ONE_DAY_LETHE : DB_CONFIRM_SKILL-FLAG_WRITE");
    int restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(0xc5);
    int preType = 0x21;
    // ORIG +0x?? `mov -0x20(%ebp),%eax` 迟装载紧贴 NumberToString arg1 spill
    // （unsigned 槽无强转 ⇒ 不提前物化，ORIG 0x08434340 体 +0xa8 段）。
    unsigned int accId = sig->m_accID;
    int serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    dbPremium->set_query("upDate member_premium set service_end=now() where event_id=%u and pre_type=%u and m_id=%s and server_id in(0,%u) and service_end>now()",
                         restrictCode, preType,
                         NumberToString(accId, 0),
                         serverGroup);
    if (!dbPremium->exec(true))
    {
        return false;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x7d29, 0)(
        "ONE_DAY_LETHE : DB_CONFIRM_SKILL-PREMIUM_END_CHANGE");
    return true;
}

// ============================================================================
// DB_UpdateOneDayLetheSkill::ProcessInitLetheSkill @0x084345a4
// 与 ProcessUseLethe 同构但 lethe_flag 用 USE_LETHE_STATE 常量，尾部
// cMyTrace(0x7d49, "ONE_DAY_LETHE : DB_INIT_LETHE_SKILL")。
// ============================================================================
#include "W8LT_DBTask/ProcessInitLetheSkill.inc"
    SIG_RESULT_PREMIUM_LETHE_SKILL_LOAD* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    char* blob = 0;
    if (!put_compressed_blob_data(
            db, 0, (char*)((unsigned long)sig + 0xdUL), 0x198, &blob))
    {
        return false;
    }
    if (sig->m_treeKind == 0xff || sig->m_treeKind == 0)
    {
        db->set_query("upDate %s set skill_slot_lethe='%s', lethe_flag=%u where charac_no=%u",
                      get_skill_table_name(false), blob,
                      (int)CPremiumLetheManager::USE_LETHE_STATE,
                      sig->m_characNo);
    }
    else
    {
        db->set_query("upDate %s set skill_slot_lethe_2nd='%s', lethe_flag_2nd=%u where charac_no=%u",
                      get_skill_table_name(false), blob,
                      (int)CPremiumLetheManager::USE_LETHE_STATE,
                      sig->m_characNo);
    }
    if (!db->exec(true))
    {
        return false;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x7d49, 0)(
        "ONE_DAY_LETHE : DB_INIT_LETHE_SKILL");
    return true;
}
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

// _CheckAlreadyGetYegamItem —— ORIG 0x0841a51a（size 0xcf）
// GetDBHandle(1,0) → 按 type 选 coin_check/item_check 查询 → exec 失败返 false；
// get_int 取到非零 id 返 true。
#include "W8LT_DBTask/_CheckAlreadyGetYegamItem.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    if (type == 1)
    {
        db->set_query("seLect coin_check from event_yegam_info where id=%d", id);
    }
    else
    {
        db->set_query("seLect item_check from event_yegam_info where id=%d", id);
    }
    if (!db->exec(true))
    {
        return 0;
    }
    int rowId = 0;
    if ((db->get_int(0, rowId) && rowId != 0) ? false : true)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// _UpdateEventYegamInfo —— ORIG 0x0841a5ea（size 0x77）
// 无显式返回（ORIG 函数体仅 leave/ret，eax 残留 exec 结果；调用方按单字节消费）。
#include "W8LT_DBTask/_UpdateEventYegamInfo.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    if (type == 1)
    {
        db->set_query("upDate event_yegam_info set coin_check=unix_timestamp(now()) where id=%d",
                      id);
    }
    else
    {
        db->set_query("upDate event_yegam_info set item_check=unix_timestamp(now()) where id=%d and item!=0",
                      id);
    }
    db->exec(true);
}

// _HardCodeEventItems —— ORIG 0x0841a662（size 0x181）
// 硬编码 event_code 0x65ae：先查 charac_event_items 是否已存在（id 非 0），
// 不存在则 insert，存在则 update stack_count。
// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
#include "W8LT_DBTask/_HardCodeEventItems.inc"
{
    // [b28-L11 fresh] 旧「je 目标块首对齐 nop 错位 ndiff 24」判词已由 nop 对齐
    // 剥离口径（§6.1）消解，fresh ndiff=0（AE 归一后逐条全等）。
    static const int handleIdx = 3;
    if (sig->m_field0c != 0x65ae)
    {
        return false;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect id from charac_event_items where charac_no=%d and it_id=%d",
                  sig->m_field04, sig->m_field0c);
    int id = 0;
    if ((db->exec(true) && db->get_n_rows() != 0 && db->fetch() &&
         db->get_int(0, id) && id != 0) ? false : true)
    {
        db->set_query("inSert into charac_event_items(charac_no,it_id,stack_count,event_code,reg_time,delete_time,delete_flag) values(%d,%d,%d,%d,now(),now(),1)",
                      sig->m_field04, sig->m_field0c, sig->m_field10, sig->m_field08);
        if (!db->exec(true))
        {
            return 1;
        }
    }
    else
    {
        db->set_query("upDate charac_event_items set stack_count=stack_count+1 where id=%d",
                      id);
        if (!db->exec(true))
        {
            return 1;
        }
    }
    return 1;
}

// dispatch —— ORIG 0x0841a7e4（size 0x34b）
// 活动类型 1..7 走对应 Check/Update 对（type 6 用 m_field00），失败返 false；
// 然后 _HardCodeEventItems；未命中硬编码且无事件标记时插 charac_event_items 表。
// [b28d-solo-2R] NEAR(5)：ORIG 先求前两实参（sig+4/sig+0）再载 loadId 入 %ecx；
// 4.4 恒先载 loadId（`mov -0x10,%ecx`）；语句表达式 `({loadId;})` 同形 ⇒ N18a/P75 装载调度封顶。
// [b28d-solo-4] P211/P315 同型化旋钮：loadId 由 int 改 unsigned int（与
//   LoadCharacEventItem 第 3 形参 unsigned 同型）⇒ 4.4 把该实参装载延后到
//   前两实参之后，复现 ORIG `mov -0x10,%ecx` 末位装载（探针 h2 实证）。
// ORIG 0x841a7e4
#include "W8LT_DBTask/dispatch_19.inc"
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    SIG_SAVE_EVENTITEM* sig = stream->GetOutBuffer<SIG_SAVE_EVENTITEM>();
    unsigned int loadId;
    bool done;
    if (sig->m_field14 != 0)
    {
        done = false;
        if (sig->m_field08 == 1)
        {
            if (_CheckAlreadyGetYegamItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = _UpdateEventYegamInfo(sig->m_field14, sig->m_field0c);
        }
        else if (sig->m_field08 == 2)
        {
            if (WongWork::EventATI::CheckAlreadyGetATIItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventATI::UpdateEventATIInfo(sig->m_field14, sig->m_field0c);
        }
        else if (sig->m_field08 == 3)
        {
            if (WongWork::EventSigong::CheckAlreadyGetSigongItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventSigong::UpdateEventSigongInfo(sig->m_field14, sig->m_field0c);
        }
        else if (sig->m_field08 == 4)
        {
            if (WongWork::EventEverland::CheckAlreadyGetEverlandItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventEverland::UpdateEventEverlandInfo(sig->m_field14, sig->m_field0c);
        }
        else if (sig->m_field08 == 5)
        {
            if (WongWork::EventMuz::CheckAlreadyGetMuzItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventMuz::UpdateEventMuzInfo(sig->m_field14, sig->m_field0c);
        }
        else if (sig->m_field08 == 6)
        {
            if (WongWork::EventAlttle::CheckAlreadyGetAlttleItem(sig->m_field00, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventAlttle::UpdateEventAlttleInfo(sig->m_field00, sig->m_field0c);
        }
        else if (sig->m_field08 == 7)
        {
            if (WongWork::EventFCCombo::CheckAlreadyFCComboEventItem(sig->m_field14, sig->m_field0c))
            {
                return false;
            }
            done = WongWork::EventFCCombo::UpdateFCComboEventInfo(sig->m_field14, sig->m_field0c);
        }
        if (!done)
        {
            return false;
        }
    }
    if (_HardCodeEventItems(sig))
    {
        return true;
    }
    if (sig->m_field18 != 0)
    {
        return true;
    }
    loadId = 0;
    if ((loadId = WongWork::EventCommon::InsertEventItem2EventTable(
             sig->m_field04, sig->m_field0c, sig->m_field10, sig->m_field08)) != 0)
    {
        // 残留（blocked）：ORIG `mov -0x10,%ecx` 单条提前提载先于 vtable 装载，
        // 4.4 恒在组栈位时才装载（或 register asm 反产三重拷贝，实测）——
        // N18a 类跨版本装载调度。
        WongWork::EventCommon::LoadCharacEventItem(sig->m_field00, sig->m_field04, loadId);
    }
    return true;
}

// ============================================================================
// DB_BillingRecord —— ORIG dispatch 0x08424574 / InsertBuyUser 0x08424602 /
// InsertSaleEntry 0x084247e0（计费：prod_buy_user 首购登记 + prod_sale_entry_
// <季度> 交易流水；句柄 GetDBHandle(DB_HANDLE_7, E_SERVER_GROUP_NONE)）。
// SIG 字段经 DB_BillingRecordSigView 组件视图强转访问（ORIG 每字段访问均
// 重载 sig 参数再 disp，视图表达式的 mov/add 形态与之一致）。
// ============================================================================

// ORIG 0x08424574：GetOutBuffer<SIG_BILLING_RECORD>；空指针记日志返 0；
// 否则 InsertBuyUser(置 bool 出参) → InsertSaleEntry(bool) → 返 1。
#include "W8LT_DBTask/dispatch_20.inc"
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_BILLING_RECORD* sig;
    sig = stream->GetOutBuffer<SIG_BILLING_RECORD>();
    if (sig != 0)
    {
        r = false;
        InsertBuyUser(sig, r);
        InsertSaleEntry(sig, r);
        return 1;
    }
    else
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x606e, "SIG_BILLING_RECORD is NULL");
        return 0;
    }
}

// ORIG 0x08424602：先 upDate 命中即返回；未命中（seLect 无行）再 inSert，
// 置 bool 出参为 true；三步任一 exec 失败记 logFormat 后返回。
#include "W8LT_DBTask/InsertBuyUser.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_7, E_SERVER_GROUP_NONE);
    result = false;
    db->set_query(
        " upDate prod_buy_user  set user_id = '%s', sex = %d, birthday = '%s', last_buy_time = now()  where m_id = %d ",
        ((DB_BillingRecordSigView*)sig)->m_userId, ((DB_BillingRecordSigView*)sig)->m_sex,
        ((DB_BillingRecordSigView*)sig)->m_birthday, ((DB_BillingRecordSigView*)sig)->m_mId);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x6117,
                              "[QUERY_ERROR] (upDate prod_buy_user m_id:%d)",
                              ((DB_BillingRecordSigView*)sig)->m_mId);
        return;
    }
    db->set_query("seLect m_id, user_id from prod_buy_user where m_id = %d",
                  ((DB_BillingRecordSigView*)sig)->m_mId);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x6120,
                              "[QUERY_ERROR] (seLect prod_buy_user m_id:%d)",
                              ((DB_BillingRecordSigView*)sig)->m_mId);
        return;
    }
    if (db->get_n_rows() == 0)
    {
        db->set_query(
            " inSert into prod_buy_user ( m_id, user_id, sex, birthday, first_buy_time, last_buy_time )  values(%d, '%s', %d, '%s', now(), now() ) ",
            ((DB_BillingRecordSigView*)sig)->m_mId, ((DB_BillingRecordSigView*)sig)->m_userId,
            ((DB_BillingRecordSigView*)sig)->m_sex, ((DB_BillingRecordSigView*)sig)->m_birthday);
        result = true;
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x6134,
                                  "[QUERY_ERROR] (inSert into prod_buy_user m_id:%d)",
                                  ((DB_BillingRecordSigView*)sig)->m_mId);
        }
    }
}

// ORIG 0x084247e0：季度表名 = "%02d%d"(tm_year+1900 后两位, tm_mon/3+1)；
// 主 insert 失败回退 prod_sale_entry_073 固定表，均失败记日志。
// [b28d-4] DIFF：N18a/P211：4.4 多 {'movl': 2} 中转 mov（单段直落/同型化待验）；ORIG 084247e0
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

// ORIG T 0x08449032：发 0x31f、uid、accId、eventId、won 到 DB_Q。
#include "W8LT_DBTask/makeRequest_33.inc"
                                      int eventId, bool won)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb75e), true);
    guard.operator*()->operator<<((int)0x31f);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(eventId);
    guard.operator*()->operator<<(static_cast<char>(won));
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_SavePvp —— ORIG dispatch 0x8417278 / SavePvp 0x84172c8
// （pvp_result 全字段保存；GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE)）。
// ============================================================================

// ORIG 0x8417278：GetOutBuffer<SIG_SAVE_PVP> → SavePvp 全绿返 1
// （无基类 dispatch 前置检查，同 DB_SaveSkill::dispatch）。
#include "W8LT_DBTask/dispatch_21.inc"
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 按声明序分配）。
    bool r;
    SIG_SAVE_PVP* sig;
    sig = stream->GetOutBuffer<SIG_SAVE_PVP>();
    if (sig == 0)
        return 0;
    r = SavePvp(sig);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x84172c8：读 m_result 前 0x68 的 22 个 int + CSystemTime::getCurSec()
// + charac_no，拼 25 占位 SQL（win..win_point/from_unixtime/play_count/play_time/
// charac_no）；exec 失败返 0。
#include "W8LT_DBTask/SavePvp.inc"
{
    // 声明序 s→r→db 与 ORIG 栈槽对应（-0x24/-0x1d/-0x1c）：s 为参数副本
    // （ORIG 先存 sig 到 -0x24 再 GetDBHandle）；characNo 直读参数 sig
    // （ORIG 该字段重读 0xc(%ebp)），其余字段经视图强转重读 s。
    SIG_SAVE_PVP* s = sig;
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate pvp_result set win=%d,lose=%d,pvp_point=%d,pvp_grade=%d,avg_kill_count=%d,"
        "avg_buf_count=%d,avg_debuf_count=%d,avg_heal_count=%d,avg_counter_count=%d,"
        "avg_back_atk_count=%d,avg_union_hit_count=%d,avg_overkill_count=%d,avg_combo_count=%d,"
        "avg_aerial_count=%d,avg_attacked_count=%d,avg_deal_damage=%d,avg_technic=%d,"
        "avg_style=%d,avg_hit_penalty=%d,pvp_count=%d,win_point=%d,last_play_time=from_unixtime(%d),"
        " play_count=%u,play_time=%u where charac_no=%u",
        ((DB_SavePvpResultView*)s)->m_win,
        ((DB_SavePvpResultView*)s)->m_lose,
        ((DB_SavePvpResultView*)s)->m_pvpPoint,
        ((DB_SavePvpResultView*)s)->m_pvpGrade,
        ((DB_SavePvpResultView*)s)->m_avg[0],
        ((DB_SavePvpResultView*)s)->m_avg[1],
        ((DB_SavePvpResultView*)s)->m_avg[2],
        ((DB_SavePvpResultView*)s)->m_avg[3],
        ((DB_SavePvpResultView*)s)->m_avg[4],
        ((DB_SavePvpResultView*)s)->m_avg[5],
        ((DB_SavePvpResultView*)s)->m_avg[6],
        ((DB_SavePvpResultView*)s)->m_avg[7],
        ((DB_SavePvpResultView*)s)->m_avg[8],
        ((DB_SavePvpResultView*)s)->m_avg[9],
        ((DB_SavePvpResultView*)s)->m_avg[10],
        ((DB_SavePvpResultView*)s)->m_avg[11],
        ((DB_SavePvpResultView*)s)->m_avg[12],
        ((DB_SavePvpResultView*)s)->m_avg[13],
        ((DB_SavePvpResultView*)s)->m_avg[14],
        ((DB_SavePvpResultView*)s)->m_pvpCount,
        ((DB_SavePvpResultView*)s)->m_winPoint,
        GlobalData::s_systemTime_.getCurSec(),
        ((DB_SavePvpResultView*)s)->m_playCount,
        ((DB_SavePvpResultView*)s)->m_playTime,
        sig->m_characNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}

// ============================================================================
// DB_SaveRank —— ORIG dispatch 0x84174c0 / SaveRankAll 0x8417510
// （charac_best_record 每行 inSert，冲突则 upDate）。
// ============================================================================

// ORIG 0x84174c0：GetOutBuffer<SIG_SAVE_RANK> → SaveRankAll 全绿返 1
// （无基类 dispatch 前置检查）。
#include "W8LT_DBTask/dispatch_22.inc"
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc）。
    bool r;
    SIG_SAVE_RANK* sig;
    sig = stream->GetOutBuffer<SIG_SAVE_RANK>();
    if (sig == 0)
        return 0;
    r = SaveRankAll(sig);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8417510：循环 i < sig->m_count（+0x04），每行 0x50 步长：
// inSert charac_best_record(charac_no,dungeon_no,difficulty,style,technic,
// attacked,rank)；inSert exec 失败改 upDate 同键；upDate 也失败返 0。
// [hc 收] (char*)sig + i*0x50 视图族：sig 为 SIG_SAVE_RANK 载荷线格式缓冲，
// 每行 0x50 步长字段经 DB_SaveRankRowView/StatsView 视图类型 cast 直读
// （§5.3 序列化缓冲豁免；ORIG 基址重算每字段同形）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08417510
#include "W8LT_DBTask/SaveRankAll.inc"
{
    // 声明序 db→i 与 ORIG 栈槽对应（-0x20/-0x1c）；无 row/sv 局部，
    // 每字段按 ORIG 视图基址重算（charac_no/count 直读参数）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    int i;
    for (i = 0; i < ((DB_SaveRankSigView*)sig)->m_count; ++i)
    {
        db->set_query(
            "inSert into charac_best_record (charac_no,dungeon_no,difficulty,style,technic,attacked,rank) values(%d,%d,%d,%d,%d,%d,%d)",
            ((DB_SaveRankSigView*)sig)->m_characNo,
            ((DB_SaveRankRowView*)((char*)sig + i * 0x50))->m_dungeonNo,
            ((DB_SaveRankRowView*)((char*)sig + i * 0x50))->m_difficulty,
            ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_style,
            ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_technic,
            ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_attacked,
            ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_rank);
        if (!db->exec(true))
        {
            db->set_query(
                "upDate charac_best_record set style=%d, technic=%d ,attacked=%d, rank=%d where charac_no=%d and dungeon_no=%d and difficulty=%d",
                ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_style,
                ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_technic,
                ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_attacked,
                ((DB_SaveRankStatsView*)((char*)sig + i * 0x50 + 0x40))->m_rank,
                ((DB_SaveRankSigView*)sig)->m_characNo,
                ((DB_SaveRankRowView*)((char*)sig + i * 0x50))->m_dungeonNo,
                ((DB_SaveRankRowView*)((char*)sig + i * 0x50))->m_difficulty);
            if (!db->exec(true))
                return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_Stat* 统计族 —— 5 个同构类：dispatch（基类检查 → GetOutBuffer<MSG_*> →
// LogWrite）+ LogWrite 恒返 1。ORIG 地址：
//   DB_StatBreakJar    dispatch 0x8417a7c / LogWrite 0x8417afa
//   DB_StatComposition dispatch 0x8417bec / LogWrite 0x8417c6a
//   DB_StatDungeonClear dispatch 0x8417cde / LogWrite 0x8417d5c
//   DB_StatGameRank    dispatch 0x8417dd0 / LogWrite 0x8417e4e
//   DB_StatPvpPlay     dispatch 0x8417ec2 / LogWrite 0x8417f40
// ============================================================================

// ORIG 0x8417a7c：基类 dispatch 前置检查后 GetOutBuffer<MSG_STAT_BREAKJAR>，
// LogWrite 全绿返 1（DBDispatcher::dispatch 失败返 0）。
#include "W8LT_DBTask/dispatch_23.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→msg 与 ORIG 栈槽对应（-0xd/-0xc）。
    bool r;
    MSG_STAT_BREAKJAR* msg;
    msg = stream->GetOutBuffer<MSG_STAT_BREAKJAR>();
    if (msg == 0)
        return 0;
    r = LogWrite(msg);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8417afa：恒返 1（mov $0x1; ret，参数未使用）。
#include "W8LT_DBTask/LogWrite.inc"
{
    return 1;
}

// ORIG 0x8417bec：同 StatBreakJar 形态，GetOutBuffer<MSG_STAT_COMPOSITON>。
#include "W8LT_DBTask/dispatch_24.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    bool r;
    MSG_STAT_COMPOSITON* msg;
    msg = stream->GetOutBuffer<MSG_STAT_COMPOSITON>();
    if (msg == 0)
        return 0;
    r = LogWrite(msg);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8417c6a：统计合成日志处理目前无额外副作用，恒返成功。
#include "W8LT_DBTask/LogWrite_1.inc"
{
    return 1;
}

// ORIG 0x8417b6e：恒返 1（mov $0x1; ret；与 DB_Stat* 族其他成员不同，
// 无基类 dispatch 前置检查、无 GetOutBuffer、不调 LogWrite）。
#include "W8LT_DBTask/dispatch_25.inc"
{
    return 1;
}

// ORIG 0x8417b78：恒返 1（mov $0x1; ret，参数未使用）。
#include "W8LT_DBTask/LogWrite_2.inc"
{
    return 1;
}

// ORIG 0x8417cde：同 StatBreakJar 形态，GetOutBuffer<MSG_STAT_DUNGEONCLEAR>。
#include "W8LT_DBTask/dispatch_26.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    bool r;
    MSG_STAT_DUNGEONCLEAR* msg;
    msg = stream->GetOutBuffer<MSG_STAT_DUNGEONCLEAR>();
    if (msg == 0)
        return 0;
    r = LogWrite(msg);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8417d5c：恒返 1。
#include "W8LT_DBTask/LogWrite_3.inc"
{
    return 1;
}

// ORIG 0x8417dd0：同 StatBreakJar 形态，GetOutBuffer<MSG_STAT_RANKRES>。
#include "W8LT_DBTask/dispatch_27.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    bool r;
    MSG_STAT_RANKRES* msg;
    msg = stream->GetOutBuffer<MSG_STAT_RANKRES>();
    if (msg == 0)
        return 0;
    r = LogWrite(msg);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8417e4e：恒返 1。
#include "W8LT_DBTask/LogWrite_4.inc"
{
    return 1;
}

// ORIG 0x8417ec2：同 StatBreakJar 形态，GetOutBuffer<MSG_STAT_PVPPLAY>。
#include "W8LT_DBTask/dispatch_28.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    bool r;
    MSG_STAT_PVPPLAY* msg;
    msg = stream->GetOutBuffer<MSG_STAT_PVPPLAY>();
    if (msg == 0)
        return 0;
    r = LogWrite(msg);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8417f40：恒返 1。
#include "W8LT_DBTask/LogWrite_5.inc"
{
    return 1;
}

// ============================================================================
// DB_UpdateChannel —— ORIG dispatch 0x8417fb4 / UpdateChannel 0x8418032
// （game_channel 在线人数/职业计数刷新）。
// ============================================================================

// ORIG 0x8417fb4：基类 dispatch 前置检查后 GetOutBuffer<SIG_UPDATE_CHANNEL>，
// UpdateChannel 全绿返 1。
#include "W8LT_DBTask/dispatch_29.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc）。
    bool r;
    SIG_UPDATE_CHANNEL* sig;
    sig = stream->GetOutBuffer<SIG_UPDATE_CHANNEL>();
    if (sig == 0)
        return 0;
    r = UpdateChannel(sig);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x8418032：gc_now(+0x04 short 符号扩展) + 8 职业计数 + hangame/nexon
// + where gc_no，11 占位 SQL；exec 失败返 0。
#include "W8LT_DBTask/UpdateChannel.inc"
{
    // 声明序 r→db 与 ORIG 栈槽对应（-0x1d/-0x1c）；每字段直接经视图强转
    // 访问参数 sig（ORIG 重读 0xc(%ebp)，P48 无 CSE）。
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate game_channel set gc_now=%d,gc_up_time=now(),gc_swordman_cnt=%d,gc_fighter_cnt=%d,"
        "gc_gunner_cnt=%d,gc_mage_cnt=%d,gc_priest_cnt=%d,gc_at_gunner_cnt=%d,gc_thief_cnt=%d,"
        "gc_hangame=%d,gc_nexon=%d where gc_no=%d",
        ((DB_UpdateChannelView*)sig)->m_gcNow,
        ((DB_UpdateChannelView*)sig)->m_swordmanCnt,
        ((DB_UpdateChannelView*)sig)->m_fighterCnt,
        ((DB_UpdateChannelView*)sig)->m_gunnerCnt,
        ((DB_UpdateChannelView*)sig)->m_mageCnt,
        ((DB_UpdateChannelView*)sig)->m_priestCnt,
        ((DB_UpdateChannelView*)sig)->m_atGunnerCnt,
        ((DB_UpdateChannelView*)sig)->m_thiefCnt,
        ((DB_UpdateChannelView*)sig)->m_hangame,
        ((DB_UpdateChannelView*)sig)->m_nexon,
        ((DB_UpdateChannelView*)sig)->m_gcNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}

// ============================================================================
// DB_UpdateGrowType —— ORIG dispatch 0x841819a / UpdateGrowType 0x84181ea
// （charac_info.grow_type 更新）。
// ============================================================================

// ORIG 0x841819a：GetOutBuffer<SIG_UPDATE_GROWTYPE> → UpdateGrowType 全绿返 1
// （无基类 dispatch 前置检查）。
#include "W8LT_DBTask/dispatch_30.inc"
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc）。
    bool r;
    SIG_UPDATE_GROWTYPE* sig;
    sig = stream->GetOutBuffer<SIG_UPDATE_GROWTYPE>();
    if (sig == 0)
        return 0;
    r = UpdateGrowType(sig);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x84181ea：grow_type(+0x04 movsbl 符号扩展) where charac_no(+0x00)；
// exec 失败返 0。
#include "W8LT_DBTask/UpdateGrowType.inc"
{
    // 声明序 r→db 与 ORIG 栈槽对应（-0xd/-0xc）；字段经视图强转直读参数。
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("upDate charac_info set grow_type=%d where charac_no=%u",
                  ((DB_UpdateGrowTypeView*)sig)->m_growType,
                  ((DB_UpdateGrowTypeView*)sig)->m_characNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}

// ============================================================================
// DB_UpdateQuest —— ORIG dispatch 0x8418bde / _updateQuests 0x8418c1c
// （new_charac_quest 任务状态保存：40 个 play/trigger 字段 + clear_quest）。
// ============================================================================

// ORIG 0x8418bde：-0xd bool 槽函数首条 movb $0 初始化 → GetOutBuffer<SIG_
// UPDATE_QUEST> 存 -0xc → cmpl $0/jne body（空指针 fall-through `return
// false`）→ body call _updateQuests 后 movzbl -0xd 返 bool 槽（值恒 0）。
#include "W8LT_DBTask/dispatch_31.inc"
{
    bool bResult = false;
    SIG_UPDATE_QUEST* sig = stream->GetOutBuffer<SIG_UPDATE_QUEST>();
    if (sig == 0)
        return false;
    _updateQuests(sig);
    return bResult;
}

// ORIG 0x8418c1c：GetDBHandle(2,0) → zipBuf[0x8cac] 清零 → compress_zip(
// zipBuf, zipLen, sig+0xa4, 30000) 失败 logFormat(0x4a5e) 返 false；
// +0x75d4 标志为 0 走无 clear_quest SQL，否则压缩串作 clear_quest='%s' 首占位；
// 40 个 play/trigger 字段 + urgentQuestIndex(+0x75d5) + quest_notify blob
// (blob_to_str(1, sig+0x75d9, 0x28)) + charac_no；exec 成功且 affected!=0 返 true。
// §6.2 巨件核销（b20 首判，b25 复验）：P167 11/11 [0缺0多]；gap 1→1；
//   残差归因 N82（帧槽平移）+ 40 个 play/trigger 字段 set_query 实参栈槽序
//   跨版本排布（详见 docs/asm_translation_patterns.md 负例表）。
//   语义与 ORIG 一致（upDate new_charac_quest 两分支 SQL + blob_to_str 载荷）
//   ⇒ 语义完成、byte 层 N 封顶。
// [b28d-4] AE：P183 去缓存（删 DB_UpdateQuestView* v 局部、85 处内联 sig 重读）+ P171 补两死局部(maxSrcLen=30000@-0x20 / origZipLen@-0x1c, ORIG 0x8418c48/0x8418c75)+ P222 交换 clear_quest 块序（ORIG 0x8418cf6 je 落 flag==0 块）
#include "W8LT_DBTask/_updateQuests.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    // ORIG 0x8418c48 `movl $0x7530,-0x20(%ebp)` 与 0x8418c75/78
    // `mov -0x28(%ebp),%eax; mov %eax,-0x1c(%ebp)`：两个写后未读的死局部
    // （全函数再无 -0x20/-0x1c 引用，objdump 实证）——P171/P232 照抄，
    // 同时把帧 0x8ed8→0x8edc 与 db(-0x24)/zipLen(-0x28) 槽位对齐 ORIG。
    int maxSrcLen = 30000;
    char zipBuf[0x8cac] = { 0 };
    unsigned int zipLen = 0x8cac;
    unsigned int origZipLen = zipLen;   // ORIG 0x8418c75/78 写后未读死槽
    (void)maxSrcLen;
    (void)origZipLen;
    if (!compress_zip(zipBuf, zipLen,
                      reinterpret_cast<const char*>(((DB_UpdateQuestView*)sig)->m_clearData),
                      30000))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__, 0x4a5e,
            "3RD_EXPEND : DB_UpdateQuest::DB ERROR!! charac_no(%u)",
            sig->m_characNo);
        return false;
    }
    // ORIG 0x8418cea/0x8418d07/0x8418d34 等处一律 `mov 0xc(%ebp),%eax;
    // add $0xa4,%eax` 重读 sig 参数（无视图指针局部）⇒ P183 去缓存，
    // 逐点内联；原 `DB_UpdateQuestView* v` 局部会多占 -0x1c 槽并挤出 Y 死槽。
    // ORIG 0x8418cf6 `test %al,%al; je <flag==0 块>`（0x8419132 在函数后段）
    // ⇒ 源码块序 = flag!=0 块在前、flag==0 块在后（4.4 对 then 恒前置）。
    if (((DB_UpdateQuestView*)sig)->m_clearFlag != 0)
    {
        db->set_query(
            "upDate new_charac_quest set clear_quest='%s', play_1=%d, play_1_trigger=%d, play_2=%d, play_2_trigger=%d, "
            "play_3=%d, play_3_trigger=%d, play_4=%d, play_4_trigger=%d, play_5=%d, play_5_trigger=%d,"
            "play_6=%d, play_6_trigger=%d, play_7=%d, play_7_trigger=%d, play_8=%d, play_8_trigger=%d, "
            "play_9=%d, play_9_trigger=%d, play_10=%d, play_10_trigger=%d, play_11=%d, play_11_trigger=%d, "
            "play_12=%d, play_12_trigger=%d, play_13=%d, play_13_trigger=%d, play_14=%d, play_14_trigger=%d, "
            "play_15=%d, play_15_trigger=%d,play_16=%d, play_16_trigger=%d, play_17=%d, play_17_trigger=%d, "
            "play_18=%d, play_18_trigger=%d, play_19=%d, play_19_trigger=%d, play_20=%d, play_20_trigger=%d,"
            "urgentQuestIndex=%d,quest_notify='%s' where charac_no=%u",
            db->blob_to_str(0, zipBuf, zipLen),
            ((DB_UpdateQuestView*)sig)->m_play[0], ((DB_UpdateQuestView*)sig)->m_playTrigger[0], ((DB_UpdateQuestView*)sig)->m_play[1], ((DB_UpdateQuestView*)sig)->m_playTrigger[1],
            ((DB_UpdateQuestView*)sig)->m_play[2], ((DB_UpdateQuestView*)sig)->m_playTrigger[2], ((DB_UpdateQuestView*)sig)->m_play[3], ((DB_UpdateQuestView*)sig)->m_playTrigger[3],
            ((DB_UpdateQuestView*)sig)->m_play[4], ((DB_UpdateQuestView*)sig)->m_playTrigger[4], ((DB_UpdateQuestView*)sig)->m_play[5], ((DB_UpdateQuestView*)sig)->m_playTrigger[5],
            ((DB_UpdateQuestView*)sig)->m_play[6], ((DB_UpdateQuestView*)sig)->m_playTrigger[6], ((DB_UpdateQuestView*)sig)->m_play[7], ((DB_UpdateQuestView*)sig)->m_playTrigger[7],
            ((DB_UpdateQuestView*)sig)->m_play[8], ((DB_UpdateQuestView*)sig)->m_playTrigger[8], ((DB_UpdateQuestView*)sig)->m_play[9], ((DB_UpdateQuestView*)sig)->m_playTrigger[9],
            ((DB_UpdateQuestView*)sig)->m_play[10], ((DB_UpdateQuestView*)sig)->m_playTrigger[10], ((DB_UpdateQuestView*)sig)->m_play[11], ((DB_UpdateQuestView*)sig)->m_playTrigger[11],
            ((DB_UpdateQuestView*)sig)->m_play[12], ((DB_UpdateQuestView*)sig)->m_playTrigger[12], ((DB_UpdateQuestView*)sig)->m_play[13], ((DB_UpdateQuestView*)sig)->m_playTrigger[13],
            ((DB_UpdateQuestView*)sig)->m_play[14], ((DB_UpdateQuestView*)sig)->m_playTrigger[14], ((DB_UpdateQuestView*)sig)->m_play[15], ((DB_UpdateQuestView*)sig)->m_playTrigger[15],
            ((DB_UpdateQuestView*)sig)->m_play[16], ((DB_UpdateQuestView*)sig)->m_playTrigger[16], ((DB_UpdateQuestView*)sig)->m_play[17], ((DB_UpdateQuestView*)sig)->m_playTrigger[17],
            ((DB_UpdateQuestView*)sig)->m_play[18], ((DB_UpdateQuestView*)sig)->m_playTrigger[18], ((DB_UpdateQuestView*)sig)->m_play[19], ((DB_UpdateQuestView*)sig)->m_playTrigger[19],
            ((DB_UpdateQuestView*)sig)->m_urgentIndex,
            db->blob_to_str(1, ((DB_UpdateQuestView*)sig)->m_questNotify, 0x28),
            ((DB_UpdateQuestView*)sig)->m_characNo);
    }
    else
    {
        db->set_query(
            "upDate new_charac_quest set play_1=%d, play_1_trigger=%d, play_2=%d, play_2_trigger=%d, "
            "play_3=%d, play_3_trigger=%d, play_4=%d, play_4_trigger=%d, play_5=%d, play_5_trigger=%d,"
            "play_6=%d, play_6_trigger=%d, play_7=%d, play_7_trigger=%d, play_8=%d, play_8_trigger=%d, "
            "play_9=%d, play_9_trigger=%d, play_10=%d, play_10_trigger=%d, play_11=%d, play_11_trigger=%d, "
            "play_12=%d, play_12_trigger=%d, play_13=%d, play_13_trigger=%d, play_14=%d, play_14_trigger=%d, "
            "play_15=%d, play_15_trigger=%d,play_16=%d, play_16_trigger=%d, play_17=%d, play_17_trigger=%d, "
            "play_18=%d, play_18_trigger=%d, play_19=%d, play_19_trigger=%d, play_20=%d, play_20_trigger=%d,"
            "urgentQuestIndex=%d,quest_notify='%s' where charac_no=%u",
            ((DB_UpdateQuestView*)sig)->m_play[0], ((DB_UpdateQuestView*)sig)->m_playTrigger[0], ((DB_UpdateQuestView*)sig)->m_play[1], ((DB_UpdateQuestView*)sig)->m_playTrigger[1],
            ((DB_UpdateQuestView*)sig)->m_play[2], ((DB_UpdateQuestView*)sig)->m_playTrigger[2], ((DB_UpdateQuestView*)sig)->m_play[3], ((DB_UpdateQuestView*)sig)->m_playTrigger[3],
            ((DB_UpdateQuestView*)sig)->m_play[4], ((DB_UpdateQuestView*)sig)->m_playTrigger[4], ((DB_UpdateQuestView*)sig)->m_play[5], ((DB_UpdateQuestView*)sig)->m_playTrigger[5],
            ((DB_UpdateQuestView*)sig)->m_play[6], ((DB_UpdateQuestView*)sig)->m_playTrigger[6], ((DB_UpdateQuestView*)sig)->m_play[7], ((DB_UpdateQuestView*)sig)->m_playTrigger[7],
            ((DB_UpdateQuestView*)sig)->m_play[8], ((DB_UpdateQuestView*)sig)->m_playTrigger[8], ((DB_UpdateQuestView*)sig)->m_play[9], ((DB_UpdateQuestView*)sig)->m_playTrigger[9],
            ((DB_UpdateQuestView*)sig)->m_play[10], ((DB_UpdateQuestView*)sig)->m_playTrigger[10], ((DB_UpdateQuestView*)sig)->m_play[11], ((DB_UpdateQuestView*)sig)->m_playTrigger[11],
            ((DB_UpdateQuestView*)sig)->m_play[12], ((DB_UpdateQuestView*)sig)->m_playTrigger[12], ((DB_UpdateQuestView*)sig)->m_play[13], ((DB_UpdateQuestView*)sig)->m_playTrigger[13],
            ((DB_UpdateQuestView*)sig)->m_play[14], ((DB_UpdateQuestView*)sig)->m_playTrigger[14], ((DB_UpdateQuestView*)sig)->m_play[15], ((DB_UpdateQuestView*)sig)->m_playTrigger[15],
            ((DB_UpdateQuestView*)sig)->m_play[16], ((DB_UpdateQuestView*)sig)->m_playTrigger[16], ((DB_UpdateQuestView*)sig)->m_play[17], ((DB_UpdateQuestView*)sig)->m_playTrigger[17],
            ((DB_UpdateQuestView*)sig)->m_play[18], ((DB_UpdateQuestView*)sig)->m_playTrigger[18], ((DB_UpdateQuestView*)sig)->m_play[19], ((DB_UpdateQuestView*)sig)->m_playTrigger[19],
            ((DB_UpdateQuestView*)sig)->m_urgentIndex,
            db->blob_to_str(1, ((DB_UpdateQuestView*)sig)->m_questNotify, 0x28),
            ((DB_UpdateQuestView*)sig)->m_characNo);
    }
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        return false;
    return true;
}

// ============================================================================
// DB_UpdateLogin —— ORIG dispatch 0x841826a（login_account_%d 登录登记）。
// ============================================================================

// ORIG 0x841826a：GetOutBuffer<SIG_UPDATE_LOGIN> → 表号取集成 PvP 通道 ?
// sig+0x08 : CEnvironment+0x378（m_serverEnvir.m_serverGroup）→ GetDBHandle
// (6,0) → upDate login_account_%d（m_channel_no=+4 / login_status=1 / now() /
// login_ip=+0x0c / m_id=NumberToString(+0)）；exec 失败或 affected==0 补
// inSert 新行（inSert exec 失败返 0）；全绿返 1。
// [b28d-4] NEAR：NEAR 封顶：助记符序列全同、仅栈槽地址/寄存器文本差（N82 槽号错位 / N7 寄存器轮换）；ORIG 0841826a
// [b28d-solo-4] NEAR 复核：残差 = tableNo 装载寄存器 %eax vs ORIG %edx（两段 set_query
//   各 1 处）+ 首段 login_ip/channelNo 的 %ecx/%ebx 角色互换（N7 寄存器轮换，446 后端下
//   仍存；第二段 esi/ebx 已与 ORIG 一致）⇒ N7/N82 封顶。
#include "W8LT_DBTask/dispatch_32.inc"
{
    SIG_UPDATE_LOGIN* sig = stream->GetOutBuffer<SIG_UPDATE_LOGIN>();
    if (sig == 0)
        return 0;
    int tableNo = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        tableNo = ((DB_UpdateLoginView*)sig)->m_serverGroup;
    else
        tableNo = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate login_account_%d set m_channel_no=%d,login_status=1,last_login_date=now(),login_ip='%s' where m_id=%s",
        tableNo, ((DB_UpdateLoginView*)sig)->m_channelNo,
        ((DB_UpdateLoginView*)sig)->m_loginIp,
        NumberToString(((DB_UpdateLoginView*)sig)->m_accId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into login_account_%d (m_id,m_channel_no,login_status,last_login_date,login_ip) values(%s,%d,1,now(),'%s')",
            tableNo, NumberToString(((DB_UpdateLoginView*)sig)->m_accId, 0),
            ((DB_UpdateLoginView*)sig)->m_channelNo,
            ((DB_UpdateLoginView*)sig)->m_loginIp);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_LoadMileage —— ORIG dispatch 0x8443d2c / DB_SaveMileage dispatch 0x8443c62
// （member_miles 里程表读写）。
// ============================================================================

// ORIG 0x8443d2c：GetOutBuffer<SIG_MILEAGE_STRUCT>（+0=m_id/+4=charac_no/
// +8=miles）→ GetDBHandle(1,0) 失败 cMyTrace(0xa7ae) 返 0 → seLect miles
// exec 失败 cMyTrace(0xa7b6) 返 0；无行 inSert 新行并 miles=0；有行
// fetch/get_int(0) 失败各 cMyTrace(0xa7cb/0xa7d2) 返 0；最后
// Acquire("DBThread.cpp",0xa7d8) → CStreamGuard(stream,true) → <<userIdx →
// <<slot → put_binary(sig,0xc) → put(DISPATCH_Q,guard) 返其结果。
#include "W8LT_DBTask/dispatch_33.inc"
{
    static const int handleIndex = 1;  // ORIG _ZZN14DB_LoadMileage8dispatch…E11handleIndex
    SIG_MILEAGE_STRUCT* sig = stream->GetOutBuffer<SIG_MILEAGE_STRUCT>();
    if (sig == 0)
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIndex, E_SERVER_GROUP_NONE);
    if (db == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa7ae, 5)(
            "[ERROR] Database Handle is NULL [index:%d]", 1);
        return 0;
    }
    db->set_query("seLect miles from member_miles where m_id = %u",
                  ((DB_MileageView*)sig)->m_mId);
    bool r = db->exec(true);
    if (!r)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa7b6, 5)(
            "[ERROR] Exec [m_id:%u][charac_no:%u]",
            ((DB_MileageView*)sig)->m_mId, ((DB_MileageView*)sig)->m_characNo);
        return 0;
    }
    // ORIG local_10：get_n_rows 结果先落 -0xc int 槽再判 0（缺此槽则整链
    // sig/db/trace 上移一格）。
    int rows = db->get_n_rows();
    if (rows == 0)
    {
        db->set_query(
            "inSert into member_miles (m_id, miles, daily_miles) values (%u, 0, 0)",
            ((DB_MileageView*)sig)->m_mId);
        db->exec(true);
        ((DB_MileageView*)sig)->m_miles = 0;
    }
    else
    {
        r = db->fetch();
        if (!r)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa7cb, 5)(
                "[ERROR] Fetch [m_id:%u][charac_no:%u]",
                ((DB_MileageView*)sig)->m_mId, ((DB_MileageView*)sig)->m_characNo);
            return 0;
        }
        r = db->get_int(0, ((DB_MileageView*)sig)->m_miles);
        if (!r)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa7d2, 5)(
                "[ERROR] Mileage [m_id:%u][charac_no:%u]",
                ((DB_MileageView*)sig)->m_mId, ((DB_MileageView*)sig)->m_characNo);
            return 0;
        }
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa7d8), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator->()->put_binary(sig, 0xc);
    return GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ORIG 0x8443c62：GetOutBuffer<SIG_MILEAGE_STRUCT> → GetDBHandle(1,0) 失败
// cMyTrace(0xa79a) 返 0 → upDate member_miles set miles=%d(+8) where m_id=%u
// (+0)；返回 exec(true)。
#include "W8LT_DBTask/dispatch_34.inc"
{
    static const int handleIndex = 1;  // ORIG _ZZN14DB_SaveMileage8dispatch…E11handleIndex
    SIG_MILEAGE_STRUCT* sig = stream->GetOutBuffer<SIG_MILEAGE_STRUCT>();
    if (sig == 0)
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIndex, E_SERVER_GROUP_NONE);
    if (db == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa79a, 5)(
            "[ERROR] Database Handle is NULL [index:%d]", 1);
        return 0;
    }
    db->set_query("upDate member_miles set miles = %d where m_id = %u",
                  ((DB_MileageView*)sig)->m_miles, ((DB_MileageView*)sig)->m_mId);
    return db->exec(true);
}

// ============================================================================
// DB_UpdateNatType / DB_UpdateNatTypeUser —— ORIG 0x841c74e / 0x841c7f6
// （NAT 打点统计：log_nat_daily 日表 / log_nat_stat 汇总表）。
// ============================================================================
// （NAT 打点统计：log_nat_daily 日表 / log_nat_stat 汇总表）。
// ============================================================================

// ORIG 0x841c74e：GetOutBuffer<SIG_NAT_TYPE>（无基类检查）→ 5 占位 inSert
// log_nat_daily(occ_time,nat_type,nat_type_other,success_cnt,fail_cnt)。
// 声明序 sig→db 与 ORIG 栈槽对应（-0x10/-0xc）；每字段直接经视图强转
// 访问 sig（ORIG 每字段重读 -0x10，P48 无 CSE）。
#include "W8LT_DBTask/dispatch_35.inc"
{
    SIG_NAT_TYPE* sig;
    MySQL* db;
    sig = stream->GetOutBuffer<SIG_NAT_TYPE>();
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into log_nat_daily(occ_time,nat_type,nat_type_other,success_cnt,fail_cnt) values(from_unixtime(%d),%d,%d,%d,%d)",
        ((DB_NatTypeView*)sig)->m_time,
        ((DB_NatTypeView*)sig)->m_natType,
        ((DB_NatTypeView*)sig)->m_natTypeOther,
        ((DB_NatTypeView*)sig)->m_successCnt,
        ((DB_NatTypeView*)sig)->m_failCnt);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ORIG 0x841c7f6：GetOutBuffer<SIG_NAT_TYPE_USER> → 3 占位 inSert
// log_nat_stat(occ_time,nat_type,nat_cnt)。声明序 sig→db（-0x10/-0xc）。
#include "W8LT_DBTask/dispatch_36.inc"
{
    SIG_NAT_TYPE_USER* sig;
    MySQL* db;
    sig = stream->GetOutBuffer<SIG_NAT_TYPE_USER>();
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into log_nat_stat(occ_time,nat_type,nat_cnt) values(from_unixtime(%d),%d,%d)",
        sig->m_time, sig->m_type, sig->m_count);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// DB_UpdateGoblinPadAdvicePwdRewardTime::dispatch —— ORIG 0x0843f5d2
// ---------------------------------------------------------------------------
// ORIG：GetDBHandle(6,0) → >>mId(uint) → NumberToString(mId,0) → set_query(
// "upDate member_mousepass set reward_time=0 where m_id=%s and enable_flag = '1'",
// s) → exec(true)。exec==1 且 getAffectedRowCount()!=0 → 返 1，否则返 0。
// 声明序 mId→db 与 ORIG 栈槽对应（-0x10/-0xc）；GetDBHandle 初始化先于
// mId=0（ORIG 0x06 先 call 后 movl $0）。
#include "W8LT_DBTask/dispatch_37.inc"
                                                     Stream* stream)
{
    // 无 bool 槽：`if (!exec || affected==0) return 0; return 1;` 使 gcc4.4 在
    // 聚合块直生 mov $1/jmp/mov $0 + test %al/je（探针实证与 ORIG 0x843f64e
    // 逐条同构；写作 return !(...) 会被 De Morgan 折叠丢 xor）。mId 占 -0x10。
    unsigned int mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    mId = 0;
    *stream >> mId;
    db->set_query(
        "upDate member_mousepass set reward_time=0 where m_id=%s and enable_flag = '1'",
        NumberToString(mId, 0));
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// DB_UpdateQuestShopBuyingStatusList::dispatch —— ORIG 0x0840568a
// ---------------------------------------------------------------------------
// ORIG：GetDBHandle(2,0) → GetOutBuffer<SIG_QUEST_SHOP_DATA> → upDate
// charac_quest_shop（29 列，实参序见下）→ exec 失败返 0；affected==0 时补
// inSert（30 列）→ inSert exec 失败返 0；全绿返 1。
// 字段映射（ORIG 逐偏移 movzwl/mov 实证）：m_qp(+4)/m_initCount(+8) uint；
// statusCounts 各槽 ushort；m_questPiece(+0xa6)/m_characNo(+0) uint。
#include "W8LT_DBTask/dispatch_38.inc"
                                                  Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 0x084056b3-0x084056c7：GetOutBuffer 结果先落临时槽 -0x20，
    // 再拷贝进 sig(-0x1c)（两跳具名中转形，4.4 直存会少一条）。
    SIG_QUEST_SHOP_DATA* rawSig = stream->GetOutBuffer<SIG_QUEST_SHOP_DATA>();
    SIG_QUEST_SHOP_DATA* sig = rawSig;
    db->set_query(
        "upDate charac_quest_shop set qp=%u, init_count=%u, max_hp=%d, max_mp=%d, psy_attack=%d, psy_defense=%d, mag_attack=%d, mag_defence=%d, move_speed=%d, attack_speed=%d, hp_regen=%d, mp_regen=%d, all_element_resist=%d, fire_element_resist=%d, water_element_resist=%d, light_element_resist=%d, dark_element_resist=%d, all_element_attack=%d,fire_element_attack=%d, water_element_attack=%d, light_element_attack=%d, dark_element_attack=%d, psy_critical=%d, mag_critical=%d, good_hit=%d, evasion=%d, hit_recovery=%d, separate_psy_mag_attack=%d, quest_piece=%d where charac_no=%u",
        sig->m_main.m_qp, sig->m_main.m_initCount,
        sig->m_main.m_data.m_maxHp, sig->m_main.m_data.m_maxMp,
        sig->m_main.m_data.m_psyAttack,
        sig->m_main.m_data.m_psyDefense, sig->m_main.m_data.m_magAttack,
        sig->m_main.m_data.m_magDefence, sig->m_main.m_data.m_moveSpeed,
        sig->m_main.m_data.m_attackSpeed, sig->m_main.m_data.m_hpRegen,
        sig->m_main.m_data.m_mpRegen, sig->m_main.m_data.m_allElementResist,
        sig->m_main.m_data.m_fireElementResist,
        sig->m_main.m_data.m_waterElementResist,
        sig->m_main.m_data.m_lightElementResist,
        sig->m_main.m_data.m_darkElementResist,
        sig->m_main.m_data.m_allElementAttack,
        sig->m_main.m_data.m_fireElementAttack,
        sig->m_main.m_data.m_waterElementAttack,
        sig->m_main.m_data.m_lightElementAttack,
        sig->m_main.m_data.m_darkElementAttack,
        sig->m_main.m_data.m_psyCritical, sig->m_main.m_data.m_magCritical,
        sig->m_main.m_data.m_goodHit, sig->m_main.m_data.m_evasion,
        sig->m_main.m_data.m_hitRecovery,
        sig->m_main.m_data.m_separatePsyMagAttack,
        sig->m_main.m_questPiece, sig->m_main.m_field0);
    // P168a①：ORIG 首段 `[call exec][xor$1][test %al][je 岛]` + 岛
    // `mov$0,%eax; jmp EPI` = bool callee `!` 裸 early-return（技巧1）。
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
                "inSert into charac_quest_shop(charac_no, qp, init_count, max_hp, max_mp, psy_attack, psy_defense, mag_attack, mag_defence, move_speed, attack_speed, hp_regen, mp_regen, all_element_resist, fire_element_resist, water_element_resist, light_element_resist, dark_element_resist, all_element_attack,fire_element_attack, water_element_attack, light_element_attack, dark_element_attack, psy_critical, mag_critical, good_hit , evasion , hit_recovery , separate_psy_mag_attack , quest_piece) values(%u, %u, %u, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
                sig->m_main.m_field0, sig->m_main.m_qp, sig->m_main.m_initCount,
                sig->m_main.m_data.m_maxHp, sig->m_main.m_data.m_maxMp,
                sig->m_main.m_data.m_psyAttack,
                sig->m_main.m_data.m_psyDefense,
                sig->m_main.m_data.m_magAttack,
                sig->m_main.m_data.m_magDefence,
                sig->m_main.m_data.m_moveSpeed,
                sig->m_main.m_data.m_attackSpeed,
                sig->m_main.m_data.m_hpRegen,
                sig->m_main.m_data.m_mpRegen,
                sig->m_main.m_data.m_allElementResist,
                sig->m_main.m_data.m_fireElementResist,
                sig->m_main.m_data.m_waterElementResist,
                sig->m_main.m_data.m_lightElementResist,
                sig->m_main.m_data.m_darkElementResist,
                sig->m_main.m_data.m_allElementAttack,
                sig->m_main.m_data.m_fireElementAttack,
                sig->m_main.m_data.m_waterElementAttack,
                sig->m_main.m_data.m_lightElementAttack,
                sig->m_main.m_data.m_darkElementAttack,
                sig->m_main.m_data.m_psyCritical,
                sig->m_main.m_data.m_magCritical,
                sig->m_main.m_data.m_goodHit, sig->m_main.m_data.m_evasion,
                sig->m_main.m_data.m_hitRecovery,
                sig->m_main.m_data.m_separatePsyMagAttack,
                sig->m_main.m_questPiece);
        // P168a①：二段落同 xor early-return 形。
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ---------------------------------------------------------------------------
// DB_InsertQueryCounterDb::dispatch —— ORIG 0x08425b0c
// ---------------------------------------------------------------------------
// ORIG：len=0/qid=0 → memset(buf,0,0x6001) → >>len → get_binary(buf,len) →
// GetDBHandle(2,0) → GetQueryCounter() → SaveQuery(buf,&qid)（失败返 0）→
// Acquire("DBThread.cpp",0x63c1) → CStreamGuard(stream,true) → <<0xbf →
// <<-1 → <<qid → <<len → put_binary(buf,len) →
// MsgQueueMgr::put(LOGDB_Q(4), guard) → 返 1。guard 具名局部产生 ORIG 的
// EH cleanup（_Unwind_Resume 重抛）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08425b0c
// [r3] 声明序改 qid→len（P331 声明序=槽序）：qid(-0x10)/len(-0x14) 复现
//   ORIG 08425b0c `lea -0x14(%ebp),%eax`（len 先读）⇒ ndiff 27→5、gap 不变。
//   残余 = 4.1 常量先装寄存器（`mov $0x6001,%edx; mov %edx,0x8(%esp)`）
//   vs 4.4 直写栈（N18a）⇒ 封顶。
#include "W8LT_DBTask/dispatch_39.inc"
{
    int qid = 0;
    int len = 0;
    // [P5S2W8LT r1] P312 数组初始化器：ORIG 0x08425b17 序列
    //   `movl $0x0,-0x10; movl $0x0,-0x14; lea -0x601d(%ebp),%eax;
    //    mov $0x6001,%edx; mov %edx,0x8(%esp); movl $0x0,0x4(%esp);
    //    mov %eax,(%esp); call memset@plt`
    // ⇒ 槽序 qid(-0x10)/len(-0x14)/buf(-0x601d) 由「声明序 qid,len,buf」决定，
    //   且 memset 由数组初始化器展开（4.4 对该形态恒走 `mov $imm,%edx` 中转；
    //   显式 memset(buf,0,0x6001) 恒直写立即数 ⇒ 差 1 条）。
    char buf[0x6001] = { '\0' };
    *stream >> len;
    stream->get_binary(buf, len);
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // [b28d-solo-W8LT_DBTask] ORIG 0x08425b9x：`call SaveQuery; xor $1,%eax;
    // test %al,%al; jne <mov $0; jmp>` ⇒ 原码 P101 早返 + 裸 bool 判定
    // （`==1` 比较形 4.4 反产无 xor 直判）；P0/P52 链式 GetQueryCounter
    // （ORIG 无 counter 槽往返，receiver 为表达式右值）。
    if (!db->GetQueryCounter()->SaveQuery(buf, qid))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x63c1), true);
    guard.operator*()->operator<<(0xbf);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(qid);
    guard.operator*()->operator<<(len);
    guard.operator->()->put_binary(buf, len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    return 1;
}

// ---------------------------------------------------------------------------
// DB_InsertQueryCounterLogdb::dispatch —— ORIG 0x08425cde
// ---------------------------------------------------------------------------
// ORIG：qid=0/len=0 → memset(buf,0,0x6001) → >>qid → >>len →
// get_binary(buf,len) → GetDBHandle(4,0) → GetQueryCounter() →
// RegisterQuery(buf,qid,0)；返回 RegisterQuery==1（xor $1/test 形态）。
// [b28d-solo-3] AE：P0/P52 链式 GetQueryCounter（无 counter 槽）+ P101 早返
// （if(!RegisterQuery)return 0; return 1;）+ 声明序 qid/len 前置 + `char
// buf[0x6001]={'\0'}`（P312 复现 ORIG lea;mov $0x6001,%edx;rep stos 慢路）。
// ORIG 08425cde。
#include "W8LT_DBTask/dispatch_40.inc"
{
    int qid = 0;
    int len = 0;
    char buf[0x6001] = { '\0' };
    *stream >> qid;
    *stream >> len;
    stream->get_binary(buf, len);
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    // P0/P52：ORIG 无 counter 槽往返（GetQueryCounter 返回值直作 this）⇒ 链式。
    if (!db->GetQueryCounter()->RegisterQuery(buf, qid, 0))
        return 0;
    return 1;
}

// ORIG 0x83ffdba：GetOutBuffer<SIG_AVATAR_INSERT> → InsertAvatarItem 失败返 0；
// 通过后 _SendResult(sig, slot)（无基类 dispatch 前置检查）。
#include "W8LT_DBTask/dispatch_41.inc"
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc）。
    bool r;
    SIG_AVATAR_INSERT* sig;
    sig = stream->GetOutBuffer<SIG_AVATAR_INSERT>();
    r = InsertAvatarItem(sig);
    if (!r)
        return 0;
    _SendResult(sig, slot);
    return 1;
}

// ORIG 0x83ffe0e：GetDBHandle(3,0)；组装 0xc000 SQL 缓冲（两份模板：
// 普通 expire_date 与 from_unixtime 版，m_period 为 0 用普通版）。
// m_period ∈ (6,0x1e] 时 expire_date = getCurSec() + period*86400，否则直接 period。
// set_query 变参 13 个（SQL 模板仅 11 个占位，末两参为 ORIG 遗留多余实参）。
// [SweepW8LT b28d] DIFF：gap 14（our 220 / ORIG 234）封顶。p167 17/19：
// EXTRA memcpy×2 —— ORIG 4.1 对 `memcpy(sql,"...",0xce/0xdd)` 内联为
// `mov $imm,%esi; mov $0x33/%0x37,%ecx; rep movsl` + 尾 word/byte（各 13 条），
// 4.4 -O0 恒 call memcpy（各 5 条）；memset 实参装载 ORIG 多 1 条/处。
// 结构体整体赋值已试劣化（N84③），长度/语义逐处核对一致。ORIG 083ffe0e
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
        if (((DB_AvatarInsertView*)sig)->m_period > 6 &&
            ((DB_AvatarInsertView*)sig)->m_period <= 0x1e) {
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
    char* jewel = db->blob_to_str(0, ((DB_AvatarInsertView*)sig)->m_jewel, 0x1e);
    db->set_query(q, ((DB_AvatarInsertView*)sig)->m_characNo,
                  ((DB_AvatarInsertView*)sig)->m_slot,
                  ((DB_AvatarInsertView*)sig)->m_itId, date,
                  ((DB_AvatarInsertView*)sig)->m_obtainFrom,
                  ((DB_AvatarInsertView*)sig)->m_abilityNo,
                  ((DB_AvatarInsertView*)sig)->m_agency,
                  ((DB_AvatarInsertView*)sig)->m_stat, jewel,
                  ((DB_AvatarInsertView*)sig)->m_hiddenOption,
                  ((DB_AvatarInsertView*)sig)->m_emblemEndurance,
                  ((DB_AvatarInsertView*)sig)->m_field4f,
                  ((DB_AvatarInsertView*)sig)->m_field51);
    bool r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x84001fe：GetDBHandle(3,0) → Acquire("DBThread.cpp",0xb18) →
// CStreamGuard(stream,true) → <<0x31(int) → <<slot(int) →
// sig=GetInBuffer<SIG_AVATAR_INSERT>() → memcpy(sig,src,0x53) →
// sig+0x4b=GetIdentity(db) → put(DISPATCH_Q,guard)。返回 void。
#include "W8LT_DBTask/_SendResult.inc"
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb18), true);
    guard.operator*()->operator<<((int)0x31);
    guard.operator*()->operator<<((int)slot);
    // ORIG 0x840022x store 区：`call; mov %eax,%edx; mov -0xc,%eax;
    // mov %edx,0x4b(%eax)`（商经 %edx+基址 reload）。cast LHS 形使 4.4 缓存
    // out 于 %ebx 跨 call 并折成单条 `mov %eax,0x4b(%ebx)`；view 型直存变量
    // + (int) 转换实参产 ORIG 三步形（探针 /tmp/w8lt_sr2.o::s4）。
    DB_AvatarInsertView* out =
        reinterpret_cast<DB_AvatarInsertView*>(
            guard.operator->()->GetInBuffer<SIG_AVATAR_INSERT>());
    memcpy(out, sig, 0x53);
    out->m_identity = (int)WongWork::DBCommon::GetIdentity(db);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ---------------------------------------------------------------------------
// DB_SaveDungeonClear —— ORIG 5 符号：C1 0x8419fec / D1 0x841a008 / D0 0x841a038 /
// dispatch 0x841a056 / SaveDungeonClear 0x841a0a6 / SendResult 0x841a1a2。
// ---------------------------------------------------------------------------

// ORIG 0x841a056：GetOutBuffer<SIG_SAVE_DUNGEON_CLEAR> 空指针返 0；
// SaveDungeonClear 失败返 0（无 SendResult 环节）。
#include "W8LT_DBTask/dispatch_42.inc"
{
    // 声明序 r→sig 与 ORIG 栈槽对应（-0xd/-0xc；gcc O0 首局部占最低槽）。
    bool r;
    SIG_SAVE_DUNGEON_CLEAR* sig = stream->GetOutBuffer<SIG_SAVE_DUNGEON_CLEAR>();
    if (sig == 0)
        return 0;
    r = SaveDungeonClear(sig);
    if (!r)
        return 0;
    return 1;
}

// ORIG 0x841a0a6：GetDBHandle(2,0) → upDate member_dungeon set dungeon='%s'
// where m_id=%s；exec 失败或 affected==0 时补 inSert into member_dungeon。
#include "W8LT_DBTask/SaveDungeonClear.inc"
{
    // ORIG 每字段重读参数（P86），不缓存视图局部。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("upDate member_dungeon set dungeon='%s' where m_id=%s",
                  ((DB_SaveDungeonClearView*)sig)->m_dungeon,
                  // [Lane31W8LTDBTaskView] CUser.h→CInventory.h 链入
                  // `NumberToString(unsigned long long,int)` 后，int 实参对
                  // (unsigned int)/(unsigned long long) 两候选歧义 ⇒ 显式同型化
                  // （m_id 语义即 uint，ORIG 无转换指令）。
                  NumberToString((unsigned int)((DB_SaveDungeonClearView*)sig)->m_mId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0)) {
        db->set_query("inSert into member_dungeon (m_id,dungeon) values (%s,'%s')",
                      NumberToString((unsigned int)((DB_SaveDungeonClearView*)sig)->m_mId, 0),
                      ((DB_SaveDungeonClearView*)sig)->m_dungeon);
        // ORIG 尾仅一个 `mov $1,%eax` 汇聚块 ⇒ inSert 失败直返 0，成功
        // fall-through 共享尾部 return 1。
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ORIG 0x841a1a2：Acquire("DBThread.cpp",0x4c23) → CStreamGuard(stream,true)
// → <<userIdx(int) → <<slot(int) → put_binary(sig,0x180c) → put(DISPATCH_Q) 返 1。
#include "W8LT_DBTask/SendResult_5.inc"
                                     SIG_SAVE_DUNGEON_CLEAR* sig)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4c23), true);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((int)slot);
    guard.operator->()->put_binary(sig, 0x180c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ---------------------------------------------------------------------------
// DB_SaveUserHandicap —— ORIG 符号：C1 0x844e702 / D1 0x845e8f0 / D2 0x845e920 /
// _getDBHandle 0x08427dfc / _getDBHandleForDevideAccountDB 0x08427e28 /
// dispatch 0x08427e54 / makeRequest 0x0842801a。
// ---------------------------------------------------------------------------

// ORIG 0x08427dfc：局部 idx=1 → GetDBHandle(idx, 0)（DB_HANDLE_1）。
#include "W8LT_DBTask/_getDBHandle.inc"
{
    // enum 型局部直传（无调用点强转）：复现 ORIG「先载 this 再重读 idx」入栈序
    // （证据：docs/class_func_reports/DB_SaveUserHandicap/_getDBHandle.md 0x08427e02+）。
    ENUM_DB_HANDLE_IDX idx = (ENUM_DB_HANDLE_IDX)1;
    return GlobalData::s_db_mgr->GetDBHandle(idx, (ENUM_SERVER_GROUP)0);
}

// ORIG 0x08427e28：同 _getDBHandle（idx=1，独立符号）。
#include "W8LT_DBTask/_getDBHandleForDevideAccountDB.inc"
{
    ENUM_DB_HANDLE_IDX idx = (ENUM_DB_HANDLE_IDX)1;
    return GlobalData::s_db_mgr->GetDBHandle(idx, (ENUM_SERVER_GROUP)0);
}

// ORIG 0x08427e54：GetDBHandle(1,0) → GetOutBuffer<SIG_UPDATE_USER_HANDICAP>；
// m_params[2](+0x10) 为 0 用永久 end_time，否则 adddate(now(),interval %d day)；
// upDate member_punish_info；exec 失败或 affected==0 时补 inSert；恒返 1。
#include "W8LT_DBTask/dispatch_43.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    SIG_UPDATE_USER_HANDICAP* sig = stream->GetOutBuffer<SIG_UPDATE_USER_HANDICAP>();
    if (sig->m_params[2] == 0) {
        db->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), end_time='9999-12-31 23:59:59', punish_value=%d, apply_flag=2 where m_id=%s and punish_type=%d",
            sig->m_params[1], NumberToString(sig->m_accId, 0), sig->m_params[0]);
    } else {
        db->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), end_time=adddate(now(),interval %d day), punish_value=%d, apply_flag=2 where m_id=%s and punish_type=%d",
            sig->m_params[2], sig->m_params[1], NumberToString(sig->m_accId, 0),
            sig->m_params[0]);
    }
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0)) {
        if (sig->m_params[2] == 0) {
            db->set_query(
                "inSert into member_punish_info(m_id, occ_time, start_time, end_time, punish_type, punish_value, apply_flag) values(%s, now(), now(),'9999-12-31 23:59:59', %d, %d, 2)",
                NumberToString(sig->m_accId, 0), sig->m_params[0],
                sig->m_params[1]);
        } else {
            db->set_query(
                "inSert into member_punish_info(m_id, occ_time, start_time, end_time, punish_type, punish_value, apply_flag) values(%s, now(), now(), adddate(now(),interval %d day), %d, %d, 2)",
                NumberToString(sig->m_accId, 0), sig->m_params[2],
                sig->m_params[0], sig->m_params[1]);
        }
        db->exec(true);
    }
    return 1;
}

// ---------------------------------------------------------------------------
// DB_SaveExpandEquipslot —— ORIG 3 符号
// （docs/class_func_reports/DB_SaveExpandEquipslot/：dispatch 0x0840da96 /
//  _SaveExpandEquipslot 0x0843ff48 / _SaveCharacStat 0x0843febe）
// ---------------------------------------------------------------------------

// [b28d-4] DIFF：N18a/P211：4.4 多 {'mov': 1} 中转 mov（单段直落/同型化待验）；ORIG 0843ff48
#include "W8LT_DBTask/_SaveExpandEquipslot.inc"
{
    char buf[0xa5c] = {0};
    unsigned int bufLen = 0xa5c;
    if (!compress_zip(buf, bufLen, (const char*)sig, 0x898))
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    // ORIG 0x0843ff48：characNo 无具名局部——首处 `mov (%eax),%ebx` 直落
    // callee-saved 跨 blob_to_str 保活、次处重新装载（P0/P183 表达式内联）。
    db->set_query(
        "upDate charac_inven_expand set expand_equipslot='%s' where charac_no=%u",
        db->blob_to_str(0, buf, (int)bufLen), sig->m_characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_inven_expand(charac_no, expand_equipslot) values(%u, '%s')",
            sig->m_characNo, db->blob_to_str(0, buf, (int)bufLen));
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ORIG 0x0843febe：GetDBHandle(DB_HANDLE_2, 0)；两 switch 以 (int)(char)
// 符号扩展进 upDate charac_stat；尾 bool 槽(-0xd 先声明)+P101 `if(!r)`
// 失败出口（xor $1/test/je/mov0/jmp/mov1）。
#include "W8LT_DBTask/_SaveCharacStat.inc"
                                             SIG_SAVE_EXPAND_CHARAC_STAT* sig)
{
    bool r;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_stat set channel_equipslot_switch=%d, expand_equipslot_switch=%d where charac_no = %u",
        (int)(char)sig->m_channelEquipslotSwitch,
        (int)(char)sig->m_expandEquipslotSwitch,
        characNo);
    r = db->exec(true);
    if (!r)
        return 0;
    return 1;
}

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
{
    SIG_SAVE_EXPAND_CHARAC* sig = stream->GetOutBuffer<SIG_SAVE_EXPAND_CHARAC>();
    if (!_SaveExpandEquipslot((SIG_SAVE_EXPAND_EQUIPSLOT*)sig))
        return 0;
    if (!_SaveCharacStat(((SIG_SAVE_EXPAND_EQUIPSLOT*)sig)->m_characNo,
                         &((DB_SaveExpandCharacView*)sig)->m_stat))
        return 0;
    return 1;
}

// ---------------------------------------------------------------------------
// DB_ReqQueryCharacInfo —— dispatch 0x0842688c / _getGiftRecvCount 0x08426f46
// （_sendResult 0x08426e04 定义在 OrigDBInter19.cpp；makeRequest 见本文件前部）
// ---------------------------------------------------------------------------

// ORIG T 0x08426f46（_getGiftRecvCount.md 全文）：GetDBHandle(DB_HANDLE_6,0)；
// lastGiftTime=0、giftCnt=-1；member_login 表查 last_gift_time,gift_cnt：
// exec 失败返 0；无行/fetch 失败返 -1；gift_cnt!=0 时比较 last_gift_time 与
// 当前时间的 tm_mon，跨月则 upDate 清零并返 0。
#include "W8LT_DBTask/_getGiftRecvCount.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    int lastGiftTime = 0;
    int giftCnt = -1;
    char table[0x14];
    memset(table, 0, 0x14);
    memcpy(table, "member_login", 0xd);
    db->set_query("seLect last_gift_time,gift_cnt from %s where m_id=%s",
                  table, NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    // ORIG 0x8427013-0x842701a：`test %eax,%eax; setne %al; test %al,%al;
    // je <共享尾 mov -0x14,%eax>` —— rows!=0 正向大块（技巧 19 int!=0 物化）
    // + giftCnt!=0 同尾 je(0x8427075)；tm 槽 tmLast=-0x5c > tmNow=-0x88 ⇒
    // tmLast 先声明（4.4 后声明占更低槽）。
    if (db->get_n_rows() != 0)
    {
        if (!db->fetch())
            return -1;
        db->get_int(0, lastGiftTime);
        db->get_int(1, giftCnt);
        if (giftCnt != 0)
        {
            time_t now = GlobalData::s_systemTime_.getCurSec();
            struct tm tmLast;
            struct tm tmNow;
            time_t t = lastGiftTime;
            localtime_r(&t, &tmLast);
            localtime_r(&now, &tmNow);
            if (tmLast.tm_mon != tmNow.tm_mon)
            {
                db->set_query(
                    "upDate %s set last_gift_time=unix_timestamp(now()),gift_cnt=0 where m_id=%s",
                    table, NumberToString(mId, 0));
                db->exec(true);
                giftCnt = 0;
            }
        }
    }
    return giftCnt;
}

// ORIG T 0x0842688c（dispatch.md 全文）：基类 dispatch 失败返 0；
// GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<SIG_QUERY_CHARAC_INFO>（首字节即
// 角色名）→ escape_string → charac_info 查 7 列；exec 失败 → _sendResult(2,...)；
// get_n_rows()==0 两次独立判定（P48 双调用）→ _sendResult(0x15,...)；
// fetch/get_int(0) 失败 → _sendResult(2,...)；delete_flag!=0 → 0x15；
// 成功 → 读列 1..6（m_id/charac_no/job/lev/grow_type/sex）→
// 成功路径 _getGiftRecvCount 前有 local_58[0x14] "member_login" 写后死存储
// （memset+memcpy 后未读，还原 ORIG 帧 sub $0xdc）→
// _getGiftRecvCount(m_id) 作尾参 → _sendResult(0, slot, sig, ...) → 返 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0842688c
#include "W8LT_DBTask/dispatch_45.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    SIG_QUERY_CHARAC_INFO* sig = stream->GetOutBuffer<SIG_QUERY_CHARAC_INFO>();

    char name[0x3c] = { '\0' };
    db->escape_string(name, (const char*)sig);
    db->set_query(
        "seLect delete_flag,m_id,charac_no,job,lev,grow_type,sex from charac_info where charac_name='%s'",
        name);
    if (!db->exec(true))
    {
        _sendResult(2, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }

    if (db->get_n_rows() == 0)
    {
        _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }
    // [P5S2W8LT r2] ORIG 0x08426a2f `test %eax,%eax; setne %al; je 0x08426cb6`：
    // 第 3 次 get_n_rows 检查是**正极性**（正文 fall-through），且
    // `_sendResult(0x15)` 块被本检查与 deleteFlag!=0 共用（块位 0x08426cb6 落在
    // 函数后部；第 2 次检查的 sete/0x084269cb 块保持原样）⇒ 原两次同形早退
    // 使此处产 sete 且块位错。
    if (db->get_n_rows() != 0)
    {
    // [G4W8LTDBTask 2026-09-10 负结论] 试删 7 处零初始化（P328⑤ 形）：
    // our 258→251（gap |+4|→|−3|）但 ndiff 47→53 劣化 ⇒ 已回滚。
    int deleteFlag = 0;
    if (!db->fetch() || !db->get_int(0, deleteFlag))
    {
        _sendResult(2, slot, 0, 0, 0, 0, 0, 0, 0);
        return 1;
    }
    if (deleteFlag == 0)
    {

    unsigned int mId = 0;
    int characNo = 0;
    int job = 0;
    int lev = 0;
    int growType = 0;
    int sex = 0;
    db->get_int(1, mId);
    db->get_int(2, characNo);
    db->get_int(3, job);
    db->get_int(4, lev);
    db->get_int(5, growType);
    db->get_int(6, sex);

    // ORIG 0x08426c0e(+0x37a) 实证：local_58[0x14] 缓冲区 memset+memcpy
    // "member_login" 后从未读取（写后死存储，-0x54(%ebp) 仅作 memset/memcpy
    // 目标）。为逐字节还原 ORIG 帧布局（sub $0xdc）与这两次调用保留之。
    char memberLogin[0x14];
    memset(memberLogin, 0, 0x14);
    memcpy(memberLogin, "member_login", 0xd);

    int giftCnt = _getGiftRecvCount(mId);
    _sendResult(0, slot, (const char*)sig, characNo, job, lev, growType, sex,
                giftCnt);
    return 1;
    }
    // [P5·2026-09-11] deleteFlag != 0 路径：ORIG 0x08426b2f 是**独立**的
    //   `_sendResult(0x15,…)` 块（其后 `jmp 08426d0d` 直落 epilogue），与函数尾
    //   0x08426d03 的 15 块**不共享** —— 批2 的共享尾块令我方少 1 个 call 点
    //   （p167fix 报 MISSING 1，ORIG 6 处 call vs 我方 5 处）。按 ORIG 拆回独立块：
    //   执行期两条路径仍各调 1 次（无「少发包」），仅恢复 call 点布局。
    _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0, 0);
    return 1;
    }
    _sendResult(0x15, slot, 0, 0, 0, 0, 0, 0, 0);
    return 1;
}

// ---------------------------------------------------------------------------
// DB_LoadKillMonsterInfo —— dispatch 0x084414e0 / LoadMonsterInfo 0x0844159a /
// SendResult 0x084419ac
// ---------------------------------------------------------------------------

// ORIG T _Z24get_compressed_blob_dataP5MySQLiPcii @0x083fbc79
// （定义 DB_Login.cpp:1967；本 TU 仅声明供 LoadMonsterInfo 调用）。
#include "W8LT_DBTask/get_compressed_blob_data.inc"
                              int recordSize);

// ORIG T 0x0844159a（LoadMonsterInfo.md 全文）：GetDBHandle(DB_HANDLE_2,0)；
// charac_kill_monster_info 按 charac_no 查 3 个 blob 列（boss/named/apc_boss）：
// select exec 失败/insert exec 失败/fetch 失败分别 cMyTrace 返 0；无行则
// inSert 新行返 1；各列 get_binary_length 判空，非空时 get_compressed_blob_data
// 解压至 sig 载荷 +0x000/0x1d4c/0x927c（0x1d4c/0x7530/0x1d4c），失败 trace 返 0。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 0844159a
// [L4-r1] 6 处 cMyTrace 改 P227 全表达式临时链 ⇒ ndiff 275→116（余 idx/col 槽序
//   + ok 的 P292 归一化，属 N18a/N82 族封顶）。
#include "W8LT_DBTask/LoadMonsterInfo.inc"
{
    static const int handleIdx = 2;
    // [SweepIntervalSet] ORIG 0x084415a4 `movb $0x0,-0x19(%ebp)`：ok 声明先于 db
    // 并初始化 false（原声明在 db 之后且未初始化 ⇒ 少一条 movb）。
    bool ok = false;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    // [SweepIntervalSet 实测回滚] ORIG 0x084415c8 `movl $0x5,-0x14(%ebp)` 疑为
    // dead 具名局部；补 `int unknown5=5;` 后 gap 7→6 但 ndiff 275→350 反劣
    // （4.4 槽序整体漂移）⇒ 该 5 非源级 dead 局部，回滚，残差归 N82/P210。
    db->set_query(
        "seLect boss_info, named_info, apc_boss_info from charac_kill_monster_info where charac_no=%u",
        sig->m_characNo);
    // [SweepIntervalSet] ORIG 0x084415a4 `movb $0x0,-0x19(%ebp)` = ok 在函数头
    // 初始化为 false（原声明未初始化）。
    ok = db->exec(true);
    if (!ok)
    {
        // [L4-r1] P227 全表达式临时链（ORIG 在 cMyTrace C1 前取 varargs 并跨 ctor
        // 保活 callee-saved；两语句形 4.4 只在 ctor 后求值 ⇒ 少 push）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa063, 5)(
            "DB_LoadKillMonsterInfo::LoadMonsterInfo select, exec() ERROR charac_no=%u",
            sig->m_characNo);
        return 0;
    }

    if (db->get_n_rows() == 0)
    {
        db->set_query(
            "inSert into charac_kill_monster_info (charac_no) values(%u)",
            sig->m_characNo);
        ok = db->exec(true);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa070, 5)(
                "DB_LoadKillMonsterInfo::LoadMonsterInfo insert, exec() ERROR charac_no=%u",
                sig->m_characNo);
            return 0;
        }
        return 1;
    }

    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xa07a, 5)(
            "DB_LoadKillMonsterInfo::LoadMonsterInfo select, fetch() ERROR charac_no=%u",
            sig->m_characNo);
        return 0;
    }

    int idx = 0;
    int len;
    int col;
    // 列 0：boss_info → sig+0x000，容量 0x1d4c
    len = db->get_binary_length(idx);
    // [SweepIntervalSet] P222 极性翻转：ORIG 0x08441793 `je <++idx 尾块>`（len==0
    // 落 0x0844182a），4.4 对 `if(len==0){++idx;}else{…}` 恒产 `jne else` 反向布局。
    if (len != 0)
    {
        // [J4S4-⑩a 回滚 2026-09-11] 删 col 改 `idx++` 实测反劣
        // （ndiff 66→75、our 236→233、D −4→−7）⇒ 恢复具名 col。
        col = idx;
        ++idx;
        // [J4S4-⑩b P0] ORIG 0x084417c8 `mov %al,-0x19(%ebp)` + `movzbl -0x19` ⇒
        // 返回值经既有 bool ok 承接（我方原直判返回寄存器，少 2 条/处）。
        ok = get_compressed_blob_data(db, col, reinterpret_cast<char*>(sig->m_boss), 0x1d4c, 0);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa087, 5)(
                "DB_LoadKillMonsterInfo::LoadMonsterInfo uncompress_zip(%u) ERROR charac_no=%u",
                idx, sig->m_characNo);
            return 0;
        }
    }
    else
    {
        ++idx;
    }
    // 列 1：named_info → sig+0x1d4c，容量 0x7530
    len = db->get_binary_length(idx);
    // [SweepIntervalSet] P222 极性翻转：ORIG 0x08441793 `je <++idx 尾块>`（len==0
    // 落 0x0844182a），4.4 对 `if(len==0){++idx;}else{…}` 恒产 `jne else` 反向布局。
    if (len != 0)
    {
        col = idx;
        ++idx;
        ok = get_compressed_blob_data(db, col, reinterpret_cast<char*>(sig->m_named), 0x7530, 0);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa09b, 5)(
                "DB_LoadKillMonsterInfo::LoadMonsterInfo uncompress_zip(%u) ERROR charac_no=%u",
                idx, sig->m_characNo);
            return 0;
        }
    }
    else
    {
        ++idx;
    }
    // 列 2：apc_boss_info → sig+0x927c，容量 0x1d4c
    len = db->get_binary_length(idx);
    // [SweepIntervalSet] P222 极性翻转：ORIG 0x08441793 `je <++idx 尾块>`（len==0
    // 落 0x0844182a），4.4 对 `if(len==0){++idx;}else{…}` 恒产 `jne else` 反向布局。
    if (len != 0)
    {
        col = idx;
        ++idx;
        ok = get_compressed_blob_data(db, col, reinterpret_cast<char*>(sig->m_apc), 0x1d4c, 0);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xa0af, 5)(
                "DB_LoadKillMonsterInfo::LoadMonsterInfo uncompress_zip(%u) ERROR charac_no=%u",
                idx, sig->m_characNo);
            return 0;
        }
    }
    else
    {
        ++idx;
    }
    return 1;
}

// ORIG T 0x084419ac（SendResult.md 全文）：BigStreamPool::Acquire("DBThread.cpp",
// 0xa0c1) → CStreamGuard(stream,true) → <<userIdx → <<slot →
// put_binary(sig,0xafcc) → s_msgq_mgr->put(DISPATCH_Q, guard) → 返 1。
#include "W8LT_DBTask/SendResult_6.inc"
                                        SIG_LOAD_KILL_MONSTER_INFO* sig)
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0xa0c1), true);

    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator->()->put_binary(sig, 0xafcc);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ORIG T 0x084414e0（dispatch.md 全文）：GetOutBuffer<SIG_LOAD_KILL_MONSTER_INFO>
// 为 null 返 0；LoadMonsterInfo 失败 cMyTrace 记录 charac_no 返 0；
// 成功 → SendResult(userIdx, slot, sig) → 返 1。
#include "W8LT_DBTask/dispatch_46.inc"
{
    // ORIG 0x084414e0：帧头 `movb $0,-0xd` 预置 bool（SendResult 结果落槽
    // 但不参与返回，P31 dead-slot 对齐）；失败分支 `mov 0xafc8(%eax),%ebx`
    // 前置缓存 charac_no 跨 ctor（P87）。
    bool r = false;
    SIG_LOAD_KILL_MONSTER_INFO* sig =
        stream->GetOutBuffer<SIG_LOAD_KILL_MONSTER_INFO>();
    if (sig == 0)
        return 0;

    if (!LoadMonsterInfo(sig))
    {
        register int cn = sig->m_characNo;
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa04b, 5);
        trace("DB_LoadKillMonsterInfo::LoadMonsterInfo ERROR charac_no=%u",
              cn);
        return 0;
    }
    r = SendResult(userIdx, slot, sig);
    return 1;
}

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

// [SweepW8LT b28d] DIFF：gap=-21（N82 槽差 + N7/N18a 寄存器分配）；ORIG 0843097e
// [M8DBA r5] gap −22 → +3：ORIG 1033 / our 1055 → 1036。
//   手法：badges/dailyBadges 共 12 处访问改「整型索引先行指针形」
//   `((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].x`
//   —— ORIG 直证（0x0843155b / 0x0843173f 区，如
//   `mov -0x1c(%ebp),%ecx; mov -0x20(%ebp),%edx; mov -0x34(%ebp),%ebx;
//    mov %edx,%eax; add %eax,%eax; add %edx,%eax; mov %ecx,%edx; shl $0x3,%edx;
//    lea 0x0(,%edx,8),%ecx; sub %edx,%ecx; add %ecx,%eax; lea (%ebx,%eax,1),%eax;
//    add $0x1d,%eax; movzbl (%eax),%eax`）：ORIG 是「3r+56p 整型和先行 + 加基址」，
//   我方旧形是 `(buf + p*0x38) + r*3` 分段（每处多 `mov %ecx,%ebx; sub; mov %ebx,%eax`
//   2 条，extra 计数实证 19 处）。12 处 × 2 = 24 条已消。
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

// ORIG 0x08434886：无基类 dispatch 前置检查（直接 Acquire 组包）。
// 流程：guard<<userIdx<<slot → GetInBuffer<SIG_*> → memset 0x80 →
// *stream>>characNo → GetCharacInfo 失败则 memset+cMyTrace(0x7eaf) →
// GetAvatarInfo 失败则 memset+cMyTrace(0x7eb7) → sig+0x00=characNo →
// MsgQueueMgr::put(DISPATCH_Q=1, guard) 返 1。
// [b28c-32] verdict=DIFF：P211 字段异型化（View::m_characNo→int 翻 RHS-first）；余 N18a 1 条 eax→edx 中转。
// [b28d-4] DIFF：P183/P171：ORIG 多 {'mov': 1}（漏具名局部/去缓存不足）；ORIG 08434886
#include "W8LT_DBTask/dispatch_48.inc"
{
    // ORIG 0x08434886 帧：guard=-0x34、characNo=-0x38（更远）、
    // trace1=-0x2c、trace2=-0x1c、sig=-0xc。实测 4.4 本函数：guard 先声明
    // 占更近槽、characNo 后声明反占远槽（对象优先），故 guard 在前。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7e9d), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    SIG_LOAD_POWER_WAR_STATUE_INFO* sig =
        guard->GetInBuffer<SIG_LOAD_POWER_WAR_STATUE_INFO>();
    memset(sig, 0, 0x80);
    // ORIG 0x08434…：`movl $0x0,-0x38` 声明即初始化（写后 stream>> 覆盖，
    // 值语义不变）。
    unsigned int characNo = 0;
    *stream >> characNo;
    if (!GetCharacInfo(sig, characNo))
    {
        memset(sig, 0, 0x80);
        // 技巧3：临时链式（具名对象致 sig 槽内收，ORIG trace=-0x2c）。
        cMyTrace(__PRETTY_FUNCTION__, 0x7eaf, 5)(
            "DB_LoadPowerWarStatueInfo::GetInvenInfo, GetCharacInfo ERROR charac_no=%u",
            characNo);
    }
    if (!GetAvatarInfo(sig, characNo))
    {
        memset(sig, 0, 0x80);
        cMyTrace(__PRETTY_FUNCTION__, 0x7eb7, 5)(
            "DB_LoadPowerWarStatueInfo::GetInvenInfo, exec GetInvenInfo charac_no=%u",
            characNo);
    }
    sig->m_characNo = characNo;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ORIG 0x08434a8a：charac db(DB_HANDLE_2) 查 charac_info（name/job/lev/
// grow_type/guild_id），失败返 0；guild_id==0 直接返 1；否则 guild db
// (DB_HANDLE_8) 查 guild_name，失败返 0。server_id 取 env+0x378。
#include "W8LT_DBTask/GetCharacInfo.inc"
    SIG_LOAD_POWER_WAR_STATUE_INFO* sig, unsigned int characNo)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    MySQL* guildDb = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect charac_name,job,lev,grow_type,guild_id from charac_info where charac_no=%d and delete_flag!=1",
        characNo);
    db->exec(true);
    if (!db->exec(true) || !db->fetch())
        return 0;
    if (db->get_n_rows() != 1)
        return 0;
    // 技巧36：删具名 v 中转（ORIG 每次 `mov 0xc(%ebp),%eax` 参数槽直读+
    // add disp，0x08434b62/0x08434b98/…；指针槽会多占 -0x10 使整帧平移）。
    if (!db->get_str(0, sig->m_name, 0x1d))
        return 0;
    if (!db->get_byte(1, sig->m_job))
        return 0;
    if (!db->get_byte(2, sig->m_lev))
        return 0;
    if (!db->get_byte(3, sig->m_growType))
        return 0;
    // ORIG 0x08434c22-0x08434c30：G_CEnvironment+0x378 取 serverId（-0x10）
    // 与死局部 -0xc 清零均发生在第 5 个 get_uint 之前（4.1 声明序）。
    // +0x378 = m_serverEnvir(+0x198).m_serverGroup(+0x1e0)；具名链访
    // ORIG 0x08434c27 `mov 0x378(%eax),%eax` 单 disp 折叠。
    int serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    int deadC = 0;
    (void)deadC;
    if (!db->get_uint(4, sig->m_guildId))
        return 0;
    if (sig->m_guildId == 0)
        return 1;
    guildDb->set_query(
        "seLect guild_name from guild_info where guild_id=%d and server_id=%d",
        sig->m_guildId, serverId);
    if (!guildDb->exec(true) || !guildDb->fetch())
        return 0;
    if (guildDb->get_n_rows() != 1)
        return 0;
    if (!guildDb->get_str(0, sig->m_guildName, 0x28))
        return 0;
    return 1;
}

// ORIG 0x08434d36：inventory db(DB_HANDLE_3) 读 equipslot blob →
// get_binary → uncompress_zip 解压到 items+0x262（失败/超限抛 unsigned int
// 0x7f1c/0x7f1e，catch 后 logFormat(0x7f29) 返 0）；解压后大小须为
// 0x3d 倍数或 0；info+0x7c=解压区 dword；再查 user_items（expire_date>now()
// and stat=0 limit 115）按 slot(0..9) 填 equipment；clear_avatar_id 非 0 时
// 再按 ui_id 二次查 it_id 覆盖对应槽。
// [b28d-4] DIFF：N1/N107：ORIG lea+add 两步寻址 vs 4.4 折单条；ORIG 08434d36
#include "W8LT_DBTask/GetAvatarInfo.inc"
    SIG_LOAD_POWER_WAR_STATUE_INFO* sig, unsigned int characNo)
{
    static const int handleIdx = 3;
    char buf[0x37a] = { '\0' };          // ORIG rep stos 清零（P4）
    unsigned int originalSize = 0x37a;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query("seLect equipslot from inventory where charac_no=%u", characNo);
    if (!db->exec(true) || !db->fetch())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x7f09, 5);
        trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, exec() or fetch() ERROR charac_no=%u",
              characNo);
        return 0;
    }
    int nItems = 0x73;                   // ORIG 死局部（-0x2c 只写不读，P31）
    Inven_Item items[22];                // ORIG 构造 0x15→-1 共 22 次（P34）
    int rows[0x73][3];                   // slot/it_id/clear_avatar_id × 115
    memset(rows, 0, sizeof(rows));
    int dbSize = db->get_binary_length(0);
    if (!db->get_binary(0, buf, 0x37a))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x7f15, 5);
        trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_binary(0) ERROR charac_no=%u",
              characNo);
        return 0;
    }
    try
    {
        if (dbSize > (int)originalSize)
            throw (unsigned int)0x7f1c;
        if (!uncompress_zip((char*)&items[10], originalSize, buf, dbSize))
            throw (unsigned int)0x7f1e;
    }
    catch (unsigned int e)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x7f29,
            "Uncompress failed. charac_no(%d)(%d)", characNo, e);
        return 0;
    }
    if (originalSize % 0x3d != 0 && originalSize != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x7f23, 5);
        trace("incorrect equipment size charac_no(%d), equipment, original_size(%d), db_size(%d)\n",
              characNo, 0x53e, dbSize);
        return 0;
    }
    SIG_LOAD_POWER_WAR_STATUE_INFO* v = sig;
    v->m_field7c =
        reinterpret_cast<DB_EquipslotBlobView*>(&items[10])->m_dword2;
    db->set_query(
        "seLect slot,it_id,clear_avatar_id from user_items where charac_no=%u and expire_date>now() and stat=0 limit %d",
        characNo, 115);
    if (!db->exec(true))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x7f35, 5);
        trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, exec() ERROR charac_no=%u",
              characNo);
        return 0;
    }
    int nRows = db->get_n_rows();
    for (int i = 0; i < nRows; ++i)
    {
        if (!db->fetch())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x7f3f, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, exec() ERROR charac_no=%u",
                  characNo);
            break;
        }
        if (!db->get_int(0, rows[i][0]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x7f45, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(0) ERROR charac_no=%u",
                  characNo);
            return 0;
        }
        if (!db->get_int(1, rows[i][1]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x7f4b, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(1) ERROR charac_no=%u",
                  characNo);
            return 0;
        }
        if (!db->get_int(2, rows[i][2]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x7f51, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(2) ERROR charac_no=%u",
                  characNo);
            return 0;
        }
        if (rows[i][0] >= 0 && rows[i][0] <= 9)
            v->m_equipment[rows[i][0]] = rows[i][1];
    }
    for (int j = 0; j < nRows; ++j)
    {
        if (rows[j][2] != 0)
        {
            db->set_query("seLect it_id from user_items where ui_id=%d", rows[j][2]);
            if (db->exec(true) && db->fetch())
                db->get_int(0, v->m_equipment[rows[j][0]]);
        }
    }
    return 1;
}

// ============================================================================
// DB_LastMsgDb —— ORIG dispatch 0x08421bcc（仅此一符号）。
// ============================================================================

// ORIG 0x08421bcc：StreamPool::Acquire("DBThread.cpp",0x59ed) →
// CStreamGuard(stream,true) → <<0x97(int) → <<stream(int)（原始 Stream* 指针值）
// → MsgQueueMgr::put(DISPATCH_Q=1,guard) → 返 1。无基类 dispatch 前置检查。
#include "W8LT_DBTask/dispatch_49.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x59ed), true);
    **guard << 0x97;
    // ORIG 0x8421c3f：`mov 0x10(%ebp),%edx` = 形参 slot（非 stream 指针）。
    **guard << slot;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_LastMsgLogDb —— ORIG dispatch 0x08421c8e（仅此一符号）。
// ============================================================================

// ORIG 0x08421c8e：同 DB_LastMsgDb 形态，Acquire("DBThread.cpp",0x59f7) →
// CStreamGuard(stream,true) → <<0x97(int) → <<stream(int)（原始 Stream* 指针值）
// → MsgQueueMgr::put(DISPATCH_Q=1,guard) → 返 1。无基类 dispatch 前置检查。
#include "W8LT_DBTask/dispatch_50.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x59f7), true);
    **guard << 0x97;
    **guard << slot;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_InsertQueueSizeLog —— ORIG dispatch 0x084225a8（仅此一符号）。
// ============================================================================

// ORIG 0x084225a8：*stream >> channelNo(-0x20) >> dbq(-0x24) →
// GetDBHandle(DB_HANDLE_4,0) → set_query("inSert into dnf_dbq_log(occ_time,
// channel_no,dbq) values(NOW(),%d,%d)", dbq, channelNo)（ORIG 实参序即
// dbq 在前 channelNo 在后，与 SQL 占位序相反）→ exec(true) 失败
// cMyTrace(0x5ad8,5) "DB_InsertQueueSizeLog, exec ERROR" 返 0；成功返 1。
#include "W8LT_DBTask/dispatch_51.inc"
{
    int channelNo = 0;
    int dbq = 0;
    // ORIG 0x08422…：channelNo(-0x20)/dbq(-0x24) 远槽、trace 临时对象
    // 复用近槽 -0x1c（CountOfHumanCertify 同法：P37 临时形翻槽序）。
    *stream >> channelNo;
    *stream >> dbq;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into dnf_dbq_log(occ_time,channel_no,dbq) values(NOW(),%d,%d)",
        dbq, channelNo);
    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x5ad8, 5)(
            "DB_InsertQueueSizeLog, exec ERROR");
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_LoadPowerWarInfo —— ORIG dispatch 0x08437756（仅此一符号）。
// ============================================================================

// ORIG 0x08437756：空处理，恒返回 1（mov $1,%eax; ret）。ORIG 帧 sub $0x40
// 为编译器伪影（函数体无局部访问），保留一个未用局部对齐帧（P31）。
#include "W8LT_DBTask/dispatch_52.inc"
{
    char buf[0x40];
    (void)buf;
    // [N8W8LT P139] ORIG 0x08437756 体仅 13B（`sub $0x40 / mov $1 / leave / ret`），
    // 但 ORIG nm 有本函数 prett 局部串 `_ZZN19DB_LoadPowerWarInfo8dispatchEiiP6Stream
    // E19__PRETTY_FUNCTION__`（0x08c5bd80, sz 0x3e，内容 `virtual bool
    // DB_LoadPowerWarInfo::dispatch(int, int, Stream*)`）⇒ 原码有被丢弃的引用。
    // 探针实证：`(void)sizeof(__PRETTY_FUNCTION__)` 只发射该串、零指令变化。
    (void)sizeof(__PRETTY_FUNCTION__);
    return 1;
}

// ============================================================================
// DB_FindFactoryHub —— ORIG dispatch 0x08436ba0（仅此一符号）。
// ============================================================================

// ORIG 0x08436ba0：GetOutBuffer<SIG_FIND_FACTORY_HUB> → GetDBHandle(
// DB_HANDLE_2,0)；char escName[0x3b]={0}（P4 内联清零）→ escape_string(
// escName,&sig->m_pad[0x4]) → set_query("seLect m_id from charac_info where
// charac_name='%s' limit 1", escName) → exec/fetch/get_int(0,&mId) 任一失败
// 返 0；sig->m_id!=mId 时 Acquire("DBThread.cpp",0x8253) → guard <<0x18a
// <<stream(int) → GetInBuffer<SIG_FIND_FACTORY_HUB_FATIGUE_CHECK>：
// +0x00=mId(uint)、+0x04=sig[0x21](byte)、strncpy(+0x05,&sig->m_pad[0x4],0x1d)
// → put(DISPATCH_Q=1,guard)；全绿返 1。无基类 dispatch 前置检查。
#include "W8LT_DBTask/dispatch_53.inc"
{
    SIG_FIND_FACTORY_HUB* sig = stream->GetOutBuffer<SIG_FIND_FACTORY_HUB>();
    unsigned int mId = 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    char escName[0x3b] = { '\0' };
    db->escape_string(escName, sig->m_characName);
    db->set_query(
        "seLect m_id from charac_info where charac_name='%s' limit 1",
        escName);
    if (!db->exec(true))
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_int(0, mId))
        return 0;
    if (sig->m_mId != mId)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8253), true);
        **guard << 0x18a;
        // ORIG 0x8436d41：`mov 0x10(%ebp),%edx` = 形参 slot。
        **guard << slot;
        SIG_FIND_FACTORY_HUB_FATIGUE_CHECK* out =
            guard->GetInBuffer<SIG_FIND_FACTORY_HUB_FATIGUE_CHECK>();
        out->m_flag = sig->m_field21;
        out->m_mId = mId;
        strncpy(out->m_characName, sig->m_characName, 0x1d);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return 1;
}

// ============================================================================
// DB_CancelRestingUserRestrict —— ORIG dispatch 0x0843712e（仅此一符号）。
// ============================================================================

// ORIG 0x0843712e：GetDBHandle(DB_HANDLE_1,0) → GetOutBuffer<
// SIG_RESTING_USER_RESTRICT> → set_query("upDate member_punish_info set
// apply_flag = 0 where m_id = %s and punish_type = 10", NumberToString(mid,0))
// → exec 失败返 0；成功 Acquire("DBThread.cpp",0x835c) → guard <<0x195
// <<stream(int) → put(DISPATCH_Q=1,guard)；再 GetDBHandle(DB_HANDLE_1,0)：
// char reason[0xff]={0}（P4）→ memcpy(reason, EUC-KR
// "|*|휴면 제재 해제|*|보안팀", 0x1b)（DAT_08c53b62，27 字节含 NUL）→
// time_t curSec = s_systemTime_.getCurSec() → localtime_r(&curSec,&tm) →
// strftime(year,10,"%Y",&tm) → set_query("upDate
// member_punish_info_history_%s set apply_flag = 0 , reason = '%s' where
// m_id = %s and punish_type = %d and apply_flag = 1", year, reason,
// NumberToString(mid,0), 10) → ok=exec(true)；ok 时 cMyTrace(0x8381,0)
// "CANCEL OK - Resting User Restrict (mid:%s)"；返 ok。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0843712e
#include "W8LT_DBTask/dispatch_54.inc"
                                            Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    SIG_RESTING_USER_RESTRICT* sig =
        stream->GetOutBuffer<SIG_RESTING_USER_RESTRICT>();
    db->set_query(
        "upDate member_punish_info set apply_flag = 0 where m_id = %s and punish_type = 10",
        NumberToString(sig->m_uid, 0));
    if (!db->exec(true))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x835c), true);
    **guard << 0x195;
    **guard << slot;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    char reason[0xff] = { '\0' };
    memcpy(reason,
           "|*|\xC8\xDE\xB8\xE9 \xC1\xA6\xC0\xE7 \xC7\xD8\xC1\xA6|*|\xBA\xB8\xBE\xC8\xC6\xC0",
           0x1b);
    char year[10] = { '\0' };
    time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
    struct tm tmNow;
    localtime_r(&curSec, &tmNow);
    strftime(year, 10, "%Y", &tmNow);
    db2->set_query(
        "upDate member_punish_info_history_%s set apply_flag = 0 , reason = '%s' where m_id = %s and punish_type = %d and apply_flag = 1",
        year, reason, NumberToString(sig->m_uid, 0), 10);
    // ORIG 0x08437380：`xor $1; test %al; je <trace>` + `mov $0/%1,%ebx` 常数岛，
    // 值跨 guard dtor 留 %ebx（尾 `mov %ebx,%eax`）⇒ 早返形（P16/P59）；
    // trace 槽 -0x34（浅于 guard -0x3c）⇒ P221 全表达式临时链。
    if (!db2->exec(true))
        return false;
    cMyTrace(__PRETTY_FUNCTION__, 0x8381, 0)(
        "CANCEL OK - Resting User Restrict (mid:%s)",
        NumberToString(sig->m_uid, 0));
    return true;
}
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

// ORIG 0x08436dec：GetOutBuffer<SIG_HACK_IP_SAVE> → GetDBHandle(6,0)。
// 每表先 upDate 计数，exec 失败或无影响行时 inSert 新行；insert exec 失败
// 分别 logFormat(0x829d/"DB_HackIPSave DB ERROR"、0x82af/FullIP) 返 0；
// 全部成功返 1。
#include "W8LT_DBTask/dispatch_55.inc"
{
    DB_HackIPSaveView* sig =
        (DB_HackIPSaveView*)stream->GetOutBuffer<SIG_HACK_IP_SAVE>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate auto_punish_hack_ip set cnt = cnt+1 where occ_date = CAST(DATE_FORMAT(now(), '%%Y-%%m-%%d') AS CHAR) and hack_type = %d and hack_sub_type = %d and c_class_ip = '%s'",
        sig->m_hackType, sig->m_hackSubType, sig->m_classIp);
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into auto_punish_hack_ip (occ_date, hack_type, hack_sub_type, c_class_ip, cnt) values (now(), %d, %d, '%s', 1)",
            sig->m_hackType, sig->m_hackSubType, sig->m_classIp);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x829d,
                "DB_HackIPSave DB ERROR");
            return 0;
        }
    }
    db->set_query(
        "upDate auto_punish_hack_full_ip set cnt = cnt+1 where occ_date = CAST(DATE_FORMAT(now(), '%%Y-%%m-%%d') AS CHAR) and hack_type = %d and hack_sub_type = %d and full_ip = '%s'",
        sig->m_hackType, sig->m_hackSubType, sig->m_fullIp);
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into auto_punish_hack_full_ip (occ_date, hack_type, hack_sub_type, full_ip, cnt) values (now(), %d, %d, '%s', 1)",
            sig->m_hackType, sig->m_hackSubType, sig->m_fullIp);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x82af,
                "DB_HackIPSave FullIP DB ERROR");
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_DeleteGuild —— ORIG dispatch 0x0843a9c0。
// ============================================================================

// ORIG 0x0843a9c0：GetDBHandle(2,0) → GetDBHandle(8,0) → *stream >>
// characNo(uint) >> guildId(uint)；任一为 0 直接返 1。charac db(2) 清
// charac_info.guild_right/guild_id，guild db(8) 置 expire_flag=1 并依次
// delete guild_introduce/member/visit/notice/skill，最后 charac db(2)
// 置 guild_id=0/guild_secede=1。任一 exec 失败返 0，全成返 1。
#include "W8LT_DBTask/dispatch_56.inc"
{
    MySQL* characDb = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    MySQL* guildDb = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8, E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    unsigned int guildId = 0;
    *stream >> characNo;
    *stream >> guildId;
    if (characNo == 0 || guildId == 0)
        return 1;
    characDb->set_query(
        "upDate charac_info set guild_right=0, guild_id=0 where charac_no=%d", characNo);
    if (!characDb->exec(true))
        return 0;
    guildDb->set_query(
        "upDate guild_info set expire_flag=1, expire_time=now() where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    guildDb->set_query("deLete from guild_introduce where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    guildDb->set_query("deLete from guild_member where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    guildDb->set_query("deLete from guild_visit where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    guildDb->set_query("deLete from guild_notice where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    guildDb->set_query("deLete from guild_skill where guild_id=%d", guildId);
    if (!guildDb->exec(true))
        return 0;
    characDb->set_query(
        "upDate charac_info set guild_id=0, guild_secede=1 where guild_id = %u", guildId);
    if (!characDb->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_LogBuyItem —— ORIG dispatch 0x0812859a。
// ============================================================================

// ORIG 0x0812859a：time/localtime 取当前 tm；tm_hour 早于
// G_CEnvironment+0x37c（m_fatigueTime）则回退 86400 秒重取；置
// tm_hour=limit、min/sec=0 后 mktime（结果不再使用）。sprintf 日串
// "%04d-%02d-%02d"（tm_year+0x76c / tm_mon+1 / tm_mday）。*stream >>
// m_id >> charac_no >> item_id >> item_count >> item_type(int)。GetDBHandle
// (4,0)。先 update log_acquire_item 累计 item_count；exec 失败或无影响行
// 则 insert 新行（m_id 经 NumberToString）；insert exec 失败 cMyTrace
// (0x566,5) 后返 0；全成返 1。
// ORIG C1 0x844f66e（sz 0x4：base ctor+vptr only）——权威头仅声明，此处外联定义。
#include "W8LT_DBTask/DB_LogBuyItem.inc"

// [b28d-4] DIFF：P183/P171：ORIG 多 {'mov': 4}（漏具名局部/去缓存不足）；ORIG 0812859a
#include "W8LT_DBTask/dispatch_57.inc"
{
    // 声明序照 ORIG 栈槽（深→浅）：buf(-0x5c) item_type(-0x50) item_count(-0x4c)
    // item_id(-0x48) charac_no(-0x44) m_id(-0x40) t(-0x3c) tm*(-0x28)
    // mktime(-0x24) hour_limit(-0x20) db(-0x1c)；cMyTrace(-0x38) 在
    // insert 失败块内具名构造。
    // [FL3W8LT 2026-09-10 P331 负结论] 声明序整体反转（P331 形）实测**编译产物
    // 逐字节不变**（our=186/diff 对数 91 均为同值）⇒ 本件槽序由 4.4「首次使用序」
    // 决定（t 由 time() 首用 ⇒ 最深），声明序不可翻；ORIG 深区
    // [buf,itemType,itemCount,itemId,characNo,m_id,t] 的 4.1 声明序倒置属
    // N82附 族（槽序与语义完成度无关）。保持与 ORIG 一致的声明书写序。
    char buf[0xc];
    int itemType;
    unsigned int itemCount;
    unsigned int itemId;
    unsigned int characNo;
    unsigned int m_id;
    time_t t;
    struct tm* p;
    time_t m;
    int hourLimit;
    MySQL* db;
    time(&t);
    p = localtime(&t);
    hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
    if (p->tm_hour < hourLimit)
    {
        p = 0;
        t -= 0x15180;
        p = localtime(&t);
    }
    p->tm_hour = hourLimit;
    p->tm_min = 0;
    p->tm_sec = 0;
    m = mktime(p);
    m_id = 0;
    characNo = 0;
    itemId = 0;
    itemCount = 0;
    itemType = 0;
    memset(buf, 0, 0xc);
    sprintf(buf, "%04d-%02d-%02d", p->tm_year + 0x76c, p->tm_mon + 1, p->tm_mday);
    *stream >> m_id;
    *stream >> characNo;
    *stream >> itemId;
    *stream >> itemCount;
    *stream >> itemType;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    db->set_query(
        "update log_acquire_item\t\t\t\t\t\tset item_count = item_count + %d\t\t\t\t\t\twhere occ_date = '%s' and charac_no = %u and item_id = %u",
        itemCount, buf, characNo, itemId);
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "insert into log_acquire_item\t\t\t\t\t\t\t(occ_date, m_id, charac_no, item_id, item_count, item_type)\t\t\t\t\t\t\tvalues ('%s', %s, %u, %u, %u, %d)",
            buf, NumberToString(m_id, 0), characNo, itemId, itemCount, itemType);
        if (!db->exec(true))
        {
            // P328⑩/P227：ORIG 0x8128847 段为**单语句全表达式临时链** ——
            // 变参（右起）在 cMyTrace ctor 前求值并跨 ctor 保活
            // （`mov -0x50,%eax; mov %eax,-0x70` itemType / `-0x4c→-0x6c` itemCount /
            // itemId→%edi / characNo→%esi / NumberToString→%ebx，随后才
            // `lea -0x38,%eax; call cMyTraceC1`）⇒ 具名 trace 两语句形把 ctor 提到
            // 串求值之前，块序失配。
            cMyTrace(__PRETTY_FUNCTION__, 0x566, 5)(
                "DB_LogBuyItem, exec ERROR m_id=%s charac_no=%u ItemID=%u Count=%u Type=%d",
                NumberToString(m_id, 0), characNo, itemId, itemCount, itemType);
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_HadesPunish —— ORIG dispatch 0x08445aa6。
// ============================================================================

// ORIG 0x08445aa6：GetDBHandle(1,0) → GetOutBuffer<SIG_HADES_PUNISH>。
// end_time 串：days==0 时 memcpy "'9999-12-31 23:59:59'"（0x16 含引号与 nul），
// 否则 sprintf "adddate(now(),interval %d day)"。apply_flag = flag==0 ? 1 : 2；
// punish_type==1 → punish_value=3，否则 0xc。update member_punish_info；
// exec 失败或无影响行则 insert（失败返 0）。punish_type==1 选长韩文 reason
// （0xa6，ORIG DAT_08c57930）否则短串（0x3d，DAT_08c579d8）。CSystemTime
// getCurSec → localtime_r → strftime "%Y" 年表名。insert
// member_punish_info_history_%Y（失败返 0）。punish_value==3 或
// (==0xc && apply_flag==1) 时再 insert member_punish_info_ars_mail（失败返 0）。
// 全成返 1。
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

// ============================================================================
// _getTodaysHackCount @ 0x08421d50
// 查当天 dnf_hack_log 计数：exec 失败或行数 !=1 → 0；fetch 失败 → 0；
// get_int 失败或计数值为 0 → 0；否则返回计数值。
// ============================================================================
#include "W8LT_DBTask/_getTodaysHackCount.inc"
                                              unsigned short hackType)
{
    int count = 0;
    db->set_query("seLect cnt from dnf_hack_log where m_id=%s and hack_type=%d "
                  "and occ_date=CAST(DATE_FORMAT(now(), '%%Y-%%m-%%d') AS CHAR)",
                  NumberToString(mid, 0), (int)hackType);
    if (!db->exec(true) || db->get_n_rows() != 1)
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_int(0, count) || count == 0)
        return 0;
    return count;
}

// ============================================================================
// _sendResult @ 0x08421e3a
// StreamPool::Acquire("DBThread.cpp", 0x5a14) → CStreamGuard → << 0x9a / << -1 →
// GetInBuffer<SIG_SAVE_CLIENT_HACK_LOG> 写 +0x00=mid → put(DISPATCH_Q)。
// ============================================================================
#include "W8LT_DBTask/_sendResult.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5a14), true);
    guard.operator*()->operator<<(0x9a);   // SIG_SAVE_CLIENT_HACK_LOG
    guard.operator*()->operator<<(-1);

    SIG_SAVE_CLIENT_HACK_LOG* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_CLIENT_HACK_LOG>();
    *reinterpret_cast<unsigned int*>(sig) = mid;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ============================================================================
// _sendHackTypeResult @ 0x08422152
// Acquire("DBThread.cpp", 0x5a6c) → guard → << 0x9a / << -1 →
// GetInBuffer 写 +0x00=mid、+0x08=(ushort)hackType、+0x0e=cnt → put(DISPATCH_Q)。
// ============================================================================
#include "W8LT_DBTask/_sendHackTypeResult.inc"
                                               int cnt)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5a6c), true);
    guard.operator*()->operator<<(0x9a);   // SIG_SAVE_CLIENT_HACK_LOG
    guard.operator*()->operator<<(-1);

    SIG_SAVE_CLIENT_HACK_LOG_VIEW* v = reinterpret_cast<SIG_SAVE_CLIENT_HACK_LOG_VIEW*>(
        guard.operator->()->GetInBuffer<SIG_SAVE_CLIENT_HACK_LOG>());
    v->m_mid = mid;
    v->m_hackType = static_cast<unsigned short>(hackType);
    v->m_cnt = cnt;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ============================================================================
// dispatch @ 0x08421f16
// 读 GetOutBuffer<SIG_SAVE_CLIENT_HACK_LOG>；GetDBHandle(DB_HANDLE_4, 0)。
// update：set charac_no=+0x04, cnt=cnt++0x0e, etc=+0x0a, where m_id=+0x00,
//   hack_type=+0x08；exec 成功且受影响行 !=0 → 特殊 hackType 处理
//   （0x193/0x1f7/0x326/0x3ba 命中 → _getTodaysHackCount + _sendHackTypeResult；
//   随后"todayCount != -1"恒假死代码段：_getTodaysHackCount >= todayCount →
//   DB_ReqSaveHackUserPunish::makeRequest(0xa8) + _sendResult）。
// 否则 insert 整行。恒返回 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_59.inc"
{
    SIG_SAVE_CLIENT_HACK_LOG_VIEW* v = reinterpret_cast<SIG_SAVE_CLIENT_HACK_LOG_VIEW*>(
        stream->GetOutBuffer<SIG_SAVE_CLIENT_HACK_LOG>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_4, static_cast<ENUM_SERVER_GROUP>(0));

    db->set_query("upDate dnf_hack_log set charac_no=%d,last_time=now(),cnt=cnt+%d,"
                  "etc=%d where m_id=%s and hack_type=%d and "
                  "occ_date=CAST(DATE_FORMAT(now(), '%%Y-%%m-%%d') AS CHAR)",
                  v->m_characNo,
                  v->m_cnt,
                  v->m_etc,
                  NumberToString(v->m_mid, 0),
                  static_cast<int>(v->m_hackType));

    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into dnf_hack_log(m_id,charac_no,hack_type,occ_date,"
                      "last_time,etc,cnt) values(%s,%d,%d,cast(now() as date),"
                      "now(),%d,%d)",
                      NumberToString(v->m_mid, 0),
                      v->m_characNo,
                      static_cast<int>(v->m_hackType),
                      v->m_etc,
                      v->m_cnt);
        db->exec(true);
    }
    else
    {
        int todayCount = -1;
        switch (v->m_hackType)
        {
        case 0x193:
        case 0x1f7:
        case 0x326:
        case 0x3ba:
            _sendHackTypeResult(v->m_mid, v->m_hackType,
                                _getTodaysHackCount(db, v->m_mid, v->m_hackType));
            break;
        default:
            break;
        }
        if (todayCount != -1)
        {
            if (_getTodaysHackCount(db, v->m_mid, v->m_hackType) >= todayCount)
            {
                DB_ReqSaveHackUserPunish::makeRequest(v->m_mid, 0xa8, 0);
                _sendResult(v->m_mid);
            }
        }
    }
    return 1;
}

// ============================================================================
// DB_SaveBloodDungeon —— ORIG 2 符号：dispatch 0x08437728 /
// SaveBloodDungeonReward 0x084374f8。
// 报告：docs/class_func_reports/DB_SaveBloodDungeon/{dispatch,
// SaveBloodDungeonReward}.md。
// ============================================================================

// getDailyInitializeTime 定义于 GameUtils.cpp（ORIG T 0x086b1f5a）；
// TU 局部前向声明同 DB_LoadEtc.cpp / DB_Login.cpp 模式。
#include "W8LT_DBTask/getDailyInitializeTime.inc"

// ============================================================================
// SaveBloodDungeonReward @ 0x084374f8
// GetDBHandle(DB_HANDLE_2, 0) → today=getDailyInitializeTime(6,0,0) →
// localtime_r → tm_wday==0 ? 0x7e900 : (tm_wday-1)*0x15180 周偏移；
// sig+0x1c(reward 标志)!=0 → update reward=1（week_occ_date=周起始-0x93a80），
// exec!=1 返 0；sig+0x14(enter_count)!=0 → update 5 字段 + last_play_date=
// sig+0x10（exec!=1 返 0），affected==0 时 insert 全行（exec!=1 返 0）。
// 全成返 1。
// ============================================================================
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

// ============================================================================
// dispatch @ 0x08437728：GetOutBuffer<SIG_BLOOD_DUNGEON> →
// SaveBloodDungeonReward(sig) → 恒返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_60.inc"
{
    SIG_BLOOD_DUNGEON* sig = stream->GetOutBuffer<SIG_BLOOD_DUNGEON>();
    SaveBloodDungeonReward(sig);
    return 1;
}

// ============================================================================
// DB_UpdatePvPBuddy —— ORIG dispatch 0x0843a9b6：空处理，恒返 1
// （mov $1,%eax; ret）。报告：docs/class_func_reports/DB_UpdatePvPBuddy/
// dispatch.md。
// ============================================================================
#include "W8LT_DBTask/dispatch_61.inc"
{
    return 1;
}

// ============================================================================
// DB_LoadPvPLiveEventData —— ORIG dispatch 0x0843ac3e：空处理，恒返 1
// （mov $1,%eax; ret）。报告：docs/class_func_reports/
// DB_LoadPvPLiveEventData/dispatch.md。
// ============================================================================
#include "W8LT_DBTask/dispatch_62.inc"
{
    return 1;
}

// ============================================================================
// DB_UpdatePvPLiveEventData —— ORIG dispatch 0x0843ac48：空处理，恒返 1
// （mov $1,%eax; ret）。报告：docs/class_func_reports/
// DB_UpdatePvPLiveEventData/dispatch.md。
// ============================================================================
#include "W8LT_DBTask/dispatch_63.inc"
{
    return 1;
}

// ============================================================================
// DB_LoadOnlinePreliminaryMember —— ORIG dispatch 0x0843ba94：空处理，恒返 1
// （mov $1,%eax; ret）。报告：
// docs/class_func_reports/DB_LoadOnlinePreliminaryMember/dispatch.md。
// ============================================================================
#include "W8LT_DBTask/dispatch_64.inc"
                                              Stream* stream)
{
    return 1;
}

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

// ============================================================================
// LoadBloodDungeonReward @ 0x08437f58
// GetDBHandle(DB_HANDLE_2, 0) → today=getDailyInitializeTime(6,0,0) →
// localtime_r → tm_wday==0 ? 0x7e900 : (tm_wday-1)*0x15180 周偏移；
// 查询本周行（exec!=1 返 0），n_rows==1 时 fetch 后逐列读 5 字段，
// tm 时/分/秒清零 mktime 归一 last_play_date（不一致则 enter_count=0）；
// 再查询上周行（exec!=1 返 0），n_rows==1 时读 rank/reward/reward_item_id/
// reward_gold（reward = 读值==0）。全成返 1。
// ============================================================================
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

// ============================================================================
// dispatch @ 0x08437df4
// 基类 DBDispatcher::dispatch 检查失败返 0 → *stream>>characNo →
// Acquire 组包 → guard<<userIdx<<slot → GetInBuffer<SIG_LOAD_NO_CACHE_DATA>
// （NULL 返 0）→ memset 0x2c → sig+0=characNo → LoadBloodDungeonReward →
// MsgQueueMgr::put(DISPATCH_Q=1, guard) 返 1。
// ============================================================================
// [Lane31W8LTDBTaskView] 视图 cast 反模式清理：同型/同偏移视角已并入权威头
// （SigTypes_C.h SIG_LOAD_NO_CACHE_DATA 的 m_characNo/m_characNoInt 双视角），
// 本地 DB_LoadNoCacheDataIntView 删除。
// [b28d-4] DIFF ndiff2：P248 档③ 同型化后 gap=0（int 视图消 `mov %eax,%edx` 中转）；
// 残差 = ORIG RHS-first（`mov -0x10,%edx` 先于载目标）vs 4.4 同型 LHS-first，
// 异型形反多中转 ⇒ N18a/N76 封顶。ORIG 08437df4。
// [b28d-solo-2R] NEAR(2) 复核：视图字段 int→unsigned 仍 LHS-first（2 条 mov 序差）；
// 直接 `sig->m_characNo = characNo`（权威 unsigned 字段 + int RHS）反多 `mov %eax,%edx`
// 中转 → DIFF。RHS-first 无中转三形不可造 ⇒ N76/N18a 封顶。ORIG 08437df4。
// [b28d-solo-4] P211 边界复核（探针 /tmp/probe_store{2,3}.cpp）：LHS 指针为**局部**
// 时 4.4 恒 LHS-first，字段/RHS 类型旋钮（int/unsigned/pack1/视图/裸解引用）全无效；
// 仅**形参**指针给 RHS-first ⇒ 本条属 N76/N18a 硬封顶（P211 档③不适用）。
#include "W8LT_DBTask/dispatch_65.inc"
{
    int characNo;
    SIG_LOAD_NO_CACHE_DATA* sig;
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    characNo = 0;
    *stream >> characNo;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8549), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    sig = guard.operator->()->GetInBuffer<SIG_LOAD_NO_CACHE_DATA>();
    if (sig == 0)
        return 0;
    memset(sig, 0, 0x2c);
    // P248 档③（b28d-4）：SIG 字段为 unsigned int 而 RHS 是 int 局部
    // （ORIG `Stream::rsERi` 实证）⇒ 4.4 恒插 `mov %eax,%edx` 转换中转；
    // ORIG 0x08437e.. store 为单段 RHS-first（无中转）。用本 TU int 视图
    // 与 RHS 同型化（布局同宽、sizeof/偏移不变）。
    sig->m_characNoInt = characNo;
    LoadBloodDungeonReward(sig);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ---- G_CDataManager 黑名单参数视图（ORIG 0x8424ce6 单指令 disp 读
// +0x3b4/+0x3b8；P1 组件视图折叠）----
struct DB_CDataManagerBlkView
{
    char m_pad[0x3b4];   // +0x00
    int m_3b4;           // +0x3b4 消减周期基数（*0x15180 = 秒）
    int m_3b8;           // +0x3b8 单周期消减点数
};
// [b28d-4] DIFF：N1/N107：ORIG lea+add 两步寻址 vs 4.4 折单条；ORIG 08424ce6
// [L4-r1] 3 处 cMyTrace 改 P227 全表达式临时链 ⇒ ndiff 389→380（余
//   jle/jg 极性 + 6 条 mov 中转，属 N82/N4 族封顶）。
#include "W8LT_DBTask/dispatch_66.inc"
{
    static const int handleIdx = 3;
    // 声明序照 ORIG 栈槽（低→高）：problemChildTime(-0x70) offsetPoint(-0x6c)
    // blackPoint(-0x68) trace 三实例(-0x64/-0x54/-0x44) sig(-0x34) db(-0x30)
    // result(-0x2c) nRows(-0x28) passCntLimit(-0x24) passCnt(-0x20)
    // computed(-0x1c)。guard 于组包处构造。
    // P171/P328⑤：ORIG 0x084... 头部无 `movl $0x0,-0x70/-0x6c/-0x68` 预清
    // （裸声明只占槽、不发指令）⇒ 不写初始化。
    unsigned int problemChildTime;
    int offsetPoint;
    int blackPoint;
    SIG_REQ_BLACK_COUNT* sig;
    MySQL* db;
    int result;
    int nRows;
    unsigned int passCntLimit;
    unsigned int passCnt;
    int computed;

    sig = stream->GetOutBuffer<SIG_REQ_BLACK_COUNT>();
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    result = 0;
    db->set_query(
        "seLect black_point,offset_point,unix_timestamp(problem_child_time) from charac_black_info where charac_no=%d",
        (int)sig->m_characNo);
    if (db->exec(true) != 1)
        return 0;
    nRows = db->get_n_rows();
    if (nRows > 1)
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x622c,
                              "DB_ReqBlackCount::dispatch error! data(%d)",
                              nRows);
    }
    else if (nRows == 0)
    {
        result = 1;
    }
    if (db->fetch() != 1)
        result = 2;
    if (db->get_int(0, blackPoint) != 1)
        result = 3;
    if (db->get_int(1, offsetPoint) != 1)
        result = 3;
    if (db->get_uint(2, problemChildTime) != 1)
        result = 3;
    if (problemChildTime != 0)
    {
        // G_CDataManager()+0x3b4：黑名单消减周期（秒/周参数）；+0x3b8：单次消减点数。
        passCntLimit =
            ((DB_CDataManagerBlkView*)G_CDataManager())->m_3b4 * 0x15180;
        if (passCntLimit != 0 &&
            GlobalData::s_systemTime_.getCurSec() >
                (int)(problemChildTime + passCntLimit))
        {
            passCnt = (GlobalData::s_systemTime_.getCurSec() -
                       problemChildTime) /
                      passCntLimit;
            if (blackPoint - offsetPoint > 0)
            {
                computed = blackPoint - offsetPoint -
                           ((DB_CDataManagerBlkView*)G_CDataManager())->m_3b8 *
                               (int)passCnt;
                // [L4-r1] P227 全表达式临时链（ORIG ctor 前取 varargs + 跨 ctor 保活）。
                cMyTrace(__PRETTY_FUNCTION__, 0x6259, 0)(
                    "BlackList Modify : c(%d) iPassCnt(%d) iOffsetPoint(%d) iBlackPoint(%d) uProblemChildTime(%d) iComputedBlackPoint(%d)",
                    (int)sig->m_characNo, passCnt, offsetPoint, blackPoint,
                    problemChildTime, computed);
                if (computed < 1)
                {
                    db->set_query(
                        "upDate charac_black_info set offset_point = black_point , problem_child_time = 0 where charac_no=%d",
                        (int)sig->m_characNo);
                    offsetPoint = blackPoint;
                    problemChildTime = 0;
                    cMyTrace(__PRETTY_FUNCTION__, 0x6269, 0)(
                        "BlackList Modify : c(%d) iPassCnt(%d) iOffsetPoint(%d) iBlackPoint(%d) uProblemChildTime(%d)",
                        (int)sig->m_characNo, passCnt, offsetPoint, blackPoint,
                        problemChildTime);
                }
                else
                {
                    db->set_query(
                        "upDate charac_black_info set offset_point = offset_point  + %d, problem_child_time = from_unixtime(%d) where charac_no=%d",
                        ((DB_CDataManagerBlkView*)G_CDataManager())->m_3b8 *
                            (int)passCnt,
                        passCntLimit * passCnt + problemChildTime,
                        (int)sig->m_characNo);
                    offsetPoint =
                        ((DB_CDataManagerBlkView*)G_CDataManager())->m_3b8 *
                            (int)passCnt +
                        offsetPoint;
                    cMyTrace(__PRETTY_FUNCTION__, 0x6262, 0)(
                        "BlackList Modify : c(%d) iPassCnt(%d) iOffsetPoint(%d) iBlackPoint(%d) uProblemChildTime(%d)",
                        (int)sig->m_characNo, passCnt, offsetPoint, blackPoint,
                        problemChildTime);
                }
                if (db->exec(true) != 1)
                {
                    LogManager::logFormat(
                        1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x626c,
                        "BlackList Modify db exec error: iPassCnt(%d) iOffsetPoint(%d) iBlackPoint(%d) uProblemChildTime(%d)",
                        passCnt, offsetPoint, blackPoint, problemChildTime);
                }
            }
        }
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6271), true);
    guard.operator*()->operator<<(0xb3);
    guard.operator*()->operator<<(slot);
    guard.operator*()->operator<<((int)sig->m_characNo);
    guard.operator*()->operator<<(result);
    int diff = blackPoint - offsetPoint;
    guard.operator*()->operator<<(diff & ~(diff >> 31));
    guard.operator*()->operator<<(problemChildTime);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

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

// [SweepW8LT b28d] DIFF：gap 14（our 320 / ORIG 334）封顶。p167 55/57：
// EXTRA memset×2 —— ORIG 4.1 对 `memset(buf,0,0x62)` 内联为 `mov $0x18,%ecx;
// rep stos; mov %ax,(%edx); add $0x2`（8 条）、对 0x2dc 内联 `mov $0xb7,%ecx;
// rep stos`（6 条）；4.4 -O0 恒 call memset（各 5 条）。长度语义已逐处核对
// （0x62 / 0x2dc 与 ORIG rep stos 计数一致），余为 N82 槽差。ORIG 0842f7f4
#include "W8LT_DBTask/dispatch_67.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_REQ_LOAD_GAME_OPTION* req =
        stream->GetOutBuffer<SIG_REQ_LOAD_GAME_OPTION>();
    db->set_query(
        "seLect option_1,option_3,shortcut_emoticon from %s where m_id = %s",
        get_member_game_option(), NumberToString(req->m_accId, 0));
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() == 0)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x74c2), true);
        guard.operator*()->operator<<(0x12f);
        guard.operator*()->operator<<(slot);
        // [b28d-solo] P103/P167：ORIG 调 CStreamGuard::operator->（ptEv）取
        // 缓冲；`operator*()->` 会丢 ptEv（p167 差集 -2 ptEv/+2 deEv）。
        SIG_LOAD_GAME_OPTION* out =
            guard.operator->()->GetInBuffer<SIG_LOAD_GAME_OPTION>();
        ((CETCOption*)out)->clear();
        ((DB_GameOptionSigView*)out)->m_quick[0].clear();
        ((DB_GameOptionSigView*)out)->m_quick[1].clear();
        ((DB_GameOptionSigView*)out)->m_emoticon.clear();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    // [SweepIntervalSet] ORIG 0x0842f9b0 对 fetch/get_binary(0)/get_binary(1) 三处
    // 均发 `mov %al,-0x29(%ebp); movzbl -0x29(%ebp),%eax; xor $0x1; test; je`
    // ⇒ 原码复用同一具名 bool 局部（P0 槽往返 / P102）；直接 `if(!call())` 少两段。
    bool bRet = db->fetch();
    if (!bRet)
    {
        // [SweepIntervalSet] P227 全表达式临时链：ORIG 0x0842f9d3 在 cMyTrace ctor
        // 之前求值 NumberToString 并以 %ebx 跨 ctor 保活（`mov %eax,%ebx` → ctor →
        // `mov %ebx,0x8(%esp)`）；具名 trace 会把求值推迟到 ctor 之后（少 2 条）。
        cMyTrace(__PRETTY_FUNCTION__, 0x74dd, 5)(
            "DB_LoadGameOption::dispatch, fetch ERROR m_id=%s",
            NumberToString(req->m_accId, 0));
        return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x74e1), true);
    guard.operator*()->operator<<(0x12f);
    guard.operator*()->operator<<(slot);
    // [b28d-solo] P103/P167：ORIG 用 operator->（ptEv）。
    SIG_LOAD_GAME_OPTION* out =
        guard.operator->()->GetInBuffer<SIG_LOAD_GAME_OPTION>();
    ((CETCOption*)out)->clear();
    ((DB_GameOptionSigView*)out)->m_quick[0].clear();
    ((DB_GameOptionSigView*)out)->m_quick[1].clear();
    ((DB_GameOptionSigView*)out)->m_emoticon.clear();
    {
        static const int compressedBufferSize = 0x62;  // ORIG _ZZ…DB_LoadGameOption…E20compressedBufferSize
        // [SweepIntervalSet] ORIG 0x0842fad1 把 0x62 清零内联为
        // `lea; mov $0; mov $0x18,%ecx; mov %edx,%edi; rep stos; mov %edi,%edx;
        //  mov %ax,(%edx); add $0x2,%edx`（4 对齐已知 ⇒ 无对齐剥离检查）；
        // 4.4 探针实测 aligned(4) 数组零初始化逐条同形（普通数组会多 7 条对齐检查，
        // memset 恒 call）。语义等价：清零 0x62 字节。
        char buf[0x62] __attribute__((aligned(4))) = { '\0' };
        bRet = db->get_binary(0, buf, compressedBufferSize);
        if (!bRet)
        {
            // P227：ORIG 0x0842fb35 同样先求值 NumberToString 存 %ebx 再 ctor。
            cMyTrace(__PRETTY_FUNCTION__, 0x74f9, 5)(
                "DB_LoadGameOption, get_binary(0) ERROR m_id=%s",
                NumberToString(req->m_accId, 0));
            return 0;
        }
        unsigned int len0 = db->get_binary_length(0);
        unsigned int cap0 = 0x48;
        if (!uncompress_zip((char*)out, cap0, buf, len0))
            return 0;
    }
    {
        static const int compressedBufferSize = 0x2dc;  // ORIG …E20compressedBufferSize_0
        // [SweepIntervalSet] ORIG 0x0842fbcc 内联 `mov $0xb7,%edx; …; rep stos`
        // （0x2dc = 0xb7 dwords，无尾字节）；aligned(4) 数组零初始化 4.4 逐条同形。
        char buf[0x2dc] __attribute__((aligned(4))) = { '\0' };
        bRet = db->get_binary(1, buf, compressedBufferSize);
        if (!bRet)
        {
            // P227：ORIG 0x0842fc18 把 req->m_accId 读入 %ebx 跨 ctor 保活。
            cMyTrace(__PRETTY_FUNCTION__, 0x7515, 5)(
                "DB_LoadGameOption, get_binary(1) ERROR m_id=%u", req->m_accId);
            return 0;
        }
        unsigned int len1 = db->get_binary_length(1);
        unsigned int cap1 = 600;
        if (!uncompress_zip((char*)&((DB_GameOptionSigView*)out)->m_quick[0],
                            cap1, buf, len1))
            return 0;
    }
    if (!get_compressed_blob_data(
            db, 2, (char*)&((DB_GameOptionSigView*)out)->m_emoticon, 0x210, 0x16))
        return 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_SaveGameOption_1 —— ORIG dispatch 0x0842ff4e。
// 报告：docs/class_func_reports/DB_SaveGameOption_1/dispatch.md。
// ============================================================================
#include "W8LT_DBTask/dispatch_68.inc"
{
    // 声明序照 ORIG 栈槽（低→高）：buf(-0x86) bufLen(-0x24) db(-0x20) sig(-0x1c)。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_GAME_OPTION_1* sig =
        stream->GetOutBuffer<SIG_SAVE_GAME_OPTION_1>();
    static const int compressedBufferSize = 0x62;  // ORIG _ZZ…DB_SaveGameOption_1…E20compressedBufferSize
    char buf[0x62] = { '\0' };
    unsigned int bufLen = compressedBufferSize;
    if (!compress_zip(buf, bufLen, (const char*)sig + 4, 0x48))
        return 0;
    db->set_query("upDate %s set option_1='%s' where m_id=%s",
                  get_member_game_option(),
                  db->blob_to_str(0, buf, (int)bufLen),
                  NumberToString(sig->m_mId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query("inSert into %s(m_id, option_1) values(%s,'%s')",
                      get_member_game_option(),
                      NumberToString(sig->m_mId, 0),
                      db->blob_to_str(0, buf, (int)bufLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

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
{
    // 声明序照 ORIG 栈槽（低→高）：buf(-0x111) bufLen(-0x24) db(-0x20)
    // sig(-0x1c)。key_type 每次从 sig+4 重读（ORIG movzbl 形态）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_B,
                                                  E_SERVER_GROUP_NONE);
    SIG_LOAD_CHARAC_HOTKEY_OPTION* sig =
        stream->GetOutBuffer<SIG_LOAD_CHARAC_HOTKEY_OPTION>();
    static const int compressedBufferSize = 0xed;  // ORIG _ZZ…DB_SaveGameOption_2…E20compressedBufferSize
    char buf[0xed] = { '\0' };
    unsigned int bufLen = compressedBufferSize;
    if (!compress_zip(buf, bufLen, (const char*)sig + 5, 0xbc))
        return 0;
    db->set_query(
        "upDate member_key_option set key_option='%s' where m_id=%s and key_type=%d",
        db->blob_to_str(0, buf, (int)bufLen),
        NumberToString(((DB_SaveGameOption2View*)sig)->m_id, 0),
        ((DB_SaveGameOption2View*)sig)->m_keyType);
    // ORIG 0x0842fd42：choose 结果（mov$1/mov$0 于 eax）+ test/je，无
    // register-bool 汇聚块 ⇒ UpdateLogin 直判形。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into member_key_option(m_id, key_type, key_option) values(%s,%d,'%s')",
            NumberToString(((DB_SaveGameOption2View*)sig)->m_id, 0),
            ((DB_SaveGameOption2View*)sig)->m_keyType,
            db->blob_to_str(0, buf, (int)bufLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateItemLock::dispatch —— ORIG 0x0843993a（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。GetOutBuffer<SIG_ITEM_LOCK_UPDATE> → 压缩 sig+4 起 800 字节
// 至 buf（0x3cc 清 0）；compress 失败返 0 → upDate charac_item_lock_info
// （item_lock_info='%s'，charac_no=%u）exec 失败返 0；affected==0 补 inSert；
// inSert exec 失败返 0；全绿返 1。无基类 dispatch 前置检查。
#include "W8LT_DBTask/dispatch_70.inc"
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    DB_UpdateItemLockView* sig =
        (DB_UpdateItemLockView*)stream->GetOutBuffer<SIG_ITEM_LOCK_UPDATE>();
    char buf[0x3cc] = { 0 };
    unsigned int bufLen = 0x3cc;
    if (!compress_zip(buf, bufLen, sig->m_lockData, 800))
        return 0;
    {
        // register 局部直落 %ebx（ORIG +0x91 `mov (%eax),%ebx` 免中转拷贝）。
        register int characNo = sig->m_characNo;
        db->set_query(
            "upDate charac_item_lock_info set item_lock_info='%s' where charac_no=%u",
            db->blob_to_str(0, buf, (int)bufLen), characNo);
    }
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_item_lock_info(charac_no, item_lock_info) values(%u, '%s')",
            (unsigned int)sig->m_characNo, db->blob_to_str(0, buf, (int)bufLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_LoadExpertJob::dispatch —— ORIG 0x08436464（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。*stream >> characNo(uint) → seLect expert_job_giveup_cnt/
// expert_job_info/expert_job_info_ex/recipe from charac_expert_job where
// charac_no=%d；exec 失败返 0。
//   get_n_rows()==0 → 组包（Acquire("DBThread.cpp",0x81b2)）<<userIdx<<slot →
//   GetInBuffer<SIG_EXPERT_JOB> → memset 0x30 → put(DISPATCH_Q) 返 1；
//   否则 fetch 失败返 0；成功再组包（0x81c2）→ memset 0x30 →
//   get_ushort(0,+4)/get_int(1,+8)/get_int(2,+0xc)/get_binary(3,+0x10,0x1e)，
//   任一失败 StreamPool::Free(guard) 返 0；全成 put(DISPATCH_Q) 返 1。
// （N 封顶：ndiff 176 残差为 CStreamGuard try/catch EH 区域 + guard 帧布局
// （-0x20 vs -0x28）+ 嵌套失败各 Free 的汇聚差异，见报告 dispatch.md）
// [b28d-4] NEAR：NEAR 封顶：助记符序列全同、仅栈槽地址/寄存器文本差（N82 槽号错位 / N7 寄存器轮换）；ORIG 08436464
#include "W8LT_DBTask/dispatch_71.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    *stream >> characNo;
    db->set_query(
        "seLect expert_job_giveup_cnt, expert_job_info, expert_job_info_ex, recipe  from charac_expert_job where charac_no=%d",
        characNo);
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() == 0)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x81b2), true);
        guard.operator*()->operator<<(userIdx);
        guard.operator*()->operator<<(slot);
        SIG_EXPERT_JOB* sig =
            guard.operator->()->GetInBuffer<SIG_EXPERT_JOB>();
        memset(sig, 0, 0x30);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    if (db->fetch() != 1)
        return 0;
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x81c2), true);
        guard.operator*()->operator<<(userIdx);
        guard.operator*()->operator<<(slot);
        SIG_EXPERT_JOB* sig =
            guard.operator->()->GetInBuffer<SIG_EXPERT_JOB>();
        memset(sig, 0, 0x30);
        if (!db->get_ushort(0, sig->m_giveupCount))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_int(1, sig->m_endurance))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_int(2, sig->m_enchantCount))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_binary(3, sig->m_recipeFlag, 0x1e))
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
}

// ============================================================================
// DB_SaveExpertJob::dispatch —— ORIG 0x084367f4（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<SIG_EXPERT_JOB> → upDate charac_expert_job
// （giveup_cnt=%d(ushort 零扩)、info/info_ex=%d、recipe='%s'、charac_no=%u）
// exec 失败返 0；affected==0 补 inSert；inSert exec 失败返 0；全绿返 1。
#include "W8LT_DBTask/dispatch_72.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_EXPERT_JOB* sig = stream->GetOutBuffer<SIG_EXPERT_JOB>();
    // register 且类型精确匹配字段（int 对 int）→ 直落 %esi（ORIG 0x039），
    // unsigned 版本或多一重 cast 会退回 scratch+copy 形态。
    {
        register int characNo = sig->m_characNo;
        db->set_query(
        "upDate charac_expert_job set expert_job_giveup_cnt=%d, expert_job_info=%d, expert_job_info_ex=%d, recipe='%s' where charac_no=%u",
        (int)sig->m_giveupCount, sig->m_endurance, sig->m_enchantCount,
        db->blob_to_str(0, sig->m_recipeFlag, 0x1e),
        characNo);
    }
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_expert_job(charac_no, expert_job_giveup_cnt, expert_job_info, expert_job_info_ex, recipe) values(%u, %d, %d, %d, '%s')",
            (unsigned int)sig->m_characNo, (int)sig->m_giveupCount,
            sig->m_endurance, sig->m_enchantCount,
            db->blob_to_str(0, sig->m_recipeFlag, 0x1e));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_SaveTitleBook::dispatch —— ORIG 0x0840db00（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<stTitleBook_Save> → 四段压缩（general 0x10ae→
// 0x1410、specific 0x2fa8→0x393c、despair 0x1c98→0x225c、event 0xe4c→
// 0x1134，源 sig+0x00/0x10ae/0x4056/0x5cee），任一失败返 0 → upDate
// charac_titlebook（四 blob，charac_no=+0x6b3a）exec 失败返 0；affected==0
// 补 inSert；inSert exec 失败返 0；全绿返 1。
#include "W8LT_DBTask/dispatch_73.inc"
{
    DB_TitleBookSaveView* sig =
        (DB_TitleBookSaveView*)stream->GetOutBuffer<stTitleBook_Save>();
    char general[0x1410] = { 0 };
    unsigned int generalLen = 0x1410;
    if (!compress_zip(general, generalLen, sig->m_data, 0x10ae))
        return 0;
    char specific[0x393c] = { 0 };
    unsigned int specificLen = 0x393c;
    if (!compress_zip(specific, specificLen, sig->m_data + 0x10ae, 0x2fa8))
        return 0;
    char despair[0x225c] = { 0 };
    unsigned int despairLen = 0x225c;
    if (!compress_zip(despair, despairLen, sig->m_data + 0x4056, 0x1c98))
        return 0;
    char event[0x1134] = { 0 };
    unsigned int eventLen = 0x1134;
    if (!compress_zip(event, eventLen, sig->m_data + 0x5cee, 0xe4c))
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_titlebook set general_section='%s', specific_section='%s', despair='%s', event='%s' where charac_no=%u",
        db->blob_to_str(0, general, (int)generalLen),
        db->blob_to_str(1, specific, (int)specificLen),
        db->blob_to_str(2, despair, (int)despairLen),
        db->blob_to_str(3, event, (int)eventLen),
        (unsigned int)sig->m_characNo);
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_titlebook(charac_no, general_section, specific_section, despair, event) values(%u, '%s','%s','%s','%s')",
            (unsigned int)sig->m_characNo,
            db->blob_to_str(0, general, (int)generalLen),
            db->blob_to_str(1, specific, (int)specificLen),
            db->blob_to_str(2, despair, (int)despairLen),
            db->blob_to_str(3, event, (int)eventLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_SaveAccountInfo::dispatch —— ORIG 0x08432646（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6。GetOutBuffer<SIG_SAVE_ACCOUNT_INFO> → upDate member_play_info
// （ip/last_play_time/pcbang_flag/end_ip/mac_addr，where occ_date=now() and
// m_id=%s[NumberToString]）；exec 失败或 affected==0 补 inSert（play_count=1）；
// inSert exec 失败返 0；全绿返 1。
#include "W8LT_DBTask/dispatch_74.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    DB_SaveAccountInfoView* sig =
        (DB_SaveAccountInfoView*)stream->GetOutBuffer<SIG_SAVE_ACCOUNT_INFO>();
    db->set_query(
        "upDate member_play_info set ip=\"%s\", last_play_time=%u, pcbang_flag=%d, end_ip=\"%s\", mac_addr=\"%s\" where occ_date=now() and m_id=%s",
        sig->m_ip, (unsigned int)sig->m_lastPlayTime, (int)sig->m_pcbangFlag,
        sig->m_endIp, sig->m_macAddr, NumberToString(sig->m_mId, 0));
    // 直判形产生 ORIG 的 mov$1/jmp/mov$0/test al 岛（同 UpdateLogin）。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into member_play_info(occ_date, m_id, play_count, ip, last_play_time, pcbang_flag, end_ip, mac_addr) values(now(), %s, 1, \"%s\", %u, %d, \"%s\", \"%s\")",
            NumberToString(sig->m_mId, 0), sig->m_ip,
            (unsigned int)sig->m_lastPlayTime, (int)sig->m_pcbangFlag,
            sig->m_endIp, sig->m_macAddr);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}
// ============================================================================
// DB_SaveAchievement::dispatch —— ORIG 0x0840deaa（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<stAchievement_DBSaveData> → 压缩 sig+0 起 0x1800
// 字节（m_data 段）至 buf（0x1ce7 清 0，P4 形态）；compress 失败返 0 →
// upDate charac_achievement（achievement='%s', last_update_time=now()，
// charac_no=%u）exec 失败返 0；affected==0 补 inSert（charac_no, achievement,
// last_update_time values(%u, '%s', now())）；inSert exec 失败返 0；全绿返 1。
#include "W8LT_DBTask/dispatch_75.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    DB_SaveAchievementView* sig =
        (DB_SaveAchievementView*)stream->GetOutBuffer<stAchievement_DBSaveData>();
    char buf[0x1ce7] = { 0 };
    unsigned int bufLen = 0x1ce7;
    if (!compress_zip(buf, bufLen, sig->m_data, 0x1800))
        return 0;
    register int characNo = sig->m_characNo;
    db->set_query(
        "upDate charac_achievement set achievement='%s', last_update_time=now() where charac_no=%u",
        db->blob_to_str(0, buf, (int)bufLen), characNo);
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_achievement(charac_no, achievement, last_update_time) values(%u, '%s', now())",
            (unsigned int)sig->m_characNo, db->blob_to_str(0, buf, (int)bufLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateCoolTimeItem::dispatch —— ORIG 0x08430570（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO> →
// blob_to_str(+4, 0x194)；upDate charac_item_stat set cooltime_item='%s'
// where charac_no=%u；exec 失败 或 affected==0 补 inSert（charac_no,
// cooltime_item values(%u, '%s')）；inSert exec 失败返 0；全绿返 1。
#include "W8LT_DBTask/dispatch_76.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    DB_ContinuousItemStatView* sig =
        (DB_ContinuousItemStatView*)
        stream->GetOutBuffer<SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    char* blob = db->blob_to_str(0, sig->m_itemData, 0x194);
    db->set_query(
        "upDate charac_item_stat set cooltime_item='%s' where charac_no=%u",
        blob, sig->m_characNo);
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_item_stat(charac_no, cooltime_item) values(%u, '%s')",
            sig->m_characNo, blob);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateEffectItem::dispatch —— ORIG 0x08430676（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。与 DB_UpdateCoolTimeItem 同构，SQL 列 effect_item。
#include "W8LT_DBTask/dispatch_77.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    DB_ContinuousItemStatView* sig =
        (DB_ContinuousItemStatView*)
        stream->GetOutBuffer<SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    char* blob = db->blob_to_str(0, sig->m_itemData, 0x194);
    db->set_query(
        "upDate charac_item_stat set effect_item='%s' where charac_no=%u",
        blob, sig->m_characNo);
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_item_stat(charac_no, effect_item) values(%u, '%s')",
            sig->m_characNo, blob);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// 虚覆盖（ORIG vtable @0x8c5eec0 槽序 [dispatch][D1][D0]）。
// GetOutBuffer<SIG_UPDATE_AVATAR_JEWEL_SOCKET> → DB_HANDLE_3 →
// blob_to_str(+8 m_emblem, 0x1e)；upDate user_items set jewel_socket='%s'
// where ui_id=%d and charac_no=%d（ui_id=m_avatarKey, charac_no=m_characNo）；
// exec(true)。Ghidra 反编译标 void：函数尾无 movzbl/置值——ORIG 源码为 bool
// 覆盖且体末无显式 return（eax 残留 exec 返回值）。故此处不写 return。
#include "W8LT_DBTask/dispatch_78.inc"
{
    static const int handleIdx = 3;
    SIG_UPDATE_AVATAR_JEWEL_SOCKET* sig =
        stream->GetOutBuffer<SIG_UPDATE_AVATAR_JEWEL_SOCKET>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set jewel_socket='%s' where ui_id=%d and charac_no=%d",
        db->blob_to_str(0, sig->m_emblem, 0x1e),
        sig->m_avatarKey, sig->m_characNo);
    db->exec(true);
}
// ============================================================================
// DB_UpdateCreatureItemLock::dispatch —— ORIG 0x08439aba（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。*stream >> ui_id(int) → *stream >> item_lock_key(int) →
// upDate creature_items set item_lock_key=%d where ui_id=%d；返回 exec(true)==1。
#include "W8LT_DBTask/dispatch_79.inc"
{
    static const int handleIdx = 3;
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    int uiId;
    int itemLockKey;
    *stream >> uiId;
    *stream >> itemLockKey;
    db->set_query(
        "upDate creature_items set item_lock_key=%d where ui_id=%d",
        itemLockKey, uiId);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_UpdateAvatarItemLock::dispatch —— ORIG 0x08439b4e（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。*stream >> ui_id(int) → *stream >> item_lock_key(int) →
// upDate user_items set item_lock_key=%d where ui_id=%d；返回 exec(true)==1。
#include "W8LT_DBTask/dispatch_80.inc"
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 无初始化（+0x26/+0x38 直接 lea 传引用给 operator>>），带 `= 0`
    // 会多两条 movl $0 清零（42 vs 40 insns）。
    int uiId;
    int itemLockKey;
    *stream >> uiId;
    *stream >> itemLockKey;
    db->set_query(
        "upDate user_items set item_lock_key=%d where ui_id=%d",
        itemLockKey, uiId);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_UpdateMemberPlayInfoPcRoom::dispatch —— ORIG 0x08439be2（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6。*stream >> m_id(uint) → NumberToString(m_id,0) →
// upDate member_play_info set pcbang_flag=1 where occ_date=now() and m_id=%s；
// 返回 exec(true)==1。
char* NumberToString(unsigned int value, int radix);   // ORIG T 0x0810904b
#include "W8LT_DBTask/dispatch_81.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId;   // ORIG 无初始化：直接 lea 传引用 >> 填充
    *stream >> mId;
    db->set_query(
        "upDate member_play_info set pcbang_flag=1 where occ_date=now() and m_id=%s",
        NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_UpdateCharacterDay::dispatch —— ORIG 0x08435d12（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6。GetOutBuffer<SIG_UPDATE_CHARACTER_DAY_EVENT>（+0x00 m_id uint、
// +0x04 entries ushort，见 DB_UpdateCharacterDayView）→ upDate
// event_1111_charac_day set entries_count=entries_count+%u, update_time=now()
// where m_id=%s（%s 前为 entries）；exec 失败返 0；affected==0 补 inSert
// （values(%s, %u, now())）；inSert exec 失败返 0；全绿返 1。
#include "W8LT_DBTask/dispatch_82.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    DB_UpdateCharacterDayView* sig = reinterpret_cast<DB_UpdateCharacterDayView*>(
        stream->GetOutBuffer<SIG_UPDATE_CHARACTER_DAY_EVENT>());
    db->set_query(
        "upDate event_1111_charac_day set entries_count=entries_count+%u, update_time=now() where m_id=%s",
        (unsigned int)sig->m_entries, NumberToString(sig->m_mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_1111_charac_day(m_id, entries_count, update_time) values(%s, %u, now())",
            NumberToString(sig->m_mId, 0), (unsigned int)sig->m_entries);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateCreatureItemList::dispatch —— ORIG 0x0841fd12（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。GetOutBuffer<SIG_CREATURE_ITEM_LIST>（首 int=条目数；+0x21a4 起
// 行数组，步长 0x38，见 DB_CreatureItemListEntry）→ 逐行
// upDate creature_items set exp=%d,stomach=%d where ui_id=%d（实参序
// exp/stomach/ui_id）；任一 exec 失败返 0；全部成功返 1。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 0841fd12
#include "W8LT_DBTask/dispatch_83.inc"
{
    static const int handleIdx = 3;
    SIG_CREATURE_ITEM_LIST* raw = stream->GetOutBuffer<SIG_CREATURE_ITEM_LIST>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    // [SweepIntervalSet] P0：ORIG 0x0841fdec 循环条件每次重读 `raw->m_count`
    // （`mov -0x14(%ebp),%eax; mov (%eax),%eax; cmp -0xc(%ebp),%eax; setg`），
    // 无 count 具名槽 ⇒ 删缓存局部。
    for (int i = 0; i < raw->m_count; ++i)
    {
        // [SweepIntervalSet 复测] P183 三次内联重算（实参位各写一遍完整
        // raw+i*0x38+0x21a4）实测 our 78 / ndiff 72，劣于具名 e 的 our 60 /
        // ndiff 60（4.4 对 i*0x38 恒发 `lea (,%eax,8)→mov→sub→mov` 四条，
        // 每处多 2 条 regmove，属 N18a 后端寄存器角色差，非源形可控）⇒ 保留 e。
        DB_CreatureItemListEntry* e = reinterpret_cast<DB_CreatureItemListEntry*>(
            reinterpret_cast<char*>(raw) + kCreatureItemRowsOffset + i * 0x38);
        db->set_query(
            "upDate creature_items set exp=%d,stomach=%d where ui_id=%d",
            e->m_exp, e->m_stomach, e->m_uiId);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// ORIG 实证（0x0841fff8 反汇编）：tmp[100] 先于检查零初始化（P165 物理展开序），
// va_start 在 (mask&flag) 检查前；isFirst==true 仅清标志，isFirst==false 时
// 前置 ","（ORIG 以 2 字节 local_e 存 "," 并 strlen 之，movzwl 物化→N 残差），
// 两分支均落入共同尾：strncat(buf,tmp,strlen(tmp))。
// ============================================================================
// [b28d-4] DIFF：N1/N107：ORIG lea+add 两步寻址 vs 4.4 折单条；ORIG 0841fff8
#include "W8LT_DBTask/AddFieldString.inc"
                    const char* fmt, ...)
{
    char tmp[100] = { '\0' };
    // ORIG 0x08420013 `movzwl <A>,%eax; mov %ax,-0xa(%ebp)` = 局部数组
    // `char szComma[] = ","`（P244 两跳：字面量先落槽再入栈实参）；直接
    // 传字面量会被 4.4 折成 memcpy（P167 EXTRA）。
    char szComma[] = ",";
    va_list ap;
    va_start(ap, fmt);
    // P257 正向包裹：早退 return/goto 使 4.4 在函数尾产 `jmp <epilogue>` 岛
    // （ORIG 0x0842009b 后直落 0x84200a0），正向 if 包裹后 je 直达 epilogue。
    if (mask & flag)
    {
        vsprintf(tmp, fmt, ap);
        va_end(ap);
        // ORIG 0x0842004f `xor $1,%eax; test %al,%al; je <else>`（P292：bool
        // 取反物化）⇒ 源码为 `if (!isFirst) strncat(...); else isFirst=false;`。
        if (!isFirst)
            strncat(buf, szComma, strlen(szComma));
        else
            isFirst = false;
        strncat(buf, tmp, strlen(tmp));
    }
}

// ============================================================================
// DB_UpdateCreatureItem::GetField —— ORIG 0x084200a8（报告 GetField.md 全文）
// ============================================================================
// 生成 creature_items 的 SET 子句：name='%s'（escape_string 经 25B 中转，
// 源 sig+0x34）、stomach(+0x44)/creature_type(+0x50 符号 char)/exp(+0x4c)/
// charac_no(+0x48)/slot(+0x2c)/it_id(+0x08)/no_charge(+0x52 符号 char)/
// stat(+0x53 符号 char) 逐项按 m_hdr 位掩码经 AddFieldString 过滤；
// expire_date 三态：m_days==0 且 m_expireTick==0 → '9999-12-31 23:59:59'；
// m_days==0 且 m_expireTick!=0 → from_unixtime(m_expireTick)；否则
// from_unixtime(GetDateTimeTick() + m_days*0x15180)。
#include "W8LT_DBTask/GetField.inc"
{
    bool isFirst = true;
    char nameBuf[25] = { '\0' };
    db->escape_string(nameBuf, sig->m_name);
    AddFieldString(isFirst, out, sig->m_hdr, 0x01, "name='%s'", nameBuf);
    AddFieldString(isFirst, out, sig->m_hdr, 0x02, "stomach=%d",
                   sig->m_stomachValue);
    AddFieldString(isFirst, out, sig->m_hdr, 0x04, "creature_type=%d",
                   (signed char)sig->m_flag50);
    AddFieldString(isFirst, out, sig->m_hdr, 0x08, "exp=%d",
                   sig->m_exp);
    AddFieldString(isFirst, out, sig->m_hdr, 0x10, "charac_no=%d",
                   sig->m_characNo);
    AddFieldString(isFirst, out, sig->m_hdr, 0x20, "slot=%d", sig->m_slot);
    AddFieldString(isFirst, out, sig->m_hdr, 0x40, "it_id=%d",
                   ((DB_CreatureItemInserView*)sig)->m_itId);
    AddFieldString(isFirst, out, sig->m_hdr, 0x80, "no_charge=%d",
                   (signed char)sig->m_keepRight);
    AddFieldString(isFirst, out, sig->m_hdr, 0x100, "stat=%d",
                   (signed char)sig->m_field53);
    if (sig->m_days == 0 && sig->m_expireTick == 0)
    {
        AddFieldString(isFirst, out, sig->m_hdr, 0x200,
                       "expire_date='9999-12-31 23:59:59'");
    }
    else if (sig->m_days == 0 && sig->m_expireTick != 0)
    {
        AddFieldString(isFirst, out, sig->m_hdr, 0x200,
                       "expire_date=from_unixtime('%d')", sig->m_expireTick);
    }
    else
    {
        // ORIG 0x08420…：`call; mov %eax,-0xc; …; imul; add %eax,-0xc` ——
        // 先赋槽再内存累加（+= RMW），非单表达式相加。
        int t = OS_API::GetDateTimeTick();
        t += sig->m_days * 0x15180;
        AddFieldString(isFirst, out, sig->m_hdr, 0x200,
                       "expire_date=from_unixtime('%d')", t);
    }
}

// ============================================================================
// DB_UpdateCreatureItem::dispatch —— ORIG 0x084203e8（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。GetOutBuffer<SIG_CREATURE_ITEM> → 按 m_field64(+0x64) 分支：
//   1: m_mapSize(+0x68)==0 → upDate inventory set creature_flag=1 where
//      charac_no=m_characNo(+0x48)；m_field70(+0x70)==1 → 再 creature_flag=0
//      where charac_no=m_field6c(+0x6c)
//   3/5: m_mapSize==1 → creature_flag=0 where charac_no=m_field6c
//   4: m_mapSize==0 → creature_flag=1 where charac_no=m_characNo
// 各 exec 失败返 0。GetField 生成 SET 子句 → upDate creature_items set %s
// where ui_id=m_addInfo2(+0x30)；exec 失败返 0。m_field64==4||1 时组回包：
// Acquire("DBThread.cpp",0x5710) → guard<<0x8b → guard<<slot →
// GetInBuffer<SIG_CREATURE_ITEM> → memcpy 0x74 → seLect 9 列
// （slot,it_id,name,stomach,exp,endurance,creature_type,no_charge,
// unix_timestamp(expire_date)）逐列读入回包体；任一失败
// cMyTrace(0x5729..0x5794,5) + Free(guard) 返 0；全成 put(DISPATCH_Q,guard)
// 返 1。其余分支返回 1。
// [SweepW8LT b28d] DIFF：gap 14（our 594 / ORIG 608）封顶。p167 72/72 [0缺0多]
// （cMyTrace C1/C2、CStreamGuard C1/C2/D1/D2 同址别名）。残差：① P0 具名 bool——
// ORIG 前 6 处检查（exec/fetch/get_int0/get_ulong1/get_int3/get_int4）产
// `movzbl %al,%eax; mov %eax,-0x1c; cmpl $0,-0x1c; jne`（共用 -0x1c 槽），后 5 处
// 与 4.4 全部为 `xor $1; test %al,%al; je`（各 -1）；② P231 ORIG 错误路径用 %ebx
// 跨 ctor 保活 sig->m_characNo（`mov 0x48(%eax),%ebx`），4.4 每处重载；③ N78
// rep movsl 段 %esi 多一条中转。ORIG 084203e8
#include "W8LT_DBTask/dispatch_84.inc"
{
    static const int handleIdx = 3;
    SIG_CREATURE_ITEM* sig = stream->GetOutBuffer<SIG_CREATURE_ITEM>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    switch (sig->m_field64)
    {
    case 1:
        if (sig->m_mapSize == 0)
        {
            db->set_query(
                "upDate inventory set creature_flag=%d where charac_no=%d",
                1, sig->m_characNo);
            if (db->exec(true) != 1)
                return 0;
        }
        if (sig->m_field70 == 1)
        {
            db->set_query(
                "upDate inventory set creature_flag=%d where charac_no=%d",
                0, sig->m_field6c);
            if (db->exec(true) != 1)
                return 0;
        }
        break;
    case 3:
        if (sig->m_mapSize == 1)
        {
            db->set_query(
                "upDate inventory set creature_flag=%d where charac_no=%d",
                0, sig->m_field6c);
            if (db->exec(true) != 1)
                return 0;
        }
        break;
    case 4:
        if (sig->m_mapSize == 0)
        {
            db->set_query(
                "upDate inventory set creature_flag=%d where charac_no=%d",
                1, sig->m_characNo);
            if (db->exec(true) != 1)
                return 0;
        }
        break;
    // [b28d-solo] P167：ORIG case 3 与 case 5 各自独立一份 set_query+exec
    // （差集 -1 set_query/-1 exec；ORIG 0x08420481 与 0x084205e4 两处），
    // 合并 case 标签只发一份 ⇒ 拆开；case 序按 P46 升序（1,3,4,5）。
    case 5:
        if (sig->m_mapSize == 1)
        {
            db->set_query(
                "upDate inventory set creature_flag=%d where charac_no=%d",
                0, sig->m_field6c);
            if (db->exec(true) != 1)
                return 0;
        }
        break;
    default:
        break;
    }
    char setClause[0x400] = { '\0' };
    GetField(db, setClause, sig);
    db->set_query(
        "upDate creature_items set %s where ui_id=%d",
        setClause, sig->m_addInfo2);
    if (db->exec(true) != 1)
        return 0;
    if (sig->m_field64 != 4 && sig->m_field64 != 1)
        return 1;
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5710), true);
        guard.operator*()->operator<<((int)0x8b);
        guard.operator*()->operator<<((int)slot);
        // [b28d-solo] P103/P167：ORIG 调 operator->（ptEv）；N84：结构整体
        // 赋值（ORIG 内联 rep movs 0x74），非 memcpy call（p167 EXTRA）。
        SIG_CREATURE_ITEM* out =
            guard.operator->()->GetInBuffer<SIG_CREATURE_ITEM>();
        *out = *sig;
        // ORIG 0x08420759 movl $0x0,-0x1c(%ebp)：具名 int 结果局部，各检查
        // 逐次赋值后 `movzbl %al,%eax; mov %eax,-0x1c; cmpl $0,-0x1c; jne`
        // （P102 同一局部保持失败出口；旧内联 `!call()` 少 1 条/处）
        int ok = 0;
        db->set_query(
            "seLect slot,it_id,name,stomach,exp,endurance,creature_type,no_charge,unix_timestamp(expire_date) from creature_items where ui_id=%d",
            sig->m_addInfo2);
        ok = db->exec(true);
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5729, 5);
            trace("DB_UpdateCreatureItem::dispatch, exec() ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        ok = db->fetch();
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5731, 5);
            trace("DB_UpdateCreatureItem::dispatch, fetch() ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        ok = db->get_int(0, out->m_slot);
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5739, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(0) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        // it_id 列回填 +0x08（DB_CreatureItemInserView::m_itId；32 位
        // unsigned long==unsigned int 同宽，ORIG 传槽地址）。
        DB_CreatureItemInserView* iv =
            reinterpret_cast<DB_CreatureItemInserView*>(out);
        ok = db->get_ulong(
            1, *reinterpret_cast<unsigned long*>((void*)&iv->m_itId));
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5741, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_ushort(1) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        // [A04-r5] ORIG 0x084209d5-0x084209df 为 `call get_str; xor $0x1,%eax;
        // test %al,%al; je`（**内联 bool 直判**，不经 ok 槽）⇒ 后 4 个检查
        // （get_str/get_ushort/get_int(6)/(7)/(8)）必须内联，与 0x08420790 等
        // 前 6 处 `movzbl %al,%eax; mov %eax,-0x1c; cmpl $0,-0x1c; jne`（ok 槽形）区分。
        if (!db->get_str(2, out->m_name, 0xd))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5756, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(2) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        ok = db->get_int(3, out->m_stomachValue);
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x575e, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(3) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        ok = db->get_int(4, out->m_exp);
        if (!ok)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5766, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(4) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        if (!db->get_ushort(5, iv->m_endurance))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x576d, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_ushort(5) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        int creatureType = 0;
        // ORIG 0x08420b8e：`call get_int; xor $0x1,%eax; test %al,%al; je`（内联）。
        if (!db->get_int(6, creatureType))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5775, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(6) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        out->m_flag50 = (unsigned char)creatureType;
        int noCharge = 0;
        if (!db->get_int(7, noCharge))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x577e, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(7) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        out->m_keepRight = (unsigned char)noCharge;
        if (!db->get_int(8, out->m_days))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5794, 5);
            trace("DB_UpdateCreatureItem::dispatch, get_int(9) ERROR charac_no=%u",
                  sig->m_characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
}

// ============================================================================
// DB_UpdateLimitItemUsage::dispatch —— ORIG 0x0843f022
// ============================================================================
// ORIG 0x843f022：sub $0x20 空帧 + mov $1,%eax + leave/ret —— 恒返 1。
#include "W8LT_DBTask/dispatch_85.inc"
{
    // 未用局部压出 ORIG 帧宽（P31；实测 char[0x14..0x20] → sub $0x20 + leave）。
    char buf[0x18];
    (void)buf;
    return 1;
}

// ============================================================================
// get_item_count_table_name —— ORIG 0x083fbe07
// ============================================================================
// ORIG 0x83fbe07（_Z25get_item_count_table_namev，DB_UpdateItemCountValue
// 同域辅助）：mov $"event_item_count_value",%eax + ret —— 表名字面量。
char* get_item_count_table_name()
{
    return "event_item_count_value";
}

// ============================================================================
// DB_UpdateItemCountValue::dispatch —— ORIG 0x0843eb6e
// ============================================================================
// ORIG 0x843eb6e：GetDBHandle(DB_HANDLE_9,0) → stream >> m_id(u32) >>
// item_cnt(u32) → upDate %s set item_cnt=item_cnt+%d where m_id=%s
// （%d=item_cnt 增量、%s=NumberToString(m_id,0)，get_item_count_table_name()）；
// getAffectedRowCount()==0 → inSert values(%s, %d)；exec 失败返 0，成功返 1。
#include "W8LT_DBTask/dispatch_86.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;       // -0x10（SQL %s m_id，第一读）
    unsigned int itemCnt = 0;   // -0x14（SQL %d 增量，第二读）
    stream->operator>>(mId);
    stream->operator>>(itemCnt);
    db->set_query(
        "upDate %s set item_cnt=item_cnt+%d where m_id=%s",
        get_item_count_table_name(), itemCnt, NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into %s(m_id, item_cnt) values(%s, %d)",
            get_item_count_table_name(), NumberToString(mId, 0), itemCnt);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateDetailChannel::dispatch —— ORIG 0x0841bcee
// ============================================================================
// ORIG 0x841bcee：GetOutBuffer<SIG_UPDATE_DETAIL_CHANNEL>（4×int 载荷）→
// getCurSec() % 0x708，余数 < 900 则回退余数、否则补 0x708-余数（对齐 15 分钟
// 槽）→ GetDBHandle(DB_HANDLE_4,0) → inSert concurrent_user_status 5 占位
// （from_unixtime(occ_time),player_status,dungeon_index,channel_no,occ_count）；
// exec 失败返 0；成功后 cMyTrace(__PRETTY_FUNCTION__,0x4f28,0) 日志同 4 值。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0841bcee
// [L4-r1] **已修 AE**（ndiff 72→0）：P227 全表达式临时链 cMyTrace(...)(...)
//   ⇒ 复现 ORIG 的 3 push（edi/esi/ebx）+ 帧 0x5c + C1 前 varargs 物化。
#include "W8LT_DBTask/dispatch_87.inc"
{
    DB_UpdateDetailChannelView* sig =
        reinterpret_cast<DB_UpdateDetailChannelView*>(
            stream->GetOutBuffer<SIG_UPDATE_DETAIL_CHANNEL>());
    int t = GlobalData::s_systemTime_.getCurSec();
    int mod = t % 0x708;
    // [SweepIntervalSet] ORIG 0x0841bd?? 是两条独立分支：`sub %eax,-0x24`（t -= mod）
    // 与 `mov $0x708,%eax; sub -0x20,%eax; add %eax,-0x24`（t += 0x708-mod）；
    // 三目 `t += (mod<900)?-mod:(0x708-mod)` 被 4.4 折成 `neg %eax` 单链。
    if (mod < 900)
        t -= mod;
    else
        t += 0x708 - mod;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into concurrent_user_status(occ_time,player_status,dungeon_index,channel_no,occ_count) values(from_unixtime(%d),%d,%d,%d,%d)",
        t, sig->m_playerStatus, sig->m_dungeonIndex, sig->m_channelNo,
        sig->m_occCount);
    if (db->exec(true) != 1)
        return 0;
    // [L4-r1] P227 全表达式临时链：ORIG 0x0841bde0-0x0841be43 在 cMyTrace C1
    // 之前把 4 个 sig 字段装入 %edi/%esi/%ebx/-0x3c 并跨 C1 保活（3 push +
    // 帧 0x5c），两语句形 4.4 只在 ctor 后求实参（仅 push ebx + 帧 0x44）。
    cMyTrace(__PRETTY_FUNCTION__, 0x4f28, 0)(
        "content concurrent %d,%d,%d,%d\n",
        sig->m_playerStatus, sig->m_dungeonIndex, sig->m_channelNo,
        sig->m_occCount);
    return 1;
}

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

// ============================================================================
// DB_UpdatePvPGrade::dispatch —— ORIG 0x08428b26
// ============================================================================
// ORIG 0x8428b26：GetDBHandle(DB_HANDLE_2,0) → seLect pvp_grade_expand 排行
// （pvp_grade>=getPvPRankRateGrade()，limit 1500）；exec 失败或 0 行返 0。
// 昨日 1:00 的 mktime 基准（limitTime=cur-0x15180）筛 last_play_time；
// 行满足「pvp_grade>=getPvPExpLossGrade() && last_play<=limitTime &&
// pvp_point>refPoint(GetPvpPoint(getPvPExpLossGrade()))」：
//   pvp_point > getPvPExpLossExp()+refPoint → lossList（pvp_point-=expLoss）
//   否则 → resetList（pvp_point=refPoint）；pvp_point=max(refPoint, 原- expLoss)。
// 全部行入 rows（stPvPTempTable_t，charac_no）。
// 然后按 getPvPRankRate() 比率（(rate[i]*rowCount+500)/1000，单调钳制 ≥1，
// rowCount!=0 && rate[3]==0 → rate[4]=1）把 rows 分级入 gradeList[5]，
// 逐级 upDate pvp_result（pvp_point 减损/置 refPoint；grade_ext=5-g）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08428b26
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
    // SQL 行字段（ORIG 声明序先于 rate 数组，直传槽给 get_uint/get_int）。
    unsigned int characNo;
    int pvpGrade;
    int pvpPoint;
    unsigned int lastPlay;
    unsigned int i = 0;
    while (i < (unsigned int)db->get_n_rows())
    {
        if (db->fetch() != 1)
            return 0;
        if (!db->get_uint(0, characNo))
            return 0;
        if (!db->get_int(1, pvpGrade))
            return 0;
        if (!db->get_int(2, pvpPoint))
            return 0;
        if (!db->get_uint(3, lastPlay))
            return 0;
        if (GlobalData::s_ref_pvp_grade->getPvPExpLossGrade() <= pvpGrade &&
            (int)lastPlay <= limitTime && refPoint < pvpPoint)
        {
            if (GlobalData::s_ref_pvp_grade->getPvPExpLossExp() + refPoint <
                pvpPoint)
                // [b28d-solo] P270：ORIG 调 push_back(int&&)（EOi），
                // 值传选中 const& 版本 ⇒ 显式右值 cast。
                lists[0].push_back(static_cast<int&&>((int)characNo));
            else
                lists[1].push_back(static_cast<int&&>((int)characNo));
            int tmp = pvpPoint -
                      GlobalData::s_ref_pvp_grade->getPvPExpLossExp();
            pvpPoint = std::max(refPoint, tmp);
        }
        // ORIG push_back 直传 charac_no 槽（元素首 4 字节即 m_characNo）。
        rows.push_back(*(stPvPTempTable_t*)&characNo);
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
            static_cast<int&&>(rows[k].m_characNo));
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

// ============================================================================
// DB_UpdateServerMessage::dispatch —— ORIG 0x08435a02
// ============================================================================
// ORIG 0x8435a02：GetDBHandle(DB_HANDLE_6,0) → GetOutBuffer<
// SIG_UPDATE_SERVER_MESSAGE>（+0x10 message/+0x48 charac_name 先 escape_string
// 到 0x80/0x40 缓冲）→ upDate event_server_message（message/charac_name/
// update_time where server_info/channel_no/message_index）；exec 失败或
// affected==0 → inSert 7 占位；仍失败返 0。
// 成功后 Acquire("DBThread.cpp",0x80e3) → CStreamGuard(stream,true) →
// <<userIdx → <<slot → GetInBuffer<SIG_UPDATE_SERVER_MESSAGE>() 回包：
// memset 0x60 + 逐字段拷贝（server_info/channel_no/message_index/kind/
// charac_name_len/strncpy(charac_name)/message_len/strncpy(message)/
// update_time）→ MsgQueueMgr::put(DISPATCH_Q(1), guard) → 返 1。
// [b28d-4] DIFF：N1/N107：ORIG lea+add 两步寻址 vs 4.4 折单条；ORIG 08435a02
// [b28d-solo-4] §6.2 三件套：p167 CLEAR（ORIG 24 = OUR 24，0/0）+ gap 0（215/215）
//   ⇒ 语义完成；余 N4/N107 lea-add 折叠（P30/P319 已试，含字段/RHS 同型化）封顶。
#include "W8LT_DBTask/dispatch_89.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    DB_UpdateServerMessageView* sig =
        reinterpret_cast<DB_UpdateServerMessageView*>(
            stream->GetOutBuffer<SIG_UPDATE_SERVER_MESSAGE>());
    char msg[0x80];    // -0xdc..-0x5d（message escape 目标）
    char name[0x40];   // -0x5c..-0x1d（charac_name escape 目标）
    db->escape_string(msg, sig->m_message);
    db->escape_string(name, sig->m_characName);
    db->set_query(
        "upDate event_server_message set message='%s', charac_name='%s', update_time=from_unixtime(%d) where server_info=%d and channel_no=%d and message_index=%d",
        msg, name, sig->m_updateTime, sig->m_serverInfo, sig->m_channelNo,
        sig->m_messageIndex);
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_server_message(server_info, channel_no, kind, message_index, charac_name, message, update_time) values(%d, %d, %d, %d, '%s', '%s', from_unixtime(%d))",
            sig->m_serverInfo, sig->m_channelNo, sig->m_kind,
            sig->m_messageIndex, name, msg, sig->m_updateTime);
        if (db->exec(true) != 1)
            return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x80e3), true);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((int)slot);
    DB_UpdateServerMessageView* out =
        reinterpret_cast<DB_UpdateServerMessageView*>(
            guard->GetInBuffer<SIG_UPDATE_SERVER_MESSAGE>());
    memset(out, 0, 0x60);
    out->m_serverInfo = sig->m_serverInfo;
    out->m_channelNo = sig->m_channelNo;
    out->m_messageIndex = sig->m_messageIndex;
    out->m_kind = sig->m_kind;
    out->m_characNameLen = sig->m_characNameLen;
    // 残留（N 类）：ORIG 0x8435… `lea 0x1(%eax),%ecx` 独立输出寄存器
    // 物化；4.4 恒 `add $0x1,%eax` 原地复用（register asm("ecx"/"eax")
    // /具名槽探针全试，均反产 spill，probe t1-t5 实证）。
    strncpy(out->m_characName, sig->m_characName, sig->m_characNameLen + 1);
    out->m_messageLen = sig->m_messageLen;
    strncpy(out->m_message, sig->m_message, sig->m_messageLen + 1);
    out->m_updateTime = sig->m_updateTime;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}
// ============================================================================
// DB_SaveChannelUserLevel::dispatch —— ORIG 0x0841be50（报告 dispatch.md 全文）
// ============================================================================
// 无基类前置。GetOutBuffer<SIG_UPDATE_STATISTIC4LEV> → GetDBHandle(4,0) →
// lev=1..0x46 循环 inSert channel_lev_status：
//   values(from_unixtime(%d),%d,%d,%d) = m_time, m_gcNo, lev, m_count[lev]；
// 任一 exec 失败返 0，循环完成返 1。
#include "W8LT_DBTask/dispatch_90.inc"
{
    // 声明序照 ORIG 栈槽（低→高）：sig(-0x14) db(-0x10) lev(-0xc)。
    SIG_UPDATE_STATISTIC4LEV* sig =
        stream->GetOutBuffer<SIG_UPDATE_STATISTIC4LEV>();
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    for (int lev = 1; lev <= 0x46; ++lev)
    {
        db->set_query(
            "inSert into channel_lev_status(occ_time,channel_no,lev,occ_count) values(from_unixtime(%d),%d,%d,%d)",
            sig->m_time, sig->m_gcNo, lev, sig->m_count[lev]);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_SavePrivateStore::dispatch —— ORIG 0x0841bf02（报告 dispatch.md 全文）
// ============================================================================
// 无基类前置。GetOutBuffer<private_store::SIG_PRIVATE_STORE> → GetDBHandle(3,0)
// → upDate store set store='%s', use_doll='%d' where charac_no=%d
//   （%s=blob_to_str(0, m_items, 0x428)、use_doll=m_dollCreated、charac_no）；
// exec 成功且 getAffectedRowCount()!=0 → 返 1；否则补 inSert
//   values(%d, '%s', '%d') = m_characNo, blob, m_dollCreated；
// inSert exec 失败 cMyTrace(0x4f6d,5) 返 0，成功返 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0841bf02
#include "W8LT_DBTask/dispatch_91.inc"
{
    static const int handleIdx = 3;
    // 声明序照 ORIG 栈槽（低→高）：sig(-0x14) db(-0xc)。
    private_store::SIG_PRIVATE_STORE* sig =
        stream->GetOutBuffer<private_store::SIG_PRIVATE_STORE>();
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate store set store = '%s', use_doll = '%d' where charac_no =%d",
        db->blob_to_str(0, sig->m_items, 0x428),
        // ORIG 0x0841bf40：`mov 0x430(%eax),%eax; mov %eax,%ebx` 两段装载 ⇒
        // 实参处 (unsigned int) cast（探针 g4；P265 变体）。
        (unsigned int)sig->m_dollCreated,
        sig->m_characNo);
    // ORIG 0x0841bf9f：bVar2 无槽——`xor $1; test; jne` + `or %edx,%eax; test;
    // jne` + `mov $1/$0,%eax` 双常数岛，`test %al; je` 跳尾 ⇒ P226/P316 语句
    // 表达式零槽短路物化。
    if (({ db->exec(true) != 1 || db->getAffectedRowCount() == 0; }))
    {
        db->set_query(
            "inSert into store(charac_no, store, use_doll) values(%d, '%s', '%d')",
            sig->m_characNo,
            db->blob_to_str(0, sig->m_items, 0x428),
            (unsigned int)sig->m_dollCreated);
        if (db->exec(true) != 1)
        {
            // ORIG 0x0841c039：m_characNo 在 ctor 前落 %ebx 跨 ctor 保活
            // ⇒ P227 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x4f6d, 5)(
                "inSert store failed at %d", sig->m_characNo);
            return 0;
        }
    }
    return 1;
}
// ============================================================================
// DB_LoadPrivateStore::dispatch —— ORIG 0x0841c08c（报告 dispatch.md 全文）
// ============================================================================
// DBDispatcher::dispatch 前置失败返 0 → *stream >> characNo(int) →
// GetDBHandle(3,0) → seLect store, use_doll from store where charac_no=%d；
// exec 失败 cMyTrace(0x4f8b,5) 返 0。Acquire("DBThread.cpp",0x4f90) →
// guard << userIdx << slot → GetInBuffer<SIG_PRIVATE_STORE> → memset 0x434
// → m_characNo=characNo、m_field04=0；get_n_rows()!=0 时：
//   fetch 失败 cMyTrace(0x4fa0,5)+Free 返 0；
//   get_binary(0, m_items, 0x428) 失败 cMyTrace(0x4fa8,5)+Free 返 0；
//   m_field04=1；get_byte(1, useDoll) 失败 cMyTrace(0x4fb4,5)+Free 返 0；
//   m_dollCreated=(int)useDoll（movsbl 符号扩展）。put(DISPATCH_Q) 返 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0841c08c
#include "W8LT_DBTask/dispatch_92.inc"
{
    static const int handleIdx = 3;
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序照 ORIG 栈槽（低→高）：useDoll(-0x61) characNo(-0x5c)
    // r(-0x11) db(-0x10) sig(-0xc)；guard 于 exec 成功后构造（ORIG 同点）。
    char useDoll;
    int characNo;
    bool r;
    MySQL* db;
    private_store::SIG_PRIVATE_STORE* sig;
    *stream >> characNo;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query("seLect store, use_doll from store where charac_no=%d",
                  characNo);
    r = db->exec(true);
    if (r != 1)
    {
        cMyTrace trace1(__PRETTY_FUNCTION__, 0x4f8b, 5);
        trace1("DB_LoadPrivateStore::dispatch, exec ERROR charac_no=%d",
               characNo);
        return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x4f90), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    sig = guard.operator->()->GetInBuffer<private_store::SIG_PRIVATE_STORE>();
    memset(sig, 0, 0x434);
    sig->m_characNo = characNo;
    sig->m_field04 = 0;
    if (db->get_n_rows() != 0)
    {
        r = db->fetch();
        if (r != 1)
        {
            cMyTrace trace2(__PRETTY_FUNCTION__, 0x4fa0, 5);
            trace2("DB_LoadPrivateStore::dispatch, fetch ERROR charac_no=%d",
                   characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        r = db->get_binary(0, sig->m_items, 0x428);
        if (r != 1)
        {
            cMyTrace trace3(__PRETTY_FUNCTION__, 0x4fa8, 5);
            trace3("DB_LoadPrivateStore::GetUserInven, get_binary(2) ERROR charac_no=%d",
                   characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        sig->m_field04 = 1;
        useDoll = 0;
        r = db->get_byte(1, useDoll);
        if (r != 1)
        {
            cMyTrace trace4(__PRETTY_FUNCTION__, 0x4fb4, 5);
            trace4("DB_LoadPrivateStore::Get_Use_State, get_byte ERROR charac_no=%d",
                   characNo);
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        sig->m_dollCreated = (int)useDoll;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_LoadDimension::dispatch —— ORIG 0x08431ece（报告 dispatch.md 全文）
// ============================================================================
// DBDispatcher::dispatch 前置失败返 0 → GetDBHandle(2,0) →
// GetOutBuffer<SIG_LOAD_DIMENSION> → seLect charac_dimension_inout 6 列
// （%u=m_field00）：exec&&fetch 成功 → 逐列 get_byte(0..5,
// m_dimensionInout[i]) 任一失败返 0；否则（无行/查询失败）inSert 新行
// （6 字节 movsbl 符号扩展）exec 失败返 0。再查 charac_blood_inout
// dungeon1（%u=m_field00）：exec&&fetch 成功 → get_byte(0, m_limitInoutCount)
// 失败返 0；否则 inSert values(%u,%d)（%d 符号扩展）exec 失败返 0。
// Acquire("DBThread.cpp",0x7955) → guard << 0x14e << slot →
// GetInBuffer<SIG_LOAD_DIMENSION> → memcpy(out, sig, 0x10) →
// put(DISPATCH_Q) → 返 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08431ece
#include "W8LT_DBTask/dispatch_93.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // 声明序照 ORIG 栈槽（低→高）：guard(-0x2c) db(-0x24) sig(-0x20)
    // out(-0x1c)；guard 于末尾 put 前构造（ORIG 同点）。
    // [L4-r2 宽度旋钮已实测回退] widthscan clr_b/cmp_b = orig 0 / our 2 曾疑为
    // 「我方偏窄」，改 `int bVar1` 后 gap/四数不变而 ndiff 107→**109 反劣** ⇒
    // 该 byte 形非局部宽度问题（归 P292/N82 槽分区，见 Main 判据⑫ 反误判条）。
    MySQL* db =
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    SIG_LOAD_DIMENSION* sig = stream->GetOutBuffer<SIG_LOAD_DIMENSION>();
    db->set_query(
        "seLect dungeon1, dungeon2, dungeon3, dungeon4, dungeon5, dungeon6 from charac_dimension_inout where charac_no=%u",
        sig->m_field00);
    // [A04-r5] ORIG 0x08431f60-0x08431f8c：`call exec; xor $0x1,%eax; test %al,%al;
    // jne L_true` + `call fetch; xor $0x1; test; je L_false` + 常量岛
    // （mov $1/jmp/mov $0）汇聚 + `test %al,%al; je <get_byte 段>` ⇒ **零槽短路
    // 物化**（值只走 %eax，无 -0x25 槽）⇒ 原码无具名 bool 局部（P0），按 P226/P316
    // 用语句表达式包住整个条件（具名 bool 形 4.4 会落 `movb $0,-0x25` + `cmpb`）。
    if (({ db->exec(true) != 1 || db->fetch() != 1; }))
    {
        db->set_query(
            "inSert into charac_dimension_inout(charac_no,dungeon1,dungeon2,dungeon3,dungeon4,dungeon5,dungeon6) values(%u,%d,%d,%d,%d,%d,%d)",
            sig->m_field00,
            (int)(char)sig->m_dimensionInout[0],
            (int)(char)sig->m_dimensionInout[1],
            (int)(char)sig->m_dimensionInout[2],
            (int)(char)sig->m_dimensionInout[3],
            (int)(char)sig->m_dimensionInout[4],
            (int)(char)sig->m_dimensionInout[5]);
        if (db->exec(true) != 1)
            return 0;
    }
    else
    {
        // [A04-r5] ORIG 0x0843204e 起 6 个 get_byte 各发独立
        // `call; xor $0x1; test %al,%al; je <下一个>; mov $0x0,%ebx; jmp <epilogue>`
        // （每个失败出口独立早退）⇒ 必须拆成 6 条独立 `if (...!=1) return 0;`
        // （P133：合并 `||` 链在 4.4 折成单条 jne 汇聚，少 5 组早退岛）。
        if (db->get_byte(0, sig->m_dimensionInout[0]) != 1)
            return 0;
        if (db->get_byte(1, sig->m_dimensionInout[1]) != 1)
            return 0;
        if (db->get_byte(2, sig->m_dimensionInout[2]) != 1)
            return 0;
        if (db->get_byte(3, sig->m_dimensionInout[3]) != 1)
            return 0;
        if (db->get_byte(4, sig->m_dimensionInout[4]) != 1)
            return 0;
        if (db->get_byte(5, sig->m_dimensionInout[5]) != 1)
            return 0;
    }
    db->set_query(
        "seLect dungeon1 from charac_blood_inout where charac_no=%u",
        sig->m_field00);
    if (({ db->exec(true) != 1 || db->fetch() != 1; }))
    {
        db->set_query(
            "inSert into charac_blood_inout(charac_no,dungeon1) values(%u,%d)",
            sig->m_field00, (int)(char)sig->m_limitInoutCount);
        if (db->exec(true) != 1)
            return 0;
    }
    else
    {
        if (db->get_byte(0, sig->m_limitInoutCount) != 1)
            return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7955), true);
    guard.operator*()->operator<<(0x14e);
    guard.operator*()->operator<<(slot);
    SIG_LOAD_DIMENSION* out = guard->GetInBuffer<SIG_LOAD_DIMENSION>();
    memcpy(out, sig, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_UpdateAvatarHistory::dispatch —— ORIG 0x08445246（报告 dispatch.md 全文）
// ============================================================================
// 无基类前置。*stream >> uiId(uint) → >> characNo(int) →
// get_binary(agency, 0x18)（0x19 字节栈区清零，%s 尾零）→ GetDBHandle(3,0)
// → upDate user_items set to_ipg_agency_no='%s', m_time=now()
//   where ui_id=%d and charac_no=%d → 返回 exec(true)==1。
#include "W8LT_DBTask/dispatch_94.inc"
{
    // 声明序照 ORIG 栈槽（低→高）：agency(-0x2d) characNo(-0x14)
    // uiId(-0x10) r(-0x11) db(-0xc)。uiId 初始化最先（ORIG movl $0 在
    // agency 展开之前）；agency 用初始化列表（ORIG 内联展开与 gcc 4.4
    // 的 ={0} 初始化展开一致，非 memset 调用）。
    unsigned int uiId = 0;
    int characNo;
    char agency[0x19] = {0};
    MySQL* db;
    *stream >> uiId;
    *stream >> characNo;
    stream->get_binary(agency, 0x18);
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set to_ipg_agency_no='%s', m_time=now() where ui_id=%d and charac_no=%d",
        agency, characNo, uiId);
    if (!db->exec(true))
        return 0;
    return 1;
}

// StatisticsPvP::lock/unlock —— ORIG W 0x0844d524 / 0x0844d538（Mutex 转发，
// 反汇编 = call Mutex::lock/unlock；声明见 GameWorld.h，Main 批准共享头两声明）。
#include "W8LT_DBTask/lock.inc"
#include "W8LT_DBTask/unlock.inc"
#include "W8LT_DBTask/lock_1.inc"
#include "W8LT_DBTask/unlock_1.inc"
// StatisticsShusiaService::lock/unlock —— ORIG W 0x0844d4fc / 0x0844d510（Mutex 转发，
// StatisticsToObserveCrash::lock/unlock —— ORIG W 0x0844d4d0 / 0x0844d4e6（Mutex 转发，
// 反汇编 = call Mutex::lock/unlock；声明见 GameWorld.h，Main 批准共享头两声明；
// DB_CrashDownReport::dispatch 0x0841abc6 遍历前 lock / 遍历后 unlock）。
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
{
    // P157/P183：ORIG 三处独立 `G_GameWorld()->getStatisticsPvP()`（p167 差集
    // -3/-3；0x0841ba96），禁指针缓存。
    G_GameWorld()->getStatisticsPvP()->lock();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    std::map<StatisticsPvP::PvP_Key, StatisticsPvP::PvP_Room>::iterator it =
        G_GameWorld()->getStatisticsPvP()->m_roomResults.begin();
    std::map<StatisticsPvP::PvP_Key, StatisticsPvP::PvP_Room>::iterator end =
        G_GameWorld()->getStatisticsPvP()->m_roomResults.end();
    // [SweepIntervalSet 真 bug] 原 `while (it != end) { … continue; … ++it; }` 的
    // continue 跳到条件处、跳过 ++it ⇒ update 也失败时 it 不变 = 死循环。
    // ORIG 0x0841bca0 单一 operator++ 调用点在循环尾（continue 必须经过它）⇒
    // 源形为 `for (; it != end; ++it)`（4.1 布局：入口 jmp 条件 / body / ++it /
    // 条件 jne body，实证 0x0841bb18/0x0841bc9a/0x0841bcb9）。
    // 另：ORIG 0x0841bb89 / 0x0841bbe8 两处 exec 结果各占具名槽（-0x1a/-0x19，
    // `mov %al,槽; movzbl 槽,%eax; xor $1`）⇒ 两个具名 bool 局部（P0/P102）。
    for (; it != end; ++it)
    {
        // P167：ORIG 用 `operator*`（deEv）×2，我方 `it->` 选中 ptEv ×2 ⇒
        // 写 `(*it).first/second`。
        const StatisticsPvP::PvP_Key& key = (*it).first;
        // [M8DBB] 真偏移视图（ORIG 16B，见上）；`&(*it).second` 仍走 deEv+8。
        const DB_StatPVPPlay2RoomView& room =
            *(const DB_StatPVPPlay2RoomView*)&(*it).second;
        db->set_query(
            "inSert into pvp_status(occ_date,room_number,play_type,no_participants,occ_count) values(cast(now() as date),%d,%d,%d,%d)",
            key.m_a, key.m_b, room.m_resultSum, room.m_roomCount);
        bool inserted = db->exec(true);
        if (!inserted)
        {
            db->set_query(
                "upDate pvp_status set occ_count = occ_count + %d , no_participants = no_participants + %d where occ_date = cast(now() as date) and room_number = %d and play_type = %d",
                room.m_roomCount, room.m_resultSum, key.m_a, key.m_b);
            bool updated = db->exec(true);
            if (!updated)
            {
                // [L4-r1] P227 全表达式临时链（ORIG ctor 前取 varargs + 跨 ctor 保活）。
                cMyTrace(__PRETTY_FUNCTION__, 0x4ed8, 5)(
                    "upDate pvp_status failed at %d", key.m_a);
                continue;  // for 的 increment 处 = ORIG 单一 operator++ 调用点
            }
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x4edc, 5)(
            "pvp_status : %d ROOM %d TYPE %d Players %d Count ",
            key.m_a, key.m_b, room.m_resultSum, room.m_roomCount);
    }
    G_GameWorld()->getStatisticsPvP()->unlock();
    return 1;
}

// ============================================================================
// DB_StatDungeonFail::dispatch —— ORIG 0x0841b6e0（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x841b6e0：G_GameWorld()->getStatisticsShusiaService() → lock →
// GetDBHandle(DB_HANDLE_4,0) → 遍历 m_dungeonFailure（键=lev，值=occ_count）：
//   每项先 inSert dungeon_fail(occ_date,lev,occ_count)
//   values(cast(now() as date),%d,%d)（lev,count）；exec!=1 → upDate
//   set occ_count=occ_count+count where occ_date=now() and lev=lev
//   （count,lev）；仍失败 → cMyTrace(0x4e8f,5) "upDate dungeon_fail failed
//   at %d"(lev)。
// 再遍历 m_serviceUsage（同形）：inSert re_stamina_shusia(key,value)；成功
//   → cMyTrace(0x4eb3,5) "re_stamina_shusia : %d SHUSIA USAGE %d"；
//   失败 → cMyTrace(0x4ea9,5) "inSert into re_stamina_shusia failed at %d"
//   + upDate re_stamina_shusia set occ_count=occ_count+value where
//   occ_date=now() and lev=key；update 成功 → 同上 0x4eb3 成功日志；
//   update 失败 → cMyTrace(0x4eaf,5) "upDate re_stamina_shusia failed at %d"。
// 两循环结束 unlock → reset → return 1。
// [b28d-solo-2R] NEAR(16)：P221/P227 三处具名 cMyTrace→全表达式临时链后残差 36→16 条；
//   余 lev/count/r 槽深差 4（-0x24/-0x20/-0x1a vs -0x20/-0x1c/-0x16）+ db2 槽方向
//   （ORIG -0x18 浅 vs 4.4 -0x24 深）= N82 分配器差（4.1 不复用/4.4 复用），封顶。ORIG 0x841b6e0
#include "W8LT_DBTask/dispatch_96.inc"
{
    G_GameWorld()->getStatisticsShusiaService()->lock();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    std::map<int, int>::iterator it =
        G_GameWorld()->getStatisticsShusiaService()->m_dungeonFailure.begin();
    std::map<int, int>::iterator end =
        G_GameWorld()->getStatisticsShusiaService()->m_dungeonFailure.end();
    while (it != end)
    {
        // [b28d-solo] P167：ORIG 调 `_Rb_tree_iterator::operator*`（deEv），
        // 我方 `it->` 选中 ptEv（差集 -4/+4）⇒ 写 `(*it).`。
        int lev = (*it).first;
        int count = (*it).second;
        db->set_query(
            "inSert into dungeon_fail(occ_date,lev,occ_count) values(cast(now() as date),%d,%d)",
            lev, count);
        bool r = db->exec(true);
        if (!r)
        {
            db->set_query(
                "upDate dungeon_fail set occ_count = occ_count + %d where occ_date = now() and lev = %d",
                count, lev);
            bool r2 = db->exec(true);
            if (!r2)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x4e8f, 5)
                    ("upDate dungeon_fail failed at %d", lev);
            }
        }
        ++it;
    }
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    std::map<int, int>::iterator it2 =
        G_GameWorld()->getStatisticsShusiaService()->m_serviceUsage.begin();
    std::map<int, int>::iterator end2 =
        G_GameWorld()->getStatisticsShusiaService()->m_serviceUsage.end();
    while (it2 != end2)
    {
        int key = (*it2).first;
        int value = (*it2).second;
        db2->set_query(
            "inSert into re_stamina_shusia(occ_date,lev,occ_count) values(cast(now() as date),%d,%d)",
            key, value);
        bool r = db2->exec(true);
        if (!r)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4ea9, 5)
                ("inSert into re_stamina_shusia failed at %d", key);
            db2->set_query(
                "upDate re_stamina_shusia set occ_count = occ_count + %d where occ_date = now() and lev = %d",
                value, key);
            bool r2 = db2->exec(true);
            if (!r2)
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x4eaf, 5)
                    ("upDate re_stamina_shusia failed at %d", key);
                goto loop_end;
            }
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x4eb3, 5)
            ("re_stamina_shusia : %d SHUSIA USAGE %d", key, value);
loop_end:
        ++it2;
    }
    G_GameWorld()->getStatisticsShusiaService()->unlock();
    G_GameWorld()->getStatisticsShusiaService()->reset();
    return 1;
}

// ============================================================================
// DB_StatUseShusiaService::dispatch —— ORIG 0x0841bce4（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x841bce4：push/mov esp/mov $0x1,%eax/pop/ret —— 恒返 1（参数未使用）。
#include "W8LT_DBTask/dispatch_97.inc"
{
    return 1;
}

// ============================================================================
// DB_SaveGamechannel::dispatch —— ORIG 0x0841c428（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x841c428：GetOutBuffer<SIG_SAVE_LOGGAMECHANNEL>() →
// WongWork::CLogGameChannel::DoSaveLog(GlobalData::s_pLogGameChannel, sig)
// （CLogGameChannel::GetLogTargetDB 取 DB_HANDLE_4 落 log_game_channel 表）
// → return 1。
#include "W8LT_DBTask/dispatch_98.inc"
{
    SIG_SAVE_LOGGAMECHANNEL* sig =
        stream->GetOutBuffer<SIG_SAVE_LOGGAMECHANNEL>();
    GlobalData::s_pLogGameChannel->DoSaveLog(sig);
    return 1;
}

// ============================================================================
// DB_UpdateQueryCountTimerLogdb::dispatch —— ORIG 0x0841c458（报告全文）
// ============================================================================
// ORIG 0x841c458：GetDBHandle(DB_HANDLE_4,0) → GetQueryCounter() →
// GetQueryCountTable()（map<int,QUERY_INFO>，QUERY_INFO{count,int;
// responseTime,double}）→ s_systemTime_.getCurSec() → for 遍历：
//   qid=first、total=second.count；total>0 时：
//   respTime=(int)(1000.0 * second.responseTime)（_DAT_08c5dec8=1000.0，
//   fldcw 0x0c + fistpl 截断，即普通 (int) 强转）→
//   set_query("inSert into log_query_stat(occ_time,q_id,total,response_time,
//   gc_no) values(from_unixtime(%d),%d,%d,%d,%d)", curSec, qid, total,
//   respTime, G_CEnvironment()->get_channel_no()) → exec(true)（返回值
//   丢弃：ORIG xor $1/test/jmp 死代码）；end() 每轮调用（P13 反例）。
// 循环结束 counter->Reset() → return 1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0841c458
// [r3] 已修：`if (!db->exec(true)) goto L_next_query;` → `continue;` ⇒ 复现
//   ORIG 0841c56d `xor $0x1,%eax; test %al,%al; jmp <incr>` 死岛 ⇒
//   ndiff 2→0、gap 2→0（101/101、差集空）。旧 P168a③「封顶」判断作废：
//   4.4 对 `continue;`（循环尾）保留物化，对 `goto L_next_query;` 才 jump-thread。
#include "W8LT_DBTask/dispatch_99.inc"
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    CQueryCounter* counter = db->GetQueryCounter();
    std::map<int, CQueryCounter::QUERY_INFO>& table =
        counter->GetQueryCountTable();
    int curSec = GlobalData::s_systemTime_.getCurSec();
    for (std::map<int, CQueryCounter::QUERY_INFO>::iterator it =
             table.begin();
         it != table.end(); it++)
    {
        int qid = it->first;
        int total = it->second.count;
        // P223 负条件 goto 共享尾空标签（复现 4.1 if-join 岛：jle→nop + 体尾 jmp）。
        if (0 >= total)
            goto L_next_query;
        {
            int respTime = (int)(1000.0 * it->second.responseTime);
            db->set_query(
                "inSert into log_query_stat(occ_time,q_id,total,response_time,gc_no) values(from_unixtime(%d),%d,%d,%d,%d)",
                curSec, qid, total, respTime,
                G_CEnvironment()->get_channel_no());
            // [b28c-33 复核 102/100：残 2 = jmp+nop 岛] ORIG 0x841c56d-0x841c572
            // `xor $1;test %al;jmp <incr>` 死岛 =
            // 4.1 对 `if(!exec()) continue;` 保留物化 + 块尾 jmp（jle 落点
            // nop pad 在前）；4.4 threading 消 jmp ⇒ N75 残留一行。
            if (!db->exec(true))
                continue;
        }
    L_next_query:;
    }
    counter->Reset();
    return 1;
}

// ============================================================================
// DB_UpdateQueryCountTimerDb::dispatch —— ORIG 0x0841c5d4（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x841c5d4：GetDBHandle(DB_HANDLE_2,0) → GetQueryCounter() →
// CQueryCounter::UpdateQueryCounter()（迁移 m_queryInfoMap 计数
// m_totalQueryCount）→ return 1。
#include "W8LT_DBTask/dispatch_100.inc"
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    CQueryCounter* counter = db->GetQueryCounter();
    counter->UpdateQueryCounter();
    return 1;
}

// ============================================================================
// DB_InsertPVPMap::dispatch —— ORIG 0x0842cb12
// ============================================================================
// DB_InsertPVPMap::dispatch —— ORIG 0x0842cb12
// ============================================================================
// ORIG 0x842cb12：GetDBHandle(DB_HANDLE_4,0) → count/mapId/playCount=0 →
// string ctor → Stream>>count → 循环 count 次：Stream>>mapId、Stream>>
// playCount → sprintf(local_424,"(%d,now(),%d,%d)",channel_no,mapId,
// playCount)（channel_no=G_CEnvironment()->get_channel_no()）→ string +=
// 缓冲；非末项 string += ","。循环后
// set_query("inSert into log_pvp_map(channel_no,occ_date,map_id,play_count)
// values%s", string.c_str()) → exec(true) → return exec==1。
// 异常路径（C++98 try/catch 由 ORIG 编译器生成）：string dtor 后 rethrow。
#include "W8LT_DBTask/dispatch_101.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int count = 0;
    int mapId = 0;
    int playCount = 0;
    std::string sql;
    char buf[1024];
    *stream >> count;
    for (int i = 0; i < count; ++i)
    {
        *stream >> mapId;
        *stream >> playCount;
        sprintf(buf, "(%d,now(),%d,%d)", G_CEnvironment()->get_channel_no(),
                mapId, playCount);
        sql += buf;
        if (i < count - 1)
            sql += ",";
    }
    db->set_query(
        "inSert into log_pvp_map(channel_no,occ_date,map_id,play_count) values%s",
        sql.c_str());
    // ORIG 0x842…：返回值经 `xor$1;test;je` 双常数物化入 %ebx 再走
    // string dtor 尾（4.1 bool 物化形）。4.4 对 `== 1` 恒折裸值直通
    // ⇒ 试直判返形（技巧1/17 系）。
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_SaveProxyLog::dispatch —— ORIG 0x08422742
// ============================================================================
// ORIG 0x8422742：GetOutBuffer<SIG_SAVE_PROXY_LOG>() →
// GetDBHandle(DB_HANDLE_4,0) → set_query("inSert into dnf_proxy_log
// values(now(),%s,'%s','%s',%d)", NumberToString(accId,0), m_strIp,
// m_peerIp, (uint)(byte)m_natType) → exec(true) → return exec==1。
#include "W8LT_DBTask/dispatch_102.inc"
{
    SIG_SAVE_PROXY_LOG* sig = stream->GetOutBuffer<SIG_SAVE_PROXY_LOG>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into dnf_proxy_log values(now(),%s,'%s','%s',%d)",
        NumberToString(sig->m_accId, 0), sig->m_strIp, sig->m_peerIp,
        (unsigned int)sig->m_natType);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_LoadPvPBuddy::dispatch —— ORIG 0x0843a998
// ============================================================================
// ORIG 0x843a998：push/mov esp/mov $1,%eax/pop/ret —— 恒返 1（无参使用）。
#include "W8LT_DBTask/dispatch_103.inc"
{
    return 1;
}

// ============================================================================
// DB_UpdateGMData::dispatch —— ORIG 0x0843990a
// ============================================================================
// ORIG 0x843990a：Stream>>bool（消费首字节，值未用）→
// Gm_List::LoadGmListFromDb(this=GlobalData::gmList) → return 1。
// 声明见 W8LT_DBTask.h（Main 批准补 virtual bool dispatch）。
#include "W8LT_DBTask/dispatch_104.inc"
{
    bool flag = false;
    *stream >> flag;
    GlobalData::gmList->LoadGmListFromDb();
    return 1;
}
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

// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
#include "W8LT_DBTask/dispatch_105.inc"
{
    // [b28c-27] AE：循环体 if 块按 P223 goto 尾标签岛复现 ORIG `ja <nop>` +
    // body 尾 `jmp <addl>; nop`（842245d/842245e 实证）；其余为 .o 重定位伪差。
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    // ORIG 槽序：out(-0x10) 高于 inSig(-0x14) 高于 mId(-0x18) ⇒ inSig 先声明。
    DB_DnfHackLogView* inSig;
    SIG_LOAD_DNF_HACK_LOG* outSig =
        stream->GetOutBuffer<SIG_LOAD_DNF_HACK_LOG>();
    mId = reinterpret_cast<DB_DnfHackLogView*>(outSig)->m_mId;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5a8e), true);
    guard.operator*()->operator<<(0x106);
    guard.operator*()->operator<<(slot);
    inSig = reinterpret_cast<DB_DnfHackLogView*>(
        guard.operator->()->GetInBuffer<SIG_LOAD_DNF_HACK_LOG>());
    memset(inSig, 0, 0x3cf4);
    inSig->m_mId = mId;
    db->set_query(
        "seLect hack_type,cnt from dnf_hack_log where m_id=%s and (hack_type>=%d and hack_type<%d) and occ_date=CAST(DATE_FORMAT(now(), '%%Y-%%m-%%d') AS CHAR)",
        NumberToString(mId, 0), 0x65, 0xfa1);
    if (!db->exec(true))
        return 0;
    for (unsigned int i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        unsigned int hackType = 0;
        unsigned int cnt = 0;
        if (!db->fetch())
            return 0;
        if (!db->get_uint(0, hackType))
            return 0;
        // P223：ORIG 842245d `ja <nop>` + body 尾 `jmp <addl>; nop`（4.1 空块
        // 占位岛）；4.4 对 if 恒 jump-thread，复现形 = goto 尾标签。
        if (hackType > 0xfa0)
            goto L_next;
        if (!db->get_uint(1, cnt))
            return 0;
        inSig->m_cnt[hackType - 0x65] = cnt;
L_next:;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}


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
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_GEO_REJECT* sig = stream->GetOutBuffer<SIG_SAVE_GEO_REJECT>();
    db->set_query(
        "upDate geo_reject set rej_ip_count = rej_ip_count + 1, rej_src = 'g' where rej_ip = '%s'",
        sig);
    // ORIG 0x08437…：汇聚 island 在 %eax（mov $1/jmp/mov $0/test %al,je）——
    // gcc4.4 `if (!(exec==1 && rows!=0))` 直判形产生同岛（UpdateLogin 实证），
    // register bool 反而落 %bl（P78），故用直判形。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into geo_reject(rej_ip, rej_c_code, rej_ip_count, rej_src) values('%s', '%s', 1, 'g')",
            // ORIG 0x844bbf9-0x844bc06：`mov -0xc,%eax; lea 0x10(%eax),%edx;
            // mov -0xc,%eax; mov %edx,0xc(%esp); mov %eax,0x8(%esp)` —— 两实参
            // 均取视图数组 decay（+0 decay 零额外指令）；首实参传裸指针时 4.4
            // 对第二实参产 `add $0x10,%eax` 覆写形（探针 /tmp/w8lt_lea.o::g）。
            ((DB_GeoRejectView*)sig)->m_rejIp,
            ((DB_GeoRejectView*)sig)->m_rejCCode);
        db->exec(true);
    }
    return 1;
}

// ============================================================================
// DB_AuctionSubMoney —— ORIG dispatch 0x08448530。
// 报告：docs/class_func_reports/DB_AuctionSubMoney/dispatch.md。
// dispatch：DB_HANDLE_2；escape_string(m_name) 后按角色名查 m_id（exec/fetch/get_int
// 任一失败返 0）→ 组包（0x309/slot，写 m_count=m_id、m_arg04=sig+4）→
// put(DISPATCH_Q) 返 1。
// b28c-28 判词（N18a）：fresh 唯一残差 = `out->m_count = mId` 处 ORIG
// 0x08448704 `mov -0x28(%ebp),%edx`（一步直落目标寄存器）vs 4.4 恒
// `mov -0x28,%eax; mov %eax,%edx`（两段，+1 insn）。探针 /tmp/l28_probe6.cpp
// h1/h2/h3（unsigned 槽→int 字段 / 形参直传 / 双字段）实测：仅「值未经 call
// 边界」时可一步，call 后装载恒两段 ⇒ N18a 封顶，余 147/147 全等。
// ============================================================================
// [b28d-4] AE：P248 档③ 局部 mId unsigned→int 同型化，消 `mov %eax,%edx`
// 符号转换中转，复现 ORIG 0x08448530 单段直落 store（156=156 全等）。
// [b28d-solo-3] 复核：int 同型化后 p167 显示 get_int 重载差（MISSING Rj /
// EXTRA Ri）；改调用点 `(unsigned int&)mId` 直传 ⇒ p167 CLEAR 且形态保持
// 156=156 AE（int 槽与 int 字段同型，仅取址类型切换，位模式不变）。
#include "W8LT_DBTask/dispatch_107.inc"
{
    // ORIG 0x08448530 语句序（P165 物理展开）：GetOutBuffer → mId=0 →
    // GetDBHandle → buf[0x3b] 零初始化（GCC 对 0x3b 错位缓冲按 &1/&2/rep 分块）。
    SIG_AUCTION_MONEY_SUB* sig =
        stream->GetOutBuffer<SIG_AUCTION_MONEY_SUB>();
    // P248 档③：m_count 为 int ⇒ 局部同型化（unsigned→int）以消 4.4 的
    // `mov %eax,%edx` 符号转换中转（ORIG 0x08448530 该 store 为单段 RHS-first）。
    // [b28d-solo] P167：ORIG 调 `MySQL::get_int(int, unsigned int&)`
    // （差集 -1 get_int(Rj)/+1 get_int(Ri)）；但 unsigned 局部与 m_count(int)
    // 异型会多 `mov %eax,%edx` 中转（diff 1→6，AE 回归）⇒ 维持 int 同型，
    // 重载差（语义等价，位模式相同）记 N。
    int mId = 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    char buf[0x3b] = { '\0' };
    db->escape_string(buf, sig->m_name);
    db->set_query("seLect m_id from charac_info where charac_name='%s' limit 1",
                  buf);
    if (!db->exec(true))
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_int(0, (unsigned int&)mId))
        return 0;
    // ORIG 0x0844866c(+0x13c)：CStreamGuard 在 get_int 之后才构造（前移会改
    // 帧布局与语句序，P165 物理展开序），非函数顶部。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb551), true);
    guard.operator*()->operator<<(0x309);
    guard.operator*()->operator<<(slot);
    // [b22] GetInBuffer 走 operator->（ORIG ptEv + GetInBuffer），非 operator*
    // （P132：operator-> 链保留，P167 显示 pt-1/de+1 实缺）。
    SIG_AUCTION_MONEY_SUB* out =
        guard.operator->()->GetInBuffer<SIG_AUCTION_MONEY_SUB>();
    out->m_count = mId;
    out->m_arg04 = sig->m_arg04;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_CancelJoinGuild —— ORIG dispatch 0x0843d6ea / makeRequest 0x0843d78c。
// 报告：docs/class_func_reports/DB_CancelJoinGuild/{dispatch,makeRequest}.md。
// dispatch：DB_HANDLE_8；Stream>>guildId>>characNo → deLete guild_join_list
// → 返回 exec(true)==1（!exec 早退形态）。
// ============================================================================
#include "W8LT_DBTask/dispatch_108.inc"
{
    unsigned int guildId;
    unsigned int characNo;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    guildId = 0;
    characNo = 0;
    *stream >> guildId;
    *stream >> characNo;
    db->set_query(
        "deLete from guild_join_list where guild_id=%d and charac_no=%d",
        guildId, characNo);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_InsertPvPBuddy —— ORIG dispatch 0x0843a9a2（报告 dispatch.md）。
// ORIG 本体即恒返 1（9 字节：push ebp/mov esp,ebp/mov $1,%eax/pop ebp/ret；
// 三参均未使用），非绕过。
// ============================================================================
#include "W8LT_DBTask/dispatch_109.inc"
{
    return 1;
}

// ============================================================================
// DB_DeletePvPBuddy —— ORIG dispatch 0x0843a9ac（报告 dispatch.md）。
// ORIG 本体即恒返 1（9 字节：push ebp/mov esp,ebp/mov $1,%eax/pop ebp/ret；
// 三参均未使用），非绕过。
// ============================================================================
#include "W8LT_DBTask/dispatch_110.inc"
{
    return 1;
}

// ============================================================================
// DB_BlackIPMonitor —— ORIG dispatch 0x08432ad2（报告 dispatch.md）。
// GetDBHandle(DB_HANDLE_6,0) → set_query(ip_monitor_punish 全表) →
// exec 失败返 0 → cMyTrace(0x7a03) 打行数 → 循环（条件每轮重取
// get_n_rows()）：fetch 失败返 0；满 100 行或末行时组包
// Acquire("DBThread.cpp",0x7a28) → <<0x156 → <<-1 →
// GetInBuffer<SIG_IP_MONITOR_PUNISH>：+0=count、+4..+0xc83=item 数组
// （行步长 0x20：ip[0x10]/type/m_id_cnt/start_time/end_time）、+0xc84=flag
// （首包 1 后续 0）→ put(DISPATCH_Q)。行缓冲 memset 0xc80、组包 memcpy 0xc80。
// （N 封顶：ndiff 263 残差为 lea/shl/add 地址重物化(P183) + cMyTrace 独立
// 槽(N82) + xor 极性，见报告 dispatch.md）
// ============================================================================
// [b28d-4] DIFF：P222：块序/极性（ORIG je 落点 vs 4.4 jne）；ORIG 08432ad2
#include "W8LT_DBTask/dispatch_111.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect ip, type, m_id_cnt, unix_timestamp(start_time), unix_timestamp(end_time) from ip_monitor_punish ORDER BY start_time ASC");
    if (!db->exec(true))
        return 0;
    // P37 临时链式：ORIG 先 call get_n_rows 以 ebx 缓存（跨 cMyTrace 构造），
    // 再构造临时并以其为 operator() 实参。
    cMyTrace(__PRETTY_FUNCTION__, 0x7a03, 0)(
        "BLACKIP GET DB ROWS COUNT : %d", db->get_n_rows());
    unsigned int row = 0;        // -0x18 行索引（循环条件无符号比较 seta）
    // [b28d-TakeW8LT] ORIG 0x08432ad2 的 `cmpl $0x62,-0x14; ja` 是无符号比较
    // （旧形 int ⇒ 4.4 发 jg）⇒ 源码类型为 unsigned int。
    unsigned int buffered = 0;   // -0x14 当前包已缓冲行数
    // [SweepIntervalSet] P248 档②/P292：ORIG 0x0843xx `movzbl -0xd(%ebp),%eax;
    // xor $0x1,%eax; test %al,%al; je`（无 setcc）⇒ flag 为 bool（char 形被 4.4
    // 折成 `cmpb $0x0,-0xd; jne`，少 2 条）。
    bool flag = false;           // -0x0d 首包标志
    char buf[0xc80];             // -0xcb4 行缓冲（100 × 0x20）
    // [A04-r5] 判据⑫ clr_l orig=4/our=3：ORIG 0x08432b7b 与 0x08432b8d **两次**
    // `movl $0x0,-0x18(%ebp)`（row 声明初始化 + for init；P191 双初始化形），
    // 循环为 for 布局（init→jmp cond→body→increment@0x08432e53→cond@0x08432e57）。
    for (row = 0; db->get_n_rows() > row; ++row)
    {
        if (!db->fetch())
            return 0;
        if (buffered == 0)
            memset(buf, 0, 0xc80);
        // [b28d-TakeW8LT 形复原] ORIG 0x08432ad2 无 item 指针局部：5 个站点
        // 各自重算 `buf + buffered*0x20`（ORIG add 18/shl 5 vs 旧形 6/1）。
        if (!db->get_str(0, ((DB_IpMonitorPunishItem*)buf)[buffered].m_ip, 0x10))
            return 0;
        if (!db->get_int(1, ((DB_IpMonitorPunishItem*)buf)[buffered].m_type))
            return 0;
        if (!db->get_uint(2, ((DB_IpMonitorPunishItem*)buf)[buffered].m_idCnt))
            return 0;
        if (!db->get_uint(3, ((DB_IpMonitorPunishItem*)buf)[buffered].m_startTime))
            return 0;
        if (!db->get_uint(4, ((DB_IpMonitorPunishItem*)buf)[buffered].m_endTime))
            return 0;
        if (buffered <= 98 && db->get_n_rows() - 1 > row)
        {
            ++buffered;
        }
        else
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7a28),
                true);
            guard.operator*()->operator<<(0x156);
            guard.operator*()->operator<<(-1);
            // [Lane31W8LTDBTaskView] 视图 cast 反模式→权威头具名直访问（ORIG
            // 0x08432ad2：`mov %eax,0x0(%edx)` 等 RHS-first；cast 形在 4.4 下
            // base 先入 callee-saved 跨 call）。
            SIG_IP_MONITOR_PUNISH* sig =
                guard.operator->()->GetInBuffer<SIG_IP_MONITOR_PUNISH>();
            sig->m_count = buffered + 1;
            memcpy(&sig->m_items[0], buf, 0xc80);
            if (!flag)
            {
                flag = true;
                // [J4S1 P73/P136] ORIG 0x08432deb `movb $1,-0xd` →
                // 0x08432df2 `movzbl -0xd,%edx` → `mov %dl,0xc84` ⇒ 原码经
                // bool 变量读回赋值（非直写常量）。
                sig->m_flag = flag;
            }
            else
            {
                sig->m_flag = 0;
            }
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            buffered = 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_CheckAddFriend —— ORIG dispatch 0x0844c6e0（报告 dispatch.md）。
// stream 空/GetOutBuffer<SIG_CHECK_ADD_FRIEND> 空返 0。请求 +0x04 字节标志
// 非 0：GetDBHandle(DB_HANDLE_2,0) → escape_string(+0x05 名字,0x3b 缓冲) →
// set_query(seLect m_id from charac_info where charac_name='%s' and
// delete_flag=0) → exec/fetch/get_n_rows==1/get_int(0) 全成后 isGM(m_id)：
// GM → 结果 0x5a，非 GM → 0；任一失败保持初始 1。标志==0 → 结果 0。
// 组包 Acquire("DBThread.cpp",0xc05e) → <<0x33e → <<slot →
// GetInBuffer<SIG_CHECK_ADD_FRIEND>：+0=结果、strncpy(+0x05,名字,0x1e)
// → put(DISPATCH_Q) 返 1；GetInBuffer 空返 0。
// ============================================================================
// [SweepW8LT b28d] DIFF：gap 18（our 200 / ORIG 218）封顶。p167 22/22 [0缺0多]
// （CStreamGuard C1/C2、D1/D2 同址别名）。残差：① P292 int→bool 归一化——ORIG
// escape_string 后 `test %eax,%eax; sete %al; test %al,%al; jne`，set_query/
// exec/fetch/get_int 后各多一条 `xor $1`（4.4 直接 test %al; je）；② P223 五个
// 失败出口各带 `nop; jmp` 中继岛（4.4 jump-thread 消岛，-10）；③ 两处
// `outCode = 0` 4.1 tail-merge 共享（844c8a9），4.4 保留两份（+1）。ORIG 0844c6e0
#include "W8LT_DBTask/dispatch_112.inc"
{
    int result = 0;              // -0x2c 查询 m_id
    MySQL* db = 0;               // -0x28
    int outCode = 1;             // -0x24 回包结果码
    DB_CheckAddFriendView* req = 0;  // -0x20
    DB_CheckAddFriendView* out = 0;  // -0x1c
    char name[0x3b] = { '\0' };      // -0x6f escape 目标
    if (stream == 0)
        return 0;
    req = (DB_CheckAddFriendView*)stream->GetOutBuffer<SIG_CHECK_ADD_FRIEND>();
    if (req == 0)
        return 0;
    if (req->m_flag != 0)
    {
        db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                               E_SERVER_GROUP_NONE);
        // P223：ORIG 0x0844c7c5/0x0844c7ee/0x0844c813/0x0844c863/0x0844c884
        // 五个失败出口各带独立 `nop; jmp` 中继岛（0x1d2/0x1d5/0x1d8/0x1db/0x1de），
        // 合并 && 短路链被 4.4 jump-thread 成单出口 ⇒ 改多源裸 goto 到同一尾标签。
        if (db == 0)
            goto send_packet;
        if (!db->escape_string(name, req->m_name))
            goto send_packet;
        if (!db->set_query(
                "seLect m_id from charac_info where charac_name='%s' and delete_flag=0",
                name))
            goto send_packet;
        if (!(db->exec(true) && db->fetch() && db->get_n_rows() == 1))
            goto send_packet;
        if (!db->get_int(0, result))
            goto send_packet;
        // [A04-r5c] C05③/P223 同目标合并：ORIG 唯一一份清零块 @0x0844c8a9
        // （`movl $0x0,-0x24(%ebp); jmp +0x1df`），两条 jcc 同目标 0844c8a9
        // 实证（flag==0 的 `je`@0x0844c79b 与 isGM==false 的 `je`@0x0844c89e）；
        // GM 路径 0x0844c8a0 `movl $0x5a,-0x24; jmp <after>` 跳过清零块。
        // 原 `else outCode=0;` 双份写法使 4.4 发两份 movl $0（多 1 条）。
        if (GlobalData::s_pGMAccounts_->isGM((unsigned int)result))
        {
            outCode = 0x5a;
            goto send_packet;
        }
    }
    goto set_zero;
set_zero:
    outCode = 0;
send_packet:
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc05e), true);
    guard.operator*()->operator<<(0x33e);
    guard.operator*()->operator<<(slot);
    out = (DB_CheckAddFriendView*)guard.operator->()
              ->GetInBuffer<SIG_CHECK_ADD_FRIEND>();
    if (out != 0)
    {
        out->m_result = outCode;
        strncpy(out->m_name, req->m_name, 0x1e);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    return 0;
}

// ============================================================================
// DB_GetUserRegdate —— ORIG dispatch 0x08442e68（报告 dispatch.md）。
// GetOutBuffer<SIG_GET_USER_REGDATE> → GetDBHandle(DB_HANDLE_1,0) →
// set_query(seLect DATE_FORMAT(from_unixtime(reg_date),'%%Y%%m%%d%%H')
// ,reg_date from member_info where m_id=%d, sig->m_id) → exec 失败返 0；
// get_n_rows()!=0 时 fetch/get_uint(0)/get_uint(1) 任一失败返 0；
// sig->m_regDay = day/100（无符号 magic 除法）、m_regDate = reg →
// return SendResult(userIdx, slot, sig)。
// ============================================================================
#include "W8LT_DBTask/dispatch_113.inc"
{
    unsigned int day = 0;   // -0x14 DATE_FORMAT 列
    unsigned int reg = 0;   // -0x18 reg_date 列
    SIG_GET_USER_REGDATE* sig =
        stream->GetOutBuffer<SIG_GET_USER_REGDATE>();  // -0x10
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect DATE_FORMAT(from_unixtime(reg_date),'%%Y%%m%%d%%H') ,reg_date from member_info where m_id=%d",
        sig->m_id);
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() != 0)
    {
        if (!db->fetch())
            return 0;
        if (!db->get_uint(0, day))
            return 0;
        if (!db->get_uint(1, reg))
            return 0;
    }
    // ORIG 0x8442f66-0x8442f77 `mov $0x51eb851f,%edx; mul; shr $0x5,%edx`
    // 商原地留 %edx ⇒ 字段与无符号商同号（int 字段恒产 mov %edx,%eax 中转，
    // 探针 /tmp/w8lt_mul3.o；unsigned 探针 /tmp/w8lt_mul2.o）。
    sig->m_regDay = day / 100;
    sig->m_regDate = reg;
    return SendResult(userIdx, slot, sig);
}

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

// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
#include "W8LT_DBTask/dispatch_114.inc"
{
    SIG_CHECK_CHARAC_NAME* sig = stream->GetOutBuffer<SIG_CHECK_CHARAC_NAME>();
    unsigned int mId = 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect m_id from charac_info where charac_name='%s' limit 1",
        reinterpret_cast<DB_CheckCharacNameView*>(sig)->m_charName);
    unsigned char flag = 0;
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() != 0)
    {
        if (!db->fetch())
            return 0;
        if (!db->get_int(0, mId))
            return 0;
        if (reinterpret_cast<DB_CheckCharacNameView*>(sig)->m_mId == mId)
            flag = 1;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa4ec), true);
    guard.operator*()->operator<<(0x2a3);
    guard.operator*()->operator<<(slot);
    // ORIG 0x84432e7-0x84432fd 尾两 store：`mov -0xc,%eax; movzbl -0xd,%edx;
    // mov %dl,(%eax)` / `mov -0x18,%eax; movzbl 0x22(%eax),%edx; mov -0xc,%eax;
    // mov %dl,0x1(%eax)`（RHS 先载）。4.4 中 LHS 表达式带 reinterpret_cast 恒
    // 翻转为 LHS 地址先载（探针 /tmp/w8lt_ord3.o::f5），LHS 用 view 类型直存
    // 则保 RHS 先（探针 /tmp/w8lt_ord4.o::f6）⇒ 单 view 指针直存。
    DB_ExceptionMidView* out =
        reinterpret_cast<DB_ExceptionMidView*>(
            guard.operator->()->GetInBuffer<SIG_GOLD_CONTROL_EXCEPTION_MID>());
    out->m_flag = flag;
    out->m_char =
        reinterpret_cast<DB_CheckCharacNameView*>(sig)->m_byte22;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

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
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084441e6
#include "W8LT_DBTask/dispatch_115.inc"
{
    SIG_CHECK_USER_SECU_REWARD* sig =
        stream->GetOutBuffer<SIG_CHECK_USER_SECU_REWARD>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    unsigned int lastCheck = 0;
    db->set_query(
        "seLect unix_timestamp(last_check_time) from member_security_grade where m_id=%s",
        NumberToString(*reinterpret_cast<unsigned int*>(sig), 0));
    // P168a 五步自检定论（goto 落点岛/正判双分支/短路合判三形实跑均劣化
    // ——首段 sete 岛+3 jmp；4.1 `xor$1;test` 物化段与尾 v 双常数岛为
    // 跨版本 bool 物化顶③b 族）⇒ 保留 && 直判原形，标 N。S 类
    // missing-calls:ptEv 已修（guard-> 形）。
    // [b28d-solo-W8LT_DBTask] ORIG 0x08444262-0x08444296：负极性短路物化
    // （`call exec; xor $1; test %al; jne FAIL` / `n_rows==0 je FAIL` /
    // `call fetch; xor $1; test; je PASS` + FAIL/PASS 双常数岛）⇒ 原码是
    // `if (!A || B==0 || !C) return 1;` 早返形（正极性 && 包裹反产无 xor 直判）。
    if (!db->exec(true) || db->get_n_rows() == 0 || !db->fetch())
        return 1;
    {
        db->get_uint(0, lastCheck);
        time_t nowT;
        time(&nowT);
        int mod = (int)nowT % 0x15180;
        nowT = nowT - mod - 0x2a30;
        if (mod > 0x1274f)
            nowT = nowT + 0x15180;
        if ((int)lastCheck < (int)nowT)
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xab25),
                true);
            guard.operator*()->operator<<(userIdx);
            guard.operator*()->operator<<(slot);
            // ORIG 0x8444397：`lea -0x28; call CStreamGuard::operator->
            // (ptEv); call GetInBuffer<SIG_SEND_USER_REWARD>` = `guard->`
            // 形（台账 S 类 missing-calls:ptEv 修复；`operator*()->` 形会
            // 丢 ptEv call）。
            // ORIG 0x84444xx store：`mov -0x18,%eax; mov 0x4(%eax),%edx;
            // mov -0xc,%eax; mov %edx,(%eax)` —— RHS 先载（sig+4 单级字段
            // 解引用）+ LHS reload；4.4 中 LHS 带 cast 翻为地址先载 + RHS
            // `add $4` 三级（探针 /tmp/w8lt_ord3.o::f5）⇒ LHS 取 view 型
            // 变量直存、RHS 取 view 字段。
            DB_SecuRewardOutView* out =
                reinterpret_cast<DB_SecuRewardOutView*>(
                    guard.operator->()->GetInBuffer<SIG_SEND_USER_REWARD>());
            out->m_reward =
                reinterpret_cast<DB_SecuRewardSigView*>(sig)->m_param4;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
    }
    return 1;
}

// ============================================================================
// DB_InsertPartyType::dispatch —— ORIG 0x0842c906（报告 dispatch.md 全文）。
// GetDBHandle(DB_HANDLE_4,0) → 局部 5×int 清零 → Stream >> 5 个 int
// （while i<=4）→ channelNo=G_CEnvironment()->get_channel_no() → set_query(
// "inSert into log_party_type(channel_no,occ_date,guild_count,member_count,
// helped_count,normal_count,member_helped_count)
// values(%d,now(),%d,%d,%d,%d,%d)", channelNo, v0..v4) →
// exec(true)==1 返 1，否则返 0。
// ============================================================================
// [b28d-4] DIFF：P183/P171：ORIG 多 {'push': 1, 'mov': 2, 'pop': 1}（漏具名局部/去缓存不足）；ORIG 0842c906
#include "W8LT_DBTask/dispatch_116.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int v[5] = { 0, 0, 0, 0, 0 };
    // [FL3W8LT 2026-09-10 P329/P0] 原具名 `channelNo` 局部已删：ORIG 0x0842c95b 段
    // 在 `call G_CEnvironment` **之前**就把 v[0..4] 全取出（v[0]→%ebx / v[1]→%esi /
    // v[2]→%edi / v[3]→-0x3c / v[4]→-0x40，右→左 varargs 保活），说明 channel_no 是
    // **实参位内联表达式**（最后一个求值）而非具名局部；具名使 4.4 先算 channelNo
    // 再取 v，并多 push %edi/帧 +0x1c。
    int i = 0;
    while (i <= 4)
    {
        *stream >> v[i];
        ++i;
    }
    db->set_query(
        "inSert into log_party_type(channel_no,occ_date,guild_count,member_count,helped_count,normal_count,member_helped_count) values(%d,now(),%d,%d,%d,%d,%d)",
        G_CEnvironment()->get_channel_no(), v[0], v[1], v[2], v[3], v[4]);
    if (db->exec(true) != 1)
        return 0;
    return 1;
}

// ============================================================================
// DB_LoadPuUserCheck::dispatch —— ORIG 0x0844bd5c（报告 dispatch.md 全文）。
// GetDBHandle(DB_HANDLE_1,0)；db==0 返 0。mId=0、flag=0 → Stream >> mId(uint)
// → set_query("seLect m_id from pu_user_list where m_id = %u", mId) →
// exec 失败返 0；get_n_rows()!=0 置 flag=1 → Acquire("DBThread.cpp",0xbcad)
// → guard << 0x2e1 << slot → GetInBuffer<SIG_LOAD_PU_USER_INFO> 写 flag →
// put(DISPATCH_Q) → 返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_117.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    unsigned int mId = 0;
    unsigned char flag = 0;
    *stream >> mId;
    db->set_query("seLect m_id from pu_user_list where m_id = %u", mId);
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() != 0)
        flag = 1;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbcad), true);
    guard.operator*()->operator<<(0x2e1);
    guard.operator*()->operator<<(slot);
    SIG_LOAD_PU_USER_INFO* out = guard->GetInBuffer<SIG_LOAD_PU_USER_INFO>();
    *reinterpret_cast<char*>(out) = static_cast<char>(flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_CrashDownReport::dispatch —— ORIG 0x0841abc6（报告 dispatch.md 全文）。
// s_systemTime_.getCurSec → localtime_r → tm_sec=0 → mktime（今晨零点，
// 全函数共享）→ GetDBHandle(DB_HANDLE_4,0) →
// getStatisticsToObserveCrash()->getAbnormalDownUserCount() → upDate
// client_down set occ_count=occ_count+%d where occ_date=cast(now() as date)
// （count）；exec!=1 → cMyTrace(0x4d5c,5) "upDate client_down failed"；
// 随后 cMyTrace(0x4d5f,0) "abnormal down user : %d"（无条件）。
// lock → 遍历 m_userLastCrashDungeonId（键=地牢,值=次数）：
//   inSert crash_down_by_dungeon(occ_time,dungeon_index,down_count)
//   values(from_unixtime(%d),%d,%d)（dayStart,dungeon,down）；exec==1 或
//   upDate 成功（实参序照 ORIG 汇编：(dayStart,down,dungeon)）→
//   cMyTrace(0x4d7d,0) "abnormal down user by dungeon : %d DUNGEON %d"
//   （dungeon,down）。
// 再遍历 m_userLastCrashPacketId：inSert crash_down_by_packet
//   (occ_date,packet_index,down_count) values(cast(now() as date),%d,%d)
//   （packet,down）；失败转 upDate（packet,down）；成功 →
//   cMyTrace(0x4d98,0) "abnormal down user by packet : %d PACKET %d"。
// 再遍历 m_userLastCrashPlayStatus（do-while 形态）：inSert
//   crash_down_by_playstatus(occ_date,play_status,down_count)
//   values(cast(now() as date),%d,%d)（play,down）；失败转 upDate
//   （play,down）；成功 → cMyTrace(0x4db2,0)
//   "abnormal down user by playerstatus : %d PLAYERSTATUS %d"。
// 三循环结束 unlock → reset → 返 1。
// ============================================================================
// [b28d-4] DIFF：P222：块序/极性（ORIG je 落点 vs 4.4 jne）；ORIG 0841abc6
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
    // （clean 401 vs orig 371 ⇒ DIFF）。改「各循环前原位初始化」后 371=371、
    // 仅余槽号差（4.4 复用 -0x70/-0x74 一对，ORIG 六槽独立 ⇒ N82），但本 TU
    // 因此不再发射 W 符号 `_ZNSt17_Rb_tree_iteratorISt4pairIKiiEEC1Ev`/`C2Ev`
    // （ORIG 0x080fa898 W 0xe 有）⇒ TU 账 identical −1 / near +1 /
    // missing(our-only) −1，触「identical 不得下降」闸门 ⇒ 回滚留 N。
    std::map<int, int>::iterator it;
    // [J4S1 P212/P309 变体B] 仅 `it` 保留顶部默认构造（保住 ORIG 实有的 W 符号
    // _ZNSt17_Rb_tree_iteratorISt4pairIKiiEEC1Ev @0x080fa898）；其余 5 个改原位
    // 初始化，消 5×5=25 条 4.4 空 ctor call 序列（ORIG 4.1 已内联为 0 条）。
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
    it = G_GameWorld()->getStatisticsToObserveCrash()
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

// ============================================================================
// DB_TerminateServer::dispatch —— ORIG 0x0841a2ee（报告 dispatch.md）。
// push/mov esp/mov $0x1,%eax/pop/ret —— 恒返 1（参数未使用）。
// ============================================================================
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

// ============================================================================
// DB_RemoveEventItem::dispatch —— ORIG 0x0841ab30（报告 dispatch.md）。
// GetDBHandle(DB_HANDLE_3,0) → GetOutBuffer<SIG_REMOVE_EVENTITEM> →
// 循环每条 upDate charac_event_items set delete_flag=1,delete_time=now()
// where id=%u → exec(true)（结果不检查）；恒返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_120.inc"
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    // view 类型局部（ORIG 循环体「先载下标再载基址」形态；证据 dispatch.md
    // 0x841ab6d：mov -0xc,%edx / mov -0x10,%eax / mov 0x4(%eax,%edx,4),%eax）。
    DB_RemoveEventItemView* raw =
        (DB_RemoveEventItemView*)stream->GetOutBuffer<SIG_REMOVE_EVENTITEM>();
    int i;
    for (i = 0; i < raw->m_count; ++i)
    {
        db->set_query(
            "upDate charac_event_items set delete_flag=1,delete_time=now() where id=%u",
            raw->m_ids[i]);
        db->exec(true);
    }
    return 1;
}

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

// ============================================================================
// DB_SavePremiumInfo::dispatch —— ORIG 0x0841b4be（报告 dispatch.md）。
// GetOutBuffer<SIG_SAVE_PREMIUM_INFO> → GetDBHandle(DB_HANDLE_6,0) →
// upDate member_premium set service_end=from_unixtime(%d) where event_id=%d
// and pre_type=%d and m_id=%s and service_end>from_unixtime(%d) and
// server_id=%d（service_end,event_id,pre_type,NumberToString(m_id),
// service_start,server_id）；exec!=1 或 getAffectedRowCount()!=0 时 ok=0
// （P84 短路 bool 局部），ok=1（需补插）时 inSert member_premium
// (pre_type,m_id,service_start,service_end,event_id,server_id)
// values(%d,%s,from_unixtime(%d),from_unixtime(%d),%d,%d)；
// inSert exec 失败返 0；全绿返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_121.inc"
{
    SIG_SAVE_PREMIUM_INFO* raw =
        stream->GetOutBuffer<SIG_SAVE_PREMIUM_INFO>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "upDate member_premium set service_end=from_unixtime(%d) where event_id=%d and pre_type=%d and m_id=%s and service_end>from_unixtime(%d) and server_id=%d",
        ((DB_SavePremiumInfoView*)raw)->m_serviceEnd,
        ((DB_SavePremiumInfoView*)raw)->m_eventId,
        ((DB_SavePremiumInfoView*)raw)->m_preType,
        NumberToString(((DB_SavePremiumInfoView*)raw)->m_mId, 0),
        ((DB_SavePremiumInfoView*)raw)->m_serviceStart,
        ((DB_SavePremiumInfoView*)raw)->m_serverId);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into member_premium(pre_type,m_id,service_start,service_end,event_id,server_id) values(%d,%s,from_unixtime(%d),from_unixtime(%d),%d,%d)",
            ((DB_SavePremiumInfoView*)raw)->m_preType,
            NumberToString(((DB_SavePremiumInfoView*)raw)->m_mId, 0),
            ((DB_SavePremiumInfoView*)raw)->m_serviceStart,
            ((DB_SavePremiumInfoView*)raw)->m_serviceEnd,
            ((DB_SavePremiumInfoView*)raw)->m_eventId,
            ((DB_SavePremiumInfoView*)raw)->m_serverId);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_SavePuUserCheck::dispatch —— ORIG 0x0844bfb8（报告 dispatch.md）。
// GetDBHandle(DB_HANDLE_1,0) 空返 0；Stream>>m_id(u32) → seLect m_id from
// pu_user_list where m_id=%u → exec 失败返 0；get_n_rows()==0 时 inSert
// into pu_user_list(m_id) values(%u) → inSert exec 失败返 0；全绿返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_122.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    if (db == 0)
        return 0;
    unsigned int mId = 0;
    *stream >> mId;
    db->set_query("seLect m_id from pu_user_list where m_id = %u", mId);
    if (!db->exec(true))
        return 0;
    if (db->get_n_rows() == 0)
    {
        db->set_query("inSert into pu_user_list(m_id) values(%u)", mId);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

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

// ============================================================================
// DB_SelectCharacMid::dispatch —— ORIG 0x08446516（报告 dispatch.md）。
// GetOutBuffer<SIG_CHECK_CHAR_MID> → GetDBHandle(DB_HANDLE_2,0) →
// escape_string(buf, sig->m_name)（buf[0x3b] 清零，P4）→ seLect m_id from
// charac_info where charac_name='%s' limit 1 → exec 失败返 0；
// get_n_rows()!=0 时 fetch / get_int(0,mId) 任一失败返 0 → Acquire(
// "DBThread.cpp",0xae30) → guard << userIdx << slot → GetInBuffer<
// SIG_CHECK_CHARAC_MID_RESULT> 写 +0x10=mId、+0/+4/+8/+0xc 复制输入 →
// MsgQueueMgr::put(DISPATCH_Q,guard)；恒返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_123.inc"
{
    SIG_CHECK_CHAR_MID* sig = stream->GetOutBuffer<SIG_CHECK_CHAR_MID>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    unsigned int mId = 0;
    int nRows;
    char buf[0x3b] = { '\0' };
    db->escape_string(buf, sig->m_name);
    db->set_query("seLect m_id from charac_info where charac_name='%s' limit 1",
                  buf);
    if (!db->exec(true))
        return 0;
    nRows = db->get_n_rows();
    if (nRows != 0)
    {
        if (!db->fetch())
            return 0;
        if (!db->get_int(0, mId))
            return 0;
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xae30), true);
        guard.operator*()->operator<<(userIdx);
        guard.operator*()->operator<<(slot);
        DB_SelectCharacMidResultView* out =
            (DB_SelectCharacMidResultView*)guard->GetInBuffer<SIG_CHECK_CHAR_MID_RESULT>();
        out->m_mId = mId;
        out->m_field04 = sig->m_field04;
        out->m_field08 = sig->m_field08;
        out->m_field0c = sig->m_field0c;
        out->m_field00 = sig->m_field00;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return 1;
}

// ============================================================================
// DB_UpdateDimension::dispatch —— ORIG 0x0843230a（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<SIG_UPDATE_DIMENSION>（视图 DB_UpdateDimensionView，
// 写侧 CUserOnlyOrig.cpp UpdateDimensionInout）。+0x0d（dimension 计数有效）时
// upDate charac_dimension_inout（6 dungeon 逐字节 movsbl 符号扩展为 int，
// charac_no=%u）；exec 失败或无影响行补 inSert；inSert exec 失败
// logFormat(0x7972) 记 DIMENSION_LOG。+0x0e（blood 计数有效）时 upDate
// charac_blood_inout（dungeon1=%d）；exec 失败或无影响行补 inSert；
// inSert exec 失败 logFormat(0x797e) 记 BLOOD_LOG。恒返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_124.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    DB_UpdateDimensionView* sig = reinterpret_cast<DB_UpdateDimensionView*>(
        stream->GetOutBuffer<SIG_UPDATE_DIMENSION>());
    if (sig->m_dimensionInoutSave != 0)
    {
        db->set_query(
            "upDate charac_dimension_inout set dungeon1=%d, dungeon2=%d, dungeon3=%d,  dungeon4=%d, dungeon5=%d, dungeon6=%d where charac_no=%u",
            (int)sig->m_dungeon[0], (int)sig->m_dungeon[1], (int)sig->m_dungeon[2],
            (int)sig->m_dungeon[3], (int)sig->m_dungeon[4], (int)sig->m_dungeon[5],
            sig->m_characNo);
        if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into charac_dimension_inout(charac_no,dungeon1,dungeon2,dungeon3,dungeon4,dungeon5,dungeon6) values(%u,%d,%d,%d,%d,%d,%d)",
                sig->m_characNo,
                (int)sig->m_dungeon[0], (int)sig->m_dungeon[1],
                (int)sig->m_dungeon[2], (int)sig->m_dungeon[3],
                (int)sig->m_dungeon[4], (int)sig->m_dungeon[5]);
            if (db->exec(true) != 1)
            {
                LogManager::logFormat(
                    1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x7972,
                    "DIMENSION_LOG : INOUT CNT INSERT DB ERROR!! values(characno:%u,dungeon1:%d,2:%d,3:%d,4:%d,5:%d,6:%d)",
                    sig->m_characNo,
                    (int)sig->m_dungeon[0], (int)sig->m_dungeon[1],
                    (int)sig->m_dungeon[2], (int)sig->m_dungeon[3],
                    (int)sig->m_dungeon[4], (int)sig->m_dungeon[5]);
            }
        }
    }
    if (sig->m_bloodInoutSave != 0)
    {
        db->set_query(
            "upDate charac_blood_inout set dungeon1=%d where charac_no=%u",
            (int)sig->m_bloodDungeon1, sig->m_characNo);
        if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into charac_blood_inout(charac_no,dungeon1) values(%u,%d)",
                sig->m_characNo, (int)sig->m_bloodDungeon1);
            if (db->exec(true) != 1)
            {
                LogManager::logFormat(
                    1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x797e,
                    "BLOOD_LOG : INOUT CNT INSERT DB ERROR!! values(charac_no:%u,dungeon1:%d)",
                    sig->m_characNo, (int)sig->m_bloodDungeon1);
            }
        }
    }
    return 1;
}

// ============================================================================
// DB_AvatarChangeStat::dispatch —— ORIG 0x0840065e（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_3。GetOutBuffer<SIG_AVATAR_CHANGE_STAT>（视图
// DB_AvatarChangeStatView）：upDate user_items set stat=%d,slot=%d where
// ui_id=%d and charac_no=%d（stat=+0x04、slot=+0x08 byte 零扩、ui_id=+0x00、
// charac_no=+0x09 起 4 字节）；exec 失败返 0。it_id(+0x04)==0 时：
// seLect 10 列（slot/ui_id/it_id/reg_date/expire_date/ability_no/
// ipg_agency_no/jewel_socket/hidden_option/emblem_endurance）→ exec/fetch
// 失败返 0 → Acquire("DBThread.cpp",0xbe2) → guard << 0x36 << slot →
// GetInBuffer<SIG_AVATAR_CHANGE_STAT> → 逐列读（col0 slot / 1 ui_id /
// 2 it_id / 3 reg_date uint / 4 expire_date uint / 5 ability_no /
// 6 ipg_agency_no str 0x18 / 7 jewel_socket binary 0x1e / 8 hidden_option /
// 9 emblem_endurance）；jewel_socket 读失败静默走 else（析构后返 0），
// 其余失败 throw int 0xbf2..0xc08，catch(int) 后 StreamPool::Free(guard)
// 返 0。全成回填回包（+0x4d/+0x4f 两 short、+0x08 slot、+0x00 ui_id、
// +0x04 it_id、+0x0a reg、+0x0e expire、+0x09 ability、+0x2f emblem 0x1e、
// +0x2b hidden、+0x2d 恒 0）→ MsgQueueMgr::put(DISPATCH_Q) → 返 1。
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0840065e
// [r3] jewel_socket 判定改极性早返 `if (!db->get_binary(7,&emblem,0x1e))
//   return 0;`（去掉 else 块）⇒ ndiff 40→30；残余 xor:1 在 218 行处（另一
//   失败出口的 4.1 归一化）+ `mov %eax,%edx` 中转（N18a/N82）⇒ 封顶。
#include "W8LT_DBTask/dispatch_125.inc"
{
    static const int handleIdx = 3;
    DB_AvatarChangeStatIn* sig = reinterpret_cast<DB_AvatarChangeStatIn*>(
        stream->GetOutBuffer<SIG_AVATAR_CHANGE_STAT>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set stat=%d,slot=%d where ui_id=%d and charac_no=%d",
        sig->m_itId, (unsigned int)sig->m_slot, sig->m_uiId, sig->m_characNo);
    if (db->exec(true) != 1)
        return 0;
    if (sig->m_itId == 0)
    {
        db->set_query(
            "seLect slot,ui_id,it_id,unix_timestamp(reg_date),unix_timestamp(expire_date),ability_no,ipg_agency_no,jewel_socket,hidden_option,emblem_endurance from user_items where charac_no=%u and ui_id=%d",
            sig->m_characNo, sig->m_uiId);
        if (db->exec(true) != 1)
            return 0;
        if (db->fetch() != 1)
            return 0;
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbe2), true);
        try
        {
            guard.operator*()->operator<<((int)0x36);
            guard.operator*()->operator<<((int)slot);
            // [b28d-solo] P103/P167：ORIG 调 operator->（ptEv），非 operator*。
            DB_AvatarChangeStatView* out =
                reinterpret_cast<DB_AvatarChangeStatView*>(
                    guard.operator->()->GetInBuffer<SIG_AVATAR_CHANGE_STAT>());
            int slotV = 0;            // col0 slot（回包 +0x08 byte）
            int uiIdV = 0;            // col1 ui_id（回包 +0x00）
            int itIdV = 0;            // col2 it_id（回包 +0x04）
            int abilityV = 0;         // col5 ability_no（回包 +0x09 byte）
            unsigned int regV = 0;    // col3 reg_date uint（回包 +0x0a）
            unsigned int expV = 0;    // col4 expire_date uint（回包 +0x0e）
            // ORIG 槽序：expV(-0x3c)/emblem(-0x5a..-0x3d)/hiddenV(-0x60)/
            // hiddenS(-0x62)/emblemEnd(-0x64)/zero(-0x10)；emblem 声明在
            // hiddenV 前（init() 调用在 get_str 后）。
            stAvatarEmblemInfo_t emblem;   // ORIG 槽 -0x5a
            int hiddenV = 0;          // col8 hidden_option int（回包 +0x2b short）
            int zero = 0;             // 回包 +0x2d 恒 0（ORIG movl 清零）
            if (!db->get_int(0, slotV))
                throw 0xbf2;
            if (!db->get_int(1, uiIdV))
                throw 0xbf3;
            if (!db->get_int(2, itIdV))
                throw 0xbf4;
            if (!db->get_uint(3, regV))
                throw 0xbf5;
            if (!db->get_uint(4, expV))
                throw 0xbf6;
            if (!db->get_int(5, abilityV))
                throw 0xbf7;
            if (db->get_str(6, out->m_ipgAgencyNo, 0x18))
                throw 0xbf9;
            emblem.init();
            if (!db->get_binary(7, &emblem, 0x1e))
                return 0;
            {
                if (!db->get_int(8, hiddenV))
                    throw 0xc00;
                short hiddenS = 0;    // col8 hidden_option short（回包 +0x4d）
                short emblemEnd = 0;  // col9 emblem_endurance（回包 +0x4f）
                if (!db->get_short(8, hiddenS))
                    throw 0xc07;
                if (!db->get_short(9, emblemEnd))
                    throw 0xc08;
                out->m_field4d = hiddenS;
                out->m_emblemEndurance = emblemEnd;
                out->m_slot = (unsigned char)slotV;
                out->m_uiId = uiIdV;
                out->m_itId = itIdV;
                out->m_regDate = (int)regV;
                out->m_expireDate = (int)expV;
                out->m_abilityNo = (unsigned char)abilityV;
                // ORIG 0x08400bd6(+0x578)（report dispatch.md）：emblem(stAvatarEmblemInfo_t
                // 0x1e) → m_jewelSocket(+0x2f) 为 8 条直拷（7 dword + 1 word），
                // 非 memcpy（同 SIG_AVATAR_INSERT::setJewelSocket 镜像法）。
                // [b28d-lane] 逐字面数组写形实测反劣（4.4 每 store 重算 base：
                // ndiff 40→92，多 15 add+15 lea）⇒ 维持结构赋值形，余 1 条
                // `add $0x2f` 基址推进为 N4/N18a 封顶。
                *reinterpret_cast<stAvatarEmblemInfo_t*>(out->m_jewelSocket) =
                    emblem;
                out->m_hiddenOption = (short)hiddenV;
                out->m_field2d = zero;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            }
        }
        catch (int)
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
    }
    return 1;
}

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
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    DB_LetterStatView* sig = reinterpret_cast<DB_LetterStatView*>(
        stream->GetOutBuffer<SIG_LETTER_STAT>());
    // ORIG 0x084260xx switch 下探链形：`movzwl; cwtl; test; je DEL;
    // test; js DEF; sub $2; cmp $1; ja DEF; [fall UPD]` —— 4.1 switch
    // {0,2,3}+default 专属；4.4 的 if/else-if 链反产三次重装载。
    switch ((int)sig->m_state)
    {
    case 0:
        db->set_query("deLete from letter where letter_id = %d",
                      sig->m_mailIdx);
        if (db->exec(true) != 1)
        {
            makeRequest(userIdx, slot, sig->m_characNo, 0, 0);
            return 0;
        }
        break;
    case 2:
    case 3:
        db->set_query("upDate letter set stat=%d where letter_id=%d",
                      (int)sig->m_state, sig->m_mailIdx);   // varargs 无 cast ⇒ 单跳
        if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        {
            makeRequest(userIdx, slot, sig->m_characNo, 0, 0);
            return 0;
        }
        break;
    default:
        makeRequest(userIdx, slot, sig->m_characNo, 0, 0);
        return 0;
    }
    makeRequest(userIdx, slot, sig->m_characNo,
                (int)sig->m_mailIdx,   // (int)unsigned cast ⇒ 双跳
                (int)sig->m_state);
    return 1;
}

// ============================================================================
// DB_ChangeLetterStat::makeRequest —— ORIG 0x084261fc（报告 makeRequest.md）
// Acquire("DBThread.cpp",0x6454) → CStreamGuard(stream,true) → <<userIdx →
// <<slot → GetInBuffer<SIG_LETTER_STAT>：m_characNo=characNo、
// m_mailIdx=mailIdx、m_state=(short)state → MsgQueueMgr::put(DISPATCH_Q)。
// ============================================================================
// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
#include "W8LT_DBTask/makeRequest_34.inc"
                                      int mailIdx, int state)
{
    // [b26L17 翻案 N76→identical] m_mailIdx int→unsigned int（SigTypes_B.h，
    //   字段↔形参异型化 → 4.4 值先载=ORIG 0x14(%ebp) 形）；m_characNo 保持
    //   int（该行 ORIG 地址先，实测异型化反翻）。69/69 逐条相等。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6454), true);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((int)slot);
    SIG_LETTER_STAT* sig = guard.operator->()->GetInBuffer<SIG_LETTER_STAT>();
    sig->m_characNo = characNo;
    sig->m_mailIdx = mailIdx;
    sig->m_state = (short)state;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ============================================================================
// DB_ForbidUserToPlay::dispatch —— ORIG 0x08421af4（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<SIG_FORBID_USER_TO_PLAY>（视图
// DB_ForbidUserToPlayView：+0x00 characNo、+0x04 flag byte、+0x05 days
// int）。flag==1 且 days!=0 → upDate charac_stat set forbidden_to_play=1,
// forbidden_due_to=date_add(now(), interval %d day) where charac_no=%d
// （days, characNo）；否则 → forbidden_to_play=0, forbidden_due_to=0
// （characNo）。返回 exec(true)==1。
// ============================================================================
// ORIG 帧：sig(-0x1c)/db(-0x18)/characNo(-0x14)/flag(-0xd)/days(-0xc)，
// 三局部先清零再赋值（gcc4.1 形态）；判定 flag==0 → else（清零 SQL）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08421af4
// [b28d-solo-4] §6.2 三件套：p167 CLEAR（ORIG 5 = OUR 5，0/0）+ gap −1（58/59）
//   ⇒ 语义完成；余 N-JMP label-stub fall-through jmp 封顶。
#include "W8LT_DBTask/dispatch_127.inc"
{
    DB_ForbidUserToPlayView* sig = reinterpret_cast<DB_ForbidUserToPlayView*>(
        stream->GetOutBuffer<SIG_FORBID_USER_TO_PLAY>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    int characNo = 0;
    bool flag = false;
    int days = 0;
    characNo = sig->m_characNo;
    flag = sig->m_flag;
    days = sig->m_days;
    // ORIG 0x8421b55-0x8421b64 双 goto 钉块序（技巧23+24）：
    // `movzbl -0xd,%eax; xor $0x1,%eax; test %al,%al; jne b66(1-arg块)` +
    // `cmpl $0,-0xc; jne b82(2-arg块)`；4.4 对 `if(a&&b){}else{}` 直判
    // cmpb$1 融合且块序反（2-arg fall）。fall-through 块 = 1-arg SQL。
    // bool 槽 + `!flag`（f1 探针形）⇒ `movzbl slot,%eax; xor $1; test
    // %al; jne` 物化；uchar 槽一切写法折 cmpb/test。
    // V2（FULL 内联 if + 单 stub）实测劣化：消掉 1 行 stub jmp 但反出
    // cond2 方向（je↔jne）+ 双 body 填参各 +2 行装载序翻转 ⇒ 净差 4 行
    // >1。维持 V1（双 goto 钉块序，残留 1 行 = 4.4 label-stub bb 的
    // fall-through jmp，N-JMP 族）。
    // [b28d-solo-W8LT_DBTask] 去 else 花括号（裸 if 链）：4.4 不再为 else 块出口
    // 发 fall-through jmp，SIMPLE_SQL 直接跟随条件跳转（ORIG 形）。
    // [J4-r1] V3 直写 `if(!flag || days==0) A else B`（P214①/P258 德摩根族）：
    // 目标是复现 ORIG 0x8421b5e `jne A` + 0x8421b64 `jne B` 后**直接落 A**
    // （无 label-stub jmp/nop）。
    if (!flag || days == 0)
    {
        db->set_query(
            "upDate charac_stat set forbidden_to_play=0, forbidden_due_to= 0 where charac_no = %d",
            characNo);
    }
    else
    {
        db->set_query(
            "upDate charac_stat set forbidden_to_play=1, forbidden_due_to = date_add(now(), interval %d day) where charac_no = %d",
            days, characNo);
    }
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_InsertDungeonExp::dispatch —— ORIG 0x0842e944（报告 dispatch.md 全文）
// ============================================================================
// DB_HANDLE_4。curSec=GlobalData::s_systemTime_.getCurSec() → *stream >>
// count → 循环 count 次：*stream >> lev >> exp >> fatigue >> user_count
// （4×int）→ sprintf(buf,"(from_unixtime(%d),%d,%d,%d,%d,%d)", curSec,
// G_CEnvironment()->get_channel_no(), lev, exp, fatigue, user_count) →
// std::string += buf；非末项 += ","。循环后 set_query("inSert into
// log_dungeon_exp(occ_time,channel_no,lev,exp,fatigue,user_count) values%s",
// sql.c_str()) → 返回 exec(true)==1。
// ============================================================================
// ORIG 帧（gcc4.1）：db(-0x24)/curSec(-0x20)/v1..v4(-0x28..-0x34)/
// sql(-0x38)/count(-0x3c)/i(-0x1c)/buf(-0x448)。声明序照 ORIG 槽序。
#include "W8LT_DBTask/dispatch_128.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    int lev = 0;
    int exp = 0;
    int fatigue = 0;
    int userCount = 0;
    std::string sql;
    int count = 0;
    char buf[1024];
    *stream >> count;
    for (int i = 0; i < count; ++i)
    {
        *stream >> lev;
        *stream >> exp;
        *stream >> fatigue;
        *stream >> userCount;
        sprintf(buf, "(from_unixtime(%d),%d,%d,%d,%d,%d)", curSec,
                G_CEnvironment()->get_channel_no(), lev, exp, fatigue,
                userCount);
        sql += buf;
        if (i < count - 1)
            sql += ",";
    }
    db->set_query(
        "inSert into log_dungeon_exp(occ_time,channel_no,lev,exp,fatigue,user_count) values%s",
        sql.c_str());
    // ORIG 0x0842e944：同 DB_InsertPVPMap 尾——bool 物化双常数岛
    // （`xor$1;test;je` + mov$0/mov$1 入 %ebx）后走 string dtor；
    // 4.4 直判返形复现（先例 DB_InsertPVPMap::dispatch 闭合）。
    if (!db->exec(true))
        return 0;
    return 1;
}
// ============================================================================
// DB_FatigueResetTimer::dispatch —— ORIG 0x0841abbc（报告 dispatch.md）
// ============================================================================
// ORIG 恒返 1（mov $0x1,%eax; ret，无 DB 访问）。
#include "W8LT_DBTask/dispatch_129.inc"
{
    return 1;
}

// ============================================================================
// DB_SuicideWhenQueueIsEmpty::dispatch —— ORIG 0x0841b206（报告 dispatch.md）
// ============================================================================
// ORIG 恒返 1（mov $0x1,%eax; ret，无 DB 访问）。
#include "W8LT_DBTask/dispatch_130.inc"
{
    return 1;
}

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
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    DB_CirculationStatisticView* sig =
        reinterpret_cast<DB_CirculationStatisticView*>(
            stream->GetOutBuffer<SIG_CIRCULATION_STATISTIC>());
    db->set_query(
        "inSert into log_gold_stat(channel_no,occ_time ,level,dungeon_drop,result_card,sell_store,quest_reward,death_tower_reward,illusion_tower_reward,war_area_drop,member_tax,blood_dungeon_reward,blood_dungeon_lotto,power_dungeon_drop,power_dungeon_result_card,buy_store,stamina_recovery,repair_item,private_store_commission,gold_card,gold_drop,upgrade,quest_use,mail_commission,punish_user,restrict_trade,guild_level_up,guild_skill,guild_mail,item_compound,blood_dungeon_enter,buy_cerashop,war_area_enter,assault_gold,upgrade_guild_agit,upgrade_guild_cargo) values(%d,from_unixtime(%d),%d,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)",
        G_CEnvironment()->get_channel_no(), sig->m_occTime, sig->m_level,
        sig->m_dungeonDrop, sig->m_resultCard, sig->m_sellStore,
        sig->m_questReward, sig->m_deathTowerReward,
        sig->m_illusionTowerReward, sig->m_warAreaDrop, sig->m_memberTax,
        sig->m_bloodDungeonReward, sig->m_bloodDungeonLotto,
        sig->m_powerDungeonDrop, sig->m_powerDungeonResultCard,
        sig->m_buyStore, sig->m_staminaRecovery, sig->m_repairItem,
        sig->m_privateStoreCommission, sig->m_goldCard, sig->m_goldDrop,
        sig->m_upgrade, sig->m_questUse, sig->m_mailCommission,
        sig->m_punishUser, sig->m_restrictTrade, sig->m_guildLevelUp,
        sig->m_guildSkill, sig->m_guildMail, sig->m_itemCompound,
        sig->m_bloodDungeonEnter, sig->m_buyCerashop, sig->m_warAreaEnter,
        sig->m_assaultGold, sig->m_upgradeGuildAgit,
        sig->m_upgradeGuildCargo);
    if (!db->exec(true))
        return 0;
    return 1;
}

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

// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084335dc
#include "W8LT_DBTask/dispatch_132.inc"
                                                  Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    bool flag = false;
    *stream >> flag;
    // [H4 2026-09-11 B 组取证] ORIG 0x0843361d `movl $0x0,-0x14(%ebp)`（nRows=0）
    // 位于 0x08433624 `memset(&sig,0,0x6c)` **之前** ⇒ nRows 声明须前置到 memset 前。
    int nRows = 0;
    DB_AutoMarketConditionView sig;
    memset(&sig, 0, 0x6c);
    db->set_query(
        "seLect optimum_gold_supply, over_gold from auto_market_condition_ctrl limit 1");
    if (db->exec(true) != 1)
        return 0;
    nRows = db->get_n_rows();
    if (nRows != 0)
    {
        if (db->fetch() != 1)
            return 0;
        if (db->get_ulonglong(0, sig.m_optimumGold) != 1)
            return 0;
        if (db->get_ulonglong(1, sig.m_overGold) != 1)
            return 0;
    }
    db->set_query(
        "seLect total_gold, auction_gold, optimum_gold_supply, over_gold, gold_phase, item_phase, durability_phase from auto_market_condition_ctrl_daily where occ_time >= DATE_SUB(CURDATE(), INTERVAL 1 DAY) order by occ_time limit 2");
    if (db->exec(true) == 1)
    {
        nRows = db->get_n_rows();
        if (nRows == 2 || nRows == 1)
        {
            for (int i = 0; i < nRows && i < 2; ++i)
            {
                if (db->fetch() != 1)
                    return 0;
                if (db->get_ulonglong(0, sig.m_rows[i].m_totalGold) != 1)
                    return 0;
                if (db->get_ulonglong(1, sig.m_rows[i].m_auctionGold) != 1)
                    return 0;
                if (db->get_ulonglong(2, sig.m_rows[i].m_optimumGold) != 1)
                    return 0;
                if (db->get_ulonglong(3, sig.m_rows[i].m_overGold) != 1)
                    return 0;
                if (db->get_int(4, sig.m_rows[i].m_goldPhase) != 1)
                    return 0;
                if (db->get_int(5, sig.m_rows[i].m_itemPhase) != 1)
                    return 0;
                if (db->get_int(6, sig.m_rows[i].m_durabilityPhase) != 1)
                    return 0;
            }
            if (nRows == 1)
                sig.m_rows[1] = sig.m_rows[0];
            Stream* out = GlobalData::s_stream_pool->Acquire("DBThread.cpp",
                                                             0x7c30);
            CStreamGuard guard(out, true);
            guard.operator*()->operator<<((int)0x15a);
            guard.operator*()->operator<<((int)-1);
            SIG_AUTO_MARKET_CONDITIONS_CONTROL* outSig =
                guard.operator->()->GetInBuffer<SIG_AUTO_MARKET_CONDITIONS_CONTROL>();
            memcpy(outSig, &sig, 0x6c);
            // [Lane31W8LTDBTaskView] 权威头具名视角直写（去 cast；ORIG 写 +0x68）。
            // [H4 2026-09-11 B 组取证] ORIG 0x08433a7a `movzbl -0x29(%ebp),%edx;
            // mov -0x10(%ebp),%eax; mov %dl,0x68(%eax)` ⇒ flag **直存**（无三元归一）。
            outSig->m_view.m_flag = flag;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        else
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x7c0c, 0);
            trace("DB_LoadAutoMarketConditionsControl error %d", nRows);
        }
    }
    return 0;
}

// ============================================================================
// DB_UpdateAutoMarketConditionsControl::dispatch —— ORIG 0x08433bb8（报告全文）
// ============================================================================
// DB_HANDLE_2。GetOutBuffer<SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL>
// （+0x00/+0x08/+0x10/+0x18=4×ull：total_gold/auction_gold/over_gold/
// optimum_gold_supply；+0x20/+0x24/+0x28=3×int phase）→ upDate daily 当日行
// （optimum=+0x18, over=+0x10, phase=+0x20/24/28）；exec 成功且
// getAffectedRowCount()==0 → inSert 当日行（total=+0x00, auction=+0x08,
// optimum=+0x18, over=+0x10, phase 同）；任一 exec 失败返 0。
// 无基类 dispatch 前置检查。
#include "W8LT_DBTask/dispatch_133.inc"
                                                    Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL* sig =
        stream->GetOutBuffer<SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL>();
    db->set_query(
        "upDate  auto_market_condition_ctrl_daily set optimum_gold_supply=%qd, over_gold=%qd, gold_phase=%d, item_phase=%d, durability_phase=%d where occ_time = CURDATE()",
        sig->m_values0[3], sig->m_values0[2], sig->m_values20[0],
        sig->m_values20[1], sig->m_values20[2]);
    // ORIG +0xbc `xor $1;test;je body; mov $0;jmp end` = P16/P59 early-return
    // `if (!exec) return 0;`（与 BoosterGage 同形）。
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into auto_market_condition_ctrl_daily(occ_time, total_gold, auction_gold, optimum_gold_supply, over_gold, gold_phase, item_phase, durability_phase) values(CURDATE(),%qd,%qd,%qd,%qd,%d,%d,%d)",
            sig->m_values0[0], sig->m_values0[1], sig->m_values0[3],
            sig->m_values0[2], sig->m_values20[0], sig->m_values20[1],
            sig->m_values20[2]);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_SwapCreatureItem::dispatch —— ORIG 0x0841fe0c（报告
// docs/class_func_reports/DB_SwapCreatureItem/dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_SWAP_CREATURE_ITEM> → GetDBHandle(DB_HANDLE_3, 无组)。
// m_empty1(+0x08)!=1 时：upDate creature_items set slot=%d where ui_id=%d
// （slot=m_slot2+0x10、ui_id=m_itemIdx1+0x00）；exec 失败返 0。
// m_empty2(+0x14)!=1 时：同 SQL（slot=m_slot1+0x04、ui_id=m_itemIdx2+0x0c）；
// exec 失败返 0。全过返 1。无回包。
#include "W8LT_DBTask/dispatch_134.inc"
{
    static const int handleIdx = 3;
    SIG_SWAP_CREATURE_ITEM* sig =
        stream->GetOutBuffer<SIG_SWAP_CREATURE_ITEM>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    if (sig->m_empty1 != 1)
    {
        db->set_query("upDate creature_items set slot=%d where ui_id=%d",
                      sig->m_slot2, sig->m_itemIdx1);
        if (db->exec(true) != 1)
            return 0;
    }
    if (sig->m_empty2 != 1)
    {
        db->set_query("upDate creature_items set slot=%d where ui_id=%d",
                      sig->m_slot1, sig->m_itemIdx2);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_AvatarChangeOwner::dispatch —— ORIG 0x084004bc（报告
// docs/class_func_reports/DB_AvatarChangeOwner/dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_AVATAR_CHANGE_OWNER> → GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, 无组)。
// upDate user_items set charac_no=%u,slot=%d where ui_id=%d && charac_no=%u
// （charac_no=m_newCharacNo+0x08、slot=m_slot+0x0c byte 零扩、
// ui_id=m_uiId+0x00、where charac_no=m_characNo+0x04）；exec 成功返 1。
#include "W8LT_DBTask/dispatch_135.inc"
{
    static const int handleIdx = 3;
    SIG_AVATAR_CHANGE_OWNER* sig =
        stream->GetOutBuffer<SIG_AVATAR_CHANGE_OWNER>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set charac_no=%u,slot=%d where ui_id=%d && charac_no=%u",
        sig->m_newCharacNo, (unsigned int)sig->m_slot, sig->m_uiId,
        sig->m_characNo);
    // ORIG +0x86-0x99：exec 后 xor $1/test/je 双返回（==1 恒等优化会直传 al，
    // 与 ORIG 指令形态不符，故用 != 1 分支）。
    if (db->exec(true) != 1)
        return 0;
    return 1;
}

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

// [b28d-4 P0 清绑] 原 `register char* mIdStr asm("ebx")` 已删（N96 全域禁绑定）；
// 具名槽化后 mIdStr 槽位 -0x10→-0x14、多 1 条 load（ORIG 0x0840888f 区 %ebx
// 免槽形）⇒ N96+N18a 封顶，语义等价。
#include "W8LT_DBTask/dispatch_136.inc"
{
    SIG_UPDATE_CHARAC_VIEW* sig =
        stream->GetOutBuffer<SIG_UPDATE_CHARAC_VIEW>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_2, (ENUM_SERVER_GROUP)sig->m_serverGroup);
    char buf[0x1905] = { 0 };
    unsigned int zipLen = 0x1905;
    // 技巧36：删 hv 具名视图指针（ORIG 每站点 `mov -0x10(%ebp),%eax` 参数
    // 结果槽直读+add disp，0x0840888f 区；hv 槽使整帧外推 8B）。
    if (!compress_zip(buf, zipLen,
                      reinterpret_cast<DB_CharacViewHeaderView*>(sig)->m_info,
                      0x14d0))
        return 0;
    // ORIG 0x08408…：`mov %eax,%ebx` mIdStr 直落 callee-saved %ebx（无槽）。
    // [b28d-4 P0 清绑] 原 `register char* mIdStr asm("ebx")` 已删（N96 全域禁绑定）。
    // [J4S1 P329/P227] 删具名 mIdStr（ORIG 无槽）⇒ 表达式内联回实参位，4.4 走
    // callee-saved %ebx 保活（同函数第二块同形已实证）。
    db->set_query("upDate charac_view set info='%s' where m_id=%s",
                  db->blob_to_str(0, buf, (int)zipLen),
                  NumberToString(
                      reinterpret_cast<DB_CharacViewHeaderView*>(sig)->m_mId, 0));
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into charac_view(m_id,info) values(%s,'%s')",
                      NumberToString(reinterpret_cast<DB_CharacViewHeaderView*>(sig)->m_mId, 0),
                      db->blob_to_str(0, buf, (int)zipLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateBingoEvent::dispatch —— ORIG 0x08448eb8（报告
// docs/class_func_reports/DB_UpdateBingoEvent/dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_9, 无组) → Stream >> uint(m_id) >> int(board) >>
// char(reward) → resetBase = GetCurrentResetBaseTime(getCurSec,
// *(int*)(G_CEnvironment()+0x37c))（同 DB_Login::GetBingoEvent 口径）。
// upDate event_1208_bingo set board=%d,reward=%d where m_id=%s and
// occ_date > from_unixtime(%u)；exec 失败返 0；getAffectedRowCount()==0 →
// inSert event_1208_bingo(m_id,occ_date,board,reward) values(%s,now(),%d,%d)
// （board/reward 重复使用，m_id=NumberToString(m_id,0)），exec 失败返 0。
// 全过返 1。
#include "W8LT_DBTask/dispatch_137.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    int board = 0;
    char reward = 0;
    *stream >> mId;
    *stream >> board;
    *stream >> reward;
    // ORIG +0x070-0x08e：G_CEnvironment()+0x37c 折叠单指令取数（P1），
    // 用具名成员 m_serverEnvir.m_fatigueTime（0x198+0x1e4 == 0x37c）。
    // ORIG +0x92 call 后 `mov %eax,-0xc(%ebp)` 全字单槽（unsigned 视图）；
    // +0xd2 `mov -0xc(%ebp),%ebx` 迟装载紧贴 5 参 spill（0x14(%esp)）⇒
    // 无 (unsigned int) 强转的具名 unsigned 槽直读。
    unsigned int resetBase = (unsigned int)GetCurrentResetBaseTime(
        (int)GlobalData::s_systemTime_.getCurSec(),
        G_CEnvironment()->m_serverEnvir.m_fatigueTime);
    db->set_query(
        "upDate event_1208_bingo set board = %d,reward=%d where m_id = %s and occ_date > from_unixtime(%u)",
        board, (int)reward, NumberToString(mId, 0), resetBase);
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_1208_bingo(m_id,occ_date,board,reward) values(%s,now(),%d,%d)",
            NumberToString(mId, 0), board, (int)reward);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateComboSkill::setComboSkill —— ORIG 0x0844aa4c（报告
// docs/class_func_reports/DB_UpdateComboSkill/setComboSkill.md 全文）
// ============================================================================
// db/data 任一空 → 0。kind 非 0/1 → cMyTrace(0xbb42) 返 0。循环 i=0..<count
// 且 i<6（comoboSkillData 0xe 字节/条）：逐 short 读（+0x00 combo_idx、
// +0x02..+0x0c value1..6）→ upDate %s set value1..6 where charac_no=%u and
// combo_idx=%d（表名 getComboSkillTableName(kind)）；exec 失败返 0；
// getAffectedRowCount()==0 → inSert %s(charac_no,combo_idx,value1..6)
// values(%u,%d,...)，exec 失败且 getDBError()!=0x426 → 返 0。全过返 1。
// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
#include "W8LT_DBTask/setComboSkill.inc"
                                        unsigned char count,
                                        comoboSkillData* data,
                                        ENUM_SKILL_TREE_KIND kind)
{
    if (db == 0 || data == 0)
        return 0;
    if (!(kind == 0 || kind == 1))
    {
        // ORIG 0x0844a8xx：`mov 0x1c(%ebp),%ebx` 前置缓存 kind，跨 ctor 后
        // `mov %ebx,0x8(%esp)`（P87；类型 int 与 ENUM 底层一致，单条直落）。
        register int k = kind;
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xbb42, 5);
        trace("DB_UpdateComboSkill::setComboSkill kind(%d) error characNo(%u)",
              k, characNo);
        return 0;
    }
    int i = 0;
    while (i < (int)(count & 0xff) && i < 6)
    {
        // ORIG 每字段独立重算 i*0xe+data（mov -0x1c; add; lea 8x; sub; add
        // 0x18），故不用指针缓存，逐 data[i] 访问（O0 无 CSE 每次重算）。
        db->set_query(
            "upDate %s set value1 = %d, value2 = %d, value3 = %d, value4 = %d, value5 = %d, value6 = %d where charac_no = %u and combo_idx = %d",
            getComboSkillTableName(kind), (int)data[i].m_comboSkillList[0],
            (int)data[i].m_comboSkillList[1], (int)data[i].m_comboSkillList[2],
            (int)data[i].m_comboSkillList[3], (int)data[i].m_comboSkillList[4],
            (int)data[i].m_comboSkillList[5], characNo,
            (int)data[i].m_comboSkillIndex);
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into %s(charac_no, combo_idx, value1, value2, value3, value4, value5, value6) values(%u, %d, %d, %d, %d, %d, %d, %d)",
                getComboSkillTableName(kind), characNo,
                (int)data[i].m_comboSkillIndex, (int)data[i].m_comboSkillList[0],
                (int)data[i].m_comboSkillList[1], (int)data[i].m_comboSkillList[2],
                (int)data[i].m_comboSkillList[3], (int)data[i].m_comboSkillList[4],
                (int)data[i].m_comboSkillList[5]);
            // ORIG 0x844ad74-0x844ad85 `cmp $0x426; sete %al; test; jne
            // <continue>; mov $0,%eax; jmp end` —— (bool) 物化 + **空 then +
            // else return** 才保 sete+jne；`!(bool)` 形被 4.4 fold 成
            // setne+je（探针 /tmp/w8lt_set3.o::t17 全等）。
            if (db->exec(true) != 1)
                if ((bool)(db->getDBError() == 0x426))
                    ;
                else
                    return 0;
        }
        ++i;
    }
    return 1;
}

// ============================================================================
// DB_UpdateComboSkill::dispatch —— ORIG 0x0844a8ea（报告
// docs/class_func_reports/DB_UpdateComboSkill/dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_3, 无组)，空 → 0；GetOutBuffer<SIG_COMBO_SKILL>，
// 空 → 0。setComboSkill(db, m_characNo+0x00, m_tree1Count+0x04,
// m_tree1Data+0x0a, kind 0)，失败 → cMyTrace(0xbb2c)；
// 再 setComboSkill(db, m_characNo, m_tree2Count+0x05, m_tree2Data+0x5e,
// kind 1)，失败 → cMyTrace(0xbb33)。返 1。
#include "W8LT_DBTask/dispatch_138.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    SIG_COMBO_SKILL* sig = stream->GetOutBuffer<SIG_COMBO_SKILL>();
    if (sig == 0)
        return 0;
    if (!setComboSkill(db, (unsigned int)sig->m_characNo, sig->m_tree1Count,
                       sig->m_tree1Data, SKILL_TREE_1))
    {
        // b28c-L19 清绑（P0/N96）：改全表达式临时链内联实参（P227），
        // 由 4.4 自然升格 callee-saved 保活 characNo 跨 ctor call。
        cMyTrace(__PRETTY_FUNCTION__, 0xbb2c, 5)(
            "DB_UpdateComboSkill::dispatch, setComboSkill ERROR charac_no=%u",
            (unsigned int)sig->m_characNo);
    }
    if (!setComboSkill(db, (unsigned int)sig->m_characNo, sig->m_tree2Count,
                       sig->m_tree2Data, SKILL_TREE_2))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xbb33, 5)(
            "DB_UpdateComboSkill::dispatch, setComboSkill ERROR charac_no=%u",
            (unsigned int)sig->m_characNo);
    }
    return 1;
}

// ============================================================================
// DB_ReqItemUpgradeLog::dispatch —— ORIG 0x08425dbe（报告
// docs/class_func_reports/DB_ReqItemUpgradeLog/dispatch.md 全文）
// ============================================================================
// DB_HANDLE_4（LOGDB）。流解包 4×uint（a,b,c,d）+ bool e + char str[29]；
// inSert into log_item_develop(occ_time,it_no,server_id,charac_no,charac_name,
// develop,sucess_flag) values(now(),%d,%d,%d,'%s',%d,%d) =
// (b,a,c,str,d,(uint)e)；exec(true)。Ghidra 标 void：函数尾无置值——ORIG 源码
// 为 bool 覆盖且体末无显式 return（eax 残留 exec 返回值，UB 与原版一致，
// 同 DB_UpdateAvatarJewelSlot::dispatch 模式）。故此处不写 return。
#include "W8LT_DBTask/dispatch_139.inc"
{
    // 声明序 db,a,b,c,d,str,e 与 ORIG 栈槽对应（-0xc/-0x10/-0x14/-0x18/
    // -0x1c/-0x39/-0x3a；gcc O0 先声明→高地址）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int a = 0;
    unsigned int b = 0;
    unsigned int c = 0;
    unsigned int d = 0;
    char str[29];
    bool e = true;
    *stream >> a;
    *stream >> b;
    *stream >> c;
    *stream >> d;
    *stream >> e;
    *stream >> str;
    db->set_query("inSert into log_item_develop(occ_time,it_no,server_id,charac_no,charac_name,develop,sucess_flag) values(now(),%d,%d,%d,'%s',%d,%d)",
                  b, a, c, str, d, (unsigned int)e);
    db->exec(true);
}

// ============================================================================
// DB_SaveCleanpadPoint::dispatch —— ORIG 0x08432f3e（报告
// docs/class_func_reports/DB_SaveCleanpadPoint/dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6。*stream >> point(uint，SQL cleanpad_point=%d) → *stream >>
// mId(uint，SQL m_id=%s 经 NumberToString(mId,0)) → upDate member_login set
// cleanpad_point=%d where m_id=%s → 返回 exec(true)==1。
#include "W8LT_DBTask/dispatch_140.inc"
{
    unsigned int point;
    unsigned int mId;
    *stream >> point;
    *stream >> mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("upDate member_login set cleanpad_point=%d where m_id=%s",
                  point, NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    return 1;
}

// ---- DB_LoadCleanPadPoint::dispatch 的 SIG_LOAD_CLEANPAD_POINT 载荷视图 ----
// SIG_LOAD_CLEANPAD_POINT（0x3cf0，SigTypes_C.h m_pad 占位）；ORIG 0x842ecf4
// memset 0x3cf0 后按 (hack_type-0x65)*4 写 int value（hack_type ∈ (100,0xfa1)）。
struct DB_CleanpadRatioView
{
    int m_value[0xf3c];   // +0x00（0x3cf0 / 4）
};
static_assert(sizeof(DB_CleanpadRatioView) == 0x3cf0,
              "DB_CleanpadRatioView ABI drift");

// ============================================================================
// DB_LoadCleanPadPoint::dispatch —— ORIG 0x0842ecf4（报告
// docs/class_func_reports/DB_LoadCleanPadPoint/dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6。seLect hack_type,value from hack_cleanpad_ratio_info；exec
// 失败 → logFormat(0x73b0) 返 0；get_n_rows()==0 → cMyTrace(0x73b6,0) 返 1；
// 否则 Acquire("DBThread.cpp",0x73be) → guard <<0x127(int) <<-1(int) →
// GetInBuffer<SIG_LOAD_CLEANPAD_POINT> memset 0x3cf0 → 逐行
// fetch/get_int(0,hack_type)/get_int(1,value)（任一失败
// s_stream_pool->Free(guard) 返 0）；100<hack_type<0xfa1 时
// m_value[hack_type-0x65]=value；循环毕 put(DISPATCH_Q,guard) 返 1。
#include "W8LT_DBTask/dispatch_141.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("seLect hack_type,value from hack_cleanpad_ratio_info");
    if (db->exec(true) != 1)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x73b0,
                              "[QUERY_ERROR] select hack_cleanpad_ratio_info");
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        // ORIG 0x0842e…：trace 临时对象槽 -0x24（比 guard -0x2c 近 8B）；
        // 具名对象使 4.4 两槽互换（CountOfHumanCertify 同法，P37）。
        cMyTrace(__PRETTY_FUNCTION__, 0x73b6, 0)(
            "[QUERY_NOTICE] have no data hack_cleanpad_ratio_info");
        return 1;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x73be), true);
    guard.operator*()->operator<<((int)0x127);
    guard.operator*()->operator<<((int)-1);
    DB_CleanpadRatioView* out = reinterpret_cast<DB_CleanpadRatioView*>(
        guard.operator->()->GetInBuffer<SIG_LOAD_CLEANPAD_POINT>());
    // ORIG 0x0842e…：`mov %eax,-0x10; mov -0x10,%eax(design 先评估);
    // movl $0x3cf0; movl $0; mov -0x10,%eax; mov (%esp)` ⇒ 首参先评估
    // 后重载（4.1 左起评估序）。数组 decay 形试验。
    memset(out->m_value, 0, 0x3cf0);
    // ORIG 循环变量为 uint（cmp/seta 无符号比较；int 会生成 setg）。
    for (unsigned int i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        int hackType = 0;
        int value = 0;
        if (db->fetch() != 1 || db->get_int(0, hackType) != 1 ||
            db->get_int(1, value) != 1)
        {
            GlobalData::s_stream_pool->Free(guard);
            return 0;
        }
        // ORIG 0x0842ef0a-0x0842ef30 双 jmp 岛（probe pad_p2 实证）：
        // `cmp $0x64;jle island1` + `cmp $0xfa0;jg island2` + write 块尾
        // `jmp ef31`；island1=nop;jmp、island2=nop(fall)。4.4 对正向
        // `if(a>b&&c<d)` 合并 fall-through 消 jmp ⇒ 唯显式 goto 复现。
        if (hackType <= 100)
            goto NEXT_ROW;
        if (hackType > 4000)
            goto NEXT_ROW;
        out->m_value[hackType - 0x65] = value;
    NEXT_ROW:;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

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

// ============================================================================
// DB_LoadServerMessage::dispatch —— ORIG 0x08435ecc（报告
// docs/class_func_reports/DB_LoadServerMessage/dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6。GetOutBuffer<SIG_SELECT_SERVER_MESSAGE> 读 server/channel →
// seLect kind, message_index, charac_name, message,
// unix_timestamp(update_time) from event_server_message where server_info=%d
// and (channel_no=%d or channel_no=0)；exec 失败返 0；Acquire("DBThread.cpp",
// 0x8131) → guard <<userIdx <<slot → GetInBuffer<SIG_EVENT_SERVER_MESSAGE>
// memset 0x1134（= 1 字节 count + 50 行 × 0x58）→ 循环 fetch（失败返 0）：
// get_byte(0,kind)/get_byte(1,msgIdx)/get_str(2,characName,0x14)/
// get_str(3,message,0x32)/get_int(4,timestamp)（任一失败返 0）→
// stSERVER_MESSAGE 行填充（+4 kind/+5 index/+8 lenMsg/m_msg/+0x40 lenMsg2/
// m_msg2/+0x58 m_endTime）→ m_count=(char)n_rows → put(DISPATCH_Q) 返 1。
// [b28d-4] DIFF：P183/P171：ORIG 多 {'mov': 2}（漏具名局部/去缓存不足）；ORIG 08435ecc
#include "W8LT_DBTask/dispatch_142.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    DB_ServerMessageSelectView* sel =
        reinterpret_cast<DB_ServerMessageSelectView*>(
            stream->GetOutBuffer<SIG_SELECT_SERVER_MESSAGE>());
    db->set_query("seLect kind, message_index, charac_name, message, unix_timestamp(update_time) from event_server_message where server_info=%d and (channel_no=%d or channel_no=0)",
                  (unsigned int)(unsigned short)sel->m_server,
                  (unsigned int)(unsigned short)sel->m_channel);
    if (db->exec(true) != 1)
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8131), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    // ORIG：call operator->（返回 this）后再 GetInBuffer（0x80c8c4e 形态）。
    SIG_EVENT_SERVER_MESSAGE* out =
        guard.operator->()->GetInBuffer<SIG_EVENT_SERVER_MESSAGE>();
    memset(out, 0, 0x1134);
    int nRows = db->get_n_rows();
    // 清零序对齐 ORIG：ok → kind → msgIdx → message(rep stos) →
    // characName(movl×5) → timestamp。
    bool ok = false;
    // [b28d-solo-W8LT_DBTask] P266 反向：ORIG store 前 `movzbl -0x3d,%eax; mov %eax,%ecx`
    // 两段装载 ⇒ 源是 char（uchar 源 4.4 恒折单段 `movzbl -0x3d,%ecx`）。
    char kind = 0;
    char msgIdx = 0;
    char message[0x32] = { '\0' };
    char characName[0x14] = { '\0' };
    int timestamp = 0;
    int rowIdx = 0;
    for (int i = 0; i < nRows; ++i)
    {
        ok = db->fetch();
        if (!ok)
            return 0;
        ok = db->get_byte(0, (char&)kind);
        if (!ok)
            return 0;
        ok = db->get_byte(1, (char&)msgIdx);
        if (!ok)
            return 0;
        ok = db->get_str(2, characName, 0x14);
        if (!ok)
            return 0;
        ok = db->get_str(3, message, 0x32);
        if (!ok)
            return 0;
        ok = db->get_int(4, timestamp);
        if (!ok)
            return 0;
        // ORIG 每字段独立重算 out + rowIdx*0x58（P36：不命名行指针缓存）。
        // 残留 ndiff=2：ORIG 0x843619x 值装载为两步 `movzbl slot,%eax; mov
        // %eax,%ecx`（4.1 值寄存器再拷入 store 数据槽），4.4 恒折单步
        // `movzbl slot,%ecx`；register asm 探针 /tmp/w8lt_bs2.o::f4 反致
        // 装载序/寻址全乱 ⇒ N18a 类跨版本折叠，标 blocked。
        out->m_msg[rowIdx].m_kind = kind;
        out->m_msg[rowIdx].m_index = msgIdx;
        out->m_msg[rowIdx].m_lenMsg = (int)strlen(message);
        strncpy(out->m_msg[rowIdx].m_msg, message, strlen(message));
        out->m_msg[rowIdx].m_lenMsg2 = (int)strlen(characName);
        strncpy(out->m_msg[rowIdx].m_msg2, characName, strlen(characName));
        out->m_msg[rowIdx].m_endTime = timestamp;
        ++rowIdx;
    }
    out->m_count = (unsigned char)nRows;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

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

// ============================================================================
// DB_SaveBestClearTime::dispatch —— ORIG 0x08447cc4（报告
// docs/class_func_reports/DB_SaveBestClearTime/dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_BEST_CLEAR_TIME> 空 → 0。DB_HANDLE_2。
// put_compressed_blob_data(db,0,sig+8,0x5dc0,&outBuf) 失败 → logFormat(0xb28d,
// "compress_zip(%u) ERROR charac_no=%u",0,charac_no) 返 0；upDate charac_option
// set best_clear_time='%s' where charac_no=%u（%s=outBuf）；exec 失败 →
// logFormat(0xb299) 返 0；getAffectedRowCount()==0 → inSert
// charac_option(charac_no,best_clear_time) values(%u,'%s')，exec 失败 →
// logFormat(0xb2a6) 返 0；全绿返 1。
#include "W8LT_DBTask/dispatch_143.inc"
{
    DB_BestClearTimeView* sig = reinterpret_cast<DB_BestClearTimeView*>(
        stream->GetOutBuffer<SIG_BEST_CLEAR_TIME>());
    if (sig == 0)
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // ORIG：put 返回值先存 bool 局部（mov %al,-0x9）再 movzbl 回读取反判失败
    // （P22 形态；直接 if(!call) 会少缓存两步）。
    bool zipOk = false;
    char* outBuf = 0;
    zipOk = put_compressed_blob_data(
        db, 0, (char*)((unsigned long)sig + 8UL), 0x5dc0, &outBuf);
    if (!zipOk)
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xb28d,
            "DB_SaveBestClearTime::dispatch compress_zip(%u) ERROR charac_no=%u",
            0, sig->m_characNo);
        return 0;
    }
    db->set_query("upDate charac_option set best_clear_time = '%s' where charac_no=%u",
                  outBuf, sig->m_characNo);
    if (db->exec(true) != 1)
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xb299,
            "DB_SaveBestClearTime->upDate Query ERROR charac_no=%u",
            sig->m_characNo);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into charac_option(charac_no, best_clear_time) values(%u, '%s')",
                      sig->m_characNo, outBuf);
        if (db->exec(true) != 1)
        {
            LogManager::logFormat(
                1, "DBThread.cpp",
                __PRETTY_FUNCTION__,
                0xb2a6,
                "DB_SaveBestClearTime->inSert Query ERROR charac_no=%u",
                sig->m_characNo);
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_UpdateQueryCountDbLogdb::dispatch —— ORIG 0x0841c61a（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_4,0) → 解包 occTime(int)/count(int) → while (i < count)：
// 解包 qId/total/responseTime 三 int → channelNo=G_CEnvironment()->get_channel_no()
// → set_query("inSert into log_query_stat(occ_time,q_id,total,response_time,gc_no)
// values(from_unixtime(%d),%d,%d,%d,%d)", occTime, qId, total, responseTime,
// channelNo) → exec(true) 失败返 0；循环完返 1。
#include "W8LT_DBTask/dispatch_144.inc"
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int occTime = 0;    // -0x14
    int count = 0;      // -0x18
    *stream >> occTime;
    *stream >> count;
    int i = 0;          // -0xc（while 形态：先 jmp 到比较）
    while (i < count)
    {
        int qId = 0;            // -0x1c（解包 1 → SQL q_id）
        int total = 0;          // -0x20（解包 2 → SQL total）
        int responseTime = 0;   // -0x24（解包 3 → SQL response_time）
        *stream >> qId;
        *stream >> total;
        *stream >> responseTime;
        db->set_query(
            "inSert into log_query_stat(occ_time,q_id,total,response_time,gc_no) "
            "values(from_unixtime(%d),%d,%d,%d,%d)",
            occTime, qId, total, responseTime,
            G_CEnvironment()->get_channel_no());
        if (db->exec(true) != 1)
            return 0;
        ++i;
    }
    return 1;
}

// ============================================================================
// DB_UpdateAddItemCountValue::dispatch —— ORIG 0x0843ed96（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_9,0) → 解包 mId(uint)/itemCnt(uint) →
// G_CDataManager()->get_event_script_mng() → get_event_entity(0x193)，空返 0；
// 表名 = toTString(*(std::string*)entity->get_db_table_name())（vtable+0x20 虚调用）。
// upDate %s set item_cnt=item_cnt+%d where m_id=%s（table, itemCnt,
// NumberToString(mId,0)）；exec 失败返 0；affectedRowCount==0 → inSert into
// %s(m_id, item_cnt) values(%s, %d)（table, mIdStr, itemCnt），insert exec 失败
// 返 0；全绿返 1。
// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（112=112）。
#include "W8LT_DBTask/dispatch_145.inc"
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;     // -0x18
    unsigned int itemCnt = 0; // -0x1c
    *stream >> mId;
    *stream >> itemCnt;
    // ORIG：G_CDataManager() 结果不落槽（链式直传 get_event_script_mng）。
    EventClassify::CEventScriptMng* sm =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    EventClassify::CEventEntity* entity = sm->get_event_entity(0x193);
    if (entity == 0)
        return 0;
    // ORIG：表名不缓存（UPDATE/INSERT 各自 vtable+0x20 虚调用 + toTString，
    // 无 CSE）；mId 串每处 NumberToString 即时求值（ebx/esi callee-saved 缓存）。
    db->set_query("upDate %s set item_cnt=item_cnt+%d where m_id=%s",
                  toTString(entity->get_db_table_name()),
                  itemCnt, NumberToString(mId, 0));
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into %s(m_id, item_cnt) values(%s, %d)",
                      toTString(entity->get_db_table_name()),
                      NumberToString(mId, 0), itemCnt);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateOntimeLastRecvIdx::dispatch —— ORIG 0x0844280a（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_D,0) → 解包 mId(uint)/lastNo(int) → tableNo = mId % 10
// → set_query("upDate event_1112_ontime_user_%u set last_no=%u where m_id=%s",
// tableNo, lastNo, NumberToString(mId,0)) → exec(true)；失败或 affectedRowCount
// ==0 → cMyTrace(__PRETTY_FUNCTION__, 0xa407, 5)("... failed at m_id(%s)",
// tableNo, lastNo, mIdStr) 返 0；成功返 1。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 0844280a
// [L4-r1] cMyTrace 改 P227 全表达式临时链 ⇒ ndiff 54→8（余 4 条 mov 中转 =
//   N18a 单段直落族封顶）。
// [r3 负结论] `unsigned int tableNo = mId % 10;` 拆成声明+赋值两行实测**零变化**
//   （our 98/ORIG 94 未动、ndiff 8、四数 141/1750/12/57 逐值相同）⇒ 4.4 对
//   `%` 的商中间值恒走槽（ORIG 0844280c `shr $0x3,%edx` 用寄存器保活）⇒ N18a 封顶。
#include "W8LT_DBTask/dispatch_146.inc"
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_D,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;   // -0x24
    int lastNo = 0;         // -0x28
    *stream >> mId;
    *stream >> lastNo;
    unsigned int tableNo = mId % 10;   // -0xc（ORIG 0xcccccccd mul 序列）
    db->set_query("upDate event_1112_ontime_user_%u set last_no=%u where m_id=%s",
                  tableNo, lastNo, NumberToString(mId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        // [L4-r1] P227 全表达式临时链（ORIG ctor 前取 varargs + 跨 ctor 保活）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa407, 5)(
            "upDate event_1112_ontime_user_%u set last_no=%u failed at m_id(%s)",
            tableNo, lastNo, NumberToString(mId, 0));
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateFatigueAttendance::dispatch —— ORIG 0x084444ec（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_9,0) → map<unsigned char,short> 构造 → 解包 mId(uint)/
// count(int) → 循环 count 次：解包 occDay(uchar)/fatigue(short) 后 insert 入 map
// → begin/end 迭代遍历：it->second > 0 时 set_query("upDate %s set fatigue =
// fatigue + %d where occ_day = %d and m_id = %s", tableName, it->second,
// it->first, NumberToString(mId,0))（表名 = std::string(get_db_table_name(0x56))
// 经 toTString）→ exec 失败或 affectedRowCount==0 → inSert into %s(occ_day,m_id,
// fatigue) values(%d,%s,%d)（tableName, occDay, mIdStr, fatigue），insert exec 返
// 回值丢弃；循环完返 1（ORIG 恒 1）。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 084444ec
#include "W8LT_DBTask/dispatch_147.inc"
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;    // -0x30
    int count = 0;           // -0x34
    std::map<unsigned char, short> fatigueMap;   // -0x4c（ORIG map C1）
    *stream >> mId;
    *stream >> count;
    for (int i = 0; i < count; ++i)
    {
        // [J4S3-12 P171 反向] ORIG 0x08444559 之后直接 lea 取址解包，无 movb/movw
        // 预清（裸声明只占槽不发指令）⇒ 删初值。
        unsigned char occDay;   // -0x59
        short fatigue;          // -0x5c
        *stream >> occDay;
        *stream >> fatigue;
        // ORIG：显式 pair 构造（_ZNSt4pairIKhsEC1IRhRsEEOT_OT0_，非 make_pair）
        fatigueMap.insert(
            std::pair<const unsigned char, short>(occDay, fatigue));
    }
    CDataManager* dm = G_CDataManager();
    EventClassify::CEventScriptMng* sm =
        static_cast<EventClassify::CEventScriptMng*>(dm->get_event_script_mng());
    // ORIG 0x08444616：get_db_table_name(int) 返回 std::string&（_ZNSsC1ERKSs 拷贝构造）。
    std::string tableStr(sm->get_db_table_name(0x56));
    // [b28d-solo] P167：ORIG 两处 set_query 各自 toTString(tableStr)（无 CSE，
    // 差集 -1 toTString）⇒ 禁具名 tableName 缓存，逐处内联。
    for (std::map<unsigned char, short>::iterator it = fatigueMap.begin();
         it != fatigueMap.end(); ++it)
    {
        if (it->second > 0)
        {
            db->set_query(
                "upDate %s set fatigue = fatigue + %d where occ_day = %d and m_id = %s",
                toTString(tableStr), (int)it->second, (unsigned int)it->first,
                NumberToString(mId, 0));
            if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
            {
                db->set_query(
                    "inSert into %s(occ_day,m_id,fatigue) values(%d,%s,%d)",
                    toTString(tableStr), (unsigned int)it->first,
                    NumberToString(mId, 0), (int)it->second);
                db->exec(true);   // ORIG insert exec 返回值丢弃
            }
        }
    }
    return 1;
}

// ============================================================================
// DB_ForceSaveSimpleSSO::dispatch —— ORIG 0x08428154（报告 dispatch.md 全文）
// ============================================================================
// 解包 mId(uint)/errCnt(int) → GlobalData::s_psimpleSSO->updateHumanCertify(
// mId, errCnt)；非 1 返 0，否则返 1（ORIG xor $1/test/je + mov $0/$1 分支）。
#include "W8LT_DBTask/dispatch_148.inc"
{
    unsigned int mId;    // -0xc（ORIG lea 首参解包位）
    int errCnt;          // -0x10
    stream->operator>>(mId);
    stream->operator>>(errCnt);
    if (!GlobalData::s_psimpleSSO->updateHumanCertify(mId, errCnt))
        return 0;
    return 1;
}

// ============================================================================
// DB_GuildExpBookDelete::dispatch —— ORIG 0x0843c35a（报告 dispatch.md 全文）
// ============================================================================
// 解包 characNo(uint)/guildSecede(int)/serverGroup(int) → GetDBHandle(2,
// serverGroup) → upDate charac_info set guild_secede=%d where charac_no=%u →
// exec(true)（结果忽略）→ 恒返 1（ORIG mov $0x1,%eax）。
#include "W8LT_DBTask/dispatch_149.inc"
{
    MySQL* db;                  // -0xc
    unsigned int characNo = 0;  // -0x10
    int guildSecede = 0;        // -0x14
    int serverGroup = 0;        // -0x18
    stream->operator>>(characNo);
    stream->operator>>(guildSecede);
    stream->operator>>(serverGroup);
    db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)serverGroup);
    db->set_query(
        "upDate charac_info set guild_secede = %d where charac_no = %u",
        guildSecede, characNo);
    db->exec(true);
    return 1;
}

// ============================================================================
// DB_InsertCreatureItem::insertCreatureItem —— ORIG 0x0841fa28
//（报告 insertCreatureItem.md 全文；static，dispatch 内调用点只压两实参）
// ============================================================================
// +0x68(m_mapSize)==0 → upDate inventory set creature_flag=1 where
// charac_no=m_characNo(+0x48)；exec 失败 cMyTrace(0x55dd,5) 返 0。
// expire_date/delete_date 默认 "'9999-12-31 23:59:59'"；m_days(+0x54)!=0 →
// from_unixtime(GetDateTimeTick() + m_days*0x15180)；m_expireTick(+0x58)!=0 →
// from_unixtime(m_expireTick)；days==0 且 expireTick!=0 → expire_date 同
// expireTick。inSert creature_items(charac_no,slot,it_id,expire_date,
// delete_date,reg_date,name,stomach,exp,endurance,no_charge,stat,
// creature_type)values(%d,%d,%d,%s,%s,NOW(),'%s',%d,%d,%d,0,%d,%d)：
// characNo, slot, it_id(*(int*)(sig+8)), expireDate, deleteDate,
// name(sig+0x34), stomach(+0x44), exp(*(int*)(sig+0x4c)), endurance(ushort
// sig+0x14 零扩展), stat((signed char)sig+0x53), creature_type((signed char)
// sig+0x50)；no_charge 为 SQL 字面 0（ORIG 未压实参槽）。返回 exec(true)。
#include "W8LT_DBTask/insertCreatureItem.inc"
                                               SIG_CREATURE_ITEM const* sig)
{
    if (sig->m_mapSize == 0)   // +0x68
    {
        db->set_query(
            "upDate inventory set creature_flag=%d where charac_no=%d",
            1, sig->m_characNo);
        if (db->exec(true) != 1)
        {
            // ORIG 0x0841fa7c：`mov 0xc(%ebp),%eax; mov 0x48(%eax),%ebx` 前置
            // 缓存 charac_no，跨 cMyTrace ctor 后 `mov %ebx,0x8(%esp)`（P87）。
            register int cn = sig->m_characNo;
            cMyTrace trace(__PRETTY_FUNCTION__, 0x55dd, 5);
            trace("DB_InsertCreatureItem::dispatch, exec() ERROR charac_no=%u",
                  cn);
            return 0;
        }
    }
    // ORIG：strncpy 0x16 + 两字节 0（24 字节 movl 序列）+ 40 字节 rep stos。
    // gcc4.4 O0 不内联 strncpy/memcpy，用数组初始化触发同形常量 store。
    char expireDate[0x40] = "'9999-12-31 23:59:59'";
    int tick = OS_API::GetDateTimeTick();   // -0x24
    int days = sig->m_days;                 // -0x20（+0x54）
    if (days != 0)
    {
        memset(expireDate, 0, 0x40);
        sprintf(expireDate, "from_unixtime('%d')", days * 0x15180 + tick);
    }
    char deleteDate[0x40] = "'9999-12-31 23:59:59'";
    int expireTick = sig->m_expireTick;   // -0x1c（+0x58）
    if (expireTick != 0)
    {
        memset(deleteDate, 0, 0x40);
        sprintf(deleteDate, "from_unixtime('%d')", expireTick);
    }
    if (days == 0 && expireTick != 0)
    {
        memset(expireDate, 0, 0x40);
        sprintf(expireDate, "from_unixtime('%d')", expireTick);
    }
    db->set_query(
        "inSert into creature_items(charac_no,slot,it_id,expire_date,delete_date,reg_date,name,stomach,exp,endurance,no_charge,stat,creature_type)values(%d,%d,%d,%s,%s,NOW(),'%s',%d,%d,%d,0,%d,%d)",
        sig->m_characNo, sig->m_slot,
        ((DB_CreatureItemInserView*)sig)->m_itId,
        expireDate, deleteDate,
        sig->m_name,
        sig->m_stomachValue,
        ((DB_CreatureItemInserView*)sig)->m_exp,
        (unsigned int)((DB_CreatureItemInserView*)sig)->m_endurance,
        (int)(signed char)sig->m_field53, (int)(signed char)sig->m_flag50);
    return db->exec(true);
}

// ============================================================================
// DB_InsertCreatureItem::dispatch —— ORIG 0x0841f8d4（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_CREATURE_ITEM> → GetDBHandle(3,0) → insertCreatureItem
// 失败返 0。成功：Acquire("DBThread.cpp",0x55c8) → CStreamGuard(stream,true)
// → operator*()-><<0x88 → operator*()-><<slot(第3参) →
// operator->()->GetInBuffer<SIG_CREATURE_ITEM> → memcpy 0x74(rep movsl 0x1d)
// → out->m_addInfo2(+0x30)=WongWork::DBCommon::GetIdentity(db) →
// MsgQueueMgr::put(s_msgq_mgr, DISPATCH_Q(1), guard) → 返 1。
#include "W8LT_DBTask/dispatch_150.inc"
{
    static const int handleIdx = 3;
    // 4.4 槽分配 = 先声明占更低槽、block 对象续顶向下：声明序 sig/db/out →
    // sig(-0x24) db(-0x20) out(-0x1c) guard(-0x2c)，对齐 ORIG 0x841f8xx。
    SIG_CREATURE_ITEM* sig;    // -0x24
    MySQL* db;                 // -0x20
    SIG_CREATURE_ITEM* out;    // -0x1c
    sig = stream->GetOutBuffer<SIG_CREATURE_ITEM>();
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                           E_SERVER_GROUP_NONE);
    if (!insertCreatureItem(db, sig))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x55c8), true);
    guard.operator*()->operator<<((int)0x88);
    guard.operator*()->operator<<((int)slot);
    out = guard.operator->()->GetInBuffer<SIG_CREATURE_ITEM>();
    *out = *sig;   // 结构整体赋值 → rep movsl 0x1d（P83；memcpy 会 call）
    out->m_addInfo2 = WongWork::DBCommon::GetIdentity(db);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

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

// ============================================================================
// DB_InsertResponseTime::dispatch —— ORIG 0x0843fc26（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(4,0) → GetOutBuffer<SIG_INSERT_RESPONSE_TIME> → 循环 i<=99 且
// i<count：values 非空先 += ","；sprintf("(%d,from_unixtime(%d),%u,%u,%llu,
// %u)", channel_no(G_CEnvironment()->get_channel_no()), baseSec, id, count,
// bytesLow|bytesHigh, avg)；values += rowBuf。尾部 monthBuf[10]={0}；
// occTime=baseSec；localtime_r；strftime("%Y%m")；
// set_query("inSert into log_response_time_%s(channel_no,occ_time,packet_id,
// packet_count,total_response_time,avg_response_time) values%s", monthBuf,
// values.c_str()) → exec(true)（结果跨 values 析构保留）→ 返回 exec 结果。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0843fc26
// [r3] db/sig 改「声明即初始化」并前置于 STL 局部之前（ORIG 0843fc4f db→
//   0843fc5d sig→values ctor 序）⇒ ndiff 62→42、gap 2 不变；残余 = ORIG
//   4.1 lea/add 地址重物化 + 槽号（N18a/N82）⇒ 封顶。
#include "W8LT_DBTask/dispatch_151.inc"
{
    // [J4S2-① P212/P309/P198①] ORIG 取得/物化序 = db(-0x24) → sig(-0x20) → i(-0x1c)：
    // 0x0843fc4f `mov %eax,-0x24`（db）→ 0x0843fc5d `mov %eax,-0x20`（sig）→
    // 0x0843fcc6 `movl $0x0,-0x1c`（i，位于 values 构造与 rowBuf 清零之后）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_INSERT_RESPONSE_TIME* sig =
        stream->GetOutBuffer<SIG_INSERT_RESPONSE_TIME>();
    int i;                          // -0x1c（ORIG 0843fcc6 movl $0x0,-0x1c）
    std::string values;             // -0x28
    // [P5S2W8LT r1+r3] ORIG 的 monthBuf(-0x32) 清零**不在声明处**：0x0843fc6b 起
    // 只对 rowBuf(-0x163) 发 rep stos，monthBuf 的
    // `movl $0,-0x32; movl $0,-0x2e; movw $0,-0x2a` 三条发在 0x0843fdfd-0x0843fe0b
    // （**循环结束后、occTime=baseSec 之前**）。声明处初始化器会把它们提前到
    // values ctor 之后 ⇒ 与 ORIG 双错位（r1 删初始化器后 fdiff 显示 ORIG-only 3 条）；
    // 故裸声明 + 循环后显式 memset 复现发码位。
    char monthBuf[10];              // -0x32（ORIG 无初始化器）
    time_t occTime;                 // -0x38
    struct tm tmBuf;                // -0x64
    char rowBuf[0xff] = { '\0' };   // -0x163（ORIG 255 字节带对齐分支 rep stos）
    i = 0;
    while (i <= 99 && i < ((DB_ResponseTimeSigView*)sig)->m_count)
    {
        if (!values.empty())
            values += ",";
        // ORIG 行字段逐次重读（P48 无 CSE）；%llu 直接读 8 字节（低字先行）。
        sprintf(rowBuf, "(%d,from_unixtime(%d),%u,%u,%llu,%u)",
                G_CEnvironment()->get_channel_no(),
                ((DB_ResponseTimeSigView*)sig)->m_baseSec,
                ((DB_ResponseTimeSigView*)sig)->m_entries[i].m_id,
                ((DB_ResponseTimeSigView*)sig)->m_entries[i].m_count,
                *(unsigned long long*)&
                    ((DB_ResponseTimeSigView*)sig)->m_entries[i].m_bytesLow,
                ((DB_ResponseTimeSigView*)sig)->m_entries[i].m_avg);
        values += rowBuf;
        ++i;
    }
    memset(monthBuf, 0, 0xa);   // ORIG 0x0843fdfd-0x0843fe0b
    occTime = (time_t)((DB_ResponseTimeSigView*)sig)->m_baseSec;
    localtime_r(&occTime, &tmBuf);
    strftime(monthBuf, 10, "%Y%m", &tmBuf);
    db->set_query(
        "inSert into log_response_time_%s(channel_no,occ_time,packet_id,packet_count,total_response_time,avg_response_time) values%s",
        monthBuf, values.c_str());
    return db->exec(true);
}

// ============================================================================
// DB_InsertUserPlayTime::dispatch —— ORIG 0x084256b4（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(6,0) → 解包 mId(uint)/playTime(int)/tradeCnt(int)/exp(uint)/
// usedFatigue(ushort)/tingCount(bool) → 表名缓冲 memset 0x14 + memcpy
// "member_play_info"(0x11)。upDate %s set play_time=play_time+%d,
// play_count=play_count+1,trade_cnt=trade_cnt+%d, exp=exp+%u,
// used_fatigue=used_fatigue+%u, ting_count=ting_count+%d where
// occ_date=cast(now() as date) and m_id=%s（playTime, tradeCnt, exp,
// (uint)usedFatigue, (uint)(!tingCount), NumberToString(mId,0)）。
// exec==1 且 getAffectedRowCount()!=0 → 跳过 insert 返 1；否则
// inSert into %s(occ_date,m_id,play_time,play_count,trade_cnt,exp,
// used_fatigue,ting_count) values(cast(now() as date),%s,%d,1,%d,%u,%u,%d)
// → insert exec 失败返 0，成功返 1。
#include "W8LT_DBTask/dispatch_152.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);   // -0x1c
    unsigned int mId = 0;      // -0x20
    int playTime = 0;          // -0x24
    int tradeCnt = 0;          // -0x28
    unsigned int exp = 0;      // -0x2c
    unsigned short usedFatigue = 0;   // -0x2e（movw 清零）
    bool tingCount;            // -0x2f（被 ushort 清零顺带覆盖，不单独初始化）
    stream->operator>>(mId);
    stream->operator>>(playTime);
    stream->operator>>(tradeCnt);
    stream->operator>>(exp);
    stream->operator>>(usedFatigue);
    stream->operator>>(tingCount);
    char tableName[0x14];      // -0x43
    memset(tableName, 0, 0x14);
    memcpy(tableName, "member_play_info", 0x11);
    db->set_query(
        "upDate %s set play_time=play_time+%d,play_count=play_count+1,trade_cnt=trade_cnt+%d, exp=exp+%u, used_fatigue=used_fatigue+%u, ting_count=ting_count+%d where occ_date=cast(now() as date) and m_id=%s",
        tableName, playTime, tradeCnt, exp, (unsigned int)usedFatigue,
        (unsigned int)(tingCount ? 0 : 1), NumberToString(mId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into %s(occ_date,m_id,play_time,play_count,trade_cnt,exp,used_fatigue,ting_count) values(cast(now() as date),%s,%d,1,%d,%u,%u,%d)",
            tableName, NumberToString(mId, 0), playTime, tradeCnt, exp,
            (unsigned int)usedFatigue, (unsigned int)(tingCount ? 0 : 1));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}
// ============================================================================
// DB_SavePvPMissionList::dispatch —— ORIG 0x0843f8fa（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x843f8fa：GetDBHandle(DB_HANDLE_3,0) → GetOutBuffer<
// SIG_REQUEST_PVP_MISSION_LIST> → 0x198 字节缓冲清零 → zipLen=0x198 →
// compress_zip(buf,&zipLen,sig,0x14a) 失败返 0；blob_to_str(0,buf,zipLen)
// → set_query("upDate fair_pvp_score set pvp_mission_info='%s' where
// charac_no=%u", str, sig->m_characNo) → exec 失败返 0；
// getAffectedRowCount()==0 → blob_to_str 再编 →
// inSert fair_pvp_score(charac_no,pvp_mission_info) values(%u,'%s')
// （m_characNo, str）→ exec 失败返 0；全绿返 1。
// ============================================================================
// [b28d-4] DIFF：N18a/P211：4.4 多 {'mov': 4} 中转 mov（单段直落/同型化待验）；ORIG 0843f8fa
// [r3] 消具名 `char* str`（两次 blob_to_str 内联进 set_query 实参）：帧不再
//   多 4B，槽序对齐 ORIG（db -0x10 / sig -0xc / zipBuf -0x1ac / zipLen -0x14）
//   ⇒ ndiff 52→0 完全一致（orig=our=99、gap=0、差集空）。
#include "W8LT_DBTask/dispatch_153.inc"
                                     Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db;                                  // -0x10
    SIG_REQUEST_PVP_MISSION_LIST* sig;          // -0xc
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                           E_SERVER_GROUP_NONE);
    sig = stream->GetOutBuffer<SIG_REQUEST_PVP_MISSION_LIST>();
    // [Lane4] 声明序按 ORIG 代码序：ORIG 0x843f8fa 的 rep stos（zipBuf 清零）位于
    // GetDBHandle/GetOutBuffer 两调用**之后**、zipLen 初始化**之前**（实测 0x843f933
    // 起 rep stos，0x843f949 才 movl $0x198,-0x14）。旧序把 zipBuf 提到函数首 ⇒
    // 我方前面多整整一段 rep stos（gap −4/ndiff 62）。
    char zipBuf[0x198] = { '\0' };              // -0x1ac（ORIG rep stos 清零）
    unsigned int zipLen = 0x198;                // -0x14（ORIG rep stos 之后初始化）
    if (!compress_zip(zipBuf, zipLen, (const char*)sig, 0x14a))
        return 0;
    db->set_query(
        "upDate fair_pvp_score set pvp_mission_info='%s' where charac_no=%u",
        db->blob_to_str(0, zipBuf, (int)zipLen), sig->m_characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into fair_pvp_score(charac_no, pvp_mission_info) values(%u, '%s')",
            sig->m_characNo, db->blob_to_str(0, zipBuf, (int)zipLen));
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

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
                                     Stream* stream)
{
    static const int handleIdx = 3;
    // 4.4 先声明占更低槽：outStr(-0x28) ok(-0x21) sig(-0x20) db(-0x1c)，
    // 对齐 ORIG 0x843fa9x（ok=exec 结果字节槽，`mov %al,-0x21; movzbl
    // -0x21,%eax; xor $1; test` 物化形，见 0x843fbxx）。
    char* outStr;                    // -0x28（put_compressed_blob_data 出参）
    bool ok;                         // -0x21
    DB_FairPvPScoreView* sig;        // -0x20
    MySQL* db;                       // -0x1c
    sig = (DB_FairPvPScoreView*)stream
              ->GetOutBuffer<SIG_FAIR_PVP_SCORE>();
    if (!sig)
        return 0;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                           E_SERVER_GROUP_NONE);
    outStr = 0;
    // ORIG 0x843faax `mov -0x20,%eax; add $0x2c,%eax` —— 4.4 指针加法恒折
    // `lea off(%eax),%edx` 换寄存器；经 (unsigned long) 域算式阻止折叠
    // （探针 /tmp/w8lt_lea3.o::g7 vs ::g8）。
    if (!put_compressed_blob_data(
            db, 0, (char*)((unsigned long)sig + 0x2cUL), 0x650, &outStr))
        return 0;
    db->set_query(
        "upDate fair_pvp_score set private_win=%u,private_lose=%u,private_draw=%u,relay_battle_win=%u,relay_battle_lose=%u,relay_battle_draw=%u,relay_battle_2kill=%u,relay_battle_3kill=%u,successive_win=%u,max_successive_win=%u,job_score='%s',daily_play_count =%u, last_play_time=from_unixtime(%d),give_item=%d where charac_no=%u",
        sig->m_privateWin, sig->m_privateLose, sig->m_privateDraw,
        sig->m_relayBattleWin, sig->m_relayBattleLose, sig->m_relayBattleDraw,
        sig->m_relayBattle2Kill, sig->m_relayBattle3Kill,
        sig->m_successiveWin, sig->m_maxSuccessiveWin, outStr,
        sig->m_dailyPlayCount, sig->m_lastPlayTime,
        (unsigned int)sig->m_giveItem, sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
        return 0;
    return 1;
}

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
// ORIG 0x842b6e6：GetDBHandle(DB_HANDLE_6,0) →
// GetOutBuffer<SIG_SECURITY_CARD_DISUSE> → turnOffSecurityFlag(db, *(uint*)sig)
// → set_query("upDate member_security_card set apply_flag=0 where m_id=%s",
// NumberToString(m_id,0)) → exec(true)（结果丢弃）→
// saveSecurityCardHistory(db, m_id, ENUM_SECURITYCARD_MOD_TYPE_DISUSE=2)
// → 返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_155.inc"
                                     Stream* stream)
{
    MySQL* db;                             // -0x10
    SIG_SECURITY_CARD_DISUSE* sig;         // -0xc
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                           E_SERVER_GROUP_NONE);
    sig = stream->GetOutBuffer<SIG_SECURITY_CARD_DISUSE>();
    turnOffSecurityFlag(db, ((SecuDisuseView*)sig)->m_id);
    db->set_query(
        "upDate member_security_card set apply_flag=0 where m_id=%s",
        NumberToString(((SecuDisuseView*)sig)->m_id, 0));
    db->exec(true);
    saveSecurityCardHistory(db, ((SecuDisuseView*)sig)->m_id,
                            ENUM_SECURITYCARD_MOD_TYPE_DISUSE);
    return 1;
}

// ============================================================================
// DB_CountOfHumanCertify::dispatch —— ORIG 0x084282a0（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x84282a0：GetDBHandle(DB_HANDLE_6,0) → *stream >> m_id(uint) →
// set_query("upDate human_certify_try_count set count = count + 1 where
// m_id = %s", NumberToString(m_id,0)) → exec(true)。exec 失败或受影响行数
// 为 0 → inSert human_certify_try_count(m_id,count) values(%s,1)（m_id）；
// insert exec 失败 → cMyTrace(0x69cd,5) "Update and Insert into
// HumanCertifyCount failed" 返 0；成功返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_156.inc"
                                      Stream* stream)
{
    unsigned int m_id;                     // -0x20（ORIG 0x8424xxx 帧顶
                                           // 无 `movl $0,-0x20` 清零 ⇒
                                           // 声明无初始化）
    MySQL* db;                             // -0xc
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                           E_SERVER_GROUP_NONE);
    *stream >> m_id;
    db->set_query(
        "upDate human_certify_try_count set count = count + 1 where m_id = %s",
        NumberToString(m_id, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into human_certify_try_count(m_id, count) values(%s, 1)",
            NumberToString(m_id, 0));
        if (db->exec(true) != 1)
        {
            // ORIG 0x0842839d(+0xfd)：trace 临时对象落 -0x1c、m_id 占远槽
            // -0x20（P37 临时形；4.4 具名对象反先分配致两槽互换）。
            cMyTrace(__PRETTY_FUNCTION__, 0x69cd, 5)(
                "Update and Insert into HumanCertifyCount failed");
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_TimerStatisticsLog::dispatch —— ORIG 0x08427c06（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8427c06：四个函数局部 static 实例（crash_down_report/
// stat_dungeon_fail/stat_pvp_play2/stat_use_shusia_service，各带 guard+
// atexit，P20 范式）依次 dispatch(userIdx, slot, stream)（返回值丢弃）；
// 最后 GetInstanceClientCrashDownLogStatistic()->CarryClientCrashDown()
// → 返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_157.inc"
{
    static DB_CrashDownReport crash_down_report;
    crash_down_report.dispatch(userIdx, slot, stream);

    static DB_StatDungeonFail stat_dungeon_fail;
    stat_dungeon_fail.dispatch(userIdx, slot, stream);

    static DB_StatPVPPlay2 stat_pvp_play2;
    stat_pvp_play2.dispatch(userIdx, slot, stream);

    static DB_StatUseShusiaService stat_use_shusia_service;
    stat_use_shusia_service.dispatch(userIdx, slot, stream);

    // [C-3/2026-09-11] ORIG 实有函数局部 static
    // `_ZZN21DB_TimerStatisticsLog8dispatchEiiP6StreamE5count`
    // （.bss 0x0943c228 size 1；全 ELF objdump 无任何引用点=死变量，原码遗留）
    // ⇒ 恢复声明以对齐符号集（未使用，零指令/零语义影响）。
    static char count;

    GetInstanceClientCrashDownLogStatistic()->CarryClientCrashDown();
    return 1;
}

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

// ============================================================================
// DB_UpdateCharacPlayTime::dispatch —— ORIG 0x0843f75a（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x843f75a：charac_no=0/play_time=0 清零 → *stream >> charac_no(int) >>
// play_time(int) → GetDBHandle(DB_HANDLE_2,0) → upDate charac_stat set
// total_play_time=total_play_time + %d where charac_no = %d（%d=play_time、
// charac_no，与 SQL 占位序一致）→ exec(true)（返回值丢弃）→ 恒返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_158.inc"
                                       Stream* stream)
{
    MySQL* db;                 // -0xc（ORIG 最高槽，声明序在前）
    int characNo = 0;          // -0x10（SQL where charac_no=%d，第一读）
    int playTime = 0;          // -0x14（SQL +%d 增量，第二读）
    *stream >> characNo;
    *stream >> playTime;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                           E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_stat set total_play_time=total_play_time + %d where charac_no = %d",
        playTime, characNo);
    db->exec(true);
    return 1;
}

// ============================================================================
// DB_InsertAttendanceEvent::dispatch —— ORIG 0x08448a48（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8448a48：GetDBHandle(DB_HANDLE_9,0) → *stream >> m_id(uint) >>
// charac_no(uint) → inSert into event_1207_attendance(m_id, charac_no,
// occ_date) \t\t\t\t  values(%s, %u, now())（%s=NumberToString(m_id,0)，
// %u=charac_no；charac_no 先缓存 ebx 再求 NumberToString，P24 内联实参）→
// 返回 exec(true)==1。
// ============================================================================
#include "W8LT_DBTask/dispatch_159.inc"
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;       // -0x10（SQL %s m_id，第一读）
    unsigned int characNo = 0;  // -0x14（SQL %u charac_no，第二读）
    *stream >> mId;
    *stream >> characNo;
    db->set_query(
        "inSert into event_1207_attendance(m_id, charac_no, occ_date) \t\t\t\t  values(%s, %u, now())",
        NumberToString(mId, 0), characNo);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_InsertDungeonEntrance::dispatch —— ORIG 0x08429696（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8429696：GetDBHandle(DB_HANDLE_4,0) → count=0 → *stream >> count →
// occDate=GlobalData::s_systemTime_.getCurSec() → channelNo=
// G_CEnvironment()->get_channel_no()（两者循环前缓存）→ dungeon_index/
// enter_count/exercise_count=0 → buf[0x400] 内联清零（P4）→ string ctor →
// for (i=0; i<count; ++i)：解包三 int → string::size()==0 决定前缀
// "(%d,from_unixtime(%d),%d,%d,%d)" / ",(...)" → sprintf → string += buf。
// 循环后 set_query("inSert into log_dungeon_entrance(channel_no,occ_date,
// dungeon_index,enter_count, exercise_count) values%s", sql.c_str()) →
// 返回 exec(true)==1（string dtor 由编译器生成，含异常清理路径）。
// ============================================================================
#include "W8LT_DBTask/dispatch_160.inc"
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int count = 0;              // -0x2c（循环次数，第一读）
    *stream >> count;
    int occDate = GlobalData::s_systemTime_.getCurSec();    // -0x24
    int channelNo = G_CEnvironment()->get_channel_no();     // -0x20
    int dungeonIndex = 0;       // -0x30
    int enterCount = 0;         // -0x34
    int exerciseCount = 0;      // -0x38
    // ORIG 0x08429696 执行序 = sql ctor(`lea -0x3c; call`) 在 rep stos 前；
    // 4.4 对 `{init}` 对象按声明逆序发射初始化 ⇒ buf 声明前置。
    char buf[0x400] = { '\0' }; // -0x43c..-0x3c（P4 rep stos 内联清零）
    std::string sql;            // -0x3c
    for (int i = 0; i < count; ++i)
    {
        *stream >> dungeonIndex;
        *stream >> enterCount;
        *stream >> exerciseCount;
        if (sql.size() != 0)
            sprintf(buf, ",(%d,from_unixtime(%d),%d,%d,%d)", channelNo,
                    occDate, dungeonIndex, enterCount, exerciseCount);
        else
            sprintf(buf, "(%d,from_unixtime(%d),%d,%d,%d)", channelNo,
                    occDate, dungeonIndex, enterCount, exerciseCount);
        sql += buf;
    }
    db->set_query(
        "inSert into log_dungeon_entrance(channel_no,occ_date,dungeon_index,enter_count, exercise_count) values%s",
        sql.c_str());
    // ORIG 0x08429696：尾 bool 物化双常数岛（xor$1;test;je + mov$0/mov$1
    // 入 %ebx）→ string dtor → 出口；catch 块置 dtor 前（4.1 EH 布局）。
    // 直判返形先例：DB_InsertPVPMap/DB_InsertDungeonExp 闭合。
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_UpdateBloodBestRecord::dispatch —— ORIG 0x08431c74（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8431c74：GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<
// SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD>（+0=charac_no/+4=best_time/
// +8=best_round/+0xc=dungeon_index）→ upDate charac_blood_best_record set
// best_round=%d, best_time=%u where charac_no=%u and dungeon_index=%d；
// exec 失败（!=1）或成功且 getAffectedRowCount()==0 → 补 inSert
// charac_blood_best_record(charac_no,dungeon_index,best_round,best_time)
// values(%u,%d,%d,%u)，返回 insert exec==1；update 成功影响行 → 返 1。
// ============================================================================
// [b28c-31] P0 清绑（原 register bool needInsert asm("eax") 已删）；残差
// N78+N18a：needInsert 汇聚 ORIG %eax vs 4.4 %ebx，且 `or %edx,%eax; test`
// 直分支 ORIG vs 4.4 setne 物化（0x08431d0a/0x08431d1a 实证）。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08431c74
#include "W8LT_DBTask/dispatch_161.inc"
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    DB_BloodBestRecordView* sig = reinterpret_cast<DB_BloodBestRecordView*>(
        stream->GetOutBuffer<SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD>());
    db->set_query(
        "upDate charac_blood_best_record set best_round=%d, best_time=%u where charac_no=%u and dungeon_index=%d",
        sig->m_bestRound, sig->m_bestTime, sig->m_characNo,
        sig->m_dungeonIndex);
    // ORIG 0x08431c74：needInsert 无槽——4.1 以 %eax 物化双常数岛
    // `mov $1; jmp T / mov $0 / T: test al; je`（首段 xor$1;test;jne =
    // exec!=1 直落 true 路）。[b28d-solo] P226/P316 语句表达式零槽短路物化
    // （`if (({ A || B; }))`）复现该形；P0 合规（无 register 绑定、无具名局部）。
    if (({ db->exec(true) != 1 || db->getAffectedRowCount() == 0; }))
    {
        db->set_query(
            "inSert into charac_blood_best_record(charac_no,dungeon_index,best_round,best_time) values(%u,%d,%d,%u)",
            sig->m_characNo, sig->m_dungeonIndex, sig->m_bestRound,
            sig->m_bestTime);
        if (!db->exec(true))
            return 0;
        return 1;
    }
    return 1;
}

// ============================================================================
// DB_UpdateBoosterGageData::dispatch —— ORIG 0x08446eec（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8446eec：GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<
// SIG_BOOSTER_GAGE_DATA>（+0=m_dwAccountNo/+4=m_dwBoosterGage）→ upDate
// member_booster_gage set gage=%u where m_id=%u；exec 失败返 0；成功且
// getAffectedRowCount()==0 → 补 inSert member_booster_gage(m_id, gage)
// values(%u, %u)，insert exec 失败返 0；成功（或 update 影响行）返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_162.inc"
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    DB_BoosterGageView* sig = reinterpret_cast<DB_BoosterGageView*>(
        stream->GetOutBuffer<SIG_BOOSTER_GAGE_DATA>());
    db->set_query("upDate member_booster_gage set gage=%u where m_id=%u",
                  sig->m_dwBoosterGage, sig->m_dwAccountNo);
    // ORIG +0x6d `xor $1;test;je body; mov $0;jmp end` = P16/P59 early-return
    // `if (!exec) return 0;`（两次 exec 均此形），非 if-else 嵌套。
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into member_booster_gage(m_id, gage) values(%u, %u)",
            sig->m_dwAccountNo, sig->m_dwBoosterGage);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateSchoolPoint::makeRequest —— ORIG 0x0842f0f0（报告 makeRequest.md 全文）
// ============================================================================
// ORIG 0x842f0f0：Acquire("DBThread.cpp",0x7425) → CStreamGuard(stream,true)
//   → <<0x129(int) → <<flag(int) → point>100 时钳制 100 并
//   logFormat(1,"DBThread.cpp",__PRETTY_FUNCTION__,0x742d,
//   "School event point error (m_id: %s)(point: %d)",NumberToString(mId,0),100)
//   → sig=GetInBuffer<SIG_UPDATE_SCHOOL_POINT>() → sig+0(uint)=mId、
//   sig+4(int)=point → put(DB_Q(2))。
// ============================================================================
#include "W8LT_DBTask/makeRequest_35.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7425), true);
    guard.operator*()->operator<<((int)0x129);
    guard.operator*()->operator<<((int)flag);
    if (point > 100)
    {
        point = 100;
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x742d, "School event point error (m_id: %s)(point: %d)",
            NumberToString(mId, 0), point);
    }
    SIG_UPDATE_SCHOOL_POINT* sig =
        guard->GetInBuffer<SIG_UPDATE_SCHOOL_POINT>();
    sig->m_mId = mId;
    sig->m_point = point;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_UpdateSchoolPoint::dispatch —— ORIG 0x0842f05e（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x842f05e：GetDBHandle(DB_HANDLE_6,0) → GetOutBuffer<
// SIG_UPDATE_SCHOOL_POINT>（+0 m_id / +4 point）→ upDate
// event_school_point set point=%d+point where m_id=%s（%d=point、
// %s=NumberToString(m_id,0)，P24 内联实参）→ 返回 exec(true)==1。
// ============================================================================
#include "W8LT_DBTask/dispatch_163.inc"
                                    Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_UPDATE_SCHOOL_POINT* sig =
        stream->GetOutBuffer<SIG_UPDATE_SCHOOL_POINT>();
    db->set_query(
        "upDate event_school_point set point=%d+point where m_id=%s",
        sig->m_point, NumberToString(sig->m_mId, 0));
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_GoblinPadUpdateValidityTime::dispatch —— ORIG 0x0842f22a（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x842f22a：GetDBHandle(DB_HANDLE_6,0) → GetOutBuffer<
// SIG_GOBLIN_UPDATE_VALIDITYTIME>（+0 m_id / +4 flag）→ upDate
// member_mousepass set validity_time=unix_timestamp(now())+%d where m_id=%s
// （%d=flag、%s=NumberToString(m_id,0)）→ exec(true)（返回值丢弃）→ 恒返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_164.inc"
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_GOBLIN_UPDATE_VALIDITYTIME* sig =
        stream->GetOutBuffer<SIG_GOBLIN_UPDATE_VALIDITYTIME>();
    db->set_query(
        "upDate member_mousepass set validity_time=unix_timestamp(now())+%d where m_id=%s",
        sig->m_flag,
        NumberToString(sig->m_mId, 0));
    db->exec(true);
    return 1;
}

// ============================================================================
// DB_SecuServiceReward::makeRequest —— ORIG 0x08437844（报告 makeRequest.md 全文）
// ============================================================================
// ORIG 0x8437844：Acquire("DBThread.cpp",0x847c) → CStreamGuard(stream,true)
//   → <<0x19d(int) → <<-1(int) → sig=GetInBuffer<SIG_SECU_SERVICE_REWARD>()
//   → sig+0(uint)=accId、sig+4(char)=flag → put(DB_Q(2))。
// ============================================================================
#include "W8LT_DBTask/makeRequest_36.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x847c), true);
    guard.operator*()->operator<<((int)0x19d);
    guard.operator*()->operator<<((int)-1);
    SIG_SECU_SERVICE_REWARD* sig =
        guard->GetInBuffer<SIG_SECU_SERVICE_REWARD>();
    sig->m_mId = accId;
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_SecuServiceReward::dispatch —— ORIG 0x08437764（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8437764：GetDBHandle(DB_HANDLE_1,0) → GetOutBuffer<
// SIG_SECU_SERVICE_REWARD>（+0 m_id）→ upDate member_security_grade set
// last_check_time=now() where m_id=%s（%s=NumberToString(m_id,0)）→
// exec(true)==1 且 getAffectedRowCount()!=0 → 返 1；否则
// cMyTrace(0x8461,0)("DB_SecuServiceReward Update Error") → 返 0。
// ============================================================================
#include "W8LT_DBTask/dispatch_165.inc"
                                    Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECU_SERVICE_REWARD* sig =
        stream->GetOutBuffer<SIG_SECU_SERVICE_REWARD>();
    db->set_query(
        "upDate member_security_grade set last_check_time=now() where m_id=%s",
        NumberToString(sig->m_mId, 0));
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x8461, 0);
        trace("DB_SecuServiceReward Update Error");
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateRevengeData::makeRequest —— ORIG 0x0843f232（报告 makeRequest.md 全文）
// ============================================================================
// ORIG 0x843f232：Acquire("DBThread.cpp",0x98bd) → CStreamGuard(stream,true)
//   → <<0x244(int) → <<-1(int) → <<characNo(uint) → <<attack(uchar 零扩)
//   → <<revenge(uchar 零扩) → put(DB_Q(2))。
// ============================================================================
#include "W8LT_DBTask/makeRequest_37.inc"
                                       unsigned char attack,
                                       unsigned char revenge)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x98bd), true);
    guard.operator*()->operator<<((int)0x244);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)characNo);
    guard.operator*()->operator<<(attack);
    guard.operator*()->operator<<(revenge);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_UpdateRevengeData::dispatch —— ORIG 0x0843f34c（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x843f34c：解包 characNo(uint)/attack(uchar)/revenge(uchar)；
// characNo==0 返 0。getCurSec → localtime；tm_hour 早于
// G_CEnvironment+0x37c（m_serverEnvir.m_fatigueTime）则回退 86400 秒重取；
// mktime 结果弃用（P31 死局部）。date[12] 清零 0xc 后 sprintf
// "%04d-%02d-%02d"（tm_year+0x76c/tm_mon+1/tm_mday）。GetDBHandle
// (DB_HANDLE_3,0)，null 返 0。upDate village_attack_dungeon set
// attack_count=%u, revenge_dungeon=%u where occ_date='%s' and charac_no=%u；
// exec 失败 cMyTrace(0x98fc,5)("village_attack_dungeon upDate exec fail") 返 0；
// getAffectedRowCount()==0 时补 inSert（occ_date,charac_no,attack_count,
// revenge_dungeon），insert exec 失败 cMyTrace(0x9907,5)
// ("event_integration_charac inSert exec fail") 返 0；全绿返 1。
// ============================================================================
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

// ============================================================================
// DB_TOD_Reset_UserAPC::GetAPCUserInfo —— ORIG 0x08440548（报告 GetAPCUserInfo.md 全文）
// ============================================================================
// ORIG 0x8440548：三句柄 db2(DB_HANDLE_2)/db8(DB_HANDLE_8)/db3(DB_HANDLE_3)。
// charac_info 查 5 列（exec 调两次，首次弃用）；exec||fetch 失败返 0；
// get_n_rows!=1 返 0；get_str(0,name,0x1e)/get_byte(1..3,lv/job/growType)/
// get_uint(4,guildId) 任一失败返 0。guildId!=0 时 guild_info 查 guild_name
// （失败或 get_str 失败 cMyTrace 0x9f46/0x9f4e 但不返 0）。user_items
// （expire_date>now() and stat=0 limit 115）读 slot/it_id/clear_avatar_id
// 三元组（fetch 失败 break、get_int 失败返 0）；slot∈[0,9] 写
// m_abilities[slot]=it_id；clear_avatar_id!=0 二次查 it_id 覆盖。equipslot
// blob → get_binary(0,0x37a) → uncompress_zip 到 items+0x262 → +0x264 dword
// 写 m_abilities[10]；creature_items(slot=238) 读 it_id/name 写
// m_creatureId/m_creatureName（exec 失败 cMyTrace 0x9fbf）。成功返 1。
// ============================================================================
// [SweepW8LT b28d] DIFF：gap 14→3（our 644 / ORIG 647）。本波修 P0 具名列索引：
// ORIG 0x0844062x 用 `int col=0` 逐列 `col++`（`mov -0x38,%eax; addl $1,-0x38`），
// 原码直传立即数 0..4 ⇒ 5 处各少 2 条。p167 61/61 [0缺0多]（cMyTrace C1/C2、
// Inven_Item C1/C2 同址别名）。余 -3 归 N82 帧-槽差。ORIG 08440548
#include "W8LT_DBTask/GetAPCUserInfo.inc"
{
    static const int handleIdx = 3;
    MySQL* db2 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                   E_SERVER_GROUP_NONE);
    MySQL* db8 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                   E_SERVER_GROUP_NONE);
    MySQL* db3 = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                   E_SERVER_GROUP_NONE);
    db2->set_query(
        "seLect charac_name,lev,job,grow_type,guild_id from charac_info where charac_no=%d",
        info.m_userCharacNo);
    db2->exec(true);
    if (!db2->exec(true) || !db2->fetch())
        return 0;
    if (db2->get_n_rows() != 1)
        return 0;
    // ORIG 0x0844062x 用具名列索引：`movl $0x0,-0x38; mov -0x38,%eax;
    // addl $0x1,-0x38; mov %eax,0x4(%esp)`（每处 2 条，共 5 处）。
    int col = 0;
    if (!db2->get_str(col++, info.m_userCharacName, 0x1e))
        return 0;
    if (!db2->get_byte(col++, info.m_lv))
        return 0;
    if (!db2->get_byte(col++, info.m_job))
        return 0;
    if (!db2->get_byte(col++, info.m_growType))
        return 0;
    unsigned int& guildId = info.m_guildId;
    if (!db2->get_uint(col++, guildId))
        return 0;
    if (guildId != 0)
    {
        int serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
        db8->set_query("seLect guild_name from guild_info where guild_id=%d",
                       guildId);
        if (!db8->exec(true) || !db8->fetch())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f46, 5);
            trace("TOD:select guild db fail, guild_id:%d, server_id:%d",
                  guildId, serverId);
        }
        else if (db8->get_n_rows() != 0 &&
                 !db8->get_str(0, info.m_guildName, 0x16))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f4e, 5);
            trace("TOD:select guild db fail, guild_id:%d, server_id:%d",
                  guildId, serverId);
        }
    }
    int triples[0x73][3];   // slot/it_id/clear_avatar_id × 115（0x564 字节）
    memset(triples, 0, sizeof(triples));
    db3->set_query(
        "seLect slot,it_id,clear_avatar_id from user_items where charac_no=%u and expire_date>now() and stat=0 limit %d",
        (unsigned int)info.m_userCharacNo, 0x73);
    if (!db3->exec(true))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x9f5d, 5);
        trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, exec() ERROR charac_no=%u",
              (unsigned int)info.m_userCharacNo);
        return 0;
    }
    int nRows = db3->get_n_rows();
    for (int i = 0; i < nRows; ++i)
    {
        if (!db3->fetch())
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f67, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, exec() ERROR charac_no=%u",
                  (unsigned int)info.m_userCharacNo);
            break;
        }
        if (!db3->get_int(0, triples[i][0]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f6d, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(0) ERROR charac_no=%u",
                  (unsigned int)info.m_userCharacNo);
            return 0;
        }
        if (!db3->get_int(1, triples[i][1]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f73, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(1) ERROR charac_no=%u",
                  (unsigned int)info.m_userCharacNo);
            return 0;
        }
        if (!db3->get_int(2, triples[i][2]))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x9f79, 5);
            trace("DB_LoadPowerWarStatueInfo::GetAvatarInfo, get_int(2) ERROR charac_no=%u",
                  (unsigned int)info.m_userCharacNo);
            return 0;
        }
        if (triples[i][0] >= 0 && triples[i][0] <= 9)
            info.m_abilities[triples[i][0]] = triples[i][1];
    }
    for (int j = 0; j < nRows; ++j)
    {
        if (triples[j][2] != 0)
        {
            db3->set_query("seLect it_id from user_items where ui_id=%d",
                           triples[j][2]);
            if (db3->exec(true) && db3->fetch())
                db3->get_int(0, info.m_abilities[triples[j][0]]);
        }
    }
    char buf[0x37a] = { '\0' };          // ORIG rep stos 清零（P4）
    unsigned int outLen = 0x37a;
    db3->set_query("seLect equipslot from inventory where charac_no=%u",
                   (unsigned int)info.m_userCharacNo);
    db3->exec(true);
    db3->fetch();
    int binLen = db3->get_binary_length(0);
    if (db3->get_binary(0, buf, 0x37a))
    {
        Inven_Item items[22];            // ORIG 构造 0x15→-1 共 22 次
        if (uncompress_zip((char*)&items[10], outLen, buf,
                           (unsigned int)binLen))
        {
            info.m_abilities[10] =
                reinterpret_cast<DB_EquipslotBlobView*>(&items[10])->m_dword2;
            db3->set_query(
                "seLect it_id,name from creature_items where slot=238 and charac_no=%u and expire_date > now()",
                (unsigned int)info.m_userCharacNo);
            if (!db3->exec(true))
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x9fbf, 5);
                trace("TOD: read DB creature_items error");
            }
            else if (db3->fetch())
            {
                db3->get_int(0, info.m_creatureId);
                db3->get_str(1, info.m_creatureName, 0xc);
            }
            return 1;
        }
        return 0;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x9f9d, 5);
    trace("DB_LoadInventory::GetUserInven, get_binary(3) ERROR charac_no=%u",
          (unsigned int)info.m_userCharacNo);
    return 0;
}

// ============================================================================
// DB_TOD_Reset_UserAPC::dispatch —— ORIG 0x084400c8（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x84400c8：GetDBHandle(DB_HANDLE_2,0) → set_query("seLect charac_no
// from charac_tower_despair_apc limit 10")；exec 失败 cMyTrace(0x9ea7,5)
// ("TOD : load apc from DB") 返 0。Acquire("DBThread.cpp",0x9eab) →
// CStreamGuard(stream,true) → <<userIdx → <<slot → sig=GetInBuffer<
// SIG_TOD_RELOADED_USER_APC>() → memset 0x758。nRows=get_n_rows()；
// !=10 → cMyTrace(0x9eb5,5)("TOD : get_n_rows() %d") 返 0。循环1
// （i<nRows && i<10）：fetch()!=1 → cMyTrace(0x9ebd,5)
// ("TOD : db->fetch(), int i=%d") 返 0；get_int(0, sig+i*0xbc)。
// 循环2 同界：sig[i].m_userCharacNo!=0 → GetAPCUserInfo(sig[i])。
// put(DISPATCH_Q(1),guard) 返 1（guard 所有出口自动析构）。
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084400c8
#include "W8LT_DBTask/dispatch_167.inc"
{
    static const int handleIdx = 2;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("seLect charac_no from charac_tower_despair_apc limit 10");
    if (db->exec(true) != 1)
    {
        // 技巧3：三处 trace 全改临时链式（具名块对象致 4.4 把 trace 槽
        // 外置，ORIG 0x084400c8 区 trace=-0x4c 比 guard=-0x54 近 8B；
        // 先例 DB_LoadPowerWarStatueInfo::dispatch 24→3）。
        cMyTrace(__PRETTY_FUNCTION__, 0x9ea7, 5)("TOD : load apc from DB");
        return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9eab), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    SIG_TOD_RELOADED_USER_APC* sig =
        guard->GetInBuffer<SIG_TOD_RELOADED_USER_APC>();
    memset(sig, 0, 0x758);
    // 行视图：SIG_TOD_RELOADED_USER_APC = _APCInfo[10]（行步长 0xbc =
    // sizeof(_APCInfo)，10×0xbc=0x758 与 memset/ORIG `+i*0xbc` 寻址实证
    // 0x084400c8 一致）。
    static_assert(sizeof(_APCInfo) == 0xbc, "TOD APC row stride drift");
    // 技巧36：删具名 apcs 指针（ORIG 站点 `mov -0x18,%eax`+imul 直算，
    // 无第二槽）。
    int nRows = db->get_n_rows();
    if (nRows != 10)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x9eb5, 5)("TOD : get_n_rows() %d",
                                                 nRows);
        return 0;
    }
    for (int i = 0; i < nRows && i < 10; ++i)
    {
        if (db->fetch() != 1)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x9ebd, 5)("TOD : db->fetch(), int i=%d",
                                                     i);
            return 0;
        }
        db->get_int(0, reinterpret_cast<_APCInfo*>(sig)[i].m_userCharacNo);
    }
    for (int i = 0; i < nRows && i < 10; ++i)
    {
        if (reinterpret_cast<_APCInfo*>(sig)[i].m_userCharacNo != 0)
            GetAPCUserInfo(reinterpret_cast<_APCInfo*>(sig)[i]);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_SaveInformNotice::dispatch —— ORIG 0x08446fcc（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_9,0) → GetOutBuffer<InformNoticeFlagData>（+0x00
// m_id、+0x04 inform_notice 字符串）→ char buf[256]={'\0'}（ORIG rep stos）
// → escape_string(buf, sig+4) → set_query("upDate login_common set
// inform_notice = '%s' where m_id = %s", buf, NumberToString(m_id,0))；
// exec(true) 失败返 0；getAffectedRowCount()==0 → set_query("inSert into
// login_common(m_id, inform_notice) values(%s, '%s')", midStr, buf)，
// exec 失败返 0；全绿返 1。无基类 dispatch 前置检查。
// ============================================================================
#include "W8LT_DBTask/dispatch_168.inc"
{
    // 执行序照 ORIG：GetDBHandle → GetOutBuffer → rep stos(buf 清零)。
    // buf 为最大局部（256B），gcc 置于帧底（-0x110），声明后置不影响槽位。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    // P89：单一视图表达式，避免 raw/sig 双局部槽（ORIG 仅一个 sig 槽 -0xc）。
    DB_InformNoticeView* sig = reinterpret_cast<DB_InformNoticeView*>(
        stream->GetOutBuffer<InformNoticeFlagData>());
    char buf[256] = { '\0' };   // -0x110（ORIG rep stos 清零，P4）
    db->escape_string(buf, sig->m_informNotice);
    db->set_query("upDate login_common set inform_notice = '%s' where m_id = %s",
                  buf, NumberToString(sig->m_mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into login_common(m_id, inform_notice) values(%s, '%s')",
            NumberToString(sig->m_mId, 0), buf);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_AvatarColorChange::dispatch —— ORIG 0x08446944（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_AVATAR_COLOR_CHANGE>（+0x00 charac_no、+0x04 ui_id、
// +0x08 color1 ushort、+0x0a color2 ushort）→ GetDBHandle(DB_HANDLE_3,0) →
// set_query("upDate user_items set color1=%d, color2=%d where ui_id=%d and
// charac_no=%d", color1, color2, ui_id, charac_no)；exec(true)!=1 →
// cMyTrace(__PRETTY_FUNCTION__,0xae70,5)("AVATAR_COLOR_CHANGE::DB_UPDATE_FAIL")
// 返 0；成功返 1。无基类 dispatch 前置检查、无回包。
// ============================================================================
#include "W8LT_DBTask/dispatch_169.inc"
{
    // P89：单一视图表达式避免双局部槽（ORIG 仅 sig(-0x10) 槽）。
    DB_AvatarColorChangeView* sig =
        reinterpret_cast<DB_AvatarColorChangeView*>(
            stream->GetOutBuffer<SIG_AVATAR_COLOR_CHANGE>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set color1=%d, color2=%d where ui_id=%d and charac_no=%d",
        (unsigned int)sig->m_color1, (unsigned int)sig->m_color2,
        sig->m_uiId, sig->m_characNo);
    if (db->exec(true) != 1)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xae70, 5);
        trace("AVATAR_COLOR_CHANGE::DB_UPDATE_FAIL");
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_SaveMercenaryData::dispatch —— ORIG 0x084458c6（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_MERCENARY_DB_DATA>；空返 0；+0x00 count 越界（<1 或
// >0x1e）返 0。GetDBHandle(DB_HANDLE_2,0) → 循环 count 次：行步长 0x54，
// +0x04 charac_no、+0x2c mercenary_start_time、+0x30 mercenary_finish_time、
// +0x34 area(char)、+0x35 period(char)。set_query("upDate charac_link_bonus
// set mercenary_start_time=%d,mercenary_finish_time=%d,mercenary_area=%d,
// mercenary_period=%d where charac_no=%u", start, finish, area, period,
// charac_no)；exec!=1 → 改 set_query("inSert into charac_info(...,) values(
// %d,%d,%d,%d) where charac_no=%u" 同 5 参)，exec!=1 → 返 0；循环完返 1。
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084458c6
#include "W8LT_DBTask/dispatch_170.inc"
                                    Stream* stream)
{
    // P89：单一视图表达式避免 raw/sig 双局部槽（ORIG 仅 sig(-0x24) 槽）。
    DB_MercenaryDataView* sig = reinterpret_cast<DB_MercenaryDataView*>(
        stream->GetOutBuffer<SIG_MERCENARY_DB_DATA>());
    if (sig == 0)
        return 0;
    if (sig->m_count < 1 || sig->m_count > 0x1e)
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    for (int i = 0; i < sig->m_count; ++i)
    {
        db->set_query(
            "upDate charac_link_bonus set mercenary_start_time=%d,mercenary_finish_time=%d,mercenary_area=%d,mercenary_period=%d where charac_no=%u",
            sig->m_rows[i].m_startTime,
            sig->m_rows[i].m_areaPeriod.m_finishTime,
            (int)sig->m_rows[i].m_areaPeriod.m_area,
            (int)sig->m_rows[i].m_areaPeriod.m_period,
            sig->m_rows[i].m_characNo);
        if (db->exec(true) != 1)
        {
            db->set_query(
                "inSert into charac_info(mercenary_start_time,mercenary_finish_time,mercenary_area,mercenary_period) values(%d,%d,%d,%d) where charac_no=%u",
                sig->m_rows[i].m_startTime,
                sig->m_rows[i].m_areaPeriod.m_finishTime,
                (int)sig->m_rows[i].m_areaPeriod.m_area,
                (int)sig->m_rows[i].m_areaPeriod.m_period,
                sig->m_rows[i].m_characNo);
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_SaveObjectBringUp::dispatch —— ORIG 0x08446a1e（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<SIG_OBJECT_BRING_UP>（+0x00
// m_accId、+0x04 m_characNo、+0x0c m_usedMaterialCnt、+0x0d m_rewardFlag）→
// G_CDataManager()->get_event_script_mng()->get_db_table_name(0x19b) 构造
// std::string → toTString 取表名。m_rewardFlag==0：upDate %s set
// use_material_cnt=use_material_cnt+%u where charac_no=%u and m_id=%s；
// 失败/affected==0 补 inSert into %s(charac_no,m_id,use_material_cnt)
// values(%u,%s,%u)；insert 失败 cMyTrace(0xaece,5)(...count(%u)) 返 0。
// m_rewardFlag!=0：upDate 追加 give_reward_flag=1, occ_date = now()；
// affected==0 补 inSert ...(charac_no,m_id,use_material_cnt,
// give_reward_flag,occ_date) values(%u,%s,%u,%u,now())；insert 失败
// cMyTrace(0xaebd,5)(...count(%u),flag(%u)) 返 0。全绿返 1。
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08446a1e
#include "W8LT_DBTask/dispatch_171.inc"
                                    Stream* stream)
{
    // 执行序照 ORIG：GetDBHandle → GetOutBuffer → get_db_table_name 链构造
    // std::string（链式内联，无 sm 局部槽）。表名每次 set_query 前 toTString
    // 即时求值（ORIG 无 CSE）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_OBJECT_BRING_UP* sig = stream->GetOutBuffer<SIG_OBJECT_BRING_UP>();
    std::string tableStr(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_db_table_name(0x19b));
    // ORIG 0x08446a1e 段 `movzbl 0xd(%eax),%eax; test %al,%al; je <flag==0 块>`
    // ⇒ flag!=0 块直落、flag==0 块在后（4.4 then 恒前置）⇒ 交换块并取反
    // （同 DB_GoblinPadUpdateReward / DB_UpdateQuest 的 P222 正向包裹形）。
    if (sig->m_rewardFlag != 0)
    {
        db->set_query(
            "upDate %s set use_material_cnt=use_material_cnt+%u, give_reward_flag=1, occ_date = now() where charac_no=%u and m_id=%s",
            toTString(tableStr), (unsigned int)sig->m_usedMaterialCnt,
            sig->m_characNo, NumberToString(sig->m_accId, 0));
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into %s(charac_no,m_id,use_material_cnt,give_reward_flag,occ_date) values(%u,%s,%u,%u,now())",
                toTString(tableStr), sig->m_characNo,
                NumberToString(sig->m_accId, 0),
                (unsigned int)sig->m_usedMaterialCnt,
                (unsigned int)sig->m_rewardFlag);
            if (db->exec(true) != 1)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0xaebd, 5);
                trace("upDate DB_SaveObjectBringUp::dispatch failed at charac_no(%u),count(%u),flag(%u)",
                      sig->m_characNo, (unsigned int)sig->m_usedMaterialCnt,
                      (unsigned int)sig->m_rewardFlag);
                return 0;
            }
        }
    }
    else
    {
        db->set_query(
            "upDate %s set use_material_cnt=use_material_cnt+%u where charac_no=%u and m_id=%s",
            toTString(tableStr), (unsigned int)sig->m_usedMaterialCnt,
            sig->m_characNo, NumberToString(sig->m_accId, 0));
        if (db->exec(true) != 1)
            return 0;
        if (db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into %s(charac_no,m_id,use_material_cnt) values(%u,%s,%u)",
                toTString(tableStr), sig->m_characNo,
                NumberToString(sig->m_accId, 0),
                (unsigned int)sig->m_usedMaterialCnt);
            if (db->exec(true) != 1)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0xaece, 5);
                trace("upDate DB_SaveObjectBringUp::dispatch failed at charac_no(%u),count(%u)",
                      sig->m_characNo, (unsigned int)sig->m_usedMaterialCnt);
                return 0;
            }
        }
    }
    return 1;
}

// ============================================================================
// DB_AvatarExtendPeriod::dispatch —— ORIG 0x083ffb22（报告 dispatch.md 全文）
// ============================================================================
// DBDispatcher::dispatch(userIdx, slot, stream) 失败返 0 → get_binary(7)
// 解包 SIG_AVATAR_RENEW 失败返 0 → ExtendPeriod(renew) 成功返 1，失败返 0。
// ============================================================================
#include "W8LT_DBTask/dispatch_172.inc"
{
    // P102：同一 bool 局部逐次保存结果（ORIG -0x9 槽：get_binary/ExtendPeriod
    // 结果落槽再 movzbl/xor/test；首检查 DBDispatcher::dispatch 直判不落槽）。
    SIG_AVATAR_RENEW renew;   // -0x10（7 字节，-0x10..-0xa）
    bool ok;                  // -0x9
    if (DBDispatcher::dispatch(userIdx, slot, stream) != 1)
        return 0;
    ok = stream->get_binary(&renew, (int)sizeof(SIG_AVATAR_RENEW));
    if (ok != 1)
        return 0;
    ok = ExtendPeriod(renew);
    if (ok != 1)
        return 0;
    return 1;
}

// ============================================================================
// DB_AvatarExtendPeriod::ExtendPeriod —— ORIG 0x083ffbb4（报告 ExtendPeriod.md）
// ============================================================================
// GetDBHandle(DB_HANDLE_3,0) → set_query("upDate user_items set expire_date =
// adddate(expire_date, interval %d day) where charac_no=%u and slot=%d",
// days(+0x05 ushort), charac_no(+0x00), slot(+0x04 byte))；exec(true) 失败
// 返 0，成功返 1（P101 分支形态）。
// ============================================================================
#include "W8LT_DBTask/ExtendPeriod.inc"
{
    // ORIG 栈槽：ok(-0xd) db(-0xc)——bool 先声明（低地址），db 后。
    bool ok;      // -0xd（exec 结果落槽，P102）
    MySQL* db;    // -0xc
    // [C-3/2026-09-11] ORIG 实有函数局部 static
    // `_ZZN21DB_AvatarExtendPeriod12ExtendPeriodER16SIG_AVATAR_RENEWE9handleIdx`
    // （.rodata 0x08c5da4c size 4，实测值 0x3；使用点 0x083ffbc7 折叠为
    //  `movl $0x3,0x4(%esp)`）⇒ 恢复声明（零 codegen 变化）。
    static const int handleIdx = 3;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                           E_SERVER_GROUP_NONE);
    // ORIG 0x83ffbb4：set_query("upDate user_items set expire_date =
    // adddate(expire_date, interval %d day) where charac_no=%u and slot=%d",
    // days, charac_no, slot) 后 exec(true)
    db->set_query(
        "upDate user_items set expire_date = adddate(expire_date, "
        "interval %d day) where charac_no=%u and slot=%d",
        renew.m_days, renew.m_characNo, renew.m_slot);
    ok = db->exec(true);
    if (ok != 1)
        return 0;
    return 1;
}

// ============================================================================
// DB_AvatarOptionChange::dispatch —— ORIG 0x0844687e（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_AVATAR_OPTION_CHANGE>（+0x00 ui_id、+0x04 charac_no byte、
// +0x08 ability_no）→ GetDBHandle(DB_HANDLE_3,0) → set_query("upDate
// user_items set ability_no=%d where ui_id=%d and charac_no=%d", charac_no,
// ui_id, ability_no)；exec(true)!=1 → cMyTrace(__PRETTY_FUNCTION__,0xae5c,5)
// ("AVATAR_OPTION_CHAGE::DB_UPDATE_FAIL") 返 0；成功返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_173.inc"
                                     Stream* stream)
{
    DB_AvatarOptionChangeView* sig =
        reinterpret_cast<DB_AvatarOptionChangeView*>(
            stream->GetOutBuffer<SIG_AVATAR_OPTION_CHANGE>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate user_items set ability_no=%d where ui_id=%d and charac_no=%d",
        (unsigned int)sig->m_characNo, sig->m_uiId, sig->m_abilityNo);
    if (db->exec(true) != 1)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xae5c, 5);
        trace("AVATAR_OPTION_CHAGE::DB_UPDATE_FAIL");
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_DeleteAccountCargo::dispatch —— ORIG 0x0843b24e（报告 dispatch.md 全文）
// ============================================================================
// *stream >> m_id(uint) → GetDBHandle(DB_HANDLE_2,0) → set_query("deLete from
// account_cargo where m_id = %s", NumberToString(m_id,0)) → 返 exec(true)==1。
// 无回包、无日志。
// ============================================================================
#include "W8LT_DBTask/dispatch_174.inc"
                                     Stream* stream)
{
    unsigned int mId = 0;
    MySQL* db;
    *stream >> mId;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("deLete from account_cargo where m_id = %s",
                  NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    return 1;
}

// ============================================================================
// DB_CreateAccountCargo::dispatch —— ORIG 0x0843ac52（报告 dispatch.md 全文）
// ============================================================================
// Acquire("DBThread.cpp",0x8bc7) → CStreamGuard → <<userIdx → <<slot →
// GetInBuffer<SIG_CREATE_ACCOUNT_CARGO_RESULT>() → memset 0x8 → *stream >>
// m_id(uint) >> capacity(uint) → GetDBHandle(DB_HANDLE_2,0) → set_query(
// "seLect * from account_cargo where m_id = %s", NumberToString(m_id,0))；
// exec 失败返 0。get_n_rows()!=0（已存在）：回包 +0x00=0x14 →
// put(DISPATCH_Q,guard) 返 1；否则 set_query("inSert into
// account_cargo(m_id, capacity, money, occ_time, cargo) values(%s, %d, 0,
// now(),'')", NumberToString(m_id,0), capacity)；exec 失败返 0；成功回包
// +0x00=0x0、+0x04=capacity → put(DISPATCH_Q,guard) 返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_175.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8bc7), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    DB_AccountCargoResultView* sig =
        reinterpret_cast<DB_AccountCargoResultView*>(
            guard.operator->()->GetInBuffer<SIG_CREATE_ACCOUNT_CARGO_RESULT>());
    MySQL* db;
    // ORIG 0x0843ac…：mId(-0x1c)/capacity(-0x20) 比 guard(this=-0x18) 远
    // ⇒ 声明置 guard 之后（4.4 后声明占远槽）；清零语句保持在 memset
    // 调用之后（ORIG 0x0843… memset 后 `movl $0,-0x1c/-0x20`）。
    unsigned int mId;
    unsigned int capacity;
    memset(sig, 0, 0x8);
    mId = 0;
    capacity = 0;
    *stream >> mId;
    *stream >> capacity;
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    db->set_query("seLect * from account_cargo where m_id = %s",
                  NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    if (db->get_n_rows() != 0)
    {
        sig->m_result = 0x14;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    db->set_query(
        "inSert into account_cargo(m_id, capacity, money, occ_time, cargo) values(%s, %d, 0, now(),'')",
        NumberToString(mId, 0), capacity);
    if (db->exec(true) != 1)
        return 0;
    sig->m_result = 0x0;
    sig->m_capacity = capacity;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_SaveRedeemItemList::dispatch —— ORIG 0x0840e074（报告 dispatch.md 全文）
// ============================================================================
// GetOutBuffer<SIG_REDEEMINFO_LIST_TO_DB>（+0x00 charac_no；压缩源全 0x59c）
// → buf[0x6c7] 清零 → outLen=0x6c7 → compress_zip(buf,&outLen,sig,0x59c)
// 失败返 0 → GetDBHandle(DB_HANDLE_3,0) → blob_to_str(0,buf,outLen) →
// set_query("upDate charac_inven_expand set redeem_info='%s' where
// charac_no=%u", blobStr, charac_no)；exec 失败返 0；
// getAffectedRowCount()==0 → blob_to_str 再编 → set_query("inSert into
// charac_inven_expand(charac_no, redeem_info) values(%u, '%s')",
// charac_no, blobStr)；exec 失败返 0；成功返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_176.inc"
                                     Stream* stream)
{
    static const int handleIdx = 3;
    DB_RedeemInfoView* sig;
    MySQL* db;
    sig = reinterpret_cast<DB_RedeemInfoView*>(
        stream->GetOutBuffer<SIG_REDEEMINFO_LIST_TO_DB>());
    char buf[0x6c7] = { '\0' };
    unsigned int outLen = 0x6c7;
    if (compress_zip(buf, outLen, (const char*)sig, 0x59c) != 1)
        return 0;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_inven_expand set redeem_info='%s' where charac_no=%u",
        db->blob_to_str(0, buf, (int)outLen), sig->m_characNo);
    if (db->exec(true) != 1)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_inven_expand(charac_no, redeem_info) values(%u, '%s')",
            sig->m_characNo, db->blob_to_str(0, buf, (int)outLen));
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_InsertEventStageLog::dispatch —— ORIG 0x08449144（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_9,0) → GetOutBuffer<SIG_EVENT_STAGE_LOG>（+0x00
// event_id、+0x04 m_id、+0x08 charac_no、+0x0c server_info、+0x10 stage、
// +0x14 stage_result）→ set_query("inSert into dnf_event_accept_list
// (event_id,m_id,charac_no,server_info,occ_date,stage,stage_result) values
// (%d,%s,%d,%d,now(),%d,'%s')", event_id, NumberToString(m_id,0), charac_no,
// server_info, stage, stage_result) → 返 exec(true)==1。
// ============================================================================
#include "W8LT_DBTask/dispatch_177.inc"
                                      Stream* stream)
{
    const char* stageResult;   // -0x2c（ORIG：GetOutBuffer 后 add $0x14 缓存）
    // 技巧31 dead 槽（ORIG 0x08449181(-0x2c) 与 db(-0x20) 间存在 4.1 遗留
    // 空洞 -0x24/-0x28；4.4 槽压紧需显式补位翻 ORIG 槽号）。
    unsigned int gapA;
    unsigned int gapB;
    MySQL* db;                 // -0x20
    DB_EventStageLogView* sig; // -0x1c
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    sig = reinterpret_cast<DB_EventStageLogView*>(
        stream->GetOutBuffer<SIG_EVENT_STAGE_LOG>());
    stageResult = sig->m_stageResult;
    (void)gapA;
    (void)gapB;
    db->set_query(
        "inSert into dnf_event_accept_list(event_id,m_id,charac_no,server_info,occ_date,stage,stage_result) values(%d,%s,%d,%d,now(),%d,'%s')",
        sig->m_eventId, NumberToString(sig->m_mId, 0), sig->m_characNo,
        sig->m_serverInfo, sig->m_stage, stageResult);
    if (db->exec(true) != 1)
        return 0;
    return 1;
}

// ============================================================================
// DBOnlyOrigWave6：10 个 DB_* dispatch/makeRequest 真实实现
// （报告 docs/class_func_reports/<类>/ 全文已读，逐指令对应；Acquire 行号常量
//   即 ORIG DBThread.cpp 内嵌行号）
// ============================================================================

// ORIG 0x0842acf4 sz 0x3b：GetOutBuffer<SIG_ARRANGE_USER_LOGIN_INFO>；
// sig+0x04(byte)==1 → s_psimpleSSO->expireLoginTime(*(uint*)sig+0)；返 1。
#include "W8LT_DBTask/dispatch_178.inc"
{
    SIG_ARRANGE_USER_LOGIN_INFO* sig =
        stream->GetOutBuffer<SIG_ARRANGE_USER_LOGIN_INFO>();
    if (sig->m_flag == 1)
        GlobalData::s_psimpleSSO->expireLoginTime(sig->m_accId);
    return 1;
}

// ORIG 0x084354da sz 0xef：GetDBHandle(1,0) → *stream>>mId(uint) →
// upDate member_security_grade set black_ip_try_time=now() where m_id=%s；
// exec 失败或 affected==0 → inSert ... values(%s, now())（exec 失败返 0）；返 1。
#include "W8LT_DBTask/dispatch_179.inc"
                                                  Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    // ORIG 0x84354fa：无 `movl $0x0,-0x10` —— 形参未经初始化直接 &mId >>。
    unsigned int mId;
    *stream >> mId;
    db->set_query(
        "upDate member_security_grade set black_ip_try_time=now() where m_id=%s",
        NumberToString(mId, 0));
    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into member_security_grade(m_id, black_ip_try_time) values(%s, now())",
            NumberToString(mId, 0));
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ORIG 0x0844c4e2：GetDBHandle(2,0)==0 返 0；GetOutBuffer 失败返 0；set_query
// cube_premium；exec 失败返 0；Acquire(0xbf71) → <<userIdx → <<slot →
// GetInBuffer<SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES> → +0=characNo、+4=0、
// +5=0 → fetch 有行时 get_ubyte(0,+4)/get_ubyte(1,+5)（失败返 0）→ put 返 1。
#include "W8LT_DBTask/dispatch_180.inc"
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    SIG_GET_PREMIUM_SELECTED_CUBE_INFO* sig =
        stream->GetOutBuffer<SIG_GET_PREMIUM_SELECTED_CUBE_INFO>();
    if (sig == 0)
        return 0;
    db->set_query("seLect selected, cube_type from cube_premium where charac_no=%u",
                  sig->m_characNo);
    // ORIG 0x0844c4e2：`mov %al,-0xd; movzbl -0xd,%eax; xor$1;test` ——
    // exec 结果经 bool 槽 -0xd spill+reload（batch-8 InformNotice 技巧，
    // 该 byte 槽同时把 db/sig/guard 整体外推 4B：ORIG db=-0x18、
    // sig=-0x14、guard=-0x20）。
    bool r;
    r = db->exec(true);
    if (!r)
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbf71), true);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((int)slot);
    SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES* out =
        guard->GetInBuffer<SIG_GET_PREMIUM_SELECTED_CUBE_INFO_RES>();
    out->m_characNo = sig->m_characNo;
    out->m_selected = 0;
    out->m_cubeType = 0;
    // ORIG 0x0844c…：三处 bool 槽 -0xd spill——fetch 正向 `cmpb $0;je`、
    // get_ubyte×2 `movzbl;xor$1;test;je`（技巧1/8 同槽复用）。
    r = db->fetch();
    if (r)
    {
        r = db->get_ubyte(0, out->m_selected);
        if (!r)
            return 0;
        r = db->get_ubyte(1, out->m_cubeType);
        if (!r)
            return 0;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ORIG 0x0843199a：DBDispatcher::dispatch 失败返 0；GetDBHandle(2,0)；
// *stream>>characNo；seLect best_round,best_time,dungeon_index ... 无数据
// trace(0x78e4,0)；有数据逐行 get_int/get_uint → Acquire(0x78f2) → <<0x13f →
// <<slot → SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD(+0 characNo/+4 best_time/
// +8 best_round/+c dungeon_index) → put(DISPATCH_Q)；返 1。
#include "W8LT_DBTask/dispatch_181.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    unsigned int bestTime = 0;
    int bestRound = 0;
    int dungeonIndex = 0;
    *stream >> characNo;
    db->set_query(
        "seLect best_round, best_time, dungeon_index from charac_blood_best_record where charac_no=%u",
        characNo);
    if (!db->exec(true))
        return 0;
    if (!(db->fetch() == 1 && db->get_n_rows() != 0))
    {
        // 技巧3：临时链式（具名块对象把 trace 槽外推至帧底 -0x34，4 ints
        // 反占近槽；ORIG 0x0843199a trace=-0x24 在 db=-0x14 与 ints
        // -0x28..-0x34 之间，先例 DB_LoadPowerWarStatueInfo::dispatch）。
        cMyTrace(__PRETTY_FUNCTION__, 0x78e4, 0)(
            "Not exist data charac_blood_best_record. [charac_no:%d]", characNo);
        return 1;
    }
    for (unsigned int i = 0; i < db->get_n_rows(); ++i)
    {
        if (!db->get_int(0, bestRound))
            return 0;
        if (!db->get_uint(1, bestTime))
            return 0;
        if (!db->get_int(2, dungeonIndex))
            return 0;
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x78f2), true);
        guard.operator*()->operator<<((int)0x13f);
        guard.operator*()->operator<<((int)slot);
        SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD* out =
            guard->GetInBuffer<SIG_LOAD_BLOOD_DUNGEON_BEST_RECORD>();
        out->m_characNo = characNo;
        out->m_bestTime = bestTime;
        out->m_bestRound = bestRound;
        out->m_dungeonIndex = dungeonIndex;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
    return 1;
}

// ORIG 0x0843a264：GetDBHandle(1,0)；GetOutBuffer<SIG_SECURITY_CARD_SERVICE_
// VALIDITYTIME>；+8(ushort)==8 → security_card_validity_time upDate；==0x10 →
// goblin_validity_time upDate（均含 validity_ip='%s' uid@+0xa）；exec(true) 返 1。
#include "W8LT_DBTask/dispatch_182.inc"
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_SERVICE_VALIDITYTIME* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_SERVICE_VALIDITYTIME>();
    if (sig->m_flag == 8)
    {
        db->set_query(
            "upDate member_security_grade set security_card_validity_time\t=unix_timestamp(now())+%d , validity_ip = '%s' where m_id=%s",
            sig->m_type,
            // ORIG +0x59 `lea 0xa(%edx),%ecx`：validity_ip 取 sig+10（数组
            // decay 取地址复现单条 lea；`+10` 指针算术会成 add+mov 两条）。
            (const char*)&((char*)sig)[10],
            NumberToString(sig->m_mId, 0));
    }
    else if (sig->m_flag == 0x10)
    {
        db->set_query(
            "upDate member_security_grade set goblin_validity_time\t\t=unix_timestamp(now())+%d , validity_ip = '%s' where m_id=%s",
            sig->m_type,
            (const char*)&((char*)sig)[10],
            NumberToString(sig->m_mId, 0));
    }
    db->exec(true);
    return 1;
}

// ORIG 0x0844d17c：*stream>>mId(uint)；GetDBHandle(6,0)；seLect m_id from
// allow_proxy_user where m_id=%s；exec 成功且 get_n_rows()==0 →
// Acquire(0xc2b7) → <<0x352 → <<slot → put(DISPATCH_Q)；返 1。
#include "W8LT_DBTask/dispatch_183.inc"
{
    unsigned int mId = 0;
    *stream >> mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect m_id from allow_proxy_user where m_id=%s",
                  NumberToString(mId, 0));
    // ORIG 0x844d17c：exec 短路 `test;je` 后，第二条件值物化
    // `test %eax,%eax; sete %al; test %al,%al; je`。4.4 在 && 中把物化消解成
    // jne+mov$1/mov$0 岛；拆嵌套 if 保 (bool) 物化形（技巧 9）。
    if (db->exec(true))
        if ((bool)(db->get_n_rows() == 0))
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc2b7),
                true);
            guard.operator*()->operator<<((int)0x352);
            guard.operator*()->operator<<((int)slot);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
    return 1;
}

// ORIG 0x08439c6c：GetDBHandle(4,0)；GetOutBuffer<SIG_UPDATE_COMPOUND_EMBLEM_
// STATISTIC>；inSert log_emblem_compound(DATE_SUB(CURDATE(),INTERVAL 1 DAY),
// channel_no,grade0..6)；exec 成功且 affected!=0 完成；否则 upDate grade 累加
// （exec 失败返 0）；返 1。
#include "W8LT_DBTask/dispatch_184.inc"
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4, E_SERVER_GROUP_NONE);
    SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC* sig =
        stream->GetOutBuffer<SIG_UPDATE_COMPOUND_EMBLEM_STATISTIC>();
    db->set_query(
        "inSert into log_emblem_compound(occ_date, channel_no, grade0, grade1, grade2, grade3, grade4, grade5, grade6) values(DATE_SUB(CURDATE(), INTERVAL 1 DAY), %d, %d, %d, %d, %d, %d, %d, %d)",
        sig->m_userIdx, sig->m_values[0], sig->m_values[1], sig->m_values[2],
        sig->m_values[3], sig->m_values[4], sig->m_values[5], sig->m_values[6]);
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "upDate log_emblem_compound set grade0 = grade0+%d, grade1=grade1+%d, grade2=grade2+%d, grade3=grade3+%d, grade4=grade4+%d, grade5=grade5+%d, grade6=grade6+%d where occ_date=DATE_SUB(CURDATE(), INTERVAL 1 DAY) and channel_no=%d",
            sig->m_values[0], sig->m_values[1], sig->m_values[2], sig->m_values[3],
            sig->m_values[4], sig->m_values[5], sig->m_values[6], sig->m_userIdx);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ORIG 0x084330d4 sz 0x449：GetDBHandle(6,0)；localtime(&g_IPMonitorLastDBAccessTime)
// sprintf "%d-%d-%d %d:%d:%d" → seLect ip,type,m_id_cnt,unix_timestamp(start_time),
// unix_timestamp(end_time) from ip_monitor_punish where start_time > '%s'；
// exec 失败或 n_rows==0 返 0；trace(0x7ac4,0)；逐行读入 0x20 行缓存（0xc80），
// 满 100 行或末行 → Acquire(0x7ae9) → <<0x156 → <<-1 → GetInBuffer<SIG_IP_
// MONITOR_PUNISH> → +0=rowCount+1 → memcpy +4..0xc83 → +0xc84=0 → put(1)；返 1。
// [SweepW8LT b28d] DIFF：gap 10→6（our 271 / ORIG 277）。本波修两处：
// ① P227 trace 改全表达式临时链（ORIG ctor 前 `call get_n_rows; mov %eax,%ebx`
//    跨 ctor 保活；具名对象使 4.4 在 ctor 后重调 get_n_rows，多一次 DB 调用）；
// ② P183 删 items 指针缓存，全表达式 `((DB_IpMonitorPunishItem*)buf)[rowCount]`
//    重算（ORIG 每处 `lea buf; shl $5; add`）。p167 31/31 [0缺0多]（C1/C2 别名）。
// 余 -6：localtime 地址装载 +1、timeBuf 清零 5 vs 6 条、N82 槽差。ORIG 084330d4
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
        if (rowCount <= 0x62 && i < db->get_n_rows() - 1)
        {
            ++rowCount;
        }
        else
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

// ORIG 0x0843b946 sz 0x132（_ZN23DB_SaveAccountCargo11makeRequestEjiP13CAccountCargo）：
// Acquire("DBThread.cpp",35999) → CStreamGuard(stream,true)
//   → <<0x1f1(int) → <<param_1(int) → <<param_2(uint)
//   → sig=GetInBuffer<SIG_ACCOUNT_CARGO_DATA>() → memset 0xd60
//   → sig+0x00(uint)=cargo->m_capacity(+0xd60)、+0x04(uint)=cargo->m_money(+0xd5c)
//   → memcpy(sig+0x08, cargo+0x04, 0xd58) → put(DB_Q(2))
//   → CAccountCargo::SetStable(cargo)。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0843b946
// [b28d-solo-4] §6.2 三件套：p167 调用点差集 CLEAR（ORIG 17 = OUR 17，0 missing/0 extra）
//   + gap −1（88/89）⇒ 语义完成；余 N104 EH 尾块相对序（正常路径 epilogue 就地内联、
//   catch/Unwind 块压最末 vs ORIG 独立尾块）⇒ 按 N104 封顶。
#include "W8LT_DBTask/makeRequest_38.inc"
                                      CAccountCargo* cargo)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c9f), true);
    guard.operator*()->operator<<((int)0x1f1);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)accId);
    // [Lane31W8LTDBTaskView] 权威头直取（去 cast；ORIG RHS-first）。
    SIG_ACCOUNT_CARGO_DATA* sig =
        guard->GetInBuffer<SIG_ACCOUNT_CARGO_DATA>();
    memset(sig, 0, 0xd60);
    sig->m_capacity = cargo->m_capacity;
    sig->m_money = cargo->m_money;
    memcpy(sig->m_cargo, &cargo->m_slots[0], 0xd58);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    cargo->SetStable();
}

// ORIG 0x0843eca4 sz 0xcb（_ZN24DB_UpdateItemCountValue11makeRequestEjj）：
// Acquire("DBThread.cpp",0x960e) → CStreamGuard(stream,true)
//   → <<0x240(int) → <<-1(int) → <<param_1(uint) → <<param_2(uint)
//   → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_39.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x960e), true);
    guard.operator*()->operator<<((int)0x240);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)param1);
    guard.operator*()->operator<<((unsigned int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x0844bee2 sz 0xaa（_ZN19DB_LoadPuUserCheck11makeRequestEji）：
// Acquire("DBThread.cpp",0xbcc2) → CStreamGuard(stream,true)
//   → <<0x2e1(int) → <<param_2(int) → <<param_1(uint)
//   → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_40.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbcc2), true);
    guard.operator*()->operator<<((int)0x2e1);
    guard.operator*()->operator<<((int)param2);
    guard.operator*()->operator<<((unsigned int)param1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
// ---------------------------------------------------------------------------
// 本批 12 个 ORIG-only DB_* dispatch/makeRequest 真实定义（报告
// docs/class_func_reports/DB_*/{dispatch,makeRequest}.md 全文已读，逐指令对应；
// Acquire 行号常量即 ORIG DBThread.cpp 内嵌行号）
// ---------------------------------------------------------------------------

// ORIG 0x08445eee（_ZN23DB_Check3rdPartyConcent8dispatchEiiP6Stream）：
// *stream>>mId(uint)；GetDBHandle(1,0)；seLect 3dparty_consent ... m_id=%s；
// exec 失败返 0；n_rows<1 → consent=0；否则 fetch 失败返 0、get_ubyte(0)
// 失败返 0；Acquire(0xad91) → <<0x2b7 → <<userIdx → <<consent(char) →
// put(DISPATCH_Q(1))；返 1。
#include "W8LT_DBTask/dispatch_186.inc"
{
    unsigned int mId = 0;
    *stream >> mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    db->set_query("seLect 3dparty_consent from member_join_info where m_id = %s",
                  NumberToString(mId, 0));
    if (db->exec(true) != 1)
        return 0;
    // ORIG 0x08445…：`mov %eax,-0xc`（get_n_rows 落槽）+ `movb $0,-0x15`
    // （consent 在其后声明占 -0x15）+ `cmpl $0,-0xc;jle`——具名槽形
    // （4.4 免槽直判会把整帧内推 4B 且 setg 折叠）。
    int nRows = db->get_n_rows();
    unsigned char consent = 0;
    if (nRows > 0)
    {
        if (db->fetch() != 1)
            return 0;
        if (db->get_ubyte(0, consent) != 1)
            return 0;
    }
    else
    {
        // ORIG 0x08445…：else 路径 `cmpl $0,-0xc;jne; movb $0,-0x15`
        // （==0 判定：jne 跳过清零）。
        if (nRows == 0)
            consent = 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xad91), true);
    guard.operator*()->operator<<((int)0x2b7);
    // ORIG 0x08445…：`mov 0x10(%ebp),%edx` = slot（成员函数 a2），非 userIdx。
    guard.operator*()->operator<<((int)slot);
    guard.operator*()->operator<<(consent);   // ORIG lsEh（unsigned char 1 字节，勿转 int）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ORIG 0x0842a9fc（_ZN23DB_CheckDoubleGuildName8dispatchEiiP6Stream）：
// GetDBHandle(8,0)；GetOutBuffer<SIG_DOUBLE_CHECK_GUILD_NAME>（guild_name
// 自 +0 起）；seLect * from guild_info where expire_flag=0 and guild_name='%s'；
// exec 失败返 0；Acquire(0x6c86) → <<0xeb → <<userIdx → n_rows==0 ? '\x01'
// : '\x02' → put(DISPATCH_Q(1))；返 1。
#include "W8LT_DBTask/dispatch_187.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    SIG_DOUBLE_CHECK_GUILD_NAME* sig =
        stream->GetOutBuffer<SIG_DOUBLE_CHECK_GUILD_NAME>();
    db->set_query(
        "seLect * from guild_info where expire_flag=0 and guild_name='%s'",
        (char*)sig);
    // ORIG 0x842aa5f-0x842aa6b：`xor $0x1,%eax; test; je <body>; mov $0,%ebx;
    // jmp end` —— bool exec 的 `!` 直判 + return 0 岛（技巧 1/6）；body 显式块
    // 保 %ebx 汇聚（技巧 12）。0x842aacd：第二 << 实参 = 0x10(%ebp) = **slot**
    // （成员函数 this=0x8，userIdx=0xc，slot=0x10）—— 非 userIdx。
    if (!db->exec(true))
        return 0;
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6c86), true);
        guard.operator*()->operator<<((int)0xeb);
        guard.operator*()->operator<<((int)slot);
        if (db->get_n_rows() == 0)
            guard.operator*()->operator<<((char)1);   // ORIG lsEc（char 1 字节，勿转 int）
        else
            guard.operator*()->operator<<((char)GUILD_NAME_DOUBLE_CHECK);  // ORIG lsEc
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
}

// ORIG 0x0842ab78（_ZN26DB_CheckDoubleGuildAddress8dispatchEiiP6Stream）：
// 同 DB_CheckDoubleGuildName 构型：GetDBHandle(8,0)；GetOutBuffer<SIG_DOUBLE_
// CHECK_GUILD_ADDRESS>（guild_url 自 +0 起）；seLect * from guild_info where
// guild_url='%s'；exec 失败返 0；Acquire(0x6c9f) → <<0xec → <<userIdx →
// n_rows==0 ? '\x01' : '\x02' → put(DISPATCH_Q(1))；返 1。
#include "W8LT_DBTask/dispatch_188.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    SIG_DOUBLE_CHECK_GUILD_ADDRESS* sig =
        stream->GetOutBuffer<SIG_DOUBLE_CHECK_GUILD_ADDRESS>();
    db->set_query("seLect * from guild_info where guild_url='%s'", (char*)sig);
    // 同 DB_CheckDoubleGuildName（ORIG 0x842ab78 同构）：`!exec` 直判 return 0
    // 岛 + body 显式块；第二 << 实参 = slot（0x10(%ebp)）。
    if (!db->exec(true))
        return 0;
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6c9f), true);
        guard.operator*()->operator<<((int)0xec);
        guard.operator*()->operator<<((int)slot);
        if (db->get_n_rows() == 0)
            guard.operator*()->operator<<((char)1);   // ORIG lsEc（char 版）
        else
            guard.operator*()->operator<<((char)2);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
}

// ORIG 0x0842ca06（_ZN22DB_InsertPVPPlayerRate8dispatchEiiP6Stream）：
// GetDBHandle(4,0)；本地 int[6] 清零；逐项 *stream>>；channel_no 取
// G_CEnvironment()->get_channel_no()；inSert log_pvp_player_rate(...)
// values(%d,now(),%d×6)；返 exec(true)==1。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0842ca06
#include "W8LT_DBTask/dispatch_189.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    // [SweepIntervalSet] ORIG 0x0842ca1x 的 24 字节清零是 4.1 对**数组零初始化**
    // 的展开（`lea; mov $0x18,%eax; and; mov $0,%eax; mov %ecx,(%edx,%eax,1);
    // add $4; cmp; jb` 循环形），而非 for 逐元素赋值（4.4 后者产 setle/test/jne）。
    int vals[6] = {0};
    for (int i = 0; i < 6; ++i)
        *stream >> vals[i];
    db->set_query(
        "inSert into log_pvp_player_rate(channel_no,occ_date,single_one_to_one,single_one_to_one_other,team_one_to_one,team_one_to_one_other,relay_one_to_one,relay_one_to_one_other) values(%d,now(),%d,%d,%d,%d,%d,%d)",
        // [Q5W8LT 2026-09-11 落地 P329 实参位内联] ORIG 0x0842ca06 无 channelNo 具名槽
        // （`call G_CEnvironment; call get_channel_no; mov %eax,0x8(%esp)`），且 v[0..5]
        // 在 call **之前**取出（v5→-0x44 / v4→-0x40 / v3→-0x3c / v2→%edi / v1→%esi /
        // v0→%ebx）⇒ channel_no 是实参位内联的**最后求值**表达式；删具名局部后
        // 本件 DIFF→**AE**（80=80 归一）。
        G_CEnvironment()->get_channel_no(), vals[0], vals[1], vals[2], vals[3],
        vals[4], vals[5]);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ORIG 0x0842945e（_ZN23DB_InsertValueStatistic8dispatchEiiP6Stream）：
// GetDBHandle(4,0)；GetOutBuffer<SIG_VALUE_STATISTIC>；按 +0x00..+0x68
// 28 个 uint（SQL 参数序 +0x00(occ_time from_unixtime) → +0x68 末列）；
// inSert log_value_stat(...28 列...)；返 exec(true)==1。
// [b28d-4] DIFF：N18a/P211：4.4 多 {'mov': 3} 中转 mov（单段直落/同型化待验）；ORIG 0842945e
#include "W8LT_DBTask/dispatch_190.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_VALUE_STATISTIC* sig = stream->GetOutBuffer<SIG_VALUE_STATISTIC>();
    // [H4 2026-09-11 D 组取证] ORIG 0x08429587 无 channelNo 具名槽（`call
    // G_CEnvironment; mov %eax,(%esp); call get_channel_no; mov %eax,0x8(%esp)`
    // ——值直落出参槽且**最后**求值，P329 实参位内联）；另 ORIG 只传 27 个 sig
    // 字段（装载偏移 +0x68→+0x00 ⇒ m_stats[0..26]），我方多传 m_stats[27]（+0x6c）。
    db->set_query(
        "inSert into log_value_stat(channel_no,occ_time ,level,uv,drop_gold,drop_item,result_card_gold,result_card_item,gold_card_item,store_item_buy,jar_item,disjoint_create,upgrade_faild_forced_disjoint,quest_reward,deathtower_card_gold,deathtower_card_item,consume_store_item_buy,consume_upgrade_attempt,consume_upgrade_faild,consume_stamina_recovery,consume_quest_consume,consume_auction_commision,consume_item_disjoint,consume_item_repair,consume_item_use,consume_item_drop,consume_gold_drop,consume_gold_card_price) values(%d,from_unixtime(%d),%d,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)",
        G_CEnvironment()->get_channel_no(), sig->m_stats[0],
        sig->m_stats[1], sig->m_stats[2],
        sig->m_stats[3], sig->m_stats[4],
        sig->m_stats[5], sig->m_stats[6],
        sig->m_stats[7], sig->m_stats[8],
        sig->m_stats[9], sig->m_stats[10],
        sig->m_stats[11], sig->m_stats[12],
        sig->m_stats[13], sig->m_stats[14],
        sig->m_stats[15], sig->m_stats[16],
        sig->m_stats[17], sig->m_stats[18],
        sig->m_stats[19], sig->m_stats[20],
        sig->m_stats[21], sig->m_stats[22],
        sig->m_stats[23], sig->m_stats[24],
        sig->m_stats[25], sig->m_stats[26]);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ORIG 0x084227f6（_ZN23DB_SaveGoldZeroTradeLog8dispatchEiiP6Stream）：
// GetDBHandle(4,0)；GetOutBuffer<SIG_SAVE_GOLD_ZERO_TRADE_LOG>；字段：
// +0 to_m_id(uint,NumberToString(,0))、+4 from_m_id(uint,NumberToString(,1))、
// +8/+0x1c from_nat_ip/from_peer_ip(str)、+0x30 from_nat_type(byte)、
// +0x31 from_mac_addr(str)、+0x72/+0x86 to_nat_ip/to_peer_ip(str)、
// +0x9a to_nat_type(byte)、+0x9b to_mac_addr(str)、+0xdc injury_money(uint)；
// inSert dnf_zeromoney_log(...)；返 exec(true)==1。
#include "W8LT_DBTask/dispatch_191.inc"
{
    SIG_SAVE_GOLD_ZERO_TRADE_LOG* sig =
        stream->GetOutBuffer<SIG_SAVE_GOLD_ZERO_TRADE_LOG>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into dnf_zeromoney_log(to_m_id,to_nat_ip,to_peer_ip,to_nat_type,from_m_id,from_nat_ip,from_peer_ip,from_nat_type,occ_time,injury_money,to_mac_addr,from_mac_addr) values(%s,'%s','%s',%d,%s,'%s','%s',%d,now(),%d,'%s','%s')",
        NumberToString(sig->m_to_m_id, 0),
        sig->m_to_nat_ip, sig->m_to_peer_ip,
        (int)sig->m_to_nat_type,
        NumberToString(sig->m_from_m_id, 1),
        sig->m_from_nat_ip, sig->m_from_peer_ip,
        (int)sig->m_from_nat_type,
        sig->m_injury_money, sig->m_to_mac_addr,
        sig->m_from_mac_addr);
    if (!db->exec(true))
        return 0;
    return 1;
}

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
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 08440f3e
#include "W8LT_DBTask/dispatch_192.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_ITEM_MAKING_SKILL* sig = stream->GetOutBuffer<SIG_ITEM_MAKING_SKILL>();
    // [FL3W8LT 2026-09-10 P183/P328①] 删两处具名缓存（原 `characNo` -0x20 槽、
    // `skills` 指针 -0x1c 槽）：ORIG 0x08440f3e 全函数只留 sig 槽（-0x20），字段
    // 访问一律基址重读 + disp 折叠（`mov -0x20(%ebp),%eax; mov (%eax),%edi`
    // charac_no@+0x00；`movzwl 0x18(%eax)` = skills[10]）；我方旧码每个
    // skills[i] 各多一条 `add $K,%eax`（11 处）⇒ 全部改内联视图表达式。
    // [P101/P328⑩] exec 收尾按 ORIG 早返形：`call exec; **xor $1,%eax**; test %al;
    // je <insert 分支>`（0x0844109f 段）⇒ 写 `if (!db->exec(true))`；两处失败
    // trace 改单语句临时链（ORIG ctor 前 `mov -0x1c(%ebp),%eax; mov (%eax),%ebx`
    // 重读 charac_no 并跨 ctor 保活）。
#define MSIG ((const DB_ItemMakingSkillView*)sig)
    db->set_query(
        "upDate item_making_skill_info set weapon=%d, cloth=%d, leather=%d, light_armor=%d, heavy_armor=%d, plate=%d, amulet=%d, wrist=%d, ring=%d, support=%d, magic_stone=%d where charac_no =%u",
        (int)MSIG->m_skillLevels[0], (int)MSIG->m_skillLevels[1],
        (int)MSIG->m_skillLevels[2], (int)MSIG->m_skillLevels[3],
        (int)MSIG->m_skillLevels[4], (int)MSIG->m_skillLevels[5],
        (int)MSIG->m_skillLevels[6], (int)MSIG->m_skillLevels[7],
        (int)MSIG->m_skillLevels[8], (int)MSIG->m_skillLevels[9],
        (int)MSIG->m_skillLevels[10], MSIG->m_characNo);
    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x9fde, 5)(
            "DB_UpdateItemMakingSkill::UpdateItemMakingSkill update, exec() ERROR charac_no=%u",
            MSIG->m_characNo);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into item_making_skill_info (charac_no, weapon, cloth, leather, light_armor, heavy_armor, plate, amulet, wrist, ring, support, magic_stone) values(%u, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d )",
            MSIG->m_characNo, (int)MSIG->m_skillLevels[0],
            (int)MSIG->m_skillLevels[1], (int)MSIG->m_skillLevels[2],
            (int)MSIG->m_skillLevels[3], (int)MSIG->m_skillLevels[4],
            (int)MSIG->m_skillLevels[5], (int)MSIG->m_skillLevels[6],
            (int)MSIG->m_skillLevels[7], (int)MSIG->m_skillLevels[8],
            (int)MSIG->m_skillLevels[9], (int)MSIG->m_skillLevels[10]);
        if (!db->exec(true))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x9ff5, 5)(
                "DB_UpdateItemMakingSkill::InsertItemMakingSkill insert, exec() ERROR charac_no=%u",
                MSIG->m_characNo);
            return 0;
        }
    }
#undef MSIG
    return 1;
}

// ORIG 0x08434718（_ZN32DB_UpdateVendingMachineStatistic8dispatchEiiP6Stream）：
// GetDBHandle(4,0)；*stream>>cnt(uint) 再 >>loop(uint)；cnt!=0 时先 inSert
// (item_id=0,cnt=cnt) 一列；再循环 loop 次：>>itemId(ulong) >>itemCnt(uint)，
// itemId==0 提前返 1，逐列 inSert log_vending_stat(occ_date,channel_no,item_id,
// cnt) values(cast(now() as date),%d,%d,%d)；返 1。
// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 08434718
#include "W8LT_DBTask/dispatch_193.inc"
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int cnt = 0;
    unsigned int loop = 0;
    *stream >> cnt;
    *stream >> loop;
    // [J4-r2] 早返形（ORIG 0x0843471e `test %eax,%eax; jne <body>; mov $0x1,%eax;
    // jmp <epilogue>` ⇒ 原码是 `if (cnt == 0) return 1;` 早返，then 块后置；
    // 4.4 对被 if 包裹的 `if(cnt!=0){body}` 产反向 `je` + 内联 mov$1 块
    // ⇒ 改早返复现 ORIG 块序/极性）。
    // [FL3W8LT 2026-09-10 P329/P0] 具名 `channelNo` 已删：ORIG 无 channelNo 槽，
    // `mov -0x14(%ebp),%ebx`（cnt）后直接 `call G_CEnvironment;
    // call get_channel_no; mov %eax,0x??(%esp)` ⇒ 实参位内联（右侧 varargs
    // 求值序保持），同时消掉我方多出的帧 0x48 vs 0x40。
    if (cnt == 0)
        return 1;
    db->set_query(
        "inSert into log_vending_stat(occ_date,channel_no,item_id,cnt) values(cast(now() as date),%d,%d,%d)",
        G_CEnvironment()->get_channel_no(), 0, cnt);
    db->exec(true);
    for (unsigned int i = 0; i < loop; ++i)
    {
        unsigned long itemId = 0;
        unsigned int itemCnt = 0;
        *stream >> itemId;
        *stream >> itemCnt;
        if (itemId == 0)
            break;          // [J4-r2] ORIG 0x0843475a `test %eax,%eax; je <尾 mov$1>`
                            // ⇒ 循环内失败与函数尾 `return 1` 共享出口；写 return 1
                            // 会被 4.4 内联成 `mov $1; jmp` 岛（P214②共享尾）。
        db->set_query(
            "inSert into log_vending_stat(occ_date,channel_no,item_id,cnt) values(cast(now() as date),%d,%d,%d)",
            G_CEnvironment()->get_channel_no(), itemId, itemCnt);
        db->exec(true);
    }
    return 1;
}

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
                                            Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_INSERT_SERVER_LOAD_STATISTIC* sig =
        stream->GetOutBuffer<SIG_INSERT_SERVER_LOAD_STATISTIC>();
    db->set_query(
        "inSert into log_gameserver_load_stat set occ_time=now(), server_id=%d, channel_no=%d, dbq_cnt=%d, logdbq_cnt=%d, netq_cnt=%d, packetq_cnt=%d",
        (int)reinterpret_cast<const DB_ServerLoadStatView*>(sig)->m_serverGroup,
        (int)reinterpret_cast<const DB_ServerLoadStatView*>(sig)->m_channelNo,
        (int)reinterpret_cast<const DB_ServerLoadStatView*>(sig)->m_dbqCnt,
        (int)reinterpret_cast<const DB_ServerLoadStatView*>(sig)->m_logdbqCnt,
        (int)reinterpret_cast<const DB_ServerLoadStatView*>(sig)->m_netqCnt,
        (int)reinterpret_cast<const DB_ServerLoadStatView*>(sig)->m_packetqCnt);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ORIG 0x08430144（_ZN22DB_SaveCharacterOption8dispatchEiiP6Stream）：
// GetDBHandle(2,0)；GetOutBuffer<SIG_CHARACTER_OPTION>（+0 characNo(uint)、
// +4 options blob 0x200）；blob_to_str(0,&sig->m_pad[0x4],0x200)；upDate
// charac_option set options='%s' where charac_no=%u；若 exec==1 且
// affected!=0 跳过 inSert；否则 inSert(charac_no,options)；exec 失败返 0；
// 返 1。
// [b28d-solo-2R] DIFF(3)：`(char*)sig + 4` 得 lea 形（差异 5→3 条，N4/N107 add-vs-lea）；
//   `(char*)((unsigned long)sig+4UL)` 得 ORIG 的 add 形但装载序反（4.4 先算 options 后载 len，
//   ORIG 反之 = N7/P75 实参求值序）；语句表达式/冗余 (int) cast 均无效。ORIG 0x8430144
#include "W8LT_DBTask/dispatch_195.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_CHARACTER_OPTION* sig = stream->GetOutBuffer<SIG_CHARACTER_OPTION>();
    // ORIG 0x08430144 帧：len(0x200) 有 int 槽 -0x10（`movl $0x200,-0x10;
    // mov -0x10,%eax` blob_to_str 实参 spill）、options(-0xc) 有槽、
    // characNo **无槽**（两处 set_query 实参位置各自 `mov -0x14,%eax;
    // mov (%eax),%eax` 直读 sig）。
    // ORIG src/len 装载序 = len 先（%eax）：4.4 左→右评估恒 src 先 ⇒
    // register asm("eax") 在实参前物化 len（技巧28 变体）。
    // [b28d-solo-4] P316 volatile 旋钮：ORIG 4.1 右→左求值（len 先于 sig+4 装载），
    // 4.4 左→右恒先算 ptr。len 加 volatile 声明强制其访问先行 ⇒ 助记符序列全等
    // （DIFF→NEAR，探针 h1 实证）；余 db/sig/len 三槽重排（-0x14/-0x10/-0x18 vs
    // ORIG -0x18/-0x14/-0x10）+ %eax/%edx 角色互换 = N82/N7 封顶。
    // [FL3W8LT 2026-09-10 A/B 回滚] 去 volatile（试让 4.4 常规槽序给 len 最浅位）
    // 实测 **NEAR→DIFF 退化**（TU near 10→9 / diff 66→67，plus=22/minus=22 同值）
    // ⇒ volatile 是 P316 求值序的必要载体，槽序三槽重排归 N82/N7；探针
    // /tmp/fl3w/ps.cpp f1/f2 已证 4.4 把 volatile 局部固定排最深槽（不可兼得）。
    volatile int len = 0x200;
    // [b28c-L19 清绑 P0/N96] 原 `register int lreg asm("eax"); lreg = len;` 已删，
    // 改实参直传 len（4.4 评估序差异记 N96）。
    // ORIG 0x08430144 src 实参 = `mov -0x14(%ebp),%edx; add $0x4,%edx`
    // 覆写形（技巧26 blob 指针），且评估晚于 len spill ⇒ 数组 decay
    // (`lea 0x4(%eax)`) 会把装载提前。
    char* options = db->blob_to_str(
        0, (char*)((unsigned int)sig + 4u), len);
    db->set_query("upDate charac_option set options = '%s' where charac_no= %u",
                  options, sig->m_characNo);
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into charac_option(charac_no, options) values(%u,'%s')",
            sig->m_characNo, options);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ORIG 0x0843ef30（_ZN26DB_UpdateAddItemCountValue11makeRequestEjj）：
// Acquire("DBThread.cpp",0x9684) → CStreamGuard(stream,true)
//   → <<0x280(int) → <<-1(int) → <<param1(uint) → <<param2(uint)
//   → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_41.inc"
                                             unsigned int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9684), true);
    guard.operator*()->operator<<((int)0x280);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)param1);
    guard.operator*()->operator<<((unsigned int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x08442966（_ZN25DB_UpdateOntimeLastRecvIdx11makeRequestEiji）：
// Acquire("DBThread.cpp",41999) → CStreamGuard(stream,true)
//   → <<0x297(int) → <<param1(int) → <<param2(uint) → <<param3(int)
//   → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_42.inc"
                                             int param3)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 41999), true);
    guard.operator*()->operator<<((int)0x297);
    guard.operator*()->operator<<((int)param1);
    guard.operator*()->operator<<((unsigned int)param2);
    guard.operator*()->operator<<((int)param3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_SaveKillMonsterInfo::dispatch —— ORIG 0x08441200（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(2,0) → GetOutBuffer<SIG_LOAD_KILL_MONSTER_INFO>，空返 0；三段
// put_compressed_blob_data（field 0: sig+0x0 长 0x1d4c；field 1: sig+0x1d4c 长
// 30000；field 2: sig+0x927c 长 0x1d4c），任一段失败 cMyTrace(0xa010/0xa01a/
// 0xa024, 5)("... compress_zip(%u) ERROR charac_no=%u", 段号, charac_no) 返 0；
// upDate charac_kill_monster_info ... where charac_no=%u（charac_no=sig+0xafc8）
// exec 失败返 0；affectedRowCount==0 → inSert ...，insert exec 失败返 0；全绿 1。
// [b28d-4] DIFF：P183/P171：ORIG 多 {'movl': 1}（漏具名局部/去缓存不足）；ORIG 08441200
#include "W8LT_DBTask/dispatch_196.inc"
                                      Stream* stream)
{
    // [J4S2-② 回滚 2026-09-11] GetOutBuffer/空检查提到 GetDBHandle 之前实测反劣
    // （ndiff 81→87）⇒ 保持 db 先取。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_LOAD_KILL_MONSTER_INFO* sig =
        stream->GetOutBuffer<SIG_LOAD_KILL_MONSTER_INFO>();
    if (sig == 0)
        return 0;
    char* bossInfo = 0;    // -0x4c
    char* namedInfo = 0;   // -0x50
    char* apcBossInfo = 0; // -0x54
    bool ok = false;       // -0x9
    ok = put_compressed_blob_data(db, 0, (char*)sig, 0x1d4c, &bossInfo);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa010, 5);
        trace("DB_SaveKillMonsterInfo::dispatch boss_info compress_zip(%u) ERROR charac_no=%u",
              0, sig->m_characNo);
        return 0;
    }
    ok = put_compressed_blob_data(db, 1, (char*)sig->m_named, 30000,
                                  &namedInfo);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa01a, 5);
        trace("DB_SaveKillMonsterInfo::dispatch named_info compress_zip(%u) ERROR charac_no=%u",
              1, sig->m_characNo);
        return 0;
    }
    ok = put_compressed_blob_data(db, 2, (char*)sig->m_apc, 0x1d4c,
                                  &apcBossInfo);
    if (!ok)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xa024, 5);
        trace("DB_SaveKillMonsterInfo::dispatch apc_boss_info compress_zip(%u) ERROR charac_no=%u",
              2, sig->m_characNo);
        return 0;
    }
    db->set_query(
        "upDate charac_kill_monster_info set boss_info='%s', named_info='%s', apc_boss_info='%s' where charac_no=%u",
        bossInfo, namedInfo, apcBossInfo, sig->m_characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_kill_monster_info(charac_no, boss_info, named_info, apc_boss_info) values(%u, '%s', '%s', '%s')",
            sig->m_characNo, bossInfo, namedInfo, apcBossInfo);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

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

// [b28d-4] DIFF：N18a/P211：4.4 多 {'push': 1, 'mov': 6, 'test': 2, 'je': 2, 'xor': 2, 'jmp': 2, 'movb': 4, 'movzbl': 2, 'lea': 1, 'rep': 1, 'pop': 1} 中转 mov（单段直落/同型化待验）；ORIG 08423bc4
#include "W8LT_DBTask/dispatch_197.inc"
                                       Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    // [FL3W8LT 2026-09-10 P183/P328①] 原具名视图局部
    // `DB_PassPadUpdateFailCntView* v = reinterpret_cast<...>(sig);` 已删：
    // ORIG 0x0843bc4 段每处字段访问都重发 `mov -0x10(%ebp),%eax;
    // mov (%eax),%eax`（重读 sig 槽再解引用，无独立视图槽），且帧
    // ORIG `push %ebx; sub $0x94` vs 我方 `push %edi; sub $0xa0` ⇒ 每点内联
    // 视图表达式（P89 单一视图表达式）。
#define PSIG ((DB_PassPadUpdateFailCntView*)sig)
    db->set_query(
        "upDate member_mousepass set fail_cnt=%d where m_id=%s and enable_flag = '1'",
        PSIG->m_failCnt, NumberToString(PSIG->m_mId, 0));
    db->exec(true);
    if (PSIG->m_failCnt != 0)
    {
        db->set_query(
            "seLect mousepass from member_mousepass where m_id=%s",
            NumberToString(PSIG->m_mId, 0));
        // [H4 2026-09-11] ORIG 0x08423c92..0x08423cc8 实证：bool 值**不落栈槽** ——
        // `xor $1,%eax; test %al,%al; jne <fail>` + `test %eax,%eax; je <fail>` +
        // `xor $1,%eax; test %al,%al; je <succ>` 短路链，`fail: mov $1,%eax` /
        // `succ: mov $0,%eax` 汇聚后 `test %al,%al; je` 直判 ⇒ 原码**无具名 bool
        // 局部**（P0 槽-局部双向对应律），直接判复合条件（旧「true 初值 + 置 false
        // + 正判」形多 4 movb + 2 movzbl 槽往返）。
        if (!(db->exec(true) == 1 && db->get_n_rows() != 0 && db->fetch() == 1))
            return 0;
        // [H4 2026-09-11] ORIG 0x08423cd4 直接 `lea -0x48(%ebp),%eax; …call get_str`
        // —— **无**预清（原码 `char mousepass[0x24];`，P171/P328⑤ 不加多余初始化；
        // 失败路径已 return 0，无未初始化读取）。
        char mousepass[0x24];           // -0x48
        if (db->get_str(0, mousepass, 0x24) != 1)
            return 0;
        char ym[10] = { 0 };            // -0x1e（strftime "%Y%m"）
        time_t curSec = GlobalData::s_systemTime_.getCurSec();   // -0x24
        struct tm tmBuf;                // -0x74
        localtime_r(&curSec, &tmBuf);
        strftime(ym, 10, "%Y%m", &tmBuf);
        db->set_query(
            "inSert into member_mousepass_history_%s (occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info) values (now(),%s,'%s',%d,'%s','')",
            ym, NumberToString(PSIG->m_mId, 0), mousepass, 0x11, PSIG->m_ipInfo);
        db->exec(true);
    }
    MySQL* dbGrade = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
    dbGrade->set_query(
        "upDate member_security_grade set goblin_fail_cnt=%d where m_id=%s",
        PSIG->m_failCnt, NumberToString(PSIG->m_mId, 0));
    // [H4 2026-09-11] 同上（P0）：ORIG 下部 bool 同走 `mov $1/$0,%eax` 寄存器物化
    // ⇒ 无具名局部，直接判复合条件。
    if (!(dbGrade->exec(true) == 1 && dbGrade->getAffectedRowCount() != 0))
    {
        dbGrade->set_query(
            "inSert into member_security_grade(m_id, goblin_fail_cnt) values(%s, %d)",
            NumberToString(PSIG->m_mId, 0), PSIG->m_failCnt);
        if (dbGrade->exec(true) != 1)
            return 0;
    }
#undef PSIG
    return 1;
}

// ============================================================================
// DB_TOD_Update_UserState::dispatch —— ORIG 0x084403aa（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(2,0) → GetOutBuffer<SIG_REQUEST_TOD_USER_STATE>；
// +4(m_firstEnterTime)==0 恒返 1；upDate charac_tower_despair set
// first_layer_start_date=from_unixtime(%d),today_enter_count=%d,
// last_clear_layer=%d,enter_count_by_week=%d,m_date=from_unixtime(%d),
// last_clear_date=from_unixtime(%d) where charac_no=%u（参数 +4,+8,GetLayer(+0xc),
// (short)+0xe,+0x10,+0x14,+0）；exec 失败返 0；affectedRowCount==0 → inSert 同列
// （+0,+4,+8,GetLayer,+0xe,+0x10,+0x14），insert 失败返 0；全绿 1。
#include "W8LT_DBTask/dispatch_198.inc"
                                       Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_REQUEST_TOD_USER_STATE* sig =
        stream->GetOutBuffer<SIG_REQUEST_TOD_USER_STATE>();
    if (sig->m_firstEnterTime == 0)
        return 1;
    db->set_query(
        "upDate charac_tower_despair set first_layer_start_date=from_unixtime(%d), today_enter_count=%d, last_clear_layer=%d, enter_count_by_week=%d, m_date=from_unixtime(%d), last_clear_date=from_unixtime(%d) where charac_no=%u",
        sig->m_firstEnterTime, sig->m_enterCount,
        (unsigned int)sig->m_todayLayer.GetLayer(),
        (int)(short)sig->m_lastEnterLayer, sig->m_lastEnterTime,
        sig->m_lastClearTime, sig->m_characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_tower_despair(charac_no,first_layer_start_date,today_enter_count,last_clear_layer,enter_count_by_week,m_date,last_clear_date) values(%u,from_unixtime(%d),%d,%d,%d,from_unixtime(%d),from_unixtime(%d))",
            sig->m_characNo, sig->m_firstEnterTime, sig->m_enterCount,
            (unsigned int)sig->m_todayLayer.GetLayer(),
            (int)(short)sig->m_lastEnterLayer, sig->m_lastEnterTime,
            sig->m_lastClearTime);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

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
                                       Stream* stream)
{
    SIG_SAVE_CHARACINFO4GUILD* sig =
        stream->GetOutBuffer<SIG_SAVE_CHARACINFO4GUILD>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_8,
                                                  E_SERVER_GROUP_NONE);
    // ORIG +0x52 将第二次 GetDBHandle 结果物化到 -0xc 后不再读取（死槽 store）；
    // 字段读取逐次重读 sig(-0x14)+具名 disp（P86），不缓存视图局部。
    MySQL* dbGuild = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate guild_member set grow_type=%d,lev=%d where guild_id=%d and charac_no=%d",
        ((DB_SaveCharacInfo4GuildView*)sig)->m_growType,
        ((DB_SaveCharacInfo4GuildView*)sig)->m_lev,
        ((DB_SaveCharacInfo4GuildView*)sig)->m_guildId,
        ((DB_SaveCharacInfo4GuildView*)sig)->m_characNo);
    (void)dbGuild;
    if (!db->exec(true))
        return 0;
    return 1;
}

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

// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 084317b2
// [F2e-Singles2 C05② 2026-09-10] 旋钮①删 `(unsigned char)i` 收窄（我方 8 位
//   `cmp %al,%dl; seta` vs ORIG 32 位 `movzbl; movzbl; cmp -N(%ebp),%eax; setg`）
//   ⇒ 翻正为 ORIG 逐条形；旋钮②mId/schoolId 具名局部内联（P0，实参直传）⇒
//   实测 our 78→76（ORIG 74，gap **−4→−2**），TU 四数 141/1743/9/67 两态逐位一致。
//   残余 = 我方多 sig/v 缓存槽 + 帧 `sub $0x34` vs ORIG `$0x20` + 缺 push %esi（P210/N82）。
#include "W8LT_DBTask/dispatch_200.inc"
                                        Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10,
                                                  E_SERVER_GROUP_NONE);
    Packet_PvPResult* sig = stream->GetOutBuffer<Packet_PvPResult>();
    // [L5 K5W8LTDBTask] P0：去 `v` 具名缓存（ORIG 0x084317b2 段无第二槽往返，
    // 全程 `((DB_PvPResultSchoolView*)sig)->...` 就地 cast 直读）。
    for (int i = 0; (unsigned char)((DB_PvPResultSchoolView*)sig)->m_count > i; ++i)
    {
        for (int j = 0; j <= 1; ++j)
        {
            db->set_query(
                "upDate school_member set last_play_time=NOW()  where m_id=%s and school_id=%d",
                NumberToString(((DB_PvPResultMIdView*)sig)->m_mId[j][i], 0),
                ((DB_PvPResultSchoolView*)sig)->m_schoolId[j][i]);
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_InsertLevelDungeonPlay::dispatch —— ORIG 0x0842cc98（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(4,0) → *stream >> count(int)；循环 count 次：>> level、>>
// characCount、>> dungeonCount；upDate log_level_dungeon_play_count set
// total_charac_count=total_charac_count+%d,total_dungeon_play_count=
// total_dungeon_play_count+%d where level=%d（characCount,dungeonCount,level）；
// exec 失败或 affectedRowCount==0 → inSert ... values(%d,%d,%d)（level,
// characCount,dungeonCount），insert 失败返 0；循环完返 1。
// [b28d-solo] AE：P0 去具名 bool upOk（ORIG 无该槽往返 ⇒ 条件内联）；
// 4.1/4.4 逐条同形（原 -0xc 多占槽致全帧 -4 平移）；ORIG 0842cc98
#include "W8LT_DBTask/dispatch_201.inc"
                                         Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int count = 0;         // -0x14
    int level = 0;         // -0x18
    int characCount = 0;   // -0x1c
    int dungeonCount = 0;  // -0x20
    *stream >> count;
    for (int i = 0; i < count; ++i)
    {
        *stream >> level;
        *stream >> characCount;
        *stream >> dungeonCount;
        db->set_query(
            "upDate log_level_dungeon_play_count set total_charac_count=total_charac_count+%d,total_dungeon_play_count=total_dungeon_play_count+%d where level=%d",
            characCount, dungeonCount, level);
        if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
        {
            db->set_query(
                "inSert into log_level_dungeon_play_count (level,total_charac_count,total_dungeon_play_count) values(%d,%d,%d)",
                level, characCount, dungeonCount);
            if (db->exec(true) != 1)
                return 0;
        }
    }
    return 1;
}

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
                                         Stream* stream)
{
    SIG_REMOVE_FEATURED_GOODSIDX* sig =
        stream->GetOutBuffer<SIG_REMOVE_FEATURED_GOODSIDX>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 无 view 缓存槽：逐字段重读 sig(-0x10)+具名 disp（P86/P89）。
    db->set_query(
        "deLete from dnf_event_entry where m_id=%s and event_id=%d and server_id=%d",
        NumberToString(((DB_RemoveFeaturedGoodsIdxView*)sig)->m_mId, 0),
        ((DB_RemoveFeaturedGoodsIdxView*)sig)->m_eventId,
        ((DB_RemoveFeaturedGoodsIdxView*)sig)->m_serverId);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_DeletePowerWarStatueMessage::dispatch —— ORIG 0x08437d4e（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(6,0) → *stream >> serverInfo(int)；deLete from
// event_server_message where server_info = %d and message_index in (1, 2, 3)；
// exec 失败 → LogManager::logFormat(1,"DBThread.cpp",__PRETTY_FUNCTION__,
// 0x853d,"deLete_power_war_statue_message Query Error")；恒返 1。
#include "W8LT_DBTask/dispatch_203.inc"
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    int serverInfo = 0;   // -0x10
    *stream >> serverInfo;
    db->set_query(
        "deLete from event_server_message where server_info = %d and message_index in (1, 2, 3)",
        serverInfo);
    if (db->exec(true) != 1)
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x853d, "deLete_power_war_statue_message Query Error");
    }
    return 1;
}

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

// [b28d-solo] P212 全头部声明 + P244 具名 char* 局部 + P133 独立守卫；
// 两个 ORIG 死局部（-0x20/-0x1c 仅初始化）按 P232 照抄以复现帧。ORIG 08437a32
#include "W8LT_DBTask/dispatch_204.inc"
                                        Stream* stream)
{
    SIG_FIND_FACTORY_HUB_USER* sig =
        stream->GetOutBuffer<SIG_FIND_FACTORY_HUB_USER>();
    int characNo = 0;        // -0x28
    int unusedRowCount = 0;  // -0x20（ORIG 死局部，仅 movl $0，P232）
    int unusedFieldIdx = 0;  // -0x1c（同上）
    unsigned int mId = 0;    // -0x2c
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 0x08437a84：`movl $<str>,-0x14` 具名 char* 局部两跳（P244）。
    const char* query =
        "seLect charac_no,m_id from charac_info where charac_name='%s'";
    // ORIG 无 v/ov 视图指针槽——每处内联 cast（P89/P183）。
    db->set_query(query, sig->m_characName);
    if (!db->exec(true))
        return 0;
    for (unsigned int i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        if (!db->fetch())
            return 0;
        if (!db->get_int(0, characNo))
            return 0;
        if (!db->get_uint(1, mId))
            return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x850f), true);
    guard.operator*()->operator<<((int)0x1a1);
    guard.operator*()->operator<<((int)slot);
    SIG_FIND_FACTORY_HUB_USER_NO_CONN* out =
        guard.operator->()->GetInBuffer<SIG_FIND_FACTORY_HUB_USER_NO_CONN>();
    // [b28d-solo-W8LT_DBTask] 复核：显式 (int)/(short) cast 探针无效（仍 RHS
    // 两段中转）⇒ 字段/RHS 异型化旋钮（P211 档③）对本件不可翻，N76/N18a 封顶。
    out->m_mId = mId;
    out->m_characNo = characNo;
    out->m_flag = sig->m_flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_UpdateGoblinPadAdvicePwdRewardTime::makeRequest —— ORIG 0x0843f684
// （报告 makeRequest.md 全文；static）
// ============================================================================
// Acquire("DBThread.cpp",0x9927) → CStreamGuard(stream,true) → <<0x247(int) →
// <<param1(int) → <<param2(uint) → put(DB_Q(2))。
#include "W8LT_DBTask/makeRequest_43.inc"
                                                        unsigned int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9927), true);
    guard.operator*()->operator<<((int)0x247);
    guard.operator*()->operator<<((int)param1);
    guard.operator*()->operator<<((unsigned int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_UpdateFatigueAttendance::makeRequest —— ORIG 0x084447d6（报告 makeRequest.md
// 全文；static）
// ============================================================================
// Acquire("DBThread.cpp",0xab72) → CStreamGuard(stream,true) → <<0x2ac(int) →
// <<-1(int) → <<param1(uint) → <<m.size()(int) → const_iterator 遍历：
// <<it->first(uchar) → <<it->second(short) → put(DB_Q(2))。
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084447d6
#include "W8LT_DBTask/makeRequest_44.inc"
    unsigned int param1,
    std::map<unsigned char, short, std::less<unsigned char>,
             std::allocator<std::pair<const unsigned char, short> > > const& m)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xab72), true);
    guard.operator*()->operator<<((int)0x2ac);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)param1);
    guard.operator*()->operator<<((int)m.size());
    for (std::map<unsigned char, short>::const_iterator it = m.begin();
         it != m.end(); ++it)
    {
        guard.operator*()->operator<<((unsigned char)it->first);
        guard.operator*()->operator<<((short)it->second);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

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
                                       Stream* stream)
{
    SIG_SAVE_FEATURED_GOODSIDX* sig =
        stream->GetOutBuffer<SIG_SAVE_FEATURED_GOODSIDX>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 0x84…：sig(-0x20)/db(-0x1c) 双槽、无 view 具名槽 ⇒ view cast
    // 内联进实参（每字段 `-mov -0x20,%eax; mov 0xNN(%eax)` 单级读）。
    db->set_query(
        "inSert into dnf_event_entry(event_id,m_id,occ_date,server_id,charac_no,obtain_date) values(%d,%s,from_unixtime(%d),%d,%d,from_unixtime(%d))",
        ((DB_SaveFeaturedGoodsIdxView*)sig)->m_eventId,
        NumberToString(((DB_SaveFeaturedGoodsIdxView*)sig)->m_mId, 0),
        ((DB_SaveFeaturedGoodsIdxView*)sig)->m_obtainDate,
        ((DB_SaveFeaturedGoodsIdxView*)sig)->m_serverId,
        ((DB_SaveFeaturedGoodsIdxView*)sig)->m_characNo,
        ((DB_SaveFeaturedGoodsIdxView*)sig)->m_obtainDate);
    if (!db->exec(true))
        return 0;
    return 1;
}
// ============================================================================
// DB_SaveSameMIDOtherCharLoginHack::dispatch —— ORIG 0x08422680（报告
// dispatch.md 全文）
// ============================================================================
// GetDBHandle(4,0) → GetOutBuffer<SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK>
// （SigTypes_E.h 已具名：+0x00 m_curAccId / +0x04 m_reqAccId / +0x08
// m_characNo / +0x0c m_peerIp[0x14]）→ set_query(
// "inSert into login_view_log values(now(),%s,%s,%d,'%s')", num(m_curAccId,0),
// num(m_reqAccId,1), m_characNo, m_peerIp) → 返 exec(true)==1。
#include "W8LT_DBTask/dispatch_206.inc"
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK* sig =
        stream->GetOutBuffer<SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK>();
    db->set_query(
        "inSert into login_view_log values(now(),%s,%s,%d,'%s')",
        NumberToString(sig->m_curAccId, 0), NumberToString(sig->m_reqAccId, 1),
        sig->m_characNo, sig->m_peerIp);
    if (!db->exec(true))
        return 0;
    return 1;
}

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

// ============================================================================
// DB_ReqSaveCrazyLevelupEvent::dispatch —— ORIG 0x0842527a（报告 dispatch.md
// 全文）
// ============================================================================
// GetOutBuffer<SIG_SAVE_CRAZY_LEVELUP_EVENT> → GetDBHandle(4,0)；先 inSert
// log_fighter_event(m_id=num(+0x05), event_id=+0x00, charac_no=+0x09)；exec
// 失败 → upDate occ_time=now(),occ_cnt=occ_cnt+1（同键）；再失败返 0。
#include "W8LT_DBTask/dispatch_207.inc"
                                           Stream* stream)
{
    DB_SaveCrazyLevelupEventView* sig =
        reinterpret_cast<DB_SaveCrazyLevelupEventView*>(
            stream->GetOutBuffer<SIG_SAVE_CRAZY_LEVELUP_EVENT>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into log_fighter_event(m_id,event_id,charac_no,occ_time,occ_cnt) values(%s,'%s',%d,now(),1)",
        NumberToString(sig->m_memberId, 0), sig->m_eventId, sig->m_characNo);
    if (db->exec(true) != 1)
    {
        db->set_query(
            "upDate log_fighter_event set occ_time=now(),occ_cnt=occ_cnt+1 where m_id=%s and event_id='%s' and charac_no=%d",
            NumberToString(sig->m_memberId, 0), sig->m_eventId,
            sig->m_characNo);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_InsertOnTimeRewardUserLog::dispatch —— ORIG 0x08442a56（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(DB_HANDLE_D,0) → *stream >> m_id(uint) >> recv_no(int)；
// 表前缀常量 "1112" → set_query(
// "inSert into event_%s_ontime_reward_user(m_id,recv_no) values(%s,%u)")；
// exec 失败 → cMyTrace(__PRETTY_FUNCTION__, 0xa439, 5) + failed 日志 → 返 0。
#include "W8LT_DBTask/dispatch_208.inc"
                                            Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_D,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    int recvNo = 0;
    *stream >> mId;
    *stream >> recvNo;
    const char* table = "1112";   // ORIG DAT_08c56b43
    db->set_query(
        "inSert into event_%s_ontime_reward_user(m_id,recv_no) values(%s,%u)",
        table, NumberToString(mId, 0), recvNo);
    if (db->exec(true) != 1)
    {
        // 技巧3：临时链式（具名块对象使 mId/recvNo 槽内收 16B；ORIG
        // 0x08442a56 mId=-0x24/recvNo=-0x28/trace=-0x34?，先例
        // DB_LoadBloodBestRecord 闭合形）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa439, 5)(
            "inSert into event_%s_ontime_reward_user(m_id,recv_no) failed at m_id(%s,%u) ",
            table, NumberToString(mId, 0), recvNo);
        return 0;
    }
    return 1;
}

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

// ============================================================================
// DB_InsertAutoPunishFirstUser::dispatch —— ORIG 0x0842d6ce（报告 dispatch.md
// 全文）
// ============================================================================
// 段1：DB_HANDLE_6 —— auto_punish_first_user 先 inSert（8 列）；失败 →
// upDate 同键计数（再失败 cMyTrace 0x70fb "upDate auto_punish_first_user
// Fail"）。段2：DB_HANDLE_1 —— punish_flag∈{2,7,8} 直接返 1；否则
// dwPunishDay=etc/10、dwApplyFlag=etc%10，punish_type=3（etc==10 → 0xc）；
// member_punish_info upDate（end_time 按天 adddate），exec!=1 或 affected==0
// → inSert 同值；inSert 失败返 0。段3：sprintf 邮件原因 → strftime("%Y") 后缀
// 表 member_punish_info_history_%s 逐日 inSert（6 惩罚值）→ 失败返 0；type∈
// {3} 或 {4 且 flag==0} → member_punish_info_ars_mail inSert 失败返 0；恒返 1。
// [b28d-4] DIFF：P222：块序/极性（ORIG je 落点 vs 4.4 jne）；ORIG 0842d6ce
#include "W8LT_DBTask/dispatch_209.inc"
                                            Stream* stream)
{
    // ORIG 0x0842d6da 函数首句 movl $0x0,-0x30（P198① 预清：声明=0 + 独立赋值）
    int punishType = 0;                 // ORIG local_34 @ -0x30
    DB_InsertAutoPunishFirstUserView* sig =
        reinterpret_cast<DB_InsertAutoPunishFirstUserView*>(
            stream->GetOutBuffer<SIG_INSERT_AUTO_PUNISH_FIRST_USER>());
    MySQL* db6 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                   E_SERVER_GROUP_NONE);
    db6->set_query(
        "inSert into auto_punish_first_user(m_id,occ_time,ip,hack_type,cnt,hack_sub_type,hack_sub_cnt,punish_flag) values(%s,now(),'%s',%d,%d,%d,%d,%d)",
        NumberToString(sig->m_memberId, 0), sig->m_ip, sig->m_hackType,
        sig->m_cnt, sig->m_hackSubType, sig->m_hackSubCnt, sig->m_punishFlag);
    if (db6->exec(true) != 1)
    {
        db6->set_query(
            "upDate auto_punish_first_user set occ_time=now(),ip='%s',cnt=%d, hack_sub_cnt=%d, punish_flag=%d where m_id=%s and hack_type=%d and hack_sub_type=%d",
            sig->m_ip, sig->m_cnt, sig->m_hackSubCnt, sig->m_punishFlag,
            NumberToString(sig->m_memberId, 0), sig->m_hackType,
            sig->m_hackSubType);
        if (db6->exec(true) != 1)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x70fb, 0);
            trace("upDate auto_punish_first_user Fail");
        }
    }
    // [b28d-TakeW8LT 声明位复原] ORIG 0x0842d8ab 才调 GetDBHandle(1,0)
    // （段1 之后、punishFlag 判断之前，结果落 -0x24）；旧形在函数开头提前调用，
    // 多发一次 GetDBHandle 并整体错帧。
    MySQL* db1 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                   E_SERVER_GROUP_NONE);
    // ORIG 0x0842d8ba..0x0842d8f3 三个独立 if-return（各 mov $1; jmp 独立岛），
    // 非短路合并链（P45 反向：合并形会产 je 共落点）
    if (sig->m_punishFlag == 2)
        return 1;
    if (sig->m_punishFlag == 7)
        return 1;
    if (sig->m_punishFlag == 8)
        return 1;
    punishType = 3;                     // ORIG 0x0842d8f8 movl $0x3,-0x30
    if (sig->m_punishFlag == 10)
        punishType = 0xc;
    unsigned int dwPunishDay = sig->m_etcValue / 10;  // ORIG local_24
    unsigned int dwApplyFlag = sig->m_etcValue % 10;  // ORIG local_20
    printf("[ON!!] request->etcValue_ : %d , dwPunishDay(%d), dwApplyFlag(%d)\n",
           sig->m_etcValue, dwPunishDay, dwApplyFlag);
    if (dwPunishDay == 0)
    {
        db1->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), end_time='9999-12-31 23:59:59', apply_flag=%d where m_id=%s and punish_type=%d",
            dwApplyFlag != 0 ? 1 : 2, NumberToString(sig->m_memberId, 0),
            punishType);
    }
    else
    {
        db1->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), end_time= adddate(now(),interval %d day), apply_flag=%d where m_id=%s and punish_type=%d",
            dwPunishDay, dwApplyFlag != 0 ? 1 : 2,
            NumberToString(sig->m_memberId, 0), punishType);
    }
    // ORIG 0x0842da37..0x0842da62 无槽往返（xor $1;test;jne; call;or;test;jne;
    // mov$1/jmp/mov$0; test %al,%al; je）⇒ 谓词整体内联，禁具名 bool（P198②）
    if (!(db1->exec(true) == 1 && db1->getAffectedRowCount() != 0))
    {
        if (dwPunishDay == 0)
        {
            db1->set_query(
                "inSert into member_punish_info(m_id, punish_type, occ_time, start_time, end_time, punish_value, apply_flag) values(%s, %d, now(), now(), '9999-12-31 23:59:59', 0, %d)",
                NumberToString(sig->m_memberId, 0), punishType,
                dwApplyFlag != 0 ? 1 : 2);
        }
        else
        {
            db1->set_query(
                "inSert into member_punish_info(m_id, punish_type, occ_time, start_time, end_time, punish_value, apply_flag) values(%s, %d, now(), now(), adddate(now(),interval %d day), 0, %d)",
                NumberToString(sig->m_memberId, 0), punishType, dwPunishDay,
                dwApplyFlag != 0 ? 1 : 2);
        }
        if (db1->exec(true) != 1)
            return 0;
    }
    // ORIG 0x0842db2d mov $0xff,%ebx（待清字节数 255）+ 起始槽 -0x17b 非 4 对齐
    // ⇒ 0xff 字节数组（P312/P253；旧 0x100 使 4.4 折 0x40 dwords 简单 rep stos）
    char reason[0xff] = { '\0' };
    sprintf(reason, "{Auto 1st Information-Mail}|*|ARS 1st Punish / %d / %d",
            sig->m_hackType, sig->m_hackSubType);
    char year[10] = { '\0' };
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm tmBuf;
    localtime_r(&t, &tmBuf);
    strftime(year, 10, "%Y", &tmBuf);
    if (dwPunishDay == 0)
    {
        db1->set_query(
            "inSert into member_punish_info_history_%s(m_id, punish_type, occ_time, punish_value, start_time, end_time, admin_id, apply_flag, reason) values(%s, %d, now(), 6, now(), '9999-12-31 23:59:59', 'ARS', %d, '%s')",
            year, NumberToString(sig->m_memberId, 0), punishType,
            dwApplyFlag != 0 ? 1 : 2, reason);
    }
    else
    {
        db1->set_query(
            "inSert into member_punish_info_history_%s(m_id, punish_type, occ_time, punish_value, start_time, end_time, admin_id, apply_flag, reason) values(%s, %d, now(), 6, now(), adddate(now(),interval %d day), 'ARS', %d, '%s')",
            year, NumberToString(sig->m_memberId, 0), punishType, dwPunishDay,
            dwApplyFlag != 0 ? 1 : 2, reason);
    }
    if (db1->exec(true) != 1)
        return 0;
    if (punishType == 3 || (punishType == 4 && dwApplyFlag == 0))
    {
        db1->set_query(
            "inSert into member_punish_info_ars_mail (m_id, punish_type, apply_flag, occ_time) values(%s, %d, %d, now())",
            NumberToString(sig->m_memberId, 0), punishType,
            dwApplyFlag != 0 ? 1 : 2);
        if (db1->exec(true) != 1)
            return 0;
    }
    return 1;
}

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

// ============================================================================
// DB_InsertAutoPunishSecondUser::dispatch —— ORIG 0x0842deb4（报告
// dispatch.md 全文）
// ============================================================================
// DB_HANDLE_6：auto_punish_second_user 先 upDate（total_trade_gold/trade_gold
// 自增 + punish_flag），affected==0 → inSert（1,1 计数）；auto_punish_second_log
// upDate（hack_m_id 键）同法 inSert。punish_flag 置位 → DB_HANDLE_1
// member_punish_info upDate/inSert（type=4）+ sprintf 邮件原因 →
// member_punish_info_history inSert（admin 'ARS'，apply_flag=2）失败返 0。
// [SweepW8LT b28d] DIFF：gap 19→22（our 293 / ORIG 315）。本波修两处：
// ① P0 去 needInsert1/2/3 具名 bool（ORIG `xor$1; test%al,%al; jne` 直判；
//    具名局部致 4.4 产 `mov %al,-0x1b; cmpb $0x0,-0x1b` 槽往返）；
// ② 真语义修正：db1=GetDBHandle(DB_HANDLE_1) 移入 `if (sig->m_punishFlag)`
//    分支（ORIG 0x0842e155 在分支内获取；原码函数头无条件获取多一次调用）
//    + 声明序改 db6→sig（ORIG db6=-0x24 / sig=-0x20）。
// 余 -22 归 N78b：`char reason[0x100]={'\0'}` 4.1 产 27 条对齐+rep stos 展开，
// 4.4 恒 6 条 `mov $0x40,%edx; rep stos`（三形互斥不可翻）；ORIG 0842deb4
#include "W8LT_DBTask/dispatch_210.inc"
                                             Stream* stream)
{
    MySQL* db6 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                   E_SERVER_GROUP_NONE);
    // 声明序 db6→sig 与 ORIG 一致（ORIG 先 GetDBHandle(6) 后 GetOutBuffer，
    // db6=-0x24/sig=-0x20）；db1 只在 punish_flag 分支内获取（ORIG 0x0842e155）。
    DB_InsertAutoPunishSecondUserView* sig =
        reinterpret_cast<DB_InsertAutoPunishSecondUserView*>(
            stream->GetOutBuffer<SIG_INSERT_AUTO_PUNISH_SECOND_USER>());
    // ORIG 实参序（汇编 0x8..0x1c）：sig+8, sig+0xc, sig+8, sig+0xc,
    // punish_flag, m_id —— 重复传递是 ORIG 原样（多余 vararg 被忽略）。
    db6->set_query(
        "upDate auto_punish_second_user set occ_time=now(),total_trade_cnt=total_trade_cnt+1,trade_cnt=trade_cnt+1,total_trade_gold=total_trade_gold+%d,trade_gold=trade_gold+%d,punish_flag=%d where m_id=%s",
        sig->m_totalTradeGold, sig->m_tradeGold, sig->m_totalTradeGold,
        sig->m_tradeGold, sig->m_punishFlag ? 1 : 0,
        NumberToString(sig->m_memberId, 0));
    // P0：ORIG 无 needInsert 槽（`xor $1; test %al,%al; jne` 直判），具名
    // bool 局部使 4.4 产 `mov %al,-0x1b; cmpb $0x0,-0x1b` 槽往返。
    if (!(db6->exec(true) == 1 && db6->getAffectedRowCount() != 0))
    {
        db6->set_query(
            "inSert into auto_punish_second_user(m_id,occ_time,total_trade_cnt,trade_cnt,total_trade_gold,trade_gold,punish_flag) values(%s,now(),1,1,%d,%d,%d)",
            NumberToString(sig->m_memberId, 0), sig->m_totalTradeGold,
            sig->m_tradeGold, sig->m_totalTradeGold, sig->m_tradeGold,
            sig->m_punishFlag ? 1 : 0);
        db6->exec(true);
    }
    db6->set_query(
        "upDate auto_punish_second_log set occ_time=now(),trade_cnt=trade_cnt+1,trade_gold=trade_gold+%d where m_id=%s and hack_m_id=%s",
        sig->m_totalTradeGold, sig->m_tradeGold,
        NumberToString(sig->m_memberId, 0),
        NumberToString(sig->m_hackMemberId, 1));
    if (!(db6->exec(true) == 1 && db6->getAffectedRowCount() != 0))
    {
        db6->set_query(
            "inSert into auto_punish_second_log(m_id,hack_m_id,occ_time,trade_cnt,trade_gold) values(%s,%s,now(),1,%d)",
            NumberToString(sig->m_memberId, 0),
            NumberToString(sig->m_hackMemberId, 1), sig->m_totalTradeGold,
            sig->m_tradeGold);
        db6->exec(true);
    }
    if (sig->m_punishFlag)
    {
        MySQL* db1 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
        db1->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), end_time='9999-12-31 23:59:59', apply_flag=%d where m_id=%d and punish_type=%d",
            sig->m_punishFlag ? 2 : 0, sig->m_memberId, 4);
        if (!(db1->exec(true) == 1 && db1->getAffectedRowCount() != 0))
        {
            db1->set_query(
                "inSert into member_punish_info(m_id, punish_type, occ_time, start_time, end_time, punish_value, apply_flag) values(%s, %d, now(), now(), '9999-12-31 23:59:59', 0, %d)",
                NumberToString(sig->m_memberId, 0), 4,
                sig->m_punishFlag ? 2 : 0);
            if (db1->exec(true) != 1)
                return 0;
        }
        // ORIG 0x0842e250 mov $0xff,%ebx + 起始槽 -0x123 非 4 对齐 ⇒ 0xff 字节
        // 数组（P312/P253；旧 0x100 使 4.4 折简单 rep stos，缺 21 条慢路）
        char reason[0xff] = { '\0' };
        sprintf(reason, "{Auto 2nd Information-Mail}|*|ARS 2nd Punish (Trade) / %s / %d",
                NumberToString(sig->m_hackMemberId, 0),
                sig->m_totalTradeGold, sig->m_tradeGold);
        db1->set_query(
            "inSert into member_punish_info_history(m_id, punish_type, occ_time, punish_value, start_time, end_time, admin_id, apply_flag, reason) values(%s, 4, now(), 6, now(), '9999-12-31 23:59:59', 'ARS', 2, '%s')",
            NumberToString(sig->m_memberId, 0), reason);
        if (db1->exec(true) != 1)
            return 0;
    }
    return 1;
}

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

// ============================================================================
// DB_ReportMannerlessUser::dispatch —— ORIG 0x0842796a（报告 dispatch.md 全文）
// ============================================================================
// GetDBHandle(4,0) → GetOutBuffer<SIG_MANNERLESS_USER>；函数局部 static
// std::string cache_query（guard + __cxa_atexit 语义，P20/P21）累积
// "(reporter,target,reason,from_unixtime(now))" 元组；长度达 0x6000-0x274
// 上限时 clear；恒返 1（由 DBThread 定期 flush）。
// [b28d-4] DIFF：S 档 gap=32（缺失转写/调用）+ N82 槽序；ORIG 0842796a
#include "W8LT_DBTask/dispatch_211.inc"
                                       Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    DB_MannerlessUserView* sig =
        reinterpret_cast<DB_MannerlessUserView*>(
            stream->GetOutBuffer<SIG_MANNERLESS_USER>());
    static std::string cache_query;
    if (!cache_query.empty())
        cache_query += ",";
    cache_query += "(";
    cache_query += sig->m_reporterId;
    cache_query += ",";
    cache_query += sig->m_targetId;
    cache_query += ",";
    cache_query += sig->m_reason;
    cache_query += ",";
    // ORIG 0x08427add mov $0xff,%ebx（待清字节数 255）+ 起始槽 -0x123 非 4 对齐
    // ⇒ 0xff 字节数组（P312/P253；旧 0x100 使 4.4 折 0x40 dwords 简单 rep stos）
    char timeStr[0xff] = { '\0' };
    // ORIG 0x08427b39..0x08427b48：getCurSec 结果先落具名槽 -0x1c 再回读传参（P0）
    time_t now = GlobalData::s_systemTime_.getCurSec();
    sprintf(timeStr, "from_unixtime(%d)", now);
    cache_query += timeStr;
    cache_query += ")";
    // [b28d-TakeW8LT 块序复原] ORIG 0x08427b9f..08427bc3 是
    // `cmp $0x273,%eax; seta %al; test; je <clear 块>` + 真分支
    // `mov $0x1,%eax; jmp <ret>` ⇒ 源码形 = 正极性提前 return 1（跳过 clear）；
    // 旧形 `< 0x274` 生成 setbe 且 clear 落成直落块（P222/P258 块序反）。
    // 语义等价：!(0x6000-size > 0x273) ⇔ (0x6000-size <= 0x273) ⇔ 旧形 clear 条件。
    if (0x6000U - cache_query.size() > 0x273U)
        return 1;
    cache_query.clear();
    return 1;
}

// ============================================================================
// DB_SaveCharacInformNotice::dispatch —— ORIG 0x0844c170（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(2,0) → GetOutBuffer<SIG_SAVE_CHARAC_INFORM_NOTICE>（null 返
// 0）→ escape_string(+0x04 flags) → upDate charac_option
// charac_inform_notice；affected==0 → inSert（options/best_clear_time 默认
// 空串）；inSert 失败返 0。
#include "W8LT_DBTask/dispatch_212.inc"
                                         Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_CHARAC_INFORM_NOTICE* sig =
        stream->GetOutBuffer<SIG_SAVE_CHARAC_INFORM_NOTICE>();
    if (sig == 0)
        return 0;
    char esc[0x101] = { '\0' };
    // bool r 占 -0x9 系近槽，自然把 db/sig/esc 外推 4B（ORIG db=-0x14、
    // sig=-0x10、esc=-0x115、r=-0x9）。
    db->escape_string(esc,
                      reinterpret_cast<char*>(sig->m_flags));
    db->set_query(
        "upDate charac_option set charac_inform_notice='%s' where charac_no=%u",
        esc, sig->m_characNo);
    // ORIG 0x0844c…：两处 exec 结果经 bool 槽 -0x9 spill+reload：
    // `mov %al,-0x9; movzbl -0x9,%eax; xor $1,%eax; test; je BODY`
    // （4.1 `if(!ok) return 0;` 物化形，技巧8 同族）。
    bool r;
    r = db->exec(true);
    if (!r)
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_option(charac_no, options, best_clear_time, blue_marble_enter_count, charac_inform_notice) values (%u, '%s', '%s', 0, '%s')",
            sig->m_characNo, "", "", esc);
        r = db->exec(true);
        if (!r)
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_SaveGiveAvengerTitleFlag::dispatch —— ORIG 0x08442226（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(9,0) → *stream >> m_id(uint) >> charac_no(uint)；inSert
// event_1107_avenger_plan(m_id, charac_no, occ_date=now())；返 exec==1。
#include "W8LT_DBTask/dispatch_213.inc"
                                           Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    unsigned int characNo = 0;
    *stream >> mId;
    *stream >> characNo;
    db->set_query(
        "inSert into event_1107_avenger_plan(m_id, charac_no, occ_date) values(%s, %u, now())",
        NumberToString(mId, 0), characNo);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_SaveItemDictionaryFlag::dispatch —— ORIG 0x0844547e（报告 dispatch.md
// 全文）
// ============================================================================
// GetOutBuffer<SIG_ITEM_DICTIONARY_FLAG>（null 返 0）→ GetDBHandle(2,0) →
// put_compressed_blob_data(0, +0x04 m_data, 5000)：失败 logFormat(0xac56)；
// upDate charac_item_stat check_flag；affected==0 → inSert（失败 logFormat
// 0xac67）；upDate 失败 logFormat(0xac5e)。
// [b28d-4] DIFF：P183/P171：ORIG 多 {'mov': 1, 'movb': 1, 'movzbl': 1, 'xor': 1}（漏具名局部/去缓存不足）；ORIG 0844547e
#include "W8LT_DBTask/dispatch_214.inc"
                                         Stream* stream)
{
    SIG_ITEM_DICTIONARY_FLAG* sig =
        stream->GetOutBuffer<SIG_ITEM_DICTIONARY_FLAG>();
    if (sig == 0)
        return 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 0x084454c2：`movb $0x0,-0x9` 预清 + call 后 `mov %al,-0x9;
    // movzbl; xor $1; test; je` ⇒ P198① 具名 bool 两句式（callee 返回 bool）。
    bool ok = false;
    char* blob = 0;
    ok = put_compressed_blob_data(db, 0, reinterpret_cast<char*>(sig->m_data),
                                  5000, &blob);
    if (!ok)
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xac56,
            "DB_SaveItemDictionaryFalg::dispatch compress_zip(%u) ERROR charac_no=%u",
            0, sig->m_characNo);
        return 0;
    }
    db->set_query(
        "upDate charac_item_stat set check_flag = '%s' where charac_no=%u",
        blob, sig->m_characNo);
    // ORIG 0x08445584：`xor $1; test %al; je <affected 检查>` 早返形
    // （bool callee 的 ==1 被 4.4 折叠，P292/P101）。
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xac5e,
            "DB_SaveItemDictionaryFalg->upDate Query ERROR charac_no=%u",
            sig->m_characNo);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_item_stat(charac_no, check_flag) values(%u, '%s')",
            sig->m_characNo, blob);
        if (!db->exec(true))
        {
            LogManager::logFormat(
                1, "DBThread.cpp",
                __PRETTY_FUNCTION__,
                0xac67,
                "DB_SaveItemDictionaryFalg->inSert Query ERROR charac_no=%u",
                sig->m_characNo);
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_UpdateClientCrashDownLog::dispatch —— ORIG 0x08427bce（报告 dispatch.md
// 全文）
// ============================================================================
// *stream >> reason(int) → GetInstanceClientCrashDownLogStatistic()->
// RegisterClientCrashDown(reason)；恒返 1。
#include "W8LT_DBTask/dispatch_215.inc"
                                           Stream* stream)
{
    int reason;
    *stream >> reason;
    GetInstanceClientCrashDownLogStatistic()->RegisterClientCrashDown(reason);
    return 1;
}

// ============================================================================
// DB_UpdateInGameEventHistory::dispatch —— ORIG 0x08444972（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(9,0) → *stream >> m_id(uint) >> type(int) >> history1(uint)
// >> history2(uint)；upDate event_ingame_history；exec!=1 或 affected==0 →
// inSert 同键；恒返 1。
#include "W8LT_DBTask/dispatch_216.inc"
                                           Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    // ORIG 仅 mId(-0x20) 带清零，其余 lea 直传引用无初始化。
    int type;
    unsigned int history1;
    unsigned int history2;
    *stream >> mId;
    *stream >> type;
    *stream >> history1;
    *stream >> history2;
    db->set_query(
        "upDate event_ingame_history set history_1 = %u,history_2 = %u where m_id=%s and type = %d",
        history1, history2, NumberToString(mId, 0), type);
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into event_ingame_history(m_id,type,history_1,history_2) values(%s,%d,%d,%d)",
            NumberToString(mId, 0), type, history1, history2);
        db->exec(true);
    }
    return 1;
}

// ============================================================================
// DB_LogIngameAdvertisement::dispatch —— ORIG 0x0844c304（报告 dispatch.md
// 全文）
// ============================================================================
// GetDBHandle(4,0) → GetOutBuffer<SIG_IN_GAME_ADVERTISEMENT_LOG>（null 返
// 0）→ inSert log_in_game_ad 10 列（state=+0x08 字节、view_code=+0x0a short、
// type_code=+0x0c 字节、banner_id=+0x10、dungeon/village/area=+0x14/18/1c）；
// 返 exec==1。
#include "W8LT_DBTask/dispatch_217.inc"
                                         Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_IN_GAME_ADVERTISEMENT_LOG* sig =
        stream->GetOutBuffer<SIG_IN_GAME_ADVERTISEMENT_LOG>();
    if (sig == 0)
        return 0;
    bool ok;   // ORIG 0x0844…：exec 结果物化 byte 槽 -0x19
    db->set_query(
        "inSert into log_in_game_ad(occ_date, m_id, charac_no, state, view_code, type_code, banner_id, dungeon_index, village_index, area_index) values (now(), %u, %u, %d, %d, %d, %u, %d, %d, %d)",
        sig->m_accId, sig->m_characNo, (int)(char)sig->m_type,
        (int)sig->m_adType, (int)(char)sig->m_flag, sig->m_extra,
        sig->m_dungeonIdx, sig->m_village, sig->m_pad1c);
    ok = db->exec(true);
    if (!ok)
        return 0;
    return 1;
}

// ============================================================================
// 本批落地（OnlyOrigBatch3）：
//   DB_UpdateOnlinePreliminaryMember::dispatch        ORIG T 0x0843ba9e
//   DB_UpdateOnlinePreliminaryMemberMatchList::dispatch ORIG T 0x0843baa8
//   DBTask::~DBTask                                   ORIG T 0x083fbfda
//   DB_UpdateDeathTowerBestRecord（占位见下，行为按报告补齐）
// ============================================================================

#include "W8LT_DBTask/dispatch_218.inc"
{
    // ORIG 0x0843ba9e：4 条指令 mov $1; ret（纯成功占位，无载荷处理）。
    return true;
}

#include "W8LT_DBTask/dispatch_219.inc"
{
    // ORIG 0x0843baa8：4 条指令 mov $1; ret（纯成功占位，无载荷处理）。
    return true;
}

#include "W8LT_DBTask/dtor_DBTask.inc"
{
    // ORIG 0x083fbfda：仅调 UnregistDispatcher（报告 DBTask/~DBTask.md）。
    UnregistDispatcher();
}

// ============================================================================
// DB_UpdateDeathTowerBestRecord（ORIG T 0x0842a35e makeRequest / 0x0842a11e dispatch；
// 报告 DB_UpdateDeathTowerBestRecord/，语义按报告转写）
// ============================================================================

#include "W8LT_DBTask/makeRequest_45.inc"
    unsigned int characNo, unsigned int accId, unsigned int memberCount,
    stDeathTowerRecordMemberInfo_t const* members, unsigned int stage, unsigned int playTime)
{
    // ORIG：StreamPool::Acquire(s_stream_pool, "DBThread.cpp", 0x6bfe)
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6bfe), true);
    guard.operator*()->operator<<((int)0xe3);
    guard.operator*()->operator<<((int)-1);
    SIG_DEATH_TOWER_UPDATE_BEST_RECORD* sig =
        guard.operator->()->GetInBuffer<SIG_DEATH_TOWER_UPDATE_BEST_RECORD>();
    sig->m_accId = accId;
    sig->m_towerIndex = static_cast<unsigned short>(characNo);
    sig->m_memberCount = static_cast<unsigned short>(memberCount & 0xff);
    memcpy(sig->m_members, members, memberCount * 0x17);   // ORIG imul 直接用原值
    sig->m_stage = stage;
    sig->m_playTime = playTime;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// [b28d-4] DIFF：S 档 gap=37（缺失转写/调用）+ N82 槽序；ORIG 0842a11e
#include "W8LT_DBTask/dispatch_220.inc"
{
    // ORIG：GetOutBuffer<SIG> + GetDBHandle(2,0) + upDate/inSert 双查询。
    SIG_DEATH_TOWER_UPDATE_BEST_RECORD* sig =
        stream->GetOutBuffer<SIG_DEATH_TOWER_UPDATE_BEST_RECORD>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    // [b28d-TakeW8LT 形复原] ORIG 0x0842a11e **无** memberCount/membersStr 具名局部：
    // gcc 对变参从右到左求值，ORIG 0x0842a158..a1c8 先倒序读 9 个 sig 字段
    // （m_stage@0x64→%edi、m_memberCount@0x6→-0x4c、m_towerIndex@0x4→-0x48、
    // m_accId@0x0→-0x44、m_memberCount→-0x40、m_playTime@0x68→-0x3c、
    // m_memberCount→-0x38、m_stage→%esi、m_memberCount→%ebx），再在实参 0xc 位
    // 置调 blob_to_str（0x0842a1de）⇒ 源码形 = blob_to_str 内联为 set_query 实参，
    // m_memberCount 逐点重取（ORIG movzwl 26 次 vs 旧形缓存后 6 次）。
    db->set_query(
        "upDate charac_tower_record set member_info_%d='%s',stage_%d=%d,play_time_%d=%d,"
        "occ_time_%d=now() where charac_no=%d and tower_index=%d and stage_%d<=%d",
        sig->m_memberCount,
        db->blob_to_str(0, sig->m_members, (unsigned int)sig->m_memberCount * 0x17u),
        sig->m_memberCount, sig->m_stage, sig->m_memberCount, sig->m_playTime,
        sig->m_memberCount, sig->m_accId, sig->m_towerIndex, sig->m_memberCount,
        sig->m_stage);
    // ORIG 0x0842a24e..a277：`exec==1 && affected!=0` 短路求值后物化到 %eax
    // （mov $0x1,%eax / mov $0x0,%eax 两分支，无 bool 局部栈槽；旧形用
    //  needInsert 局部 ⇒ 多发 movb/cmpb 栈槽往返）⇒ 源码形 = 单个取反表达式。
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into charac_tower_record(charac_no,tower_index,member_info_%d,stage_%d,"
            "play_time_%d,occ_time_%d) values(%d,%d,'%s',%d,%d,now())",
            sig->m_memberCount, sig->m_memberCount, sig->m_memberCount,
            sig->m_memberCount, sig->m_accId, sig->m_towerIndex,
            db->blob_to_str(0, sig->m_members, (unsigned int)sig->m_memberCount * 0x17u),
            sig->m_stage, sig->m_playTime);
        db->exec(true);
    }
    return true;
}

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
{
    // ORIG 0x0843babc：4 条指令 mov $1; ret（纯成功占位，无载荷处理）。
    return true;
}

#include "W8LT_DBTask/dispatch_222.inc"
{
    // ORIG 0x0843bad0：4 条指令 mov $1; ret（纯成功占位，无载荷处理）。
    return true;
}

#include "W8LT_DBTask/makeRequest_46.inc"
{
    // ORIG 0x08441c2a：StreamPool::Acquire("DBThread.cpp", 0xa14c) →
    // guard << 0x287 << -1 << accId << qty → MsgQueueMgr::put(2, guard)。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa14c), true);
    guard.operator*()->operator<<((int)0x287);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(qty);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

#include "W8LT_DBTask/makeRequest_47.inc"
                                                   unsigned int characNo)
{
    // ORIG 0x08441fde：Acquire("DBThread.cpp", 0xa1b1) → 0x288/-1/accId/qty/characNo → put(2)。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa1b1), true);
    guard.operator*()->operator<<((int)0x288);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<(accId);
    guard.operator*()->operator<<(qty);
    guard.operator*()->operator<<(characNo);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

#include "W8LT_DBTask/dispatch_223.inc"
{
    // ORIG 0x08441a8e：GetDBHandle(9,0) → >>u1 >>u2 → 表名 get_db_table_name(0x199)
    //   → upDate 加量（m_id 匹配）→ exec 失败返 0 → 影响行数 0 则 inSert → exec 失败返 0 → 返 1。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    unsigned int u1 = 0;
    unsigned int u2 = 0;
    stream->operator>>(u1);
    stream->operator>>(u2);
    std::string table(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_db_table_name(0x199));
    db->set_query(
        "upDate %s set fatigue_quantity=fatigue_quantity+%d where m_id=%s",
        toTString(table), u2, NumberToString(u1, 0));
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into %s(m_id, fatigue_quantity) values(%s, %d)",
            toTString(table), NumberToString(u1, 0), u2);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

#include "W8LT_DBTask/dispatch_224.inc"
{
    // ORIG 0x08441d1c：event_script_mng 空返 0；实体 0x65 为空或
    //   !CUsedFatigueGiveItem::getIsCharacEvent(charac) 返 0；>>u1 >>u2 >>u3 后按
    //   charac 选 DB_HANDLE_9/DB_HANDLE_2 与 m_id/charac_no 双查询，exec 失败返 0。
    // b28c-34 判词（N7 封顶，推断）：
    //   ①charac inSert 分支实参序修正为 values(charac_no=u2, fatigue_quantity=u3)：
    //     ORIG 0x0841f56 `mov -0x24(%ebp),%esi; mov -0x20(%ebp),%ebx` ⇒ 0xc(%esp)=u2、
    //     0x10(%esp)=u3，与列序 (charac_no, fatigue_quantity) 一致（旧码 u3,u2 为真反）。
    //   ②evt 声明为 CUsedFatigueGiveItem*（赋值处 static_cast）以复现 ORIG 0x0841d64
    //     调用点 `lea -0x15(%ebp),%eax; mov %eax,0x4(%esp); mov -0x10(%ebp),%eax` 的
    //     「&charac 先装载」实参序；表达式内 static_cast 形 4.4 会反转装载序（探针
    //     /tmp/gf_probe2.cpp t5/t7 反形 vs t2/t3/t6 正形）。
    //   ③残余 NEAR 仅 N7（结构性，不可复现）：ORIG 0x0841df1 虚目标 `mov (%eax),%edx`
    //     + this 重载 `mov -0x10(%ebp),%eax; call *%edx`；4.4 -O0 对间接 call 目标恒
    //     %eax、this 恒 %edx（探针 /tmp/gf_probe3.cpp t1 成员调用/t2 引用/t3 解引用/t4
    //     丢弃全为 `call *%eax`）。已试形：成员调用、(*evt) 解引用、引用形、丢弃形；
    //     槽位 +0x20 与 ORIG vtable slot 8 一致（非 P193 选错虚函数）。语义与 ORIG 等价。
    //   实测：TU 级 compare 由 DIFF→NEAR（190/190 insn、gap=0），ndiff 16→8（残余
    //   8 行全部为上述 N7 寄存器名差）；探针 TU /tmp/gf_before.cpp 复刻修改前体得
    //   ndiff_before=16（口径 audit_tu_ledger：AE 归一+<T>/<A>+尾 nop 豁免）。
    EventClassify::CEventScriptMng* mng =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
    if (mng == 0)
        return 0;
    EventClassify::CUsedFatigueGiveItem* evt =
        static_cast<EventClassify::CUsedFatigueGiveItem*>(mng->get_event_entity(0x65));
    bool charac = false;
    if (evt == 0 || !evt->getIsCharacEvent(charac))
        return 0;
    unsigned int u1 = 0;
    unsigned int u2 = 0;
    unsigned int u3 = 0;
    stream->operator>>(u1);
    stream->operator>>(u2);
    stream->operator>>(u3);
    MySQL* db = 0;
    std::string table(evt->get_db_table_name());
    // ORIG 0x08441e0e-0x08441e13 `xor $1;test;je <DB_2块>`：charac==1 →
    // DB_2/charac_no 查询、charac==0 → DB_9/m_id（getIsCharacEvent 语义
    // 为“走角色维度”，前批分支写反，本次按 ORIG 汇编修正）。
    if (!charac)
    {
        db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
        db->set_query(
            "upDate %s set fatigue_quantity=%u where m_id=%s",
            toTString(table), u3, NumberToString(u1, 0));
    }
    else
    {
        db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
        db->set_query(
            "upDate %s set fatigue_quantity=%u where charac_no=%u",
            toTString(table), u3, u2);
    }
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        if (!charac)
        {
            db->set_query(
                "inSert into %s(m_id, fatigue_quantity) values(%s, %u)",
                toTString(table), NumberToString(u1, 0), u3);
        }
        else
        {
            db->set_query(
                "inSert into %s(charac_no, fatigue_quantity) values(%u, %u)",
                toTString(table), u2, u3);
        }
        if (!db->exec(true))
            return 0;
    }
    return 1;
}
// ============================================================================
// DB_UpdateReturnUserFirstLogin::dispatch —— ORIG T 0x0843dffe（报告
// docs/class_func_reports/DB_UpdateReturnUserFirstLogin/dispatch.md）。
// GetDBHandle(DB_HANDLE_9,0) → >>m_id(u32) >>expire(int) >>first_login(int) →
// INSERT；exec 失败且 getDBError()==0x426 时改 UPDATE（update 成功也返 0，
// ORIG 汇编 0x843e133 恒置 0）。
// ============================================================================
#include "W8LT_DBTask/dispatch_225.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    int expire_time = 0;
    int first_login = 0;
    *stream >> m_id;
    *stream >> expire_time;
    *stream >> first_login;
    db->set_query(
        "inSert into return_user(m_id, expire_time, first_login) values(%s, from_unixtime(%d), %d)",
        NumberToString(m_id, 0), first_login, expire_time);
    if (!db->exec(true))
    {
        if (db->getDBError() == 0x426)
        {
            db->set_query(
                "upDate return_user set expire_time = from_unixtime(%d), first_login=%d where m_id=%s",
                first_login, expire_time, NumberToString(m_id, 0));
            if (!db->exec(true))
                return 0;
        }
        return 0;
    }
    return 1;
}

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

// ============================================================================
// DB_AutoPunishedSecondUserData::dispatch —— ORIG 0x0843a462
// 查询 auto_punish_second_user：无数据发 0x88f1 失败包返 0；有数据发 0x8901
// 包（trade_cnt+1、trade_gold 直传）返 1。出站头 0x1ca 前导 m_id。
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 0843a462
#include "W8LT_DBTask/dispatch_226.inc"
                                             Stream* stream)
{
    // [H4 2026-09-11] P171 死存储照抄：ORIG 0x0843a46a `movb $0x0,-0x19(%ebp)`
    // 位于函数首条（该槽全函数只写不读）⇒ 原码函数头遗留的 bool 声明预清。
    bool deadFlag = false;
    (void)deadFlag;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    DB_AutoPunishedSecondUserReqView* req =
        reinterpret_cast<DB_AutoPunishedSecondUserReqView*>(
            stream->GetOutBuffer<SIG_SECOND_USER_DATA>());
    db->set_query(
        "seLect trade_cnt,trade_gold from auto_punish_second_user where m_id=%s",
        NumberToString(req->m_mId, 0));
    // ORIG：exec 失败或 rows==0 → 失败路径（0x88f1 包，return 0）。
    if (!(db->exec(true) && db->get_n_rows() != 0))
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x88f1), true);
        guard.operator*()->operator<<(0x1ca);
        guard.operator*()->operator<<(req->m_tradeCnt);
        DB_AutoPunishedSecondUserSetView* out =
            reinterpret_cast<DB_AutoPunishedSecondUserSetView*>(
                guard->GetInBuffer<SIG_SECOND_USER_DATA_SET>());
        memset(out, 0, sizeof(*out));
        // ORIG 0x0843a4fd 失败分支逐字段（report dispatch.md）：
        // out+0x14=req+0x10、out+0=req goldLo、out+0x4=req goldHi、
        // out+0x8=0、out+0xc=0、out+0x10=1（punished 硬编码 1，非 req）。
        out->m_mId = req->m_punished;
        // [K5W8LTDBTask W8DiagD 负结论] u64 别名拷贝旋钮（P246/P92）实测无变化
        // （our 215 未动、四数 141/1750/12/57 逐值相同）：4.4 对
        // `*(unsigned long long*)&a = *(unsigned long long*)&b` 与逐字段赋值同码。
        // ORIG 0x0843a5b3-0x0843a5b6 的 6 条 8 字节拷贝属 4.1 后端，不可迁。回滚。
        out->m_goldLo = req->m_goldLo;
        out->m_goldHi = req->m_goldHi;
        out->m_tradeCnt = 0;
        out->m_pad0c = 0;
        out->m_punished = 1;
        // ORIG 0x0843a5e8(+0x186)：put 队列 = DISPATCH_Q(1)，非 DB_Q(2)
        // （两分支同；见 report dispatch.md 反汇编 movl $0x1,0x4(%esp)）。
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 0;
    }
    db->fetch();
    // P171/P328⑤：ORIG 此处（Acquire 之后）无 `movl $0x0,-0x1c/-0x24/-0x28`
    // 预清（裸声明只占槽、不发指令；两值随后由 get_uint/get_ulonglong 写入）。
    unsigned int tradeCnt;
    unsigned long long tradeGold;
    db->get_uint(0, tradeCnt);
    db->get_ulonglong(1, tradeGold);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8901), true);
    guard.operator*()->operator<<(0x1ca);
    guard.operator*()->operator<<(req->m_tradeCnt);
    DB_AutoPunishedSecondUserSetView* out =
        reinterpret_cast<DB_AutoPunishedSecondUserSetView*>(
            guard->GetInBuffer<SIG_SECOND_USER_DATA_SET>());
    memset(out, 0, sizeof(*out));
    // ORIG 0x0843a4fd 成功分支逐字段（report dispatch.md）：
    // out+0x14=req+0x10、out+0=req goldLo、out+0x4=req goldHi、
    // out+0x8=tradeGold 低 32（DB col1）、out+0xc=tradeGold 高 32、
    // out+0x10=tradeCnt+1。
    out->m_mId = req->m_punished;
    // [J4S3-15 P246] ORIG 0x0843a715 一次 8 字节拷贝（`mov 0x8(%eax),%edx;
    // mov 0x4(%eax),%eax; mov -0x10(%ebp),%ecx; mov %eax,(%ecx); mov %edx,0x4(%ecx)`）、
    // 0x0843a723 一次 8 字节写 ⇒ 视图整体拷贝（out+0/+4 与 req goldLo/goldHi 同布局）。
    *(unsigned long long*)&out->m_goldLo = *(unsigned long long*)&req->m_goldLo;
    *(unsigned long long*)&out->m_tradeCnt = tradeGold;
    out->m_punished = tradeCnt + 1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_SaveProperDungeonClearCount::dispatch —— ORIG 0x0843a8b2
// SIG_SAVE_BLOOD_CLEAR_COUNT{dungeonIdx, subNo}：先 UPDATE event_dungeon_clear，
// 受影响行数为 0 时改 INSERT。exec 失败返 0，成功返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_227.inc"
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_BLOOD_CLEAR_COUNT* sig =
        stream->GetOutBuffer<SIG_SAVE_BLOOD_CLEAR_COUNT>();
    db->set_query(
        "upDate event_dungeon_clear set clear_cnt=%d, update_time=now() where charac_no=%u",
        sig->m_subNo, sig->m_dungeonIdx);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_dungeon_clear(charac_no, clear_cnt, update_time) values(%u, %d, now())",
            sig->m_dungeonIdx, sig->m_subNo);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// CLocalChina_DB_LastDay_BreakAway::dispatch —— ORIG 0x0812801a
// 读 accId、flag、day；flag==0 → quest_time 回退 day 天；flag==1 →
// last_reward_time 回退 day 天。失败 log 返 0。
// ============================================================================
#include "W8LT_DBTask/dispatch_228.inc"
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 0x0812801a 帧：db(-0xc) id(-0x10) day(-0x14) flag(-0x18)；
    // 本 TU 实测（DeleteChannel/此函数双探针）4.4 对「声明即初始化」
    // 局部按声明序近→远落槽 ⇒ 声明序 id,day,flag（技巧21 按实证方向）。
    unsigned int m_id = 0;
    int m_day = 0;
    int m_flag = 0;
    *stream >> m_id;
    *stream >> m_flag;
    *stream >> m_day;
    // ORIG 0x08128097-0x08128097 switch(int) 两 case 链形：`mov flag,%eax;
    // test; je A; cmp $1,%eax; je B; jmp END`（4.1 线性 case 下探）。4.4 的
    // if/else-if 产 test+jne+二次装载形 ⇒ 直接写 switch。
    switch (m_flag)
    {
    case 0:
        db->set_query(
            "UPDATE churn_member_info \t\t\t\t  SET quest_time = UNIX_TIMESTAMP(SUBDATE(FROM_UNIXTIME(quest_time), INTERVAL %d DAY)) \t\t\t\t   WHERE m_id=%s",
            m_day, NumberToString(m_id, 0));
        break;
    case 1:
        db->set_query(
            "UPDATE churn_member_info \t\t\t\t  SET last_reward_time = UNIX_TIMESTAMP(SUBDATE(FROM_UNIXTIME(last_reward_time), INTERVAL %d DAY)) \t\t\t\t   WHERE m_id=%s",
            m_day, NumberToString(m_id, 0));
        break;
    }
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localchina/localchina_DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x42f, "[GM]CLocalChina_DB_LastDay_BreakAway Fail!!");
        return 0;
    }
    return 1;
}

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

// [b28d-solo-3] DIFF(残 4/ORIG 211)：三缓冲 `={'\0'}` 复现 0x800/0x3b/0x1ff 慢路
// （P312）+ P198 sig 两段清零 + P101 早返已修；余 +0x16/+0x1a 两处字段两段装载
// （P254 档③：ORIG `mov disp,%eax; mov %eax,%esi/ebx`，4.4 单段直落）。ORIG 084276b0。
#include "W8LT_DBTask/dispatch_229.inc"
                                               Stream* stream)
{
    // ORIG 0x084276bc `movl $0x0,-0x20(%ebp)` 先清零再赋值（P198/P314 两段形）。
    Packet_Mannerless_User_Accusation* sig = 0;
    sig = stream->GetOutBuffer<Packet_Mannerless_User_Accusation>();
    if (sig == 0)
        return 0;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    // [b28d-solo] P185/P171：ORIG 0x08427701 `lea -0x24(%ebp),%eax; call time`
    // （结果未被 sprintf 使用，死调用照抄）；sql 缓冲 = 0x800 字节
    // （0x0842770c `lea -0xa5e` + `mov $0x800,%ebx` 清零计数实证，非 lea 偏移）。
    time_t occTime;
    time(&occTime);
    (void)occTime;
    // P312/P202①：`= {'\0'}` 复现 4.1 对齐剥离慢路（ORIG 0x0842770c 起）。
    char sql[0x800] = { '\0' };
    char escapedTargetName[0x3b] = { '\0' };
    db->escape_string(escapedTargetName, sig->m_targetName);

    if (sig->isNeedTypeForChattingMsg(sig->m_serverGroup))
    {
        // ORIG 0x084277e6 `mov $0x1ff,%ebx`（-0x25e 起）⇒ 0x1ff 字节。
        char escapedChatMsg[0x1ff] = { '\0' };
        db->escape_string(escapedChatMsg, sig->m_chatMsg);
        sprintf(sql,
                "values(%d, %s, %d, '%s', %d, '%s', now(), '%s')",
                sig->m_serverGroup, NumberToString(sig->m_accIdHi, 0),
                (unsigned int)sig->m_reporterGroup, sig->m_name, (int)sig->m_accIdLo,
                escapedTargetName, escapedChatMsg);
    }
    else
    {
        sprintf(sql,
                "values(%d, %s, %d, '%s', %d, '%s', now(), 'NULL')",
                sig->m_serverGroup, NumberToString(sig->m_accIdHi, 0),
                (unsigned int)sig->m_reporterGroup, sig->m_name, (int)sig->m_accIdLo,
                escapedTargetName);
    }

    db->set_query(
        "inSert into charac_accusation(accusation_type, m_id, server_id, charac_name, server_id_accu, charac_name_accu, occ_time, chatting_msg) %s",
        sql);
    // ORIG 0x08427952 `xor $1,%eax; test %al,%al`（P101：bool==1 折叠）⇒ 早返形。
    if (!db->exec(true))
        return 0;
    return 1;
}

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

// [b28d-solo-2R] AE：三旋钮组合——①P102/P198① 共享具名 bool 局部（`bool ok; ok=call(); if(!ok)`
//   ⇒ 复现 ORIG `mov %al,-0x9; movzbl -0x9,%eax` bool 槽物化 ×4）；
//   ②P183 去 DB_TagCharacSigView* v 缓存（4 处内联 reinterpret 重读 sig ⇒ 复现 `mov -0x10,%eax; mov (%eax)`）；
//   ③P227/P221 具名 cMyTrace→全表达式临时链（复现 ORIG 四 trace 槽 -0x50/-0x40/-0x30/-0x20 方向与
//   characNo 跨 ctor %ebx 保活）。原 DIFF(114)→0。ORIG 0x844768c
#include "W8LT_DBTask/dispatch_230.inc"
{
    SIG_REQUEST_TAG_CHARAC_INFO* sig =
        stream->GetOutBuffer<SIG_REQUEST_TAG_CHARAC_INFO>();
    bool ok;
    ok = DB_LoadCharac::_GetTagCharacInfo(
        reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo,
        &reinterpret_cast<DB_TagCharacSigView*>(sig)->m_tagCharac);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb21b, 5)
            ("DB_RequestTagChracInfo::_GetTagCharacInfo fail, characNo(%d)",
             reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo);
        return 0;
    }
    ok = DB_LoadInventory::_GetTagCharacInven(
        reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo,
        &reinterpret_cast<DB_TagCharacInvenView*>(sig)->m_tagInven);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb223, 5)
            ("DB_RequestTagChracInfo::_GetTagCharacInven fail, characNo(%d)",
             reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo);
        return 0;
    }
    ok = DB_LoadInventory::_GetUserAvatar(
        reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo,
        &reinterpret_cast<DB_TagCharacAvatarView*>(sig)->m_avatarDetail,
        reinterpret_cast<DB_TagCharacAvatarView*>(sig)->m_avatarItems,
        reinterpret_cast<DB_TagCharacAvatarView*>(sig)->m_equipItems,
        reinterpret_cast<DB_TagCharacAvatarView*>(sig)->m_avatarItemCnt);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb22c, 5)
            ("DB_RequestTagCharacInfo::_GetUserAvatar fail, characNo(%d)",
             reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo);
        return 0;
    }
    ok = DB_LoadEtc::_LoadQuestShop(
        reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo,
        &reinterpret_cast<DB_TagCharacSigView*>(sig)->m_questShop);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb234, 5)
            ("DB_RequestTagCharacInfo::_LoadQuestShop fail, characNo(%d)",
             reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo);
        return 0;
    }

    sendResult(slot, sig);
    return 1;
}

// ============================================================================
// DB_SaveCharacSlotExtend::dispatch —— ORIG 0x084420ea
// upDate charac_view（slot_effect_count/slot_charac_limit/m_id）；成功条件 =
// exec && affectedRowCount!=0，否则 cMyTrace 失败日志返 0。
// ============================================================================

// [b28d-4] DIFF：P210/N82：帧继承差（ORIG push/pop callee-saved vs 4.4 leave/槽化）；ORIG 084420ea
// [L4-r1] cMyTrace 改 P227 全表达式临时链 ⇒ ndiff 58（持平；余 push/pop 差来自
//   NumberToString 三实参跨 call 保活，属 P52/N7 族）。
#include "W8LT_DBTask/dispatch_231.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    SaveCharacSlotView* sig = reinterpret_cast<SaveCharacSlotView*>(
        stream->GetOutBuffer<SIG_EXTEND_CHARAC_SLOT>());
    db->set_query(
        "upDate charac_view set slot_effect_count=%u, charac_slot_limit=%u where m_id=%s",
        (unsigned int)sig->m_slotEffect, (unsigned int)sig->m_slotLimit,
        NumberToString(sig->m_mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        // [M8DBB 2026-09-11 试] P329 具名实参中介内联回实参位（ORIG 0x084421b9
        // `mov %eax,%ebx` / 0x084421c2 `movzbl %al,%edi` / 0x084421cc `movzbl
        // %al,%esi` = 三实参跨 ctor 保活在 callee-saved ⇒ 无具名槽）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa1f5, 5)(
            "upDate charac_view set slot_effect_count(%u), charac_slot_limit(%u) failed at m_id(%u)",
            (unsigned int)sig->m_slotEffect, (unsigned int)sig->m_slotLimit,
            NumberToString(sig->m_mId, 0));
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_SaveEventLevelupSupport::dispatch —— ORIG 0x0844cafe
// stream >> 3×int；DB_HANDLE_2（NULL 返 0）→ upDate event_levelup_support；
// exec 失败返 0；affectedRowCount==0 补 inSert，exec 失败返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_232.inc"
                                          Stream* stream)
{
    int characNo = 0;
    int level = 0;
    int state = 0;
    *stream >> characNo;
    *stream >> level;
    *stream >> state;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    db->set_query(
        "upDate event_levelup_support set state = %d where charac_no = %d and level = %d",
        state, characNo, level);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into event_levelup_support(charac_no, level, state) values(%d, %d, %d)",
            characNo, level, state);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_SaveGameOption_ChattingEmoticon::dispatch —— ORIG 0x08430412
// DB_HANDLE_6 → put_compressed_blob_data(sig+4, 0x210) 失败返 0 → upDate %s
// set shortcut_emoticon（get_member_game_option 表名）；exec 失败或
// affectedRowCount==0 补 inSert，exec 失败返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_233.inc"
                                                  Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    ChattingEmoticonView* sig = reinterpret_cast<ChattingEmoticonView*>(
        stream->GetOutBuffer<SIG_SAVE_CHATTING_EMOTICON>());
    char* blob = 0;
    if (!put_compressed_blob_data(
        db, 0, (char*)((unsigned long)sig + 4UL), 0x210, &blob))
        return 0;
    db->set_query(
        "upDate %s set shortcut_emoticon='%s' where m_id=%s",
        get_member_game_option(), blob, NumberToString(sig->m_mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into %s(m_id, shortcut_emoticon) values(%s,'%s')",
            get_member_game_option(), NumberToString(sig->m_mId, 0), blob);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_SaveGameOption_QuickChatting::dispatch —— ORIG 0x08430250
// DB_HANDLE_6 → compress_zip(buf,&len,sig+4,0x258) 失败返 0 → blob_to_str(1) →
// upDate %s set option_3；exec 失败或 affectedRowCount==0 补 inSert
// （blob_to_str(0)），exec 失败返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_234.inc"
                                               Stream* stream)
{
    static const int compressedBufferSize_QuickChat = 0x2dc;  // ORIG _ZZ…QuickChatting…E30compressedBufferSize_QuickChat
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    QuickChattingView* sig = reinterpret_cast<QuickChattingView*>(
        stream->GetOutBuffer<SIG_SAVE_GAME_OPTION_QUICKCHAT>());
    // ORIG 0x08430250：`lea -0x300,%ebx; mov $0xb7; rep stos` 内联清零
    // （P4 = 声明初始化数组形，4.4 对 `= {0}` 亦内联）；尺寸 0x2dc。
    char buf[0x2dc] = { 0 };
    unsigned int compressedLen = compressedBufferSize_QuickChat;
    if (!compress_zip(buf, compressedLen, sig->m_data, 0x258))
        return 0;
    db->set_query(
        "upDate %s set option_3='%s' where m_id=%s",
        get_member_game_option(), db->blob_to_str(1, buf, (int)compressedLen),
        NumberToString(sig->m_mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into %s(m_id, option_3) values(%s,'%s')",
            get_member_game_option(), NumberToString(sig->m_mId, 0),
            db->blob_to_str(0, buf, (int)compressedLen));
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_SavePcRoomPlayTimeEvent::dispatch —— ORIG 0x08447992
// DB_HANDLE_9 → 表名 get_db_table_name(0x5c)（std::string 视图经 toTString）→
// upDate %s set play_time/reward_cnt；exec 失败 logFormat(0xb25b) 返 0；
// affectedRowCount==0 补 inSert，exec 失败 logFormat(0xb266) 返 0；成功返 1。
// （makeRequest 已定义于 W8LT_DBTask.cpp。）
// ============================================================================

#include "W8LT_DBTask/dispatch_235.inc"
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_PC_ROOM_PLAY_TIME* sig =
        stream->GetOutBuffer<SIG_SAVE_PC_ROOM_PLAY_TIME>();
    std::string tableName(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_db_table_name(0x5c));

    db->set_query(
        "upDate %s set play_time = %u, reward_cnt = %u, occ_date = now() where m_id = %s",
        toTString(tableName), sig->m_totalTime, (unsigned int)sig->m_rewardCnt,
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xb25b,
            "<PCROOM_PLAY_TIME_EVENT> update_error m_id(%d), total_time(%d), reward_cnt(%d)",
            sig->m_accId, sig->m_totalTime, (unsigned int)sig->m_rewardCnt);
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into %s(m_id, play_time, reward_cnt, occ_date) values(%s,%u,%u,now())",
            toTString(tableName), NumberToString(sig->m_accId, 0),
            sig->m_totalTime, (unsigned int)sig->m_rewardCnt);
        if (!db->exec(true))
        {
            LogManager::logFormat(
                1, "DBThread.cpp",
                __PRETTY_FUNCTION__,
                0xb266,
                "<PCROOM_PLAY_TIME_EVENT> insert_error m_id(%d), total_time(%d), reward_cnt(%d)",
                sig->m_accId, sig->m_totalTime, (unsigned int)sig->m_rewardCnt);
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_SecurityCardAddCertCnt::makeRequest —— ORIG 0x08436388（static）
// Acquire("DBThread.cpp",0x8199) → CStreamGuard(stream,true) → <<0x17c → <<-1
// → GetInBuffer<SIG_SECURITY_CARD_ADD_CERT_CNT>() → sig[0]=accId →
// put(DB_Q)。
// ============================================================================

#include "W8LT_DBTask/makeRequest_48.inc"
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8199), true);
    guard.operator*()->operator<<(0x17c);
    guard.operator*()->operator<<(-1);
    SIG_SECURITY_CARD_ADD_CERT_CNT* sig =
        guard->GetInBuffer<SIG_SECURITY_CARD_ADD_CERT_CNT>();
    sig->m_mId = accId;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

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

// [b28d-4] DIFF：N18a/P211：4.4 多 {'mov': 2} 中转 mov（单段直落/同型化待验）；ORIG 0842c362
#include "W8LT_DBTask/dispatch_236.inc"
                                         Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    SecuRetransferReqView* sig = reinterpret_cast<SecuRetransferReqView*>(
        stream->GetOutBuffer<SIG_SECURITY_CARD_RETRANSFER>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect re_issue_cnt,unix_timestamp(last_issue_time), phone from member_security_card where m_id=%s",
        NumberToString(sig->m_mId, 0));
    try
    {
        if (!db->exec(true))
            throw (ENUM_ERROR)1;
        if (!db->fetch())
            throw (ENUM_ERROR)1;
        // P171/P328⑤：ORIG 无预清（两值随后由 get_uint 写入）。
        // [H4 2026-09-11 复核] ORIG 0x0842c43e/0x0842c445 虽有 `movl $0x0,
        // -0x1c/-0x20(%ebp)`，但给本两值加 `= 0` 实测**反劣**（ours 272→274、
        // 差异行 93→188）⇒ 那两条清零属其它语义（非本两值声明初始化）⇒ 维持无预清。
        unsigned int reIssueCnt;
        unsigned int lastIssueTime;
        db->get_uint(0, reIssueCnt);
        db->get_uint(1, lastIssueTime);
        char phone[0x29];
        memset(phone, 0, sizeof(phone));
        db->get_str(2, phone, 0x29);
        if (strncmp(sig->m_phone, phone, 0x29) != 0)
            throw (ENUM_ERROR)0x7d;

        // [H4 2026-09-11 复核] 试删 `time_t cur` 副本（改用 &curSec 直传）实测**反劣**
        // （ours 272→270、差异行 93→211）⇒ 维持 `time_t cur` 具名副本（4.4 槽分配联动）。
        int curSec = GlobalData::s_systemTime_.getCurSec();
        time_t last = (time_t)lastIssueTime;
        time_t cur = (time_t)curSec;
        struct tm tLast;
        struct tm tCur;
        localtime_r(&last, &tLast);
        localtime_r(&cur, &tCur);
        if (tLast.tm_yday == tCur.tm_yday)
        {
            reIssueCnt += 1;
            if (reIssueCnt > 3)
                throw (ENUM_ERROR)0x78;
        }
        else
        {
            reIssueCnt = 0;
        }

        // [H4 2026-09-11 语义轴·照抄] ORIG 0x0842c5ed..0x0842c60f 实证实参序为
        //   `lea 0x2b(%ecx),%esi; lea 0x4(%ecx),%ebx; add $0x25,%ecx` →
        //   `mov %ecx,0x8(%esp)`（第1个 %s=cert_key）/ `mov %ebx,0xc(%esp)`
        //   （server_key）/ `mov %esi,0x10(%esp)`（card）
        // ⇒ ORIG 传 (+0x25, +0x4, +0x2b)，即 m_card/m_certKey/m_serverKey 的次序
        //（与列名字面语义错位，Ghidra C 亦为 `local_1c+0x25, local_1c+4,
        //  local_1c+0x2b`）——按 P171 原码行为照抄原则复现，勿"按语义纠正"。
        // 同函数 `_sendResult(slot, sig+0x52, sig+0x4, sig+0x2b, 0)` 保持不变。
        db->set_query(
            "upDate member_security_card set cert_key='%s',server_key='%s',card='%s',fail_cnt=0,cancel_cnt=0,re_issue_cnt=%d,last_issue_time=now(),validity_time=0,apply_flag=1,cert_flag=0,cert_cnt=0 where m_id=%s",
            sig->m_card, sig->m_certKey, sig->m_serverKey, reIssueCnt,
            NumberToString(sig->m_mId, 0));
        db->exec(true);
        _sendResult(slot, sig->m_serial, sig->m_certKey, sig->m_serverKey,
                    (ENUM_ERROR)0);
    }
    // [2026-09-11 真 bug 修复] ORIG 4 处 throw 的 typeinfo 均为 `_ZTI10ENUM_ERROR`
    // （0x0842c42e / 0x0842c47a / 0x0842c538 / 0x0842c5be 均 `movl $0x8b75f40,0x4(%esp)`，
    //  nm: _ZTI10ENUM_ERROR @0x08b75f40）；我方原为 4×`_ZTIi`（throw int + catch(int)）
    //  ⇒ 异常类型错、跨模块不可接。按 ORIG 改为 throw/catch (ENUM_ERROR)。
    catch (ENUM_ERROR err)
    {
        _sendResult(slot, 0, 0, 0, err);
    }

    MySQL* dbGrade = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
    dbGrade->set_query(
        "upDate member_security_grade set security_card_reg=now() where m_id=%s",
        NumberToString(sig->m_mId, 0));
    if (!(dbGrade->exec(true) && dbGrade->getAffectedRowCount() != 0))
    {
        dbGrade->set_query(
            "inSert into member_security_grade(m_id, security_card_reg) values(%s, now())",
            NumberToString(sig->m_mId, 0));
        if (!dbGrade->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_SecurityCardUpdateFailCnt::makeRequest —— ORIG 0x0842bfaa（static）
// Acquire("DBThread.cpp",0x6f0b) → CStreamGuard(stream,true) → <<0x101 → <<-1
// → GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>() → sig[0]=accId、
// sig[4]=failCnt → put(DB_Q)。
// ============================================================================

#include "W8LT_DBTask/makeRequest_49.inc"
                                               unsigned int failCnt)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6f0b), true);
    guard.operator*()->operator<<(0x101);
    guard.operator*()->operator<<(-1);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        guard->GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    sig->m_mId = accId;
    sig->m_cnt = failCnt;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_SelectLevelUpReward::dispatch —— ORIG 0x084423cc
// DB_HANDLE_2 → >>characNo(uint) >>rewardIdx(int) → 表名 get_db_table_name
// (0x194) → seLect recv_level；exec 失败返 0；rows==0 → inSert；有行 fetch/
// get_ubyte 失败返 0；recvLevel==rewardIdx → logFormat(0xa2a5) 返 0；否则
// upDate recv_level（exec 失败返 0）→ 组包（0xa2ba）：<<userIdx <<slot
// <<rewardIdx <<recvLevel → put(DISPATCH_Q) 返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_237.inc"
{
    static const int handleIdx = 2;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    int rewardIdx = 0;
    *stream >> characNo;
    *stream >> rewardIdx;
    std::string tableStr(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_db_table_name(0x194));

    db->set_query("seLect  recv_level from %s where charac_no = %u",
                  toTString(tableStr), characNo);
    if (!db->exec(true))
        return 0;

    int rows = db->get_n_rows();
    unsigned char recvLevel = 0;
    if (rows != 0)
    {
        if (!db->fetch())
            return 0;
        if (!db->get_ubyte(0, recvLevel))
            return 0;
        if (recvLevel == rewardIdx)
        {
            LogManager::logFormat(
                1, "DBThread.cpp",
                __PRETTY_FUNCTION__,
                0xa2a5,
                "DB_SelectLevelUpReward error char:%u lv:%u recv_lv:%u",
                characNo, rewardIdx, (unsigned int)recvLevel);
            return 0;
        }
        db->set_query(
            "upDate %s set recv_level=%u,occ_time=now() where charac_no=%u",
            toTString(tableStr), rewardIdx, characNo);
        if (!db->exec(true))
            return 0;
    }
    else
    {
        db->set_query(
            "inSert into %s(charac_no,recv_level,occ_time) values (%u,%u,now() )",
            // ORIG 0x84425b3-0x84425c8：`mov -0x18,%esi; mov -0x14,%ebx; …
            // mov %esi,0x10(%esp); mov %ebx,0xc(%esp)`（-0x14=characNo→arg2，
            // -0x18=rewardIdx→arg3）—— 列序 (charac_no,recv_level) 对应实参
            // (characNo, rewardIdx)；此前写反属语义 bug。
            toTString(tableStr), characNo, rewardIdx);
        if (!db->exec(true))
            return 0;
    }

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa2ba), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    guard.operator*()->operator<<(rewardIdx);
    guard.operator*()->operator<<((int)recvLevel);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_StatisticCeraShopGiftLog::dispatch —— ORIG 0x08444066
// DB_HANDLE_4（NULL 返 0）→ inSert log_cerashop_gift（6 个 uint 直传）；exec
// 失败 cMyTrace(0xa9c1/0xa9c3) 返 0；成功返 1。
// ============================================================================

// [b28d-4] DIFF：P183/P171：ORIG 多 {'mov': 6}（漏具名局部/去缓存不足）；ORIG 08444066
#include "W8LT_DBTask/dispatch_238.inc"
                                           Stream* stream)
{
    CeraShopGiftLogView* sig = reinterpret_cast<CeraShopGiftLogView*>(
        stream->GetOutBuffer<SIG_STATISTIC_CERASHOP_GIFT_LOG>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
        return 0;
    db->set_query(
        "inSert into log_cerashop_gift(occ_time, from_mid, from_charac_no, to_mid, to_charac_no, item_idx, item_cnt) values(unix_timestamp(), %u, %u, %u, %u, %u, %u)",
        sig->m_fromMid, sig->m_fromCharacNo, sig->m_toMid, sig->m_toCharacNo,
        sig->m_itemIdx, sig->m_itemCnt);
    if (!db->exec(true))
    {
        // [SweepIntervalSet] P227 全表达式临时链：ORIG 0x08444137/0x0844414f 两处
        // 都用 ctor 返回的 this 调 operator()（`mov %eax,(%esp)`，非 lea 槽地址）；
        // 且第二处在 ctor **之前**把 6 个字段读入 %edi/%esi/%ebx + 3 个临时槽
        // （0x08444152..0x08444179），正是 6 个实参跨 ctor 保活的形。
        cMyTrace(__PRETTY_FUNCTION__, 0xa9c1, 5)(
            "DB_StatisticAmazingBoxProtectLucky_USE inSert exec fail");
        cMyTrace(__PRETTY_FUNCTION__, 0xa9c3, 5)(
            "inSert into log_cerashop_gift(occ_time, from_mid, from_charac_no, to_m_id, to_charac_no, item_idx, count) values(unix_timestamp(), %u, %u, %u, %u, %u, %u)",
            sig->m_fromMid, sig->m_fromCharacNo, sig->m_toMid,
            sig->m_toCharacNo, sig->m_itemIdx, sig->m_itemCnt);
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_TOD_Random_Select_UserAPC::dispatch —— ORIG 0x084400b8
// 空实现，恒返 1（ORIG 仅 push/mov $1/leave/ret 四指令）。
// ============================================================================

#include "W8LT_DBTask/dispatch_239.inc"
                                            Stream* stream)
{
    // ORIG 0x84400b8：sub $0xb0 空帧 + mov $1 + leave/ret（P31 未用局部压帧，
    // 实测 char[0xa4..0xb0] → sub $0xb0）。
    char buf[0xa8];
    (void)buf;
    // [N8W8LT P139] ORIG nm 有 prett 局部串（0x08c5ba00, sz 0x47：`virtual bool
    // DB_TOD_Random_Select_UserAPC::dispatch(int, int, Stream*)`）而体无引用 ⇒ 原码有被
    // 丢弃的引用；sizeof 形只发射该串、零指令变化（探针实证）。
    (void)sizeof(__PRETTY_FUNCTION__);
    return 1;
}

// ============================================================================
// DB_UpdateAuraAvatarOption::makeRequest —— ORIG 0x0844f26：
// 发 0x2af、-1、uid、characNo、idx、value 到 DB_Q。
// ============================================================================
#include "W8LT_DBTask/makeRequest_50.inc"
                                            int idx, int value)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xabf6), true);
    // ORIG 0x0844f26：写 0x2af、-1、characNo(无符号)、idx、value 五字段；
    // uid 参数存在（符号 4 参）但 ORIG 不写入包。
    guard.operator*()->operator<<((int)0x2af);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(characNo);
    guard.operator*()->operator<<(idx);
    guard.operator*()->operator<<(value);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ============================================================================
// DB_UpdateAuraAvatarOption::dispatch —— ORIG 0x08444e1e
// DB_HANDLE_2 → >>characNo(uint) >>optionType(int) >>value(int) → upDate
// aura_avatar_option；exec 失败或 affectedRowCount==0 补 inSert（exec 结果
// 忽略）→ 恒返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_240.inc"
                                         Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    unsigned int characNo = 0;
    // ORIG 仅 -0x10（characNo）带清零；optionType/value 直接 lea 传引用。
    int optionType;
    int value;
    *stream >> characNo;
    *stream >> optionType;
    *stream >> value;
    db->set_query(
        "upDate aura_avatar_option set value_1=%d where charac_no=%u and option_type = %d",
        value, characNo, optionType);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into aura_avatar_option(charac_no,option_type,value_1) values(%u,%d,%d)",
            characNo, optionType, value);
        db->exec(true);
    }
    return 1;
}

// ============================================================================
// DB_UpdateBlueMarbleEnterCount::dispatch —— ORIG 0x0844874e
// DB_HANDLE_2 → >>characNo(uint) >>enterCnt(uchar) → upDate charac_option；
// exec 失败返 0；affectedRowCount==0 补 inSert，exec 失败返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_241.inc"
                                             Stream* stream)
{
    unsigned int characNo = 0;
    unsigned char enterCnt = 0;
    *stream >> characNo;
    *stream >> enterCnt;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate charac_option set blue_marble_enter_count = %u where charac_no = %u",
        (unsigned int)enterCnt, characNo);
    if (!db->exec(true))
        return 0;
    if (db->getAffectedRowCount() == 0)
    {
        db->set_query(
            "inSert into charac_option(charac_no, blue_marble_enter_count) values(%u, %u)",
            characNo, (unsigned int)enterCnt);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateBreakAwayAccruedCera::dispatch —— ORIG 0x0843865c
// DB_HANDLE_6 → >>m_id(uint) >>cera(uint) → upDate churn_member_info
// accrue_cera 累加；exec 失败 logFormat(0x8619) 返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_242.inc"
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    unsigned int cera = 0;
    *stream >> mId;
    *stream >> cera;
    db->set_query(
        "upDate churn_member_info set accrue_cera= accrue_cera+%u where m_id=%s",
        cera, NumberToString(mId, 0));
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x8619, "BREAK_AWAY : UPDATE CERA ERROR!!");
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateBreakAwayClearAddInfo::dispatch —— ORIG 0x0843848c
// DB_HANDLE_6 → >>m_id(uint) → upDate churn_member_info set add_info=-1；
// exec 失败或 affectedRowCount==0 → logFormat(0x85f4) 返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_243.inc"
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    *stream >> mId;
    db->set_query(
        "upDate churn_member_info set add_info=-1 where m_id=%s",
        NumberToString(mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x85f4, "BREAK_AWAY : ADD INFO UPDATE ERROR!!");
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateBreakAwayDungeonClearCnt::dispatch —— ORIG 0x084383ba
// DB_HANDLE_6 → >>m_id(uint) >>clearCnt(int) → upDate churn_member_info
// set add_info=%d；exec 失败或 affectedRowCount==0 返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_244.inc"
                                                 Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    int clearCnt = 0;
    *stream >> mId;
    *stream >> clearCnt;
    db->set_query(
        "upDate churn_member_info set add_info=%d where m_id=%s",
        clearCnt, NumberToString(mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        return 0;
    return 1;
}

// ============================================================================
// DB_UpdateBreakAwayRuckPoint::dispatch —— ORIG 0x08438300
// DB_HANDLE_6 → >>m_id(uint) → upDate churn_member_info set luck_point=0；
// exec 失败或 affectedRowCount==0 返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_245.inc"
                                           Stream* stream)
{
    // ORIG 0x8438300 槽位：mId(-0x14)/db(-0x10)/dead(-0xc)；+0x2d 处
    // `movl $0x0,-0xc` 后有零槽但后续未再引用（NST 结果直推），
    // 以未用局部复现（-O0 无 DCE）。
    unsigned int mId;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    mId = 0;
    char* dead = 0;
    (void)dead;
    *stream >> mId;
    db->set_query(
        "upDate churn_member_info set luck_point=0 where m_id=%s",
        NumberToString(mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        return 0;
    return 1;
}

// ============================================================================
// DB_UpdateBreakAwayUvList::dispatch —— ORIG 0x08438842
// DB_HANDLE_6 → curSec=getCurSec() → >>m_id(uint) → GetOutBuffer<
// SIG_BREAK_AWAY_UV_LIST>（play_info 字符串）→ upDate churn_member_info
// play_info/last_update_time；exec 失败或 affectedRowCount==0 返 0；成功返 1。
// （makeRequest 定义于 Dispatchers.cpp 域，本 TU 不重复。）
// ============================================================================

#include "W8LT_DBTask/dispatch_246.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    unsigned int mId = 0;
    *stream >> mId;
    SIG_BREAK_AWAY_UV_LIST* sig =
        stream->GetOutBuffer<SIG_BREAK_AWAY_UV_LIST>();
    db->set_query(
        "upDate churn_member_info set play_info ='%s', last_update_time=%u where m_id=%s",
        (char*)sig, curSec, NumberToString(mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        return 0;
    return 1;
}

// ============================================================================
// DB_UpdateDungeonDataStatisticLogdb::dispatch —— ORIG 0x0842456a
// 空实现，恒返 1（ORIG 仅 push/mov $1/pop/ret 四指令）。
// ============================================================================

#include "W8LT_DBTask/dispatch_247.inc"
                                                  Stream* stream)
{
    return 1;
}

// ============================================================================
// DB_UpdateExpertJobStatistic::dispatch —— ORIG 0x08436964
// DB_HANDLE_4 → >>count(uint)（0 则返 1）→ 循环 count 次：>>type(int)
// >>itemCnt(ulong) >>itemId(uint)；itemCnt==0 break；upDate
// expert_job_product_cnt（item_cnt 累加）；exec 失败或 affectedRowCount==0 补
// inSert；循环结束返 1（恒）。
// ============================================================================

#include "W8LT_DBTask/dispatch_248.inc"
                                           Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    unsigned int count = 0;
    *stream >> count;
    if (count == 0)
        return 1;

    for (unsigned int i = 0; i < count; ++i)
    {
        int expertJobType = 0;
        unsigned long itemCnt = 0;
        unsigned int itemId = 0;
        *stream >> expertJobType;
        *stream >> itemCnt;
        *stream >> itemId;
        if (itemCnt == 0)
            break;
        db->set_query(
            "upDate expert_job_product_cnt set item_cnt=item_cnt+%d where expert_job_type=%d and occ_date=curdate() and item_id=%d",
            itemId, expertJobType, itemCnt);
        if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        {
            db->set_query(
                "inSert into expert_job_product_cnt(expert_job_type,occ_date,item_id,item_cnt) values(%d,curdate(),%d,%d)",
                expertJobType, itemCnt, itemId);
            db->exec(true);
        }
    }
    return 1;
}

// ============================================================================
// DB_UpdateItemDropStatistics::dispatch —— ORIG 0x084285ae
// DB_HANDLE_4 → >>channelNo(int) >>dropType(int) → 4×6 个 uint 数组
// （stackable/equip/recipe/artifact）从 stream 读 → inSert log_item_drop
// （26 个 %d 直传）→ 返 exec(true) 结果（ORIG 无失败分支）。
// ============================================================================

// [SweepW8LT b28d] AE：P214③ 四数组 `{0,0,0,0,0,0}`→`{0}` 复现 4.1/4.4
// 循环清零展开（11 条/数组），gap 20→0（234/234）；ORIG 084285ae
#include "W8LT_DBTask/dispatch_249.inc"
                                           Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int channelNo = 0;
    int dropType = 0;
    // P214③：单元素 `{0}` 触发 4.1/4.4 循环清零展开（11 条/数组：and $0xfffffffc
    // + 4 字节步进回跳）；写满 6 个 0 则退化为 6 条 movl（gap 20 的根因）。
    unsigned int stackable[6] = { 0 };
    unsigned int equip[6] = { 0 };
    unsigned int recipe[6] = { 0 };
    unsigned int artifact[6] = { 0 };

    *stream >> channelNo;
    *stream >> dropType;
    for (int i = 0; i <= 5; ++i)
        *stream >> stackable[i];
    for (int i = 0; i <= 5; ++i)
        *stream >> equip[i];
    for (int i = 0; i <= 5; ++i)
        *stream >> recipe[i];
    for (int i = 0; i <= 5; ++i)
        *stream >> artifact[i];

    db->set_query(
        "inSert into log_item_drop(occ_date,channel_no,drop_type,stackable_common,stackable_uncommon,stackable_rare,stackable_unique,stackable_epic,stackable_chronicle,equip_common,equip_uncommon,equip_rare,equip_unique,equip_epic,equip_chronicle,recipe_common,recipe_uncommon,recipe_rare,recipe_unique,recipe_epic,recipe_chronicle,artifact_common,artifact_uncommon,artifact_rare,artifact_unique,artifact_epic,artifact_chronicle) values(cast(now() as date),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
        channelNo, dropType,
        stackable[0], stackable[1], stackable[2], stackable[3], stackable[4],
        stackable[5],
        equip[0], equip[1], equip[2], equip[3], equip[4], equip[5],
        recipe[0], recipe[1], recipe[2], recipe[3], recipe[4], recipe[5],
        artifact[0], artifact[1], artifact[2], artifact[3], artifact[4],
        artifact[5]);
    return db->exec(true);
}

// ============================================================================
// DB_UpdateMemberDoubtTrade::dispatch —— ORIG 0x084284a2
// >>m_id(uint) → DB_HANDLE_6 → curSec=getCurSec() → upDate member_doubt_trade
// （over_count+1）；exec 失败或 affectedRowCount==0 补 inSert（over_count=1），
// exec 失败返 0；成功返 1。
// ============================================================================

#include "W8LT_DBTask/dispatch_250.inc"
                                         Stream* stream)
{
    unsigned int mId;
    *stream >> mId;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    db->set_query(
        "upDate member_doubt_trade set last_update_time=from_unixtime(%d),over_count=over_count+1 where m_id=%s",
        curSec, NumberToString(mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into member_doubt_trade(m_id,last_update_time,over_count) values(%s,from_unixtime(%d),1)",
            NumberToString(mId, 0), curSec);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_UpdateNPCRelationship::dispatch —— ORIG 0x08435e1e
// DB_HANDLE_2 → GetOutBuffer<SIG_UPDATE_NPC_RELATIONSHIP> → blob_to_str(0,
// sig+8, npcCnt*12) → upDate charac_npc（npc_cnt/npc_data/charac_no）→
// exec 结果忽略，恒返 1。
// ============================================================================

// [b28d-solo-3] AE：P231 裸 register 缓存 characNo（ORIG `mov (%eax),%ebx` 跨
// blob_to_str call 保活）+ blob_to_str 结果内联进 set_query 实参（P0 无 blob 槽）。
// ORIG 08435e1e。
#include "W8LT_DBTask/dispatch_251.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    UpdateNpcRelationView* sig = reinterpret_cast<UpdateNpcRelationView*>(
        stream->GetOutBuffer<SIG_UPDATE_NPC_RELATIONSHIP>());
    // 残留（N 类）：ORIG 0x08435e1e `mov (%eax),%ebx` npcCnt 入 callee-saved
    // %ebx 跨 call 保活（push %ebx/sub $0x34 帧）；4.4 具名槽外推 4B、
    // register asm("ebx") 反产 spill 槽+双段 mov（_SaveExpandEquipslot 同症）。
    register int characNo = sig->m_characNo;
    db->set_query(
        "upDate charac_npc set npc_cnt=%d,npc_data='%s' where charac_no=%d",
        sig->m_npcCnt,
        db->blob_to_str(0, sig->m_npcData, sig->m_npcCnt * 12),
        characNo);
    db->exec(true);
    return 1;
}


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

// ORIG T __GetCurrentDNFDate(struct tm&) @0x086b1fc0（定义 GameUtils.cpp）
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

// ============================================================================
// CLocalGlobal_DB_STATICS_BOSSTOWER::dispatch —— ORIG 0x0812825e
// GetDBHandle(DB_HANDLE_4,0) → >>keyCount（<=0 返 0）→ __GetCurrentDNFDate +
// sprintf("%04d-%02d-%02d") 日期 → 循环 keyCount 次：>>key(u32) +
// GetOutBuffer<STBossDungeonTowerLog>(+0 play_cnt/+4 success_cnt/+8 uniq_cnt/
// +0xc play_time) → UPDATE log_monstertower_layer_stat；exec 失败或受影响行数
// ==0 时改 INSERT（key 高 4 位打包 layer/monster_diff/charac_job/charac_grow/
// level，ORIG 逐字节分解）。INSERT exec 结果丢弃。恒返 1。
// ============================================================================
// [b28d-4] DIFF：P219/P248：字节字段物化/同型化差（ORIG movzbl vs 4.4 mov/shr/and）；ORIG 0812825e
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

// ============================================================================
// DB_DeathTowerLoadTopRanker::dispatch —— ORIG 0x0842a484
// >>reload(byte) → GetDBHandle(DB_HANDLE_2,0) → 回包流 0xe4/-1 +
// SIG_DEATH_TOWER_LOAD_TOP_RANKER{+0 count=0, +4 reload}。双层循环
// partType 0..4 × rank 1..4：reload==0 时先 count(*) 得 total（>5 时
// interval=max(total/0x5f,1)）再查 rank>5 且 rank%interval==0 的行（limit 95）；
// reload!=0 时查 top5 表（limit 5）。每段 m_count += get_n_rows()，逐行
// fetch 后写 rank(+0x06+i*2)、tower_index(+0xfa0+..+0xc)、member_info
// (+0xfa0+..+0x10,0x17)、stage/play_time(+0x1000+..+0xc/+0x10)、
// type=rank(+0xfa0+..+0xe)。任一步失败返 0（不发包）；成功 put(DISPATCH_Q)。
// ============================================================================
// [SweepW8LT b28d] DIFF：gap 11→7（our 301 / ORIG 308）。**真 bug 修复**：ORIG
// 842a702 先存旧 m_count（-0x14）再 `m_count += get_n_rows()`，k 从旧值起（只遍历
// 本次新增行）；原码 k=0 起会重复覆盖历史行。另 interval 声明序前移（ORIG
// `movl $0x1,-0x10` 在 total=0 之前）。p167 29/29 [0缺0多]（C1/C2 别名）。余 -7
// 归 N82 帧（0x80 vs 0x70）+ 槽号差（interval -0x40 vs -0x10）。ORIG 0842a484
#include "W8LT_DBTask/dispatch_253.inc"
{
    unsigned char reload = 1;
    *stream >> reload;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x6c21), true);
    guard.operator*()->operator<<(0xe4);
    guard.operator*()->operator<<(-1);
    // [Lane31W8LTDBTaskView] 权威头直取（去 cast）。
    SIG_DEATH_TOWER_LOAD_TOP_RANKER* out =
        guard->GetInBuffer<SIG_DEATH_TOWER_LOAD_TOP_RANKER>();
    out->m_count = 0;
    out->m_flag = reload;
    // ORIG 0x0842a8e6 `cmp $0x4,%eax; setbe`（无符号上界，P2/P310）
    // [F2e-Singles2 轴B 2026-09-10 实测回滚] 试 `int partType` 复现 ORIG 0842a89a
    // `cmpl $0x4,-0x18(%ebp); setle`：setle 确已翻正（我方 -0x18 槽），但 our
    // 309→**310**（+1 条）、diff 451→452 行 ⇒ 净劣，按判据回滚。值域 0..4 无
    // 符号分叉（纯 codegen 形态差）⇒ 维持 N96/N18a 封顶（内层 rank 两段寄存器形同）。
    // （值域 0..4 恒非负 ⇒ 非 P196 分叉，属 codegen 轴，守闸门；内外两循环已穷举。）
    for (unsigned int partType = 0; partType <= 4; ++partType)
    {
        // [A04-r5j · 轴 B **已试无效**] ORIG 0x0842a89a 为 `cmpl $0x4,-0x18(%ebp); setle;
        // test; jne`（有符号）而我方 setbe；按轴 B 把 rank 改 int 后 **ndiff 117→117、
        // our 309→309（零改善、零劣化）** ⇒ 按协议**回滚**（该点 setcc 差非此局部所致，
        // 归 N82 帧-槽族残差），保持 unsigned。
        for (unsigned int rank = 1; rank <= 4; ++rank)
        {
            if (reload == 0)
            {
                // ORIG 842a702 先存旧 m_count（-0x14），再 `m_count += get_n_rows()`，
                // k 从**旧值**起（只遍历本次新增行）；原码 k=0 起会重复覆盖历史行。
                unsigned int interval = 1;
                unsigned int total = 0;
                db->set_query(
                    "seLect count(*) from charac_tower_rank where tower_index=%d and part_type=%d",
                    partType, rank);
                if (!db->exec(true))
                    return 0;
                if (!db->fetch())
                    return 0;
                if (!db->get_uint(0, total))
                    return 0;
                if (total > 5)
                {
                    // 真 bug 修正：ORIG 0x0842a6a2 段魔数 0x58ed2309+shr$6
                    // = total / 0x5f（Ghidra C `local_40 / 0x5f`）；旧码写 /100
                    // 使采样间隔差 5%，rank%interval 命中行完全不同。
                    interval = total / 0x5f;
                    interval = std::max(interval, (unsigned int)1);
                }
                db->set_query(
                    "seLect b.rank,a.tower_index, a.member_info_%d,a.stage_%d,a.play_time_%d from charac_tower_record a,charac_tower_rank b where b.tower_index=%d and b.tower_index=a.tower_index and (b.rank>5 and (b.rank%%%d)=0) and a.charac_no=b.charac_no and b.part_type=%d order by b.rank asc limit %d",
                    rank, rank, rank, partType, interval, partType, 0x5f);
            }
            else
            {
                db->set_query(
                    "seLect b.rank,a.tower_index, a.member_info_%d,a.stage_%d,a.play_time_%d from charac_tower_record a,charac_tower_rank_top5 b where b.tower_index=%d and b.tower_index=a.tower_index and b.rank<=5 and a.charac_no=b.charac_no and b.part_type=%d order by b.rank asc limit %d",
                    rank, rank, rank, partType, partType, 5);
            }
            if (!db->exec(true))
                return 0;
            unsigned int oldCount = out->m_count;
            out->m_count += db->get_n_rows();
            for (unsigned int k = oldCount; k < out->m_count; ++k)
            {
                if (!db->fetch())
                    return 0;
                // P183：ORIG 每次字段访问重发完整 `lea+add` 地址链（无具名指针
                // 缓存；缓存 3 个指针多占 0x10 帧并省物化次数）
                if (!db->get_ushort(
                        0, reinterpret_cast<DB_TowerRankerRankView*>(
                               (char*)out + 6)->m_ranks[k]))
                    return 0;
                if (!db->get_ushort(
                        1, reinterpret_cast<DB_TowerRankerRowView*>(
                               (char*)out + 0xfa0 + k * 0x6c)->m_towerIndex))
                    return 0;
                if (!db->get_binary(
                        2, reinterpret_cast<DB_TowerRankerRowView*>(
                               (char*)out + 0xfa0 + k * 0x6c)->m_memberInfo,
                        0x17))
                    return 0;
                if (!db->get_uint(
                        3, reinterpret_cast<DB_TowerRankerPlayView*>(
                               (char*)out + 0x1000 + k * 0x6c)->m_stage))
                    return 0;
                if (!db->get_uint(
                        4, reinterpret_cast<DB_TowerRankerPlayView*>(
                               (char*)out + 0x1000 + k * 0x6c)->m_playTime))
                    return 0;
                reinterpret_cast<DB_TowerRankerRowView*>(
                    (char*)out + 0xfa0 + k * 0x6c)->m_type =
                    (unsigned short)rank;
            }
        }
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_EventAccountCharacterLevelUpOnceGift::dispatch —— ORIG 0x08449e3e
// GetDBHandle(DB_HANDLE_9,0) → >>m_id(u32) >>serverGroup(short) >>
// characNo(int) >>jobType(short) >>level(short) → inSert
// event_1208_level_achieve（%s m_id / server_id / charac_no / job_type /
// level，now()）。exec 失败返 0，成功返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_254.inc"
                                                       Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    short serverGroup = 0;
    int characNo = 0;
    short jobType = 0;
    short level = 0;
    *stream >> m_id;
    *stream >> serverGroup;
    *stream >> characNo;
    *stream >> jobType;
    *stream >> level;
    db->set_query(
        "inSert into event_1208_level_achieve(m_id ,server_id, charac_no, job_type, level, occ_date) values(%s, %d, %d, %d, %d, now())",
        NumberToString(m_id, 0), (int)serverGroup, characNo, (int)jobType,
        (int)level);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_EventSaveGiveGrowCreature::dispatch —— ORIG 0x08449a58
// GetDBHandle(DB_HANDLE_9,0) → >>m_id(u32) → GetOutBuffer<eventCharacInfo>
// （+0 serverGroup ushort / +4 characNo / +8 job / +0xc name[30]）→ inSert
// event_1208_growthcreature：server_id=+0x8(job)、charac_no=+0x4、
// job_type=+0x0(serverGroup)、charac_name=+0xc（SQL 原样不转义）。
// exec 失败返 0，成功返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_255.inc"
                                            Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    *stream >> m_id;
    SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo* sig =
        stream->GetOutBuffer<SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo>();
    db->set_query(
        "inSert into event_1208_growthcreature(m_id ,server_id, charac_no, job_type, charac_name, occ_date) values(%s, %d, %d, %d, '%s', now())",
        // ORIG 实参序：server_id=(int)(short)+0x0（ebx 落 0xc）、charac_no=+0x4、
        // job_type=+0x8 int（右起先求值 edi←mov 0x8）；此前误将 m_job 置于 server_id。
        NumberToString(m_id, 0), (int)(signed short)sig->m_serverGroup,
        sig->m_characNo, sig->m_job, sig->m_name);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_EventUpdateGiveGrowCreature::dispatch —— ORIG 0x08449c54
// GetDBHandle(DB_HANDLE_9,0) → >>m_id(u32) → GetOutBuffer<eventCharacInfo>
// → upDate event_1208_growthcreature：charac_name=+0xc、m_id=%s、
// server_id=+0x0(serverGroup)、charac_no=+0x4。exec 失败返 0，成功返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_256.inc"
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    *stream >> m_id;
    SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo* sig =
        stream->GetOutBuffer<SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo>();
    db->set_query(
        "upDate event_1208_growthcreature set charac_name = %s) where m_id = %s and server_id =%d and charac_no = %d",
        sig->m_name, NumberToString(m_id, 0),
        (int)(signed short)sig->m_serverGroup, sig->m_characNo);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_EventSaveGrowthWeapon::dispatch —— ORIG 0x084492fc
// GetDBHandle(DB_HANDLE_9,0) → >>m_id(u32) >>serverGroup(short) → inSert
// event_1207_growthweapon(m_id, server_id, occ_date)。exec 失败 →
// logFormat 0xb8e6 → 返 0；成功返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_257.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    short serverGroup = 0;
    *stream >> m_id;
    *stream >> serverGroup;
    db->set_query(
        "inSert into event_1207_growthweapon(m_id, server_id, occ_date) values(%s, %d, now())",
        NumberToString(m_id, 0), (int)serverGroup);
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xb8e6, "DB_EventSaveGrowthWeapon Query Error : m_id(%d)", m_id);
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_EventUpdateGrowthWeaponCharac::dispatch —— ORIG 0x084494e2
// GetDBHandle(DB_HANDLE_9,0) → >>m_id(u32) >>characNo(u32) >>
// growthweaponId(int) >>serverGroup(short) → upDate event_1207_growthweapon
// set occ_date=now(), charac_no=%d, growthweapon_id=%d, server_id=%d
// where m_id=%u。exec 失败返 0，成功返 1。
// ============================================================================
// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
#include "W8LT_DBTask/dispatch_258.inc"
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    unsigned int characNo = 0;
    // [b28d-solo] P167：ORIG +0x6d 第三个读取是 operator>>(int&)（差集
    // -1 rsERi/+1 rsERj 实证）；short 无清零（ORIG 仅三条 movl $0）。
    int growthweaponId = 0;
    short serverGroup;
    *stream >> m_id;
    *stream >> characNo;
    *stream >> growthweaponId;
    *stream >> serverGroup;
    db->set_query(
        "upDate event_1207_growthweapon set occ_date = now(), charac_no = %d, growthweapon_id = %d, server_id = %d where m_id = %u",
        characNo, growthweaponId, (int)serverGroup, m_id);
    if (!db->exec(true))
        return 0;
    return 1;
}

// ============================================================================
// DB_GetDeathTowerBestRecord::dispatch —— ORIG 0x08429ad0
// 先调基类 DBDispatcher::dispatch(userIdx, slot, stream)，失败返 0。→
// >>characNo(u32) → GetDBHandle(DB_HANDLE_2,0) → seLect tower_index +
// 4×member_info/stage/play_time from charac_tower_record where charac_no=%d
// limit 5；exec 失败或无行返 0。回包流 0xe2/slot + SIG_DEATH_TOWER_GET_
// BEST_RECORD{+0 count=get_n_rows, +4 characNo}。逐行 fetch：tower_index/
// member_info_i/stage_i/play_time_i 写入行视图（行步长 0x1b0，type 标记
// 1..4）。任一步失败返 0；成功 put(DISPATCH_Q) 返 1。
// （N 封顶：ndiff 458 残差为 imul/add 地址重物化(P183) + 帧布局，见报告
// dispatch.md）
// ============================================================================
// [b28d-4] DIFF：S 档 gap=43（缺失转写/调用）+ N82 槽序；ORIG 08429ad0
#include "W8LT_DBTask/dispatch_259.inc"
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;
    // ORIG 0x08429b09 直接 `lea -0x18(%ebp),%eax; call Stream::operator>>`，
    // **无零初始化**（值完全由 >> 写入）。旧形多发 1 条 `movl $0x0,-0x18(%ebp)`
    // ⇒ 其后每条指令整体错位一行 ⇒ 356 行函数按位 diff 全线翻红（这就是本 TU
    // 大件 ndiff 被推到 300~900 的主因：1 条多余指令 × 全帧位移）。
    unsigned int characNo;
    *stream >> characNo;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect tower_index, \t\tmember_info_1,stage_1,play_time_1, \t\tmember_info_2,stage_2,play_time_2, \t\tmember_info_3,stage_3,play_time_3, \t\tmember_info_4,stage_4,play_time_4 \t\tfrom charac_tower_record where charac_no=%d limit %d",
        characNo, 5);
    if (!(db->exec(true) && db->get_n_rows() != 0))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6bb2), true);
    guard.operator*()->operator<<(0xe2);
    guard.operator*()->operator<<(slot);
    // [Lane31W8LTDBTaskView] 权威头直取（去 cast）。
    SIG_DEATH_TOWER_GET_BEST_RECORD* out =
        guard.operator->()->GetInBuffer<SIG_DEATH_TOWER_GET_BEST_RECORD>();
    out->m_count = db->get_n_rows();
    out->m_characNo = characNo;
    // [b28d-BigW8LT 循环形复原] ORIG 0x08429c38 `movl $0x0,-0xc(%ebp)` +
    // `jmp 0x08429fdc`（先跳尾测）→ 循环体 0x08429c44，尾部
    // `addl $1,-0xc; call get_n_rows; cmp -0xc,%eax; seta; test; jne 0x08429c44`
    // = 底测 do-while 形（前置 for 头测多一次 get_n_rows 且整块错位）。
    // [G4W8LTDBTask 2026-09-10 负结论·指针加法求值序]
    // ORIG 0x08429c60 段每站点为 `imul $0x1b0,%eax,%eax; add $0x60,%eax;
    // add -0x10(%ebp),%eax; add $0xc,%eax`（base 作第二操作数、内存直加，
    // 零额外装载）；我方 4.4 恒 base-first `mov -0x10(%ebp),%eax;
    // mov -0xc(%ebp),%edx; imul; add $0x60,%edx; add %edx,%eax` ⇒ 每站点
    // 多 1 条 base 装载（全函数净 +13，orig 356 / our 370）。
    // 两种分组（`i*0x1b0+0x60+(char*)out` 与 `(char*)out+i*0x1b0+0x60`）
    // 实测编译产物逐条相同（net_load 均 13、ours 均 370）⇒ 4.4 对 ptr+int
    // 固定 base-first，P269 分组旋钮不适用，判 N（跨版本求值序）。
    unsigned int i = 0;
    do
    {
        if (!db->fetch())
            return 0;
        // [REAL-BUG 修复 + 寻址复原] 旧形 ① 行基址多算 0xc（包头已在行视图
        // m_pad0 内计入）⇒ 17 个字段整体后移 12B，末行 play_time4 写到 +0x1c0
        // 越出包体；② 提升出 `row` 行指针局部，ORIG 无此局部——ORIG 每站点
        // 都重算 `i*0x1b0 + 组偏移 + out`（0x08429c60/…c97/…cd6/…d10/…e32/
        // …e73/…eeb/…f49/…f85/…fbe），我方提升后少发 16 条 imul + 36 条
        // add/lea（实测助记符 census imul 17/1、add 41/16），并整体挪帧。
        // 故改为逐站点内联两级视图（entry 族 @行+0x00、play 族 @行+0x60，
        // 组步长各 0x6c；两族字段互叠，与 ORIG 位移逐条一致）。
        if (!db->get_ushort(
                0,
                reinterpret_cast<DB_TowerBestRecordEntryView*>(
                    i * 0x1b0 + (char*)out)->m_towerIndex))
            return 0;
        if (!db->get_binary(
                1,
                reinterpret_cast<DB_TowerBestRecordEntryView*>(
                    i * 0x1b0 + (char*)out)->m_member,
                0x17))
            return 0;
        if (!db->get_uint(
                2,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x60 +
                                                              (char*)out)
                    ->m_stage))
            return 0;
        if (!db->get_uint(
                3,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x60 +
                                                              (char*)out)
                    ->m_playTime))
            return 0;
        reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + (char*)out)
            ->m_type = 1;
        if (!db->get_binary(
                4,
                reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0x6c +
                                                              (char*)out)
                    ->m_member,
                0x2e))
            return 0;
        if (!db->get_uint(
                5,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0xcc +
                                                              (char*)out)
                    ->m_stage))
            return 0;
        if (!db->get_uint(
                6,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0xcc +
                                                              (char*)out)
                    ->m_playTime))
            return 0;
        reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0x6c +
                                                       (char*)out)
            ->m_type = 2;
        if (!db->get_binary(
                7,
                reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0xd8 +
                                                              (char*)out)
                    ->m_member,
                0x45))
            return 0;
        if (!db->get_uint(
                8,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x138 +
                                                              (char*)out)
                    ->m_stage))
            return 0;
        if (!db->get_uint(
                9,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x138 +
                                                              (char*)out)
                    ->m_playTime))
            return 0;
        reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0xd8 +
                                                       (char*)out)
            ->m_type = 3;
        if (!db->get_binary(
                10,
                reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0x144 +
                                                              (char*)out)
                    ->m_member,
                0x5c))
            return 0;
        if (!db->get_uint(
                11,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x1a4 +
                                                              (char*)out)
                    ->m_stage))
            return 0;
        if (!db->get_uint(
                12,
                reinterpret_cast<DB_TowerBestRecordPlayView*>(i * 0x1b0 + 0x1a4 +
                                                              (char*)out)
                    ->m_playTime))
            return 0;
        reinterpret_cast<DB_TowerBestRecordEntryView*>(i * 0x1b0 + 0x144 +
                                                       (char*)out)
            ->m_type = 4;
        ++i;
    } while (i < (unsigned int)db->get_n_rows());
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_GoblinPadUpdateReward::dispatch —— ORIG 0x0842413c
// GetDBHandle(DB_HANDLE_6,0) → GetOutBuffer<SIG_GOBLIN_UPDATE_REWARD> →
// seLect mousepass from member_mousepass where m_id=%s；exec&&有行&&fetch
// 失败返 0；get_str(0,buf,0x24) 失败返 0。m_field4==2 或 m_field5==0 →
// modify_type=0xf（仅 INSERT history）；否则 modify_type=0x10（先 UPDATE
// reward_time=unix_timestamp(now()) 且 exec&&受影响行数非 0，失败返 0，再
// INSERT history）。INSERT history（表名 %Y%m）exec 失败或受影响行数==0
// 返 0；成功返 1。（gap=0；残差为 field5 分支 je/jne 极性 + 物理块序 ——
// N 类布局码，见 report dispatch.md）
// ============================================================================
// [b28d-4] AE：P222/P258 德摩根块序交换：ORIG 0x0842413c cmp$2;je<then> + test;je<then> 两跳同落 then ⇒ if(field4!=2&&field5!=0){else}else{then}；探针 /tmp/l4_probe_pol.cpp f2/f3/f4
#include "W8LT_DBTask/dispatch_260.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    // ORIG 用 GetOutBuffer<SIG_GOBLIN_UPDATE_REWARD>（模板实例符号一致）；
    // View 仅作字段访问视图（reinterpret_cast 保留，布局 +0x18 相同）。
    DB_GoblinPadUpdateRewardView* sig =
        reinterpret_cast<DB_GoblinPadUpdateRewardView*>(
            stream->GetOutBuffer<SIG_GOBLIN_UPDATE_REWARD>());
    db->set_query(
        "seLect mousepass from member_mousepass where m_id=%s",
        NumberToString(sig->m_mId, 0));
    if (!(db->exec(true) && db->get_n_rows() != 0 && db->fetch()))
        return 0;
    char mousepass[0x24];
    if (!db->get_str(0, mousepass, 0x24))
        return 0;
    // ORIG 0x0842413c：getCurSec/localtime_r/strftime/NumberToString 在
    // m_field4==2||m_field5==0 与否则两分支内各自重复（独立 ym 缓冲
    // local_2e/local_1e，见 report dispatch.md），非合并共享 —— 逐分支保留。
    // ORIG 0x0842413c 段 `cmp $0x2,%al; je <then>` + `test %al,%al; je <then>`
    // 两跳同落 then（then 在函数后段、else 直落）⇒ 源码形 = 德摩根
    // `if (field4 != 2 && field5 != 0) { else } else { then }`
    // （探针 /tmp/l4_probe_pol.cpp f2/f3/f4 三形同产物；正极性 || 形 f1 反产
    //  `jne <else>` 块序 DIFF）。
    if (sig->m_field4 != 2 && sig->m_field5 != 0)
    {
        char ym[10] = { 0 };
        time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
        struct tm tmBuf;
        localtime_r(&curSec, &tmBuf);
        strftime(ym, 10, "%Y%m", &tmBuf);
        db->set_query(
            "upDate member_mousepass set reward_time = unix_timestamp(now()) where m_id=%s and enable_flag = '1'",
            NumberToString(sig->m_mId, 0));
        if (!(db->exec(true) && db->getAffectedRowCount() != 0))
            return 0;
        db->set_query(
            "inSert into member_mousepass_history_%s (occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info) values (now(),%s,'%s',%d,'%s','')",
            ym, NumberToString(sig->m_mId, 0), mousepass, 0x10, sig->m_ip);
    }
    else
    {
        char ym[10] = { 0 };
        time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
        struct tm tmBuf;
        localtime_r(&curSec, &tmBuf);
        strftime(ym, 10, "%Y%m", &tmBuf);
        db->set_query(
            "inSert into member_mousepass_history_%s (occ_time, m_id, pre_mousepass, modify_type, ip_info, port_info) values (now(),%s,'%s',%d,'%s','')",
            ym, NumberToString(sig->m_mId, 0), mousepass, 0xf, sig->m_ip);
    }
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        return 0;
    return 1;
}

// ============================================================================
// DB_InsertDungeonEntrance_hour::dispatch —— ORIG 0x084298a2
// GetDBHandle(DB_HANDLE_4,0) → >>count(int) → occDate=getCurSec()、
// channelNo=get_channel_no()。循环 count 次：>>dungeonIndex >>enterCount
// >>exerciseCount（3×int）；enterCount!=0 时按 s.size()!=0 选逗号前缀
// sprintf 拼 "(ch,from_unixtime(occ),dung,hour_enter,cnt_enter)" 入 buf
// [0x400] 再 s+=buf。s 非空时 inSert log_dungeon_entrance_hour values %s；
// exec 失败返 0；成功返 1。
// ============================================================================
// [b28d-solo-3] AE + 真 bug 修复：ORIG `mov $0x100,%edx; rep stos` 的 edx 是
// **dword 计数** = 0x400 字节（P116/P253），旧 buf[0x100] 少清 0x300 字节 ⇒
// buf[0x400]={'\0'}；帧/槽与 ORIG 全等（151=151）。ORIG 084298a2。
#include "W8LT_DBTask/dispatch_261.inc"
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    int count = 0;
    *stream >> count;
    int occDate = GlobalData::s_systemTime_.getCurSec();
    int channelNo = G_CEnvironment()->get_channel_no();
    int dungeonIndex = 0;
    int enterCount = 0;
    int exerciseCount = 0;
    // ORIG 0x084298a2：`mov $0x100,%edx; rep stos` = 0x100 dwords = 0x400 字节
    // （P116/P253：ecx 是 dword 计数，旧 buf[0x100] 少清 0x300 字节 = 真 bug）。
    char buf[0x400] = { '\0' };
    std::string s;
    for (int i = 0; i < count; ++i)
    {
        *stream >> dungeonIndex;
        *stream >> enterCount;
        *stream >> exerciseCount;
        if (enterCount == 0)
            continue;
        if (s.size() != 0)
        {
            sprintf(buf, ",(%d,from_unixtime(%d),%d,%d,%d)", channelNo,
                    occDate, dungeonIndex, enterCount, exerciseCount);
        }
        else
        {
            sprintf(buf, "(%d,from_unixtime(%d),%d,%d,%d)", channelNo,
                    occDate, dungeonIndex, enterCount, exerciseCount);
        }
        s += buf;
    }
    if (s.size() != 0)
    {
        db->set_query(
            "inSert into log_dungeon_entrance_hour(channel_no,occ_date,dungeon_index,hour_enter_count, cnt_enter_count) values %s",
            s.c_str());
        if (!db->exec(true))
            return 0;
    }
    return 1;
}

// ============================================================================
// DB_InsertPoliceSaveChatting::dispatch —— ORIG 0x0843c514
// >>characNo(u32) >>msgType(char) >>message(char* 读入 0x100 缓冲) →
// GetDBHandle(DB_HANDLE_4,0) → escape_string(message) →
// inSert log_police_chatting(charac_no, msg_type, occ_time, message)
// values(%u,%d,unix_timestamp(now()),'%s')。exec 结果丢弃，恒返 1。
// ============================================================================
// [b28d-solo-3] AE：message[0x100]/escapeBuf[0x200] 改 `={'\0'}` 并连排声明于
// 读取前（P312/P202① 复现 ORIG 两段对齐剥离慢路，0x100/0x200）。ORIG 0843c514。
#include "W8LT_DBTask/dispatch_262.inc"
                                           Stream* stream)
{
    unsigned int characNo = 0;
    char msgType = 0;
    // ORIG 0x0843c514：message(-0x111,0x100)/escapeBuf(-0x311,0x200) 连续声明，
    // 两个清零都在读取前（`= {'\0'}` 复现 4.1 对齐剥离慢路，P312/P202①）。
    char message[0x100] = { '\0' };
    char escapeBuf[0x200] = { '\0' };
    *stream >> characNo;
    *stream >> msgType;
    *stream >> message;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->escape_string(escapeBuf, message);
    db->set_query(
        "inSert into log_police_chatting(charac_no, msg_type, occ_time, message) values(%u, %d, unix_timestamp(now()), '%s')",
        characNo, (int)msgType, escapeBuf);
    db->exec(true);
    return 1;
}

// ============================================================================
// DB_InsertRandomOptionItemInform::dispatch —— ORIG 0x08442c6a
// GetDBHandle(DB_HANDLE_1,0) → >>m_id(u32) >>checkCount(uchar) → inSert
// check_pick_up_random_option_item(m_id, check_count) values(%s, %u)。
// exec 失败返 0（ORIG 附 cMyTrace 日志，语义无关省略），成功返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_263.inc"
                                               Stream* stream)
{
    unsigned int m_id = 0;
    unsigned char checkCount = 0;
    *stream >> m_id;
    *stream >> checkCount;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into check_pick_up_random_option_item(m_id, check_count) values(%s, %u)",
        NumberToString(m_id, 0), (unsigned int)checkCount);
    if (!db->exec(true))
    {
        // ORIG 0x08442d42：cMyTrace(__PRETTY_FUNCTION__, 0xa47e, 5) + trace 日志。
        // ORIG 0x08442d2a `mov %eax,%ebx`：NumberToString 结果无具名槽、直落
        // callee-saved %ebx 跨 ctor 保活 ⇒ P0 实参内联 + P227 临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xa47e, 5)(
            "inSert into check_pickup_random_option_item failed at m_id(%s) ",
            NumberToString(m_id, 0));
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_LoadBreakAwayPreventData::dispatch —— ORIG 0x084389fe
// GetDBHandle(DB_HANDLE_6,0) → >>flag(bool)。SIG_BREAK_AWAY_PREVENT_SYSTEM_
// MANAGER 栈对象（ctor 清零 0x2e4）。seLect churn_system_manager 最新行；
// exec 失败或行数!=1 返 0；fetch + 逐列读：no(int)、weekday_var_a..z(short
// ×6)、next_reward_day(short)、state_flag(byte)。flag==0 → UPDATE
// state_flag=0 where no（exec 失败/无影响行返 0；成功也返 0——ORIG 两路皆
// 返 0 不再加载）。flag!=0 → UPDATE state_flag=1（失败返 0）→ 置
// sig.m_fieldE=flag → seLect churn_reward_manager 全表，逐行 fetch 后按
// 步长 0x18 填 m_rewards[i]（quest_id/min_day/max_day 为 byte、
// min_val/max_val/luck_point 为 int、item_id 为 uint、add_info 为 int）。
// 回包流 0x141/-1 + GetInBuffer<SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER> +
// memcpy 0x2e4 → put(DISPATCH_Q) 返 1。
// ============================================================================
// [b28d-4] DIFF：P219/P248：字节字段物化/同型化差（ORIG movzbl vs 4.4 mov/shr/and）；ORIG 084389fe
// [r3 负结论] `if (rowCount == 1) {} else { return 0; }` 空-if 极性包裹（P222）
//   实测**无效**（484/484、ndiff 8、差集 {jne}/{je} 全同）：ORIG 08438c76 `jne`
//   跳后部 return-0 块 + 两处 tail-merge 块 vs 4.4 本地 `je` ⇒ N 封顶。
// [R5BigTURest 2026-09-11 复核] 已落 1 处：两处 int 零初始化次序（ORIG
//   `movl $0x0,-0x14` 先 / `-0x40` 后 ⇒ 源码 rowCount 声明须在 no 之前）⇒
//   ndiff 4→2（484/485、gap -1 不变）。残留 2 处均为 N：① ORIG 08438c76
//   `jne` + 后部 return-0 块 vs 4.4 `je` + 目标后置块（块布局）；② ORIG
//   0843?? 两处**重复** `mov $0x0,%ebx; jmp` 出口块（4.1 不合并），4.4 合并
//   为一份 ⇒ 源码不可造。
#include "W8LT_DBTask/dispatch_264.inc"
                                           Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    bool flag = false;
    *stream >> flag;
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER sig;
    // [R5BigTURest 偏移/槽序] ORIG 0x08438c5x 两处 int 零初始化次序为
    // `movl $0x0,-0x14(%ebp)`（rowCount，后接 `cmpl $0x1,-0x14(%ebp)` 判行数）
    // **先**、`movl $0x0,-0x40(%ebp)`（no，后作 UPDATE 第二实参）**后**
    // ⇒ 源码中 rowCount 声明在 no 之前（4.4 按声明序发零初始化）。
    int rowCount = 0;
    int no = 0;
    db->set_query(
        "seLect no, weekday_var_a, weekday_var_b, weekday_var_c, weekend_var_x, weekend_var_y, weekend_var_z, next_reward_day, state_flag from churn_system_manager order by no desc limit 1");
    if (!db->exec(true))
        return 0;
    rowCount = db->get_n_rows();
    if (rowCount != 1)
        return 0;
    if (!db->fetch())
        return 0;
    if (!db->get_int(0, no))
        return 0;
    if (!db->get_short(1, reinterpret_cast<short&>(sig.m_field0)))
        return 0;
    if (!db->get_short(2, reinterpret_cast<short&>(sig.m_field2)))
        return 0;
    if (!db->get_short(3, reinterpret_cast<short&>(sig.m_field4)))
        return 0;
    if (!db->get_short(4, reinterpret_cast<short&>(sig.m_field6)))
        return 0;
    if (!db->get_short(5, reinterpret_cast<short&>(sig.m_field8)))
        return 0;
    if (!db->get_short(6, reinterpret_cast<short&>(sig.m_fieldA)))
        return 0;
    if (!db->get_short(7, reinterpret_cast<short&>(sig.m_fieldC)))
        return 0;
    if (!db->get_byte(8, reinterpret_cast<char&>(sig.m_fieldE)))
        return 0;
    if (!flag)
    {
        db->set_query(
            "upDate churn_system_manager set state_flag=%d where no = %d",
            // [b28d-BigW8LT 转写复原] ORIG 0x08438c8f 两路 UPDATE 的第一实参
            // 都是 `movzbl -0x39(%ebp)`（**flag 变量本身**，非字面量 0/1）：
            // `mov -0x40(%ebp),%edx; movzbl -0x39(%ebp),%eax; movzbl %al,%eax;
            //  mov %edx,0xc(%esp); mov %eax,0x8(%esp)`。字面量形少发 2 组
            // movzbl+mov（实测助记符 census movzbl 6/2）。bool 取值域 {0,1}
            // ⇒ 两路传 flag 与原传常量**同值**，行为不变。
            flag, no);
        if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x8684, 0)(
                "BREAK_AWAY : STATE_FALG ALREADY OFF!!");
        }
        // ORIG 0x08438d25 为**单一** return 0 站点：0x08438ceb `test %al,%al;
        // je 08438d25` 跳过 trace 直落此处，成功/失败同路返回（Ghidra 反编译
        // 亦只有一个 `return 0;` 在 if(bVar1) 之后）。
        return 0;
    }
    db->set_query(
        "upDate churn_system_manager set state_flag=%d where no = %d", flag, no);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x868a, 0)(
            "BREAK_AWAY : STATE_FALG ALREADY ON!!");
        // ORIG：ON 分支 UPDATE 失败/无影响行时打日志后继续执行（不返回）
    }
    sig.m_fieldE = (unsigned char)flag;
    db->set_query(
        "seLect min_day, max_day, min_val, max_val, item_id, add_info, luck_point, quest_id from churn_reward_manager");
    if (!db->exec(true))
        return 0;
    rowCount = db->get_n_rows();
    for (int i = 0; i < rowCount; ++i)
    {
        if (!db->fetch())
            return 0;
        // [b28d-BigW8LT 寻址复原] 旧形提升出 `row = &sig.m_rewards[i]` 行指针
        // 局部，ORIG 0x084389fe 无此局部——8 个站点各自重算
        // `i*0x18 + &sig + 字段偏移`（实测 ORIG add 38/shl 8/lea 39 vs 我方
        // 18/1/25，缺的 49 条全在此）。改回逐站点下标式成员访问。
        if (!db->get_byte(
                0, reinterpret_cast<char&>(sig.m_rewards[i].m_field1)))  // min_day
            return 0;
        if (!db->get_byte(
                1, reinterpret_cast<char&>(sig.m_rewards[i].m_field2)))  // max_day
            return 0;
        if (!db->get_int(2, sig.m_rewards[i].m_field4))    // min_val
            return 0;
        if (!db->get_int(3, sig.m_rewards[i].m_field8))    // max_val
            return 0;
        if (!db->get_uint(4, (unsigned int&)sig.m_rewards[i].m_fieldc))  // item_id
            return 0;
        if (!db->get_int(5, sig.m_rewards[i].m_field10))   // add_info
            return 0;
        if (!db->get_int(6, sig.m_rewards[i].m_field14))   // luck_point
            return 0;
        if (!db->get_byte(
                7,
                reinterpret_cast<char&>(sig.m_rewards[i].m_field0)))  // quest_id
            return 0;
    }
    // [K5W8LTDBTask W8DiagC 语义修正 ③⑨] ORIG 0x08439063-0x08439066
    //   `mov -0x14(%ebp),%eax; mov %eax,-0x4c(%ebp)`
    // —— sig 栈对象基址 -0x32c（ctor `lea -0x32c(%ebp)` @0x08438a3f，回包
    //    `memcpy(out,&sig,0x2e4)` 同址）⇒ -0x4c = sig+0x2e0 = m_field2e0；
    //    -0x14 = rowCount（循环上界，来自第二次 get_n_rows）。
    // 该写无我方替代路径（-0x4c 全函数只此一写）；缺失使我方回包 m_field2e0
    // 恒为 ctor 清零值 0 ⇒ 消费方 GlobalData.cpp:5870
    //   `for (i=0; i<((const SIG_...*)this)->m_field2e0; ++i)` 循环 0 次，
    //   断线重连奖励表整表不装载（真语义 bug）。
    sig.m_field2e0 = rowCount;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x86c5), true);
    guard.operator*()->operator<<(0x141);
    guard.operator*()->operator<<(-1);
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* out =
        guard->GetInBuffer<SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER>();
    memcpy(out, &sig, 0x2e4);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_LoadBreakAwayRewardData::dispatch —— ORIG 0x084392dc
// GetDBHandle(DB_HANDLE_6,0) → serverId=env+0x378(m_serverGroup)；curSec=
// getCurSec → localtime_r → sprintf("%d%02d",年,月+1) 表名后缀。读 7 字段：
// m_id(u32)/characNo(u32)/lastRewardTime(u32)/questTime(int)/luckPoint(int)
// /rewardOrder(int)/addInfo(u32)。按 rewardOrder 0..4 选 UPDATE
// churn_member_info（0: 三时间+luck+add_info=3；1: last+luck m_id=%u；
// 2: last+second+quest+luck+add_info=3；3: last+luck；4: last+luck+
// add_info=-1）；其他值直接返 0。UPDATE exec 失败或无影响行返 0。
// inSert churn_reward_history_%s（occ_time=curSec、server_id、charac_no、
// item_id=lastRewardTime、add_info=questTime、luck_point、cera=addInfo、
// reward_order）失败返 0。回包流 0x143/slot + SIG_BREAK_AWAY_PREVENT_MESSAGE
// {+0x00 = rewardOrder∈{0,2} ? curSec+0x15180 : 未写, +0x04 = rewardOrder}
// → put(DISPATCH_Q) 返 1。
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084392dc
#include "W8LT_DBTask/dispatch_265.inc"
                                          Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int m_id = 0;
    unsigned int characNo = 0;
    // ORIG 0x084392dc：`call G_CEnvironment;mov 0x378;→-0x20` 夹在第 2、3
    // 个清零槽之间（-0x4c 后、-0x50 前）⇒ 声明位=characNo 后。
    int serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    unsigned int lastRewardTime = 0;
    int questTime = 0;
    int luckPoint = 0;
    int rewardOrder = 0;
    unsigned int addInfo = 0;
    time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
    struct tm tmBuf;
    localtime_r(&curSec, &tmBuf);
    // ORIG 0x08439…：`movl $0,-0x6b; movw $0,-0x67; movb $0,-0x65` 逐段
    // 清零 = 声明初始化形（P4/29），非 memset 调用。
    char ym[7] = { 0 };
    sprintf(ym, "%d%02d", tmBuf.tm_year + 0x76c, tmBuf.tm_mon + 1);
    *stream >> m_id;
    *stream >> characNo;
    *stream >> lastRewardTime;
    *stream >> questTime;
    *stream >> luckPoint;
    *stream >> rewardOrder;
    *stream >> addInfo;
    if (rewardOrder == 0)
    {
        db->set_query(
            "upDate churn_member_info set last_reward_time =%u, first_reward_time = %u, quest_time =%u, luck_point=%d, add_info=%d where m_id=%s",
            (unsigned int)curSec, (unsigned int)curSec,
            (unsigned int)curSec + 0x15180, luckPoint, 3,
            NumberToString(m_id, 0));
    }
    else if (rewardOrder == 1)
    {
        db->set_query(
            "upDate churn_member_info set last_reward_time =%u, luck_point=%d where m_id=%u",
            (unsigned int)curSec, luckPoint, m_id);
    }
    else if (rewardOrder == 2)
    {
        db->set_query(
            "upDate churn_member_info set last_reward_time =%u, second_reward_time = %u, quest_time =%u, luck_point=%d, add_info=%d where m_id=%s",
            (unsigned int)curSec, (unsigned int)curSec,
            (unsigned int)curSec + 0x15180, luckPoint, 3,
            NumberToString(m_id, 0));
    }
    else if (rewardOrder == 3)
    {
        db->set_query(
            "upDate churn_member_info set last_reward_time =%u, luck_point=%d where m_id=%s",
            (unsigned int)curSec, luckPoint, NumberToString(m_id, 0));
    }
    else if (rewardOrder == 4)
    {
        db->set_query(
            "upDate churn_member_info set last_reward_time =%u, luck_point=%d, add_info=%d where m_id=%s",
            (unsigned int)curSec, luckPoint, -1, NumberToString(m_id, 0));
    }
    else
    {
        return 0;
    }
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x871e, 5)(
            "BREAK_AWAY : ABNORMAL DB_LoadBreakAwayRewardData REWARD!!");
        return 0;
    }
    db->set_query(
        "inSert into churn_reward_history_%s(m_id,occ_time,server_id,charac_no,item_id,add_info,luck_point,cera,reward_order) values(%s, from_unixtime(%u), %d, %u, %u, %d, %d, %u, %d)",
        ym, NumberToString(m_id, 0), (unsigned int)curSec, serverId,
        characNo, lastRewardTime, questTime, luckPoint, addInfo, rewardOrder);
    if (!db->exec(true))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x8740, 5)(
            "BREAK_AWAY : ABNORMAL DB_LoadBreakAwayRewardData LOG!!");
        return 0;
    }
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8745), true);
    guard.operator*()->operator<<(0x143);
    guard.operator*()->operator<<(slot);
    // [Lane31W8LTDBTaskView] 权威头具名直访问（ORIG RHS-first，避免 cast 形 base 先载）。
    SIG_BREAK_AWAY_PREVENT_MESSAGE* out =
        guard->GetInBuffer<SIG_BREAK_AWAY_PREVENT_MESSAGE>();
    if (rewardOrder == 0 || rewardOrder == 2)
        out->m_nextRewardTime = (unsigned int)curSec + 0x15180;
    out->m_rewardOrder = rewardOrder;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_LoadCharacHotKeyOption::dispatch —— ORIG 0x0842f3a0
// GetDBHandle(DB_HANDLE_B,0) → GetOutBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_
// OPTION>(+0 accId u32 / +4 keyType int) → seLect key_option from
// member_key_option where m_id=%s and key_type=%d；exec 失败返 0。回包流
// 0x320/slot + SIG_LOAD_CHARAC_HOTKEY_OPTION（+0 accId、+4 keyType byte、
// +5 CHotkeyOption）。无行 → CHotkeyOption::clear(out+5) → put 返 1。
// 有行：fetch 失败返 0；get_binary(0, buf[0xed]) 失败返 0；
// get_binary_length → destLen=0xbc → uncompress_zip(out+5) 失败返 0；
// put(DISPATCH_Q) 返 1。
// ============================================================================
// [b28d-solo-3] DIFF：`char buf[0xed]={'\0'}`（P312）已消 p167 EXTRA memset
// （差集 CLEAR）；余 N82 槽差（ORIG db(-0x2c) 6 读 vs 我方 -0x24 2 读 + 帧 4B）。
// [r3 负结论] destLen/binLen 声明序互换（P331）实测**反劣**：ndiff 2→4、gap 0
//   不变（ORIG 0842f3a0 该块 dst(lea/add) 先、len `mov -0x1c(%ebp),%edx` 后 =
//   4.1 左→右求值；4.4 恒右→左）⇒ 回滚，判 N 封顶。
// ORIG 0842f3a0。
#include "W8LT_DBTask/dispatch_266.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_B,
                                                  E_SERVER_GROUP_NONE);
    SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION* req =
        stream->GetOutBuffer<SIG_REQ_LOAD_CHARAC_HOTKEY_OPTION>();
    db->set_query(
        "seLect key_option from member_key_option where m_id = %s and key_type = %d",
        NumberToString(req->m_accId, 0),
        req->m_hotkeyType);
    if (!db->exec(true))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x746e), true);
    guard.operator*()->operator<<(0x320);
    guard.operator*()->operator<<(slot);
    // [Lane31W8LTDBTaskView] 权威头具名直访问（+0x00 权威名 m_uid）。
    SIG_LOAD_CHARAC_HOTKEY_OPTION* out =
        guard.operator->()->GetInBuffer<SIG_LOAD_CHARAC_HOTKEY_OPTION>();
    out->m_uid = req->m_accId;
    out->m_hotkeyType = (unsigned char)req->m_hotkeyType;
    if (db->get_n_rows() == 0)
    {
        // [Lane31W8LTDBTaskView] +0x05 为 CHotkeyOption 字节视角（C++98 匿名
        // union 不得含有构造函数成员）⇒ 取址转类型调真实 clear（保留 ORIG 的
        // _ZN14CHotkeyOption5clearEv 调用）。
        reinterpret_cast<CHotkeyOption*>(&out->m_hotkey)->clear();
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    if (!db->fetch())
    {
        // ORIG 0x0842f513-0x0842f57b：cMyTrace(0x747f,5) + NumberToString。
        cMyTrace(__PRETTY_FUNCTION__, 0x747f, 5)(
            "DB_LoadCharacHotKeyOption::dispatch, fetch ERROR m_id=%s",
            NumberToString(req->m_accId, 0));
        return 0;
    }
    // ORIG 0x0842f586 `mov $0xed,%ebx` + 对齐剥离慢路（P312/P202①）。
    static const int compressedBufferSize = 0xed;  // ORIG _ZZ…DB_LoadCharacHotKeyOption…E20compressedBufferSize
    char buf[0xed] = { '\0' };
    // [H4 2026-09-11 D 组取证] ORIG 0x0842f600 `mov %al,-0x1d(%ebp)` +
    // 0x0842f603 `movzbl -0x1d(%ebp),%eax` + `xor $1; test; je` ⇒ 原码此处**有**
    // 具名 bool 局部（P0/P102 同局部失败出口）；我方内联 `!call()` 少 2 条且全帧浅 4B。
    // （同函数 exec/fetch/uncompress_zip 三处 ORIG 无槽：0x0842f428/0x0842f523/
    //   0x0842f6b1 均直 `xor $1;test;je`，勿一并加局部。）
    bool bRet = db->get_binary(0, buf, compressedBufferSize);
    if (!bRet)
    {
        // ORIG 0x0842f60e-0x0842f664：cMyTrace(0x7489,5) + NumberToString。
        cMyTrace(__PRETTY_FUNCTION__, 0x7489, 5)(
            "DB_LoadCharacHotKeyOption, get_binary(0) ERROR m_id=%s",
            NumberToString(req->m_accId, 0));
        return 0;
    }
    int binLen = db->get_binary_length(0);
    unsigned int destLen = 0xbc;
    if (!uncompress_zip((char*)&out->m_hotkey, destLen, buf, binLen))
        return 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_LoadEventLevelupSupport::dispatch —— ORIG 0x0844cd38
// >>characNo(int) → GetDBHandle(DB_HANDLE_2,0)；句柄空返 0。seLect level,
// state from event_levelup_support where charac_no=%d；exec 失败 throw
// (unsigned int)0xc277。回包流 0x350/slot + SIG_EVENT_LEVEUP_SUPPORT_INFO
// {+0 count, +4 起 ushort level/ushort state 对（步长 4）}。逐行 fetch：
// get_ushort(0/1) 失败分别 throw 0xc28c/0xc28d/0xc28e。put(DISPATCH_Q) 返
// 1。catch(unsigned int) → 日志（省略）→ 返 0。
// ============================================================================
// [b28d-4] DIFF：N1/N107：ORIG lea+add 两步寻址 vs 4.4 折单条；ORIG 0844cd38
// [b28d-solo-4] §6.2 三件套：p167 CLEAR（ORIG 36 = OUR 36，0/0）+ gap 1（211/210）
//   ⇒ 语义完成；余 N104 EH 尾块相对序（ushort 槽已修）封顶。
// [r3 负结论] 尾部 ORIG 0844cffb `jmp 0844d012`（跳过 EH landing-pad 岛 `mov
//   %edx,%ebx; mov %eax,%esi`）在 4.4 被 jump-thread 消除、且我方把 `mov
//   $0x1,%ebx` 提前到 put 之后（ORIG 在 +0x35f）⇒ our 210/ORIG 211 恒少 1 条，
//   无源形可复现 ⇒ N104 封顶。
#include "W8LT_DBTask/dispatch_267.inc"
{
    int characNo = 0;
    *stream >> characNo;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    if (db == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xc26d, 5)(
            "[ERROR] Database Handle is NULL [index:%d]", 2);
        return 0;
    }
    try
    {
        db->set_query(
            "seLect level, state from event_levelup_support where charac_no=%d",
            characNo);
        // [K5W8LTDBTask W8DiagC 类型轴 ⑦] ORIG 0x0844ce17 `movl $_ZTIi,0x4(%esp)`
        // ⇒ 抛出静态类型是 **int**（我方原 `unsigned int` 发 _ZTIj）；
        // catch 同刻改 int（throw/catch 必须同型，否则异常逃逸）。
        if (!db->exec(true))
            throw (int)0xc277;
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc279), true);
        guard.operator*()->operator<<(0x350);
        guard.operator*()->operator<<(slot);
        // [Lane31W8LTDBTaskView] 权威头具名直访问。
        SIG_EVENT_LEVEUP_SUPPORT_INFO* out =
            guard->GetInBuffer<SIG_EVENT_LEVEUP_SUPPORT_INFO>();
        int rowCount = db->get_n_rows();
        out->m_count = rowCount;
        // ORIG 0x0844cebc：循环前 `movw $0,-0x4a/-0x4c` 两 ushort 槽预清
        // （P198①/P232：声明在循环外，get_ushort(int,unsigned short&) 引用形参
        // 物化），循环尾 movzwl 回写成员。
        unsigned short level = 0;
        unsigned short state = 0;
        for (int i = 0; i < rowCount; ++i)
        {
            if (!db->fetch())
                throw (int)0xc28c;
            if (!db->get_ushort(0, level))
                throw (int)0xc28d;
            if (!db->get_ushort(1, state))
                throw (int)0xc28e;
            out->m_entries[i].m_level = level;
            out->m_entries[i].m_state = state;
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    catch (int e)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xc297, 5)(
            "[ERROR] DB_LoadEventLevelupSupport. [line:%d]", e);
        return 0;
    }
}

// ============================================================================
// DB_LoadOnlinePreliminaryTeam::dispatch —— ORIG 0x0843bab2（4 条：mov $1; ret）
// ============================================================================
#include "W8LT_DBTask/dispatch_268.inc"
                                            Stream* stream)
{
    (void)userIdx;
    (void)slot;
    (void)stream;
    return 1;
}

// ============================================================================
// DB_LoadOnlinePreliminaryTeamMatchList::dispatch —— ORIG 0x0843bac6
// （4 条：mov $1; ret）
// ============================================================================
#include "W8LT_DBTask/dispatch_269.inc"
                                                     Stream* stream)
{
    (void)userIdx;
    (void)slot;
    (void)stream;
    return 1;
}

// ============================================================================
// DB_LoadPowerWarStatueRanker::dispatch —— ORIG 0x08434878（mov $1; ret）
// ============================================================================
#include "W8LT_DBTask/dispatch_270.inc"
                                           Stream* stream)
{
    (void)userIdx;
    (void)slot;
    (void)stream;
    // ORIG 0x8434878：sub $0x40 空帧 + mov $1 + leave/ret（P31，
    // 实测 char[0x34..0x40] → sub $0x40）。
    char buf[0x38];
    (void)buf;
    // [N8W8LT P139] ORIG nm 有 prett 局部串（0x08c5bf40, sz 0x46：`virtual bool
    // DB_LoadPowerWarStatueRanker::dispatch(int, int, Stream*)`）而体无引用 ⇒ 原码有被
    // 丢弃的引用；sizeof 形只发射该串、零指令变化（探针实证）。
    (void)sizeof(__PRETTY_FUNCTION__);
    return 1;
}

// ============================================================================
// DB_MailBoxSendNewMailWithDBWork::dispatch —— ORIG 0x084272de
// GetDBHandle(DB_HANDLE_3,0) → GetOutBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_
// DBWORK>。本地 SIG_MAILBOX_SEND_NEW_MAIL 0x168 清零后：strncpy(sender,
// sig+0x04,0x1d)；+0x22=sig+0x24、+0x1e=sig+0x28、+0x26=0、+0x57=sig+0x1a2；
// 标题长度 sig+0xa0 非 0 时 +0x59=长度、strncpy(+0x5b, sig+0xa2, 长度)。
// 附件：sig+0x00==0 → InsertAvatarItem(sig+0x2c) 失败 throw 0x6871、
// identity=GetIdentity(db)（0 → throw 0x6874）、+0x2b=sig+0x30、
// +0x34=sig+0x35、+0x2f=identity、+0x36=sig+0x54、+0x56=1；
// sig+0x00==1 → insertCreatureItem(db, sig+0x2c) 失败 throw 0x6883、
// identity（0 → throw 0x6886）、+0x2b=sig+0x34、+0x34=sig+0x40、
// +0x2f=identity、+0x58=1；其余无附件（+0x2a=0、+0x33=0 已清零）。
// insertMailData(db, buf) 失败 throw 0x6891；sendResult(slot,0,sig+0x24,0)
// 失败 throw 0x6894。成功返 1；catch(unsigned int) → 日志（省略）→ 返 1。
// （N 封顶：ndiff 171 残差为局部栈槽分配差异 + EH/catch 布局，见报告
// dispatch.md）
// ============================================================================
// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084272de
#include "W8LT_DBTask/dispatch_271.inc"
                                               Stream* stream)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    DB_MailBoxSendNewMailView* sig =
        reinterpret_cast<DB_MailBoxSendNewMailView*>(
            stream->GetOutBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>());
    // [K5W8LTDBTask W8DiagD 旋钮 P161/P311] ORIG 0x0842735b/…67/…7a/…96 对 mailBuf
    // 字段是**单条 ebp 偏移 store**（`mov %eax,-0x172(%ebp)`、`mov %al,-0x13d(%ebp)`、
    // `mov %dx,-0x13b(%ebp)`），无取址；我方逐次
    // `reinterpret_cast<DB_MailSendDataView*>(mailBuf)->X` 每写多发 1 条
    // `lea -0x194(%ebp),%eax`（chunk4/5/11/17/21 合计 +11）⇒ 改用具名视图对象。
    // DB_MailSendDataView 位于 `#pragma pack(push,1)` 内、末字段 m_title[0x10d]@+0x5b
    // ⇒ sizeof 恰 0x168，槽大小与全部偏移不变。
    DB_MailSendDataView mailBuf;
    memset(&mailBuf, 0, sizeof(mailBuf));
    strncpy(mailBuf.m_sender, sig->m_sender, 0x1d);
    mailBuf.m_field22 = sig->m_field24;
    mailBuf.m_field1e = sig->m_field28;
    mailBuf.m_field26 = 0;
    mailBuf.m_field57 = sig->m_field1a2;
    // [P5S2W8LT r1] 补 && 短路首项：ORIG 0x0842738a `mov -0x18(%ebp),%eax;
    //   add $0xa2,%eax; test %eax,%eax; je +0x104`（数组名退化指针真值测试，
    //   恒真但 4.1 照发）→ 0x08427396 才测 m_titleLen；原缺首项 ⇒ 少 4 条。
    if (sig->m_title != 0 && sig->m_titleLen != 0)
    {
        mailBuf.m_field59 = sig->m_titleLen;
        strncpy(mailBuf.m_title, sig->m_title, sig->m_titleLen);
    }
    try
    {
        if (sig->m_type == 0)
        {
            if (!DB_AvatarItemInsert::InsertAvatarItem(
                    reinterpret_cast<SIG_AVATAR_INSERT*>(sig->m_attach)))
                throw (int)0x6871;
            unsigned int identity = WongWork::DBCommon::GetIdentity(db);
            if (identity == 0)
                throw (int)0x6874;
            // [P5S2W8LT r1] P212/P309 物化序：ORIG 0x08427470/0x08427552
            //   `movb $0x0,-0x16a`（=mailBuf+0x2a）在各附件分支**内部**首条，
            //   0x08427494/0x08427573 的 `movb $0x0,-0x161`（=+0x33）在
            //   m_field34 之后、m_identity 之前——原两行提到 try 之前共用
            //   ⇒ 少 2 条 store 且块序错。
            mailBuf.m_field2a = 0;
            mailBuf.m_field2b = sig->m_field30;
            mailBuf.m_field34 = (unsigned short)sig->m_field35;
            mailBuf.m_field33 = 0;
            mailBuf.m_identity = identity;
            mailBuf.m_field36 = sig->m_field54;
            mailBuf.m_field56 = 1;
        }
        else if (sig->m_type == 1)
        {
            if (!DB_InsertCreatureItem::insertCreatureItem(
                    db, reinterpret_cast<SIG_CREATURE_ITEM*>(sig->m_attach)))
                throw (int)0x6883;
            unsigned int identity = WongWork::DBCommon::GetIdentity(db);
            if (identity == 0)
                throw (int)0x6886;
            mailBuf.m_field2a = 0;   // ORIG 0x08427552
            mailBuf.m_field2b = sig->m_field34;
            mailBuf.m_field34 = sig->m_field40;
            mailBuf.m_field33 = 0;   // ORIG 0x08427573
            mailBuf.m_identity = identity;
            mailBuf.m_field58 = 1;
        }
        if (!DB_MailBox_Req_Send::insertMailData(
                db, reinterpret_cast<const SIG_MAILBOX_SEND_NEW_MAIL*>(&mailBuf)))
            throw (int)0x6891;
        if (!DB_MailBox_Req_Send::sendResult(slot, 0, sig->m_field24,
                                             (ENUM_SERVER_GROUP)0))
            throw (int)0x6894;   // ORIG 0x084275f7 `xor $0x1,%eax; test %al,%al; je`
                                 // vs 我方 `test %eax,%eax; sete %al`（ndiff 唯一残差）。
                                 // [Q5W8LT 2026-09-11 报告·未落] `xor $1` + `test %al` 表明
                                 // ORIG 的 sendResult 返回 **1 字节类型**（bool/char）；
                                 // 改我方**声明侧**（cpp:1103 `static int sendResult`）为 bool
                                 // 即可消此差，但**定义侧**在 DB_MailBox_Req_Send.cpp（非本
                                 // TU 写权）⇒ 声明/定义须同轮同步，留报告（禁单侧改致 ABI 不一致）。
    }
    catch (int e)
    {
        // ORIG 0x0842761c：cMyTrace(__PRETTY_FUNCTION__, 0x6899, 5) + trace("%d", e)。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x6899, 5);
        trace("%d", e);
    }
    return 1;
}

// ============================================================================
// DB_NoticeIngameAdvertisement::dispatch —— ORIG 0x0844710e
// InGameADManagerInstance()->ReloadADExposeInfo()；失败 logFormat 0xb17f
// 返 0；成功返 1。
// ============================================================================
#include "W8LT_DBTask/dispatch_272.inc"
                                            Stream* stream)
{
    (void)userIdx;
    (void)slot;
    (void)stream;
    if (!InGameADManagerInstance()->ReloadADExposeInfo())
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0xb17f, "Fail Load In Game Advertisement Expose Info");
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_PassPadUpdateCancelCnt::dispatch —— ORIG 0x08423fb6
// GetDBHandle(DB_HANDLE_6,0) → GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_
// CNT>(+0 m_id / +4 cancelCnt) → upDate member_mousepass set cancel_cnt=%d
// where m_id=%s and enable_flag='1'。exec 结果丢弃，恒返 1。
// （makeRequest 已由 DB_AccountCargoSync.cpp 定义，不重复。）
// ============================================================================
#include "W8LT_DBTask/dispatch_273.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    DB_PassPadUpdateCancelCntView* sig = reinterpret_cast<DB_PassPadUpdateCancelCntView*>(
        stream->GetOutBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>());
    db->set_query(
        "upDate member_mousepass set cancel_cnt=%d where m_id=%s and enable_flag = '1'",
        sig->m_cancelCnt, NumberToString(sig->m_mId, 0));
    db->exec(true);
    return 1;
}

// ============================================================================
// DB_ReloadAutoPunishRuleBackIP::dispatch —— ORIG 0x0842cdf6
// GetDBHandle(DB_HANDLE_6,0) → seLect ip,start_ip,end_ip from
// auto_punish_blackip_info where apply_flag=1 limit %d(0x7d0)；exec 失败返
// 0。回包流 0x108/-1 + SIG_AUTO_PUNISH_RULE_BLACK_IP{+0 ushort count}。
// 逐行 fetch：get_str(0,ipStr,0xc)、get_uint(1,startIp)、get_uint(2,endIp)
// 任一失败返 0；OS_API::snprintf(ipBuf,0x10,"%s.0",ipStr) →
// inet_addr 写 +0x04+i*8、start 写 +0x08+i*8、end 写 +0x09+i*8。
// put(DISPATCH_Q) 返 1。
// （N 封顶：ndiff 89 残差为帧 -4 偏移 + seta/setl 符号比较极性 + P183
// 地址重物化，见报告 dispatch.md）
// ============================================================================
#include "W8LT_DBTask/dispatch_274.inc"
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect ip,start_ip,end_ip from auto_punish_blackip_info where apply_flag=1 limit %d",
        0x7d0);
    if (!db->exec(true))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7059), true);
    guard.operator*()->operator<<(0x108);
    guard.operator*()->operator<<(-1);
    // [b23S3C] ORIG 0x0842cee3 形：get_n_rows 结果直 `mov %eax,%edx; mov %dx,(%eax)`
    // 落 out+0（无 rowCount 具名槽，P0/P48），且循环条件 ORIG 0x0842d043 **每轮重取**
    // get_n_rows 并按无符号比较（`cmp -0xc(%ebp),%eax; seta`）⇒ i 改 unsigned int、
    // 界改内联 call；ipStr ORIG 清零 15 字节（`movl×3 + movw + movb`）⇒ 数组改 0xf。
    DB_AutoPunishBlackIpView* out = reinterpret_cast<DB_AutoPunishBlackIpView*>(
        guard->GetInBuffer<SIG_AUTO_PUNISH_RULE_BLACK_IP>());
    out->m_count = (unsigned short)db->get_n_rows();
    for (unsigned int i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        if (!db->fetch())
            return 0;
        char ipStr[0xf] = {};
        unsigned int startIp = 0;   // [b23S3C] ORIG 0x0842cf37/-0x30,-0x34 两清零在
        unsigned int endIp = 0;     // get_str 之前（4.1 初始化前置）⇒ 声明上移

        if (!db->get_str(0, ipStr, 0xc))
            return 0;
        if (!db->get_uint(1, startIp))
            return 0;
        if (!db->get_uint(2, endIp))
            return 0;
        char ipBuf[0x10] = {};
        OS_API::snprintf(ipBuf, 0x10, "%s.0", ipStr);
        out->m_entries[i].m_ip = inet_addr(ipBuf);
        out->m_entries[i].m_start = (unsigned char)startIp;
        out->m_entries[i].m_end = (unsigned char)endIp;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

// ============================================================================
// DB_ReloadAutoPunishRuleHackType::dispatch —— ORIG 0x0842d16c
// GetDBHandle(DB_HANDLE_6,0) → seLect hack_type,cnt,etc,hack_sub_type,
// hack_sub_cnt,apply_flag, ip_cnt from auto_punish_hack_info where
// apply_flag > 0；exec 失败返 0。回包流 0x109/-1 + SIG_AUTO_PUNISH_RULE_
// HACK_TYPE，memset 0x2720。行循环 i < min(0x1f4, get_n_rows())：fetch +
// get_uint(0..1)、get_ulonglong(2,etc)、get_uint(3..6) 任一失败返 0。
// hackType==0 → 头部 +0x00=cnt、+0x04/+0x08=etc 低/高，跳过条目；
// 0x64<hackType<=0xfa0 且 (hackSubType==0 或 0x64<hackSubType<=0xfa0)
// → hackSubType==0 时 hackSubCnt=0，写条目（ushort hackType/cnt/
// hackSubType/hackSubCnt/applyFlag + uint ipCnt/etcLo），m_count++；
// 否则跳过该行。put(DISPATCH_Q) 返 1。
// （N 封顶：ndiff 394 残差为 lea/movzwl/shl/add 地址重物化(P183) + 帧偏移，
// 见报告 dispatch.md）
// ============================================================================
// [b28d-4] DIFF：S 档 gap=40（缺失转写/调用）+ N82 槽序；ORIG 0842d16c
#include "W8LT_DBTask/dispatch_275.inc"
                                               Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect hack_type,cnt,etc,hack_sub_type,hack_sub_cnt,apply_flag, ip_cnt from auto_punish_hack_info where apply_flag > 0");
    if (!db->exec(true))
        return 0;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7094), true);
    guard.operator*()->operator<<(0x109);
    guard.operator*()->operator<<(-1);
    // [Lane31W8LTDBTaskView] 权威头直取（去 cast）。
    SIG_AUTO_PUNISH_RULE_HACK_TYPE* out =
        guard.operator->()->GetInBuffer<SIG_AUTO_PUNISH_RULE_HACK_TYPE>();
    memset(out, 0, 0x2720);
    // [b28d-TakeW8LT 形复原] ORIG 0x0842d586 每轮循环**重算**循环上界：
    // `movl $0x1f4,-0x1c; call get_n_rows; mov %eax,-0x18; lea -0x1c,0x4(%esp);
    //  lea -0x18,(%esp); call std::min<unsigned int>`（0x0842d5a8 的
    // _ZSt3minIjERKT_S2_S2_）⇒ 源码形 = min 内联在 for 条件里、get_n_rows 在
    // 第一实参位（旧形提升到循环外，少发每轮 9 条）。
    for (unsigned int i = 0; i < std::min<unsigned int>(db->get_n_rows(), 0x1f4); ++i)
    {
        if (!db->fetch())
            return 0;
        unsigned int hackType = 0;
        unsigned int cnt = 0;
        unsigned long long etc = 0;
        unsigned int hackSubType = 0;
        unsigned int hackSubCnt = 0;
        unsigned int applyFlag = 0;
        unsigned int ipCnt = 0;
        if (!db->get_uint(0, hackType))
            return 0;
        if (!db->get_uint(1, cnt))
            return 0;
        if (!db->get_ulonglong(2, etc))
            return 0;
        if (!db->get_uint(3, hackSubType))
            return 0;
        if (!db->get_uint(4, hackSubCnt))
            return 0;
        if (!db->get_uint(5, applyFlag))
            return 0;
        if (!db->get_uint(6, ipCnt))
            return 0;
        if (hackType == 0)
        {
            out->m_field0 = cnt;
            // [K5W8LTDBTask W8DiagD 负结论] 旋钮 `(unsigned int)(etc & 0xffffffffULL)`
            // → `(unsigned int)etc` 实测无变化（our 336 未动，四数 141/1750/12/57
            // 逐值相同）⇒ 4.4 两形同码；ORIG 0x0842d402-0x0842d40e 的 9 条短形
            // 属 4.1 后端，P246/P92 族不可迁。回滚保原式。
            out->m_field4 = (unsigned int)(etc & 0xffffffffULL);
            out->m_field8 = (unsigned int)(etc >> 32);
            continue;
        }
        if (hackType <= 0x64 || hackType > 0xfa0)
            continue;
        if (hackSubType != 0 &&
            (hackSubType <= 0x64 || hackSubType > 0xfa0))
            continue;
        if (hackSubType == 0)
            hackSubCnt = 0;
        // [b28d-TakeW8LT 形复原] ORIG 0x0842d45f..d563 七个写入站点**各自**
        // 重读 out+0xc 的 m_count 重算索引（ORIG movzwl 15/shl 14 vs 旧形 3/2），
        // 且写入顺序 = +0x10,+0x12,+0x14,+0x16,+0x1c,+0x18,+0x20（ipCnt 在
        // applyFlag 之前，Ghidra 反编译同序）。
        out->m_entries[(unsigned int)out->m_count].m_hackType =
            (unsigned short)hackType;
        out->m_entries[(unsigned int)out->m_count].m_cnt = (unsigned short)cnt;
        out->m_entries[(unsigned int)out->m_count].m_hackSubType =
            (unsigned short)hackSubType;
        out->m_entries[(unsigned int)out->m_count].m_hackSubCnt =
            (unsigned short)hackSubCnt;
        out->m_entries[(unsigned int)out->m_count].m_ipCnt = ipCnt;
        out->m_entries[(unsigned int)out->m_count].m_applyFlag =
            (unsigned short)applyFlag;
        out->m_entries[(unsigned int)out->m_count].m_etcLo =
            (unsigned int)(etc & 0xffffffffULL);
        out->m_count = (unsigned short)(out->m_count + 1);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}
