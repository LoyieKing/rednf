int WarRoom::GetWaiterCount() const
{
    int count = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
            ++count;
    }
    return count;
