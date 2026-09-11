unsigned int CBlowFish::F(unsigned int x)
{
    unsigned char b1 = Byte(x >> 24);
    unsigned char b2 = Byte(x >> 16);
    unsigned char b3 = Byte(x >> 8);
    unsigned char b4 = Byte(x);
    return ((m_sbox[0][b1] + m_sbox[1][b2]) ^ m_sbox[2][b3]) + m_sbox[3][b4];
}