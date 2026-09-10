void cUserHistoryLog::GuildCargoMoveItemResult(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                               int a, int b, int c)               // 0x08686800
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d", (int)reason, a, b, c);
