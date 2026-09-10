unsigned int PISenderManager::MakeHashForSending(unsigned char* data,
                                                 unsigned int len,
                                                 bool bNewKey)
{
    if (bNewKey)
        reinterpret_cast<PISenderPadView*>(this)->m_pad[0] = 0x27;
    unsigned char key =
        reinterpret_cast<PISenderPadView*>(this)->m_pad[0];
    unsigned char rnd = static_cast<unsigned char>(rand());
    reinterpret_cast<PISenderPadView*>(this)->m_pad[0] = rnd;

    unsigned int nHash = 0;
    unsigned int ret = 0;
    unsigned char h = 0;
    if (len == 0)
        h = static_cast<unsigned char>(rand());
    else
        h = MakeHash(data, len, key);

    nHash = static_cast<unsigned short>(
        ((h & 3) << 6) | ((h & 0xc) << 12) | ((h & 0x30) >> 2) |
        ((h & 0xc0) << 4) | ((rnd & 3) << 12) | ((rnd & 0xc) << 2) |
        ((rnd & 0x30) << 4) | ((rnd & 0xc0) >> 6));
    ret = (nHash << 8) | (rand() & 0xff);
    return ret;
