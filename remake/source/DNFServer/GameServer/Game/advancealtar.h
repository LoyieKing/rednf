// advancealtar 基础类型声明。
#pragma once

#include <vector>
#include <map>
#include <cstring>

#include "TimerQueue.h"  // TIMER_MESSAGE（StageControl::onTimer 参数；权威枚举，随 TimerDispatcher 批次增长）
#include "advancealtar_StageDifficulty.h"  // StageDifficulty::T（Manager::selectStage 参数，ORIG mangled NS_15StageDifficulty1TE）

#include "AdvanceAltarField.h"  // _RewardItem/AchievementRewardData（sendAchievementReward 签名需要；幂等守卫叶子头）
#include "DBDispatcher.h"  // 全局 ::DBDispatcher（DB_Update/DB_Load 等基类；dispatch bool @0x83fbf7c）
// ORIG 枚举 advancealtar::AdvanceAltarShopType::T（mangled NS_20AdvanceAltarShopType1TE，
// BuyItem dispatch req+0x14 传参）；枚举器名不可从二进制恢复【推断】，
// 取值经 getBuyUpgradeDataVec(key, T) 按 0/1/2 分派。
#ifndef DNF_GAME_ADVANCE_ALTAR_SHOP_TYPE_DEFINED
#define DNF_GAME_ADVANCE_ALTAR_SHOP_TYPE_DEFINED
struct AdvanceAltarShopType
{
    enum T
    {
        SHOP_N0 = 0,
        T_1 = 1,   //【推断】ORIG 枚举器名不可恢复；值 1/2 由 DB_Load::getTowerInfo
        T_2 = 2    // 调用（advancealtar_DB.cpp 原批）与商店分表逻辑实证
    };
};
#endif

// ORIG 全局枚举（mangled 10ENUM_ERROR；权威定义 CUser.h/CMailBoxHelper.h，
// 同守卫幂等复制——本头不可反向 include CUser.h（重入环））。
#ifndef DNF_GAME_ENUM_ERROR_DEFINED
#define DNF_GAME_ENUM_ERROR_DEFINED
enum ENUM_ERROR { ENUM_ERROR_NONE = 0 };
#endif

class MSG_CHARAC_SELECT;   // 全局作用域（PacketTypes.h 权威定义；仅引用）
class PacketGuard;   // 全局作用域权威 PacketGuard.h（putRidableInfoToPacket 仅引用）
// 前置声明全局 CUser（实现 TU include CUser.h 获得完整类型）
class CUser;
struct MSG_MONSTER_DIE;    // 完整定义 Secu_HackLogCheckByParty.h（advancealtar::Manager::dieMob 引用；全局作用域权威）

