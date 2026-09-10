Dispatcher_RequestCharacSkillInfo::~Dispatcher_RequestCharacSkillInfo() {}
// ORIG 0x081e3c08 T _ZN33Dispatcher_RequestCharacSkillInfo11check_errorEP5CUserR8MSG_BASE
// （docs/class_func_reports/Dispatcher_RequestCharacSkillInfo/check_error.md 汇编+反编译 C 全文转写）
// user+0x796e8 = m_characList（vector<_Charac_info>）；槽 char 截断按 movsbl；
// _Charac_info+0x27 = m_level（short）；G_CDataManager()+0x629c = m_characLink.m_field0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e3c08 orig/ours=75/68 ndiff-blocks=9
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,-0xc(%ebp)
