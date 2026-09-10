// CCharacterDayEvent —— 角色生日事件（CEventBase 派生）。
// 布局/虚表证据：docs/class_func_reports/CCharacterDayEvent/ 下报告；
// vtable @0x08b48c00：槽2=StartEvent() 0x0810a53c、槽3=EndEvent() 0x0810a5ec、
// 槽4=StartEvent(Word_Param) 0x0810a58e（覆写基类弱实现），其余槽继承 CEventBase。
// +0x0a 为 16 位事件参数（ctor 0x0810a4c8 movw $0xffff 置 -1 位模式；
// StartEvent(Word_Param) 0x0810a58e 写入——movzwl 0xc(%ebp),%eax → mov %eax,%edx
// → mov %dx,0xa；GetEventJob 0x08697886 movzwl 0xa 直返，返回类型 unsigned short
// 保证读侧零扩展）。字段声明 short（signed）而与源 unsigned short 异型：这是
// P211 档③/P248 的类型旋钮——同型时 4.4 恒折 `movzwl 0xc(%ebp),%edx` 单段，
// 异型才复现 ORIG 的 eax→edx 两段中转（探针 probe_hetero B/G、probe3 S/P、
// 修正后 AE 23/23 gap 0，2026-09-11 N5C_EventChar）。
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
    // signed short：与源表达式 param.m_duration（unsigned short）异型 ⇒ 4.4 复现
    // ORIG 0x0810a58e 的 `movzwl 0xc(%ebp),%eax; mov %eax,%edx; mov 0x8(%ebp),%eax;
    // mov %dx,0xa` 两段中转（P211 档③ / P248 类型旋钮；探针
    // /tmp/r5/n5/N5C_EventChar/probe_hetero.cpp 的 B/G 与 probe3.cpp 的 P/S 实证：
    // 同型 ushort 字段恒折 `movzwl 0xc(%ebp),%edx` 单段，异型 signed 才出 eax→edx
    // 中转；GetEventJob 返回 unsigned short ⇒ 读侧仍 movzwl 直返，ctor `= -1` ⇒
    // 仍 movw $0xffff。sizeof/偏移不变）。
    short m_eventJob;            // +0x0a
};


#endif // DNF_GAME_CCHARACTERDAYEVENT_H_
