// ---------------------------------------------------------------------------
// ORIG 0x0811b9f2：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x6b,0)
// ("LevelupSupport2ndEvent Event Start!")。
void LevelupSupport2ndEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x6b, 0);
    tr("LevelupSupport2ndEvent Event Start!");
}