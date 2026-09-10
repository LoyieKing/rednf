unsigned int CInGameAdvertisementManager::RandNum(int range)
{
    return static_cast<unsigned int>(m_rand->randInt()) %
           static_cast<unsigned int>(range);
