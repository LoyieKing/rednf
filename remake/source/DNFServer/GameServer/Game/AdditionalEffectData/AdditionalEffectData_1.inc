AdditionalEffectData::AdditionalEffectData(AdditionalEffectData const& other)
    : m_id(other.m_id), m_strings(other.m_strings)
{
    m_mode = other.m_mode;
    m_count = other.m_count;
    m_min = other.m_min;
    m_max = other.m_max;
    m_valueA = other.m_valueA;
    m_valueB = other.m_valueB;
    m_valueC = other.m_valueC;
    m_flag = other.m_flag;
