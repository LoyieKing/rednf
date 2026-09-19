int CNCryptoRc6::Initialize(const unsigned char* key, int keyLen)
{
    if (!m_pCrypto)
        return 0x7000000c;
    if (keyLen < 0x3c)
        return 0x70000002;
    RC6_ALG_INFO* c = (RC6_ALG_INFO*)m_pCrypto;
    // [DNF-FIX-RC6 2026-09-19] ORIG 0x0809bc84 逐条实证：
    //   memcpy(m_pCrypto+0x00, key,      0x20)   ← K 之前的 key 区
    //   addl $0x20,key                           ← **源指针前移 32**（原注释漏了这一步）
    //   memcpy(m_pCrypto+0x20, key+0x20, 0x2c)   ← K 区（44 字节）取自 key+0x20
    //   rc6_key_setup(c, 0x20)
    // 旧码把第二段写成 `memcpy(c->K, key, 0x2c)`（源未前移）⇒ K 初值错 ⇒ RC6 加解密与
    // 客户端不一致（表现为 id%14==3 的包校验和恒不匹配，如 DPROTO_CALLBACK=605）。
    std::memcpy(c->key, key, 0x20);
    std::memcpy(c->K, key + 0x20, 0x2c);
    rc6_key_setup(c, 0x20);
    return 0x6fffffff;
}