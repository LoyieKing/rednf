// ---------------------------------------------------------------------------
// ORIG 0x081848bc：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0xd,0)
// ("[ConditionEvent] Start!")。
void CConditionEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0xd, 0);
    tr("[ConditionEvent] Start!");
}