void EpollHandler::destroy()
{
    if (m_events != NULL)
    {
        ::operator delete[](m_events);
    }
    m_events = NULL;
