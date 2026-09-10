void COnlinePreliminary::_SetRating(CUser* user, float v)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
        reinterpret_cast<COnlinePreliminaryMember*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))->SetRating(v);
    else if (user->GetCurCharacOnlinePreliminaryTeam() != 0)
        user->GetCurCharacOnlinePreliminaryTeam()->SetRating(v);
