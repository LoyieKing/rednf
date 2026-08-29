#ifndef DNF_GAME_SIG_TYPES_C_H_
#define DNF_GAME_SIG_TYPES_C_H_

// ============================================================================
// df_game_r 还原 —— SIG_* 包结构分片 C（CHARAC_DATA/TAG_*/SIG_LOAD_* 域）
// 从 SigTypes.h 手动拆分（纯移动，类型定义零改动）；umbrella 见 SigTypes.h。
// ============================================================================

#include <string>  // std::string（CHARAC_LOAD_MERCENARY::GetRewardItem 形参）

#include <cstddef>  // offsetof static_assert
// SIG_LOAD_PVP_BUDDY 载荷引用 shared 权威 STPvPBuddyDBInfo（0x2b 步长）。
#include <string.h>  // user_gold_info::reset/toCopy（ORIG 头内联，W 弱符号）
#include "CCharacter.h"   // _Additioal_info 权威定义（CHARAC_DATA 真实子对象 +0x60）
#include "Inven_Item.h"   // Inven_Item 权威定义（CHARAC_DATA 真实成员 m_items[21]）
#include "MasteredSkill.h"  // _Mastered_skill（TAG_CHARAC_ETC_DATA/SIG_LOAD_SKILL 数组成员构造）
#include "STSkillCommandData.h"  // STSkillCommandData::STCommand（SIG_LOAD_SKILL 命令区）
#include "QuestClear.h"   // WongWork::STClearedQuest（SIG_LOAD_QUEST 完成位图元素，1 字节）
#include "GameResultType.h"  // GameResultType（SIG_LOAD_ETC +0x3c8 记录区，0x50 步长）
#include "PvpResultType.h"   // PvpResultType（SIG_LOAD_ETC +0x340 区）
#include "CTitleBook.h"      // stItemLockInfo（SIG_LOAD_INVENTORY +0x14784 锁区，8 字节）
#include "SIG_SAVE_EXPAND_EQUIPSLOT.h"  // SIG_SAVE_EXPAND_EQUIPSLOT（SIG_LOAD_INVENTORY +0x14aa4 子对象）
#include "../../../shared/packet/include/STPvPBuddyDBInfo.h"

// ---- CHARAC_LOAD_MERCENARY 方法声明依赖的前向（完整定义见 CUser.h/PacketTypes.h）----
struct CUser;
struct competition_reward_result;
class Inven_Item;

// ORIG sizeof = 0x703（CACHE_CHARACTER_TYPE@+0x0 → +0x703）。
// CHARAC_DATA（SIG_LOAD_CHARAC 的 +0xc 区，0x67a 字节）：getCharacInfoDetail @0x864bdba
// 依偏移填充（源为 _Charac_info 成员；具名字段见 CUser.cpp 实现）。
// 尾部 +0x676（SIG 内绝对偏移 0x682）为 school no：RequestDBCharac @0x81c6ca0
// 写 CUser::GetSchoolNo；CHARAC_DATA::Reset（W 0x822ec9e）不覆盖该 4 字节。
// TAG_CHARAC_DATA（+0x68b 区，0x78 字节）：getTagCharacInfo @0x86904f0 填充。
#pragma pack(push, 1)
struct CHARAC_DATA
{
    char m_data[0x60];          // +0x000..+0x05f（blob 前段视图）
    _Additioal_info m_add;      // +0x060（pack1，0x52）
    // +0x0b2..+0x137 未还原字段组件视图（ORIG Reset 0x822ec9e 尾部字节簇 + dword）
    struct UnknownB2
    {
        char m_pad0[0x72];      // +0x0b2..+0x123
        char m_b124;            // +0x124
        char m_pad125[4];       // +0x125..+0x128
        char m_b129;            // +0x129
        char m_b12a;            // +0x12a
        char m_b12b;            // +0x12b
        char m_b12c;            // +0x12c
        char m_b12d;            // +0x12d
        char m_b12e;            // +0x12e（ORIG Reset 不清）
        char m_b12f;            // +0x12f
        int  m_i130;            // +0x130
        char m_pad134[4];       // +0x134..+0x137
    };
    UnknownB2 m_unkB2;          // +0x0b2（sizeof 0x86）
    // ORIG C1 0x822ec4c 循环 esi=0x15 起减到 0 共 22 次 → m_items[22]
    // （0x138 + 22×0x3d = 0x676；Reset memset(0x138,0,0x53e) 覆盖到 +0x676）。
    Inven_Item m_items[22];     // +0x138（22×0x3d = 0x53e，至 +0x676）
    unsigned int m_schoolNo;    // +0x676（RequestDBCharac 写入；Reset 不清）

    CHARAC_DATA() { Reset(); }  // ORIG W 0x822ec4c _ZN11CHARAC_DATAC1Ev
    void Reset();               // ORIG W 0x822ec9e _ZN11CHARAC_DATA5ResetEv
};
static_assert(sizeof(CHARAC_DATA) == 0x67a, "CHARAC_DATA ABI drift");
#pragma pack(pop)

#pragma pack(push, 1)
struct TAG_CHARAC_DATA
{
    // ORIG C1 0x822edae 实证：+0x26 构造 _Additioal_info（0x52）后调 clear()。
    char m_data[0x26];          // +0x00..+0x25（blob 前段视图）
    _Additioal_info m_add;      // +0x26（pack1 0x52）

    TAG_CHARAC_DATA() { clear(); }  // ORIG W 0x822edae _ZN15TAG_CHARAC_DATAC1Ev
    void clear();              // ORIG W 0x822ed84 _ZN15TAG_CHARAC_DATA5clearEv
};
static_assert(sizeof(TAG_CHARAC_DATA) == 0x78, "TAG_CHARAC_DATA ABI drift");
#pragma pack(pop)

