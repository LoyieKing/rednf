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
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-DBM-DIFF-0112 已消除）
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
// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-DBM-DIFF-0111 已消除）
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
    m_field24 = 0;
    if (m_field0[0] == 0)
    {
        m_field0[0] = 1;
        m_field14 = 0;
        m_fieldC = times(&t);
        while ((int)m_fieldC == -1)
        {
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
    }
    else
    {
        m_field10 = times(&t);
        while ((int)m_field10 == -1)
        {
            throw CDNFException("CFrameCountHandler::GetFrameCountInfo() times() Exception Break!");
        }
        if (m_fieldC > (unsigned int)m_field10)
            m_fieldC = m_field10;
        l1 = m_field10 - m_fieldC;
        q = l1 / m_field8;
        if (m_field14 < q)
        {
            m_field14++;
            m_field24 = 1;
            l2 = m_field10 - m_fieldC;
            if (99 < l2)
            {
                m_field18 = m_field14;
                m_field24 = 2;
                m_field14 = 0;
                m_fieldC = m_field10 - l2 + 0x64;
                m_field20 = 0;
                if (0x3b < (unsigned char)(++m_field25))
                {
                    m_field24 = 3;
                    m_field25 = 0;
                    if (0x3b < (unsigned char)(++m_field26))
                    {
                        m_field24 = 4;
                        m_field26 = 0;
                    }
                }
            }
        }
    }
    return this;
}
