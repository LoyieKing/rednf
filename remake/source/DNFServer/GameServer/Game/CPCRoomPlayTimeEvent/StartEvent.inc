void CPCRoomPlayTimeEvent::StartEvent()
{
    // ORIG 0x0811c5c4：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x15, 0)。
    // [R11fixE5] ORIG 0x0811c600 串=0x8b4fcb8（raw 逐字节
    // 'CPCRoomPlayTimeEvent Event Start!'）；旧文 'PC Room PlayTime Event Start!'。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x15, 0);
    tr("CPCRoomPlayTimeEvent Event Start!");
}