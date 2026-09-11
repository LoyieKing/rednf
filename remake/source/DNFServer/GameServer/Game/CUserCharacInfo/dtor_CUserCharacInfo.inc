CUserCharacInfo::~CUserCharacInfo()
{
    // 空体：隐式成员逆序析构（m_dungeonClear→m_battleRecord→m_pvpResult =
    // ORIG +0xb4→+0x9c→+0x18 调用序）。显式伪析构调用使 4.4 每调用点再布
    // cleanup 链（+22 条 vs ORIG 43）。
}