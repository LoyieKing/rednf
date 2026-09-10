Dispatcher_New_Gmdebug_Command::~Dispatcher_New_Gmdebug_Command() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x0820bbde orig/ours=234/228 ndiff-blocks=39
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 9 个，4.4 libstdc++ 全内联不可复现
//   首差: sub    $0x190,%esp | movb   $0x1,-0x1a(%ebp) | movb   $0x1,-0x19(%ebp)
