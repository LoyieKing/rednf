void EventNewCharacterReward::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x2f, 0);
    trace("[Taiwan, EventCharacterBonus] Event stop!");
