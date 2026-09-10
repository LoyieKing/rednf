inline unsigned char* NCashMessage::data()
{
    return &m_wireMagic;  // this+4
