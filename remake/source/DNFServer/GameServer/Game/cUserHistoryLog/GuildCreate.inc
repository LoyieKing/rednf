void cUserHistoryLog::GuildCreate(unsigned int guildKey, const char* guildName,
                                  unsigned int characNo)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    struct tm* r = localtime_r(&now, &broken);
    // b28d-50：同 GuildAddExp——ORIG 0x086859a2 段三具名日期槽 -0x14/-0x10/-0xc。
    int year = r->tm_year;
    int month = r->tm_mon + 1;
    int day = r->tm_mday;
    m_user->LogHistory("GuildCreate,\"%d/%d/%d\",%d,\"%s\",%d",
                       year + 0x76c, month, day,
                       guildKey, guildName, characNo);
