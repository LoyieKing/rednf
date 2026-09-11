// ---------------------------------------------------------------------------
// ORIG 0x0811bffc：置事件标志并打日志（__PRETTY_FUNCTION__，line 0x11）。
void CPCRoomWorldDropEvent2nd::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x11, 0);
    tr("PCRoom World Drop Event 2nd Start!");
}