int CBossTower::onClearBossTower()
{
    if (m_party->m_battleField.m_bloodState == 1)
    {
        PacketGuard packet;
        packet.put_header(0, 0xfe);
        packet.finalize(true);
        m_party->send_to_party(packet);
    }
    else
    {
        if (!generateRewardExp())
            return 0;
        if (!generateRewardItem())
            return 0;
        if (!SendRewardPacket())
            return 0;
        if (!InsertRewardExp())
            return 0;
        if (!InsertRewardItems())
            return 0;
    }

    if (!SendELPLPacket())
        return 0;

    m_party->SetEPLPState(2);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)7, m_idx,
                                (TIMER_MESSAGE)0xa4, 0x14, 0xa4, 0xa4);
    return 1;
