// CBingo::CBingo @ 0x080ca82c（docs/class_func_reports/CBingo/CBingo.md 全文已读）：
//   vector<int> C1(this) → time(0) → new CMTRand(0x9c8, seed) →
//   shared_ptr<CMTRand>(this+0x10, px) → vector::clear(this) → +0xc=0 → +0x18=0。
CBingo::CBingo()
    : m_boards(),
      m_rand(new CMTRand(static_cast<unsigned long>(time(0))))
{
    m_boards.clear();
    m_fieldC = 0;
    m_today = 0;
}