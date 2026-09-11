int CNCryptoBlowFish::Initialize(const unsigned char* key, int keyLen)
{
    // ORIG 0x080999a8..0x08099b29：按 keyLen 两路 new CBlowFish（>=0x38 用原 key，
    // <0x38 先 memset 0x38 + memcpy keyLen 补零），成功后 ::operator delete 旧指针
    // 并换入新对象；new 失败返回 0x7000000d，成功 0x6fffffff。
    if (keyLen < 1)
        return 0x70000002;
    if (keyLen > 0x37)
    {
        SBlock chain(0, 0);
        CBlowFish* c = new CBlowFish((unsigned char*)key, 0x38, chain);
        if (c != 0)
        {
            ::operator delete(m_pCrypto);
            m_pCrypto = c;
        }
        else
        {
            return 0x7000000d;
        }
    }
    else
    {
        unsigned char buf[0x38];
        std::memset(buf, 0, 0x38);
        std::memcpy(buf, key, keyLen);
        SBlock chain(0, 0);
        CBlowFish* c = new CBlowFish(buf, 0x38, chain);
        if (c != 0)
        {
            ::operator delete(m_pCrypto);
            m_pCrypto = c;
        }
        else
        {
            return 0x7000000d;
        }
    }
    return 0x6fffffff;
}