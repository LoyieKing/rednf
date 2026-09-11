// ---------------------------------------------------------------------------
// ORIG 0x0811c90e：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__@0x08b51ee0,0x1c,0)
// ("CFatigueGiveItemEvent Event End!"@0x08b4fd20)。
void CFatigueGiveItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1c, 0);
    tr("CFatigueGiveItemEvent Event End!");
}