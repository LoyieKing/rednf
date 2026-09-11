// 0x0868de0a
// ORIG：光环 avatar 选项变更 -> DB_UpdateAuraAvatarOption::makeRequest(uid, characNo, idx, value)
void CUser::UpdateAuraAvatarOption(int a, int b)
{
    if (a >= 0 && a <= 2)
    {
        DB_UpdateAuraAvatarOption::makeRequest(GetUID(), getCurCharacNo(), a, b);
    }
}