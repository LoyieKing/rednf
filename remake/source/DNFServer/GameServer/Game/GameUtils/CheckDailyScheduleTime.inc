// ---- CheckDailyScheduleTime（每日调度时间检查，ORIG _Z22CheckDailyScheduleTimeill）----
// [P327 GI 键名] ORIG 本 TU `_GLOBAL__I_` 键名 = _Z22CheckDailyScheduleTimeill ⇒ 该
// 函数定义须为本 TU 首个强符号（此前 trim 定义在前 ⇒ 键名错为 _GLOBAL__I__Z4trimPc）。
// 纯前置定义顺序（函数体/符号集合/构造序零变化）。
// ORIG：localtime_r 拆 lCurTime，复制 tm 后置 nScheduleTime:00:00，mktime；
// 若原时刻小时 < nScheduleTime 则回退一天；返回 lLastPlayTime <= 该时刻。
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime)
