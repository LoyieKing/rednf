void cUserHistoryLog::GuildCargoItemRecovery(ENUM_GUILD_CARGO_HISTORY_REASON reason,
                                             int a, int b, int c, int d, int e,
                                             unsigned char f)                     // 0x08686694
{
    m_user->LogHistory("GuildCargo,%d,%d,%d,%d,%d,%d,%d",
                       (int)reason, a, b, c, d, e, f & 0xff);
}