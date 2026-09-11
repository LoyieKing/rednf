// ORIG set 0x85d48a0：
//   this+0x134 = param+0x10 (m_pvpExpLoss1)
//   this+0x138 = param+0x14 (m_pvpExpLoss2)
//   this+0x13c = param+0x18 (m_rankRateGrade)
//   vector@+0x140 operator= param+0x1c (m_rankRate)
void RefPvpGrade::set(const stPvpRefTable_t& refTable)
{
    m_pvpExpLoss1   = refTable.m_pvpExpLoss1;
    m_pvpExpLoss2   = refTable.m_pvpExpLoss2;
    m_rankRateGrade = refTable.m_rankRateGrade;
    m_rankRate      = refTable.m_rankRate;
}