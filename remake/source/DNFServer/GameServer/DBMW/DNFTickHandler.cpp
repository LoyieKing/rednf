// df_dbmw_r - DNFTickHandler (ORIG DNFTickHandler.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

CFrameCountHandler::CFrameCountHandler()
{
    m_field28 = 0;
    m_app = 0;
}
void CFrameCountHandler::SaveProcess()
{
    if (++m_field28)
    {
        DNF_LOG_SCOPE_LINE(0xa8, "./log/frame", "FPS(%02d) / DFC(%02d)\n", m_field18, m_field4);
        m_field28 = 0;
    }
}
void CFrameCountHandler::SaveProcess(int n)
{
    if (++m_field28)
    {
        DNF_LOG_SCOPE_LINE(0xb8, "./log/frame", "Thread(%2d) / FPS(%02d) / DFC(%02d)", n, m_field18, m_field4);
        m_field28 = 0;
    }
}
void CFrameCountHandler::InitFrameCountInfo(CApplication* app, unsigned int a, unsigned short b)
{
    if (a == 0)
        goto InitFrameCountInfo_throw;
    m_app = app;
    memset((char*)this, 0, 0x28);
    m_field4 = a;
    m_field8 = 100 / a;
    return;
InitFrameCountInfo_throw:
    throw CDNFException("CFrameCountHandler::InitFrameCountInfo() Exception Break!");
}
void* CFrameCountHandler::GetFrameCountInfo()
{
    clock_t dummy0 = 0;
    clock_t dummy1 = 0;
    clock_t dummy2 = 0;
    clock_t dummy3 = 0;
    (void)dummy0;
    (void)dummy1;
    (void)dummy2;
    (void)dummy3;
    struct tms tmsbuf;
    bool fail;
    m_field24 = 0;
    if (!m_field0[0])
    {
        m_field0[0] = 1;
        m_field14 = 0;
        m_fieldC = times(&tmsbuf);
        fail = (m_fieldC == (unsigned int)-1);
        if (fail)
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        return this;
    }
    m_field10 = times(&tmsbuf);
    fail = (m_field10 == -1);
    if (fail)
        throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
    if (m_fieldC > (unsigned int)m_field10)
        m_fieldC = m_field10;
    unsigned int diff = m_field10 - m_fieldC;
    unsigned int frame = diff / m_field8;
    if (m_field14 < frame)
    {
        m_field14++;
        m_field24 = 1;
    }
    if (diff > 0x63)
    {
        m_field18 = m_field14;
        m_field24 = 2;
        m_field14 = 0;
        m_fieldC = m_field10 - diff + 0x64;
        m_field20 = 0;
        m_field25++;
        if (m_field25 > 0x3b)
        {
            m_field24 = 3;
            m_field25 = 0;
            m_field26++;
            if (m_field26 > 0x3b)
            {
                m_field24 = 4;
                m_field26 = 0;
            }
        }
    }
    return this;
}
