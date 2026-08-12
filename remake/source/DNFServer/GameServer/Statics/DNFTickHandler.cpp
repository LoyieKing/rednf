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
    m_pApp = 0;
}
void CFrameCountHandler::InitFrameCountInfo(CApplication* app, unsigned int frameCount,
                                            unsigned short tick)
{
    if (frameCount != 0)
    {
        m_pApp = app;
        memset(&m_state0, 0, 0x28);
        m_tick = frameCount;
        m_framePerTick = 100 / frameCount;
    }
    else
    {
        throw CDNFException("CFrameCountHandler::InitFrameCountInfo() Exception Break!");
    }
}
CFrameCountHandler* CFrameCountHandler::GetFrameCountInfo()
{
    unsigned int l1 = 0;
    unsigned int l2 = 0;
    unsigned int l3 = 0;
    unsigned int q;
    unsigned int l4 = 0;
    (void)l1;
    (void)l2;
    (void)l3;
    (void)q;
    (void)l4;
    struct tms t;
    m_state = 0;
    if (m_state0 == 0)
    {
        m_state0 = 1;
        m_frameCount = 0;
        m_startTime = times(&t);
        while ((int)m_startTime == -1)
        {
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
    }
    else
    {
        m_endTime = times(&t);
        while ((int)m_endTime == -1)
        {
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
        if (m_startTime > m_endTime)
        {
            m_startTime = m_endTime;
        }
        l1 = m_endTime - m_startTime;
        q = l1 / m_framePerTick;
        if (m_frameCount < q)
        {
            m_frameCount++;
            m_state = 1;
            l2 = m_endTime - m_startTime;
            if (99 < l2)
            {
                m_fps = m_frameCount;
                m_state = 2;
                m_frameCount = 0;
                m_startTime = m_endTime - l2 + 100;
                m_pad20 = 0;
                if (0x3b < (unsigned char)(++m_counter1))
                {
                    m_state = 3;
                    m_counter1 = 0;
                }
            }
        }
    }
    return this;
}
void CFrameCountHandler::SaveProcess()
{
    m_writeTick++;
    while (m_writeTick != 0)
    {
        DNF_LOG_SCOPE_LINE(0x9d, "./log/frame", "FPS(%02d) / DFC(%02d)\n", m_fps, m_tick);
        m_writeTick = 0;
        break;
    }
}
