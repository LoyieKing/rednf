// df_game_r 还原 —— CAradRyosikaEvent（LinkClosureB-2 权威声明头）
// ORIG 符号（nm -S --defined-only 核对）：
//   T _ZN17CAradRyosikaEventC1Ev/C2Ev        0x081a0ea0 sz 0x30
//   T _ZN17CAradRyosikaEventD1Ev/D2Ev        0x081a0ed0 sz 0x30
//   T _ZN17CAradRyosikaEventD0Ev             0x081a0f00 sz 0x1e
//   T _ZN17CAradRyosikaEvent10StartEventEv   0x081a0f1e sz 0x6c
//   T _ZN17CAradRyosikaEvent10StartEventE11Word_Param 0x081a0f8a sz 0x34
//   T _ZN17CAradRyosikaEvent9EndEventEv      0x081a0fbe sz 0x1b
//   T _ZN17CAradRyosikaEvent15ApplyDatePeriodEP5CUser 0x081a0fda sz 0xa
//   T _ZN17CAradRyosikaEvent8SendMailEP5CUseriii 0x081a0fe4 sz 0x97
//   T _ZN17CAradRyosikaEvent7DoEventEP5CUseriii 0x081a107c sz 0x7d
// 报告：docs/class_func_reports/CAradRyosikaEvent/（C1/D1/StartEvent×2/
// EndEvent/ApplyDatePeriod/SendMail/DoEvent）。
// 虚槽序与 CEventBase 权威 vtable 对位（ORIG _ZTV10CEventBase 0x08b4bae0
// 实测：+0x28=changeCharacName、+0x34=deliveryEnd、+0x3c=IsEventing；
// 头声明序即 vtable 槽序，与 _ZTV17CAradRyosikaEvent 0x08bab960 一致）。
#ifndef DNF_GAME_C_ARAD_RYOSIKA_EVENT_H_
#define DNF_GAME_C_ARAD_RYOSIKA_EVENT_H_

#include "CGuildServerProxy.h"   // CEventBase 权威基类 + CUser 前向可见性

class CAradRyosikaEvent : public CEventBase
{
public:
    CAradRyosikaEvent();                            // 0x081a0ea0
    ~CAradRyosikaEvent();                           // 0x081a0ed0 / D0 0x081a0f00

    void StartEvent();                              // 0x081a0f1e（ORIG 纯虚覆盖）
    void StartEvent(Word_Param param);              // 0x081a0f8a
    void EndEvent();                                // 0x081a0fbe（ORIG 纯虚覆盖）
    bool ApplyDatePeriod(CUser* user);              // 0x081a0fda
    void SendMail(CUser* user, int param2, int param3, int param4);  // 0x081a0fe4
    void DoEvent(CUser* user, int param2, int param3, int param4);   // 0x081a107c

private:
    int m_field0c;   // +0x0c（ctor 置 -1）
    int m_field10;   // +0x10（ctor 置 -1）
};

#endif  // DNF_GAME_C_ARAD_RYOSIKA_EVENT_H_
