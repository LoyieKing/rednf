// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_secret_shop_event_buff —— ORIG 0x08196dd3（报告全文转写）。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: P0/P52 — ORIG 无 area/vill 具名局部（get_area 结果
// 留 %ebx、getCurCharacVill 留 %esi 跨 G_GameWorld() 保活），实参内联单语句。
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
}