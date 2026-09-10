void CUnlimitFatigueEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x20, 0);
    tr("Unlimit Fatigue Event End!");
