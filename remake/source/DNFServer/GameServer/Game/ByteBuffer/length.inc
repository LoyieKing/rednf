unsigned int ByteBuffer::length() const
{
    return static_cast<unsigned int>(m_writePtr - m_start);
}
