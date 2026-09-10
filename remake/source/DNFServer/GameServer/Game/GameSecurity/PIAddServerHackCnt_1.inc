void PIAddServerHackCnt(CUser* user, WongWork::ENUM_HACKTYPE type, int count)
{
    if (static_cast<int>(type) == 0x524)
    {
        if (user->CheckInParty())
        {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            if (party->get_member_count() > 1)
                return;
        }
        }
    }
    user->getHackAnalyzer()->addServerHackCnt(user, type, 1u,
        static_cast<unsigned int>(user->SetETC(static_cast<short>(type), count)), 0u);
