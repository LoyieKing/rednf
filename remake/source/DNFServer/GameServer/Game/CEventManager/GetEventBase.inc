CEventBase* CEventTable::GetEventBase(int idx)
{
    if (idx < 0 || GetEventTableCount() <= idx)
        return 0;
    return events[idx].base;
