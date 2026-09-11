// ---------------------------------------------------------------------------
// ORIG 0x08285d00：与 StartEvent() 同体（file 串 "StartEvent"，行 0x19）。
void CUnlimitFatigueEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x19, 0);
    tr("Unlimit Fatigue Event Start!");
}