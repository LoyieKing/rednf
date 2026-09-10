void LevelupSupport2ndEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x75, 0);
    tr("LevelupSupport2ndEvent Event End!");