// CMDPacketStruct altar 请求域类型前置声明（注意：必须在 advancealtar
// namespace 外部，否则会成为 advancealtar::CMDPacketStruct 遮蔽全局权威名，
// 导致 Dispatchers.h 等处 CMDPacketStruct::STBaseRequest 查找失败。
// 完整定义归口 CMDPacketStruct.h——OnlyOrig_Exchange 批，2026-08-27 修正）。
namespace CMDPacketStruct { struct _SetSlotData; }
#ifndef DNF_GAME_CMDPACKET_RESETSTARTYPE_DEFINED
#define DNF_GAME_CMDPACKET_RESETSTARTYPE_DEFINED
namespace CMDPacketStruct { struct ResetStarType { enum T { RS_N0 = 0 }; }; }
#endif
namespace advancealtar
{

// CharacAdvanceAltarManager 权威定义在 CUser.h（唯一归口；本头前置声明，
// 供仅包含 advancealtar.h 的 TU 引用该类型）。
class CharacData;   // 完整定义 advancealtar_ProcStage.h（StageControl::getCharacData 返回类型）
class ProcStage;    // 完整定义 advancealtar_ProcStage.h（StageControl::getProcStage 返回类型）
class CharacAdvanceAltarManager;
class ActionDefine;  // 完整定义 CDataManager.h（_ActionAssign 仅持指针）

// ---- 子结构布局（advancealtar__DB_Load parse* 报告推导，尺寸与 ORIG 一致）----
struct _Stage {
    // ORIG W C1/C2 @0x08134452（0x23 字节）：memset(this,0,sizeof) —— 8 字节全清零。
    _Stage() { std::memset(this, 0, sizeof(*this)); }
    int m_field0; int m_field4;
};            // 8 字节
#pragma pack(push, 1)
struct _Slot {
    // ORIG W C1/C2 @0x0813449a：memset(this,0,10)。
    _Slot() { std::memset(this, 0, sizeof(*this)); }
    unsigned char m_byte0; int m_int1; unsigned char m_byte5; int m_int6;   // pack(1)，10 字节
};  // 10 字节
struct _Reward {
    // ORIG W C1/C2 @0x081344be：memset(this,0,5)。
    _Reward() { std::memset(this, 0, sizeof(*this)); }
    int m_field0; unsigned char m_byte4;                                    // pack(1)，5 字节
};  // 5 字节
#pragma pack(pop)
struct _BuyItem {
    // ORIG W C1/C2 @0x08134476：memset(this,0,12)；另有 trivial D1/D2 @0x813b4cc（W，随用点发射）。
    _BuyItem() { std::memset(this, 0, sizeof(*this)); }
    ~_BuyItem() {}   // ORIG W D1/D2 @0x0813b4cc（空体；随使用点发射弱符号）
    int m_field0; int m_field4; short m_short8; short m_short10;            // 12 字节
};  // 12 字节

// _RidableBase（AdvanceAltarData 基类，+0x00；仅前 0x21 序列化，ORIG ctor 置 0）
struct _RidableBase
{
    _RidableBase() { std::memset(this, 0, 0x21); }
    int m_ridableId;         // +0x00（refillTicketFree 0x813159a 判定槽位坐骑）
    int m_ticket;            // +0x04（付费票数；refillTicketFree/gmSetTicket 写入）
    // ---- +0x08..+0x23 计数区（OnlyOrig_Altar.AltarMgr 按
    //      decTicket/gmSetStar/buyItem/clearStage/resetStar 报告写点具名化；
    //      字段语义名【推断】，偏移与总尺寸 0x24 不变）----
    int m_freeTicket;        // +0x08（decTicket 第二票源：m_ticket<1 时扣减）
    int m_curStar;           // +0x0c（gmSetStar 主写点；=当前星）
    int m_starPoolBase;      // +0x10（gmSetStar：m_totalStar = 本值 + v）
    int m_starPoint;         // +0x14（buyItem 余额比较/扣减、gmSetStar 写入；
                             //   clearStage 加 kRewardStar）
    short m_maxClearRound;   // +0x18（clearStage：*(short*)(cur+0x18) 上限更新）
    short m_resetCount;      // +0x1a（resetStar 递增；费用表 key）
    short m_field1c;         // +0x1c（无报告写点）
    short m_field1e;         // +0x1e（无报告写点）
    unsigned char m_field20; // +0x20（setNextStage 0x081331d4 推进新幕时置 1；语义【推断】）
    char m_pad21[3];         // +0x21..+0x23（ctor memset 覆盖；8B 保总尺寸 0x24）

};

// ORIG 枚举 advancealtar::RidableId::T（mangled NS_9RidableId1TE）。报告仅实证
// 值 1 为有效坐骑：Dispatcher_StartGame/send 以常量 1 调用
// setCurrentAdvanceAltarData(RidableId::T)；isValidRidableId 仅判 ==1。
// 枚举器名为重建命名「推断」（ORIG 枚举器名不可从二进制恢复）。
struct RidableId
{
    enum T
    {
        RIDEABLE = 1,
    };
};

// ORIG T 0x8898ff0（_ZN12advancealtar16isValidRidableIdENS_9RidableId1TE）：
// ridableId == RidableId::RIDEABLE。定义在 LinkGlobalsB2.cpp（唯一定义点）。
bool isValidRidableId(RidableId::T ridableId);
struct _RewardItem;   // 完整定义 AdvanceAltarField.h（sendAchievementReward 仅指针形参引用）
struct _SlotItem;      // 完整定义 advancealtar_ProcStage.h（getSlotItem 形参 map 值类型；前置免循环 include）
// ORIG 枚举 advancealtar::AdvanceAltarSlotType::T（mangled
// NS_20AdvanceAltarSlotType1TE）。getSlotMap 仅认 1/2（0 与其它→0）；
// 枚举器名【推断】，与 AdvanceAltarField.h 同守卫幂等副本。
#ifndef DNF_GAME_ADVANCE_ALTAR_SLOT_TYPE_DEFINED
#define DNF_GAME_ADVANCE_ALTAR_SLOT_TYPE_DEFINED
struct AdvanceAltarSlotType { enum T { SLOT_N1 = 1, SLOT_N2 = 2 }; };
#endif
// ---- ORIG 参数类型 advancealtar::AdvanceAltarShopType::T（NS_20...）在本
// 命名空间内的守卫幂等副本：ns 作用域内成员签名必须解析到 ns 版本，mangled
// 才与 ORIG 一致；全局版副本仅供域外既有引用。
#ifndef DNF_GAME_ADVANCE_ALTAR_SHOP_TYPE_NS_DEFINED
#define DNF_GAME_ADVANCE_ALTAR_SHOP_TYPE_NS_DEFINED
struct AdvanceAltarShopType { enum T { T_0 = 0, T_1 = 1, T_2 = 2 }; };
#endif

// _AdvanceAltarDbData：0x518（get() 序列化布局，pack(1) 匹配 ORIG 扁平 buffer）
#pragma pack(push, 1)
struct _AdvanceAltarDbData
{
    // ORIG W C1/C2 @0x0848f488（0xd7）：_RidableBase ctor + 50×_Stage + 12×_Slot +
    // 50×_BuyItem + 29×_Reward 各子对象 memset ctor 后整体 memset(this,0,0x518)
    // —— 全 POD，等价于单次全清零。
    _AdvanceAltarDbData() { std::memset(this, 0, sizeof(*this)); }
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
    // ORIG W C1/C2 @0x848f560（0x54）：1 次 _AdvanceAltarDbData ctor（+0x04）后
    // 整体 memset(this,0,0x51c) —— 等价于单次全清零。
    _CharacAdvanceAltarDbData() { std::memset(this, 0, sizeof(*this)); }
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
    std::map<int,_BuyItem>* getBuyItemMap(AdvanceAltarShopType::T type);   // ORIG T 0x088999d8（无 K 后缀——勿加 const）
    std::map<unsigned char,_Slot>* getSlotMap(AdvanceAltarSlotType::T type);  // ORIG T 0x08899a0c（无 K）
    void set(const _AdvanceAltarDbData& db);                  // ORIG T 0x088992da（get 的逆序列化）
    std::map<int,_Stage> m_stages;           // +0x24
    std::map<int,_BuyItem> m_buyItems1;      // +0x3c
    std::map<int,_BuyItem> m_buyItems2;      // +0x54
    std::map<int,_BuyItem> m_buyItems3;      // +0x6c
    std::map<unsigned char,_Slot> m_slots1;  // +0x84
    std::map<unsigned char,_Slot> m_slots2;  // +0x9c
    std::map<int,_Reward> m_rewards;         // +0xb4
    unsigned char m_update;                  // +0xcc
};                                           // sizeof 0xd0

class DB_Update : public ::DBDispatcher
{
public:
    static void makeRequest(int uid, const _CharacAdvanceAltarDbData& data);  // ORIG T 0x0813eb38
    bool dispatch(int uid, int uniqueId, Stream* stream);   // ORIG T 0x813ec24（vtable slot0 覆写）

