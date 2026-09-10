Dispatcher_SDC_Damage_Check::~Dispatcher_SDC_Damage_Check() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e9e84 orig/ours=170/167 ndiff-blocks=33
//   帧差 sub $440 vs $454 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x440,%esp | movl   $0x0,-0x2c(%ebp)
