void _Additioal_info::add(const _Additioal_info& other)
{
    m_field00 += other.m_field00;
    m_field04 += other.m_field04;
    m_field08 += other.m_field08;
    m_field0a += other.m_field0a;
    m_field0c += other.m_field0c;
    m_field0e += other.m_field0e;
    for (int i = 0; i <= 3; ++i)
        m_field10[i] += other.m_field10[i];
    for (int j = 0; j <= 0x10; ++j)
        m_field18[j] += other.m_field18[j];
    m_field3a += other.m_field3a;
    m_field3e += other.m_field3e;
    m_field40 += other.m_field40;
    m_field42 += other.m_field42;
    m_field46 += other.m_field46;
    m_field48 += other.m_field48;
    m_field4a += other.m_field4a;
    m_field4c += other.m_field4c;
    m_field4e += other.m_field4e;
