bool CLuckPoint::UseLuckPoint(CUser* user, int itemType, int rarity, int grade)
{
    if (user == 0)
        return false;
    if (itemType != 1)
        return false;
    if (rarity != 2)
        return false;
    int rate = G_CDataManager()->m_serverParameter.getLuckPointDeductionRate(
        user->get_charac_level(), abs(grade - user->get_charac_level()));
    user->DecCurCharacLuckPoint(rate);
    return true;
