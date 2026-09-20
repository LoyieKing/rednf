int Dispatcher_Break_Away_Quest_Check::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BREAK_AWAY& req = *reinterpret_cast<MSG_BREAK_AWAY*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_checkFlag)))
    {
        return LineFunc(0x3197,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
