// W8LT_EventTail.h —— Batch05_EventTail(W8_LongTail) 独立事件小类族声明。
// OneADayItemShopEvent / GiveGrowCreatureEvent / SeriaRoomAniDecoEvent /
// SDCActiveStatusSyncCommon。全部签名/mangled 与 ORIG(df_game_r) 一致；
// 定义位于 W8LT_EventTail.cpp。
#ifndef GAME_W8LT_EVENTTAIL_H_
#define GAME_W8LT_EVENTTAIL_H_

#include "SigTypes.h"
#include "MoreTypes.h"
#include "CGuildServerProxy.h"   // CEventBase/CDelivery/DeliveryMsg/Word_Param
#include "DBDispatcher.h"        // DB_InsertEventStageLog 等权威基类（ORIG 单一类：static makeRequest + virtual dispatch）

class STSDCActiveStatusSyncDataSend;   // 前置：SDC 同步数据（仅 SDC TU 内按报告布局使用）
class IRDActiveObject;

namespace _SDC
{
// ORIG _ZN4_SDC12isObjectTypeEii T 0x080950e8。
// BLOCKED：定义属 ActiveStatus 域批次，本批不做桩实现。
bool isObjectType(int objectType, int code);
}

// ENUM_ACTIVESTATUS（ORIG mangled 宽度 17）。数值 = ACTIVESTATUS_* 表下标；
// 0..16 名称对应 ACTIVESTATUS_TAG[17]（ScriptDepsA.cpp .rodata 解析实证），
// 17 由 makeEtcParameters case 使用但无表项 [推断] 仅给占位名。
#ifndef DNF_ENUM_ACTIVESTATUS_DEFINED
#define DNF_ENUM_ACTIVESTATUS_DEFINED
enum ENUM_ACTIVESTATUS
{
    ACTIVE_STATUS_SLOW = 0,
    ACTIVE_STATUS_FREEZE = 1,
    ACTIVE_STATUS_POISON = 2,
    ACTIVE_STATUS_STUN = 3,
    ACTIVE_STATUS_CURSE = 4,
    ACTIVE_STATUS_BLIND = 5,
    ACTIVE_STATUS_LIGHTNING = 6,
    ACTIVE_STATUS_STONE = 7,
    ACTIVE_STATUS_SLEEP = 8,
    ACTIVE_STATUS_BURN = 9,
    ACTIVE_STATUS_WEAPON_BREAK = 10,
    ACTIVE_STATUS_BLEEDING = 11,
    ACTIVE_STATUS_PIERCING = 12,
    ACTIVE_STATUS_STUCK = 13,
    ACTIVE_STATUS_CONFUSE = 14,
    ACTIVE_STATUS_HOLD = 15,
    ACTIVE_STATUS_ARMOR_BREAK = 16,
    ACTIVE_STATUS_17 = 17   // [推断] makeEtcParameters case 0x11；语义待考证
};
#endif  // DNF_ENUM_ACTIVESTATUS_DEFINED

// ---- ORIG 投递日志扩长消息视图 [推断名；questClearLog/buyItemLog 实证] ----
// delivery(DeliveryMsg&) 收到的真实对象为事件日志扩长消息：基 DeliveryMsg 之外
// 携带 +0x10 日志类型、+0x14 载荷指针(+2 起为文本)、+0x20 进度 vector。
struct EventStageLogDeliveryMsg : public DeliveryMsg
{
    int m_logType;                                     // +0x10
    const unsigned char* m_payloadPtr;                 // +0x14（+2 起 \0 结尾文本）
    int m_field18;                                     // +0x18（未使用）
    int m_field1c;                                     // +0x1c（未使用）
    std::vector<std::pair<int, int> >* m_progressList;        // +0x20（ORIG：BOOST_FOREACH 非 const 实例）
};

// ---- 依赖的 DB request 类（权威声明；makeRequest 定义属 W8LT_EventTail.cpp，
// dispatch 定义属 W8LT_DBTask.cpp，与 ORIG 单一类一致） ----
class DB_InsertEventStageLog : public DBDispatcher
{
public:
    // _ZN22DB_InsertEventStageLog8dispatchEiiP6Stream T 0x08449144（W8LT_DBTask.cpp）
    // _ZN22DB_InsertEventStageLog11makeRequestER19SIG_EVENT_STAGE_LOG T 0x0844920e
    DB_InsertEventStageLog() {}                    // ORIG C1 0x844ff4a (sz 0x4: base ctor+vptr only)
    virtual bool dispatch(int userIdx, int slot, Stream* stream);
    static void makeRequest(SIG_EVENT_STAGE_LOG& sig);   // ORIG void（无 mov $1 前的 ret）
};

