// ORIG _ZN5CUser12SendCashDataEb T 0x0865082a：包 0x35（商城货币数据；
// flag=1 时全 0，否则 Cera/Mileage/CeraPoint）。
void CUser::SendCashData(bool flag)
{
    PacketGuard pg;
    pg.put_header(0, 0x35);
    pg.put_byte(1);
    if (flag)
    {
        pg.put_int(0);
        pg.put_int(0);
        pg.put_int(0);
    }
    else
    {
        pg.put_int(GetCera());
        pg.put_int(GetMileage());
        pg.put_int(GetCeraPoint());
    }
    pg.finalize(true);
    Send(pg);
}