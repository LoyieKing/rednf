void CEventScriptMng::update_time_values(int hour)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    // ORIG 对 now 与 prev 各独立调一次 getCurSec()（两次 call），
    // prev = getCurSec() - 0x15180，不复用 now 结果。
    time_t prev = GlobalData::s_systemTime_.getCurSec() - 0x15180;
    struct tm localT, prevT;
    // ORIG 保存 localtime_r 返回的指针局部（-0x14=&localT / -0x10=&prevT），
    // 后续经指针成员访问（==&localT/+0x8 tm_hour 等），非每次 lea 重求址。
    struct tm* localTp = localtime_r(&now, &localT);
    struct tm* prevTp = localtime_r(&prev, &prevT);

    // ORIG 0x0810c628：`movl $0x0,-0xc(%ebp)` 死局部（此后从不被读；P232 死槽
    // 占帧，使 now/prev/localT/prevT/指针组整体下移 4，槽位与 ORIG 全等）。
    int deadFlag = 0;

    // ORIG 0x0810c62f：两条守卫（js/jge）同落**出线**的 localTp 块，prevTp 块为
    // fall-through（4.1 的 `||` 条件 then 块外置）⇒ 4.4 复现形为德摩根正向包裹
    // （P222/P272）：条件取反、两块互换，语义与 ORIG 完全一致。
    if (localTp->tm_hour >= 0 && localTp->tm_hour < hour)
    {
        m_todayTimeDay = static_cast<unsigned char>(prevTp->tm_mday);
        m_todayTime = prevTp->tm_year * 10000 + (prevTp->tm_mon + 1) * 100
                      + 19000000 + static_cast<unsigned char>(m_todayTimeDay);
    }
    else
    {
        m_todayTimeDay = static_cast<unsigned char>(localTp->tm_mday);
        m_todayTime = localTp->tm_year * 10000 + (localTp->tm_mon + 1) * 100
                      + 19000000 + static_cast<unsigned char>(m_todayTimeDay);
    }
