void CStabToDeathEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x11, 0);
    tr("CStabToDeathEvent Start!");
