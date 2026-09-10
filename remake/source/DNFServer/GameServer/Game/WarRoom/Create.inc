int WarRoom::Create(CUser* user, char* name, short idx)
{
    m_state = (WARROOM_STATE)0;
    m_slots[0] = user;
    m_slotState[0] = 1;
    user->SetWarRoomIndex((short)m_index);
    return 0;
