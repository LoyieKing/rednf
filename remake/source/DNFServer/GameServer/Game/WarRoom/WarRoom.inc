WarRoom::WarRoom()
{
    SetState((WARROOM_STATE)-1);
    for (int i = 0; i <= 5; ++i)
        m_slots[i] = 0;
