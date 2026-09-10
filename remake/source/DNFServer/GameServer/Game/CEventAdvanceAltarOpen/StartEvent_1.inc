void CEventAdvanceAltarOpen::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x48e, 0);
    tr("CEventAdvanceAltarOpen Event Start! ");
