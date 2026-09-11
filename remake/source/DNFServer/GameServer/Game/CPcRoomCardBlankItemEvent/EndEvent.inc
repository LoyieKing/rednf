// ---------------------------------------------------------------------------
// ORIG 0x082674fe：清事件标志并打日志。
void CPcRoomCardBlankItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Pc Room Card Blank Item Event End");
}