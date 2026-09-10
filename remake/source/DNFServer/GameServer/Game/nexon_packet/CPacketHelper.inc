CPacketHelper::CPacketHelper(unsigned char* buffer, int size, int startOffset)
    : m_offset(startOffset), m_size(size), m_buffer(buffer)
{
