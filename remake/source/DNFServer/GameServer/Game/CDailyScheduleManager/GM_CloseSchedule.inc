void CDailyScheduleManager::GM_CloseSchedule(int idx, _DailyScheduleType type,
                                             _DailySchedule& out)
{
    const unsigned int count = m_schedules.size();   // -0x14
    unsigned int i = 0;                              // -0x10
    while (i < count)
    {
        register long curIdx = idx;
        if (curIdx == m_schedules[i].m_idx &&
            m_schedules[i].m_typeE == type &&
            idx == out.m_idx)
        {
            if (m_schedules[i].m_state == 1)
                out.m_state = 1;                     // +0x20
            else
                out.m_state = 0;
            memcpy(&m_schedules[i], &out, sizeof(_DailySchedule));  // 0x24
            break;
        }
        ++i;
    }

    unsigned int j = 0;                              // -0xc
    while (j < count)
    {
        register long curIdx = idx;
        if (curIdx == m_schedules[j].m_idx &&
            m_schedules[j].m_type == idx &&
            idx == out.m_idx)
        {
            memcpy(&m_schedules[j], &out, sizeof(_DailySchedule));
        }
        ++j;
    }
