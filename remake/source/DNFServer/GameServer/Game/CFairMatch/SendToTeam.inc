void TeamInfoThird::SendToTeam(PacketGuard& pkt)
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_players[i].m_flag != 0 && m_players[i].m_user != 0)
            m_players[i].m_user->Send(pkt);
    }
