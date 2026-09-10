template <> void cElection<int, 8, 8>::cVoter::Init()
{
    std::fill(m_votes, m_votes + 8, 0);   // ORIG 0x85e0304
