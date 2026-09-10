void cUserHistoryLog::GuildChangeGrade(unsigned int guildKey, const char* guildName,
                                       unsigned int characNo, unsigned int grade,
                                       unsigned char oldGrade, unsigned char newGrade)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm broken;
    struct tm* r = localtime_r(&now, &broken);
    int year = r->tm_year;
    int month = r->tm_mon + 1;
    int day = r->tm_mday;
    m_user->LogHistory("GuildChangeGrade,\"%d/%d/%d\",%d,\"%s\",%d,%d,%d,%d",
                       year + 0x76c, month, day,
                       guildKey, guildName, characNo, grade,
                       (unsigned)(oldGrade & 0xff), (unsigned)(newGrade & 0xff));
