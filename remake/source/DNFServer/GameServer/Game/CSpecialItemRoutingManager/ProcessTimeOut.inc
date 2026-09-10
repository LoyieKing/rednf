int CSpecialItemRoutingManager::ProcessTimeOut(CParty* party)
{
    if (!party->IsRoutingState())
    {
        party->SetEndRouting();
        return 1;
    }
    switch (party->GetFirstValidMemberState())
    {
    default:
        party->SetEndRouting();
        return 1;
    case 5:
        break;
    case 0xc:
        break;
    }
    party->SetRoutingTimerOver();
    if (party->HasAllMemberSelection())
        processRouting(party);
    return 1;
