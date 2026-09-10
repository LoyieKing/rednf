CMission_remain_hp::CMission_remain_hp(const MissionScript& script, int hp,
                                       int clear)
    : CMission(script)
{
    m_remainHp = hp;
    m_clear = clear;
