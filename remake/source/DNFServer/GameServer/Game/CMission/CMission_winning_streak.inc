CMission_winning_streak::CMission_winning_streak(const MissionScript& script,
                                                 int streak, int clear)
    : CMission(script)
{
    m_streak = streak;
    m_clear = clear;
}