Dispatcher_DisJointAvatar::~Dispatcher_DisJointAvatar() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x08217c06 orig/ours=579/538 ndiff-blocks=91
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 3 个，4.4 libstdc++ 全内联不可复现
//   首差: mov    %eax,-0x24(%ebp) | cmpl   $0x0,-0x24(%ebp) | mov    -0x24(%ebp),%eax
