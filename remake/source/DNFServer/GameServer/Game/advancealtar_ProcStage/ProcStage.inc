// ----------------------------------------------------------------------------
// ORIG T 0x0812dc18（ProcStage.md 全文）：存参 characData；state=7；
//   field8/fieldc/field10=0；field14=0（short）；ProcSummonObjectMs ctor
//   (this+0x18, *characData, &field8)；map(+0x50) 构造；difficulty=-1；
//   mapBasisLevel/dungeon/map=0；survivalFirst=1；survivalRound=0；
//   vector(+0x80) 构造；startStageTime=0；timeLine=0。
// ----------------------------------------------------------------------------
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
}