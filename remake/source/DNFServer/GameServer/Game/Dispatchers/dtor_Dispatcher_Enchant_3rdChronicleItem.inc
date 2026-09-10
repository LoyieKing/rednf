Dispatcher_Enchant_3rdChronicleItem::~Dispatcher_Enchant_3rdChronicleItem() {}
// b22 桥清零（P201，Model22）：WongWork::CItemUpgrade::ProcUpgrade 原 extern"C"
// mangled 直连桥转真成员调用。CDataManager.h:3024 权威虚槽声明、定义
// CItemUpgrade.cpp:410；ORIG 0x081d3808 调用即 devirt 直调 call 同 mangled，
// 真调用同形。

// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d343a orig/ours=127/125 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    &_ZN10GlobalData22s_pSecuProtectionFieldE,%eax | mov    0x15(%eax),%eax | mov    %eax,%ebx