    // ---- 2026-08-27 AltarDB 批追加（ORIG T 符号地址见各报告）----
    bool update(const _CharacAdvanceAltarDbData& data);                        // ORIG T 0x0813ec5e
    void makeStageString(std::string& out, const _Stage* arr, int count);      // ORIG T 0x813f380
    void makeSlotString(std::string& out, const _Slot* arr, int count);        // ORIG T 0x813f442
    void makeRewardString(std::string& out, const _Reward* arr, int count);    // ORIG T 0x813f66a
    void makeBuyItemString(std::string& out, const _BuyItem* arr, int count);  // ORIG T 0x813f53c
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

// ---- EplpType：ORIG mangled NS_8EplpType1TE。Manager::eplp 0x0813101e 判
// param_2<3 合法，取值域 {0,1,2} 实证；枚举器名【推断】——二进制不可恢复。
#ifndef DNF_GAME_ADVANCEALTAR_EPLPTYPE_DEFINED
#define DNF_GAME_ADVANCEALTAR_EPLPTYPE_DEFINED
struct EplpType
{
    enum T
    {
        EPLP_N0 = 0,
        EPLP_N1 = 1,
        EPLP_N2 = 2
    };
};
#endif

class Manager
{
public:
    Manager();    // ORIG T/W 0x08130902：空体（定义 advancealtar_Manager.cpp）
    ~Manager();   // ORIG T/W 0x08130908：空体（同上）
    static int selectStage(CUser* user, int stageId, StageDifficulty::T diff);  // ORIG T 0x0813090e
    static void procErrorSelectStage(CUser* user, ENUM_ERROR err);              // ORIG T 0x081309f0
    static int finishLoad(CUser* user);                                         // ORIG T 0x08130eb2（DisP_GroupA 调用）
    static void dieCharac(CUser* user);                                         // ORIG T 0x08130ab2（DisP_GroupG 调用）
    static void giveUpGame(CUser* user);                                        // ORIG T 0x08130fa8（DisP_GroupG 调用）

