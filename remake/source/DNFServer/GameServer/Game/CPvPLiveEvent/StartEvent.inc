void CPvPLiveEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("CPvPLiveEvent Start!");
