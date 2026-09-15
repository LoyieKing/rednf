// df_monitor_r — DNFTickHandler（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFTickHandler.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFApplication.h"
#include "DNFTableBase.h"

CFrameCountHandler::CFrameCountHandler()
{
    m_logCounter = 0;
    m_app = 0;
}


void CFrameCountHandler::InitFrameCountInfo(CApplication* app, unsigned int frameCount,
                                            unsigned short tick)
{
    if (frameCount != 0)
    {
        m_app = app;
        memset((char*)this, 0, 0x28);
        m_frameCount = frameCount;
        m_interval = 100 / frameCount;
    }
    else
    {
        throw CDNFException("CFrameCountHandler::InitFrameCountInfo() Exception Break!");
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0116 已消除）
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
    tms tm;
    m_state = 0;
    if (m_inited == 0)
    {
        m_inited = 1;
        m_frameCounter = 0;
        m_prevTick = times(&tm);
        while ((int)m_prevTick == -1)
        {
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
    }
    else
    {
        m_curTick = times(&tm);
        while ((int)m_curTick == -1)
        {
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
        if ((unsigned int)m_prevTick > (unsigned int)m_curTick)
        {
            m_prevTick = m_curTick;
        }
        l1 = (unsigned int)m_curTick - (unsigned int)m_prevTick;
        q = l1 / (unsigned int)m_interval;
        if (m_frameCounter < q)
        {
            m_frameCounter++;
            m_state = 1;
            l2 = (unsigned int)m_curTick - (unsigned int)m_prevTick;
            if (99 < l2)
            {
                m_fps = m_frameCounter;
                m_state = 2;
                m_frameCounter = 0;
                m_prevTick = m_curTick - l2 + 100;
                m_field20 = 0;  // 保留原名（未使用）
                if (0x3b < (unsigned char)(++m_secCounter))
                {
                    m_state = 3;
                    m_secCounter = 0;
                    if (0x3b < (unsigned char)(++m_minCounter))
                    {
                        m_state = 4;
                        m_minCounter = 0;
                    }
                }
            }
        }
    }
    return this;
}

void CFrameCountHandler::SaveProcess()
{
    if ((m_logCounter = m_logCounter + 1) != 0)
    {
        DNF_LOG_SCOPE_LINE(0xa8, "./log/frame", "FPS(%02d) / DFC(%02d)\n", m_fps, m_frameCount);
        m_logCounter = 0;
    }
}

void CFrameCountHandler::SaveProcess(int threadNo)
{
    if ((m_logCounter = m_logCounter + 1) != 0)
    {
        DNF_LOG_SCOPE_LINE(0xb8,"./log/frame", "Thread(%2d) / FPS(%02d) / DFC(%02d)", threadNo, m_fps,
            m_frameCount);
        m_logCounter = 0;
    }
}
