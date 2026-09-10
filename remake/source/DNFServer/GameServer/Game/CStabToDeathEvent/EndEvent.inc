void CStabToDeathEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1d, 0);
    tr("CStabToDeathEvent End!");
