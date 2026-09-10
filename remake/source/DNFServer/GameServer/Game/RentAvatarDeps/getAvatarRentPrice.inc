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
