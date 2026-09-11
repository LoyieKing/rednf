// ---------------------------------------------------------------------------
// ORIG 0x08268e2a：仅 SetEventFlag(false)，无 trace（sub $0x18 帧实证）。
void CPvPExpPenaltyEvent::EndEvent()
{
    SetEventFlag(false);
}