void CUser::ClearChildrensDayEvent()
{
    if (m_childrensDayEventState == 0)
        return;
    m_childrensDayEventState = 0;
    for (int i = 0; i <= 9; i++)
    {
        m_childrensDaySlots[i] = -1;
        m_childrensDayPeriods[i] = -1;
    }
