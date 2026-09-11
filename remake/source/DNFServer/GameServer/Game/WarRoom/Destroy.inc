int WarRoom::Destroy(int idx)
{
    int user = (int)m_slots[idx];
    if (user == 0)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                user = (int)m_slots[i];
                break;
            }
        }
    }
    Reset();
    return user;
}