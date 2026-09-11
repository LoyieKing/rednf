// ===========================================================================
// _NS_IPScan  ORIG T 0x8274676：读 1 字节 → HIP_GetHackType →
// addServerHackCnt(user, type, 1, 0, 0)。读取失败返回 1。
// ===========================================================================
int _NS_IPScan(CUser* user, PacketBuf& packet)
{
    unsigned char a = 0;
    if (!packet.get_byte(a))
        return 1;

    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(HIP_GetHackType(a)), 1, 0, 0);
    return 0;
}