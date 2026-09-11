// ---------------------------------------------------------------------------
// ORIG 0x080e94e2：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x48e,0)
// ("CEventAdvanceAltarOpen Event Start! "，尾部带空格)。
void CEventAdvanceAltarOpen::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x48e, 0);
    tr("CEventAdvanceAltarOpen Event Start! ");
}