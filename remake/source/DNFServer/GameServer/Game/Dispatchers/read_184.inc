int Dispatcher_ValidateScriptHash::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SCRIPT_HASH& req = *reinterpret_cast<MSG_SCRIPT_HASH*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_field0d)))
    {
        return LineFunc(0x37d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_field0e)))
    {
        return LineFunc(0x382,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_field0f))
    {
        return LineFunc(0x387,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
