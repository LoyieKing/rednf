// ORIG 0x8126dc0：找到 (m_idx==idx && m_type==type) 条目，把其拷贝到 out（+0x20 清 0），
// 并置该条目 m_wday=7、起始时分秒(0,0,0)、结束时(23,50,50)；随后第二遍处理 m_type==1
// （进行中）的同类条目（GM 强制开启）。
// [b28d-solo-CDailyScheduleManager] AE：m_type 经匿名 union 的 enum 视图 m_typeE 与形参比较，
// 复现 ORIG `mov 0x4(%eax),%eax; cmp 0x10(%ebp),%eax`（P254 enum 字段保留直载；int 字段 4.4
// 恒两段 `mov,%edx; mov,%eax; cmp`）；m_idx unsigned 化（P211 档③）令 idx 形参出 ebx 缓存形。
// p167 CLEAR。
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
}