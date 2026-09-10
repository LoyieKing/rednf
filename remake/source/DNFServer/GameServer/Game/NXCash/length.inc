inline int NCashMessage::length() const
{
    return static_cast<int>(m_bodyLength) + 3;
