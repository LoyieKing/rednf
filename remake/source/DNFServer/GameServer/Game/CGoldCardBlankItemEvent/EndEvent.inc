// ---------------------------------------------------------------------------
// ORIG 0x0811cee6：SetEventFlag(false) +
// cMyTrace(__FUNCTION__,0x1e,0)("Gold Card Blank Item Event End")。
void CGoldCardBlankItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Gold Card Blank Item Event End");
}