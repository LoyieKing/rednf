void CHackAnalyzer::setUserP(CUser* user)
{
    // ORIG T 0x080f79c0（setUserP.md）：*(this+0) = user（与 beginCollectHackInfo
    // 共用首槽槽位复用）。
    // 指针赋值形（ORIG 先 load this 再 load 实参；int cast 形会先物化实参）。
    *(CUser**)&m_baseHackType = user;
