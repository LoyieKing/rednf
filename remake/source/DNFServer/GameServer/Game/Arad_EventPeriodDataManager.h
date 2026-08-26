// df_game_r Game/ ARAD::Arad_EventPeriodDataManager（批3：从 GameStubs.cpp 迁移）。
//
// ORIG（df_game_r, elf32-i386）：
//   class ARAD::Arad_EventPeriodDataManager（namespace ARAD）
//   类名 "Arad_EventPeriodDataManager" = 27 字符
//   bool isApplied(ENUM_REPEAT_EVENT_CODE, unsigned int)
//   mangled: _ZN4ARAD27Arad_EventPeriodDataManager9isAppliedE22ENUM_REPEAT_EVENT_CODEj  @ 0x081956b0
//
// 注意：ENUM_REPEAT_EVENT_CODE 必须位于全局命名空间（非 ARAD 内），
//   否则 Itanium ABI 会生成 NS_22ENUM_REPEAT_EVENT_CODEE 而无法匹配 ORIG 的
//   E22ENUM_REPEAT_EVENT_CODE 符号（已用 g++ 实测验证）。

#ifndef DNF_ARAD_EVENTPERIODDATAMANAGER_H
#define DNF_ARAD_EVENTPERIODDATAMANAGER_H

#include <vector>
#include <list>
namespace ARAD { class FnRewardLethe; class FnRewardMomoji; }


#include "Arad_DataManager.h"   // ARAD::Arad_InterfaceGameScript
#include "GameTypes.h"          // Mutex / Guard<Mutex>（ORIG +0x24 锁）

namespace arad { struct SigLoadRewardEventItem; }   // 完整定义见 MoreTypes.h

// 全局作用域枚举（ORIG ABI 要求，勿移入 ARAD 命名空间）。
enum ENUM_REPEAT_EVENT_CODE
{
    ENUM_REPEAT_EVENT_CODE_NONE = 0,
    ENUM_REPEAT_EVENT_CODE_0x8e = 0x8e,   // ResetAllFatigue 中用于判断的重复事件码
};

namespace ARAD
{
namespace SCRIPT
{
// ARAD::SCRIPT::SEventPeriodData —— isApplied 遍历的事件周期数据项。
//   +0x00 m_eventCode  事件码（SearchEventPeriod 匹配键）
//   +0x04 m_beginTime  生效开始时间（含）
//   +0x08 m_endTime    生效结束时间（不含）
struct SEventPeriodData
{
    unsigned int m_eventCode;
    unsigned int m_beginTime;
    unsigned int m_endTime;
};

// ARAD::SCRIPT::SRewardPeriodData —— +0x18 vector 元素类型。
// ORIG 布局由 SRewardPeriodDataC1Eimsjj 0x088b439c 反汇编实证（5 字段）：
//   C1(int eventCode, unsigned long f4, short f8, unsigned int begin, unsigned int end)
//   +0x00 int   m_eventCode
//   +0x04 u32   m_field4（eventReward<Fn> 转发 operator() 第2参 itemIndex）
//   +0x08 u16   m_field8（转发第3参 count，sw 符号扩展）
//   +0x0c u32   m_beginTime（eventReward 判 begin<=time）
//   +0x10 u32   m_endTime（判 time<end）
// sizeof = 0x14。旧 12 字节三字段【推断】版与 ORIG 不符，已按实证修正。
struct SRewardPeriodData
{
    SRewardPeriodData(int eventCode, unsigned long f4, short f8,
                      unsigned int beginTime, unsigned int endTime);

    int           m_eventCode;    // +0x00
    unsigned int  m_field4;       // +0x04
    short         m_field8;       // +0x08（对齐 pad +0x0a..+0x0b）
    unsigned int  m_beginTime;    // +0x0c
    unsigned int  m_endTime;      // +0x10
};

}  // namespace SCRIPT

// ARAD::EventRewardItemArad —— 奖励记录键（ORIG 0x8195d94 C1 / 0x8195db4 operator==，均 W）。
// ctor(i,jj) 三字段直存；operator== 依次比较 +0x00/+0x04/+0x08。
class EventRewardItemArad
{
public:
    EventRewardItemArad(int eventCode, unsigned int accId, unsigned int characNo)
        : m_eventCode(eventCode), m_accId(accId), m_characNo(characNo)
    {
    }

