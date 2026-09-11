// ORIG 0x081850e0：find(code)；!=(end) 返回 second，否则 NULL。
Arad_IDatabaseDispatcher* Arad_DatabaseDispatcher::GetDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    std::map<ENUM_INTERNALPACKET_JPN, Arad_IDatabaseDispatcher*>::iterator it = m_dispatchers.find(code);
    if (it != m_dispatchers.end())
        return it->second;
    return 0;
}