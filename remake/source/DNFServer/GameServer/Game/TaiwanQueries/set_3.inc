// ORIG W 0x0815de38：整结构拷贝（两段 memcpy 同型展开）。
void SigLoadRestrictPolicy::set(SigLoadRestrictPolicy& other)
{
    m_restrictValue = other.m_restrictValue;
    m_count = other.m_count;
    memcpy(m_data, other.m_data, sizeof(m_data));
    memcpy(m_extra, other.m_extra, sizeof(m_extra));
}