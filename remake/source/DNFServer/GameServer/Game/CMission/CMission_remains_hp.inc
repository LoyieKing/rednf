CMission_remains_hp::CMission_remains_hp(const MissionScript& script, int hp,
                                         int clear)
    : CMission(script)
{
    m_remainHp = hp;
    m_clear = clear;
}