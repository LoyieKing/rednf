void GiveGrowCreatureEvent::StartEvent(Word_Param)      // 0x081b6a18
{
    SetEventFlag(true);
    cMyTrace trace(
        __PRETTY_FUNCTION__, 0x1e, 0);
    trace("GiveGrowCreatureEvent Event start! [monitor request]");
}