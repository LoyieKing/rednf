void TimeLine::reset()
{
    // ORIG T 0x08899dfa：vector<Phase>::clear + TimeLineSummary::reset。
    m_phases.clear();
    m_summary.reset();
