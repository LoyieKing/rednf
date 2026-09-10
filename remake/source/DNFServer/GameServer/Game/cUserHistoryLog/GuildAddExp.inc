void cUserHistoryLog::GuildAddExp(unsigned int guildKey,
                                  const char* guildDBInfo,
                                  unsigned int characNo, int exp)
{
    const time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    struct tm* r = localtime_r(&now, &local);
    // b28d-50：ORIG 0x08685be2 段把 tm_year/tm_mon+1/tm_mday 各落具名槽
    // （-0x14/-0x10/-0xc，P0 槽往返⇒原码有具名局部），调用点再 `lea 0x76c(%eax)`；
    // 直接内联读 r-> 字段会丢槽并改帧（0x78→0x70 + push esi/ebx）。
    int year = r->tm_year;
    int month = r->tm_mon + 1;
    int day = r->tm_mday;
    m_user->LogHistory(
        "GuildAddExp,\"%d/%d/%d\",%d,\"%s\",%d,%d",
        year + 1900, month, day,
        guildKey, guildDBInfo, characNo, exp);
