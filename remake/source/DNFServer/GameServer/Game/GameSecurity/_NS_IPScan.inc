int _NS_IPScan(CUser* user, PacketBuf& packet)
{
    unsigned char a = 0;
    if (!packet.get_byte(a))
        return 1;

    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(HIP_GetHackType(a)), 1, 0, 0);
    return 0;
