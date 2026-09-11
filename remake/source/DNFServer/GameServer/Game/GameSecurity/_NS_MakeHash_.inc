// ---------------------------------------------------------------------------
// PI 安全域函数（ORIG 符号经 nm 核实；真实定义在本 TU 后段）：
//   char _NS_MakeHash_(char*, short, int&)
//       _Z13_NS_MakeHash_PcsRi            T 0x808c8b5（报告 _lt_global_gt_/_NS_MakeHash_.md）
//   void _NS_MakeHash_(char*, unsigned short, unsigned int&)
//       _Z13_NS_MakeHash_PctRj            T 0x0808c976（报告 _NS_MakeHash__0808c976.md，薄包装）
//   char PI_GetType_CS(char, short)
//       _Z13PI_GetType_CScs               T 0x808c5d8（报告 _lt_global_gt_/PI_GetType_CS.md）
//   void PIAddServerHackCnt(CUser*, WongWork::ENUM_HACKTYPE, int)
//       _Z18PIAddServerHackCntP5CUserN8WongWork13ENUM_HACKTYPEEi  W 0x82772b7
//   int PI_GetHackType(short)
//       _Z14PI_GetHackTypes               W 0x82758e3
//       注：ORIG 符号看着"缺参数编码"，实为 `_Z14` + 名长 14 = "PI_GetHackType"
//       + `s`(short) ⇒ 本名（**单数 HackType**）自然 mangling 即 `_Z14PI_GetHackTypes`。
//       原先我方误写复数 PI_GetHackTypes（会 mangling 成 _Z15PI_GetHackTypess），
//       才不得不用 `__asm__` 标签纠正——按 guide §4.5 已删桥、改名正形（零码变）。
// 返回类型 char（非 bool/int）：ORIG 调用点 _NS_PI_MakeHash_NOTI(0x82746f3) 为
// `xor $0x1,%eax; test %al,%al; jne` —— 只测低字节，char 值域的 ==1 判据；
// ORIG 本体(0x0808c8b5) 两条返回 `mov $0x0,%eax`/`mov $0x1,%eax` 严格 0/1。
// mangled 不含返回类型 ⇒ ABI 无影响（Main 授权，batch-23 S 线）。
bool _NS_MakeHash_(char* data, short len, int& out);
