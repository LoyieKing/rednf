void ARAD_ITEM::arad_secret_shop_event_buff(CUser* user, unsigned long param)
{
    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0x100);
    pg.put_byte(1);
    pg.put_int(param);
    pg.put_int(0x708);
    pg.finalize(true);
    user->Send(pg);

    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                pg);
