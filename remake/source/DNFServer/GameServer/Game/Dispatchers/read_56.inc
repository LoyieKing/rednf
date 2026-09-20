int Dispatcher_ScanBotByDrv::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SCAN_DRV& req = *reinterpret_cast<MSG_SCAN_DRV*>(&msg);
    if (!packet.get_int(req.m_field0d))
    {
        return LineFunc(0x42c, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_field11)))
    {
        return LineFunc(0x431, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
