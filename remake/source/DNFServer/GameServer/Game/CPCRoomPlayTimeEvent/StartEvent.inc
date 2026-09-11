void CPCRoomPlayTimeEvent::StartEvent()
{
    // ORIG 0x0811c5c4：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x15, 0)。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x15, 0);
    tr("PC Room PlayTime Event Start!");
}