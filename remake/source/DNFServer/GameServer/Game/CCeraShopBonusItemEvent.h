// CCeraShopBonusItemEvent —— Cera 商城加成事件（CEventBase 叶事件）。
// ORIG vtable 0x08b484a8（C1 0x0810a2dc 汇编实证）：0/1 虚析构，2/3 =
// StartEvent()/EndEvent() 纯虚覆盖，4 = StartEvent(Word_Param)，其余槽继承
// CEventBase（权威 vtable 0x08b4bae0）。
// 布局（InitEventManager new 0x10 + getBonusRate W 0x0817a2e2 读 +0x0c +
// StartEvent(Word_Param) 写 +0x0c 实证）：CEventBase 基区（vptr +0x00 /
// stADDINFO +0x04 / m_isEventing +0x08，sizeof 0x0c）+ m_rate +0x0c
// （unsigned int bonusRate），sizeof 0x10。
// 2026-08-29 EventManagerWave：由 W6_CeraShopDeps.cpp / CCeraShopAddRestrict.cpp
// 的局部调用视图提升为唯一权威声明（P104 ODR 收敛），消费 TU 统一 include。
#ifndef DNF_GAME_CCERASHOPBONUSITEMEVENT_H_
#define DNF_GAME_CCERASHOPBONUSITEMEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CCeraShopBonusItemEvent : public CEventBase
{
public:
    CCeraShopBonusItemEvent();
    virtual ~CCeraShopBonusItemEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
    int getBonusRate() const;   // ORIG W _ZNK23CCeraShopBonusItemEvent12getBonusRateEv @0x0817a2e2

    unsigned int m_rate;   // +0x0c（bonusRate；StartEvent(Word_Param) 写入）
};

#endif // DNF_GAME_CCERASHOPBONUSITEMEVENT_H_
