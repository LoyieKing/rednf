void CAccountCargo::SetDBData(CUser* user, Inven_Item* items, unsigned int money,
                              unsigned int capacity)
{
    m_pUser = user;
    m_capacity = capacity;
    m_money = money;
    m_alter = 0;
    if (items != 0)
        memcpy(m_slots, items, 0xd58);
