Dispatcher_TraceError::~Dispatcher_TraceError() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08204b0c orig/ours=62/62 ndiff-blocks=10
//   帧差 sub $34 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x34,%esp | lea    -0x19(%ebp),%eax
