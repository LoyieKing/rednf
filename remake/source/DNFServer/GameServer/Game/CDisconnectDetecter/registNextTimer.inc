// ---- TimerDetectDisconnect::registNextTimer @ 0x086374e0 ----
// 断线检测器定时器：10 秒后触发 dispatch_sig(0x6d)，若 DoDetectingDisconnect
// 仍检测到断线用户则再次登记（自续期），见 TimerDetectDisconnect 报告。
// ORIG 以 int 局部（-0xc(%ebp)）承载 10 再传参，帧 sub $0x38。
void TimerDetectDisconnect::registNextTimer()
