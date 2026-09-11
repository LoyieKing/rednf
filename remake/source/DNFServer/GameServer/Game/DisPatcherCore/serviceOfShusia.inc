// ============================================================================
// ORIG 0x0865798a T _ZN5CUser15serviceOfShusiaEv —— 休沙疲劳恢复服务。
// 幽灵 0x13；疲劳>99 0x12；Restrict(1,0x16) 0xd1；余额<价 0x16；
// 扣费 use_money(price,0xa,true)、AddValueStatistic(0x10,user,price)、
// setCurCharacStamina(100)、UseShusiaService((short)m_level)，成功返 0。
// 定义宿主为本 TU（CUser.cpp 由 RestoreCUserStatMethods 独占，经广播协调）。
// ============================================================================
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
