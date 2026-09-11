// ORIG T 0x0858838a Reset：+0/+4 清零；memset(+0x14,0,0x10)；
// +8/+0xc/+0x10 清零；clear map；+0x3c/+0x3d 标志清零。
void COnlinePreliminaryTeam::Reset()
{
    m_field00 = 0;
    m_field04 = 0;
    memset(m_field14, 0, 0x10);
    m_rating08 = 0;
    m_winCount = 0;
    m_loseCount = 0;
    m_playCountMap.clear();
    m_hasWinFlag = 0;
    m_hasLoseFlag = 0;
}