void get_str_date(char* buf, int len, char sep)
{
    (void)len;
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm value;
    // ORIG：使用 localtime_r 的返回值指针访问字段。
    struct tm* pValue = localtime_r(&now, &value);
    // ORIG 0x086b19f1：rodata 0x8cf5a7c = "%04d%02d%02d"（无分隔符；
    // 相邻串 0x8cf5a89 与之相距 0xd 字节，长度自洽）。
    if (sep == 0)
        sprintf(buf, "%04d%02d%02d", pValue->tm_year + 1900, pValue->tm_mon + 1, pValue->tm_mday);
    else
        sprintf(buf, "%04d%c%02d%c%02d", pValue->tm_year + 1900, sep,
                pValue->tm_mon + 1, sep, pValue->tm_mday);
