void CSpecialItemRoutingManager::ProcessGiveUp(CParty* party)
{
    if (party == 0)
        return;

    if (party->IsRoutingState() && party->HasAllMemberSelection()
        && (party->GetFirstValidMemberState() == 5
            || party->GetFirstValidMemberState() == 0xc))
        processRouting(party);
