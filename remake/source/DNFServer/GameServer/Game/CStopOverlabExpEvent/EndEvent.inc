// ---------------------------------------------------------------------------
// ORIG 0x08117876：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x1f,0)
// ("CStopOverlabExpEvent Event End!")。
void CStopOverlabExpEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1f, 0);
    tr("CStopOverlabExpEvent Event End!");
}