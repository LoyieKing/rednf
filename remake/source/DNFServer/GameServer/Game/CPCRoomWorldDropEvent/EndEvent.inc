// ---------------------------------------------------------------------------
// ORIG 0x0826792a：清事件标志并打日志（__FUNCTION__ "EndEvent"，line 0x2c）。
void CPCRoomWorldDropEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x2c, 0);
    tr("PCRoom World Drop Event End!");
}