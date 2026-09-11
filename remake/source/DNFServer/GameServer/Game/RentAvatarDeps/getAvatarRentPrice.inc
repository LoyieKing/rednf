// ORIG T _Z18getAvatarRentPriceii @ 0x0898c999 (size 0x39)
// int getAvatarRentPrice(int, int) — 逐指令转写：
//   arg2(type) == 0 或 1 → price
//   == 2                → price * 2
//   == 3                → price * 4（shl 2）
//   其它                → price
int getAvatarRentPrice(int price, int rentType)
{
    if (rentType == 0)
        return price;
    if (rentType == 1)
        return price;
    if (rentType == 2)
        return price * 2;
    if (rentType == 3)
        return price * 4;
    return price;
}