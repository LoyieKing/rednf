int CCoinRefillEvent::getNextEventTime(tm const& cur) const
{
    // [b28d-59] ORIG 0x0832c148：m_slots[i] 单数组视角（mov 0xc(%eax,%edx,4)）；
    // 独立 continue 守卫（P133：不得合并 && 短路）——第一循环 `== fatigueTime
    // continue`（sete/jne 形），第二循环 `!= fatigueTime return`（setne/je 形）。
    // 语义：找第一个 != m_fatigueTime 且 > cur.tm_hour 的槽；否则跳过
    // == m_fatigueTime 槽；全失败返回基准槽 m_slots[1]（= m_times[0]）。
    for (int i = 1; i <= m_timeCount; i = i + 1)
    {
        if (m_slots[i] == G_CEnvironment()->m_serverEnvir.m_fatigueTime)
        {
            continue;
        }
        if (cur.tm_hour < m_slots[i])
        {
            return m_slots[i];
        }
    }
    for (int i = 1; i <= m_timeCount; i = i + 1)
    {
        if (m_slots[i] != G_CEnvironment()->m_serverEnvir.m_fatigueTime)
        {
            return m_slots[i];
        }
    }
    return m_slots[1];
}