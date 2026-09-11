// ---------------------------------------------------------------------------
// ORIG 0x081b9c9a：清事件标志并打日志（__FUNCTION__ "EndEvent"，line 0x1e）。
void CNoNeedGoldOnGuildCreateEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("No Need Money On Guild Create Event End!");
}