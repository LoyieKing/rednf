int Dispatcher_Equipslot_Switch::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_EQUIPSLOT_SWITCH& req = *reinterpret_cast<MSG_EQUIPSLOT_SWITCH*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_field0d)))
    {
        return LineFunc(0x3e12,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_field0e)))
    {
        return LineFunc(0x3e14,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_field0f)))
    {
        return LineFunc(0x3e16,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
