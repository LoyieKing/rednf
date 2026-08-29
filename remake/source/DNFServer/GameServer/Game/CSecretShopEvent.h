// CSecretShopEvent —— 秘密商店事件（CEventManager event 叶类）。
// ORIG vtable 0x08b4f320（实测）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 纯虚覆盖，4 = StartEvent(Word_Param)（thunk：经 vptr slot2 虚调 StartEvent()），
// 其余槽继承 CEventBase。无扩展成员（C1 0x081172c2 仅基类构造+vptr，objdump 实证）。
#ifndef DNF_GAME_CSECRETSHOPEVENT_H_
#define DNF_GAME_CSECRETSHOPEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CSecretShopEvent : public CEventBase
{
public:
    CSecretShopEvent();
    virtual ~CSecretShopEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_CSECRETSHOPEVENT_H_
