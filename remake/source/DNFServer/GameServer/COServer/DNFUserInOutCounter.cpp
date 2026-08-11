// df_coserver_r — CDNFUserInOutCounter（ORIG DNFUserInOutCounter.cpp）
#include <stdio.h>
#include <string.h>

#include "DNFUserInOutCounter.h"
#include "DNFFileLog.h"

static char USER_IN_OUT_WRITE_LOG_TIME = 0x3c;

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
    // ORIG 实测（0x804dc66 起）：函数入口零初始化以 rep stos 内联展开
    // （`char buf[4000] = {0};` 形态，4.4.6-3 精确复现；显式 memset 会退化为调用）。
    char buf[4000] = {0};
    for (int g = 0; g < 10; g++)
    {
        for (int c = 0; c < 0x40; c++)
        {
            if (m_counters[g][c][0] != 0 ||
                m_counters[g][c][1] != 0)
            {
                sprintf(buf, "%s G(%d)Ch(%d)In(%d)Out(%d) ", buf, g, c,
                        m_counters[g][c][0],
                        m_counters[g][c][1]);
            }
        }
        DNF_LOG_SCOPE_LINE(0x24, "./log/UserCounter", "%s", buf);
        memset(buf, 0, 4000);
    }
}

void CDNFUserInOutCounter::ProcessWrite()
{
    if (++m_tick > (unsigned char)USER_IN_OUT_WRITE_LOG_TIME)
    {
        WriteLog();
        Reset();
    }
}

void CDNFUserInOutCounter::IncUserIn(int group, int index)
{
    if (group < 0x65 && index < 0xff)
    {
        // ORIG 实测（0x8051e0e 起）：单次求址 + 自增形态，
        // ++ 前缀式可复现（避免分离的读/写两次地址计算）。
        ++m_counters[group][index][0];
        return;
    }
    return;
}

void CDNFUserInOutCounter::IncUserOut(int group, int index)
{
    if (group < 0x65 && index < 0xff)
    {
        ++m_counters[group][index][1];
        return;
    }
    return;
}
