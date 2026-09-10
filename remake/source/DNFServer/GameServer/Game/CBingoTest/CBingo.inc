CBingo::CBingo()
    : m_boards(),
      m_rand(new CMTRand(static_cast<unsigned long>(time(0))))
{
    m_boards.clear();
    m_fieldC = 0;
    m_today = 0;
