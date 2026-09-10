void Arad_IPGProcess::CashShopModeCoin(CUser* user, unsigned short param2,
                                       unsigned int param3, int param4, char buyFlag)
{
    if (buyFlag == 1)
    {
        // esp: shop / user / (uint)param2 / param3 —— mangled (CUser*, t, j, i)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCoinGift(user, param2, param3, param4);
    }
    else
    {
        // esp: shop / user / (uint)param2 / 1 —— mangled (CUser*, t, b)；bool 实参 = 1
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCoin(user, param2, true);
    }
