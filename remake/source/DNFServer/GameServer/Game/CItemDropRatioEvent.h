// CItemDropRatioEvent —— 物品掉落倍率事件（CEventBase 叶事件）。
// ORIG vtable 0x08c99368（报告实证）：0/1 虚析构，2/3 = StartEvent()/EndEvent()
// 覆盖，4 = StartEvent(Word_Param)，其余槽继承 CEventBase。
// 布局（ORIG ctor 0x085331d0 + StartEvent 0x08533244 汇编实证）：
//   CEventBase 基区（sizeof 0x0c）；unsigned int m_dropRate @ +0x0c
//   （C1/StartEvent/EndEvent 均 movl 写 1，StartEvent(Word_Param) 写
//   param.m_duration；StartEvent(Word_Param) 以 (float)m_dropRate 无符号
//   扩展 fildll 转 float 传给 CDataManager::SetItemDropRate）。
#ifndef DNF_GAME_CITEMDROPRATIOEVENT_H_
#define DNF_GAME_CITEMDROPRATIOEVENT_H_

#include "CGuildServerProxy.h"   // CEventBase / Word_Param（ORIG 权威声明）

class CItemDropRatioEvent : public CEventBase
{
public:
    CItemDropRatioEvent();
    virtual ~CItemDropRatioEvent();

    virtual void StartEvent();
    virtual void EndEvent();
    virtual void StartEvent(Word_Param param);

    unsigned int m_dropRate;   // +0x0c
};

#endif // DNF_GAME_CITEMDROPRATIOEVENT_H_
