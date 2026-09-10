void Dispatcher_UseBoosterItem::sendBroadCastItems(CUser* user, const Inven_Item& item, int itemId)
{
    if (user == 0)
        return;

    char flag = 0;

    // ORIG：isUnCommonItem/checkUnCommonNestedItem/checkCommonItem 三次分别
    // 重新调用 G_CDataManager()（P48 O0 无 CSE），结果直接 +0xb5bc 使用，不落槽。
    if (G_CDataManager()->m_broadCastItem.isUnCommonItem(itemId))
        flag = G_CDataManager()->m_broadCastItem.checkUnCommonNestedItem(itemId, item.m_addInfo);
    else
        flag = G_CDataManager()->m_broadCastItem.checkCommonItem(item.m_addInfo);

    if (flag)
    {
        PacketGuard pg;
        pg.put_header(0, 0x56);
        pg.put_byte(2);
        pg.put_byte(1);
        pg.put_short((unsigned short)(user->get_unique_id()));   // put_short(uid & 0xffff)
        pg.put_int(item.m_addInfo);
        pg.put_byte(item.GetUpgrade());
        pg.finalize(true);
        G_GameWorld()->send_all(pg);
    }
