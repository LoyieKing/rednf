// getToday(long,int) @ 0x080ca942（docs/class_func_reports/CBingo/getToday_080ca942.md）：
// 当前秒数未过当日小时槽（hourLimit，即 G_CEnvironment+0x37c 的 m_fatigueTime）时取
// 昨天日号，否则取今天日号（跨零点结算窗口）。localtime_r 双展开。
int CBingo::getToday(long sec, int hourLimit)
