int SECRET_SHOP_DATA::IsCompleteBuy()
{
    for (int i = 0; i < 4; ++i)
    {
        if (m_buying[i] != 0)
        {
            return 0;
        }
    }
    return 1;
}