// ---------------------------------------------------------------------------
// ORIG 0x0811c24c：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x2b,0)
// ("CDnFLeaguePromoteSecondEvent Event Start!")。
void CDnFLeaguePromoteSecondEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x2b, 0);
    tr("CDnFLeaguePromoteSecondEvent Event Start!");
}