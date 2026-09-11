// ---------------------------------------------------------------------------
// ORIG 0x08285d52：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x20,0)
// ("Unlimit Fatigue Event End!")。
void CUnlimitFatigueEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x20, 0);
    tr("Unlimit Fatigue Event End!");
}