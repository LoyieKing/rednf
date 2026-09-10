// [Q5BigTURest.DisPatcherCore 2026-09-11 返回类型轴] 与 CParty.h:387（bool）同步：
// ORIG 定义 0x0859d656 为 `setne %al` + `mov $0x1/%0x0,%eax`（0/1 语义），调用点
// 0x081ca16f `xor $0x1,%eax; test %al,%al; je` 直证 bool；返回类型不入 mangled ⇒ 零 ABI。
bool CParty::CheckMemberArea()
