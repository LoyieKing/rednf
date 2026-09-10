void CTEA::DecryptBlock(const unsigned char* pucIn, unsigned char* pucOut)
{
    unsigned int v[2];
    unsigned int w[2];
    BytesToWord(pucIn, v[0]);
    BytesToWord(pucIn + 4, v[1]);
    register unsigned int y = v[0];
    register unsigned int z = v[1];
    register unsigned int sum = 0xc6ef3720;
    register unsigned int delta = 0x9e3779b9;
    register unsigned int n = 0x20;
    while (n-- != 0)
    {
        z -= (((y << 4) ^ (y >> 5)) + y) ^ (sum + m_auiKey[(sum >> 0xb) & 3]);
        sum -= delta;
        y -= (((z << 4) ^ (z >> 5)) + z) ^ (sum + m_auiKey[sum & 3]);
    }
    w[0] = y;
    w[1] = z;
    WordToBytes(w[0], pucOut);
    WordToBytes(w[1], pucOut + 4);