    static int gmTimeSpeed_;   // ORIG _ZN12advancealtar7Manager12gmTimeSpeed_E
                               // （incCurrentMs 0x812e146 读用；全局 ctor 初始化 0）
    static void eplp(CUser* user, EplpType::T type);   // ORIG T 0x0813101e：user/control 非空且 type<3 → StageControl::leaveUser()
    static int dieMob(CUser* user, unsigned short mobSeq, unsigned short param,
                      const MSG_MONSTER_DIE& msg);     // ORIG T 0x08130bb8（undefined4 恒返 1【推断 int】）
    static void leaveUser(CUser* user);                // ORIG T 0x081310b2：out_from_dungeon + 0x2 make_basic_info 包广播
    static void pause(CUser* user, bool flag);  // ORIG T 0x0813105c：Dispatcher_Pause::process_packet 调用（定义归 advancealtar_Manager.cpp [跨片协调]）
};

// ORIG T 0x0812cfa6 _ZN12advancealtar9isOnEventEv：s_event_manager->GetRepeatEvent(0x77)
// → 虚槽 +0x34（CEventBase::IsEventing(0)）==1。sendAchievementReward 消费。
bool isOnEvent();


// ---- CharacAdvanceAltarManager（自 CUser.h 迁入唯一归口 2026-08-27 Wave5，
//      CUser.h 同段删除改由本头承载；sizeof 0xd4 断言随迁）----
class CharacAdvanceAltarManager
{
public:
    CharacAdvanceAltarManager();
    ~CharacAdvanceAltarManager();

    void getCharacAdvanceAltarDbData(_CharacAdvanceAltarDbData& out) const;  // ORIG T 0x08131710
    bool isUpdated() const;                                                  // ORIG T 0x081316a0

    bool isAvailableTciket() const;        // ORIG T 0x081312ec：current 非空且 m_ticket 区和>0
    bool isEnterStage(int stageId) const;  // ORIG T 0x0813131a：current->m_stages 命中
    int getCurrentRidableId() const;       // ORIG T 0x081312d0：current 非空 ? ridableId : 0

    void refillTicketFree();  // ORIG T 0x0813159a：免费票补满（AddDailyItem 调用）
    void reset();             // ORIG T 0x08131660：m_field0=0 + m_data.reset()
    // ORIG T 0x081328ea：current 空/ m_buyItems3 无 towerIdx / 商店表无该 id /
    // 等级字段缺失 / 计数为 0 任一 → false；否则 outValue=FieldData::m_value、
    // outCount=买项计数，返回 true。
    bool getTowerInfo(int towerIdx, int& outValue, int& outCount) const;
    // ---- Wave5 ProcStage 清关链依赖（声明追加 2026-08-27 AltarProc；定义归
    //      CharacAdvanceAltarManager 批次 [跨片协调]）----
    bool clearStage(int stageIdx, StageDifficulty::T diff, StageType::T type,
                    int clearRound, int& starDelta, bool& unlockBuy,
                    bool& bOut);                        // ORIG T 0x0813175a
    void setNextStage(int stageIdx);                    // ORIG T 0x081331d4
    char decTicket();                                   // ORIG T 0x081313b6：返回失败标志（0=成功扣票）
    void setUnlockBuyItem(AdvanceAltarShopType::T shopType, int stageIdx,
                          std::vector<_BuyItem>* out);  // ORIG T 0x08132d20
    void getClearStageSummary(int out[3]) const;        // ORIG T 0x081318e2
    AdvanceAltarData* getCurrentAdvanceAltarData() const;  // ORIG W 0x081348c4
    int addRewardData(int rewardId, std::map<int, _Reward>& out);  // ORIG T 0x08132abe


