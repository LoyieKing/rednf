void cUserHistoryLog::GuildCargoResult(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                       int a, int b)                              // 0x08686512
{
    m_user->LogHistory("GuildCargo,%d,%d,%d", (int)reason, a, b);
