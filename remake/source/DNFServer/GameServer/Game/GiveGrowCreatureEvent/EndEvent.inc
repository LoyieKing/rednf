void GiveGrowCreatureEvent::EndEvent()                  // 0x081b6a6a
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x24, 0);
    trace("GiveGrowCreatureEvent Event end!");
}