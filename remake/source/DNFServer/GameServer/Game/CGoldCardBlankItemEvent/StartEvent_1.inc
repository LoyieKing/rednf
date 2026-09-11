// ---------------------------------------------------------------------------
// ORIG 0x0811ce94：SetEventFlag(true) +
// cMyTrace(__FUNCTION__,0x18,0)("Gold Card Blank Item Event Start")。
void CGoldCardBlankItemEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("Gold Card Blank Item Event Start");
}