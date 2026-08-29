// df_game_r ORIG 还原 —— CUnixTimer（Game 计时族）
// 参照 objdump：GetNowTime 0x085ed776 / SetLastTime 0x085ed7ae /
// GetTimeInterval 0x085ed7c8 / CUnixTimer::CUnixTimer 0x085ed7f2
// （基类 CDnFTimer::CDnFTimer 0x085ed7e4 仅设 vptr，隐式生成即可对齐）。
#include "CUnixTimer.h"

#include <sys/time.h>

CUnixTimer::CUnixTimer()
{
    // ORIG：call CDnFTimer::CDnFTimer()（设基类 vptr）+ 覆盖本类 vptr；体空。
}

double CUnixTimer::GetNowTime()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void CUnixTimer::SetLastTime()
{
    m_lastTime = GetNowTime();
}

double CUnixTimer::GetTimeInterval()
{
    return GetNowTime() - m_lastTime;
}
