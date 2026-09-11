// ---------------------------------------------------------------------------
// ORIG 0x08164a6c：与 StartEvent() 同体（同样 SetEventFlag(true) + trace，
// __PRETTY_FUNCTION__ 局部静态 @0x08b7d9c0，行 0x17）。
void EventGiveMeBox::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x17, 0);
    tr("[Taiwan, EventGiveMeBox] Event start!");
}