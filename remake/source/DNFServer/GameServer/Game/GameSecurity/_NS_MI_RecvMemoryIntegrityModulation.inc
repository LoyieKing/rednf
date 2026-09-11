// ===========================================================================
// _NS_MI_RecvMemoryIntegrityModulation  ORIG T 0x8274b09：读 2 字节 →
// MI_GetHackType(a) → SetETC(type, (signed char)b) → addServerHackCnt。
// 读取失败依次返回 1/2。
// ===========================================================================
int _NS_MI_RecvMemoryIntegrityModulation(CUser* user, PacketBuf& packet)
{
    char a, b;
    if (!packet.get_byte(a))
        return 1;
    if (!packet.get_byte(b))
        return 2;

    unsigned int hackType = static_cast<unsigned int>(MI_GetHackType(a));
    user->getHackAnalyzer()->addServerHackCnt(
        user, static_cast<WongWork::ENUM_HACKTYPE>(hackType), 1,
        static_cast<unsigned int>(user->SetETC(static_cast<short>(hackType),
            static_cast<int>(static_cast<signed char>(b)))), 0);
    return 0;
}