Dispatcher_CallGuildCreateRight::~Dispatcher_CallGuildCreateRight() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081ffcc2 orig/ours=287/293 ndiff-blocks=30
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 9 个，4.4 libstdc++ 全内联不可复现
//   首差: push   %edi | sub    $0x3c,%esp | movl   $&_ZZN31Dispatcher_CallGuildCreateRight12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
