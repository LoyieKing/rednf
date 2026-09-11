int ActiveStaticInfo::getMpMax()
{
    return static_cast<int>(static_cast<double>(m_mpBase) *
                            static_cast<double>(m_mpRatio));
}