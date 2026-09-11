// ---- ComputeDaysScheduleTime（ORIG _Z23ComputeDaysScheduleTimeill，0x846c144，0x1a5 字节）----
// ORIG：分别以 curTime/lastTime 求当日 hour:00:00 基准时刻（未到 hour 回退一天），
// 差值 /86400 取整天数，负数归 0。
int ComputeDaysScheduleTime(int hour, long lastTime, long curTime)
