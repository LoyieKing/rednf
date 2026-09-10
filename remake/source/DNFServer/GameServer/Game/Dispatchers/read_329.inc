int Dispatcher_Skill_Command_Customizing::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(m[0xd]))
    {
        return LineFunc(0x2ff8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    const char commandCount = m[0xd];
    for (int i = 0; i < (int)commandCount; ++i)
    {
        if (!packet.get_short(*reinterpret_cast<short*>(m + i * 8 + 0xe)))
        {
            return LineFunc(0x2fff,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(m[i * 8 + 0x10]))
        {
            return LineFunc(0x3002,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        const char keyCount = (char)m[i * 8 + 0x10];
        for (int j = 0; j < (int)keyCount; ++j)
        {
            if (!packet.get_byte(m[i * 8 + j + 0x11]))
            {
                return LineFunc(0x3009,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
        }
    }
    return 0;
