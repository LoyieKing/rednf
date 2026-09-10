void CNoNeedGoldOnGuildCreateEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("No Need Money On Guild Create Event End!");
