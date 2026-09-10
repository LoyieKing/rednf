void CSetMultiboxFailCnt::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }
    user->set_multiboxLotteryItemFailCnt(m_failCnt);
    if (m_failCnt > 2)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x1c6);
        packet.finalize(true);
        user->Send(packet);
    }
