int Dispatcher_PeerConnectResult::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x082033e6：连读 get_byte(result)/get_short(uniqueId)，任一失败
    // → LineFunc(0x9a1c / 0x9a1f, 0, 0)；find_from_world(uniqueId)==0 返 0；
    // 取双方 nat_type（movsbl 扩展）+ result 字节 → 统计器 IncreaseNatType 后返 0。
    unsigned char result = 0;
    unsigned short uniqueId = 0;
    if (!packet.get_byte(result))
    {
        return LineFunc(0x9a1c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&uniqueId)))
    {
        return LineFunc(0x9a1f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CUser* peer = G_GameWorld()->find_from_world(uniqueId);
    if (peer == 0)
    {
        return 0;
    }
    // asm：movsbl nat 值 → int（有符号字节扩展）
    int localNat = static_cast<int>(static_cast<signed char>(user->m_network.get_nat_type()));
    int peerNat = static_cast<int>(static_cast<signed char>(peer->m_network.get_nat_type()));
    StatisticsNatType* stats = G_GameWorld()->GetStatisticsNatType();
    stats->IncreaseNatType(localNat, peerNat, (unsigned char)result);
    return 0;