// SIG_LOAD_INVENTORY +0xc 的 TAG_CHARAC_INVEN_DATA（0x4024 字节）：getTagCharacInventory @0x869067c 填充。
#pragma pack(push, 1)
struct TAG_CHARAC_INVEN_DATA
{
    // ORIG C1 0x0848f774：Inven_Item[0x16]@+0（0x16×0x3d=0x52c）+ Inven_Item[0x69]@+0x53e
    // （0x69×0x3d=0x18f9）两组倒计数构造后调 clear()（P151）。
    Inven_Item m_items[0x16];         // +0x000（0x16×0x3d = 0x53e）
    Inven_Item m_items2[0x69];        // +0x53e（0x69×0x3d = 0x1905 → +0x1e43）
    char m_pad1e43[0x4024 - 0x1e43];  // +0x1e43（0x21e1）

    TAG_CHARAC_INVEN_DATA() { clear(); }  // ORIG C1 0x0848f774
    void clear();              // ORIG W 0x822edd0 _ZN21TAG_CHARAC_INVEN_DATA5clearEv
};
static_assert(sizeof(TAG_CHARAC_INVEN_DATA) == 0x4024, "TAG_CHARAC_INVEN_DATA ABI drift");
#pragma pack(pop)

// RequestDBETC @0x81c7602 在 SIG_LOAD_ETC +0x10 调用的组合 tag 区：
// 覆盖 tagSkillSlot[0x198] + tagSkillCombo[0x198] = 0x330 字节
//（ORIG W 0x822edf4 _ZN19TAG_CHARAC_ETC_DATA5clearEv = memset(this,0,0x330)）。
// ORIG C1 0x0848f7dc：外层 2 轮 × 内层 _Mastered_skill[0xcb]（步长 2，0x196）+
// 行尾 2 字节空隙（行步长 0x198）后调 clear()。
#pragma pack(push, 1)
struct TAG_CHARAC_ETC_DATA
{
    struct Row
    {
        _Mastered_skill m_skills[0xcb];  // +0x00（0x196）
        char m_pad[2];                   // +0x196（行步长 0x198）
    };
    Row m_rows[2];                       // 2×0x198 = 0x330

    TAG_CHARAC_ETC_DATA() { clear(); }   // ORIG C1 0x0848f7dc
    void clear();              // ORIG W 0x822edf4
};
static_assert(sizeof(TAG_CHARAC_ETC_DATA) == 0x330, "TAG_CHARAC_ETC_DATA ABI drift");
#pragma pack(pop)

#pragma pack(push, 1)
struct SIG_LOAD_CHARAC
{
    unsigned int     m_accId;           // +0x000
    unsigned int     m_characNo;        // +0x004
    int              m_result;          // +0x008
    CHARAC_DATA      m_characData;      // +0x00c（getCharacInfoDetail 写；尾部含 m_schoolNo）
    unsigned int     m_tagCharacNo;     // +0x686
    unsigned char    m_strikerSkillIndex; // +0x68a
    TAG_CHARAC_DATA   m_tagCharacData;  // +0x68b（0x78 字节；getTagCharacInfo 写 + CUser.h:7469 直接
                                        //   透传，clear() 为 ORIG W 0x822ed84；类型化保 .clear() 调用）
};
static_assert(sizeof(SIG_LOAD_CHARAC) == 0x703, "SIG_LOAD_CHARAC ABI drift");
// 注：SIG_LOAD_CHARAC 含 non-trivial 成员（CHARAC_DATA 用户构造），GCC4.4
// offsetof 对其未定义，偏移由成员声明序 + pack(1) 保证，删除 offsetof 断言。
#pragma pack(pop)

struct SIG_LOAD_CHARAC_HOTKEY_OPTION
{
    char m_pad[0xc4];
};

struct SIG_LOAD_CLEANPAD_POINT
{
    char m_pad[0x3cf0];
};

// 字段化（LinkClosureB-2，DB_LoadDimension::makeRequest 0x08431d8c 证据，
// sizeof 不变）：
struct SIG_LOAD_DIMENSION
{
    unsigned int m_field00;        // +0x00（makeRequest 写 accId 类参数）
    char m_dimensionInout[6];      // +0x04..+0x09（get_dimensionInout(0..5)）
    char m_pad0a[2];               // +0x0a..+0x0b
    char m_limitInoutCount;        // +0x0c（get_limit_inout_count(2)）
    char m_pad0d[3];
};

struct SIG_LOAD_DNF_HACK_LOG
{
    char m_pad[0x3cf4];
};

// ORIG sizeof = 0x1810（CACHE_CHARACTER_TYPE@+0x1d2f3 → +0x1eb03）。
// CUser::GetDungeonClear @0x864de58：+0xc 清零 int，+0x10 起 CDungeonClear::toString(buf,0x1800)。
struct SIG_LOAD_DUNGEON_CLEAR
{
    unsigned int m_characNo;       // +0x000（RequestDBDungeonClear @0x81c7436 ← get_charac_no）
    unsigned int m_accId;          // +0x004（同上 ← get_acc_id）
    char         m_pad8[4];        // +0x008
    int      m_selected;           // +0x00c
    char     m_clearData[0x1800];  // +0x010
};
static_assert(sizeof(SIG_LOAD_DUNGEON_CLEAR) == 0x1810, "SIG_LOAD_DUNGEON_CLEAR ABI drift");
static_assert(offsetof(SIG_LOAD_DUNGEON_CLEAR, m_selected) == 0x00c, "SIG_LOAD_DUNGEON_CLEAR m_selected offset drift");

