void CUXGameLogEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr("CUXGameLogEvent::EndEvent()", 0x1e, 0);
    tr("CUXGameLogEvent Event End!");
