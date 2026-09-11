// ---------------------------------------------------------------------------
// ORIG 0x0811cd76：仅 cMyTrace(__PRETTY_FUNCTION__,0x24,0)
// ("CGmRegistEvent End!")（无 SetEventFlag）。
void CGmRegistEvent::EndEvent()
{
    cMyTrace tr(__PRETTY_FUNCTION__, 0x24, 0);
    tr("CGmRegistEvent End!");
}