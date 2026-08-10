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
    *(time_t*)(m_data + 4) = time(0);
    if (dir == 0)
    {
        sprintf(m_data + 0x1d540, "./log/%s", name);
    }
    else
    {
        sprintf(m_data + 0x1d540, "./log/%s/%s", dir, name);
    }
    *(unsigned char*)(m_data + 0x1d640) = 1;
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
        *(int*)(m_data + i * 4 + 8) = 0;
        *(int*)(m_data + (i + 0x5138) * 4) = 0;
        *(int*)(m_data + (i + 0x2418) * 4 + 8) = 0;
        m_data[i + 0x120c8] = 0;
    }
    *(int*)m_data = 0;
    m_data[0x1d641] = 0;
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::IncrementPacketCount(int id)
{
    if (id < 0x2800 && 999 < id &&
        (m_data[0x1d640] == 1 ||
         *(unsigned int*)(m_data + 8 + (id - 1000) * 4) < 0xb))
    {
        *(int*)(m_data + 8 + (id - 1000) * 4) =
            *(int*)(m_data + 8 + (id - 1000) * 4) + 1;
    }
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::BeforeProcess()
{
    *(int*)(m_data + 0x9068) = *(int*)m_data;
    if (*(int*)(m_data + 0x9068) == -1)
        *(int*)(m_data + 0x9068) = 0;
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::AfterProcess(int id)
{
    if (id < 0x2800 && 999 < id &&
        (m_data[0x1d640] == 1 ||
         *(unsigned int*)(m_data + 8 + (id - 1000) * 4) < 0xb) &&
        *(int*)m_data != -1)
    {
        int prev;
        if (m_data[0x1d640] == 0)
        {
            prev = *(int*)(m_data + 0x9068 + (id - 1000) * 4);
            *(int*)(m_data + 8 + (id - 1000) * 4) += 1;
            m_data[0x11ce0 + id] = 0;
        }
        else
        {
            prev = *(int*)(m_data + 0x9068);
        }
        int diff = *(int*)m_data - prev;
        *(int*)(m_data + (id + 0x4d50) * 4) += diff;
    }
}

template class CPacketCounter<1000, 10240>;
