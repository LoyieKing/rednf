Dispatcher_ClearAction::~Dispatcher_ClearAction() {}
// b28d-solo-Dispatchers verdict=AE(routed) [P226/P0] ORIG 0x0812513c orig/ours=48/48
//   ORIG bool 物化于 %al（0x0812516x `test %al,%al`）无槽往返 ⇒ 条件表达式直用；
//   具名 bool 落 -0x9/帧漂移（探针 /tmp/b28d_disp/probe_bool.cpp fB 同形）。
