void CBreakAwayPreventEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x14, 0);
    trace("CBreakAwayPreventEvent Start!");
