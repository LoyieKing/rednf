#include <time.h>
#include <stdio.h>

#include "CSystemTime.h"

// 构造：仅记录 m_lastSec 与毫秒（m_sec 留待 update 首次填充）
CSystemTime::CSystemTime()
{
    gettimeofday(&m_tv, 0);
    m_lastSec = m_tv.tv_sec;
    m_msec = m_tv.tv_usec / 1000;
}

CSystemTime::~CSystemTime()
{
}

int CSystemTime::getCurSec()
{
    return m_sec;
}

unsigned int CSystemTime::getCurTickCount()
{
    return m_msec;
}

char* CSystemTime::getCurDate()
{
    static __thread char buf[0x100];
    struct tm t;
    struct tm* p = localtime_r((const time_t*)&m_sec, &t);

    sprintf(buf, "%04d-%02d-%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday);
    return buf;
}

void CSystemTime::update()
{
    gettimeofday(&m_tv, 0);
    m_sec = m_tv.tv_sec;
    m_msec = (m_sec - m_lastSec) * 1000 + m_tv.tv_usec / 1000;
}
