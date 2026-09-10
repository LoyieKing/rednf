unsigned short CPad::GetSelectedCipherIdx()
{
    unsigned short limit = 0x1387;
    return std::min(m_keyIdx2e, limit);
