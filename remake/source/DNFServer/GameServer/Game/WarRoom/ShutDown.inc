int WarRoom::ShutDown()
{
    if (m_state < 1)
    {
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] != 0)
            {
                CUser* u = m_slotsUser[i];
                WalkOutUserBySlot(i);
                SendWalkOut(u, (ENUM_WALKOUT_TYPE)7);
            }
        }
        Close();
        return 1;
    }
    m_field30 = 1;
    return 0;
