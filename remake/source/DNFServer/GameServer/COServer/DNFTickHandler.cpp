// df_coserver_r — CFrameCountHandler（ORIG DNFTickHandler.cpp）
#include <stdio.h>
#include <string.h>
#include <sys/times.h>

#include "DNFTickHandler.h"
#include "DNFApplication.h"
#include "DNFFileLog.h"
#include "DNFTableBase.h"

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
        m_value = app;
        memset((char*)this, 0, 0x28);
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
    struct tms t;
    unsigned int l1 = 0;
    unsigned int l2 = 0;
    unsigned int l3 = 0;
    unsigned int q;
    unsigned int l4 = 0;
    (void)l1;
    (void)l2;
    (void)l3;
    (void)l4;
    m_state = 0;
    if (m_state0 == 0)
    {
        m_state0 = 1;
        m_frameCount = 0;
        if ((int)(m_startTime = times(&t)) == -1)
        {
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
    }
    else
    {
        if ((int)(m_endTime = times(&t)) == -1)
        {
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
        if (m_startTime > m_endTime)
        {
            m_startTime = m_endTime;
        }
        l1 = m_endTime - m_startTime;
        q = l1 / m_framePerTick;
        if (q > m_frameCount)
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
                m_unk20 = 0;
                if (0x3b < (unsigned char)++m_counter1)
                {
                    m_state = 3;
                    m_counter1 = 0;
                    if (0x3b < (unsigned char)++m_counter2)
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
    if (++m_writeTick != 0)
    {
        DNF_LOG_SCOPE_LINE(0xa8, "./log/frame", "FPS(%02d) / DFC(%02d)\n", m_fps, m_tick);
        m_writeTick = 0;
    }
}

void CFrameCountHandler::SaveProcess(int interval)
{
    if (++m_writeTick != 0)
    {
        DNF_LOG_SCOPE_LINE(0xb8, "./log/frame", "Thread(%2d) / FPS(%02d) / DFC(%02d)", interval, m_fps, m_tick);
        m_writeTick = 0;
    }
}