    // ---- Wave5 altar dispatcher 链依赖（声明由 AltarDisp 代持，定义归
    //      advancealtar_Manager.cpp [跨片协调]）----
    // ORIG T 0x081319ae（Dispatcher_BuyItem process_packet 调用）
    int buyItem(AdvanceAltarShopType::T shopType, int level,
                short fromSlot, short toSlot, int totalCost, int& outReward);
    // ORIG T 0x08131b58（Dispatcher_SetSlot process_packet 调用）
    int setSlot(bool isSet, const CMDPacketStruct::_SetSlotData& data);
    // ORIG T 0x08131f0a（Dispatcher_ExchangeSlot process_packet 调用）
    int exchangeSlot(CMDPacketStruct::_SetSlotData& slotA, CMDPacketStruct::_SetSlotData& slotB);
    // ORIG T 0x081333b2（Dispatcher_ResetStar process_packet 调用）
    int resetStar(CUser* user, CMDPacketStruct::ResetStarType::T type);
    // ---- OnlyOrig_Altar.AltarMgr 批（定义 advancealtar_Manager.cpp）----
    void gmSetTicket(int ticket);   // ORIG T 0x0813375c：cur->m_ticket=v、m_update=1
    void setDefaultBuyItem(AdvanceAltarShopType::T type);  // ORIG T 0x08132cb8
    bool setCurrentAdvanceAltarData(RidableId::T ridableId);  // ORIG T 0x08131254
    void setDefaultStage();                         // ORIG T 0x08132f72
    void gmSetStar(int star);                              // ORIG T 0x08133720
    int resetSlot(advancealtar::AdvanceAltarData& data,
                  const CMDPacketStruct::_SetSlotData& slot);   // ORIG T 0x08132bfa
    void setUnlockItemToEmptySlot(AdvanceAltarShopType::T shopType,
                                  int itemIdx);                 // ORIG T 0x08132234
    int sendAchievementReward(CUser* user, int achievementId,
                              const std::vector<_RewardItem>** outItems);  // ORIG T 0x0813234e
    void setDefaultSlot();                                 // ORIG W 0x08132f8e
    void checkAndCreateDefaultRidable();                   // ORIG T 0x08131426
    // ---- OnlyOrig_Altar.AltarMgr 终态收口批 ----
    AdvanceAltarData* getAdvanceAltarDataByIndex(int idx);          // ORIG T 0x081316e8（nm 权威；任务文本 0x8133194 为笔误）
    void set(const _CharacAdvanceAltarDbData& db);                  // ORIG T 0x08131616：分派到 AdvanceAltarData::set
    void getSlotItem(std::map<unsigned char, _SlotItem>& out) const;   // ORIG T 0x08132638
    int m_field0;              // +0x00
    AdvanceAltarData m_data;   // +0x04（0xd0）
};                             // sizeof 0xd4
typedef char _assert_charac_manager_size[(sizeof(CharacAdvanceAltarManager) == 0xd4) ? 1 : -1];

// 布局断言（ORIG 尺寸：AdvanceAltarData 0xd0 / _AdvanceAltarDbData 0x518 /
// _CharacAdvanceAltarDbData 0x51c / 子结构 8/10/12/5；Manager 0xd4 断言在 CUser.h）
typedef char _assert_advancealtar_data_size[(sizeof(AdvanceAltarData) == 0xd0) ? 1 : -1];
typedef char _assert_advdb_size[(sizeof(_AdvanceAltarDbData) == 0x518) ? 1 : -1];
typedef char _assert_characadvdb_size[(sizeof(_CharacAdvanceAltarDbData) == 0x51c) ? 1 : -1];
typedef char _assert_stage_size[(sizeof(_Stage) == 8) ? 1 : -1];
typedef char _assert_slot_size[(sizeof(_Slot) == 10) ? 1 : -1];
typedef char _assert_buyitem_size[(sizeof(_BuyItem) == 12) ? 1 : -1];
typedef char _assert_reward_size[(sizeof(_Reward) == 5) ? 1 : -1];
struct _ActionAssign
{
    _ActionAssign() { std::memset(this, 0, sizeof(*this)); }  // ORIG W C1/C2 @0x0889b99c：memset 0x10
    int m_field0;            // +0x00（ctor 清零；语义未证；checkAndSetActionDefine 以其作 ActionDefine id）
    ActionDefine* m_define;  // +0x04（makeCurrentPhaseSummonObject 取 m_id/m_actions）
    short m_minAliveCount;   // +0x08（-1 = 不限下限）
    short m_maxAliveCount;   // +0x0a（-1 = 不限上限）
    int m_fieldC;            // +0x0c（ctor 清零；用途未证）
};                           // sizeof 0x10（ORIG ctor 0x889b99c memset 0x10）

typedef char _assert_action_assign_size[(sizeof(_ActionAssign) == 0x10) ? 1 : -1];

struct Phase
{
    Phase();
    void reset();

