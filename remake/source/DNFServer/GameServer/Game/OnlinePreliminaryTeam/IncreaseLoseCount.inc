// ORIG T 0x0858841a：++lose(@0x10)；+0x3c 置 1（ORIG 同写 0x3c，
// 非 0x3d——两条计数共用 dirty 标志）。
void COnlinePreliminaryTeam::IncreaseLoseCount()
{
    ++m_loseCount;
    m_hasWinFlag = 1;
}