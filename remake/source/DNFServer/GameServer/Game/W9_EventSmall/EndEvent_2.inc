void CWeekendBonusEvent::EndEvent()          // ORIG 0x08117626
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1c, 0);
    trace("CWeekendBonusEvent Event End!");
}