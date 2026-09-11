// ---------------------------------------------------------------------------
// ORIG 0x082678be：置事件标志并打日志（__FUNCTION__ "StartEvent"，line 0x21）。
void CPCRoomWorldDropEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x21, 0);
    tr("PCRoom World Drop Event Start!");
}