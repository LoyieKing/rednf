bool StringData::isShared() const
{
    return m_refCount > 1;
