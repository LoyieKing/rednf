void CUser::UpdateAuraAvatarOption(int a, int b)
{
    if (a >= 0 && a <= 2)
    {
        DB_UpdateAuraAvatarOption::makeRequest(GetUID(), getCurCharacNo(), a, b);
    }
