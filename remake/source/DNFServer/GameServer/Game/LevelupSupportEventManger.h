// df_game_r Game/ LevelupSupport 升级支持活动域权威声明。
//
// 覆盖类（全部 ORIG df_game_r elf32-i386 nm/objdump 实证，地址见各项注释）：
//   LevelupSupportSciprt          —— Event/tw_LevelupSupport.evt 解析器
//   LevelupSupport2ndScript       —— Event/tw_LevelupSupport2nd.evt 解析器
//                                    （[job type]/[mail title]/[mail content] 节）
//   LevelupSupportEventManger     —— ARAD 脚本服务 #6（ScriptManager::Init 注册序）
//   LevelupSupport2ndEventManger  —— ARAD 脚本服务 #9
//
// 关键 ORIG 符号（nm 实证）：
//   _ZN20LevelupSupportSciprtC1Ev              T 0x08ab5124   D1 T 0x08ab5138
//   _ZN20LevelupSupportSciprt10loadScriptEv    T 0x08ab514c   11clearScript T 0x08ab5442
//   _ZN20LevelupSupportSciprt8ItemInfoC1Eii    W 0x08ab5a44
//   _ZN23LevelupSupport2ndScriptC1Ev           T 0x08ab54be   D1 T 0x08ab5530
//   _ZN23LevelupSupport2ndScript10loadScriptEv T 0x08ab55a4   11clearScript T 0x08ab5964
//   _ZN23LevelupSupport2ndScript8ItemInfoC1Eii W 0x08ab5a5e
//   _ZN25LevelupSupportEventMangerC1Ev         T 0x08146d30   D1 T 0x08146dc0
//   _ZN25LevelupSupportEventManger4LoadEv      T 0x08146e3c   7Destroy T 0x08146fda
//   _ZN25LevelupSupportEventManger9EventInfoC1Ett W 0x08147b2a（双 ushort 参）
//   _ZTV25LevelupSupportEventManger V 0x08b73c80（仅两虚槽：槽0=Load、槽1=Destroy）
//   _ZN28LevelupSupport2ndEventMangerC1Ev      T 0x0814778e   D1 T 0x081477c4
//   _ZN28LevelupSupport2ndEventManger4LoadEv   T 0x081477e4   7Destroy T 0x08147864
//   _ZTV28LevelupSupport2ndEventManger V 0x08b73c70（同两虚槽）
//
class CUser;   // UpdateEventInfo/SendCharacState/Process 参数（定义 CUserDepsB.cpp）
// 布局（ctor 反汇编实证）：
//   LevelupSupportEventManger（0x40 字节）：
//     +0x00 vptr（ARAD::ScriptInterface 主基类；ARAD::Singleton<T> 空基 EBO 同址）
//     +0x04 LevelupSupportSciprt m_script（0x18）
//     +0x1c std::map<int,int> m_stepIndexMap（0x18，level → 档位序号）
//     +0x34 std::vector<EventInfo> m_events
//   LevelupSupport2ndEventManger（0x28 字节）：+0x00 vptr | +0x04 m_script（0x24）
//
// 尚未恢复的同类 ORIG 方法（后续批次按 docs/class_func_reports/ 对应报告补齐，
// 本头暂不声明以免引入未定义依赖）：
//   LevelupSupportEventManger::SendCharacState(CUser*,int)         0x0814701e
//   LevelupSupportEventManger::CheckNewStep(int)                   0x08147164
//   LevelupSupportEventManger::UpdateEventInfo(CUser*)             0x081471c6
//   LevelupSupportEventManger::GetStepIndex(int)                   0x08147362
//   LevelupSupportEventManger::InsertItem(CUser*,int,ENUM_ERROR&)  0x081473cc
//   LevelupSupportEventManger::GetItemInfoVector(int)              0x08147724
//   LevelupSupportEventManger::GetDefaultEventInfo() const         0x08147012
//   LevelupSupport2ndEventManger::Process(CUser*)                  0x08147880

