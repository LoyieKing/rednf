void cUserHistoryLog::GuildCargoPopItemResult(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                              int a, int b, int c, int d, int e, int f) // 0x08686780
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e, f);
