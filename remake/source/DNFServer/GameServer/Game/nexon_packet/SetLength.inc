// ORIG 0x086941e8 (sz 0x22): m_length = htons(length);
void stHeader::SetLength(unsigned short length)
{
    m_length = htons(length);
}