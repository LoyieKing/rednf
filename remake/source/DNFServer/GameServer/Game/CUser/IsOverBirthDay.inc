bool CUser::IsOverBirthDay()
{
    // ORIG 0x0867ec66：出生月/日是否已过（用于 PowerWar 最低年龄判定）。
    // ORIG 0x867ec66 形：cur@-0x18 → localtime_r **返回值**落槽 pLocal@-0x14
    // （0x867ec8d mov %eax,-0x14），month/day 各自 reload pLocal（0x867ec90/9c，
    // T-CU14 同款）；birth 字段无具名槽——两处成员直读+展宽直发 cmp。
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    tm* pLocal = localtime_r(&cur, &local);
    int curMonth = pLocal->tm_mon + 1;
    int curDay = pLocal->tm_mday;
    if (m_birthMonth < curMonth)
        return true;
    if (m_birthMonth == curMonth &&
        m_birthDay < curDay)
        return true;
    return false;
