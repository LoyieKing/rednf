// ORIG 0x081ae5ae：bodyLength 参数直存 +0x02 后调用 encode_header()。
inline void NCashMessage::encode_header(unsigned int bodyLength)
{
    m_bodyLength = static_cast<unsigned short>(bodyLength);
    encode_header();
}