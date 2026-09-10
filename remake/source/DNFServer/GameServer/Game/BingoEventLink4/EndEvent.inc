void BingoEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x3f, 0);
    trace("BingoEvent Event End!");
