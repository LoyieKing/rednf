// ---- getPrevScheduleTime（ORIG _Z19getPrevScheduleTimeil，0x846c2e9，0x61 字节）----
// ORIG：t = time - hour*3600；localtime_r；置 hour:00:00；mktime。
// 返回 time 当日 hour 时刻的时间戳（未到 hour 则属前一天）。
long getPrevScheduleTime(int hour, long time)
