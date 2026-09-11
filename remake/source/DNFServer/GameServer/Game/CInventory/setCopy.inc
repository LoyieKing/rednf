void CInventory::setCopy(const CInventory& other)
{
    m_pParent = other.m_pParent;
    m_money = other.m_money;
    m_coin = other.m_coin;
    m_payCoin = other.m_payCoin;
    m_eventCoin = other.m_eventCoin;
    m_field14 = other.m_field14;
    for (int i = 0; i < 26; i++)
    {
        m_invenItem[i].setCopy(other.m_invenItem[i]);
    }
    for (int i = 0; i < 0x138; i++)
    {
        m_pEquipSlot[i].setCopy(other.m_pEquipSlot[i]);
    }
    for (int i = 0; i < 0x69; i++)
    {
        m_pAvatarSlot[i].setCopy(other.m_pAvatarSlot[i]);
    }
    m_creatureMgr.SetUser(m_pParent);
    for (int i = 0; i < 0xf2; i++)
    {
        m_pCreatureSlot[i].setCopy(other.m_pCreatureSlot[i]);
    }
    m_inventoryCapacity = other.get_inventory_capacity();
}