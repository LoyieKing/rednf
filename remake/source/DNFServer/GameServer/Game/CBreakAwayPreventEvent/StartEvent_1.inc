void CBreakAwayPreventEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    DB_LoadBreakAwayPreventData::makeRequest(true);
    cMyTrace trace(__FUNCTION__, 0x1b, 0);
    trace("CBreakAwayPreventEvent Start!");
