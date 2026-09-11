// ---------------------------------------------------------------------------
// ORIG 0x08164abe：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x1d,0)（ORIG 局部静态 @0x08b7d980）
// ("[Taiwan, EventGiveMeBox] Event stop!")。
void EventGiveMeBox::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1d, 0);
    tr("[Taiwan, EventGiveMeBox] Event stop!");
}