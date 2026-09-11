// ORIG 0x0860a634：累加 GetSpAtLevelUp(1..charLevel)。
int CSkillChanger::_GetBaseSkillPoint(const CUser* user) const
{
    int sum = 0;
    for (int level = 1; level <= user->get_charac_level(); level++)
    {
        sum += (int)G_CDataManager()->GetSpAtLevelUp(level);
    }
    return sum;
}