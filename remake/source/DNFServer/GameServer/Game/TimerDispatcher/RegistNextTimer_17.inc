// ORIG T 0x0863a9a2 sz 0xed（_ZN23Timer_Schedule_MidNight15RegistNextTimerEv）：
// cur=getCurSec()；localtime_r(&cur+0x15180,&tm) → tm_hour/min/sec=0；
// next=mktime(&tm)；delta=next-getCurSec()；
// cMyTrace(__PRETTY_FUNCTION__,0x1626,0)
//   ("RegistNextTimer Schedule_MidNight delta %d\n", delta)；
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0x9a, delta, 0, 0)。
// [b28c-31] AE：cMyTrace 全表达式临时链（P221）复现 ORIG 0x0863aa1c 槽序
// （sec=-0x28 / trace=-0x24），54=54。
void Timer_Schedule_MidNight::RegistNextTimer()
