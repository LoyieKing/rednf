// CCharacterDayEvent —— 角色生日事件（CEventBase 派生）。
// 布局/虚表证据：docs/class_func_reports/CCharacterDayEvent/ 下报告；
// vtable @0x08b48c00：槽2=StartEvent() 0x0810a53c、槽3=EndEvent() 0x0810a5ec、
// 槽4=StartEvent(Word_Param) 0x0810a58e（覆写基类弱实现），其余槽继承 CEventBase。
// +0x0a 为 ushort 事件参数（ctor 0x0810a4c8 movw $0xffff 置 0xffff/-1；
// StartEvent(Word_Param) 0x0810a58e 写入——movzwl 0xc(%ebp)→mov %dx,0xa；
// GetEventJob 0x08697886 movzwl 0xa 直返，零扩展证据 ⇒ unsigned 成员）。
#ifndef DNF_GAME_CCHARACTERDAYEVENT_H_
#define DNF_GAME_CCHARACTERDAYEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase 基类 / Word_Param（按值传参）

class CCharacterDayEvent : public CEventBase
{
public:
    CCharacterDayEvent();                          // 0x0810a4c8
    virtual ~CCharacterDayEvent();                 // D1/D0 0x0810a4ee / 0x0810a51e
    virtual void StartEvent();                     // 0x0810a53c
    virtual void StartEvent(Word_Param param);     // 0x0810a58e（vtable+0x18）
    virtual void EndEvent();                       // 0x0810a5ec
    unsigned short GetEventJob();                  // 0x08697886（非虚，movzwl 0xa 直返）

private:
    unsigned short m_eventJob;   // +0x0a（ctor 置 0xffff/-1；StartEvent(Word_Param) 写入）
};


#endif // DNF_GAME_CCHARACTERDAYEVENT_H_
