void LevelUpBefore70LvEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x18, 0);
    trace("LevelUpBefore70Lv Event start! [monitor request]");
