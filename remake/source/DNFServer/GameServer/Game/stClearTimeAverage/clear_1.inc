void stClearTimeAverageMapKey::clear()
{
    m_a = 0;
    m_b = 0;
}

bool stClearTimeAverageMapKey::operator<(const stClearTimeAverageMapKey& o) const
{
    if (m_a < o.m_a)
        return true;
    if (m_a == o.m_a && m_b < o.m_b)
        return true;
    return false;
}