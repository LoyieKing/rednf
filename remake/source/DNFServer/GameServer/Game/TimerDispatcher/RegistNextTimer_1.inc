bool TimerStatisticsDetailChannelInfo::RegistNextTimer()
{
    const int curSec = GlobalData::s_systemTime_.getCurSec();
    tm timeInfo;
    // [b28c-27] ORIG 0x08635782：localtime_r 第一实参是 curSec 槽本身的地址
    // （`lea -0x34(%ebp),%eax`），无 time_t now 中间槽；4.4 对具名 now 恒多一槽
    // 与两条 mov。按 ORIG 直取 curSec 地址（int/time_t 同 4 字节）。
    tm* pt = localtime_r(reinterpret_cast<const time_t*>(&curSec), &timeInfo);
    // ORIG 086358xx：`imul $-0x3c; lea 0x6cc(%eax),%ecx; mov $0x3c; sub sec; lea`
    // ⇒ 常量按 ORIG 拆成 0x6cc + (0x3c - sec)（0x6cc=0x708-0x3c，等价）。
    const int nextSec = (pt->tm_min % 0x1e) * -0x3c + 0x6cc + (0x3c - pt->tm_sec);
    // P0：ORIG channelNo 无槽（086358xx `mov %eax,%ebx` 直入 callee-saved）⇒ 实参内联；
    // P221/P227 全表达式临时链令 tm_min/tm_sec/channelNo 在 ctor 前求值驻 edi/esi/ebx。
    cMyTrace(__PRETTY_FUNCTION__, 0x92c, 0)(
        "content min:%d sec:%d next sec:%d  (channel no:%d)\n",
        pt->tm_min, pt->tm_sec, nextSec, G_CEnvironment()->get_channel_no());
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x54, nextSec, 0, 0);
    return true;
