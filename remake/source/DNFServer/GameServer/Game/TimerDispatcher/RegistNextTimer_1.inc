// ORIG T 0x08635782 (_ZN32TimerStatisticsDetailChannelInfo15RegistNextTimerEv)
// cur=getCurSec()；localtime_r → next=(tm_min%0x1e)*(-0x3c)+(0x708-tm_sec)（对齐半小时）；
// cMyTrace("content min:%d sec:%d next sec:%d  (channel no:%d)")；
// InsertTimer(2, 0, 0x54, next, 0, 0)；返回 1。
// SIGFIX：ORIG __PRETTY_FUNCTION__ 串为 "bool TimerStatisticsDetailChannelInfo::RegistNextTimer()"。
// [b28d-30] 复核 DIFF：push edi/esi 缓存组/帧差（N7/N82 族），ORIG 0x086355xx。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：push edi/esi 缓存组 + 帧差（85/80）。
// [TimerDispatcher-2 2026-09-09 复核] 残差 = `%0x1e` 强度归约模板（ORIG
// `lea (%eax,%eax,1),%edx; mov %edx,%eax; shl $4,%eax; sub %edx,%eax` vs 4.4
// `add %eax,%eax; mov %eax,%edx; shl $4,%edx; sub %eax,%edx`，N86b/N1）⇒ 封顶。
// [TimerDispatcher-3 2026-09-10 判据①] 同族：唯一 jg 目标差 1 = 该 lea↔add 单条
// （探针 r1..r6 六形恒 add），分支语义等价。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=nd 多重集 85/85：唯一残差 = `x*0x1e`
// 归约 4 条 `lea (%eax,%eax,1),%edx; mov %edx,%eax; shl $0x4,%eax; sub %edx,%eax`（ORIG）
// vs `add %eax,%eax; mov %eax,%edx; shl $0x4,%edx; sub %eax,%edx`（我方）；前轮 22 形探针
// （/tmp/mod_probe{,2,3}.cpp）封底 + prett/strall/p167 CLEAR ⇒ 维持（新=DIFF gap=0, blocks=2）。
bool TimerStatisticsDetailChannelInfo::RegistNextTimer()
{
    // [TimerDispatcher-4 2026-09-11] 同 DDStat/UCE：唯一真残差 = `%0x1e` 余数段
    // ORIG `lea (%eax,%eax,1),%edx; mov %edx,%eax; shl $4,%eax; sub %edx,%eax`（4 条）
    // vs 4.4 `add %eax,%eax; mov %eax,%edx; shl $4,%edx; sub %eax,%edx`（4 条）
    // ⇒ N86b/N1（22 形探针无一命中，见 DDStat 判词）。p167：8=8，MISSING/EXTRA
    // 仅 `cMyTrace C2Ev↔C1Ev` 同址别名（P93）⇒ 无缺口。
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
}