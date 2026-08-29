// CRestrictCharacCreationEvent —— 限制创建角色事件（CEventManager event 叶类）。
// ORIG vtable 0x08bfb7a8（报告 0x08273c44 实证）：0/1 虚析构，2/3 =
// StartEvent()/EndEvent() 纯虚覆盖，4 = StartEvent(Word_Param)（完整实现：
// 写 m_limitType + trace），其余槽继承 CEventBase。
// 布局（报告 ctor 0x08273c44 汇编实证）：CEventBase 基区（vptr +0x00 /
// stADDINFO +0x04 / m_isEventing +0x08，sizeof 0x0c）；
// int m_limitType @ +0x0c（ctor 置 2；StartEvent(Word_Param) 以整 4 字节
// movl 写入 param 低 ushort；getLimitCreateCharacter 0x08273dba 消费）。
#ifndef DNF_GAME_CRESTRICTCHARACCREATIONEVENT_H_
#define DNF_GAME_CRESTRICTCHARACCREATIONEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CRestrictCharacCreationEvent : public CEventBase
{
public:
    CRestrictCharacCreationEvent();
    virtual ~CRestrictCharacCreationEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    // ORIG 0x08273dba：this+0xc 类型槽 0 → characCount==0 时限 7 否则 0；
    // 1 → 限 8；其它 → 0。
    int getLimitCreateCharacter(int characCount) const;

    int m_limitType;   // +0x0c
};

#endif // DNF_GAME_CRESTRICTCHARACCREATIONEVENT_H_
