ProcStage::ProcStage(CharacData& characData)
    : m_characData(&characData),
      m_state(State::S_N7),
      m_field8(0),
      m_fieldc(0),
      m_field10(0),
      m_field14(0),
      m_summonObjectMs(*m_characData, m_field8),
      m_createdMapMonsters(),
      m_difficulty(static_cast<StageDifficulty::T>(-1)),
      m_mapBasisLevel(0),
      m_dungeon(0),
      m_map(0),
      m_survivalFirst(1),
      m_survivalRound(0),
      m_roundMaps(),
      m_startStageTime(0),
      m_timeLine(0)
{
