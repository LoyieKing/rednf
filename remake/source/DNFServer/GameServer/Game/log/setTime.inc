// ORIG TimeLog_::setTime 0x0854e228（本 TU 依赖，报告 docs/class_func_reports/
// TimeLog_/setTime.md）：与 getTime 同构，写 this 字段。
void TimeLog_::setTime()
{
    std::time_t now = std::time(0);
    std::tm lt;
    struct tm* p = localtime_r(&now, &lt);
    m_year = (short)(p->tm_year + 0x76c);
    m_mon = p->tm_mon + 1;
    m_day = p->tm_mday;
    m_hour = p->tm_hour;
    m_min = p->tm_min;
    m_sec = p->tm_sec;
}
