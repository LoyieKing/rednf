void CFirstServerOpenEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 32, 0);
    trace("First Server Open Event Start!");
