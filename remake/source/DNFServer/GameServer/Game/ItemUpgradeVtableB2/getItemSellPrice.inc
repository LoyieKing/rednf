int getItemSellPrice(int price, int a, int b, bool flag)
{
    unsigned char sel = 0;
    if (flag)
    {
        sel = 2;
    }

    if (a > b)
    {
        a = b;
    }

    if (b == 0)
    {
        return itemPriceRates_[sel] * price / 1000;
    }
    return itemPriceRates_[sel] * price / 1000 * a / b;