// ORIG sizeof = 0x10（CACHE_CHARACTER_TYPE@+0x1eb03 → +0x1eb13）。
// RequestBlackCount @0x81c79f2：memset 0x10 后 +0x0 写 CUser::get_charac_no(slot)。
// CUser::GetBlackListCount @0x867f1d4：+0x4 清零，+0x8 = getCurrCharacBlackCount()。
struct SIG_REQ_BLACK_COUNT
{
    unsigned int m_characNo;   // +0x000（RequestBlackCount 写入）
    int      m_count;          // +0x004
    int      m_blackCount;     // +0x008
    char     m_pad_c[0x10 - 0xc];
};
static_assert(sizeof(SIG_REQ_BLACK_COUNT) == 0x10, "SIG_REQ_BLACK_COUNT ABI drift");
static_assert(offsetof(SIG_REQ_BLACK_COUNT, m_characNo) == 0x000, "SIG_REQ_BLACK_COUNT m_characNo offset drift");
static_assert(offsetof(SIG_REQ_BLACK_COUNT, m_count) == 0x004, "SIG_REQ_BLACK_COUNT m_count offset drift");
static_assert(offsetof(SIG_REQ_BLACK_COUNT, m_blackCount) == 0x008, "SIG_REQ_BLACK_COUNT m_blackCount offset drift");

// ORIG sizeof = 0x11（GetEtcInfo 传 &sig+0x14a99，下一字段 combo +0x14aaa）。
// CUser::GetCharacCashAttendanceInfo @0x86910a6 仅清零首字节；
// Inter_LoadEtc::processAttendAnceEvent @0x84e807e 把首字节读作启用标志(bool)。
// 其余 0x10 字节仅作为 blob 在 DB/Inter_LoadEtc 间传递，无字段级访问 → 保留 pad。
struct SIG_ATTENDANCE_EVENT
{
    unsigned char m_isEnabled;   // +0x00（启用标志；GetCharacCashAttendanceInfo 清零，processAttendAnceEvent 读作 bool）
    unsigned char m_pad[0x10];   // +0x01（opaque blob，无 ORIG 字段级证据）

    // ORIG W 0x0848fb84：仅 movb $0,(%eax)（只清首字节）。
    SIG_ATTENDANCE_EVENT() { m_isEnabled = 0; }
};
static_assert(sizeof(SIG_ATTENDANCE_EVENT) == 0x11, "SIG_ATTENDANCE_EVENT ABI drift");

// ORIG sizeof = 0x14b60（CACHE_CHARACTER_TYPE@+0x1fd20 → +0x34880 cacheTime）。
// CUser::GetEtcInfo @0x867794c 依此偏移填充；子对象区（skill/expand/aura/option）
// 为可重叠序列化区，经命名数组 + 文档化偏移访问（同 GetContinuousItemInfo 约定）。
// 字节打包：ORIG 在 +0xc9b3（奇数偏移）写 dword，故必须 pack(1)。
#pragma pack(push, 1)
struct SIG_LOAD_ETC
{
    unsigned int m_accId;                              // +0x000（RequestDBETC ← get_acc_id）
    unsigned int m_characNo;                           // +0x004（RequestDBETC ← get_charac_no）
    int      m_field8;                                 // +0x008（GetEtcInfo 先清零）
    unsigned int m_fieldC;                             // +0x00c（RequestDBETC ← MSG_CHARAC_SELECT.m_0e，
                                                       //       同 SIG_LOAD_CHARAC.m_tagCharacNo 数据源）
    char     m_tagSkillSlot[0x198];                  // +0x010（get_skill_slot out1）
    char     m_tagSkillCombo[0x198];                 // +0x1a8（get_skill_slot out2）
    PvpResultType m_pvpResult;                       // +0x340（0x84）
    int      m_recordCount;                          // +0x3c4
    GameResultType m_records[0x64];                  // +0x3c8（0x64×0x50 = 0x1f40）
    char     m_pad2308[0x29fc - (0x3c8 + 0x1f40)];   // +0x2308（0x6f4）
    unsigned char m_oneDayLetheFlag;                 // +0x29fc
    char     m_backupSkill[0x199];                   // +0x29fd
    unsigned char m_oneDayLetheFlag2ND;              // +0x2b96
    char     m_backupSkill2ND[0x199];                // +0x2b97
    char     m_pad2d30[0x2d30 - (0x2b97 + 0x199)];     // +0x2d30 前垫
    unsigned int m_serverGroup;                        // +0x2d30（RequestDBETC ← G_CEnvironment()->m_serverEnvir.m_serverGroup）
    int      m_channelNo;                              // +0x2d34（RequestDBETC ← CEnvironment::get_channel_no）
    char     m_pad2d38[0x40c8 - 0x2d38];               // +0x2d38
    unsigned short m_curConditionEventStep;          // +0x40c8
    unsigned short m_curConditionEventRewardStep;    // +0x40ca
    unsigned short m_properDungeonClearCount;        // +0x40cc
    char     m_expandData6[0x466a - 0x40ce];         // +0x40ce（type 6）
    char     m_expandDataE[0xb1a4 - 0x466a];         // +0x466a（type 0xe）
    char     m_expandDataF[0xc9ac - 0xb1a4];         // +0xb1a4（type 0xf）
    unsigned char m_curVillageAttackCount;           // +0xc9ac
    unsigned char m_curRevengeDungeonCount;          // +0xc9ad
    unsigned char m_flagPresentAvengerTitle;         // +0xc9ae
    char     m_padc9af[0xc9b3 - 0xc9af];             // +0xc9af
    int      m_totalPcRoomPlayTime;                  // +0xc9b3
    unsigned char m_pcRoomPlayTimeRewardCnt;         // +0xc9b7
    int      m_curCharacUsedGiftFatigueQuantity;     // +0xc9b8
    unsigned int m_bitset;                           // +0xc9bc（std::bitset<32>，4 字节）
    char     m_mixedRegion[0x14a98 - 0xc9c0];        // +0xc9c0（skill + expand8/7/b/10/d/10 + aura + option）
    unsigned char m_blueMarbleEnterCount;            // +0x14a98
    SIG_ATTENDANCE_EVENT m_attendanceEvent;          // +0x14a99
    char     m_comboSkill[0x14b60 - 0x14aaa];        // +0x14aaa