#ifndef DNF_GAME_LEVELUP_SUPPORT_EVENT_MANGER_H
#define DNF_GAME_LEVELUP_SUPPORT_EVENT_MANGER_H

#include <map>
#include <string>
#include <vector>

#include "Arad_DataManager.h"   // ARAD::ScriptInterface / ARAD::Singleton

#ifndef DNF_GAME_ENUM_ERROR_DEFINED
#define DNF_GAME_ENUM_ERROR_DEFINED
enum ENUM_ERROR { ENUM_ERROR_NONE = 0 };
#endif

// ============================================================================
// LevelupSupportSciprt —— tw_LevelupSupport.evt 解析器（ORIG 全局类，size 0x18）。
// 成员 +0x00 即 map<int, vector<ItemInfo>*>（LevelupSupportEventManger TU 内
// map<...>::clear/begin/end/find 模板实例均以 this+4 为 map 地址实证）。
// ============================================================================
class LevelupSupportSciprt
{
public:
    // ORIG ItemInfo::ItemInfo(int,int)（W 0x08ab5a44）：+0 dword=首参、+4 word=次参截断。
    // GetItemInfo 族输出 pair<int,int> 即取这两字段。
    struct ItemInfo
    {
        ItemInfo(int itemIndex, int itemCount)
            : m_itemIndex(itemIndex), m_itemCount((unsigned short)itemCount) {}
        int            m_itemIndex;   // +0x00
        unsigned short m_itemCount;   // +0x04
    };

    LevelupSupportSciprt();    // _ZN20LevelupSupportSciprtC1Ev @0x08ab5124
    ~LevelupSupportSciprt();   // _ZN20LevelupSupportSciprtD1Ev @0x08ab5138

    // ORIG 0x08ab514c："Event/"+"tw_LevelupSupport.evt"；[step][level][reward][/step]
    // 四节状态机。返回 0=成功；-1=文件打开失败或重复 [level]（重复路径不 unload，忠实）。
    int loadScript();
    // ORIG 0x08ab5442：遍历 delete 堆上 vector 后 map.clear。注意 ORIG 循环体无迭代器
    // 递增（忠实复刻，见 cpp 注释）。
    void clearScript();

    std::map<int, std::vector<ItemInfo>*> m_stepItems;   // +0x00 level → 奖励列表
};

// ============================================================================
// LevelupSupport2ndScript —— tw_LevelupSupport2nd.evt 解析器（ORIG 全局类，size 0x24）。
// +0x00 int 职业（[job type]；ORIG C1 故意不初始化该字段——仅构造 map 与两个 string）
// +0x04 map<int, vector<ItemInfo>*>（0x18）| +0x1c/+0x20 两个 std::string（邮件标题/正文）。
// ============================================================================
class LevelupSupport2ndScript
{
public:
    struct ItemInfo
    {
        ItemInfo(int itemIndex, int itemCount)
            : m_itemIndex(itemIndex), m_itemCount((unsigned short)itemCount) {}
        int            m_itemIndex;   // +0x00
        unsigned short m_itemCount;   // +0x04
    };

    LevelupSupport2ndScript();    // _ZN23LevelupSupport2ndScriptC1Ev @0x08ab54be
    ~LevelupSupport2ndScript();   // _ZN23LevelupSupport2ndScriptD1Ev @0x08ab5530

    // ORIG 0x08ab55a4："Event/"+"tw_LevelupSupport2nd.evt"；[job type]/
    // [step][level][reward][/step]/[mail title]/[mail content]。返回 0=成功；
    // -1=打开失败或重复 [level]（重复路径不 unload，忠实）。
    int loadScript();
    // ORIG 0x08ab5964：与 LevelupSupportSciprt::clearScript 同构（同样无迭代器递增）。
    void clearScript();

    int                                   m_jobType;      // +0x00
    std::map<int, std::vector<ItemInfo>*> m_stepItems;    // +0x04
    std::string                           m_mailTitle;    // +0x1c
    std::string                           m_mailContent;  // +0x20
};

