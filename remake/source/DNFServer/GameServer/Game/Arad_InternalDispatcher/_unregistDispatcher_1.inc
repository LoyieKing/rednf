void Arad_InternalDispatcher::_unregistDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    // ORIG 0x08198a48：命中则 erase(find(code))。
    if (GetDispatcher(code) != 0)
        m_dispatchers.erase(m_dispatchers.find(code));
