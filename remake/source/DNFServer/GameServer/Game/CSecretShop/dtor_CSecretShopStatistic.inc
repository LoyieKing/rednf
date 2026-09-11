CSecretShopStatistic::~CSecretShopStatistic()
{
    for (int i = 0; i < 3; ++i)
        m_pos[i].clear();
}