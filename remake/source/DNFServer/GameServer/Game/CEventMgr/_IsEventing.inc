bool CEventMgr::_IsEventing(int startYear, int startMon, int startDay, int startHour,
                            int endYear, int endMon, int endDay, int endHour,
                            EVENT_TYPE type)
{
    struct tm endTm;
    struct tm begin;
    memset(&endTm, 0, sizeof(struct tm));   // ORIG memset 0x2c == sizeof(tm)@i386
    memset(&begin, 0, sizeof(struct tm));

    begin.tm_year = startYear - 1900;       // ORIG sub $0x76c
    begin.tm_mon  = startMon - 1;
    begin.tm_mday = startDay;
    begin.tm_hour = startHour;

    endTm.tm_year = endYear - 1900;
    endTm.tm_mon  = endMon - 1;
    endTm.tm_mday = endDay;
    endTm.tm_hour = endHour;

    // ORIG Ghidra 形态：cVar1 = in_time(&begin,&end); if (cVar1) {
    //   if (type != EVENT_WEEKEND) return 1;
    //   local_14 = CSystemTime::getCurSec(&GlobalData::s_systemTime_);
    //   ptVar2 = localtime_r(&local_14,&local_98);
    //   if (ptVar2->tm_wday==0 || ptVar2->tm_wday==6) return 1; }
    // return 0;
    if (in_time(&begin, &endTm))
    {
        if (type == EVENT_WEEKEND)
        {
            time_t now = (time_t)GlobalData::s_systemTime_.getCurSec();
            struct tm cur;
            struct tm* pcur = localtime_r(&now, &cur);
            if ((pcur->tm_wday == 0) || (pcur->tm_wday == 6))
                return true;
        }
        else
        {
            return true;
        }
    }
    return false;
