int CEventEntity::get_event_end_time()
{
    // ORIG 0x810b0a0：return *(int*)(this+0x10)。
    return m_endTime;
