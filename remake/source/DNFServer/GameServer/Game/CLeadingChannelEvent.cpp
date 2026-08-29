// df_game_r 引导频道事件 CLeadingChannelEvent 实现。
// 报告：docs/class_func_reports/CLeadingChannelEvent/ 全部 6 个方法
// （CLeadingChannelEvent / ~CLeadingChannelEvent / ~CLeadingChannelEvent_0811fdba /
//  StartEvent / StartEvent_0811fe34 / EndEvent）。
#include "CLeadingChannelEvent.h"

#include <iostream>          // std::ios_base::Init 静态初始化（ORIG _GLOBAL__I__ZN20CLeadingChannelEventC2Ev 0x0811ff2d）

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811fd64：基类构造 + vptr 置位 + m_channelFactor = 100。
CLeadingChannelEvent::CLeadingChannelEvent()
    : m_channelFactor(100)
{
}

// ORIG 0x0811fd8a / 0x0811fdba：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CLeadingChannelEvent::~CLeadingChannelEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811fdd8：SetEventFlag(true) + m_channelFactor = 100 +
// cMyTrace(__FUNCTION__,0x13,0)("Leading Channel Event Start!")。
void CLeadingChannelEvent::StartEvent()
{
    SetEventFlag(true);
    m_channelFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("Leading Channel Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811fe34：SetEventFlag(true) + m_channelFactor = param.m_duration +
// cMyTrace(__FUNCTION__,0x1b,0)(同字符串)。
void CLeadingChannelEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_channelFactor = param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x1b, 0);
    tr("Leading Channel Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811fe92：SetEventFlag(false) + m_channelFactor = 100 +
// cMyTrace(__FUNCTION__,0x23,0)("Leading Channel Event End!")。
void CLeadingChannelEvent::EndEvent()
{
    SetEventFlag(false);
    m_channelFactor = 100;
    cMyTrace tr(__FUNCTION__, 0x23, 0);
    tr("Leading Channel Event End!");
}
