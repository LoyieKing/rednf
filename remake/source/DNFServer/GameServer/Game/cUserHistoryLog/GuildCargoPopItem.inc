void cUserHistoryLog::GuildCargoPopItem(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                        int a, int b, int c, int d, int e)        // 0x086866ea
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d", (int)reason, a, b, c, d, e);
}