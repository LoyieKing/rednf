ProcStage::ProcSummonObjectMs::ProcSummonObjectMs(CharacData& characData,
                                                  int const& currentMs)
    : m_currentMs(0), m_summons(), m_summonMap(),
      m_deadCount(0), m_aliveDelta(0),
      m_characData(&characData), m_pCurrentMs(const_cast<int*>(&currentMs))
{
}