void SECRET_SHOP_DATA::clear()
{
    m_openCode = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_info[i].clear();
        m_buying[i] = 0;
    }
