void CNoNeedGoldOnGuildCreateEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x12, 0);
    tr("No Need Money On Guild Create Event Start!");
