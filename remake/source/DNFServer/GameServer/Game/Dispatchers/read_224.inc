int Dispatcher_PvpMissionComboClear::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PVP_MISSION_COMBO_CLEAR& req = *reinterpret_cast<MSG_PVP_MISSION_COMBO_CLEAR*>(&msg);
    if (!packet.get_byte(req.m_field0d))
        return LineFunc(0x394e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_byte(req.m_field0e))
        return LineFunc(0x3950, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}
