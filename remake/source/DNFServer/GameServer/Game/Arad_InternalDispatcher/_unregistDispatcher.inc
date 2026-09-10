void Arad_DatabaseDispatcher::_unregistDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    if (GetDispatcher(code) != 0)
        m_dispatchers.erase(m_dispatchers.find(code));
