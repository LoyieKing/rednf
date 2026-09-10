int CSpecialItemRoutingManager::ArriveMemberSelection(CUser* user, CParty* party,
                                                      char sel)
{
    int state = party->GetMemberRoutingState(user);           // ORIG 0x085ba150
    if (state == 0)
    {
        if (sel == 0)
            party->SetMemberRoutingState(user, 1);            // ORIG 0x085ba0d8
        else if (sel == 1)
            party->SetMemberRoutingState(user, 2);
    }
    else
    {
        return 0;
    }
    if (party->HasAllMemberSelection())                       // ORIG 0x085ba1c0
    {
        cancelTimer(party);
        processRouting(party);
    }
    return 1;
