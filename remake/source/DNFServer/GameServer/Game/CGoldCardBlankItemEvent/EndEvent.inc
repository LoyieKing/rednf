void CGoldCardBlankItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Gold Card Blank Item Event End");
