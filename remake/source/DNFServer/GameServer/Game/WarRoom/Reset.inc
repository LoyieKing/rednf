void WarRoom::Reset()
{
    BattleReset();
    m_field8 = 0;
    m_levelBand = 2;
    m_roomSeq = 0;
    m_field30 = 0;
    for (int i = 0; i <= 5; ++i)
    {
        m_slots[i] = 0;
        m_slotState[i] = 0xff;
        m_slotData[i] = 0;
    }
}