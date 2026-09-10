void COnlinePreliminary::_IncreaseWinCount(CUser* user)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
        reinterpret_cast<COnlinePreliminaryMember*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))->IncreaseWinCount();
    else if (user->GetCurCharacOnlinePreliminaryTeam() != 0)
        user->GetCurCharacOnlinePreliminaryTeam()->IncreaseWinCount();
