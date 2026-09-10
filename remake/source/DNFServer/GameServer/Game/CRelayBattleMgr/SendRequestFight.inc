int CRelayBattleMgr::SendRequestFight(int seat)
{
    PacketGuard guard;
    guard.put_header(0, 0x71);
    guard.put_byte(seat);
    guard.finalize(true);
    m_pRoom->send_to_pvp(guard);
    return 1;
