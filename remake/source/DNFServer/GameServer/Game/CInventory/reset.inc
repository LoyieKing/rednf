void CInventory::reset()
{
    m_pParent = 0;
    m_money = 0;
    m_coin = 1;
    m_payCoin = 0;
    m_eventCoin = 0;
    m_field14 = 0;
    memset(m_invenItem, 0, 0x632);
    if (m_pEquipSlot != 0)
    {
        memset(m_pEquipSlot, 0, 0x4a58);
    }
    if (m_pAvatarSlot != 0)
    {
        memset(m_pAvatarSlot, 0, 0x1905);
    }
    if (m_pCreatureSlot != 0)
    {
        memset(m_pCreatureSlot, 0, 0x39aa);
    }
    m_creatureMgr.Reset();
    m_creatureMgr.SetUser(m_pParent);
    m_avatarItemMgr.Reset();
    memset(m_clearAvatar, 0, 0x28);
    m_inventoryCapacity = 0;
    m_avatarCoin.Set(0);
    for (int i = 0; i < 12; i++)
    {
        m_avatarVisible[i].init();
    }
