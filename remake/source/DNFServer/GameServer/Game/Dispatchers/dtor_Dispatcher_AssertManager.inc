Dispatcher_AssertManager::~Dispatcher_AssertManager() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821e1f4 orig/ours=134/179 ndiff-blocks=31
//   帧差 sub $234 vs $25c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x234,%esp | lea    -0x21c(%ebp),%eax | lea    -0x21c(%ebp),%eax