    int m_field0;                           // +0x00
    int m_field4;                           // +0x04
    std::vector<_ActionAssign> m_assigns;   // +0x08
};                                          // sizeof 0x14

struct _PhaseSummary
{
    // ORIG W C1/C2 @0x0889b954（memset 0xc）/ D1/D2 @0x0889ebc0（空体）。
    // 用户 ctor/dtor 使使用点发射对应弱符号。
    _PhaseSummary() { std::memset(this, 0, sizeof(*this)); }
    ~_PhaseSummary() {}
    char m_pad[0xc];
};  // sizeof 0xc

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

    // ORIG 各函数以 TimeLine+0x00/+0x0c 偏移直取（makeCurrentPhaseSummonObject
    // /sendMapInfo 等），重建按 public 成员暴露。
    std::vector<Phase> m_phases;            // +0x00
    TimeLineSummary m_summary;              // +0x0c
};                                          // sizeof 0x1c

// ---- ORIG 枚举 advancealtar::StageEndType::T（mangled NS_12StageEndType1TE）----
// 权威定义自 advancealtar_ProcStage.h 迁入（该头 include 本头，副本已同守卫幂等化；
// 枚举器名 END_N1/END_N2 为重建命名「推断」，值 1/2 由 dieCharac/giveUpGame 报告实证）。
#ifndef DNF_GAME_ADVANCE_ALTAR_STAGE_END_TYPE_DEFINED
#define DNF_GAME_ADVANCE_ALTAR_STAGE_END_TYPE_DEFINED
struct StageEndType
{
    enum T
    {
        END_N0 = 0,     // isEndStage 全清死亡标记 [名推断，AltarProc 0x0812e2c6 实证值]
        END_N1 = 1,
        END_N2 = 2,
        END_N3 = 3      // isEndStage 全阶段完成 [名推断，同报告实证值]
    };
};
#endif

// ---- StageControl（docs/class_func_reports/advancealtar__StageControl/ 全 16 函数）----
// ORIG 布局：index@+0x00（getIndex/setIndex thunk 实证）、ProcStage@+0x04
// （getProcStage W 0x081348ae）、CharacData@+0x98（getCharacData W 0x081348a0），
// 总尺寸 0xc0（原 CGameManager.h m_pad[0xc0] 占位实测；本头为唯一声明归口）。
// 子对象以字节槽占位：ProcStage sizeof==0x94 断言在 advancealtar_ProcStage.h；
// CharacData 含未建模 map<unsigned char,_SlotItem> 区，无完整尺寸可按值嵌入。
class StageControl
{
public:
    StageControl();                              // ORIG T 0x0812f9a2
    ~StageControl();                             // ORIG T 0x0812fa06

    int getIndex() const;                        // ORIG W 0x081348ba：return m_index
    void setIndex(int idx);                      // ORIG W 0x82a669c：m_index = idx
    ProcStage* getProcStage();                   // ORIG W 0x081348ae：(ProcStage*)(this+0x04)
    CharacData* getCharacData();                 // ORIG W 0x081348a0：(CharacData*)(this+0x98)

