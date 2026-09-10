void cUserHistoryLog::GuildCargo(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a)   // 0x086864e6
{
    m_user->LogHistory("GuildCargo,%d,%d", (int)reason, a);
