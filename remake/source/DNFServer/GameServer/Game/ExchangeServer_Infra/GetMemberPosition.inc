int CACHE_CHARACTER_TYPE::GetMemberPosition(ENUM_INTERNALPACKET type,
                                            int* size, bool& found)
{
    switch (static_cast<int>(type))
    {
    case 0x07:
        found = m_memberFlag[0];
        *size = static_cast<int>(sizeof(m_charac));
        return reinterpret_cast<int>(&m_charac);
    case 0x08:
        found = m_memberFlag[1];
        *size = static_cast<int>(sizeof(m_inventory));
        return reinterpret_cast<int>(&m_inventory);
    case 0xb3:
        found = m_memberFlag[7];
        *size = static_cast<int>(sizeof(m_blackList));
        return reinterpret_cast<int>(&m_blackList[0]);
    case 0x0a:
        found = m_memberFlag[3];
        *size = static_cast<int>(sizeof(m_quest));
        return reinterpret_cast<int>(&m_quest);
    case 0x0b:
        found = m_memberFlag[4];
        *size = static_cast<int>(sizeof(m_ticket));
        return reinterpret_cast<int>(&m_ticket[0]);
    case 0x0c:
        found = m_memberFlag[5];
        *size = static_cast<int>(sizeof(m_dungeonClear));
        return reinterpret_cast<int>(&m_dungeonClear[0]);
    case 0x11:
        found = m_memberFlag[8];
        *size = static_cast<int>(sizeof(m_actionPoint));
        return reinterpret_cast<int>(&m_actionPoint);
    case 0x282:
        found = m_memberFlag[9];
        *size = static_cast<int>(sizeof(m_expand));
        return reinterpret_cast<int>(&m_expand);
    case 0x19:
        found = m_memberFlag[0xa];
        *size = static_cast<int>(sizeof(m_altar));
        return reinterpret_cast<int>(&m_altar);
    case 0x20:
        found = m_memberFlag[0xb];
        *size = static_cast<int>(sizeof(m_etc));
        return reinterpret_cast<int>(&m_etc);
    default:
        return 0;
    }
