int CEventScriptMng::get_pass_day(int yyyymmdd1, int yyyymmdd2)
{
    if (yyyymmdd1 < 1 && yyyymmdd2 < 1)
        return -1;

    // ORIG 0x810c974：{0} 初始化产生 rep stos 内联（P4），非 call memset。
    struct tm t1 = {0};
    t1.tm_year = yyyymmdd1 / 10000 - 1900;
    t1.tm_mon  = (yyyymmdd1 % 10000) / 100 - 1;
    t1.tm_mday = yyyymmdd1 % 100;
    t1.tm_hour = 0;
    time_t v1 = std::mktime(&t1);

    struct tm t2 = {0};
    t2.tm_year = yyyymmdd2 / 10000 - 1900;
    t2.tm_mon  = (yyyymmdd2 % 10000) / 100 - 1;
    t2.tm_mday = yyyymmdd2 % 100;
    t2.tm_hour = 0;
    time_t v2 = std::mktime(&t2);

    if (v2 >= v1)
        return static_cast<int>((v2 - v1) / 0x15180);
    return -1;
