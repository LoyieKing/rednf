int Dispatcher_ScanBotByDll::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SCAN_DLL& req = *reinterpret_cast<MSG_SCAN_DLL*>(&msg);
    if (!packet.get_int(req.m_field0d))
    {
        return LineFunc(0x3d6, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_field11)))
    {
        return LineFunc(0x3db, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_field12))
    {
        return LineFunc(0x3e0, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
