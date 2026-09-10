int CCoinRefillEvent::getLastEventTime() const
{
    // [b28d-59] ORIG 0x0832c0b8：改用 m_slots[i] 单数组视角（P235 union 双视角）
    // 复现 ORIG 的 `mov 0xc(%eax,%edx,4)` 折叠形；逻辑槽 i 语义与
    // ORIG `this+i*4+0xc` 逐条一致（旧 b28c-35 真语义 bug 修正保持不变）。
    int curSec = GlobalData::s_systemTime_.getCurSec();
    tm cur;
    localtime_r(reinterpret_cast<time_t*>(&curSec), &cur);
    if (cur.tm_hour < m_slots[1])
    {
        return m_slots[m_timeCount];
    }
    for (int i = 2; i <= m_timeCount; i = i + 1)
    {
        if (cur.tm_hour < m_slots[i])
        {
            return m_slots[i - 1];
        }
    }
    return m_slots[m_timeCount];
