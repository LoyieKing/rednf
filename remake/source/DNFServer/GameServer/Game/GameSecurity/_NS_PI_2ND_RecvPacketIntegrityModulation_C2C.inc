// ===========================================================================
// _NS_PI_2ND_RecvPacketIntegrityModulation_C2C  ORIG T 0x8275290：读 ushort，
// v = v + 0xe11；v > 0xf9e 则 0xf9f；addServerHackCnt(user, v, 1, 0, 0)。
// ===========================================================================
int _NS_PI_2ND_RecvPacketIntegrityModulation_C2C(CUser* user,
                                                 PacketBuf& packet)
{
    unsigned short a;
    if (!packet.get_short(a))
        return 1;

    unsigned int v = a + 0xe11;
    if (v > 0xf9e)
        v = 0xf9f;
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(v), 1, 0, 0);
    return 0;
}