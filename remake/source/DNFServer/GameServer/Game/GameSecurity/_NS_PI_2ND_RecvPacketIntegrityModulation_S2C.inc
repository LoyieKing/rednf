// ===========================================================================
// _NS_PI_2ND_RecvPacketIntegrityModulation_S2C  ORIG T 0x8275140：读 byte+short
// → _NS_PI_2ND_GetHackType(1, a, b) → addServerHackCnt(user, type, 1, 0, 0)。
// ===========================================================================
int _NS_PI_2ND_RecvPacketIntegrityModulation_S2C(CUser* user,
                                                 PacketBuf& packet)
{
    unsigned char a;
    unsigned short b;
    if (!packet.get_byte(a))
        return 1;
    if (!packet.get_short(b))
        return 2;

    unsigned int hackType = static_cast<unsigned int>(_NS_PI_2ND_GetHackType(1, a, b));
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1, 0, 0);
    return 0;
}