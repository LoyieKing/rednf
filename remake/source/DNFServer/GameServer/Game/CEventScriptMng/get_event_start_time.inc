int CEventEntity::get_event_start_time()
{
    // ORIG 0x810b094：return *(int*)(this+0x0c)。
    return m_startTime;
}