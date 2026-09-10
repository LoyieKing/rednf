Dispatcher_LimitNpcBuyItem::~Dispatcher_LimitNpcBuyItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e1172 orig/ours=207/210 ndiff-blocks=42
//   帧差 sub $f0 vs $104 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0xf0,%esp | mov    %eax,-0x10(%ebp)
