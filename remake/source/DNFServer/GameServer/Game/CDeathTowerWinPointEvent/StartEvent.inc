void CDeathTowerWinPointEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace(__PRETTY_FUNCTION__, 16, 0)("EVENT_FORM : Death Tower Win Point Event Start! (%x%%)", m_ratio);
