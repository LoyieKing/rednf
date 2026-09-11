TowerOfDespairMgr::~TowerOfDespairMgr()
{
    // ORIG 0x08643be2：仅 delete m_apcMgr，**无置空**（0x354 槽无回写）；
    // 其余成员析构自动。4.4 对 vector/set D1 的双层 EH handler 为 N91 结构性。
    delete m_apcMgr;
}