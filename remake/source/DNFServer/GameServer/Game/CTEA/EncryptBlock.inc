int CTEA::EncryptBlock(unsigned char const* src, unsigned char* dst)
{
    unsigned int v0;
    unsigned int v1;
    BytesToWord(src, v0);
    BytesToWord(src + 4, v1);
    const unsigned int* key = reinterpret_cast<const unsigned int*>(m_key);
    unsigned int sum = 0;
    const unsigned int delta = 0x9e3779b9;
    for (int n = 0; n < 32; ++n)
    {
        sum += delta;
        v0 += ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
        v1 += ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
    }
    WordToBytes(v0, dst);
    WordToBytes(v1, dst + 4);
    return 0;
