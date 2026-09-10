bool CDailyScheduleManager::CheckContinueSchedule(const _DailySchedule& entry,
                                                  _DailySchedule** prev,
                                                  _DailySchedule** next)
{
    std::tm tm;        // -0x5c
    std::tm tmStart;   // -0x88（= tm 拷贝，写入 entry 起始 h/m/s 后 mktime-1s）
    std::tm tmEnd;     // -0xb4（= tm 拷贝，写入 entry 结束 h/m/s 后 mktime+1s）
    std::tm tmNow2;    // -0xe0（isNext 分支用）
    std::tm tmNow3;    // -0x10c（isNext 分支用）
    std::time_t cur;   // -0x28
    std::time_t startT;// -0x2c
    std::time_t endT;  // -0x30
    int wday;          // -0x24
    unsigned int count;// -0x20
    unsigned int i;    // -0x1c
    bool isPrev;       // -0x16
    bool isNext;       // -0x15
    unsigned int wday2;// -0x14
    std::time_t s;     // -0x10
    std::time_t e;     // -0xc

    cur = GlobalData::s_systemTime_.getCurSec();
    localtime_r(&cur, &tm);
    wday = tm.tm_wday;
    memcpy(&tmStart, &tm, sizeof(std::tm));   // 0x2c
    memcpy(&tmEnd, &tm, sizeof(std::tm));     // 0x2c

    tmStart.tm_hour = entry.m_startHour;
    tmStart.tm_min = entry.m_startMin;
    tmStart.tm_sec = entry.m_startSec2;
    startT = mktime(&tmStart);
    startT -= 1;
    localtime_r(&startT, &tmStart);

    tmEnd.tm_hour = entry.m_endHour;
    tmEnd.tm_min = entry.m_endMin;
    tmEnd.tm_sec = entry.m_endSec2;
    endT = mktime(&tmEnd);
    endT += 1;
    localtime_r(&endT, &tmEnd);

    count = m_schedules.size();
    i = 0;
    for (; i < count; ++i)
    {
        if (entry.m_idx != m_schedules[i].m_idx ||
            entry.m_typeE != m_schedules[i].m_typeE) continue;
        if (&m_schedules[i] == &entry) continue;

        isPrev = false;
        isNext = false;

        if (entry.m_state == 0)
        {
            // 本条起点 == 输入终点+1s 的分解 → 后一条。
            if (tmEnd.tm_hour == m_schedules[i].m_startHour &&
                tmEnd.tm_min == m_schedules[i].m_startMin &&
                tmEnd.tm_sec == m_schedules[i].m_startSec2)
                isNext = true;
        }
        else if (entry.m_state == 1)
        {
            // 本条终点 == 输入起点-1s 的分解 → 前一条。
            if (tmStart.tm_hour == m_schedules[i].m_endHour &&
                tmStart.tm_min == m_schedules[i].m_endMin &&
                tmStart.tm_sec == m_schedules[i].m_endSec2)
                isPrev = true;
        }

        if (isNext)
        {
            // 后一条：仅当其在当前时间窗内被活动。
            localtime_r(&cur, &tmNow2);
            localtime_r(&cur, &tmNow3);
            wday2 = tmNow2.tm_wday;
            if (m_schedules[i].m_wday == wday2 || m_schedules[i].m_wday == 7)
            {
                tmNow3.tm_hour = m_schedules[i].m_startHour;
                tmNow3.tm_min = m_schedules[i].m_startMin;
                tmNow3.tm_sec = m_schedules[i].m_startSec2;
                s = mktime(&tmNow3);
                tmNow3.tm_hour = m_schedules[i].m_endHour;
                tmNow3.tm_min = m_schedules[i].m_endMin;
                tmNow3.tm_sec = m_schedules[i].m_endSec2;
                e = mktime(&tmNow3);
                if (cur >= s && cur <= e)
                    *next = &m_schedules[i];
            }
        }
        else
        {
            if (isPrev)
                *prev = &m_schedules[i];
        }
    }

    return (*next != 0 || *prev != 0);
