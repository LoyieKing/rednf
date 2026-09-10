void CStopOverlabExpEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x15, 0);
    tr("CStopOverlabExpEvent Event Start!");