    void reset();                                // ORIG T 0x0812fa54
    void destroy();                              // ORIG T 0x081300ce：reset() + returnAdvanceAltar(this)
    int selectStage(CUser* user, int stageId, StageDifficulty::T diff);  // ORIG T 0x0812fa7a
    void sendStageInfo();                        // ORIG T 0x081300f6：0x209 舞台信息包
    char leaveUser();                            // ORIG T 0x0812fc4e
    char onTimerStageTick();                     // ORIG T 0x0812fb98（恒返回 1）
    void gotoStage(CUser* user);                 // ORIG T 0x0812fdd4（静态语义，asm 不用 this）
    void onTimer(TIMER_MESSAGE msg, StageEndType::T endType);            // ORIG T 0x0812fbfa
    void onClearReward(StageEndType::T endType); // ORIG T 0x081305f6
    void onEplp(StageEndType::T endType);        // ORIG T 0x08130644
    void onEplpReturnVillage();                  // ORIG T 0x08130692
    void sendEplp(StageEndType::T endType);      // ORIG T 0x081306be：0x20e EPLP 状态包
    int getCenterCount() const;                  // ORIG T 0x081307a6
    void sendMapInfo();                          // ORIG T 0x081301e0：0x20a 地图信息包

private:
    int m_index;                         // +0x00
    unsigned char m_procStage[0x94];     // +0x04 ProcStage 槽（sizeof(ProcStage)==0x94）
    unsigned char m_characData[0x28];    // +0x98 CharacData 槽（0xc0-0x98）
};                                       // sizeof 0xc0

// ---- NOTIPacketStruct::_NotiAdvanceAltar*（advancealtar 专属通知包体；
// 自 shared/packet/include/NOTIPacketStruct.h 死草稿吸收（Main 裁决 2026-08-25），
// 布局/ctor/memset 语义与 static_assert 原样保留。_SummonObject 引用改字节槽
// （权威在 CDataManager.h，本头经其 +201 行 include 反向不可达）；_Stage 用本头
// 权威定义。）----
struct NOTIPacketStruct
{
    // header 0x20b, payload 0x28
    struct _NotiAdvanceAltarSpawnUnit
    {
        unsigned short m_spawnIndex;      // +0x00（u16，= _SummonObjectMs+4 序号）
        unsigned char m_pad[2];           // +0x02
        unsigned char m_summon[0x24];     // +0x04 = _SummonObject（CDataManager.h 权威，
                                          //   ORIG 为子对象 ctor 后 memcpy 整体）
        _NotiAdvanceAltarSpawnUnit() { std::memset(this, 0, 0x28); }
    } __attribute__((packed));
    typedef char _assert_spawnunit_size[(sizeof(_NotiAdvanceAltarSpawnUnit) == 0x28) ? 1 : -1];

    // header 0x209, payload 8
    struct _NotiAdvanceAltarStageInfo
    {
        int m_stageIndex;       // +0x00 ProcStage::getStageIndex()
        int m_stageDifficulty;  // +0x04 ProcStage::getStageDifficulty()
        _NotiAdvanceAltarStageInfo() { std::memset(this, 0, 8); }
    } __attribute__((packed));
    typedef char _assert_stageinfo_size[(sizeof(_NotiAdvanceAltarStageInfo) == 8) ? 1 : -1];

    // header 0x20a, payload 0x14
    struct _NotiAdvanceAltarMapInfo
    {
        int m_stageType;               // +0x00 ProcStage::getStageType()
        int m_currentSurvivalRoundNo;  // +0x04 getCurrentSurvivalRoundNo()
        int m_mapIndex;                // +0x08 CMap::get_index()
        int m_currentPhaseIndex;       // +0x0c TimeLineSummary 首字段（TimeLine+0xc）
        int m_mapBasisLevel;           // +0x10 getMapBasisLevel()
        _NotiAdvanceAltarMapInfo() { std::memset(this, 0, 0x14); }
    } __attribute__((packed));
    typedef char _assert_mapinfo_size[(sizeof(_NotiAdvanceAltarMapInfo) == 0x14) ? 1 : -1];

    // header 0x20f, payload 0x1c
    // 注意：_Stage 现为带 ctor 的非 POD，packed 内嵌会被 GCC 忽略 packed 警告并
    // 引发填充 → 此处用 8B 字节槽保持 ORIG 扁平布局（_SummonObject 同先例）。
    struct _NotiAdvanceAltarClearStage
    {
        int m_stageType;              // +0x00
        int m_clearSurvivalRoundNo;   // +0x04
        int m_ridableId;              // +0x08（u16 符号扩展）
        char m_clearFlag;             // +0x0c
        unsigned char m_stage[8];     // +0x0d = _Stage（8B {stageIndex, stageDifficulty}）
        unsigned char m_pad[3];       // +0x15
        int m_starCount;              // +0x18
        _NotiAdvanceAltarClearStage() { std::memset(this, 0, 0x1c); }
    } __attribute__((packed));
    typedef char _assert_clearstage_size[(sizeof(_NotiAdvanceAltarClearStage) == 0x1c) ? 1 : -1];

