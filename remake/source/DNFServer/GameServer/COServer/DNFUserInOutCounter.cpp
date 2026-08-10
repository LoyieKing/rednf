// df_coserver_r — CDNFUserInOutCounter（ORIG DNFUserInOutCounter.cpp）
#include <stdio.h>
#include <string.h>

#include "DNFUserInOutCounter.h"
#include "DNFFileLog.h"

char USER_IN_OUT_WRITE_LOG_TIME = 0x3c;

CDNFUserInOutCounter::CDNFUserInOutCounter()
{
    Reset();
}

CDNFUserInOutCounter::~CDNFUserInOutCounter()
{
    Reset();
}

void CDNFUserInOutCounter::Reset()
{
    memset(m_counters, 0, 0x1926c);
    m_tick = 0;
}

void CDNFUserInOutCounter::WriteLog()
{
    char buf[4000];
    memset(buf, 0, sizeof(buf));
    for (int g = 0; g < 10; g++)
    {
        for (int c = 0; c < 0x40; c++)
        {
            if (*(int*)(m_counters + (g * 0xff + c) * 8) != 0 ||
                *(int*)(m_counters + ((g * 0xff + c) * 2 + 1) * 4) != 0)
            {
                sprintf(buf, "%s G(%d)Ch(%d)In(%d)Out(%d) ", buf, g, c,
                        *(int*)(m_counters + (g * 0xff + c) * 8),
                        *(int*)(m_counters + ((g * 0xff + c) * 2 + 1) * 4));
            }
        }
        DNF_LOG_SCOPE_LINE(0x24, "./log/UserCounter", "%s", buf);
        memset(buf, 0, 4000);
    }
}

void CDNFUserInOutCounter::ProcessWrite()
{
    m_tick = (char)(m_tick + 1);
    if ((unsigned char)m_tick > (unsigned char)USER_IN_OUT_WRITE_LOG_TIME)
    {
        WriteLog();
        Reset();
    }
}

void CDNFUserInOutCounter::IncUserIn(int group, int index)
{
    if (group < 0x65 && index < 0xff)
    {
        *(int*)(m_counters + (group * 0xff + index) * 8) =
            *(int*)(m_counters + (group * 0xff + index) * 8) + 1;
    }
}

void CDNFUserInOutCounter::IncUserOut(int group, int index)
{
    if (group < 0x65 && index < 0xff)
    {
        *(int*)(m_counters + ((group * 0xff + index) * 2 + 1) * 4) =
            *(int*)(m_counters + ((group * 0xff + index) * 2 + 1) * 4) + 1;
    }
}
