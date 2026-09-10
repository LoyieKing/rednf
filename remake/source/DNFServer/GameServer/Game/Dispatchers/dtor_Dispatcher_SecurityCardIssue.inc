Dispatcher_SecurityCardIssue::~Dispatcher_SecurityCardIssue() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08209c8c orig/ours=291/251 ndiff-blocks=29
//   帧差 sub $18c vs $180 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x18c,%esp | movl   $&_ZZN28Dispatcher_SecurityCardIssue12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
// [SweepDispatchers 2026-09-09] 形态收敛（251→290 = ORIG 291；p167 仅 C1/C2 别名）：
//   ① certKey 前 6 字节 ORIG 0x08209f7d 为 `movl $0 + movw $0` 两写（P202①），旧码 memset call；
//   ② phone 缓冲 0xc 字节 ORIG 0x08209cdf/e6/ed 为三个 movl（4.1 内联），旧码 memset call；
//   ③ serverKey[0x21]/cardData[0x27] 改块内 `= {0}` 复现 ORIG 0x08209f8a/0x08209fe2 慢路
//      （旧码两处 memset call ⇒ p167 EXTRA memset）。
