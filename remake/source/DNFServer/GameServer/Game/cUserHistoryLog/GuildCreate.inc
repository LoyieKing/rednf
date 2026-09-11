// ---- 公会日期族：ORIG 取 GlobalData::s_systemTime_ 当前秒 + localtime_r，
//      "%d/%d/%d" = tm_year+1900(tm_year+0x76c)/tm_mon+1/tm_mday ----
// ORIG _ZN15cUserHistoryLog11GuildCreateEjPKcj @0x08685978
void cUserHistoryLog::GuildCreate(unsigned int guildKey, const char* guildName,
