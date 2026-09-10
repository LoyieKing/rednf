STAgitUpgradeInfo* GuildParameterScript::getGuildAgitUpgradeInfo(int level)
{
    if (level - 1 < (int)m_vec2c.size())
        return &m_vec2c.at(level - 1);   // ORIG 用 at（含界检），非 operator[]
    return 0;
