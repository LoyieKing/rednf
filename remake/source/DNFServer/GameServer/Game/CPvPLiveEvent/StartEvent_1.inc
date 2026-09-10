void CPvPLiveEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x19, 0);
    tr("CPvPLiveEvent Start!");
