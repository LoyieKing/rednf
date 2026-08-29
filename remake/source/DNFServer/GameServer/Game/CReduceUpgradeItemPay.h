// CReduceUpgradeItemPay —— 强化减费重复事件（CEventManager event 叶类；
// CItemUpgrade/ItemUpgradeVtableB2 消费 GetReduceRate）。
// ORIG vtable 0x08b4f360（实测）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 纯虚覆盖，4 = StartEvent(Word_Param)（完整实现：把 param 两个 ushort 写入
// 基类 stADDINFO（+0x04/+0x06）→ 虚调 StartEvent() → trace），其余槽继承
// CEventBase。无扩展成员（C1 0x0811715c 仅基类构造+vptr，objdump 实证）。
#ifndef DNF_GAME_CREDUCEUPGRADEITEMPAY_H_
#define DNF_GAME_CREDUCEUPGRADEITEMPAY_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CReduceUpgradeItemPay : public CEventBase
{
public:
    CReduceUpgradeItemPay();
    virtual ~CReduceUpgradeItemPay();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    // ORIG W 0x0854b56e：返回 +0x04 short 经 cwtl 符号扩展。
    int GetReduceRate();
};

#endif // DNF_GAME_CREDUCEUPGRADEITEMPAY_H_
