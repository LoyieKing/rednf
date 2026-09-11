bool StringData::isUnique() const
{
    return m_refCount == 1;
}