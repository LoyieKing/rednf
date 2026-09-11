// ---------------------------------------------------------------------------
// ORIG 0x0826e996：SetEventFlag(true) +
// cMyTrace("virtual void CReformingDanjinEvent::StartEvent()",0x13,0)
// ("Reforming Danjin Event Start!")。
void CReformingDanjinEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("Reforming Danjin Event Start!");
}