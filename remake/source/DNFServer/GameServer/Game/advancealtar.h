// advancealtar 基础类型声明。
#pragma once

#include <vector>
#include <map>
#include <cstring>

#include "advancealtar_StageDifficulty.h"  // StageDifficulty::T（Manager::selectStage 参数，ORIG mangled NS_15StageDifficulty1TE）

// ORIG 全局枚举（mangled 10ENUM_ERROR；权威定义 CUser.h/CMailBoxHelper.h，
// 同守卫幂等复制——本头不可反向 include CUser.h（重入环））。
#ifndef DNF_GAME_ENUM_ERROR_DEFINED
#define DNF_GAME_ENUM_ERROR_DEFINED
enum ENUM_ERROR { ENUM_ERROR_NONE = 0 };
#endif

class MSG_CHARAC_SELECT;   // 全局作用域（PacketTypes.h 权威定义；仅引用）
// 前置声明全局 CUser（实现 TU include CUser.h 获得完整类型）
class CUser;

namespace advancealtar
{

// CharacAdvanceAltarManager 权威定义在 CUser.h（唯一归口；本头前置声明，
// 供仅包含 advancealtar.h 的 TU 引用该类型）。
class CharacAdvanceAltarManager;

// ---- 子结构布局（advancealtar__DB_Load parse* 报告推导，尺寸与 ORIG 一致）----
struct _Stage { int m_field0; int m_field4; };            // 8 字节
#pragma pack(push, 1)
struct _Slot { unsigned char m_byte0; int m_int1; unsigned char m_byte5; int m_int6; };  // 10 字节
struct _Reward { int m_field0; unsigned char m_byte4; };  // 5 字节
#pragma pack(pop)
struct _BuyItem { int m_field0; int m_field4; short m_short8; short m_short10; };  // 12 字节

// _RidableBase（AdvanceAltarData 基类，+0x00；仅前 0x21 序列化，ORIG ctor 置 0）
struct _RidableBase
{
    _RidableBase() { std::memset(this, 0, 0x21); }
    int m_ridableId;         // +0x00（refillTicketFree 0x813159a 判定槽位坐骑）
    int m_ticket;            // +0x04（免费票数；refillTicketFree 写入配置值）
    char m_pad8[0x24 - 8];
};

// ORIG T 0x8898ff0：ridableId == 1。
bool isValidRidableId(int ridableId);

// _AdvanceAltarDbData：0x518（get() 序列化布局，pack(1) 匹配 ORIG 扁平 buffer）
#pragma pack(push, 1)
struct _AdvanceAltarDbData
{
    char m_ridable[0x21];            // +0x00
    _Stage m_stages[0x32];           // +0x21（50）
    _Slot  m_slots[0xc];             // +0x1b1（12）
    _BuyItem m_buyItems[0x32];       // +0x229（50）
    _Reward m_rewards[0x1d];         // +0x481（29）
    char m_pad512[5];                // +0x512
    unsigned char m_update;          // +0x517
};                                   // sizeof 0x518
#pragma pack(pop)

// _CharacAdvanceAltarDbData：characNo@+0 + _AdvanceAltarDbData@+4 = 0x51c
#pragma pack(push, 1)
struct _CharacAdvanceAltarDbData
{
    unsigned int m_characNo;         // +0x00
    _AdvanceAltarDbData m_db;        // +0x04
};                                   // sizeof 0x51c
#pragma pack(pop)

// AdvanceAltarData：0xd0（ctor 布局：_RidableBase@+0 + 7 maps + update@+0xcc）
class AdvanceAltarData : public _RidableBase
{
public:
    AdvanceAltarData();
    ~AdvanceAltarData();
    void reset();                              // ORIG T 0x08899244（memset 前 0x21 + m_update=0 + 7 map clear）
    void get(_AdvanceAltarDbData& out) const;  // ORIG T 0x088996d8
    void getSlot(const std::map<unsigned char,_Slot>& m, _Slot* out, int count) const;
    void getBuyItem(const std::map<int,_BuyItem>& m, _BuyItem* out, int count) const;

