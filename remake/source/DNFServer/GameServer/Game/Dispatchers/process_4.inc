// b28d-solo-Dispatchers verdict=AE(routed) [P0/P211档③] ORIG 0x0813f8ce orig/ours=30/30
//   [S162] 已修：①具名 ec + dst 两局部（帧 +8、槽整体漂移）→ 单语句内联实参
//   `p->m4 = check_error(...)`（P0 右值直传）；②_P4::m4 改 unsigned 与 int 返回值
//   异型 ⇒ 4.4 翻 RHS-first+%edx 中转（P211 档③）。
int Dispatcher_StartGame::process(CUser* user, MSG_BASE& msg, ParamBase& param)
