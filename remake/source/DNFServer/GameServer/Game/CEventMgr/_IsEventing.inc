// ---- ORIG @0x0848ef0c ------------------------------------------------------
// begin.tm = {year-1900, mon-1, mday, hour}；end 同构；
// in_time 通过后：EVENT_WEEKEND 时再要求 localtime_r(now).tm_wday ∈ {0,6}。
bool CEventMgr::_IsEventing(int startYear, int startMon, int startDay, int startHour,
