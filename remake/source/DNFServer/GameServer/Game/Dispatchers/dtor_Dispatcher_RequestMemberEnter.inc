Dispatcher_RequestMemberEnter::~Dispatcher_RequestMemberEnter() {}
// b28d-solo-Dispatchers verdict=AE(routed) [P226/P0] ORIG 0x081cb82a orig/ours=26/26
//   ORIG bool 物化于 %al（0x081cb85b `test %al,%al`）无槽往返 ⇒ P0：原码无具名
//   bool 局部，是条件表达式直用（具名版落 -0x9 且帧 +0x10）。
//   探针 /tmp/b28d_disp/probe_bool.cpp fB 逐条同形。
