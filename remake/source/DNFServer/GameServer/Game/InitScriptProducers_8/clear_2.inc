// ORIG clear 0x0891b3fe：+0=0、+4=-1、+8=0、+0xc=0、vecGive.clear()、
// +0x1c=0、vecBonus.clear()。注意：不清 m_lottery（+0x2c），忠实保留。
void stVendingMachineInfo_t::clear()
{
    m_groupNum = 0;
    m_payType = -1;
    m_payAmount = 0;
    m_giveCount = 0;
    m_vecGive.clear();
    m_bonusCount = 0;
    m_vecBonus.clear();
}