// _ZN28DB_EventSaveGiveGrowCreature11makeRequestEjRN30SIG_GIVE_GROWTH_CREATURE_EVENT15eventCharacInfoE
// T 0x08449b32；_ZN28DB_EventSaveGiveGrowCreature8dispatchEiiP6Stream T 0x08449c50。
class DB_EventSaveGiveGrowCreature : public DBDispatcher
{
public:
    DB_EventSaveGiveGrowCreature() {}              // ORIG C1 0x844fdfa (sz 0x4: base ctor+vptr only)
    static bool makeRequest(unsigned int accId,
                            SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo& info);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08449c50
};
class DB_EventUpdateGiveGrowCreature : public DBDispatcher
{
public:
    // ORIG T（地址见 nm _ZN29DB_EventUpdateGiveGrowCreature11makeRequestEjRN30SIG...）。
    DB_EventUpdateGiveGrowCreature() {}            // ORIG C1 0x844fe16 (sz 0x4: base ctor+vptr only)
    static bool makeRequest(unsigned int accId,
                            SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo& info);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x08449d00
};
class DB_UpdateSeriaRoomAnimationDeco : public DBDispatcher
{
public:
    // _ZN31DB_UpdateSeriaRoomAnimationDeco11makeRequestEjsiss T 0x0844af74
    DB_UpdateSeriaRoomAnimationDeco() {}           // ORIG C1 0x844febe (sz 0x4: base ctor+vptr only)
    static void makeRequest(unsigned int accId, short serverGroup, int characNo,
                            short field1, short field2);
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG T 0x0844adc0（W8LT_EventTail.cpp）
};

// ============================ OneADayItemShopEvent ============================
// repeat 事件 0x6d（GameWorld::ResetOneADayItemShop 经 GetRepeatEvent(0x6d) 取得）。
class OneADayItemShopEvent : public CEventBase
{
public:
    OneADayItemShopEvent();                       // C1/C2 0x081b9fb4
    ~OneADayItemShopEvent();                      // D1/D2 0x081b9fd0（D0 编译器生成）

    virtual void StartEvent();                    // 0x081ba01e
    virtual void EndEvent();                      // 0x081ba0ae
    virtual void StartEvent(Word_Param param);    // 0x081ba03a
    virtual void StartAction();                   // 0x081ba100
    virtual void deliveryStart(CDelivery* d);     // 0x081ba156
    virtual void deliveryEnd(CDelivery* d);       // 0x081ba19c
    virtual void delivery(DeliveryMsg& msg);      // 0x081ba116

    bool isQuestId(int questId);                  // 0x081ba1e2

private:
    void questClearLog(DeliveryMsg& msg);         // 0x081ba21e
    void buyItemLog(DeliveryMsg& msg);            // 0x081ba5e8
};

// ============================ GiveGrowCreatureEvent ============================
// 新号成长生物赠送事件（CharacterCreateCreatureGiftScript 数据域）。
class GiveGrowCreatureEvent : public CEventBase
{
public:
    GiveGrowCreatureEvent();                       // C1/C2 0x081b695c
    ~GiveGrowCreatureEvent();                      // D1/D2 0x081b6978（D0 编译器生成）

    virtual void StartEvent();                     // 0x081b69c6
    virtual void EndEvent();                       // 0x081b6a6a
    virtual void StartEvent(Word_Param param);     // 0x081b6a18
    virtual void StartAction();                    // 0x081b6abc
    virtual void changeCharacName(CUser* user, int key, char* old_, int new_);  // 0x081b717a
    virtual bool IsEventing(CUser* user) const;    // 0x081b739e（透传基类）

    void SendGrowthCreatureEventInfo(CUser& user); // 0x081b6b70
    int processEvent(CUser& user);                 // 0x081b6f76
    int CheckEventInfo(CUser& user);               // 0x081b6e7a
    bool isGrowCreatureMaxLevel(CUser& user);      // 0x081b7156
};

// ============================ SeriaRoomAniDecoEvent ============================
// 塞丽亚房间装饰摆件 repeat 事件 0x69（Dispatchers.cpp processEvent 待接线注释处）。
class SeriaRoomAniDecoEvent : public CEventBase
{
public:
    SeriaRoomAniDecoEvent();                       // C1/C2 0x0827b490
    ~SeriaRoomAniDecoEvent();                      // D1/D2 0x0827b4ac（D0 编译器生成）

    virtual void StartEvent();                     // 0x0827b4fa
    virtual void EndEvent();                       // 0x0827b59e
    virtual void StartEvent(Word_Param param);     // 0x0827b54c
    virtual void StartAction();                    // 0x0827b6a6
    virtual void dailyresetData();                 // 0x0827b5f0
    virtual bool IsEventing(CUser* user) const;    // 0x0827bc4c（透传基类）

    void SendAniDecoItem(CUser& user);             // 0x0827b75a
    void SendEventInfo(CUser& user);               // 0x0827b99a
    int processEvent(CUser& user);                 // 0x0827ba52
    int CheckEventInfo(CUser& user);               // 0x0827ba48
};

#endif  // GAME_W8LT_EVENTTAIL_H_
