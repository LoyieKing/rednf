int CNCryptoShift::Decrypt(const unsigned char* in, int inLen,
                           unsigned char* out, int outCap)
{
    if (m_shift >= 0x20)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (inLen % blockSize != 0)
        return 0x70000006;
    if (inLen > outCap)
        return 0x7000000a;
    const unsigned int* src = (const unsigned int*)in;
    unsigned int* dst = (unsigned int*)out;
    int count = inLen / GetBlockSize();
    for (int i = 0; i < count; ++i)
    {
        unsigned int v = *src;
        ++src;
        v ^= m_key;
        ROL(v, (int)m_shift);   // ORIG 丢弃返回值（原码遗留，P171 照抄）
        *dst = v;
        ++dst;
    }
    return 0x6fffffff;