    // header 0x20e, payload 8
    struct _NotiAdvanceAltarEplpState
    {
        int m_eplpType;                // +0x00 StageEndType::T
        char m_isAvailableTicket;      // +0x04 isAvailableTciket()
        unsigned char m_pad[3];        // +0x05
        _NotiAdvanceAltarEplpState() { std::memset(this, 0, 8); }
    } __attribute__((packed));
    typedef char _assert_eplpstate_size[(sizeof(_NotiAdvanceAltarEplpState) == 8) ? 1 : -1];

    // header 0x210, payload 1（标记包；发送方从不写字节）
    struct _NotiAdvanceAltarUpdateRidable
    {
        unsigned char m_flag;          // +0x00 恒 0
        _NotiAdvanceAltarUpdateRidable() { std::memset(this, 0, 1); }
    } __attribute__((packed));
    typedef char _assert_updateridable_size[(sizeof(_NotiAdvanceAltarUpdateRidable) == 1) ? 1 : -1];
};  // struct NOTIPacketStruct

// ---- HistoryLog（advancealtar__HistoryLog/README.md；定义归 advancealtar_ProcStage.cpp
//      [跨片协调 AltarProc]）----
// ORIG 枚举 advancealtar::SubStarType::T（NS_11SubStarType1TE）；枚举器名不可恢复【推断】。
struct SubStarType { enum T { SUB_N0 = 0 }; };

// ORIG 枚举 advancealtar::AddStarType::T（NS_11AddStarType1TE）；值证据：
// clearStageAndReward 0x0812e659 传 0、resetStar 0x08133713 传 2；枚举器名【推断】。
struct AddStarType { enum T { ADD_BY_CLEAR = 0, ADD_UNKNOWN1 = 1, ADD_BY_RESET = 2 }; };

class AdvanceAltarData;
class HistoryLog
{
public:
    // ORIG T 0x08133db0：asm 不使用 this，按静态成员声明（mangled 一致）。
    // starSub(user, type, amount) → CUser::LogHistory("star-,%d,%d", type, amount)。
    static void starSub(CUser& user, SubStarType::T type, int amount);
    // ---- AltarProc 批次并入（原 advancealtar_ProcStage.h HistoryLog 版；报告全文）----
    static void dungeonClearInfo(CUser& user, StageControl& control, bool isLast);   // T 0x08133bd4
    static void enterDungeon(CUser& user, const ProcStage& procStage);               // T 0x081339b0
    static void killMonster(CUser& user, int monsterId, const MSG_MONSTER_DIE& msg); // T 0x08133b40
    // ---- AltarProc Wave5 追加（0x08133abc..0x08133dda 报告全文）----
    static void leaveDungeon(CUser& user, const ProcStage& procStage);   // T 0x08133abc
    static void updateStatistics();                                      // T 0x08133e30
    static void buyItem(AdvanceAltarShopType::T shopType, int a,         // T 0x08133dda
                        short b, short c, int d);
    static void starAdd(CUser& user, AddStarType::T type, int stageIndex,// T 0x08133d56
                        int count);
};

// ORIG T 0x08141781（Dispatcher_ResetStar process_packet 调用）；定义归骑乘包批次。
void putRidableInfoToPacket(PacketGuard& guard, const AdvanceAltarData& data);

// ORIG T 0x0812cfea：isOpenAdvanceAltar()——CDailyScheduleManager(4,10)==1；
// 定义归 advancealtar_misc.cpp [跨片协调 AltarMisc]。5 个 altar dispatcher
// check_error 调用。
    static void leaveDungeon(CUser& user, const ProcStage& procStage);   // T 0x08133abc
    static void updateStatistics();                                      // T 0x08133e30
    static void buyItem(AdvanceAltarShopType::T shopType, int a,         // T 0x08133dda
                        short b, short c, int d);
    static void starAdd(CUser& user, AddStarType::T type, int stageIndex,// T 0x08133d56
                        int count);
bool isOpenAdvanceAltar();

}  // namespace advancealtar

