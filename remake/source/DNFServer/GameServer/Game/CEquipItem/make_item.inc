void CEquipItem::make_item(Inven_Item& item) const
{
    if (IsAvatarItem())
    {
        item.m_field1 = 8;
    }
    else
    {
        item.m_field1 = 1;
    }
    if (m_itemType == 0x16)
    {
        item.m_field1 = 5;
    }
    else if (m_itemType == 0x17 || m_itemType == 0x18 || m_itemType == 0x19)
    {
        item.m_field1 = 6;
    }
    item.set_add_info(get_rand_int(0));
    item.ResetItemAttr();
    item.m_fieldb = (unsigned short)m_randomTable.m_endurance;
    if (item.m_field1 == 8)
    {
        if (m_avatarSelectIdx > 0 && m_avatarSelectIdx <
            (int)((std::vector<stAvatarTypeSelect_t>*)getAvatarTypeSelect())->size())
        {
            item.m_fieldb = (unsigned short)m_avatarSelectIdx;
        }
    }
    if (m_attachType == 3)
    {
        item.m_field0 = 1;
    }
    if (PeriodAfterUnsealing() == 0)
    {
        int t = 0;
        if (getUsablePeriod() != 0)
        {
            t = GlobalData::s_systemTime_.getCurSec() +
                getUsablePeriod() * 0x15180;
        }
        else if (getExpirationDate() != 0)
        {
            t = getExpirationDate();
        }
        if (t != 0 && m_itemType != 10)
        {
            item.m_fieldb = (unsigned short)((t - 0x44a54a80) / 0x15180);
        }
    }
    item.m_fieldd = 0;
    // ORIG（make_item）：(`inven` + 0x15) 处 0x11 字节打包全局唯一标识，位于
    // Inven_Item 的 m_amp(+0x11).m_pad4 起始（即 &item + 0x15），跨 ampen/pad 与
    // m_random 的交叠打包区。保留地址形式（打包交叠区非独立具名成员），锚定到
    // 具名子成员 m_amp.m_pad4 使偏移可审计。
    ((itemGloballyUniqueIdentifier_t*)(char*)&item.m_amp.m_pad4[0])->reset();
    if ((unsigned int)m_itemType < 0x1a &&
        ((1 << m_itemType) & 0x3bffc00U) != 0)
    {
        g_ItemGloballyUniqueIdentifierGenerator.generate(
            (itemGloballyUniqueIdentifier_t*)(char*)&item.m_amp.m_pad4[0],
            G_CEnvironment()->m_serverEnvir.m_gcNo);
    }
    item.m_amp.reset();
