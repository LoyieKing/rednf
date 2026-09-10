void cUserHistoryLog::GuildOut(unsigned int guildKey, const char* guildName,
                               unsigned int characNo, char a, unsigned short b)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    struct tm* r = localtime_r(&now, &broken);
    int year = r->tm_year;
    int month = r->tm_mon + 1;
    int day = r->tm_mday;
    m_user->LogHistory("GuildOut,\"%d/%d/%d\",%d,\"%s\",%d,%d,%d",
                       year + 0x76c, month, day,
                       guildKey, guildName, characNo, (int)a, (unsigned)b);
