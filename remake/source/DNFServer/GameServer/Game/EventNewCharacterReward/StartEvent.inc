// ORIG 0x0816bb42：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x16,0)。
void EventNewCharacterReward::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x16, 0);
    trace("[Taiwan, EventCharacterBonus] Event start!");
}