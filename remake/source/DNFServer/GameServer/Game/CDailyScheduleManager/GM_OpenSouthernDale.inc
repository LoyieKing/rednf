// ---- SouthernDale（GM_OpenSouthernDale 0x81267bc / GM_CloseSouthernDale 0x8126ad2）----
// 打开：第一目标 (m_idx==100 && m_type==0) 回填 this+0x0（m_southernDaleA），
// 第二目标 (m_idx==0x5045 && m_type==1) 回填 m_southernDale(+0x24)；
// 各自把条目 m_wday 置 7，起始 h/m/s = 当前时刻，结束 h/m/s = 当前+600s；
// 关闭：把两目标各自写回 m_schedules 中匹配条目。
void CDailyScheduleManager::GM_OpenSouthernDale()
{
    unsigned int count;                              // -0x14
    unsigned int i;                                  // -0x10
    unsigned int j;                                  // -0xc
    int target[2];                                   // -0x1c/-0x18（死局部，帧对齐）
    std::time_t cur;                                 // -0x20
    std::time_t endT;                                // -0x24
    unsigned char start[3];                          // -0x27..-0x25
    unsigned char end[3];                            // -0x2a..-0x28
    std::tm tmStart;                                 // -0x58
    std::tm tmEnd;                                   // -0x84

    target[0] = 100;
    target[1] = 0x5045;
    cur = GlobalData::s_systemTime_.getCurSec();
    localtime_r(&cur, &tmStart);
    endT = cur + 600;
    localtime_r(&endT, &tmEnd);

    start[0] = (unsigned char)tmStart.tm_hour;
    start[1] = (unsigned char)tmStart.tm_min;
    start[2] = (unsigned char)tmStart.tm_sec;
    end[0] = (unsigned char)tmEnd.tm_hour;
    end[1] = (unsigned char)tmEnd.tm_min;
    end[2] = (unsigned char)tmEnd.tm_sec;

    count = m_schedules.size();
    i = 0;
    while (i < count)
    {
        if (m_schedules[i].m_idx == 100 && m_schedules[i].m_type == 0)
        {
            if (m_southernDaleA.m_idx != 100)
            {
                memcpy(&m_southernDaleA, &m_schedules[i], sizeof(_DailySchedule));  // 0x24
                m_southernDaleA.m_state = 0;
            }
            m_schedules[i].m_wday = 7;               // +0x14
            memcpy(&m_schedules[i].m_startHour, start, 3);
            memcpy(&m_schedules[i].m_endHour, end, 3);
            break;
        }
        ++i;
    }

    j = 0;
    while (j < count)
    {
        if (m_schedules[j].m_idx == 0x5045 && m_schedules[j].m_type == 1)
        {
            if (m_southernDale.m_idx != 0x5045)
            {
                memcpy(&m_southernDale, &m_schedules[j], sizeof(_DailySchedule));
                m_southernDale.m_state = 0;
            }
            m_schedules[j].m_wday = 7;
            memcpy(&m_schedules[j].m_startHour, start, 3);
            memcpy(&m_schedules[j].m_endHour, end, 3);
            break;
        }
        ++j;
    }
}