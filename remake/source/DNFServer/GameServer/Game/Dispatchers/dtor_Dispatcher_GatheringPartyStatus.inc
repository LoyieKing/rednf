Dispatcher_GatheringPartyStatus::~Dispatcher_GatheringPartyStatus() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08203878 orig/ours=532/490 ndiff-blocks=83
//   帧差 sub $1f0 vs $200 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x1f0,%esp | lea    -0x1d0(%ebp),%eax | lea    -0x1d0(%ebp),%eax
// [SweepDispatchers 2026-09-09] 真缺口修复（490→533 = ORIG 531；p167 仅 C1/C2 别名）：
//   ① 两循环计数改 int + `(int)recv.m_xxxNum > i`：ORIG 0x08203f80/0x08203f84 为
//      `addl $1,-0xc; movzwl num; movzwl %ax,%eax; cmp i,%eax; setg`（32 位计数器 + 有符号比较），
//      旧码 unsigned short 计数致 `addw`+`cmp %ax`+`seta`（P196 符号性族）；
//   ② 删 m_tryEntries/m_partyEntries 引用缓存（P183）：ORIG 每字段重发 i*10 / i*13 地址链。
