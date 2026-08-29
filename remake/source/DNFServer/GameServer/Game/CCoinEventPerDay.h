// CCoinEventPerDay —— 每日币事件（CEventBase 叶事件，s_event_manager->GetRepeatEvent(4)）。
// ORIG 报告：docs/class_func_reports/CCoinEventPerDay/（7 函数全读）。
//   C1 0x0810ab10 / D1 0x0810ab48 / D0 0x0810ab78 / StartEvent() 0x0810ab96 /
//   StartEvent(Word_Param) 0x0810ac04 / EndEvent 0x0810aca8 / GetCoinNoPerDay 0x0810ad16。
// 布局（C1/StartEvent/EndEvent/GetCoinNoPerDay 汇编实证）：CEventBase 基区
// （vptr +0x00 / stADDINFO +0x04 / m_isEventing +0x08，sizeof 0x0c）+ 4 个字节档位
// 字段 @ +0x09/+0x0a/+0x0b/+0x0c（复用基类尾 padding；StartEvent(Word_Param) 以
// 整 4 字节 movl 写入 +0x09，GetCoinNoPerDay 按等级 <0x12/<0x1b/其余 取 +0x09/+0x0a/+0x0b）。
#ifndef DNF_GAME_CCOINEVENTPERDAY_H_
#define DNF_GAME_CCOINEVENTPERDAY_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CCoinEventPerDay : public CEventBase
{
public:
    CCoinEventPerDay();
    virtual ~CCoinEventPerDay();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    int GetCoinNoPerDay(int level);   // 0x0810ad16

    // +0x09..+0x0c：4 个字节档位币数（ORIG 复用基类尾 padding +0x09..+0x0b）。
    // 匿名 union 加 __attribute__((packed)) 使对齐降为 1：字段置于 +0x09（与
    // ORIG movb 0x9/0xa/0xb/0xc 一致），m_coinPack 作为编译期常量偏移成员让
    // StartEvent(Word_Param) 生成单 disp movl 写 +0x09（ORIG 未对齐整写实证；
    // 裸 unsigned int 成员/强转指针均会推到 +0x0c 或产生 add 两步）。
    union {
        struct {
            unsigned char m_coin01;   // +0x09：等级 < 0x12 档
            unsigned char m_coin02;   // +0x0a：等级 < 0x1b 档
            unsigned char m_coin03;   // +0x0b：其余档
            unsigned char m_coin04;   // +0x0c
        };
        unsigned int m_coinPack;      // +0x09（4 字节整体写视图）
    } __attribute__((packed));
};

#endif // DNF_GAME_CCOINEVENTPERDAY_H_
