void EventGiveMeBox::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x17, 0);
    tr("[Taiwan, EventGiveMeBox] Event start!");
