int Dispatcher_Expand_Equipslot_Flag_Update::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE& req = *reinterpret_cast<MSG_EXPAND_EQUIPSLOT_FLAG_UPDATE*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_flag)))
    {
        return LineFunc(0x3de4, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
