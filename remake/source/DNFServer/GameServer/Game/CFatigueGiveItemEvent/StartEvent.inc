void CFatigueGiveItemEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("CFatigueGiveItemEvent Event Start!");
