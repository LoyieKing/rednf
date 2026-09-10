void EventProcessManager::destroy()
{
    for (std::map<EVENT_TYPE, std::map<int, EventProcessor*> >::iterator group =
             m_impl->processors.begin(); group != m_impl->processors.end(); ++group)
    {
        std::map<int, EventProcessor*>& inner = (*group).second;
        for (std::map<int, EventProcessor*>::iterator it = inner.begin();
             it != inner.end(); ++it)
        {
            EventProcessor* p = (*it).second;
            if (p != 0)
            {
                delete p;
                p = 0;
            }
        }
    }
