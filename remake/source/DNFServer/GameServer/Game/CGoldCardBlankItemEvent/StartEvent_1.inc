void CGoldCardBlankItemEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("Gold Card Blank Item Event Start");
