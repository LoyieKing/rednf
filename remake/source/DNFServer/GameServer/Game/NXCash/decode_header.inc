inline bool NCashMessage::decode_header()
{
    m_bodyLength = ntohs(*reinterpret_cast<unsigned short*>(m_netLenB));
    if (m_bodyLength > 0x200)
    {
        m_bodyLength = 0;
        return false;
    }
    return true;
