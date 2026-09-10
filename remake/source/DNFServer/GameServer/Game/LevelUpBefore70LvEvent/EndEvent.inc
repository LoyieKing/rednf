void LevelUpBefore70LvEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1e, 0);
    trace("LevelUpBefore70Lv Event end!");
