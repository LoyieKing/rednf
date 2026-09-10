int CNCryptoRc6::Initialize(const unsigned char* key, int keyLen)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (keyLen < 0x3c)
        return 0x70000002;
    RC6_ALG_INFO* c = (RC6_ALG_INFO*)m_pCrypto;
    // ORIG 0x809bc84：memcpy(+0x0, key, 0x20) + memcpy(+0x20, key, 0x2c)
    // （K 区初始 = key 前 0x2c 字节）→ rc6_key_setup(c, 0x20)。
    std::memcpy(c->key, key, 0x20);
    std::memcpy(c->K, key, 0x2c);
    rc6_key_setup(c, 0x20);
    return 0x6fffffff;
