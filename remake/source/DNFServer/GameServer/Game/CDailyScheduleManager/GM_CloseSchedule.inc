// ORIG 0x812707c：找到 (m_idx==idx && m_type==type && idx==out.m_idx) 条目，按其当前
// m_state 写 out.state（进行中则 1，否则 0），memcpy 回条目；第二遍对
// (m_idx==idx && m_type==idx && idx==out.m_idx) 条目做 memcpy（ORIG 第二遍用 idx 比较
// m_type，历史行为照抄）。
// [b28d-solo-CDailyScheduleManager] AE：循环体内 `register long curIdx = idx;`（P231 纯
// register，每轮重载 %ebx，与 ORIG 装载点同层；long 与 unsigned m_idx 异型使 4.4 出
// `cmp %ebx,%eax`）+ m_typeE 视图 + 第三处比较回退形参 idx（复现 ORIG `mov 0xc(%ebp),%edx`）。
// p167 CLEAR。
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
}