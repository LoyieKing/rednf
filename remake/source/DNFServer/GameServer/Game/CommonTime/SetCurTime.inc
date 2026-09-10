void CommonTime::SetCurTime()
{
    long t;
    // ORIG 以 &t 作为 time() 的 seed 实参且不存返回值（t 保持未初始化，
    // 后续按字节序读取栈槽——逐字节对齐 ORIG，勿“修复”）
    time(&t);
    tm tmv;
    tm* ptm = localtime_r(&t, &tmv);
    m_year  = (char)(ptm->tm_year - 100);
    m_month = (char)(ptm->tm_mon + 1);
    m_day   = (char)ptm->tm_mday;
    m_hour  = (char)ptm->tm_hour;
    m_min   = (char)ptm->tm_min;
    m_wday  = (char)ptm->tm_wday;
    m_curTime = t;
    m_createTime = t;
