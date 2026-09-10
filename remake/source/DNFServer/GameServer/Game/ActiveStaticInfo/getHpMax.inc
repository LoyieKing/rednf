int ActiveStaticInfo::getHpMax()
{
    return static_cast<int>(static_cast<double>(m_hpBase) *
                            static_cast<double>(m_hpRatio));
