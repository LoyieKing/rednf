void CDnFLeaguePromoteFirstEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1d, 0);
    tr("CDnFLeaguePromoteFirstEvent Event End!");
