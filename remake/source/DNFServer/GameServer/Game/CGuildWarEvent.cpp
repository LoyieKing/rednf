// df_game_r 公会战事件 CGuildWarEvent 实现（G3 公会域，2026-08-17）。
// 语义依据 docs/class_func_reports/CGuildWarEvent.md + ORIG 反汇编
// （C1 0x0811cfa8 / StartEvent 0x0811d024 / StartEvent(Word_Param) 0x0811d152 /
//   EndEvent 0x0811d28c / vtable 0x08b553a0）。
#include "CGuildWarEvent.h"

#include <stddef.h>   // offsetof

#include "CServerProxyMgr.h"
#include "CEnvironment.h"
#include "GameWorld.h"
#include "TimerQueue.h"

// ORIG W 0x80f647c（TimerQueue.h 权威声明；此处单独声明避免整头依赖）
TimerQueue* G_TimerQueue();

// CEnvironment +0x1b0 视图（服务器组号；GameWorld.cpp 同偏移读取）
struct CEnvView
{
    char m_pad[0x1b0];
    int m_gcNo;   // +0x1b0
};

// this+0x0a 处 Word_Param 整体 4 字节拷贝视图（ORIG mov 0xc(%ebp),%edx;
// mov %edx,0xa(%eax) 单 movl，非两次 movw；P1 组件视图折叠单 disp）
struct GWView
{
    char m_pad[0xa];
    Word_Param m_wp;   // +0x0a
};
// ORIG C1（0x0811cfa8）movw $0x5,+0xa / movw $0x0,+0xc；ORIG new 两处
// （0x08114f9d/0x0811ad0c）均为 _Znwj(0x10) → sizeof 0x10（基类 align 4 上舍入；
// ushort 字段复用基类尾 padding 落在 +0xa/+0xc，gcc4.4 ABI-2 行为）。
static_assert(sizeof(CGuildWarEvent) == 0x10, "CGuildWarEvent sizeof");
static_assert(offsetof(CGuildWarEvent, m_duration) == 0xa, "m_duration +0xa");
static_assert(offsetof(CGuildWarEvent, m_field0c) == 0xc, "m_field0c +0xc");
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
    // ORIG xor 形：call;xor $0x1;test;je 尾 ⇒ if (!IsEventing(0)) { body } 块形。
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        m_duration = 5;
        m_field0c = 0;
        if (G_GameWorld()->GetChannelType() == 6)
        {
            G_TimerQueue()->InsertTimer(
                TimerEntry::OBJ_TYPE_2, 0, TIMER_MESSAGE_0x62,
                (int)m_duration * 0xe10 - 600, 10, 0);
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGuildWarStart(
                    (unsigned char)((CEnvView*)G_CEnvironment())->m_gcNo);
            cMyTrace(__PRETTY_FUNCTION__, 0x27, 0)(
                "Guild War Event Start! time(%d)", 5);
        }
    }
}

void CGuildWarEvent::StartEvent(Word_Param param)
{
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        ((GWView*)this)->m_wp = param;
        if (G_GameWorld()->GetChannelType() == 6)
        {
            G_TimerQueue()->InsertTimer(
                TimerEntry::OBJ_TYPE_2, 0, TIMER_MESSAGE_0x62,
                (int)m_duration * 0xe10 - 600, 10, 0);
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGuildWarStart(
                    (unsigned char)((CEnvView*)G_CEnvironment())->m_gcNo);
            cMyTrace(__PRETTY_FUNCTION__, 0x39, 0)(
                "Guild War Event Start! time(%d), server(%d)",
                (int)param.m_duration,
                ((CEnvView*)G_CEnvironment())->m_gcNo);
        }
    }
}

void CGuildWarEvent::EndEvent()
{
    if (IsEventing(0))
    {
        if (G_GameWorld()->GetChannelType() == 6)
        {
            GlobalData::s_guild_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendGuildWarEnd(
                    (unsigned char)((CEnvView*)G_CEnvironment())->m_gcNo);
        }
        SetEventFlag(false);
        m_duration = 0;
        m_field0c = 0;
        cMyTrace(__PRETTY_FUNCTION__, 0x49, 0)("Guild War Event End");
    }
}