    // ORIG D1/D2 W 0x084921f0：GameResultType[0x64] 析构循环 + PvpResultType 析构。
    ~SIG_LOAD_ETC() {}
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_ETC) == 0x14b60, "SIG_LOAD_ETC ABI drift");

struct SIG_LOAD_EVENTITEM
{
    char m_pad[0x5c];
};

// ---- 临时生物期限（ORIG C1 W 0x0848f976：两 int 均置 -1）----
// 使用点：CInventory::LoadCreatureTempPeriod（CInventory.h 前向声明）、
// SIG_LOAD_INVENTORY m_tempPeriods[0x14]（ORIG ctor 步长 8）。
struct SIG_POSTAL_CREATURE_TEMP_PERIOD
{
    int m_uid;           // +0x00
    int m_tempPeriod;    // +0x04

    SIG_POSTAL_CREATURE_TEMP_PERIOD()   // ORIG W 0x0848f976
    {
        m_uid = -1;
        m_tempPeriod = -1;
    }
};

struct SIG_LOAD_GAME_OPTION
{
    char m_pad[0x4b0];
};

// ORIG SIG_SAVE_PC_ROOM_RENT_ITEM（CUser::GetPcRoomRentInfo @0x86769ea 填充）：
// 容器 2 个 0xc 条目；条目类型权威名 SIG_SAVE_PC_ROOM_RENT_INFO
//（W ctor 0x848f98e / clear 0x0848f9a2：+0 int=0 / +4,+5,+6 byte=0 / +8 int=-1）。
// ITEM ctor 0x8491cbe 仅构造 entry[0]；ITEM clear 0x8694c9a 仅清 entry[0]。
#pragma pack(push, 1)
struct SIG_SAVE_PC_ROOM_RENT_INFO
{
    int  m_itemIndex;   // +0x00
    char m_field4;      // +0x04
    char m_field5;      // +0x05
    char m_inEquip;     // +0x06
    char m_pad7;        // +0x07
    int  m_field8;      // +0x08

    SIG_SAVE_PC_ROOM_RENT_INFO() { clear(); }   // ORIG W 0x848f98e（内联，转调 clear）
    void clear();                               // ORIG W 0x0848f9a2
};
struct SIG_SAVE_PC_ROOM_RENT_ITEM
{
    SIG_SAVE_PC_ROOM_RENT_INFO m_entry[2];
    SIG_SAVE_PC_ROOM_RENT_ITEM() {}               // ORIG W 0x8491cbe（隐式逐元素构造 entry[0..1]）
    void clear();                                 // ORIG W 0x08694c9a
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_PC_ROOM_RENT_ITEM) == 0x18, "SIG_SAVE_PC_ROOM_RENT_ITEM ABI drift");
static_assert(sizeof(SIG_SAVE_PC_ROOM_RENT_INFO) == 0xc, "SIG_SAVE_PC_ROOM_RENT_INFO ABI drift");

// ORIG SIG_SAVE_DIMENSION_RENT_ITEM（CUser::GetDimensionRentInfo @0x8676bec 填充）：
// 容器 12 个 8 字节条目；条目类型权威名 SIG_SAVE_DIMENSION_RENT_INFO
//（W ctor 0x848f9d0 / clear 0x0848f9e4：+0 int=0 / +4..+7 byte=0）。
// ITEM ctor 0x8491cf2 仅构造 entry[0..10]；ITEM clear 0x8694cd2 仅清 entry[0..10]。
#pragma pack(push, 1)
struct SIG_SAVE_DIMENSION_RENT_INFO
{
    int  m_itemIndex;   // +0x00
    char m_field4;      // +0x04
    char m_field5;      // +0x05
    char m_inEquip;     // +0x06
    char m_field7;      // +0x07

    SIG_SAVE_DIMENSION_RENT_INFO() { clear(); }   // ORIG W 0x848f9d0（内联，转调 clear）
    void clear();                                 // ORIG W 0x0848f9e4
};
struct SIG_SAVE_DIMENSION_RENT_ITEM
{
    SIG_SAVE_DIMENSION_RENT_INFO m_entry[12];
    SIG_SAVE_DIMENSION_RENT_ITEM() {}             // ORIG W 0x8491cf2（隐式逐元素构造 entry[0..11]）
    void clear();                                 // ORIG W 0x08694cd2
};
#pragma pack(pop)
static_assert(sizeof(SIG_SAVE_DIMENSION_RENT_ITEM) == 0x60, "SIG_SAVE_DIMENSION_RENT_ITEM ABI drift");
static_assert(sizeof(SIG_SAVE_DIMENSION_RENT_INFO) == 8, "SIG_SAVE_DIMENSION_RENT_INFO ABI drift");

