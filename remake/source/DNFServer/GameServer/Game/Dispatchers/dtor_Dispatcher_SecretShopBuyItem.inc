Dispatcher_SecretShopBuyItem::~Dispatcher_SecretShopBuyItem() {}
// ORIG 0x081d26ea T _ZN28Dispatcher_SecretShopBuyItem7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_SecretShopBuyItem/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 stSecretShopBuyItemParam，ORIG 裸访问 +0x04 错误码槽（m_4）。
// 调用同 TU 模板 check_error(CUser*, MSG_BASE&, ParamBase&)（ORIG check_error(CUser*,MSG_BASE&,ParamBase&)）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d26ea orig/ours=75/77 ndiff-blocks=15
//   帧差 sub $38 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x38,%esp | mov    %eax,-0x18(%ebp) | mov    %eax,%edx
