Dispatcher_RequestMatch::~Dispatcher_RequestMatch() {}
// ORIG 0x08217ad0 T _ZN23Dispatcher_RequestMatch12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/Dispatcher_RequestMatch/dispatch_sig.md 汇编+反编译 C 全文转写）
// b28d-solo-Dispatchers verdict=AE(routed) [S206 翻正] ORIG 0x08217ad0
//   [N7→P63] 根因=两分支各自 `return 0` 各发一条 mov $0,%eax；改 if/else 单出口
//   （then call 后 jmp 共享尾 mov $0,%eax、else 直落）⇒ 归零。
