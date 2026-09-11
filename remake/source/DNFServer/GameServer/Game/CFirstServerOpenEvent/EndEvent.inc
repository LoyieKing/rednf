void CFirstServerOpenEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 52, 0);
    trace("First Server Open Event End!");
}