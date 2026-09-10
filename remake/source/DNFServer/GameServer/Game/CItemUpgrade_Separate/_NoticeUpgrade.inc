void CItemUpgrade_Separate::_NoticeUpgrade(const CUser* user, const Inven_Item& item,
                                           bool success)
{
    PacketGuard pg;
    pg.put_header(0, 0x56);
    pg.put_byte(10);
    // ORIG movzbl -0x1c,%eax 直装参：bool 隐式转 int 即零扩展（三目反生岛）。
    pg.put_byte(success);
    pg.put_short(user->get_unique_id());
    pg.put_int(item.m_addInfo);
    pg.put_byte(item.GetUpgrade());
    pg.put_byte(item.m_upgradeSep.GetUpgradeSeparate());
    // Inven_Item+0x25 = m_random(+0x1f).m_random（RandomOption，+0x06）。
    item.m_random.m_random.put_packet_random_option(pg);
    pg.finalize(true);
    G_GameWorld()->send_all(pg);
