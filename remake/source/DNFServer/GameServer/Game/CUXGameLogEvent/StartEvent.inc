void CUXGameLogEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr("CUXGameLogEvent::StartEvent()", 0x13, 0);
    tr("CUXGameLogEvent Event Start!");
