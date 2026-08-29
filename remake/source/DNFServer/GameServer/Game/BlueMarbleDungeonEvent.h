// BlueMarbleDungeonEvent —— 蓝大理石地下城事件（CEventBase 叶事件）。
// ORIG vtable 0x08b2a240（实测）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 纯虚覆盖，4 = StartEvent(Word_Param)（完整实现，非 thunk），其余槽继承
// CEventBase。
// 布局：CEventBase 基区（vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，
// sizeof 0x0c），无扩展成员（C1 0x080dc470 仅基类构造+vptr 置位，报告实证）。
// 报告：docs/class_func_reports/BlueMarbleDungeonEvent/ 全部 6 个方法已读。
#ifndef DNF_GAME_BLUEMARBLEDUNGEONEVENT_H_
#define DNF_GAME_BLUEMARBLEDUNGEONEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class BlueMarbleDungeonEvent : public CEventBase
{
public:
    BlueMarbleDungeonEvent();
    virtual ~BlueMarbleDungeonEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);
};

#endif // DNF_GAME_BLUEMARBLEDUNGEONEVENT_H_
