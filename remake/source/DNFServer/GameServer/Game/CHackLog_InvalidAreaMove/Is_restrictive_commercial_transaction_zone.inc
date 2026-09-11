// ORIG 0x086c4568
//   _Z42Is_restrictive_commercial_transaction_zoneiiii
//   自由函数；点 (x2,y2) 在以 (x1,y1) 为中心、半宽 0x50/半高 0x96 的矩形内
bool Is_restrictive_commercial_transaction_zone(int x1, int y1, int x2, int y2)
{
    return (y1 - 0x96 < y2) && (y1 + 0x96 > y2) &&
           (x1 - 0x50 < x2) && (x1 + 0x50 > x2);
}