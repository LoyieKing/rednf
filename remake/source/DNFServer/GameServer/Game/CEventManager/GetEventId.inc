int CEventTable::GetEventId(int idx)
{
    if (idx < 0 || GetEventTableCount() <= idx)
        return 0;
    return events[idx].id;
}