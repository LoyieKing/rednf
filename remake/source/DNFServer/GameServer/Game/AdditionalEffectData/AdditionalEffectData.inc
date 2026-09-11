// ORIG C1 0x0891b05e：vector 默认构造 → m_id=-1 → vector clear →
// m_mode=2 → m_count=0 → m_max=-1 → m_min=-1 → m_valueA/B/C=0 → m_flag=0。
AdditionalEffectData::AdditionalEffectData()
    : m_strings()
{
    m_id = -1;
    m_strings.clear();
    m_mode = 2;
    m_count = 0;
    m_max = -1;
    m_min = -1;
    m_valueA = 0;
    m_valueB = 0;
    m_valueC = 0;
    m_flag = 0;
}