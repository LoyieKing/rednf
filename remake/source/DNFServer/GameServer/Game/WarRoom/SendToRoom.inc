void WarRoom::SendToRoom(PacketGuard& guard)
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
            (m_slotsUser[i])->Send(guard);
    }
