void InterDispatcher::register_dispatcher(int code, ISigDispatcher* dispatcher)
{
    m_dispatchers[code] = dispatcher;
