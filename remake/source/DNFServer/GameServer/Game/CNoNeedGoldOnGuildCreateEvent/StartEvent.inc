// ---------------------------------------------------------------------------
// ORIG 0x081b9bf6：置事件标志并打日志（__FUNCTION__ "StartEvent"，line 0x12）。
void CNoNeedGoldOnGuildCreateEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x12, 0);
    tr("No Need Money On Guild Create Event Start!");
}