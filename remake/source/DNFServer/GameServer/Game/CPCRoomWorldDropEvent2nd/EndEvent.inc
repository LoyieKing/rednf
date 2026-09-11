// ---------------------------------------------------------------------------
// ORIG 0x0811c068：清事件标志并打日志（__PRETTY_FUNCTION__，line 0x1b）。
void CPCRoomWorldDropEvent2nd::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1b, 0);
    tr("PCRoom World Drop Event 2nd End!");
}