// ORIG sizeof = 0x1544c（CACHE_CHARACTER_TYPE@+0x703 → +0x15b4f）。
// 填充函数与关键偏移（GetInventoryInfo @0x864d4ba / GetInventory @0x864d646 /
// GetCreatureItem @0x833b80c / GetRentInfo @0x86769b4）：
//   +0x0000 背包序列化区起 / +0x000c TAG_CHARAC_INVEN_DATA（getTagCharacInventory）/
//   +0x4030 result(0 初始,失败 1) / +0x4034 money / +0x4038 coin / +0x403c payCoin /
//   +0x4040 eventCoin / +0x4044 invenType2(0x1905) / +0x5949 invenType1(0x4a58) /
//   +0xa3a1 装备镜像(0x53e) / +0xa8e0 cargo 容量 / +0xa8e4 cargo / +0xcd1c avatar 计数 /
//   +0xcd20 avatar / +0xeed1 invenType3(0x39aa) / +0x12787 宠物尾(0xf4) /
//   +0x1287c creature 计数 / +0x12880 creature(0x1ed8) / +0x14758 clearAvatar /
//   +0x14780 avatarCoin / +0x14784 expand type2 / +0x14aa4 expand type9 /
//   +0x1533c inventory_capacity / +0x15340 creature2 计数 / +0x153e4 pcroom rent /
//   +0x153f0 dimension rent / +0x15448 maxEquipLevel。字节打包（+0x5949 等奇偏移）。
#pragma pack(push, 1)
struct SIG_LOAD_INVENTORY
{
    unsigned int m_accId;                          // +0x000（RequestDBInventory_forchina ← get_acc_id）
    unsigned int m_characNo;                       // +0x004（同上 ← get_charac_no）
    unsigned int m_field8;                         // +0x008（← MSG_CHARAC_SELECT.m_0e）
    TAG_CHARAC_INVEN_DATA m_tagInven;              // +0x00c（getTagCharacInventory 填充；
                                                   //   RequestDBInventory_forchina 调 clear）
    int      m_result;                            // +0x4030
    int      m_money;                             // +0x4034
    unsigned int m_coin;                          // +0x4038
    unsigned int m_payCoin;                       // +0x403c
    unsigned int m_eventCoin;                     // +0x4040
    // ORIG C1 0x08491d26：TAG_CHARAC_INVEN_DATA → Inven_Item[0x69]@+0x4044 →
    // Inven_Item[0x138]@+0x5949 → Inven_Item[0x16]@+0xa3a1 → Inven_Item[0x98]@+0xa8e4 →
    // Inven_Item[0xf2]@+0xeed1 → stItemLockInfo[0x64]@+0x14784 →
    // SIG_SAVE_EXPAND_EQUIPSLOT@+0x14aa4 → SIG_POSTAL_CREATURE_TEMP_PERIOD[0x14]@+0x15344。
    Inven_Item m_invenType2[0x69];                // +0x4044（0x69×0x3d = 0x1905 → +0x5949）
    Inven_Item m_invenType1[0x138];               // +0x5949（0x138×0x3d = 0x4a58 → +0xa3a1）
    Inven_Item m_equipMirror[0x16];               // +0xa3a1（0x16×0x3d = 0x53e → +0xa8df）
    char m_padA8df[0xa8e0 - 0xa8df];              // +0xa8df（1 字节）
    unsigned int m_cargoCapacity;                 // +0xa8e0（CCargo::GetCapacity）
    Inven_Item m_cargo[0x98];                     // +0xa8e4（0x98×0x3d = 0x2438 → +0xcd1c）
    int      m_avatarCount;                       // +0xcd1c
    char     m_avatarDetail[0xeed1 - 0xcd20];     // +0xcd20（AvatarDetailInfo）
    Inven_Item m_invenType3[0xf2];                // +0xeed1（0xf2×0x3d = 0x39aa → +0x1287b）
    char     m_pad1287b[0x1287c - 0xeed1 - 0x39aa]; // +0x1287b（creatureCount 前 1 字节）
    int      m_creatureCount;                     // +0x1287c
    char     m_creature[0x1ed8];                  // +0x12880（CCreatureMgr::GetCreatureItem）
    char     m_clearAvatar[0x14780 - 0x14758];    // +0x14758（GetClearAvatar）
    unsigned int m_avatarCoin;                    // +0x14780（AvatarCoin::Get）
    stItemLockInfo m_locks[0x64];                 // +0x14784（0x64×8 = 0x320 → +0x14aa4）
    SIG_SAVE_EXPAND_EQUIPSLOT m_expandEquip;      // +0x14aa4（0x898 → +0x1533c）
    int      m_inventoryCapacity;                 // +0x1533c（get_inventory_capacity）
    int      m_creatureCount2;                    // +0x15340（GetCreatureItem 第二区计数）
    SIG_POSTAL_CREATURE_TEMP_PERIOD m_tempPeriods[0x14];  // +0x15344（0x14×8 = 0xa0 → +0x153e4）
    char     m_rentPcRoom[0xc];                   // +0x153e4（SIG_SAVE_PC_ROOM_RENT_ITEM）
    char     m_rentDimension[0x15448 - 0x153f0];  // +0x153f0（SIG_SAVE_DIMENSION_RENT_ITEM）
    int      m_maxEquipLevel;                     // +0x15448

    SIG_LOAD_INVENTORY() {}                       // ORIG W 0x08491d26（数组成员隐式构造）
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_INVENTORY) == 0x1544c, "SIG_LOAD_INVENTORY ABI drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_result) == 0x4030, "SIG_LOAD_INVENTORY m_result offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_money) == 0x4034, "SIG_LOAD_INVENTORY m_money offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_coin) == 0x4038, "SIG_LOAD_INVENTORY m_coin offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_payCoin) == 0x403c, "SIG_LOAD_INVENTORY m_payCoin offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_eventCoin) == 0x4040, "SIG_LOAD_INVENTORY m_eventCoin offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_cargoCapacity) == 0xa8e0, "SIG_LOAD_INVENTORY m_cargoCapacity offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_avatarCount) == 0xcd1c, "SIG_LOAD_INVENTORY m_avatarCount offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_creatureCount) == 0x1287c, "SIG_LOAD_INVENTORY m_creatureCount offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_avatarCoin) == 0x14780, "SIG_LOAD_INVENTORY m_avatarCoin offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_inventoryCapacity) == 0x1533c, "SIG_LOAD_INVENTORY m_inventoryCapacity offset drift");
static_assert(offsetof(SIG_LOAD_INVENTORY, m_maxEquipLevel) == 0x15448, "SIG_LOAD_INVENTORY m_maxEquipLevel offset drift");

