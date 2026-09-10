void CTournamentDungeonReward::MakeSelectPacket(PacketGuard* packet)
{
    for (int type = 0; type < 2; ++type)
    {
        packet->put_byte(2);
        for (int slot = 0; slot < 2; ++slot)
        {
            packet->put_byte(static_cast<char>(m_rewards[type][slot].m_flag08));
        }
    }
