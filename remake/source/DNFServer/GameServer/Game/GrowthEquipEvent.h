#ifndef DNF_GROWTHEQUIPEVENT_H
#define DNF_GROWTHEQUIPEVENT_H

// df_game_r Game/ GrowthEquipEvent 权威头（成长武器活动事件，GetRepeatEvent(100)）。
// 函数清单依据 docs/class_func_reports/GrowthEquipEvent/README.md（11 函数，
// ORIG 0x080f6ef0 族）。类无自有数据成员：C1 0x080f6ef0 仅调 CEventBase::CEventBase()
// 后写本类 vptr；sizeof == sizeof(CEventBase)。
// 唯一声明点：Dispatcher_UseBoosterItem_impl.cpp 的本地镜像类已删除、改 include 本头。

#include "CGuildServerProxy.h"   // CEventBase / Word_Param

class CUser;
class Inven_Item;

class GrowthEquipEvent : public CEventBase
{
public:
    GrowthEquipEvent();                            // C1 0x080f6ef0
    virtual ~GrowthEquipEvent();                   // D1 0x080f6f0c / D0 0x080f6f3c

    virtual void StartEvent();                     // 0x080f6f5a
    virtual void StartEvent(Word_Param param);     // 0x080f6fac
    virtual void EndEvent();                       // 0x080f6ffe
    virtual void StartAction();                    // 0x080f7710
    virtual bool IsEventing(CUser* user) const;    // W 0x080f7820（转发基类实现）

    void SelectGrowthWeapon(CUser& user, int itemId, int addInfo);   // T 0x080f73be
    void SendGrowthWeaponEventInfo(CUser& user);                      // T 0x080f7226

    // 以下两个方法已有完整报告但尚未还原（依赖 RDAR 字符串/邮件/DB 链）；
    // 声明占位以维持权威头完整，勿在无定义前 ODR-use。
    void GiftGrowthWeaponBox(CUser& user) const;                       // T 0x080f7050
    void SendInfinityGrowthWeapon(CUser* user, const Inven_Item* item,
                                  int a, int b) const;                 // T 0x080f74b2
};

#endif  // DNF_GROWTHEQUIPEVENT_H
