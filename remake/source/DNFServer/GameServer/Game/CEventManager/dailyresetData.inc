void CEventManager::dailyresetData()
{
    for (int i = 0; i <= 0xa5; ++i)
    {
        if (m_events[i] != 0)
        {
            if (m_events[i]->IsEventing(0))
                m_events[i]->dailyresetData();
        }
    }
