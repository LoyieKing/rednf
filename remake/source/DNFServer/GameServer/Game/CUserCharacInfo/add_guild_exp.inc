// [b28d-solo-CUserCharacInfo AE] 21=21：P274 异型视角（m_guildExpU）复现 ORIG
// 0x08645cb1 `mov 0xde9(%edx),%ecx; mov 0xc(%ebp),%edx; lea (%ecx,%edx,1),%edx`。
void CUserCharacInfo::add_guild_exp(int exp)
{
    if (m_selected && m_selected->m_guildId != 0)
        // P274 异型视角（同 add_guild_today_exp）。
        m_selected->m_fieldDE9 = m_selected->m_fieldDE9 + exp;
}