void SigLoadGeolocation::setData(SigLoadGeolocation& other)
{
    m_count = other.m_count;
    memcpy(&m_countries, &other.m_countries, sizeof(m_countries));
