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
    // [b27] G_CEnvironment() 两次内联调用（P48）；参数序 (channelNo,
    // serverGroup, sec, m_nTimeCount)（ORIG esp 槽实证）；getCurSec 预语句
    // 落槽。已 31=31 全条数。
    // [b27 封顶] 残差 = m_nTimeCount 读位（ORIG 延后至 env#1 后，4.4 右→左
    // 恒最先，P75 不可定向）+ sec/t 槽号/寄存器（esi↔ebx）跨版本分配差。
    int sec = GlobalData::s_systemTime_.getCurSec();
    snprintf(out, 0x14, "%02d%02d%10d%05d",
             G_CEnvironment()->m_serverEnvir.m_channelNo,
             G_CEnvironment()->m_serverEnvir.m_serverGroup,
             sec, (unsigned int)m_nTimeCount);
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
