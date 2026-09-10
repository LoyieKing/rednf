int CUser::GetPowerWarPartyVictory()
{
    CParty* party = reinterpret_cast<CParty*>(GetParty());
    if (party == 0)
        return getStraightVictories();
    return party->getStraightVictories();
}
