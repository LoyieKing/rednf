bool TimerQueue::GetTimerMess(TimerEntry& entry) throw()
{
    unsigned int cur = GlobalData::s_systemTime_.getCurTickCount();

    Guard<Mutex> guard(&m_mutex);
    if (m_queue.empty())
    {
        return false;
    }

    entry = m_queue.top();
    if (entry.time <= cur)
    {
        m_queue.pop();

        std::map<int, int>::iterator it = m_map.find(entry.msg);
        if (it != m_map.end())
        {
            it->second -= 1;
        }
        return true;
    }
    return false;
