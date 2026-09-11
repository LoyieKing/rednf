// ORIG T 0x084600a6：header(1,0x32)；byte(1)；byte(order)；short(xy)；int(key)；
// finalize(true)。
void CDeathTower::CPacketHandler::makeDropItem(char order, int xy, int itemKey)
{
    put_header(1, 0x32);
    put_byte(1);
    put_byte((int)(signed char)order);
    put_short(xy);
    put_int(itemKey);
    finalize(true);
}