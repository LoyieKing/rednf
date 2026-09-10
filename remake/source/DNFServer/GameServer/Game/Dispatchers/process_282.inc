int Dispatcher_TournamentRewardSelectState::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081dfca2
    stTournamentRewardSelectState& p = static_cast<stTournamentRewardSelectState&>(param);
    *(int*)((char*)&p + 4) = check_error(user, msg);
    if (*(int*)((char*)&p + 4) >= 1)
    {
        return 0;
    }
    if (*(int*)((char*)&p + 4) < 0)
    {
        return LineFunc(0x52af,
                        __PRETTY_FUNCTION__,
                        *(int*)((char*)&p + 4), 0);
    }
    int partyIndex = user->GetPartyIndex();
    CParty* party = G_CGameManager()->GetParty(partyIndex);
    if (party == 0)
    {
        return LineFunc(0x52b4,
                        __PRETTY_FUNCTION__,
                        0, (unsigned int)user->get_acc_id());
    }
    return 0;
