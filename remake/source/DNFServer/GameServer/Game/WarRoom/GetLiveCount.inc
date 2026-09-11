int WarRoom::GetLiveCount() const
{
    int count = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if ((m_slots[i] != 0) && (m_liveFlag[i] != 0))
            ++count;
    }
    return count;
}