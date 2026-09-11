// ORIG execute 0x084ab23e：有队伍设队伍连胜，否则设个人（CUserCharacInfo）连胜。
void CPowerWarStraightVictory::execute()
{
    CUser* user = GetUser();
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        party->setStraightVictories(m_straight);
    }
    else
    {
        user->setStraightVictories(m_straight);
    }
}