    std::map<int,_Stage> m_stages;           // +0x24
    std::map<int,_BuyItem> m_buyItems1;      // +0x3c
    std::map<int,_BuyItem> m_buyItems2;      // +0x54
    std::map<int,_BuyItem> m_buyItems3;      // +0x6c
    std::map<unsigned char,_Slot> m_slots1;  // +0x84
    std::map<unsigned char,_Slot> m_slots2;  // +0x9c
    std::map<int,_Reward> m_rewards;         // +0xb4
    unsigned char m_update;                  // +0xcc
};                                           // sizeof 0xd0

class DB_Update
{
public:
    static void makeRequest(int uid, const _CharacAdvanceAltarDbData& data);  // ORIG T 0x0813eb38
};


class DBAndExchangeServerProc
{
public:
    // ORIG T 0x0813394c（静态调用：ClassName::getFromMemory(user, data)）
    static void getFromMemory(const CUser& user, _CharacAdvanceAltarDbData& data);
    // ORIG T 0x08133784（SelectCharac::process 调用；返回 1，ORIG 体见
    // docs/class_func_reports/advancealtar__DBAndExchangeServerProc/）。
    static bool requestLoadFromDBAndES(CUser& user, MSG_CHARAC_SELECT& msg);
};

// ---- Manager（advancealtar__Manager/README.md，11 方法；ORIG asm 均不使用
// this，按静态成员声明——mangled 与非静态一致）。本批落地 selectStage/
// procErrorSelectStage（定义 advancealtar_Manager.cpp）；ctor/dtor 空体
// （0x08130902/0x08130908）与其余方法定义待后续批次。
class Manager
{
public:
    static int selectStage(CUser* user, int stageId, StageDifficulty::T diff);  // ORIG T 0x0813090e
    static void procErrorSelectStage(CUser* user, ENUM_ERROR err);              // ORIG T 0x081309f0
    static int finishLoad(CUser* user);                                         // ORIG T 0x08130eb2（DisP_GroupA 调用；定义待批次）
    static void dieCharac(CUser* user);                                         // ORIG T 0x08130ab2（DisP_GroupG 调用；定义待批次）
    static void giveUpGame(CUser* user);                                        // ORIG T 0x08130fa8（DisP_GroupG 调用；定义待批次）
};

// 布局断言（ORIG 尺寸：AdvanceAltarData 0xd0 / _AdvanceAltarDbData 0x518 /
// _CharacAdvanceAltarDbData 0x51c / 子结构 8/10/12/5；Manager 0xd4 断言在 CUser.h）
typedef char _assert_advancealtar_data_size[(sizeof(AdvanceAltarData) == 0xd0) ? 1 : -1];
typedef char _assert_advdb_size[(sizeof(_AdvanceAltarDbData) == 0x518) ? 1 : -1];
typedef char _assert_characadvdb_size[(sizeof(_CharacAdvanceAltarDbData) == 0x51c) ? 1 : -1];
typedef char _assert_stage_size[(sizeof(_Stage) == 8) ? 1 : -1];
typedef char _assert_slot_size[(sizeof(_Slot) == 10) ? 1 : -1];
typedef char _assert_buyitem_size[(sizeof(_BuyItem) == 12) ? 1 : -1];
typedef char _assert_reward_size[(sizeof(_Reward) == 5) ? 1 : -1];

// ---- ORIG 布局（advancealtar__TimeLine/Phase/TimeLineSummary 报告）----
struct _ActionAssign { char m_pad[0x10]; }; // ORIG ctor memset 0x10（0x889b99c）

struct Phase
{
    Phase();
    void reset();

    int m_field0;                           // +0x00
    int m_field4;                           // +0x04
    std::vector<_ActionAssign> m_assigns;   // +0x08
};                                          // sizeof 0x14

struct _PhaseSummary { char m_pad[0xc]; };  // ORIG ctor memset 0xc（0x889b954）

struct TimeLineSummary
{
    TimeLineSummary();
    ~TimeLineSummary();
    TimeLineSummary(const TimeLineSummary& other);
    TimeLineSummary& operator=(const TimeLineSummary& other);
    void reset();

    int m_field0;                               // +0x00
    std::vector<_PhaseSummary> m_phaseSummarys; // +0x04
};                                              // sizeof 0x10

class TimeLine
{
public:
    TimeLine();
    ~TimeLine();
    TimeLine(const TimeLine& other);
    TimeLine& operator=(const TimeLine& other);
    void reset();                           // ORIG T 0x08899dfa

private:
    std::vector<Phase> m_phases;            // +0x00
    TimeLineSummary m_summary;              // +0x0c
};                                          // sizeof 0x1c

}  // namespace advancealtar
