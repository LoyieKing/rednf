void CDailyScheduleManager::GM_OpenSchedule(int idx, _DailyScheduleType type,
                                            _DailySchedule& out)
{
    unsigned char start[3] = { 0, 0, 0 };            // -0x17..-0x15
    unsigned char end[3] = { 0x17, 0x32, 0x32 };     // -0x1a..-0x18

    const unsigned int count = m_schedules.size();   // -0x14
    unsigned int i = 0;                              // -0x10
    while (i < count)
    {
        if (idx == m_schedules[i].m_idx && m_schedules[i].m_typeE == type)
        {
            if (idx != out.m_idx)
            {
                memcpy(&out, &m_schedules[i], sizeof(_DailySchedule));  // 0x24
                out.m_state = 0;
            }
            m_schedules[i].m_wday = 7;               // +0x14
            memcpy(&m_schedules[i].m_startHour, start, 3);   // +0x18（0,0,0）
            memcpy(&m_schedules[i].m_endHour, end, 3);       // +0x1b（23,50,50）
            break;
        }
        ++i;
    }

    unsigned int j = 0;                              // -0xc
    while (j < count)
    {
        if (idx == m_schedules[j].m_idx && m_schedules[j].m_type == 1)
        {
            if (idx != out.m_idx)
            {
                memcpy(&out, &m_schedules[j], sizeof(_DailySchedule));
                out.m_state = 0;
            }
            m_schedules[j].m_wday = 7;
            memcpy(&m_schedules[j].m_startHour, start, 3);
            memcpy(&m_schedules[j].m_endHour, end, 3);
            break;
        }
        ++j;
    }
