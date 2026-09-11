// ---- GuildParameterScript::getGuildAgitUpgradeInfo（ORIG T 0x089796dc）----
// m_vec2c（STAgitUpgradeInfo[agit upgrade info]）取下标 level-1；
// level-1 >= size 返回 NULL（ORIG cmp/setl 分支一致）。
STAgitUpgradeInfo* GuildParameterScript::getGuildAgitUpgradeInfo(int level)
{
    if (level - 1 < (int)m_vec2c.size())
        return &m_vec2c.at(level - 1);   // ORIG 用 at（含界检），非 operator[]
    return 0;
}