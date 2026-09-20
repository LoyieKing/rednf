unsigned char* StringData::getBuffer()
{
    return reinterpret_cast<unsigned char*>(m_buf);
}