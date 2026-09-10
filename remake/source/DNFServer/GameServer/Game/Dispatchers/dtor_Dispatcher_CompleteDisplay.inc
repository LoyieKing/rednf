Dispatcher_CompleteDisplay::~Dispatcher_CompleteDisplay() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x08202cda orig/ours=430/395 ndiff-blocks=64
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 7 个，4.4 libstdc++ 全内联不可复现
//   首差: push   %edi | sub    $0xbc,%esp | jne    08202d0c <+0x32>
// [SweepDispatchers 2026-09-09] 重载选型真缺口修复：dollSlot 读取 ORIG 0x0820324c 调
//   get_short(unsigned short&)（Rt），旧码 `(short&)dollSlot` 误选 Rs ⇒ p167 MISSING/EXTRA
//   各 1；字段本型直传后该对清零（残余为 N89 STL 内联族 + C1/C2 别名）。