    bool operator==(EventRewardItemArad const& other) const
    {
        return m_eventCode == other.m_eventCode &&
               m_accId == other.m_accId &&
               m_characNo == other.m_characNo;
    }

    int m_eventCode;            // +0x00（SigLoadRewardEventItem 条目事件码）
    unsigned int m_accId;       // +0x04（delUserRewardData 与 get_acc_id 比对）
    unsigned int m_characNo;    // +0x08【推断：角色编号】
};

// ARAD::SearchEventPeriod —— find_if 谓词（ORIG 0x8195d72 C1 / 0x8195d80 op()，均 W）。
class SearchEventPeriod
{
public:
    SearchEventPeriod(ENUM_REPEAT_EVENT_CODE eventCode) : m_eventCode(eventCode) {}
    bool operator()(SCRIPT::SEventPeriodData const& data) const
    {
        return data.m_eventCode == m_eventCode;
    }
    // ORIG W 0x0819a8b4：SRewardPeriodData 版本（eventReward<F> 的 find_if 使用）。
    bool operator()(SCRIPT::SRewardPeriodData const& data) const
    {
        return data.m_eventCode == (int)m_eventCode;
    }

private:
    ENUM_REPEAT_EVENT_CODE m_eventCode;   // +0x00
};

class Arad_EventPeriodDataManager : public Arad_InterfaceGameScript
{
public:
    Arad_EventPeriodDataManager();   // ORIG 0x81953fa（基类 Arad_InterfaceGameScript + 容器）
    virtual ~Arad_EventPeriodDataManager();   // ORIG ~ 0x8195632 / 0x81956bc

    // ORIG 0x08195650（vtable 槽2 loadScriptData）：装载事件周期脚本数据。
    virtual bool loadScriptData();
    // ORIG 0x08195670（vtable 槽3 clearScriptData）：清空事件周期脚本数据。
    virtual void clearScriptData();
    // ORIG 0x08195794：事件 code 在有效周期内（m_beginTime <= time < m_endTime）返回 true。
    bool isApplied(ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time);

    // ORIG 0x08195686：以当前 tick 为 time 调用 isApplied（供调用方/宿主迁移参考）。
    bool isRunning(ENUM_REPEAT_EVENT_CODE eventCode);

    // ORIG 0x08195994：按 SigLoadRewardEventItem 条目构造 EventRewardItemArad，
    // 未奖励过的加入记录，已奖励的写 logFormat 日志。
    void addUserRewardData(CUser* user, arad::SigLoadRewardEventItem& sig);
    // ORIG 0x08195aa4：加锁后 push_back 到 +0x3c list<EventRewardItemArad>。
    void addUserRewardData(CUser* user, EventRewardItemArad& item);
    // ORIG 0x081958b6：加锁后在 +0x3c list 中 std::find 该奖励记录（按值传参）。
    bool isRewarded(EventRewardItemArad item);
    // ORIG 0x08195794：周期存在且 begin(GM 扩展) <= time < m_endTime 返回 true（无锁）。
    bool checkCreateTime(ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time,
                         unsigned int gmId);
    // ORIG 0x08195b06：加锁后删除 +0x3c list 中 accId 匹配的全部记录。
    void delUserRewardData(CUser* user);
    // ORIG 0x0819589e：param >= 0 时写入 m_field8（GM 建号回溯天数）。
    void setCreatePeriod_GM(int days);
    // ORIG 模板成员（W 实例）：0x0819abc8 eventReward<FnRewardLethe>、
    // 0x0819adb6 eventReward<FnRewardMomoji>。定义（含 FnReward* 函数对象）
    // 归 Arad_EventPeriodDataManager 生产批次；此处仅声明供派发器调用。
    template <typename F>
    bool eventReward(ENUM_REPEAT_EVENT_CODE eventCode, unsigned int time, CUser* user);


private:
    int m_field8;                    // +0x08（ctor 置 0；setCreatePeriod_GM 写入）
    std::vector<ARAD::SCRIPT::SEventPeriodData> m_eventPeriodList;   // +0x0c
    std::vector<ARAD::SCRIPT::SRewardPeriodData> m_rewardPeriodList; // +0x18
    Mutex m_mutex;                                    // +0x24（pthread_mutex_t 0x18）
    std::list<EventRewardItemArad> m_rewardItemList;  // +0x3c
};

}  // namespace ARAD

#endif  // DNF_ARAD_EVENTPERIODDATAMANAGER_H