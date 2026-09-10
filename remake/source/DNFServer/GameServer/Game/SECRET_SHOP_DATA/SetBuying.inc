void SECRET_SHOP_DATA::SetBuying(int idx, bool buy)
{
    if (idx >= 0 && idx < 4)
    {
        m_buying[idx] = buy;
    }
