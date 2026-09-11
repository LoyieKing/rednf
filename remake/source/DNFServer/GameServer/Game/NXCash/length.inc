// ORIG 0x081ae54e：return u16(this+2)+3。
inline int NCashMessage::length() const
{
    return static_cast<int>(m_bodyLength) + 3;
}