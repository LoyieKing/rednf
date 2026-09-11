// [b28d-solo-CUserCharacInfo AE] 21=21：P274 异型视角（字段 unsigned 视角
// m_guildTodayExpU + 形参 int）⇒ 复现 ORIG `mov 0xdef(%edx),%ecx; mov 0xc(%ebp),%edx;
// lea (%ecx,%edx,1),%edx`（旧判「字段 unsigned 无效」系未用 union 视角 + 字段在前写法）。
void CUserCharacInfo::add_guild_today_exp(int exp)
{
    if (m_selected && CUR->m_guildkey != 0)
        // P274 异型视角（字段 unsigned + 形参 int）：复现 ORIG
        // `mov 0xdef(%edx),%ecx; mov 0xc(%ebp),%edx; lea (%ecx,%edx,1),%edx`。
        CUR->m_guildTodayExpU = CUR->m_guildTodayExpU + exp;
}