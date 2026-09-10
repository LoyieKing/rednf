void get_str_time(char* out, int c, char sep)
{
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    struct tm tmv;
    struct tm* ptm = localtime_r(&curSec, &tmv);
    // ORIG 0x086b1ac7/0x086b1b01：栈实参 0x8(%esp)=tm_hour(+0x8)、
    // 0xc/0x10(%esp)=tm_min(+0x4)、末位=tm_sec(+0x0)，即 时→分→秒 顺序
    // （汇编取值顺序与压栈槽位相反，此前按取值顺序写反）。
    if (sep == 0)
    {
        sprintf(out, "%02d%02d%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    }
    else
    {
        sprintf(out, "%02d%c%02d%c%02d", ptm->tm_hour, sep, ptm->tm_min, sep,
                ptm->tm_sec);
    }
