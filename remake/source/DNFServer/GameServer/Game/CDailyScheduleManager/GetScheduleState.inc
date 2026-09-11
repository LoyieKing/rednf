// ORIG 0x81266e2：size() 缓存 + while 循环；命中条件 (m_idx==idx && type==m_type)
// 且 m_state==1 时返回其 state；未命中返回 0。
int CDailyScheduleManager::GetScheduleState(_DailyScheduleType type,
                                            unsigned int idx) const
{
    const unsigned int count = m_schedules.size();
    unsigned int i = 0;
    while (i < count)
    {
        if (m_schedules[i].m_idx == idx && type == m_schedules[i].m_type)
        {
            if (m_schedules[i].m_state == 1)
                return m_schedules[i].m_state;
        }
        ++i;
    }
    return 0;
}