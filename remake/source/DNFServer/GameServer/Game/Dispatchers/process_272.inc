// ORIG 0x0825dc64 T _ZN35Dispatcher_UseRightOfChangeGrowType7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_UseRightOfChangeGrowType/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 stRightOfChangeGrowType，ORIG 裸访问 +0x04 错误码槽（m_4）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825dc64 orig/ours=75/77 ndiff-blocks=16
//   帧差 sub $38 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x38,%esp | mov    %eax,-0x14(%ebp) | mov    -0x14(%ebp),%edx
int Dispatcher_UseRightOfChangeGrowType::process(CUser* user, MSG_BASE& msg, ParamBase& param)
