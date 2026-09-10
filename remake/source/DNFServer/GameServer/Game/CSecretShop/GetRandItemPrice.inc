int SecretShopScript::GetRandItemPrice(int price)
{
    int m = rand() % 3;
    if (m == 0)
        return price + (int)((double)price * ((double)m_priceVarPercent / 100.0));
    if (m == 1)
        return price - (int)((double)price * ((double)m_priceVarPercent / 100.0));
    return price;
