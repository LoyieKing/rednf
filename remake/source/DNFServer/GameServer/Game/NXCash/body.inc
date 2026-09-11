inline unsigned char* NCashMessage::body()
{
    return &m_bodyBytes[0];  // this+7
}