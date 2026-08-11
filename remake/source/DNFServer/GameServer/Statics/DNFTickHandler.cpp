// df_statics_r — DNFTickHandler（ORIG DNFTickHandler.o 拆分）
#include <stdio.h>
#include <sys/times.h>

#include "DNFTickHandler.h"
#include "DNFApplication.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"

CFrameCountHandler::CFrameCountHandler()
{
    m_writeTick = 0;
    m_value = 0;
}
void CFrameCountHandler::InitFrameCountInfo(CApplication* app, unsigned int frameCount,
                                            unsigned short tick)
{
    if (frameCount != 0)
    {
        m_value = (unsigned int)app;
        memset(this, 0, 0x28);
        m_tick = frameCount;
        m_framePerTick = 100 / frameCount;
        return;
    }
    throw CDNFException("CFrameCountHandler::InitFrameCountInfo() Exception Break!");
}
CFrameCountHandler* CFrameCountHandler::GetFrameCountInfo()
{
    unsigned int l1 = 0;
    unsigned int l2 = 0;
    unsigned int l3 = 0;
    unsigned int l4 = 0;
    (void)l1;
    (void)l2;
    (void)l3;
    (void)l4;
    unsigned int now;
    struct tms t;
    m_state = 0;
    if (m_state0 == 0)
    {
        m_state0 = 1;
        m_frameCount = 0;
        now = times(&t);
        m_startTime = now;
        if ((int)m_startTime == -1)
        {
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
    }
    else
    {
        now = times(&t);
        m_endTime = now;
        if ((int)m_endTime == -1)
        {
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
        if (m_endTime < m_startTime)
        {
            m_startTime = m_endTime;
        }
        if (m_frameCount < (m_endTime - m_startTime) / m_framePerTick)
        {
            m_frameCount++;
            m_state = 1;
            if (99 < m_endTime - m_startTime)
            {
                m_fps = m_frameCount;
                m_state = 2;
                m_frameCount = 0;
                m_startTime = m_endTime - (m_endTime - m_startTime) + 100;
                *(unsigned int*)((char*)this + 0x20) = 0;
                m_counter1 = (char)(m_counter1 + 1);
                if (0x3b < (unsigned char)m_counter1)
                {
                    m_state = 3;
                    m_counter1 = 0;
                    m_counter2 = (char)(m_counter2 + 1);
                    if (0x3b < (unsigned char)m_counter2)
                    {
                        m_state = 4;
                        m_counter2 = 0;
                    }
                }
            }
        }
    }
    return this;
}
void CFrameCountHandler::SaveProcess()
{
    m_writeTick++;
    bool b = m_writeTick != 0;
    if (b)
    {
        DNF_LOG_SCOPE_LINE(0x9d, "./log/frame", "FPS(%02d) / DFC(%02d)\n", m_fps, m_tick);
        m_writeTick = 0;
    }
}
