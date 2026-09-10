bool Is_restrictive_commercial_transaction_zone(int x1, int y1, int x2, int y2)
{
    return (y1 - 0x96 < y2) && (y1 + 0x96 > y2) &&
           (x1 - 0x50 < x2) && (x1 + 0x50 > x2);
