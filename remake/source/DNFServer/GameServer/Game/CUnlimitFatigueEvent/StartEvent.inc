// ---------------------------------------------------------------------------
// ORIG 0x08285cae：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x12,0)
// ("Unlimit Fatigue Event Start!")。
void CUnlimitFatigueEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x12, 0);
    tr("Unlimit Fatigue Event Start!");
}