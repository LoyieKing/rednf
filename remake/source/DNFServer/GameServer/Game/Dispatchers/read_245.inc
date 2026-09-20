int Dispatcher_Float_RDATA_Modulation::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_FLOAT_RDATA_MODULATION& req = *reinterpret_cast<MSG_FLOAT_RDATA_MODULATION*>(&msg);
    if (!packet.get_int(req.m_encoded1))
    {
        return LineFunc(0x582, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(req.m_encoded2))
    {
        return LineFunc(0x586, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
