// ============================================================================
// STGainedEachExpData / getMonsterTotalExp / sumGainedEachExp
// ============================================================================
// ORIG _ZN19STGainedEachExpDataC1Ev @ 0x08151a56（W，C2 同址）：
//   movl $0x0,(%eax)           → m_exp = 0
//   movl $0x3f800000,0x4(%eax) → m_expPartyDivisor = 1.0f（0x3f800000）
// ORIG 无 D1/D2（平凡析构）。
STGainedEachExpData::STGainedEachExpData()
    : m_exp(0), m_expPartyDivisor(1.0f)
{
}