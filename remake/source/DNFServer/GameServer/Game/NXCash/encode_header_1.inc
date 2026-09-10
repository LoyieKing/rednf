inline void NCashMessage::encode_header(unsigned int bodyLength)
{
    m_bodyLength = static_cast<unsigned short>(bodyLength);
    encode_header();
