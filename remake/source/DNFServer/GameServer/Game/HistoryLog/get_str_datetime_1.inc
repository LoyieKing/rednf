void get_str_datetime(char* buf, int len)
{
    (void)len;
    time_t now = GlobalData::s_systemTime_.getCurSec();
    struct tm value;
    struct tm* pValue = localtime_r(&now, &value);
    // ORIG 0x086b199e：rodata 0x8cf5a63 = "%04d%02d%02d%02d%02d%02d"
    // （与 0x8cf5a7c 相距 0x19 = 24+1 字节，确证无分隔符）。
    sprintf(buf, "%04d%02d%02d%02d%02d%02d", pValue->tm_year + 1900,
            pValue->tm_mon + 1, pValue->tm_mday, pValue->tm_hour,
            pValue->tm_min, pValue->tm_sec);
