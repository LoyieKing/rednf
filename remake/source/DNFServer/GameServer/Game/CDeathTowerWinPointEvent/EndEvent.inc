void CDeathTowerWinPointEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 34, 0);
    trace("Death Tower Win Point Event End!");
