// ---------------------------------------------------------------------------
// ORIG 0x080e9534：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x494,0)
// ("CEventAdvanceAltarOpen Event End!")。
void CEventAdvanceAltarOpen::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x494, 0);
    tr("CEventAdvanceAltarOpen Event End!");
}