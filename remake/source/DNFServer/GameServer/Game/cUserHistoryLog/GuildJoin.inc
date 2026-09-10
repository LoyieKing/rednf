void cUserHistoryLog::GuildJoin(unsigned int guildKey, const char* guildName,
                                unsigned int characNo, unsigned short a, char b)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    struct tm* r = localtime_r(&now, &broken);
    int year = r->tm_year;
    int month = r->tm_mon + 1;
    int day = r->tm_mday;
    m_user->LogHistory("GuildJoin,\"%d/%d/%d\",%d,\"%s\",%d,%d,%d",
                       year + 0x76c, month, day,
                       guildKey, guildName, characNo, (unsigned)a, (int)b);