// ============================================================================
// LevelupSupportEventManger —— ARAD 脚本服务 #6（ORIG size 0x40）。
// Load（ORIG 0x08146e3c）：清空脚本 map → loadScript（非 0 即失败）→ 清空
// m_stepIndexMap/m_events → 按 map 升序写入 level→序号 与 EventInfo(level,0)。
// ============================================================================
class LevelupSupportEventManger : public ARAD::Singleton<LevelupSupportEventManger>,
                                  public ARAD::ScriptInterface
{
public:
    // ORIG EventInfo::EventInfo(unsigned short,unsigned short)（W 0x08147b2a，4 字节）。
    // UpdateEventInfo(0x081471c6) 实证语义：+0 达标等级门槛（与角色等级比较），
    // +2 领取状态（0 默认 → 写 1；==2 视为已完成跳过）。
    struct EventInfo
    {
        EventInfo(unsigned short level, unsigned short state)
            : m_level(level), m_state(state) {}
        unsigned short m_level;   // +0x00
        unsigned short m_state;   // +0x02
    };

    LevelupSupportEventManger();    // _ZN25LevelupSupportEventMangerC1Ev @0x08146d30
    ~LevelupSupportEventManger();   // _ZN25LevelupSupportEventMangerD1Ev @0x08146dc0

    virtual bool Load();    // vtable 槽0（ORIG 槽值 0x08146e3c）
    virtual int Destroy(); // vtable 槽1（ORIG 槽值 0x08146fda）
    // 以下三方法 ORIG 直调依赖（CUser::processLevelUpEvent 域；定义 CUserDepsB.cpp）。
    bool CheckNewStep(int level);                     // ORIG T 0x08147164
    void UpdateEventInfo(CUser* user);                // ORIG T 0x081471c6
    static void SendCharacState(CUser* user, int state);  // ORIG T 0x0814701e（静态，操作 user->m_levelupEvents）
    const std::vector<EventInfo>& GetDefaultEventInfo() const;  // ORIG T 0x08147012

    // ---- UserMiscLink 批补（ORIG 实证；定义 Game/UserMiscLink.cpp）----
    int GetStepIndex(int requestLevel);                 // ORIG T 0x08147362（m_stepIndexMap find→second/-1）
    static void InsertItem(CUser* user, int eventIdx, ENUM_ERROR& err);  // ORIG T 0x081473cc（静态：无 this，user@0x8；内部经 Singleton 重取实例）
    std::vector<LevelupSupportSciprt::ItemInfo>* GetItemInfoVector(int level);  // ORIG T 0x08147724（m_script.m_stepItems find→second/NULL）

    LevelupSupportSciprt   m_script;        // +0x04
    std::map<int, int>     m_stepIndexMap;  // +0x1c level → 档位序号
    std::vector<EventInfo> m_events;        // +0x34
};

// ============================================================================
// LevelupSupport2ndEventManger —— ARAD 脚本服务 #9（ORIG size 0x28）。
// Load（ORIG 0x081477e4）：清空脚本 map → loadScript → 返回 (ret==0)；无额外成员。
// ============================================================================
class LevelupSupport2ndEventManger : public ARAD::Singleton<LevelupSupport2ndEventManger>,
                                     public ARAD::ScriptInterface
{
public:
    LevelupSupport2ndEventManger();    // _ZN28LevelupSupport2ndEventMangerC1Ev @0x0814778e
    ~LevelupSupport2ndEventManger();   // _ZN28LevelupSupport2ndEventMangerD1Ev @0x081477c4

    virtual bool Load();    // vtable 槽0（ORIG 槽值 0x081477e4）
    virtual int Destroy(); // vtable 槽1（ORIG 槽值 0x08147864）
    void Process(CUser* user);   // ORIG T 0x08147880（定义 CUserDepsB.cpp）

    LevelupSupport2ndScript m_script;   // +0x04
};

#endif  // DNF_GAME_LEVELUP_SUPPORT_EVENT_MANGER_H
