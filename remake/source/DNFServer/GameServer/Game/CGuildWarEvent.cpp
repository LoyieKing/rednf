// df_game_r 公会战事件 CGuildWarEvent 实现（G3 公会域，2026-08-17）。
// 语义依据 docs/class_func_reports/CGuildWarEvent.md + ORIG 反汇编。
#include "CGuildWarEvent.h"

#include "CServerProxyMgr.h"
#include "CEnvironment.h"
#include "GameWorld.h"
#include "TimerQueue.h"

// ORIG W 0x80f647c（CBattle_Field_deps.h 的本地声明含冲突 CEventManager，
// 此处单独声明本 TU 需要的外部符号）
TimerQueue* G_TimerQueue();

// CEnvironment +0x1b0 视图（服务器组号；GameWorld.cpp 同偏移读取）
struct CEnvView
{
    char m_pad[0x1b0];
    int m_gcNo;   // +0x1b0
};

CGuildWarEvent::CGuildWarEvent()
{
    m_duration = 5;
    m_field0c = 0;
}

CGuildWarEvent::~CGuildWarEvent()
{
}

void CGuildWarEvent::StartEvent()
{
    if (IsEventing(0))
    {
        return;
    }
    SetEventFlag(true);
    m_duration = 5;
    m_field0c = 0;
    if (G_GameWorld()->GetChannelType() == 6)
    {
        G_TimerQueue()->InsertTimer(
            TimerEntry::OBJ_TYPE_2, 0, TIMER_MESSAGE_0x62,
            (int)m_duration * 0xe10 - 600, 10, 0);
        CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr
                                       ->GetServerProxy((ENUM_SERVER_GROUP)0);
        proxy->SendGuildWarStart(
            (unsigned char)((CEnvView*)G_CEnvironment())->m_gcNo);
        cMyTrace("virtual void CGuildWarEvent::StartEvent()", 0x27, 0)(
            "Guild War Event Start! time(%d)", (int)m_duration);
    }
}

void CGuildWarEvent::StartEvent(Word_Param param)
{
    if (IsEventing(0))
    {
        return;
    }
    SetEventFlag(true);
    m_duration = param.m_duration;
    m_field0c = param.m_field2;
    if (G_GameWorld()->GetChannelType() == 6)
    {
        G_TimerQueue()->InsertTimer(
            TimerEntry::OBJ_TYPE_2, 0, TIMER_MESSAGE_0x62,
            (int)m_duration * 0xe10 - 600, 10, 0);
        CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr
                                       ->GetServerProxy((ENUM_SERVER_GROUP)0);
        int serverGroup = ((CEnvView*)G_CEnvironment())->m_gcNo;
        proxy->SendGuildWarStart((unsigned char)serverGroup);
        cMyTrace("virtual void CGuildWarEvent::StartEvent(Word_Param)", 0x39,
                 0)("Guild War Event Start! time(%d), server(%d)",
                    (int)param.m_duration, serverGroup);
    }
}

void CGuildWarEvent::EndEvent()
{
    if (!IsEventing(0))
    {
        return;
    }
    if (G_GameWorld()->GetChannelType() == 6)
    {
        CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr
                                       ->GetServerProxy((ENUM_SERVER_GROUP)0);
        proxy->SendGuildWarEnd(
            (unsigned char)((CEnvView*)G_CEnvironment())->m_gcNo);
    }
    SetEventFlag(false);
    m_duration = 0;
    m_field0c = 0;
    cMyTrace("virtual void CGuildWarEvent::EndEvent()", 0x49, 0)(
        "Guild War Event End");
}
