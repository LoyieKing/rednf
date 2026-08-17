// df_dbmw_r - PacketCounter (ORIG PacketCounter.cpp)
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

template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::CPacketCounter(char* dir, char* name)
{
    Reset();
    m_time = time(0);
    if (dir != 0)
    {
        sprintf(m_name, "./log/%s/%s", dir, name);
    }
    else
    {
        sprintf(m_name, "./log/%s", name);
    }
    m_flag = true;
}

template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::~CPacketCounter()
{
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::Reset()
{
    for (int i = 0; i < 0x2418; i++)
    {
        m_a1[i] = 0;
        m_a2[i] = 0;
        m_a3[i] = 0;
        m_b[i] = 0;
    }
    m0 = 0;
    m_tail = 0;
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::IncrementPacketCount(int id)
{
    if (id > 0x27ff)
        return;
    if (id <= 0x3e7)
        return;
    if (!m_flag)
    {
        if ((unsigned int)m_a1[id - 1000] >= 0xb)
            return;
    }
    m_a1[id - 1000]++;
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::BeforeProcess()
{
    m_a3[0] = m0;
    while (m_a3[0] == -1)
    {
        m_a3[0] = 0;
        break;
    }
}

// [DNF-NONIDENTICAL] DNF-DBM-DIFF-0081 | dbmw | 与ORIG差异=DIFF | CPacketCounter<1000, 10240>::AfterProcess | 详见 function_reports/dbmw/_ZN14CPacketCounterILi1000ELi10240EE12AfterProcessEi.md
template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::AfterProcess(int id)
{
    if (id > 0x27ff)
        return;
    if (id <= 0x3e7)
        return;
    if (!m_flag)
    {
        if ((unsigned int)m_a1[id - 1000] >= 0xb)
            return;
    }
    int prev = m0;
    if (prev == -1)
    {
        prev = 0;
    }
    else
    {
        int delta;
        if (m_flag != 0)
        {
            delta = prev - m_a3[0];
        }
        else
        {
            delta = prev - m_a3[id - 1000];
            m_a1[id - 1000]++;
            m_b[id - 1000] = 0;
        }
        m_a2[id - 1000] = m_a2[id - 1000] + delta;
    }
}

template class CPacketCounter<1000, 10240>;
