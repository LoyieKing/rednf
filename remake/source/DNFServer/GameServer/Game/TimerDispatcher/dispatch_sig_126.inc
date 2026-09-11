// Timer_DungeonInoutOpenTime::dispatch_sig ORIG T 0x8639e0a (0x3c5)
// [b28d-30] 复核 DIFF：全帧槽号平移（N82 族），ORIG 0x08639e0a。
// [b28d-solo-TimerDispatcher] 判词 N82 封顶：全帧槽号平移（233/231，调用点全等）。
// [TimerDispatcher-2 2026-09-09] 修 2 处：①nextOpenT 拆两句式（ORIG 08639f1a
// `addl $0x15180,-0x1c(%ebp)` 内存自增）②"DELTA..." cMyTrace 改全表达式临时链
// （P227 ⇒ curT 升格 %ebx 跨 ctor）⇒ ndiff 185→179；残余 = 全帧槽平移（ORIG 标量区
// -0x78..-0x14 vs 4.4 -0x30..-0x10，帧同为 0xd4，N82/N95）+ dungeonIdx 实参升格序 N7。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；残差为 tm/map 对象区与标量区
// 的槽排布（非恒定平移：tm -0xac↔-0xb0、map -0x78↔-0x3c）⇒ N82/N95，无语义缺口。
bool Timer_DungeonInoutOpenTime::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
