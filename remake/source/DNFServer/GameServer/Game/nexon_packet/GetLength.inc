// ORIG 0x0869420a (sz 0x1a): return ntohs(m_length);
unsigned short stHeader::GetLength()
{
    return ntohs(m_length);
}