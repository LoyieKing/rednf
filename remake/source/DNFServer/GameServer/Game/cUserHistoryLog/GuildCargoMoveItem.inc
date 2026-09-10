void cUserHistoryLog::GuildCargoMoveItem(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                         int a, int b)                            // 0x086867ce
{
    m_user->LogHistory("GuildCargo,%d,%d,%d", (int)reason, a, b);
