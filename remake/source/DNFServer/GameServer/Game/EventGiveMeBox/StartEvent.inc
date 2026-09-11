// ---------------------------------------------------------------------------
// ORIG 0x08164a1a：SetEventFlag(true) +
// cMyTrace(__PRETTY_FUNCTION__,0x12,0)（ORIG 局部静态
// _ZZN14EventGiveMeBox10StartEventEvE19__PRETTY_FUNCTION__ @0x08b7da00）
// ("[Taiwan, EventGiveMeBox] Event start!")。
void EventGiveMeBox::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("[Taiwan, EventGiveMeBox] Event start!");
}