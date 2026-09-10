int CUser::serviceOfShusia()

{
    if (IsCurCharacGhost())
    {
        return 0x13;
    }
    if (getCurCharacR()->m_stamina > 99)  // ORIG getCurCharacR + movzbl 0x75
    {
        return 0x12;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            this, (RestrictCategory::Enum)1, 0x16))
    {
        return 0xd1;
    }

    const int price = G_CDataManager()->m_serverParameter.getRecoverStaminaPrice(
        get_charac_level(), getCurCharacR()->m_stamina);
    if (getCurCharacMoney() < price)
    {
        return 0x16;
    }

    getCurCharacInvenW()->use_money(price, (eMoneySubReason)0xa, true);
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x10, this,
                                                   (unsigned int)price);
    setCurCharacStamina(100);
    Statistics::UseShusiaService((int)(short)getCurCharacR()->m_level);
    return 0;
}
