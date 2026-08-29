// Arad_MomijiEvent —— 阿拉德红叶事件（CEventBase 叶事件，Game 域）。
// ORIG vtable 0x08ba8c00（实测）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 纯虚覆盖，4 = StartEvent(Word_Param)（本类为 thunk，经 vptr+8 虚调
// StartEvent()），其余槽继承 CEventBase。
// 布局：CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
// sizeof 0x0c），无扩展成员（C1 0x0819e738 仅基类构造+vptr 置位，报告实证）。
// 注意：StartEvent()/EndEvent() 中 cMyTrace 日志在 SetEventFlag 之前
// （ORIG 0x0819e7a2/0x0819e80e 汇编序，报告实证）。
// 报告：docs/class_func_reports/Arad_MomijiEvent/ 全部 6 个方法已读。
#ifndef DNF_GAME_ARAD_MOMIJIEVENT_H_
#define DNF_GAME_ARAD_MOMIJIEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class Arad_MomijiEvent : public CEventBase
{
public:
    Arad_MomijiEvent();
    virtual ~Arad_MomijiEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_ARAD_MOMIJIEVENT_H_
