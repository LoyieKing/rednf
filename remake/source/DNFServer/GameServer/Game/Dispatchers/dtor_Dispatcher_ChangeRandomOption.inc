Dispatcher_ChangeRandomOption::~Dispatcher_ChangeRandomOption() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081dc9ba orig/ours=511/537 ndiff-blocks=97
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 5 个，4.4 libstdc++ 全内联不可复现
//   [S162+S172] 已修：m_seedd.m_field & 3 → m_seedd.m_bits.m_low2 位域视角
//   （P308；Inven_Item.h 匿名 union，sizeof/偏移不变）⇒ 首差位 0→5。
//   首差: mov    %eax,-0x34(%ebp) | ... （S162 后首差位=5）
