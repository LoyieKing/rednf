void DESTROY_TimerDispatcher()  { GlobalInstance<TimerDispatcher>::destroy(); }

// ---------------------------------------------------------------------------
// regist_signal(int, void(*)(int)) —— ORIG 0x829f1fc（逐指令转写）：
// sigaction；sa_flags = sig==14(SIGALRM) ? 0x20000000 : 0x10000000（ORIG 实证 cmpl $0xe）；
// 失败 printf 后返回 0，成功返回 1。
// P248 档①：ORIG regist_signals 调用点为 `call; xor $0x1,%eax; test %al,%al; je`
// （无 setne）⇒ callee 返回类型必须为 bool（int 在 4.4 恒产 `test; sete` 物化）。
// 返回类型不入 mangled（_Z13regist_signaliPFviE），ORIG 尾 `mov $0x1,%eax` 与 bool 兼容。
// ---------------------------------------------------------------------------
