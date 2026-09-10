ISigDispatcher* InterDispatcher::get_dispatcher(int code)
{
    if (m_dispatchers[code] != 0)
    {
        return m_dispatchers[code];
    }
    LogManager::logFormat(1, "InterDispatcher.cpp",
                          __PRETTY_FUNCTION__,
                          0x5e5, "cmd_map.find( code %d )", code);
    return 0;
