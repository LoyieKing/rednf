Dispatcher_Avatar_Disjoint_Random_Reward::~Dispatcher_Avatar_Disjoint_Random_Reward() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081dea06 orig/ours=171/220 ndiff-blocks=43
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 3 个，4.4 libstdc++ 全内联不可复现
//   首差: push   %ebx | sub    $0x74,%esp | mov    %eax,-0x18(%ebp)
