bool _NS_MakeHash_(char* data, short len, int& out);

// [b28c-l23 复通·推翻 b22「不可源码复现」判词] b22 的 13 形全部在 **char callee**
// 前提下试（故恒产 `cmp $0x1,%al; sete`）。Main 已授权把 _NS_MakeHash_ 定为
// **bool**（GameSecurity.cpp:38/185，属主 C28L22 确认；返回类型不入 mangled，
// 本 TU 局部声明必须同步——见上）。bool callee 下 `!call()` 直产 ORIG 的
// `call; xor $0x1,%eax; test %al,%al`（P168a① 技巧1 / lane7 实测口径）。
// [b28d-solo-PacketBuf] verdict=AE（check_tu_routed clean；p167 4/4 CLEAR）。
// 修法：P223 多岛——ORIG 两守卫落点不同（packet==0 的 je 落 `nop;jmp` 岛；
// _NS_MakeHash_ 失败的 jne 落 `nop` 岛，两岛物理相邻），4.4 对裸 `return`
// 恒 jump-thread 掉一对 `jmp+nop`。改为两条独立负条件 goto + 连续尾标签串
// `L1:; L2:; Lend:;`（体包入内层复合块以合法跳出 scope）逐岛复现。
