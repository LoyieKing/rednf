Dispatcher_CompoundItemByExpertJob::~Dispatcher_CompoundItemByExpertJob() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821eb66 orig/ours=210/211 ndiff-blocks=20
//   帧差 sub $50 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x50,%esp | movl   $&_ZZN34Dispatcher_CompoundItemByExpertJob12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN
