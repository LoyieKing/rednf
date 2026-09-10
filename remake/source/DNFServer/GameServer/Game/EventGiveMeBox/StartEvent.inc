void EventGiveMeBox::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("[Taiwan, EventGiveMeBox] Event start!");
