int WarRoom::GetUserSlot(CUser* user)
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slotsUser[i] == user)
            return i;
    }
    return -1;
