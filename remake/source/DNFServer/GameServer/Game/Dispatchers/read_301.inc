int Dispatcher_P2PHolePunchingSuccessRateStatistics::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x0825feda：byte@0xd → uint@0xe → uint@0x12
    MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE& m =
        reinterpret_cast<MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE&>(msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_0d)))    // +0x0d byte
        return LineFunc(0xb3d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_0e))                                // +0x0e int
        return LineFunc(0xb40, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_12))                                // +0x12 int
        return LineFunc(0xb43, __PRETTY_FUNCTION__, 0, 0);
    return 0;
