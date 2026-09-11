// ---------------------------------------------------------------------------
// ORIG 0x08116ace：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x49,0)
// ("LevelupSupportEvent Event End!")。
void LevelupSupportEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x49, 0);
    tr("LevelupSupportEvent Event End!");
}