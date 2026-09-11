// ORIG T 0x08636856 (_ZN25TimerDungeonDataStatistic15RegistNextTimerEv)
// ORIG 非 static（调用点传 this）返回 bool（ORIG T 0x08636856）
// [b28d-30] 复核 DIFF：帧差 0x14（0x64 vs 0x78，P232/P210 族），ORIG 0x08636856。
// [b28d-solo-TimerDispatcher] 判词 N1 封顶：`%0x1e` 常量除法展开算术折叠（ORIG
// lea+shl+sub 链 vs 4.4 add+shl+sub），源形不可择（74/74）。
// [TimerDispatcher-2 2026-09-09 复核] 同上 N86b/N1（ORIG-only lea/mov vs OURS-only
// add 各 1，余全等）⇒ 封顶，无语义缺口。
// [TimerDispatcher-3 2026-09-10 判据①] 唯一 jg 目标差 1 = `%0x1e` 强度归约 2x 的
// lea↔add（探针 /tmp/td_probe5.cpp r1..r6 六形恒 add）⇒ N86b/N1；分支语义等价。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=nd 多重集 74/74：唯一残差同 SDCI 件
// （`x*0x1e` 归约 lea↔add 4 条；22 形探针封底）+ prett/strall/p167 CLEAR ⇒ 维持
// （新=DIFF gap=0, blocks=2）。
bool TimerDungeonDataStatistic::RegistNextTimer()
{
    // [TimerDispatcher-4 2026-09-11] N86b/N1 复证（22 形探针 /tmp/mod_probe{,2,3}.cpp）：
    // 4.4 对 `x*0x1e` 独立展开同为 add 形（f17 `add %eax,%eax; mov %eax,%edx;
    // shl $4,%edx; sub %eax,%edx`），与 `%0x1e` 余数段逐条同构 ⇒ ORIG 的
    // `lea (%eax,%eax,1),%edx`（4.1 expand_mult 目标寄存器选择）不可由源码形诱导；
    // 探针含显式 `X-(X/30)*30`(f3/f9/f15/imul 形)、手写分解 (2q<<4)-2q(f4/f8/f19)、
    // 具名 q(f16/f20 reload)、语句表达式——唯一命中 lea 的 f8 需具名 q 落槽且整体多
    // reload+add 反劣。p167：ORIG 6 calls = OURS 6 CLEAR ⇒ 无语义缺口，封底。
    // ORIG 0x086368xx：curSec 单 time_t 槽（免 int→long 拷贝）；localtime_r 返回
    // pt 落槽 -0x18，各字段经 `mov -0x18,%eax` 重读 + 指针 disp 读写（技巧37 反用：
    // 具名指针槽正是 ORIG 形）
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    tm t;
    tm* pt = localtime_r(&curSec, &t);
    const int curHour = pt->tm_hour;
    pt->tm_hour = 5;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    // ORIG 0x086368a7-0x086368e1：chn 全程 %ecx、x=chn/30（0x88888889+sar4 魔数）
    // 存 %ebx（sign 修正后）、2x=%edx；`shl $4; sub %edx` =30x；
    // tm_min = chn-30x = chn%30（P196：旧 /15*45 形在 chn≥15 分叉系误判）。
    // P0：ORIG chn/x/x2 全程寄存器无槽（086368a7 `mov 0x1b0(%eax),%ecx` 直入
    // 魔数除法）⇒ 表达式内联为 `% 0x1e`（旧三个具名局部多 3 槽 + 全帧平移）。
    pt->tm_min = G_CEnvironment()->m_serverEnvir.m_channelNo % 0x1e;  // +0x1b0
    int target = static_cast<int>(mktime(pt));
    if (pt->tm_hour <= curHour)
        target += 0x15180;
    const int delay = target - static_cast<int>(curSec);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x66,
                                delay, 0, 0);
    return 1;
}