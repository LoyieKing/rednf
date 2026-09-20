int Dispatcher_Buy_Charac_Status_Using_QP::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BUY_CHARAC_STATUS_USING_QP& req = *reinterpret_cast<MSG_BUY_CHARAC_STATUS_USING_QP*>(&msg);
    if (!packet.get_byte(req.m_field0d))
    {
        return LineFunc(0x3fb8, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(req.m_field0e))
    {
        return LineFunc(0x3fba, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
