Dispatcher_SecretShopOpenClose::~Dispatcher_SecretShopOpenClose() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d28b4 orig/ours=66/62 ndiff-blocks=11
//   帧差 sub $24 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    0x14(%ebp),%eax
