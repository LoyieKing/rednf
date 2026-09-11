// ---------------------------------------------------------------------------
// ORIG 0x0828468e：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x1e,0)
// ("Tournament PvP Channel Open End")。
void CTournamentPvPEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Tournament PvP Channel Open End");
}