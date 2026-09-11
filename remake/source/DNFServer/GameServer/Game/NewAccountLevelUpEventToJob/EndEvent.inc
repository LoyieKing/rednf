// ORIG 0x081b824c：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x2b,0)。
void NewAccountLevelUpEventToJob::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x2b, 0);
    trace("NewAccountLevelUpEvent Event End!");
}