ByteBuffer::ByteBuffer(char* buffer, unsigned int len)
    : m_start(buffer), m_writePtr(buffer), m_readPtr(buffer), m_capacity(len)
{
