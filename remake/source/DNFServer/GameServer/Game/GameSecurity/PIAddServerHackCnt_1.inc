// ===========================================================================
// PIAddServerHackCnt  ORIG W 0x082772b7
// type==0x524 且在组队（CParty 成员数>1）时跳过（防组队场景误判）；
// 否则 SetETC((short)type, count) 记 ETC 并把回写值交给 CHackAnalyzer：
//   addServerHackCnt(user, type, 1, etcValue, 0)
// ===========================================================================
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
}