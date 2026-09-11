void CDailyScheduleManager::GM_CloseSouthernDale()
{
    int target[2];                                   // -0x1c/-0x18（死局部，帧对齐）
    unsigned int count;                              // -0x14
    unsigned int i;                                  // -0x10
    unsigned int j;                                  // -0xc

    target[0] = 100;
    target[1] = 0x5045;
    count = m_schedules.size();
    i = 0;
    while (i < count)
    {
        if (m_schedules[i].m_idx == 100 && m_schedules[i].m_type == 0
            && m_southernDaleA.m_idx == 100)
        {
            memcpy(&m_schedules[i], &m_southernDaleA, sizeof(_DailySchedule));
        }
        ++i;
    }

    j = 0;
    while (j < count)
    {
        if (m_schedules[j].m_idx == 0x5045 && m_schedules[j].m_type == 1
            && m_southernDale.m_idx == 0x5045)
        {
            memcpy(&m_schedules[j], &m_southernDale, sizeof(_DailySchedule));
        }
        ++j;
    }
}