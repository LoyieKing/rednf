void CUnlimitFatigueEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x12, 0);
    tr("Unlimit Fatigue Event Start!");
