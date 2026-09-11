int CNCryptoShift::Encrypt(unsigned char* buf, int len)
{
    if (m_shift >= 0x20)
        return 0x7000000c;
    int blockSize = GetBlockSize();
    if (len % blockSize != 0)
        return 0x70000006;
    unsigned int* p = (unsigned int*)buf;
    int count = len / GetBlockSize();
    for (int i = 0; i < count; ++i)
    {
        unsigned int v = *p;
        ROR(v, (int)m_shift);   // ORIG 丢弃返回值（原码遗留，P171 照抄）
        v ^= m_key;
        *p = v;
        ++p;
    }
    return 0x6fffffff;
}