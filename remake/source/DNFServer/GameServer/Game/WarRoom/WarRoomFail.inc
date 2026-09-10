void WarRoom::WarRoomFail()
{
    if (GetLiveCount() != 0)
        return;
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            AddBanList(m_slotsUser[i]);
            CUser* u = m_slotsUser[i];
            WalkOutUserBySlot(i);
            SendWalkOut(u, (ENUM_WALKOUT_TYPE)4);
        }
    }
    ChangeState(WARROOM_STATE_0);
