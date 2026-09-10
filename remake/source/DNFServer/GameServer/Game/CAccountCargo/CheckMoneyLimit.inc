bool CAccountCargo::CheckMoneyLimit(unsigned int money)
{
    // [b28c-11] AE（i486 旗标下）：UpgradeInfo 具名视图消裸偏移 add$4 中转 +
    // (unsigned int) 零扩展消符号扩展段（ORIG `mov $0x0,%edx`）。
    UpgradeInfo* info =
        (UpgradeInfo*)G_CDataManager()->m_accountCargo.GetCurrUpgradeInfo(m_capacity);
    if (info == 0)
        return 0;
    // ORIG：64 位槽比较（total 先双清零落槽，比较链 ja/jb/jae）。
    unsigned long long total = 0;
    total = (unsigned long long)(m_money + money);
    if ((unsigned long long)(unsigned int)info->m_field4 < total)
        return 0;
    return 1;
