void CUser::SetAccountCargoDBData(Inven_Item* items, unsigned int money,
                                  unsigned int capacity)
{
    m_accountCargo.SetDBData(this, items, money, capacity);
