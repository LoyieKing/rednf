void CAccountCargo::SubMoney(unsigned int money)
{
    if (money != 0 && money <= m_money)
        m_money = m_money - money;
}