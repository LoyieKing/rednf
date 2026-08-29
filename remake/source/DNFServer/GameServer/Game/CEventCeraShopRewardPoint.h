// CEventCeraShopRewardPoint —— Cera 商城奖励点数事件（CEventBase 叶事件）。
// ORIG vtable 0x08b7cb68（C1 0x0816459a 汇编实证）：0/1 虚析构，
// 2/3 = StartEvent()/EndEvent() 纯虚覆盖，4 = StartEvent(Word_Param)
// （thunk：经 vptr slot2 虚调 StartEvent()），其余槽继承 CEventBase
// （权威 vtable 0x08b4bae0）。
// 布局（ORIG C1 0x0816459a / StartEvent(Word_Param) 0x08164684 汇编实证）：
//   CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
//   sizeof 0x0c）+ m_fieldC +0x0c（ceraPoint）+ m_field10 +0x10（timing），
//   sizeof 0x14；C1 先清 +0x10 再清 +0x0c。
#ifndef DNF_GAME_CEVENTCERASHOPREWARDPOINT_H_
#define DNF_GAME_CEVENTCERASHOPREWARDPOINT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CUser;

class CEventCeraShopRewardPoint : public CEventBase
{
public:
    CEventCeraShopRewardPoint();
    virtual ~CEventCeraShopRewardPoint();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
    void SendReward(CUser* user);

    int m_fieldC;    // +0x0c（ceraPoint）
    int m_field10;   // +0x10（timing）
};

#endif // DNF_GAME_CEVENTCERASHOPREWARDPOINT_H_
