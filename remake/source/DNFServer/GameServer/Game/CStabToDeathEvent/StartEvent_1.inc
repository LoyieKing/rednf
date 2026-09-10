void CStabToDeathEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x17, 0);
    tr("CStabToDeathEvent Start!");
