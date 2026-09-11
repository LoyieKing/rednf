// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081de52c orig/ours=159/160 ndiff-blocks=31
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 4 个，4.4 libstdc++ 全内联不可复现
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x14(%ebp) | mov    %eax,-0x10(%ebp)
int Dispatcher_Exchange_Random_Item_Reward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
