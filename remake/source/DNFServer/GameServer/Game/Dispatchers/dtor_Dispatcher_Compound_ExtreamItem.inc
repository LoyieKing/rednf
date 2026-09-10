Dispatcher_Compound_ExtreamItem::~Dispatcher_Compound_ExtreamItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08225b54 orig/ours=58/59 ndiff-blocks=6
//   帧差 sub $58 vs $68 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x58,%esp | mov    %eax,%edx | mov    0x90(%eax),%ecx
