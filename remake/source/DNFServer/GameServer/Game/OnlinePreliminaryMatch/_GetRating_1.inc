float COnlinePreliminary::_GetRating(CUser* user)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
        return reinterpret_cast<COnlinePreliminaryMember*>(
                   user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))->GetRating();
    if (user->GetCurCharacOnlinePreliminaryTeam() != 0)
        return user->GetCurCharacOnlinePreliminaryTeam()->GetRating();
    return 0.0f;
