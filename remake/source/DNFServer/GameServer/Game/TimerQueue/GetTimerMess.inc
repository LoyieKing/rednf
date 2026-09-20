bool TimerQueue::GetTimerMess(TimerEntry& entry) throw()
{
    unsigned int cur = GlobalData::s_systemTime_.getCurTickCount();

    Guard<Mutex> guard(&m_mutex);
    if (m_queue.empty())
    {
        return false;
    }

    entry = m_queue.top();
    // ORIG 0x08630f4a..0x08630f66：hi 有符号 vs 0（jg 未到期 / jl 到期），
    // hi==0 时 lo 无符号 vs cur（ja 未到期）。cur 高 32 位恒 0。
    if (entry.words.hi > 0)
        return false;
    if (entry.words.hi == 0 && entry.words.lo > cur)
        return false;

    m_queue.pop();

    std::map<int, int>::iterator it = m_map.find(entry.msg);
    if (it != m_map.end())
    {
        it->second -= 1;
    }
    return true;
}
