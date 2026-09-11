// ---------------------------------------------------------------------------
// ORIG 0x081b9c48：同 StartEvent()（忽略 param，line 0x18）。
void CNoNeedGoldOnGuildCreateEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("No Need Money On Guild Create Event Start!");
}