// ORIG sizeof = 0xafcc（RequestDBKillMonsterInfo @0x81c7836 memset 0xafcc；
// charac_no 写于 +0xafc8 尾部）。
// ORIG C1 0x0848fc8e：stMonsterInfo_t[0x5dc]@+0 → [0x1770]@+0x1d4c → [0x5dc]@+0x927c
// 三组倒计数构造（0x5dc×5=0x1d4c；0x1770×5=0x7530；步长 5，P151）。
// stMonsterInfo_t（pack(1) {uint mobIdx; uchar diff;}）与 CKillMonsterInfo_chain.cpp
// 的 TU 局部定义同构（ODR 相同）；该 cpp 不 include 本头，无冲突。
#pragma pack(push, 1)
struct stMonsterInfo_t
{
    // ORIG C1/C2 0x0848fc78（W）：mobIdx=0、diff=0。
    stMonsterInfo_t()
    {
        mobIdx = 0;
        diff = 0;
    }
    unsigned int  mobIdx;   // +0x00
    unsigned char diff;     // +0x04
};
struct SIG_LOAD_KILL_MONSTER_INFO
{
    stMonsterInfo_t m_boss[0x5dc];      // +0x000（0x1d4c）
    stMonsterInfo_t m_named[0x1770];    // +0x1d4c（0x7530 → +0x927c）
    stMonsterInfo_t m_apc[0x5dc];       // +0x927c（0x1d4c → +0xafc8）
    unsigned int m_characNo;            // +0xafc8

    SIG_LOAD_KILL_MONSTER_INFO() {}     // ORIG W 0x0848fc8e（数组成员隐式构造）
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_KILL_MONSTER_INFO) == 0xafcc, "SIG_LOAD_KILL_MONSTER_INFO ABI drift");
static_assert(offsetof(SIG_LOAD_KILL_MONSTER_INFO, m_characNo) == 0xafc8, "SIG_LOAD_KILL_MONSTER_INFO m_characNo offset drift");

struct SIG_LOAD_NO_CACHE_DATA
{
    char m_pad[0x2c];
};

struct SIG_LOAD_POWER_WAR_STATUE_INFO
{
    char m_pad[0x80];
};

struct SIG_LOAD_PU_USER_INFO
{
    char m_pad[0x1];
};

// ---- PvP 好友列表（Community SendLogin 载荷）----
// ORIG：CCommunityServerProxy::SendLogin 0x0846ca06 实证——
//   +0x00 int m_buddyCount（SendLogin 拷 param_2[0] → 包 +0x3e buddyCount）
//   +0x04 STPvPBuddyDBInfo[0x20]（SendLogin memcpy 0x560 → 包 +0x42 buddies）
//   sizeof = 0x564。DB 侧（Inter_LoadPvPBuddy 域）填充。
struct SIG_LOAD_PVP_BUDDY
{
    int m_buddyCount;                    // +0x00
    STPvPBuddyDBInfo m_buddies[0x20];    // +0x04（0x2b × 0x20 = 0x560）
};
static_assert(sizeof(SIG_LOAD_PVP_BUDDY) == 0x564, "SIG_LOAD_PVP_BUDDY ABI drift");
static_assert(offsetof(SIG_LOAD_PVP_BUDDY, m_buddyCount) == 0x00, "SIG_LOAD_PVP_BUDDY m_buddyCount offset drift");
static_assert(offsetof(SIG_LOAD_PVP_BUDDY, m_buddies) == 0x04, "SIG_LOAD_PVP_BUDDY m_buddies offset drift");
// ORIG sizeof = 0x7608（RequestDBQuest @0x81c70c2 memset 0x7608；布局与
// UserQuest.cpp 本地视图/get_quest@0x869067c 族一致：m_cleared 0x7530 位图 +
// m_authen 0x28）。
// ORIG C1 0x08491e92：+0xb0 起 WongWork::STClearedQuest C1 × 0x7530（步长 1，
// 倒计数 P151；STClearedQuest 为 1 字节 {uchar}）。
#pragma pack(push, 1)
struct SIG_LOAD_QUEST
{
    unsigned int m_characNo;        // +0x000（RequestDBQuest 写入）
    int          m_field04;         // +0x004
    char         m_pad08[4];        // +0x008
    int          m_quest[20];       // +0x00c
    int          m_trigger[20];     // +0x05c
    int          m_urgentQuest;     // +0x0ac
    WongWork::STClearedQuest m_cleared[0x7530]; // +0x0b0（任务完成位图，1 字节/项）
    char         m_authen[0x28];    // +0x75e0

    SIG_LOAD_QUEST() {}             // ORIG W 0x08491e92（数组成员隐式构造）
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_QUEST) == 0x7608, "SIG_LOAD_QUEST ABI drift");
static_assert(offsetof(SIG_LOAD_QUEST, m_cleared) == 0xb0, "SIG_LOAD_QUEST m_cleared offset drift");

// 具名化（证据：docs/class_func_reports/CUser/GetSkill.md ORIG 0x864dcf2 反编译段：
// +0x4 清零；SkillSlot::get_remain_sp(skill,+8,+0x1a4,+0x340,+0x344)（本体 0x8694ec0
// 依序拷 SkillSlot+0x4/+0xc/+0x14/+0x18 → SP1/SP2/SFP1/SFP2）；
// SkillSlot::get_skill_slot(skill,+0xc,+0x1a8,0x198) 两 0x198 槽区；
// get_skill_command_buf 后 memcpy(+0x348, buf, 200)；
// quest_reward.md 0x0864xxxx 写 +0x410=1；+0x411 ← CDataManager+0xa850）。
// 0x358..0x410 无字段级证据保留 pad。
#pragma pack(push, 1)
struct SIG_LOAD_SKILL
{
    unsigned char m_pad0[4];        // +0x000（GetSkill 不触碰）
    int m_field04;                  // +0x004（GetSkill 清零）
    int m_remainSpTree1;            // +0x008（SkillSlot+0x4）
    _Mastered_skill m_skillSlotTree1[0xcb];  // +0x00c（0xcb×2 = 0x196 → +0x1a2）
    char m_pad1a2[2];               // +0x1a2
    int m_remainSpTree2;            // +0x1a4（SkillSlot+0xc）
    _Mastered_skill m_skillSlotTree2[0xcb];  // +0x1a8（0x196 → +0x33e）
    char m_pad33e[2];               // +0x33e
    int m_remainSfpTree1;           // +0x340（SkillSlot+0x14）
    int m_remainSfpTree2;           // +0x344（SkillSlot+0x18）
    STSkillCommandData::STCommand m_commands[0x31];  // +0x348（0x31×4 = 0xc4 → +0x40c）
    char m_pad40c[4];               // +0x40c
    bool m_flag410;                 // +0x410（quest_reward 写 1；SaveSkill 传 get_skill_table_name 直读）
    unsigned char m_field411;       // +0x411（← G_CDataManager()+0xa850）
    char m_pad412[2];               // +0x412（对齐尾，ORIG memset 覆盖范围 0x414）

