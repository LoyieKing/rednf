// ============================================================================
// reset @ 0x08899ef6：清两个 map + TimeLine::reset（0x08899dfa，位于
// advancealtar_StageControl.cpp）。无其它副作用。
// ============================================================================
void StageTimeLineParameter::reset()
{
    m_summonMap.clear();
    m_actionMap.clear();
    m_timeLine.reset();
}