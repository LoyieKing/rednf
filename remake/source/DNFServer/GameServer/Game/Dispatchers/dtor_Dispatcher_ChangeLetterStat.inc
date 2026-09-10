Dispatcher_ChangeLetterStat::~Dispatcher_ChangeLetterStat() {}
// b28d-solo-Dispatchers verdict=AE(routed) [P226/P0] ORIG 0x081ce2ba orig/ours=26/26
//   ORIG bool 物化于 %al（0x081ce2f1 `test %al,%al`）无槽往返 ⇒ P0：原码无具名
//   bool 局部，是条件表达式直用。原具名 bool 落 -0x9 且 dead 槽漂到 -0x10。
//   探针 /tmp/b28d_disp/probe_bool.cpp fB（`if (!(A && B)) return -1;`）逐条同形。
