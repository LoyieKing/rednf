inline bool NCashMessage::encode_header()
{
    m_wireMagic = 'U';
    *reinterpret_cast<unsigned short*>(m_netLenB) = htons(m_bodyLength);
    return true;
