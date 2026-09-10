void CUser::add_pvp_play_info(unsigned int a, unsigned int b)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    // PvpResultType：+0x60 m_todayLoseCount / +0x64 m_todayPlayCount /
    // +0x68 m_nextResetTime（ORIG add_pvp_play_info 0x0865d9a7/0x0865da2b/
    // 0x0865da3f 均 movl 4B 实证）。
    if (getPVPResultRefR()->m_nextResetTime + 0x15180 < (int)now)
    {
        getPVPResultRefW()->m_todayLoseCount = 0;
        // ORIG 0x0865d??：&tmv 存指针槽 -0xc（`lea -0x3c,%eax; mov %eax,-0xc`），
        // 三清零与 mktime 均 `mov -0xc,%eax` reload 解引用——4.4 值形会直写折掉
        // 指针槽（技巧 36 反向：ORIG 保留中转），显式指针对象还原。
        tm tmv;
        // ORIG 0x0865da??：指针 = **localtime_r 返回值直用**（call 后 %eax 即 &tmv，
        // `mov %eax,-0xc` 存槽，无二次 lea）。
        tm* pTmv = localtime_r(&now, &tmv);
        (*pTmv).tm_hour = 0;
        (*pTmv).tm_min = 0;
        (*pTmv).tm_sec = 0;
        getPVPResultRefW()->m_nextResetTime = mktime(pTmv) + 0x15180;
    }
    getPVPResultRefW()->m_todayLoseCount += a;
    getPVPResultRefW()->m_todayPlayCount += b;
