void CBlowFish::Encrypt(SBlock& block)
{
    unsigned int l = block.m_data0;
    unsigned int r = block.m_data1;
    l ^= m_pbox[0];
    r ^= F(l) ^ m_pbox[1];
    l ^= F(r) ^ m_pbox[2];
    r ^= F(l) ^ m_pbox[3];
    l ^= F(r) ^ m_pbox[4];
    r ^= F(l) ^ m_pbox[5];
    l ^= F(r) ^ m_pbox[6];
    r ^= F(l) ^ m_pbox[7];
    l ^= F(r) ^ m_pbox[8];
    r ^= F(l) ^ m_pbox[9];
    l ^= F(r) ^ m_pbox[10];
    r ^= F(l) ^ m_pbox[11];
    l ^= F(r) ^ m_pbox[12];
    r ^= F(l) ^ m_pbox[13];
    l ^= F(r) ^ m_pbox[14];
    r ^= F(l) ^ m_pbox[15];
    // ORIG 末轮形态（汇编实证 0x080a9984-0x080a99ab：F(r8)^P[16] 更新 l 后
    // 直接存 l，out0 = r^P[17]）
    block.m_data0 = r ^ m_pbox[17];
    block.m_data1 = l;
    // ORIG 末轮重读形态（O0 无 CSE，F(r)/P[16] 各算两遍，语义为输出交换）
    block.m_data0 = r ^ m_pbox[17];
    block.m_data1 = l ^ F(r) ^ m_pbox[16];
