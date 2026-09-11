// ---------------------------------------------------------------------------
// ORIG 0x0811c8a2：SetEventFlag(true) +
// cMyTrace(__PRETTY_FUNCTION__@0x08b51f20,0x12,0)
// ("CFatigueGiveItemEvent Event Start!"@0x08b4fcfc)。
void CFatigueGiveItemEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("CFatigueGiveItemEvent Event Start!");
}