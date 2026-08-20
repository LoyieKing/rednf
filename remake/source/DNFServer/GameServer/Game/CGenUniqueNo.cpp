#include "CGenUniqueNo.h"
#include "CSystemTime.h"
#include "GlobalData.h"
#include "CEnvironment.h"

#include <stdio.h>
#include <time.h>

namespace WongWork
{
unsigned short CGenUniqueNo::m_nTimeCount = 0;

CGenUniqueNo::CGenUniqueNo()
{
}

void CGenUniqueNo::_updateTimeCount()
{
    ++m_nTimeCount;
    if (m_nTimeCount > 49999)
        m_nTimeCount = 1;
}

char* CGenUniqueNo::GenUniqueNo(char* out)
{
    _updateTimeCount();
    CEnvironment* env = G_CEnvironment();
    snprintf(out, 0x14, "%02d%02d%10d%05d",
             env->m_serverEnvir.m_channelNo,
             env->m_serverEnvir.m_processSequence,
             GlobalData::s_systemTime_.getCurSec(),
             (unsigned int)m_nTimeCount);
    return out;
}

static const char typeConst[] = "CLQBVFOE";

char* CGenUniqueNo::genIPGNo(ENUM_IPGNO_TYPE type, unsigned int no, char* out)
{
    time_t curSec = (time_t)GlobalData::s_systemTime_.getCurSec();
    struct tm local_tm;
    struct tm* cur = localtime_r(&curSec, &local_tm);
    CEnvironment* env = G_CEnvironment();
    int year = cur->tm_year + 1900;
    snprintf(out, 0x14, "%c%02d%02d%02d%02d%010d",
             (int)(signed char)typeConst[(unsigned int)type],
             env->m_serverEnvir.m_channelNo,
             year % 2000, cur->tm_mon + 1, cur->tm_mday, no);
    return out;
}
}
