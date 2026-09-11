// ---------------------------------------------------------------------------
// ORIG 0x082674ac：置事件标志并打日志（忽略 param）。
void CPcRoomCardBlankItemEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("Pc Room Card Blank Item Event Start");
}