    // ORIG W 0x08491ee0：_Mastered_skill[0xcb]×2 + STCommand[0x31] 数组成员隐式构造。
    SIG_LOAD_SKILL() {}
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_SKILL) == 0x414, "SIG_LOAD_SKILL ABI drift");

// ORIG sizeof = 0x19c（CACHE_CHARACTER_TYPE@+0x1d157 → +0x1d2f3）。
// CUser::GetTicket @0x867846e：+0x4 清零 / +0x8 count / +0xc int[100] 票表（list<int> 遍历，
// 上限 99）。字节打包（+0x0c 奇偏移起）。
#pragma pack(push, 1)
struct SIG_LOAD_TICKET
{
    char     m_pad0[0x4];     // +0x000
    int      m_field4;        // +0x004（GetTicket 先清零）
    int      m_count;         // +0x008
    int      m_tickets[100];  // +0x00c
};
#pragma pack(pop)
static_assert(sizeof(SIG_LOAD_TICKET) == 0x19c, "SIG_LOAD_TICKET ABI drift");
static_assert(offsetof(SIG_LOAD_TICKET, m_field4) == 0x004, "SIG_LOAD_TICKET m_field4 offset drift");
static_assert(offsetof(SIG_LOAD_TICKET, m_count) == 0x008, "SIG_LOAD_TICKET m_count offset drift");
static_assert(offsetof(SIG_LOAD_TICKET, m_tickets) == 0x00c, "SIG_LOAD_TICKET m_tickets offset drift");

// ---- 雇佣兵加载结构（vector 元素；ORIG 步长 0x54，见 _M_insert_aux/ix imul $0x54） ----
struct CHARAC_LOAD_MERCENARY
{
    // ORIG C1 W 0x08694c08：+0/+4/+0x24/+0x28/+0x2c 清零、+0x30/0x31=0xff、
    // +0x32=0xa、memset(+6,0,0x1e)、memset(+0x34,0,0x20)（定义在
    // CUserMissingA2.cpp，标量在前两 memset 在后的独立展开，勿调 Reset）。
    CHARAC_LOAD_MERCENARY();
    ~CHARAC_LOAD_MERCENARY() {}
    void Reset();                 // ORIG T 0x081b410a
    int GetCompetitionState() const;  // ORIG T 0x081b2e86
    // ---- 雇佣兵竞技域方法声明（CUserMissingA2.cpp 独占定义；ORIG 报告齐备）----
    int CalcItemDropRate(CUser*) const;                  // ORIG T 0x081b2d6a
    bool CheckCompetitionAreaIndex(char, int) const;     // ORIG T 0x081b2b34
    bool CheckCompetitionPeriodIndex(char) const;        // ORIG T 0x081b2bc4
    bool Competition(CUser*, char, char);                // ORIG T 0x081b3814
    bool GetRewardItem(CUser*, competition_reward_result&, Inven_Item&,
                       std::string&);                    // ORIG T 0x081b3276
    int Reward(CUser*, competition_reward_result&);      // ORIG T 0x081b3b68
    int GetRewardGold(CUser*, competition_reward_result&) const;  // ORIG T 0x081b3046
    float GetAvatarBonus() const;                        // ORIG T 0x081b2c94
    float GetPeriodBonus() const;                        // ORIG T 0x081b2c1e
    float GetRewardCritical(int) const;                  // ORIG T 0x081b2d0a
    int GetCompetitionPeriodTerm(bool*) const;           // ORIG T 0x081b2f32
    int GetCompetitionPeriodSecond(char) const;          // ORIG T 0x081b2ed2
    void SendMessageToCUser(CUser*, char*) const;        // ORIG T 0x081b4190（空体）
    void alter();                                        // ORIG W 0x081b47c8（+0x24 写 1）
    void save_complete();                                // ORIG W 0x08694c8e（+0x24 写 0）

    int m_characNo;               // +0x00（FindIndexByCharacNo 按首 dword 无符号比较）


// NOTE_MARKER_NOOP
    short m_level;                // +0x04（AddCurCharacMercenaryInfo 写 charac+0x27）
    char m_name[0x1e];            // +0x06..0x23（Reset memset(this+6,0,0x1e)）
    unsigned char m_field24;      // +0x24（Reset 写 0）
    char m_pad25[3];              // +0x25..0x27（Reset 不触碰）
    int m_field28;                // +0x28（Reset 写 0）
    int m_field2c;                // +0x2c（Reset 写 0；GetCompetitionState 竞技结束秒）
    unsigned char m_field30;      // +0x30（Reset 写 0xff；GetCompetitionState 未报名标志）
    unsigned char m_field31;      // +0x31（Reset 写 0xff）
    unsigned char m_avatarBonus;  // +0x32 avatar 等级加成：Reset 写 0xa=CalcMercenaryAvatarBonus
                                  //   初值；setAvatarTypeBonus(0x0868ebaa) 写、GetAvatarBonus
                                  //   (0x081b2c94) 读、AddCharac(0x081b4300) 按 m_avatarItems 重算
    unsigned char m_pad33;        // +0x33（Reset 不触碰）
    int m_avatarItems[8];         // +0x34..0x53（Reset memset(this+0x34,0,0x20)；AddCharac 循环读取重算 bonus）
};

