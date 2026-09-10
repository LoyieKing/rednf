void CBurnigGoldMonsterEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1c, 0);
    trace("CBurnigGoldMonsterEvent Event End!");
