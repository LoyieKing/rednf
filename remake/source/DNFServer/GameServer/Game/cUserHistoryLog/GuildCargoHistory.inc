void cUserHistoryLog::GuildCargoHistory(ENUM_GUILD_CARGO_HISTORY_REASON reason, int a) // 0x08686544
{
    m_user->LogHistory("GuildCargo,%d,%d", (int)reason, a);
