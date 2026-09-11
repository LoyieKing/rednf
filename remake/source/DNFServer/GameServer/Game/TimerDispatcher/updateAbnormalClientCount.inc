// ---- TimerFatigueReset ----
// 报告：docs/class_func_reports/TimerFatigueReset/{dispatch_sig,RegistNextTimer,updateAbnormalClientCount}.md
// 头已有声明：dispatch_sig / ~ / static int RegistNextTimer()（静态与非静态 mangled 相同 _ZN17..15RegistNextTimerEv，无需改头）。
// 类对象仅 vptr。
// ORIG T 0x086339fa（0xa 字节）：mov $0x1,%eax 直返 1。
int TimerFatigueReset::updateAbnormalClientCount()
{
    return 1;
}