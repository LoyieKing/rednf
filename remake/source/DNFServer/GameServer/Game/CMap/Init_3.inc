template <> void cElection<int, 4, 4>::cVoter::Init()
{
    std::fill(m_votes, m_votes + 4, 0);   // ORIG 0x82beb62