static_assert(sizeof(CHARAC_LOAD_MERCENARY) == 0x54,
              "CHARAC_LOAD_MERCENARY ABI drift (ORIG vector stride 0x54 @0x81b4ba9)");
#ifndef DNF_SIG_GIVE_GROWTH_CREATURE_EVENT_DEFINED
#define DNF_SIG_GIVE_GROWTH_CREATURE_EVENT_DEFINED
// Wave5 APSystem 批调和 + Inter_LoginInfo/dispatch_sig 权威化（与 MoreTypes.h
// 同一守卫互斥；SetGrowthCreatureEventInfo(SIG const&) 0x8691cde stride 0x2c 循环，
// 缓冲区总长 0xb*0x2c = 0x1e4 = SIG_LOGIN_DATA+0x4154..0x4338）。
struct SIG_GIVE_GROWTH_CREATURE_EVENT
{
    struct eventCharacInfo
    {
        unsigned short m_serverGroup;   // +0x00
        int m_characNo;                 // +0x04
        int m_job;                      // +0x08
        char m_name[30];                // +0x0c..0x29

        eventCharacInfo() : m_serverGroup(0), m_characNo(0), m_job(0)
        {
            m_name[0] = '\0';
        }
    };
    static_assert(sizeof(eventCharacInfo) == 0x2c, "eventCharacInfo size");

    eventCharacInfo m_entries[11];      // stride 0x2c × 0xb = 0x1e4
};
static_assert(sizeof(SIG_GIVE_GROWTH_CREATURE_EVENT) == 0x1e4,
                            "SIG_GIVE_GROWTH_CREATURE_EVENT ABI drift");
#endif  // DNF_SIG_GIVE_GROWTH_CREATURE_EVENT_DEFINED

// ---- 登录域整包转发视图（dispatch_sig 只作整包指针转发；内部字段归各 owner 批次）
struct SIG_INGAME_EVENT_HISTORY       { unsigned char m_data[0x178]; };  // +0x3fac..0x4123
static_assert(sizeof(SIG_INGAME_EVENT_HISTORY) == 0x178, "SIG_INGAME_EVENT_HISTORY size");
struct SIG_GROWTH_WEAPON_EVENT        { unsigned char m_data[0x14]; };   // +0x4140..0x4153
static_assert(sizeof(SIG_GROWTH_WEAPON_EVENT) == 0x14, "SIG_GROWTH_WEAPON_EVENT size");
struct SIG_LEVELUP_GIFT_ACCOUNT_ONCE  { unsigned char m_data[0x20]; };   // +0x4338..0x4357
static_assert(sizeof(SIG_LEVELUP_GIFT_ACCOUNT_ONCE) == 0x20, "SIG_LEVELUP_GIFT_ACCOUNT_ONCE size");

// 金币控制统计结构（Secu_GoldControl 用），ORIG sizeof = 0x3c。
// 6 组 64 位值（gold/import/sub/importSub/auctionAdd/auctionSub），后随 6 个计数器。
// 方法 C1/C2/reset/toCopy 为 ORIG 头文件内联弱符号（nm W）：
//  - C1：6 组 64 位逐组 movl $0,(%eax)+movl $0,0x4(%eax)（每组共读一次 this）
//  - reset：memset(this,0,0x3c)；toCopy：memcpy(&other,this,0x3c)
// （2026-08-27 自本文件尾段上移：SIG_LOGIN_DATA::m_goldInfo 值成员需完整类型。）
struct user_gold_info
{
    unsigned long long m_gold;         // +0x00
    unsigned long long m_import;       // +0x08
    unsigned long long m_sub;          // +0x10
    unsigned long long m_importSub;    // +0x18
    unsigned long long m_auctionAdd;   // +0x20
    unsigned long long m_auctionSub;   // +0x28
    unsigned short m_count[6];         // +0x30..0x3a

    user_gold_info()
        : m_gold(0), m_import(0), m_sub(0), m_importSub(0),
          m_auctionAdd(0), m_auctionSub(0), m_count{0,0,0,0,0,0}
    {}
    void reset() { memset(this, 0, 0x3c); }
    void toCopy(user_gold_info& other) { memcpy(&other, this, 0x3c); }
};
static_assert(sizeof(user_gold_info) == 0x3c, "user_gold_info ABI drift");

// ---- 冷却道具载入结构（SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO +0x4 区）----
// ORIG C1 0x0848fa0e：movb 0→+0；for i<=0x31 循环清零 {int,uint}@+4+i*8 两段
// （setle 有符号界，P2/P51）。CUser::LoadCoolTimeItemInfo（ORIG 0x08658ff6）
// 读 count@+0 与 entries{int itemIdx; uint endSec}。sizeof = 4+0x32*8 = 0x194。
struct CONT_ENTRY
{
    int m_itemIdx;          // +0x00
    unsigned int m_expireTime;  // +0x04
};
struct CONTINUOUS_ITEM_INFO
{
    unsigned char m_count;          // +0x00
    CONT_ENTRY m_entries[0x32];     // +0x04（0x32×8 = 0x190）

    CONTINUOUS_ITEM_INFO()          // ORIG W 0x0848fa0e（C1/C2 同址）
    {
        m_count = 0;
        for (int i = 0; i <= 0x31; ++i)
        {
            m_entries[i].m_itemIdx = 0;
            m_entries[i].m_expireTime = 0;
        }
    }
};

#endif  // DNF_GAME_SIG_TYPES_C_H_
