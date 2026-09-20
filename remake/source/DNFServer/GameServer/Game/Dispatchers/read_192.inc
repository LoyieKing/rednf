int Dispatcher_GuildAttendanceInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_GUILD_ATTENDANCE& req = *reinterpret_cast<MSG_GUILD_ATTENDANCE*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_field0d)))
    {
        return LineFunc(0x35d9, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
