CMission_kill::CMission_kill(const MissionScript& script, int minKill,
                             int clear)
    : CMission(script)
{
    m_minKill = minKill;
    m_clear = clear;
