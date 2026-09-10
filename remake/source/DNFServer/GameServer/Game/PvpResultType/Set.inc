void PvpResultType::Set(const PvpResultType& other)
{
    *this = other;
    if (m_expPoint < 0)
    {
        m_expPoint = 0;
        m_pvpGrade = 0;
    }
