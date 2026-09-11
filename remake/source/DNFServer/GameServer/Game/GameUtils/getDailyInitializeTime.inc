// ---- getDailyInitializeTime（ORIG _Z22getDailyInitializeTimeiii，0x86b1f5a，0x66 字节）----
// ORIG：取 CSystemTime::getCurSec()，localtime_r，置 hour:min:sec，mktime；
// 若结果晚于当前时刻则回退一天（返回当日该时刻，已过则昨日）。
int getDailyInitializeTime(int hour, int min, int sec)
