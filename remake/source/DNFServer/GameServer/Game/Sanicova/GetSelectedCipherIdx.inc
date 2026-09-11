// ORIG T 0x0859984c：min(+0x2e, 0x1387)。
unsigned short CPad::GetSelectedCipherIdx()
{
    unsigned short limit = 0x1387;
    return std::min(m_keyIdx2e, limit);
}