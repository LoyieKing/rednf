// ============================================================================
// WongWork::CEventMgr 实现 —— 严格转写 ORIG df_game_r：
//   _IsEventing            0x0848ef0c（docs/class_func_reports/WongWork__CEventMgr/_IsEventing.md）
//   GetPremiumServiceBonus 0x0848f078（同目录 GetPremiumServiceBonus.md）
//   in_time                0x086b1d86（ORIG objdump，全局自由函数）
// ============================================================================
#include "CEventMgr.h"

#include <string.h>
#include <time.h>

#include "GlobalData.h" // GlobalData::s_systemTime_（CSystemTime::getCurSec 0x080cbc9e）

// ---- ORIG @0x086b1d86 _Z7in_timeP2tmS0_ ------------------------------------
// t1 = mktime(begin); t2 = mktime(end); now = time(NULL);
// return now >= t1 && now < t2;
bool in_time(struct tm* begin, struct tm* end)
{
    time_t t1 = mktime(begin);
    time_t t2 = mktime(end);
    time_t now = time(NULL);
    return (now >= t1 && now < t2);
}

namespace WongWork
{

// ---- ORIG @0x0848ef0c ------------------------------------------------------
// begin.tm = {year-1900, mon-1, mday, hour}；end 同构；
// in_time 通过后：EVENT_WEEKEND 时再要求 localtime_r(now).tm_wday ∈ {0,6}。
bool CEventMgr::_IsEventing(int startYear, int startMon, int startDay, int startHour,
                            int endYear, int endMon, int endDay, int endHour,
                            EVENT_TYPE type)
{
    struct tm begin;
    struct tm endTm;
    memset(&begin, 0, sizeof(struct tm));   // ORIG memset 0x2c == sizeof(tm)@i386
    memset(&endTm, 0, sizeof(struct tm));

    begin.tm_year = startYear - 1900;       // ORIG sub $0x76c
    begin.tm_mon  = startMon - 1;
    begin.tm_mday = startDay;
    begin.tm_hour = startHour;

    endTm.tm_year = endYear - 1900;
    endTm.tm_mon  = endMon - 1;
    endTm.tm_mday = endDay;
    endTm.tm_hour = endHour;

    if (!in_time(&begin, &endTm))
        return false;

    if (type != EVENT_WEEKEND)
        return true;

    // ORIG: local_14 = CSystemTime::getCurSec(&GlobalData::s_systemTime_);
    //       localtime_r → tm_wday==0(Sun) || tm_wday==6(Sat) → true
    time_t now = (time_t)GlobalData::s_systemTime_.getCurSec();
    struct tm cur;
    localtime_r(&now, &cur);                 // ORIG 直接解引用返回指针，不判空
    return cur.tm_wday == 0 || cur.tm_wday == 6;
}

// ---- ORIG @0x0848f078 ------------------------------------------------------
// GetPremiumServiceBonus: _IsEventing(2006,1,2,0, 2006,1,11,0, EVENT_PLAIN) ? 7 : 0
int CEventMgr::GetPremiumServiceBonus()
{
    if (_IsEventing(2006, 1, 2, 0,
                    2006, 1, 11, 0,
                    EVENT_PLAIN))
    {
        return 7;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// ORIG T 0x0848f140 _ZN8WongWork9CEventMgr22IsChildrensDayEventingEv：
// 逐指令转写 —— return _IsEventing(0x7d6,5,5,6, 0x7d6,5,8,6, EVENT_PLAIN);
// （ORIG 栈 movl 常量序：(esp)=0x7d6,(4)=5,(8)=5,(c)=6,(10)=0x7d6,
//   (14)=5,(18)=8,(1c)=6,(20)=EVENT_TYPE(0)）
// ---------------------------------------------------------------------------
bool CEventMgr::IsChildrensDayEventing()
{
    if (_IsEventing(0x7d6, 5, 5, 6,
                    0x7d6, 5, 8, 6,
                    EVENT_PLAIN))
    {
        return true;
    }
    return false;
}

} // namespace WongWork
