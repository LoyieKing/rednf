void CBlowFish::Decrypt(SBlock& block)
{
    unsigned int l = block.m_data0;
    unsigned int r = block.m_data1;
    l ^= m_pbox[17];
    r ^= F(l) ^ m_pbox[16];
    l ^= F(r) ^ m_pbox[15];
    r ^= F(l) ^ m_pbox[14];
    l ^= F(r) ^ m_pbox[13];
    r ^= F(l) ^ m_pbox[12];
    l ^= F(r) ^ m_pbox[11];
    r ^= F(l) ^ m_pbox[10];
    l ^= F(r) ^ m_pbox[9];
    r ^= F(l) ^ m_pbox[8];
    l ^= F(r) ^ m_pbox[7];
    r ^= F(l) ^ m_pbox[6];
    l ^= F(r) ^ m_pbox[5];
    r ^= F(l) ^ m_pbox[4];
    l ^= F(r) ^ m_pbox[3];
    r ^= F(l) ^ m_pbox[2];
    // ORIG 末轮形态（Decrypt 对称：存 l）
    block.m_data0 = r ^ m_pbox[0];
    block.m_data1 = l;
    // ORIG 末轮重读形态（F(r)/P[1] 各算两遍）
    block.m_data0 = r ^ m_pbox[0];
    block.m_data1 = l ^ F(r) ^ m_pbox[1];
}