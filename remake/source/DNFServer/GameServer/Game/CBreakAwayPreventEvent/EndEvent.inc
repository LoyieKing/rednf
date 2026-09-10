void CBreakAwayPreventEvent::EndEvent()
{
    SetEventFlag(false);
    DB_LoadBreakAwayPreventData::makeRequest(false);
    cMyTrace trace(__FUNCTION__, 0x23, 0);
    trace("CBreakAwayPreventEvent End!");
