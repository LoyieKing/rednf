CEventBase* CEventManager::GetRepeatEvent(int type)
{
    if (type > 0xa5)
        return 0;
    return m_events[type];
}