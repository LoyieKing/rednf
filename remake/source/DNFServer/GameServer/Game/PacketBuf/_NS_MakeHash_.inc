// 权威定义 GameSecurity.cpp（ORIG T 0x0808c8b5，属主 C28L22 已按 Main 授权声明
// 为 **bool**）；本 TU 的局部声明必须同步——返回类型不入 mangled 但**决定调用点
// 归一化形**（P168a/lane14/lane7 实测：char callee ⇒ 4.4 对 `== 1` 恒产
// `cmp $0x1,%al; sete; test; je`；bool callee ⇒ 直产 ORIG 的
// `call; xor $0x1,%eax; test %al,%al; jne`，b28c-l23 档①）。
bool _NS_MakeHash_(char* data, short len, int& out);
