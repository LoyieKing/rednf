void OneADayItemShopEvent::EndEvent()                   // 0x081ba0ae
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x2d, 0);
    trace("OneADayItemShopEvent Event End!");
