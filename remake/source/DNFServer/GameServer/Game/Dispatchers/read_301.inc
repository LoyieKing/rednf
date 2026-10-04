// b28d-solo-Dispatchers verdict=AE(routed) [S162-verify] ORIG 0x0825feda orig/ours=53/53
int Dispatcher_P2PHolePunchingSuccessRateStatistics::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x0825feda：byte@0xd → uint@0xe → uint@0x12
    MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE& m =
        reinterpret_cast<MSG_P2P_HOLE_PUNCHING_SUCCESS_RATE&>(msg);
    // [p167 2026-09-11] ORIG 三段依次为 `get_byteERh` + `get_intERj`×2
    //   （MISSING 2×get_intERj/get_byteERh，EXTRA get_intERi/get_byteERc）⇒ 取址类型改 unsigned。
    if (!packet.get_byte(m.m_0d))    // +0x0d byte
        return LineFunc(0xb3d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_0e))      // +0x0e int
        return LineFunc(0xb40, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_12))      // +0x12 int
        return LineFunc(0xb43, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}