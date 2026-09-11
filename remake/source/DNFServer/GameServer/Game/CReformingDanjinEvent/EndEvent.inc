// ---------------------------------------------------------------------------
// ORIG 0x0826ea3a：SetEventFlag(false) +
// cMyTrace("virtual void CReformingDanjinEvent::EndEvent()",0x1f,0)
// ("Reforming Danjin Event End!")。
void CReformingDanjinEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1f, 0);
    tr("Reforming Danjin Event End!");
}