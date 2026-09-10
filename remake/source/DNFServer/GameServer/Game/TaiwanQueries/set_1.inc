void SigLoadGeolocation::set(char* country)
{
    strncpy(m_countries[m_count], country, 15);
    ++m_count;
