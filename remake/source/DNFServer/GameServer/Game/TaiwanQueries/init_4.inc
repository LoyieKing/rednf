void SigLoadRestrictPolicy::init()
{
    m_restrictValue = 0;
    m_count = 0;
    memset(m_data, 0, sizeof(m_data));
    memset(m_extra, 0, sizeof(m_extra));
