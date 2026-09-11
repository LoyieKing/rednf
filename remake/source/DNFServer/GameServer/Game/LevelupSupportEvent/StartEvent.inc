// ---------------------------------------------------------------------------
// ORIG 0x08116a62：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x3f,0)
// ("LevelupSupportEvent Event Start!")。
void LevelupSupportEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x3f, 0);
    tr("LevelupSupportEvent Event Start!");
}