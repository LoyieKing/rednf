// ORIG 0x086940d2 (sz 0x1f): m_offset=offset; m_size=size; m_buffer=buffer
CPacketHelper::CPacketHelper(unsigned char* buffer, int size, int startOffset)
    : m_offset(startOffset), m_size(size), m_buffer(buffer)
{
}