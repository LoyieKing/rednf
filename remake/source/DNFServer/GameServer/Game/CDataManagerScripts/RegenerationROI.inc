RegenerationROI::RegenerationROI()
    : m_regenerator()
{
    // ORIG C1 0x85f8486：构造 +0x04 regenerator 后 new CMTRand(0x9c8) 存 +0x00；
    // 不初始化 +0x50。
    m_regeneratorPtr = new CMTRand();
}