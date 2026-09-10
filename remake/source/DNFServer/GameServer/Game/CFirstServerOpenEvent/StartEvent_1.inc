void CFirstServerOpenEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 46, 0);
    trace("First Server Open Event Start!");
