// ---------------------------------------------------------------------------
// ORIG 0x0811c2b8：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x35,0)
// ("CDnFLeaguePromoteSecondEvent Event End!")。
void CDnFLeaguePromoteSecondEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x35, 0);
    tr("CDnFLeaguePromoteSecondEvent Event End!");
}