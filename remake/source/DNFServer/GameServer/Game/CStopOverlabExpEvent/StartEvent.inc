// ---------------------------------------------------------------------------
// ORIG 0x0811780a：SetEventFlag(true) +
// cMyTrace(__PRETTY_FUNCTION__,0x15,0)
// ("CStopOverlabExpEvent Event Start!")。
void CStopOverlabExpEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x15, 0);
    tr("CStopOverlabExpEvent Event Start!");
}