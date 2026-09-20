int Dispatcher_DonateGuildFund::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_DONATE_GUILD_FUND& req = *reinterpret_cast<MSG_DONATE_GUILD_FUND*>(&msg);
    if (!packet.get_int(req.m_donateMoney))
    {
        return LineFunc(0x3491, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
