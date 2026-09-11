// ---------------------------------------------------------------------------
// ORIG 0x0811ba5e：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x75,0)
// ("LevelupSupport2ndEvent Event End!")。
void LevelupSupport2ndEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x75, 0);
    tr("LevelupSupport2ndEvent Event End!");
}