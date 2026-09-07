#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#include "CSystemTime.h"
#include "GlobalData.h"

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

// ============================================================================
// OS_API::GetDateTimeTick（ORIG 0x858c802 T）
// 返回全局 CSystemTime（ORIG 0x941f714 = GlobalData::s_systemTime_）的
// getCurSec()，即当前 Unix 秒（已由 update 周期刷新）。
// 声明见 CInventory.h：namespace OS_API { int GetDateTimeTick(); }
// 注意：必须位于 getCurSec 定义之前——callee 未见定义时 4.4 帧 0x28/槽 -0xc
// 与 ORIG 一致；定义前置会使帧缩为 0x14（probe12/13 实证）。
// ============================================================================
namespace OS_API
{
int GetDateTimeTick()
{
    int tick = GlobalData::s_systemTime_.getCurSec();
    return tick;
}
}

int CSystemTime::getCurSec()
{
    return m_sec;
}

unsigned int CSystemTime::getCurTickCount()
{
    return m_msec;
}

// getCurDate：定义已移入 CSystemTime.h 内联（ORIG W 形，buf 随发）。

void CSystemTime::update()
{
    gettimeofday(&m_tv, 0);
    m_sec = m_tv.tv_sec;
    m_msec = (m_sec - m_lastSec) * 1000 + m_tv.tv_usec / 1000;
}

namespace OS_API
{
int snprintf(char* buf, unsigned int n, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int r = vsnprintf(buf, n, fmt, ap);
    va_end(ap);
    return r;
}
}
