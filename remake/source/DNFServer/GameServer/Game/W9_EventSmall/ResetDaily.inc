void CCharacBloodDungeon::ResetDaily()       // ORIG 0x832a5b6
{
    m_enterCount = 0;

    time_t curSec =
        static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    struct tm tmBuf;
    // P43：返回指针须经返回值访问（ORIG lret 槽 -0xc，tm_wday 经其读取）。
    struct tm* t = localtime_r(&curSec, &tmBuf);
    if (t->tm_wday == 1)                     // 周一：weekly 区清零
    {
        m_point = 0;
        m_weeklyEnterCount = 0;
        m_useGold = 0;
        m_flag20 = 0;
    }
    // P184 死块照抄：ORIG 尾部对 tm_hour/min/sec 的归零死写（-O0 逐条发射）。
    t->tm_hour = 0;
    t->tm_min = 0;
    t->tm_sec = 0;
