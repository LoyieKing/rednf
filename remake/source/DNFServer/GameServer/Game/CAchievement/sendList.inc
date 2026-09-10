void CAchievement::sendList()
{
    stAchievement_DBData data;
    getData((char*)&data);

    PacketGuard packet;
    packet.put_header(0, 0x166);
    packet.put_int(data.m_count);
    for (int i = 0; i < data.m_count; ++i)
    {
        packet.put_int((int)data.m_entries[i].m_id);
        packet.put_short((int)data.m_entries[i].m_trigger0);
        packet.put_short((int)data.m_entries[i].m_trigger1);
        packet.put_short((int)data.m_entries[i].m_trigger2);
    }
    packet.finalize(true);
    m_user->Send(packet);
