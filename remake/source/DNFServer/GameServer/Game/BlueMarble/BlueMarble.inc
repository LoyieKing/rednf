BlueMarble::BlueMarble()
    : m_memoryPoolIndex(-1), m_state(BlueMarbleState::T_0), m_diceNumber(0), m_blueMarbleMap(0),
      m_pScript(0), m_maxBoardSize(0)
{
    m_pRands = new CMTRand[4];
    volatile unsigned long seedValue = (unsigned long)time((time_t*)0);
    for (int i = 0; i <= 3; ++i)
    {
        m_pRands[i].seed(seedValue);
    }
    m_users.clear();
    m_set.clear();
