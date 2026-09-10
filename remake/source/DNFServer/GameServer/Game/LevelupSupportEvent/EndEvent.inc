void LevelupSupportEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x49, 0);
    tr("LevelupSupportEvent Event End!");
