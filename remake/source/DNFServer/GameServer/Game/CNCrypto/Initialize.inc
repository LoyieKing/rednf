int CNCryptoShift::Initialize(const unsigned char* key, int keyLen)
{
    if (keyLen < 8)
        return 0;
    // ORIG 0x0809c4fe：32 位直读 + 形参自增（addl $0x4,0xc(%ebp)）。
    m_shift = *(const unsigned int*)key & 0x1f;
    key += 4;
    m_key = *(const unsigned int*)key;
    return 0x6fffffff;
