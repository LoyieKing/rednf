// [b23S3C] ORIG T 0x080c339c 全量重转写：旧形为猜测版（缺 +0x14 写/双 memcmp 复用
// 判定/BytesToWord×4），参数语义照 ORIG：第 3 参是与 m_chain2 比较的另一 key，
// 第 4/5 参分别落 +0x10/+0x14 并做 0..2 值域守卫。
int CTEA::Initialize(const char* key, int keyLen, const char* otherKey,
                     int mode1, int mode2)
{
    if (key == 0)
        return 0x70000001;
    if (keyLen < 1)
        return 0x70000002;
    if (mode1 < 0 || mode1 > 2)
        return 0x70000003;
    if (mode2 < 0 || mode2 > 2)
        return 0x70000004;

    m_mode = mode1;
    m_padding = mode2;

    // 循环填充 buf（ORIG -0x24；圈计数用 this->m_keyLength，取模用 keyLen）。
    unsigned char buf[0x10];
    int i = 0;
    int src = 0;
    for (; i < m_keyLength; src = (src + 1) % keyLen)
    {
        buf[i] = (unsigned char)key[src];
        ++i;   // [b23S3C] ORIG 0x84 i++ 在体内、src 取模在增补段（装载序实证）
    }

    bool sameKey = false;
    bool sameChain = false;
    if (m_bInit)
    {
        if (std::memcmp(m_chain, otherKey, m_blockSize) == 0)
            sameChain = true;
        if (std::memcmp(m_key, buf, m_keyLength) == 0)
            sameKey = true;
    }
    if (sameChain)
    {
        std::memcpy(m_chain2, m_chain, m_blockSize);
    }
    else
    {
        std::memcpy(m_chain, otherKey, m_blockSize);
        std::memcpy(m_chain2, otherKey, m_blockSize);
    }
    if (sameKey)
        return 0x6fffffff;

    std::memcpy(m_key, buf, m_keyLength);
    IMethod::BytesToWord(buf + 0x0, m_iv[0]);
    IMethod::BytesToWord(buf + 0x4, m_iv[1]);
    IMethod::BytesToWord(buf + 0x8, m_iv[2]);
    IMethod::BytesToWord(buf + 0xc, m_iv[3]);
    m_bInit = true;
    return 0x6fffffff;
}