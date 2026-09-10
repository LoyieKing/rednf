void CEventAdvanceAltarOpen::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x494, 0);
    tr("CEventAdvanceAltarOpen Event End!");
