bool Arad_DatabaseDispatcher::_registDispatcher(ENUM_INTERNALPACKET_JPN code, Arad_IDatabaseDispatcher* d)
{
    // ORIG 0x08185144：已存在则立即 false 早退；否则注册并 true。
    if (GetDispatcher(code) != 0)
        return false;
    m_dispatchers[code] = d;
    return true;
