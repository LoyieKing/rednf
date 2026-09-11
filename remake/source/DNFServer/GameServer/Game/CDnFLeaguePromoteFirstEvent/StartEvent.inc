// ---------------------------------------------------------------------------
// ORIG 0x0811c124：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x13,0)
// ("CDnFLeaguePromoteFirstEvent Event Start!")。
void CDnFLeaguePromoteFirstEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x13, 0);
    tr("CDnFLeaguePromoteFirstEvent Event Start!");
}