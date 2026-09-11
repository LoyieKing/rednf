// ---------------------------------------------------------------------------
// ORIG 0x08268fc6：仅 SetEventFlag(false)，无 trace（sub $0x18 帧实证）。
void CPvPLiveEvent::EndEvent()
{
    SetEventFlag(false);
}