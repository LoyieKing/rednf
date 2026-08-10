// df_coserver_r — CommonTime/CSystemTime/CSystemTimeHandler（ORIG SystemTimeHandler.cpp）
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "SystemTimeHandler.h"

void CommonTime::SetCurTime()
{
    time_t t;
    time(&t);
    tm* pt = localtime(&t);
    m_time[0] = (char)((char)pt->tm_year - 100);
    m_time[1] = (char)((char)pt->tm_mon + 1);
    m_time[2] = (char)pt->tm_mday;
    m_time[3] = (char)pt->tm_hour;
    m_time[4] = (char)pt->tm_min;
    m_time[5] = (char)pt->tm_wday;
}

CSystemTime::CSystemTime()
{
    gettimeofday(&m_tv, 0);
    m_sec = m_tv.tv_sec;
    m_msec = m_tv.tv_usec / 1000;
}

CSystemTime::~CSystemTime()
{
}

CSystemTimeHandler::CSystemTimeHandler()
{
}

CSystemTimeHandler::~CSystemTimeHandler()
{
}

CSystemTimeHandler* CSystemTimeHandlerInstance()
{
    static CSystemTimeHandler instance;
    return &instance;
}
